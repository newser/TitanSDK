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

#ifdef TT_MEMORY_TAG_ENABLE

#define tt_malloc(s) tt_malloc_tag((s), __FILE__, __FUNCTION__, __LINE__)

#define tt_realloc(p, s)                                                       \
    tt_realloc_tag((p), (s), __FILE__, __FUNCTION__, __LINE__)

#define tt_zalloc(s) tt_zalloc_tag((s), __FILE__, __FUNCTION__, __LINE__)

#define tt_xmalloc(s) tt_xmalloc_tag((s), __FILE__, __FUNCTION__, __LINE__)

#define tt_free(p) tt_free_tag((p))

#else

#define tt_malloc(s) tt_malloc_tag((s))

#define tt_realloc(p, s) tt_realloc_tag((p), (s))

#define tt_zalloc(s) tt_zalloc_tag((s))

#define tt_xmalloc(s) tt_xmalloc_tag((s))

#define tt_free tt_c_free

#endif

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef void *(*tt_oom_handler_t)(IN void *param);

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export void tt_memory_tag_component_register();

tt_export void tt_memory_tag_dump(IN tt_u32_t flag);
#define TT_MEMORY_TAG_EACH (1 << 0)
#define TT_MEMORY_TAG_TOTAL (1 << 1)

tt_export void tt_set_oom_handler(IN tt_oom_handler_t handler, IN void *param);

#ifdef TT_MEMORY_TAG_ENABLE

tt_export void *tt_malloc_tag(IN size_t size,
                              IN const tt_char_t *file,
                              IN const tt_char_t *function,
                              IN const tt_u32_t line);

tt_export void tt_free_tag(IN void *p);

tt_inline void *tt_realloc_tag(IN OPT void *ptr,
                               IN size_t size,
                               IN const tt_char_t *file,
                               IN const tt_char_t *function,
                               IN const tt_u32_t line)
{
    void *p = tt_malloc_tag(size, file, function, line);
    if (p != NULL) {
        if (ptr != NULL) {
            tt_memcpy(p, ptr, size);
            tt_free_tag(ptr);
        }
    }
    return p;
}

tt_inline void *tt_zalloc_tag(IN tt_size_t size,
                              IN const tt_char_t *file,
                              IN const tt_char_t *function,
                              IN const tt_u32_t line)
{
    void *p = tt_malloc_tag(size, file, function, line);
    if (p != NULL) {
        tt_memset(p, 0, size);
    }
    return p;
}

tt_inline void *tt_xmalloc_tag(IN size_t size,
                               IN const tt_char_t *file,
                               IN const tt_char_t *function,
                               IN const tt_u32_t line)
{
    void *p = tt_malloc_tag(size, file, function, line);
    TT_ASSERT_ALWAYS(p != NULL);
    return p;
}

#else

tt_export void *tt_malloc_tag(IN size_t size);

tt_inline void *tt_realloc_tag(IN void *ptr, IN size_t size)
{
    void *p = tt_malloc_tag(size);
    if (p != NULL) {
        if (ptr != NULL) {
            tt_memcpy(p, ptr, size);
            tt_free(ptr);
        }
    }
    return p;
}

tt_inline void *tt_zalloc_tag(IN tt_size_t size)
{
    void *p = tt_malloc_tag(size);
    if (p != NULL) {
        tt_memset(p, 0, size);
    }
    return p;
}

tt_inline void *tt_xmalloc_tag(IN size_t size)
{
    void *p = tt_malloc_tag(size);
    TT_ASSERT_ALWAYS(p != NULL);
    return p;
}

#endif

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
