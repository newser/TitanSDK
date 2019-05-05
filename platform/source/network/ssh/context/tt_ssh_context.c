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

#include <network/ssh/context/tt_ssh_context.h>

#include <memory/tt_memory_alloc.h>
#include <network/ssh/context/tt_ssh_kdf.h>
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

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_sshctx_create(IN tt_sshctx_t *sshctx, IN tt_sshctx_attr_t *attr)
{
    TT_ASSERT(sshctx != NULL);
    TT_ASSERT(attr != NULL);

    tt_memset(sshctx, 0, sizeof(tt_sshctx_t));

    tt_memcpy(&sshctx->attr, attr, sizeof(tt_sshctx_attr_t));

    // key exchange
    sshctx->kex = NULL;
    if (!TT_OK(tt_sshctx_kex_create(sshctx))) { return TT_FAIL; }
    TT_ASSERT(sshctx->kex != NULL);

    // public key
    sshctx->pubk = NULL;
    if (!TT_OK(tt_sshctx_pubk_create(sshctx))) { return TT_FAIL; }
    TT_ASSERT(sshctx->pubk != NULL);

    // encryption
    sshctx->enc_alg = TT_SSH_ENC_ALG_NUM;
    tt_sshenc_init(&sshctx->encrypt);

    sshctx->dec_alg = TT_SSH_ENC_ALG_NUM;
    tt_sshenc_init(&sshctx->decrypt);

    // integrity
    sshctx->sign_alg = TT_SSH_MAC_ALG_NUM;
    tt_sshmac_init(&sshctx->sign);
    // the version exchange msg should not be count
    sshctx->sign_seq = ~0;

    sshctx->verify_alg = TT_SSH_MAC_ALG_NUM;
    tt_sshmac_init(&sshctx->verify);
    // the version exchange msg should not be count
    sshctx->verify_seq = ~0;

    return TT_SUCCESS;
}

void tt_sshctx_destroy(IN tt_sshctx_t *sshctx)
{
    TT_ASSERT(sshctx != NULL);

    // key exchange
    tt_sshctx_kex_destroy(sshctx);
    tt_blob_destroy(&sshctx->session_id);

    // public key
    tt_sshctx_pubk_destroy(sshctx);

    // encryption
    tt_sshenc_destroy(&sshctx->encrypt);
    tt_sshenc_destroy(&sshctx->decrypt);

    // integrity
    tt_sshmac_destroy(&sshctx->sign);
    tt_sshmac_destroy(&sshctx->verify);
}

void tt_sshctx_clear(IN tt_sshctx_t *sshctx)
{
    tt_sshctx_kex_destroy(sshctx);
    sshctx->kex = NULL;

    tt_sshctx_pubk_destroy(sshctx);
    sshctx->pubk = NULL;
}

// ========================================
// key exchange
// ========================================

tt_result_t tt_sshctx_kex_create(IN tt_sshctx_t *sshctx)
{
    tt_sshkex_t *kex;

    if (sshctx->kex != NULL) {
        TT_ERROR("sshctx already has kex");
        return TT_FAIL;
    }

    kex = (tt_sshkex_t *)tt_malloc(sizeof(tt_sshkex_t));
    if (kex == NULL) {
        TT_ERROR("no mem for sshctx kex");
        return TT_FAIL;
    }

    if (!TT_OK(tt_sshkex_create(kex))) {
        tt_free(kex);
        return TT_FAIL;
    }

    sshctx->kex = kex;
    return TT_SUCCESS;
}

void tt_sshctx_kex_destroy(IN tt_sshctx_t *sshctx)
{
    if (sshctx->kex != NULL) {
        tt_sshkex_destroy(sshctx->kex);
        sshctx->kex = NULL;
    }
}

tt_result_t tt_sshctx_kex_setalg(IN tt_sshctx_t *sshctx,
                                 IN tt_ssh_kex_alg_t alg)
{
    if (sshctx->kex == NULL) {
        TT_ERROR("sshctx has no kex");
        return TT_FAIL;
    }

    return tt_sshkex_setalg(sshctx->kex, alg);
}

tt_result_t tt_sshctx_kex_setks(IN tt_sshctx_t *sshctx, IN tt_u8_t *ks,
                                IN tt_u32_t ks_len, IN tt_bool_t format)
{
    if (sshctx->kex == NULL) {
        TT_ERROR("sshctx has no kex");
        return TT_FAIL;
    }

    return tt_sshkex_setks(sshctx->kex, ks, ks_len, format);
}

tt_result_t tt_sshctx_kex_setks_rsa(IN tt_sshctx_t *sshctx, IN tt_blob_t *e,
                                    IN tt_blob_t *n)
{
    if (sshctx->kex == NULL) {
        TT_ERROR("sshctx has no kex");
        return TT_FAIL;
    }

    return tt_sshkex_setks_rsa(sshctx->kex, e, n);
}

tt_result_t tt_sshctx_kex_setvs(IN tt_sshctx_t *sshctx, IN tt_u8_t *vs,
                                IN tt_u32_t vs_len, IN tt_bool_t format)
{
    if (sshctx->kex == NULL) {
        TT_ERROR("sshctx has no kex");
        return TT_FAIL;
    }

    return tt_sshkex_setvs(sshctx->kex, vs, vs_len, format);
}

tt_result_t tt_sshctx_kex_setvc(IN tt_sshctx_t *sshctx, IN tt_u8_t *vc,
                                IN tt_u32_t vc_len, IN tt_bool_t format)
{
    if (sshctx->kex == NULL) {
        TT_ERROR("sshctx has no kex");
        return TT_FAIL;
    }

    return tt_sshkex_setvc(sshctx->kex, vc, vc_len, format);
}

tt_result_t tt_sshctx_kex_setis(IN tt_sshctx_t *sshctx, IN tt_u8_t *is,
                                IN tt_u32_t is_len, IN tt_bool_t format)
{
    if (sshctx->kex == NULL) {
        TT_ERROR("sshctx has no kex");
        return TT_FAIL;
    }

    return tt_sshkex_setis(sshctx->kex, is, is_len, format);
}

tt_result_t tt_sshctx_kex_setic(IN tt_sshctx_t *sshctx, IN tt_u8_t *ic,
                                IN tt_u32_t ic_len, IN tt_bool_t format)
{
    if (sshctx->kex == NULL) {
        TT_ERROR("sshctx has no kex");
        return TT_FAIL;
    }

    return tt_sshkex_setic(sshctx->kex, ic, ic_len, format);
}

tt_result_t tt_sshctx_kex_calc_h(IN tt_sshctx_t *sshctx)
{
    if (sshctx->kex == NULL) {
        TT_ERROR("sshctx has no kex");
        return TT_FAIL;
    }

    return tt_sshkex_calc_h(sshctx->kex);
}

tt_result_t tt_sshctx_load_session_id(IN tt_sshctx_t *sshctx)
{
    tt_buf_t *h;

    if (sshctx->kex == NULL) {
        TT_ERROR("sshctx has no kex");
        return TT_FAIL;
    }

    h = &sshctx->kex->h;
    if (TT_BUF_RLEN(h) == 0) {
        TT_ERROR("sshctx has no h");
        return TT_FAIL;
    }

    if (sshctx->session_id.addr != NULL) {
        TT_ERROR("sshctx already has session id set");
        return TT_FAIL;
    }

    return tt_blob_create(&sshctx->session_id, TT_BUF_RPOS(h), TT_BUF_RLEN(h));
}

tt_result_t tt_sshctx_kexdh_compute(IN tt_sshctx_t *sshctx, IN tt_u8_t *peerpub,
                                    IN tt_u32_t peerpub_len)
{
    if (sshctx->kex == NULL) {
        TT_ERROR("sshctx has no kex");
        return TT_FAIL;
    }

    if (!TT_SSH_KEX_ALG_DH(sshctx->kex->alg)) {
        TT_ERROR("sshctx has no kex dh");
        return TT_FAIL;
    }

    return tt_sshkexdh_compute(&sshctx->kex->alg_u.kexdh, peerpub, peerpub_len);
}

tt_result_t tt_sshctx_kexdh_set_e(IN tt_sshctx_t *sshctx, IN tt_u8_t *e,
                                  IN tt_u32_t e_len, IN tt_bool_t format)
{
    if (sshctx->kex == NULL) {
        TT_ERROR("sshctx has no kex");
        return TT_FAIL;
    }

    if (!TT_SSH_KEX_ALG_DH(sshctx->kex->alg)) {
        TT_ERROR("sshctx has no kex dh");
        return TT_FAIL;
    }

    return tt_sshkexdh_set_e(&sshctx->kex->alg_u.kexdh, e, e_len, format);
}

tt_result_t tt_sshctx_kexdh_load_e(IN tt_sshctx_t *sshctx)
{
    if (sshctx->kex == NULL) {
        TT_ERROR("sshctx has no kex");
        return TT_FAIL;
    }

    if (!TT_SSH_KEX_ALG_DH(sshctx->kex->alg)) {
        TT_ERROR("sshctx has no kex dh");
        return TT_FAIL;
    }

    return tt_sshkexdh_load_e(&sshctx->kex->alg_u.kexdh);
}

tt_result_t tt_sshctx_kexdh_set_f(IN tt_sshctx_t *sshctx, IN tt_u8_t *f,
                                  IN tt_u32_t f_len, IN tt_bool_t format)
{
    if (sshctx->kex == NULL) {
        TT_ERROR("sshctx has no kex");
        return TT_FAIL;
    }

    if (!TT_SSH_KEX_ALG_DH(sshctx->kex->alg)) {
        TT_ERROR("sshctx has no kex dh");
        return TT_FAIL;
    }

    return tt_sshkexdh_set_f(&sshctx->kex->alg_u.kexdh, f, f_len, format);
}

tt_result_t tt_sshctx_kexdh_load_f(IN tt_sshctx_t *sshctx)
{
    if (sshctx->kex == NULL) {
        TT_ERROR("sshctx has no kex");
        return TT_FAIL;
    }

    if (!TT_SSH_KEX_ALG_DH(sshctx->kex->alg)) {
        TT_ERROR("sshctx has no kex dh");
        return TT_FAIL;
    }

    return tt_sshkexdh_load_f(&sshctx->kex->alg_u.kexdh);
}

tt_result_t tt_sshctx_kexdh_get_k(IN tt_sshctx_t *sshctx)
{
    if (sshctx->kex == NULL) {
        TT_ERROR("sshctx has no kex");
        return TT_FAIL;
    }

    if (!TT_SSH_KEX_ALG_DH(sshctx->kex->alg)) {
        TT_ERROR("sshctx has no kex dh");
        return TT_FAIL;
    }

    return tt_sshkexdh_get_k(&sshctx->kex->alg_u.kexdh, &sshctx->kex->k);
}

// ========================================
// public key
// ========================================

tt_result_t tt_sshctx_pubk_create(IN tt_sshctx_t *sshctx)
{
    tt_sshpubk_t *pubk;

    if (sshctx->pubk != NULL) {
        TT_ERROR("sshctx already has pubk");
        return TT_FAIL;
    }

    pubk = (tt_sshpubk_t *)tt_malloc(sizeof(tt_sshpubk_t));
    if (pubk == NULL) {
        TT_ERROR("no mem for sshctx pubk");
        return TT_FAIL;
    }

    tt_sshpubk_init(pubk);

    sshctx->pubk = pubk;
    return TT_SUCCESS;
}

void tt_sshctx_pubk_destroy(IN tt_sshctx_t *sshctx)
{
    if (sshctx->pubk != NULL) {
        tt_sshpubk_destroy(sshctx->pubk);
        sshctx->pubk = NULL;
    }
}

tt_result_t tt_sshctx_pubk_setalg(IN tt_sshctx_t *sshctx,
                                  IN tt_ssh_pubkey_alg_t alg, IN void *key)
{
    if (sshctx->pubk == NULL) {
        TT_ERROR("sshctx has no pubk");
        return TT_FAIL;
    }

    tt_sshpubk_setalg(sshctx->pubk, alg, key);
    return TT_SUCCESS;
}

tt_result_t tt_sshctx_pubk_sign(IN tt_sshctx_t *sshctx, IN tt_u8_t *data,
                                IN tt_u32_t data_len)
{
    if (sshctx->pubk == NULL) {
        TT_ERROR("sshctx has no pubk");
        return TT_FAIL;
    }

    return tt_sshpubk_sign(sshctx->pubk, data, data_len);
}

// ========================================
// kdf
// ========================================

tt_result_t tt_sshctx_kdf(IN tt_sshctx_t *sshctx)
{
    tt_sshkex_t *kex = sshctx->kex;
    tt_bool_t is_server = sshctx->attr.is_server;
    tt_result_t result;

    tt_u32_t enc_iv_len, enc_key_len, dec_iv_len, dec_key_len;
    tt_u32_t sign_key_len, verify_key_len;
    tt_blob_t k_blob, h_blob;
    tt_sshkdf_t kdf;

    tt_u32_t __done = 0;
#define __KDF_ENC (1 << 0)
#define __KDF_DEC (1 << 1)
#define __KDF_SIGN (1 << 2)
#define __KDF_VERIFY (1 << 3)

    if (kex == NULL) {
        TT_ERROR("no kex");
        return TT_FAIL;
    }
    switch (kex->alg) {
    case TT_SSH_KEX_ALG_DH_G14_SHA1:
    case TT_SSH_KEX_ALG_DH_G1_SHA1: {
        tt_sshkdf_init(&kdf, TT_SSH_HASH_ALG_SHA1);
    } break;

    default: {
        return TT_FAIL;
    } break;
    }

    enc_iv_len = tt_ssh_enc_iv_len(sshctx->enc_alg);
    enc_key_len = tt_ssh_enc_key_len(sshctx->enc_alg);
    dec_iv_len = tt_ssh_enc_iv_len(sshctx->dec_alg);
    dec_key_len = tt_ssh_enc_key_len(sshctx->dec_alg);
    sign_key_len = tt_ssh_mac_key_len(sshctx->sign_alg);
    verify_key_len = tt_ssh_mac_key_len(sshctx->verify_alg);

    tt_buf_get_rblob(&kex->k, &k_blob);
    tt_buf_get_rblob(&kex->h, &h_blob);
    result = tt_sshkdf_run(&kdf, &k_blob, &h_blob, &sshctx->session_id,
                           dec_iv_len, enc_iv_len, dec_key_len, enc_key_len,
                           verify_key_len, sign_key_len);
    if (!TT_OK(result)) { goto k_fail; }

    // enc
    result = tt_sshctx_enc_setalg(sshctx, sshctx->enc_alg,
                                  TT_COND(is_server, TT_BUF_RPOS(&kdf.iv_s2c),
                                          TT_BUF_RPOS(&kdf.iv_c2s)),
                                  enc_iv_len,
                                  TT_COND(is_server, TT_BUF_RPOS(&kdf.enc_s2c),
                                          TT_BUF_RPOS(&kdf.enc_c2s)),
                                  enc_key_len);
    if (!TT_OK(result)) { goto k_fail; }
    __done |= __KDF_ENC;

    // dec
    result = tt_sshctx_dec_setalg(sshctx, sshctx->dec_alg,
                                  TT_COND(is_server, TT_BUF_RPOS(&kdf.iv_c2s),
                                          TT_BUF_RPOS(&kdf.iv_s2c)),
                                  dec_iv_len,
                                  TT_COND(is_server, TT_BUF_RPOS(&kdf.enc_c2s),
                                          TT_BUF_RPOS(&kdf.enc_s2c)),
                                  dec_key_len);
    if (!TT_OK(result)) { goto k_fail; }
    __done |= __KDF_DEC;

    // sign
    result = tt_sshctx_sign_setalg(sshctx, sshctx->sign_alg,
                                   TT_COND(is_server, TT_BUF_RPOS(&kdf.mac_s2c),
                                           TT_BUF_RPOS(&kdf.mac_c2s)),
                                   sign_key_len);
    if (!TT_OK(result)) { goto k_fail; }
    __done |= __KDF_SIGN;

    // verify
    result =
        tt_sshctx_verify_setalg(sshctx, sshctx->verify_alg,
                                TT_COND(is_server, TT_BUF_RPOS(&kdf.mac_c2s),
                                        TT_BUF_RPOS(&kdf.mac_s2c)),
                                verify_key_len);
    if (!TT_OK(result)) { goto k_fail; }
    __done |= __KDF_VERIFY;

    tt_sshkdf_destroy(&kdf);
    return TT_SUCCESS;

k_fail:

    if (__done & __KDF_ENC) { tt_sshctx_enc_destroy(sshctx); }

    if (__done & __KDF_DEC) { tt_sshctx_dec_destroy(sshctx); }

    if (__done & __KDF_SIGN) { tt_sshctx_sign_destroy(sshctx); }

    if (__done & __KDF_VERIFY) { tt_sshctx_verify_destroy(sshctx); }

    tt_sshkdf_destroy(&kdf);
    return TT_FAIL;
}
