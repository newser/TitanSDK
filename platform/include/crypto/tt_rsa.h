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
@file tt_rsa.h
@brief rsa

this file defines rsa APIs
*/

#ifndef __TT_RSA__
#define __TT_RSA__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <crypto/tt_message_digest.h>

#include <rsa.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_pk_s;

typedef struct
{
    mbedtls_rsa_context ctx;
} tt_rsa_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern void tt_rsa_init(IN tt_rsa_t *rsa);

extern void tt_rsa_destroy(IN tt_rsa_t *rsa);

extern tt_result_t tt_rsa_load(IN tt_rsa_t *rsa, IN struct tt_pk_s *pk);

extern tt_result_t tt_rsa_generate(OUT tt_rsa_t *pub,
                                   OUT tt_rsa_t *priv,
                                   IN tt_u32_t bit_num,
                                   IN tt_u32_t exponent);

extern tt_result_t tt_rsa_check(IN tt_rsa_t *pub, IN tt_rsa_t *priv);

// pkcs1 v1.5
extern tt_result_t tt_rsa_encrypt_pkcs1(IN tt_rsa_t *rsa,
                                        IN tt_u8_t *input,
                                        IN tt_u32_t ilen,
                                        IN tt_u8_t *output);

extern tt_result_t tt_rsa_decrypt_pkcs1(IN tt_rsa_t *rsa,
                                        IN tt_u8_t *input,
                                        IN tt_u8_t *output,
                                        IN OUT tt_u32_t *olen);

extern tt_result_t tt_rsa_sign_pkcs1(IN tt_rsa_t *rsa,
                                     IN tt_u8_t *input,
                                     IN tt_u32_t ilen,
                                     IN tt_md_type_t md_type,
                                     IN tt_u8_t *sig);

extern tt_result_t tt_rsa_verify_pkcs1(IN tt_rsa_t *rsa,
                                       IN tt_u8_t *input,
                                       IN tt_u32_t ilen,
                                       IN tt_md_type_t md_type,
                                       IN tt_u8_t *sig);

// pkcs1 v2.1
extern tt_result_t tt_rsa_encrypt_oaep(IN tt_rsa_t *rsa,
                                       IN tt_u8_t *input,
                                       IN tt_u32_t ilen,
                                       IN const tt_u8_t *label,
                                       IN tt_u32_t label_len,
                                       IN tt_md_type_t md_type,
                                       IN tt_u8_t *output);

extern tt_result_t tt_rsa_decrypt_oaep(IN tt_rsa_t *rsa,
                                       IN tt_u8_t *input,
                                       IN const tt_u8_t *label,
                                       IN tt_u32_t label_len,
                                       IN tt_md_type_t md_type,
                                       IN tt_u8_t *output,
                                       IN tt_u32_t *olen);

extern tt_result_t tt_rsa_sign_pss(IN tt_rsa_t *rsa,
                                   IN tt_u8_t *input,
                                   IN tt_u32_t ilen,
                                   IN tt_md_type_t md_type,
                                   IN tt_u8_t *sig);

extern tt_result_t tt_rsa_verify_pss(IN tt_rsa_t *rsa,
                                     IN tt_u8_t *input,
                                     IN tt_u32_t ilen,
                                     IN tt_md_type_t md_type,
                                     IN tt_u8_t *sig);

#endif
