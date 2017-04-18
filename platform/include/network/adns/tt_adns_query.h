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
@file tt_adns_query.h
@brief async dns query APIs

this file defines async dns query APIs
*/

#ifndef __TT_ASYNC_QUERY__
#define __TT_ASYNC_QUERY__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <network/adns/tt_adns_rr.h>

// header dependency:
//  adns_rr adns_name_server
//  adns_query
//  adns_domain
//  adcache

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct tt_adns_qryctx_s
{
    void *cb_param;

    tt_result_t result;
    tt_dlist_t *rrlist;
} tt_adns_qryctx_t;

typedef void (*tt_adns_on_query_t)(IN tt_char_t *domain,
                                   IN tt_adns_rr_type_t type,
                                   IN tt_u32_t flag,
                                   IN tt_adns_qryctx_t *qryctx);

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

// qryctx must not be used before, including no rr
extern tt_result_t tt_adns_query(IN const tt_char_t *domain,
                                 IN tt_adns_rr_type_t type,
                                 IN tt_u32_t flag,
                                 OUT tt_adns_qryctx_t *qryctx);
// flag
#define TT_ADNS_QUERY_ALL (1 << 0)
#define TT_ADNS_QUERY_NEW (1 << 1)

extern tt_result_t tt_adns_query_async(IN const tt_char_t *domain,
                                       IN tt_adns_rr_type_t type,
                                       IN tt_u32_t flag,
                                       IN tt_adns_on_query_t on_query,
                                       IN OPT void *cb_param);
// flags are same as tt_adns_query

#endif /* __TT_ASYNC_QUERY__ */
