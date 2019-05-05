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

#include <network/ssh/server/tt_ssh_server_state_authdone.h>

#include <network/ssh/message/tt_ssh_message.h>
#include <network/ssh/message/tt_ssh_msg_channel_open.h>
#include <network/ssh/message/tt_ssh_msg_channel_open_confirmation.h>
#include <network/ssh/message/tt_ssh_msg_channel_open_failure.h>
#include <network/ssh/server/tt_ssh_server_conn.h>
#include <network/ssh/tt_ssh_channel.h>
#include <network/ssh/tt_ssh_channel_cb.h>

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

static void __svr_authdone_packet(IN tt_sshsvrconn_t *svrconn,
                                  IN tt_sshmsg_t *msg,
                                  OUT tt_sshsvr_action_t *svract);

static void __svr_authdone_pkt_chopen(IN tt_sshsvrconn_t *svrconn,
                                      IN tt_sshmsg_t *msg,
                                      OUT tt_sshsvr_action_t *svract);

static tt_result_t __send_chopc(IN tt_sshsvrconn_t *svrconn,
                                IN tt_u32_t rcv_chnum, IN tt_u32_t snd_chnum,
                                IN tt_u32_t win_size, IN tt_u32_t pkt_size);
static tt_result_t __send_chopf(IN tt_sshsvrconn_t *svrconn,
                                IN tt_u32_t rcv_chnum, IN tt_u32_t reason_code);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_sshsvr_state_authdone(IN struct tt_sshsvrconn_s *svrconn,
                              IN tt_sshsvr_event_t event, IN void *param,
                              OUT tt_sshsvr_action_t *svract)
{
    TT_ASSERT(event < TT_SSHSVREV_NUM);

    switch (event) {
    case TT_SSHSVREV_PACKET: {
        __svr_authdone_packet(svrconn, (tt_sshmsg_t *)param, svract);
        return;
    } break;

    default: {
        TT_SSH_EV_IGNORED(TT_SSHSVRST_AUTH_DONE, event);
        return;
    } break;
    }
}

void __svr_authdone_packet(IN tt_sshsvrconn_t *svrconn, IN tt_sshmsg_t *msg,
                           OUT tt_sshsvr_action_t *svract)
{
    switch (msg->msg_id) {
    case TT_SSH_MSGID_USERAUTH_REQUEST: {
        return;
    } break;
    case TT_SSH_MSGID_CHANNEL_OPEN: {
        __svr_authdone_pkt_chopen(svrconn, msg, svract);
        return;
    } break;
    case TT_SSH_MSGID_CHANNEL_WINDOW_ADJUST:
    case TT_SSH_MSGID_CHANNEL_EOF:
    case TT_SSH_MSGID_CHANNEL_CLOSE:
    case TT_SSH_MSGID_CHANNEL_DATA:
    case TT_SSH_MSGID_CHANNEL_REQUEST: {
        tt_sshsvrconn_ch_pkt_handler(svrconn, msg, svract);
        return;
    } break;

    default: {
        TT_SSH_MSG_FAILURE(TT_SSHSVRST_AUTH_DONE, msg->msg_id);
        svract->new_event = TT_SSHSVREV_DISCONNECT;

        return;
    } break;
    }
}

void __svr_authdone_pkt_chopen(IN tt_sshsvrconn_t *svrconn, IN tt_sshmsg_t *msg,
                               OUT tt_sshsvr_action_t *svract)
{
    tt_sshmsg_chopen_t *chopen = TT_SSHMSG_CAST(msg, tt_sshmsg_chopen_t);
    tt_sshch_t *ch;
    tt_u32_t size;

    // todo: ask if type is supported
    ch = tt_sshsvrconn_ch_create(svrconn, chopen->type);
    if (ch == NULL) {
        __send_chopf(svrconn, chopen->snd_chnum,
                     TT_SSH_CHOPF_RC_RESOURCE_SHORTAGE);
        return;
    }

    tt_sshch_set_peerchnum(ch, chopen->snd_chnum);
    tt_sshch_set_winsize(ch, chopen->window_size);
    tt_sshch_set_pktsize(ch, chopen->packet_size);

    // to tell remote peer of local win and pkt size, here it gives an
    // estimated value which is half of the size of connection's recv
    // buf. the flow control mechnism may be improved in future...
    size = TT_BUF_WLEN(&svrconn->recv_buf) >> 1;

    if (!TT_OK(__send_chopc(svrconn, ch->peer_chnum, ch->chnum, size, size))) {
        TT_FATAL("fail to send channel open confirmation");
        return;
    }

    if (ch->cb->on_connect != NULL) { ch->cb->on_connect(ch, chopen); }
}

tt_result_t __send_chopc(IN tt_sshsvrconn_t *svrconn, IN tt_u32_t rcv_chnum,
                         IN tt_u32_t snd_chnum, IN tt_u32_t win_size,
                         IN tt_u32_t pkt_size)
{
    tt_sshmsg_t *chopc;

    chopc = tt_sshmsg_chopc_create();
    if (chopc == NULL) {
        TT_FATAL("fail to create chopc");
        return TT_FAIL;
    }

    tt_sshmsg_chopc_set_rcvchnum(chopc, rcv_chnum);
    tt_sshmsg_chopc_set_sndchnum(chopc, snd_chnum);
    tt_sshmsg_chopc_set_winsize(chopc, win_size);
    tt_sshmsg_chopc_set_pktsize(chopc, pkt_size);

    return tt_sshsvrconn_send(svrconn, chopc);
}

tt_result_t __send_chopf(IN tt_sshsvrconn_t *svrconn, IN tt_u32_t rcv_chnum,
                         IN tt_u32_t reason_code)
{
    tt_sshmsg_t *chopf;

    chopf = tt_sshmsg_chopf_create();
    if (chopf == NULL) {
        TT_FATAL("fail to create chopf");
        return TT_FAIL;
    }

    tt_sshmsg_chopf_set_rcvchnum(chopf, rcv_chnum);
    tt_sshmsg_chopf_set_reason(chopf, reason_code);

    return tt_sshsvrconn_send(svrconn, chopf);
}
