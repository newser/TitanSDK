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

#include <network/ssh/message/tt_ssh_msg_service_accept.h>

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

static tt_result_t __servacc_create(IN struct tt_sshmsg_s *msg);

static tt_result_t __servacc_render_prepare(IN struct tt_sshmsg_s *msg,
                                            OUT tt_u32_t *len,
                                            OUT tt_ssh_render_mode_t *mode);
static tt_result_t __servacc_render(IN struct tt_sshmsg_s *msg,
                                    IN OUT tt_buf_t *buf);

static tt_result_t __servacc_parse(IN struct tt_sshmsg_s *msg,
                                   IN tt_buf_t *data);

static tt_sshmsg_itf_t __servacc_op = {
    __servacc_create,
    NULL,
    NULL,

    __servacc_render_prepare,
    __servacc_render,

    __servacc_parse,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_sshmsg_t *tt_sshmsg_servacc_create()
{
    return tt_sshmsg_create(TT_SSH_MSGID_SERVICE_ACCEPT,
                            sizeof(tt_sshmsg_servacc_t),
                            &__servacc_op);
}

void tt_sshmsg_servacc_set_service(IN tt_sshmsg_t *msg,
                                   IN tt_ssh_service_t service)
{
    tt_sshmsg_servacc_t *msg_servacc;

    TT_ASSERT(msg != NULL);
    TT_ASSERT(msg->msg_id == TT_SSH_MSGID_SERVICE_ACCEPT);
    TT_ASSERT(TT_SSH_SERVICE_VALID(service));

    msg_servacc = TT_SSHMSG_CAST(msg, tt_sshmsg_servacc_t);

    msg_servacc->service = service;
}

tt_ssh_service_t tt_sshmsg_servacc_get_service(IN tt_sshmsg_t *msg)
{
    tt_sshmsg_servacc_t *msg_servacc;

    TT_ASSERT(msg != NULL);
    TT_ASSERT(msg->msg_id == TT_SSH_MSGID_SERVICE_ACCEPT);

    msg_servacc = TT_SSHMSG_CAST(msg, tt_sshmsg_servacc_t);

    return msg_servacc->service;
}

tt_result_t __servacc_create(IN struct tt_sshmsg_s *msg)
{
    tt_sshmsg_servacc_t *servacc;

    servacc = TT_SSHMSG_CAST(msg, tt_sshmsg_servacc_t);

    servacc->service = TT_SSH_SERVICE_NUM;

    return TT_SUCCESS;
}

tt_result_t __servacc_render_prepare(IN struct tt_sshmsg_s *msg,
                                     OUT tt_u32_t *len,
                                     OUT tt_ssh_render_mode_t *mode)
{
    tt_sshmsg_servacc_t *servacc;
    tt_ssh_service_t sshserv;
    tt_u32_t msg_len = 0;

    /*
     byte TT_SSH_MSGID_SERVICE_ACCEPT
     string service name
     */

    servacc = TT_SSHMSG_CAST(msg, tt_sshmsg_servacc_t);

    sshserv = servacc->service;
    if (!TT_SSH_SERVICE_VALID(sshserv)) {
        TT_ERROR("no service set");
        return TT_FAIL;
    }

    // byte
    msg_len += tt_ssh_byte_render_prepare();

    // string
    // currently no data
    msg_len +=
        tt_ssh_string_render_prepare((tt_u8_t *)tt_g_ssh_serv_name[sshserv],
                                     (tt_u32_t)tt_strlen(
                                         tt_g_ssh_serv_name[sshserv]));

    *len = msg_len;
    *mode = TT_SSH_RENDER_MODE_BINARY;
    return TT_SUCCESS;
}

tt_result_t __servacc_render(IN struct tt_sshmsg_s *msg, IN OUT tt_buf_t *buf)
{
    tt_sshmsg_servacc_t *servacc;
    tt_ssh_service_t sshserv;

    servacc = TT_SSHMSG_CAST(msg, tt_sshmsg_servacc_t);

    sshserv = servacc->service;

    // byte
    TT_DO(tt_ssh_byte_render(buf, TT_SSH_MSGID_SERVICE_ACCEPT));

    // string
    TT_DO(
        tt_ssh_string_render(buf,
                             (tt_u8_t *)tt_g_ssh_serv_name[sshserv],
                             (tt_u32_t)tt_strlen(tt_g_ssh_serv_name[sshserv])));

    return TT_SUCCESS;
}

tt_result_t __servacc_parse(IN struct tt_sshmsg_s *msg, IN tt_buf_t *data)
{
    tt_sshmsg_servacc_t *servacc;
    tt_u8_t *serv;
    tt_u32_t serv_len;
    tt_ssh_service_t sshserv;

    servacc = TT_SSHMSG_CAST(msg, tt_sshmsg_servacc_t);

    /*
    byte TT_SSH_MSGID_SERVICE_ACCEPT
    string service name
    */

    TT_DO(tt_ssh_string_parse(data, &serv, &serv_len));
    sshserv = tt_ssh_serv_match(serv, serv_len);
    if (sshserv == TT_SSH_SERVICE_NUM) {
        TT_ERROR("not supported ssh service");
        return TT_FAIL;
    }
    servacc->service = sshserv;

    return TT_SUCCESS;
}
