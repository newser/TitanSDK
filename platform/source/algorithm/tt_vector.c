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

#include <algorithm/tt_algorithm_def.h>
#include <misc/tt_assert.h>
#include <misc/tt_util.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __V_OBJ(v, idx) TT_PTR_INC(void *, (v)->p, (v)->obj_size *(idx))
#define __V_SIZE(v, num) ((v)->obj_size * num)

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
    TT_ASSERT_ALWAYS(!TT_U32_MUL_WOULD_OVFL(obj_size, attr->max_extent_num));
    TT_ASSERT_ALWAYS(!TT_U32_MUL_WOULD_OVFL(obj_size, attr->max_limit_num));

    vec->p = NULL;
    vec->cmp = attr->cmp;
    vec->size = 0;
    tt_memspg_init(&vec->mspg,
                   attr->min_extent_num * obj_size,
                   attr->max_extent_num * obj_size,
                   attr->max_limit_num * obj_size);
    vec->obj_size = obj_size;
    vec->count = 0;
    vec->capacity = 0;
}

void tt_vec_destroy(IN tt_vec_t *vec)
{
    TT_ASSERT(vec != NULL);

    if (vec->p != NULL) {
        tt_memspg_compress(&vec->mspg, &vec->p, &vec->size, 0);
    }
}

void tt_vec_attr_default(IN tt_vec_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->cmp = NULL;

    attr->min_extent_num = 16;
    attr->max_extent_num = 128;
    attr->max_limit_num = 0;
}

tt_result_t __vec_reserve(IN tt_vec_t *vec, IN tt_u32_t count)
{
    TT_DO(tt_memspg_extend(&vec->mspg,
                           &vec->p,
                           &vec->size,
                           (vec->capacity + count) * vec->obj_size));
    vec->capacity = vec->size / vec->obj_size;

    return TT_SUCCESS;
}

tt_result_t tt_vec_push_head(IN tt_vec_t *vec, IN void *obj)
{
    TT_DO(tt_vec_reserve(vec, 1));
    tt_memmove(__V_OBJ(vec, 1), vec->p, __V_SIZE(vec, vec->count));
    tt_memcpy(vec->p, obj, vec->obj_size);
    ++vec->count;

    return TT_SUCCESS;
}

tt_result_t tt_vec_push_tail(IN tt_vec_t *vec, IN void *obj)
{
    TT_DO(tt_vec_reserve(vec, 1));
    tt_memcpy(__V_OBJ(vec, vec->count), obj, vec->obj_size);
    ++vec->count;

    return TT_SUCCESS;
}

tt_result_t tt_vec_pop_head(IN tt_vec_t *vec, OUT OPT void *obj)
{
    if (vec->count == 0) {
        return TT_FAIL;
    }

    if (obj != NULL) {
        tt_memcpy(obj, vec->p, vec->obj_size);
    }
    tt_memmove(vec->p, __V_OBJ(vec, 1), __V_SIZE(vec, vec->count - 1));
    --vec->count;

    return TT_SUCCESS;
}

tt_result_t tt_vec_pop_tail(IN tt_vec_t *vec, OUT OPT void *obj)
{
    if (vec->count == 0) {
        return TT_FAIL;
    }

    if (obj != NULL) {
        tt_memcpy(obj, __V_OBJ(vec, vec->count - 1), vec->obj_size);
    }
    --vec->count;

    return TT_SUCCESS;
}

void *tt_vec_head(IN tt_vec_t *vec)
{
    if (vec->count == 0) {
        return NULL;
    }

    return vec->p;
}

void *tt_vec_tail(IN tt_vec_t *vec)
{
    if (vec->count == 0) {
        return NULL;
    }

    return __V_OBJ(vec, vec->count - 1);
}

tt_result_t tt_vec_insert(IN tt_vec_t *vec, IN tt_u32_t idx, IN void *obj)
{
    if (idx >= vec->count) {
        return TT_FAIL;
    }

    TT_DO(tt_vec_reserve(vec, 1));
    tt_memmove(__V_OBJ(vec, idx + 1),
               __V_OBJ(vec, idx),
               __V_SIZE(vec, vec->count - idx));
    tt_memcpy(__V_OBJ(vec, idx), obj, vec->obj_size);
    ++vec->count;

    return TT_SUCCESS;
}

tt_result_t tt_vec_move_all(IN tt_vec_t *dst, IN tt_vec_t *src)
{
    TT_ASSERT_ALWAYS(dst != src);
    TT_ASSERT_ALWAYS(dst->obj_size == src->obj_size);

    TT_DO(tt_vec_reserve(dst, src->count));
    tt_memcpy(__V_OBJ(dst, dst->count), src->p, __V_SIZE(src, src->count));
    dst->count += src->count;

    tt_vec_clear(src);

    return TT_SUCCESS;
}

tt_result_t tt_vec_move_from(IN tt_vec_t *dst,
                             IN tt_vec_t *src,
                             IN tt_u32_t from_idx)
{
    tt_u32_t n;

    TT_ASSERT_ALWAYS(dst != src);
    TT_ASSERT_ALWAYS(dst->obj_size == src->obj_size);

    if (from_idx >= src->count) {
        return TT_SUCCESS;
    }
    n = src->count - from_idx;

    TT_DO(tt_vec_reserve(dst, n));
    tt_memcpy(__V_OBJ(dst, dst->count),
              __V_OBJ(src, from_idx),
              __V_SIZE(src, n));
    dst->count += n;

    src->count -= n;

    return TT_SUCCESS;
}

tt_result_t tt_vec_move_range(IN tt_vec_t *dst,
                              IN tt_vec_t *src,
                              IN tt_u32_t from_idx,
                              IN tt_u32_t to_idx)
{
    tt_u32_t n;

    TT_ASSERT_ALWAYS(dst != src);
    TT_ASSERT_ALWAYS(dst->obj_size == src->obj_size);
    TT_ASSERT_ALWAYS(from_idx <= to_idx);

    if (from_idx >= src->count) {
        return TT_SUCCESS;
    }
    if (to_idx >= src->count) {
        to_idx = src->count;
    }
    n = to_idx - from_idx;

    TT_DO(tt_vec_reserve(dst, n));
    tt_memcpy(__V_OBJ(dst, dst->count),
              __V_OBJ(src, from_idx),
              __V_SIZE(src, n));
    dst->count += n;

    tt_memcpy(__V_OBJ(src, from_idx),
              __V_OBJ(src, to_idx),
              __V_SIZE(src, src->count - to_idx));
    src->count -= n;

    return TT_SUCCESS;
}

tt_bool_t tt_vec_comtain(IN tt_vec_t *vec, IN void *obj)
{
    return TT_BOOL(tt_vec_find(vec, obj) != TT_POS_NULL);
}

tt_bool_t tt_vec_comtain_all(IN tt_vec_t *vec, IN tt_vec_t *vec2)
{
    tt_u32_t i;
    for (i = 0; i < vec2->count; ++i) {
        if (tt_vec_find(vec, __V_OBJ(vec2, i)) == TT_POS_NULL) {
            return TT_FALSE;
        }
    }
    return TT_TRUE;
}

void *tt_vec_get(IN tt_vec_t *vec, IN tt_u32_t idx)
{
    if (idx >= vec->count) {
        return NULL;
    }

    return __V_OBJ(vec, idx);
}

tt_result_t tt_vec_set(IN tt_vec_t *vec, IN tt_u32_t idx, IN void *obj)
{
    if (idx >= vec->count) {
        return TT_FAIL;
    }

    tt_memcpy(__V_OBJ(vec, idx), obj, vec->obj_size);
    return TT_SUCCESS;
}

tt_u32_t tt_vec_find(IN tt_vec_t *vec, IN void *obj)
{
    tt_u32_t i;

    if (vec->cmp != NULL) {
        for (i = 0; i < vec->count; ++i) {
            if (vec->cmp(__V_OBJ(vec, i), obj) == 0) {
                return i;
            }
        }
    } else {
        for (i = 0; i < vec->count; ++i) {
            if (tt_memcmp(__V_OBJ(vec, i), obj, vec->obj_size) == 0) {
                return i;
            }
        }
    }

    return TT_POS_NULL;
}

tt_u32_t tt_vec_find_last(IN tt_vec_t *vec, IN void *obj)
{
    tt_u32_t i;
    
    if (vec->cmp != NULL) {
        for (i = vec->count - 1; i != ~0; --i) {
            if (vec->cmp(__V_OBJ(vec, i), obj) == 0) {
                return i;
            }
        }
    } else {
        for (i = vec->count - 1; i != ~0; --i) {
            if (tt_memcmp(__V_OBJ(vec, i), obj, vec->obj_size) == 0) {
                return i;
            }
        }
    }
    
    return TT_POS_NULL;
}

tt_u32_t tt_vec_find_from(IN tt_vec_t *vec, IN void *obj, IN tt_u32_t from_idx)
{
    tt_u32_t i;

    if (from_idx >= vec->count) {
        return TT_POS_NULL;
    }

    if (vec->cmp != NULL) {
        for (i = from_idx; i < vec->count; ++i) {
            if (vec->cmp(__V_OBJ(vec, i), obj) == 0) {
                return i;
            }
        }
    } else {
        for (i = from_idx; i < vec->count; ++i) {
            if (tt_memcmp(__V_OBJ(vec, i), obj, vec->obj_size) == 0) {
                return i;
            }
        }
    }

    return TT_POS_NULL;
}

tt_u32_t tt_vec_find_range(IN tt_vec_t *vec,
                           IN void *obj,
                           IN tt_u32_t from_idx,
                           IN tt_u32_t to_idx)
{
    tt_u32_t i;

    TT_ASSERT_ALWAYS(from_idx <= to_idx);

    if (from_idx >= vec->count) {
        return TT_POS_NULL;
    }
    if (to_idx >= vec->count) {
        to_idx = vec->count;
    }

    if (vec->cmp != NULL) {
        for (i = from_idx; i < to_idx; ++i) {
            if (vec->cmp(__V_OBJ(vec, i), obj) == 0) {
                return i;
            }
        }
    } else {
        for (i = from_idx; i < to_idx; ++i) {
            if (tt_memcmp(__V_OBJ(vec, i), obj, vec->obj_size) == 0) {
                return i;
            }
        }
    }

    return TT_POS_NULL;
}

void tt_vec_remove(IN tt_vec_t *vec, IN tt_u32_t idx)
{
    if (idx >= vec->count) {
        return;
    }

    tt_memmove(__V_OBJ(vec, idx),
               __V_OBJ(vec, idx + 1),
               __V_SIZE(vec, vec->count - idx - 1));
    --vec->count;
}

// return removed idx
tt_u32_t tt_vec_remove_equal(IN tt_vec_t *vec, IN void *obj)
{
    tt_u32_t idx = tt_vec_find(vec, obj);
    if (idx != TT_POS_NULL) {
        tt_vec_remove(vec, idx);
    }
    return idx;
}

void tt_vec_remove_range(IN tt_vec_t *vec,
                         IN tt_u32_t from_idx,
                         IN tt_u32_t to_idx)
{
    TT_ASSERT_ALWAYS(from_idx <= to_idx);

    if (from_idx >= vec->count) {
        return;
    }
    if (to_idx >= vec->count) {
        to_idx = vec->count;
    }

    tt_memmove(__V_OBJ(vec, from_idx),
               __V_OBJ(vec, to_idx),
               __V_SIZE(vec, vec->count - to_idx));
    vec->count -= (to_idx - from_idx);
}

void tt_vec_trim(IN tt_vec_t *vec)
{
    if (vec->count < vec->capacity) {
        tt_memspg_compress(&vec->mspg,
                           &vec->p,
                           &vec->size,
                           __V_SIZE(vec, vec->count));
        vec->capacity = vec->size / vec->obj_size;
    }
}
