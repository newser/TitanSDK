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
@file tt_public_key.h
@brief public key definitions

this file defines public key APIs
*/

#ifndef __TT_PUBLIC_KEY__
#define __TT_PUBLIC_KEY__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

#include <pk.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef enum {
    TT_RSA,
    TT_ECKEY,

    TT_PK_TYPE_NUM
} tt_pk_type_t;
#define TT_PK_TYPE_VALID(t) ((t) < TT_PK_TYPE_NUM)

typedef struct tt_pk_s
{
    mbedtls_pk_context ctx;
} tt_pk_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern void tt_pk_init(IN tt_pk_t *pk);

extern void tt_pk_destroy(IN tt_pk_t *pk);

extern tt_result_t tt_pk_load_public(IN tt_pk_t *pk,
                                     IN tt_u8_t *key,
                                     IN tt_u32_t len);

extern tt_result_t tt_pk_load_private(IN tt_pk_t *pk,
                                      IN tt_u8_t *key,
                                      IN tt_u32_t key_len,
                                      IN OPT tt_u8_t *pwd,
                                      IN tt_u32_t pwd_len);

extern tt_result_t tt_pk_load_public_file(IN tt_pk_t *pk,
                                          IN const tt_char_t *path);

extern tt_result_t tt_pk_load_private_file(IN tt_pk_t *pk,
                                           IN const tt_char_t *path,
                                           IN OPT const tt_u8_t *pwd,
                                           IN tt_u32_t pwd_len);

extern tt_result_t tt_pk_check(IN tt_pk_t *pub, IN tt_pk_t *priv);

extern tt_pk_type_t tt_pk_get_type(IN tt_pk_t *pk);

extern int tt_pk_rng(IN void *param, IN unsigned char *buf, IN size_t len);

#endif
