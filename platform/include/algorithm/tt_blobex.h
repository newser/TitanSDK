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
 @file tt_blobex.h
 @brief bex

 this file bex
 */

#ifndef __TT_BLOBEX__
#define __TT_BLOBEX__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <memory/tt_memory_alloc.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define __BLOBEX_MAX_LEN 0x80000000

#define __BLOBEX_ADDR(b) ((b)->addr)

#define __BLOBEX_LEN(b) (((b)->len) & ~__BLOBEX_MAX_LEN)

#define __BLOBEX_IS_OWNER(b) (((b)->len) & __BLOBEX_MAX_LEN)

#define __BLOBEX_SET_OWNER(b)                                                  \
    do {                                                                       \
        (b)->len |= __BLOBEX_MAX_LEN;                                          \
    } while (0)

#define __BLOBEX_CLEAR_OWNER(b)                                                \
    do {                                                                       \
        (b)->len &= ~__BLOBEX_MAX_LEN;                                         \
    } while (0)

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

/**
 @struct tt_blobex_t
 buffer information
 */
typedef struct tt_blobex_s
{
    tt_u8_t *addr;
    tt_u32_t len;
} tt_blobex_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

// if addr is NULL, it only allocated memory of size len
tt_inline tt_result_t tt_blobex_create(OUT tt_blobex_t *bex,
                                       IN OPT tt_u8_t *addr,
                                       IN tt_u32_t len)
{
    tt_u8_t *p;

    TT_ASSERT((len > 0) && (len < __BLOBEX_MAX_LEN));
    p = (tt_u8_t *)tt_malloc(len);
    if (p != NULL) {
        if (addr != NULL) {
            tt_memcpy(p, addr, len);
        }
        bex->addr = p;
        bex->len = len;
        __BLOBEX_SET_OWNER(bex);
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_inline void tt_blobex_destroy(IN tt_blobex_t *bex)
{
    if ((bex->addr != NULL) && (__BLOBEX_IS_OWNER(bex))) {
        tt_free(__BLOBEX_ADDR(bex));
    }
}

tt_inline void tt_blobex_init(IN tt_blobex_t *bex,
                              IN OPT tt_u8_t *addr,
                              IN tt_u32_t len)
{
    TT_ASSERT(len < __BLOBEX_MAX_LEN);

    bex->addr = addr;
    bex->len = len;
}

tt_inline void *tt_blobex_addr(IN tt_blobex_t *bex)
{
    return __BLOBEX_ADDR(bex);
}

tt_inline tt_u32_t tt_blobex_len(IN tt_blobex_t *bex)
{
    return __BLOBEX_LEN(bex);
}

tt_inline tt_bool_t tt_blobex_empty(IN tt_blobex_t *bex)
{
    return TT_BOOL((tt_blobex_addr(bex) == NULL) || (tt_blobex_len(bex) == 0));
}

tt_inline tt_result_t tt_blobex_set(IN tt_blobex_t *bex,
                                    IN OPT tt_u8_t *addr,
                                    IN tt_u32_t len,
                                    IN tt_bool_t owner)
{
    tt_blobex_destroy(bex);
    if (owner) {
        return tt_blobex_create(bex, addr, len);
    } else {
        tt_blobex_init(bex, addr, len);
        return TT_SUCCESS;
    }
}

tt_inline void tt_blobex_clear(IN tt_blobex_t *bex)
{
    tt_blobex_destroy(bex);
    tt_blobex_init(bex, NULL, 0);
}

tt_inline tt_s32_t tt_blobex_cmp(IN tt_blobex_t *a, IN tt_blobex_t *b)
{
    if (a == b) {
        return 0;
    } else if (__BLOBEX_LEN(a) < __BLOBEX_LEN(b)) {
        return -1;
    } else if (__BLOBEX_LEN(a) > __BLOBEX_LEN(b)) {
        return 1;
    } else {
        return tt_memcmp(__BLOBEX_ADDR(a), __BLOBEX_ADDR(b), __BLOBEX_LEN(a));
    }
}

tt_inline tt_s32_t tt_blobex_strcmp(IN tt_blobex_t *bex,
                                    IN const tt_char_t *cstr)
{
    tt_u32_t n = (tt_u32_t)tt_strlen(cstr);
    if (__BLOBEX_LEN(bex) < n) {
        return -1;
    } else if (__BLOBEX_LEN(bex) > n) {
        return 1;
    } else {
        return tt_memcmp(__BLOBEX_ADDR(bex), cstr, __BLOBEX_LEN(bex));
    }
}

tt_inline tt_s32_t tt_blobex_memcmp(IN tt_blobex_t *a,
                                    IN void *addr,
                                    IN tt_u32_t len)
{
    if (__BLOBEX_LEN(a) < len) {
        return -1;
    } else if (__BLOBEX_LEN(a) > len) {
        return 1;
    } else {
        return tt_memcmp(__BLOBEX_ADDR(a), addr, len);
    }
}

tt_inline tt_result_t tt_blobex_memcat(IN tt_blobex_t *bex,
                                       IN const tt_u8_t *addr,
                                       IN tt_u32_t len)
{
    if (len > 0) {
        tt_u32_t cur_len = __BLOBEX_LEN(bex);
        tt_u8_t *p = tt_malloc(cur_len + len);
        if (p == NULL) {
            TT_ERROR("no mem for blobex memcat");
            return TT_SUCCESS;
        }
        if (cur_len > 0) {
            tt_memcpy(p, __BLOBEX_ADDR(bex), cur_len);
        }
        if (len > 0) {
            tt_memcpy(p + cur_len, addr, len);
        }
        tt_blobex_destroy(bex);
        bex->addr = p;
        bex->len = cur_len + len;
        __BLOBEX_SET_OWNER(bex);
    }
    return TT_SUCCESS;
}

tt_inline tt_result_t tt_blobex_own(IN tt_blobex_t *bex)
{
    if (!tt_blobex_empty(bex) && !__BLOBEX_IS_OWNER(bex)) {
        tt_u8_t *p = tt_memdup(__BLOBEX_ADDR(bex), __BLOBEX_LEN(bex));
        if (p == NULL) {
            TT_ERROR("no mem for blobex own");
            return TT_E_NOMEM;
        }

        bex->addr = p;
        __BLOBEX_SET_OWNER(bex);
    }
    return TT_SUCCESS;
}

// if src is owner, dst will alloc new memory, as src's memory must be
// dynamically allocated, else dst simply refer src's memory
tt_inline tt_result_t tt_blobex_smart_copy(IN tt_blobex_t *dst,
                                           IN tt_blobex_t *src)
{
    return tt_blobex_set(dst,
                         tt_blobex_addr(src),
                         tt_blobex_len(src),
                         TT_BOOL(__BLOBEX_IS_OWNER(src)));
}

#if 1
tt_inline void tt_blobex_take(IN tt_blobex_t *bex,
                              IN TO tt_u8_t *addr,
                              IN tt_u32_t len)
{
    tt_blobex_destroy(bex);
    tt_blobex_init(bex, addr, len);
    // take the ownership
    __BLOBEX_SET_OWNER(bex);
}

tt_inline void tt_blobex_take_blobex(IN tt_blobex_t *dst, IN tt_blobex_t *src)
{
    tt_blobex_destroy(dst);
    tt_memcpy(dst, src, sizeof(tt_blobex_t));
    tt_blobex_init(src, NULL, 0);
}
#endif

#endif /* __TT_BLOBEX__ */
