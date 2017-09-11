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
@file tt_dns.h
@brief dns API
 */

#ifndef __TT_DNS__
#define __TT_DNS__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_dns_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct ares_channeldata *tt_dns_t;

typedef struct
{
    tt_bool_t enable_edns;
    tt_bool_t prefer_tcp;
    tt_u32_t timeout_ms;
    tt_u32_t try_num;
    tt_u32_t send_buf_size;
    tt_u32_t recv_buf_size;
    const tt_char_t **server;
    tt_u32_t server_num;
    const tt_char_t *local_ip4;
    const tt_char_t *local_ip6;
    const tt_char_t *local_device;
} tt_dns_attr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export void tt_dns_component_register();

tt_export tt_dns_t tt_dns_create(IN OPT tt_dns_attr_t *attr);

tt_export void tt_dns_destroy(IN tt_dns_t d);

tt_export void tt_dns_attr_default(IN tt_dns_attr_t *attr);

tt_inline tt_s64_t tt_dns_run(IN tt_dns_t d)
{
    return tt_dns_run_ntv(d);
}

#endif /* __TT_DNS__ */
