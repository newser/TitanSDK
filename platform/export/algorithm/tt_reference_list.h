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
 @file tt_reference_list.h
 @brief reference list
 this file defines apis of reference list data structure.
 */

#ifndef __TT_REFERENCE_LIST__
#define __TT_REFERENCE_LIST__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <log/tt_log.h>
#include <memory/tt_memory_alloc.h>
#include <misc/tt_util.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct tt_refnode_s
{
    struct tt_refnode_s *prev;
    struct tt_refnode_s *next;
    void *p;
} tt_refnode_t;

typedef tt_refnode_t tt_reflist_t;
#define __REFLIST_HEAD(rl) ((rl)->next)
#define __REFLIST_TAIL(rl) ((rl)->prev)

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_inline void tt_reflist_init(IN tt_reflist_t *list)
{
    __REFLIST_HEAD(list) = list;
    __REFLIST_TAIL(list) = list;
}

tt_inline void tt_refnode_init(IN tt_refnode_t *node)
{
    node->prev = NULL;
    node->next = NULL;
}

tt_inline tt_u32_t tt_reflist_count(IN tt_reflist_t *list)
{
    tt_u32_t count = 0;
    tt_refnode_t *node = __REFLIST_HEAD(list);

    // never use while (node != NULL)
    while (node != list) {
        ++count;
        node = node->next;
    }
    return count;
}

tt_inline tt_bool_t tt_reflist_empty(IN tt_reflist_t *list)
{
    return __REFLIST_HEAD((list)) == (list) ? TT_TRUE : TT_FALSE;
}

tt_inline tt_refnode_t *tt_reflist_head(IN tt_reflist_t *list)
{
    return tt_reflist_empty(list) ? NULL : __REFLIST_HEAD(list);
}

tt_inline tt_refnode_t *tt_reflist_tail(IN tt_reflist_t *list)
{
    return tt_reflist_empty(list) ? NULL : __REFLIST_TAIL(list);
}

tt_inline tt_refnode_t *tt_reflist_next(IN tt_reflist_t *list,
                                        IN tt_refnode_t *node)
{
    return node->next == list ? NULL : node->next;
}

tt_inline tt_result_t tt_reflist_pushhead(IN tt_reflist_t *list, IN void *p)
{
    tt_refnode_t *node = tt_mem_alloc(sizeof(tt_refnode_t));
    if (node == NULL) {
        TT_ERROR("no mem for ref node");
        return TT_FAIL;
    }
    node->p = p;

    // later nodes
    __REFLIST_HEAD(list)->prev = node;
    node->next = __REFLIST_HEAD(list);

    // former nodes
    node->prev = list;
    __REFLIST_HEAD(list) = node;

    return TT_SUCCESS;
}

tt_inline tt_result_t tt_reflist_pushtail(IN tt_reflist_t *list, IN void *p)
{
    tt_refnode_t *node = tt_mem_alloc(sizeof(tt_refnode_t));
    if (node == NULL) {
        TT_ERROR("no mem for ref node");
        return TT_FAIL;
    }
    node->p = p;

    // later nodes
    __REFLIST_TAIL(list)->next = node;
    node->prev = __REFLIST_TAIL(list);

    // former nodes
    node->next = list;
    __REFLIST_TAIL(list) = node;

    return TT_SUCCESS;
}

// return false if no more node
tt_inline tt_bool_t tt_reflist_pophead(IN tt_reflist_t *list, OUT OPT void **pp)
{
    tt_refnode_t *head = tt_reflist_head(list);
    if (head != NULL) {
        head->next->prev = head->prev; // = list;
        __REFLIST_HEAD(list) = head->next;

        TT_SAFE_ASSIGN(pp, head->p);
        tt_mem_free(head);
    }
    return TT_BOOL(head != NULL);
}

tt_inline tt_bool_t tt_reflist_poptail(IN tt_reflist_t *list, OUT OPT void **pp)
{
    tt_refnode_t *tail = tt_reflist_tail(list);
    if (tail != NULL) {
        tail->prev->next = tail->next; // = list;
        __REFLIST_TAIL(list) = tail->prev;

        TT_SAFE_ASSIGN(pp, tail->p);
        tt_mem_free(tail);
    }
    return TT_BOOL(tail != NULL);
}

tt_inline void tt_reflist_remove(IN tt_refnode_t *node)
{
    node->next->prev = node->prev;
    node->prev->next = node->next;
    tt_mem_free(node);
}

tt_inline void tt_reflist_merge(IN tt_reflist_t *dst, IN tt_reflist_t *src)
{
    if (tt_reflist_empty(src)) {
        return;
    }

    __REFLIST_TAIL(dst)->next = __REFLIST_HEAD(src);
    __REFLIST_HEAD(src)->prev = __REFLIST_TAIL(dst);

    __REFLIST_TAIL(dst) = __REFLIST_TAIL(src);
    __REFLIST_TAIL(src)->next = dst;

    tt_reflist_init(src);
}

#endif /* __TT_REFERENCE_LIST__ */
