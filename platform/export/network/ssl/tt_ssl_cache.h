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

/**
@file tt_ssl_cache.h
@brief ssl cache

this file defines ssl cache APIs
*/

#ifndef __TT_SSL_CACHE__
#define __TT_SSL_CACHE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_map_hashlist.h>
#include <io/tt_socket_addr.h>
#include <misc/tt_reference_counter.h>
#include <os/tt_spinlock.h>

#include <tt_ssl_peer_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

// peer id format:
// <C|S>:<sslctx(64b)>:<app priv[32b]]>:<local ip(128b)>:<local port(16b):<peer
// ip(128b)>:<peer port(16b)>
// 1+16+8+32+4+32+4 + 6 => 128 bytes are enough
#define TT_SSL_PEER_ID_LEN (128)

#define tt_sslpeer_ref(sslpeer) TT_REF_ADD(tt_sslpeer_t, (sslpeer), ref)
#define tt_sslpeer_release(sslpeer)                                            \
    TT_REF_RELEASE(tt_sslpeer_t, (sslpeer), ref, tt_sslpeer_destroy)

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_sslctx_s;

typedef struct
{
    tt_mnode_t hnode;

    tt_char_t peer_id[TT_SSL_PEER_ID_LEN];
    tt_u32_t peer_id_len;

    tt_atomic_s32_t ref;
    tt_s64_t last_updated;

    tt_sslpeer_ntv_t sys_peer;
} tt_sslpeer_t;

typedef struct
{
    tt_u32_t peer_map_slot_num;
    tt_map_hl_attr_t peer_map_attr;
    tt_s64_t peer_expire_ms;

    tt_spinlock_attr_t lock_attr;

    tt_bool_t multi_thread : 1;
} tt_sslcache_attr_t;

typedef struct
{
    tt_sslcache_attr_t attr;

    tt_map_t *peer_map;
    tt_spinlock_t lock;
} tt_sslcache_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_sslcache_create(IN tt_sslcache_t *sslcache,
                                      IN tt_sslcache_attr_t *attr);

extern void tt_sslcache_destroy(IN tt_sslcache_t *sslcache);

extern void tt_sslcache_attr_default(IN tt_sslcache_attr_t *attr);

extern void tt_sslcache_clear(IN tt_sslcache_t *sslcache);

extern void tt_sslcache_scan(IN tt_sslcache_t *sslcache);

// remember tt_sslpeer_release the peer returned
extern tt_sslpeer_t *tt_sslcache_find(IN tt_sslcache_t *sslcache,
                                      IN struct tt_sslctx_s *sslctx,
                                      IN tt_sktaddr_t *local_addr,
                                      IN tt_sktaddr_t *peer_addr,
                                      IN tt_u32_t app_private);

extern tt_result_t tt_sslcache_add(IN tt_sslcache_t *sslcache,
                                   IN struct tt_sslctx_s *sslctx,
                                   IN tt_sktaddr_t *local_addr,
                                   IN tt_sktaddr_t *peer_addr,
                                   IN tt_u32_t app_private,
                                   IN tt_sslpeer_ntv_t *sys_peer);

extern void tt_sslpeer_destroy(IN tt_sslpeer_t *sslpeer);

extern tt_u32_t tt_sslpeer_num();

#endif /* __TT_SSL_CACHE__ */
