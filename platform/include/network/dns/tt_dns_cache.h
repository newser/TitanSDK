/* Copyright (C) 2017 haniu (niuhao.cn@gmail.com)
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
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

#include <algorithm/ptr/tt_ptr_heap.h>
#include <algorithm/tt_hashmap.h>
#include <network/dns/tt_dns.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_dns_rrlist_s;

typedef struct tt_dns_cache_s
{
    tt_s64_t next_check;
    tt_dns_t d;
    tt_hashmap_t map;
    tt_ptrheap_t heap;
} tt_dns_cache_t;

typedef struct
{
    tt_dns_attr_t dns_attr;
    tt_u32_t slot_num;
    tt_hmap_attr_t map_attr;
    tt_ptrheap_attr_t heap_attr;
} tt_dns_cache_attr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export void tt_dns_cache_component_register();

tt_export tt_dns_cache_t *tt_dns_cache_create(IN OPT tt_dns_cache_attr_t *attr);

tt_export void tt_dns_cache_destroy(IN tt_dns_cache_t *dc);

tt_export void tt_dns_cache_attr_default(IN tt_dns_cache_attr_t *attr);

tt_export tt_s64_t tt_dns_cache_run(IN tt_dns_cache_t *dc);

tt_export struct tt_dns_rrlist_s *tt_dns_get_a(IN const tt_char_t *name);

tt_export struct tt_dns_rrlist_s *tt_dns_get_aaaa(IN const tt_char_t *name);

#endif /* __TT_DNS_CACHE__ */
