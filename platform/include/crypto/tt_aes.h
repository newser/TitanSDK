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
@file tt_aes.h
@brief crypto: AES

this file defines AES APIs
*/

#ifndef __TT_AES__
#define __TT_AES__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_blob.h>
#include <crypto/tt_aes_def.h>
#include <crypto/tt_crypto_pad.h>

#include <aes.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_profile_s;

typedef struct
{
    size_t iv_off;
    mbedtls_aes_context ctx;
    tt_u8_t iv[16];
    tt_u8_t ctr[16];
    tt_aes_mode_t mode : 4;
    tt_crypto_pad_t pad : 4;
} tt_aes_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

// - key->len should be less than aes size, and would be padded with 0
// - attr->cbc.ivec.len should be less than aes size, and would
//   padded with 0
extern tt_result_t tt_aes_create(IN tt_aes_t *aes,
                                 IN tt_bool_t encrypt,
                                 IN tt_blob_t *key,
                                 IN tt_aes_keybit_t keybit);

extern void tt_aes_destroy(IN tt_aes_t *aes);

tt_inline void tt_aes_set_mode(IN tt_aes_t *aes, IN tt_aes_mode_t mode)
{
    TT_ASSERT(TT_AES_MODE_VALID(mode));
    aes->mode = mode;
}

extern void tt_aes_set_iv(IN tt_aes_t *aes, IN tt_u8_t *iv, IN tt_u32_t iv_len);

tt_inline void tt_aes_set_nonce(IN tt_aes_t *aes,
                                IN tt_u8_t *nc,
                                IN tt_u32_t nc_len)
{
    return tt_aes_set_iv(aes, nc, nc_len);
}

tt_inline void tt_aes_set_pad(IN tt_aes_t *aes, IN tt_crypto_pad_t pad)
{
    TT_ASSERT(TT_CRYPTO_PAD_VALID(pad));
    aes->pad = pad;
}

// - if pad mode is none: 1. input->len must be multiple of aes block size;
//   2. length of buffer output should be at lease input->len;
// - if pad mode is pkcs7: 1. input->len can be any value; 2. length
//   of buffer output should be at lease input->len + aes block size;
// - input and output can point to same buffer
extern tt_result_t tt_aes_encrypt(IN tt_aes_t *aes,
                                  IN tt_u8_t *input,
                                  IN tt_u32_t input_len,
                                  OUT tt_u8_t *output,
                                  IN OUT tt_u32_t *output_len);

// - length of buffer output should be at lease input->len. output_len
//   is the length of the buffer output as input and the encrypted data
//   length as output
extern tt_result_t tt_aes_decrypt(IN tt_aes_t *aes,
                                  IN tt_u8_t *input,
                                  IN tt_u32_t input_len,
                                  OUT tt_u8_t *output,
                                  IN OUT tt_u32_t *output_len);

extern tt_result_t tt_aes(IN tt_bool_t encrypt,
                          IN tt_blob_t *key,
                          IN tt_aes_keybit_t keybit,
                          IN tt_aes_mode_t mode,
                          IN OPT tt_blob_t *iv,
                          IN tt_crypto_pad_t pad,
                          IN tt_u8_t *input,
                          IN tt_u32_t input_len,
                          OUT tt_u8_t *output,
                          IN OUT tt_u32_t *output_len);

#endif
