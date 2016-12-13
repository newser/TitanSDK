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

#include <network/ssh/context/tt_ssh_kdf.h>

#include <crypto/tt_sha.h>
#include <misc/tt_assert.h>
#include <network/ssh/context/tt_ssh_context.h>

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

static tt_result_t __sshkdf_sha1(IN tt_sshkdf_t *kdf,
                                 IN tt_blob_t *k,
                                 IN tt_blob_t *h,
                                 IN tt_blob_t *session_id,
                                 IN tt_u32_t iv_c2s_len,
                                 IN tt_u32_t iv_s2c_len,
                                 IN tt_u32_t enc_c2s_len,
                                 IN tt_u32_t enc_s2c_len,
                                 IN tt_u32_t mac_c2s_len,
                                 IN tt_u32_t mac_s2c_len);
static tt_result_t __sshkdf_sha1_n(IN tt_blob_t *k,
                                   IN tt_blob_t *h,
                                   IN tt_char_t x,
                                   IN tt_blob_t *session_id,
                                   IN tt_u32_t len,
                                   OUT tt_buf_t *key);
static tt_result_t __sshkdf_sha1_1(IN tt_blob_t *k,
                                   IN tt_blob_t *h,
                                   IN tt_char_t x,
                                   IN tt_blob_t *blob,
                                   IN tt_u8_t *hash_val);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_sshkdf_init(IN tt_sshkdf_t *kdf, IN tt_ssh_hash_alg_t hash_alg)
{
    kdf->hash_alg = hash_alg;

    tt_buf_init(&kdf->iv_c2s, NULL);
    tt_buf_init(&kdf->iv_s2c, NULL);

    tt_buf_init(&kdf->enc_c2s, NULL);
    tt_buf_init(&kdf->enc_s2c, NULL);

    tt_buf_init(&kdf->mac_c2s, NULL);
    tt_buf_init(&kdf->mac_s2c, NULL);
}

void tt_sshkdf_destroy(IN tt_sshkdf_t *kdf)
{
    tt_buf_destroy(&kdf->iv_c2s);
    tt_buf_destroy(&kdf->iv_s2c);

    tt_buf_destroy(&kdf->enc_c2s);
    tt_buf_destroy(&kdf->enc_s2c);

    tt_buf_destroy(&kdf->mac_c2s);
    tt_buf_destroy(&kdf->mac_s2c);
}

tt_result_t tt_sshkdf_run(IN tt_sshkdf_t *kdf,
                          IN tt_blob_t *k,
                          IN tt_blob_t *h,
                          IN tt_blob_t *session_id,
                          IN tt_u32_t iv_c2s_len,
                          IN tt_u32_t iv_s2c_len,
                          IN tt_u32_t enc_c2s_len,
                          IN tt_u32_t enc_s2c_len,
                          IN tt_u32_t mac_c2s_len,
                          IN tt_u32_t mac_s2c_len)
{
    TT_ASSERT(TT_SSH_HASH_ALG_VALID(kdf->hash_alg));

    // check k
    if ((k == NULL) || (k->addr == NULL) || (k->len == 0)) {
        TT_ERROR("invalid k");
        return TT_FAIL;
    }

    // check h
    if ((h == NULL) || (h->addr == NULL) || (h->len == 0)) {
        TT_ERROR("invalid h");
        return TT_FAIL;
    }

    // check session id
    if ((session_id == NULL) || (session_id->addr == NULL) ||
        (session_id->len == 0)) {
        TT_ERROR("invalid session_id");
        return TT_FAIL;
    }

    // check lengths
    if ((iv_c2s_len == 0) || (iv_s2c_len == 0) || (enc_c2s_len == 0) ||
        (enc_s2c_len == 0) || (mac_c2s_len == 0) || (mac_s2c_len == 0)) {
        TT_ERROR("invalid lengths: [%d, %d, %d, %d, %d, %d]",
                 iv_c2s_len,
                 iv_s2c_len,
                 enc_c2s_len,
                 enc_s2c_len,
                 mac_c2s_len,
                 mac_s2c_len);
        return TT_FAIL;
    }

    // kdf
    switch (kdf->hash_alg) {
        case TT_SSH_HASH_ALG_SHA1: {
            return __sshkdf_sha1(kdf,
                                 k,
                                 h,
                                 session_id,
                                 iv_c2s_len,
                                 iv_s2c_len,
                                 enc_c2s_len,
                                 enc_s2c_len,
                                 mac_c2s_len,
                                 mac_s2c_len);
        } break;

        default: {
            TT_ERROR("unsupported ssh hash alg[%d]", kdf->hash_alg);
            return TT_FAIL;
        } break;
    }
}

tt_result_t __sshkdf_sha1(IN tt_sshkdf_t *kdf,
                          IN tt_blob_t *k,
                          IN tt_blob_t *h,
                          IN tt_blob_t *session_id,
                          IN tt_u32_t iv_c2s_len,
                          IN tt_u32_t iv_s2c_len,
                          IN tt_u32_t enc_c2s_len,
                          IN tt_u32_t enc_s2c_len,
                          IN tt_u32_t mac_c2s_len,
                          IN tt_u32_t mac_s2c_len)
{
    tt_result_t result;

    // Initial IV client to server: HASH(K || H || "A" || session_id)
    result = __sshkdf_sha1_n(k, h, 'A', session_id, iv_c2s_len, &kdf->iv_c2s);
    if (!TT_OK(result)) {
        TT_ERROR("fail to generate iv_c2s");
        return TT_FAIL;
    }

    // Initial IV server to client: HASH(K || H || "B" || session_id)
    result = __sshkdf_sha1_n(k, h, 'B', session_id, iv_s2c_len, &kdf->iv_s2c);
    if (!TT_OK(result)) {
        TT_ERROR("fail to generate iv_s2c");
        return TT_FAIL;
    }

    // Encryption key client to server: HASH(K || H || "C" || session_id)
    result = __sshkdf_sha1_n(k, h, 'C', session_id, enc_c2s_len, &kdf->enc_c2s);
    if (!TT_OK(result)) {
        TT_ERROR("fail to generate enc_c2s");
        return TT_FAIL;
    }

    // Encryption key server to client: HASH(K || H || "D" || session_id)
    result = __sshkdf_sha1_n(k, h, 'D', session_id, enc_s2c_len, &kdf->enc_s2c);
    if (!TT_OK(result)) {
        TT_ERROR("fail to generate enc_s2c");
        return TT_FAIL;
    }

    // Integrity key client to server: HASH(K || H || "E" || session_id)
    result = __sshkdf_sha1_n(k, h, 'E', session_id, mac_c2s_len, &kdf->mac_c2s);
    if (!TT_OK(result)) {
        TT_ERROR("fail to generate mac_c2s");
        return TT_FAIL;
    }

    // Integrity key server to client: HASH(K || H || "F" || session_id)
    result = __sshkdf_sha1_n(k, h, 'F', session_id, mac_s2c_len, &kdf->mac_s2c);
    if (!TT_OK(result)) {
        TT_ERROR("fail to generate mac_s2c");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t __sshkdf_sha1_n(IN tt_blob_t *k,
                            IN tt_blob_t *h,
                            IN tt_char_t x,
                            IN tt_blob_t *session_id,
                            IN tt_u32_t len,
                            OUT tt_buf_t *key)
{
    tt_u32_t i, n;
    tt_result_t result;

    n = len;
    if ((n % TT_SHA1_DIGEST_LENGTH) != 0) {
        n += TT_SHA1_DIGEST_LENGTH;
    }
    n /= TT_SHA1_DIGEST_LENGTH;

    tt_buf_reset_rwp(key);
    result = tt_buf_reserve(key, n * TT_SHA1_DIGEST_LENGTH);
    if (!TT_OK(result)) {
        return TT_FAIL;
    }

    // K1 = HASH(K || H || X || session_id)
    result = __sshkdf_sha1_1(k, h, x, session_id, TT_BUF_WPOS(key));
    if (!TT_OK(result)) {
        return TT_FAIL;
    }
    tt_buf_inc_wp(key, TT_SHA1_DIGEST_LENGTH);

    // K2 = HASH(K || H || K1)
    // K3 = HASH(K || H || K1 || K2)
    // ...
    for (i = 1; i < n; ++i) {
        tt_blob_t tail;

        tt_buf_getptr_rpblob(key, &tail);
        result = __sshkdf_sha1_1(k, h, 0, &tail, TT_BUF_WPOS(key));
        if (!TT_OK(result)) {
            return TT_FAIL;
        }
        tt_buf_inc_wp(key, TT_SHA1_DIGEST_LENGTH);
    }

    return TT_SUCCESS;
}

tt_result_t __sshkdf_sha1_1(IN tt_blob_t *k,
                            IN tt_blob_t *h,
                            IN tt_char_t x,
                            IN tt_blob_t *tail,
                            IN tt_u8_t *hash_val)
{
    tt_sha_t sha;

    if (!TT_OK(tt_sha_create(&sha, TT_SHA_VER_SHA1))) {
        return TT_FAIL;
    }

    if (!TT_OK(tt_sha_update(&sha, k->addr, k->len))) {
        return TT_FAIL;
    }
    if (!TT_OK(tt_sha_update(&sha, h->addr, h->len))) {
        return TT_FAIL;
    }
    if ((x != 0) && !TT_OK(tt_sha_update(&sha, (tt_u8_t *)&x, 1))) {
        return TT_FAIL;
    }
    if (!TT_OK(tt_sha_update(&sha, tail->addr, tail->len))) {
        return TT_FAIL;
    }

    if (!TT_OK(tt_sha_final(&sha, hash_val))) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}
