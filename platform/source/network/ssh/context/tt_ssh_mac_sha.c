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

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <network/ssh/context/tt_ssh_mac_sha.h>

#include <crypto/tt_hmac.h>
#include <misc/tt_assert.h>
#include <network/ssh/context/tt_ssh_context.h>
#include <network/ssh/context/tt_ssh_mac.h>

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

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_sshmac_sha1_create(IN struct tt_sshmac_s *mac,
                                  IN tt_u8_t *key,
                                  IN tt_u32_t key_len)
{
    tt_blob_t kb;

    kb.addr = key;
    kb.len = key_len;

    if (!TT_OK(tt_hmac_create(&mac->hmac, TT_HMAC_VER_SHA1, &kb))) {
        return TT_FAIL;
    }

    mac->alg = TT_SSH_MAC_ALG_HMAC_SHA1;
    mac->mac_len = TT_HMAC_SHA1_DIGEST_LENGTH;

    return TT_SUCCESS;
}

void tt_sshmac_sha1_destroy(IN struct tt_sshmac_s *mac)
{
    tt_hmac_destroy(&mac->hmac);
}

tt_result_t tt_sshmac_sha1_sign(IN struct tt_sshmac_s *mac,
                                IN tt_u32_t seq_number,
                                IN tt_u8_t *data,
                                IN tt_u32_t data_len,
                                OUT tt_u8_t *signature,
                                IN tt_u32_t signature_len)
{
    tt_u32_t n;

    if (signature_len < TT_HMAC_SHA1_DIGEST_LENGTH) {
        TT_ERROR("not enough space for hmac sha1");
        return TT_FAIL;
    }

    if (!TT_OK(tt_hmac_reset(&mac->hmac))) {
        return TT_FAIL;
    }

    // mac = MAC(key, sequence_number || unencrypted_packet)
    n = tt_hton32(seq_number);
    if (!TT_OK(tt_hmac_update(&mac->hmac, (tt_u8_t *)&n, sizeof(n)))) {
        return TT_FAIL;
    }
    if (!TT_OK(tt_hmac_update(&mac->hmac, data, data_len))) {
        return TT_FAIL;
    }

    return tt_hmac_final(&mac->hmac, signature);
}

tt_result_t tt_sshmac_sha1_verify(IN struct tt_sshmac_s *mac,
                                  IN tt_u32_t seq_number,
                                  IN tt_u8_t *data,
                                  IN tt_u32_t data_len,
                                  OUT tt_u8_t *signature,
                                  IN tt_u32_t signature_len)
{
    tt_u32_t n;
    tt_u8_t sig[TT_HMAC_SHA1_DIGEST_LENGTH];

    if (signature_len != TT_HMAC_SHA1_DIGEST_LENGTH) {
        TT_ERROR("not enough hmac sha1 length");
        return TT_FAIL;
    }

    if (!TT_OK(tt_hmac_reset(&mac->hmac))) {
        return TT_FAIL;
    }

    // mac = MAC(key, sequence_number || unencrypted_packet)
    n = tt_hton32(seq_number);
    if (!TT_OK(tt_hmac_update(&mac->hmac, (tt_u8_t *)&n, sizeof(n)))) {
        return TT_FAIL;
    }
    if (!TT_OK(tt_hmac_update(&mac->hmac, data, data_len))) {
        return TT_FAIL;
    }

    if (!TT_OK(tt_hmac_final(&mac->hmac, sig))) {
        return TT_FAIL;
    }
    if (tt_memcmp(sig, signature, signature_len) == 0) {
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}
