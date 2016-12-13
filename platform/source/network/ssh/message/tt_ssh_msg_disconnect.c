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

#include <network/ssh/message/tt_ssh_msg_disconnect.h>

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

static tt_result_t __disconnect_render_prepare(IN struct tt_sshmsg_s *msg,
                                               OUT tt_u32_t *len,
                                               OUT tt_ssh_render_mode_t *mode);
static tt_result_t __disconnect_render(IN struct tt_sshmsg_s *msg,
                                       IN OUT tt_buf_t *buf);

static tt_result_t __disconnect_parse(IN struct tt_sshmsg_s *msg,
                                      IN tt_buf_t *data);

static tt_sshmsg_itf_t __disconnect_op = {
    NULL,
    NULL,
    NULL,

    __disconnect_render_prepare,
    __disconnect_render,

    __disconnect_parse,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_sshmsg_t *tt_sshmsg_disconnect_create()
{
    return tt_sshmsg_create(TT_SSH_MSGID_DISCONNECT,
                            sizeof(tt_sshmsg_disconnect_t),
                            &__disconnect_op);
}

void tt_sshmsg_disconnect_set_reason(IN tt_sshmsg_t *msg,
                                     IN tt_u32_t reason_code)
{
    tt_sshmsg_disconnect_t *msg_disc;

    TT_ASSERT_SSH(msg != NULL);
    TT_ASSERT_SSH(msg->msg_id == TT_SSH_MSGID_DISCONNECT);

    msg_disc = TT_SSHMSG_CAST(msg, tt_sshmsg_disconnect_t);

    msg_disc->reason_code = reason_code;
}

void tt_sshmsg_disconnect_set_desc(IN tt_sshmsg_t *msg,
                                   IN const tt_char_t *description)
{
    tt_sshmsg_disconnect_t *msg_disc;

    TT_ASSERT_SSH(msg != NULL);
    TT_ASSERT_SSH(msg->msg_id == TT_SSH_MSGID_DISCONNECT);
    TT_ASSERT_SSH(description != NULL);

    msg_disc = TT_SSHMSG_CAST(msg, tt_sshmsg_disconnect_t);

    tt_strncpy(msg_disc->description, description, TT_SSHMSG_DISC_DESC_LEN);
}

tt_result_t __disconnect_render_prepare(IN struct tt_sshmsg_s *msg,
                                        OUT tt_u32_t *len,
                                        OUT tt_ssh_render_mode_t *mode)
{
    tt_sshmsg_disconnect_t *msg_disc;
    tt_u32_t msg_len = 0;

    msg_disc = TT_SSHMSG_CAST(msg, tt_sshmsg_disconnect_t);

    /*
     byte SSH_MSG_DISCONNECT
     uint32 reason code
     string description in ISO-10646 UTF-8 encoding [RFC3629]
     string language tag [RFC3066]
     */

    // byte
    msg_len += tt_ssh_byte_render_prepare();

    // uint32
    msg_len += tt_ssh_byte_render_prepare();

    // string
    msg_len += tt_ssh_string_render_prepare((tt_u8_t *)msg_disc->description,
                                            (tt_u32_t)tt_strlen(
                                                msg_disc->description));

    // string
    msg_len += tt_ssh_string_render_prepare(NULL, 0);

    *len = msg_len;
    *mode = TT_SSH_RENDER_MODE_BINARY;
    return TT_SUCCESS;
}

tt_result_t __disconnect_render(IN struct tt_sshmsg_s *msg,
                                IN OUT tt_buf_t *buf)
{
    tt_sshmsg_disconnect_t *msg_disc;

    msg_disc = TT_SSHMSG_CAST(msg, tt_sshmsg_disconnect_t);

    // byte
    TT_DO(tt_ssh_byte_render(buf, TT_SSH_MSGID_DISCONNECT));

    // uint32
    TT_DO(tt_ssh_uint32_render(buf, msg_disc->reason_code));

    // string
    TT_DO(tt_ssh_string_render(buf,
                               (tt_u8_t *)msg_disc->description,
                               (tt_u32_t)tt_strlen(msg_disc->description)));

    // string
    TT_DO(tt_ssh_string_render(buf, NULL, 0));

    return TT_SUCCESS;
}

tt_result_t __disconnect_parse(IN struct tt_sshmsg_s *msg, IN tt_buf_t *data)
{
    tt_sshmsg_disconnect_t *msg_disc;
    tt_u8_t *desc;
    tt_u32_t desc_len;

    msg_disc = TT_SSHMSG_CAST(msg, tt_sshmsg_disconnect_t);

    // uint32
    TT_DO(tt_ssh_uint32_parse(data, &msg_disc->reason_code));

    // string
    TT_DO(tt_ssh_string_parse(data, &desc, &desc_len));
    if (desc_len > TT_SSHMSG_DISC_DESC_LEN) {
        desc_len = TT_SSHMSG_DISC_DESC_LEN;
    }
    tt_memcpy(msg_disc->description, desc, desc_len);
    msg_disc->description[desc_len] = 0;

    return TT_SUCCESS;
}
