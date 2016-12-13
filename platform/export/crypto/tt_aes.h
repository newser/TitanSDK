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

#include <crypto/tt_aes_def.h>

#include <tt_aes_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_profile_s;

typedef struct
{
    tt_aes_ntv_t sys_aes;
} tt_aes_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_inline tt_result_t tt_aes_component_init(IN struct tt_profile_s *profile)
{
    return tt_aes_component_init_ntv(profile);
}

// - key->len should be less than aes size, and would padded with 0
// - attr->cbc.ivec.len should be less than aes size, and would
//   padded with 0
extern tt_result_t tt_aes_create(IN tt_aes_t *aes,
                                 IN tt_bool_t encrypt,
                                 IN tt_blob_t *key,
                                 IN tt_aes_size_t size,
                                 IN tt_aes_attr_t *attr);

extern void tt_aes_destroy(IN tt_aes_t *aes);

// - if padding mode is none: 1. input->len must be multiple of aes block size;
//   2. length of buffer output should be at lease input->len;
// - if padding mode is pkcs7: 1. input->len can be any value; 2. length
//   of buffer output should be at lease input->len + aes block size;
// - input and output can point to same buffer
tt_inline tt_result_t tt_aes_encrypt(IN tt_aes_t *aes,
                                     IN tt_u8_t *input,
                                     IN tt_u32_t input_len,
                                     OUT tt_u8_t *output,
                                     IN OUT tt_u32_t *output_len)
{
    if (input_len == 0) {
        *output_len = 0;
        return TT_SUCCESS;
    }

    return tt_aes_encrypt_ntv(&aes->sys_aes,
                              input,
                              input_len,
                              output,
                              output_len);
}

// - length of buffer output should be at lease input->len. output_len
//   is the length of the buffer output as input and the encrypted data
//   length as output
tt_inline tt_result_t tt_aes_decrypt(IN tt_aes_t *aes,
                                     IN tt_u8_t *input,
                                     IN tt_u32_t input_len,
                                     OUT tt_u8_t *output,
                                     IN OUT tt_u32_t *output_len)
{
    if (input_len == 0) {
        *output_len = 0;
        return TT_SUCCESS;
    }

    return tt_aes_decrypt_ntv(&aes->sys_aes,
                              input,
                              input_len,
                              output,
                              output_len);
}

extern tt_result_t tt_aes(IN tt_bool_t encrypt,
                          IN tt_blob_t *key,
                          IN tt_aes_size_t size,
                          IN tt_aes_attr_t *attr,
                          IN tt_blob_t *input,
                          OUT tt_u8_t *output,
                          IN OUT tt_u32_t *output_len);

#endif
