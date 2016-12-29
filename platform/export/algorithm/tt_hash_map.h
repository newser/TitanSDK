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
@file tt_hash_map.h
@brief hash

this file specifies hash map APIs

 - clear
 - hash
 - contain_key/contain_value
 - find(key)/insert(k,v)
 - empty/count
 - remove(key)/remove_kv(key, value)
 - replace/replace_kv
*/

#ifndef __TT_HASH_MAP__
#define __TT_HASH_MAP__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_list.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_hashmap_s;

// ========================================
// hash calculate
// ========================================

typedef enum {
    TT_HASHFUNC_MURMUR3,
    TT_HASHFUNC_FNV1A,

    TT_HASHFUNC_NUM
} tt_hashalg_t;
#define TT_HASHFUNC_VALID(t) ((t) < TT_HASHFUNC_NUM)

typedef tt_u32_t tt_hashcode_t;

typedef union
{
    tt_u32_t seed;
} tt_hashctx_t;

typedef tt_hashcode_t (*tt_hashfunc_t)(IN const tt_u8_t *key,
                                       IN tt_u32_t key_len,
                                       IN tt_hashctx_t *hctx);

// ========================================
// hash node
// ========================================

// node in hslot
typedef union
{
    tt_lnode_t list_node;
} tt_hnode_t;

// get key of node, this is called once per searching, so
// won't be a performance issue
typedef void (*tt_hnode2key_t)(IN tt_hnode_t *node,
                               OUT const tt_u8_t **key,
                               OUT tt_u32_t *key_len);

typedef void (*tt_hnode_do_t)(IN struct tt_hashmap_s *hmap,
                              IN tt_hnode_t *hnode,
                              IN void *param);

// ========================================
// hash hslot
// ========================================

typedef enum {
    // [single hslot type]_[hslot container]

    TT_HSLOT_TYPE_LIST_ARRAY,

    TT_HSLOT_TYPE_NUM
} tt_hslot_type_t;
#define TT_HSLOT_TYPE_VALID(t) ((t) < TT_HSLOT_TYPE_NUM)

// TT_HSLOT_TYPE_LIST_ARRAY
typedef struct
{
    tt_list_t *list;
    tt_u32_t list_num;
} tt_hslot_list_array_t;

// how hslot arrange nodes
typedef union
{
    tt_hslot_list_array_t list_array;
} tt_hslot_t;

typedef tt_result_t (*tt_hslot_create_t)(IN tt_hslot_t *hslot);

typedef tt_result_t (*tt_hslot_destroy_t)(IN tt_hslot_t *hslot);

typedef tt_result_t (*tt_hslot_add_t)(IN tt_hslot_t *hslot,
                                      IN tt_hnode_t *node);

typedef tt_result_t (*tt_hslot_adduniq_t)(IN tt_hslot_t *hslot,
                                          IN tt_hnode_t *node,
                                          OUT tt_hnode_t **exist);

typedef tt_result_t (*tt_hslot_remove_t)(IN tt_hslot_t *hslot,
                                         IN tt_hnode_t *node);

typedef tt_hnode_t *(*tt_hslot_find_t)(IN tt_hslot_t *hslot,
                                       IN const tt_u8_t *key,
                                       IN tt_u32_t key_len,
                                       IN tt_hnode_t *cur_node);

typedef void (*tt_hslot_foreach_t)(IN tt_hslot_t *hslot,
                                   IN tt_hnode_do_t action,
                                   IN void *param);

// ========================================
// hash map
// ========================================

typedef struct
{
    tt_hashalg_t hashalg;
    tt_hslot_type_t hslot_type;
} tt_hashmap_attr_t;

typedef struct
{
    tt_hslot_create_t create;
    tt_hslot_destroy_t destroy;
    tt_hslot_add_t add;
    tt_hslot_adduniq_t adduniq;
    tt_hslot_remove_t remove;
    tt_hslot_find_t find;
    tt_hslot_foreach_t foreach;
} tt_hashmap_itf_t;

typedef struct tt_hashmap_s
{
    tt_hslot_t hslot;

    tt_u32_t slot_num;
    tt_hnode2key_t hnode2key;
    tt_hashmap_attr_t attr;

    tt_hashfunc_t hash;
    tt_hashctx_t hashctx;
    tt_hashmap_itf_t *itf;
} tt_hashmap_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_hashmap_create(IN tt_hashmap_t *hmap,
                                     IN tt_u32_t slot_num,
                                     IN tt_hnode2key_t hnode2key,
                                     IN OPT tt_hashmap_attr_t *attr);

tt_inline tt_result_t tt_hashmap_destroy(IN tt_hashmap_t *hmap)
{
    return hmap->itf->destroy(&hmap->hslot);
}

extern void tt_hashmap_attr_default(IN tt_hashmap_attr_t *attr);

tt_inline tt_result_t tt_hashmap_add(IN tt_hashmap_t *hmap, IN tt_hnode_t *node)
{
    return hmap->itf->add(&hmap->hslot, node);
}

// return TT_ALREADY_EXIST if not unique
tt_inline tt_result_t tt_hashmap_adduniq(IN tt_hashmap_t *hmap,
                                         IN tt_hnode_t *node,
                                         OUT tt_hnode_t **exist)
{
    return hmap->itf->adduniq(&hmap->hslot, node, exist);
}

tt_inline tt_result_t tt_hashmap_remove(IN tt_hashmap_t *hmap,
                                        IN tt_hnode_t *node)
{
    return hmap->itf->remove(&hmap->hslot, node);
}

// search from cur_node if it's not null, this is used when
// hash map allow nodes with completely same key
tt_inline tt_hnode_t *tt_hashmap_find(IN tt_hashmap_t *hmap,
                                      IN tt_u8_t *key,
                                      IN tt_u32_t key_len,
                                      IN tt_hnode_t *cur_node)
{
    return hmap->itf->find(&hmap->hslot, key, key_len, cur_node);
}

tt_inline void tt_hashmap_foreach(IN tt_hashmap_t *hmap,
                                  IN tt_hnode_do_t action,
                                  IN void *param)
{
    hmap->itf->foreach (&hmap->hslot, action, param);
}

tt_inline tt_hashcode_t tt_hashmap_hash(IN tt_hashmap_t *hmap,
                                        IN tt_u8_t *key,
                                        IN tt_u32_t len)
{
    return hmap->hash(key, len, &hmap->hashctx);
}

tt_inline void tt_hnode_init(IN tt_hnode_t *hnode)
{
    // setting to 0 fits all kinds of node
    tt_memset(hnode, 0, sizeof(tt_hnode_t));
}

#endif /* __TT_HASH_MAP__ */
