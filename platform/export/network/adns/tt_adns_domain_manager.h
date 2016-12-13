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
@file tt_adns_domain_manager.h
@brief async dns domain manager

this file defines async dns domain manager
*/

#ifndef __TT_ASYNC_DNS_DOMAIN_MANAGER__
#define __TT_ASYNC_DNS_DOMAIN_MANAGER__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_hash_map.h>
#include <algorithm/tt_red_black_tree.h>
#include <network/adns/tt_adns_domain.h>
#include <network/adns/tt_adns_query.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_evcenter_s;
struct tt_evpoller_s;
struct tt_adns_resolver_s;
struct tt_netaddr_s;
struct tt_ev_s;

typedef struct
{
    tt_u32_t domain_map_slot_num;
    tt_hashmap_attr_t domain_map_attr;

    tt_adns_tmr_attr_t tmr_attr;
} tt_adns_dmgr_attr_t;

typedef struct tt_adns_dmgr_s
{
    tt_adns_dmgr_attr_t attr;
    tt_u32_t flag;

    struct tt_evcenter_s *evc;
    tt_hashmap_t domain_map;
    tt_rbtree_t trx_tree;

    struct tt_adns_resolver_s *resolver;
    tt_u32_t resolver_num;
} tt_adns_dmgr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_adns_dmgr_t *tt_adns_dmgr_create(
    IN struct tt_evcenter_s *evc,
    IN OPT struct tt_netaddr_s *server_addr,
    IN OPT tt_u32_t server_num,
    IN OPT tt_adns_dmgr_attr_t *attr);

extern void tt_adns_dmgr_destroy(IN tt_adns_dmgr_t *dmgr);

extern void tt_adns_dmgr_attr_default(IN tt_adns_dmgr_attr_t *attr);

extern void tt_adns_dmgr_clear(IN tt_adns_dmgr_t *dmgr);

extern tt_result_t tt_adns_dmgr_tev_handler(IN struct tt_evpoller_s *evp,
                                            IN struct tt_ev_s *ev);

extern void tt_adns_dmgr_pkt_handler(IN tt_adns_dmgr_t *dmgr,
                                     IN struct tt_adns_pkt_s *pkt);

// ========================================
// dns query API
// ========================================

/*
@return
- TT_SUCCESS, if result is filled with RR
- TT_FAIL, otherwise

@note
- result must not be used before, including no rr
- result->result is valid only when this function return TT_SUCCESS
*/
extern tt_result_t tt_adns_dmgr_query(IN tt_adns_dmgr_t *dmgr,
                                      IN const tt_char_t *domain,
                                      IN tt_adns_rr_type_t type,
                                      IN tt_u32_t flag,
                                      OUT tt_adns_qryctx_t *qryctx);

extern tt_result_t tt_adns_dmgr_query_async(IN tt_adns_dmgr_t *dmgr,
                                            IN const tt_char_t *domain,
                                            IN tt_adns_rr_type_t type,
                                            IN tt_u32_t flag,
                                            IN tt_adns_on_query_t on_query,
                                            IN OPT void *cb_param);

#endif /* __TT_ASYNC_DNS_DOMAIN_MANAGER__ */
