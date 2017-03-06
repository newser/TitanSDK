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
@file tt_rsa_native.h
@brief crypto: RSA native

this file defines RSA native APIs
*/

#ifndef __TT_RSA_NATIVE__
#define __TT_RSA_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_blob.h>
#include <crypto/tt_rsa_def.h>

#ifdef TTCM_PLATFORM_CRYPTO_ENABLE
#include <Security/Security.h>
#endif

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_profile_s;
struct tt_rsa_attr_s;
struct tt_rsa_number_s;

typedef struct
{
#ifdef TTCM_PLATFORM_CRYPTO_ENABLE
    // on mac os, TT_RSA_TYPE_PRIVATE may have both pub_key and
    // priv_key. while TT_RSA_TYPE_PUBLIC only has pub_key
    SecKeyRef pub_key;
    SecKeyRef priv_key;
#endif
    tt_rsa_type_t type;
} tt_rsa_ntv_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_rsa_component_init_ntv(IN struct tt_profile_s *profile);

extern tt_result_t tt_rsa_generate_ntv(OUT tt_rsa_ntv_t *sys_rsa,
                                       IN tt_rsa_size_t size,
                                       IN struct tt_rsa_attr_s *attr);

extern tt_result_t tt_rsa_create_ntv(IN tt_rsa_ntv_t *sys_rsa,
                                     IN tt_rsa_format_t key_format,
                                     IN tt_rsa_type_t key_type,
                                     IN tt_blob_t *key_data,
                                     IN OPT struct tt_rsa_attr_s *attr);

extern void tt_rsa_destroy_ntv(IN tt_rsa_ntv_t *sys_rsa);

extern tt_result_t tt_rsa_encrypt_ntv(IN tt_rsa_ntv_t *sys_rsa,
                                      IN tt_u8_t *input,
                                      IN tt_u32_t input_len,
                                      OUT tt_u8_t *output,
                                      IN OUT tt_u32_t *output_len);

extern tt_result_t tt_rsa_decrypt_ntv(IN tt_rsa_ntv_t *sys_rsa,
                                      IN tt_u8_t *input,
                                      IN tt_u32_t input_len,
                                      OUT tt_u8_t *output,
                                      IN OUT tt_u32_t *output_len);

extern tt_result_t tt_rsa_sign_ntv(IN tt_rsa_ntv_t *sys_rsa,
                                   IN tt_u8_t *input,
                                   IN tt_u32_t input_len,
                                   OUT tt_u8_t *signature,
                                   IN OUT tt_u32_t *signature_len);
extern tt_result_t tt_rsa_verify_ntv(IN tt_rsa_ntv_t *sys_rsa,
                                     IN tt_u8_t *input,
                                     IN tt_u32_t input_len,
                                     IN tt_u8_t *signature,
                                     IN tt_u32_t signature_len);

extern tt_result_t tt_rsa_show_ntv(IN tt_rsa_ntv_t *sys_rsa);

tt_inline tt_rsa_type_t tt_rsa_get_type_ntv(IN tt_rsa_ntv_t *sys_rsa)
{
    return sys_rsa->type;
}

extern tt_result_t tt_rsa_get_number_ntv(IN tt_rsa_ntv_t *sys_rsa,
                                         IN struct tt_rsa_number_s *rsa_number);

#endif /* __TT_RSA_NATIVE__ */
