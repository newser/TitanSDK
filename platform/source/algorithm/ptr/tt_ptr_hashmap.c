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

#include <algorithm/ptr/tt_ptr_hashmap.h>

#include <memory/tt_memory_alloc.h>
#include <misc/tt_assert.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __KEQ(pn, k, klen)                                                     \
    (((klen) == (pn)->key_len) && (tt_memcmp((k), (pn)->key, (klen)) == 0))

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef struct
{
    tt_u8_t *key;
    tt_ptr_t ptr;
    tt_snode_t snode;
    tt_u32_t key_len;
} __phmnode_t;

////////////////////////////////////////////////////////////
// declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static __phmnode_t *__find_node(IN tt_ptrhmap_t *phm,
                                IN tt_u8_t *key,
                                IN tt_u32_t key_len);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_inline tt_slist_t *__find_sll(IN tt_ptrhmap_t *phm,
                                 IN tt_u8_t *key,
                                 IN tt_u32_t key_len)
{
    return &phm->sll[phm->hash(key, key_len, &phm->hashctx) % phm->sll_num];
}

tt_result_t tt_ptrhmap_create(IN tt_ptrhmap_t *phm,
                              IN tt_u32_t slot_num,
                              IN OPT tt_ptrhmap_attr_t *attr)
{
    tt_ptrhmap_attr_t __attr;
    tt_u32_t i;

    TT_ASSERT(phm != NULL);
    TT_ASSERT(slot_num != 0);

    if (attr == NULL) {
        tt_ptrhmap_attr_default(&__attr);
        attr = &__attr;
    }
    TT_ASSERT(TT_HASH_ALG_VALID(attr->hash_alg));

    phm->sll = tt_malloc(slot_num * sizeof(tt_slist_t));
    if (phm->sll == NULL) {
        TT_ERROR("no mem for ptrmap list");
        return TT_FAIL;
    }
    phm->sll_num = slot_num;
    for (i = 0; i < slot_num; ++i) {
        tt_slist_init(&phm->sll[i]);
    }

    if (attr->hash_alg == TT_HASH_ALG_MURMUR3) {
        phm->hash = tt_hash_murmur3;
    } else {
        phm->hash = tt_hash_fnv1a;
    }
    tt_hashctx_init(&phm->hashctx);

    tt_slist_init(&phm->cache);
    phm->cache_threshold = attr->cache_threshold;
    phm->cache_count = 0;

    phm->count = 0;

    return TT_SUCCESS;
}

void tt_ptrhmap_destroy(IN tt_ptrhmap_t *phm)
{
    tt_u32_t i;
    tt_snode_t *sn;

    TT_ASSERT(phm != NULL);

    for (i = 0; i < phm->sll_num; ++i) {
        while ((sn = tt_slist_pop_head(&phm->sll[i])) != NULL) {
            tt_free(TT_CONTAINER(sn, __phmnode_t, snode));
        }
    }

    while ((sn = tt_slist_pop_head(&phm->cache)) != NULL) {
        tt_free(TT_CONTAINER(sn, __phmnode_t, snode));
    }

    tt_free(phm->sll);
}

void tt_ptrhmap_attr_default(IN tt_ptrhmap_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->hash_alg = TT_HASH_ALG_MURMUR3;

    attr->cache_threshold = 16;
}

void tt_ptrhmap_clear(IN tt_ptrhmap_t *phm)
{
    tt_u32_t i;
    for (i = 0; i < phm->sll_num; ++i) {
        tt_snode_t *sn;
        while ((sn = tt_slist_pop_head(&phm->sll[i])) != NULL) {
            --phm->count;
            if (phm->cache_count < phm->cache_threshold) {
                tt_slist_push_head(&phm->cache, sn);
                ++phm->cache_count;
            } else {
                tt_free(TT_CONTAINER(sn, __phmnode_t, snode));
            }
        }
    }
}

tt_ptr_t tt_ptrhmap_find(IN tt_ptrhmap_t *phm,
                         IN tt_u8_t *key,
                         IN tt_u32_t key_len)
{
    __phmnode_t *pn = __find_node(phm, key, key_len);
    return TT_COND(pn != NULL, pn->ptr, NULL);
}

tt_bool_t tt_ptrhmap_contain_ptr(IN tt_ptrhmap_t *phm, IN tt_ptr_t ptr)
{
    tt_u32_t i;
    for (i = 0; i < phm->sll_num; ++i) {
        tt_snode_t *sn = tt_slist_head(&phm->sll[i]);
        while (sn != NULL) {
            if (TT_CONTAINER(sn, __phmnode_t, snode)->ptr == ptr) {
                return TT_TRUE;
            }
            sn = sn->next;
        }
    }
    return TT_FALSE;
}

tt_result_t tt_ptrhmap_add(IN tt_ptrhmap_t *phm,
                           IN tt_u8_t *key,
                           IN tt_u32_t key_len,
                           IN tt_ptr_t ptr)
{
    tt_slist_t *sll;
    tt_snode_t *sn;
    __phmnode_t *pn;

    TT_ASSERT(ptr != NULL);

    sll = __find_sll(phm, key, key_len);
    sn = tt_slist_head(sll);
    while (sn != NULL) {
        __phmnode_t *pn = TT_CONTAINER(sn, __phmnode_t, snode);
        if (__KEQ(pn, key, key_len)) {
            break;
        }

        sn = sn->next;
    }
    if (sn != NULL) {
        TT_ERROR("duplicated key");
        return TT_FAIL;
    }

    sn = tt_slist_pop_head(&phm->cache);
    if (sn != NULL) {
        pn = TT_CONTAINER(sn, __phmnode_t, snode);
    } else {
        pn = tt_malloc(sizeof(__phmnode_t));
        if (pn == NULL) {
            TT_ERROR("no mem for new node");
            return TT_FAIL;
        }
    }

    pn->key = key;
    pn->key_len = key_len;
    pn->ptr = ptr;
    tt_snode_init(&pn->snode);

    tt_slist_push_head(sll, &pn->snode);
    ++phm->count;
    return TT_SUCCESS;
}

tt_bool_t tt_ptrhmap_remove_key(IN tt_ptrhmap_t *phm,
                                IN tt_u8_t *key,
                                IN tt_u32_t key_len)
{
    tt_slist_t *sll;
    tt_snode_t *prev, *sn;

    sll = __find_sll(phm, key, key_len);
    prev = NULL;
    sn = tt_slist_head(sll);
    while (sn != NULL) {
        __phmnode_t *pn = TT_CONTAINER(sn, __phmnode_t, snode);
        if (__KEQ(pn, key, key_len)) {
            break;
        }

        prev = sn;
        sn = sn->next;
    }

    if (sn != NULL) {
        tt_slist_fast_remove(sll, prev, sn);
        --phm->count;

        if (phm->cache_count < phm->cache_threshold) {
            tt_slist_push_head(&phm->cache, sn);
            ++phm->cache_count;
        } else {
            tt_free(TT_CONTAINER(sn, __phmnode_t, snode));
        }

        return TT_TRUE;
    } else {
        return TT_FALSE;
    }
}

tt_bool_t tt_ptrhmap_remove_ptr(IN tt_ptrhmap_t *phm, IN tt_ptr_t ptr)
{
    tt_u32_t i;
    for (i = 0; i < phm->sll_num; ++i) {
        tt_snode_t *prev = NULL;
        tt_snode_t *sn = tt_slist_head(&phm->sll[i]);
        while (sn != NULL) {
            if (TT_CONTAINER(sn, __phmnode_t, snode)->ptr == ptr) {
                tt_slist_fast_remove(&phm->sll[i], prev, sn);
                --phm->count;

                if (phm->cache_count < phm->cache_threshold) {
                    tt_slist_push_head(&phm->cache, sn);
                    ++phm->cache_count;
                } else {
                    tt_free(TT_CONTAINER(sn, __phmnode_t, snode));
                }

                return TT_TRUE;
            }

            prev = sn;
            sn = sn->next;
        }
    }
    return TT_FALSE;
}

tt_bool_t tt_ptrhmap_remove_pair(IN tt_ptrhmap_t *phm,
                                 IN tt_u8_t *key,
                                 IN tt_u32_t key_len,
                                 IN tt_ptr_t ptr)
{
    tt_u32_t i;
    for (i = 0; i < phm->sll_num; ++i) {
        tt_snode_t *prev = NULL;
        tt_snode_t *sn = tt_slist_head(&phm->sll[i]);
        while (sn != NULL) {
            __phmnode_t *pn = TT_CONTAINER(sn, __phmnode_t, snode);
            if ((pn->ptr == ptr) && __KEQ(pn, key, key_len)) {
                tt_slist_fast_remove(&phm->sll[i], prev, sn);
                --phm->count;

                if (phm->cache_count < phm->cache_threshold) {
                    tt_slist_push_head(&phm->cache, sn);
                    ++phm->cache_count;
                } else {
                    tt_free(TT_CONTAINER(sn, __phmnode_t, snode));
                }

                return TT_TRUE;
            }

            prev = sn;
            sn = sn->next;
        }
    }
    return TT_FALSE;
}

void tt_ptrhmap_foreach(IN tt_ptrhmap_t *phm,
                        IN tt_ptrhmap_action_t action,
                        IN void *param)
{
    tt_u32_t i;
    for (i = 0; i < phm->sll_num; ++i) {
        tt_snode_t *sn = tt_slist_head(&phm->sll[i]);
        while (sn != NULL) {
            __phmnode_t *pn = TT_CONTAINER(sn, __phmnode_t, snode);
            if (!action(pn->key, pn->key_len, pn->ptr, param)) {
                return;
            }

            sn = sn->next;
        }
    }
}

tt_bool_t tt_ptrhmap_replace(IN tt_ptrhmap_t *phm,
                             IN tt_u8_t *key,
                             IN tt_u32_t key_len,
                             IN tt_ptr_t new_ptr)
{
    __phmnode_t *pn = __find_node(phm, key, key_len);
    TT_ASSERT(new_ptr != NULL);
    if (pn != NULL) {
        pn->ptr = new_ptr;
        return TT_TRUE;
    } else {
        return TT_FALSE;
    }
}

tt_bool_t tt_ptrhmap_replace_equal(IN tt_ptrhmap_t *phm,
                                   IN tt_u8_t *key,
                                   IN tt_u32_t key_len,
                                   IN tt_ptr_t old_ptr,
                                   IN tt_ptr_t new_ptr)
{
    __phmnode_t *pn = __find_node(phm, key, key_len);
    TT_ASSERT(new_ptr != NULL);
    if ((pn != NULL) && (pn->ptr == old_ptr)) {
        pn->ptr = new_ptr;
        return TT_TRUE;
    } else {
        return TT_FALSE;
    }
}

__phmnode_t *__find_node(IN tt_ptrhmap_t *phm,
                         IN tt_u8_t *key,
                         IN tt_u32_t key_len)
{
    tt_slist_t *sll = __find_sll(phm, key, key_len);
    tt_snode_t *node = tt_slist_head(sll);
    while (node != NULL) {
        __phmnode_t *pn = TT_CONTAINER(node, __phmnode_t, snode);
        if (__KEQ(pn, key, key_len)) {
            return pn;
        }

        node = node->next;
    }
    return NULL;
}
