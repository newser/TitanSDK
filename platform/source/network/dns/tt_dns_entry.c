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

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <network/dns/tt_dns_entry.h>

#include <algorithm/ptr/tt_ptr_heap.h>
#include <algorithm/tt_algorithm_def.h>
#include <memory/tt_memory_alloc.h>
#include <network/dns/tt_dns_cache.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __de_add(IN tt_dns_entry_t *de);

static void __de_remove(IN tt_dns_entry_t *de);

static void __de_destroy(IN tt_dns_entry_t *de);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_dns_entry_t *tt_dns_entry_create(IN struct tt_dns_cache_s *dc,
                                    IN const tt_char_t *name,
                                    IN tt_u32_t len)
{
    tt_dns_entry_t *de;
    tt_dns_type_t t;

    de = tt_malloc(sizeof(tt_dns_entry_t) + len + 1);
    if (de == NULL) {
        TT_ERROR("no mem for new dns entry");
        return NULL;
    }

    de->ttl = TT_TIME_INFINITE;
    de->dc = dc;

    de->name = TT_PTR_INC(const tt_char_t, de, sizeof(tt_dns_entry_t));
    tt_memcpy((tt_u8_t *)de->name, name, len + 1);
    de->name_len = len;

    tt_hnode_init(&de->hnode);
    de->ph_idx = TT_POS_NULL;

    for (t = 0; t < TT_DNS_TYPE_NUM; ++t) {
        tt_dns_rr_init(&de->rr[t], de->name, t);
    }

    if (!TT_OK(__de_add(de))) {
        __de_destroy(de);
        return TT_FAIL;
    }

    return de;
}

void tt_dns_entry_destroy(IN tt_dns_entry_t *de)
{
    __de_remove(de);
    __de_destroy(de);
}

void tt_dns_entry_update_ttl(IN tt_dns_entry_t *de, IN tt_s64_t ttl)
{
    if (de->ttl < ttl) {
        de->ttl = ttl;
        tt_ptrheap_fix(&de->dc->heap, de->ph_idx);
    }
}

tt_dns_rrlist_t *tt_dns_entry_get_a(IN tt_dns_entry_t *de)
{
    return tt_dns_rr_get(&de->rr[TT_DNS_A_IN], de->dc->d);
}

tt_dns_rrlist_t *tt_dns_entry_get_aaaa(IN tt_dns_entry_t *de)
{
    return tt_dns_rr_get(&de->rr[TT_DNS_AAAA_IN], de->dc->d);
}

tt_result_t __de_add(IN tt_dns_entry_t *de)
{
    tt_hashmap_t *map = &de->dc->map;
    tt_ptrheap_t *heap = &de->dc->heap;

    if (!TT_OK(
            tt_hmap_add(map, (tt_u8_t *)de->name, de->name_len, &de->hnode))) {
        TT_ERROR("fail to add dns entry to map");
        return TT_FAIL;
    }

    if (!TT_OK(tt_ptrheap_add(heap, de, &de->ph_idx))) {
        TT_ERROR("fail to add dns entry to heap");
        tt_hmap_remove(map, &de->hnode);
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void __de_remove(IN tt_dns_entry_t *de)
{
    tt_hmap_remove(&de->dc->map, &de->hnode);
    tt_ptrheap_remove(&de->dc->heap, de->ph_idx);
}

void __de_destroy(IN tt_dns_entry_t *de)
{
    tt_dns_type_t t;

    for (t = 0; t < TT_DNS_TYPE_NUM; ++t) {
        tt_dns_rr_destroy(&de->rr[t]);
    }

    tt_free(de);
}
