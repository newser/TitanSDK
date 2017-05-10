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
@file tt_crypto_pad.h
@brief crypto padding

this file defines crypto padding APIs
*/

#ifndef __TT_CRYPTO_PAD__
#define __TT_CRYPTO_PAD__

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

typedef enum {
    TT_CRYPTO_PAD_NONE,
    TT_CRYPTO_PAD_PKCS7,

    TT_CRYPTO_PAD_NUM
} tt_crypto_pad_t;
#define TT_CRYPTO_PAD_VALID(p) ((p) < TT_CRYPTO_PAD_NUM)

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

// - size of tail must be at least block bytes
extern tt_result_t tt_crypto_pad(IN tt_crypto_pad_t pad,
                                 IN tt_u8_t block,
                                 IN tt_u8_t *data,
                                 IN OUT tt_u32_t *data_len,
                                 IN OUT tt_u8_t *tail,
                                 IN OUT tt_u32_t *tail_len);

extern tt_result_t tt_crypto_unpad(IN tt_crypto_pad_t pad,
                                   IN tt_u8_t block,
                                   IN tt_u8_t *data,
                                   IN OUT tt_u32_t *data_len);

#endif /* __TT_CRYPTO_PAD__ */
