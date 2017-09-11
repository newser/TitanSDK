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

#include <network/ssh/message/tt_ssh_msg_kexdh_reply.h>

#include <crypto/tt_rsa.h>
#include <crypto/tt_sha.h>
#include <network/ssh/context/tt_ssh_context.h>
#include <network/ssh/context/tt_ssh_kex.h>
#include <network/ssh/context/tt_ssh_pubkey.h>
#include <network/ssh/message/tt_ssh_message.h>
#include <network/ssh/message/tt_ssh_render.h>

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

static tt_result_t __kexdh_reply_render_prepare(IN struct tt_sshmsg_s *msg,
                                                OUT tt_u32_t *len,
                                                OUT tt_ssh_render_mode_t *mode);
static tt_result_t __kexdh_reply_render(IN struct tt_sshmsg_s *msg,
                                        IN OUT tt_buf_t *buf);

static tt_result_t __kexdh_reply_parse(IN struct tt_sshmsg_s *msg,
                                       IN tt_buf_t *data);

static tt_sshmsg_itf_t __kexdh_reply_op = {
    NULL,
    NULL,
    NULL,

    __kexdh_reply_render_prepare,
    __kexdh_reply_render,

    __kexdh_reply_parse,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_sshmsg_t *tt_sshmsg_kexdh_reply_create()
{
    return tt_sshmsg_create(TT_SSH_MSGID_KEXDH_REPLY,
                            sizeof(tt_sshmsg_kexdh_reply_t),
                            &__kexdh_reply_op);
}

void tt_sshmsg_kexdh_reply_setctx(IN tt_sshmsg_t *msg,
                                  IN struct tt_sshctx_s *sshctx)
{
    tt_sshmsg_kexdh_reply_t *kr;

    TT_ASSERT_SSH(msg != NULL);
    TT_ASSERT_SSH(msg->msg_id == TT_SSH_MSGID_KEXDH_REPLY);

    kr = TT_SSHMSG_CAST(msg, tt_sshmsg_kexdh_reply_t);

    kr->sshctx = sshctx;
}

tt_result_t __kexdh_reply_render_prepare(IN struct tt_sshmsg_s *msg,
                                         OUT tt_u32_t *len,
                                         OUT tt_ssh_render_mode_t *mode)
{
    tt_sshmsg_kexdh_reply_t *kr;
    tt_u32_t msg_len;

    tt_sshkex_t *kex;
    tt_sshpubk_t *pubk;

    TT_ASSERT_SSH(msg != NULL);
    TT_ASSERT_SSH(msg->msg_id == TT_SSH_MSGID_KEXDH_REPLY);

    kr = TT_SSHMSG_CAST(msg, tt_sshmsg_kexdh_reply_t);
    TT_ASSERT(kr->sshctx != NULL);

    kex = kr->sshctx->kex;
    TT_ASSERT(kex != NULL);

    pubk = kr->sshctx->pubk;
    TT_ASSERT(pubk != NULL);

    /*
     byte SSH_MSG_KEXDH_REPLY
     string server public host key and certificates (K_S)
     mpint f
     string signature of H
     */

    /*
     signature:
     string "ssh-rsa"
     string rsa_signature_blob
     */

    // id
    msg_len = 1;

    // K_S, already formatted as string
    msg_len += TT_BUF_RLEN(&kex->k_s);

    // f, already formatted as mpint
    switch (kex->alg) {
        case TT_SSH_KEX_ALG_DH_G1_SHA1:
        case TT_SSH_KEX_ALG_DH_G14_SHA1: {
            msg_len += TT_BUF_RLEN(&kex->alg_u.kexdh.f);
        } break;

        default: {
            return TT_FAIL;
        } break;
    }

    // signature of H
    switch (pubk->alg) {
        case TT_SSH_PUBKEY_ALG_RSA: {
            msg_len += 19 + pubk->alg_u.rsa->block_size;
        } break;

        default: {
            return TT_FAIL;
        } break;
    }

    *len = msg_len;
    *mode = TT_SSH_RENDER_MODE_BINARY;
    return TT_SUCCESS;
}

tt_result_t __kexdh_reply_render(IN struct tt_sshmsg_s *msg,
                                 IN OUT tt_buf_t *buf)
{
    tt_sshmsg_kexdh_reply_t *kr;
    tt_buf_t *data;

    tt_sshkex_t *kex;
    tt_sshpubk_t *pubk;

    TT_ASSERT_SSH(msg != NULL);
    TT_ASSERT_SSH(msg->msg_id == TT_SSH_MSGID_KEXDH_REPLY);

    kr = TT_SSHMSG_CAST(msg, tt_sshmsg_kexdh_reply_t);
    TT_ASSERT(kr->sshctx != NULL);

    kex = kr->sshctx->kex;
    TT_ASSERT(kex != NULL);

    pubk = kr->sshctx->pubk;
    TT_ASSERT(pubk != NULL);

    /*
     byte SSH_MSG_KEXDH_REPLY
     string server public host key and certificates (K_S)
     mpint f
     string signature of H
     */

    /*
     signature:
     string "ssh-rsa"
     string rsa_signature_blob
     */

    // id
    TT_DO(tt_buf_put_u8(buf, TT_SSH_MSGID_KEXDH_REPLY));

    // K_S, already formatted as string
    data = &kex->k_s;
    TT_DO(tt_buf_put(buf, TT_BUF_RPOS(data), TT_BUF_RLEN(data)));

    // f, already formatted as mpint
    switch (kex->alg) {
        case TT_SSH_KEX_ALG_DH_G1_SHA1:
        case TT_SSH_KEX_ALG_DH_G14_SHA1: {
            data = &kex->alg_u.kexdh.f;
            TT_DO(tt_buf_put(buf, TT_BUF_RPOS(data), TT_BUF_RLEN(data)));
        } break;

        default: {
            return TT_FAIL;
        } break;
    }

    // signature of H
    switch (pubk->alg) {
        case TT_SSH_PUBKEY_ALG_RSA: {
            const tt_char_t *ssh_rsa;
            tt_u32_t ssh_rsa_len;

            TT_DO(tt_buf_put_u32_n(buf, 15 + pubk->alg_u.rsa->block_size));

            // string "ssh-rsa"
            ssh_rsa = tt_g_ssh_pubkey_alg_name[TT_SSH_PUBKEY_ALG_RSA];
            // ssh_rsa_len = (tt_u32_t)tt_strlen(ssh_rsa);
            ssh_rsa_len = 7;
            TT_DO(tt_ssh_string_render(buf, (tt_u8_t *)ssh_rsa, ssh_rsa_len));

            // string rsa_signature_blob
            TT_DO(tt_ssh_string_render(buf,
                                       TT_BUF_RPOS(&pubk->signature),
                                       TT_BUF_RLEN(&pubk->signature)));
        } break;

        default: {
            return TT_FAIL;
        } break;
    }

    return TT_SUCCESS;
}

tt_result_t __kexdh_reply_parse(IN struct tt_sshmsg_s *msg, IN tt_buf_t *data)
{
    tt_sshmsg_kexdh_reply_t *kr;

    TT_ASSERT_SSH(msg != NULL);
    TT_ASSERT_SSH(msg->msg_id == TT_SSH_MSGID_KEXDH_REPLY);

    kr = TT_SSHMSG_CAST(msg, tt_sshmsg_kexdh_reply_t);

    // todo:
    (void)kr;
    TT_ASSERT(0);

    return TT_SUCCESS;
}
