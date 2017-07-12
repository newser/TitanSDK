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
@file tt_adns_resolver.h
@brief async dns resolver

this file defines async dns resolver
*/

#ifndef __TT_ADNS_RESOLVER__
#define __TT_ADNS_RESOLVER__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_buffer.h>
#include <io/tt_socket.h>
#include <network/tt_network_def.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_adns_dmgr_s;
struct tt_adns_pkt_s;
struct tt_tmr_s;

typedef struct
{
    tt_u32_t reconn_delay;
    tt_u32_t init_buf_size;
    tt_buf_attr_t buf_attr;
    tt_sktaddr_t local_addr;

    tt_bool_t has_local_addr : 1;
} tt_adns_resolver_attr_t;

typedef enum {
    TT_ADRSLVR_DISCONNECTED,
    TT_ADRSLVR_CONNECTING,
    TT_ADRSLVR_CONNECTED,

    TT_ADRSLVR_STATE_NUM
} tt_adns_resolver_state_t;
#define TT_ADNS_RESOLVER_STATE_VALID(s) ((s) < TT_ADRSLVR_STATE_NUM)

typedef struct tt_adns_resolver_s
{
    tt_adns_resolver_attr_t attr;
    tt_netaddr_t ns_addr;
    struct tt_adns_dmgr_s *dmgr;
    tt_buf_t data_buf;

    struct tt_tmr_s *reconn_tmr;
    tt_skt_t skt;

    tt_adns_resolver_state_t state;
} tt_adns_resolver_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_adns_resolver_create(IN tt_adns_resolver_t *res,
                                           IN struct tt_adns_dmgr_s *dmgr,
                                           IN tt_netaddr_t *addr,
                                           IN OPT
                                               tt_adns_resolver_attr_t *attr);

// this function would destroy socket brutely
extern void tt_adns_resolver_destroy(IN tt_adns_resolver_t *res);

extern void tt_adns_resolver_attr_default(IN tt_adns_resolver_attr_t *attr);

extern void tt_adns_resolver_send(IN tt_adns_resolver_t *res,
                                  IN struct tt_adns_pkt_s *pkt);

#endif /* __TT_ADNS_RESOLVER__ */
