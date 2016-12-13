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

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_stack.h>

#include <memory/tt_memory_alloc.h>
#include <memory/tt_memory_pool.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __MIN_OBJ_PER_FRAME 16

#if 1
#define TT_ASSERT_STACK TT_ASSERT
#else
#define TT_ASSERT_STACK(...)
#endif

#define __STACK_OBJ(stack, frame, obj_idx)                                     \
    TT_PTR_INC(tt_u8_t, (frame)->data, (obj_idx) * (stack)->obj_size_aligned)

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_stack_frame_t *__stack_expand(IN tt_stack_t *stack);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_stack_create(IN tt_stack_t *stack,
                            IN tt_u32_t obj_size,
                            IN OPT tt_u32_t obj_num,
                            IN OPT tt_stack_attr_t *attr)
{
    tt_result_t result = TT_FAIL;

    TT_ASSERT(stack != NULL);
    TT_ASSERT(obj_size != 0);

    // caculate obj sizes
    stack->obj_size = obj_size;

    TT_U32_ALIGN_INC_CPU(obj_size);
    stack->obj_size_aligned = obj_size;
    TT_ASSERT(stack->obj_size_aligned > 0);
    TT_ASSERT(stack->obj_size_aligned >= stack->obj_size);

    // calculate frame size
    if (obj_num == 0) {
        obj_num = __MIN_OBJ_PER_FRAME;
    }
    stack->frame_size = sizeof(tt_stack_frame_t);
    stack->frame_size += stack->obj_size_aligned * obj_num;
    stack->frame_size = tt_mem_size(stack->frame_size);

    tt_list_init(&stack->frame_list);
    stack->current_frame = NULL;

    if (attr != NULL) {
        tt_memcpy(&stack->attr, attr, sizeof(tt_stack_attr_t));
    } else {
        tt_stack_attr_default(&stack->attr);
    }

    return TT_SUCCESS;
}

void tt_stack_attr_default(IN tt_stack_attr_t *attr)
{
    TT_ASSERT(attr != 0);

    attr->obj_destroy = NULL;

    attr->destroy_obj_when_push_fail = TT_FALSE;
}

tt_result_t tt_stack_destroy(IN tt_stack_t *stack)
{
    tt_lnode_t *node = NULL;
    tt_result_t result = TT_SUCCESS;

    TT_ASSERT(stack != NULL);

    // destroy each frame, begin with the last frame
    while ((node = tt_list_poptail(&stack->frame_list)) != NULL) {
        tt_stack_frame_t *frame = TT_CONTAINER(node, tt_stack_frame_t, node);
        tt_stack_obj_destroy_t obj_destroy = stack->attr.obj_destroy;

        // destroy each object, begin with the top object
        TT_ASSERT_STACK(frame->data != NULL);
        if (obj_destroy != NULL) {
            tt_u8_t *obj = TT_PTR_INC(tt_u8_t,
                                      frame->data,
                                      frame->top * stack->obj_size_aligned);
            tt_u32_t idx;

            for (idx = 0; idx < frame->top; ++idx) {
                obj = TT_PTR_DEC(tt_u8_t, obj, stack->obj_size_aligned);
                obj_destroy(obj);
            }
            TT_ASSERT_STACK(obj == frame->data);
        }

        // free frame
        tt_mem_free(frame);
    }

    return result;
}

tt_result_t tt_stack_push(IN tt_stack_t *stack,
                          IN tt_u8_t *obj,
                          IN tt_u32_t obj_size)
{
    tt_stack_frame_t *frame = NULL;
    tt_result_t result = TT_SUCCESS;

    TT_ASSERT(stack != NULL);
    TT_ASSERT(obj != NULL);
    TT_ASSERT(obj_size == stack->obj_size);

    frame = stack->current_frame;

    if (frame == NULL) {
        TT_ASSERT_STACK(tt_list_empty(&stack->frame_list));

        frame = __stack_expand(stack);
    } else if (frame->top == frame->max_num) {
        if (frame->node.next != NULL) {
            // next frame is available

            frame = TT_CONTAINER(frame->node.next, tt_stack_frame_t, node);
            stack->current_frame = frame;

            TT_ASSERT_STACK(frame->data != NULL);
            TT_ASSERT_STACK(frame->top == 0);
        } else {
            frame = __stack_expand(stack);
        }
    }

    if (frame != NULL) {
        TT_ASSERT_STACK(frame == stack->current_frame);
        TT_ASSERT_STACK(frame->top < frame->max_num);

        // save on top
        tt_memcpy(__STACK_OBJ(stack, frame, frame->top), obj, obj_size);
        ++frame->top;
    } else if ((stack->attr.destroy_obj_when_push_fail) &&
               (stack->attr.obj_destroy != NULL)) {
        TT_WARN("stack push fail, obj is destroyed");
        stack->attr.obj_destroy(obj);
    } else {
        TT_ERROR("no memory to store new object");
        result = TT_NO_RESOURCE;
    }

    return result;
}

tt_result_t tt_stack_pop(IN tt_stack_t *stack,
                         OUT tt_u8_t *obj,
                         IN tt_u32_t obj_size)
{
    void *popped = NULL;
    tt_stack_frame_t *frame = NULL;

    TT_ASSERT(stack != NULL);
    TT_ASSERT(obj != NULL);
    TT_ASSERT(obj_size == stack->obj_size);

    frame = stack->current_frame;
    if (frame != NULL) {
        if (frame->top > 0) {
            --frame->top;
            popped = __STACK_OBJ(stack, frame, frame->top);
        } else if (&frame->node != tt_list_head(&stack->frame_list)) {
            // pop from previous frame

            TT_ASSERT_STACK(frame->node.prev != NULL);
            frame = TT_CONTAINER(frame->node.prev, tt_stack_frame_t, node);
            TT_ASSERT_STACK(frame->data != NULL);
            TT_ASSERT_STACK(frame->top == frame->max_num);
            stack->current_frame = frame;

            --frame->top;
            popped = __STACK_OBJ(stack, frame, frame->top);

            // check if need shrink
            if ((tt_list_count(&stack->frame_list) > 2) &&
                (frame->idx < (tt_list_count(&stack->frame_list) - 2))) {
                tt_stack_frame_t *tail_frame =
                    TT_CONTAINER(tt_list_poptail(&stack->frame_list),
                                 tt_stack_frame_t,
                                 node);
                TT_ASSERT_STACK(tail_frame->top == 0);
                TT_ASSERT_STACK(tail_frame->idx == (frame->idx + 2));

                // free the tail frame
                tt_mem_free(tail_frame);
            }
        }
        // else the stack has only an emtpy frame
    } else {
        // this is merely an optimization
        __stack_expand(stack);
    }

    if (popped) {
        tt_memcpy(obj, popped, obj_size);
        return TT_SUCCESS;
    } else {
        return TT_NO_RESOURCE;
    }
}

tt_u8_t *tt_stack_top(IN tt_stack_t *stack)
{
    void *top = NULL;
    tt_stack_frame_t *frame = NULL;

    TT_ASSERT(stack != NULL);

    frame = stack->current_frame;
    if (frame != NULL) {
        if (frame->top > 0) {
            top = __STACK_OBJ(stack, frame, (frame->top - 1));
        } else if (&frame->node != tt_list_head(&stack->frame_list)) {
            // pop from previous frame

            TT_ASSERT_STACK(frame->node.prev != NULL);
            frame = TT_CONTAINER(frame->node.prev, tt_stack_frame_t, node);
            TT_ASSERT_STACK(frame->data != NULL);
            TT_ASSERT_STACK(frame->top == frame->max_num);
            stack->current_frame = frame;

            top = __STACK_OBJ(stack, frame, (frame->top - 1));

            // check if need shrink
            if ((tt_list_count(&stack->frame_list) > 2) &&
                (frame->idx < (tt_list_count(&stack->frame_list) - 2))) {
                tt_stack_frame_t *tail_frame =
                    TT_CONTAINER(tt_list_poptail(&stack->frame_list),
                                 tt_stack_frame_t,
                                 node);
                TT_ASSERT_STACK(tail_frame->top == 0);
                TT_ASSERT_STACK(tail_frame->idx == (frame->idx + 2));

                // free the tail frame
                tt_mem_free(tail_frame);
            }
        }
        // else the stack has only an emtpy frame
    } else {
        // this is merely an optimization
        __stack_expand(stack);
    }

    return top;
}

void tt_stack_clear(IN tt_stack_t *stack)
{
    tt_list_t *frame_list = &stack->frame_list;
    tt_lnode_t *node;
    tt_stack_frame_t *frame;

    if (tt_list_empty(frame_list)) {
        TT_ASSERT(stack->current_frame == NULL);
        return;
    }

    while (tt_list_count(frame_list) > 1) {
        node = tt_list_poptail(frame_list);
        tt_mem_free(TT_CONTAINER(node, tt_stack_frame_t, node));
    }

    node = tt_list_head(frame_list);
    frame = TT_CONTAINER(node, tt_stack_frame_t, node);
    frame->top = 0;

    stack->current_frame = frame;
}

tt_stack_frame_t *__stack_expand(IN tt_stack_t *stack)
{
    tt_stack_frame_t *frame =
        (tt_stack_frame_t *)tt_mem_alloc(stack->frame_size);
    if (frame != NULL) {
        tt_u32_t frame_st_size = sizeof(tt_stack_frame_t);

        // get the beginning structure size aligned
        TT_U32_ALIGN_INC_CPU(frame_st_size);

        // init frame;
        frame->idx = tt_list_count(&stack->frame_list);

        frame->data = TT_PTR_INC(tt_u8_t, frame, frame_st_size);
        frame->max_num = (stack->frame_size - frame_st_size);
        frame->max_num /= stack->obj_size_aligned;
        frame->top = 0;

        tt_lnode_init(&frame->node);

        // add to stack
        TT_ASSERT_STACK(
            (stack->current_frame == NULL) ||
            (&stack->current_frame->node == tt_list_tail(&stack->frame_list)));
        tt_list_addtail(&stack->frame_list, &frame->node);
        stack->current_frame = frame;
    }

    return frame;
}
