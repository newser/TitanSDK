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
@file tt_ptr_stack.h
@brief ptr stack
 */

#ifndef __TT_PTR_STACK__
#define __TT_PTR_STACK__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_double_linked_list.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct
{
    tt_u32_t ptr_per_frame;
} tt_ptrstk_attr_t;

typedef struct tt_ptrstk_s
{
    tt_dlist_t frame;
    void *cached_frame;
    tt_u32_t count;
    tt_u32_t ptr_per_frame;
} tt_ptrstk_t;

typedef struct
{
    tt_ptrstk_t *pstk;
    void *frame;
    tt_u32_t idx;
} tt_ptrstk_iter_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern void tt_ptrstk_init(IN tt_ptrstk_t *pstk, IN OPT tt_ptrstk_attr_t *attr);

extern void tt_ptrstk_destroy(IN tt_ptrstk_t *pstk);

extern void tt_ptrstk_attr_default(IN tt_ptrstk_attr_t *attr);

tt_inline tt_u32_t tt_ptrstk_count(IN tt_ptrstk_t *pstk)
{
    return pstk->count;
}

tt_inline tt_bool_t tt_ptrstk_empty(IN tt_ptrstk_t *pstk)
{
    return pstk->count == 0 ? TT_TRUE : TT_FALSE;
}

extern void tt_ptrstk_clear(IN tt_ptrstk_t *pstk);

extern tt_result_t tt_ptrstk_push(IN tt_ptrstk_t *pstk, IN tt_ptr_t p);

extern tt_ptr_t tt_ptrstk_pop(IN tt_ptrstk_t *pstk);

extern tt_ptr_t tt_ptrstk_top(IN tt_ptrstk_t *pstk);

extern void tt_ptrstk_iter(IN tt_ptrstk_t *pstk, OUT tt_ptrstk_iter_t *iter);

extern tt_ptr_t tt_ptrstk_iter_next(IN OUT tt_ptrstk_iter_t *iter);

#endif /* __TT_PTR_STACK__ */
