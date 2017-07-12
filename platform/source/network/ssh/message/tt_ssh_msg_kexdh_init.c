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

#include <network/ssh/message/tt_ssh_msg_kexdh_init.h>

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

static tt_result_t __kexdh_init_create(IN struct tt_sshmsg_s *msg);
static void __kexdh_init_destroy(IN struct tt_sshmsg_s *msg);

static tt_result_t __kexdh_init_render_prepare(IN struct tt_sshmsg_s *msg,
                                               OUT tt_u32_t *len,
                                               OUT tt_ssh_render_mode_t *mode);
static tt_result_t __kexdh_init_render(IN struct tt_sshmsg_s *msg,
                                       IN OUT tt_buf_t *buf);

static tt_result_t __kexdh_init_parse(IN struct tt_sshmsg_s *msg,
                                      IN tt_buf_t *data);

static tt_sshmsg_itf_t __kexdh_init_op = {
    __kexdh_init_create,
    __kexdh_init_destroy,
    NULL,

    __kexdh_init_render_prepare,
    __kexdh_init_render,

    __kexdh_init_parse,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_sshmsg_t *tt_sshmsg_kexdh_init_create()
{
    return tt_sshmsg_create(TT_SSH_MSGID_KEXDH_INIT,
                            sizeof(tt_sshmsg_kexdh_init_t),
                            &__kexdh_init_op);
}

tt_result_t __kexdh_init_create(IN struct tt_sshmsg_s *msg)
{
    tt_sshmsg_kexdh_init_t *ki;

    ki = TT_SSHMSG_CAST(msg, tt_sshmsg_kexdh_init_t);

    tt_blob_init(&ki->e);

    return TT_SUCCESS;
}

void __kexdh_init_destroy(IN struct tt_sshmsg_s *msg)
{
    tt_sshmsg_kexdh_init_t *ki;

    ki = TT_SSHMSG_CAST(msg, tt_sshmsg_kexdh_init_t);

    tt_blob_destroy(&ki->e);
}

tt_result_t __kexdh_init_render_prepare(IN struct tt_sshmsg_s *msg,
                                        OUT tt_u32_t *len,
                                        OUT tt_ssh_render_mode_t *mode)
{
    tt_sshmsg_kexdh_init_t *ki;

    ki = TT_SSHMSG_CAST(msg, tt_sshmsg_kexdh_init_t);

    // todo
    (void)ki;
    TT_ASSERT(0);
    return TT_SUCCESS;
}

tt_result_t __kexdh_init_render(IN struct tt_sshmsg_s *msg,
                                IN OUT tt_buf_t *buf)
{
    tt_sshmsg_kexdh_init_t *ki;

    ki = TT_SSHMSG_CAST(msg, tt_sshmsg_kexdh_init_t);

    // todo
    (void)ki;
    TT_ASSERT(0);
    return TT_SUCCESS;
}

tt_result_t __kexdh_init_parse(IN struct tt_sshmsg_s *msg, IN tt_buf_t *data)
{
    tt_sshmsg_kexdh_init_t *ki;
    tt_u8_t *mpint;
    tt_u32_t mpint_len;

    ki = TT_SSHMSG_CAST(msg, tt_sshmsg_kexdh_init_t);

    /*
     byte SSH_MSG_KEXDH_INIT
     mpint e
     */

    TT_DO(tt_ssh_mpint_parse(data, &mpint, &mpint_len));
    if ((mpint_len == 0) || !TT_OK(tt_blob_create(&ki->e, mpint, mpint_len))) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}
