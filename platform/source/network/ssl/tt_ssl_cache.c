/* Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <network/ssl/tt_ssl_cache.h>

#include <event/tt_event_center.h>
#include <io/tt_socket.h>
#include <memory/tt_memory_alloc.h>
#include <misc/tt_reference_counter.h>
#include <network/ssl/tt_ssl_context.h>
#include <timer/tt_time_reference.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define TT_SSL_PEER_EXPIRE_MIN 60000 // 1 minute
//#define TT_SSL_PEER_EXPIRE 600000 // 10 minutes
#define TT_SSL_PEER_EXPIRE 120000 // 2 minutes

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

#if 1

tt_atomic_s32_t tt_g_ssl_peer_num;
#define __PEER_NUM_INC() tt_atomic_s32_inc(&tt_g_ssl_peer_num)
#define __PEER_NUM_DEC() tt_atomic_s32_dec(&tt_g_ssl_peer_num)

#else

#define __PEER_NUM_INC()
#define __PEER_NUM_DEC()

#endif

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static void __peer_map_node2key(IN tt_hnode_t *node,
                                OUT const tt_u8_t **key,
                                OUT tt_u32_t *key_len);

static tt_bool_t __peer_map_node_release(IN tt_u8_t *key,
                                         IN tt_u32_t key_len,
                                         IN tt_hnode_t *hnode,
                                         IN void *param);

static tt_bool_t __peer_check_expire(IN tt_u8_t *key,
                                     IN tt_u32_t key_len,
                                     IN tt_hnode_t *mnode,
                                     IN void *param);

// this function assumes size of buffer peer_id
// is TT_SSL_PEER_ID_LEN
static tt_u32_t __mk_peer_id(IN tt_char_t *peer_id,
                             IN struct tt_sslctx_s *sslctx,
                             IN tt_sktaddr_t *local_addr,
                             IN tt_sktaddr_t *remote_addr,
                             IN tt_u32_t peer_private);

static void __peer_init(IN tt_sslpeer_t *sslpeer,
                        IN tt_char_t *ses_id,
                        IN tt_u32_t ses_id_len,
                        IN OPT tt_sslpeer_ntv_t *sys_peer);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_sslcache_create(IN tt_sslcache_t *sslcache,
                               IN OPT tt_sslcache_attr_t *attr)
{
    tt_u32_t __done = 0;
#define __SCC_LOCK (1 << 0)
#define __SCC_MAP (1 << 1)

    TT_ASSERT(sslcache != NULL);

    // attr
    if (attr == NULL) {
        tt_sslcache_attr_default(&sslcache->attr);
    } else {
        tt_memcpy(&sslcache->attr, attr, sizeof(tt_sslcache_attr_t));
    }
    attr = &sslcache->attr;

    if (attr->peer_expire_ms < TT_SSL_PEER_EXPIRE_MIN) {
        attr->peer_expire_ms = TT_SSL_PEER_EXPIRE_MIN;
    }

    // peer map
    if (!TT_OK(tt_hmap_create(&sslcache->peer_map,
                              attr->peer_map_slot_num,
                              &attr->peer_map_attr))) {
        goto __scc_fail;
    }
    __done |= __SCC_MAP;

    // lock
    if (attr->multi_thread) {
        if (!TT_OK(tt_spinlock_create(&sslcache->lock, &attr->lock_attr))) {
            goto __scc_fail;
        }
        __done |= __SCC_LOCK;
    }

    return TT_SUCCESS;

__scc_fail:

    if (__done & __SCC_LOCK) {
        tt_spinlock_destroy(&sslcache->lock);
    }

    if (__done & __SCC_MAP) {
        tt_hmap_destroy(&sslcache->peer_map);
    }

    return TT_FAIL;
}

void tt_sslcache_destroy(IN tt_sslcache_t *sslcache)
{
    TT_ASSERT(sslcache != NULL);

    tt_sslcache_clear(sslcache);
    tt_hmap_destroy(&sslcache->peer_map);

    if (sslcache->attr.multi_thread) {
        tt_spinlock_destroy(&sslcache->lock);
    }
}

void tt_sslcache_attr_default(IN tt_sslcache_attr_t *attr)
{
    attr->peer_map_slot_num = 16;
    tt_hmap_attr_default(&attr->peer_map_attr);
    attr->peer_expire_ms = TT_SSL_PEER_EXPIRE;

    tt_spinlock_attr_default(&attr->lock_attr);

    attr->multi_thread = TT_TRUE;
}

void tt_sslcache_clear(IN tt_sslcache_t *sslcache)
{
    tt_hmap_foreach(&sslcache->peer_map,
                    __peer_map_node_release,
                    &sslcache->peer_map);
}

void tt_sslcache_scan(IN tt_sslcache_t *sslcache)
{
    tt_s64_t time_now = tt_time_ref();

    tt_hmap_foreach(&sslcache->peer_map,
                    __peer_check_expire,
                    (void *)(tt_uintptr_t)time_now);
}

tt_sslpeer_t *tt_sslcache_find(IN tt_sslcache_t *sslcache,
                               IN struct tt_sslctx_s *sslctx,
                               IN tt_sktaddr_t *local_addr,
                               IN tt_sktaddr_t *peer_addr,
                               IN tt_u32_t ses_private)
{
    tt_char_t peer_id[TT_SSL_PEER_ID_LEN];
    tt_u32_t peer_id_len;
    tt_hnode_t *hnode;
    tt_sslpeer_t *sslpeer = NULL;

    if ((local_addr == NULL) && (peer_addr == NULL)) {
        return NULL;
    }

    peer_id_len =
        __mk_peer_id(peer_id, sslctx, local_addr, peer_addr, ses_private);

    if (sslcache->attr.multi_thread) {
        tt_spinlock_acquire(&sslcache->lock);
    }

    // try find
    hnode = tt_hmap_find(&sslcache->peer_map, (tt_u8_t *)peer_id, peer_id_len);
    if (hnode != NULL) {
        sslpeer = TT_CONTAINER(hnode, tt_sslpeer_t, hnode);
        tt_sslpeer_ref(sslpeer);
    }

    if (sslcache->attr.multi_thread) {
        tt_spinlock_release(&sslcache->lock);
    }

    return sslpeer;
}

tt_result_t tt_sslcache_add(IN tt_sslcache_t *sslcache,
                            IN struct tt_sslctx_s *sslctx,
                            IN tt_sktaddr_t *local_addr,
                            IN tt_sktaddr_t *peer_addr,
                            IN tt_u32_t peer_private,
                            IN OPT tt_sslpeer_ntv_t *sys_peer)
{
    tt_char_t peer_id[TT_SSL_PEER_ID_LEN];
    tt_u32_t peer_id_len;
    tt_hnode_t *hnode;
    tt_sslpeer_t *sslpeer = NULL;

    if ((local_addr == NULL) && (peer_addr == NULL)) {
        return TT_FAIL;
    }

    peer_id_len =
        __mk_peer_id(peer_id, sslctx, local_addr, peer_addr, peer_private);

    if (sslcache->attr.multi_thread) {
        tt_spinlock_acquire(&sslcache->lock);
    }

    // try find
    hnode = tt_hmap_find(&sslcache->peer_map, (tt_u8_t *)peer_id, peer_id_len);
    if (hnode != NULL) {
        sslpeer = TT_CONTAINER(hnode, tt_sslpeer_t, hnode);

        if (sys_peer != NULL) {
            tt_memcpy(&sslpeer->sys_peer, sys_peer, sizeof(tt_sslpeer_ntv_t));
        }

        // update peer's timestamp
        sslpeer->last_updated = tt_time_ref();

        // TT_DEBUG("ssl peer[%s] is updated", sslpeer->peer_id);
    } else {
        sslpeer = (tt_sslpeer_t *)tt_malloc(sizeof(tt_sslpeer_t));
        if (sslpeer != NULL) {
            __peer_init(sslpeer, peer_id, peer_id_len, sys_peer);
            tt_hmap_add(&sslcache->peer_map,
                        (tt_u8_t *)sslpeer->peer_id,
                        sslpeer->peer_id_len,
                        &sslpeer->hnode);

            // TT_DEBUG("ssl peer[%s] is created", sslpeer->peer_id);
        }
    }

    if (sslcache->attr.multi_thread) {
        tt_spinlock_release(&sslcache->lock);
    }

    if (sslpeer != NULL) {
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

void tt_sslpeer_destroy(IN tt_sslpeer_t *sslpeer)
{
    tt_sslpeer_destroy_ntv(&sslpeer->sys_peer);

    __PEER_NUM_DEC();
}

tt_u32_t tt_sslpeer_num()
{
    return tt_atomic_s32_get(&tt_g_ssl_peer_num);
}

void __peer_map_node2key(IN tt_hnode_t *node,
                         OUT const tt_u8_t **key,
                         OUT tt_u32_t *key_len)
{
    tt_sslpeer_t *peer = TT_CONTAINER(node, tt_sslpeer_t, hnode);

    *key = (const tt_u8_t *)peer->peer_id;
    *key_len = peer->peer_id_len;
}

tt_bool_t __peer_map_node_release(IN tt_u8_t *key,
                                  IN tt_u32_t key_len,
                                  IN tt_hnode_t *hnode,
                                  IN void *param)
{
    tt_sslpeer_t *peer = TT_CONTAINER(hnode, tt_sslpeer_t, hnode);

    tt_hmap_remove((tt_hashmap_t *)param, hnode);
    tt_sslpeer_release(peer);
    // the map has only 1 ref of the peer. if ref of peer is greater than 1,
    // then there must be someone called tt_sslcache_find, the peer would
    // be destroyed when that caller released the ref

    return TT_TRUE;
}

tt_bool_t __peer_check_expire(IN tt_u8_t *key,
                              IN tt_u32_t key_len,
                              IN tt_hnode_t *hnode,
                              IN void *param)
{
    tt_sslpeer_t *sslpeer = TT_CONTAINER(hnode, tt_sslpeer_t, hnode);
    tt_hashmap_t *hmap = (tt_hashmap_t *)param;
    tt_sslcache_t *sslcache = TT_CONTAINER(hmap, tt_sslcache_t, peer_map);
    tt_s64_t time_val;

    // todo, during whole process, we actually only need get current time once
    time_val = tt_time_ref();
    time_val = tt_time_ref2ms(time_val - sslpeer->last_updated);
    TT_ASSERT(time_val >= 0);
    if (time_val >= sslcache->attr.peer_expire_ms) {
        tt_hmap_remove(hmap, hnode);
        tt_sslpeer_release(sslpeer);
    }

    return TT_TRUE;
}

tt_u32_t __mk_peer_id(IN tt_char_t *peer_id,
                      IN struct tt_sslctx_s *sslctx,
                      IN tt_sktaddr_t *local_addr,
                      IN tt_sktaddr_t *remote_addr,
                      IN tt_u32_t peer_private)
{
    tt_net_family_t af;
    tt_sktaddr_addr_t sock_addr;
    tt_u16_t port;
    tt_u32_t len;

    if (sslctx->sys_ctx.role == TT_SSL_ROLE_CLIENT) {
        len = 0;

        // client does not use local port
        af = tt_sktaddr_get_family(local_addr);
        tt_sktaddr_get_addr_n(local_addr, &sock_addr);
        if (af == TT_NET_AF_INET) {
            len += tt_snprintf(peer_id + len,
                               TT_SSL_PEER_ID_LEN - 1 - len,
                               "C:%p:%x:%x:0:",
                               sslctx,
                               peer_private,
                               sock_addr.addr32.__u32);
        } else {
            TT_ASSERT(af == TT_NET_AF_INET6);

            len += tt_snprintf(peer_id + len,
                               TT_SSL_PEER_ID_LEN - 1 - len,
                               "C:%p:%x:%x%x%x%x:0:",
                               sslctx,
                               peer_private,
                               sock_addr.addr128.__u32[0],
                               sock_addr.addr128.__u32[1],
                               sock_addr.addr128.__u32[2],
                               sock_addr.addr128.__u32[3]);
        }

        // client uses remote port as a part of peer key
        af = tt_sktaddr_get_family(remote_addr);
        tt_sktaddr_get_addr_n(remote_addr, &sock_addr);
        tt_sktaddr_get_port(remote_addr, &port);
        if (af == TT_NET_AF_INET) {
            len += tt_snprintf(peer_id + len,
                               TT_SSL_PEER_ID_LEN - 1 - len,
                               "%x:%x",
                               sock_addr.addr32.__u32,
                               port);
        } else {
            TT_ASSERT(af == TT_NET_AF_INET6);

            len += tt_snprintf(peer_id + len,
                               TT_SSL_PEER_ID_LEN - 1 - len,
                               "%x%x%x%x:%x",
                               sock_addr.addr128.__u32[0],
                               sock_addr.addr128.__u32[1],
                               sock_addr.addr128.__u32[2],
                               sock_addr.addr128.__u32[3],
                               port);
        }

        TT_ASSERT(len < TT_SSL_PEER_ID_LEN);
        peer_id[len++] = 0;
    } else {
        len = 0;

        // server uses local port
        af = tt_sktaddr_get_family(local_addr);
        tt_sktaddr_get_addr_n(local_addr, &sock_addr);
        tt_sktaddr_get_port(local_addr, &port);
        if (af == TT_NET_AF_INET) {
            len += tt_snprintf(peer_id + len,
                               TT_SSL_PEER_ID_LEN - 1 - len,
                               "S:%p:%x:%x:%x:",
                               sslctx,
                               peer_private,
                               sock_addr.addr32.__u32,
                               port);
        } else {
            TT_ASSERT(af == TT_NET_AF_INET6);

            len += tt_snprintf(peer_id + len,
                               TT_SSL_PEER_ID_LEN - 1 - len,
                               "S:%p:%x:%x%x%x%x:%x:",
                               sslctx,
                               peer_private,
                               sock_addr.addr128.__u32[0],
                               sock_addr.addr128.__u32[1],
                               sock_addr.addr128.__u32[2],
                               sock_addr.addr128.__u32[3],
                               port);
        }

        // server does not use remote port as a part of peer key
        af = tt_sktaddr_get_family(remote_addr);
        tt_sktaddr_get_addr_n(remote_addr, &sock_addr);
        if (af == TT_NET_AF_INET) {
            len += tt_snprintf(peer_id + len,
                               TT_SSL_PEER_ID_LEN - 1 - len,
                               "%x:0",
                               sock_addr.addr32.__u32);
        } else {
            TT_ASSERT(af == TT_NET_AF_INET6);

            len += tt_snprintf(peer_id + len,
                               TT_SSL_PEER_ID_LEN - 1 - len,
                               "%x%x%x%x:0",
                               sock_addr.addr128.__u32[0],
                               sock_addr.addr128.__u32[1],
                               sock_addr.addr128.__u32[2],
                               sock_addr.addr128.__u32[3]);
        }

        TT_ASSERT(len < TT_SSL_PEER_ID_LEN);
        peer_id[len++] = 0;
    }

#if 1
    TT_ASSERT(len <= TT_SSL_PEER_ID_LEN);
    if (len == TT_SSL_PEER_ID_LEN) {
        TT_WARN("peer id may be truncated");
    }
#endif

    return len;
}

void __peer_init(IN tt_sslpeer_t *sslpeer,
                 IN tt_char_t *peer_id,
                 IN tt_u32_t peer_id_len,
                 IN OPT tt_sslpeer_ntv_t *sys_peer)
{
    tt_mnode_init(&sslpeer->hnode);

    TT_ASSERT(peer_id_len < TT_SSL_PEER_ID_LEN);
    tt_memcpy(sslpeer->peer_id, peer_id, peer_id_len);
    tt_memset(sslpeer->peer_id + peer_id_len,
              0,
              TT_SSL_PEER_ID_LEN - peer_id_len);
    sslpeer->peer_id_len = peer_id_len;

    // ref of owner, generally the hash map hold this ref
    tt_atomic_s32_init(&sslpeer->ref, 1);

    sslpeer->last_updated = tt_time_ref();

    if (sys_peer != NULL) {
        tt_memcpy(&sslpeer->sys_peer, sys_peer, sizeof(tt_sslpeer_ntv_t));
    } else {
        tt_memset(&sslpeer->sys_peer, 0, sizeof(tt_sslpeer_ntv_t));
    }

    __PEER_NUM_INC();
}
