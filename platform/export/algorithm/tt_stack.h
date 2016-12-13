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
@file tt_stack.h
@brief stack

this file specifies stack APIs
*/

#ifndef __TT_STACK__
#define __TT_STACK__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_list.h>
#include <misc/tt_util.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef void (*tt_stack_obj_destroy_t)(IN void *obj);

typedef struct tt_stack_frame_s
{
    tt_u32_t idx;

    tt_u8_t *data;
    tt_u32_t max_num;
    tt_u32_t top;
    // top point to an available slot

    tt_lnode_t node;
} tt_stack_frame_t;

typedef struct
{
    tt_stack_obj_destroy_t obj_destroy;

    tt_bool_t destroy_obj_when_push_fail : 1;
} tt_stack_attr_t;

typedef struct tt_stack_s
{
    tt_u32_t obj_size;
    tt_u32_t obj_size_aligned;

    tt_u32_t frame_size;
    tt_list_t frame_list;
    tt_stack_frame_t *current_frame;

    tt_stack_attr_t attr;
} tt_stack_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

// set obj_num to 0 to use default value
extern tt_result_t tt_stack_create(IN tt_stack_t *stack,
                                   IN tt_u32_t obj_size,
                                   IN OPT tt_u32_t obj_num,
                                   IN OPT tt_stack_attr_t *attr);

extern tt_result_t tt_stack_destroy(IN tt_stack_t *stack);

extern void tt_stack_attr_default(IN tt_stack_attr_t *attr);

// - no data type check, so the 3rd param is for checking that
// - only return fail when no memory, but this rarely happen
// - the obj is "copied" into stack
extern tt_result_t tt_stack_push(IN tt_stack_t *stack,
                                 IN tt_u8_t *obj,
                                 IN tt_u32_t obj_size);

extern tt_result_t tt_stack_pop(IN tt_stack_t *stack,
                                OUT tt_u8_t *obj,
                                IN tt_u32_t obj_size);

extern tt_u8_t *tt_stack_top(IN tt_stack_t *stack);

extern void tt_stack_clear(IN tt_stack_t *stack);

// ========================================
// pointer stack
// ========================================

typedef tt_stack_t tt_ptrstack_t;

tt_inline tt_result_t tt_ptrstack_create(IN tt_ptrstack_t *stack,
                                         IN OPT tt_u32_t ptr_num,
                                         IN OPT tt_stack_attr_t *attr)
{
    return tt_stack_create(stack, sizeof(tt_ptr_t), ptr_num, attr);
}

tt_inline tt_result_t tt_ptrstack_destroy(IN tt_ptrstack_t *stack)
{
    return tt_stack_destroy(stack);
}

tt_inline tt_result_t tt_ptrstack_push(IN tt_stack_t *stack, IN void *obj)
{
    return tt_stack_push(stack, (void *)&obj, sizeof(void *));
}

tt_inline void *tt_ptrstack_pop(IN tt_stack_t *stack)
{
    void *ptr = NULL;
    tt_stack_pop(stack, (void *)&ptr, sizeof(void *));
    return ptr;
}

tt_inline void *tt_ptrstack_top(IN tt_stack_t *stack)
{
    void **p = (void **)tt_stack_top(stack);
    return TT_COND(p != NULL, *p, NULL);
}

tt_inline void tt_ptrstack_clear(IN tt_ptrstack_t *stack)
{
    tt_stack_clear(stack);
}

// ========================================
// other stack ...
// ========================================

#endif /* __TT_STACK__ */
