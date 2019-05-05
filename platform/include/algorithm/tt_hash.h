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
 @file tt_hash.h
 @brief hash

 this file hash definitions
 */

#ifndef __TT_HASH__
#define __TT_HASH__

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

typedef enum
{
    TT_HASH_ALG_MURMUR3,
    TT_HASH_ALG_FNV1A,

    TT_HASH_ALG_NUM
} tt_hash_alg_t;
#define TT_HASH_ALG_VALID(t) ((t) < TT_HASH_ALG_NUM)

typedef tt_u32_t tt_hashcode_t;

typedef union
{
    tt_u32_t seed;
} tt_hashctx_t;

typedef tt_hashcode_t (*tt_hash_t)(IN const tt_u8_t *key, IN tt_u32_t key_len,
                                   IN tt_hashctx_t *hctx);

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export void tt_hashctx_init(IN tt_hashctx_t *hctx);

tt_export tt_hashcode_t tt_hash_murmur3(IN const tt_u8_t *key,
                                        IN tt_u32_t key_len,
                                        IN tt_hashctx_t *hctx);

tt_export tt_hashcode_t tt_hash_fnv1a(IN const tt_u8_t *key,
                                      IN tt_u32_t key_len,
                                      IN tt_hashctx_t *hctx);

#endif /* __TT_HASH__ */
