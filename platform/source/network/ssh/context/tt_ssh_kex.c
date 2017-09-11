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

#include <network/ssh/context/tt_ssh_kex.h>

#include <crypto/tt_sha.h>
#include <memory/tt_memory_alloc.h>
#include <network/ssh/message/tt_ssh_render.h>

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

static tt_result_t __calc_h_dh_sha1(IN tt_sshkex_t *kex);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_sshkex_create(IN tt_sshkex_t *kex)
{
    kex->alg = TT_SSH_KEX_ALG_NUM;
    tt_memset(&kex->alg_u, 0, sizeof(kex->alg_u));

    tt_buf_init(&kex->v_c, NULL);
    tt_buf_init(&kex->v_s, NULL);
    tt_buf_init(&kex->i_c, NULL);
    tt_buf_init(&kex->i_s, NULL);
    tt_buf_init(&kex->k_s, NULL);
    tt_buf_init(&kex->k, NULL);
    tt_buf_init(&kex->h, NULL);

    return TT_SUCCESS;
}

void tt_sshkex_destroy(IN tt_sshkex_t *kex)
{
    switch (kex->alg) {
        case TT_SSH_KEX_ALG_DH_G1_SHA1:
        case TT_SSH_KEX_ALG_DH_G14_SHA1: {
            tt_sshkexdh_destroy(&kex->alg_u.kexdh);
        } break;

        default: {
        } break;
    }

    tt_buf_destroy(&kex->v_c);
    tt_buf_destroy(&kex->v_s);
    tt_buf_destroy(&kex->i_c);
    tt_buf_destroy(&kex->i_s);
    tt_buf_destroy(&kex->k_s);
    tt_buf_destroy(&kex->k);
    tt_buf_destroy(&kex->h);
}

tt_result_t tt_sshkex_setalg(IN tt_sshkex_t *kex, IN tt_ssh_kex_alg_t alg)
{
    if (kex->alg != TT_SSH_KEX_ALG_NUM) {
        TT_ERROR("already set kex alg[%d]", kex->alg);
        return TT_FAIL;
    }

    switch (alg) {
        case TT_SSH_KEX_ALG_DH_G1_SHA1:
        case TT_SSH_KEX_ALG_DH_G14_SHA1: {
            if (!TT_OK(tt_sshkexdh_create(&kex->alg_u.kexdh, alg))) {
                return TT_FAIL;
            };

            kex->alg = alg;
            return TT_SUCCESS;
        } break;

        default: {
            TT_ERROR("does not support [%s] kex", tt_g_ssh_kex_alg_name[alg]);
            return TT_FAIL;
        } break;
    }
}

tt_result_t tt_sshkex_setvc(IN tt_sshkex_t *kex,
                            IN tt_u8_t *vc,
                            IN tt_u32_t vc_len,
                            IN tt_bool_t format)
{
    tt_buf_reset_rwp(&kex->v_c);
    if (format) {
        return tt_ssh_string_render(&kex->v_c, vc, vc_len);
    } else {
        return tt_buf_put(&kex->v_c, vc, vc_len);
    }
}

tt_result_t tt_sshkex_setvs(IN tt_sshkex_t *kex,
                            IN tt_u8_t *vs,
                            IN tt_u32_t vs_len,
                            IN tt_bool_t format)
{
    tt_buf_reset_rwp(&kex->v_s);
    if (format) {
        return tt_ssh_string_render(&kex->v_s, vs, vs_len);
    } else {
        return tt_buf_put(&kex->v_s, vs, vs_len);
    }
}

tt_result_t tt_sshkex_setic(IN tt_sshkex_t *kex,
                            IN tt_u8_t *ic,
                            IN tt_u32_t ic_len,
                            IN tt_bool_t format)
{
    tt_buf_reset_rwp(&kex->i_c);
    if (format) {
        return tt_ssh_string_render(&kex->i_c, ic, ic_len);
    } else {
        return tt_buf_put(&kex->i_c, ic, ic_len);
    }
}

tt_result_t tt_sshkex_setis(IN tt_sshkex_t *kex,
                            IN tt_u8_t *is,
                            IN tt_u32_t is_len,
                            IN tt_bool_t format)
{
    tt_buf_reset_rwp(&kex->i_s);
    if (format) {
        return tt_ssh_string_render(&kex->i_s, is, is_len);
    } else {
        return tt_buf_put(&kex->i_s, is, is_len);
    }
}

tt_result_t tt_sshkex_setks(IN tt_sshkex_t *kex,
                            IN tt_u8_t *ks,
                            IN tt_u32_t ks_len,
                            IN tt_bool_t format)
{
    tt_buf_reset_rwp(&kex->k_s);
    if (format) {
        return tt_ssh_string_render(&kex->k_s, ks, ks_len);
    } else {
        return tt_buf_put(&kex->k_s, ks, ks_len);
    }
}

tt_result_t tt_sshkex_setks_rsa(IN tt_sshkex_t *kex,
                                IN tt_blob_t *e,
                                IN tt_blob_t *n)
{
    tt_u32_t len = 0;

    const tt_char_t *ssh_rsa;
    tt_u32_t ssh_rsa_len;

    /*
     string server public host key and certificates (K_S)

     string "ssh-rsa"
     mpint e
     mpint n
     */

    ssh_rsa = tt_g_ssh_pubkey_alg_name[TT_SSH_PUBKEY_ALG_RSA];
    // ssh_rsa_len = (tt_u32_t)tt_strlen(ssh_rsa);
    ssh_rsa_len = 7;

    // render len
    len += tt_ssh_string_render_prepare((tt_u8_t *)ssh_rsa, ssh_rsa_len);
    len += tt_ssh_mpint_render_prepare(e->addr, e->len, TT_FALSE);
    len += tt_ssh_mpint_render_prepare(n->addr, n->len, TT_FALSE);

    // render
    tt_buf_reset_rwp(&kex->k_s);
    TT_DO(tt_buf_reserve(&kex->k_s, len));

    TT_DO(tt_buf_put_u32_n(&kex->k_s, len));
    TT_DO(tt_ssh_string_render(&kex->k_s, (tt_u8_t *)ssh_rsa, ssh_rsa_len));
    TT_DO(tt_ssh_mpint_render(&kex->k_s, e->addr, e->len, TT_FALSE));
    TT_DO(tt_ssh_mpint_render(&kex->k_s, n->addr, n->len, TT_FALSE));

    return TT_SUCCESS;
}

tt_result_t tt_sshkex_calc_h(IN tt_sshkex_t *kex)
{
    switch (kex->alg) {
        case TT_SSH_KEX_ALG_DH_G1_SHA1:
        case TT_SSH_KEX_ALG_DH_G14_SHA1: {
            return __calc_h_dh_sha1(kex);
        } break;

        default: {
            return TT_FAIL;
        } break;
    }
}

tt_result_t __calc_h_dh_sha1(IN tt_sshkex_t *kex)
{
    tt_result_t result;
    tt_sha_t sha;
    tt_buf_t *data;

    // H = hash(V_C || V_S || I_C || I_S || K_S || e || f || K)

    result = tt_sha_create(&sha, TT_SHA_VER_SHA1);
    if (!TT_OK(result)) {
        TT_ERROR("fail to create kex dh sha1");
        return TT_FAIL;
    }

    // V_C
    data = &kex->v_c;
    if (TT_BUF_RLEN(data) == 0) {
        TT_ERROR("V_C is not set yet");
        goto h_out;
    }
    result = tt_sha_update(&sha, TT_BUF_RPOS(data), TT_BUF_RLEN(data));
    if (!TT_OK(result)) {
        goto h_out;
    }

    // V_S
    data = &kex->v_s;
    if (TT_BUF_RLEN(data) == 0) {
        TT_ERROR("V_S is not set yet");
        goto h_out;
    }
    result = tt_sha_update(&sha, TT_BUF_RPOS(data), TT_BUF_RLEN(data));
    if (!TT_OK(result)) {
        goto h_out;
    }

    // I_C
    data = &kex->i_c;
    if (TT_BUF_RLEN(data) == 0) {
        TT_ERROR("I_C is not set yet");
        goto h_out;
    }
    result = tt_sha_update(&sha, TT_BUF_RPOS(data), TT_BUF_RLEN(data));
    if (!TT_OK(result)) {
        goto h_out;
    }

    // I_S
    data = &kex->i_s;
    if (TT_BUF_RLEN(data) == 0) {
        TT_ERROR("I_S is not set yet");
        goto h_out;
    }
    result = tt_sha_update(&sha, TT_BUF_RPOS(data), TT_BUF_RLEN(data));
    if (!TT_OK(result)) {
        goto h_out;
    }

    // K_S
    data = &kex->k_s;
    if (TT_BUF_RLEN(data) == 0) {
        TT_ERROR("K_S is not set yet");
        goto h_out;
    }
    result = tt_sha_update(&sha, TT_BUF_RPOS(data), TT_BUF_RLEN(data));
    if (!TT_OK(result)) {
        goto h_out;
    }

    // e
    data = &kex->alg_u.kexdh.e;
    if (TT_BUF_RLEN(data) == 0) {
        TT_ERROR("e is not set yet");
        goto h_out;
    }
    result = tt_sha_update(&sha, TT_BUF_RPOS(data), TT_BUF_RLEN(data));
    if (!TT_OK(result)) {
        goto h_out;
    }

    // f
    data = &kex->alg_u.kexdh.f;
    if (TT_BUF_RLEN(data) == 0) {
        TT_ERROR("f is not set yet");
        goto h_out;
    }
    result = tt_sha_update(&sha, TT_BUF_RPOS(data), TT_BUF_RLEN(data));
    if (!TT_OK(result)) {
        goto h_out;
    }

    // K
    data = &kex->k;
    if (TT_BUF_RLEN(data) == 0) {
        TT_ERROR("K is not set yet");
        goto h_out;
    }
    result = tt_sha_update(&sha, TT_BUF_RPOS(data), TT_BUF_RLEN(data));
    if (!TT_OK(result)) {
        goto h_out;
    }

    // hash
    tt_buf_reset_rwp(&kex->h);
    result = tt_sha_final_buf(&sha, &kex->h);

h_out:

    tt_sha_destroy(&sha);
    return result;
}
