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
@file tt_ptrlist.h
@brief simple list
this file defines apis of simple list data structure.
*/

#ifndef __TT_PTRLIST__
#define __TT_PTRLIST__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef tt_ptr_t tt_plnode_t;

typedef struct tt_ptrlist_s
{
    tt_plnode_t *head;
} tt_ptrlist_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_inline void tt_ptrlist_init(IN tt_ptrlist_t *list)
{
    list->head = NULL;
}

tt_inline void tt_plnode_init(IN tt_plnode_t *node)
{
    *node = NULL;
}

tt_inline tt_u32_t tt_ptrlist_count(IN tt_ptrlist_t *list)
{
    tt_u32_t count = 0;
    tt_plnode_t *node = NULL;

    node = list->head;
    while (node != NULL) {
        ++count;
        node = *(tt_plnode_t **)node;
    }
    return count;
}

tt_inline tt_plnode_t *tt_ptrlist_head(IN tt_ptrlist_t *list)
{
    return list->head;
}

tt_inline tt_plnode_t *tt_ptrlist_next(IN tt_plnode_t *node)
{
    return *(tt_plnode_t **)node;
}

tt_inline tt_bool_t tt_ptrlist_empty(IN tt_ptrlist_t *list)
{
    return tt_ptrlist_head((list)) == NULL ? TT_TRUE : TT_FALSE;
}

tt_inline void tt_ptrlist_pushhead(IN tt_ptrlist_t *list, IN tt_plnode_t *node)
{
    *node = list->head;
    list->head = node;
}

tt_inline tt_plnode_t *tt_ptrlist_pophead(IN tt_ptrlist_t *list)
{
    tt_plnode_t *head = NULL;

    if (list->head != NULL) {
        head = list->head;
        list->head = *(tt_plnode_t **)head;
    }

    return head;
}

// if prev_node is NULL, then the node to be removed is the head
tt_inline void tt_ptrlist_remove(IN tt_ptrlist_t *list,
                                 IN tt_plnode_t *node,
                                 IN tt_plnode_t *prev_node)
{
    if (prev_node == NULL) {
        list->head = *(tt_plnode_t **)node;
    } else {
        *prev_node = *node;
    }
}

tt_inline void tt_ptrlist_merge(IN tt_ptrlist_t *dst, IN tt_ptrlist_t *src)
{
    tt_plnode_t *node = NULL;

    node = src->head;
    while (node != NULL) {
        tt_plnode_t *next = *(tt_plnode_t **)node;

        *node = dst->head;
        dst->head = node;

        node = next;
    }
    src->head = NULL;
}

tt_inline tt_u32_t tt_ptrlist_merge_n(IN tt_ptrlist_t *dst,
                                      IN tt_ptrlist_t *src,
                                      IN tt_u32_t count)
{
    tt_plnode_t *node = NULL;
    tt_u32_t merged_count = 0;

    node = src->head;
    while ((node != NULL) && (merged_count < count)) {
        tt_plnode_t *next = *(tt_plnode_t **)node;

        *node = dst->head;
        dst->head = node;

        ++merged_count;

        node = next;
    }
    src->head = node;

    // returns how many are merged to dst
    return merged_count;
}

tt_inline void tt_ptrlist_swap(IN tt_ptrlist_t *l, IN tt_ptrlist_t *r)
{
    tt_plnode_t *l_head = NULL;

    l_head = l->head;
    l->head = r->head;
    r->head = l_head;
}

#endif /* __TT_PTRLIST__ */
