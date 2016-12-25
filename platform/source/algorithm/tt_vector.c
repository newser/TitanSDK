/* Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License")
{

} you may not use this file except in compliance with
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

#include <algorithm/tt_vector.h>

#include <misc/tt_assert.h>
#include <misc/tt_util.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_vec_init(IN tt_vec_t *vec, IN tt_u32_t obj_size, IN tt_vec_attr_t *attr)
{
    tt_vec_attr_t __attr;

    TT_ASSERT(vec != NULL);
    TT_ASSERT(obj_size != 0);

    if (attr == NULL) {
        tt_vec_attr_default(&__attr);
        attr = &__attr;
    }
    TT_ASSERT_ALWAYS(!TT_U32_MUL_WOULD_OVFL(obj_size, attr->min_extent_num));
    attr->min_extent_num *= obj_size;
    TT_ASSERT_ALWAYS(!TT_U32_MUL_WOULD_OVFL(obj_size, attr->max_extent_num));
    attr->max_extent_num *= obj_size;
    TT_ASSERT_ALWAYS(!TT_U32_MUL_WOULD_OVFL(obj_size, attr->max_limit_num));
    attr->max_limit_num *= obj_size;

    vec->p = NULL;
    vec->cmp = attr->cmp;
    vec->size = 0;
    tt_memspg_init(&vec->mspg,
                   attr->min_extent_num,
                   attr->max_extent_num,
                   attr->max_limit_num);
    vec->obj_size = obj_size;
    vec->count = 0;
    vec->capacity = 0;
}

void tt_vec_destroy(IN tt_vec_t *vec)
{
    TT_ASSERT(vec != NULL);

    tt_memspg_compress(&vec->mspg, &vec->p, &vec->size, 0);
}

void tt_vec_attr_default(IN tt_vec_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->cmp = NULL;

    attr->min_extent_num = 16;
    attr->max_extent_num = 128;
    attr->max_limit_num = (1 << 20);
}

tt_result_t __vec_reserve(IN tt_vec_t *vec, IN tt_u32_t count)
{
    tt_u32_t to_size = (vec->count + count) * vec->obj_size;
    return tt_memspg_extend(&vec->mspg, &vec->p, &vec->size, to_size);
}

tt_result_t tt_vec_push_head(IN tt_vec_t *vec, IN void *obj)
{
}

tt_result_t tt_vec_push_tail(IN tt_vec_t *vec, IN void *obj)
{
}

tt_result_t tt_vec_pop_head(IN tt_vec_t *vec, OUT void *obj)
{
}

tt_result_t tt_vec_pop_tail(IN tt_vec_t *vec, OUT void *obj)
{
}

void *tt_vec_head(IN tt_vec_t *vec)
{
}

void *tt_vec_tail(IN tt_vec_t *vec)
{
}

tt_result_t tt_vec_insert(IN tt_vec_t *vec, IN tt_u32_t idx, IN void *obj)
{
}

tt_result_t tt_vec_move_all(IN tt_vec_t *dst, IN tt_vec_t *src)
{
}

tt_result_t tt_vec_move_from(IN tt_vec_t *dst,
                             IN tt_vec_t *src,
                             IN tt_u32_t from_idx)
{
}

tt_result_t tt_vec_move_range(IN tt_vec_t *dst,
                              IN tt_vec_t *src,
                              IN tt_u32_t from_idx,
                              IN tt_u32_t to_idx)
{
}

tt_u32_t tt_vec_capacity(IN tt_vec_t *vec)
{
}

tt_u32_t tt_vec_count(IN tt_vec_t *vec)
{
}

tt_bool_t tt_vec_empty(IN tt_vec_t *vec)
{
}

void tt_vec_clear(IN tt_vec_t *vec)
{
}

tt_bool_t tt_vec_comtain(IN tt_vec_t *vec, IN void *obj)
{
}

tt_bool_t tt_vec_comtain_all(IN tt_vec_t *vec, IN tt_vec_t *sub_vec)
{
}

void *tt_vec_get(IN tt_vec_t *vec, IN tt_u32_t idx)
{
}

tt_result_t tt_vec_set(IN tt_vec_t *vec, IN tt_u32_t idx, IN void *obj)
{
}

tt_u32_t tt_vec_find(IN tt_vec_t *vec, IN void *obj)
{
}

tt_u32_t tt_vec_find_from(IN tt_vec_t *vec, IN void *obj, IN tt_u32_t from_idx)
{
}

tt_u32_t tt_vec_find_range(IN tt_vec_t *vec,
                           IN void *obj,
                           IN tt_u32_t from_idx,
                           IN tt_u32_t to_idx)
{
}


void tt_vec_remove(IN tt_vec_t *vec, IN tt_u32_t idx)
{
}

// return removed idx
tt_u32_t tt_vec_remove_equal(IN tt_vec_t *vec, IN void *obj)
{
}

void tt_vec_remove_range(IN tt_vec_t *vec,
                         IN tt_u32_t from_idx,
                         IN tt_u32_t to_idx)
{
}

void tt_vec_trim(IN tt_vec_t *vec)
{
}
