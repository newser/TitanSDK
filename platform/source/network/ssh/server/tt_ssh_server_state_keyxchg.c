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

#include <network/ssh/server/tt_ssh_server_state_keyxchg.h>

#include <network/ssh/message/tt_ssh_message.h>
#include <network/ssh/message/tt_ssh_msg_keyinit.h>
#include <network/ssh/server/tt_ssh_server.h>
#include <network/ssh/server/tt_ssh_server_conn.h>

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

static tt_result_t __svr_keyxchg_packet(IN tt_sshsvrconn_t *svrconn,
                                        IN tt_sshmsg_t *msg);
static tt_result_t __svr_keyxchg_keyinit(IN tt_sshsvrconn_t *svrconn,
                                         IN void *param);

static tt_result_t __svr_kex_negotiate(IN tt_sshsvrconn_t *svrconn,
                                       IN tt_sshms_keyinit_t *msg);
static tt_result_t __svr_kex_install(IN tt_sshsvrconn_t *svrconn,
                                     IN tt_ssh_kex_alg_t kex_alg);

static tt_result_t __svr_pubkey_negotiate(IN tt_sshsvrconn_t *svrconn,
                                          IN tt_sshms_keyinit_t *msg);
static tt_result_t __svr_pubkey_install(IN tt_sshsvrconn_t *svrconn,
                                        IN tt_ssh_pubkey_alg_t pubkey_alg);

static tt_result_t __svr_enc_negotiate(IN tt_sshsvrconn_t *svrconn,
                                       IN tt_sshms_keyinit_t *msg);
static tt_result_t __svr_enc_install(IN tt_sshsvrconn_t *svrconn,
                                     IN tt_ssh_enc_alg_t enc_alg);

static tt_result_t __svr_dec_negotiate(IN tt_sshsvrconn_t *svrconn,
                                       IN tt_sshms_keyinit_t *msg);
static tt_result_t __svr_dec_install(IN tt_sshsvrconn_t *svrconn,
                                     IN tt_ssh_enc_alg_t dec_alg);

static tt_result_t __svr_sign_negotiate(IN tt_sshsvrconn_t *svrconn,
                                        IN tt_sshms_keyinit_t *msg);
static tt_result_t __svr_sign_install(IN tt_sshsvrconn_t *svrconn,
                                      IN tt_ssh_mac_alg_t sign_alg);

static tt_result_t __svr_verify_negotiate(IN tt_sshsvrconn_t *svrconn,
                                          IN tt_sshms_keyinit_t *msg);
static tt_result_t __svr_verify_install(IN tt_sshsvrconn_t *svrconn,
                                        IN tt_ssh_mac_alg_t verify_alg);

static tt_result_t __svr_comp_negotiate(IN tt_sshsvrconn_t *svrconn,
                                        IN tt_sshms_keyinit_t *msg);
static tt_result_t __svr_comp_install(IN tt_sshsvrconn_t *svrconn,
                                      IN tt_ssh_cmprs_alg_t comp_alg);

static tt_result_t __svr_uncomp_negotiate(IN tt_sshsvrconn_t *svrconn,
                                          IN tt_sshms_keyinit_t *msg);
static tt_result_t __svr_uncomp_install(IN tt_sshsvrconn_t *svrconn,
                                        IN tt_ssh_cmprs_alg_t uncomp_alg);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_sshsvr_state_keyxchg(IN struct tt_sshsvrconn_s *svrconn,
                             IN tt_sshsvr_event_t event,
                             IN void *param,
                             OUT tt_sshsvr_action_t *svract)
{
    tt_result_t result;

    TT_ASSERT(event < TT_SSHSVREV_NUM);

    switch (event) {
        case TT_SSHSVREV_PACKET: {
            tt_sshmsg_t *msg = (tt_sshmsg_t *)param;

            result = __svr_keyxchg_packet(svrconn, msg);
            if (TT_OK(result)) {
                svrconn->ms_keyinit_in = TT_TRUE;
                if (svrconn->ms_keyinit_out) {
                    svract->new_state = TT_SSHSVRST_KEXDH;
                }
            } else if (result == TT_PROCEEDING) {
                TT_SSH_MSGID_IGNORED(TT_SSHSVRST_KEYXCHG, msg->msg_id);
            } else {
                svract->new_event = TT_SSHSVREV_DISCONNECT;
            }
            return;
        } break;

        case TT_SSHSVREV_KEYINIT: {
            result = __svr_keyxchg_keyinit(svrconn, param);
            if (!TT_OK(result)) {
                svract->new_event = TT_SSHSVREV_DISCONNECT;
                return;
            }

            svrconn->ms_keyinit_out = TT_TRUE;
            if (svrconn->ms_keyinit_in) {
                svract->new_state = TT_SSHSVRST_KEXDH;
            }
            return;
        } break;

        default: {
            TT_SSH_EV_IGNORED(TT_SSHSVRST_KEYXCHG, event);
            return;
        } break;
    }
}

tt_result_t __svr_keyxchg_packet(IN tt_sshsvrconn_t *svrconn,
                                 IN tt_sshmsg_t *msg)
{
    switch (msg->msg_id) {
        case TT_SSH_MSGID_KEXINIT: {
            tt_sshms_keyinit_t *ms_keyinit =
                TT_SSHMSG_CAST(msg, tt_sshms_keyinit_t);
            tt_result_t result;
            tt_u8_t *ic;
            tt_u32_t ic_len;

            // record i_c
            result = tt_sshmsg_peek_payload(&msg->buf, &ic, &ic_len);
            TT_ASSERT_SSH(TT_OK(result));
            if (!TT_OK(
                    tt_sshctx_kex_setic(&svrconn->ctx, ic, ic_len, TT_TRUE))) {
                return TT_FAIL;
            }

            // algorithms negotiation

            result = __svr_kex_negotiate(svrconn, ms_keyinit);
            if (!TT_OK(result)) {
                return TT_FAIL;
            }

            result = __svr_pubkey_negotiate(svrconn, ms_keyinit);
            if (!TT_OK(result)) {
                return TT_FAIL;
            }

            result = __svr_enc_negotiate(svrconn, ms_keyinit);
            if (!TT_OK(result)) {
                return TT_FAIL;
            }
            result = __svr_dec_negotiate(svrconn, ms_keyinit);
            if (!TT_OK(result)) {
                return TT_FAIL;
            }

            result = __svr_sign_negotiate(svrconn, ms_keyinit);
            if (!TT_OK(result)) {
                return TT_FAIL;
            }
            result = __svr_verify_negotiate(svrconn, ms_keyinit);
            if (!TT_OK(result)) {
                return TT_FAIL;
            }

            result = __svr_comp_negotiate(svrconn, ms_keyinit);
            if (!TT_OK(result)) {
                return TT_FAIL;
            }
            result = __svr_uncomp_negotiate(svrconn, ms_keyinit);
            if (!TT_OK(result)) {
                return TT_FAIL;
            }

            return TT_SUCCESS;
        } break;

        default: {
            TT_SSH_MSG_FAILURE(TT_SSHSVRST_KEYXCHG, msg->msg_id);
            return TT_FAIL;
        } break;
    }
}

tt_result_t __svr_keyxchg_keyinit(IN tt_sshsvrconn_t *svrconn, IN void *param)
{
    tt_sshmsg_t *msg;
    tt_u32_t i;
    tt_u8_t *is;
    tt_u32_t is_len;

    msg = tt_sshms_keyinit_create();
    if (msg == NULL) {
        return TT_FAIL;
    }

    // may ask callback to set cipher algorithms, but now use default

    // key exchange
    for (i = 0; i < tt_g_ssh_kex_pref_num; ++i) {
        tt_sshms_keyinit_addalg_kex(msg, tt_g_ssh_kex_pref[i]);
    }

    // public key alg
    for (i = 0; i < tt_g_ssh_pubkey_pref_num; ++i) {
        tt_sshms_keyinit_addalg_pubkey(msg, tt_g_ssh_pubkey_pref[i]);
    }

    // encrypt, client to server
    for (i = 0; i < tt_g_ssh_enc_c2s_pref_num; ++i) {
        tt_sshms_keyinit_addalg_enc(msg, tt_g_ssh_enc_c2s_pref[i], TT_TRUE);
    }

    // encrypt, server to client
    for (i = 0; i < tt_g_ssh_enc_s2c_pref_num; ++i) {
        tt_sshms_keyinit_addalg_enc(msg, tt_g_ssh_enc_s2c_pref[i], TT_FALSE);
    }

    // mac, client to server
    for (i = 0; i < tt_g_ssh_mac_c2s_pref_num; ++i) {
        tt_sshms_keyinit_addalg_mac(msg, tt_g_ssh_mac_c2s_pref[i], TT_TRUE);
    }

    // mac, server to client
    for (i = 0; i < tt_g_ssh_mac_s2c_pref_num; ++i) {
        tt_sshms_keyinit_addalg_mac(msg, tt_g_ssh_mac_s2c_pref[i], TT_FALSE);
    }

    // compression, client to server
    for (i = 0; i < tt_g_ssh_cmprs_c2s_pref_num; ++i) {
        tt_sshms_keyinit_addalg_comp(msg, tt_g_ssh_cmprs_c2s_pref[i], TT_TRUE);
    }

    // compression, server to client
    for (i = 0; i < tt_g_ssh_cmprs_s2c_pref_num; ++i) {
        tt_sshms_keyinit_addalg_comp(msg, tt_g_ssh_cmprs_s2c_pref[i], TT_FALSE);
    }

    // need record is
    if (!TT_OK(tt_sshmsg_render(msg, svrconn->ctx.encrypt.block_len, NULL))) {
        return TT_FAIL;
    }
    tt_sshmsg_peek_payload(&msg->buf, &is, &is_len);
    if (!TT_OK(tt_sshctx_kex_setis(&svrconn->ctx, is, is_len, TT_TRUE))) {
        return TT_FAIL;
    }

    return tt_sshsvrconn_send(svrconn, msg);
}

tt_result_t __svr_kex_negotiate(IN tt_sshsvrconn_t *svrconn,
                                IN tt_sshms_keyinit_t *msg)
{
    tt_u32_t i, j;

    for (i = 0; i < tt_g_ssh_kex_pref_num; ++i) {
        for (j = 0; j < msg->kex_alg_num; ++j) {
            if (msg->kex_alg[j] == tt_g_ssh_kex_pref[i]) {
                return __svr_kex_install(svrconn, msg->kex_alg[j]);
            }
        }
    }

    TT_ERROR("fail to negotiate key exchange alg");
    return TT_FAIL;
}

tt_result_t __svr_kex_install(IN tt_sshsvrconn_t *svrconn,
                              IN tt_ssh_kex_alg_t kex_alg)
{
    tt_sshctx_t *ctx = &svrconn->ctx;

    switch (kex_alg) {
        case TT_SSH_KEX_ALG_DH_G14_SHA1:
        case TT_SSH_KEX_ALG_DH_G1_SHA1: {
            if (!TT_OK(tt_sshctx_kex_setalg(ctx, kex_alg))) {
                return TT_FAIL;
            }

            return TT_SUCCESS;
        } break;

        default: {
            // never reach here
            return TT_FAIL;
        } break;
    }
}

tt_result_t __svr_pubkey_negotiate(IN tt_sshsvrconn_t *svrconn,
                                   IN tt_sshms_keyinit_t *msg)
{
    tt_u32_t i, j;

    for (i = 0; i < tt_g_ssh_pubkey_pref_num; ++i) {
        for (j = 0; j < msg->pubkey_alg_num; ++j) {
            if (msg->pubkey_alg[j] == tt_g_ssh_pubkey_pref[i]) {
                return __svr_pubkey_install(svrconn, msg->pubkey_alg[j]);
            }
        }
    }

    TT_ERROR("fail to choose pub key alg");
    return TT_FAIL;
}

tt_result_t __svr_pubkey_install(IN tt_sshsvrconn_t *svrconn,
                                 IN tt_ssh_pubkey_alg_t pubkey_alg)
{
    tt_sshctx_t *ctx = &svrconn->ctx;

    switch (pubkey_alg) {
        case TT_SSH_PUBKEY_ALG_RSA: {
            tt_rsa_t *rsapub, *rsapriv;
            tt_rsa_number_t rsa_number;
            tt_result_t result;

            rsapub = tt_sshsvr_get_rsapub(svrconn->server);
            if (rsapub == NULL) {
                return TT_FAIL;
            }
            rsapriv = tt_sshsvr_get_rsapriv(svrconn->server);
            if (rsapriv == NULL) {
                return TT_FAIL;
            }

            // use public key to fill k_s
            result = tt_rsa_get_number(rsapub, &rsa_number);
            if (!TT_OK(result)) {
                return TT_FAIL;
            }
            result = tt_sshctx_kex_setks_rsa(ctx,
                                             &rsa_number.pubnum.pub_exp,
                                             &rsa_number.pubnum.modulus);
            if (!TT_OK(result)) {
                return TT_FAIL;
            }

            // use private key to sign
            tt_sshctx_pubk_setalg(&svrconn->ctx,
                                  TT_SSH_PUBKEY_ALG_RSA,
                                  rsapriv);

            return TT_SUCCESS;
        } break;

        default: {
            // never reach here
            return TT_FAIL;
        } break;
    }
}

tt_result_t __svr_enc_negotiate(IN tt_sshsvrconn_t *svrconn,
                                IN tt_sshms_keyinit_t *msg)
{
    tt_u32_t i, j;

    // must follow remote preference
    for (j = 0; j < msg->enc_s2c_num; ++j) {
        for (i = 0; i < tt_g_ssh_enc_s2c_pref_num; ++i) {
            if (msg->enc_s2c[j] == tt_g_ssh_enc_s2c_pref[i]) {
                return __svr_enc_install(svrconn, msg->enc_s2c[j]);
            }
        }
    }

    TT_ERROR("fail to choose enc alg");
    return TT_FAIL;
}

tt_result_t __svr_enc_install(IN tt_sshsvrconn_t *svrconn,
                              IN tt_ssh_enc_alg_t enc_alg)
{
    tt_sshctx_t *ctx = &svrconn->ctx;

    // save enc alg
    ctx->enc_alg = enc_alg;

    return TT_SUCCESS;
}

tt_result_t __svr_dec_negotiate(IN tt_sshsvrconn_t *svrconn,
                                IN tt_sshms_keyinit_t *msg)
{
    tt_u32_t i, j;

    // must follow remote preference
    for (j = 0; j < msg->enc_c2s_num; ++j) {
        for (i = 0; i < tt_g_ssh_enc_c2s_pref_num; ++i) {
            if (msg->enc_c2s[j] == tt_g_ssh_enc_c2s_pref[i]) {
                return __svr_dec_install(svrconn, msg->enc_c2s[j]);
            }
        }
    }

    TT_ERROR("fail to choose dec alg");
    return TT_FAIL;
}

tt_result_t __svr_dec_install(IN tt_sshsvrconn_t *svrconn,
                              IN tt_ssh_enc_alg_t dec_alg)
{
    tt_sshctx_t *ctx = &svrconn->ctx;

    // save dec alg
    ctx->dec_alg = dec_alg;

    return TT_SUCCESS;
}

tt_result_t __svr_sign_negotiate(IN tt_sshsvrconn_t *svrconn,
                                 IN tt_sshms_keyinit_t *msg)
{
    tt_u32_t i, j;

    // must follow remote preference
    for (j = 0; j < msg->mac_s2c_num; ++j) {
        for (i = 0; i < tt_g_ssh_mac_s2c_pref_num; ++i) {
            if (msg->mac_s2c[j] == tt_g_ssh_mac_s2c_pref[i]) {
                return __svr_sign_install(svrconn, msg->mac_s2c[j]);
            }
        }
    }

    TT_ERROR("fail to choose sign alg");
    return TT_FAIL;
}

tt_result_t __svr_sign_install(IN tt_sshsvrconn_t *svrconn,
                               IN tt_ssh_mac_alg_t sign_alg)
{
    tt_sshctx_t *ctx = &svrconn->ctx;

    // save sign alg
    ctx->sign_alg = sign_alg;

    return TT_SUCCESS;
}

tt_result_t __svr_verify_negotiate(IN tt_sshsvrconn_t *svrconn,
                                   IN tt_sshms_keyinit_t *msg)
{
    tt_u32_t i, j;

    // must follow remote preference
    for (j = 0; j < msg->mac_c2s_num; ++j) {
        for (i = 0; i < tt_g_ssh_mac_c2s_pref_num; ++i) {
            if (msg->mac_c2s[j] == tt_g_ssh_mac_c2s_pref[i]) {
                return __svr_verify_install(svrconn, msg->mac_c2s[j]);
            }
        }
    }

    TT_ERROR("fail to choose verify alg");
    return TT_FAIL;
}

tt_result_t __svr_verify_install(IN tt_sshsvrconn_t *svrconn,
                                 IN tt_ssh_mac_alg_t verify_alg)
{
    tt_sshctx_t *ctx = &svrconn->ctx;

    // save verify alg
    ctx->verify_alg = verify_alg;

    return TT_SUCCESS;
}

tt_result_t __svr_comp_negotiate(IN tt_sshsvrconn_t *svrconn,
                                 IN tt_sshms_keyinit_t *msg)
{
    tt_u32_t i, j;

    for (i = 0; i < tt_g_ssh_cmprs_s2c_pref_num; ++i) {
        for (j = 0; j < msg->comp_s2c_num; ++j) {
            if (msg->comp_s2c[j] == tt_g_ssh_cmprs_s2c_pref[i]) {
                return __svr_comp_install(svrconn, msg->comp_s2c[j]);
            }
        }
    }

    TT_ERROR("fail to choose compress alg");
    return TT_FAIL;
}

tt_result_t __svr_comp_install(IN tt_sshsvrconn_t *svrconn,
                               IN tt_ssh_cmprs_alg_t comp_alg)
{
    // only support none
    return TT_COND(comp_alg == TT_SSH_CMPRS_ALG_NONE, TT_SUCCESS, TT_FAIL);
}

tt_result_t __svr_uncomp_negotiate(IN tt_sshsvrconn_t *svrconn,
                                   IN tt_sshms_keyinit_t *msg)
{
    tt_u32_t i, j;

    for (i = 0; i < tt_g_ssh_cmprs_c2s_pref_num; ++i) {
        for (j = 0; j < msg->comp_c2s_num; ++j) {
            if (msg->comp_c2s[j] == tt_g_ssh_cmprs_c2s_pref[i]) {
                return __svr_uncomp_install(svrconn, msg->comp_c2s[j]);
            }
        }
    }

    TT_ERROR("fail to choose uncompress alg");
    return TT_FAIL;
}

tt_result_t __svr_uncomp_install(IN tt_sshsvrconn_t *svrconn,
                                 IN tt_ssh_cmprs_alg_t uncomp_alg)
{
    // only support none
    return TT_COND(uncomp_alg == TT_SSH_CMPRS_ALG_NONE, TT_SUCCESS, TT_FAIL);
}
