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
@file tt_dns_entry.h
@brief dns entry API
 */

#ifndef __TT_DNS_ENTRY__
#define __TT_DNS_ENTRY__

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

struct tt_dns_cache_s;

typedef struct
{
    tt_s64_t ttl;
    tt_s64_t timestamp;
    struct tt_dns_cache_s *dc;
    const tt_char_t *name;
    tt_hnode_t hnode;
    tt_dns_rr_t rr[TT_DNS_TYPE_NUM];
    tt_u32_t name_len;
    tt_u32_t ph_idx;
} tt_dns_entry_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

// len does not count terminating null
tt_export tt_dns_entry_t *tt_dns_entry_create(IN struct tt_dns_cache_s *dc,
                                              IN const tt_char_t *name,
                                              IN tt_u32_t len);

tt_export void tt_dns_entry_destroy(IN tt_dns_entry_t *de);

tt_export void tt_dns_entry_clear(IN tt_dns_entry_t *de);

tt_export void tt_dns_entry_update_ttl(IN tt_dns_entry_t *de, IN tt_s64_t ttl);

tt_export tt_bool_t tt_dns_entry_run(IN tt_dns_entry_t *de,
                                     IN tt_s64_t now,
                                     OUT tt_s64_t *ttl_ms);

tt_export tt_bool_t tt_dns_entry_inuse(IN tt_dns_entry_t *de,
                                       IN tt_s64_t now,
                                       IN tt_s64_t limit);

tt_export tt_dns_rrlist_t *tt_dns_entry_get_a(IN tt_dns_entry_t *de);

tt_export tt_dns_rrlist_t *tt_dns_entry_get_aaaa(IN tt_dns_entry_t *de);

#endif /* __TT_DNS_ENTRY__ */
