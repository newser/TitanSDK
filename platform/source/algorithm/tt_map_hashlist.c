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

#include <algorithm/tt_map_hashlist.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define TT_ASSERT_HL TT_ASSERT

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static void __hl_destroy(IN tt_map_t *map);
static void __hl_clear(IN tt_map_t *map);
static tt_u32_t __hl_count(IN tt_map_t *map);
static tt_result_t __hl_add(IN tt_map_t *map, IN tt_mnode_t *mnode);
static void __hl_remove(IN tt_map_t *map, IN tt_mnode_t *mnode);
static tt_mnode_t *__hl_find(IN tt_map_t *map,
                             IN const tt_u8_t *key,
                             IN tt_u32_t key_len,
                             IN tt_mnode_t *cur_mnode);
static void __hl_foreach(IN tt_map_t *map,
                         IN tt_mnode_action_t action,
                         IN void *param);

static tt_map_itf_t __hl_itf = {NULL,
                                __hl_destroy,
                                __hl_clear,
                                __hl_count,
                                __hl_add,
                                __hl_remove,
                                __hl_find,
                                __hl_foreach};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_map_t *tt_map_hashlist_create(IN tt_mnode_key_t get_key,
                                 IN tt_u32_t list_num,
                                 IN OPT tt_map_hl_attr_t *attr)
{
    tt_map_hl_attr_t __attr;
    tt_u32_t size, i;
    tt_map_t *map;
    tt_map_hl_t *hl;

    TT_ASSERT(get_key != NULL);
    TT_ASSERT(list_num != 0);

    if (attr == NULL) {
        tt_map_hashlist_attr_default(&__attr);
        attr = &__attr;
    }
    TT_ASSERT(TT_HASH_ALG_VALID(attr->hash_alg));

    size = sizeof(tt_map_hl_t) + sizeof(tt_dlist_t) * list_num;
    map = tt_map_create(size, &__hl_itf);
    if (map == NULL) {
        return NULL;
    }

    hl = TT_MAP_CAST(map, tt_map_hl_t);

    hl->dll = TT_PTR_INC(tt_dlist_t, hl, sizeof(tt_map_hl_t));
    hl->dll_num = list_num;
    for (i = 0; i < list_num; ++i) {
        tt_dlist_init(&hl->dll[i]);
    }

    if (attr->hash_alg == TT_HASH_ALG_MURMUR3) {
        hl->hash = tt_hash_murmur3;
    } else {
        hl->hash = tt_hash_fnv1a;
    }
    tt_hashctx_init(&hl->hashctx);

    hl->get_key = get_key;
    hl->uniq_key = attr->uniq_key;

    return map;
}

void tt_map_hashlist_attr_default(IN tt_map_hl_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->hash_alg = TT_HASH_ALG_MURMUR3;
    attr->uniq_key = TT_FALSE;
}

void __hl_destroy(IN tt_map_t *map)
{
    tt_map_hl_t *hl = TT_MAP_CAST(map, tt_map_hl_t);
    tt_u32_t i;

    for (i = 0; i < hl->dll_num; ++i) {
        if (!tt_dlist_empty(&hl->dll[i])) {
            TT_FATAL("list %d is not empty", i);
        }
    }
}

void __hl_clear(IN tt_map_t *map)
{
    tt_map_hl_t *hl = TT_MAP_CAST(map, tt_map_hl_t);
    tt_u32_t i;

    for (i = 0; i < hl->dll_num; ++i) {
        tt_dlist_clear(&hl->dll[i]);
    }
}

tt_u32_t __hl_count(IN tt_map_t *map)
{
    tt_map_hl_t *hl = TT_MAP_CAST(map, tt_map_hl_t);
    tt_u32_t i, count = 0;

    for (i = 0; i < hl->dll_num; ++i) {
        count += tt_dlist_count(&hl->dll[i]);
    }

    return count;
}

tt_result_t __hl_add(IN tt_map_t *map, IN tt_mnode_t *mnode)
{
    tt_map_hl_t *hl = TT_MAP_CAST(map, tt_map_hl_t);

    const tt_u8_t *key;
    tt_u32_t key_len;
    tt_hashcode_t hashcode;
    tt_dlist_t *dll;

    hl->get_key(mnode, &key, &key_len);
    TT_ASSERT_HL(key != NULL);
    TT_ASSERT_HL(key_len != 0);

    hashcode = hl->hash(key, key_len, &hl->hashctx);

    dll = &hl->dll[hashcode % hl->dll_num];
    if (hl->uniq_key) {
        tt_dnode_t *dnode = tt_dlist_head(dll);
        while (dnode != NULL) {
            const tt_u8_t *k;
            tt_u32_t klen;
            hl->get_key(TT_CONTAINER(dnode, tt_mnode_t, dnode), &k, &klen);
            if ((klen == key_len) && (tt_memcmp(k, key, klen) == 0)) {
                TT_ERROR("duplicated key");
                return TT_FAIL;
            }

            dnode = tt_dlist_next(dll, dnode);
        }
    }
    tt_dnode_init(&mnode->dnode);
    tt_dlist_push_head(dll, &mnode->dnode);

    return TT_SUCCESS;
}

#if 0
tt_result_t __hl_adduniq(IN tt_map_t *map,
                         IN tt_mnode_t *mnode,
                         OUT tt_mnode_t **exist)
{
    tt_map_t *hmap = TT_CONTAINER(hslot, tt_map_t, hslot);
    const tt_u8_t *key = NULL;
    tt_u32_t key_len = 0;
    tt_hashcode_t hval = 0;
    tt_list_t *list = NULL;
    
    tt_hnode2key_t mnode2key = hmap->mnode2key;
    tt_lnode_t *cur_mnode = NULL;
    
    // get key
    mnode2key(mnode, &key, &key_len);
    TT_ASSERT_HL(key != NULL);
    TT_ASSERT_HL(key_len != 0);
    
    // calc hash value
    hval = hmap->hash(key, key_len, &hmap->hashctx);
    
    // find dllot by index
    list = &hslot->list_array.list[hval % hslot->list_array.list_num];
    
    // check uniqueness
    cur_mnode = tt_list_head(list);
    while (cur_mnode != NULL) {
        const tt_u8_t *cur_key = NULL;
        tt_u32_t cur_key_len = 0;
        
        // get mnode key
        mnode2key(TT_CONTAINER(cur_mnode, tt_mnode_t, list_node),
                  &cur_key,
                  &cur_key_len);
        TT_ASSERT_HL(cur_key != NULL);
        TT_ASSERT_HL(cur_key_len != 0);
        
        // compare key
        if ((key_len == cur_key_len) &&
            (tt_memcmp(cur_key, key, key_len) == 0)) {
            // found
            break;
        }
        
        // next mnode
        cur_mnode = cur_mnode->next;
    }
    if (cur_mnode != NULL) {
        if (exist != NULL) {
            *exist = TT_CONTAINER(cur_mnode, tt_mnode_t, list_node);
        }
        return TT_ALREADY_EXIST;
    }
    
    // add to dllot
    tt_lnode_init(&mnode->list_node);
    tt_list_push_head(list, &mnode->list_node);
    
    return TT_SUCCESS;
}
#endif

void __hl_remove(IN tt_map_t *map, IN tt_mnode_t *mnode)
{
    tt_dlist_remove(&mnode->dnode);
}

tt_mnode_t *__hl_find(IN tt_map_t *map,
                      IN const tt_u8_t *key,
                      IN tt_u32_t key_len,
                      IN tt_mnode_t *pos)
{
    tt_map_hl_t *hl = TT_MAP_CAST(map, tt_map_hl_t);

    tt_hashcode_t hashcode = hl->hash(key, key_len, &hl->hashctx);
    tt_dlist_t *dll = &hl->dll[hashcode % hl->dll_num];
    tt_dnode_t *dnode = TT_COND(pos == NULL,
                                tt_dlist_head(dll),
                                tt_dlist_next(dll, &pos->dnode));

    while (dnode != NULL) {
        const tt_u8_t *k;
        tt_u32_t klen;
        hl->get_key(TT_CONTAINER(dnode, tt_mnode_t, dnode), &k, &klen);
        if ((klen == key_len) && (tt_memcmp(k, key, klen) == 0)) {
            break;
        }

        dnode = tt_dlist_next(dll, dnode);
    }
    return TT_COND(dnode != NULL, TT_CONTAINER(dnode, tt_mnode_t, dnode), NULL);
}

void __hl_foreach(IN tt_map_t *map, IN tt_mnode_action_t action, IN void *param)
{
    tt_map_hl_t *hl = TT_MAP_CAST(map, tt_map_hl_t);
    tt_u32_t i;

    for (i = 0; i < hl->dll_num; ++i) {
        tt_dnode_t *dnode = tt_dlist_head(&hl->dll[i]);
        while (dnode != NULL) {
            action(map, TT_CONTAINER(dnode, tt_mnode_t, dnode), param);
            dnode = tt_dlist_next(&hl->dll[i], dnode);
        }
    }
}
