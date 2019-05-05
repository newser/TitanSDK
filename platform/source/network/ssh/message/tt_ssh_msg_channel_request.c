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

#include <network/ssh/message/tt_ssh_msg_channel_request.h>

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

static tt_result_t __chreq_create(IN struct tt_sshmsg_s *msg);
static void __chreq_destroy(IN struct tt_sshmsg_s *msg);

static tt_result_t __chreq_render_prepare(IN struct tt_sshmsg_s *msg,
                                          OUT tt_u32_t *len,
                                          OUT tt_ssh_render_mode_t *mode);
static tt_result_t __chreq_render(IN struct tt_sshmsg_s *msg,
                                  IN OUT tt_buf_t *buf);

static tt_result_t __chreq_parse(IN struct tt_sshmsg_s *msg, IN tt_buf_t *data);

static tt_sshmsg_itf_t __chreq_op = {
    __chreq_create,         __chreq_destroy, NULL,

    __chreq_render_prepare, __chreq_render,

    __chreq_parse,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_sshmsg_t *tt_sshmsg_chreq_create()
{
    return tt_sshmsg_create(TT_SSH_MSGID_CHANNEL_REQUEST,
                            sizeof(tt_sshmsg_chreq_t), &__chreq_op);
}

void tt_sshmsg_chreq_set_rcvchnum(IN tt_sshmsg_t *msg, IN tt_u32_t rcv_chnum)
{
    tt_sshmsg_chreq_t *chreq;

    TT_ASSERT(msg != NULL);
    TT_ASSERT(msg->msg_id == TT_SSH_MSGID_CHANNEL_REQUEST);

    chreq = TT_SSHMSG_CAST(msg, tt_sshmsg_chreq_t);

    chreq->rcv_chnum = rcv_chnum;
}

void tt_sshmsg_chreq_set_reqtype(IN tt_sshmsg_t *msg,
                                 IN tt_ssh_chreqtype_t req_type)
{
    tt_sshmsg_chreq_t *chreq;

    TT_ASSERT(msg != NULL);
    TT_ASSERT(msg->msg_id == TT_SSH_MSGID_CHANNEL_REQUEST);

    chreq = TT_SSHMSG_CAST(msg, tt_sshmsg_chreq_t);

    chreq->req_type = req_type;
}

void tt_sshmsg_chreq_set_wantreply(IN tt_sshmsg_t *msg, IN tt_bool_t want_reply)
{
    tt_sshmsg_chreq_t *chreq;

    TT_ASSERT(msg != NULL);
    TT_ASSERT(msg->msg_id == TT_SSH_MSGID_CHANNEL_REQUEST);

    chreq = TT_SSHMSG_CAST(msg, tt_sshmsg_chreq_t);

    chreq->want_reply = want_reply;
}

tt_result_t __chreq_create(IN struct tt_sshmsg_s *msg)
{
    tt_sshmsg_chreq_t *chreq;

    chreq = TT_SSHMSG_CAST(msg, tt_sshmsg_chreq_t);

    chreq->req_type = TT_SSH_CHREQTYPE_NUM;

    return TT_SUCCESS;
}

void __chreq_destroy(IN struct tt_sshmsg_s *msg)
{
    tt_sshmsg_chreq_t *chreq;

    chreq = TT_SSHMSG_CAST(msg, tt_sshmsg_chreq_t);
    (void)chreq;
}

tt_result_t __chreq_render_prepare(IN struct tt_sshmsg_s *msg,
                                   OUT tt_u32_t *len,
                                   OUT tt_ssh_render_mode_t *mode)
{
    tt_sshmsg_chreq_t *chreq;
    tt_ssh_chreqtype_t rtype;
    tt_u32_t msg_len = 0;

    /*
     byte SSH_MSG_CHANNEL_REQUEST
     uint32 recipient channel
     string request type in US-ASCII characters only
     boolean want reply
     .... type-specific data follows
     */

    chreq = TT_SSHMSG_CAST(msg, tt_sshmsg_chreq_t);
    (void)chreq;

    rtype = chreq->req_type;
    if (!TT_SSH_CHREQTYPE_VALID(rtype)) {
        TT_ERROR("no channel request type set");
        return TT_FAIL;
    }

    // byte SSH_MSG_CHANNEL_REQUEST
    msg_len += tt_ssh_byte_render_prepare();

    // uint32 recipient channel
    msg_len += tt_ssh_uint32_render_prepare();

    // string request type
    msg_len +=
        tt_ssh_string_render_prepare(NULL, (tt_u32_t)tt_strlen(
                                               tt_g_ssh_chreqtype_name[rtype]));

    // boolean want reply
    msg_len += tt_ssh_boolean_render_prepare();

    // request-specific data
    // todo

    *len = msg_len;
    *mode = TT_SSH_RENDER_MODE_BINARY;
    return TT_SUCCESS;
}

tt_result_t __chreq_render(IN struct tt_sshmsg_s *msg, IN OUT tt_buf_t *buf)
{
    tt_sshmsg_chreq_t *chreq;
    tt_ssh_chreqtype_t rtype;

    /*
     byte SSH_MSG_CHANNEL_REQUEST
     uint32 recipient channel
     string request type in US-ASCII characters only
     boolean want reply
     .... type-specific data follows
     */

    chreq = TT_SSHMSG_CAST(msg, tt_sshmsg_chreq_t);

    rtype = chreq->req_type;

    // byte SSH_MSG_CHANNEL_REQUEST
    TT_DO(tt_ssh_byte_render(buf, TT_SSH_MSGID_CHANNEL_REQUEST));

    // uint32 recipient channel
    TT_DO(tt_ssh_uint32_render(buf, chreq->rcv_chnum));

    // string request type
    TT_DO(tt_ssh_string_render(buf, (tt_u8_t *)tt_g_ssh_chreqtype_name[rtype],
                               (tt_u32_t)tt_strlen(
                                   tt_g_ssh_chreqtype_name[rtype])));

    // boolean want reply
    TT_DO(tt_ssh_boolean_render(buf, chreq->want_reply));

    // request-specific data
    // todo

    return TT_SUCCESS;
}

tt_result_t __chreq_parse(IN struct tt_sshmsg_s *msg, IN tt_buf_t *data)
{
    tt_sshmsg_chreq_t *chreq;
    tt_u8_t *addr;
    tt_u32_t len;
    tt_ssh_chreqtype_t req_type;

    /*
     byte SSH_MSG_CHANNEL_REQUEST
     uint32 recipient channel
     string request type in US-ASCII characters only
     boolean want reply
     .... type-specific data follows
     */

    chreq = TT_SSHMSG_CAST(msg, tt_sshmsg_chreq_t);

    // uint32 recipient channel
    TT_DO(tt_ssh_uint32_parse(data, &chreq->rcv_chnum));

    // string request type
    TT_DO(tt_ssh_string_parse(data, &addr, &len));
    req_type = tt_ssh_chreqtype_match(addr, len);
    if (req_type == TT_SSH_CHREQTYPE_NUM) {
        TT_ERROR("not supported ssh type");
        return TT_FAIL;
    }
    chreq->req_type = req_type;

    // boolean want reply
    TT_DO(tt_ssh_boolean_parse(data, &chreq->want_reply));

    // request-specific data
    // todo

    return TT_SUCCESS;
}
