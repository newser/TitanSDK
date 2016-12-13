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
@file tt_aes_native.h
@brief crypto: AES native

this file defines AES native APIs
*/

#ifndef __TT_AES_NATIVE__
#define __TT_AES_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <crypto/tt_aes_def.h>

#ifdef TSCM_PLATFORM_CRYPTO_ENABLE
#include <CommonCrypto/CommonCrypto.h>
#endif

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define __AES_MAX_KEY_SIZE TT_AES256_KEY_SIZE

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_profile_s;

typedef struct tt_aes_ntv_s
{
#ifdef TSCM_PLATFORM_CRYPTO_ENABLE
    CCCryptorRef cref;
#endif

    tt_aes_padding_t padding;
} tt_aes_ntv_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_inline tt_result_t tt_aes_component_init_ntv(IN struct tt_profile_s *profile)
{
    return TT_SUCCESS;
}

extern tt_result_t tt_aes_create_ntv(IN tt_aes_ntv_t *sys_aes,
                                     IN tt_bool_t encrypt,
                                     IN tt_blob_t *key,
                                     IN tt_aes_size_t size,
                                     IN tt_aes_attr_t *attr);

extern void tt_aes_destroy_ntv(IN tt_aes_ntv_t *sys_aes);

extern tt_result_t tt_aes_encrypt_ntv(IN tt_aes_ntv_t *sys_aes,
                                      IN tt_u8_t *input,
                                      IN tt_u32_t input_len,
                                      OUT tt_u8_t *output,
                                      IN OUT tt_u32_t *output_len);

extern tt_result_t tt_aes_decrypt_ntv(IN tt_aes_ntv_t *sys_aes,
                                      IN tt_u8_t *input,
                                      IN tt_u32_t input_len,
                                      OUT tt_u8_t *output,
                                      IN OUT tt_u32_t *output_len);

#endif
