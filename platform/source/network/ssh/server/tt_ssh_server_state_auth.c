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

#include <network/ssh/server/tt_ssh_server_state_auth.h>

#include <misc/tt_reference_counter.h>
#include <network/ssh/message/tt_ssh_message.h>
#include <network/ssh/message/tt_ssh_msg_userauth_banner.h>
#include <network/ssh/message/tt_ssh_msg_userauth_failure.h>
#include <network/ssh/message/tt_ssh_msg_userauth_request.h>
#include <network/ssh/message/tt_ssh_msg_userauth_success.h>
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

static const tt_char_t *__svr_banner =
    "\n"
    " _____  _  _                 ____   ____   _  __\n"
    "|_   _|(_)| |_  __ _  _ __  / ___| |  _ \\ | |/ /\n"
    "  | |  | || __|/ _` || '_ \\ \\___ \\ | | | || ' / \n"
    "  | |  | || |_| (_| || | | | ___) || |_| || . \\ \n"
    "  |_|  |_| \\__|\\__,_||_| |_||____/ |____/ |_|\\_\\\n"
    "\n";

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static void __svr_auth_packet(IN tt_sshsvrconn_t *svrconn,
                              IN tt_sshmsg_t *msg,
                              OUT tt_sshsvr_action_t *svract);
static void __svr_auth_pkt_uar(IN tt_sshsvrconn_t *svrconn,
                               IN tt_sshmsg_t *msg,
                               OUT tt_sshsvr_action_t *svract);

static tt_result_t __svr_auth_conn(IN tt_sshsvrconn_t *svrconn,
                                   IN tt_sshmsg_uar_t *uar);
static tt_result_t __svr_auth_conn_pubkey(IN tt_sshsvrconn_t *svrconn,
                                          IN tt_sshmsg_uar_t *uar);
static tt_result_t __svr_auth_conn_pwd(IN tt_sshsvrconn_t *svrconn,
                                       IN tt_sshmsg_uar_t *uar);

static tt_result_t __svr_auth_success(IN tt_sshsvrconn_t *svrconn);
static tt_result_t __svr_auth_fail(IN tt_sshsvrconn_t *svrconn,
                                   IN tt_ssh_auth_t *auth,
                                   IN tt_u32_t auth_num,
                                   IN tt_bool_t partial_succ);
static tt_result_t __svr_auth_banner(IN tt_sshsvrconn_t *svrconn,
                                     IN const tt_char_t *banner);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_sshsvr_state_auth(IN struct tt_sshsvrconn_s *svrconn,
                          IN tt_sshsvr_event_t event,
                          IN void *param,
                          OUT tt_sshsvr_action_t *svract)
{
    TT_ASSERT(event < TT_SSHSVREV_NUM);

    switch (event) {
        case TT_SSHSVREV_PACKET: {
            __svr_auth_packet(svrconn, (tt_sshmsg_t *)param, svract);
            return;
        } break;

        default: {
            TT_SSH_EV_IGNORED(TT_SSHSVRST_AUTH, event);
            return;
        } break;
    }
}

void __svr_auth_packet(IN tt_sshsvrconn_t *svrconn,
                       IN tt_sshmsg_t *msg,
                       OUT tt_sshsvr_action_t *svract)
{
    switch (msg->msg_id) {
        case TT_SSH_MSGID_USERAUTH_REQUEST: {
            __svr_auth_pkt_uar(svrconn, msg, svract);
            return;
        } break;

        default: {
            TT_SSH_MSG_FAILURE(TT_SSHSVRST_KEX_DONE, msg->msg_id);
            svract->new_event = TT_SSHSVREV_DISCONNECT;

            return;
        } break;
    }
}

void __svr_auth_pkt_uar(IN tt_sshsvrconn_t *svrconn,
                        IN tt_sshmsg_t *msg,
                        OUT tt_sshsvr_action_t *svract)
{
    tt_sshmsg_uar_t *uar = TT_SSHMSG_CAST(msg, tt_sshmsg_uar_t);
    tt_result_t result;

    switch (uar->service) {
        case TT_SSH_SERVICE_CONNECTION: {
            result = __svr_auth_conn(svrconn, uar);
        } break;

        default: {
            TT_ERROR("can not auth ssh service");
            result = TT_FAIL;
        } break;
    }

    if (TT_OK(result)) {
        svract->new_state = TT_SSHSVRST_AUTH_DONE;
    } else if (result != TT_PROCEEDING) {
        svract->new_event = TT_SSHSVREV_DISCONNECT;
    } else {
        // TT_PROCEEDING means requiring more authentication
    }
}

tt_result_t __svr_auth_conn(IN tt_sshsvrconn_t *svrconn,
                            IN tt_sshmsg_uar_t *uar)
{
    switch (uar->auth) {
        case TT_SSH_AUTH_PUBLICKEY: {
            return __svr_auth_conn_pubkey(svrconn, uar);
        } break;
        case TT_SSH_AUTH_PASSWORD: {
            return __svr_auth_conn_pwd(svrconn, uar);
        } break;

        default: {
            tt_ssh_auth_t auth[] = {
                // TT_SSH_AUTH_PUBLICKEY,
                TT_SSH_AUTH_PASSWORD,
            };
            tt_u32_t auth_num = sizeof(auth) / sizeof(auth[0]);

            if (TT_OK(__svr_auth_fail(svrconn, auth, auth_num, TT_TRUE))) {
                return TT_PROCEEDING;
            } else {
                return TT_FAIL;
            }
        } break;
    }
}

tt_result_t __svr_auth_conn_pubkey(IN tt_sshsvrconn_t *svrconn,
                                   IN tt_sshmsg_uar_t *uar)
{
    // todo...
    return TT_FAIL;
}

tt_result_t __svr_auth_conn_pwd(IN tt_sshsvrconn_t *svrconn,
                                IN tt_sshmsg_uar_t *uar)
{
    tt_ssh_auth_pwd_t *pwd = &uar->auth_u.pwd;

    // todo: auth password here
    tt_hex_dump(pwd->pwd.addr, pwd->pwd.len, 8);

    // send banner
    if (!TT_OK(__svr_auth_banner(svrconn, __svr_banner))) {
        return TT_FAIL;
    }

    // send success
    if (!TT_OK(__svr_auth_success(svrconn))) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t __svr_auth_success(IN tt_sshsvrconn_t *svrconn)
{
    tt_sshmsg_t *msg = tt_sshmsg_uas_create();

    if (msg == NULL) {
        return TT_FAIL;
    }

    if (!TT_OK(tt_sshsvrconn_send(svrconn, msg))) {
        tt_sshmsg_release(msg);
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t __svr_auth_fail(IN tt_sshsvrconn_t *svrconn,
                            IN tt_ssh_auth_t *auth,
                            IN tt_u32_t auth_num,
                            IN tt_bool_t partial_succ)
{
    tt_sshmsg_t *msg = tt_sshmsg_uaf_create();
    tt_u32_t i;

    if (msg == NULL) {
        return TT_FAIL;
    }

    for (i = 0; i < auth_num; ++i) {
        tt_sshmsg_uaf_add_auth(msg, auth[i]);
    }

    tt_sshmsg_uaf_set_parial_succ(msg, partial_succ);

    if (!TT_OK(tt_sshsvrconn_send(svrconn, msg))) {
        tt_sshmsg_release(msg);
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t __svr_auth_banner(IN tt_sshsvrconn_t *svrconn,
                              IN const tt_char_t *banner)
{
    tt_sshmsg_t *msg = tt_sshmsg_uab_create();

    if (msg == NULL) {
        return TT_FAIL;
    }

    if (!TT_OK(tt_sshmsg_uab_set_banner(msg, banner))) {
        tt_sshmsg_release(msg);
        return TT_FAIL;
    }

    if (!TT_OK(tt_sshsvrconn_send(svrconn, msg))) {
        tt_sshmsg_release(msg);
        return TT_FAIL;
    }

    return TT_SUCCESS;
}
