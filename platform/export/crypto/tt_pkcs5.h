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
@file tt_pkcs5.h
@brief pkcs5

this file defines pkcs5 api
*/

#ifndef __TT_PKCS5__
#define __TT_PKCS5__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_blob.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef enum {
    TT_PBES_VER_1,
    TT_PBES_VER_2,

    TT_PBES_VER_NUM
} tt_pbes_ver_t;
#define TT_PBES_VER_VALID(p) ((p) < TT_PBES_VER_NUM)

typedef enum {
    TT_PBKDF_VER_1,
    TT_PBKDF_VER_2,

    TT_PBKDF_VER_NUM
} tt_pbkdf_ver_t;
#define TT_PBKDF_VER_VALID(p) ((p) < TT_PBKDF_VER_NUM)

typedef enum {
    TT_PKCS5_PRF_HMAC_SHA1,

    TT_PKCS5_PRF_NUM,
} tt_pkcs5_prf_t;
#define TT_PKCS5_PRF_VALID(p) ((p) < TT_PKCS5_PRF_NUM)

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/*
- derived_key must have at least derived_key_len bytes space
*/
extern tt_result_t tt_pbkdf2(IN tt_pkcs5_prf_t prf,
                             IN tt_blob_t *password,
                             IN tt_blob_t *salt,
                             IN tt_u32_t iter_count,
                             IN tt_u32_t derived_key_len,
                             OUT tt_u8_t *derived_key);

#endif /* __TT_PKCS5__ */
