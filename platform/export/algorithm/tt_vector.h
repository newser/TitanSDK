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

 - pushtail/pushhead/poptail/pophead
 - head/tail
 - insert
 - move
 - capacity/count/empty
 - clear
 - contain/contain_n
 - get(idx)/set(idx)
 - indexof
 - indexof/indexof_from/indexof_last
 - remove/remove_eq/remove_range
 - refine/reserve/expand

 optional:
 - clone
 - equal
 - sort
 -
 */

#ifndef __TT_VECTOR__
#define __TT_VECTOR__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct
{
    tt_u32_t min_expand_order;
    tt_u32_t max_expand_order;

    tt_u32_t max_size_order;
#define TT_VECTOR_MAX_SIZE_ORDER (30) // 1G

} tt_vector_attr_t;

typedef struct
{
    tt_vector_attr_t attr;

    tt_u8_t *mem;
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
