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
@file tt_cipher.h
@brief encrypt & decrypt

this file defines encrypt and decrypt APIs
*/

#ifndef __TT_CIPHER__
#define __TT_CIPHER__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <misc/tt_assert.h>

#include <cipher.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_buf_s;

typedef enum {
    TT_CIPHER_NULL,
    TT_AES_128_ECB,
    TT_AES_192_ECB,
    TT_AES_256_ECB,
    TT_AES_128_CBC,
    TT_AES_192_CBC,
    TT_AES_256_CBC,
    TT_AES_128_CFB128,
    TT_AES_192_CFB128,
    TT_AES_256_CFB128,
    TT_AES_128_CTR,
    TT_AES_192_CTR,
    TT_AES_256_CTR,
    TT_AES_128_GCM,
    TT_AES_192_GCM,
    TT_AES_256_GCM,
    TT_CAMELLIA_128_ECB,
    TT_CAMELLIA_192_ECB,
    TT_CAMELLIA_256_ECB,
    TT_CAMELLIA_128_CBC,
    TT_CAMELLIA_192_CBC,
    TT_CAMELLIA_256_CBC,
    TT_CAMELLIA_128_CFB128,
    TT_CAMELLIA_192_CFB128,
    TT_CAMELLIA_256_CFB128,
    TT_CAMELLIA_128_CTR,
    TT_CAMELLIA_192_CTR,
    TT_CAMELLIA_256_CTR,
    TT_CAMELLIA_128_GCM,
    TT_CAMELLIA_192_GCM,
    TT_CAMELLIA_256_GCM,
    TT_DES_ECB,
    TT_DES_CBC,
    TT_DES_EDE_ECB,
    TT_DES_EDE_CBC,
    TT_DES_EDE3_ECB,
    TT_DES_EDE3_CBC,
    TT_BLOWFISH_ECB,
    TT_BLOWFISH_CBC,
    TT_BLOWFISH_CFB64,
    TT_BLOWFISH_CTR,
    TT_ARC4_128,
    TT_AES_128_CCM,
    TT_AES_192_CCM,
    TT_AES_256_CCM,
    TT_CAMELLIA_128_CCM,
    TT_CAMELLIA_192_CCM,
    TT_CAMELLIA_256_CCM,

    TT_CIPHER_TYPE_NUM
} tt_cipher_type_t;
#define TT_CIPHER_TYPE_VALID(i) ((i) < TT_CIPHER_TYPE_NUM)

typedef enum {
    TT_PADDING_PKCS7,
    TT_PADDING_ONE_AND_ZEROS,
    TT_PADDING_ZEROS_AND_LEN,
    TT_PADDING_ZEROS,
    TT_PADDING_NONE,

    TT_PADDING_NUM
} tt_cipher_padding_t;
#define TT_CIPHER_PADDING_VALID(i) ((i) < TT_PADDING_NUM)

typedef struct
{
    mbedtls_cipher_context_t ctx;
} tt_cipher_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_inline void tt_cipher_init(IN tt_cipher_t *cipher)
{
    TT_ASSERT(cipher != NULL);
    mbedtls_cipher_init(&cipher->ctx);
}

tt_inline void tt_cipher_destroy(IN tt_cipher_t *cipher)
{
    mbedtls_cipher_free(&cipher->ctx);
}

extern tt_result_t tt_cipher_setup(IN tt_cipher_t *cipher,
                                   IN tt_cipher_type_t type,
                                   IN tt_bool_t encrypt,
                                   IN tt_u8_t *key,
                                   IN tt_u32_t len);

extern tt_result_t tt_cipher_set_iv(IN tt_cipher_t *cipher,
                                    IN tt_u8_t *iv,
                                    IN tt_u32_t len);

extern tt_result_t tt_cipher_set_pad(IN tt_cipher_t *cipher,
                                     IN tt_cipher_padding_t padding);

// must be called after tt_cipher_setup and tt_cipher_set_iv
extern tt_result_t tt_cipher_set_aad(IN tt_cipher_t *cipher,
                                     IN tt_u8_t *aad,
                                     IN tt_u32_t len);

extern tt_result_t tt_cipher_update(IN tt_cipher_t *cipher,
                                    IN tt_u8_t *input,
                                    IN tt_u32_t ilen,
                                    IN tt_u8_t *output,
                                    IN tt_u32_t *olen);

extern tt_result_t tt_cipher_finish(IN tt_cipher_t *cipher,
                                    IN tt_u8_t *output,
                                    IN tt_u32_t *olen);

extern tt_result_t tt_cipher_tag(IN tt_cipher_t *cipher,
                                 IN tt_u8_t *tag,
                                 IN tt_u32_t len);

extern tt_result_t tt_cipher_auth(IN tt_cipher_t *cipher,
                                  IN tt_u8_t *tag,
                                  IN tt_u32_t len);

extern tt_result_t tt_cipher_update_buf(IN tt_cipher_t *cipher,
                                        IN tt_u8_t *input,
                                        IN tt_u32_t ilen,
                                        IN struct tt_buf_s *obuf);

extern tt_result_t tt_cipher_finish_buf(IN tt_cipher_t *cipher,
                                        IN struct tt_buf_s *obuf);

extern tt_result_t tt_cipher_encrypt_tag(IN tt_cipher_t *cipher,
                                         IN tt_u8_t *iv,
                                         IN tt_u32_t iv_len,
                                         IN tt_u8_t *aad,
                                         IN tt_u32_t ad_len,
                                         IN tt_u8_t *input,
                                         IN tt_u32_t ilen,
                                         IN tt_u8_t *output,
                                         IN tt_u32_t *olen,
                                         IN tt_u8_t *tag,
                                         IN tt_u32_t tag_len);

extern tt_result_t tt_cipher_decrypt_auth(IN tt_cipher_t *cipher,
                                          IN tt_u8_t *iv,
                                          IN tt_u32_t iv_len,
                                          IN tt_u8_t *aad,
                                          IN tt_u32_t ad_len,
                                          IN tt_u8_t *input,
                                          IN tt_u32_t ilen,
                                          IN tt_u8_t *output,
                                          IN tt_u32_t *olen,
                                          IN tt_u8_t *tag,
                                          IN tt_u32_t tag_len);

extern tt_result_t tt_cipher_encrypt_tag_buf(IN tt_cipher_t *cipher,
                                             IN tt_u8_t *iv,
                                             IN tt_u32_t iv_len,
                                             IN tt_u8_t *aad,
                                             IN tt_u32_t ad_len,
                                             IN tt_u8_t *input,
                                             IN tt_u32_t ilen,
                                             IN struct tt_buf_s *obuf,
                                             IN tt_u8_t *tag,
                                             IN tt_u32_t tag_len);

extern tt_result_t tt_cipher_decrypt_auth_buf(IN tt_cipher_t *cipher,
                                              IN tt_u8_t *iv,
                                              IN tt_u32_t iv_len,
                                              IN tt_u8_t *aad,
                                              IN tt_u32_t ad_len,
                                              IN tt_u8_t *input,
                                              IN tt_u32_t ilen,
                                              IN struct tt_buf_s *obuf,
                                              IN tt_u8_t *tag,
                                              IN tt_u32_t tag_len);

#endif
