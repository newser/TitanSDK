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

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct
{
    tt_cmp_t cmp;

    tt_u32_t min_extent;
    tt_u32_t max_extent;
    tt_u32_t max_limit;
} tt_vector_attr_t;

typedef struct
{
    tt_u8_t *p;
    tt_cmp_t cmp;
    tt_u32_t size;
    tt_memspg_t mspg;
    tt_u32_t obj_size;
    tt_u32_t count;
    tt_u32_t capacity;
} tt_vector_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern void tt_vector_init(IN tt_vector_t *vec,
                           IN tt_u32_t obj_size,
                           IN tt_vector_attr_t *attr);

extern void tt_vector_destroy(IN tt_vector_t *vec);

extern void tt_vector_attr_default(IN tt_vector_attr_t *attr);

extern tt_result_t tt_vector_add(IN tt_vector_t *vec, IN void *obj);

extern tt_result_t tt_vector_insert(IN tt_vector_t *vec, IN void *obj);

#endif /* __TT_VECTOR__ */
