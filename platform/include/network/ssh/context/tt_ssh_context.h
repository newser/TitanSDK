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
@file tt_ssh_context.h
@brief ts ssh context
*/

#ifndef __TT_SSH_CONTEXT__
#define __TT_SSH_CONTEXT__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <network/ssh/context/tt_ssh_encrypt.h>
#include <network/ssh/context/tt_ssh_kex.h>
#include <network/ssh/context/tt_ssh_mac.h>
#include <network/ssh/context/tt_ssh_pubkey.h>
#include <network/ssh/tt_ssh_def.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct
{
    tt_bool_t is_server : 1;
} tt_sshctx_attr_t;

typedef struct tt_sshctx_s
{
    tt_sshctx_attr_t attr;

    // key exchange
    tt_sshkex_t *kex;
    tt_blob_t session_id;

    // public key algorithm
    tt_sshpubk_t *pubk;

    // encryption
    tt_ssh_enc_alg_t enc_alg;
    tt_sshenc_t encrypt;

    tt_ssh_enc_alg_t dec_alg;
    tt_sshenc_t decrypt;

    // integrity
    tt_ssh_mac_alg_t sign_alg;
    tt_sshmac_t sign;
    tt_u32_t sign_seq;

    tt_ssh_mac_alg_t verify_alg;
    tt_sshmac_t verify;
    tt_u32_t verify_seq;
} tt_sshctx_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_result_t tt_sshctx_create(IN tt_sshctx_t *sshctx,
                                    IN tt_sshctx_attr_t *attr);

tt_export void tt_sshctx_destroy(IN tt_sshctx_t *sshctx);

// remove unnecessary memory, only keep data for enc/dec/sign/verify
tt_export void tt_sshctx_clear(IN tt_sshctx_t *sshctx);

// ========================================
// key exchange
// ========================================

tt_export tt_result_t tt_sshctx_kex_create(IN tt_sshctx_t *sshctx);

tt_export void tt_sshctx_kex_destroy(IN tt_sshctx_t *sshctx);

tt_export tt_result_t tt_sshctx_kex_setalg(IN tt_sshctx_t *sshctx,
                                        IN tt_ssh_kex_alg_t alg);

tt_export tt_result_t tt_sshctx_kex_setks(IN tt_sshctx_t *sshctx,
                                       IN tt_u8_t *ks,
                                       IN tt_u32_t ks_len,
                                       IN tt_bool_t format);
tt_export tt_result_t tt_sshctx_kex_setks_rsa(IN tt_sshctx_t *sshctx,
                                           IN tt_blob_t *e,
                                           IN tt_blob_t *n);

tt_export tt_result_t tt_sshctx_kex_setvs(IN tt_sshctx_t *sshctx,
                                       IN tt_u8_t *vs,
                                       IN tt_u32_t vs_len,
                                       IN tt_bool_t format);
tt_export tt_result_t tt_sshctx_kex_setvc(IN tt_sshctx_t *sshctx,
                                       IN tt_u8_t *vc,
                                       IN tt_u32_t vc_len,
                                       IN tt_bool_t format);
tt_export tt_result_t tt_sshctx_kex_setis(IN tt_sshctx_t *sshctx,
                                       IN tt_u8_t *is,
                                       IN tt_u32_t is_len,
                                       IN tt_bool_t format);
tt_export tt_result_t tt_sshctx_kex_setic(IN tt_sshctx_t *sshctx,
                                       IN tt_u8_t *ic,
                                       IN tt_u32_t ic_len,
                                       IN tt_bool_t format);

tt_export tt_result_t tt_sshctx_kex_calc_h(IN tt_sshctx_t *sshctx);

tt_export tt_result_t tt_sshctx_load_session_id(IN tt_sshctx_t *sshctx);

// dh ...
tt_export tt_result_t tt_sshctx_kexdh_compute(IN tt_sshctx_t *sshctx,
                                           IN tt_u8_t *peerpub,
                                           IN tt_u32_t peerpub_len);

tt_export tt_result_t tt_sshctx_kexdh_set_e(IN tt_sshctx_t *sshctx,
                                         IN tt_u8_t *e,
                                         IN tt_u32_t e_len,
                                         IN tt_bool_t format);
tt_export tt_result_t tt_sshctx_kexdh_load_e(IN tt_sshctx_t *sshctx);

tt_export tt_result_t tt_sshctx_kexdh_set_f(IN tt_sshctx_t *sshctx,
                                         IN tt_u8_t *f,
                                         IN tt_u32_t f_len,
                                         IN tt_bool_t format);
tt_export tt_result_t tt_sshctx_kexdh_load_f(IN tt_sshctx_t *sshctx);

tt_export tt_result_t tt_sshctx_kexdh_get_k(IN tt_sshctx_t *sshctx);

// ========================================
// public key
// ========================================

tt_export tt_result_t tt_sshctx_pubk_create(IN tt_sshctx_t *sshctx);

tt_export void tt_sshctx_pubk_destroy(IN tt_sshctx_t *sshctx);

tt_export tt_result_t tt_sshctx_pubk_setalg(IN tt_sshctx_t *sshctx,
                                         IN tt_ssh_pubkey_alg_t alg,
                                         IN void *key);

tt_export tt_result_t tt_sshctx_pubk_sign(IN tt_sshctx_t *sshctx,
                                       IN tt_u8_t *data,
                                       IN tt_u32_t data_len);

// ========================================
// encryption
// ========================================

tt_inline tt_result_t tt_sshctx_enc_setalg(IN tt_sshctx_t *sshctx,
                                           IN tt_ssh_enc_alg_t alg,
                                           IN tt_u8_t *iv,
                                           IN tt_u32_t iv_len,
                                           IN tt_u8_t *key,
                                           IN tt_u32_t key_len)
{
    return tt_sshenc_setalg(&sshctx->encrypt,
                            alg,
                            TT_TRUE,
                            iv,
                            iv_len,
                            key,
                            key_len);
}

tt_inline void tt_sshctx_enc_destroy(IN tt_sshctx_t *sshctx)
{
    tt_sshenc_destroy(&sshctx->encrypt);
}

tt_inline tt_result_t tt_sshctx_encrypt(IN tt_sshctx_t *sshctx,
                                        IN tt_u8_t *data,
                                        IN tt_u32_t data_len)
{
    return tt_sshenc_encrypt(&sshctx->encrypt, data, data_len);
}

tt_inline tt_result_t tt_sshctx_dec_setalg(IN tt_sshctx_t *sshctx,
                                           IN tt_ssh_enc_alg_t alg,
                                           IN tt_u8_t *iv,
                                           IN tt_u32_t iv_len,
                                           IN tt_u8_t *key,
                                           IN tt_u32_t key_len)
{
    return tt_sshenc_setalg(&sshctx->decrypt,
                            alg,
                            TT_FALSE,
                            iv,
                            iv_len,
                            key,
                            key_len);
}

tt_inline void tt_sshctx_dec_destroy(IN tt_sshctx_t *sshctx)
{
    tt_sshenc_destroy(&sshctx->decrypt);
}

tt_inline tt_result_t tt_sshctx_decrypt(IN tt_sshctx_t *sshctx,
                                        IN tt_u8_t *data,
                                        IN tt_u32_t data_len)
{
    return tt_sshenc_decrypt(&sshctx->decrypt, data, data_len);
}

// ========================================
// integrity
// ========================================

tt_inline tt_result_t tt_sshctx_sign_setalg(IN tt_sshctx_t *sshctx,
                                            IN tt_ssh_mac_alg_t alg,
                                            IN tt_u8_t *key,
                                            IN tt_u32_t key_len)
{
    return tt_sshmac_setalg(&sshctx->sign, alg, key, key_len);
}

tt_inline void tt_sshctx_sign_destroy(IN tt_sshctx_t *sshctx)
{
    tt_sshmac_destroy(&sshctx->sign);
}

tt_inline void tt_sshctx_sign_inc_seq(IN tt_sshctx_t *sshctx)
{
    ++sshctx->sign_seq;
}

tt_inline tt_result_t tt_sshctx_sign(IN tt_sshctx_t *sshctx,
                                     IN tt_u8_t *data,
                                     IN tt_u32_t data_len,
                                     OUT tt_u8_t *signature,
                                     IN tt_u32_t signature_len)
{
    return tt_sshmac_sign(&sshctx->sign,
                          sshctx->sign_seq,
                          data,
                          data_len,
                          signature,
                          signature_len);
}

tt_inline tt_result_t tt_sshctx_verify_setalg(IN tt_sshctx_t *sshctx,
                                              IN tt_ssh_mac_alg_t alg,
                                              IN tt_u8_t *key,
                                              IN tt_u32_t key_len)
{
    return tt_sshmac_setalg(&sshctx->verify, alg, key, key_len);
}

tt_inline void tt_sshctx_verify_destroy(IN tt_sshctx_t *sshctx)
{
    tt_sshmac_destroy(&sshctx->verify);
}

tt_inline void tt_sshctx_verify_inc_seq(IN tt_sshctx_t *sshctx)
{
    ++sshctx->verify_seq;
}

tt_inline tt_result_t tt_sshctx_verify(IN tt_sshctx_t *sshctx,
                                       IN tt_u8_t *data,
                                       IN tt_u32_t data_len,
                                       IN tt_u8_t *signature,
                                       IN tt_u32_t signature_len)
{
    return tt_sshmac_verify(&sshctx->verify,
                            sshctx->verify_seq,
                            data,
                            data_len,
                            signature,
                            signature_len);
}

// ========================================
// kdf
// ========================================

tt_export tt_result_t tt_sshctx_kdf(IN tt_sshctx_t *sshctx);

#endif /* __TT_SSH_CONTEXT__ */
