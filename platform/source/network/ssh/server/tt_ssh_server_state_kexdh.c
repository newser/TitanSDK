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

#include <network/ssh/server/tt_ssh_server_state_kexdh.h>

#include <misc/tt_reference_counter.h>
#include <network/ssh/message/tt_ssh_message.h>
#include <network/ssh/message/tt_ssh_msg_kexdh_init.h>
#include <network/ssh/message/tt_ssh_msg_kexdh_reply.h>
#include <network/ssh/message/tt_ssh_msg_newkeys.h>
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

static void __svr_kexdh_packet(IN tt_sshsvrconn_t *svrconn,
                               IN tt_sshmsg_t *msg,
                               OUT tt_sshsvr_action_t *svract);
tt_result_t __svr_kexdh_pkt_dhinit(IN tt_sshsvrconn_t *svrconn,
                                   IN tt_sshmsg_t *msg);

static tt_result_t __svr_kexdh_reply(IN tt_sshsvrconn_t *svrconn,
                                     IN void *param);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_sshsvr_state_kexdh(IN struct tt_sshsvrconn_s *svrconn,
                           IN tt_sshsvr_event_t event,
                           IN void *param,
                           OUT tt_sshsvr_action_t *svract)
{
    tt_result_t result;

    TT_ASSERT(event < TT_SSHSVREV_NUM);

    switch (event) {
        case TT_SSHSVREV_PACKET: {
            __svr_kexdh_packet(svrconn, (tt_sshmsg_t *)param, svract);
            return;
        } break;

        case TT_SSHSVREV_KEXDH_REPLY: {
            tt_sshmsg_t *newkeys;

            result = __svr_kexdh_reply(svrconn, param);
            if (!TT_OK(result)) {
                svract->new_event = TT_SSHSVREV_DISCONNECT;
                return;
            }

            // send SSH_MSG_NEWKEYS
            newkeys = tt_sshms_newkeys_create();
            if (newkeys == NULL) {
                svract->new_event = TT_SSHSVREV_DISCONNECT;
                return;
            }
            if (!TT_OK(tt_sshsvrconn_send(svrconn, newkeys))) {
                tt_sshmsg_release(newkeys);

                svract->new_event = TT_SSHSVREV_DISCONNECT;
                return;
            }

            svrconn->ms_newkeys_out = TT_TRUE;
            if (svrconn->ms_newkeys_in) {
                svract->new_state = TT_SSHSVRST_KEX_DONE;

                // remove unnecessary things
                tt_sshctx_clear(&svrconn->ctx);

                return;
            }

            return;
        } break;

        default: {
            TT_SSH_EV_IGNORED(TT_SSHSVRST_KEXDH, event);
            return;
        } break;
    }
}

void __svr_kexdh_packet(IN tt_sshsvrconn_t *svrconn,
                        IN tt_sshmsg_t *msg,
                        OUT tt_sshsvr_action_t *svract)
{
    tt_result_t result;

    switch (msg->msg_id) {
        case TT_SSH_MSGID_KEXDH_INIT: {
            result = __svr_kexdh_pkt_dhinit(svrconn, msg);
            if (TT_OK(result)) {
                svract->new_event = TT_SSHSVREV_KEXDH_REPLY;
            } else if (result == TT_PROCEEDING) {
                TT_SSH_MSGID_IGNORED(TT_SSHSVRST_KEXDH, msg->msg_id);
            } else {
                svract->new_event = TT_SSHSVREV_DISCONNECT;
            }

            return;
        } break;

        case TT_SSH_MSGID_NEWKEYS: {
            svrconn->ms_newkeys_in = TT_TRUE;
            if (svrconn->ms_newkeys_out) {
                svract->new_state = TT_SSHSVRST_KEX_DONE;

                // remove unnecessary things
                tt_sshctx_clear(&svrconn->ctx);
            }

            return;
        } break;

        default: {
            TT_SSH_MSG_FAILURE(TT_SSHSVRST_KEXDH, msg->msg_id);
            svract->new_event = TT_SSHSVREV_DISCONNECT;

            return;
        } break;
    }
}

tt_result_t __svr_kexdh_pkt_dhinit(IN tt_sshsvrconn_t *svrconn,
                                   IN tt_sshmsg_t *msg)
{
    tt_sshmsg_kexdh_init_t *ki = TT_SSHMSG_CAST(msg, tt_sshmsg_kexdh_init_t);
    tt_sshctx_t *ctx = &svrconn->ctx;
    tt_result_t result;

    // compute shared secret
    result = tt_sshctx_kexdh_compute(ctx, ki->e.addr, ki->e.len);
    if (!TT_OK(result)) {
        return TT_FAIL;
    }

    // e
    result = tt_sshctx_kexdh_set_e(ctx, ki->e.addr, ki->e.len, TT_TRUE);
    if (!TT_OK(result)) {
        return TT_FAIL;
    }

    // f
    result = tt_sshctx_kexdh_load_f(ctx);
    if (!TT_OK(result)) {
        return TT_FAIL;
    }

    // k
    result = tt_sshctx_kexdh_get_k(ctx);
    if (!TT_OK(result)) {
        return TT_FAIL;
    }

    // h
    result = tt_sshctx_kex_calc_h(ctx);
    if (!TT_OK(result)) {
        return TT_FAIL;
    }

    // signature of h
    result = tt_sshctx_pubk_sign(ctx,
                                 TT_BUF_RPOS(&ctx->kex->h),
                                 TT_BUF_RLEN(&ctx->kex->h));
    if (!TT_OK(result)) {
        return TT_FAIL;
    }

    // session id
    result = tt_sshctx_load_session_id(ctx);
    if (!TT_OK(result)) {
        return TT_FAIL;
    }

    // kdf
    result = tt_sshctx_kdf(ctx);
    if (!TT_OK(result)) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t __svr_kexdh_reply(IN tt_sshsvrconn_t *svrconn, IN void *param)
{
    tt_sshmsg_t *msg;

    msg = tt_sshmsg_kexdh_reply_create();
    if (msg == NULL) {
        return TT_FAIL;
    }

    tt_sshmsg_kexdh_reply_setctx(msg, &svrconn->ctx);

    return tt_sshsvrconn_send(svrconn, msg);
}
