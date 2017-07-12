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

#include <network/ssh/message/tt_ssh_msg_channel_open_confirmation.h>

#include <network/ssh/message/tt_ssh_message.h>
#include <network/ssh/message/tt_ssh_parse.h>
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

static tt_result_t __chopc_render_prepare(IN struct tt_sshmsg_s *msg,
                                          OUT tt_u32_t *len,
                                          OUT tt_ssh_render_mode_t *mode);
static tt_result_t __chopc_render(IN struct tt_sshmsg_s *msg,
                                  IN OUT tt_buf_t *buf);

static tt_result_t __chopc_parse(IN struct tt_sshmsg_s *msg, IN tt_buf_t *data);

static tt_sshmsg_itf_t __chopc_op = {
    NULL,
    NULL,
    NULL,

    __chopc_render_prepare,
    __chopc_render,

    __chopc_parse,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_sshmsg_t *tt_sshmsg_chopc_create()
{
    return tt_sshmsg_create(TT_SSH_MSGID_CHANNEL_OPEN_CONFIRMATION,
                            sizeof(tt_sshmsg_chopc_t),
                            &__chopc_op);
}

void tt_sshmsg_chopc_set_rcvchnum(IN tt_sshmsg_t *msg, IN tt_u32_t rcv_chnum)
{
    tt_sshmsg_chopc_t *chopc;

    TT_ASSERT(msg != NULL);
    TT_ASSERT(msg->msg_id == TT_SSH_MSGID_CHANNEL_OPEN_CONFIRMATION);

    chopc = TT_SSHMSG_CAST(msg, tt_sshmsg_chopc_t);

    chopc->rcv_chnum = rcv_chnum;
}

void tt_sshmsg_chopc_set_sndchnum(IN tt_sshmsg_t *msg, IN tt_u32_t snd_chnum)
{
    tt_sshmsg_chopc_t *chopc;

    TT_ASSERT(msg != NULL);
    TT_ASSERT(msg->msg_id == TT_SSH_MSGID_CHANNEL_OPEN_CONFIRMATION);

    chopc = TT_SSHMSG_CAST(msg, tt_sshmsg_chopc_t);

    chopc->snd_chnum = snd_chnum;
}

void tt_sshmsg_chopc_set_winsize(IN tt_sshmsg_t *msg, IN tt_u32_t window_size)
{
    tt_sshmsg_chopc_t *chopc;

    TT_ASSERT(msg != NULL);
    TT_ASSERT(msg->msg_id == TT_SSH_MSGID_CHANNEL_OPEN_CONFIRMATION);

    chopc = TT_SSHMSG_CAST(msg, tt_sshmsg_chopc_t);

    chopc->window_size = window_size;
}

void tt_sshmsg_chopc_set_pktsize(IN tt_sshmsg_t *msg, IN tt_u32_t packet_size)
{
    tt_sshmsg_chopc_t *chopc;

    TT_ASSERT(msg != NULL);
    TT_ASSERT(msg->msg_id == TT_SSH_MSGID_CHANNEL_OPEN_CONFIRMATION);

    chopc = TT_SSHMSG_CAST(msg, tt_sshmsg_chopc_t);

    chopc->packet_size = packet_size;
}

tt_result_t __chopc_render_prepare(IN struct tt_sshmsg_s *msg,
                                   OUT tt_u32_t *len,
                                   OUT tt_ssh_render_mode_t *mode)
{
    tt_sshmsg_chopc_t *chopc;
    tt_u32_t msg_len = 0;

    /*
    byte SSH_MSG_CHANNEL_OPEN_CONFIRMATION
    uint32 recipient channel
    uint32 sender channel
    uint32 initial window size
    uint32 maximum packet size
    .... channel type specific data follows
    */

    chopc = TT_SSHMSG_CAST(msg, tt_sshmsg_chopc_t);
    (void)chopc;

    // byte
    msg_len += tt_ssh_byte_render_prepare();

    // uint32 recipient channel
    // uint32 sender channel
    // uint32 initial window size
    // uint32 maximum packet size
    msg_len += (tt_ssh_uint32_render_prepare() << 2);

    *len = msg_len;
    *mode = TT_SSH_RENDER_MODE_BINARY;
    return TT_SUCCESS;
}

tt_result_t __chopc_render(IN struct tt_sshmsg_s *msg, IN OUT tt_buf_t *buf)
{
    tt_sshmsg_chopc_t *chopc;

    chopc = TT_SSHMSG_CAST(msg, tt_sshmsg_chopc_t);

    // byte
    TT_DO(tt_ssh_byte_render(buf, TT_SSH_MSGID_CHANNEL_OPEN_CONFIRMATION));

    // uint32 sender channel
    TT_DO(tt_ssh_uint32_render(buf, chopc->rcv_chnum));

    // uint32 sender channel
    TT_DO(tt_ssh_uint32_render(buf, chopc->snd_chnum));

    // uint32 initial window size
    TT_DO(tt_ssh_uint32_render(buf, chopc->window_size));

    // uint32 maximum packet size
    TT_DO(tt_ssh_uint32_render(buf, chopc->packet_size));

    return TT_SUCCESS;
}

tt_result_t __chopc_parse(IN struct tt_sshmsg_s *msg, IN tt_buf_t *data)
{
    tt_sshmsg_chopc_t *chopc;

    chopc = TT_SSHMSG_CAST(msg, tt_sshmsg_chopc_t);

    /*
    byte SSH_MSG_CHANNEL_OPEN_CONFIRMATION
    uint32 recipient channel
    uint32 sender channel
    uint32 initial window size
    uint32 maximum packet size
    .... channel type specific data follows
    */

    // uint32 recipient channel
    TT_DO(tt_ssh_uint32_parse(data, &chopc->rcv_chnum));

    // uint32 sender channel
    TT_DO(tt_ssh_uint32_parse(data, &chopc->snd_chnum));

    // uint32 initial window size
    TT_DO(tt_ssh_uint32_parse(data, &chopc->window_size));

    // uint32 maximum packet size
    TT_DO(tt_ssh_uint32_parse(data, &chopc->packet_size));

    return TT_SUCCESS;
}
