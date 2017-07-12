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

#include <network/ssh/message/tt_ssh_msg_userauth_banner.h>

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

static tt_result_t __uab_create(IN struct tt_sshmsg_s *msg);

static tt_result_t __uab_render_prepare(IN struct tt_sshmsg_s *msg,
                                        OUT tt_u32_t *len,
                                        OUT tt_ssh_render_mode_t *mode);
static tt_result_t __uab_render(IN struct tt_sshmsg_s *msg,
                                IN OUT tt_buf_t *buf);

static tt_result_t __uab_parse(IN struct tt_sshmsg_s *msg, IN tt_buf_t *data);

static tt_sshmsg_itf_t __uab_op = {
    __uab_create,
    NULL,
    NULL,

    __uab_render_prepare,
    __uab_render,

    __uab_parse,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_sshmsg_t *tt_sshmsg_uab_create()
{
    return tt_sshmsg_create(TT_SSH_MSGID_USERAUTH_BANNER,
                            sizeof(tt_sshmsg_uab_t),
                            &__uab_op);
}

tt_result_t tt_sshmsg_uab_set_banner(IN tt_sshmsg_t *msg,
                                     IN const tt_char_t *banner)
{
    tt_sshmsg_uab_t *uab;

    TT_ASSERT(msg != NULL);
    TT_ASSERT(msg->msg_id == TT_SSH_MSGID_USERAUTH_BANNER);
    TT_ASSERT(banner != NULL);

    uab = TT_SSHMSG_CAST(msg, tt_sshmsg_uab_t);

    tt_buf_reset_rwp(&uab->banner);
    TT_DO(tt_buf_put(&uab->banner,
                     (tt_u8_t *)banner,
                     (tt_u32_t)tt_strlen(banner)));

    return TT_SUCCESS;
}

tt_result_t __uab_create(IN struct tt_sshmsg_s *msg)
{
    tt_sshmsg_uab_t *uab;

    uab = TT_SSHMSG_CAST(msg, tt_sshmsg_uab_t);

    tt_buf_init(&uab->banner, NULL);

    return TT_SUCCESS;
}

tt_result_t __uab_render_prepare(IN struct tt_sshmsg_s *msg,
                                 OUT tt_u32_t *len,
                                 OUT tt_ssh_render_mode_t *mode)
{
    tt_sshmsg_uab_t *uab;
    tt_u32_t msg_len = 0;

    /*
     byte SSH_MSG_USERAUTH_BANNER
     string message in ISO-10646 UTF-8 encoding [RFC3629]
     string language tag [RFC3066]
     */

    uab = TT_SSHMSG_CAST(msg, tt_sshmsg_uab_t);

    // byte
    msg_len += tt_ssh_byte_render_prepare();

    // string message
    msg_len += tt_ssh_string_render_prepare(NULL, TT_BUF_RLEN(&uab->banner));

    // string language tag
    msg_len += tt_ssh_string_render_prepare(NULL, 0);

    *len = msg_len;
    *mode = TT_SSH_RENDER_MODE_BINARY;
    return TT_SUCCESS;
}

tt_result_t __uab_render(IN struct tt_sshmsg_s *msg, IN OUT tt_buf_t *buf)
{
    tt_sshmsg_uab_t *uab;

    /*
     byte SSH_MSG_USERAUTH_BANNER
     string message in ISO-10646 UTF-8 encoding [RFC3629]
     string language tag [RFC3066]
     */

    uab = TT_SSHMSG_CAST(msg, tt_sshmsg_uab_t);

    // byte
    TT_DO(tt_ssh_byte_render(buf, TT_SSH_MSGID_USERAUTH_BANNER));

    // string message
    TT_DO(tt_ssh_string_render(buf,
                               TT_BUF_RPOS(&uab->banner),
                               TT_BUF_RLEN(&uab->banner)));

    // string language tag
    TT_DO(tt_ssh_string_render(buf, NULL, 0));

    return TT_SUCCESS;
}

tt_result_t __uab_parse(IN struct tt_sshmsg_s *msg, IN tt_buf_t *data)
{
    tt_sshmsg_uab_t *uab;
    tt_u8_t *addr;
    tt_u32_t len;

    uab = TT_SSHMSG_CAST(msg, tt_sshmsg_uab_t);

    /*
     byte SSH_MSG_USERAUTH_BANNER
     string message in ISO-10646 UTF-8 encoding [RFC3629]
     string language tag [RFC3066]
     */

    // string message
    TT_DO(tt_ssh_string_parse(data, &addr, &len));
    TT_DO(tt_buf_put(&uab->banner, addr, len));

    return TT_SUCCESS;
}
