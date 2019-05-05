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

#include <network/ssh/message/tt_ssh_message.h>

#include <memory/tt_memory_alloc.h>
#include <misc/tt_reference_counter.h>
#include <network/ssh/message/tt_ssh_msg_channel_close.h>
#include <network/ssh/message/tt_ssh_msg_channel_data.h>
#include <network/ssh/message/tt_ssh_msg_channel_eof.h>
#include <network/ssh/message/tt_ssh_msg_channel_failure.h>
#include <network/ssh/message/tt_ssh_msg_channel_open.h>
#include <network/ssh/message/tt_ssh_msg_channel_open_confirmation.h>
#include <network/ssh/message/tt_ssh_msg_channel_open_failure.h>
#include <network/ssh/message/tt_ssh_msg_channel_request.h>
#include <network/ssh/message/tt_ssh_msg_channel_success.h>
#include <network/ssh/message/tt_ssh_msg_channel_window_adjust.h>
#include <network/ssh/message/tt_ssh_msg_disconnect.h>
#include <network/ssh/message/tt_ssh_msg_global_request.h>
#include <network/ssh/message/tt_ssh_msg_ignore.h>
#include <network/ssh/message/tt_ssh_msg_kexdh_init.h>
#include <network/ssh/message/tt_ssh_msg_keyinit.h>
#include <network/ssh/message/tt_ssh_msg_newkeys.h>
#include <network/ssh/message/tt_ssh_msg_request_failure.h>
#include <network/ssh/message/tt_ssh_msg_request_success.h>
#include <network/ssh/message/tt_ssh_msg_service_accept.h>
#include <network/ssh/message/tt_ssh_msg_service_request.h>
#include <network/ssh/message/tt_ssh_msg_userauth_banner.h>
#include <network/ssh/message/tt_ssh_msg_userauth_failure.h>
#include <network/ssh/message/tt_ssh_msg_userauth_request.h>
#include <network/ssh/message/tt_ssh_msg_userauth_success.h>
#include <network/ssh/message/tt_ssh_msg_verxchg.h>
#include <network/ssh/message/tt_ssh_parse.h>
#include <network/ssh/message/tt_ssh_render.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __MAX_SSH_MAC_LEN 32
#define __MAX_SSH_PAD_BLOCK 2

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_sshmsg_t *__sshmsg_parse(IN tt_u8_t *payload,
                                   IN tt_u32_t payload_length);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_sshmsg_t *tt_sshmsg_create(IN tt_u32_t msg_id, IN tt_u32_t msg_len,
                              IN tt_sshmsg_itf_t *itf)
{
    tt_sshmsg_t *msg;

    TT_ASSERT(itf != NULL);
    TT_ASSERT(itf->render_prepare != NULL);
    TT_ASSERT(itf->render != NULL);
    TT_ASSERT(itf->parse != NULL);

    msg_len += sizeof(tt_sshmsg_t);
    msg = (tt_sshmsg_t *)tt_malloc(msg_len);
    tt_memset(msg, 0, msg_len);

    if (msg != NULL) {
        msg->msg_id = msg_id;
        msg->msg_private = 0;
        tt_atomic_s32_set(&msg->ref, 1);

        tt_buf_init(&msg->buf, NULL);
        msg->itf = itf;

        // flags are all set to 0

        if ((msg->itf->create != NULL) && !TT_OK(msg->itf->create(msg))) {
            tt_buf_destroy(&msg->buf);
            tt_free(msg);
            return NULL;
        }
    }

    return msg;
}

void __sshmsg_destroy(IN tt_sshmsg_t *msg)
{
    TT_ASSERT(msg != NULL);
    TT_ASSERT(msg->itf != NULL);

    if ((msg->itf != NULL) && (msg->itf->destroy != NULL)) {
        msg->itf->destroy(msg);
    }

    tt_buf_destroy(&msg->buf);
    tt_free(msg);
}

void tt_sshmsg_dump(IN tt_sshmsg_t *msg)
{
    TT_INFO("%s", tt_sshmsg_name(msg->msg_id));
    if (msg->itf->dump != NULL) { msg->itf->dump(msg); }
}

tt_result_t tt_sshmsg_render(IN tt_sshmsg_t *msg, IN tt_u32_t block_len,
                             IN OPT tt_buf_t *buf)
{
    tt_result_t result;
    tt_u32_t len;
    tt_ssh_render_mode_t mode;

    TT_ASSERT(msg != NULL);

#if 0
    if (msg->rendered)
    {
        return TT_SUCCESS;
    }
#else
    TT_ASSERT(!msg->rendered);
#endif

    if (buf == NULL) { buf = &msg->buf; }

    result = msg->itf->render_prepare(msg, &len, &mode);
    if (!TT_OK(result)) {
        TT_ERROR("fail to calc ssh msg length");
        return TT_FAIL;
    }

    TT_ASSERT(TT_SSH_RENDER_MODE_VALID(mode));
    if (mode == TT_SSH_RENDER_MODE_RAW) {
        result = tt_buf_reserve(buf, len);
        if (!TT_OK(result)) {
            TT_ERROR("fail to reserve buf for ssh msg");
            return TT_FAIL;
        }

        result = msg->itf->render(msg, buf);
        if (!TT_OK(result)) {
            TT_ERROR("fail to render ssh msg");
            return TT_FAIL;
        }

        msg->rendered = TT_TRUE;
        return TT_SUCCESS;
    } else if (mode == TT_SSH_RENDER_MODE_BINARY) {
        tt_u8_t padlen;

        /*
         uint32 packet_length
         byte padding_length
         byte[n1] payload; n1 = packet_length - padding_length - 1
         byte[n2] random padding; n2 = padding_length
         byte[m] mac (Message Authentication Code - MAC); m = mac_length
         */

        // pad __MAX_SSH_PAD_BLOCK blocks at most
        padlen = tt_sshmsg_padlen(len, block_len, msg->pad_block,
                                  __MAX_SSH_PAD_BLOCK);

        // also reserve space for mac data
        result = tt_buf_reserve(buf, 5 + len + padlen + __MAX_SSH_MAC_LEN);
        if (!TT_OK(result)) {
            TT_ERROR("fail to reserve buf for ssh msg");
            return TT_FAIL;
        }

        // packet_length
        TT_DO(tt_buf_put_u32_n(buf, 1 + len + padlen));

        // padding_length
        TT_DO(tt_buf_put_u8(buf, padlen));

        // payload
        result = msg->itf->render(msg, buf);
        if (!TT_OK(result)) {
            TT_ERROR("fail to render ssh msg");
            return TT_FAIL;
        }

        // random padding
        TT_DO(tt_buf_put_rand(buf, padlen));

        msg->rendered = TT_TRUE;
        return TT_SUCCESS;
    } else {
        TT_ERROR("invalid render mode[%d]", mode);
        return TT_FAIL;
    }
}

tt_result_t tt_sshmsg_parse(IN tt_buf_t *msg_buf, OUT tt_sshmsg_t **p_msg)
{
    tt_sshmsg_t *msg;
    tt_u8_t *msg_data;
    tt_u32_t msg_len;

    tt_u32_t packet_length, payload_length;
    tt_u8_t padding_length;
    tt_u8_t *payload;

    TT_ASSERT(msg_buf != NULL);
    TT_ASSERT(p_msg != NULL);

    msg_data = TT_BUF_RPOS(msg_buf);

    /*
     RFC4253:

     Each packet is in the following format:
     uint32 packet_length
     byte padding_length
     byte[n1] payload; n1 = packet_length - padding_length - 1
     byte[n2] random padding; n2 = padding_length
     byte[m] mac (Message Authentication Code - MAC); m = mac_length

     packet_length
        The length of the packet in bytes, not including ’mac’ or the
        ’packet_length’ field itself.
     padding_length
        Length of ’random padding’ (bytes).
     payload
        The useful contents of the packet. If compression has been
        negotiated, this field is compressed. Initially, compression
        MUST be "none".
     random padding
        Arbitrary-length padding, such that the total length of
        (packet_length || padding_length || payload || random padding)
        is a multiple of the cipher block size or 8, whichever is
        larger. There MUST be at least four bytes of padding. The
        padding SHOULD consist of random bytes. The maximum amount of
        padding is 255 bytes.
     mac
        Message Authentication Code. If message authentication has
        been negotiated, this field contains the MAC bytes. Initially,
        the MAC algorithm MUST be "none".
     */

    // when key exchange is not finished yet, there is no mac data.
    // otherwise mac verification is done when parsing data in recv_buf,
    // so this function won't see mac data

    // packet_length
    TT_DO(tt_buf_get_u32_h(msg_buf, &packet_length));
    if (packet_length == 0) { return TT_FAIL; }
    if (packet_length > TT_BUF_RLEN(msg_buf)) { return TT_E_BUF_NOBUFS; }

    // padding_length
    TT_DO(tt_buf_get_u8(msg_buf, &padding_length));

    if ((tt_u32_t)padding_length + 1 >= packet_length) {
        TT_ERROR("invalid padding_length[%d] or packet_length[%d]",
                 padding_length, packet_length);
        return TT_FAIL;
    }

    // payload
    payload = TT_BUF_RPOS(msg_buf);
    payload_length = packet_length - 1 - padding_length;

    msg = __sshmsg_parse(payload, payload_length);
    if (msg == NULL) { return TT_FAIL; }

    // random padding
    // ignored, won't fail
    tt_buf_inc_rp(msg_buf, payload_length + padding_length);

    msg_len = (tt_u32_t)(TT_BUF_RPOS(msg_buf) - msg_data);
    if (msg->save) {
        tt_buf_reset_rwp(&msg->buf);
        if (!TT_OK(tt_buf_put(&msg->buf, msg_data, msg_len))) {
            tt_sshmsg_release(msg);
            return TT_FAIL;
        }
    }

    *p_msg = msg;
    return TT_SUCCESS;
}

tt_result_t tt_sshmsg_parse_verxchg(IN tt_buf_t *msg_buf,
                                    OUT tt_sshmsg_t **p_msg)
{
    tt_sshmsg_t *msg;
    tt_u8_t *msg_data;
    tt_u32_t msg_len;
    tt_result_t result;

    msg = tt_sshms_verxchg_create();
    if (msg == NULL) { return TT_FAIL; }

    msg_data = TT_BUF_RPOS(msg_buf);

    // behavior of parsing verxchg is different with other msg:
    // all other msg has a packet_length field, while verxchg, so
    // we must care what the parse() return, which may be TT_E_BUF_NOBUFS
    result = msg->itf->parse(msg, msg_buf);
    if (TT_OK(result)) {
        msg_len = (tt_u32_t)(TT_BUF_RPOS(msg_buf) - msg_data);
        if (msg->save) {
            tt_buf_reset_rwp(&msg->buf);
            if (!TT_OK(tt_buf_put(&msg->buf, msg_data, msg_len))) {
                tt_sshmsg_release(msg);
                return TT_FAIL;
            }
        }

        *p_msg = msg;
        return TT_SUCCESS;
    } else {
        tt_sshmsg_release(msg);
        return result;
    }
}

tt_result_t tt_sshmsg_peek_payload(IN tt_buf_t *msg_buf, OUT tt_u8_t **payload,
                                   OUT tt_u32_t *payload_len)
{
    tt_u32_t rpos, wpos;
    tt_result_t result = TT_FAIL;

    tt_u32_t len;
    tt_u8_t pad_len;

    tt_buf_backup_rwp(msg_buf, &rpos, &wpos);

    TT_DO_G(s_out, tt_buf_get_u32_h(msg_buf, &len));
    TT_DO_G(s_out, tt_buf_get_u8(msg_buf, &pad_len));
    if (len < (tt_u32_t)(pad_len + 1)) {
        // invalid packet
        goto s_out;
    }
    len -= (pad_len + 1);
    if (TT_BUF_RLEN(msg_buf) < len) {
        result = TT_E_BUF_NOBUFS;
        goto s_out;
    }

    *payload = TT_BUF_RPOS(msg_buf);
    *payload_len = len;
    result = TT_SUCCESS;

s_out:

    tt_buf_restore_rwp(msg_buf, &rpos, &wpos);
    return result;
}

const tt_char_t *tt_sshmsg_name(IN tt_u32_t msg_id)
{
    switch (msg_id) {
    // SSH-TRANS
    case TT_SSH_MSGID_DISCONNECT: return "SSH_MSG_DISCONNECT";
    case TT_SSH_MSGID_IGNORE: return "SSH_MSG_IGNORE";
    case TT_SSH_MSGID_UNIMPLEMENTED: return "SSH_MSG_UNIMPLEMENTED";
    case TT_SSH_MSGID_DEBUG: return "SSH_MSG_DEBUG";
    case TT_SSH_MSGID_SERVICE_REQUEST: return "SSH_MSG_SERVICE_REQUEST";
    case TT_SSH_MSGID_SERVICE_ACCEPT: return "SSH_MSG_SERVICE_ACCEPT";
    case TT_SSH_MSGID_KEXINIT: return "SSH_MSG_KEXINIT";
    case TT_SSH_MSGID_KEXDH_INIT: return "SSH_MSG_KEXDH_INIT";
    case TT_SSH_MSGID_KEXDH_REPLY: return "SSH_MSG_KEXDH_REPLY";

    default: return "Unknown";
    }
}

tt_sshmsg_t *__sshmsg_parse(IN tt_u8_t *payload, IN tt_u32_t payload_length)
{
    tt_sshmsg_t *msg = NULL;
    tt_buf_t pbuf;
    tt_u8_t msgid;

    tt_buf_create_nocopy(&pbuf, payload, payload_length, NULL);

    TT_DO_R(NULL, tt_buf_get_u8(&pbuf, &msgid));
    switch (msgid) {
    case TT_SSH_MSGID_KEXINIT: {
        msg = tt_sshms_keyinit_create();
    } break;
    case TT_SSH_MSGID_IGNORE: {
        msg = tt_sshmsg_ignore_create();
    } break;
    case TT_SSH_MSGID_DISCONNECT: {
        msg = tt_sshmsg_disconnect_create();
    } break;
    case TT_SSH_MSGID_KEXDH_INIT: {
        msg = tt_sshmsg_kexdh_init_create();
    } break;
    case TT_SSH_MSGID_NEWKEYS: {
        msg = tt_sshms_newkeys_create();
    } break;
    case TT_SSH_MSGID_SERVICE_REQUEST: {
        msg = tt_sshmsg_servreq_create();
    } break;
    case TT_SSH_MSGID_SERVICE_ACCEPT: {
        msg = tt_sshmsg_servacc_create();
    } break;
    case TT_SSH_MSGID_USERAUTH_REQUEST: {
        msg = tt_sshmsg_uar_create();
    } break;
    case TT_SSH_MSGID_USERAUTH_SUCCESS: {
        msg = tt_sshmsg_uas_create();
    } break;
    case TT_SSH_MSGID_USERAUTH_FAILURE: {
        msg = tt_sshmsg_uaf_create();
    } break;
    case TT_SSH_MSGID_USERAUTH_BANNER: {
        msg = tt_sshmsg_uab_create();
    } break;
    case TT_SSH_MSGID_GLOBAL_REQUEST: {
        msg = tt_sshmsg_glbreq_create();
    } break;
    case TT_SSH_MSGID_REQUEST_SUCCESS: {
        msg = tt_sshmsg_reqsucc_create();
    } break;
    case TT_SSH_MSGID_REQUEST_FAILURE: {
        msg = tt_sshmsg_reqfail_create();
    } break;
    case TT_SSH_MSGID_CHANNEL_OPEN: {
        msg = tt_sshmsg_chopen_create();
    } break;
    case TT_SSH_MSGID_CHANNEL_OPEN_CONFIRMATION: {
        msg = tt_sshmsg_chopc_create();
    } break;
    case TT_SSH_MSGID_CHANNEL_OPEN_FAILURE: {
        msg = tt_sshmsg_chopf_create();
    } break;
    case TT_SSH_MSGID_CHANNEL_WINDOW_ADJUST: {
        msg = tt_sshmsg_chwinadj_create();
    } break;
    case TT_SSH_MSGID_CHANNEL_DATA: {
        msg = tt_sshmsg_chdata_create();
    } break;
    case TT_SSH_MSGID_CHANNEL_CLOSE: {
        msg = tt_sshmsg_chclose_create();
    } break;
    case TT_SSH_MSGID_CHANNEL_EOF: {
        msg = tt_sshmsg_cheof_create();
    } break;
    case TT_SSH_MSGID_CHANNEL_REQUEST: {
        msg = tt_sshmsg_chreq_create();
    } break;
    case TT_SSH_MSGID_CHANNEL_SUCCESS: {
        msg = tt_sshmsg_chsucc_create();
    } break;
    case TT_SSH_MSGID_CHANNEL_FAILURE: {
        msg = tt_sshmsg_chfail_create();
    } break;

    default: {
        TT_ERROR("unsupported ssh msg[%d]", msgid);
        return NULL;
    } break;
    }
    if (msg == NULL) { return NULL; }

    if (!TT_OK(msg->itf->parse(msg, &pbuf))) {
        tt_sshmsg_release(msg);
        return NULL;
    }

    return msg;
}
