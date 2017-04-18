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
@file tt_vector.h
@brief vector

this file includes vector api

 - reserve
 - push_tail/push_head/pop_tail/pop_head
 - head/tail
 - insert
 - move_all/move_from/move_range
 - capacity/count/empty
 - clear
 - contain/contain_all
 - get(idx)/set(idx)
 - find/find_from/find_last
 - remove(idx)/remove_equal/remove_range(idx)
 - trim

 todo:
 - clone
 - sort
 -
 */

#ifndef __TT_VECTOR__
#define __TT_VECTOR__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_compare.h>
#include <memory/tt_memory_spring.h>
#include <misc/tt_util.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct
{
    tt_u32_t min_extent_num;
    tt_u32_t max_extent_num;
    tt_u32_t max_limit_num;
} tt_vec_attr_t;

typedef struct
{
    union
    {
        tt_u8_t *p;
        void **ptr;
    };
    tt_cmp_t cmp;
    tt_u32_t size;
    tt_memspg_t mspg;
    tt_u32_t obj_size;
    tt_u32_t count;
    tt_u32_t capacity;
} tt_vec_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern void tt_vec_init(IN tt_vec_t *vec,
                        IN tt_u32_t obj_size,
                        IN OPT tt_cmp_t cmp,
                        IN OPT tt_vec_attr_t *attr);

extern void tt_vec_destroy(IN tt_vec_t *vec);

extern void tt_vec_attr_default(IN tt_vec_attr_t *attr);

extern tt_result_t __vec_reserve(IN tt_vec_t *vec, IN tt_u32_t count);

tt_inline tt_result_t tt_vec_reserve(IN tt_vec_t *vec, IN tt_u32_t count)
{
    return TT_COND((vec->count + count) <= vec->capacity,
                   TT_SUCCESS,
                   __vec_reserve(vec, count));
}

extern tt_result_t tt_vec_push_head(IN tt_vec_t *vec, IN void *obj);

extern tt_result_t tt_vec_push_tail(IN tt_vec_t *vec, IN void *obj);

extern tt_result_t tt_vec_pop_head(IN tt_vec_t *vec, OUT OPT void *obj);

extern tt_result_t tt_vec_pop_tail(IN tt_vec_t *vec, OUT OPT void *obj);

extern void *tt_vec_head(IN tt_vec_t *vec);

extern void *tt_vec_tail(IN tt_vec_t *vec);

extern tt_result_t tt_vec_insert(IN tt_vec_t *vec,
                                 IN tt_u32_t idx,
                                 IN void *obj);

extern tt_result_t tt_vec_move_all(IN tt_vec_t *dst, IN tt_vec_t *src);

extern tt_result_t tt_vec_move_from(IN tt_vec_t *dst,
                                    IN tt_vec_t *src,
                                    IN tt_u32_t from_idx);

// [from_idx, to_idx)
extern tt_result_t tt_vec_move_range(IN tt_vec_t *dst,
                                     IN tt_vec_t *src,
                                     IN tt_u32_t from_idx,
                                     IN tt_u32_t to_idx);

tt_inline tt_u32_t tt_vec_capacity(IN tt_vec_t *vec)
{
    return vec->capacity;
}

tt_inline tt_u32_t tt_vec_count(IN tt_vec_t *vec)
{
    return vec->count;
}

tt_inline tt_bool_t tt_vec_empty(IN tt_vec_t *vec)
{
    return TT_BOOL(vec->count == 0);
}

tt_inline void tt_vec_clear(IN tt_vec_t *vec)
{
    vec->count = 0;
}

extern tt_bool_t tt_vec_comtain(IN tt_vec_t *vec, IN void *obj);

extern tt_bool_t tt_vec_comtain_all(IN tt_vec_t *vec, IN tt_vec_t *vec2);

extern void *tt_vec_get(IN tt_vec_t *vec, IN tt_u32_t idx);

extern tt_result_t tt_vec_set(IN tt_vec_t *vec, IN tt_u32_t idx, IN void *obj);

extern tt_u32_t tt_vec_find(IN tt_vec_t *vec, IN void *obj);

extern tt_u32_t tt_vec_find_last(IN tt_vec_t *vec, IN void *obj);

extern tt_u32_t tt_vec_find_from(IN tt_vec_t *vec,
                                 IN void *obj,
                                 IN tt_u32_t from_idx);

extern tt_u32_t tt_vec_find_range(IN tt_vec_t *vec,
                                  IN void *obj,
                                  IN tt_u32_t from_idx,
                                  IN tt_u32_t to_idx);

extern void tt_vec_remove(IN tt_vec_t *vec, IN tt_u32_t idx);

// return removed idx
extern tt_u32_t tt_vec_remove_equal(IN tt_vec_t *vec, IN void *obj);

// [from_idx, to_idx)
extern void tt_vec_remove_range(IN tt_vec_t *vec,
                                IN tt_u32_t from_idx,
                                IN tt_u32_t to_idx);

extern void tt_vec_trim(IN tt_vec_t *vec);

#endif /* __TT_VECTOR__ */
