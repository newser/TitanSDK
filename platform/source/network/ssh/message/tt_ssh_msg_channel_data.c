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

#include <network/ssh/message/tt_ssh_msg_channel_data.h>

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

static tt_result_t __chdata_create(IN struct tt_sshmsg_s *msg);
static void __chdata_destroy(IN struct tt_sshmsg_s *msg);

static tt_result_t __chdata_render_prepare(IN struct tt_sshmsg_s *msg,
                                           OUT tt_u32_t *len,
                                           OUT tt_ssh_render_mode_t *mode);
static tt_result_t __chdata_render(IN struct tt_sshmsg_s *msg,
                                   IN OUT tt_buf_t *buf);

static tt_result_t __chdata_parse(IN struct tt_sshmsg_s *msg,
                                  IN tt_buf_t *data);

static tt_sshmsg_itf_t __chdata_op = {
    __chdata_create,
    __chdata_destroy,
    NULL,

    __chdata_render_prepare,
    __chdata_render,

    __chdata_parse,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_sshmsg_t *tt_sshmsg_chdata_create()
{
    return tt_sshmsg_create(TT_SSH_MSGID_CHANNEL_DATA,
                            sizeof(tt_sshmsg_chdata_t),
                            &__chdata_op);
}

void tt_sshmsg_chdata_set_rcvchnum(IN tt_sshmsg_t *msg, IN tt_u32_t rcv_chnum)
{
    tt_sshmsg_chdata_t *chdata;

    TT_ASSERT(msg != NULL);
    TT_ASSERT(msg->msg_id == TT_SSH_MSGID_CHANNEL_DATA);

    chdata = TT_SSHMSG_CAST(msg, tt_sshmsg_chdata_t);

    chdata->rcv_chnum = rcv_chnum;
}

tt_result_t tt_sshmsg_chdata_set_data(IN tt_sshmsg_t *msg,
                                      IN tt_u8_t *data,
                                      IN tt_u32_t data_len,
                                      IN tt_bool_t no_copy)
{
    tt_sshmsg_chdata_t *chdata;
    tt_buf_t *data_buf;

    TT_ASSERT(msg != NULL);
    TT_ASSERT(msg->msg_id == TT_SSH_MSGID_CHANNEL_DATA);

    chdata = TT_SSHMSG_CAST(msg, tt_sshmsg_chdata_t);
    data_buf = &chdata->data;

    tt_buf_destroy(data_buf);
    if (no_copy) {
        if (!TT_OK(tt_buf_create_nocopy(data_buf, data, data_len, NULL))) {
            TT_ERROR("fail to create chdata buf");
            return TT_FAIL;
        }

        return TT_SUCCESS;
    } else {
        if (!TT_OK(tt_buf_create(data_buf, data_len, NULL))) {
            TT_ERROR("fail to create chdata buf");
            return TT_FAIL;
        }
        tt_buf_put(data_buf, data, data_len);

        return TT_SUCCESS;
    }
}

tt_result_t __chdata_create(IN struct tt_sshmsg_s *msg)
{
    tt_sshmsg_chdata_t *chdata;

    chdata = TT_SSHMSG_CAST(msg, tt_sshmsg_chdata_t);

    chdata->rcv_chnum = 0;
    tt_buf_init(&chdata->data, NULL);

    return TT_SUCCESS;
}

void __chdata_destroy(IN struct tt_sshmsg_s *msg)
{
    tt_sshmsg_chdata_t *chdata;

    chdata = TT_SSHMSG_CAST(msg, tt_sshmsg_chdata_t);

    tt_buf_destroy(&chdata->data);
}

tt_result_t __chdata_render_prepare(IN struct tt_sshmsg_s *msg,
                                    OUT tt_u32_t *len,
                                    OUT tt_ssh_render_mode_t *mode)
{
    tt_sshmsg_chdata_t *chdata;
    tt_u32_t msg_len = 0;

    /*
     byte SSH_MSG_CHANNEL_DATA
     uint32 recipient channel
     string data
    */

    chdata = TT_SSHMSG_CAST(msg, tt_sshmsg_chdata_t);

    // byte
    msg_len += tt_ssh_byte_render_prepare();

    // recipient channel
    msg_len += tt_ssh_uint32_render_prepare();

    // string data
    msg_len += tt_ssh_string_render_prepare(NULL, TT_BUF_RLEN(&chdata->data));

    *len = msg_len;
    *mode = TT_SSH_RENDER_MODE_BINARY;
    return TT_SUCCESS;
}

tt_result_t __chdata_render(IN struct tt_sshmsg_s *msg, IN OUT tt_buf_t *buf)
{
    tt_sshmsg_chdata_t *chdata;
    tt_buf_t *data_buf;

    chdata = TT_SSHMSG_CAST(msg, tt_sshmsg_chdata_t);
    data_buf = &chdata->data;

    // byte
    TT_DO(tt_ssh_byte_render(buf, TT_SSH_MSGID_CHANNEL_DATA));

    // uint32 recipient channel
    TT_DO(tt_ssh_uint32_render(buf, chdata->rcv_chnum));

    // string data
    TT_DO(tt_ssh_string_render(buf,
                               TT_BUF_RPOS(data_buf),
                               TT_BUF_RLEN(data_buf)));

    return TT_SUCCESS;
}

tt_result_t __chdata_parse(IN struct tt_sshmsg_s *msg, IN tt_buf_t *data)
{
    tt_sshmsg_chdata_t *chdata;
    tt_u8_t *addr;
    tt_u32_t len;

    chdata = TT_SSHMSG_CAST(msg, tt_sshmsg_chdata_t);

    /*
     byte SSH_MSG_CHANNEL_DATA
     uint32 recipient channel
     string data
     */

    // uint32 sender channel
    TT_DO(tt_ssh_uint32_parse(data, &chdata->rcv_chnum));

    // string data
    TT_DO(tt_ssh_string_parse(data, &addr, &len));
    tt_buf_reset_rwp(&chdata->data);
    TT_DO(tt_buf_put(&chdata->data, addr, len));

    return TT_SUCCESS;
}
