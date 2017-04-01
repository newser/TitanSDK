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

#include <network/adns/tt_adns_domain.h>

#include <event/tt_timer_event.h>
#include <memory/tt_memory_alloc.h>
#include <network/adns/tt_adns_domain_manager.h>
#include <network/adns/tt_adns_packet.h>
#include <network/adns/tt_adns_question.h>
#include <network/adns/tt_adns_resolver.h>
#include <network/adns/tt_adns_rr_cname_in.h>
#include <os/tt_thread.h>
#include <timer/tt_timer.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __RRS_MAX_CNAME_LOOP (10)

#define __RRS_NEW_STATE(rrs, new_state)                                        \
    do {                                                                       \
        TT_DEBUG("adns rrs[%p]: [%d] => [%d]", rrs, rrs->state, new_state);    \
        rrs->state = new_state;                                                \
    } while (0)

#define __NEXT_RETRANS(rrs, tmrattr)                                           \
    TT_COND((((rrs)->cur_retrans) << 1) <= (tmrattr)->max_retrans,             \
            (((rrs)->cur_retrans) << 1),                                       \
            ((rrs)->cur_retrans))

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef enum {
    __RRS_EV_TIMER,
    __RRS_EV_QUERY,
    __RRS_EV_PACKET,
} __rrs_event_t;

typedef enum {
    __RRS_TMR_EV_QUERY,
    __RRS_TMR_EV_EXPIRE,
    __RRS_TMR_EV_DESTROY,
} __rrs_tmr_event_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

extern void __adns_rrs_notify(IN tt_adns_rrset_t *rrs);
extern void __adns_rrs_unsubscribe(IN tt_adns_rrset_t *rrs);

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

#if 1

tt_atomic_s32_t tt_g_adns_dm_num;
#define __ADNS_DM_INC() tt_atomic_s32_inc(&tt_g_adns_dm_num)
#define __ADNS_DM_DEC() tt_atomic_s32_dec(&tt_g_adns_dm_num)

#else

#define __ADNS_DM_INC()
#define __ADNS_DM_DEC()

#endif

#define __ADNS_MAX_TTL 0x7FFFFFFF

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_adns_domain_t *__adns_dm_create(IN const tt_char_t *name,
                                          IN tt_u32_t name_len,
                                          IN tt_adns_dmgr_t *dmgr);

static tt_adns_pkt_t *__adns_query_create(IN tt_u16_t trx_id,
                                          IN const tt_char_t *name,
                                          IN tt_u32_t name_len,
                                          IN tt_adns_rr_type_t type);

// ========================================
// rr set
// ========================================

static tt_adns_rrset_t *__adns_rrs_create(IN tt_adns_rr_type_t type,
                                          IN tt_adns_domain_t *dm);

static void __adns_rrs_destroy(IN tt_adns_rrset_t *rrs);

static tt_result_t __adns_rrs_start_query(IN tt_adns_rrset_t *rrs);

static void __adns_rrs_reset(IN tt_adns_rrset_t *rrs);

static void __adns_rrs_tmr_cb(IN struct tt_tmr_s *timer,
                              IN void *param,
                              IN tt_u32_t reason);

static void __adns_rrs_fsm(IN tt_adns_rrset_t *rrs,
                           IN __rrs_event_t ev,
                           IN void *param);

static tt_adns_resolver_t *__next_rslvr(IN tt_adns_dmgr_t *dmgr,
                                        IN tt_adns_resolver_t *cur_rslvr);

static tt_result_t __adns_rrs_pkt_handler(IN tt_adns_rrset_t *rrs,
                                          IN tt_adns_pkt_t *pkt,
                                          OUT tt_s64_t *next_expire);

// ========================================
// rr
// ========================================

static void __adrr_filter(IN tt_dlist_t *in_rrlist,
                          OUT tt_dlist_t *out_rrlist,
                          IN tt_cmp_t cmp,
                          IN void *cmp_param);
static tt_s32_t __adrr_cmp_type_name(IN void *l, IN void *r);
static tt_s32_t __adrr_cmp_type(IN void *l, IN void *r);
static tt_s32_t __adrr_cmp_name(IN void *l, IN void *r);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_adns_domain_t *tt_adns_domain_create(IN const tt_char_t *name,
                                        IN tt_u32_t name_len,
                                        IN struct tt_adns_dmgr_s *dmgr,
                                        IN OPT tt_adns_rr_type_t *type,
                                        IN tt_u32_t type_num)
{
    tt_hnode_t *hnode;
    tt_adns_domain_t *dm;

    TT_ASSERT(name != NULL);
    TT_ASSERT(name_len != 0);
    TT_ASSERT(dmgr != NULL);

    // find if exist
    hnode = tt_hmap_find(&dmgr->domain_map, (tt_u8_t *)name, name_len);
    if (hnode != NULL) {
        dm = TT_CONTAINER(hnode, tt_adns_domain_t, dmgr_node);
        tt_adns_domain_ref(dm);
        return dm;
    }

    // create
    dm = __adns_dm_create(name, name_len, dmgr);
    if (dm == NULL) {
        return NULL;
    }

    // add to cache
    if (!TT_OK(tt_hmap_add(&dmgr->domain_map,
                           (tt_u8_t *)name,
                           name_len,
                           &dm->dmgr_node))) {
        __adns_domain_destroy(dm);
        return NULL;
    }

    // create and run rr set
    if (type != NULL) {
        tt_u32_t i;

        for (i = 0; i < type_num; ++i) {
            tt_adns_rr_type_t rr_type = type[i];
            TT_ASSERT(TT_ADNS_RR_TYPE_VALID(rr_type));

            dm->rr_set[rr_type] = __adns_rrs_create(rr_type, dm);
            if (dm->rr_set[rr_type] == NULL) {
                __adns_domain_destroy(dm);
                return NULL;
            }
        }

        for (i = 0; i < type_num; ++i) {
            __adns_rrs_start_query(dm->rr_set[type[i]]);
        }
    }

    __ADNS_DM_INC();
    tt_adns_domain_ref(dm);
    return dm;
}

void __adns_domain_destroy(IN tt_adns_domain_t *dm)
{
    tt_u32_t i;

    for (i = 0; i < TT_ADNS_RR_TYPE_NUM; ++i) {
        if (dm->rr_set[i] != NULL) {
            __adns_rrs_destroy(dm->rr_set[i]);
        }
    }

    tt_hmap_remove(&dm->dmgr->domain_map, &dm->dmgr_node);

    tt_free(dm);
}

void tt_adns_domain_query(IN tt_adns_domain_t *dm, IN tt_adns_rr_type_t type)
{
    TT_ASSERT(TT_ADNS_RR_TYPE_VALID(type));

    if (dm->rr_set[type] != NULL) {
        __adns_rrs_fsm(dm->rr_set[type], __RRS_EV_QUERY, NULL);
    }
}

void tt_adns_domain_copy_rr(IN tt_adns_domain_t *dm,
                            IN tt_adns_rr_type_t type,
                            IN tt_u32_t flag,
                            OUT tt_dlist_t *rrlist)
{
    TT_ASSERT(dm != NULL);
    TT_ASSERT(TT_ADNS_RR_TYPE_VALID(type));
    TT_ASSERT(rrlist != NULL);

    if (dm->rr_set[type] != NULL) {
        tt_u32_t cp_flag = 0;

        if (flag & TT_ADNS_QUERY_ALL) {
            cp_flag |= TT_ADNS_RRLIST_COPY_ALL;
        }

        tt_adns_rrlist_copy(rrlist, &dm->rr_set[type]->rrlist, cp_flag);
    } else {
        tt_adns_rrlist_clear(rrlist);
    }
}

tt_dlist_t *tt_adns_domain_get_rrlist(IN tt_adns_domain_t *dm,
                                      IN tt_adns_rr_type_t type)
{
    TT_ASSERT(dm != NULL);
    TT_ASSERT(TT_ADNS_RR_TYPE_VALID(type));

    if (dm->rr_set[type] != NULL) {
        return &dm->rr_set[type]->rrlist;
    } else {
        return NULL;
    }
}

void tt_adns_domain_hnode2key(IN tt_hnode_t *node,
                              OUT const tt_u8_t **key,
                              OUT tt_u32_t *key_len)
{
    tt_adns_domain_t *dm = TT_CONTAINER(node, tt_adns_domain_t, dmgr_node);
    *key = (tt_u8_t *)dm->name;
    *key_len = dm->name_len;
}

tt_result_t tt_adns_domain_subscribe(IN tt_adns_domain_t *dm,
                                     IN tt_adns_rr_type_t type,
                                     IN tt_lnode_t *subscriber)
{
    tt_adns_rrset_t *rrs;

    TT_ASSERT(dm != NULL);
    TT_ASSERT(TT_ADNS_RR_TYPE_VALID(type));
    TT_ASSERT(subscriber != NULL);

    rrs = dm->rr_set[type];
    if (rrs == NULL) {
        rrs = __adns_rrs_create(type, dm);
        if (rrs == NULL) {
            TT_ERROR("fail to subscribe rrs of type[%d]", type);
            return TT_FAIL;
        }
        dm->rr_set[type] = rrs;
    }

    tt_list_push_tail(&rrs->subscriber_q, subscriber);
    return TT_SUCCESS;
}

void tt_adns_rrset_pkt_handler(IN tt_adns_rrset_t *rrs, IN tt_adns_pkt_t *pkt)
{
    __adns_rrs_fsm(rrs, __RRS_EV_PACKET, pkt);
}

tt_adns_domain_t *__adns_dm_create(IN const tt_char_t *name,
                                   IN tt_u32_t name_len,
                                   IN tt_adns_dmgr_t *dmgr)
{
    tt_adns_domain_t *dm;
    tt_u32_t i;

    dm = (tt_adns_domain_t *)tt_malloc(sizeof(tt_adns_domain_t) + name_len);
    if (dm == NULL) {
        TT_ERROR("no mem for dm");
        return NULL;
    }

    dm->name = TT_PTR_INC(const tt_char_t, dm, sizeof(tt_adns_domain_t));
    dm->name_len = name_len;
    tt_memcpy((char *)dm->name, name, name_len);
    // ending null has been copied

    dm->dmgr = dmgr;
    tt_mnode_init(&dm->dmgr_node);
    dm->ref = 0;

    for (i = 0; i < TT_ADNS_RR_TYPE_NUM; ++i) {
        dm->rr_set[i] = NULL;
    }

    return dm;
}

tt_adns_pkt_t *__adns_query_create(IN tt_u16_t trx_id,
                                   IN const tt_char_t *name,
                                   IN tt_u32_t name_len,
                                   IN tt_adns_rr_type_t type)
{
    tt_adns_pkt_t *pkt =
        tt_adns_pkt_create(trx_id, 0, 0, __ADNS_PKT_RECUR_DESIRED);
    if (pkt != NULL) {
        tt_adns_rr_t *quest;

        // must use TT_ADNS_RR_COPY_NAME, as pkt may exist even when
        // the dm is freed
        quest = tt_adns_quest_create(name, type, TT_ADNS_RR_COPY_NAME);
        if (quest != NULL) {
            tt_adns_pkt_add_question(pkt, quest);
        }
    }
    return pkt;
}

// ========================================
// rr set
// ========================================

tt_adns_rrset_t *__adns_rrs_create(IN tt_adns_rr_type_t type,
                                   IN tt_adns_domain_t *dm)
{
    tt_adns_rrset_t *rrs;

    rrs = (tt_adns_rrset_t *)tt_malloc(sizeof(tt_adns_rrset_t));
    if (rrs == NULL) {
        TT_ERROR("fail to create rrs");
        return NULL;
    }

    rrs->type = type;
    rrs->dm = dm;

    tt_dlist_init(&rrs->rrlist);
    tt_list_init(&rrs->subscriber_q);

    rrs->cur_rslvr = NULL;
    rrs->state = TT_ADNS_RRS_INIT;

    tt_rbnode_init(&rrs->trx_node);
    rrs->trx_id = 0;

    rrs->query_pkt = NULL;
    rrs->cur_retrans = dm->dmgr->attr.tmr_attr.init_retrans;
    rrs->elapsed_retrans = 0;
    rrs->refresh_tmr = NULL;

    return rrs;
}

void __adns_rrs_destroy(IN tt_adns_rrset_t *rrs)
{
    __adns_rrs_reset(rrs);

    if (rrs->refresh_tmr != NULL) {
        tt_tmr_destroy(rrs->refresh_tmr);
        rrs->refresh_tmr = NULL;
    }
}

tt_result_t __adns_rrs_start_query(IN tt_adns_rrset_t *rrs)
{
    TT_ASSERT(rrs->refresh_tmr == NULL);

    rrs->refresh_tmr = tt_tmr_create(NULL,
                                     0,
                                     rrs,
                                     __adns_rrs_tmr_cb,
                                     (void *)(tt_uintptr_t)__RRS_TMR_EV_QUERY,
                                     0);
    if (rrs->refresh_tmr == NULL) {
        TT_ERROR("fail to create rrs timer");
        return TT_FAIL;
    }

    if (!TT_OK(tt_tmr_start(rrs->refresh_tmr))) {
        TT_ERROR("fail to start rrs timer");

        tt_tmr_destroy(rrs->refresh_tmr);
        rrs->refresh_tmr = NULL;
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void __adns_rrs_reset(IN tt_adns_rrset_t *rrs)
{
    tt_adns_rrlist_clear(&rrs->rrlist);
    __adns_rrs_unsubscribe(rrs);

    rrs->cur_rslvr = NULL;
    rrs->state = TT_ADNS_RRS_INIT;

    tt_rbtree_remove(&rrs->dm->dmgr->trx_tree, &rrs->trx_node);
    rrs->trx_id = 0;

    if (rrs->query_pkt != NULL) {
        tt_adns_pkt_release(rrs->query_pkt);
        rrs->query_pkt = NULL;
    }

    rrs->cur_retrans = rrs->dm->dmgr->attr.tmr_attr.init_retrans;
    rrs->elapsed_retrans = 0;

    if (rrs->refresh_tmr != NULL) {
        tt_tmr_stop(rrs->refresh_tmr);
    }
}

void __adns_rrs_tmr_cb(IN struct tt_tmr_s *timer,
                       IN void *param,
                       IN tt_u32_t reason)
{
    tt_adns_rrset_t *rrs = (tt_adns_rrset_t *)timer->opaque;

    __adns_rrs_fsm(rrs, __RRS_EV_TIMER, param);
    // do not access rrs after __adns_rrs_fsm(), which may have
    // been destroyed
}

void __adns_rrs_fsm(IN tt_adns_rrset_t *rrs,
                    IN __rrs_event_t ev,
                    IN void *param)
{
    tt_adns_dmgr_t *dmgr = rrs->dm->dmgr;
    tt_adns_tmr_attr_t *tmr_attr = &dmgr->attr.tmr_attr;

    TT_DEBUG("rrs fsm state[%d], ev[%d], param[%p]", rrs->state, ev, param);

    if (ev == __RRS_EV_TIMER) {
        __rrs_tmr_event_t tmr_ev = (__rrs_tmr_event_t)param;
        switch (rrs->state) {
            case TT_ADNS_RRS_INIT: {
                // ignored events
                if (tmr_ev != __RRS_TMR_EV_QUERY) {
                    TT_DEBUG("ingored tmr ev[%d]", tmr_ev);
                    break;
                }

                rrs->cur_rslvr = __next_rslvr(dmgr, rrs->cur_rslvr);
                if (rrs->cur_rslvr != NULL) {
                    __RRS_NEW_STATE(rrs, TT_ADNS_RRS_QUERYING);

                    tt_rbtree_remove(&dmgr->trx_tree, &rrs->trx_node);
                    rrs->trx_id = (tt_u16_t)tt_rand_u32();
                    tt_rbtree_add(&dmgr->trx_tree,
                                  (tt_u8_t *)&rrs->trx_id,
                                  sizeof(rrs->trx_id),
                                  &rrs->trx_node);

                    if (rrs->query_pkt != NULL) {
                        tt_adns_pkt_release(rrs->query_pkt);
                        rrs->query_pkt = NULL;
                    }

                    // send new query request
                    rrs->query_pkt = __adns_query_create(rrs->trx_id,
                                                         rrs->dm->name,
                                                         rrs->dm->name_len,
                                                         rrs->type);
                    if (rrs->query_pkt != NULL) {
                        tt_adns_pkt_generate(rrs->query_pkt);
                        tt_adns_resolver_send(rrs->cur_rslvr, rrs->query_pkt);
                    }

                    rrs->cur_retrans = tmr_attr->init_retrans;
                    rrs->elapsed_retrans = 0;

                    tt_tmr_set_delay(rrs->refresh_tmr, rrs->cur_retrans);
                    tt_tmr_set_cbparam(rrs->refresh_tmr,
                                       (void *)__RRS_TMR_EV_QUERY);
                    tt_tmr_start(rrs->refresh_tmr);

                    return;
                } else {
                    // notify failure
                    tt_adns_rrlist_clear(&rrs->rrlist);
                    __adns_rrs_notify(rrs);

                    __RRS_NEW_STATE(rrs, TT_ADNS_RRS_UNAVAIL);

                    tt_rbtree_remove(&rrs->dm->dmgr->trx_tree, &rrs->trx_node);

                    if (rrs->query_pkt != NULL) {
                        tt_adns_pkt_release(rrs->query_pkt);
                        rrs->query_pkt = NULL;
                    }

                    tt_tmr_set_delay(rrs->refresh_tmr, tmr_attr->max_unavail);
                    tt_tmr_set_cbparam(rrs->refresh_tmr,
                                       (void *)__RRS_TMR_EV_DESTROY);
                    tt_tmr_start(rrs->refresh_tmr);

                    return;
                }
            } break;

            case TT_ADNS_RRS_QUERYING: {
                // ignored events
                if (tmr_ev != __RRS_TMR_EV_QUERY) {
                    TT_DEBUG("ingored tmr ev[%d]", tmr_ev);
                    break;
                }

                rrs->elapsed_retrans += rrs->cur_retrans;
                rrs->cur_retrans = __NEXT_RETRANS(rrs, tmr_attr);

                if (rrs->elapsed_retrans <= tmr_attr->total_retrans) {
                    tt_adns_resolver_send(rrs->cur_rslvr, rrs->query_pkt);

                    // if timer expired at [0], [2], [4], ... and total_retrans
                    // is [6],
                    // then the query packet would still be sent at [6], and rr
                    // turn to
                    // invalid state at [8]x
                    tt_tmr_set_delay(rrs->refresh_tmr, rrs->cur_retrans);
                    tt_tmr_set_cbparam(rrs->refresh_tmr,
                                       (void *)__RRS_TMR_EV_QUERY);
                    tt_tmr_start(rrs->refresh_tmr);

                    return;
                } else {
                    // try next name server
                    __RRS_NEW_STATE(rrs, TT_ADNS_RRS_INIT);

                    tt_tmr_set_delay(rrs->refresh_tmr, 0);
                    tt_tmr_set_cbparam(rrs->refresh_tmr,
                                       (void *)__RRS_TMR_EV_QUERY);
                    tt_tmr_start(rrs->refresh_tmr);

                    return;
                }
            } break;

            case TT_ADNS_RRS_READY: {
                // ignored events
                if (tmr_ev != __RRS_TMR_EV_EXPIRE) {
                    TT_DEBUG("ingored tmr ev[%d]", tmr_ev);
                    break;
                }

                // RR TTL expires, do refreshing
                rrs->cur_rslvr = NULL;
                __RRS_NEW_STATE(rrs, TT_ADNS_RRS_INIT);

                tt_tmr_set_delay(rrs->refresh_tmr, 0);
                tt_tmr_set_cbparam(rrs->refresh_tmr,
                                   (void *)__RRS_TMR_EV_QUERY);
                tt_tmr_start(rrs->refresh_tmr);

                return;
            } break;

            case TT_ADNS_RRS_UNAVAIL: {
                // ignored events
                if (tmr_ev != __RRS_TMR_EV_DESTROY) {
                    TT_DEBUG("ingored tmr ev[%d]", tmr_ev);
                    break;
                }

                TT_ASSERT(rrs->dm->rr_set[rrs->type] == rrs);
                rrs->dm->rr_set[rrs->type] = NULL;
                __adns_rrs_destroy(rrs);
                return;
            } break;

            default: {
                TT_DEBUG("ignored ev[%d] in state[%d]", ev, rrs->state);
            } break;
        }
    } else if (ev == __RRS_EV_QUERY) {
        switch (rrs->state) {
            case TT_ADNS_RRS_UNAVAIL: {
                // restart query
                rrs->cur_rslvr = NULL;
                __RRS_NEW_STATE(rrs, TT_ADNS_RRS_INIT);

                tt_tmr_set_delay(rrs->refresh_tmr, 0);
                tt_tmr_set_cbparam(rrs->refresh_tmr,
                                   (void *)__RRS_TMR_EV_QUERY);
                tt_tmr_start(rrs->refresh_tmr);

                return;
            } break;

            // ignore query req in other state
            default: {
                TT_DEBUG("ignored ev[%d] in state[%d]", ev, rrs->state);
            } break;
        }
    } else if (ev == __RRS_EV_PACKET) {
        tt_adns_pkt_t *pkt = (tt_adns_pkt_t *)param;
        tt_s64_t next_expire = TT_TIME_INFINITE;
        tt_result_t result;

        if (!(pkt->flag & __ADNS_PKT_RESP)) {
            TT_WARN("not dns response");
            return;
        }

        // the response could match an ongoing query, so destroy
        // the existing trasaction
        tt_rbtree_remove(&rrs->dm->dmgr->trx_tree, &rrs->trx_node);

        if (rrs->query_pkt != NULL) {
            tt_adns_pkt_release(rrs->query_pkt);
            rrs->query_pkt = NULL;
        }

        if (pkt->rcode != 0) {
            // try next ns
            __RRS_NEW_STATE(rrs, TT_ADNS_RRS_INIT);

            tt_tmr_set_delay(rrs->refresh_tmr, 0);
            tt_tmr_set_cbparam(rrs->refresh_tmr, (void *)__RRS_TMR_EV_QUERY);
            tt_tmr_start(rrs->refresh_tmr);

            return;
        }

        result = __adns_rrs_pkt_handler(rrs, pkt, &next_expire);
        if (TT_OK(result)) {
            // notify waiter
            __adns_rrs_notify(rrs);

            if (next_expire > 0) {
                __RRS_NEW_STATE(rrs, TT_ADNS_RRS_READY);

                tt_tmr_set_delay(rrs->refresh_tmr, next_expire);
                tt_tmr_set_cbparam(rrs->refresh_tmr,
                                   (void *)__RRS_TMR_EV_EXPIRE);
                tt_tmr_start(rrs->refresh_tmr);

                return;
            } else {
                // next expire 0 indicates "not cachable"
                tt_adns_rrlist_clear(&rrs->rrlist);

                __RRS_NEW_STATE(rrs, TT_ADNS_RRS_UNAVAIL);

                tt_tmr_set_delay(rrs->refresh_tmr, tmr_attr->max_unavail);
                tt_tmr_set_cbparam(rrs->refresh_tmr,
                                   (void *)__RRS_TMR_EV_DESTROY);
                tt_tmr_start(rrs->refresh_tmr);

                return;
            }
        } else {
            // try next name server
            __RRS_NEW_STATE(rrs, TT_ADNS_RRS_INIT);

            tt_tmr_set_delay(rrs->refresh_tmr, 0);
            tt_tmr_set_cbparam(rrs->refresh_tmr, (void *)__RRS_TMR_EV_QUERY);
            tt_tmr_start(rrs->refresh_tmr);

            return;
        }
    }
}

tt_adns_resolver_t *__next_rslvr(IN tt_adns_dmgr_t *dmgr,
                                 IN tt_adns_resolver_t *cur_rslvr)
{
    tt_u32_t idx;

    if (cur_rslvr != NULL) {
        TT_ASSERT(cur_rslvr >= &dmgr->resolver[0]);
        TT_ASSERT(cur_rslvr < &dmgr->resolver[dmgr->resolver_num]);
        idx = (tt_u32_t)(cur_rslvr - &dmgr->resolver[0]);
        idx += 1;
        /*
        idx = (tt_u32_t)TT_PTR_DIFF(cur_rslvr, &dmgr->ns[0]);
        idx /= sizeof(tt_adns_resolver_t);
        idx += 1;
         */
    } else {
        idx = 0;
    }

    while (idx < dmgr->resolver_num) {
        if (dmgr->resolver[idx].state == TT_ADRSLVR_CONNECTED) {
            return &dmgr->resolver[idx];
        }

        ++idx;
    }
    return NULL;
}

tt_result_t __adns_rrs_pkt_handler(IN tt_adns_rrset_t *rrs,
                                   IN tt_adns_pkt_t *pkt,
                                   OUT tt_s64_t *next_expire)
{
    tt_adns_domain_t *dm = rrs->dm;
    tt_dlist_t rrlist, cname_rrlist, cname_rrlist2;
    tt_dnode_t *node;
    tt_u32_t cname_loop_level = 0;

    // find rr matching specified type and name
    tt_dlist_init(&rrlist);
    tt_adns_rrlist_filter_tn(&pkt->answer,
                             rrs->type,
                             dm->name,
                             dm->name_len,
                             &rrlist);
    if (!tt_dlist_empty(&rrlist)) {
        goto __calc_exp;
        // so it would ignore cname rr when server explicitly provided answer
    }

    if (rrs->type == TT_ADNS_RR_CNAME_IN) {
        return TT_FAIL;
    }

    // if not find rr answer, check if cname rr exist and have
    // corresponding rr of expecting type
    tt_dlist_init(&cname_rrlist);
    tt_dlist_init(&cname_rrlist2);
    tt_adns_rrlist_filter_tn(&pkt->answer,
                             TT_ADNS_RR_CNAME_IN,
                             dm->name,
                             dm->name_len,
                             &cname_rrlist);

__cn_ag:
    while ((node = tt_dlist_pop_head(&cname_rrlist)) != NULL) {
        tt_adns_rr_t *rr = TT_CONTAINER(node, tt_adns_rr_t, node);
        tt_adrr_cname_t *cname = TT_ADRR_CAST(rr, tt_adrr_cname_t);

        // __adrr_filter_type_name could directly move matching rr
        // in answer list to rrlist
        tt_adns_rrlist_filter_tn(&pkt->answer,
                                 rrs->type,
                                 cname->cname,
                                 cname->cname_len,
                                 &rrlist);

        // cname1 -> cname2 -> ...
        // but note infinite loop
        tt_adns_rrlist_filter_tn(&pkt->answer,
                                 TT_ADNS_RR_CNAME_IN,
                                 cname->cname,
                                 cname->cname_len,
                                 &cname_rrlist2);

        // we do not save cname rr now
        tt_adns_rr_destroy(rr);
    }
    TT_ASSERT(tt_dlist_empty(&cname_rrlist));
    if (!tt_dlist_empty(&cname_rrlist2) &&
        (cname_loop_level++ < __RRS_MAX_CNAME_LOOP)) {
        tt_dlist_move(&cname_rrlist, &cname_rrlist2);
        TT_ASSERT(tt_dlist_empty(&cname_rrlist2));

        goto __cn_ag;
    }

__calc_exp:
    if (!tt_dlist_empty(&rrlist)) {
        tt_s64_t min_ttl = __ADNS_MAX_TTL;
        tt_adns_rr_t *rr;
        tt_adns_dmgr_t *dmgr = rrs->dm->dmgr;
        tt_adns_tmr_attr_t *tmr_cfg = &dmgr->attr.tmr_attr;

        // those rr managed by rr sets of dm does not need to hold
        // a copy of dm name
        tt_adns_rrlist_set_name(&rrlist, NULL, 0, TT_ADNS_RR_REF_NAME);

        tt_adns_rrlist_clear(&rrs->rrlist);
        tt_dlist_move(&rrs->rrlist, &rrlist);

        // calculate next expiration
        node = tt_dlist_head(&rrs->rrlist);
        while (node != NULL) {
            rr = TT_CONTAINER(node, tt_adns_rr_t, node);
            if (rr->ttl < min_ttl) {
                min_ttl = rr->ttl;
            }

            node = node->next;
        }
        min_ttl *= 1000;

        if (min_ttl > (dmgr->resolver_num * tmr_cfg->total_retrans)) {
            *next_expire =
                min_ttl - (dmgr->resolver_num * tmr_cfg->total_retrans);
            TT_DEBUG("next expire is set to %d", *next_expire);
        } else if (min_ttl > tmr_cfg->total_retrans) {
            *next_expire = min_ttl - tmr_cfg->total_retrans;
            TT_DEBUG("next expire is set to %d", *next_expire);
        } else {
            *next_expire = 0;
            TT_DEBUG("next expire is set to 0");
        }

        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}
