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
@file tt_dns_rr.h
@brief dns rr API
 */

#ifndef __TT_DNS_RR__
#define __TT_DNS_RR__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_double_linked_list.h>
#include <io/tt_socket_addr.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef enum {
    TT_DNS_A_IN,
    TT_DNS_AAAA_IN,

    TT_DNS_TYPE_NUM,
} tt_dns_type_t;
#define TT_DNS_TYPE_VALID(t) ((t) < TT_DNS_TYPE_NUM)

typedef struct
{
    tt_dlist_t rr;
    tt_dns_type_t type;
} tt_dns_rrlist_t;

typedef struct
{
    tt_s64_t ttl;
    tt_dlist_t waiting;
    tt_dns_rrlist_t rrlist;
} tt_dns_rr_t;

typedef struct tt_dns_a_s
{
    tt_dnode_t node;
    tt_sktaddr_t addr;
} tt_dns_a_t;

typedef struct tt_dns_aaaa_s
{
    tt_dnode_t node;
    tt_sktaddr_t addr;
} tt_dns_aaaa_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern void tt_dns_rr_init(IN tt_dns_rr_t *drr, IN tt_dns_type_t type);

extern void tt_dns_rr_clear(IN tt_dns_rr_t *drr);

extern tt_dns_rrlist_t *tt_dns_rr_wait(IN tt_dns_rr_t *drr);

extern void tt_dns_rr_noitfy(IN tt_dns_rr_t *drr);

tt_inline void tt_dns_rr_check(IN tt_dns_rr_t *drr, IN tt_s64_t now)
{
    if ((drr->ttl != 0) && (now > drr->ttl)) {
        tt_dns_rr_clear(drr);
    }
}

// ========================================
// rr list
// ========================================

tt_inline void tt_dns_rrlist_init(IN tt_dns_rrlist_t *rrl,
                                  IN tt_dns_type_t type)
{
    tt_dlist_init(&rrl->rr);
    rrl->type = type;
}

extern void tt_dns_rrlist_clear(IN tt_dns_rrlist_t *rrl);

// RR: A
extern tt_dns_a_t *tt_dns_a_head(IN tt_dns_rrlist_t *rrl);

extern tt_dns_a_t *tt_dns_a_next(IN tt_dns_a_t *a);

extern tt_result_t tt_dns_a_copy(IN tt_dns_rrlist_t *dst,
                                 IN tt_dns_rrlist_t *src);

extern void tt_dns_a_clear(IN tt_dns_rrlist_t *rrl);

// RR: AAAA
extern tt_dns_aaaa_t *tt_dns_aaaa_head(IN tt_dns_rrlist_t *rrl);

extern tt_dns_aaaa_t *tt_dns_aaaa_next(IN tt_dns_aaaa_t *aaaa);

extern tt_result_t tt_dns_aaaa_copy(IN tt_dns_rrlist_t *dst,
                                    IN tt_dns_rrlist_t *src);

extern void tt_dns_aaaa_clear(IN tt_dns_rrlist_t *rrl);

#endif /* __TT_DNS_RR__ */
