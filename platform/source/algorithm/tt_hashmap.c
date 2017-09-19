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

#include <algorithm/tt_hashmap.h>

#include <algorithm/tt_hash.h>
#include <memory/tt_memory_alloc.h>
#include <misc/tt_assert.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __KEQ(hn, k, klen)                                                     \
    (((klen) == (hn)->key_len) && (tt_memcmp((k), (hn)->key, (klen)) == 0))

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

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_inline tt_slist_t *__find_sll(IN tt_hashmap_t *hmap,
                                 IN tt_u8_t *key,
                                 IN tt_u32_t key_len)
{
    return &hmap->sll[hmap->hash(key, key_len, &hmap->hashctx) % hmap->sll_num];
}

tt_result_t tt_hmap_create(IN tt_hashmap_t *hmap,
                           IN tt_u32_t slot_num,
                           IN OPT tt_hmap_attr_t *attr)
{
    tt_hmap_attr_t __attr;
    tt_u32_t i;

    TT_ASSERT(hmap != NULL);
    TT_ASSERT(slot_num != 0);

    if (attr == NULL) {
        tt_hmap_attr_default(&__attr);
        attr = &__attr;
    }
    TT_ASSERT(TT_HASH_ALG_VALID(attr->hash_alg));

    hmap->sll = tt_malloc(slot_num * sizeof(tt_slist_t));
    if (hmap->sll == NULL) {
        TT_ERROR("no mem for ptrmap list");
        return TT_FAIL;
    }
    hmap->sll_num = slot_num;
    for (i = 0; i < slot_num; ++i) {
        tt_slist_init(&hmap->sll[i]);
    }

    if (attr->hash_alg == TT_HASH_ALG_MURMUR3) {
        hmap->hash = tt_hash_murmur3;
    } else {
        hmap->hash = tt_hash_fnv1a;
    }
    tt_hashctx_init(&hmap->hashctx);

    hmap->count = 0;

    return TT_SUCCESS;
}

void tt_hmap_destroy(IN tt_hashmap_t *hmap)
{
    TT_ASSERT(hmap != NULL);

    tt_hmap_clear(hmap);

    tt_free(hmap->sll);
}

void tt_hmap_attr_default(IN tt_hmap_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->hash_alg = TT_HASH_ALG_MURMUR3;
}

void tt_hmap_clear(IN tt_hashmap_t *hmap)
{
    tt_u32_t i;
    for (i = 0; i < hmap->sll_num; ++i) {
        while (tt_slist_pop_head(&hmap->sll[i]) != NULL)
            ;
    }
    hmap->count = 0;
}

tt_hnode_t *tt_hmap_find(IN tt_hashmap_t *hmap,
                         IN tt_u8_t *key,
                         IN tt_u32_t key_len)
{
    tt_slist_t *sll = __find_sll(hmap, key, key_len);
    tt_snode_t *sn = tt_slist_head(sll);
    while (sn != NULL) {
        tt_hnode_t *hn = TT_CONTAINER(sn, tt_hnode_t, snode);
        if (__KEQ(hn, key, key_len)) {
            return hn;
        }

        sn = sn->next;
    }
    return NULL;
}

tt_bool_t tt_hmap_contain(IN tt_hashmap_t *hmap, IN tt_hnode_t *hnode)
{
    tt_u32_t i;
    for (i = 0; i < hmap->sll_num; ++i) {
        tt_snode_t *sn = tt_slist_head(&hmap->sll[i]);
        while (sn != NULL) {
            if (hnode == TT_CONTAINER(sn, tt_hnode_t, snode)) {
                return TT_TRUE;
            }

            sn = sn->next;
        }
    }
    return TT_FALSE;
}

tt_result_t tt_hmap_add(IN tt_hashmap_t *hmap,
                        IN tt_u8_t *key,
                        IN tt_u32_t key_len,
                        IN tt_hnode_t *hnode)
{
    tt_slist_t *sll;
    tt_snode_t *sn;

    sll = __find_sll(hmap, key, key_len);
    sn = tt_slist_head(sll);
    while (sn != NULL) {
        tt_hnode_t *hn = TT_CONTAINER(sn, tt_hnode_t, snode);
        if (__KEQ(hn, key, key_len)) {
            break;
        }

        sn = sn->next;
    }
    if (sn != NULL) {
        TT_ERROR("duplicated key");
        return TT_FAIL;
    }

    hnode->key = key;
    hnode->key_len = key_len;
    tt_slist_push_head(sll, &hnode->snode);
    ++hmap->count;
    return TT_SUCCESS;
}

void tt_hmap_remove(IN tt_hashmap_t *hmap, IN tt_hnode_t *hnode)
{
    tt_slist_remove(__find_sll(hmap, hnode->key, hnode->key_len),
                    &hnode->snode);
    --hmap->count;
}

tt_bool_t tt_hmap_remove_key(IN tt_hashmap_t *hmap,
                             IN tt_u8_t *key,
                             IN tt_u32_t key_len)
{
    tt_slist_t *sll = __find_sll(hmap, key, key_len);
    tt_snode_t *prev = NULL, *sn = tt_slist_head(sll);

    while (sn != NULL) {
        tt_hnode_t *hn = TT_CONTAINER(sn, tt_hnode_t, snode);
        if (__KEQ(hn, key, key_len)) {
            tt_slist_fast_remove(sll, prev, sn);
            --hmap->count;
            return TT_TRUE;
        }

        prev = sn;
        sn = sn->next;
    }
    return TT_FALSE;
}

void tt_hmap_foreach(IN tt_hashmap_t *hmap,
                     IN tt_hmap_action_t action,
                     IN void *param)
{
    tt_u32_t i;
    for (i = 0; i < hmap->sll_num; ++i) {
        tt_snode_t *sn = tt_slist_head(&hmap->sll[i]);
        while (sn != NULL) {
            tt_hnode_t *hn = TT_CONTAINER(sn, tt_hnode_t, snode);

            sn = sn->next;

            if (!action(hn->key, hn->key_len, hn, param)) {
                return;
            }
        }
    }
}
