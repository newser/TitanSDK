/* Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownechfailhip.
 * The ASF licenses this file to You under the Apache License, Vechfailion 2.0
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

#include <network/ssh/message/tt_ssh_msg_channel_failure.h>

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

static tt_result_t __chfail_render_prepare(IN struct tt_sshmsg_s *msg,
                                           OUT tt_u32_t *len,
                                           OUT tt_ssh_render_mode_t *mode);
static tt_result_t __chfail_render(IN struct tt_sshmsg_s *msg,
                                   IN OUT tt_buf_t *buf);

static tt_result_t __chfail_parse(IN struct tt_sshmsg_s *msg,
                                  IN tt_buf_t *data);

static tt_sshmsg_itf_t __chfail_op = {
    NULL,
    NULL,
    NULL,

    __chfail_render_prepare,
    __chfail_render,

    __chfail_parse,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_sshmsg_t *tt_sshmsg_chfail_create()
{
    return tt_sshmsg_create(TT_SSH_MSGID_CHANNEL_FAILURE,
                            sizeof(tt_sshmsg_chfail_t),
                            &__chfail_op);
}

void tt_sshmsg_chfail_set_rcvchnum(IN tt_sshmsg_t *msg, IN tt_u32_t rcv_chnum)
{
    tt_sshmsg_chfail_t *chfail;

    TT_ASSERT(msg != NULL);
    TT_ASSERT(msg->msg_id == TT_SSH_MSGID_CHANNEL_FAILURE);

    chfail = TT_SSHMSG_CAST(msg, tt_sshmsg_chfail_t);

    chfail->rcv_chnum = rcv_chnum;
}

tt_result_t __chfail_render_prepare(IN struct tt_sshmsg_s *msg,
                                    OUT tt_u32_t *len,
                                    OUT tt_ssh_render_mode_t *mode)
{
    tt_sshmsg_chfail_t *chfail;
    tt_u32_t msg_len = 0;

    /*
     byte SSH_MSG_CHANNEL_FAILURE
     uint32 recipient channel
     */

    chfail = TT_SSHMSG_CAST(msg, tt_sshmsg_chfail_t);
    (void)chfail;

    // byte SSH_MSG_CHANNEL_FAILURE
    msg_len += tt_ssh_byte_render_prepare();

    // uint32 recipient channel
    msg_len += tt_ssh_uint32_render_prepare();

    *len = msg_len;
    *mode = TT_SSH_RENDER_MODE_BINARY;
    return TT_SUCCESS;
}

tt_result_t __chfail_render(IN struct tt_sshmsg_s *msg, IN OUT tt_buf_t *buf)
{
    tt_sshmsg_chfail_t *chfail;

    /*
     byte SSH_MSG_CHANNEL_FAILURE
     uint32 recipient channel
     */

    chfail = TT_SSHMSG_CAST(msg, tt_sshmsg_chfail_t);

    // byte SSH_MSG_CHANNEL_FAILURE
    TT_DO(tt_ssh_byte_render(buf, TT_SSH_MSGID_CHANNEL_FAILURE));

    // uint32 recipient channel
    TT_DO(tt_ssh_uint32_render(buf, chfail->rcv_chnum));

    return TT_SUCCESS;
}

tt_result_t __chfail_parse(IN struct tt_sshmsg_s *msg, IN tt_buf_t *data)
{
    tt_sshmsg_chfail_t *chfail;

    /*
     byte SSH_MSG_CHANNEL_FAILURE
     uint32 recipient channel
     */

    chfail = TT_SSHMSG_CAST(msg, tt_sshmsg_chfail_t);

    // uint32 recipient channel
    TT_DO(tt_ssh_uint32_parse(data, &chfail->rcv_chnum));

    return TT_SUCCESS;
}
