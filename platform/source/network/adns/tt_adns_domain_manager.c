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

#include <network/adns/tt_adns_domain_manager.h>

#include <event/tt_event_base.h>
#include <event/tt_event_center.h>
#include <memory/tt_memory_alloc.h>
#include <misc/tt_assert.h>
#include <network/adns/tt_adns_domain.h>
#include <network/adns/tt_adns_domain_name.h>
#include <network/adns/tt_adns_packet.h>
#include <network/adns/tt_adns_query.h>
#include <network/adns/tt_adns_resolver.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

enum
{
    TT_ADNS_RANGE_DMGR,
};

enum
{
    __ADNS_DMGR_EV_START =
        TT_EV_MKID_SUB(TT_EV_RANGE_INTERNAL_ASYNC_DNS, TT_ADNS_RANGE_DMGR, 0),

    __ADM_QUERY_REQ,

    __ADNS_DMGR_EV_END,
};

typedef struct
{
    tt_char_t *name;
    tt_u32_t name_len;
    tt_adns_rr_type_t type;
    tt_u32_t flag;

    tt_adns_on_query_t on_query;
    tt_adns_qryctx_t qryctx;
} __admgr_query_req_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __adns_tmr_attr_check(IN tt_adns_tmr_attr_t *tmr_attr);

static tt_s32_t __trx_cmp(IN void *l, IN void *r);
static tt_s32_t __trx_cmpkey(IN void *n,
                             IN const tt_u8_t *key,
                             IN tt_u32_t key_len);

static tt_bool_t __adns_dm_entry_destroy(IN tt_u8_t *key,
                                         IN tt_u32_t key_len,
                                         IN tt_hnode_t *mnode,
                                         IN void *param);

static tt_bool_t __do_admgr_query(IN __admgr_query_req_t *req,
                                  IN tt_adns_dmgr_t *dmgr);

static tt_adns_domain_t *__admgr_query(IN tt_adns_dmgr_t *dmgr,
                                       IN const tt_char_t *domain,
                                       IN tt_u32_t name_len,
                                       IN tt_adns_rr_type_t type,
                                       IN tt_u32_t flag,
                                       OUT tt_adns_qryctx_t *qryctx);

static void __do_admgr_query_cb(IN __admgr_query_req_t *req,
                                IN tt_adns_dmgr_t *dmgr);

static void __copy_dm_name(IN tt_char_t *dst,
                           IN const tt_char_t *src,
                           IN tt_u32_t len);

void __adns_rrs_notify(IN tt_adns_rrset_t *rrs);

void __adns_rrs_unsubscribe(IN tt_adns_rrset_t *rrs);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_adns_dmgr_t *tt_adns_dmgr_create(IN struct tt_evcenter_s *evc,
                                    IN OPT struct tt_netaddr_s *server_addr,
                                    IN OPT tt_u32_t server_num,
                                    IN OPT tt_adns_dmgr_attr_t *attr)
{
    tt_u32_t dmgr_size;
    tt_adns_dmgr_t *dmgr;
    tt_result_t result;
    tt_u32_t idx;

    tt_u32_t __done = 0;
#define __ADC_SVR_ADDR (1 << 0)
#define __ADC_MEM (1 << 1)
#define __ADC_HASHMAP (1 << 2)
#define __ADC_RSLVR (1 << 3)

    TT_ASSERT(evc != NULL);
    TT_ASSERT((server_addr == NULL) || (server_num > 0));

    if (server_addr == NULL) {
        // load name server addr from os

        __done |= __ADC_SVR_ADDR;
    }
    TT_ASSERT(server_addr != NULL);
    TT_ASSERT(server_num > 0);

    dmgr_size = sizeof(tt_adns_dmgr_t);
    // each server has an corresponding resolver
    dmgr_size += sizeof(tt_adns_resolver_t) * server_num;

    // alloc
    dmgr = (tt_adns_dmgr_t *)tt_malloc(dmgr_size);
    if (dmgr == NULL) {
        TT_ERROR("no mem for adns cache");
        return NULL;
    }
    __done |= __ADC_MEM;

    // attribute
    if (attr == NULL) {
        tt_adns_dmgr_attr_default(&dmgr->attr);
    } else {
        tt_memcpy(&dmgr->attr, attr, sizeof(tt_adns_dmgr_attr_t));
    }
    attr = &dmgr->attr;

    // check timer attr
    result = __adns_tmr_attr_check(&attr->tmr_attr);
    if (!TT_OK(result)) {
        goto __acc_fail;
    }

    dmgr->flag = 0;

    // evc
    dmgr->evc = evc;

    // domain map
    if (TT_OK(tt_hmap_create(&dmgr->domain_map,
                             attr->domain_map_slot_num,
                             &attr->domain_map_attr))) {
        TT_ERROR("fail to create adns dmgr domain map");
        goto __acc_fail;
    }
    __done |= __ADC_HASHMAP;

    // transaction tree
    tt_rbtree_init(&dmgr->trx_tree, __trx_cmp, __trx_cmpkey);

    // name server
    dmgr->resolver =
        TT_PTR_INC(tt_adns_resolver_t, dmgr, sizeof(tt_adns_dmgr_t));
    dmgr->resolver_num = server_num;
    for (idx = 0; idx < dmgr->resolver_num; ++idx) {
        tt_adns_resolver_attr_t resolver_attr;

        tt_adns_resolver_attr_default(&resolver_attr);
        // change any default value of resovler attr?

        if (!TT_OK(tt_adns_resolver_create(&dmgr->resolver[idx],
                                           dmgr,
                                           &server_addr[idx],
                                           &resolver_attr))) {
            tt_u32_t k;
            for (k = 0; k < idx; ++k) {
                tt_adns_resolver_destroy(&dmgr->resolver[k]);
            }
            goto __acc_fail;
        }
    }
    __done |= __ADC_RSLVR;

    if (__done & __ADC_SVR_ADDR) {
        // free server addr
    }

    return dmgr;

__acc_fail:

    if (__done & __ADC_RSLVR) {
        for (idx = 0; idx < dmgr->resolver_num; ++idx) {
            tt_adns_resolver_destroy(&dmgr->resolver[idx]);
        }
    }

    if (__done & __ADC_HASHMAP) {
        tt_hmap_destroy(&dmgr->domain_map);
    }

    if (__done & __ADC_MEM) {
        tt_free(dmgr);
    }

    if (__done & __ADC_SVR_ADDR) {
        // free server addr
    }

    return NULL;
}

void tt_adns_dmgr_destroy(IN tt_adns_dmgr_t *dmgr)
{
    tt_u32_t idx;

    TT_ASSERT(dmgr != NULL);

    for (idx = 0; idx < dmgr->resolver_num; ++idx) {
        tt_adns_resolver_destroy(&dmgr->resolver[idx]);
    }

    tt_adns_dmgr_clear(dmgr);
    tt_hmap_destroy(&dmgr->domain_map);

    // all transactions should have been destroyed in
    // tt_adns_dmgr_clear()
    TT_ASSERT(tt_rbtree_empty(&dmgr->trx_tree));

    tt_free(dmgr);
}

void tt_adns_dmgr_attr_default(IN tt_adns_dmgr_attr_t *attr)
{
    tt_hmap_attr_t *domain_map_attr;
    tt_adns_tmr_attr_t *tmr_attr;

    TT_ASSERT(attr != NULL);

    // domain map
    attr->domain_map_slot_num = 32;

    domain_map_attr = &attr->domain_map_attr;
    tt_hmap_attr_default(domain_map_attr);

    // timer
    tmr_attr = &attr->tmr_attr;

    tmr_attr->init_retrans = 1 * 1000; // 1s
    tmr_attr->max_retrans = 4 * 1000; // 4s
    tmr_attr->total_retrans = 16 * 1000; // 16s

    tmr_attr->max_unavail = 30 * 1000; // to be estimated
}

void tt_adns_dmgr_clear(IN tt_adns_dmgr_t *dmgr)
{
    TT_ASSERT(dmgr != NULL);

    tt_hmap_foreach(&dmgr->domain_map, __adns_dm_entry_destroy, dmgr);
}

tt_result_t tt_adns_dmgr_tev_handler(IN tt_evpoller_t *evp, IN tt_ev_t *ev)
{
    tt_adns_dmgr_t *dmgr = evp->evc->adns_dmgr;
    TT_ASSERT(dmgr != NULL);

    switch (ev->ev_id) {
        case __ADM_QUERY_REQ: {
            __admgr_query_req_t *req = TT_EV_DATA(ev, __admgr_query_req_t);

            if (__do_admgr_query(req, dmgr)) {
                tt_ev_destroy(ev);
            }
        } break;

        default: {
            TT_FATAL("unknown adns dmgr event: %x", ev->ev_id);
            tt_ev_destroy(ev);
        } break;
    }
    return TT_SUCCESS;
}

void tt_adns_dmgr_pkt_handler(IN tt_adns_dmgr_t *dmgr, IN tt_adns_pkt_t *pkt)
{
    tt_rbnode_t *rbt_node;

    TT_ASSERT(dmgr != NULL);
    TT_ASSERT(pkt != NULL);

    rbt_node = tt_rbtree_find_k(dmgr->trx_tree.root,
                                (tt_u8_t *)&pkt->__id,
                                sizeof(pkt->__id));
    if (rbt_node != NULL) {
        tt_adns_rrset_t *rrs =
            TT_CONTAINER(rbt_node, tt_adns_rrset_t, trx_node);
        tt_adns_rrset_pkt_handler(rrs, pkt);
    }
    // else ignore it
}

tt_result_t tt_adns_dmgr_query(IN tt_adns_dmgr_t *dmgr,
                               IN const tt_char_t *domain,
                               IN tt_adns_rr_type_t type,
                               IN tt_u32_t flag,
                               OUT tt_adns_qryctx_t *qryctx)
{
    tt_u32_t name_len;
    tt_char_t *q_name;

    TT_ASSERT(dmgr != NULL);
    TT_ASSERT(domain != NULL);
    TT_ASSERT(TT_ADNS_RR_TYPE_VALID(type));
    TT_ASSERT(qryctx != NULL);

    // check domain name
    name_len = (tt_u32_t)tt_strnlen(domain, TT_MAX_DOMAIN_NAME_LEN);
    if (name_len >= TT_MAX_DOMAIN_NAME_LEN) {
        TT_ERROR("too long domain name");
        return TT_BAD_PARAM;
    }
    name_len += 1;

    q_name = (tt_char_t *)tt_malloc(name_len);
    if (q_name == NULL) {
        TT_ERROR("no mem for q_name");
        return TT_NO_RESOURCE;
    }
    __copy_dm_name(q_name, domain, name_len);

    qryctx->cb_param = NULL;
    qryctx->result = TT_FAIL;
    qryctx->rrlist = NULL;

    __admgr_query(dmgr, q_name, name_len, type, flag, qryctx);

    tt_free(q_name);
    return qryctx->result;
}

tt_result_t tt_adns_dmgr_query_async(IN tt_adns_dmgr_t *dmgr,
                                     IN const tt_char_t *domain,
                                     IN tt_adns_rr_type_t type,
                                     IN tt_u32_t flag,
                                     IN tt_adns_on_query_t on_query,
                                     IN OPT void *cb_param)
{
    tt_u32_t name_len;
    tt_ev_t *ev;
    __admgr_query_req_t *req;
    tt_u32_t req_len = 0;

    TT_ASSERT(dmgr != NULL);
    TT_ASSERT(domain != NULL);
    TT_ASSERT(TT_ADNS_RR_TYPE_VALID(type));
    TT_ASSERT(on_query != NULL);

    // check domain name
    name_len = (tt_u32_t)tt_strnlen(domain, TT_MAX_DOMAIN_NAME_LEN);
    if (name_len >= TT_MAX_DOMAIN_NAME_LEN) {
        TT_ERROR("too long domain name");
        return TT_BAD_PARAM;
    }
    // ending null
    name_len += 1;

    // req
    req_len = sizeof(__admgr_query_req_t) + name_len;
    ev = tt_thread_ev_create(__ADM_QUERY_REQ, req_len, NULL);
    if (ev == NULL) {
        TT_ERROR("no memory for adns query req");
        return TT_NO_RESOURCE;
    }
    req = TT_EV_DATA(ev, __admgr_query_req_t);

    req->name = TT_PTR_INC(tt_char_t, req, sizeof(__admgr_query_req_t));
    __copy_dm_name(req->name, domain, name_len);
    // ending null has been copied
    req->name_len = name_len;

    req->type = type;
    req->flag = flag;

    req->on_query = on_query;

    req->qryctx.result = TT_PROCEEDING;
    req->qryctx.cb_param = cb_param;
    req->qryctx.rrlist = NULL;

    // send req
    if (!TT_OK(tt_evc_sendto_thread(dmgr->evc, ev))) {
        TT_ERROR("fail to send adns query req");
        tt_ev_destroy(ev);
        return TT_FAIL;
    }
    return TT_SUCCESS;
}

tt_result_t __adns_tmr_attr_check(IN tt_adns_tmr_attr_t *tmr_attr)
{
    if (tmr_attr->init_retrans == 0) {
        TT_ERROR("init retrans can not be 0");
        return TT_FAIL;
    }

    if (tmr_attr->init_retrans > tmr_attr->max_retrans) {
        TT_ERROR("init_retrans[%u] > max_retrans[%u]",
                 tmr_attr->init_retrans,
                 tmr_attr->max_retrans);
        return TT_FAIL;
    }

    if (tmr_attr->max_retrans > tmr_attr->total_retrans) {
        TT_ERROR("max_retrans[%u] > total_retrans[%u]",
                 tmr_attr->max_retrans,
                 tmr_attr->total_retrans);
        return TT_FAIL;
    }

    if ((tmr_attr->max_retrans >= 0x80000000) ||
        (tmr_attr->total_retrans >= 0x80000000)) {
        TT_ERROR("max_retrans[%u] or total_retrans[%u] too large",
                 tmr_attr->max_retrans,
                 tmr_attr->total_retrans);
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_s32_t __trx_cmp(IN void *l, IN void *r)
{
    tt_adns_rrset_t *l_rrs =
        TT_CONTAINER((tt_rbnode_t *)l, tt_adns_rrset_t, trx_node);
    tt_adns_rrset_t *r_rrs =
        TT_CONTAINER((tt_rbnode_t *)r, tt_adns_rrset_t, trx_node);
    return (tt_s32_t)(l_rrs->trx_id - r_rrs->trx_id);
}

tt_s32_t __trx_cmpkey(IN void *n, IN const tt_u8_t *key, IN tt_u32_t key_len)
{
    tt_adns_rrset_t *rrs =
        TT_CONTAINER((tt_rbnode_t *)n, tt_adns_rrset_t, trx_node);
    tt_u16_t trx_id = *((tt_u16_t *)key);
    return (tt_s32_t)rrs->trx_id - trx_id;
}

tt_bool_t __adns_dm_entry_destroy(IN tt_u8_t *key,
                                  IN tt_u32_t key_len,
                                  IN tt_hnode_t *mnode,
                                  IN void *param)
{
    tt_adns_domain_t *dm = TT_CONTAINER(mnode, tt_adns_domain_t, dmgr_node);
    tt_adns_domain_release(dm);

    return TT_TRUE;
}

tt_bool_t __do_admgr_query(IN __admgr_query_req_t *req, IN tt_adns_dmgr_t *dmgr)
{
    tt_adns_qryctx_t *qryctx = &req->qryctx;
    tt_adns_domain_t *dm = NULL;

    dm = __admgr_query(dmgr,
                       req->name,
                       req->name_len,
                       req->type,
                       req->flag,
                       qryctx);
    if ((qryctx->rrlist != NULL) && !tt_dlist_empty(qryctx->rrlist)) {
        TT_ASSERT(TT_OK(qryctx->result));
        __do_admgr_query_cb(req, dmgr);
        return TT_TRUE;
    } else if (dm != NULL) {
        tt_thread_ev_t *tev = TT_TEV_OF(req);
        tt_adns_domain_subscribe(dm, req->type, &tev->node);
        tt_adns_domain_query(dm, req->type);
        return TT_FALSE;
    } else {
        qryctx->result = TT_FAIL;
        __do_admgr_query_cb(req, dmgr);
        return TT_TRUE;
    }
}

tt_adns_domain_t *__admgr_query(IN tt_adns_dmgr_t *dmgr,
                                IN const tt_char_t *name,
                                IN tt_u32_t name_len,
                                IN tt_adns_rr_type_t type,
                                IN tt_u32_t flag,
                                OUT tt_adns_qryctx_t *qryctx)
{
    tt_hnode_t *hnode;
    tt_adns_domain_t *dm = NULL;

    // this function assumes qryctx had been initialized

    hnode = tt_hmap_find(&dmgr->domain_map, (tt_u8_t *)name, name_len);
    if (hnode != NULL) {
        dm = TT_CONTAINER(hnode, tt_adns_domain_t, dmgr_node);
        qryctx->rrlist = tt_adns_domain_get_rrlist(dm, type);
        if ((qryctx->rrlist != NULL) && !tt_dlist_empty(qryctx->rrlist)) {
            qryctx->result = TT_SUCCESS;
        } else {
            // maybe this domain has rr of other rr types.
            // ask domain to start dns query for this rr type
            tt_adns_domain_query(dm, type);
        }
    } else if (flag & TT_ADNS_QUERY_NEW) {
        dm = tt_adns_domain_create(name, name_len, dmgr, &type, 1);
    }

    return dm;
}

void __do_admgr_query_cb(IN __admgr_query_req_t *req, IN tt_adns_dmgr_t *dmgr)
{
    TT_ASSERT(req->qryctx.result != TT_PROCEEDING);

    req->on_query(req->name, req->type, req->flag, &req->qryctx);
}

void __copy_dm_name(IN tt_char_t *dst, IN const tt_char_t *src, IN tt_u32_t len)
{
    tt_u32_t i = 0;
    while (i < len) {
        tt_char_t c = src[i];
        if ((c >= 'A') && (c <= 'Z')) {
            dst[i] = c - 'A' + 'a';
        } else {
            dst[i] = c;
        }
        ++i;
    }
}

void __adns_rrs_notify(IN tt_adns_rrset_t *rrs)
{
    tt_lnode_t *node;
    while ((node = tt_list_pop_head(&rrs->subscriber_q)) != NULL) {
        tt_ev_t *ev = TT_EV_OF(TT_CONTAINER(node, tt_thread_ev_t, node));
        __admgr_query_req_t *req = TT_EV_DATA(ev, __admgr_query_req_t);
        tt_adns_qryctx_t *qryctx = &req->qryctx;
        tt_u32_t c_flag = 0;

        TT_ASSERT(req->type == rrs->type);

        // keep qryctx->cb_param
        qryctx->result = TT_FAIL;
        qryctx->rrlist = NULL;

        if (req->flag & TT_ADNS_QUERY_ALL) {
            c_flag |= TT_ADNS_RRLIST_COPY_ALL;
        }
        qryctx->rrlist = &rrs->rrlist;
        if (!tt_dlist_empty(qryctx->rrlist)) {
            qryctx->result = TT_SUCCESS;
        }

        __do_admgr_query_cb(req, rrs->dm->dmgr);
        tt_ev_destroy(ev);
    }
}

void __adns_rrs_unsubscribe(IN tt_adns_rrset_t *rrs)
{
    tt_lnode_t *node;
    while ((node = tt_list_pop_head(&rrs->subscriber_q)) != NULL) {
        tt_ev_t *ev = TT_EV_OF(TT_CONTAINER(node, tt_thread_ev_t, node));
        __admgr_query_req_t *req = TT_EV_DATA(ev, __admgr_query_req_t);
        TT_ASSERT(req->type == rrs->type);

        tt_ev_destroy(ev);
    }
}
