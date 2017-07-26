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
@file tt_ssh_encrypt.h
@brief ts ssh encrypt & decrypt
*/

#ifndef __TT_SSH_ENCRYPT__
#define __TT_SSH_ENCRYPT__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <network/ssh/tt_ssh_def.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct tt_sshenc_s
{
    tt_ssh_enc_alg_t alg;
    /*union
    {
        tt_aes_t aes;
    } alg_u;*/

    tt_u32_t block_len;
} tt_sshenc_t;

typedef tt_result_t (*tt_sshenc_create_t)(IN tt_sshenc_t *enc,
                                          IN tt_bool_t encrypt,
                                          IN tt_u8_t *iv,
                                          IN tt_u32_t iv_len,
                                          IN tt_u8_t *key,
                                          IN tt_u32_t key_len);
typedef void (*tt_sshenc_destroy_t)(IN tt_sshenc_t *enc);

typedef tt_result_t (*tt_sshenc_encrypt_t)(IN tt_sshenc_t *enc,
                                           IN OUT tt_u8_t *data,
                                           IN tt_u32_t data_len);
typedef tt_result_t (*tt_sshenc_decrypt_t)(IN tt_sshenc_t *enc,
                                           IN OUT tt_u8_t *data,
                                           IN tt_u32_t data_len);

typedef struct
{
    tt_sshenc_create_t create;
    tt_sshenc_destroy_t destroy;

    tt_sshenc_encrypt_t encrypt;
    tt_sshenc_decrypt_t decrypt;
} tt_sshenc_itf_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export void tt_sshenc_init(IN tt_sshenc_t *enc);

tt_export void tt_sshenc_destroy(IN tt_sshenc_t *enc);

tt_export tt_result_t tt_sshenc_setalg(IN tt_sshenc_t *enc,
                                    IN tt_ssh_enc_alg_t alg,
                                    IN tt_bool_t encrypt,
                                    IN tt_u8_t *iv,
                                    IN tt_u32_t iv_len,
                                    IN tt_u8_t *key,
                                    IN tt_u32_t key_len);

// it seems all encryption algs can be done in-place, that means
// decrypted bytes are also put in @ref data
tt_export tt_result_t tt_sshenc_encrypt(IN tt_sshenc_t *enc,
                                     IN OUT tt_u8_t *data,
                                     IN tt_u32_t data_len);

tt_export tt_result_t tt_sshenc_decrypt(IN tt_sshenc_t *enc,
                                     IN OUT tt_u8_t *data,
                                     IN tt_u32_t data_len);

#endif /* __TT_SSH_ENCRYPT__ */
