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
@file tt_single_linked_list.h
@brief simple sl
this file defines apis of simple sl data structure.

 - push_head/push_tail/pop_head/pop_tail
 - head/tail
 - insert_before/insert_after
 - empty
 - clear
 - remove(sn)
 - swap
*/

#ifndef __TT_SINGLE_LINKED_LIST__
#define __TT_SINGLE_LINKED_LIST__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <misc/tt_assert.h>
#include <misc/tt_util.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct tt_snode_s
{
    struct tt_snode_s *next;
} tt_snode_t;

typedef struct
{
    tt_snode_t *head;
} tt_slist_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_inline void tt_snode_init(IN tt_snode_t *sn)
{
    sn->next = NULL;
}

tt_inline void tt_slist_init(IN tt_slist_t *sl)
{
    sl->head = NULL;
}

tt_inline tt_snode_t *tt_slist_head(IN tt_slist_t *sl)
{
    return sl->head;
}

tt_inline tt_snode_t *tt_slist_tail(IN tt_slist_t *sl)
{
    tt_snode_t *sn = sl->head;
    if (sn != NULL) {
        while (sn->next != NULL) {
            sn = sn->next;
        }
    }
    return sn;
}

tt_inline tt_u32_t tt_slist_count(IN tt_slist_t *sl)
{
    tt_u32_t count = 0;
    tt_snode_t *sn = sl->head;
    while (sn != NULL) {
        ++count;
        sn = sn->next;
    }
    return count;
}

tt_inline void tt_slist_clear(IN tt_slist_t *sl)
{
    tt_snode_t *sn = sl->head;
    while (sn != NULL) {
        tt_snode_t *next = sn->next;
        sn->next = NULL;
        sn = next;
    }
    sl->head = NULL;
}

tt_inline tt_bool_t tt_slist_empty(IN tt_slist_t *sl)
{
    return sl->head == NULL ? TT_TRUE : TT_FALSE;
}

tt_inline void tt_slist_push_head(IN tt_slist_t *sl, IN tt_snode_t *sn)
{
    TT_ASSERT(sn->next == NULL);

    sn->next = sl->head;
    sl->head = sn;
}

tt_inline void tt_slist_push_tail(IN tt_slist_t *sl, IN tt_snode_t *sn)
{
    tt_snode_t *tail = tt_slist_tail(sl);
    TT_ASSERT(sn->next == NULL);
    if (tail != NULL) {
        tail->next = sn;
    } else {
        sl->head = sn;
    }
}

tt_inline tt_snode_t *tt_slist_pop_head(IN tt_slist_t *sl)
{
    tt_snode_t *head = sl->head;
    if (head != NULL) {
        sl->head = head->next;
        head->next = NULL;
    }
    return head;
}

tt_inline tt_snode_t *tt_slist_pop_tail(IN tt_slist_t *sl)
{
    tt_snode_t *sn = sl->head;
    if (sn == NULL) {
        return NULL;
    } else if (sn->next == NULL) {
        sl->head = NULL;
        return sn;
    } else {
        tt_snode_t *prev;
        do {
            prev = sn;
            sn = sn->next;
        } while (sn->next != NULL);
        prev->next = NULL;

        sn->next = NULL;
        return sn;
    }
}

tt_inline void tt_slist_insert_after(IN tt_snode_t *pos, IN tt_snode_t *sn)
{
    TT_ASSERT(sn->next == NULL);

    sn->next = pos->next;
    pos->next = sn;
}

tt_inline tt_snode_t *tt_slist_remove(IN tt_slist_t *sl, IN tt_snode_t *sn)
{
    tt_snode_t *node = sl->head;
    tt_snode_t *next = sn->next;
    if (node == sn) {
        sl->head = next;
    } else {
        while ((node != NULL) && (node->next != sn)) {
            node = node->next;
        }
        if (node != NULL) {
            node->next = next;
        }
    }
    sn->next = NULL;
    return next;
}

tt_inline tt_snode_t *tt_slist_fast_remove(IN tt_slist_t *sl,
                                           IN tt_snode_t *prev,
                                           IN tt_snode_t *sn)
{
    tt_snode_t *next = sn->next;
    if (prev != NULL) {
        TT_ASSERT(prev->next == sn);
        prev->next = next;
    } else {
        TT_ASSERT(sl->head == sn);
        sl->head = next;
    }
    sn->next = NULL;
    return next;
}

tt_inline void tt_slist_move(IN tt_slist_t *dst, IN tt_slist_t *src)
{
    tt_snode_t *tail = tt_slist_tail(dst);
    if (tail != NULL) {
        tail->next = src->head;
    } else {
        dst->head = src->head;
    }
    src->head = NULL;
}

// returns how many nodes are moved to dst
tt_inline tt_u32_t tt_slist_move_count(IN tt_slist_t *dst,
                                       IN tt_slist_t *src,
                                       IN tt_u32_t count)
{
    tt_snode_t *prev, *node;
    tt_u32_t n = 0;

    if (dst == src) {
        return 0;
    }

    prev = NULL;
    node = src->head;
    while ((node != NULL) && (n < count)) {
        prev = node;
        node = node->next;
        ++n;
    }

    if (prev != NULL) {
        if (dst->head != NULL) {
            tt_slist_tail(dst)->next = src->head;
        } else {
            dst->head = src->head;
        }
        src->head = prev->next;
        prev->next = NULL;
    }

    return n;
}

tt_inline void tt_slist_swap(IN tt_slist_t *l, IN tt_slist_t *r)
{
    TT_SWAP(tt_snode_t *, l->head, r->head);
}

#endif /* __TT_SINGLE_LINKED_LIST__ */
