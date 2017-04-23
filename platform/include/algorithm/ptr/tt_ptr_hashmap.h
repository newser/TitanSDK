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
@file tt_ptr_hashmap.h
@brief hash

this file specifies hash map APIs

 - clear
 - contain_key/contain_ptr
 - find(key)/add(key, p)
 - move
 - empty/count
 - remove(ptr)/remove_key(key)/remove_pair(key, ptr)
 - foreach
 - replace(key, ptr)/replace_equal(key, oldp, newp)
*/

#ifndef __TT_PTR_HASHMAP__
#define __TT_PTR_HASHMAP__

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

// return false if want to stop
typedef tt_bool_t (*tt_ptrhmap_action_t)(IN tt_u8_t *key,
                                         IN tt_u32_t key_len,
                                         IN tt_ptr_t ptr,
                                         IN void *param);

typedef struct
{
    tt_hash_alg_t hash_alg;
    tt_u32_t cache_threshold;
} tt_ptrhmap_attr_t;

typedef struct tt_ptrhmap_s
{
    tt_slist_t *sll;
    tt_hash_t hash;
    tt_slist_t cache;
    tt_u32_t sll_num;
    tt_hashctx_t hashctx;
    tt_u32_t cache_threshold;
    tt_u32_t cache_count;
    tt_u32_t count;
} tt_ptrhmap_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_ptrhmap_create(IN tt_ptrhmap_t *phm,
                                     IN tt_u32_t slot_num,
                                     IN OPT tt_ptrhmap_attr_t *attr);

extern void tt_ptrhmap_destroy(IN tt_ptrhmap_t *phm);

extern void tt_ptrhmap_attr_default(IN tt_ptrhmap_attr_t *attr);

extern void tt_ptrhmap_clear(IN tt_ptrhmap_t *phm);

tt_inline tt_u32_t tt_ptrhmap_count(IN tt_ptrhmap_t *phm)
{
    return phm->count;
}

tt_inline tt_bool_t tt_ptrhmap_empty(IN tt_ptrhmap_t *phm)
{
    return phm->count == 0 ? TT_TRUE : TT_FALSE;
}

extern tt_ptr_t tt_ptrhmap_find(IN tt_ptrhmap_t *phm,
                                IN tt_u8_t *key,
                                IN tt_u32_t key_len);

tt_inline tt_bool_t tt_ptrhmap_contain_key(IN tt_ptrhmap_t *phm,
                                           IN tt_u8_t *key,
                                           IN tt_u32_t key_len)
{
    return tt_ptrhmap_find(phm, key, key_len) != NULL ? TT_TRUE : TT_FALSE;
}

extern tt_bool_t tt_ptrhmap_contain_ptr(IN tt_ptrhmap_t *phm, IN tt_ptr_t ptr);

extern tt_result_t tt_ptrhmap_add(IN tt_ptrhmap_t *phm,
                                  IN tt_u8_t *key,
                                  IN tt_u32_t key_len,
                                  IN tt_ptr_t ptr);

extern tt_bool_t tt_ptrhmap_remove_key(IN tt_ptrhmap_t *phm,
                                       IN tt_u8_t *key,
                                       IN tt_u32_t key_len);

extern tt_bool_t tt_ptrhmap_remove_ptr(IN tt_ptrhmap_t *phm, IN tt_ptr_t ptr);

extern tt_bool_t tt_ptrhmap_remove_pair(IN tt_ptrhmap_t *phm,
                                        IN tt_u8_t *key,
                                        IN tt_u32_t key_len,
                                        IN tt_ptr_t ptr);

extern void tt_ptrhmap_foreach(IN tt_ptrhmap_t *phm,
                               IN tt_ptrhmap_action_t action,
                               IN void *param);

extern tt_bool_t tt_ptrhmap_replace(IN tt_ptrhmap_t *phm,
                                    IN tt_u8_t *key,
                                    IN tt_u32_t key_len,
                                    IN tt_ptr_t new_ptr);

extern tt_bool_t tt_ptrhmap_replace_equal(IN tt_ptrhmap_t *phm,
                                          IN tt_u8_t *key,
                                          IN tt_u32_t key_len,
                                          IN tt_ptr_t old_ptr,
                                          IN tt_ptr_t new_ptr);

#endif /* __TT_PTR_HASHMAP__ */
