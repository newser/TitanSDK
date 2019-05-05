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

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/ptr/tt_ptr_stack.h>

#include <memory/tt_memory_alloc.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __F_PTR(f, idx) TT_PTR_INC(tt_ptr_t, (f), sizeof(__frame_t))[(idx)]

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

static __frame_t *__alloc_frame(IN tt_ptrstk_t *pstk);

static void __free_frame(IN tt_ptrstk_t *pstk, IN __frame_t *frame);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_ptrstk_init(IN tt_ptrstk_t *pstk, IN OPT tt_ptrstk_attr_t *attr)
{
    tt_ptrstk_attr_t __attr;

    TT_ASSERT(pstk != NULL);

    if (attr == NULL) {
        tt_ptrstk_attr_default(&__attr);
        attr = &__attr;
    }
    TT_ASSERT(attr->ptr_per_frame != 0);
    TT_ASSERT(!TT_U32_MUL_WOULD_OVFL(sizeof(tt_ptr_t), attr->ptr_per_frame));

    tt_dlist_init(&pstk->frame);
    pstk->cached_frame = NULL;
    pstk->count = 0;
    pstk->ptr_per_frame = attr->ptr_per_frame;
}

void tt_ptrstk_destroy(IN tt_ptrstk_t *pstk)
{
    tt_dnode_t *dnode;

    TT_ASSERT(pstk != NULL);

    while ((dnode = tt_dlist_pop_head(&pstk->frame)) != NULL) {
        tt_free(TT_CONTAINER(dnode, __frame_t, node));
    }

    if (pstk->cached_frame != NULL) { tt_free(pstk->cached_frame); }
}

void tt_ptrstk_attr_default(IN tt_ptrstk_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    // overhead of each frame is about 24bytes, so set ptr_per_frame
    // to 32 would reduct overhead rate to 10%
    attr->ptr_per_frame = 32;
}

void tt_ptrstk_clear(IN tt_ptrstk_t *pstk)
{
    tt_dnode_t *dnode;
    while ((dnode = tt_dlist_pop_head(&pstk->frame)) != NULL) {
        __free_frame(pstk, TT_CONTAINER(dnode, __frame_t, node));
    }
}

tt_result_t tt_ptrstk_push(IN tt_ptrstk_t *pstk, IN tt_ptr_t p)
{
    tt_dnode_t *dnode;
    __frame_t *frame;

    if (p == NULL) {
        TT_ERROR("can not be null");
        return TT_FAIL;
    }

    dnode = tt_dlist_tail(&pstk->frame);
    if (dnode != NULL) {
        frame = TT_CONTAINER(dnode, __frame_t, node);
    } else {
        frame = __alloc_frame(pstk);
        if (frame == NULL) { return TT_FAIL; }
        tt_dlist_push_tail(&pstk->frame, &frame->node);
    }

    if (frame->top == pstk->ptr_per_frame) {
        frame = __alloc_frame(pstk);
        if (frame == NULL) { return TT_FAIL; }
        tt_dlist_push_tail(&pstk->frame, &frame->node);
    }
    TT_ASSERT(frame->top < pstk->ptr_per_frame);

    __F_PTR(frame, frame->top) = p;
    ++frame->top;
    ++pstk->count;

    return TT_SUCCESS;
}

tt_ptr_t tt_ptrstk_pop(IN tt_ptrstk_t *pstk)
{
    tt_dnode_t *dnode;
    __frame_t *frame;
    tt_ptr_t p;

    dnode = tt_dlist_tail(&pstk->frame);
    if (dnode == NULL) { return NULL; }

    frame = TT_CONTAINER(dnode, __frame_t, node);
    TT_ASSERT(frame->top > 0);

    --frame->top;
    p = __F_PTR(frame, frame->top);
    if (frame->top == 0) {
        tt_dlist_remove(&pstk->frame, &frame->node);
        __free_frame(pstk, frame);
    }
    --pstk->count;

    return p;
}

tt_ptr_t tt_ptrstk_top(IN tt_ptrstk_t *pstk)
{
    tt_dnode_t *dnode;
    __frame_t *frame;

    dnode = tt_dlist_tail(&pstk->frame);
    if (dnode == NULL) { return NULL; }

    frame = TT_CONTAINER(dnode, __frame_t, node);
    TT_ASSERT(frame->top > 0);
    return __F_PTR(frame, frame->top - 1);
}

void tt_ptrstk_iter(IN tt_ptrstk_t *pstk, OUT tt_ptrstk_iter_t *iter)
{
    tt_dnode_t *node;

    iter->pstk = pstk;

    node = tt_dlist_head(&pstk->frame);
    if (node != NULL) {
        iter->frame = TT_CONTAINER(node, __frame_t, node);
    } else {
        iter->frame = NULL;
    }

    iter->idx = 0;
}

tt_ptr_t tt_ptrstk_iter_next(IN OUT tt_ptrstk_iter_t *iter)
{
    __frame_t *frame = iter->frame;
    tt_ptr_t p;

    if (frame == NULL) { return NULL; }

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

    p = __F_PTR(frame, iter->idx);
    ++iter->idx;
    return p;
}

__frame_t *__alloc_frame(IN tt_ptrstk_t *pstk)
{
    __frame_t *frame;

    if (pstk->cached_frame != NULL) {
        frame = pstk->cached_frame;
        pstk->cached_frame = NULL;
    } else {
        frame = tt_malloc(sizeof(__frame_t) +
                          (sizeof(tt_ptr_t) * pstk->ptr_per_frame));
        if (frame == NULL) {
            TT_ERROR("no mem for new frame");
            return NULL;
        }
    }

    tt_dnode_init(&frame->node);
    frame->top = 0;
    return frame;
}

void __free_frame(IN tt_ptrstk_t *pstk, IN __frame_t *frame)
{
    if (pstk->cached_frame != NULL) {
        tt_free(frame);
    } else {
        pstk->cached_frame = frame;
    }
}
