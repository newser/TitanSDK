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

#include <network/ssh/message/tt_ssh_msg_channel_open.h>

#include <network/ssh/message/tt_ssh_message.h>
#include <network/ssh/message/tt_ssh_parse.h>
#include <network/ssh/message/tt_ssh_render.h>

#include <tt_cstd_api.h>

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

static tt_result_t __chopen_create(IN struct tt_sshmsg_s *msg);

static tt_result_t __chopen_render_prepare(IN struct tt_sshmsg_s *msg,
                                           OUT tt_u32_t *len,
                                           OUT tt_ssh_render_mode_t *mode);
static tt_result_t __chopen_render(IN struct tt_sshmsg_s *msg,
                                   IN OUT tt_buf_t *buf);

static tt_result_t __chopen_parse(IN struct tt_sshmsg_s *msg,
                                  IN tt_buf_t *data);

static tt_sshmsg_itf_t __chopen_op = {
    __chopen_create,
    NULL,
    NULL,

    __chopen_render_prepare,
    __chopen_render,

    __chopen_parse,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_sshmsg_t *tt_sshmsg_chopen_create()
{
    return tt_sshmsg_create(TT_SSH_MSGID_CHANNEL_OPEN,
                            sizeof(tt_sshmsg_chopen_t),
                            &__chopen_op);
}

void tt_sshmsg_chopen_set_type(IN tt_sshmsg_t *msg, IN tt_ssh_chtype_t type)
{
    tt_sshmsg_chopen_t *chopen;

    TT_ASSERT(msg != NULL);
    TT_ASSERT(msg->msg_id == TT_SSH_MSGID_CHANNEL_OPEN);
    TT_ASSERT(TT_SSH_CHTYPE_VALID(type));

    chopen = TT_SSHMSG_CAST(msg, tt_sshmsg_chopen_t);

    chopen->type = type;
}

void tt_sshmsg_chopen_set_sndchnum(IN tt_sshmsg_t *msg, IN tt_u32_t snd_chnum)
{
    tt_sshmsg_chopen_t *chopen;

    TT_ASSERT(msg != NULL);
    TT_ASSERT(msg->msg_id == TT_SSH_MSGID_CHANNEL_OPEN);

    chopen = TT_SSHMSG_CAST(msg, tt_sshmsg_chopen_t);

    chopen->snd_chnum = snd_chnum;
}

void tt_sshmsg_chopen_set_winsize(IN tt_sshmsg_t *msg, IN tt_u32_t window_size)
{
    tt_sshmsg_chopen_t *chopen;

    TT_ASSERT(msg != NULL);
    TT_ASSERT(msg->msg_id == TT_SSH_MSGID_CHANNEL_OPEN);

    chopen = TT_SSHMSG_CAST(msg, tt_sshmsg_chopen_t);

    chopen->window_size = window_size;
}

void tt_sshmsg_chopen_set_pktsize(IN tt_sshmsg_t *msg, IN tt_u32_t packet_size)
{
    tt_sshmsg_chopen_t *chopen;

    TT_ASSERT(msg != NULL);
    TT_ASSERT(msg->msg_id == TT_SSH_MSGID_CHANNEL_OPEN);

    chopen = TT_SSHMSG_CAST(msg, tt_sshmsg_chopen_t);

    chopen->packet_size = packet_size;
}

tt_result_t __chopen_create(IN struct tt_sshmsg_s *msg)
{
    tt_sshmsg_chopen_t *chopen;

    chopen = TT_SSHMSG_CAST(msg, tt_sshmsg_chopen_t);

    chopen->type = TT_SSH_CHTYPE_NUM;
    chopen->snd_chnum = 0;
    chopen->window_size = 0;
    chopen->packet_size = 0;

    return TT_SUCCESS;
}

tt_result_t __chopen_render_prepare(IN struct tt_sshmsg_s *msg,
                                    OUT tt_u32_t *len,
                                    OUT tt_ssh_render_mode_t *mode)
{
    tt_sshmsg_chopen_t *chopen;
    tt_ssh_chtype_t type;
    tt_u32_t msg_len = 0;

    /*
    byte SSH_MSG_CHANNEL_OPEN
    string channel type in US-ASCII only
    uint32 sender channel
    uint32 initial window size
    uint32 maximum packet size
    .... channel type specific data follows
    */

    chopen = TT_SSHMSG_CAST(msg, tt_sshmsg_chopen_t);

    type = chopen->type;
    if (!TT_SSH_CHTYPE_VALID(type)) {
        TT_ERROR("no channel type set");
        return TT_FAIL;
    }

    // byte
    msg_len += tt_ssh_byte_render_prepare();

    // string channel type
    msg_len += tt_ssh_string_render_prepare(NULL,
                                            (tt_u32_t)tt_strlen(
                                                tt_g_ssh_chtype_name[type]));

    // uint32 sender channel
    msg_len += tt_ssh_uint32_render_prepare();

    // uint32 initial window size
    msg_len += tt_ssh_uint32_render_prepare();

    // uint32 maximum packet size
    msg_len += tt_ssh_uint32_render_prepare();

    // channel type specific data
    if (type == TT_SSH_CHTYPE_SESSION) {
        // todo
    }

    *len = msg_len;
    *mode = TT_SSH_RENDER_MODE_BINARY;
    return TT_SUCCESS;
}

tt_result_t __chopen_render(IN struct tt_sshmsg_s *msg, IN OUT tt_buf_t *buf)
{
    tt_sshmsg_chopen_t *chopen;
    tt_ssh_chtype_t type;

    chopen = TT_SSHMSG_CAST(msg, tt_sshmsg_chopen_t);

    type = chopen->type;

    // byte
    TT_DO(tt_ssh_byte_render(buf, TT_SSH_MSGID_CHANNEL_OPEN));

    // string channel type
    TT_DO(
        tt_ssh_string_render(buf,
                             (tt_u8_t *)tt_g_ssh_chtype_name[type],
                             (tt_u32_t)tt_strlen(tt_g_ssh_chtype_name[type])));

    // uint32 sender channel
    TT_DO(tt_ssh_uint32_render(buf, chopen->snd_chnum));

    // uint32 initial window size
    TT_DO(tt_ssh_uint32_render(buf, chopen->window_size));

    // uint32 maximum packet size
    TT_DO(tt_ssh_uint32_render(buf, chopen->packet_size));

    // channel type specific data
    if (type == TT_SSH_CHTYPE_SESSION) {
        // todo
    }

    return TT_SUCCESS;
}

tt_result_t __chopen_parse(IN struct tt_sshmsg_s *msg, IN tt_buf_t *data)
{
    tt_sshmsg_chopen_t *chopen;
    tt_u8_t *addr;
    tt_u32_t len;
    tt_ssh_chtype_t type;

    chopen = TT_SSHMSG_CAST(msg, tt_sshmsg_chopen_t);

    /*
    byte SSH_MSG_CHANNEL_OPEN
    string channel type in US-ASCII only
    uint32 sender channel
    uint32 initial window size
    uint32 maximum packet size
    .... channel type specific data follows
    */

    // string channel type
    TT_DO(tt_ssh_string_parse(data, &addr, &len));
    type = tt_ssh_chtype_match(addr, len);
    if (type == TT_SSH_CHTYPE_NUM) {
        TT_ERROR("not supported ssh type");
        return TT_FAIL;
    }
    chopen->type = type;

    // uint32 sender channel
    TT_DO(tt_ssh_uint32_parse(data, &chopen->snd_chnum));

    // uint32 initial window size
    TT_DO(tt_ssh_uint32_parse(data, &chopen->window_size));

    // uint32 maximum packet size
    TT_DO(tt_ssh_uint32_parse(data, &chopen->packet_size));

    // channel type specific data
    if (type == TT_SSH_CHTYPE_SESSION) {
        // todo
    }

    return TT_SUCCESS;
}
