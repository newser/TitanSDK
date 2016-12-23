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

#include <tt_ssl_aio_native.h>

#include <event/tt_event_center.h>
#include <io/tt_socket_aio.h>
#include <memory/tt_memory_alloc.h>
#include <network/ssl/tt_ssl.h>
#include <network/ssl/tt_ssl_aio.h>

#include <tt_socket_aio_native.h>
#include <tt_ssl_context_native.h>
#include <tt_wchar.h>

#ifdef TT_PLATFORM_SSL_ENABLE

#include <schannel.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define TT_ASSERT_SSL TT_ASSERT

#if 0
#define TT_SSL_DETAIL(...) TT_DEBUG(__VA_ARGS__)
#else
#define TT_SSL_DETAIL(...)
#endif

#define __SSL_BUF_SIZE (300) // test => 1024
//#define __SSL_BUF_SIZE (3900) // => 4096
#define __SSL_BUF_MAX_SIZE (16 * 1024) // => 20K
// generally a tls connection consumes 8k extra memory and 10000 tls
// connections thus use 800M memory. if another endian always sends
// large tls record, the memory consumption increases to 2.4g. method
// to reduce memory pressure includes:
//  - limit __SSL_BUF_MAX_SIZE, this may lead to fail to receive
//    tls data
//  - limit tls connection number, this reduces concurrency but it's
//    the most feasible solution
//  - by implementation, free the sec buf when there is no data to
//    be transferred and alloc buf when required, this increases code
//    complexity

#define __SEC_OK(sec_st) ((sec_st) >= 0)

// no official doc specified invalid value of CtxtHandle
#define __HCTX_INIT(hctx)                                                      \
    do {                                                                       \
        tt_memcpy((hctx), &__s_hctx_invalid, sizeof(CtxtHandle));              \
    } while (0)
#define __HCTX_VALID(hctx)                                                     \
    (tt_memcmp((hctx), &__s_hctx_invalid, sizeof(CtxtHandle)) != 0)
#define __HCTX_SET(hctx, new_hctx)                                             \
    do {                                                                       \
        tt_memcpy((hctx), (new_hctx), sizeof(CtxtHandle));                     \
    } while (0)

// sec buffer helper
#define __SB_INIT(sb, len, type, addr)                                         \
    do {                                                                       \
        (sb)->cbBuffer = (len);                                                \
        (sb)->BufferType = (type);                                             \
        (sb)->pvBuffer = (addr);                                               \
    } while (0)
#define __SBD_INIT(sbd, num, p_buf)                                            \
    do {                                                                       \
        (sbd)->ulVersion = SECBUFFER_VERSION;                                  \
        (sbd)->cBuffers = (num);                                               \
        (sbd)->pBuffers = (p_buf);                                             \
    } while (0)

#define __SSL_IN_CIPHER_ADDR(sys_ssl)                                          \
    TT_PTR_INC(tt_u8_t, (sys_ssl)->input.addr, (sys_ssl)->input_plain_len)
#define __SSL_IN_CIPHER_LEN(sys_ssl)                                           \
    ((sys_ssl)->input_pos - (sys_ssl)->input_plain_len)
#define __SSL_IN_ADDR(sys_ssl)                                                 \
    TT_PTR_INC(tt_u8_t, (sys_ssl)->input.addr, (sys_ssl)->input_pos)
#define __SSL_IN_LEN(sys_ssl) ((sys_ssl)->input.len - (sys_ssl)->input_pos)

#define __SSL_OUT_ADDR(sys_ssl)                                                \
    TT_PTR_INC(tt_u8_t, (sys_ssl)->output.addr, (sys_ssl)->output_pos)
#define __SSL_OUT_LEN(sys_ssl)                                                 \
    ((sys_ssl)->output_data_len - (sys_ssl)->output_pos)

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

enum
{
    TT_SSL_RANGE_AIO,
};

enum
{
    // ========================================
    // ssl aio
    // ========================================

    EV_SSLAIO_START =
        TT_EV_MKID_SUB(TT_EV_RANGE_INTERNAL_SSL, TT_SSL_RANGE_AIO, 0),

    EV_SSL_CONNECT_WR,
    EV_SSL_CONNECT_RD,

    EV_SSL_ACCEPT_WR,
    EV_SSL_ACCEPT_RD,

    EV_SSL_SEND,
    EV_SSL_RECV,

    EV_SSLAIO_END,

};

typedef struct
{
    WSAOVERLAPPED wov;

    tt_ssl_t *listening_ssl;
    tt_ssl_t *new_ssl;
    tt_ssl_attr_t new_ssl_attr;
    tt_ssl_exit_t new_ssl_exit;

    tt_ssl_on_accept_t on_accept;
    tt_result_t result;
    void *cb_param;
} __ssl_accept_t;

typedef struct
{
    WSAOVERLAPPED wov;

    tt_ssl_t *ssl;
    tt_sktaddr_t remote_addr;

    tt_ssl_on_connect_t on_connect;
    tt_result_t result;
    void *cb_param;
} __ssl_connect_t;

typedef struct
{
    tt_ssl_t *ssl;

    tt_ssl_on_send_t on_send;
    void *cb_param;
} __ssl_send_t;

typedef struct
{
    tt_ssl_t *ssl;

    tt_ssl_on_recv_t on_recv;
    void *cb_param;
} __ssl_recv_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

extern void __cert_store_show(IN HCERTSTORE cert_store,
                              IN const tt_char_t *prefix);

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static CtxtHandle __s_hctx_invalid = {0};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __ssl_create_client(IN tt_ssl_t *ssl);
static tt_result_t __ssl_create_server(IN tt_ssl_t *ssl);
static void __sys_ssl_init(IN tt_ssl_ntv_t *sys_ssl);
static void __sys_ssl_destroy(IN tt_ssl_ntv_t *sys_ssl);

static void __ssl_on_destroy(IN tt_skt_t *skt, IN void *cb_param);
// static
tt_result_t __ssl_destroy_aio(IN tt_ev_t *ev);

// static
tt_result_t __ssl_close_notify(IN struct tt_ssl_s *ssl);

// accept
static void __ssl_on_accept(IN tt_skt_t *listening_skt,
                            IN tt_skt_t *new_skt,
                            IN tt_skt_aioctx_t *aioctx);
static void __ssl_accept_cb(IN __ssl_accept_t *aio);

// connect
static void __ssl_on_connect(IN tt_skt_t *skt,
                             IN tt_sktaddr_t *remote_addr,
                             IN tt_skt_aioctx_t *aioctx);
static void __ssl_connect_cb(IN __ssl_connect_t *aio);

// send
static void __ssl_on_send(IN tt_skt_t *skt,
                          IN tt_blob_t *blob,
                          IN tt_u32_t blob_num,
                          IN tt_skt_aioctx_t *aioctx,
                          IN tt_u32_t send_len);

// recv
static void __ssl_on_recv(IN tt_skt_t *skt,
                          IN tt_blob_t *blob,
                          IN tt_u32_t blob_num,
                          IN tt_skt_aioctx_t *aioctx,
                          IN tt_u32_t recv_len);

static tt_result_t __ssl_verify(IN tt_ssl_t *ssl);
static tt_result_t __ssl_do_verify(IN tt_ssl_t *ssl);
static tt_result_t __ssl_do_verify_leaf(IN CERT_CONTEXT *cert,
                                        IN tt_ssl_verify_t *ssl_verify);

// sec buf
static void __ssl_input_init(IN tt_ssl_ntv_t *sys_ssl);
static void __ssl_input_update(IN tt_ssl_ntv_t *sys_ssl, IN tt_u32_t io_bytes);
static tt_result_t __ssl_input_expand(IN tt_ssl_ntv_t *sys_ssl);
static tt_result_t __ssl_input_resize(IN tt_ssl_ntv_t *sys_ssl,
                                      IN tt_u32_t size);
static tt_bool_t __ssl_input_keep_extra(IN tt_ssl_ntv_t *sys_ssl,
                                        IN SecBufferDesc *output_sbd);
static void __ssl_input_fix(IN tt_ssl_ntv_t *sys_ssl);

static void __ssl_output_init(IN tt_ssl_ntv_t *sys_ssl);
static tt_bool_t __ssl_output_update(IN tt_ssl_ntv_t *sys_ssl,
                                     IN tt_u32_t io_bytes);
static tt_result_t __ssl_output_expand(IN tt_ssl_ntv_t *sys_ssl);
static tt_result_t __ssl_output_resize(IN tt_ssl_ntv_t *sys_ssl,
                                       IN tt_u32_t size);

// handshake
static tt_result_t __ssl_handshake_read(IN tt_ssl_t *ssl,
                                        IN WSAOVERLAPPED *wov);
static tt_result_t __ssl_handshake_write(IN tt_ssl_t *ssl,
                                         IN WSAOVERLAPPED *wov);

// static
tt_result_t __ssl_handshake(IN tt_ssl_t *ssl,
                            IN tt_ev_t *ev,
                            IN DWORD NumberOfBytes,
                            IN tt_result_t iocp_result);
static tt_result_t __ssl_client_on_read(IN tt_ssl_t *ssl,
                                        IN __ssl_connect_t *aio,
                                        IN DWORD NumberOfBytes,
                                        IN tt_result_t iocp_result);
static tt_result_t __ssl_client_on_write(IN tt_ssl_t *ssl,
                                         IN __ssl_connect_t *aio,
                                         IN DWORD NumberOfBytes,
                                         IN tt_result_t iocp_result);
static tt_result_t __ssl_server_on_read(IN tt_ssl_t *ssl,
                                        IN __ssl_accept_t *aio,
                                        IN DWORD NumberOfBytes,
                                        IN tt_result_t iocp_result);
static tt_result_t __ssl_server_on_write(IN tt_ssl_t *ssl,
                                         IN __ssl_accept_t *aio,
                                         IN DWORD NumberOfBytes,
                                         IN tt_result_t iocp_result);
static tt_result_t __ssl_client_handshake(IN tt_ssl_t *ssl,
                                          IN __ssl_connect_t *aio,
                                          OUT tt_bool_t *to_read);
static tt_result_t __ssl_server_handshake(IN tt_ssl_t *ssl,
                                          IN __ssl_accept_t *aio,
                                          OUT tt_bool_t *to_read);

// static
tt_result_t __ssl_encrypt(IN tt_ssl_t *ssl,
                          IN tt_u8_t *data,
                          IN OUT tt_u32_t *data_len);
// static
tt_result_t __ssl_decrypt(IN tt_ssl_t *ssl,
                          IN tt_u32_t last_io_bytes,
                          IN tt_ssl_on_decrypt_t on_decrypt,
                          IN void *cb_param);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_async_ssl_create_ntv(IN struct tt_ssl_s *ssl,
                                    IN TO tt_skt_t *skt,
                                    IN tt_sslctx_t *sslctx,
                                    IN OPT tt_ssl_attr_t *attr,
                                    IN tt_ssl_exit_t *exit)
{
    tt_u32_t skt_role;
    tt_ssl_role_t ssl_role;

    TT_ASSERT(ssl != NULL);
    TT_ASSERT(skt != NULL);
    TT_ASSERT(skt->sys_socket.ssl == NULL);
    TT_ASSERT(sslctx != NULL);
    TT_ASSERT(exit != NULL);
    TT_ASSERT(exit->on_destroy != NULL);

    // tt ssl
    skt_role = skt->sys_socket.role;
    ssl_role = sslctx->sys_ctx.role;
    if (skt_role == TT_SKT_ROLE_TCP_CONNECT) {
        TT_ASSERT_SSL(ssl_role == TT_SSL_ROLE_CLIENT);
    } else {
        TT_ASSERT_SSL((skt_role == TT_SKT_ROLE_TCP_LISTEN) ||
                      (skt_role == TT_SKT_ROLE_TCP_ACCEPT));
        TT_ASSERT_SSL(ssl_role == TT_SSL_ROLE_SERVER);
    }
    __sys_ssl_init(&ssl->sys_ssl);

    ssl->skt = skt;
    ssl->sslctx = sslctx;

    tt_memcpy(&ssl->exit, exit, sizeof(tt_ssl_exit_t));

    if (attr != NULL) {
        tt_memcpy(&ssl->attr, attr, sizeof(tt_ssl_attr_t));
    } else {
        tt_ssl_attr_default(&ssl->attr);
    }

    ssl->session_mark = 0;

    // be sure skt is not doing io
    skt->sys_socket.on_destroy = __ssl_on_destroy;
    skt->sys_socket.on_destroy_param = ssl;
    skt->sys_socket.ssl = ssl;

    return TT_SUCCESS;
}

void tt_async_ssl_destroy_ntv(IN struct tt_ssl_s *ssl, IN tt_bool_t immediate)
{
    TT_ASSERT(ssl != NULL);

    tt_async_skt_destroy(ssl->skt, immediate);
}

tt_result_t tt_async_ssl_shutdown_ntv(IN tt_ssl_t *ssl, IN tt_u32_t mode)
{
    tt_result_t result = TT_FAIL;

    if (mode & TT_SSL_SHUTDOWN_RD) {
        if (tt_async_skt_shutdown(ssl->skt, TT_SKT_SHUTDOWN_RD) ==
            TT_PROCEEDING) {
            result = TT_PROCEEDING;
        }
    }

    if (mode & TT_SSL_SHUTDOWN_WR) {
        if (tt_async_skt_shutdown(ssl->skt, TT_SKT_SHUTDOWN_WR) ==
            TT_PROCEEDING) {
            result = TT_PROCEEDING;
        }
    }

    return result;
}

void tt_ssl_attr_default_ntv(IN tt_ssl_attr_t *attr)
{
    tt_memset(attr, 0, sizeof(tt_ssl_attr_t));
}

tt_result_t tt_ssl_accept_async_ntv(IN struct tt_ssl_s *listening_ssl,
                                    IN struct tt_ssl_s *new_ssl,
                                    IN TO tt_skt_t *new_skt,
                                    IN OPT tt_skt_attr_t *new_skt_attr,
                                    IN OPT tt_ssl_attr_t *new_ssl_attr,
                                    IN tt_ssl_exit_t *new_ssl_exit,
                                    IN tt_ssl_on_accept_t on_accept,
                                    IN OPT void *cb_param)
{
    tt_ev_t *ev;
    __ssl_accept_t *aio;
    tt_skt_exit_t skt_exit;

    // accepted ssl socket is initially to read
    ev = tt_thread_ev_create(EV_SSL_ACCEPT_RD, sizeof(__ssl_accept_t), NULL);
    if (ev == NULL) {
        TT_ERROR("fail to allocate ssl accept");
        return TT_FAIL;
    }
    aio = TT_EV_DATA(ev, __ssl_accept_t);

    tt_memset(&aio->wov, 0, sizeof(WSAOVERLAPPED));
    aio->listening_ssl = listening_ssl;
    tt_memset(new_ssl, 0, sizeof(tt_ssl_t));
    aio->new_ssl = new_ssl;

    if (new_ssl_attr != NULL) {
        tt_memcpy(&aio->new_ssl_attr, new_ssl_attr, sizeof(tt_ssl_attr_t));
    } else {
        tt_ssl_attr_default(&aio->new_ssl_attr);
    }

    tt_memcpy(&aio->new_ssl_exit, new_ssl_exit, sizeof(tt_ssl_exit_t));

    aio->on_accept = on_accept;
    aio->result = TT_PROCEEDING;
    aio->cb_param = cb_param;

    // new_ssl can not be created here as the new_skt is
    // actually created in tt_skt_accept_async()

    skt_exit.on_destroy = __ssl_on_destroy;
    skt_exit.cb_param = new_ssl;

    if (!TT_OK(tt_skt_accept_async(listening_ssl->skt,
                                   new_skt,
                                   new_skt_attr,
                                   &skt_exit,
                                   __ssl_on_accept,
                                   ev))) {
        tt_ev_destroy(ev);
        return TT_FAIL;
    }
    return TT_SUCCESS;
}

tt_result_t tt_ssl_connect_async_ntv(IN struct tt_ssl_s *ssl,
                                     IN tt_sktaddr_t *remote_addr,
                                     IN tt_ssl_on_connect_t on_connect,
                                     IN OPT void *cb_param)
{
    tt_ev_t *ev;
    __ssl_connect_t *aio;

    TT_ASSERT(ssl != NULL);
    TT_ASSERT(remote_addr != NULL);
    TT_ASSERT(on_connect != NULL);

    // initially client is to write data
    ev = tt_thread_ev_create(EV_SSL_CONNECT_WR, sizeof(__ssl_connect_t), NULL);
    if (ev == NULL) {
        TT_ERROR("fail to allocate ssl connect");
        return TT_FAIL;
    }
    aio = TT_EV_DATA(ev, __ssl_connect_t);

    tt_memset(&aio->wov, 0, sizeof(WSAOVERLAPPED));
    aio->ssl = ssl;
    tt_memset(&aio->remote_addr, 0, sizeof(tt_sktaddr_t));

    aio->on_connect = on_connect;
    aio->result = TT_PROCEEDING;
    aio->cb_param = cb_param;

    if (!TT_OK(tt_skt_connect_async(ssl->skt,
                                    remote_addr,
                                    __ssl_on_connect,
                                    ev))) {
        tt_ev_destroy(ev);
        return TT_FAIL;
    }
    return TT_SUCCESS;
}

tt_result_t tt_ssl_send_async_ntv(IN struct tt_ssl_s *ssl,
                                  IN tt_blob_t *blob,
                                  IN tt_u32_t blob_num,
                                  IN tt_ssl_on_send_t on_send,
                                  IN OPT void *cb_param)
{
    tt_ev_t *ev;
    __ssl_send_t *aio;

    ev = tt_thread_ev_create(EV_SSL_SEND, sizeof(__ssl_send_t), NULL);
    if (ev == NULL) {
        TT_ERROR("fail to allocate ssl connect");
        return TT_FAIL;
    }
    aio = TT_EV_DATA(ev, __ssl_send_t);

    aio->ssl = ssl;

    aio->on_send = on_send;
    aio->cb_param = cb_param;

    if (!TT_OK(
            tt_skt_send_async(ssl->skt, blob, blob_num, __ssl_on_send, ev))) {
        tt_ev_destroy(ev);
        return TT_FAIL;
    }
    return TT_SUCCESS;
}

tt_result_t tt_ssl_recv_async_ntv(IN struct tt_ssl_s *ssl,
                                  IN tt_blob_t *blob,
                                  IN tt_u32_t blob_num,
                                  IN tt_ssl_on_recv_t on_recv,
                                  IN OPT void *cb_param)
{
    tt_ev_t *ev;
    __ssl_recv_t *aio;

    ev = tt_thread_ev_create(EV_SSL_RECV, sizeof(__ssl_recv_t), NULL);
    if (ev == NULL) {
        TT_ERROR("fail to allocate ssl recv");
        return TT_FAIL;
    }
    aio = TT_EV_DATA(ev, __ssl_recv_t);

    aio->ssl = ssl;

    aio->on_recv = on_recv;
    aio->cb_param = cb_param;

    if (!TT_OK(
            tt_skt_recv_async(ssl->skt, blob, blob_num, __ssl_on_recv, ev))) {
        tt_ev_destroy(ev);
        return TT_FAIL;
    }
    return TT_SUCCESS;
}

tt_result_t __ssl_create_client(IN tt_ssl_t *ssl)
{
    tt_ssl_ntv_t *sys_ssl = &ssl->sys_ssl;
    tt_sslctx_t *sslctx = ssl->sslctx;
    tt_skt_ntv_t *sys_skt = &ssl->skt->sys_socket;

    SecBufferDesc osbd;
    SecBuffer osb;
    SECURITY_STATUS sec_st;

    // encrypt buffer
    if (!TT_OK(tt_blob_create(&sys_ssl->input, NULL, __SSL_BUF_SIZE))) {
        TT_ERROR("no mem for ssl input");
        goto __ssc_fail;
    }

    // decrypt buffer
    if (!TT_OK(tt_blob_create(&sys_ssl->output, NULL, __SSL_BUF_SIZE))) {
        TT_ERROR("no mem for ssl output");
        goto __ssc_fail;
    }

// create context, generate handshake token
__isc_ag:
    __ssl_output_init(sys_ssl);
    __SB_INIT(&osb, sys_ssl->output.len, SECBUFFER_TOKEN, sys_ssl->output.addr);
    __SBD_INIT(&osbd, 1, &osb);

    sec_st =
        tt_g_sspi->InitializeSecurityContextW(&sslctx->sys_ctx.hcred,
                                              NULL,
                                              sys_ssl->peer_id,
                                              (ISC_REQ_CONFIDENTIALITY |
                                               ISC_REQ_MANUAL_CRED_VALIDATION |
                                               ISC_REQ_REPLAY_DETECT |
                                               ISC_REQ_SEQUENCE_DETECT |
                                               ISC_REQ_STREAM |
                                               ISC_REQ_USE_SUPPLIED_CREDS),
                                              0,
                                              0,
                                              NULL,
                                              0,
                                              &sys_ssl->hctx,
                                              &osbd,
                                              &sys_ssl->pfContextAttr,
                                              NULL);
    if (!__SEC_OK(sec_st)) {
        if ((sec_st == SEC_E_BUFFER_TOO_SMALL) &&
            TT_OK(__ssl_output_expand(sys_ssl))) {
            goto __isc_ag;
        }

        TT_ERROR("fail to init ssl context");
        goto __ssc_fail;
    }
    sys_ssl->output_data_len = osb.cbBuffer;

    return TT_SUCCESS;

__ssc_fail:

    __sys_ssl_destroy(sys_ssl);

    return TT_FAIL;
}

tt_result_t __ssl_create_server(IN tt_ssl_t *ssl)
{
    tt_ssl_ntv_t *sys_ssl = &ssl->sys_ssl;
    tt_skt_ntv_t *sys_skt = &ssl->skt->sys_socket;

    // encrypt buffer
    if (!TT_OK(tt_blob_create(&sys_ssl->input, NULL, __SSL_BUF_SIZE))) {
        TT_ERROR("no mem for ssl input");
        goto __ssc_fail;
    }

    // decrypt buffer
    if (!TT_OK(tt_blob_create(&sys_ssl->output, NULL, __SSL_BUF_SIZE))) {
        TT_ERROR("no mem for ssl output");
        goto __ssc_fail;
    }

    // server ctx would be created by AcceptSecurityContext
    // when it has received some token

    return TT_SUCCESS;

__ssc_fail:

    __sys_ssl_destroy(sys_ssl);

    return TT_FAIL;
}

void __sys_ssl_init(IN tt_ssl_ntv_t *sys_ssl)
{
    __HCTX_INIT(&sys_ssl->hctx);
    sys_ssl->pfContextAttr = 0;
    tt_memset(&sys_ssl->stream_size, 0, sizeof(SecPkgContext_StreamSizes));

    sys_ssl->peer_id = NULL;
    sys_ssl->peer_id_len = 0;

    tt_blob_init(&sys_ssl->input);
    sys_ssl->input_pos = 0;
    sys_ssl->input_plain_len = 0;

    tt_blob_init(&sys_ssl->output);
    sys_ssl->output_data_len = 0;
    sys_ssl->output_pos = 0;
    sys_ssl->extra_len = 0;
}

void __sys_ssl_destroy(IN tt_ssl_ntv_t *sys_ssl)
{
    if (__HCTX_VALID(&sys_ssl->hctx)) {
        if (tt_g_sspi->DeleteSecurityContext(&sys_ssl->hctx) != SEC_E_OK) {
            TT_ERROR("fail to delete sys_ssl context");
        }
        __HCTX_INIT(&sys_ssl->hctx);
    }

    if (sys_ssl->peer_id != NULL) {
        tt_wchar_destroy(sys_ssl->peer_id);
    }

    tt_blob_destroy(&sys_ssl->input);
    tt_blob_destroy(&sys_ssl->output);
}

void __ssl_on_destroy(IN tt_skt_t *skt, IN void *cb_param)
{
    tt_ssl_t *ssl = (tt_ssl_t *)cb_param;
    tt_ssl_exit_t *exit = &ssl->exit;
    tt_bool_t from_alloc = ssl->attr.from_alloc;

    __sys_ssl_destroy(&ssl->sys_ssl);

    exit->on_destroy(ssl, exit->cb_param);

    if (from_alloc) {
        tt_mem_free(ssl);
    }
}

tt_result_t __ssl_destroy_aio(IN tt_ev_t *ev)
{
    tt_ev_destroy(ev);
    return TT_SUCCESS;
}

tt_result_t __ssl_close_notify(IN struct tt_ssl_s *ssl)
{
    tt_ssl_ntv_t *sys_ssl = &ssl->sys_ssl;
    tt_sslctx_t *sslctx = ssl->sslctx;

    DWORD dwType = SCHANNEL_SHUTDOWN;
    SecBufferDesc osbd;
    SecBuffer osb;
    SECURITY_STATUS sec_st;

    __SB_INIT(&osb, sizeof(DWORD), SECBUFFER_TOKEN, &dwType);
    __SBD_INIT(&osbd, 1, &osb);

    sec_st = tt_g_sspi->ApplyControlToken(&sys_ssl->hctx, &osbd);
    if (sec_st != SEC_E_OK) {
        TT_ERROR("ssl close failed");
        return TT_FAIL;
    }

    __ssl_output_init(sys_ssl);
    __SB_INIT(&osb, sys_ssl->output.len, SECBUFFER_TOKEN, sys_ssl->output.addr);
    __SBD_INIT(&osbd, 1, &osb);
    if (ssl->sslctx->sys_ctx.role == TT_SSL_ROLE_CLIENT) {
        sec_st =
            tt_g_sspi
                ->InitializeSecurityContextW(&sslctx->sys_ctx.hcred,
                                             &sys_ssl->hctx,
                                             sys_ssl->peer_id,
                                             (ISC_REQ_CONFIDENTIALITY |
                                              ISC_REQ_MANUAL_CRED_VALIDATION |
                                              ISC_REQ_REPLAY_DETECT |
                                              ISC_REQ_SEQUENCE_DETECT |
                                              ISC_REQ_STREAM |
                                              ISC_REQ_USE_SUPPLIED_CREDS),
                                             0,
                                             0,
                                             NULL,
                                             0,
                                             NULL,
                                             &osbd,
                                             &sys_ssl->pfContextAttr,
                                             NULL);
        sys_ssl->output_data_len = osb.cbBuffer;
    } else {
        sec_st = tt_g_sspi->AcceptSecurityContext(&sslctx->sys_ctx.hcred,
                                                  &sys_ssl->hctx,
                                                  NULL,
                                                  (ASC_REQ_CONFIDENTIALITY |
                                                   ASC_REQ_REPLAY_DETECT |
                                                   ASC_REQ_SEQUENCE_DETECT |
                                                   ASC_REQ_STREAM |
                                                   ASC_REQ_EXTENDED_ERROR),
                                                  0,
                                                  &sys_ssl->hctx,
                                                  &osbd,
                                                  &sys_ssl->pfContextAttr,
                                                  NULL);
        sys_ssl->output_data_len = osb.cbBuffer;
    }
    if ((sec_st == SEC_E_OK) || (sec_st == SEC_I_CONTEXT_EXPIRED)) {
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

void __ssl_on_accept(IN tt_skt_t *listening_skt,
                     IN tt_skt_t *new_skt,
                     IN tt_skt_aioctx_t *aioctx)
{
    tt_ev_t *ev = (tt_ev_t *)aioctx->cb_param;
    tt_thread_ev_t *tev = TT_EV_HDR(ev, tt_thread_ev_t);
    __ssl_accept_t *aio = TT_EV_DATA(ev, __ssl_accept_t);

    tt_skt_ntv_t *listening_sys_skt = &listening_skt->sys_socket;
    tt_skt_ntv_t *new_sys_skt = &new_skt->sys_socket;
    tt_result_t result;

    TT_ASSERT_SSL(listening_sys_skt->ssl == aio->listening_ssl);
    TT_ASSERT_SSL(listening_sys_skt == &aio->listening_ssl->skt->sys_socket);

    if (!TT_OK(aioctx->result)) {
        aio->result = aioctx->result;
        __ssl_accept_cb(aio);
        tt_ev_destroy(ev);
        return;
    }
    // later if error occur, the new_sys_skt->s should be closed

    result = tt_async_ssl_create(aio->new_ssl,
                                 new_skt,
                                 aio->listening_ssl->sslctx,
                                 &aio->new_ssl_attr,
                                 &aio->new_ssl_exit);
    if (!TT_OK(result)) {
        closesocket(new_skt->sys_socket.s);
        new_skt->sys_socket.s = INVALID_SOCKET;

        aio->result = TT_FAIL;
        __ssl_accept_cb(aio);
        tt_ev_destroy(ev);
        return;
    }

    result = __ssl_create_server(aio->new_ssl);
    if (!TT_OK(result)) {
        closesocket(new_skt->sys_socket.s);
        new_skt->sys_socket.s = INVALID_SOCKET;

        aio->result = TT_FAIL;
        __ssl_accept_cb(aio);
        tt_ev_destroy(ev);
        return;
    }
    // later if error occur, the aio->new_ssl should be closed

    // start ssl handshake, the first step is to recv token
    TT_ASSERT_SSL(ev->ev_id == EV_SSL_ACCEPT_RD);
    result = __ssl_handshake_read(aio->new_ssl, &aio->wov);
    if (result != TT_PROCEEDING) {
        closesocket(new_skt->sys_socket.s);
        new_skt->sys_socket.s = INVALID_SOCKET;

        __sys_ssl_destroy(&aio->new_ssl->sys_ssl);

        aio->result = TT_FAIL;
        __ssl_accept_cb(aio);
        tt_ev_destroy(ev);
        return;
    }

    tt_list_addhead(&new_sys_skt->read_q, &tev->node);
    new_sys_skt->ssl_handshaking = TT_TRUE;
}

void __ssl_accept_cb(IN __ssl_accept_t *aio)
{
    tt_ssl_aioctx_t ssl_env;

    TT_ASSERT_SSL(aio->result != TT_PROCEEDING);

    // if something failed in __ssl_on_accept, then aio->new_ssl->skt
    // may be a null pointer
    if (TT_OK(aio->result)) {
        tt_skt_ntv_t *sys_skt = &aio->new_ssl->skt->sys_socket;

        // we can only access aio->new_ssl when on_accept return success,
        // especially when listening socket is closed, in which aio->new_ssl
        // has not been initialized

        // so caller should do by itself:
        //  - release new_skt resource
        //  - release new_skt memory

        sys_skt->ssl_handshaking = TT_FALSE;

        if (!TT_OK(__ssl_verify(aio->new_ssl))) {
#if 0
            tt_async_ssl_shutdown_ntv(aio->new_ssl, TT_SSL_SHUTDOWN_RDWR);
            //tt_async_ssl_destroy(aio->ssl, TT_FALSE);
#else
            closesocket(sys_skt->s);
            sys_skt->s = INVALID_SOCKET;

            __sys_ssl_destroy(&aio->new_ssl->sys_ssl);
#endif

            aio->result = TT_FAIL;
        }
    }

    ssl_env.result = aio->result;
    ssl_env.cb_param = aio->cb_param;

    aio->on_accept(aio->listening_ssl, aio->new_ssl, &ssl_env);
    // note aio->new_ssl not be valid now if ssl_env->result
    // is not TT_SUCCESS
}

void __ssl_on_connect(IN tt_skt_t *skt,
                      IN tt_sktaddr_t *remote_addr,
                      IN tt_skt_aioctx_t *aioctx)
{
    tt_ev_t *ev = (tt_ev_t *)aioctx->cb_param;
    tt_thread_ev_t *tev = TT_EV_HDR(ev, tt_thread_ev_t);
    __ssl_connect_t *aio = TT_EV_DATA(ev, __ssl_connect_t);

    tt_skt_ntv_t *sys_skt = &skt->sys_socket;
    tt_result_t result;

    tt_ssl_t *ssl = aio->ssl;
    tt_ssl_ntv_t *sys_ssl = &ssl->sys_ssl;
    tt_sslctx_t *sslctx = ssl->sslctx;

    TT_ASSERT_SSL(sys_skt->ssl == aio->ssl);
    TT_ASSERT_SSL(sys_skt == &aio->ssl->skt->sys_socket);

    tt_memcpy(&aio->remote_addr, remote_addr, sizeof(tt_sktaddr_t));

    if (!TT_OK(aioctx->result)) {
        aio->result = aioctx->result;
        __ssl_connect_cb(aio);
        tt_ev_destroy(ev);
        return;
    }

    // try to find cached ssl peer
    if (ssl->attr.session_resume) {
        tt_sktaddr_t local_addr;
        tt_sslpeer_t *sslpeer;

        tt_skt_local_addr(ssl->skt, &local_addr);

        sslpeer = tt_sslcache_find(&sslctx->ssl_cache,
                                   sslctx,
                                   &local_addr,
                                   remote_addr,
                                   ssl->session_mark);
        if (sslpeer != NULL) {
            TT_ASSERT_SSL(sys_ssl->peer_id == NULL);
            sys_ssl->peer_id =
                tt_wchar_create(sslpeer->peer_id, &sys_ssl->peer_id_len);

            tt_sslpeer_release(sslpeer);
        }
    }

    result = __ssl_create_client(aio->ssl);
    if (!TT_OK(result)) {
        aio->result = TT_FAIL;
        __ssl_connect_cb(aio);
        tt_ev_destroy(ev);
        return;
    }

    // start ssl handshake, the first step should be a send token
    TT_ASSERT_SSL(ev->ev_id == EV_SSL_CONNECT_WR);
    result = __ssl_handshake_write(aio->ssl, &aio->wov);
    if (result != TT_PROCEEDING) {
        aio->result = TT_FAIL;
        __ssl_connect_cb(aio);
        tt_ev_destroy(ev);
        return;
    }

    tt_list_addhead(&sys_skt->read_q, &tev->node);
    sys_skt->ssl_handshaking = TT_TRUE;
}

void __ssl_connect_cb(IN __ssl_connect_t *aio)
{
    tt_ssl_aioctx_t ssl_env;
    tt_skt_ntv_t *sys_skt = &aio->ssl->skt->sys_socket;

    TT_ASSERT_SSL(aio->result != TT_PROCEEDING);
    ssl_env.result = aio->result;
    ssl_env.cb_param = aio->cb_param;

    sys_skt->ssl_handshaking = TT_FALSE;

    if (TT_OK(ssl_env.result)) {
        if (TT_OK(__ssl_verify(aio->ssl))) {
            tt_ssl_t *ssl = aio->ssl;
            tt_sslctx_t *sslctx = aio->ssl->sslctx;
            tt_sktaddr_t local_addr;

            tt_skt_local_addr(ssl->skt, &local_addr);

            if (ssl->attr.session_resume) {
                // cache the session if it's established successfully
                tt_sslcache_add(&sslctx->ssl_cache,
                                sslctx,
                                &local_addr,
                                &aio->remote_addr,
                                ssl->session_mark,
                                NULL);
            }
        } else {
            ssl_env.result = TT_FAIL;

#if 0            
            tt_async_ssl_shutdown(aio->ssl, TT_SSL_SHUTDOWN_RDWR);
            // tt_async_ssl_destroy(aio->ssl, TT_FALSE);
            //__sys_ssl_destroy(&aio->ssl->sys_ssl);
#endif
        }
    }

    aio->on_connect(aio->ssl, &aio->remote_addr, &ssl_env);
    // note aio->ssl not be valid now if ssl_env->result is not
    // TT_SUCCESS
}

void __ssl_on_send(IN tt_skt_t *skt,
                   IN tt_blob_t *blob,
                   IN tt_u32_t blob_num,
                   IN tt_skt_aioctx_t *aioctx,
                   IN tt_u32_t send_len)
{
    tt_ev_t *ev = (tt_ev_t *)aioctx->cb_param;
    __ssl_send_t *aio = TT_EV_DATA(ev, __ssl_send_t);
    tt_ssl_aioctx_t ssl_env;

    ssl_env.result = aioctx->result;
    ssl_env.cb_param = aio->cb_param;

    aio->on_send(aio->ssl, blob, blob_num, &ssl_env, send_len);

    tt_ev_destroy(ev);
}

void __ssl_on_recv(IN tt_skt_t *skt,
                   IN tt_blob_t *blob,
                   IN tt_u32_t blob_num,
                   IN tt_skt_aioctx_t *aioctx,
                   IN tt_u32_t recv_len)
{
    tt_ev_t *ev = (tt_ev_t *)aioctx->cb_param;
    __ssl_recv_t *aio = TT_EV_DATA(ev, __ssl_recv_t);
    tt_ssl_aioctx_t ssl_env;

    ssl_env.result = aioctx->result;
    ssl_env.cb_param = aio->cb_param;

    aio->on_recv(aio->ssl, blob, blob_num, &ssl_env, recv_len);

    tt_ev_destroy(ev);
}

tt_result_t __ssl_verify(IN tt_ssl_t *ssl)
{
    tt_ssl_verify_t *ssl_verify = &ssl->sslctx->verify;
    tt_sslctx_ntv_t *sslctx = &ssl->sslctx->sys_ctx;
    tt_result_t result = TT_SUCCESS;

    if ((sslctx->role == TT_SSL_ROLE_CLIENT) && (ssl_verify->verify_peer)) {
        result = __ssl_do_verify(ssl);
    }

    if ((sslctx->role == TT_SSL_ROLE_SERVER) && (ssl_verify->verify_peer)) {
        result = __ssl_do_verify(ssl);
    }

    return result;
}

tt_result_t __ssl_do_verify(IN tt_ssl_t *ssl)
{
    tt_ssl_ntv_t *sys_ssl = &ssl->sys_ssl;
    tt_sslctx_t *sslctx = ssl->sslctx;
    tt_ssl_verify_t *ssl_verify = &sslctx->verify;
    tt_result_t result = TT_FAIL;

    SECURITY_STATUS sec_st;
    CERT_CONTEXT *cert = NULL;

    CERT_ENHKEY_USAGE EnhkeyUsage;
    CERT_USAGE_MATCH CertUsage;
    CERT_CHAIN_PARA chain_para = {0};
    PCCERT_CHAIN_CONTEXT chain_context = NULL;

    DWORD dwErrorStatus, dwInfoStatus;

    sec_st = tt_g_sspi->QueryContextAttributes(&sys_ssl->hctx,
                                               SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                               &cert);
    if (sec_st != SEC_E_OK) {
        TT_ERROR("fail to get remote certificates");
        return TT_FAIL;
    }

    EnhkeyUsage.cUsageIdentifier = 0;
    EnhkeyUsage.rgpszUsageIdentifier = NULL;
    CertUsage.dwType = USAGE_MATCH_TYPE_AND;
    CertUsage.Usage = EnhkeyUsage;

    chain_para.cbSize = sizeof(CERT_CHAIN_PARA);
    chain_para.RequestedUsage = CertUsage;

    if (!CertGetCertificateChain(sslctx->sys_ctx.chain_engine,
                                 cert,
                                 NULL,
                                 NULL,
                                 &chain_para,
                                 CERT_CHAIN_ENABLE_PEER_TRUST |
                                     CERT_CHAIN_DISABLE_AUTH_ROOT_AUTO_UPDATE,
                                 0,
                                 &chain_context)) {
        TT_ERROR_NTV("fail to build cert chain");
        goto __v_out;
    }

    // we don't make use of CertVerifyCertificateChainPolicy() which
    // can not do verification with a CA which is not self signed

    dwErrorStatus = chain_context->TrustStatus.dwErrorStatus;
    dwInfoStatus = chain_context->TrustStatus.dwInfoStatus;

    // allow partial chain
    if ((dwErrorStatus & CERT_TRUST_IS_PARTIAL_CHAIN) &&
        (dwInfoStatus & CERT_TRUST_IS_FROM_EXCLUSIVE_TRUST_STORE)) {
        dwErrorStatus &= ~CERT_TRUST_IS_PARTIAL_CHAIN;
    }

    // may allow expired cert
    if ((dwErrorStatus & CERT_TRUST_IS_NOT_TIME_VALID) &&
        ssl_verify->allow_expired_cert) {
        dwErrorStatus &= ~CERT_TRUST_IS_NOT_TIME_VALID;
    }

    if (dwErrorStatus == CERT_TRUST_NO_ERROR) {
        result = __ssl_do_verify_leaf(cert, &sslctx->verify);
    } else {
        goto __v_out;
    }

__v_out:

    if (chain_context != NULL) {
        CertFreeCertificateChain(chain_context);
    }

    if (cert != NULL) {
        CertFreeCertificateContext(cert);
    }

    return result;
}

tt_result_t __ssl_do_verify_leaf(IN CERT_CONTEXT *cert,
                                 IN tt_ssl_verify_t *ssl_verify)
{
    if (ssl_verify->common_name != NULL) {
        wchar_t *cn, *w_v_cn;
        int cn_len;
        tt_u32_t cn_size;

        cn_len = CertGetNameStringW(cert,
                                    CERT_NAME_SIMPLE_DISPLAY_TYPE,
                                    0,
                                    NULL,
                                    NULL,
                                    0);
        if (cn_len <= 0) {
            TT_ERROR("fail to get common name len");
            return TT_FAIL;
        }
        cn_size = sizeof(wchar_t) * (cn_len + 2);

        cn = (wchar_t *)tt_mem_alloc(cn_size);
        if (cn == NULL) {
            TT_ERROR("fail to alloc cn mem");
            return TT_FAIL;
        }
        cn[cn_len + 1] = 0;

        CertGetNameStringW(cert,
                           CERT_NAME_SIMPLE_DISPLAY_TYPE,
                           0,
                           NULL,
                           cn,
                           cn_len + 1);

        w_v_cn = tt_wchar_create(ssl_verify->common_name, NULL);
        if (w_v_cn == NULL) {
            tt_mem_free(cn);
            return TT_FAIL;
        }

        // these two strings should already be null terminated
        if (wcscmp(w_v_cn, cn) != 0) {
            tt_mem_free(w_v_cn);
            tt_mem_free(cn);
            return TT_FAIL;
        }

        tt_mem_free(w_v_cn);
        tt_mem_free(cn);
    }

    return TT_SUCCESS;
}

void __ssl_input_init(IN tt_ssl_ntv_t *sys_ssl)
{
    sys_ssl->input_pos = 0;
    sys_ssl->input_plain_len = 0;
}

void __ssl_input_update(IN tt_ssl_ntv_t *sys_ssl, IN tt_u32_t io_bytes)
{
    sys_ssl->input_pos += io_bytes;
    TT_ASSERT_SSL(sys_ssl->input_pos <= sys_ssl->input.len);
}

tt_result_t __ssl_input_expand(IN tt_ssl_ntv_t *sys_ssl)
{
    tt_blob_t *ibuf = &sys_ssl->input;
    if (ibuf->len < __SSL_BUF_MAX_SIZE) {
        tt_u8_t *new_buf = (tt_u8_t *)tt_mem_alloc(__SSL_BUF_MAX_SIZE);
        if (new_buf == NULL) {
            TT_ERROR("fail to enlarge ssl input buf");
            return TT_FAIL;
        }
        tt_memcpy(new_buf, ibuf->addr, ibuf->len);

        tt_mem_free(ibuf->addr);
        ibuf->addr = new_buf;
        ibuf->len = __SSL_BUF_MAX_SIZE;
        return TT_SUCCESS;
    } else {
        // TT_WARN("ssl input buf reach limitation");
        return TT_FAIL;
    }
}

tt_result_t __ssl_input_resize(IN tt_ssl_ntv_t *sys_ssl, IN tt_u32_t size)
{
    tt_blob_t *ibuf = &sys_ssl->input;
    tt_u8_t *new_buf;

    // this function is for test purpose

    // min
    if (size < __SSL_BUF_SIZE) {
        size = __SSL_BUF_SIZE;
    }
    if (size < sys_ssl->input_pos) {
        size = sys_ssl->input_pos;
    }

    // max
    if (size > __SSL_BUF_MAX_SIZE) {
        size = __SSL_BUF_MAX_SIZE;
    }

    if (size == ibuf->len) {
        return TT_SUCCESS;
    }

    new_buf = (tt_u8_t *)tt_mem_alloc(size);
    if (new_buf == NULL) {
        TT_ERROR("fail to resize ssl input buf");
        return TT_FAIL;
    }
    tt_memcpy(new_buf, ibuf->addr, ibuf->len);

    tt_mem_free(ibuf->addr);
    ibuf->addr = new_buf;
    ibuf->len = size;
    return TT_SUCCESS;
}

tt_bool_t __ssl_input_keep_extra(IN tt_ssl_ntv_t *sys_ssl,
                                 IN SecBufferDesc *output_sbd)
{
    tt_blob_t *ibuf = &sys_ssl->input;
    ULONG i;
    tt_bool_t has_extra = TT_FALSE;

    for (i = 0; i < output_sbd->cBuffers; ++i) {
        SecBuffer *sb = &output_sbd->pBuffers[i];
        if (sb->BufferType == SECBUFFER_EXTRA) {
            TT_ASSERT_SSL(sys_ssl->input_pos + sb->cbBuffer <= ibuf->len);
            tt_memmove(ibuf->addr + sys_ssl->input_pos,
                       sb->pvBuffer,
                       sb->cbBuffer);
            sys_ssl->input_pos += sb->cbBuffer;

            has_extra = TT_TRUE;
        }
    }
    TT_ASSERT_SSL(sys_ssl->input_pos < ibuf->len);

    return has_extra;
}

void __ssl_input_fix(IN tt_ssl_ntv_t *sys_ssl)
{
    tt_blob_t *ibuf = &sys_ssl->input;
    tt_u8_t *__pos = ibuf->addr;
    tt_u8_t *__end = ibuf->addr + sys_ssl->input_pos;
    tt_u32_t discard_len;

    // note sslv2 is assumed disabled
    while (__pos + 5 <= __end) {
        tt_u32_t pkt_len = 5 + ((__pos[3] << 8) | __pos[4]);
        if ((__pos + pkt_len) <= __end) {
            __pos += pkt_len;
        } else {
            break;
        }
    }

    discard_len = (tt_u32_t)TT_PTR_DIFF(__pos, ibuf->addr);
    TT_ASSERT(discard_len <= sys_ssl->input_pos);
    if (discard_len == sys_ssl->input_pos) {
        sys_ssl->input_pos = 0;
    } else if (discard_len > 0) {
        TT_SSL_DETAIL("discard [%d] bytes", discard_len);
        tt_memmove(ibuf->addr, __pos, sys_ssl->input_pos - discard_len);
        sys_ssl->input_pos -= discard_len;
    }
    // else nothing is discarded
}

void __ssl_output_init(IN tt_ssl_ntv_t *sys_ssl)
{
    sys_ssl->output_data_len = 0;
    sys_ssl->output_pos = 0;
}

tt_bool_t __ssl_output_update(IN tt_ssl_ntv_t *sys_ssl, IN tt_u32_t io_bytes)
{
    sys_ssl->output_pos += io_bytes;
    TT_ASSERT_SSL(sys_ssl->output_pos <= sys_ssl->output_data_len);
    if (sys_ssl->output_pos < sys_ssl->output_data_len) {
        return TT_FALSE;
    } else {
        return TT_TRUE;
    }
}

tt_result_t __ssl_output_expand(IN tt_ssl_ntv_t *sys_ssl)
{
    tt_blob_t *obuf = &sys_ssl->output;
    if (obuf->len < __SSL_BUF_MAX_SIZE) {
        tt_u8_t *new_buf = (tt_u8_t *)tt_mem_alloc(__SSL_BUF_MAX_SIZE);
        if (new_buf == NULL) {
            TT_ERROR("fail to enlarge ssl output buf");
            return TT_FAIL;
        }
        // tt_memcpy(new_buf, obuf->addr, obuf->len);

        tt_mem_free(obuf->addr);
        obuf->addr = new_buf;
        obuf->len = __SSL_BUF_MAX_SIZE;
        return TT_SUCCESS;
    } else {
        // TT_WARN("ssl output buf reach limitation");
        return TT_FAIL;
    }
}

tt_result_t __ssl_output_resize(IN tt_ssl_ntv_t *sys_ssl, IN tt_u32_t size)
{
    tt_blob_t *obuf = &sys_ssl->output;
    tt_u8_t *new_buf;

    // this function is for test purpose

    // min
    if (size < __SSL_BUF_SIZE) {
        size = __SSL_BUF_SIZE;
    }
    if (size < sys_ssl->output_data_len) {
        size = sys_ssl->output_data_len;
    }

    // max
    if (size > __SSL_BUF_MAX_SIZE) {
        size = __SSL_BUF_MAX_SIZE;
    }

    if (size == obuf->len) {
        return TT_SUCCESS;
    }

    new_buf = (tt_u8_t *)tt_mem_alloc(size);
    if (new_buf == NULL) {
        TT_ERROR("fail to resize ssl output buf");
        return TT_FAIL;
    }
    tt_memcpy(new_buf, obuf->addr, obuf->len);

    tt_mem_free(obuf->addr);
    obuf->addr = new_buf;
    obuf->len = size;
    return TT_SUCCESS;
}

tt_result_t __ssl_handshake_read(IN tt_ssl_t *ssl, IN WSAOVERLAPPED *wov)
{
    tt_ssl_ntv_t *sys_ssl = &ssl->sys_ssl;
    tt_skt_ntv_t *sys_skt = &ssl->skt->sys_socket;

    WSABUF Buffer;
    DWORD NumberOfBytesRecvd = 0;
    DWORD Flags = 0;
    DWORD dwError;

    // read data to sys_ssl->input

    TT_ASSERT_SSL(sys_ssl->input.len > sys_ssl->input_pos);
    Buffer.buf = __SSL_IN_ADDR(sys_ssl);
    Buffer.len = __SSL_IN_LEN(sys_ssl);

    TT_SSL_DETAIL("to recv[%d] bytes", Buffer.len);

    tt_memset(wov, 0, sizeof(WSAOVERLAPPED));

    if ((WSARecv(sys_skt->s,
                 &Buffer,
                 1,
                 &NumberOfBytesRecvd,
                 &Flags,
                 wov,
                 NULL) != 0) &&
        ((dwError = WSAGetLastError()) != WSA_IO_PENDING)) {
        TT_NET_ERROR_NTV("recv fail");

        if ((dwError == WSAECONNABORTED) || (dwError == WSAECONNRESET)) {
            return TT_END;
        } else {
            return TT_FAIL;
        }
    }

    return TT_PROCEEDING;
}

tt_result_t __ssl_handshake_write(IN tt_ssl_t *ssl, IN WSAOVERLAPPED *wov)
{
    tt_ssl_ntv_t *sys_ssl = &ssl->sys_ssl;
    tt_skt_ntv_t *sys_skt = &ssl->skt->sys_socket;

    WSABUF Buffer;
    DWORD NumberOfBytesSent = 0;
    DWORD dwError;

    TT_ASSERT_SSL(sys_ssl->output_data_len > sys_ssl->output_pos);
    Buffer.buf = __SSL_OUT_ADDR(sys_ssl);
    Buffer.len = __SSL_OUT_LEN(sys_ssl);

    TT_SSL_DETAIL("to send[%d] bytes", Buffer.len);

    tt_memset(wov, 0, sizeof(WSAOVERLAPPED));

    if ((WSASend(sys_skt->s, &Buffer, 1, &NumberOfBytesSent, 0, wov, NULL) !=
         0) &&
        ((dwError = WSAGetLastError()) != WSA_IO_PENDING)) {
        TT_NET_ERROR_NTV("send fail");

        if ((dwError == WSAECONNABORTED) || (dwError == WSAECONNRESET)) {
            return TT_END;
        } else {
            return TT_FAIL;
        }
    }

    return TT_PROCEEDING;
}

tt_result_t __ssl_handshake(IN tt_ssl_t *ssl,
                            IN tt_ev_t *ev,
                            IN DWORD NumberOfBytes,
                            IN tt_result_t iocp_result)
{
    tt_result_t result;

    switch (ev->ev_id) {
        case EV_SSL_CONNECT_RD: {
            __ssl_connect_t *aio = TT_EV_DATA(ev, __ssl_connect_t);

            result = __ssl_client_on_read(ssl, aio, NumberOfBytes, iocp_result);
        } break;
        case EV_SSL_CONNECT_WR: {
            __ssl_connect_t *aio = TT_EV_DATA(ev, __ssl_connect_t);

            result =
                __ssl_client_on_write(ssl, aio, NumberOfBytes, iocp_result);
        } break;

        case EV_SSL_ACCEPT_RD: {
            __ssl_accept_t *aio = TT_EV_DATA(ev, __ssl_accept_t);

            result = __ssl_server_on_read(ssl, aio, NumberOfBytes, iocp_result);
        } break;
        case EV_SSL_ACCEPT_WR: {
            __ssl_accept_t *aio = TT_EV_DATA(ev, __ssl_accept_t);

            result =
                __ssl_server_on_write(ssl, aio, NumberOfBytes, iocp_result);
        } break;

        default: {
            TT_FATAL("unknown ssl aio: %x", ev->ev_id);
            result = TT_FAIL;
        } break;
    }

    if (result != TT_PROCEEDING) {
        tt_thread_ev_t *tev = TT_EV_HDR(ev, tt_thread_ev_t);

        tt_list_remove(&tev->node);
        tt_ev_destroy(ev);
    }

    return result;
}

tt_result_t __ssl_client_on_read(IN tt_ssl_t *ssl,
                                 IN __ssl_connect_t *aio,
                                 IN DWORD NumberOfBytes,
                                 IN tt_result_t iocp_result)
{
    tt_ssl_ntv_t *sys_ssl = &ssl->sys_ssl;
    tt_result_t result;
    tt_bool_t to_read;

    TT_SSL_DETAIL("result[%d], bytes[%d]", iocp_result, NumberOfBytes);

    if (!TT_OK(iocp_result)) {
        aio->result = iocp_result;
        __ssl_connect_cb(aio);
        return TT_FAIL;
    }

    if (NumberOfBytes == 0) {
        // socket is shutdown during handshaking
        aio->result = TT_END;
        __ssl_connect_cb(aio);
        return TT_FAIL;
    }

    // inpput:
    // update received data
    __ssl_input_update(sys_ssl, NumberOfBytes);

    // output:
    // as we are sure data in output buf has been sent, otherwise
    // it won't start receiving, so it can clean output buffer
    __ssl_output_init(sys_ssl);

    // do handshake
    result = __ssl_client_handshake(ssl, aio, &to_read);
    if (result == TT_PROCEEDING) {
        tt_thread_ev_t *tev = TT_TEV_OF(aio);
        tt_ev_t *ev = TT_EV_OF(tev);

        if (to_read) {
            ev->ev_id = EV_SSL_CONNECT_RD;
            result = __ssl_handshake_read(ssl, &aio->wov);
        } else {
            ev->ev_id = EV_SSL_CONNECT_WR;
            result = __ssl_handshake_write(ssl, &aio->wov);
        }
    }

    if (TT_OK(result)) {
        aio->result = TT_SUCCESS;
        __ssl_connect_cb(aio);
        return TT_SUCCESS;
    } else if (result == TT_PROCEEDING) {
        return TT_PROCEEDING;
    } else {
        aio->result = result;
        __ssl_connect_cb(aio);
        return TT_FAIL;
    }
}

tt_result_t __ssl_client_on_write(IN tt_ssl_t *ssl,
                                  IN __ssl_connect_t *aio,
                                  IN DWORD NumberOfBytes,
                                  IN tt_result_t iocp_result)
{
    tt_ssl_ntv_t *sys_ssl = &ssl->sys_ssl;
    tt_result_t result;

    tt_thread_ev_t *tev = TT_TEV_OF(aio);
    tt_ev_t *ev = TT_EV_OF(tev);

    TT_SSL_DETAIL("result[%d], bytes[%d]", iocp_result, NumberOfBytes);

    if (!TT_OK(iocp_result)) {
        aio->result = iocp_result;
        __ssl_connect_cb(aio);
        return TT_FAIL;
    }

    if (__ssl_output_update(sys_ssl, NumberOfBytes)) {
        // all sent
        if (ssl->skt->sys_socket.ssl_connected) {
            result = TT_SUCCESS;
        } else {
            ev->ev_id = EV_SSL_CONNECT_RD;
            result = __ssl_handshake_read(ssl, &aio->wov);
        }
    } else {
        ev->ev_id = EV_SSL_CONNECT_WR;
        result = __ssl_handshake_write(ssl, &aio->wov);
    }

    if (TT_OK(result)) {
        aio->result = TT_SUCCESS;
        __ssl_connect_cb(aio);
        return TT_SUCCESS;
    } else if (result == TT_PROCEEDING) {
        return TT_PROCEEDING;
    } else {
        aio->result = result;
        __ssl_connect_cb(aio);
        return TT_FAIL;
    }
}

tt_result_t __ssl_server_on_read(IN tt_ssl_t *ssl,
                                 IN __ssl_accept_t *aio,
                                 IN DWORD NumberOfBytes,
                                 IN tt_result_t iocp_result)
{
    tt_ssl_ntv_t *sys_ssl = &ssl->sys_ssl;
    tt_result_t result = TT_FAIL;
    tt_bool_t to_read;

    TT_SSL_DETAIL("result[%d], bytes[%d]", iocp_result, NumberOfBytes);

    if (!TT_OK(iocp_result)) {
        result = iocp_result;
        goto __sor_out;
    }

    if (NumberOfBytes == 0) {
        // socket is shutdown during handshaking
        goto __sor_out;
    }

    // inpput:
    // update received data
    __ssl_input_update(sys_ssl, NumberOfBytes);

    // output:
    // as we are sure data in output buf has been sent, otherwise
    // it won't start receiving, so it can clean output buffer
    __ssl_output_init(sys_ssl);

    // do handshake
    result = __ssl_server_handshake(ssl, aio, &to_read);
    if (result == TT_PROCEEDING) {
        tt_thread_ev_t *tev = TT_TEV_OF(aio);
        tt_ev_t *ev = TT_EV_OF(tev);

        if (to_read) {
            ev->ev_id = EV_SSL_ACCEPT_RD;
            result = __ssl_handshake_read(ssl, &aio->wov);
        } else {
            ev->ev_id = EV_SSL_ACCEPT_WR;
            result = __ssl_handshake_write(ssl, &aio->wov);
        }
    }

__sor_out:
    if (TT_OK(result)) {
        aio->result = TT_SUCCESS;
        __ssl_accept_cb(aio);
        return TT_SUCCESS;
    } else if (result == TT_PROCEEDING) {
        return TT_PROCEEDING;
    } else {
        closesocket(ssl->skt->sys_socket.s);
        ssl->skt->sys_socket.s = INVALID_SOCKET;

        __sys_ssl_destroy(sys_ssl);

        aio->result = result;
        __ssl_accept_cb(aio);
        return TT_FAIL;
    }
}

tt_result_t __ssl_server_on_write(IN tt_ssl_t *ssl,
                                  IN __ssl_accept_t *aio,
                                  IN DWORD NumberOfBytes,
                                  IN tt_result_t iocp_result)
{
    tt_ssl_ntv_t *sys_ssl = &ssl->sys_ssl;
    tt_result_t result = TT_FAIL;

    tt_thread_ev_t *tev = TT_TEV_OF(aio);
    tt_ev_t *ev = TT_EV_OF(tev);

    TT_SSL_DETAIL("result[%d], bytes[%d]", iocp_result, NumberOfBytes);

    if (!TT_OK(iocp_result)) {
        result = iocp_result;
        goto __sow_out;
    }

    if (__ssl_output_update(sys_ssl, NumberOfBytes)) {
        // all sent
        if (ssl->skt->sys_socket.ssl_connected) {
            result = TT_SUCCESS;
        } else {
            ev->ev_id = EV_SSL_ACCEPT_RD;
            result = __ssl_handshake_read(ssl, &aio->wov);
        }
    } else {
        ev->ev_id = EV_SSL_ACCEPT_WR;
        result = __ssl_handshake_write(ssl, &aio->wov);
    }

__sow_out:
    if (TT_OK(result)) {
        aio->result = TT_SUCCESS;
        __ssl_accept_cb(aio);
        return TT_SUCCESS;
    } else if (result == TT_PROCEEDING) {
        return TT_PROCEEDING;
    } else {
        closesocket(ssl->skt->sys_socket.s);
        ssl->skt->sys_socket.s = INVALID_SOCKET;

        __sys_ssl_destroy(sys_ssl);

        aio->result = result;
        __ssl_accept_cb(aio);
        return TT_FAIL;
    }
}

tt_result_t __ssl_client_handshake(IN tt_ssl_t *ssl,
                                   IN __ssl_connect_t *aio,
                                   OUT tt_bool_t *to_read)
{
    tt_skt_t *skt = ssl->skt;
    tt_skt_ntv_t *sys_skt = &skt->sys_socket;
    tt_ssl_ntv_t *sys_ssl = &ssl->sys_ssl;
    tt_sslctx_t *sslctx = ssl->sslctx;

    SecBufferDesc isbd, osbd;
    SecBuffer isb[2], osb;
    SECURITY_STATUS sec_st;

    // - input data: in sys_ssl->input
    // - output data: put to sys_ssl->output
    // - return:
    //     TT_SUCCESS: handshake done
    //     TT_PROCEEDING: continue, check to_read
    //     other: handshake fail

    // input:
    __SB_INIT(&isb[0],
              sys_ssl->input_pos,
              SECBUFFER_TOKEN,
              sys_ssl->input.addr);
    __SB_INIT(&isb[1], 0, SECBUFFER_EMPTY, NULL);
    __SBD_INIT(&isbd, 2, isb);

__isc_ag:
    // output:
    __SB_INIT(&osb, sys_ssl->output.len, SECBUFFER_TOKEN, sys_ssl->output.addr);
    __SBD_INIT(&osbd, 1, &osb);

    sec_st =
        tt_g_sspi->InitializeSecurityContextW(&sslctx->sys_ctx.hcred,
                                              &sys_ssl->hctx,
                                              sys_ssl->peer_id,
                                              (ISC_REQ_CONFIDENTIALITY |
                                               ISC_REQ_MANUAL_CRED_VALIDATION |
                                               ISC_REQ_REPLAY_DETECT |
                                               ISC_REQ_SEQUENCE_DETECT |
                                               ISC_REQ_STREAM |
                                               ISC_REQ_USE_SUPPLIED_CREDS),
                                              0,
                                              0,
                                              &isbd,
                                              0,
                                              NULL,
                                              &osbd,
                                              &sys_ssl->pfContextAttr,
                                              NULL);

    if ((sec_st == SEC_E_BUFFER_TOO_SMALL) &&
        TT_OK(__ssl_output_expand(sys_ssl))) {
        goto __isc_ag;
    }

    if ((sec_st == SEC_I_COMPLETE_AND_CONTINUE) ||
        (sec_st == SEC_I_COMPLETE_NEEDED)) {
        if (tt_g_sspi->CompleteAuthToken(&sys_ssl->hctx, &osbd) != SEC_E_OK) {
            TT_ERROR("fail to complete auth");
            return TT_FAIL;
        }
    }

    if (((sec_st == SEC_I_COMPLETE_AND_CONTINUE) ||
         (sec_st == SEC_I_CONTINUE_NEEDED)) &&
        (osb.cbBuffer == 0)) {
#if 1
        // as verified, InitializeSecurityContextW return
        // SEC_E_INCOMPLETE_MESSAGE only when the beginning ssl message
        // is incomplete, and return SEC_I_CONTINUE_NEEDED if received
        // data includes a complete ssl message and followed by an
        // incomplete message with empty output_sbd. to fix this, it
        // should discard all the complete message and leave the partial
        // message in buffer
        __ssl_input_fix(sys_ssl);
        sec_st = SEC_E_INCOMPLETE_MESSAGE;
#else
        // or consider error?
        return TT_FAIL;
#endif
    }

    if (sec_st == SEC_E_INCOMPLETE_MESSAGE) {
        // it's possible that ssl has delivered a large enough buffer
        // but still a partial of ssl handshake data is received
        __ssl_input_expand(sys_ssl);
        if (sys_ssl->input_pos >= sys_ssl->input.len) {
            return TT_FAIL;
        }

        // there should be no SECBUFFER_EXTRA
    } else {
        __ssl_input_init(sys_ssl);
        __ssl_input_keep_extra(sys_ssl, &osbd);
    }

    switch (sec_st) {
        case SEC_I_COMPLETE_AND_CONTINUE:
        case SEC_I_CONTINUE_NEEDED: {
            sys_ssl->output_data_len = osb.cbBuffer;

            *to_read = TT_FALSE;
            return TT_PROCEEDING;
        } break;
        case SEC_I_COMPLETE_NEEDED:
        case SEC_E_OK: {
            // handshake done

            if (tt_g_sspi->QueryContextAttributes(&sys_ssl->hctx,
                                                  SECPKG_ATTR_STREAM_SIZES,
                                                  &sys_ssl->stream_size) !=
                SEC_E_OK) {
                TT_ERROR("fail to query stream size");
                return TT_FAIL;
            }

            sys_skt->ssl_connected = TT_TRUE;

            if ((osb.BufferType == SECBUFFER_TOKEN) && (osb.cbBuffer > 0)) {
                sys_ssl->output_data_len = osb.cbBuffer;

                // still some data to sent
                *to_read = TT_FALSE;
                return TT_PROCEEDING;
            } else {
                __ssl_input_init(sys_ssl);
                __ssl_output_init(sys_ssl);
                return TT_SUCCESS;
            }
        } break;
        case SEC_I_INCOMPLETE_CREDENTIALS: {
            TT_ERROR("client has no matching cert as server required");
            return TT_FAIL;
        } break;
        case SEC_E_INCOMPLETE_MESSAGE: {
            // continue read
            *to_read = TT_TRUE;
            return TT_PROCEEDING;
        }

        default: {
            // should not reach here
            return TT_FAIL;
        } break;
    }
}

tt_result_t __ssl_server_handshake(IN tt_ssl_t *ssl,
                                   IN __ssl_accept_t *aio,
                                   OUT tt_bool_t *to_read)
{
    tt_skt_t *skt = ssl->skt;
    tt_skt_ntv_t *sys_skt = &skt->sys_socket;
    tt_ssl_ntv_t *sys_ssl = &ssl->sys_ssl;
    tt_sslctx_t *sslctx = ssl->sslctx;

    SecBufferDesc isbd, osbd;
    SecBuffer isb[2], osb;
    SECURITY_STATUS sec_st;
    ULONG fContextReq =
        (ASC_REQ_CONFIDENTIALITY | ASC_REQ_REPLAY_DETECT |
         ASC_REQ_SEQUENCE_DETECT | ASC_REQ_STREAM | ASC_REQ_EXTENDED_ERROR);

    if (sslctx->verify.verify_peer) {
        fContextReq |= ASC_REQ_MUTUAL_AUTH;
    }

    // - input data: in sys_ssl->input
    // - output data: put to sys_ssl->buf_desc
    // - return:
    //     TT_SUCCESS: handshake done
    //     TT_PROCEEDING: continue, check to_read
    //     other: handshake fail

    // input:
    __SB_INIT(&isb[0],
              sys_ssl->input_pos,
              SECBUFFER_TOKEN,
              sys_ssl->input.addr);
    __SB_INIT(&isb[1], 0, SECBUFFER_EMPTY, NULL);
    __SBD_INIT(&isbd, 2, isb);

__isc_ag:
    // output:
    __SB_INIT(&osb, sys_ssl->output.len, SECBUFFER_TOKEN, sys_ssl->output.addr);
    __SBD_INIT(&osbd, 1, &osb);

    if (__HCTX_VALID(&sys_ssl->hctx)) {
        sec_st = tt_g_sspi->AcceptSecurityContext(&sslctx->sys_ctx.hcred,
                                                  &sys_ssl->hctx,
                                                  &isbd,
                                                  fContextReq,
                                                  0,
                                                  &sys_ssl->hctx,
                                                  &osbd,
                                                  &sys_ssl->pfContextAttr,
                                                  NULL);
    } else {
        sec_st = tt_g_sspi->AcceptSecurityContext(&sslctx->sys_ctx.hcred,
                                                  NULL,
                                                  &isbd,
                                                  fContextReq,
                                                  0,
                                                  &sys_ssl->hctx,
                                                  &osbd,
                                                  &sys_ssl->pfContextAttr,
                                                  NULL);
    }

    if ((sec_st == SEC_E_BUFFER_TOO_SMALL) &&
        TT_OK(__ssl_output_expand(sys_ssl))) {
        goto __isc_ag;
    }

    if ((sec_st == SEC_I_COMPLETE_AND_CONTINUE) ||
        (sec_st == SEC_I_COMPLETE_NEEDED)) {
        if (tt_g_sspi->CompleteAuthToken(&sys_ssl->hctx, &osbd) != SEC_E_OK) {
            TT_ERROR("fail to complete auth");
            return TT_FAIL;
        }
    }

    if (((sec_st == SEC_I_COMPLETE_AND_CONTINUE) ||
         (sec_st == SEC_I_CONTINUE_NEEDED)) &&
        (osb.cbBuffer == 0)) {
#if 1
        // as verified, InitializeSecurityContextW return
        // SEC_E_INCOMPLETE_MESSAGE only when the beginning ssl message
        // is incomplete, and return SEC_I_CONTINUE_NEEDED if received
        // data includes a complete ssl message and followed by an
        // incomplete message with empty output_sbd. to fix this, it
        // should discard all the complete message and leave the partial
        // message in buffer
        __ssl_input_fix(sys_ssl);
        sec_st = SEC_E_INCOMPLETE_MESSAGE;
#else
        // or consider error?
        return TT_FAIL;
#endif
    }

    if (sec_st == SEC_E_INCOMPLETE_MESSAGE) {
        // it's possible that ssl has delivered a large enough buffer
        // but still a partial of ssl handshake data is received
        __ssl_input_expand(sys_ssl);
        if (sys_ssl->input_pos >= sys_ssl->input.len) {
            return TT_FAIL;
        }

        // there should be no SECBUFFER_EXTRA
    } else {
        __ssl_input_init(sys_ssl);
        __ssl_input_keep_extra(sys_ssl, &osbd);
    }

    switch (sec_st) {
        case SEC_I_COMPLETE_AND_CONTINUE:
        case SEC_I_CONTINUE_NEEDED: {
            sys_ssl->output_data_len = osb.cbBuffer;

            *to_read = TT_FALSE;
            return TT_PROCEEDING;
        } break;
        case SEC_I_COMPLETE_NEEDED:
        case SEC_E_OK: {
            // handshake done

            if (tt_g_sspi->QueryContextAttributes(&sys_ssl->hctx,
                                                  SECPKG_ATTR_STREAM_SIZES,
                                                  &sys_ssl->stream_size) !=
                SEC_E_OK) {
                TT_ERROR("fail to query stream size");
                return TT_FAIL;
            }

            sys_skt->ssl_connected = TT_TRUE;

            if ((osb.BufferType == SECBUFFER_TOKEN) && (osb.cbBuffer > 0)) {
                sys_ssl->output_data_len = osb.cbBuffer;

                *to_read = TT_FALSE;
                return TT_PROCEEDING;
            } else {
                __ssl_input_init(sys_ssl);
                __ssl_output_init(sys_ssl);
                return TT_SUCCESS;
            }
        } break;
        case SEC_E_INCOMPLETE_MESSAGE: {
            // continue read
            *to_read = TT_TRUE;
            return TT_PROCEEDING;
        }

        default: {
            // should not reach here
            return TT_FAIL;
        } break;
    }
}

tt_result_t __ssl_encrypt(IN tt_ssl_t *ssl,
                          IN tt_u8_t *data,
                          IN OUT tt_u32_t *data_len)
{
    tt_ssl_ntv_t *sys_ssl = &ssl->sys_ssl;
    tt_u32_t len = *data_len;
    SecPkgContext_StreamSizes *ssize = &sys_ssl->stream_size;
    tt_u32_t extra_size = ssize->cbHeader + ssize->cbTrailer;

    SecBufferDesc ebuf_desc;
    SecBuffer ebuf[4];
    SECURITY_STATUS sec_st;

    // discard all data in output buffer, caller should be
    // sure that all data in output buffer have been sent
    __ssl_output_init(sys_ssl);

    TT_SSL_DETAIL("plain data len[%d]", len);
    if (len > sys_ssl->stream_size.cbMaximumMessage) {
        len = sys_ssl->stream_size.cbMaximumMessage;
    }
    TT_ASSERT_SSL(extra_size < sys_ssl->output.len);
    if ((len + extra_size) > sys_ssl->output.len) {
        __ssl_output_expand(sys_ssl);
        if ((len + extra_size) > sys_ssl->output.len) {
            len = sys_ssl->output.len - extra_size;
        }
    }
    TT_SSL_DETAIL("encrypted data len[%d]", len);
    tt_memcpy(TT_PTR_INC(tt_u8_t, sys_ssl->output.addr, ssize->cbHeader),
              data,
              len);

    // encrypt
    __SB_INIT(&ebuf[0],
              ssize->cbHeader,
              SECBUFFER_STREAM_HEADER,
              sys_ssl->output.addr);
    __SB_INIT(&ebuf[1],
              len,
              SECBUFFER_DATA,
              TT_PTR_INC(tt_u8_t, sys_ssl->output.addr, ssize->cbHeader));
    __SB_INIT(&ebuf[2],
              ssize->cbTrailer,
              SECBUFFER_STREAM_TRAILER,
              TT_PTR_INC(tt_u8_t, sys_ssl->output.addr, ssize->cbHeader + len));
    __SB_INIT(&ebuf[3], 0, SECBUFFER_EMPTY, NULL);
    __SBD_INIT(&ebuf_desc, 4, ebuf);

    sec_st = tt_g_sspi->EncryptMessage(&sys_ssl->hctx, 0, &ebuf_desc, 0);
    if (sec_st == SEC_E_OK) {
        // note values of ebuf[0].cbBuffer and ebuf[2].cbBuffer
        // may change after calling EncryptMessage, so that they
        // must be saved
        sys_ssl->extra_len = ebuf[0].cbBuffer + ebuf[2].cbBuffer;
        sys_ssl->output_data_len = sys_ssl->extra_len + len;

        *data_len = len;
        return TT_SUCCESS;
    } else {
        TT_ERROR("ssl encrypt failed");
        return TT_FAIL;
    }
}

// return:
//  - TT_SUCCESS: data have been extracted
//  - TT_PROCEEDING: no data is extracted, should keep receiving
//  - otherwise: decrypting failed
tt_result_t __ssl_decrypt(IN tt_ssl_t *ssl,
                          IN tt_u32_t last_io_bytes,
                          IN tt_ssl_on_decrypt_t on_decrypt,
                          IN void *cb_param)
{
    tt_ssl_ntv_t *sys_ssl = &ssl->sys_ssl;

    SecBuffer dbuf[4];
    SecBufferDesc dbuf_desc;
    SECURITY_STATUS sec_st;
    tt_u8_t *cipher_data;
    tt_u32_t cipher_data_len;
    tt_bool_t do_cb = TT_TRUE;
    tt_bool_t data_extracted = TT_FALSE;

    // case1: no new data, no plain data in ssl buffer
    if ((last_io_bytes == 0) && (sys_ssl->input_plain_len == 0)) {
        return TT_PROCEEDING;
    }

    // case2: has plain data in ssl buffer
    if ((last_io_bytes == 0) && (sys_ssl->input_plain_len > 0)) {
        tt_u32_t n;

        n = on_decrypt(sys_ssl->input.addr, sys_ssl->input_plain_len, cb_param);
        TT_ASSERT_SSL(n <= sys_ssl->input_plain_len);
        if (n < sys_ssl->input_plain_len) {
            tt_memmove(sys_ssl->input.addr,
                       TT_PTR_INC(tt_u8_t, sys_ssl->input.addr, n),
                       sys_ssl->input_pos - n);
        }
        sys_ssl->input_plain_len -= n;
        sys_ssl->input_pos -= n;

        // return success because decrypted data has been feed by
        // on_decrypt
        return TT_SUCCESS;
    }

    // case3: receive data, decrypt and extract
    sys_ssl->input_pos += last_io_bytes;
    cipher_data = __SSL_IN_CIPHER_ADDR(sys_ssl);
    cipher_data_len = __SSL_IN_CIPHER_LEN(sys_ssl);

#ifdef __SIMULATE_SSL_AIO_FAIL
    do {
        /*
        static tt_u32_t __counter = 0;
        __ssl_input_expand(sys_ssl); // for test
        if (__counter++ % 6 != 5)
        {
            return TT_PROCEEDING;
        }
        */

        // receive untill buffer is full, this need limit input size
        __ssl_input_resize(sys_ssl, 0);
        if (sys_ssl->input_pos != sys_ssl->input.len) {
            return TT_PROCEEDING;
        }
    } while (0);
#endif

__dec_ag:
    __SB_INIT(&dbuf[0], cipher_data_len, SECBUFFER_DATA, cipher_data);
    __SB_INIT(&dbuf[1], 0, SECBUFFER_EMPTY, NULL);
    __SB_INIT(&dbuf[2], 0, SECBUFFER_EMPTY, NULL);
    __SB_INIT(&dbuf[3], 0, SECBUFFER_EMPTY, NULL);
    __SBD_INIT(&dbuf_desc, 4, dbuf);

    sec_st = tt_g_sspi->DecryptMessage(&sys_ssl->hctx, &dbuf_desc, 0, NULL);
    if (sec_st == SEC_E_OK) {
        tt_u32_t i;
        tt_bool_t more_data = TT_FALSE;

        // secbuf returned by DecryptMessage() is supposed including
        // only 1 header, 1 trailer, 1 data and perhaps 1 extra
        for (i = 0; i < 4; ++i) {
            SecBuffer *sb = &dbuf[i];
            if (sb->BufferType == SECBUFFER_DATA) {
                tt_u32_t n = 0;

                if (do_cb) {
                    n = on_decrypt((tt_u8_t *)sb->pvBuffer,
                                   (tt_u32_t)sb->cbBuffer,
                                   cb_param);
                    data_extracted = TT_TRUE;
                }
                TT_ASSERT_SSL(n <= (tt_u32_t)sb->cbBuffer);
                if (n < (tt_u32_t)sb->cbBuffer) {
                    // if caller does not eat all data, those data
                    // decrypted would be cached
                    do_cb = TT_FALSE;

                    tt_memmove(TT_PTR_INC(tt_u8_t,
                                          sys_ssl->input.addr,
                                          sys_ssl->input_plain_len),
                               TT_PTR_INC(tt_u8_t, sb->pvBuffer, n),
                               sb->cbBuffer - n);
                    sys_ssl->input_plain_len += (sb->cbBuffer - n);
                }
            } else if (sb->BufferType == SECBUFFER_EXTRA) {
                cipher_data = (tt_u8_t *)sb->pvBuffer;
                cipher_data_len = (tt_u32_t)sb->cbBuffer;
                more_data = TT_TRUE;
            }
        }
        if (more_data) {
            // note sys_ssl->input_pos can not be updated here
            // as there are still cipher data
            goto __dec_ag;
        }

        // all are decrypted
        sys_ssl->input_pos = sys_ssl->input_plain_len;
        return TT_SUCCESS;
    } else if (sec_st == SEC_E_INCOMPLETE_MESSAGE) {
        tt_u8_t *cipher_addr = __SSL_IN_CIPHER_ADDR(sys_ssl);

        if (cipher_addr != cipher_data) {
            tt_memmove(cipher_addr, cipher_data, cipher_data_len);
        }
        sys_ssl->input_pos = sys_ssl->input_plain_len + cipher_data_len;

        TT_ASSERT_SSL(sys_ssl->input_pos <= sys_ssl->input.len);
        if ((sys_ssl->input_pos == sys_ssl->input.len) &&
            !TT_OK(__ssl_input_expand(sys_ssl))) {
            return TT_FAIL;
        }

        if (data_extracted) {
            // return success to tell caller to extract decrypted
            // data, altough there are still partial cipher data
            // in the in-buffer, which would be processed when all
            // plain data are extracted and a new read aio is posted
            return TT_SUCCESS;
        } else {
            return TT_PROCEEDING;
        }
    } else if (sec_st == SEC_I_CONTEXT_EXPIRED) {
        // received a ssl close notify
        return TT_END;
    } else {
        TT_ERROR("ssl decrypt failed");
        return TT_FAIL;
    }
}

#endif
