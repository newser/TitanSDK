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
@file tt_adns_domain.h
@brief async dns dm

this file defines async dns dm
*/

#ifndef __TT_ASYNC_DNS_DOMAIN__
#define __TT_ASYNC_DNS_DOMAIN__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_double_linked_list.h>
#include <algorithm/tt_hashmap.h>
#include <algorithm/tt_list.h>
#include <algorithm/tt_red_black_tree.h>
#include <misc/tt_reference_counter.h>
#include <network/adns/tt_adns_rr.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_adns_dmgr_s;
struct tt_adns_domain_s;
struct tt_adns_pkt_s;
struct tt_tmr_s;

typedef enum {
    TT_ADNS_RRS_INIT,
    TT_ADNS_RRS_QUERYING,
    TT_ADNS_RRS_READY,
    TT_ADNS_RRS_UNAVAIL,
} tt_adns_rrs_state_t;

typedef struct
{
    tt_adns_rr_type_t type;
    struct tt_adns_domain_s *dm;

    tt_dlist_t rrlist;
    tt_list_t subscriber_q;

    struct tt_adns_resolver_s *cur_rslvr;
    tt_adns_rrs_state_t state;

    tt_rbnode_t trx_node;
    tt_u16_t trx_id;

    struct tt_adns_pkt_s *query_pkt;
    tt_u32_t cur_retrans;
    tt_u32_t elapsed_retrans;
    struct tt_tmr_s *refresh_tmr;
} tt_adns_rrset_t;

typedef struct tt_adns_domain_s
{
    // [howto]as rfc suggested, dm name should reserve char case,
    // but current implementation does not follow that
    const tt_char_t *name;
    tt_u32_t name_len; // including terminating null

    struct tt_adns_dmgr_s *dmgr;
    tt_hnode_t dmgr_node;
    tt_atomic_s32_t ref;

    tt_adns_rrset_t *rr_set[TT_ADNS_RR_TYPE_NUM];
} tt_adns_domain_t;

typedef struct tt_adns_tmr_attr_s
{
    // retrans query
    tt_u32_t init_retrans;
    tt_u32_t max_retrans;
    tt_u32_t total_retrans;

    // time to keep rr set when ttl expired and refresh failed, but note
    // during this period, no rr is valid, only the rrs is there
    tt_u32_t max_unavail;
} tt_adns_tmr_attr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/**
 @note
 - name_len is the length including 0
 */
tt_export tt_adns_domain_t *tt_adns_domain_create(IN const tt_char_t *name,
                                               IN tt_u32_t name_len,
                                               IN struct tt_adns_dmgr_s *dmgr,
                                               IN OPT tt_adns_rr_type_t *type,
                                               IN tt_u32_t type_num);

tt_export void __adns_domain_destroy(IN tt_adns_domain_t *dm);

#define tt_adns_domain_ref(p) TT_REF_ADD(tt_adns_domain_t, p, ref)
#define tt_adns_domain_release(p)                                              \
    TT_REF_RELEASE(tt_adns_domain_t, p, ref, __adns_domain_destroy)

tt_export void tt_adns_domain_query(IN tt_adns_domain_t *dm,
                                 IN tt_adns_rr_type_t type);

tt_export void tt_adns_domain_copy_rr(IN tt_adns_domain_t *dm,
                                   IN tt_adns_rr_type_t type,
                                   IN tt_u32_t flag,
                                   OUT tt_dlist_t *rrlist);

tt_export tt_dlist_t *tt_adns_domain_get_rrlist(IN tt_adns_domain_t *dm,
                                             IN tt_adns_rr_type_t type);

tt_export void tt_adns_domain_hnode2key(IN tt_hnode_t *node,
                                     OUT const tt_u8_t **key,
                                     OUT tt_u32_t *key_len);

tt_export tt_result_t tt_adns_domain_subscribe(IN tt_adns_domain_t *dm,
                                            IN tt_adns_rr_type_t type,
                                            IN tt_lnode_t *subscriber);

tt_export void tt_adns_rrset_pkt_handler(IN tt_adns_rrset_t *rrs,
                                      IN struct tt_adns_pkt_s *pkt);

#endif /* __TT_ASYNC_DNS_DOMAIN__ */
