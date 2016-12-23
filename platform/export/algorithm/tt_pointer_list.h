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
 @brief pointer list
 this file defines apis of pointer list data structure.
 */

#ifndef __TT_POINTER_LIST__
#define __TT_POINTER_LIST__

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

typedef struct tt_ptrnode_s
{
    struct tt_ptrnode_s *prev;
    struct tt_ptrnode_s *next;
    void *p;
} tt_ptrnode_t;

typedef tt_ptrnode_t tt_ptrlist_t;
#define __PTRLIST_HEAD(rl) ((rl)->next)
#define __PTRLIST_TAIL(rl) ((rl)->prev)

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_inline void tt_ptrlist_init(IN tt_ptrlist_t *list)
{
    __PTRLIST_HEAD(list) = list;
    __PTRLIST_TAIL(list) = list;
}

tt_inline void tt_ptrnode_init(IN tt_ptrnode_t *node)
{
    node->prev = NULL;
    node->next = NULL;
}

tt_inline tt_u32_t tt_ptrlist_count(IN tt_ptrlist_t *list)
{
    tt_u32_t count = 0;
    tt_ptrnode_t *node = __PTRLIST_HEAD(list);

    // never use while (node != NULL)
    while (node != list) {
        ++count;
        node = node->next;
    }
    return count;
}

tt_inline tt_bool_t tt_ptrlist_empty(IN tt_ptrlist_t *list)
{
    return __PTRLIST_HEAD((list)) == (list) ? TT_TRUE : TT_FALSE;
}

tt_inline tt_ptrnode_t *tt_ptrlist_head(IN tt_ptrlist_t *list)
{
    return tt_ptrlist_empty(list) ? NULL : __PTRLIST_HEAD(list);
}

tt_inline tt_ptrnode_t *tt_ptrlist_tail(IN tt_ptrlist_t *list)
{
    return tt_ptrlist_empty(list) ? NULL : __PTRLIST_TAIL(list);
}

tt_inline tt_ptrnode_t *tt_ptrlist_next(IN tt_ptrlist_t *list,
                                        IN tt_ptrnode_t *node)
{
    return node->next == list ? NULL : node->next;
}

tt_inline tt_result_t tt_ptrlist_pushhead(IN tt_ptrlist_t *list, IN void *p)
{
    tt_ptrnode_t *node = tt_mem_alloc(sizeof(tt_ptrnode_t));
    if (node == NULL) {
        return TT_FAIL;
    }
    node->p = p;

    // later nodes
    __PTRLIST_HEAD(list)->prev = node;
    node->next = __PTRLIST_HEAD(list);

    // former nodes
    node->prev = list;
    __PTRLIST_HEAD(list) = node;

    return TT_SUCCESS;
}

tt_inline tt_result_t tt_ptrlist_pushtail(IN tt_ptrlist_t *list, IN void *p)
{
    tt_ptrnode_t *node = tt_mem_alloc(sizeof(tt_ptrnode_t));
    if (node == NULL) {
        return TT_FAIL;
    }
    node->p = p;

    // later nodes
    __PTRLIST_TAIL(list)->next = node;
    node->prev = __PTRLIST_TAIL(list);

    // former nodes
    node->next = list;
    __PTRLIST_TAIL(list) = node;

    return TT_SUCCESS;
}

// return false if no more node
tt_inline tt_bool_t tt_ptrlist_pophead(IN tt_ptrlist_t *list, OUT OPT void **pp)
{
    tt_ptrnode_t *head = tt_ptrlist_head(list);
    if (head != NULL) {
        head->next->prev = head->prev; // = list;
        __PTRLIST_HEAD(list) = head->next;

        TT_SAFE_ASSIGN(pp, head->p);
        tt_mem_free(head);
    }
    return TT_BOOL(head != NULL);
}

tt_inline tt_bool_t tt_ptrlist_poptail(IN tt_ptrlist_t *list, OUT OPT void **pp)
{
    tt_ptrnode_t *tail = tt_ptrlist_tail(list);
    if (tail != NULL) {
        tail->prev->next = tail->next; // = list;
        __PTRLIST_TAIL(list) = tail->prev;

        TT_SAFE_ASSIGN(pp, tail->p);
        tt_mem_free(tail);
    }
    return TT_BOOL(tail != NULL);
}

tt_inline void tt_ptrlist_remove(IN tt_ptrnode_t *node)
{
    node->next->prev = node->prev;
    node->prev->next = node->next;
    tt_mem_free(node);
}

tt_inline void tt_ptrlist_merge(IN tt_ptrlist_t *dst, IN tt_ptrlist_t *src)
{
    if (tt_ptrlist_empty(src)) {
        return;
    }

    __PTRLIST_TAIL(dst)->next = __PTRLIST_HEAD(src);
    __PTRLIST_HEAD(src)->prev = __PTRLIST_TAIL(dst);

    __PTRLIST_TAIL(dst) = __PTRLIST_TAIL(src);
    __PTRLIST_TAIL(src)->next = dst;

    tt_ptrlist_init(src);
}

#endif /* __TT_POINTER_LIST__ */
