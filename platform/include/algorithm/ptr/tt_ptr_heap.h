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
@file tt_ptr_heap.h
@brief ph implemented as array

this file defines ph data structure and related operations.

 - add
 - clear
 - contain
 - count
 - pop
 - head
 - remove
*/

#ifndef __TT_PTR_HEAP__
#define __TT_PTR_HEAP__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_compare.h>
#include <memory/tt_memory_spring.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct
{
    tt_u32_t min_extent_num;
    tt_u32_t max_extent_num;
    tt_u32_t max_limit_num;
} tt_ptrheap_attr_t;

typedef struct
{
    tt_ptr_t p;
    tt_u32_t *idx;
} tt_phnode_t;

typedef struct tt_ptrheap_s
{
    union
    {
        tt_u8_t *p;
        tt_phnode_t *node;
    };
    tt_cmp_t cmp;
    tt_memspg_t mspg;
    tt_u32_t size;
    tt_u32_t capacity;
    tt_u32_t count;
} tt_ptrheap_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export void tt_ptrheap_init(IN tt_ptrheap_t *ph,
                               IN OPT tt_cmp_t cmp,
                               IN OPT tt_ptrheap_attr_t *attr);

tt_export void tt_ptrheap_destroy(IN tt_ptrheap_t *ph);

tt_export void tt_ptrheap_attr_default(IN tt_ptrheap_attr_t *attr);

tt_export tt_result_t __ptrheap_reserve(IN tt_ptrheap_t *ph, IN tt_u32_t count);

tt_inline tt_result_t tt_ptrheap_reserve(IN tt_ptrheap_t *ph, IN tt_u32_t count)
{
    if ((ph->count + count) <= ph->capacity) {
        return TT_SUCCESS;
    } else {
        return __ptrheap_reserve(ph, count);
    }
}

// tt_ptrheap_fix is to fix ph when the key value of a node that is
// in the ph is modified. by such method, app need not first remove
// then change then insert
tt_export void tt_ptrheap_fix(IN tt_ptrheap_t *ph, IN tt_u32_t idx);

tt_inline tt_u32_t tt_ptrheap_count(IN tt_ptrheap_t *ph)
{
    return ph->count;
}

tt_inline tt_bool_t tt_ptrheap_empty(IN tt_ptrheap_t *ph)
{
    return tt_ptrheap_count(ph) == 0 ? TT_TRUE : TT_FALSE;
}

// NOTE if a non-empty heap is cleared, then all idx returned by
// tt_ptrheap_add becomes invalid
tt_export void tt_ptrheap_clear(IN tt_ptrheap_t *ph);

// - the value of returned idx could be used as a key to query
//   the entry in ph, i.e. passing to tt_ptrheap_remove() to remove
//   the inserted entry
tt_export tt_result_t tt_ptrheap_add(IN tt_ptrheap_t *ph,
                                     IN tt_ptr_t p,
                                     OUT tt_u32_t *idx);

tt_export void tt_ptrheap_remove(IN tt_ptrheap_t *ph, IN tt_u32_t idx);

tt_export tt_bool_t tt_ptrheap_contain(IN tt_ptrheap_t *ph, IN tt_ptr_t p);

tt_inline tt_ptr_t tt_ptrheap_head(IN tt_ptrheap_t *ph)
{
    return ph->count == 0 ? NULL : ph->node[0].p;
}

/**
@fn tt_ptr_t tt_ptrheap_pop(IN tt_ptrheap_t *ph)
pop the head of array ph

@param [in] ph array ph

@return
the head of array ph

@note
- if array ph is empty, it returns NULL
- if value of current head of the ph is NULL, it also returns NULL
*/
tt_export tt_ptr_t tt_ptrheap_pop(IN tt_ptrheap_t *ph);

#endif
