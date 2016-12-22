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
@file tt_list_single_link.h
@brief simple list
this file defines apis of simple list data structure.
*/

#ifndef __TT_LIST_SINGLE_LINK__
#define __TT_LIST_SINGLE_LINK__

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

typedef tt_ptr_t tt_slnode_t;

typedef struct tt_slist_s
{
    tt_slnode_t *head;
} tt_slist_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_inline void tt_slist_init(IN tt_slist_t *list)
{
    list->head = NULL;
}

tt_inline void tt_plnode_init(IN tt_slnode_t *node)
{
    *node = NULL;
}

tt_inline tt_u32_t tt_slist_count(IN tt_slist_t *list)
{
    tt_u32_t count = 0;
    tt_slnode_t *node = NULL;

    node = list->head;
    while (node != NULL) {
        ++count;
        node = *(tt_slnode_t **)node;
    }
    return count;
}

tt_inline tt_slnode_t *tt_slist_head(IN tt_slist_t *list)
{
    return list->head;
}

tt_inline tt_slnode_t *tt_slist_next(IN tt_slnode_t *node)
{
    return *(tt_slnode_t **)node;
}

tt_inline tt_bool_t tt_slist_empty(IN tt_slist_t *list)
{
    return tt_slist_head((list)) == NULL ? TT_TRUE : TT_FALSE;
}

tt_inline void tt_slist_pushhead(IN tt_slist_t *list, IN tt_slnode_t *node)
{
    *node = list->head;
    list->head = node;
}

tt_inline tt_slnode_t *tt_slist_pophead(IN tt_slist_t *list)
{
    tt_slnode_t *head = NULL;

    if (list->head != NULL) {
        head = list->head;
        list->head = *(tt_slnode_t **)head;
    }

    return head;
}

// if prev_node is NULL, then the node to be removed is the head
tt_inline void tt_slist_remove(IN tt_slist_t *list,
                               IN tt_slnode_t *node,
                               IN tt_slnode_t *prev_node)
{
    if (prev_node == NULL) {
        list->head = *(tt_slnode_t **)node;
    } else {
        *prev_node = *node;
    }
}

tt_inline void tt_slist_merge(IN tt_slist_t *dst, IN tt_slist_t *src)
{
    tt_slnode_t *node = NULL;

    node = src->head;
    while (node != NULL) {
        tt_slnode_t *next = *(tt_slnode_t **)node;

        *node = dst->head;
        dst->head = node;

        node = next;
    }
    src->head = NULL;
}

tt_inline tt_u32_t tt_slist_merge_n(IN tt_slist_t *dst,
                                    IN tt_slist_t *src,
                                    IN tt_u32_t count)
{
    tt_slnode_t *node = NULL;
    tt_u32_t merged_count = 0;

    node = src->head;
    while ((node != NULL) && (merged_count < count)) {
        tt_slnode_t *next = *(tt_slnode_t **)node;

        *node = dst->head;
        dst->head = node;

        ++merged_count;

        node = next;
    }
    src->head = node;

    // returns how many are merged to dst
    return merged_count;
}

tt_inline void tt_slist_swap(IN tt_slist_t *l, IN tt_slist_t *r)
{
    tt_slnode_t *l_head = NULL;

    l_head = l->head;
    l->head = r->head;
    r->head = l_head;
}

#endif /* __TT_LIST_SINGLE_LINK__ */
