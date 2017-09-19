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
@file tt_ptr_vector.h
@brief pointer pvector
 */

#ifndef __TT_PTR_VECTOR__
#define __TT_PTR_VECTOR__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_vector.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef tt_vec_t tt_ptrvec_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_inline void tt_ptrvec_init(IN tt_ptrvec_t *pvec,
                              IN tt_cmp_t cmp,
                              IN OPT tt_vec_attr_t *attr)
{
    tt_vec_init(pvec,
                sizeof(tt_ptr_t),
                TT_COND(cmp != NULL, cmp, tt_cmp_ptr),
                attr);
}

tt_inline void tt_ptrvec_destroy(IN tt_ptrvec_t *pvec)
{
    tt_vec_destroy(pvec);
}

tt_export tt_result_t __ptrvec_reserve(IN tt_ptrvec_t *pvec, IN tt_u32_t count);

tt_inline tt_result_t tt_ptrvec_reserve(IN tt_ptrvec_t *pvec, IN tt_u32_t count)
{
    return TT_COND((pvec->count + count) <= pvec->capacity,
                   TT_SUCCESS,
                   __ptrvec_reserve(pvec, count));
}

tt_export tt_result_t tt_ptrvec_push_head(IN tt_ptrvec_t *pvec, IN tt_ptr_t p);

tt_export tt_result_t tt_ptrvec_push_tail(IN tt_ptrvec_t *pvec, IN tt_ptr_t p);

tt_export tt_ptr_t tt_ptrvec_pop_head(IN tt_ptrvec_t *pvec);

tt_export tt_ptr_t tt_ptrvec_pop_tail(IN tt_ptrvec_t *pvec);

tt_export tt_ptr_t tt_ptrvec_head(IN tt_ptrvec_t *pvec);

tt_export tt_ptr_t tt_ptrvec_tail(IN tt_ptrvec_t *pvec);

tt_export tt_result_t tt_ptrvec_insert(IN tt_ptrvec_t *pvec,
                                       IN tt_u32_t idx,
                                       IN tt_ptr_t p);

tt_export tt_result_t tt_ptrvec_move_all(IN tt_ptrvec_t *dst,
                                         IN tt_ptrvec_t *src);

tt_export tt_result_t tt_ptrvec_move_from(IN tt_ptrvec_t *dst,
                                          IN tt_ptrvec_t *src,
                                          IN tt_u32_t from_idx);

// [from_idx, to_idx)
tt_export tt_result_t tt_ptrvec_move_range(IN tt_ptrvec_t *dst,
                                           IN tt_ptrvec_t *src,
                                           IN tt_u32_t from_idx,
                                           IN tt_u32_t to_idx);

tt_inline tt_u32_t tt_ptrvec_capacity(IN tt_ptrvec_t *pvec)
{
    return tt_vec_capacity(pvec);
}

tt_inline tt_u32_t tt_ptrvec_count(IN tt_ptrvec_t *pvec)
{
    return tt_vec_count(pvec);
}

tt_inline tt_bool_t tt_ptrvec_empty(IN tt_ptrvec_t *pvec)
{
    return tt_vec_empty(pvec);
}

tt_inline void tt_ptrvec_clear(IN tt_ptrvec_t *pvec)
{
    tt_vec_clear(pvec);
}

tt_export tt_bool_t tt_ptrvec_comtain(IN tt_ptrvec_t *pvec, IN tt_ptr_t p);

tt_export tt_bool_t tt_ptrvec_comtain_all(IN tt_ptrvec_t *pvec,
                                          IN tt_ptrvec_t *pvec2);

tt_export tt_ptr_t tt_ptrvec_get(IN tt_ptrvec_t *pvec, IN tt_u32_t idx);

tt_export tt_result_t tt_ptrvec_set(IN tt_ptrvec_t *pvec,
                                    IN tt_u32_t idx,
                                    IN tt_ptr_t p);

tt_export tt_u32_t tt_ptrvec_find(IN tt_ptrvec_t *pvec, IN tt_ptr_t p);

tt_export tt_u32_t tt_ptrvec_find_last(IN tt_ptrvec_t *pvec, IN tt_ptr_t p);

tt_export tt_u32_t tt_ptrvec_find_from(IN tt_ptrvec_t *pvec,
                                       IN tt_ptr_t p,
                                       IN tt_u32_t from_idx);

tt_export tt_u32_t tt_ptrvec_find_range(IN tt_ptrvec_t *pvec,
                                        IN tt_ptr_t p,
                                        IN tt_u32_t from_idx,
                                        IN tt_u32_t to_idx);

tt_export void tt_ptrvec_remove(IN tt_ptrvec_t *pvec, IN tt_u32_t idx);

// return removed idx
tt_export tt_u32_t tt_ptrvec_remove_equal(IN tt_ptrvec_t *pvec, IN tt_ptr_t p);

// [from_idx, to_idx)
tt_export void tt_ptrvec_remove_range(IN tt_ptrvec_t *pvec,
                                      IN tt_u32_t from_idx,
                                      IN tt_u32_t to_idx);

tt_export void tt_ptrvec_trim(IN tt_ptrvec_t *pvec);

#endif /* __TT_PTR_VECTOR__ */
