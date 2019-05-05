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

void tt_list_push_head(IN tt_list_t *lst, IN tt_lnode_t *node)
{
    TT_ASSERT(node->lst == NULL);

    node->next = lst->head;
    node->lst = lst;

    // link it
    if (lst->head != NULL) {
        lst->head->prev = node;
    } else {
        lst->tail = node;
    }
    lst->head = node;
    ++lst->count;
}

void tt_list_push_tail(IN tt_list_t *lst, IN tt_lnode_t *node)
{
    TT_ASSERT(node->lst == NULL);

    node->prev = lst->tail;
    node->lst = lst;

    // link it
    if (lst->tail != NULL) {
        lst->tail->next = node;
    } else {
        lst->head = node;
    }
    lst->tail = node;
    ++lst->count;
}

tt_lnode_t *tt_list_pop_head(IN tt_list_t *lst)
{
    tt_lnode_t *head = lst->head;
    if (head != NULL) {
        lst->head = head->next;
        if (head->next != NULL) {
            head->next->prev = NULL;
        } else {
            lst->tail = NULL;
        }
        --lst->count;

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
        lst->tail = tail->prev;
        if (tail->prev != NULL) {
            tail->prev->next = NULL;
        } else {
            lst->head = NULL;
        }
        --lst->count;

        tail->lst = NULL;
        tail->prev = NULL;
        tail->next = NULL;
    }

    return tail;
}

void tt_list_insert_before(IN tt_lnode_t *pos, IN tt_lnode_t *node)
{
    tt_list_t *lst = pos->lst;

    TT_ASSERT(node->lst == NULL);
    TT_ASSERT(lst != NULL);

    node->next = pos;
    node->prev = pos->prev;
    node->lst = lst;

    // link it
    if (pos->prev != NULL) {
        pos->prev->next = node;
    } else {
        lst->head = node;
    }
    pos->prev = node;
    ++lst->count;
}

void tt_list_insert_after(IN tt_lnode_t *pos, IN tt_lnode_t *node)
{
    tt_list_t *lst = pos->lst;

    TT_ASSERT(node->lst == NULL);
    TT_ASSERT(lst != NULL);

    node->prev = pos;
    node->next = pos->next;
    node->lst = lst;

    if (pos->next != NULL) {
        pos->next->prev = node;
    } else {
        lst->tail = node;
    }
    pos->next = node;
    ++lst->count;
}

tt_lnode_t *tt_list_remove(IN tt_lnode_t *node)
{
    tt_list_t *lst;
    tt_lnode_t *next;

    lst = node->lst;
    if (lst == NULL) { return NULL; }

    next = node->next;

    if (node->prev != NULL) {
        node->prev->next = next;
    } else {
        lst->head = node->next;
    }

    if (next != NULL) {
        next->prev = node->prev;
    } else {
        lst->tail = node->prev;
    }

    --lst->count;

    node->lst = NULL;
    node->prev = NULL;
    node->next = NULL;

    return next;
}
