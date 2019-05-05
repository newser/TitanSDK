/* Copyright (C) 2017 haniu (niuhao.cn@gmail.com)
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
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

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <network/ssh/context/tt_ssh_encrypt_aes.h>

#include <crypto/tt_aes.h>
#include <misc/tt_assert.h>
#include <network/ssh/context/tt_ssh_encrypt.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_sshenc_aes_create_128cbc(IN struct tt_sshenc_s *enc,
                                        IN tt_bool_t encrypt, IN tt_u8_t *iv,
                                        IN tt_u32_t iv_len, IN tt_u8_t *key,
                                        IN tt_u32_t key_len)
{
    tt_blob_t key_blob;
    tt_aes_attr_t attr;

    key_blob.addr = key;
    key_blob.len = key_len;

    // padding
    attr.padding = TT_AES_PADDING_NONE;

    // mode
    attr.mode = TT_AES_MODE_CBC;
    attr.cbc.ivec.addr = iv;
    attr.cbc.ivec.len = iv_len;

    if (!TT_OK(tt_aes_create(&enc->alg_u.aes, encrypt, &key_blob, TT_AES128,
                             &attr))) {
        TT_ERROR("fail to create ssh aes");
        return TT_FAIL;
    }

    enc->alg = TT_SSH_ENC_ALG_AES128_CBC;
    enc->block_len = TT_AES_BLOCK_SIZE;

    return TT_SUCCESS;
}

tt_result_t tt_sshenc_aes_create_256cbc(IN struct tt_sshenc_s *enc,
                                        IN tt_bool_t encrypt, IN tt_u8_t *iv,
                                        IN tt_u32_t iv_len, IN tt_u8_t *key,
                                        IN tt_u32_t key_len)
{
    tt_blob_t key_blob;
    tt_aes_attr_t attr;

    key_blob.addr = key;
    key_blob.len = key_len;

    // padding
    attr.padding = TT_AES_PADDING_NONE;

    // mode
    attr.mode = TT_AES_MODE_CBC;
    attr.cbc.ivec.addr = iv;
    attr.cbc.ivec.len = iv_len;

    if (!TT_OK(tt_aes_create(&enc->alg_u.aes, encrypt, &key_blob, TT_AES256,
                             &attr))) {
        TT_ERROR("fail to create ssh aes");
        return TT_FAIL;
    }

    enc->alg = TT_SSH_ENC_ALG_AES256_CBC;
    enc->block_len = TT_AES_BLOCK_SIZE;

    return TT_SUCCESS;
}

void tt_sshenc_aes_destroy(IN struct tt_sshenc_s *enc)
{
    tt_aes_destroy(&enc->alg_u.aes);
}

tt_result_t tt_sshenc_aes_encrypt(IN struct tt_sshenc_s *enc,
                                  IN OUT tt_u8_t *data, IN tt_u32_t data_len)
{
    tt_u32_t output_len = data_len;

    return tt_aes_encrypt(&enc->alg_u.aes, data, data_len, data, &output_len);
}

tt_result_t tt_sshenc_aes_decrypt(IN struct tt_sshenc_s *enc,
                                  IN OUT tt_u8_t *data, IN tt_u32_t data_len)
{
    tt_u32_t output_len = data_len;

    return tt_aes_decrypt(&enc->alg_u.aes, data, data_len, data, &output_len);
}
