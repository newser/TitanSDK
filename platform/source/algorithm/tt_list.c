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
    TT_ASSERT_LST(lst != NULL);

    lst->node_num = 0;
    lst->head = NULL;
    lst->tail = NULL;
}

void tt_lnode_init(IN tt_lnode_t *node)
{
    TT_ASSERT_LST(node != NULL);

    node->lst = NULL;
    node->prev = NULL;
    node->next = NULL;
}

void tt_list_addhead(IN tt_list_t *lst, IN tt_lnode_t *node)
{
    TT_ASSERT_LST(lst != NULL);
    TT_ASSERT_LST(node != NULL);
    TT_ASSERT_LST(node->lst == NULL);

    if (lst->head != NULL) {
        // set node data
        node->next = lst->head;
        node->lst = lst;

        // link it into list
        lst->head->prev = node;
        lst->head = node;
        lst->node_num++;
    } else {
        // set node data
        node->lst = lst;

        // link it into list
        lst->head = node;
        lst->tail = node;
        lst->node_num++;
    }
}

void tt_list_addtail(IN tt_list_t *lst, IN tt_lnode_t *node)
{
    TT_ASSERT_LST(lst != NULL);
    TT_ASSERT_LST(node != NULL);
    TT_ASSERT_LST(node->lst == NULL);

    if (lst->tail != NULL) {
        // set node data
        node->prev = lst->tail;
        node->lst = lst;

        // link it into list
        lst->tail->next = node;
        lst->tail = node;
        lst->node_num++;
    } else {
        // set node data
        node->lst = lst;

        // link it into list
        lst->head = node;
        lst->tail = node;
        lst->node_num++;
    }
}

void tt_list_insert_prev(IN tt_lnode_t *pos, IN tt_lnode_t *node)
{
    tt_list_t *lst = NULL;

    TT_ASSERT_LST(pos != NULL);
    TT_ASSERT_LST(node != NULL);
    TT_ASSERT_LST(node->lst == NULL);

    lst = pos->lst;
    TT_ASSERT_LST(lst != NULL);

    // set node data
    node->next = pos;
    node->prev = pos->prev;
    node->lst = lst;

    // link it
    if (pos->prev != NULL) {
        pos->prev->next = node;
        pos->prev = node;
    } else {
        pos->prev = node;
        lst->head = node;
    }
    lst->node_num++;
}

void tt_list_insert_next(IN tt_lnode_t *pos, IN tt_lnode_t *node)
{
    tt_list_t *lst = NULL;

    TT_ASSERT_LST(pos != NULL);
    TT_ASSERT_LST(node != NULL);
    TT_ASSERT_LST(node->lst == NULL);

    lst = pos->lst;
    TT_ASSERT_LST(lst != NULL);

    // set node data
    node->prev = pos;
    node->next = pos->next;
    node->lst = lst;

    // link it
    if (pos->next != NULL) {
        pos->next->prev = node;
        pos->next = node;
    } else {
        pos->next = node;
        lst->tail = node;
    }
    lst->node_num++;
}

tt_lnode_t *tt_list_remove(IN tt_lnode_t *node)
{
    tt_list_t *lst = NULL;
    tt_lnode_t *next = NULL;

    TT_ASSERT_LST(node != NULL);

    lst = node->lst;
    // TT_ASSERT_LST(lst != NULL);
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

    lst->node_num--;

    // set node data
    node->lst = NULL;
    node->prev = NULL;
    node->next = NULL;

    return next;
}

tt_lnode_t *tt_list_pophead(IN tt_list_t *lst)
{
    tt_lnode_t *head = lst->head;

    TT_ASSERT_LST(lst != NULL);

    if (head != NULL) {
        // a consistency check
        TT_ASSERT_LST(lst->node_num > 0);

        lst->head = head->next;

        if (head->next != NULL) {
            head->next->prev = NULL;
        } else {
            lst->tail = NULL;
        }

        lst->node_num--;

        // set node data
        head->lst = NULL;
        head->prev = NULL;
        head->next = NULL;
    }

    return head;
}

tt_lnode_t *tt_list_poptail(IN tt_list_t *lst)
{
    tt_lnode_t *tail = lst->tail;

    TT_ASSERT_LST(lst != NULL);

    if (tail != NULL) {
        // a consistency check
        TT_ASSERT_LST(lst->node_num > 0);

        if (tail->prev != NULL) {
            tail->prev->next = NULL;
        } else {
            lst->head = NULL;
        }

        lst->tail = tail->prev;

        lst->node_num--;

        // set node data
        tail->lst = NULL;
        tail->prev = NULL;
        tail->next = NULL;
    }

    return tail;
}
