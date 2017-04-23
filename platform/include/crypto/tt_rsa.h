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
@brief crypto: RSA

this file defines RSA APIs
*/

#ifndef __TT_RSA__
#define __TT_RSA__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <crypto/tt_rsa_def.h>

#include <tt_cstd_api.h>
#include <tt_rsa_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_profile_s;
struct tt_buf_s;

typedef struct tt_rsa_attr_s
{
    const tt_char_t *password;
    tt_rsa_padding_attr_t padding;

    tt_bool_t pem_armor : 1;
} tt_rsa_attr_t;

typedef struct tt_rsa_s
{
    tt_rsa_ntv_t sys_rsa;

    tt_rsa_size_t size;
    tt_u32_t block_size;
    tt_u32_t min_padding_size;
    tt_u32_t max_data_size;

    tt_rsa_attr_t attr;
} tt_rsa_t;

typedef struct
{
    tt_blob_t modulus;
    tt_blob_t pub_exp;
} tt_rsa_pubnum_t;

typedef struct
{
    tt_blob_t modulus;
    tt_blob_t pub_exp;
    tt_blob_t priv_exp;
    tt_blob_t prime1;
    tt_blob_t prime2;
    tt_blob_t exp1;
    tt_blob_t exp2;
    tt_blob_t coefficient;
} tt_rsa_privnum_t;

typedef struct tt_rsa_number_s
{
    tt_rsa_type_t type;
    union
    {
        tt_rsa_pubnum_t pubnum;
        tt_rsa_privnum_t privnum;
    };
} tt_rsa_number_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_inline tt_result_t tt_rsa_component_init(IN struct tt_profile_s *profile)
{
    return tt_rsa_component_init_ntv(profile);
}

extern tt_result_t tt_rsa_generate(IN tt_rsa_t *rsa,
                                   IN tt_rsa_size_t size,
                                   IN OPT tt_rsa_attr_t *attr);

extern tt_result_t tt_rsa_create(IN tt_rsa_t *rsa,
                                 IN tt_rsa_format_t key_format,
                                 IN tt_rsa_type_t key_type,
                                 IN tt_blob_t *key_data,
                                 IN OPT tt_rsa_attr_t *attr);

extern tt_result_t tt_rsa_create_number(IN tt_rsa_t *rsa,
                                        IN tt_rsa_number_t *rsa_num,
                                        IN OPT tt_rsa_attr_t *attr);

extern void tt_rsa_destroy(IN tt_rsa_t *rsa);

extern void tt_rsa_attr_default(IN tt_rsa_attr_t *attr);

extern void tt_rsa_padding_attr_default(IN tt_rsa_padding_attr_t *attr);

// - input_len must be less than or equal rsa->max_data_size
// - output_len must be larger than or equal rsa->block_size
extern tt_result_t tt_rsa_encrypt(IN tt_rsa_t *rsa,
                                  IN tt_u8_t *input,
                                  IN tt_u32_t input_len,
                                  OUT tt_u8_t *output,
                                  IN OUT tt_u32_t *output_len);
extern tt_result_t tt_rsa_encrypt_buf(IN tt_rsa_t *rsa,
                                      IN tt_u8_t *input,
                                      IN tt_u32_t input_len,
                                      OUT struct tt_buf_s *output);

// input_len must equal rsa->block_size
// - output_len must be larger than or equal rsa->block_size
extern tt_result_t tt_rsa_decrypt(IN tt_rsa_t *rsa,
                                  IN tt_u8_t *input,
                                  IN tt_u32_t input_len,
                                  OUT tt_u8_t *output,
                                  IN OUT tt_u32_t *output_len);
extern tt_result_t tt_rsa_decrypt_buf(IN tt_rsa_t *rsa,
                                      IN tt_u8_t *input,
                                      IN tt_u32_t input_len,
                                      OUT struct tt_buf_s *output);

extern tt_result_t tt_rsa_sign(IN tt_rsa_t *rsa,
                               IN tt_u8_t *input,
                               IN tt_u32_t input_len,
                               OUT tt_u8_t *signature,
                               IN OUT tt_u32_t *signature_len);
extern tt_result_t tt_rsa_sign_buf(IN tt_rsa_t *rsa,
                                   IN tt_u8_t *input,
                                   IN tt_u32_t input_len,
                                   OUT struct tt_buf_s *output);

extern tt_result_t tt_rsa_verify(IN tt_rsa_t *rsa,
                                 IN tt_u8_t *input,
                                 IN tt_u32_t input_len,
                                 IN tt_u8_t *signature,
                                 IN tt_u32_t signature_len);

extern tt_result_t tt_rsa_show(IN tt_rsa_t *rsa);

tt_inline tt_rsa_type_t tt_rsa_get_type(IN tt_rsa_t *rsa)
{
    return tt_rsa_get_type_ntv(&rsa->sys_rsa);
}

extern tt_result_t tt_rsa_get_number(IN tt_rsa_t *rsa,
                                     IN tt_rsa_number_t *rsa_number);

tt_inline void tt_rsa_number_init(IN tt_rsa_number_t *rsa_number)
{
    tt_memset(rsa_number, 0, sizeof(tt_rsa_number_t));
}

extern void tt_rsa_number_destroy(IN tt_rsa_number_t *rsa_number);

#endif /* __TT_RSA__ */
