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
#include <network/dns/tt_dns_rr.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct
{
    const tt_char_t *name;
    tt_dns_rr_t rr[TT_DNS_TYPE_NUM];
    tt_hnode_t hnode;
    tt_u32_t name_len;
} tt_dns_domain_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_dns_domain_t *tt_domain_create(IN const tt_char_t *name,
                                         IN tt_u32_t len);

extern void tt_domain_destroy(IN tt_dns_domain_t *ddm);

#endif /* __TT_DNS_CACHE__ */
