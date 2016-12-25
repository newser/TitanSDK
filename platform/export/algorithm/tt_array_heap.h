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
@file tt_array_heap.h
@brief heap implemented as array

this file defines heap data structure and related operations.
*/

#ifndef __TT_ARRAY_HEAP__
#define __TT_ARRAY_HEAP__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_compare.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct
{
    tt_u32_t initial_node_num;
} tt_arheap_attr_t;

typedef struct
{
    tt_ptr_t data;

    tt_s32_t *node_idx;
#define TT_ARHEAP_NODE_NULL (-1)

} tt_ahnode_t;

typedef struct
{
    tt_ahnode_t *node;
    tt_u32_t node_num;
    // tail_idx is initally 0
    tt_u32_t tail_idx;

    /**
    @var comparer
    - return -1 if "l" < "r" to make a max heap
    - return 1 if "l" < "r" to make a min heap
    */
    tt_cmp_t comparer;

    tt_arheap_attr_t attr;
} tt_arheap_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_arheap_create(IN tt_arheap_t *heap,
                                    IN tt_cmp_t comparer,
                                    IN tt_arheap_attr_t *attr);

extern tt_result_t tt_arheap_destroy(IN tt_arheap_t *heap);

extern void tt_arheap_attr_default(IN tt_arheap_attr_t *attr);

tt_inline tt_bool_t tt_arheap_empty(IN tt_arheap_t *heap)
{
    return heap->tail_idx == 0 ? TT_TRUE : TT_FALSE;
}

// - the value of returned node_idx could be used as a key to query
//   the entry in heap, i.e. passing to tt_arheap_remove() to remove
//   the inserted entry
extern tt_result_t tt_arheap_add(IN tt_arheap_t *heap,
                                 IN tt_ptr_t data,
                                 OUT tt_s32_t *node_idx);

extern void tt_arheap_remove(IN tt_arheap_t *heap, IN tt_s32_t node_idx);

// tt_arheap_fix is to fix heap when the key value of a node that is
// in the heap is modified. by such method, app need not first remove
// then change then insert
extern void tt_arheap_fix(IN tt_arheap_t *heap, IN tt_s32_t node_idx);

tt_inline void *tt_arheap_head(IN tt_arheap_t *heap)
{
    return heap->tail_idx == 0 ? NULL : heap->node[0].data;
}

/**
@fn tt_ptr_t tt_arheap_pophead(IN tt_arheap_t *heap)
pop the head of array heap

@param [in] heap array heap

@return
the head of array heap

@note
- if array heap is empty, it returns NULL
- if value of current head of the heap is NULL, it also returns NULL
*/
extern void *tt_arheap_pophead(IN tt_arheap_t *heap);

#endif
