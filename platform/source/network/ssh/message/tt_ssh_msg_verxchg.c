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

#include <network/ssh/message/tt_ssh_msg_verxchg.h>

#include <algorithm/tt_buffer_format.h>
#include <network/ssh/message/tt_ssh_message.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __VX_CRLF "\r\n"

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_result_t __verxchg_create(IN struct tt_sshmsg_s *msg);
static void __verxchg_destroy(IN struct tt_sshmsg_s *msg);

static tt_result_t __verxchg_render_prepare(IN struct tt_sshmsg_s *msg,
                                            OUT tt_u32_t *len,
                                            OUT tt_ssh_render_mode_t *mode);
static tt_result_t __verxchg_render(IN struct tt_sshmsg_s *msg,
                                    IN OUT tt_buf_t *buf);

static tt_result_t __verxchg_parse(IN struct tt_sshmsg_s *msg,
                                   IN tt_buf_t *data);

static tt_sshmsg_itf_t __verxchg_op = {
    __verxchg_create,         __verxchg_destroy, NULL,

    __verxchg_render_prepare, __verxchg_render,

    __verxchg_parse,
};

static const tt_char_t *__ssh_protover_name[TT_SSH_VER_NUM] = {
    "SSH-1.0",
    "SSH-1.99",
    "SSH-2.0",
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_sshmsg_t *tt_sshms_verxchg_create()
{
    return tt_sshmsg_create(TT_SSH_MSGID_VERXCHG, sizeof(tt_sshms_verxchg_t),
                            &__verxchg_op);
}

void tt_sshms_verxchg_set_protover(IN tt_sshmsg_t *msg, IN tt_ssh_ver_t ver)
{
    tt_sshms_verxchg_t *vx;

    TT_ASSERT_SSH(msg != NULL);
    TT_ASSERT_SSH(msg->msg_id == TT_SSH_MSGID_VERXCHG);

    vx = TT_SSHMSG_CAST(msg, tt_sshms_verxchg_t);

    TT_ASSERT(TT_SSH_VER_VALID(ver));
    vx->protover = ver;
}

tt_result_t tt_sshms_verxchg_set_swver(IN tt_sshmsg_t *msg,
                                       IN const tt_char_t *ver)
{
    tt_sshms_verxchg_t *vx;
    tt_u32_t ver_len;

    TT_ASSERT_SSH(msg != NULL);
    TT_ASSERT_SSH(msg->msg_id == TT_SSH_MSGID_VERXCHG);
    TT_ASSERT_SSH(ver != NULL);

    vx = TT_SSHMSG_CAST(msg, tt_sshms_verxchg_t);

    ver_len = (tt_u32_t)tt_strlen(ver);
    if (ver_len == 0) {
        TT_ERROR("software version can not be empty");
        return TT_FAIL;
    }

    tt_blob_destroy(&vx->swver);
    return tt_blob_create(&vx->swver, (tt_u8_t *)ver, ver_len);
}

tt_result_t tt_sshms_verxchg_set_comment(IN tt_sshmsg_t *msg,
                                         IN const tt_char_t *comment)
{
    tt_sshms_verxchg_t *vx;
    tt_u32_t len;

    TT_ASSERT_SSH(msg != NULL);
    TT_ASSERT_SSH(msg->msg_id == TT_SSH_MSGID_VERXCHG);

    vx = TT_SSHMSG_CAST(msg, tt_sshms_verxchg_t);

    if ((comment != NULL) && (len = (tt_u32_t)tt_strlen(comment)) != 0) {
        tt_blob_destroy(&vx->comment);
        return tt_blob_create(&vx->comment, (tt_u8_t *)comment, len);
    } else {
        tt_blob_destroy(&vx->comment);
        tt_blob_init(&vx->comment);
        return TT_SUCCESS;
    }
}

tt_result_t __verxchg_create(IN struct tt_sshmsg_s *msg)
{
    tt_sshms_verxchg_t *vx;

    vx = TT_SSHMSG_CAST(msg, tt_sshms_verxchg_t);

    vx->protover = TT_SSH_VER_NUM;
    tt_blob_init(&vx->swver);
    tt_blob_init(&vx->comment);

    // vs and vc need be saved
    msg->save = TT_TRUE;

    return TT_SUCCESS;
}

void __verxchg_destroy(IN struct tt_sshmsg_s *msg)
{
    tt_sshms_verxchg_t *vx;

    vx = TT_SSHMSG_CAST(msg, tt_sshms_verxchg_t);

    tt_blob_destroy(&vx->swver);
    tt_blob_destroy(&vx->comment);
}

tt_result_t __verxchg_render_prepare(IN struct tt_sshmsg_s *msg,
                                     OUT tt_u32_t *len,
                                     OUT tt_ssh_render_mode_t *mode)
{
    tt_sshms_verxchg_t *vx;
    tt_u32_t msg_len = 0;

    vx = TT_SSHMSG_CAST(msg, tt_sshms_verxchg_t);

    // SSH-protoversion-softwareversion SP comments CR LF

    // protocol version
    TT_ASSERT_SSH(TT_SSH_VER_VALID(vx->protover));
    msg_len += (tt_u32_t)tt_strlen(__ssh_protover_name[vx->protover]);

    // software version
    TT_ASSERT_SSH(vx->swver.len != 0);
    msg_len += 1 + vx->swver.len;

    // comments
    if (vx->comment.len != 0) { msg_len += 1 + vx->comment.len; }

    // cr lf
    msg_len += 2;

    *len = msg_len;
    *mode = TT_SSH_RENDER_MODE_RAW;
    return TT_SUCCESS;
}

tt_result_t __verxchg_render(IN struct tt_sshmsg_s *msg, IN OUT tt_buf_t *buf)
{
    tt_sshms_verxchg_t *vx;

    vx = TT_SSHMSG_CAST(msg, tt_sshms_verxchg_t);

    // protocol version
    TT_ASSERT_SSH(TT_SSH_VER_VALID(vx->protover));
    TT_DO(tt_buf_put(buf, (tt_u8_t *)__ssh_protover_name[vx->protover],
                     (tt_u32_t)tt_strlen(__ssh_protover_name[vx->protover])));

    // software version
    TT_ASSERT_SSH(vx->swver.len != 0);
    TT_DO(tt_buf_put_u8(buf, '-'));
    TT_DO(tt_buf_put(buf, vx->swver.addr, vx->swver.len));

    // comments
    if (vx->comment.len != 0) {
        TT_DO(tt_buf_put_u8(buf, ' '));
        TT_DO(tt_buf_put(buf, vx->comment.addr, vx->comment.len));
    }

    // cr lf
    TT_DO(tt_buf_put(buf, (tt_u8_t *)"\r\n", 2));

    return TT_SUCCESS;
}

tt_result_t __verxchg_parse(IN struct tt_sshmsg_s *msg, IN tt_buf_t *data)
{
    tt_sshms_verxchg_t *vx;
    tt_u32_t data_len, pos, end_pos;
    tt_char_t *vx_pos, *vx_end;
    tt_result_t result = TT_FAIL;

    vx = TT_SSHMSG_CAST(msg, tt_sshms_verxchg_t);

    /*
     RFC4253:

     The server MAY send other lines of data before sending the version
     string. Each line SHOULD be terminated by a Carriage Return and Line
     Feed. Such lines MUST NOT begin with "SSH-", and SHOULD be encoded
     in ISO-10646 UTF-8
     */

    data_len = TT_BUF_RLEN(data);
    if (data_len < 4) { return TT_E_BUF_NOBUFS; }

    // find "SSH-"
    pos = 0;
    end_pos = data_len - 4;
    while (pos <= end_pos) {
        if (tt_memcmp(&data->p[pos], "SSH-", 4) == 0) { break; }
        ++pos;
    }
    if (pos > end_pos) {
        return TT_COND(data_len < 1000, TT_E_BUF_NOBUFS, TT_FAIL);
    }
    vx_pos = (tt_char_t *)&data->p[pos];
    pos += 4;

#if 0
    // find "\r\n"
    end_pos = data_len - 2;
    while (pos <= end_pos)
    {
        if (tt_memcmp(&data->p[pos], "\r\n", 2) == 0)
        {
            break;
        }
        ++pos;
    }
    if (pos > end_pos)
    {
        return TT_COND(data_len < 1000, TT_E_BUF_NOBUFS, TT_FAIL);
    }
    data->p[pos] = 0;
    vx_end = (tt_char_t*)&data->p[pos + 2];
#else
    // find "\n"
    end_pos = data_len - 1;
    while (pos <= end_pos) {
        if (data->p[pos] == '\n') { break; }
        ++pos;
    }
    if (pos > end_pos) {
        return TT_COND(data_len < 1000, TT_E_BUF_NOBUFS, TT_FAIL);
    }
    data->p[pos] = 0;
    vx_end = (tt_char_t *)&data->p[pos + 1];
#endif

    // protocol version
    if (tt_strncmp(vx_pos, "SSH-1.0-", 8) == 0) {
        vx->protover = TT_SSH_VER_1_0;
        vx_pos += 8;
    } else if (tt_strncmp(vx_pos, "SSH-1.99-", 9) == 0) {
        vx->protover = TT_SSH_VER_1_99;
        vx_pos += 9;
    } else if (tt_strncmp(vx_pos, "SSH-2.0-", 8) == 0) {
        vx->protover = TT_SSH_VER_2_0;
        vx_pos += 8;
    } else {
        TT_ERROR("unsupported ssh version");
        goto p_out;
    }

    // software version
    // ignored

    // comments
    // ignored

    tt_buf_set_rptr(data, (tt_u8_t *)vx_end);
    result = TT_SUCCESS;

p_out:

#if 0
    data->p[pos] = '\r';
#else
    data->p[pos] = '\n';
#endif

    return result;
}
