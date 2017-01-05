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

#include <algorithm/tt_queue.h>

#include <memory/tt_memory_alloc.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __F_OBJ(f, q, idx)                                                     \
    TT_PTR_INC(void, (f), sizeof(__q_frame_t) + ((q)->obj_size * (idx)))

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

static __q_frame_t *__alloc_frame(IN tt_queue_t *q);

static void __free_frame(IN tt_queue_t *q, IN __q_frame_t *frame);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_queue_init(IN tt_queue_t *q,
                   IN tt_u32_t obj_size,
                   IN OPT tt_queue_attr_t *attr)
{
    tt_queue_attr_t __attr;

    TT_ASSERT(q != NULL);
    TT_ASSERT(obj_size != 0);

    if (attr == NULL) {
        tt_queue_attr_default(&__attr);
        attr = &__attr;
    }
    TT_ASSERT(attr->obj_per_frame != 0);
    TT_ASSERT(!TT_U32_MUL_WOULD_OVFL(obj_size, attr->obj_per_frame));

    tt_dlist_init(&q->frame);
    q->cached_frame = NULL;
    q->count = 0;
    q->obj_size = obj_size;
    q->obj_per_frame = attr->obj_per_frame;
}

void tt_queue_destroy(IN tt_queue_t *q)
{
    tt_dnode_t *dnode;

    TT_ASSERT(q != NULL);

    while ((dnode = tt_dlist_pop_head(&q->frame)) != NULL) {
        tt_free(TT_CONTAINER(dnode, __q_frame_t, node));
    }

    if (q->cached_frame != NULL) {
        tt_free(q->cached_frame);
    }
}

void tt_queue_attr_default(IN tt_queue_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->obj_per_frame = 16;
}

void tt_queue_clear(IN tt_queue_t *q)
{
    tt_dnode_t *dnode;
    while ((dnode = tt_dlist_pop_head(&q->frame)) != NULL) {
        __free_frame(q, TT_CONTAINER(dnode, __q_frame_t, node));
    }
    q->count = 0;
}

tt_result_t tt_queue_push(IN tt_queue_t *q, IN void *obj)
{
    tt_dnode_t *dnode;
    __q_frame_t *frame;

    TT_ASSERT(obj != NULL);

    dnode = tt_dlist_tail(&q->frame);
    if (dnode != NULL) {
        frame = TT_CONTAINER(dnode, __q_frame_t, node);
    } else {
        frame = __alloc_frame(q);
        if (frame == NULL) {
            return TT_FAIL;
        }
        tt_dlist_push_tail(&q->frame, &frame->node);
    }

    if (frame->end == q->obj_per_frame) {
        frame = __alloc_frame(q);
        if (frame == NULL) {
            return TT_FAIL;
        }
        tt_dlist_push_tail(&q->frame, &frame->node);
    }
    TT_ASSERT(frame->end < q->obj_per_frame);

    tt_memcpy(__F_OBJ(frame, q, frame->end), obj, q->obj_size);
    ++frame->end;
    ++q->count;

    return TT_SUCCESS;
}

tt_result_t tt_queue_pop(IN tt_queue_t *q, OUT void *obj)
{
    tt_dnode_t *dnode;
    __q_frame_t *frame;

    dnode = tt_dlist_head(&q->frame);
    if (dnode == NULL) {
        return TT_FAIL;
    }

    frame = TT_CONTAINER(dnode, __q_frame_t, node);
    TT_ASSERT(frame->start < frame->end);

    tt_memcpy(obj, __F_OBJ(frame, q, frame->start), q->obj_size);
    ++frame->start;
    if (frame->start == frame->end) {
        tt_dlist_remove(&q->frame, &frame->node);
        __free_frame(q, frame);
    }
    --q->count;

    return TT_SUCCESS;
}

void *tt_queue_head(IN tt_queue_t *q)
{
    tt_dnode_t *dnode;
    __q_frame_t *frame;

    dnode = tt_dlist_head(&q->frame);
    if (dnode == NULL) {
        return NULL;
    }

    frame = TT_CONTAINER(dnode, __q_frame_t, node);
    TT_ASSERT(frame->start < frame->end);
    return __F_OBJ(frame, q, frame->start);
}

void *tt_queue_tail(IN tt_queue_t *q)
{
    tt_dnode_t *dnode;
    __q_frame_t *frame;

    dnode = tt_dlist_tail(&q->frame);
    if (dnode == NULL) {
        return NULL;
    }

    frame = TT_CONTAINER(dnode, __q_frame_t, node);
    TT_ASSERT(frame->start < frame->end);
    return __F_OBJ(frame, q, frame->end - 1);
}

void tt_queue_iter(IN tt_queue_t *q, OUT tt_qiter_t *iter)
{
    tt_dnode_t *node;

    iter->q = q;

    node = tt_dlist_head(&q->frame);
    if (node != NULL) {
        iter->frame = TT_CONTAINER(node, __q_frame_t, node);
    } else {
        iter->frame = NULL;
    }

    iter->idx = 0;
}

void *tt_qiter_next(IN OUT tt_qiter_t *iter)
{
    __q_frame_t *frame = iter->frame;
    void *obj;

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

    obj = __F_OBJ(frame, iter->q, frame->start + iter->idx);
    ++iter->idx;
    return obj;
}

__q_frame_t *__alloc_frame(IN tt_queue_t *q)
{
    __q_frame_t *frame;

    if (q->cached_frame != NULL) {
        frame = q->cached_frame;
        q->cached_frame = NULL;
    } else {
        frame =
            tt_malloc(sizeof(__q_frame_t) + (q->obj_size * q->obj_per_frame));
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

void __free_frame(IN tt_queue_t *q, IN __q_frame_t *frame)
{
    if (q->cached_frame != NULL) {
        tt_free(frame);
    } else {
        q->cached_frame = frame;
    }
}
