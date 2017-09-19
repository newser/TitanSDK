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

#include <network/ssh/message/tt_ssh_msg_userauth_failure.h>

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

static tt_result_t __uaf_create(IN struct tt_sshmsg_s *msg);

static tt_result_t __uaf_render_prepare(IN struct tt_sshmsg_s *msg,
                                        OUT tt_u32_t *len,
                                        OUT tt_ssh_render_mode_t *mode);
static tt_result_t __uaf_render(IN struct tt_sshmsg_s *msg,
                                IN OUT tt_buf_t *buf);

static tt_result_t __uaf_parse(IN struct tt_sshmsg_s *msg, IN tt_buf_t *data);

static tt_sshmsg_itf_t __uaf_op = {
    __uaf_create,
    NULL,
    NULL,

    __uaf_render_prepare,
    __uaf_render,

    __uaf_parse,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_sshmsg_t *tt_sshmsg_uaf_create()
{
    return tt_sshmsg_create(TT_SSH_MSGID_USERAUTH_FAILURE,
                            sizeof(tt_sshmsg_uaf_t),
                            &__uaf_op);
}

void tt_sshmsg_uaf_add_auth(IN tt_sshmsg_t *msg, IN tt_ssh_auth_t auth)
{
    tt_sshmsg_uaf_t *uaf;
    tt_u32_t i;

    TT_ASSERT(msg != NULL);
    TT_ASSERT(msg->msg_id == TT_SSH_MSGID_USERAUTH_FAILURE);
    TT_ASSERT(TT_SSH_AUTH_VALID(auth));

    uaf = TT_SSHMSG_CAST(msg, tt_sshmsg_uaf_t);

    for (i = 0; i < uaf->auth_num; ++i) {
        if (uaf->auth[i] == auth) {
            return;
        }
    }
    uaf->auth[uaf->auth_num++] = auth;
}

void tt_sshmsg_uaf_set_parial_succ(IN tt_sshmsg_t *msg,
                                   IN tt_bool_t partial_success)
{
    tt_sshmsg_uaf_t *uaf;

    TT_ASSERT(msg != NULL);
    TT_ASSERT(msg->msg_id == TT_SSH_MSGID_USERAUTH_FAILURE);

    uaf = TT_SSHMSG_CAST(msg, tt_sshmsg_uaf_t);

    uaf->partial_success = partial_success;
}

tt_result_t __uaf_create(IN struct tt_sshmsg_s *msg)
{
    tt_sshmsg_uaf_t *uaf;

    uaf = TT_SSHMSG_CAST(msg, tt_sshmsg_uaf_t);

    tt_memset(uaf->auth, 0, sizeof(uaf->auth));
    uaf->auth_num = 0;

    uaf->partial_success = TT_FALSE;

    return TT_SUCCESS;
}

tt_result_t __uaf_render_prepare(IN struct tt_sshmsg_s *msg,
                                 OUT tt_u32_t *len,
                                 OUT tt_ssh_render_mode_t *mode)
{
    tt_sshmsg_uaf_t *uaf;
    tt_u32_t msg_len = 0;

    const tt_char_t *auth_name[TT_SSH_AUTH_NUM] = {0};
    tt_u32_t i;

    /*
     byte SSH_MSG_USERAUTH_FAILURE
     name-list authentications that can continue
     boolean partial success
     */

    uaf = TT_SSHMSG_CAST(msg, tt_sshmsg_uaf_t);

    // byte
    msg_len += tt_ssh_byte_render_prepare();

    // name-list authentications
    for (i = 0; i < uaf->auth_num; ++i) {
        auth_name[i] = tt_g_ssh_auth_name[uaf->auth[i]];
    }
    msg_len += tt_ssh_namelist_render_prepare(auth_name, uaf->auth_num);

    // boolean partial success
    msg_len += tt_ssh_boolean_render_prepare();

    *len = msg_len;
    *mode = TT_SSH_RENDER_MODE_BINARY;
    return TT_SUCCESS;
}

tt_result_t __uaf_render(IN struct tt_sshmsg_s *msg, IN OUT tt_buf_t *buf)
{
    tt_sshmsg_uaf_t *uaf;

    const tt_char_t *auth_name[TT_SSH_AUTH_NUM] = {0};
    tt_u32_t i;

    /*
     byte SSH_MSG_USERAUTH_FAILURE
     name-list authentications that can continue
     boolean partial success
     */

    uaf = TT_SSHMSG_CAST(msg, tt_sshmsg_uaf_t);

    // byte
    TT_DO(tt_ssh_byte_render(buf, TT_SSH_MSGID_USERAUTH_FAILURE));

    // name-list authentications
    for (i = 0; i < uaf->auth_num; ++i) {
        auth_name[i] = tt_g_ssh_auth_name[uaf->auth[i]];
    }
    TT_DO(tt_ssh_namelist_render(buf, auth_name, uaf->auth_num));

    // boolean partial success
    TT_DO(tt_ssh_boolean_render(buf, uaf->partial_success));

    return TT_SUCCESS;
}

tt_result_t __uaf_parse_auth(IN tt_char_t *name,
                             IN tt_u32_t name_len,
                             IN void *param)
{
    tt_sshmsg_uaf_t *uaf = (tt_sshmsg_uaf_t *)param;
    tt_ssh_auth_t auth;
    tt_u32_t i;

    auth = tt_ssh_auth_match((tt_u8_t *)name, name_len);
    if (!TT_SSH_AUTH_VALID(auth)) {
        TT_ERROR("unsupported ssh auth");
        return TT_FAIL;
    }

    for (i = 0; i < uaf->auth_num; ++i) {
        if (uaf->auth[i] == auth) {
            break;
        }
    }
    if (i == uaf->auth_num) {
        uaf->auth[uaf->auth_num++] = auth;
    }

    return TT_SUCCESS;
}

tt_result_t __uaf_parse(IN struct tt_sshmsg_s *msg, IN tt_buf_t *data)
{
    tt_sshmsg_uaf_t *uaf;

    uaf = TT_SSHMSG_CAST(msg, tt_sshmsg_uaf_t);

    /*
     byte SSH_MSG_USERAUTH_FAILURE
     name-list authentications that can continue
     boolean partial success
     */

    // name-list authentications
    TT_DO(tt_ssh_namelist_parse(data, __uaf_parse_auth, uaf));

    // boolean partial success
    TT_DO(tt_ssh_boolean_parse(data, &uaf->partial_success));

    return TT_SUCCESS;
}
