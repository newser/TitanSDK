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
@file tt_map.h
@brief hash

this file specifies hash map APIs

 - clear
 - contain_key
 - find(key)/add(k,v)
 - empty/count
 - remove(mnode)/remove_equal(key)
 - foreach
*/

#ifndef __TT_MAP__
#define __TT_MAP__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_double_linked_list.h>
#include <memory/tt_memory_alloc.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_MAP_CAST(m, type) TT_PTR_INC(type, m, sizeof(tt_map_t))

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_map_s;

// ========================================
// map mnode
// =======================================

typedef union
{
    tt_dnode_t dnode;
} tt_mnode_t;

// get key of mnode, this is called once per searching, so
// won't be a performance issue
typedef void (*tt_mnode_key_t)(IN tt_mnode_t *mnode,
                               OUT const tt_u8_t **key,
                               OUT tt_u32_t *key_len);

typedef void (*tt_mnode_action_t)(IN struct tt_map_s *map,
                                  IN tt_mnode_t *mnode,
                                  IN void *param);

// ========================================
// map
// =======================================

typedef tt_result_t (*tt_map_create_t)(IN struct tt_map_s *map);

typedef void (*tt_map_destroy_t)(IN struct tt_map_s *map);

typedef void (*tt_map_clear_t)(IN struct tt_map_s *map);

typedef tt_u32_t (*tt_map_count_t)(IN struct tt_map_s *map);

typedef tt_result_t (*tt_map_add_t)(IN struct tt_map_s *map,
                                    IN tt_mnode_t *mnode);

typedef void (*tt_map_remove_t)(IN struct tt_map_s *map, IN tt_mnode_t *mnode);

typedef tt_mnode_t *(*tt_map_find_t)(IN struct tt_map_s *map,
                                     IN const tt_u8_t *key,
                                     IN tt_u32_t key_len,
                                     IN tt_mnode_t *cur_mnode);

typedef void (*tt_map_foreach_t)(IN struct tt_map_s *map,
                                 IN tt_mnode_action_t action,
                                 IN void *param);

typedef struct
{
    tt_map_create_t create;
    tt_map_destroy_t destroy;
    tt_map_clear_t clear;
    tt_map_count_t count;
    tt_map_add_t add;
    tt_map_remove_t remove;
    tt_map_find_t find;
    tt_map_foreach_t foreach;
} tt_map_itf_t;

typedef struct tt_map_s
{
    tt_map_itf_t *itf;
} tt_map_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_inline void tt_mnode_init(IN tt_mnode_t *hnode)
{
    // setting to 0 fits all kinds of mnode
    tt_memset(hnode, 0, sizeof(tt_mnode_t));
}

extern tt_map_t *tt_map_create(IN tt_u32_t size, IN tt_map_itf_t *itf);

tt_inline void tt_map_destroy(IN tt_map_t *map)
{
    map->itf->destroy(map);
    tt_free(map);
}

tt_inline void tt_map_clear(IN tt_map_t *map)
{
    return map->itf->clear(map);
}

tt_inline tt_u32_t tt_map_count(IN tt_map_t *map)
{
    return map->itf->count(map);
}

tt_inline tt_bool_t tt_map_empty(IN tt_map_t *map)
{
    return map->itf->count(map) == 0 ? TT_TRUE : TT_FALSE;
}

tt_inline tt_bool_t tt_map_contain_key(IN tt_map_t *map,
                                       IN const tt_u8_t *key,
                                       IN tt_u32_t key_len)
{
    return map->itf->find(map, key, key_len, NULL) != NULL ? TT_TRUE : TT_FALSE;
}

tt_inline tt_result_t tt_map_add(IN tt_map_t *map, IN tt_mnode_t *mnode)
{
    return map->itf->add(map, mnode);
}

// search from cur_mnode if it's not null, this is used when
// hash map allow nodes with completely same key
tt_inline tt_mnode_t *tt_map_find(IN tt_map_t *map,
                                  IN tt_u8_t *key,
                                  IN tt_u32_t key_len,
                                  IN tt_mnode_t *cur_mnode)
{
    return map->itf->find(map, key, key_len, cur_mnode);
}

tt_inline void tt_map_remove(IN tt_map_t *map, IN tt_mnode_t *mnode)
{
    map->itf->remove(map, mnode);
}

tt_inline void tt_map_remove_equal(IN tt_map_t *map,
                                   IN tt_u8_t *key,
                                   IN tt_u32_t key_len)
{
    tt_mnode_t *mnode = tt_map_find(map, key, key_len, NULL);
    if (mnode != NULL) {
        map->itf->remove(map, mnode);
    }
}

tt_inline void tt_map_foreach(IN tt_map_t *map,
                              IN tt_mnode_action_t action,
                              IN void *param)
{
    map->itf->foreach (map, action, param);
}

#endif /* __TT_MAP__ */
