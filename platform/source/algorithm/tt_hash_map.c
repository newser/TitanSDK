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

#include <algorithm/tt_hash_map.h>

#include <memory/tt_memory_alloc.h>
#include <misc/tt_assert.h>
#include <misc/tt_util.h>
#include <os/tt_thread.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define TT_ASSERT_HMAP TT_ASSERT

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

// ========================================
// hash hslot: list array
// ========================================

static tt_result_t __la_create(IN tt_hslot_t *hslot);
static tt_result_t __la_destroy(IN tt_hslot_t *hslot);
static tt_result_t __la_add(IN tt_hslot_t *hslot, IN tt_hnode_t *hnode);
static tt_result_t __la_adduniq(IN tt_hslot_t *hslot,
                                IN tt_hnode_t *node,
                                OUT tt_hnode_t **exist);
static tt_result_t __la_remove(IN tt_hslot_t *hslot, IN tt_hnode_t *hnode);
static tt_hnode_t *__la_find(IN tt_hslot_t *hslot,
                             IN const tt_u8_t *key,
                             IN tt_u32_t key_len,
                             IN tt_hnode_t *cur_lst_node);
static void __la_foreach(IN tt_hslot_t *hslot,
                         IN tt_hnode_action_t action,
                         IN void *param);
static tt_hashmap_itf_t __s_la_itf = {__la_create,
                                      __la_destroy,
                                      __la_add,
                                      __la_adduniq,
                                      __la_remove,
                                      __la_find,
                                      __la_foreach};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static void __load_hslot_op(IN tt_hashmap_t *hmap,
                            IN tt_hslot_type_t hslot_type);

// hash functions
static tt_hashval_t __hash_murmur3(IN const tt_u8_t *key,
                                   IN tt_u32_t key_len,
                                   IN tt_hashctx_t *hctx);
static tt_hashval_t __hash_fnv1a(IN const tt_u8_t *key,
                                 IN tt_u32_t key_len,
                                 IN tt_hashctx_t *hctx);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_hashmap_create(IN tt_hashmap_t *hmap,
                              IN tt_u32_t slot_num,
                              IN tt_hnode2key_t hnode2key,
                              IN OPT tt_hashmap_attr_t *attr)
{
    TT_ASSERT(hmap != NULL);
    TT_ASSERT(slot_num != 0);
    TT_ASSERT(hnode2key != NULL);

    tt_memset(hmap, 0, sizeof(tt_hashmap_t));

    // just record information

    hmap->slot_num = slot_num;
    hmap->hnode2key = hnode2key;

    if (attr == NULL) {
        tt_hashmap_attr_default(&hmap->attr);
    } else {
        tt_memcpy(&hmap->attr, attr, sizeof(tt_hashmap_attr_t));
    }

    switch (hmap->attr.hashalg) {
        case TT_HASHALG_MURMUR3: {
            hmap->hash = __hash_murmur3;
            hmap->hashctx.seed = tt_rand_u32();
            // although tt_rand_u32() is not applicable out of
            // ts thread
        } break;
        case TT_HASHALG_FNV1A: {
            hmap->hash = __hash_fnv1a;
            hmap->hashctx.seed = tt_rand_u32();
            // although tt_rand_u32() is not applicable out of
            // ts thread
        } break;
        default: {
            TT_ERROR("unknown hash func type: %d", hmap->attr.hashalg);
            return TT_BAD_PARAM;
        } break;
    }
    TT_ASSERT(hmap->hash != NULL);

    __load_hslot_op(hmap, hmap->attr.hslot_type);

    return hmap->itf->create(&hmap->hslot);
}

void tt_hashmap_attr_default(IN tt_hashmap_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->hashalg = TT_HASHALG_MURMUR3;

    attr->hslot_type = TT_HSLOT_TYPE_LIST_ARRAY;
}

void __load_hslot_op(IN tt_hashmap_t *hmap, IN tt_hslot_type_t hslot_type)
{
    TT_ASSERT(TT_HSLOT_TYPE_VALID(hmap->attr.hslot_type));
    switch (hslot_type) {
        case TT_HSLOT_TYPE_LIST_ARRAY:
        default: {
            hmap->itf = &__s_la_itf;
        } break;
    }
    TT_ASSERT(hmap->itf->create != NULL);
    TT_ASSERT(hmap->itf->destroy != NULL);
    TT_ASSERT(hmap->itf->add != NULL);
    TT_ASSERT(hmap->itf->adduniq != NULL);
    TT_ASSERT(hmap->itf->remove != NULL);
    TT_ASSERT(hmap->itf->find != NULL);
    TT_ASSERT(hmap->itf->foreach != NULL);
}

// ========================================
// hash functions
// ========================================

tt_hashval_t __hash_murmur3(IN const tt_u8_t *key,
                            IN tt_u32_t key_len,
                            IN tt_hashctx_t *hctx)
{
    static const tt_u32_t c1 = 0xcc9e2d51;
    static const tt_u32_t c2 = 0x1b873593;
    static const tt_u32_t r1 = 15;
    static const tt_u32_t r2 = 13;
    static const tt_u32_t m = 5;
    static const tt_u32_t n = 0xe6546b64;
    // accessing these static const vars should have been optimized by
    // compiler as constants

    const int nblocks = key_len >> 2;
    const tt_u32_t *blocks = (const tt_u32_t *)key;
    int i;

    const tt_u8_t *tail = (const tt_u8_t *)(key + (nblocks << 2));
    tt_u32_t k1 = 0;

    tt_u32_t hash = hctx->seed;
    // tt_u32_t hash = (tt_u32_t)((tt_uintptr_t)key);

    for (i = 0; i < nblocks; i++) {
        tt_u32_t k = blocks[i];
        k *= c1;
        k = (k << r1) | (k >> (32 - r1));
        k *= c2;

        hash ^= k;
        hash = ((hash << r2) | (hash >> (32 - r2))) * m + n;
    }

    switch (key_len & 3) {
        case 3:
            k1 ^= tail[2] << 16;
        case 2:
            k1 ^= tail[1] << 8;
        case 1:
            k1 ^= tail[0];

            k1 *= c1;
            k1 = (k1 << r1) | (k1 >> (32 - r1));
            k1 *= c2;
            hash ^= k1;
    }

    hash ^= key_len;
    hash ^= (hash >> 16);
    hash *= 0x85ebca6b;
    hash ^= (hash >> 13);
    hash *= 0xc2b2ae35;
    hash ^= (hash >> 16);

    return hash;
}

tt_hashval_t __hash_fnv1a(IN const tt_u8_t *key,
                          IN tt_u32_t key_len,
                          IN tt_hashctx_t *hctx)
{
    tt_u8_t *bp = (tt_u8_t *)key;
    tt_u8_t *be = TT_PTR_INC(tt_u8_t, key, key_len);
    tt_hashval_t hval = hctx->seed;

    while (bp < be) {
        hval ^= (tt_u32_t)(*bp++);
        hval += (hval << 1) + (hval << 4) + (hval << 7) + (hval << 8) +
                (hval << 24);
    }

    // fnv1a requires passing hval to next call, but woule this generate
    // different hash value from two same inputs?
    return hval;
}

// ========================================
// hash hslot: list by index
// ========================================

tt_result_t __la_create(IN tt_hslot_t *hslot)
{
    tt_hashmap_t *hmap = TT_CONTAINER(hslot, tt_hashmap_t, hslot);

    tt_list_t *list = NULL;
    tt_u32_t list_num = hmap->slot_num;
    tt_u32_t size = sizeof(tt_list_t) * list_num;
    tt_u32_t i = 0;

    // list num should be adjusted
    list_num = size / sizeof(tt_list_t);

    // hslot
    list = (tt_list_t *)tt_malloc(size);
    if (list == NULL) {
        TT_ERROR("no memory");
        return TT_NO_RESOURCE;
    }
    // it's better not to change list_num, which may affect
    // hash performance, some hash func may especially use
    // a prime number as the hslot number

    // init hslot
    for (i = 0; i < list_num; ++i) {
        tt_list_init(&list[i]);
    }

    hslot->list_array.list = list;
    hslot->list_array.list_num = list_num;

    hmap->slot_num = list_num;

    return TT_SUCCESS;
}

tt_result_t __la_destroy(IN tt_hslot_t *hslot)
{
    tt_hashmap_t *hmap = TT_CONTAINER(hslot, tt_hashmap_t, hslot);

    tt_list_t *list = hslot->list_array.list;
    tt_u32_t list_num = hslot->list_array.list_num;
    tt_u32_t i;

    for (i = 0; i < list_num; ++i) {
        if (!tt_list_empty(&list[i])) {
            TT_ERROR("slot %d is not empty", i);
            return TT_FAIL;
        }
    }

    tt_free(list);

    return TT_SUCCESS;
}

tt_result_t __la_add(IN tt_hslot_t *hslot, IN tt_hnode_t *node)
{
    tt_hashmap_t *hmap = TT_CONTAINER(hslot, tt_hashmap_t, hslot);
    const tt_u8_t *key = NULL;
    tt_u32_t key_len = 0;
    tt_hashval_t hval = 0;
    tt_list_t *list = NULL;

    // get key
    hmap->hnode2key(node, &key, &key_len);
    TT_ASSERT_HMAP(key != NULL);
    TT_ASSERT_HMAP(key_len != 0);

    // calc hash value
    hval = hmap->hash(key, key_len, &hmap->hashctx);

    // find slot by index
    list = &hslot->list_array.list[hval % hslot->list_array.list_num];

    // add to slot
    tt_lnode_init(&node->list_node);
    tt_list_addhead(list, &node->list_node);

    return TT_SUCCESS;
}

tt_result_t __la_adduniq(IN tt_hslot_t *hslot,
                         IN tt_hnode_t *node,
                         OUT tt_hnode_t **exist)
{
    tt_hashmap_t *hmap = TT_CONTAINER(hslot, tt_hashmap_t, hslot);
    const tt_u8_t *key = NULL;
    tt_u32_t key_len = 0;
    tt_hashval_t hval = 0;
    tt_list_t *list = NULL;

    tt_hnode2key_t hnode2key = hmap->hnode2key;
    tt_lnode_t *cur_lst_node = NULL;

    // get key
    hnode2key(node, &key, &key_len);
    TT_ASSERT_HMAP(key != NULL);
    TT_ASSERT_HMAP(key_len != 0);

    // calc hash value
    hval = hmap->hash(key, key_len, &hmap->hashctx);

    // find slot by index
    list = &hslot->list_array.list[hval % hslot->list_array.list_num];

    // check uniqueness
    cur_lst_node = tt_list_head(list);
    while (cur_lst_node != NULL) {
        const tt_u8_t *cur_key = NULL;
        tt_u32_t cur_key_len = 0;

        // get node key
        hnode2key(TT_CONTAINER(cur_lst_node, tt_hnode_t, list_node),
                  &cur_key,
                  &cur_key_len);
        TT_ASSERT_HMAP(cur_key != NULL);
        TT_ASSERT_HMAP(cur_key_len != 0);

        // compare key
        if ((key_len == cur_key_len) &&
            (tt_memcmp(cur_key, key, key_len) == 0)) {
            // found
            break;
        }

        // next node
        cur_lst_node = cur_lst_node->next;
    }
    if (cur_lst_node != NULL) {
        if (exist != NULL) {
            *exist = TT_CONTAINER(cur_lst_node, tt_hnode_t, list_node);
        }
        return TT_ALREADY_EXIST;
    }

    // add to slot
    tt_lnode_init(&node->list_node);
    tt_list_addhead(list, &node->list_node);

    return TT_SUCCESS;
}

tt_result_t __la_remove(IN tt_hslot_t *hslot, IN tt_hnode_t *node)
{
    tt_list_remove(&node->list_node);
    return TT_SUCCESS;
}

tt_hnode_t *__la_find(IN tt_hslot_t *hslot,
                      IN const tt_u8_t *key,
                      IN tt_u32_t key_len,
                      IN tt_hnode_t *cur_node)
{
    tt_hashmap_t *hmap = TT_CONTAINER(hslot, tt_hashmap_t, hslot);
    tt_hashval_t hval = 0;
    tt_list_t *list = NULL;
    tt_hnode2key_t hnode2key = NULL;
    tt_lnode_t *cur_lst_node = NULL;

    if (cur_node == NULL) {
        // find the first matching node if not specified

        // calc hash value
        hval = hmap->hash(key, key_len, &hmap->hashctx);

        // find slot
        list = &hslot->list_array.list[hval % hslot->list_array.list_num];

        // search each node in slot
        cur_lst_node = tt_list_head(list);
    } else {
        // if specified, start from the next node
        cur_lst_node = cur_node->list_node.next;
    }

    hnode2key = hmap->hnode2key;
    while (cur_lst_node != NULL) {
        const tt_u8_t *cur_key = NULL;
        tt_u32_t cur_key_len = 0;

        // get node key
        hnode2key(TT_CONTAINER(cur_lst_node, tt_hnode_t, list_node),
                  &cur_key,
                  &cur_key_len);
        TT_ASSERT_HMAP(cur_key != NULL);
        TT_ASSERT_HMAP(cur_key_len != 0);

        // compare key
        if ((key_len == cur_key_len) &&
            (tt_memcmp(cur_key, key, key_len) == 0)) {
            // found
            break;
        }

        // next node
        cur_lst_node = cur_lst_node->next;
    }

    // return
    if (cur_lst_node != NULL) {
        return TT_CONTAINER(cur_lst_node, tt_hnode_t, list_node);
    } else {
        return NULL;
    }
}

void __la_foreach(IN tt_hslot_t *hslot,
                  IN tt_hnode_action_t action,
                  IN void *param)
{
    tt_hslot_list_array_t *lbi = &hslot->list_array;
    tt_u32_t l_idx;

    for (l_idx = 0; l_idx < lbi->list_num; ++l_idx) {
        tt_list_t *list = &lbi->list[l_idx];
        tt_lnode_t *node = tt_list_head(list);
        while (node != NULL) {
            tt_lnode_t *next = node->next;

            action(TT_CONTAINER(hslot, tt_hashmap_t, hslot),
                   TT_CONTAINER(node, tt_hnode_t, list_node),
                   param);

            node = next;
        }
    }
}
