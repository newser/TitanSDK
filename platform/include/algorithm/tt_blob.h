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
 @file tt_blob.h
 @brief blob

 this file blob
 */

#ifndef __TT_BLOB__
#define __TT_BLOB__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <memory/tt_memory_alloc.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

/**
 @struct tt_buf_t
 buffer information
 */
typedef struct tt_blob_s
{
    tt_u8_t *addr;
    tt_u32_t len;
} tt_blob_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

// if addr is NULL, it only allocated memory of size len
tt_inline tt_result_t tt_blob_create(OUT tt_blob_t *blob,
                                     IN OPT tt_u8_t *addr,
                                     IN tt_u32_t len)
{
    tt_u8_t *p;

    // len can not be 0, or use tt_blob_init() instead
    TT_ASSERT(len != 0);

    p = (tt_u8_t *)tt_malloc(len);
    if (p != NULL) {
        if (addr != NULL) {
            tt_memcpy(p, addr, len);
        }
        blob->addr = p;
        blob->len = len;
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_inline void tt_blob_destroy(IN tt_blob_t *blob)
{
    if (blob->addr != NULL) {
        tt_free(blob->addr);
    }
}

tt_inline void tt_blob_init(IN tt_blob_t *blob)
{
    blob->addr = NULL;
    blob->len = 0;
}

tt_inline tt_s32_t tt_blob_cmp(IN tt_blob_t *a, IN tt_blob_t *b)
{
    if (a == b) {
        return 0;
    } else if (a->len < b->len) {
        return -1;
    } else if (a->len > b->len) {
        return 1;
    } else {
        return tt_memcmp(a->addr, b->addr, a->len);
    }
}

tt_inline tt_s32_t tt_blob_strcmp(IN tt_blob_t *blob, IN const tt_char_t *cstr)
{
    tt_u32_t n = (tt_u32_t)tt_strlen(cstr);
    if (blob->len < n) {
        return -1;
    } else if (blob->len > n) {
        return 1;
    } else {
        return tt_memcmp(blob->addr, cstr, n);
    }
}

#endif /* __TT_BLOB__ */
