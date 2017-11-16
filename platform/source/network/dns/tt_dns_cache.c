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

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <network/dns/tt_dns_cache.h>

#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <memory/tt_memory_alloc.h>
#include <misc/tt_assert.h>
#include <network/dns/tt_dns_entry.h>
#include <os/tt_task.h>
#include <time/tt_time_reference.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __INUSE_LIMIT (3600 * 1000) // 1 hour

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

tt_dns_rrlist_t __empty_rrlist_a;

tt_dns_rrlist_t __empty_rrlist_aaaa;

static tt_s64_t __s_inuse_limit;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __dc_component_init(IN tt_component_t *comp,
                                       IN tt_profile_t *profile);

void __dc_check_inuse(IN tt_dns_cache_t *dc, IN tt_s64_t now);

static tt_s32_t __de_cmp(IN void *l, IN void *r);

static tt_bool_t __de_destroy(IN tt_u8_t *key,
                              IN tt_u32_t key_len,
                              IN tt_hnode_t *hnode,
                              IN void *param);

tt_dns_entry_t *__de_get(IN tt_dns_cache_t *dc,
                         IN const tt_char_t *name,
                         IN tt_bool_t create);

static tt_bool_t __de_check_inuse(IN tt_u8_t *key,
                                  IN tt_u32_t key_len,
                                  IN tt_hnode_t *hnode,
                                  IN void *param);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_dns_cache_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __dc_component_init,
    };

    // init component
    tt_component_init(&comp, TT_COMPONENT_DNS_CACHE, "DNS Cache", NULL, &itf);

    // register component
    tt_component_register(&comp);
}

tt_dns_cache_t *tt_dns_cache_create(IN OPT tt_dns_cache_attr_t *attr)
{
    tt_dns_cache_attr_t __attr;
    tt_dns_cache_t *dc;

    if (attr == NULL) {
        tt_dns_cache_attr_default(&__attr);
        attr = &__attr;
    }

    dc = tt_malloc(sizeof(tt_dns_cache_t));
    if (dc == NULL) {
        TT_ERROR("no mem for new dns cache");
        return NULL;
    }

    dc->next_check = tt_time_ref() + __s_inuse_limit;

    dc->d = tt_dns_create(&attr->dns_attr);
    if (dc->d == NULL) {
        TT_ERROR("fail to create dns resolver");
        tt_free(dc);
        return NULL;
    }

    if (!TT_OK(tt_hmap_create(&dc->map, attr->slot_num, &attr->map_attr))) {
        TT_ERROR("fail to create dns cache map");
        tt_dns_destroy(dc->d);
        tt_free(dc);
        return NULL;
    }

    tt_ptrheap_init(&dc->heap, __de_cmp, &attr->heap_attr);

    return dc;
}

void tt_dns_cache_destroy(IN tt_dns_cache_t *dc)
{
    TT_ASSERT(dc != NULL);

    tt_hmap_foreach(&dc->map, __de_destroy, &dc->map);
    TT_ASSERT(tt_ptrheap_count(&dc->heap) == 0);

    tt_dns_destroy(dc->d);
}

void tt_dns_cache_attr_default(IN tt_dns_cache_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    tt_dns_attr_default(&attr->dns_attr);

    attr->slot_num = 11;
    tt_hmap_attr_default(&attr->map_attr);

    tt_ptrheap_attr_default(&attr->heap_attr);
}

tt_s64_t tt_dns_cache_run(IN tt_dns_cache_t *dc)
{
    tt_s64_t min_ms, ms, now;
    tt_dns_entry_t *de;

    min_ms = tt_dns_run(dc->d);

    ms = TT_TIME_INFINITE;
    now = tt_time_ref();
    while (((de = tt_ptrheap_head(&dc->heap)) != NULL) &&
           tt_dns_entry_run(de, now, &ms))
        ;
    min_ms = TT_MIN(min_ms, ms);

    if (dc->next_check <= now) {
        dc->next_check = now + __s_inuse_limit;
        __dc_check_inuse(dc, now);
    }
    TT_ASSERT(dc->next_check > now);
    ms = tt_time_ref2ms(dc->next_check - now);
    min_ms = TT_MIN(min_ms, ms);

    return min_ms;
}

tt_dns_rrlist_t *tt_dns_get_a(IN const tt_char_t *name)
{
    tt_dns_entry_t *de;

    TT_ASSERT(name != NULL);

    de = __de_get(tt_current_task()->dns_cache, name, TT_TRUE);
    if (de != NULL) {
        return tt_dns_entry_get_a(de);
    } else {
        return &__empty_rrlist_a;
    }
}

tt_dns_rrlist_t *tt_dns_get_aaaa(IN const tt_char_t *name)
{
    tt_dns_entry_t *de;

    TT_ASSERT(name != NULL);

    de = __de_get(tt_current_task()->dns_cache, name, TT_TRUE);
    if (de != NULL) {
        return tt_dns_entry_get_aaaa(de);
    } else {
        return &__empty_rrlist_aaaa;
    }
}

tt_result_t __dc_component_init(IN tt_component_t *comp,
                                IN tt_profile_t *profile)
{
    tt_dns_rrlist_init(&__empty_rrlist_a, TT_DNS_A_IN);

    tt_dns_rrlist_init(&__empty_rrlist_aaaa, TT_DNS_AAAA_IN);

    __s_inuse_limit = tt_time_ms2ref(__INUSE_LIMIT);

    return TT_SUCCESS;
}

void __dc_check_inuse(IN tt_dns_cache_t *dc, IN tt_s64_t now)
{
    tt_hmap_foreach(&dc->map, __de_check_inuse, (void *)&now);
}

tt_s32_t __de_cmp(IN void *l, IN void *r)
{
    tt_s64_t lv = ((tt_dns_entry_t *)l)->ttl;
    tt_s64_t rv = ((tt_dns_entry_t *)r)->ttl;
    if (lv > rv) {
        // make a min heap
        return -1;
    } else if (lv == rv) {
        return 0;
    } else {
        return 1;
    }
}

tt_bool_t __de_destroy(IN tt_u8_t *key,
                       IN tt_u32_t key_len,
                       IN tt_hnode_t *hnode,
                       IN void *param)
{
    // tt_dns_entry_destroy() will remove hnode from hash map
    tt_dns_entry_destroy(TT_CONTAINER(hnode, tt_dns_entry_t, hnode));
    return TT_TRUE;
}

tt_dns_entry_t *__de_get(IN tt_dns_cache_t *dc,
                         IN const tt_char_t *name,
                         IN tt_bool_t create)
{
    tt_u32_t len;
    tt_hnode_t *node;

    len = (tt_u32_t)tt_strlen(name);
    node = tt_hmap_find(&dc->map, (tt_u8_t *)name, len);
    if (node != NULL) {
        return TT_CONTAINER(node, tt_dns_entry_t, hnode);
    } else if (create) {
        return tt_dns_entry_create(dc, name, len);
    } else {
        return NULL;
    }
}

tt_bool_t __de_check_inuse(IN tt_u8_t *key,
                           IN tt_u32_t key_len,
                           IN tt_hnode_t *hnode,
                           IN void *param)
{
    tt_dns_entry_t *de = TT_CONTAINER(hnode, tt_dns_entry_t, hnode);
    if (!tt_dns_entry_inuse(de, *(tt_s64_t *)param, __s_inuse_limit)) {
        tt_dns_entry_destroy(de);
    }
    return TT_TRUE;
}
