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
@file tt_hashmap.h
@brief hash

this file specifies hash hmap APIs

 - clear
 - contain_key
 - find(key)/add(k,v)
 - empty/count
 - remove(hnode)/remove_equal(key)
 - foreach
*/

#ifndef __TT_MAP__
#define __TT_MAP__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_hash.h>
#include <algorithm/tt_single_linked_list.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct
{
    tt_u8_t *key;
    tt_snode_t snode;
    tt_u32_t key_len;
} tt_hnode_t;

// return false if want to stop
typedef tt_bool_t (*tt_hmap_action_t)(IN tt_u8_t *key,
                                      IN tt_u32_t key_len,
                                      IN tt_hnode_t *hnode,
                                      IN void *param);

typedef struct
{
    tt_hash_alg_t hash_alg;
} tt_hmap_attr_t;

typedef struct tt_hashmap_s
{
    tt_slist_t *sll;
    tt_hash_t hash;
    tt_hashctx_t hashctx;
    tt_u32_t sll_num;
    tt_u32_t count;
} tt_hashmap_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_inline void tt_hnode_init(IN tt_hnode_t *hnode)
{
    hnode->key = NULL;
    tt_snode_init(&hnode->snode);
    hnode->key_len = 0;
}

tt_export tt_result_t tt_hmap_create(IN tt_hashmap_t *hmap,
                                     IN tt_u32_t slot_num,
                                     IN OPT tt_hmap_attr_t *attr);

tt_export void tt_hmap_destroy(IN tt_hashmap_t *hmap);

tt_export void tt_hmap_attr_default(IN tt_hmap_attr_t *attr);

tt_export void tt_hmap_clear(IN tt_hashmap_t *hmap);

tt_inline tt_u32_t tt_hmap_count(IN tt_hashmap_t *hmap)
{
    return hmap->count;
}

tt_inline tt_bool_t tt_hmap_empty(IN tt_hashmap_t *hmap)
{
    return hmap->count == 0 ? TT_TRUE : TT_FALSE;
}

tt_export tt_hnode_t *tt_hmap_find(IN tt_hashmap_t *hmap,
                                   IN tt_u8_t *key,
                                   IN tt_u32_t key_len);

tt_inline tt_bool_t tt_hmap_contain_key(IN tt_hashmap_t *hmap,
                                        IN tt_u8_t *key,
                                        IN tt_u32_t key_len)
{
    return tt_hmap_find(hmap, key, key_len) != NULL ? TT_TRUE : TT_FALSE;
}

tt_export tt_bool_t tt_hmap_contain(IN tt_hashmap_t *hmap,
                                    IN tt_hnode_t *hnode);

tt_export tt_result_t tt_hmap_add(IN tt_hashmap_t *hmap,
                                  IN tt_u8_t *key,
                                  IN tt_u32_t key_len,
                                  IN tt_hnode_t *hnode);

tt_export void tt_hmap_remove(IN tt_hashmap_t *hmap, IN tt_hnode_t *hnode);

tt_export tt_bool_t tt_hmap_remove_key(IN tt_hashmap_t *hmap,
                                       IN tt_u8_t *key,
                                       IN tt_u32_t key_len);

tt_export void tt_hmap_foreach(IN tt_hashmap_t *hmap,
                               IN tt_hmap_action_t action,
                               IN void *param);

#endif /* __TT_MAP__ */
