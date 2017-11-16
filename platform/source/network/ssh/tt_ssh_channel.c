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

#include <network/ssh/tt_ssh_channel.h>

#include <memory/tt_memory_alloc.h>
#include <misc/tt_reference_counter.h>
#include <network/ssh/message/tt_ssh_message.h>
#include <network/ssh/message/tt_ssh_msg_channel_close.h>
#include <network/ssh/message/tt_ssh_msg_channel_data.h>
#include <network/ssh/message/tt_ssh_msg_channel_eof.h>
#include <network/ssh/message/tt_ssh_msg_channel_failure.h>
#include <network/ssh/message/tt_ssh_msg_channel_request.h>
#include <network/ssh/message/tt_ssh_msg_channel_success.h>
#include <network/ssh/message/tt_ssh_msg_channel_window_adjust.h>
#include <network/ssh/tt_ssh_channel_cb.h>
#include <network/ssh/tt_ssh_channel_manager.h>

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

static tt_u32_t __can_send(IN tt_sshch_t *ch, IN tt_u32_t data_len);
static tt_u32_t __can_send_len(IN tt_u32_t head_mac_size,
                               IN tt_u32_t block_size,
                               IN tt_u32_t win_size,
                               IN tt_u32_t data_len);
static void __check_can_send(IN tt_sshch_t *ch);

static void __sshch_pkt_winadj(IN tt_sshch_t *ch,
                               IN tt_sshmsg_chwinadj_t *chwa,
                               OUT struct tt_sshsvr_action_s *svract);
static void __sshch_pkt_chdata(IN tt_sshch_t *ch,
                               IN tt_sshmsg_chdata_t *chdata,
                               OUT struct tt_sshsvr_action_s *svract);
static void __sshch_pkt_chreq(IN tt_sshch_t *ch,
                              IN tt_sshmsg_chreq_t *chreq,
                              OUT struct tt_sshsvr_action_s *svract);

static tt_result_t __send_chdata(IN tt_sshch_t *ch,
                                 IN tt_u8_t *data,
                                 IN tt_u32_t data_len,
                                 IN tt_bool_t no_copy);
static tt_result_t __send_chsucc(IN tt_sshch_t *ch, IN tt_u32_t rcv_chnum);
static tt_result_t __send_chfail(IN tt_sshch_t *ch, IN tt_u32_t rcv_chnum);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_sshch_t *tt_sshch_create(IN tt_ssh_chtype_t type,
                            IN tt_u32_t chnum,
                            IN struct tt_sshch_cb_s *cb,
                            IN struct tt_sshchmgr_s *chmgr)
{
    tt_sshch_t *ch;

    TT_ASSERT(TT_SSH_CHTYPE_VALID(type));
    TT_ASSERT(cb != NULL);
    TT_ASSERT(chmgr != NULL);

    ch = (tt_sshch_t *)tt_malloc(sizeof(tt_sshch_t));
    if (ch == NULL) {
        TT_ERROR("no mem for ssh svr ch");
        return NULL;
    }
    tt_memset(ch, 0, sizeof(tt_sshch_t));

    ch->type = type;
    ch->chnum = chnum;
    ch->peer_chnum = 0;
    ch->win_size = ~0; // unlimited
    ch->pkt_size = ~0; // unlimited
    ch->cb = cb;
    ch->opaque = NULL;
    tt_buf_init(&ch->send_buf, NULL);

    ch->chmgr = chmgr;
    if (!TT_OK(tt_sshchmgr_extra_size(chmgr,
                                      &ch->head_mac_size,
                                      &ch->block_size))) {
        tt_buf_destroy(&ch->send_buf);
        tt_free(ch);
        return NULL;
    }
    TT_ASSERT(ch->head_mac_size != 0);
    TT_ASSERT(ch->block_size != 0);

    return ch;
}

void tt_sshch_destroy(IN tt_sshch_t *ch)
{
    tt_sshchmgr_on_ch_destroy(ch->chmgr, ch);

    tt_buf_destroy(&ch->send_buf);
    tt_free(ch);
}

void tt_sshch_shutdown(IN tt_sshch_t *ch)
{
    tt_sshmsg_t *msg;

    // send eof
    if (!ch->eof_out) {
        msg = tt_sshmsg_cheof_create();
        if (msg != NULL) {
            tt_sshmsg_cheof_set_rcvchnum(msg, ch->peer_chnum);
            tt_sshchmgr_send(ch->chmgr, msg);
        }
        ch->eof_out = TT_TRUE;
    }

    // send close
    if (!ch->close_out) {
        msg = tt_sshmsg_chclose_create();
        if (msg != NULL) {
            tt_sshmsg_chclose_set_rcvchnum(msg, ch->peer_chnum);
            tt_sshchmgr_send(ch->chmgr, msg);
        }
        ch->close_out = TT_TRUE;
    }

    // destroy
    if (ch->close_in && ch->close_out) {
        tt_sshch_destroy(ch);
    }
}

tt_result_t tt_sshch_send(IN tt_sshch_t *ch,
                          IN tt_u8_t *data,
                          IN tt_u32_t data_len,
                          IN tt_u32_t flag)
{
    tt_u32_t n;

    TT_ASSERT(ch != NULL);

    if (data_len == 0) {
        return TT_SUCCESS;
    }
    TT_ASSERT(data != NULL);

    n = __can_send(ch, data_len);
    TT_ASSERT(n <= data_len);
    if (n == 0) {
        return tt_buf_put(&ch->send_buf, data, data_len);
    }

    if (!TT_OK(__send_chdata(ch, data, data_len, TT_FALSE))) {
        return TT_FAIL;
    }

    if ((n < data_len) &&
        !TT_OK(tt_buf_put(&ch->send_buf, data + n, data_len - n))) {
        TT_FATAL("ssh data may be lost");
    }

    return TT_SUCCESS;
}

void tt_sshch_pkt_handler(IN tt_sshch_t *ch,
                          IN tt_sshmsg_t *msg,
                          OUT struct tt_sshsvr_action_s *svract)
{
    switch (msg->msg_id) {
        case TT_SSH_MSGID_CHANNEL_REQUEST: {
            __sshch_pkt_chreq(ch,
                              TT_SSHMSG_CAST(msg, tt_sshmsg_chreq_t),
                              svract);
            return;
        } break;
        case TT_SSH_MSGID_CHANNEL_DATA: {
            __sshch_pkt_chdata(ch,
                               TT_SSHMSG_CAST(msg, tt_sshmsg_chdata_t),
                               svract);
            return;
        } break;
        case TT_SSH_MSGID_CHANNEL_WINDOW_ADJUST: {
            __sshch_pkt_winadj(ch,
                               TT_SSHMSG_CAST(msg, tt_sshmsg_chwinadj_t),
                               svract);
            return;
        } break;
        case TT_SSH_MSGID_CHANNEL_EOF: {
            ch->eof_in = TT_TRUE;
            return;
        } break;
        case TT_SSH_MSGID_CHANNEL_CLOSE: {
            ch->close_in = TT_TRUE;
            if (ch->close_in && ch->close_out) {
                tt_sshch_destroy(ch);
            }
            return;
        } break;

        default: {
            TT_ERROR("not a channel packet: %d", msg->msg_id);
            return;
        } break;
    }
}

tt_u32_t __can_send(IN tt_sshch_t *ch, IN tt_u32_t data_len)
{
    if (TT_BUF_RLEN(&ch->send_buf) != 0) {
        return 0;
    }

    return __can_send_len(ch->head_mac_size,
                          ch->block_size,
                          TT_MIN(ch->pkt_size, ch->win_size),
                          data_len);
}

tt_u32_t __can_send_len(IN tt_u32_t head_mac_size,
                        IN tt_u32_t block_size,
                        IN tt_u32_t win_size,
                        IN tt_u32_t data_len)
{
    tt_u32_t n;

    // 9 is extra size of SSH_MSG_CHANNEL_DATA
    // by setting msg->pad_block to 1, there are at most two pad blocks
    n = head_mac_size + 9 + (block_size << 1);
    if (n >= win_size) {
        // window size is even less than overhead of a single packet
        return 0;
    }
    n = win_size - n;

    return TT_COND(n < data_len, n, data_len);
}

void __check_can_send(IN tt_sshch_t *ch)
{
    tt_buf_t *send_buf = &ch->send_buf;
    tt_u32_t len;

    // do nothing if no data is buffered
    len = TT_BUF_RLEN(send_buf);
    if (len == 0) {
        return;
    }

    // how many bytes can send
    len = __can_send_len(ch->head_mac_size,
                         ch->block_size,
                         TT_MIN(ch->pkt_size, ch->win_size),
                         len);
    if (len == 0) {
        return;
    }

    // send buffered data
    if (!TT_OK(__send_chdata(ch, TT_BUF_RPOS(send_buf), len, TT_FALSE))) {
        return;
    }

    // as data are copied to chdata msg, no need to keep them
    tt_buf_inc_rp(send_buf, len);
    if ((TT_BUF_RLEN(send_buf) == 0) || (TT_BUF_REFINABLE(send_buf) > 1000)) {
        tt_buf_refine(send_buf);
    }
}

void __sshch_pkt_winadj(IN tt_sshch_t *ch,
                        IN tt_sshmsg_chwinadj_t *chwa,
                        OUT struct tt_sshsvr_action_s *svract)
{
    // prevent from overflow
    if ((ch->win_size + chwa->capacity) > ch->win_size) {
        ch->win_size += chwa->capacity;
    }
}

void __sshch_pkt_chdata(IN tt_sshch_t *ch,
                        IN tt_sshmsg_chdata_t *chdata,
                        OUT struct tt_sshsvr_action_s *svract)
{
    tt_buf_t *data = &chdata->data;
    tt_u32_t data_len = TT_BUF_RLEN(data);

    if (data_len > 0) {
        ch->cb->on_recv(ch, TT_BUF_RPOS(data), data_len);
    }
}

void __sshch_pkt_chreq(IN tt_sshch_t *ch,
                       IN tt_sshmsg_chreq_t *chreq,
                       OUT struct tt_sshsvr_action_s *svract)
{
    tt_result_t result;

    // todo: handle channel request
    result = TT_SUCCESS;

    if (chreq->want_reply) {
        if (TT_OK(result)) {
            __send_chsucc(ch, chreq->rcv_chnum);
        }
    }
}

tt_result_t __send_chdata(IN tt_sshch_t *ch,
                          IN tt_u8_t *data,
                          IN tt_u32_t data_len,
                          IN tt_bool_t no_copy)
{
    tt_sshmsg_t *chdata;

    chdata = tt_sshmsg_chdata_create();
    if (chdata == NULL) {
        return TT_FAIL;
    }

    // rcv channel
    tt_sshmsg_chdata_set_rcvchnum(chdata, ch->peer_chnum);

    // data
    if (!TT_OK(tt_sshmsg_chdata_set_data(chdata, data, data_len, no_copy))) {
        tt_sshmsg_release(chdata);
        return TT_FAIL;
    }

    return tt_sshchmgr_send(ch->chmgr, chdata);
}

tt_result_t __send_chsucc(IN tt_sshch_t *ch, IN tt_u32_t rcv_chnum)
{
    tt_sshmsg_t *chsucc;

    chsucc = tt_sshmsg_chsucc_create();
    if (chsucc == NULL) {
        TT_FATAL("fail to create chsucc");
        return TT_FAIL;
    }

    tt_sshmsg_chsucc_set_rcvchnum(chsucc, rcv_chnum);

    return tt_sshchmgr_send(ch->chmgr, chsucc);
}

tt_result_t __send_chfail(IN tt_sshch_t *ch, IN tt_u32_t rcv_chnum)
{
    tt_sshmsg_t *chfail;

    chfail = tt_sshmsg_chfail_create();
    if (chfail == NULL) {
        TT_FATAL("fail to create chfail");
        return TT_FAIL;
    }

    tt_sshmsg_chfail_set_rcvchnum(chfail, rcv_chnum);

    return tt_sshchmgr_send(ch->chmgr, chfail);
}
