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
@file tt_list.h
@brief list data structure

this file defines list data structure and related operations.

 - push_tail/push_head/pop_tail/pop_head
 - head/tail
 - insert_front/insert_back
 - count/empty
 - clear
 - contain/contain_all
 - find/find_from/find_last
 - remove(node)/remove_equal

*/

#ifndef __TT_LIST__
#define __TT_LIST__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_compare.h>
#include <misc/tt_util.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_list_s;

/**
@struct tt_lnode_t
a node in a list
*/
typedef struct tt_lnode_s
{
    struct tt_list_s *lst;
    struct tt_lnode_s *prev;
    struct tt_lnode_s *next;
} tt_lnode_t;

/**
@struct tt_list_t
list
*/
typedef struct tt_list_s
{
    tt_lnode_t *head;
    tt_lnode_t *tail;
    tt_u32_t count;
} tt_list_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/**
 @fn void tt_lnode_init(IN tt_lnode_t *node)
 initialize a tt_lnode_t structure pointed by node

 @param [in] node pointer to a tt_lnode_t

 @return
 void

 @note
 there is no memory operation in such function, it only initialize
 the struct passed in by param
 */
tt_inline void tt_lnode_init(IN tt_lnode_t *node)
{
    node->lst = NULL;
    node->prev = NULL;
    node->next = NULL;
}

/**
 @fn void tt_list_init(IN tt_list_t *lst)
 initialize a tt_list_t structure pointed by lst

 @param [in] lst pointer to a tt_list_t

 @return
 void

 @note
 there is no memory operation in such function, it only initialize
 the struct passed in by param
 */
tt_inline void tt_list_init(IN tt_list_t *lst)
{
    lst->head = NULL;
    lst->tail = NULL;
    lst->count = 0;
}

/**
 @def tt_list_count(lst)
 get the count of the list nodes

 @param [in] lst pointer to a tt_list_t

 @return
 count of the list nodes

 @note
 the parameter must be a @b pointer
 */
tt_inline tt_u32_t tt_list_count(IN tt_list_t *lst)
{
    return lst->count;
}

/**
 @def tt_list_empty(lst)
 check if a list is empty

 @param [in] lst pointer to a tt_list_t

 @return
 - 1 if list is empty
 - 0 if list is not empty

 @note
 the parameter must be a @b pointer
 */
tt_inline tt_bool_t tt_list_empty(IN tt_list_t *lst)
{
    return tt_list_count(lst) == 0 ? TT_TRUE : TT_FALSE;
}

/**
 @def tt_list_head(lst)
 get head node of the list specified by lst

 @param [in] lst pointer to a tt_list_t

 @return
 head node of the list

 @note
 - the parameter must be a @b pointer
 - the pointer returned is just a node, use TT_NODE_DATA()
 to get the data
 */
tt_inline tt_lnode_t *tt_list_head(IN tt_list_t *lst)
{
    return lst->head;
}

/**
 @def tt_list_tail(lst)
 get tail node of the list specified by lst

 @param [in] lst pointer to a tt_list_t

 @return
 tail node of the list

 @note
 - the parameter must be a @b pointer
 - the pointer returned is just a node, use TT_NODE_DATA()
 to get the data
 */
tt_inline tt_lnode_t *tt_list_tail(IN tt_list_t *lst)
{
    return lst->tail;
}

/**
@fn void tt_list_push_head(IN tt_list_t *lst,
                          IN tt_lnode_t *node)
add a node to the head of the list

@param [in] lst pointer to a tt_list_t
@param [in] node pointer to a tt_lnode_t

@return
void
*/
tt_export void tt_list_push_head(IN tt_list_t *lst, IN tt_lnode_t *node);

/**
@fn void tt_list_push_tail(IN tt_list_t *lst,
                          IN tt_lnode_t *node)
add a node to the tail of the list

@param [in] lst pointer to a tt_list_t
@param [in] node pointer to a tt_lnode_t

@return
void
*/
tt_export void tt_list_push_tail(IN tt_list_t *lst, IN tt_lnode_t *node);

/**
@fn tt_lnode_t* tt_list_pop_head(IN tt_list_t *lst)
pop the head node in the list

@param [in] lst pointer to a tt_list_t

@return
- head node if list is not empty
- NULL otherwise
*/
tt_export tt_lnode_t *tt_list_pop_head(IN tt_list_t *lst);

/**
@fn tt_lnode_t* tt_list_pop_tail(IN tt_list_t *lst)
pop the tail node in the list

@param [in] lst pointer to a tt_list_t

@return
- tail node if list is not empty
- NULL otherwise
*/
tt_export tt_lnode_t *tt_list_pop_tail(IN tt_list_t *lst);

tt_inline void tt_list_clear(IN tt_list_t *lst)
{
    while (tt_list_pop_head(lst) != NULL)
        ;
}

/**
 @fn void tt_list_insert_before(IN tt_lnode_t *pos,
 IN tt_lnode_t *node)
 insert a node to the list before node pointed by pos

 @param [in] pos pointer to a node before which new node would be inserted
 @param [in] node new node would be inserted

 @return
 void
 */
tt_export void tt_list_insert_before(IN tt_lnode_t *pos, IN tt_lnode_t *node);

/**
 @fn void tt_list_insert_after(IN tt_lnode_t *pos,
 IN tt_lnode_t *node)
 insert a node to the list after node pointed by pos

 @param [in] pos pointer to a node after which new node would be inserted
 @param [in] node new node would be inserted

 @return
 void
 */
tt_export void tt_list_insert_after(IN tt_lnode_t *pos, IN tt_lnode_t *node);

/**
 @fn tt_lnode_t* tt_list_remove(IN tt_lnode_t *node)
 remove the node in the list

 @param [in] node pointer to a node to be removed

 @return
 - the node next to the pos just removed
 - NULL if the pos is the tail
 */
tt_export tt_lnode_t *tt_list_remove(IN tt_lnode_t *node);

tt_inline tt_bool_t tt_list_contain(IN tt_list_t *lst, IN tt_lnode_t *node)
{
    return TT_BOOL(lst == node->lst);
}

#endif /* __TT_LIST__ */
