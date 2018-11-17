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
@file tt_double_linked_list.h
@brief doulbe linked dl
this file defines apis of doulbe linked dl data structure.

 - push_tail/push_head/pop_tail/pop_head
 - head/tail
 - insert_before/insert_after
 - move
 - empty
 - clear
 - remove(dn)

*/

#ifndef __TT_DOUBLE_LINKED_LIST__
#define __TT_DOUBLE_LINKED_LIST__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <misc/tt_util.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_ASSERT_DL TT_ASSERT

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct tt_dnode_s
{
    struct tt_dnode_s *prev;
    struct tt_dnode_s *next;
} tt_dnode_t;

typedef struct tt_dlist_s
{
    tt_dnode_t *head;
    tt_dnode_t *tail;
} tt_dlist_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_inline void tt_dnode_init(IN tt_dnode_t *dn)
{
    dn->prev = NULL;
    dn->next = NULL;
}

tt_inline tt_bool_t tt_dnode_in_dlist(IN tt_dnode_t *dn)
{
    return TT_BOOL((dn->prev != NULL) || (dn->next != NULL));
}

tt_inline void tt_dlist_init(IN tt_dlist_t *dl)
{
    dl->head = NULL;
    dl->tail = NULL;
}

tt_inline tt_u32_t tt_dlist_count(IN tt_dlist_t *dl)
{
    tt_u32_t count = 0;
    tt_dnode_t *dn = dl->head;
    while (dn != NULL) {
        ++count;
        dn = dn->next;
    }
    return count;
}

tt_inline tt_bool_t tt_dlist_empty(IN tt_dlist_t *dl)
{
    return dl->head == NULL ? TT_TRUE : TT_FALSE;
}

tt_inline tt_dnode_t *tt_dlist_head(IN tt_dlist_t *dl)
{
    return dl->head;
}

tt_inline tt_dnode_t *tt_dlist_tail(IN tt_dlist_t *dl)
{
    return dl->tail;
}

tt_inline void tt_dlist_push_head(IN tt_dlist_t *dl, IN tt_dnode_t *dn)
{
    TT_ASSERT((dn->next == NULL) && (dn->prev == NULL));
    if (dl->head != NULL) {
        dl->head->prev = dn;
        dn->next = dl->head;
    } else {
        dl->tail = dn;
    }
    dl->head = dn;
}

tt_inline void tt_dlist_push_tail(IN tt_dlist_t *dl, IN tt_dnode_t *dn)
{
    TT_ASSERT((dn->next == NULL) && (dn->prev == NULL));
    if (dl->tail != NULL) {
        dl->tail->next = dn;
        dn->prev = dl->tail;
    } else {
        dl->head = dn;
    }
    dl->tail = dn;
}

tt_inline tt_dnode_t *tt_dlist_pop_head(IN tt_dlist_t *dl)
{
    tt_dnode_t *head = dl->head;
    if (head != NULL) {
        dl->head = head->next;
        if (head->next != NULL) {
            head->next->prev = NULL;
        } else {
            dl->tail = NULL;
        }
        head->next = NULL;
        head->prev = NULL;
    }
    return head;
}

tt_inline tt_dnode_t *tt_dlist_pop_tail(IN tt_dlist_t *dl)
{
    tt_dnode_t *tail = dl->tail;
    if (tail != NULL) {
        if (tail->prev != NULL) {
            tail->prev->next = NULL;
        } else {
            dl->head = NULL;
        }
        dl->tail = tail->prev;
        tail->next = NULL;
        tail->prev = NULL;
    }
    return tail;
}

tt_inline void tt_dlist_clear(IN tt_dlist_t *dl)
{
    while (tt_dlist_pop_head(dl) != NULL)
        ;
}

tt_inline tt_bool_t tt_dlist_contain(IN tt_dlist_t *dl, IN tt_dnode_t *dn)
{
    tt_dnode_t *node = dl->head;
    while (node != NULL) {
        if (node == dn) {
            return TT_TRUE;
        }
        node = node->next;
    }
    return TT_FALSE;
}

tt_inline tt_dnode_t *tt_dlist_remove(IN tt_dlist_t *dl, IN tt_dnode_t *dn)
{
    tt_dnode_t *next = dn->next;

    if ((next == NULL) && (dn->prev == NULL) && (dl->head != dn)) {
        // this may be a removed node
        return NULL;
    }
    TT_ASSERT_DL(tt_dlist_contain(dl, dn));

    if (next != NULL) {
        next->prev = dn->prev;
    } else {
        dl->tail = dn->prev;
    }

    if (dn->prev != NULL) {
        dn->prev->next = next;
    } else {
        dl->head = next;
    }

    dn->next = NULL;
    dn->prev = NULL;

    return next;
}

tt_inline void tt_dlist_move(IN tt_dlist_t *dst, IN tt_dlist_t *src)
{
    if (tt_dlist_empty(src)) {
        return;
    }

    if (dst->tail != NULL) {
        dst->tail->next = src->head;
        src->head->prev = dst->tail;
    } else {
        dst->head = src->head;
    }
    dst->tail = src->tail;

    tt_dlist_init(src);
}

tt_inline void tt_dlist_swap(IN tt_dlist_t *a, IN tt_dlist_t *b)
{
    TT_SWAP(tt_dnode_t *, a->head, b->head);
    TT_SWAP(tt_dnode_t *, a->tail, b->tail);
}

tt_inline void tt_dlist_insert_before(IN tt_dlist_t *dl,
                                      IN tt_dnode_t *pos,
                                      IN tt_dnode_t *dn)
{
    TT_ASSERT((dn->prev == NULL) && (dn->next == NULL));

    if (pos->prev != NULL) {
        pos->prev->next = dn;
    } else {
        dl->head = dn;
    }
    dn->prev = pos->prev;

    dn->next = pos;
    pos->prev = dn;
}

tt_inline void tt_dlist_insert_after(IN tt_dlist_t *dl,
                                     IN tt_dnode_t *pos,
                                     IN tt_dnode_t *dn)
{
    TT_ASSERT((dn->prev == NULL) && (dn->next == NULL));

    if (pos->next != NULL) {
        pos->next->prev = dn;
    } else {
        dl->tail = dn;
    }
    dn->next = pos->next;

    dn->prev = pos;
    pos->next = dn;
}

#endif /* __TT_DOUBLE_LINKED_LIST__ */
