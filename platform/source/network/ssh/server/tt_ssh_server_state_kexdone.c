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

#include <network/ssh/server/tt_ssh_server_state_kexdone.h>

#include <network/ssh/message/tt_ssh_message.h>
#include <network/ssh/message/tt_ssh_msg_service_accept.h>
#include <network/ssh/message/tt_ssh_msg_service_request.h>
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

static void __svr_kexdone_packet(IN tt_sshsvrconn_t *svrconn,
                                 IN tt_sshmsg_t *msg,
                                 OUT tt_sshsvr_action_t *svract);
tt_result_t __svr_kexdone_pkt_servreq(IN tt_sshsvrconn_t *svrconn,
                                      IN tt_sshmsg_t *msg);

static tt_result_t __svr_kexdone_servacc(IN tt_sshsvrconn_t *svrconn,
                                         IN void *param);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_sshsvr_state_kexdone(IN struct tt_sshsvrconn_s *svrconn,
                             IN tt_sshsvr_event_t event,
                             IN void *param,
                             OUT tt_sshsvr_action_t *svract)
{
    tt_result_t result;

    TT_ASSERT(event < TT_SSHSVREV_NUM);

    switch (event) {
        case TT_SSHSVREV_PACKET: {
            __svr_kexdone_packet(svrconn, (tt_sshmsg_t *)param, svract);
            return;
        } break;

        case TT_SSHSVREV_SERVICE_ACCEPT: {
            result = __svr_kexdone_servacc(svrconn, param);
            if (!TT_OK(result)) {
                svract->new_event = TT_SSHSVREV_DISCONNECT;
                return;
            }

            svract->new_state = TT_SSHSVRST_AUTH;
            return;
        } break;

        default: {
            TT_SSH_EV_IGNORED(TT_SSHSVRST_KEXDH, event);
            return;
        } break;
    }
}

void __svr_kexdone_packet(IN tt_sshsvrconn_t *svrconn,
                          IN tt_sshmsg_t *msg,
                          OUT tt_sshsvr_action_t *svract)
{
    tt_result_t result;

    switch (msg->msg_id) {
        case TT_SSH_MSGID_SERVICE_REQUEST: {
            result = __svr_kexdone_pkt_servreq(svrconn, msg);
            if (TT_OK(result)) {
                svract->new_event = TT_SSHSVREV_SERVICE_ACCEPT;
            } else if (result == TT_E_PROCEED) {
                TT_SSH_MSGID_IGNORED(TT_SSHSVRST_KEX_DONE, msg->msg_id);
            } else {
                svract->new_event = TT_SSHSVREV_DISCONNECT;
            }

            return;
        } break;

        default: {
            TT_SSH_MSG_FAILURE(TT_SSHSVRST_KEX_DONE, msg->msg_id);
            svract->new_event = TT_SSHSVREV_DISCONNECT;

            return;
        } break;
    }
}

tt_result_t __svr_kexdone_pkt_servreq(IN tt_sshsvrconn_t *svrconn,
                                      IN tt_sshmsg_t *msg)
{
    tt_sshmsg_servreq_t *sr = TT_SSHMSG_CAST(msg, tt_sshmsg_servreq_t);

    if (sr->service == TT_SSH_SERVICE_USERAUTH) {
        return TT_SUCCESS;
    } else {
        // todo, check config to determine if need auth
        return TT_FAIL;
    }
}

tt_result_t __svr_kexdone_servacc(IN tt_sshsvrconn_t *svrconn, IN void *param)
{
    tt_sshmsg_t *msg;

    msg = tt_sshmsg_servacc_create();
    if (msg == NULL) {
        return TT_FAIL;
    }

    tt_sshmsg_servacc_set_service(msg, TT_SSH_SERVICE_USERAUTH);

    return tt_sshsvrconn_send(svrconn, msg);
}
