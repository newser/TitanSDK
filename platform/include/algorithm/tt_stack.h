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
@file tt_stack.h
@brief queue
 */

#ifndef __TT_STACK__
#define __TT_STACK__

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
    tt_u32_t obj_per_frame;
} tt_stack_attr_t;

typedef struct tt_stack_s
{
    tt_dlist_t frame;
    void *cached_frame;
    tt_u32_t count;
    tt_u32_t obj_size;
    tt_u32_t obj_per_frame;
} tt_stack_t;

typedef struct
{
    tt_stack_t *stk;
    void *frame;
    tt_u32_t idx;
} tt_stack_iter_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export void tt_stack_init(IN tt_stack_t *stk, IN tt_u32_t obj_size,
                             IN OPT tt_stack_attr_t *attr);

tt_export void tt_stack_destroy(IN tt_stack_t *stk);

tt_export void tt_stack_attr_default(IN tt_stack_attr_t *attr);

tt_inline tt_u32_t tt_stack_count(IN tt_stack_t *stk)
{
    return stk->count;
}

tt_inline tt_bool_t tt_stack_empty(IN tt_stack_t *stk)
{
    return stk->count == 0 ? TT_TRUE : TT_FALSE;
}

tt_export void tt_stack_clear(IN tt_stack_t *stk);

tt_export tt_result_t tt_stack_push(IN tt_stack_t *stk, IN void *obj);

tt_export tt_result_t tt_stack_pop(IN tt_stack_t *stk, OUT void *obj);

tt_export void *tt_stack_top(IN tt_stack_t *stk);

tt_export void tt_stack_iter(IN tt_stack_t *stk, OUT tt_stack_iter_t *iter);

tt_export void *tt_stack_iter_next(IN OUT tt_stack_iter_t *iter);

#endif /* __TT_STACK__ */
