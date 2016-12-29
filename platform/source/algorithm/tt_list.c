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

#include <algorithm/tt_list.h>

#include <misc/tt_assert.h>
#include <misc/tt_util.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __LIST_ASSERTION

#ifdef __LIST_ASSERTION
#define TT_ASSERT_LST TT_ASSERT
#else
#define TT_ASSERT_LST(...)
#endif

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

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_list_init(IN tt_list_t *lst)
{
    lst->head = NULL;
    lst->tail = NULL;
    lst->count = 0;
}

void tt_lnode_init(IN tt_lnode_t *node)
{
    node->lst = NULL;
    node->prev = NULL;
    node->next = NULL;
}

void tt_list_clear(IN tt_list_t *lst)
{
    while (tt_list_pop_head(lst) != NULL)
        ;
}

void tt_list_push_head(IN tt_list_t *lst, IN tt_lnode_t *node)
{
    TT_ASSERT_ALWAYS(node->lst == NULL);

    if (lst->head != NULL) {
        node->next = lst->head;
        node->lst = lst;

        // link
        lst->head->prev = node;
        lst->head = node;
        lst->count++;
    } else {
        node->lst = lst;

        // link
        lst->head = node;
        lst->tail = node;
        lst->count++;
    }
}

void tt_list_push_tail(IN tt_list_t *lst, IN tt_lnode_t *node)
{
    TT_ASSERT_ALWAYS(node->lst == NULL);

    if (lst->tail != NULL) {
        node->prev = lst->tail;
        node->lst = lst;

        // link
        lst->tail->next = node;
        lst->tail = node;
        lst->count++;
    } else {
        node->lst = lst;

        // link
        lst->head = node;
        lst->tail = node;
        lst->count++;
    }
}

tt_lnode_t *tt_list_pop_head(IN tt_list_t *lst)
{
    tt_lnode_t *head = lst->head;
    if (head != NULL) {
        TT_ASSERT_LST(lst->count > 0);

        lst->head = head->next;
        --lst->count;

        if (head->next != NULL) {
            head->next->prev = NULL;
        } else {
            lst->tail = NULL;
        }

        head->lst = NULL;
        head->prev = NULL;
        head->next = NULL;
    }

    return head;
}

tt_lnode_t *tt_list_pop_tail(IN tt_list_t *lst)
{
    tt_lnode_t *tail = lst->tail;
    if (tail != NULL) {
        // a consistency check
        TT_ASSERT_LST(lst->count > 0);

        lst->tail = tail->prev;
        --lst->count;

        if (tail->prev != NULL) {
            tail->prev->next = NULL;
        } else {
            lst->head = NULL;
        }

        tail->lst = NULL;
        tail->prev = NULL;
        tail->next = NULL;
    }

    return tail;
}

void tt_list_insert_before(IN tt_lnode_t *pos, IN tt_lnode_t *node)
{
    tt_list_t *lst;

    TT_ASSERT(node->lst == NULL);

    lst = pos->lst;
    TT_ASSERT_LST(lst != NULL);

    node->next = pos;
    node->prev = pos->prev;
    node->lst = lst;

    if (pos->prev != NULL) {
        pos->prev->next = node;
        pos->prev = node;
    } else {
        pos->prev = node;
        lst->head = node;
    }
    ++lst->count;
}

void tt_list_insert_after(IN tt_lnode_t *pos, IN tt_lnode_t *node)
{
    tt_list_t *lst;

    TT_ASSERT_LST(node->lst == NULL);

    lst = pos->lst;
    TT_ASSERT_LST(lst != NULL);

    node->prev = pos;
    node->next = pos->next;
    node->lst = lst;

    if (pos->next != NULL) {
        pos->next->prev = node;
        pos->next = node;
    } else {
        pos->next = node;
        lst->tail = node;
    }
    ++lst->count;
}

tt_lnode_t *tt_list_remove(IN tt_lnode_t *node)
{
    tt_list_t *lst;
    tt_lnode_t *next;

    lst = node->lst;
    if (lst == NULL) {
        return NULL;
    }

    next = node->next;

    if (node->prev != NULL) {
        node->prev->next = node->next;
    } else {
        lst->head = node->next;
    }

    if (node->next != NULL) {
        node->next->prev = node->prev;
    } else {
        lst->tail = node->prev;
    }

    --lst->count;

    node->lst = NULL;
    node->prev = NULL;
    node->next = NULL;

    return next;
}
