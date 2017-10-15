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

#include <network/ssh/server/tt_ssh_server_state_verxchg.h>

#include <network/ssh/message/tt_ssh_message.h>
#include <network/ssh/message/tt_ssh_msg_verxchg.h>
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

static tt_result_t __svr_verxchg_packet(IN tt_sshsvrconn_t *svrconn,
                                        IN tt_sshmsg_t *msg);
static tt_result_t __svr_verxchg_verxchg(IN tt_sshsvrconn_t *svrconn,
                                         IN void *param);

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_sshsvr_state_verxchg(IN struct tt_sshsvrconn_s *svrconn,
                             IN tt_sshsvr_event_t event,
                             IN void *param,
                             OUT tt_sshsvr_action_t *svract)
{
    tt_result_t result;

    TT_ASSERT(event < TT_SSHSVREV_NUM);

    switch (event) {
        case TT_SSHSVREV_PACKET: {
            tt_sshmsg_t *msg = (tt_sshmsg_t *)param;

            result = __svr_verxchg_packet(svrconn, msg);
            if (TT_OK(result)) {
                svrconn->ms_verxchg_in = TT_TRUE;
                if (svrconn->ms_verxchg_out) {
                    svract->new_state = TT_SSHSVRST_KEYXCHG;
                    svract->new_event = TT_SSHSVREV_KEYINIT;
                }
            } else if (result == TT_E_PROCEED) {
                TT_SSH_MSGID_IGNORED(TT_SSHSVRST_VERXCHG, msg->msg_id);
            } else {
                svract->new_event = TT_SSHSVREV_DISCONNECT;
            }
            return;
        } break;

        case TT_SSHSVREV_VERXCHG: {
            result = __svr_verxchg_verxchg(svrconn, param);
            if (!TT_OK(result)) {
                svract->new_event = TT_SSHSVREV_DISCONNECT;
                return;
            }

            svrconn->ms_verxchg_out = TT_TRUE;
            if (svrconn->ms_verxchg_in) {
                svract->new_state = TT_SSHSVRST_KEYXCHG;
                svract->new_event = TT_SSHSVREV_KEYINIT;
            }
            return;
        } break;

        default: {
            TT_SSH_EV_IGNORED(TT_SSHSVRST_VERXCHG, event);
            return;
        } break;
    }
}

tt_result_t __svr_verxchg_packet(IN tt_sshsvrconn_t *svrconn,
                                 IN tt_sshmsg_t *msg)
{
    switch (msg->msg_id) {
        case TT_SSH_MSGID_VERXCHG: {
            tt_sshms_verxchg_t *vx = TT_SSHMSG_CAST(msg, tt_sshms_verxchg_t);

            if ((vx->protover != TT_SSH_VER_1_99) &&
                (vx->protover != TT_SSH_VER_2_0)) {
                TT_ERROR("only ssh v2.0 is supported");
                return TT_FAIL;
            }

            if (!TT_OK(tt_sshctx_kex_setvc(&svrconn->ctx,
                                           TT_BUF_RPOS(&msg->buf),
                                           TT_BUF_RLEN(&msg->buf) - 2,
                                           TT_TRUE))) {
                return TT_FAIL;
            }

            return TT_SUCCESS;
        } break;

        default: {
            TT_SSH_MSG_FAILURE(TT_SSHSVRST_VERXCHG, msg->msg_id);
            return TT_FAIL;
        } break;
    }
}

tt_result_t __svr_verxchg_verxchg(IN tt_sshsvrconn_t *svrconn, IN void *param)
{
    tt_sshmsg_t *msg;

    msg = tt_sshms_verxchg_create();
    if (msg == NULL) {
        return TT_FAIL;
    }

    tt_sshms_verxchg_set_protover(msg, TT_SSH_VER_2_0);

    if (!TT_OK(tt_sshms_verxchg_set_swver(msg, "TitanSDK"))) {
        return TT_FAIL;
    }

    // need record vs
    if (!TT_OK(tt_sshmsg_render(msg, svrconn->ctx.encrypt.block_len, NULL))) {
        return TT_FAIL;
    }
    if (!TT_OK(tt_sshctx_kex_setvs(&svrconn->ctx,
                                   TT_BUF_RPOS(&msg->buf),
                                   TT_BUF_RLEN(&msg->buf) - 2,
                                   TT_TRUE))) {
        return TT_FAIL;
    }

    return tt_sshsvrconn_send(svrconn, msg);
}
