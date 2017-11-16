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

#include <network/ssh/context/tt_ssh_encrypt.h>

#include <misc/tt_assert.h>
#include <network/ssh/context/tt_ssh_encrypt_aes.h>

#include <tt_cstd_api.h>

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

static tt_sshenc_itf_t tt_s_sshenc_itf[TT_SSH_ENC_ALG_NUM] = {
    // TT_SSH_ENC_ALG_AES128_CBC
    {
        tt_sshenc_aes_create_128cbc,
        tt_sshenc_aes_destroy,
        tt_sshenc_aes_encrypt,
        tt_sshenc_aes_decrypt,
    },

    // TT_SSH_ENC_ALG_AES256_CBC
    {
        tt_sshenc_aes_create_256cbc,
        tt_sshenc_aes_destroy,
        tt_sshenc_aes_encrypt,
        tt_sshenc_aes_decrypt,
    },
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_sshenc_init(IN tt_sshenc_t *enc)
{
    enc->alg = TT_SSH_ENC_ALG_NUM;

    tt_memset(&enc->alg_u, 0, sizeof(enc->alg_u));

    enc->block_len = 0;
}

void tt_sshenc_destroy(IN tt_sshenc_t *enc)
{
    if (enc->alg == TT_SSH_ENC_ALG_NUM) {
        return;
    }

    TT_ASSERT_SSH(TT_SSH_ENC_ALG_VALID(enc->alg));
    tt_s_sshenc_itf[enc->alg].destroy(enc);
}

tt_result_t tt_sshenc_setalg(IN tt_sshenc_t *enc,
                             IN tt_ssh_enc_alg_t alg,
                             IN tt_bool_t encrypt,
                             IN tt_u8_t *iv,
                             IN tt_u32_t iv_len,
                             IN tt_u8_t *key,
                             IN tt_u32_t key_len)
{
    TT_ASSERT_SSH(TT_SSH_ENC_ALG_VALID(alg));

    if (enc->alg != TT_SSH_ENC_ALG_NUM) {
        TT_ERROR("enc already has alg");
        return TT_FAIL;
    }

    return tt_s_sshenc_itf[alg].create(enc, encrypt, iv, iv_len, key, key_len);
}

tt_result_t tt_sshenc_encrypt(IN tt_sshenc_t *enc,
                              IN OUT tt_u8_t *data,
                              IN tt_u32_t data_len)
{
    if (enc->alg == TT_SSH_ENC_ALG_NUM) {
        TT_ERROR("enc has no alg set");
        return TT_FAIL;
    }

    TT_ASSERT_SSH(TT_SSH_ENC_ALG_VALID(enc->alg));
    if (data_len > 0) {
        return tt_s_sshenc_itf[enc->alg].encrypt(enc, data, data_len);
    } else {
        return TT_SUCCESS;
    }
}

tt_result_t tt_sshenc_decrypt(IN tt_sshenc_t *enc,
                              IN OUT tt_u8_t *data,
                              IN tt_u32_t data_len)
{
    if (enc->alg == TT_SSH_ENC_ALG_NUM) {
        TT_ERROR("enc has no alg set");
        return TT_FAIL;
    }

    TT_ASSERT_SSH(TT_SSH_ENC_ALG_VALID(enc->alg));
    if (data_len > 0) {
        return tt_s_sshenc_itf[enc->alg].decrypt(enc, data, data_len);
    } else {
        return TT_SUCCESS;
    }
}
