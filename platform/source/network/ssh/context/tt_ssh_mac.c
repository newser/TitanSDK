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

#include <network/ssh/context/tt_ssh_mac.h>

#include <misc/tt_assert.h>
#include <network/ssh/context/tt_ssh_mac_sha.h>

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

static tt_sshmac_itf_t tt_s_sshmac_itf[TT_SSH_MAC_ALG_NUM] = {
    // TT_SSH_MAC_ALG_HMAC_SHA1
    {
        tt_sshmac_sha1_create,
        tt_sshmac_sha1_destroy,
        tt_sshmac_sha1_sign,
        tt_sshmac_sha1_verify,
    },
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_sshmac_init(IN tt_sshmac_t *mac)
{
    mac->alg = TT_SSH_MAC_ALG_NUM;
    tt_memset(&mac->hmac, 0, sizeof(tt_hmac_t));

    mac->mac_len = 0;
}

void tt_sshmac_destroy(IN tt_sshmac_t *mac)
{
    if (mac->alg == TT_SSH_MAC_ALG_NUM) { return; }

    TT_ASSERT_SSH(TT_SSH_MAC_ALG_VALID(mac->alg));
    tt_s_sshmac_itf[mac->alg].destroy(mac);
}

tt_result_t tt_sshmac_setalg(IN tt_sshmac_t *mac, IN tt_ssh_mac_alg_t alg,
                             IN tt_u8_t *key, IN tt_u32_t key_len)
{
    TT_ASSERT_SSH(TT_SSH_MAC_ALG_VALID(alg));

    if (mac->alg != TT_SSH_MAC_ALG_NUM) {
        TT_ERROR("mac already has alg");
        return TT_FAIL;
    }

    return tt_s_sshmac_itf[alg].create(mac, key, key_len);
}

tt_result_t tt_sshmac_sign(IN tt_sshmac_t *mac, IN tt_u32_t seq_number,
                           IN tt_u8_t *data, IN tt_u32_t data_len,
                           OUT tt_u8_t *signature, IN tt_u32_t signature_len)
{
    if (mac->alg == TT_SSH_MAC_ALG_NUM) {
        TT_ERROR("mac has no alg set");
        return TT_FAIL;
    }

    TT_ASSERT_SSH(TT_SSH_MAC_ALG_VALID(mac->alg));
    return tt_s_sshmac_itf[mac->alg].sign(mac, seq_number, data, data_len,
                                          signature, signature_len);
}

tt_result_t tt_sshmac_verify(IN tt_sshmac_t *mac, IN tt_u32_t seq_number,
                             IN tt_u8_t *data, IN tt_u32_t data_len,
                             IN tt_u8_t *signature, IN tt_u32_t signature_len)
{
    if (mac->alg == TT_SSH_MAC_ALG_NUM) {
        TT_ERROR("mac has no alg set");
        return TT_FAIL;
    }

    TT_ASSERT_SSH(TT_SSH_MAC_ALG_VALID(mac->alg));
    return tt_s_sshmac_itf[mac->alg].verify(mac, seq_number, data, data_len,
                                            signature, signature_len);
}
