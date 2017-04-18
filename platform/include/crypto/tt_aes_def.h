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
@file tt_aes_def.h
@brief aes definitions

this file defines aes
*/

#ifndef __TT_AES_DEF__
#define __TT_AES_DEF__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_blob.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_AES_BLOCK_SIZE 16
#define TT_AES_IV_SIZE 16

#define TT_AES128_KEY_SIZE 16
#define TT_AES256_KEY_SIZE 32

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef enum {
    TT_AES_SIZE_128,
    TT_AES_SIZE_256,

    TT_AES_SIZE_NUM
} tt_aes_size_t;
#define TT_AES_SIZE_VALID(s) ((s) < TT_AES_SIZE_NUM)

typedef enum {
    TT_AES_PADDING_NONE,
    TT_AES_PADDING_PKCS7,

    TT_AES_PADDING_NUM
} tt_aes_padding_t;
#define TT_AES_PADDING_VALID(p) ((p) < TT_AES_PADDING_NUM)

typedef enum {
    TT_AES_MODE_CBC,

    TT_AES_MODE_NUM
} tt_aes_mode_t;
#define TT_AES_MODE_VALID(m) ((m) < TT_AES_MODE_NUM)

typedef struct
{
    tt_blob_t ivec;
} tt_aes_mode_cbc_t;

typedef struct
{
    tt_aes_padding_t padding;

    tt_aes_mode_t mode;
    union
    {
        tt_aes_mode_cbc_t cbc;
    };
} tt_aes_attr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif
