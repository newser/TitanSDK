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

#include <algorithm/ptr/tt_ptr_vector.h>

#include <algorithm/tt_algorithm_def.h>
#include <misc/tt_assert.h>
#include <misc/tt_util.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#if TT_ENV_IS_64BIT
#define __PV_SIZE(num) ((num) << 3)
#define __PV_NUM(size) ((size) >> 3)
#else
#define __PV_SIZE(num) ((num) << 2)
#define __PV_NUM(size) ((size) >> 2)
#endif

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

tt_result_t __ptrvec_reserve(IN tt_ptrvec_t *pvec, IN tt_u32_t count)
{
    TT_DO(tt_memspg_extend(&pvec->mspg,
                           &pvec->p,
                           &pvec->size,
                           __PV_SIZE(pvec->capacity + count)));
    pvec->capacity = __PV_NUM(pvec->size);

    return TT_SUCCESS;
}

tt_result_t tt_ptrvec_push_head(IN tt_ptrvec_t *pvec, IN tt_ptr_t p)
{
    // holding a null pointer is meaningless
    if (p == NULL) {
        return TT_FAIL;
    }

    TT_DO(tt_ptrvec_reserve(pvec, 1));
    tt_memmove(&pvec->ptr[1], pvec->p, __PV_SIZE(pvec->count));
    pvec->ptr[0] = p;
    ++pvec->count;

    return TT_SUCCESS;
}

tt_result_t tt_ptrvec_push_tail(IN tt_ptrvec_t *pvec, IN tt_ptr_t p)
{
    if (p == NULL) {
        return TT_FAIL;
    }

    TT_DO(tt_ptrvec_reserve(pvec, 1));
    pvec->ptr[pvec->count] = p;
    ++pvec->count;

    return TT_SUCCESS;
}

tt_ptr_t tt_ptrvec_pop_head(IN tt_ptrvec_t *pvec)
{
    tt_ptr_t ptr;

    if (pvec->count == 0) {
        return NULL;
    }
    ptr = pvec->ptr[0];

    tt_memmove(pvec->p, &pvec->ptr[1], __PV_SIZE(pvec->count - 1));
    --pvec->count;

    return ptr;
}

tt_ptr_t tt_ptrvec_pop_tail(IN tt_ptrvec_t *pvec)
{
    tt_ptr_t ptr;

    if (pvec->count == 0) {
        return NULL;
    }
    ptr = pvec->ptr[pvec->count - 1];

    --pvec->count;

    return ptr;
}

tt_ptr_t tt_ptrvec_head(IN tt_ptrvec_t *pvec)
{
    if (pvec->count == 0) {
        return NULL;
    }

    return pvec->ptr[0];
}

tt_ptr_t tt_ptrvec_tail(IN tt_ptrvec_t *pvec)
{
    if (pvec->count == 0) {
        return NULL;
    }

    return pvec->ptr[pvec->count - 1];
}

tt_result_t tt_ptrvec_insert(IN tt_ptrvec_t *pvec,
                             IN tt_u32_t idx,
                             IN tt_ptr_t p)
{
    if (p == NULL) {
        return TT_FAIL;
    }

    if (idx >= pvec->count) {
        return TT_FAIL;
    }

    TT_DO(tt_ptrvec_reserve(pvec, 1));
    tt_memmove(&pvec->ptr[idx + 1],
               &pvec->ptr[idx],
               __PV_SIZE(pvec->count - idx));
    pvec->ptr[idx] = p;
    ++pvec->count;

    return TT_SUCCESS;
}

tt_result_t tt_ptrvec_move_all(IN tt_ptrvec_t *dst, IN tt_ptrvec_t *src)
{
    TT_ASSERT_ALWAYS(dst != src);

    TT_DO(tt_ptrvec_reserve(dst, src->count));
    tt_memcpy(&dst->ptr[dst->count], src->ptr, __PV_SIZE(src->count));
    dst->count += src->count;

    tt_ptrvec_clear(src);

    return TT_SUCCESS;
}

tt_result_t tt_ptrvec_move_from(IN tt_ptrvec_t *dst,
                                IN tt_ptrvec_t *src,
                                IN tt_u32_t from_idx)
{
    tt_u32_t n;

    TT_ASSERT_ALWAYS(dst != src);

    if (from_idx >= src->count) {
        return TT_SUCCESS;
    }
    n = src->count - from_idx;

    TT_DO(tt_ptrvec_reserve(dst, n));
    tt_memcpy(&dst->ptr[dst->count], &src->ptr[from_idx], __PV_SIZE(n));
    dst->count += n;

    src->count -= n;

    return TT_SUCCESS;
}

tt_result_t tt_ptrvec_move_range(IN tt_ptrvec_t *dst,
                                 IN tt_ptrvec_t *src,
                                 IN tt_u32_t from_idx,
                                 IN tt_u32_t to_idx)
{
    tt_u32_t n;

    TT_ASSERT_ALWAYS(dst != src);
    TT_ASSERT_ALWAYS(from_idx <= to_idx);

    if (from_idx >= src->count) {
        return TT_SUCCESS;
    }
    if (to_idx >= src->count) {
        to_idx = src->count;
    }
    n = to_idx - from_idx;

    TT_DO(tt_ptrvec_reserve(dst, n));
    tt_memcpy(&dst->ptr[dst->count], &src->ptr[from_idx], __PV_SIZE(n));
    dst->count += n;

    tt_memcpy(&src->ptr[from_idx],
              &src->ptr[to_idx],
              __PV_SIZE(src->count - to_idx));
    src->count -= n;

    return TT_SUCCESS;
}

tt_bool_t tt_ptrvec_comtain(IN tt_ptrvec_t *pvec, IN tt_ptr_t p)
{
    return TT_BOOL(tt_ptrvec_find(pvec, p) != TT_POS_NULL);
}

tt_bool_t tt_ptrvec_comtain_all(IN tt_ptrvec_t *pvec, IN tt_ptrvec_t *pvec2)
{
    tt_u32_t i;
    for (i = 0; i < pvec2->count; ++i) {
        if (tt_ptrvec_find(pvec, pvec2->ptr[i]) == TT_POS_NULL) {
            return TT_FALSE;
        }
    }
    return TT_TRUE;
}

tt_ptr_t tt_ptrvec_get(IN tt_ptrvec_t *pvec, IN tt_u32_t idx)
{
    if (idx >= pvec->count) {
        return NULL;
    }

    return pvec->ptr[idx];
}

tt_result_t tt_ptrvec_set(IN tt_ptrvec_t *pvec, IN tt_u32_t idx, IN tt_ptr_t p)
{
    if (p == NULL) {
        return TT_FAIL;
    }

    if (idx >= pvec->count) {
        return TT_FAIL;
    }

    pvec->ptr[idx] = p;
    return TT_SUCCESS;
}

tt_u32_t tt_ptrvec_find(IN tt_ptrvec_t *pvec, IN tt_ptr_t p)
{
    tt_u32_t i;

    if (pvec->cmp != NULL) {
        for (i = 0; i < pvec->count; ++i) {
            if (pvec->cmp(pvec->ptr[i], p) == 0) {
                return i;
            }
        }
    } else {
        for (i = 0; i < pvec->count; ++i) {
            if (pvec->ptr[i] == p) {
                return i;
            }
        }
    }

    return TT_POS_NULL;
}

tt_u32_t tt_ptrvec_find_last(IN tt_ptrvec_t *pvec, IN tt_ptr_t p)
{
    tt_u32_t i;
    
    if (pvec->cmp != NULL) {
        for (i = pvec->count - 1; i != ~0; --i) {
            if (pvec->cmp(pvec->ptr[i], p) == 0) {
                return i;
            }
        }
    } else {
        for (i = pvec->count - 1; i != ~0; --i) {
            if (pvec->ptr[i] == p) {
                return i;
            }
        }
    }
    
    return TT_POS_NULL;
}

tt_u32_t tt_ptrvec_find_from(IN tt_ptrvec_t *pvec,
                             IN tt_ptr_t p,
                             IN tt_u32_t from_idx)
{
    tt_u32_t i;

    if (from_idx >= pvec->count) {
        return TT_POS_NULL;
    }

    if (pvec->cmp != NULL) {
        for (i = from_idx; i < pvec->count; ++i) {
            if (pvec->cmp(pvec->ptr[i], p) == 0) {
                return i;
            }
        }
    } else {
        for (i = from_idx; i < pvec->count; ++i) {
            if (pvec->ptr[i] == p) {
                return i;
            }
        }
    }

    return TT_POS_NULL;
}

tt_u32_t tt_ptrvec_find_range(IN tt_ptrvec_t *pvec,
                              IN tt_ptr_t p,
                              IN tt_u32_t from_idx,
                              IN tt_u32_t to_idx)
{
    tt_u32_t i;

    TT_ASSERT_ALWAYS(from_idx <= to_idx);

    if (from_idx >= pvec->count) {
        return TT_POS_NULL;
    }
    if (to_idx >= pvec->count) {
        to_idx = pvec->count;
    }

    if (pvec->cmp != NULL) {
        for (i = from_idx; i < to_idx; ++i) {
            if (pvec->cmp(pvec->ptr[i], p) == 0) {
                return i;
            }
        }
    } else {
        for (i = from_idx; i < to_idx; ++i) {
            if (pvec->ptr[i] == p) {
                return i;
            }
        }
    }

    return TT_POS_NULL;
}

void tt_ptrvec_remove(IN tt_ptrvec_t *pvec, IN tt_u32_t idx)
{
    if (idx >= pvec->count) {
        return;
    }

    tt_memmove(&pvec->ptr[idx],
               &pvec->ptr[idx + 1],
               __PV_SIZE(pvec->count - idx - 1));
    --pvec->count;
}

// return removed idx
tt_u32_t tt_ptrvec_remove_equal(IN tt_ptrvec_t *pvec, IN tt_ptr_t *p)
{
    tt_u32_t idx = tt_ptrvec_find(pvec, p);
    if (idx != TT_POS_NULL) {
        tt_ptrvec_remove(pvec, idx);
    }
    return idx;
}

void tt_ptrvec_remove_range(IN tt_ptrvec_t *pvec,
                            IN tt_u32_t from_idx,
                            IN tt_u32_t to_idx)
{
    TT_ASSERT_ALWAYS(from_idx <= to_idx);

    if (from_idx >= pvec->count) {
        return;
    }
    if (to_idx >= pvec->count) {
        to_idx = pvec->count;
    }

    tt_memmove(&pvec->ptr[from_idx],
               &pvec->ptr[to_idx],
               __PV_SIZE(pvec->count - to_idx));
    pvec->count -= (to_idx - from_idx);
}

void tt_ptrvec_trim(IN tt_ptrvec_t *pvec)
{
    if (pvec->count < pvec->capacity) {
        tt_memspg_compress(&pvec->mspg,
                           &pvec->p,
                           &pvec->size,
                           __PV_SIZE(pvec->count));
        pvec->capacity = __PV_NUM(pvec->size);
    }
}
