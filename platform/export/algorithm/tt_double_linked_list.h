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
@file tt_double_linked_list.h
@brief doulbe linked list
this file defines apis of doulbe linked list data structure.

 - push_tail/push_head/pop_tail/pop_head
 - head/tail
 - insert_before/insert_after
 - move
 - empty
 - clear
 - remove(node)

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

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct tt_dnode_s
{
    struct tt_dnode_s *prev;
    struct tt_dnode_s *next;
} tt_dnode_t;

typedef tt_dnode_t tt_dlist_t;
#define __DLIST_HEAD(dl) ((dl)->next)
#define __DLIST_TAIL(dl) ((dl)->prev)

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_inline void tt_dlist_init(IN tt_dlist_t *list)
{
    __DLIST_HEAD(list) = list;
    __DLIST_TAIL(list) = list;
}

tt_inline void tt_dnode_init(IN tt_dnode_t *node)
{
    node->prev = NULL;
    node->next = NULL;
}

tt_inline tt_u32_t tt_dlist_count(IN tt_dlist_t *list)
{
    tt_u32_t count = 0;
    tt_dnode_t *node = __DLIST_HEAD(list);

    // never use while (node != NULL)
    while (node != list) {
        ++count;
        node = node->next;
    }
    return count;
}

tt_inline tt_bool_t tt_dlist_empty(IN tt_dlist_t *list)
{
    return __DLIST_HEAD((list)) == (list) ? TT_TRUE : TT_FALSE;
}

tt_inline tt_dnode_t *tt_dlist_head(IN tt_dlist_t *list)
{
    return tt_dlist_empty(list) ? NULL : __DLIST_HEAD(list);
}

tt_inline tt_dnode_t *tt_dlist_tail(IN tt_dlist_t *list)
{
    return tt_dlist_empty(list) ? NULL : __DLIST_TAIL(list);
}

tt_inline tt_dnode_t *tt_dlist_next(IN tt_dlist_t *list, IN tt_dnode_t *node)
{
    return node->next == list ? NULL : node->next;
}

tt_inline void tt_dlist_push_head(IN tt_dlist_t *list, IN tt_dnode_t *node)
{
    // update next
    __DLIST_HEAD(list)->prev = node;
    node->next = __DLIST_HEAD(list);

    // update prev
    node->prev = list;
    __DLIST_HEAD(list) = node;
}

tt_inline void tt_dlist_push_tail(IN tt_dlist_t *list, IN tt_dnode_t *node)
{
    // update prev
    __DLIST_TAIL(list)->next = node;
    node->prev = __DLIST_TAIL(list);

    // update next
    node->next = list;
    __DLIST_TAIL(list) = node;
}

tt_inline tt_dnode_t *tt_dlist_pop_head(IN tt_dlist_t *list)
{
    tt_dnode_t *head = NULL;

    if (!tt_dlist_empty(list)) {
        head = __DLIST_HEAD(list);
        head->next->prev = head->prev; // = list;
        __DLIST_HEAD(list) = head->next;
    }

    return head;
}

tt_inline tt_dnode_t *tt_dlist_pop_tail(IN tt_dlist_t *list)
{
    tt_dnode_t *tail = NULL;

    if (!tt_dlist_empty(list)) {
        tail = __DLIST_TAIL(list);
        tail->prev->next = tail->next; // = list;
        __DLIST_TAIL(list) = tail->prev;
    }

    return tail;
}

tt_inline void tt_dlist_clear(IN tt_dlist_t *list)
{
    while (tt_dlist_pop_head(list) != NULL)
        ;
}

tt_inline void tt_dlist_remove(IN tt_dnode_t *node)
{
    if (node->next == NULL) {
        return;
    }

    node->next->prev = node->prev;
    node->prev->next = node->next;
}

tt_inline void tt_dlist_move(IN tt_dlist_t *dst, IN tt_dlist_t *src)
{
    if (tt_dlist_empty(src)) {
        return;
    }

    __DLIST_TAIL(dst)->next = __DLIST_HEAD(src);
    __DLIST_HEAD(src)->prev = __DLIST_TAIL(dst);

    __DLIST_TAIL(dst) = __DLIST_TAIL(src);
    __DLIST_TAIL(src)->next = dst;

    tt_dlist_init(src);
}

tt_inline void tt_dlist_insert_before(IN tt_dnode_t *pos, IN tt_dnode_t *node)
{
    TT_ASSERT((node->prev == NULL) && (node->next == NULL));

    pos->prev->next = node;
    node->prev = pos->prev;

    node->next = pos;
    pos->prev = node;
}

tt_inline void tt_dlist_insert_after(IN tt_dnode_t *pos, IN tt_dnode_t *node)
{
    TT_ASSERT((node->prev == NULL) && (node->next == NULL));

    pos->next->prev = node;
    node->next = pos->next;

    node->prev = pos;
    pos->next = node;
}

#endif /* __TT_DOUBLE_LINKED_LIST__ */
