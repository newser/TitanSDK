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

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __F_OBJ(f, s, idx)                                                     \
    TT_PTR_INC(void, (f), sizeof(__frame_t) + ((s)->obj_size * (idx)))

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef struct
{
    tt_dnode_t node;
    tt_u32_t top;
} __frame_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static __frame_t *__alloc_frame(IN tt_stack_t *stk);

static void __free_frame(IN tt_stack_t *stk, IN __frame_t *frame);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_stack_init(IN tt_stack_t *stk,
                   IN tt_u32_t obj_size,
                   IN OPT tt_stack_attr_t *attr)
{
    tt_stack_attr_t __attr;

    TT_ASSERT(stk != NULL);
    TT_ASSERT(obj_size != 0);

    if (attr == NULL) {
        tt_stack_attr_default(&__attr);
        attr = &__attr;
    }
    TT_ASSERT(attr->obj_per_frame != 0);
    TT_ASSERT(!TT_U32_MUL_WOULD_OVFL(obj_size, attr->obj_per_frame));

    tt_dlist_init(&stk->frame);
    stk->cached_frame = NULL;
    stk->count = 0;
    stk->obj_size = obj_size;
    stk->obj_per_frame = attr->obj_per_frame;
}

void tt_stack_destroy(IN tt_stack_t *stk)
{
    tt_dnode_t *dnode;

    TT_ASSERT(stk != NULL);

    while ((dnode = tt_dlist_pop_head(&stk->frame)) != NULL) {
        tt_free(TT_CONTAINER(dnode, __frame_t, node));
    }

    if (stk->cached_frame != NULL) {
        tt_free(stk->cached_frame);
    }
}

void tt_stack_attr_default(IN tt_stack_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->obj_per_frame = 16;
}

void tt_stack_clear(IN tt_stack_t *stk)
{
    tt_dnode_t *dnode;
    while ((dnode = tt_dlist_pop_head(&stk->frame)) != NULL) {
        __free_frame(stk, TT_CONTAINER(dnode, __frame_t, node));
    }
    stk->count = 0;
}

tt_result_t tt_stack_push(IN tt_stack_t *stk, IN void *obj)
{
    tt_dnode_t *dnode;
    __frame_t *frame;

    TT_ASSERT(obj != NULL);

    dnode = tt_dlist_tail(&stk->frame);
    if (dnode != NULL) {
        frame = TT_CONTAINER(dnode, __frame_t, node);
    } else {
        frame = __alloc_frame(stk);
        if (frame == NULL) {
            return TT_FAIL;
        }
        tt_dlist_push_tail(&stk->frame, &frame->node);
    }

    if (frame->top == stk->obj_per_frame) {
        frame = __alloc_frame(stk);
        if (frame == NULL) {
            return TT_FAIL;
        }
        tt_dlist_push_tail(&stk->frame, &frame->node);
    }
    TT_ASSERT(frame->top < stk->obj_per_frame);

    tt_memcpy(__F_OBJ(frame, stk, frame->top), obj, stk->obj_size);
    ++frame->top;
    ++stk->count;

    return TT_SUCCESS;
}

tt_result_t tt_stack_pop(IN tt_stack_t *stk, OUT void *obj)
{
    tt_dnode_t *dnode;
    __frame_t *frame;

    dnode = tt_dlist_tail(&stk->frame);
    if (dnode == NULL) {
        return TT_FAIL;
    }

    frame = TT_CONTAINER(dnode, __frame_t, node);
    TT_ASSERT(frame->top > 0);

    --frame->top;
    tt_memcpy(obj, __F_OBJ(frame, stk, frame->top), stk->obj_size);
    if (frame->top == 0) {
        tt_dlist_remove(&stk->frame, &frame->node);
        __free_frame(stk, frame);
    }
    --stk->count;

    return TT_SUCCESS;
}

void *tt_stack_top(IN tt_stack_t *stk)
{
    tt_dnode_t *dnode;
    __frame_t *frame;

    dnode = tt_dlist_tail(&stk->frame);
    if (dnode == NULL) {
        return NULL;
    }

    frame = TT_CONTAINER(dnode, __frame_t, node);
    TT_ASSERT(frame->top > 0);
    return __F_OBJ(frame, stk, frame->top - 1);
}

void tt_stack_iter(IN tt_stack_t *stk, OUT tt_stack_iter_t *iter)
{
    tt_dnode_t *node;

    iter->stk = stk;

    node = tt_dlist_head(&stk->frame);
    if (node != NULL) {
        iter->frame = TT_CONTAINER(node, __frame_t, node);
    } else {
        iter->frame = NULL;
    }

    iter->idx = 0;
}

void *tt_stack_iter_next(IN OUT tt_stack_iter_t *iter)
{
    __frame_t *frame = iter->frame;
    void *obj;

    if (frame == NULL) {
        return NULL;
    }

    TT_ASSERT(iter->idx <= frame->top);
    if (iter->idx == frame->top) {
        if (frame->node.next != NULL) {
            frame = TT_CONTAINER(frame->node.next, __frame_t, node);
            iter->frame = frame;
            iter->idx = 0;
        } else {
            return NULL;
        }
    }

    obj = __F_OBJ(frame, iter->stk, iter->idx);
    ++iter->idx;
    return obj;
}

__frame_t *__alloc_frame(IN tt_stack_t *stk)
{
    __frame_t *frame;

    if (stk->cached_frame != NULL) {
        frame = stk->cached_frame;
        stk->cached_frame = NULL;
    } else {
        frame =
            tt_malloc(sizeof(__frame_t) + (stk->obj_size * stk->obj_per_frame));
        if (frame == NULL) {
            TT_ERROR("no mem for new frame");
            return NULL;
        }
    }

    tt_dnode_init(&frame->node);
    frame->top = 0;
    return frame;
}

void __free_frame(IN tt_stack_t *stk, IN __frame_t *frame)
{
    if (stk->cached_frame != NULL) {
        tt_free(frame);
    } else {
        stk->cached_frame = frame;
    }
}
