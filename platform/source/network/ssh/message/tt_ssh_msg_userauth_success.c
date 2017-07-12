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

#include <network/ssh/message/tt_ssh_msg_userauth_success.h>

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

static tt_result_t __uas_render_prepare(IN struct tt_sshmsg_s *msg,
                                        OUT tt_u32_t *len,
                                        OUT tt_ssh_render_mode_t *mode);
static tt_result_t __uas_render(IN struct tt_sshmsg_s *msg,
                                IN OUT tt_buf_t *buf);

static tt_result_t __uas_parse(IN struct tt_sshmsg_s *msg, IN tt_buf_t *data);

static tt_sshmsg_itf_t __uas_op = {
    NULL,
    NULL,
    NULL,

    __uas_render_prepare,
    __uas_render,

    __uas_parse,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_sshmsg_t *tt_sshmsg_uas_create()
{
    return tt_sshmsg_create(TT_SSH_MSGID_USERAUTH_SUCCESS, 0, &__uas_op);
}

tt_result_t __uas_render_prepare(IN struct tt_sshmsg_s *msg,
                                 OUT tt_u32_t *len,
                                 OUT tt_ssh_render_mode_t *mode)
{
    tt_u32_t msg_len = 0;

    /*
    byte SSH_MSG_USERAUTH_SUCCESS
    */

    // byte
    msg_len += tt_ssh_byte_render_prepare();

    *len = msg_len;
    *mode = TT_SSH_RENDER_MODE_BINARY;
    return TT_SUCCESS;
}

tt_result_t __uas_render(IN struct tt_sshmsg_s *msg, IN OUT tt_buf_t *buf)
{
    // byte
    TT_DO(tt_ssh_byte_render(buf, TT_SSH_MSGID_USERAUTH_SUCCESS));

    return TT_SUCCESS;
}

tt_result_t __uas_parse(IN struct tt_sshmsg_s *msg, IN tt_buf_t *data)
{
    return TT_SUCCESS;
}
