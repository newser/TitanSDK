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
@file tt_dns_cache.h
@brief dns cache API
 */

#ifndef __TT_DNS_CACHE__
#define __TT_DNS_CACHE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_hashmap.h>
#include <io/tt_socket_addr.h>
#include <network/dns/tt_dns.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct
{
    tt_dns_t d;
    tt_hashmap_t map;
} tt_dns_cache_t;

typedef struct
{
    tt_dns_attr_t dns_attr;
    tt_u32_t slot_num;
    tt_hmap_attr_t map_attr;
} tt_dns_cache_attr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_dns_cache_create(IN tt_dns_cache_t *dc,
                                       IN OPT tt_dns_cache_attr_t *attr);

extern void tt_dns_cache_destroy(IN tt_dns_cache_t *dc);

extern void tt_dns_cache_attr_default(IN tt_dns_cache_attr_t *attr);

#if 0
extern tt_dns_a_t *tt_dns_get_a(IN tt_dns_cache_t *dc,
                                IN const tt_char_t *name);

extern tt_dns_aaaa_t *tt_dns_get_aaaa(IN tt_dns_cache_t *dc,
                                      IN const tt_char_t *name);
#endif

#endif /* __TT_DNS_CACHE__ */
