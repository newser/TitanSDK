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
@file tt_list_double_link.h
@brief doulbe linked list
this file defines apis of doulbe linked list data structure.
*/

#ifndef __TT_LIST_DOUBLE_LINK__
#define __TT_LIST_DOUBLE_LINK__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct tt_dnode_s
{
    struct tt_dnode_s *dprev;
    struct tt_dnode_s *dnext;
} tt_dnode_t;

typedef tt_dnode_t tt_dlist_t;
#define __DLIST_HEAD(dl) ((dl)->dnext)
#define __DLIST_TAIL(dl) ((dl)->dprev)

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
    node->dprev = NULL;
    node->dnext = NULL;
}

tt_inline tt_u32_t tt_dlist_count(IN tt_dlist_t *list)
{
    tt_u32_t count = 0;
    tt_dnode_t *node = __DLIST_HEAD(list);

    // never use while (node != NULL)
    while (node != list) {
        ++count;
        node = node->dnext;
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
    return node->dnext == list ? NULL : node->dnext;
}

tt_inline void tt_dlist_pushhead(IN tt_dlist_t *list, IN tt_dnode_t *node)
{
    // later nodes
    __DLIST_HEAD(list)->dprev = node;
    node->dnext = __DLIST_HEAD(list);

    // former nodes
    node->dprev = list;
    __DLIST_HEAD(list) = node;
}

tt_inline void tt_dlist_pushtail(IN tt_dlist_t *list, IN tt_dnode_t *node)
{
    // later nodes
    __DLIST_TAIL(list)->dnext = node;
    node->dprev = __DLIST_TAIL(list);

    // former nodes
    node->dnext = list;
    __DLIST_TAIL(list) = node;
}

tt_inline tt_dnode_t *tt_dlist_pophead(IN tt_dlist_t *list)
{
    tt_dnode_t *head = NULL;

    if (!tt_dlist_empty(list)) {
        head = __DLIST_HEAD(list);
        head->dnext->dprev = head->dprev; // = list;
        __DLIST_HEAD(list) = head->dnext;
    }

    return head;
}

tt_inline tt_dnode_t *tt_dlist_poptail(IN tt_dlist_t *list)
{
    tt_dnode_t *tail = NULL;

    if (!tt_dlist_empty(list)) {
        tail = __DLIST_TAIL(list);
        tail->dprev->dnext = tail->dnext; // = list;
        __DLIST_TAIL(list) = tail->dprev;
    }

    return tail;
}

tt_inline void tt_dlist_remove(IN tt_dnode_t *node)
{
    node->dnext->dprev = node->dprev;
    node->dprev->dnext = node->dnext;
}

tt_inline void tt_dlist_merge(IN tt_dlist_t *dst, IN tt_dlist_t *src)
{
    if (tt_dlist_empty(src)) {
        return;
    }

    __DLIST_TAIL(dst)->dnext = __DLIST_HEAD(src);
    __DLIST_HEAD(src)->dprev = __DLIST_TAIL(dst);

    __DLIST_TAIL(dst) = __DLIST_TAIL(src);
    __DLIST_TAIL(src)->dnext = dst;

    tt_dlist_init(src);
}

#endif /* __TT_LIST_DOUBLE_LINK__ */
