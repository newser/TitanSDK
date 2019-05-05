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

#include <network/ssh/message/tt_ssh_msg_channel_open_failure.h>

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

static tt_result_t __chopf_create(IN struct tt_sshmsg_s *msg);
static void __chopf_destroy(IN struct tt_sshmsg_s *msg);

static tt_result_t __chopf_render_prepare(IN struct tt_sshmsg_s *msg,
                                          OUT tt_u32_t *len,
                                          OUT tt_ssh_render_mode_t *mode);
static tt_result_t __chopf_render(IN struct tt_sshmsg_s *msg,
                                  IN OUT tt_buf_t *buf);

static tt_result_t __chopf_parse(IN struct tt_sshmsg_s *msg, IN tt_buf_t *data);

static tt_sshmsg_itf_t __chopf_op = {
    __chopf_create,         __chopf_destroy, NULL,

    __chopf_render_prepare, __chopf_render,

    __chopf_parse,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_sshmsg_t *tt_sshmsg_chopf_create()
{
    return tt_sshmsg_create(TT_SSH_MSGID_CHANNEL_OPEN_FAILURE,
                            sizeof(tt_sshmsg_chopf_t), &__chopf_op);
}

void tt_sshmsg_chopf_set_rcvchnum(IN tt_sshmsg_t *msg, IN tt_u32_t rcv_chnum)
{
    tt_sshmsg_chopf_t *chopf;

    TT_ASSERT(msg != NULL);
    TT_ASSERT(msg->msg_id == TT_SSH_MSGID_CHANNEL_OPEN_FAILURE);

    chopf = TT_SSHMSG_CAST(msg, tt_sshmsg_chopf_t);

    chopf->rcv_chnum = rcv_chnum;
}

void tt_sshmsg_chopf_set_reason(IN tt_sshmsg_t *msg, IN tt_u32_t reason_code)
{
    tt_sshmsg_chopf_t *chopf;

    TT_ASSERT(msg != NULL);
    TT_ASSERT(msg->msg_id == TT_SSH_MSGID_CHANNEL_OPEN_FAILURE);

    chopf = TT_SSHMSG_CAST(msg, tt_sshmsg_chopf_t);

    chopf->reason_code = reason_code;
}

tt_result_t tt_sshmsg_chopf_set_desc(IN tt_sshmsg_t *msg,
                                     IN const tt_char_t *desc)
{
    tt_sshmsg_chopf_t *chopf;
    tt_u32_t desc_len;

    TT_ASSERT(msg != NULL);
    TT_ASSERT(msg->msg_id == TT_SSH_MSGID_CHANNEL_OPEN_FAILURE);
    TT_ASSERT(desc != NULL);

    chopf = TT_SSHMSG_CAST(msg, tt_sshmsg_chopf_t);

    tt_blob_destroy(&chopf->desc);
    desc_len = (tt_u32_t)tt_strlen(desc);
    return tt_blob_create(&chopf->desc, (tt_u8_t *)desc, desc_len);
}

tt_result_t __chopf_create(IN struct tt_sshmsg_s *msg)
{
    tt_sshmsg_chopf_t *chopf;

    chopf = TT_SSHMSG_CAST(msg, tt_sshmsg_chopf_t);

    chopf->rcv_chnum = 0;
    chopf->reason_code = 0;
    tt_blob_init(&chopf->desc);

    return TT_SUCCESS;
}

void __chopf_destroy(IN struct tt_sshmsg_s *msg)
{
    tt_sshmsg_chopf_t *chopf;

    chopf = TT_SSHMSG_CAST(msg, tt_sshmsg_chopf_t);

    tt_blob_destroy(&chopf->desc);
}

tt_result_t __chopf_render_prepare(IN struct tt_sshmsg_s *msg,
                                   OUT tt_u32_t *len,
                                   OUT tt_ssh_render_mode_t *mode)
{
    tt_sshmsg_chopf_t *chopf;
    tt_u32_t msg_len = 0;

    /*
    byte SSH_MSG_CHANNEL_OPEN_FAILURE
    uint32 recipient channel
    uint32 reason code
    string description in ISO-10646 UTF-8 encoding [RFC3629]
    string language tag [RFC3066]
    */

    chopf = TT_SSHMSG_CAST(msg, tt_sshmsg_chopf_t);

    // byte
    msg_len += tt_ssh_byte_render_prepare();

    // uint32 recipient channel
    // uint32 reason code
    msg_len += (tt_ssh_uint32_render_prepare() << 1);

    // string description
    msg_len += tt_ssh_string_render_prepare(NULL, chopf->desc.len);

    // string language tag
    msg_len += tt_ssh_string_render_prepare(NULL, 0);

    *len = msg_len;
    *mode = TT_SSH_RENDER_MODE_BINARY;
    return TT_SUCCESS;
}

tt_result_t __chopf_render(IN struct tt_sshmsg_s *msg, IN OUT tt_buf_t *buf)
{
    tt_sshmsg_chopf_t *chopf;

    chopf = TT_SSHMSG_CAST(msg, tt_sshmsg_chopf_t);

    // byte
    TT_DO(tt_ssh_byte_render(buf, TT_SSH_MSGID_CHANNEL_OPEN_FAILURE));

    // uint32 recipient channel
    TT_DO(tt_ssh_uint32_render(buf, chopf->rcv_chnum));

    // uint32 reason code
    TT_DO(tt_ssh_uint32_render(buf, chopf->reason_code));

    // string description
    TT_DO(tt_ssh_string_render(buf, chopf->desc.addr, chopf->desc.len));

    // string language tag
    TT_DO(tt_ssh_string_render(buf, NULL, 0));

    return TT_SUCCESS;
}

tt_result_t __chopf_parse(IN struct tt_sshmsg_s *msg, IN tt_buf_t *data)
{
    tt_sshmsg_chopf_t *chopf;
    tt_u8_t *addr;
    tt_u32_t len;

    chopf = TT_SSHMSG_CAST(msg, tt_sshmsg_chopf_t);

    /*
    byte SSH_MSG_CHANNEL_OPEN_FAILURE
    uint32 recipient channel
    uint32 reason code
    string description in ISO-10646 UTF-8 encoding [RFC3629]
    string language tag [RFC3066]
    */

    // uint32 recipient channel
    TT_DO(tt_ssh_uint32_parse(data, &chopf->rcv_chnum));

    // uint32 reason code
    TT_DO(tt_ssh_uint32_parse(data, &chopf->reason_code));

    // string description
    TT_DO(tt_ssh_string_parse(data, &addr, &len));
    if ((len != 0) && !TT_OK(tt_blob_create(&chopf->desc, addr, len))) {
        TT_ERROR("fail to create desc blob");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}
