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

#include <network/ssh/server/tt_ssh_server_conn.h>

#include <algorithm/tt_buffer_format.h>
#include <memory/tt_memory_alloc.h>
#include <misc/tt_reference_counter.h>
#include <network/ssh/message/tt_ssh_message.h>
#include <network/ssh/message/tt_ssh_msg_channel_close.h>
#include <network/ssh/message/tt_ssh_msg_channel_data.h>
#include <network/ssh/message/tt_ssh_msg_channel_eof.h>
#include <network/ssh/message/tt_ssh_msg_channel_request.h>
#include <network/ssh/message/tt_ssh_msg_channel_window_adjust.h>
#include <network/ssh/server/tt_ssh_server.h>
#include <network/ssh/server/tt_ssh_server_fsm.h>
#include <network/ssh/tt_ssh_channel.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

extern void __sshsvr_destroy(IN tt_sshsvr_t *sshsvr);

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_result_t __sshsvrch_send(IN struct tt_sshchmgr_s *chmgr,
                                   IN struct tt_sshmsg_s *msg);
static tt_result_t __sshsvrch_extra_size(IN struct tt_sshchmgr_s *chmgr,
                                         OUT tt_u32_t *head_mac,
                                         OUT tt_u32_t *block_size);

static void __sshsvrch_on_ch_destroy(IN struct tt_sshchmgr_s *chmgr,
                                     IN struct tt_sshch_s *ch);

static tt_sshchmgr_itf_t tt_s_sshsvr_chmgr_itf = {
    __sshsvrch_send,
    __sshsvrch_extra_size,

    __sshsvrch_on_ch_destroy,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static void __svrconn_on_destroy(IN tt_skt_t *skt, IN void *cb_param);
static void __svrconn_on_accept(IN tt_skt_t *listening_skt,
                                IN tt_skt_t *new_skt,
                                IN tt_skt_aioctx_t *aioctx);

static void __svrconn_on_recv(IN tt_skt_t *skt,
                              IN tt_blob_t *blob_array,
                              IN tt_u32_t blob_num,
                              IN tt_skt_aioctx_t *aioctx,
                              IN tt_u32_t recv_len);
static void __svrconn_on_send(IN tt_skt_t *skt,
                              IN tt_blob_t *blob_array,
                              IN tt_u32_t blob_num,
                              IN tt_skt_aioctx_t *aioctx,
                              IN tt_u32_t send_len);

static tt_result_t __svrconn_parse(IN tt_sshsvrconn_t *svrconn,
                                   OUT tt_sshmsg_t **msg);
static tt_result_t __svrconn_decrypt(IN tt_sshsvrconn_t *svrconn,
                                     OUT tt_buf_t *msg_buf);

static tt_result_t __svrconn_encrypt(IN tt_sshsvrconn_t *svrconn,
                                     OUT tt_buf_t *msg_buf);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_sshsvrconn_t *tt_sshsvrconn_create(IN tt_sshsvr_t *server)
{
    tt_sshsvr_attr_t *attr;
    tt_u32_t size;
    tt_sshsvrconn_t *svrconn;
    tt_result_t result;
    tt_sshctx_attr_t ctx_attr;

    tt_skt_attr_t skt_attr;
    tt_skt_exit_t exit;

    tt_u32_t __done = 0;
#define __SCO_MEM (1 << 0)
#define __SCO_RBUF (1 << 1)
#define __SCO_LINKED (1 << 2)
#define __SCO_CTX (1 << 3)

    TT_ASSERT(server != NULL);
    attr = &server->attr;

    size = sizeof(tt_sshsvrconn_t);
    size += sizeof(tt_sshch_t *) * server->attr.channel_per_conn;
    svrconn = tt_malloc(size);
    if (svrconn == NULL) {
        TT_ERROR("no mem for server connection");
        return NULL;
    }
    __done |= __SCO_MEM;
    tt_memset(svrconn, 0, size);

    // server
    svrconn->server = server;

    // channel manager
    svrconn->chmgr.itf = &tt_s_sshsvr_chmgr_itf;

    // receive buf
    result = tt_buf_create(&svrconn->recv_buf,
                           attr->conn_buf_init_size,
                           &attr->conn_buf_attr);
    if (!TT_OK(result)) {
        TT_ERROR("fail to create sshsvr conn recv buf");
        goto scfail;
    }
    __done |= __SCO_RBUF;

    svrconn->plaintext_len = 0;

    // node
    tt_lnode_init(&svrconn->node);

    // init state
    svrconn->state = TT_SSHSVRST_VERXCHG;

    // context
    ctx_attr.is_server = TT_TRUE;

    result = tt_sshctx_create(&svrconn->ctx, &ctx_attr);
    if (!TT_OK(result)) {
        TT_ERROR("fail to create sshsvr conn context");
        goto scfail;
    }
    __done |= __SCO_CTX;

    // channels
    svrconn->ch = TT_PTR_INC(tt_sshch_t *, svrconn, sizeof(tt_sshsvrconn_t));
    svrconn->ch_num = server->attr.channel_per_conn;
    // channels have already be set to null

    svrconn->flag = 0;

    // all mesage status bits are already set to 0

    svrconn->last_state = TT_SSHSVRST_NUM;
    svrconn->by_event = TT_SSHSVREV_NUM;

    // start accept
    tt_skt_attr_default(&skt_attr);
    tt_skt_attr_set_nodelay(&skt_attr, TT_TRUE);

    exit.on_destroy = __svrconn_on_destroy;
    exit.cb_param = NULL;

    result = tt_skt_accept_async(&server->skt,
                                 &svrconn->skt,
                                 &skt_attr,
                                 &exit,
                                 __svrconn_on_accept,
                                 NULL);
    if (!TT_OK(result)) {
        TT_ERROR("fail to accept ssh server connection");

        // nothing to be clean when async accept failed
        goto scfail;
    }

    // add to server conn list
    tt_list_addtail(&server->conn_list, &svrconn->node);
    __done |= __SCO_LINKED;

    return svrconn;

scfail:

    if (__done & __SCO_LINKED) {
        tt_list_remove(&svrconn->node);
    }

    if (__done & __SCO_CTX) {
        tt_sshctx_destroy(&svrconn->ctx);
    }

    if (__done & __SCO_RBUF) {
        tt_buf_destroy(&svrconn->recv_buf);
    }

    if (__done & __SCO_MEM) {
        tt_free(svrconn);
    }

    return NULL;
}

void tt_sshsvrconn_destroy(IN tt_sshsvrconn_t *svrconn, IN tt_bool_t immediate)
{
    TT_ASSERT(svrconn != NULL);

    if (immediate) {
        tt_u32_t i;

        tt_async_skt_destroy(&svrconn->skt, TT_TRUE);
        tt_buf_destroy(&svrconn->recv_buf);
        tt_list_remove(&svrconn->node);

        tt_sshctx_destroy(&svrconn->ctx);

        for (i = 0; i < svrconn->ch_num; ++i) {
            if (svrconn->ch[i] != NULL) {
                tt_sshch_destroy(svrconn->ch[i]);
            }
        }
    } else {
        tt_async_skt_destroy(&svrconn->skt, TT_FALSE);
    }
}

void tt_sshsvrconn_shutdown(IN tt_sshsvrconn_t *svrconn)
{
    tt_u32_t i;

    TT_ASSERT(svrconn != NULL);

    for (i = 0; i < svrconn->ch_num; ++i) {
        if (svrconn->ch[i] != NULL) {
            tt_sshch_shutdown(svrconn->ch[i]);
        }
    }

    if (!TT_OK(tt_async_skt_shutdown(&svrconn->skt, TT_SKT_SHUTDOWN_RDWR))) {
        TT_FATAL("can not shutdown ssh connection");
    }
}

tt_result_t tt_sshsvrconn_recv(IN tt_sshsvrconn_t *svrconn)
{
    tt_buf_t *recv_buf;
    tt_blob_t recv_blob;

    TT_ASSERT(svrconn != NULL);
    recv_buf = &svrconn->recv_buf;

    if (TT_BUF_WLEN(recv_buf) == 0) {
        // try expanding recv buf??
        return TT_END;
    }

    tt_buf_getptr_wpblob(recv_buf, &recv_blob);
    if (!TT_OK(tt_skt_recv_async(&svrconn->skt,
                                 &recv_blob,
                                 1,
                                 __svrconn_on_recv,
                                 NULL))) {
        TT_ERROR("fail to start ssh recv");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_sshsvrconn_send(IN tt_sshsvrconn_t *svrconn,
                               IN TO tt_sshmsg_t *sshmsg)
{
    tt_blob_t data_blob;

    TT_ASSERT(svrconn != NULL);
    TT_ASSERT(sshmsg != NULL);

    if (!sshmsg->rendered &&
        !TT_OK(
            tt_sshmsg_render(sshmsg, svrconn->ctx.encrypt.block_len, NULL))) {
        tt_sshmsg_release(sshmsg);
        return TT_FAIL;
    }

    if (TT_SSHSVR_CIPHER_STATE(svrconn->state) &&
        !TT_OK(__svrconn_encrypt(svrconn, &sshmsg->buf))) {
        tt_sshmsg_release(sshmsg);
        return TT_FAIL;
    }

    tt_buf_getptr_rpblob(&sshmsg->buf, &data_blob);
    if (!TT_OK(tt_skt_send_async(&svrconn->skt,
                                 &data_blob,
                                 1,
                                 __svrconn_on_send,
                                 NULL))) {
        TT_FATAL("ssh send failed");

        tt_sshmsg_release(sshmsg);
        return TT_FAIL;
    }

    tt_sshctx_sign_inc_seq(&svrconn->ctx);

    return TT_SUCCESS;
}

// ========================================
// ssh server channel
// ========================================

struct tt_sshch_s *tt_sshsvrconn_ch_create(IN tt_sshsvrconn_t *svrconn,
                                           IN tt_ssh_chtype_t type)
{
    tt_u32_t i;
    tt_sshch_t *ch;

    for (i = 0; i < svrconn->ch_num; ++i) {
        if (svrconn->ch[i] == NULL) {
            break;
        }
    }
    if (i == svrconn->ch_num) {
        TT_ERROR("no free ssh channel");
        return NULL;
    }

    ch = tt_sshch_create(type, i, &svrconn->server->sshch_cb, &svrconn->chmgr);
    if (ch == NULL) {
        return NULL;
    }

    svrconn->ch[i] = ch;
    return ch;
}

void tt_sshsvrconn_ch_pkt_handler(IN tt_sshsvrconn_t *svrconn,
                                  IN tt_sshmsg_t *msg,
                                  OUT tt_sshsvr_action_t *svract)
{
    tt_u32_t channel;
    tt_sshch_t *ch;

    switch (msg->msg_id) {
        case TT_SSH_MSGID_CHANNEL_DATA: {
            channel = TT_SSHMSG_CAST(msg, tt_sshmsg_chdata_t)->rcv_chnum;
        } break;
        case TT_SSH_MSGID_CHANNEL_REQUEST: {
            channel = TT_SSHMSG_CAST(msg, tt_sshmsg_chreq_t)->rcv_chnum;
        } break;
        case TT_SSH_MSGID_CHANNEL_WINDOW_ADJUST: {
            channel = TT_SSHMSG_CAST(msg, tt_sshmsg_chwinadj_t)->rcv_chnum;
        } break;
        case TT_SSH_MSGID_CHANNEL_EOF: {
            channel = TT_SSHMSG_CAST(msg, tt_sshmsg_cheof_t)->rcv_chnum;
        } break;
        case TT_SSH_MSGID_CHANNEL_CLOSE: {
            channel = TT_SSHMSG_CAST(msg, tt_sshmsg_chclose_t)->rcv_chnum;
        } break;

        default: {
            TT_ERROR("not a channel packet");
            return;
        } break;
    }

    ch = TT_COND(channel < svrconn->ch_num, svrconn->ch[channel], NULL);
    if (ch == NULL) {
        TT_ERROR("channel[%d] does not exist", ch);
        svract->new_event = TT_SSHSVREV_DISCONNECT;
        return;
    }

    tt_sshch_pkt_handler(ch, msg, svract);
}

void __svrconn_on_destroy(IN tt_skt_t *skt, IN void *cb_param)
{
    tt_sshsvrconn_t *svrconn;
    tt_sshsvr_t *server;
    tt_u32_t i;

    svrconn = TT_CONTAINER(skt, tt_sshsvrconn_t, skt);

    tt_buf_destroy(&svrconn->recv_buf);
    tt_list_remove(&svrconn->node);

    tt_sshctx_destroy(&svrconn->ctx);

    for (i = 0; i < svrconn->ch_num; ++i) {
        if (svrconn->ch[i] != NULL) {
            tt_sshch_destroy(svrconn->ch[i]);
        }
    }

    // check if server need be destroyed
    TT_ASSERT(svrconn->server != NULL);
    server = svrconn->server;

    tt_free(svrconn);

    if (server->skt_destroyed && tt_list_empty(&server->conn_list)) {
        __sshsvr_destroy(server);
    }
}

void __svrconn_on_accept(IN tt_skt_t *listening_skt,
                         IN tt_skt_t *new_skt,
                         IN tt_skt_aioctx_t *aioctx)
{
    tt_sshsvrconn_t *svrconn;
    tt_sshsvr_t *server;

    if (!TT_OK(aioctx->result)) {
        return;
    }

    svrconn = TT_CONTAINER(new_skt, tt_sshsvrconn_t, skt);

    TT_ASSERT(svrconn->server != NULL);
    server = svrconn->server;
    TT_ASSERT(&server->skt == listening_skt);

    // check if need deliver a new conn
    if ((server->attr.max_conn_num == 0) ||
        (tt_list_count(&server->conn_list) < server->attr.max_conn_num)) {
        if (tt_sshsvrconn_create(server) == NULL) {
            TT_WARN("fail to create a new ssh server connection");
        }
    }

    // start receiving data
    if (!TT_OK(tt_sshsvrconn_recv(svrconn))) {
        tt_sshsvrconn_destroy(svrconn, TT_FALSE);
        return;
    }

    // start negotiating
    tt_sshsvr_fsm(svrconn, TT_SSHSVREV_VERXCHG, NULL);
}

void __svrconn_on_recv(IN tt_skt_t *skt,
                       IN tt_blob_t *blob_array,
                       IN tt_u32_t blob_num,
                       IN tt_skt_aioctx_t *aioctx,
                       IN tt_u32_t recv_len)
{
    tt_sshsvrconn_t *svrconn = TT_CONTAINER(skt, tt_sshsvrconn_t, skt);
    tt_buf_t *recv_buf = &svrconn->recv_buf;
    tt_sshmsg_t *msg = NULL;
    tt_result_t result;

    if (!TT_OK(aioctx->result) || (recv_len == 0)) {
        return;
    }
    tt_buf_inc_wp(recv_buf, recv_len);

    while (1) {
        result = __svrconn_parse(svrconn, &msg);
        if (!TT_OK(result)) {
            break;
        }
        tt_sshctx_verify_inc_seq(&svrconn->ctx);

        result = tt_sshsvr_fsm(svrconn, TT_SSHSVREV_PACKET, msg);
        tt_sshmsg_release(msg);
        msg = NULL;
        if (!TT_OK(result)) {
            break;
        }

        if (TT_BUF_RLEN(recv_buf) == 0) {
            result = TT_BUFFER_INCOMPLETE;
            break;
        }
    }

    TT_ASSERT(!TT_OK(result));
    if (result == TT_BUFFER_INCOMPLETE) {
        if ((TT_BUF_RLEN(recv_buf) == 0) ||
            (TT_BUF_REFINABLE(recv_buf) > 1000)) {
            tt_buf_refine(recv_buf);
        }

        if (!TT_OK(tt_sshsvrconn_recv(svrconn))) {
            tt_sshsvrconn_destroy(svrconn, TT_FALSE);
        }
    } else if (result == TT_END) {
        // tt_sshctx_sign_inc_seq(&svrconn->ctx);

        tt_sshsvrconn_shutdown(svrconn);
    } else {
        tt_sshsvrconn_destroy(svrconn, TT_FALSE);
    }
}

void __svrconn_on_send(IN tt_skt_t *skt,
                       IN tt_blob_t *blob_array,
                       IN tt_u32_t blob_num,
                       IN tt_skt_aioctx_t *aioctx,
                       IN tt_u32_t send_len)
{
    // tt_sshsvrconn_t *svrconn = TT_CONTAINER(skt, tt_sshsvrconn_t, skt);
    tt_sshmsg_t *msg = (tt_sshmsg_t *)aioctx->cb_param;

    if (msg != NULL) {
        if (TT_OK(aioctx->result) && (send_len != blob_array[0].len)) {
            TT_FATAL("partial ssh msg data are sent, unpredictable");
        }

        tt_sshmsg_release(msg);
    }
}

tt_result_t __svrconn_parse(IN tt_sshsvrconn_t *svrconn, OUT tt_sshmsg_t **msg)
{
    if (TT_SSHSVR_CIPHER_STATE(svrconn->state)) {
        tt_buf_t msg_buf;
        tt_result_t result;

        result = __svrconn_decrypt(svrconn, &msg_buf);
        if (TT_OK(result)) {
            // note __svrconn_decrypt() has already updated recv_buf's pos
            result = tt_sshmsg_parse(&msg_buf, msg);
            tt_buf_destroy(&msg_buf);
            if (TT_OK(result)) {
                return TT_SUCCESS;
            } else {
                // parsing msg_buf returned by __svrconn_decrypt() can only
                // be either TT_SUCCESS or TT_FAIL
                return TT_FAIL;
            }
        } else if (result == TT_BUFFER_INCOMPLETE) {
            return TT_BUFFER_INCOMPLETE;
        } else {
            return TT_FAIL;
        }
    } else {
        tt_buf_t *recv_buf = &svrconn->recv_buf;
        tt_u32_t rd_pos, wr_pos;
        tt_result_t result;

        tt_buf_backup_rwp(recv_buf, &rd_pos, &wr_pos);
        if (svrconn->state == TT_SSHSVRST_VERXCHG) {
            result = tt_sshmsg_parse_verxchg(recv_buf, msg);
        } else {
            result = tt_sshmsg_parse(recv_buf, msg);
        }
        if (TT_OK(result)) {
            // recv_buf's pos should have been correctly updated when
            // parsing api returned TT_SUCCESS
            return TT_SUCCESS;
        } else if (result == TT_BUFFER_INCOMPLETE) {
            tt_buf_restore_rwp(recv_buf, &rd_pos, &wr_pos);
            return TT_BUFFER_INCOMPLETE;
        } else {
            return TT_FAIL;
        }
    }
}

// return:
//  - TT_SUCCESS, if decryption and verification is done successfully
//  - TT_BUFFER_INCOMPLETE, if data in buf is not complete
//  - TT_FAIL, error occurred
tt_result_t __svrconn_decrypt(IN tt_sshsvrconn_t *svrconn,
                              OUT tt_buf_t *msg_buf)
{
    tt_buf_t *recv_buf = &svrconn->recv_buf;
    tt_sshctx_t *sshctx = &svrconn->ctx;
    tt_u32_t block_len = sshctx->decrypt.block_len;
    tt_u32_t mac_len = sshctx->verify.mac_len;
    tt_u32_t pkt_len, data_len, dec_len;
    tt_result_t result;

    // decrypt the first block
    if (svrconn->plaintext_len == 0) {
        if ((recv_buf->rd_pos + block_len) > recv_buf->wr_pos) {
            // less than 1 block
            return TT_BUFFER_INCOMPLETE;
        }

        if (!TT_OK(
                tt_sshctx_decrypt(sshctx, TT_BUF_RPOS(recv_buf), block_len))) {
            return TT_FAIL;
        }
        svrconn->plaintext_len += block_len;
    }

    // decrypt
    result = tt_buf_peek_u32_h(recv_buf, &pkt_len);
    TT_ASSERT_SSH(TT_OK(result));
    pkt_len += 4;

    data_len = TT_BUF_RLEN(recv_buf);

    dec_len = TT_MIN(pkt_len, data_len);
    dec_len /= block_len; // may use shift
    dec_len *= block_len;
    TT_ASSERT_SSH(dec_len >= svrconn->plaintext_len);
    dec_len -= svrconn->plaintext_len;
    if (dec_len > 0) {
        result =
            tt_sshctx_decrypt(sshctx,
                              TT_BUF_RPOS(recv_buf) + svrconn->plaintext_len,
                              dec_len);
        if (!TT_OK(result)) {
            return TT_FAIL;
        }
        svrconn->plaintext_len += dec_len;
    }

    if (svrconn->plaintext_len < pkt_len) {
        return TT_BUFFER_INCOMPLETE;
    }
    TT_ASSERT_SSH(svrconn->plaintext_len == pkt_len);

    // mac
    if ((svrconn->plaintext_len + mac_len) > data_len) {
        return TT_BUFFER_INCOMPLETE;
    }

    result = tt_sshctx_verify(sshctx,
                              TT_BUF_RPOS(recv_buf),
                              svrconn->plaintext_len,
                              TT_BUF_RPOS(recv_buf) + svrconn->plaintext_len,
                              mac_len);
    if (!TT_OK(result)) {
        TT_ERROR("ssh mac verification failed");
        return TT_FAIL;
    }

    // all done
    tt_buf_create_nocopy(msg_buf,
                         TT_BUF_RPOS(recv_buf),
                         svrconn->plaintext_len,
                         NULL);

    tt_buf_inc_rp(recv_buf, svrconn->plaintext_len + mac_len);
    if ((TT_BUF_RLEN(recv_buf) == 0) || (TT_BUF_REFINABLE(recv_buf) > 1000)) {
        tt_buf_refine(recv_buf);
    }

    svrconn->plaintext_len = 0;

    return TT_SUCCESS;
}

tt_result_t __svrconn_encrypt(IN tt_sshsvrconn_t *svrconn,
                              OUT tt_buf_t *msg_buf)
{
    tt_sshctx_t *sshctx = &svrconn->ctx;
    tt_u32_t mac_len = sshctx->sign.mac_len;

    // sign unencrypted data
    if (!TT_OK(tt_buf_reserve(msg_buf, mac_len)) ||
        !TT_OK(tt_sshctx_sign(sshctx,
                              TT_BUF_RPOS(msg_buf),
                              TT_BUF_RLEN(msg_buf),
                              TT_BUF_WPOS(msg_buf),
                              mac_len))) {
        return TT_FAIL;
    }

    // encrypt data
    if (!TT_OK(tt_sshctx_encrypt(sshctx,
                                 TT_BUF_RPOS(msg_buf),
                                 TT_BUF_RLEN(msg_buf)))) {
        return TT_FAIL;
    }

    // append signature
    tt_buf_inc_wp(msg_buf, mac_len);

    return TT_SUCCESS;
}

tt_result_t __sshsvrch_send(IN struct tt_sshchmgr_s *chmgr,
                            IN struct tt_sshmsg_s *msg)
{
    return tt_sshsvrconn_send(TT_CONTAINER(chmgr, tt_sshsvrconn_t, chmgr), msg);
}

tt_result_t __sshsvrch_extra_size(IN struct tt_sshchmgr_s *chmgr,
                                  OUT tt_u32_t *head_mac,
                                  OUT tt_u32_t *block_size)
{
    tt_sshsvrconn_t *svrconn = TT_CONTAINER(chmgr, tt_sshsvrconn_t, chmgr);
    tt_sshctx_t *sshctx = &svrconn->ctx;

    if (!TT_SSH_ENC_ALG_VALID(sshctx->enc_alg) ||
        !TT_SSH_MAC_ALG_VALID(sshctx->sign.alg)) {
        TT_ERROR("ssh enc or mac is not negotiated");
        return TT_FAIL;
    }

    *head_mac = 5 + sshctx->sign.mac_len;
    *block_size = sshctx->encrypt.block_len;
    return TT_SUCCESS;
}

void __sshsvrch_on_ch_destroy(IN struct tt_sshchmgr_s *chmgr,
                              IN struct tt_sshch_s *ch)
{
    tt_sshsvrconn_t *svrconn = TT_CONTAINER(chmgr, tt_sshsvrconn_t, chmgr);

    TT_ASSERT(ch == svrconn->ch[ch->chnum]);
    svrconn->ch[ch->chnum] = NULL;
}
