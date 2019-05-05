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

#include <network/ssh/message/tt_ssh_msg_global_request.h>

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

static tt_result_t __glbreq_create(IN struct tt_sshmsg_s *msg);
static void __glbreq_destroy(IN struct tt_sshmsg_s *msg);

static tt_result_t __glbreq_render_prepare(IN struct tt_sshmsg_s *msg,
                                           OUT tt_u32_t *len,
                                           OUT tt_ssh_render_mode_t *mode);
static tt_result_t __glbreq_render(IN struct tt_sshmsg_s *msg,
                                   IN OUT tt_buf_t *buf);

static tt_result_t __glbreq_parse(IN struct tt_sshmsg_s *msg,
                                  IN tt_buf_t *data);

static tt_sshmsg_itf_t __glbreq_op = {
    __glbreq_create,         __glbreq_destroy, NULL,

    __glbreq_render_prepare, __glbreq_render,

    __glbreq_parse,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_sshmsg_t *tt_sshmsg_glbreq_create()
{
    return tt_sshmsg_create(TT_SSH_MSGID_GLOBAL_REQUEST,
                            sizeof(tt_sshmsg_glbreq_t), &__glbreq_op);
}

tt_result_t tt_sshmsg_glbreq_set_reqname(IN tt_sshmsg_t *msg,
                                         IN const tt_char_t *req_name)
{
    tt_sshmsg_glbreq_t *gr;
    tt_u32_t req_name_len;

    TT_ASSERT(msg != NULL);
    TT_ASSERT(msg->msg_id == TT_SSH_MSGID_GLOBAL_REQUEST);
    TT_ASSERT(req_name != NULL);

    gr = TT_SSHMSG_CAST(msg, tt_sshmsg_glbreq_t);

    tt_blob_destroy(&gr->req_name);
    req_name_len = (tt_u32_t)tt_strlen(req_name);
    if (req_name_len > 0) {
        return tt_blob_create(&gr->req_name, (tt_u8_t *)req_name, req_name_len);
    } else {
        tt_blob_init(&gr->req_name);
        return TT_SUCCESS;
    }
}

void tt_sshmsg_glbreq_set_wantreply(IN tt_sshmsg_t *msg,
                                    IN tt_bool_t want_reply)
{
    tt_sshmsg_glbreq_t *gr;

    TT_ASSERT(msg != NULL);
    TT_ASSERT(msg->msg_id == TT_SSH_MSGID_GLOBAL_REQUEST);

    gr = TT_SSHMSG_CAST(msg, tt_sshmsg_glbreq_t);

    gr->want_reply = want_reply;
}

tt_result_t __glbreq_create(IN struct tt_sshmsg_s *msg)
{
    tt_sshmsg_glbreq_t *gr;

    gr = TT_SSHMSG_CAST(msg, tt_sshmsg_glbreq_t);

    tt_blob_init(&gr->req_name);
    gr->want_reply = TT_FALSE;

    return TT_SUCCESS;
}

void __glbreq_destroy(IN struct tt_sshmsg_s *msg)
{
    tt_sshmsg_glbreq_t *gr;

    gr = TT_SSHMSG_CAST(msg, tt_sshmsg_glbreq_t);

    tt_blob_destroy(&gr->req_name);
}

tt_result_t __glbreq_render_prepare(IN struct tt_sshmsg_s *msg,
                                    OUT tt_u32_t *len,
                                    OUT tt_ssh_render_mode_t *mode)
{
    tt_sshmsg_glbreq_t *gr;
    tt_u32_t msg_len = 0;

    /*
     byte SSH_MSG_GLOBAL_REQUEST
     string request name in US-ASCII only
     boolean want reply
     .... request-specific data follows
     */

    gr = TT_SSHMSG_CAST(msg, tt_sshmsg_glbreq_t);

    // byte SSH_MSG_GLOBAL_REQUEST
    msg_len += tt_ssh_byte_render_prepare();

    // string request name
    msg_len += tt_ssh_string_render_prepare(NULL, gr->req_name.len);

    // boolean want reply
    msg_len += tt_ssh_boolean_render_prepare();

    // request-specific data
    // todo

    *len = msg_len;
    *mode = TT_SSH_RENDER_MODE_BINARY;
    return TT_SUCCESS;
}

tt_result_t __glbreq_render(IN struct tt_sshmsg_s *msg, IN OUT tt_buf_t *buf)
{
    tt_sshmsg_glbreq_t *gr;

    /*
     byte SSH_MSG_GLOBAL_REQUEST
     string request name in US-ASCII only
     boolean want reply
     .... request-specific data follows
     */

    gr = TT_SSHMSG_CAST(msg, tt_sshmsg_glbreq_t);

    // byte SSH_MSG_GLOBAL_REQUEST
    TT_DO(tt_ssh_byte_render(buf, TT_SSH_MSGID_GLOBAL_REQUEST));

    // string request name
    TT_DO(tt_ssh_string_render(buf, gr->req_name.addr, gr->req_name.len));

    // boolean want reply
    TT_DO(tt_ssh_boolean_render(buf, gr->want_reply));

    // request-specific data
    // todo

    return TT_SUCCESS;
}

tt_result_t __glbreq_parse(IN struct tt_sshmsg_s *msg, IN tt_buf_t *data)
{
    tt_sshmsg_glbreq_t *gr;
    tt_u8_t *addr;
    tt_u32_t len;

    /*
     byte SSH_MSG_GLOBAL_REQUEST
     string request name in US-ASCII only
     boolean want reply
     .... request-specific data follows
     */

    gr = TT_SSHMSG_CAST(msg, tt_sshmsg_glbreq_t);

    // string request name
    TT_DO(tt_ssh_string_parse(data, &addr, &len));
    if ((len != 0) && !TT_OK(tt_blob_create(&gr->req_name, addr, len))) {
        TT_ERROR("fail to create req name blob");
        return TT_FAIL;
    }

    // boolean want reply
    TT_DO(tt_ssh_boolean_parse(data, &gr->want_reply));

    // request-specific data
    // todo

    return TT_SUCCESS;
}
