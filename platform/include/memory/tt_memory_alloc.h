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
@file tt_memory_alloc.h
@brief memory allocation APIs

APIs to allocate/free memory
*/

#ifndef __TT_MEMORY_ALLOC__
#define __TT_MEMORY_ALLOC__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <misc/tt_util.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define tt_malloc tt_c_malloc

#define tt_realloc tt_c_realloc

#define tt_xmalloc tt_malloc

#define tt_free tt_c_free

// real size to be allocated when caller is requiring s bytes
#define tt_msize(s) (s)

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_inline void *tt_zalloc(IN tt_size_t size)
{
    void *p = tt_malloc(size);
    if (p != NULL) {
        tt_memset(p, 0, size);
    }
    return p;
}

tt_inline void *tt_malloc_align(IN tt_size_t size, IN tt_u32_t order)
{
    void *p = tt_malloc(size + (((tt_size_t)1) << (order + 1)) +
                        sizeof(tt_uintptr_t));
    if (p != NULL) {
        void *org = p;
        p = TT_PTR_INC(void, p, sizeof(tt_uintptr_t));
        TT_PTR_ALIGN_INC(p, order);
        *TT_PTR_DEC(void *, p, sizeof(tt_uintptr_t)) = org;
    }
    return p;
}

tt_inline void tt_free_align(IN void *p)
{
    tt_free(*TT_PTR_DEC(void *, p, sizeof(tt_uintptr_t)));
}

tt_inline void *tt_xmalloc_align(IN tt_size_t size, IN tt_u32_t order)
{
    void *p, *org;

    p = tt_xmalloc(size + (((tt_size_t)1) << (order + 1)) +
                   sizeof(tt_uintptr_t));
    org = p;
    p = TT_PTR_INC(void, p, sizeof(tt_uintptr_t));
    TT_PTR_ALIGN_INC(p, order);
    *TT_PTR_DEC(void *, p, sizeof(tt_uintptr_t)) = org;

    return p;
}

#endif // __TT_MEMORY_ALLOC__
