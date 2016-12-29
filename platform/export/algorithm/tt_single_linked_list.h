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
@file tt_single_linked_list.h
@brief simple list
this file defines apis of simple list data structure.

 - push_head/push_tail/pop_head/pop_tail
 - next/tail
 - insert_before/insert_after
 - empty
 - clear
 - remove(node)
 - swap
*/

#ifndef __TT_SINGLE_LINKED_LIST__
#define __TT_SINGLE_LINKED_LIST__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <misc/tt_assert.h>

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

typedef tt_snode_t tt_slist_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_inline void tt_slist_init(IN tt_slist_t *list)
{
    list->next = NULL;
}

tt_inline void tt_snode_init(IN tt_snode_t *node)
{
    node->next = NULL;
}

tt_inline tt_u32_t tt_slist_count(IN tt_slist_t *list)
{
    tt_u32_t count = 0;
    tt_snode_t *node;

    node = list->next;
    while (node != NULL) {
        ++count;
        node = node->next;
    }
    return count;
}

tt_inline void tt_slist_clear(IN tt_slist_t *list)
{
    tt_snode_t *node = list->next;
    while (node != NULL) {
        tt_snode_t *next = node->next;
        node->next = NULL;
        node = next;
    }
    list->next = NULL;
}

tt_inline tt_bool_t tt_slist_empty(IN tt_slist_t *list)
{
    return list->next == NULL ? TT_TRUE : TT_FALSE;
}

tt_inline tt_snode_t *tt_slist_head(IN tt_slist_t *list)
{
    return list->next;
}

tt_inline tt_snode_t *tt_slist_tail(IN tt_slist_t *list)
{
    tt_snode_t *tail = list->next;
    if (tail != NULL) {
        while (tail->next != NULL) {
            tail = tail->next;
        }
    }
    return tail;
}

tt_inline void tt_slist_push_head(IN tt_slist_t *list, IN tt_snode_t *node)
{
    TT_ASSERT(node->next == NULL);

    node->next = list->next;
    list->next = node;
}

tt_inline void tt_slist_push_tail(IN tt_slist_t *list, IN tt_snode_t *node)
{
    TT_ASSERT(node->next == NULL);

    if (list->next != NULL) {
        tt_slist_tail(list)->next = node;
    } else {
        list->next = node;
    }
}

tt_inline tt_snode_t *tt_slist_pop_head(IN tt_slist_t *list)
{
    tt_snode_t *next = NULL;

    if (list->next != NULL) {
        next = list->next;
        list->next = next->next;
        next->next = NULL;
    }

    return next;
}

tt_inline tt_snode_t *tt_slist_pop_tail(IN tt_slist_t *list)
{
    tt_snode_t *tail = list->next;
    if (tail == NULL) {
        return NULL;
    } else if (tail->next == NULL) {
        list->next = NULL;
        return tail;
    } else {
        tt_snode_t *prev;
        do {
            prev = tail;
            tail = tail->next;
        } while (tail->next != NULL);
        prev->next = NULL;

        tail->next = NULL;
        return tail;
    }
}

tt_inline void tt_slist_insert_after(IN tt_snode_t *pos, IN tt_snode_t *node)
{
    TT_ASSERT(node->next == NULL);

    node->next = pos->next;
    pos->next = node;
}

// if prev_node is NULL, then the node to be removed is the next
tt_inline void tt_slist_remove(IN tt_snode_t *prev, IN tt_snode_t *node)
{
    TT_ASSERT(prev->next == node);

    prev->next = node->next;
    node->next = NULL;
}

tt_inline void tt_slist_move(IN tt_slist_t *dst, IN tt_slist_t *src)
{
    if (dst->next == NULL) {
        dst->next = src->next;
    } else {
        tt_slist_tail(dst)->next = src->next;
    }
    src->next = NULL;
}

// returns how many nodes are moved to dst
tt_inline tt_u32_t tt_slist_move_count(IN tt_slist_t *dst,
                                       IN tt_slist_t *src,
                                       IN tt_u32_t count)
{
    tt_snode_t *prev, *node;
    tt_u32_t n = 0;

    prev = src;
    node = src->next;
    while ((node != NULL) && (n++ < count)) {
        prev = node;
        node = node->next;
    }
    prev->next = NULL;

    if (dst->next == NULL) {
        dst->next = src->next;
    } else {
        tt_slist_tail(dst)->next = src->next;
    }
    src->next = node;

    return n;
}

tt_inline void tt_slist_swap(IN tt_slist_t *l, IN tt_slist_t *r)
{
    TT_SWAP(tt_ptr_t, l->next, r->next);
}

#endif /* __TT_SINGLE_LINKED_LIST__ */
