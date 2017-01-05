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

#include <algorithm/ptr/tt_ptr_queue.h>

#include <memory/tt_memory_alloc.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __F_PTR(f, idx) TT_PTR_INC(tt_ptr_t, (f), sizeof(__q_frame_t))[(idx)]

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef struct
{
    tt_dnode_t node;
    tt_u32_t start;
    tt_u32_t end;
} __q_frame_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static __q_frame_t *__alloc_frame(IN tt_ptrq_t *pq);

static void __free_frame(IN tt_ptrq_t *pq, IN __q_frame_t *frame);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_ptrq_init(IN tt_ptrq_t *pq, IN OPT tt_ptrq_attr_t *attr)
{
    tt_ptrq_attr_t __attr;

    TT_ASSERT(pq != NULL);

    if (attr == NULL) {
        tt_ptrq_attr_default(&__attr);
        attr = &__attr;
    }
    TT_ASSERT(attr->ptr_per_frame != 0);
    TT_ASSERT(!TT_U32_MUL_WOULD_OVFL(sizeof(tt_ptr_t), attr->ptr_per_frame));

    tt_dlist_init(&pq->frame);
    pq->cached_frame = NULL;
    pq->count = 0;
    pq->ptr_per_frame = attr->ptr_per_frame;
}

void tt_ptrq_destroy(IN tt_ptrq_t *pq)
{
    tt_dnode_t *dnode;

    TT_ASSERT(pq != NULL);

    while ((dnode = tt_dlist_pop_head(&pq->frame)) != NULL) {
        tt_free(TT_CONTAINER(dnode, __q_frame_t, node));
    }

    if (pq->cached_frame != NULL) {
        tt_free(pq->cached_frame);
    }
}

void tt_ptrq_attr_default(IN tt_ptrq_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    // overhead of each frame is about 24bytes, so set ptr_per_frame
    // to 32 would reduct overhead rate to 10%
    attr->ptr_per_frame = 32;
}

void tt_ptrq_clear(IN tt_ptrq_t *pq)
{
    tt_dnode_t *dnode;
    while ((dnode = tt_dlist_pop_head(&pq->frame)) != NULL) {
        __free_frame(pq, TT_CONTAINER(dnode, __q_frame_t, node));
    }
}

tt_result_t tt_ptrq_push(IN tt_ptrq_t *pq, IN tt_ptr_t p)
{
    tt_dnode_t *dnode;
    __q_frame_t *frame;

    if (p == NULL) {
        TT_ERROR("can not be null");
        return TT_FAIL;
    }

    dnode = tt_dlist_tail(&pq->frame);
    if (dnode != NULL) {
        frame = TT_CONTAINER(dnode, __q_frame_t, node);
    } else {
        frame = __alloc_frame(pq);
        if (frame == NULL) {
            return TT_FAIL;
        }
        tt_dlist_push_tail(&pq->frame, &frame->node);
    }

    if (frame->end == pq->ptr_per_frame) {
        frame = __alloc_frame(pq);
        if (frame == NULL) {
            return TT_FAIL;
        }
        tt_dlist_push_tail(&pq->frame, &frame->node);
    }
    TT_ASSERT(frame->end < pq->ptr_per_frame);

    __F_PTR(frame, frame->end) = p;
    ++frame->end;
    ++pq->count;

    return TT_SUCCESS;
}

tt_ptr_t tt_ptrq_pop(IN tt_ptrq_t *pq)
{
    tt_dnode_t *dnode;
    __q_frame_t *frame;
    tt_ptr_t p;

    dnode = tt_dlist_head(&pq->frame);
    if (dnode == NULL) {
        return NULL;
    }

    frame = TT_CONTAINER(dnode, __q_frame_t, node);
    TT_ASSERT(frame->start < frame->end);

    p = __F_PTR(frame, frame->start);
    ++frame->start;
    if (frame->start == frame->end) {
        tt_dlist_remove(&pq->frame, &frame->node);
        __free_frame(pq, frame);
    }
    --pq->count;

    return p;
}

tt_ptr_t tt_ptrq_head(IN tt_ptrq_t *pq)
{
    tt_dnode_t *dnode;
    __q_frame_t *frame;

    dnode = tt_dlist_head(&pq->frame);
    if (dnode == NULL) {
        return NULL;
    }

    frame = TT_CONTAINER(dnode, __q_frame_t, node);
    TT_ASSERT(frame->start < frame->end);
    return __F_PTR(frame, frame->start);
}

tt_ptr_t tt_ptrq_tail(IN tt_ptrq_t *pq)
{
    tt_dnode_t *dnode;
    __q_frame_t *frame;

    dnode = tt_dlist_tail(&pq->frame);
    if (dnode == NULL) {
        return NULL;
    }

    frame = TT_CONTAINER(dnode, __q_frame_t, node);
    TT_ASSERT(frame->start < frame->end);
    return __F_PTR(frame, frame->end - 1);
}

void tt_ptrq_iter(IN tt_ptrq_t *pq, OUT tt_pqiter_t *iter)
{
    tt_dnode_t *node;

    iter->pq = pq;

    node = tt_dlist_head(&pq->frame);
    if (node != NULL) {
        iter->frame = TT_CONTAINER(node, __q_frame_t, node);
    } else {
        iter->frame = NULL;
    }

    iter->idx = 0;
}

tt_ptr_t tt_pqiter_next(IN OUT tt_pqiter_t *iter)
{
    __q_frame_t *frame = iter->frame;
    tt_ptr_t p;

    if (frame == NULL) {
        return NULL;
    }

    TT_ASSERT((frame->start + iter->idx) <= frame->end);
    if ((frame->start + iter->idx) == frame->end) {
        if (frame->node.next != NULL) {
            frame = TT_CONTAINER(frame->node.next, __q_frame_t, node);
            iter->frame = frame;
            iter->idx = 0;
        } else {
            return NULL;
        }
    }

    p = __F_PTR(frame, frame->start + iter->idx);
    ++iter->idx;
    return p;
}

__q_frame_t *__alloc_frame(IN tt_ptrq_t *pq)
{
    __q_frame_t *frame;

    if (pq->cached_frame != NULL) {
        frame = pq->cached_frame;
        pq->cached_frame = NULL;
    } else {
        frame = tt_malloc(sizeof(__q_frame_t) +
                          (sizeof(tt_ptr_t) * pq->ptr_per_frame));
        if (frame == NULL) {
            TT_ERROR("no mem for new frame");
            return NULL;
        }
    }

    tt_dnode_init(&frame->node);
    frame->start = 0;
    frame->end = 0;
    return frame;
}

void __free_frame(IN tt_ptrq_t *pq, IN __q_frame_t *frame)
{
    if (pq->cached_frame != NULL) {
        tt_free(frame);
    } else {
        pq->cached_frame = frame;
    }
}
