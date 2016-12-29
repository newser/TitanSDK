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
#include <misc/tt_reference_counter.h>
#include <network/ssl/tt_ssl.h>
#include <network/ssl/tt_ssl_aio.h>

#include <tt_socket_aio_native.h>
#include <tt_ssl_context_native.h>
#include <tt_util_native.h>

#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef TT_PLATFORM_SSL_ENABLE

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define TT_ASSERT_SSL TT_ASSERT

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

    EV_SSL_CONNECT,
    EV_SSL_ACCEPT,
    EV_SSL_SEND,
    EV_SSL_RECV,

    EV_SSLAIO_END,

};

typedef struct
{
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

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __ssl_ref_config(IN tt_ssl_t *ssl);

// static
tt_result_t __ssl_handshake(IN tt_ssl_t *ssl, IN tt_result_t kqryctx);

static void __ssl_on_destroy(IN tt_skt_t *skt, IN void *cb_param);
// static
tt_result_t __ssl_destroy_aio(IN tt_ev_t *ev);

// static
tt_result_t __ssl_close_notify(IN struct tt_ssl_s *ssl);

// accept
static __ssl_accept_t *__ssl_accept_get(IN tt_skt_ntv_t *sys_skt);
static void __ssl_on_accept(IN tt_skt_t *listening_skt,
                            IN tt_skt_t *new_skt,
                            IN tt_skt_aioctx_t *aioctx);
static tt_result_t __ssl_accept_io(IN __ssl_accept_t *aio,
                                   IN tt_skt_ntv_t *sys_skt);
static void __ssl_accept_cb(IN __ssl_accept_t *aio);

// connect
static __ssl_connect_t *__ssl_connect_get(IN tt_skt_ntv_t *sys_skt);
static void __ssl_on_connect(IN tt_skt_t *skt,
                             IN tt_sktaddr_t *remote_addr,
                             IN tt_skt_aioctx_t *aioctx);
static tt_result_t __ssl_connect_io(IN __ssl_connect_t *aio,
                                    IN tt_skt_ntv_t *sys_skt);
static void __ssl_connect_cb(IN __ssl_connect_t *aio);

// send
static OSStatus __ssl_send_io(IN SSLConnectionRef connection,
                              IN const void *data,
                              IN OUT size_t *dataLength);
static void __ssl_on_send(IN tt_skt_t *skt,
                          IN tt_blob_t *blob,
                          IN tt_u32_t blob_num,
                          IN tt_skt_aioctx_t *aioctx,
                          IN tt_u32_t send_len);

// recv
static OSStatus __ssl_recv_io(IN SSLConnectionRef connection,
                              IN void *data,
                              IN OUT size_t *dataLength);
static void __ssl_on_recv(IN tt_skt_t *skt,
                          IN tt_blob_t *blob,
                          IN tt_u32_t blob_num,
                          IN tt_skt_aioctx_t *aioctx,
                          IN tt_u32_t recv_len);

static tt_result_t __ssl_verify(IN tt_ssl_t *ssl);
static tt_result_t __ssl_do_verify(IN tt_ssl_t *ssl);
static tt_result_t __ssl_vfy_common_name(IN SecCertificateRef cert,
                                         IN tt_char_t *common_name);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_async_ssl_create_ntv(IN struct tt_ssl_s *ssl,
                                    IN TO tt_skt_t *skt,
                                    IN tt_sslctx_t *sslctx,
                                    IN OPT tt_ssl_attr_t *attr,
                                    IN tt_ssl_exit_t *exit)
{
    SSLContextRef ssl_ref = NULL;
    tt_u32_t skt_role;
    tt_ssl_role_t ssl_role;
    SSLProtocolSide protocolSide;
    OSStatus osst;

    TT_ASSERT(ssl != NULL);
    TT_ASSERT(skt != NULL);
    TT_ASSERT(skt->sys_socket.ssl == NULL);
    TT_ASSERT(sslctx != NULL);
    TT_ASSERT(exit != NULL);
    TT_ASSERT(exit->on_destroy != NULL);

    skt_role = skt->sys_socket.role;
    ssl_role = sslctx->sys_ctx.role;
    if (skt_role == TT_SKT_ROLE_TCP_CONNECT) {
        TT_ASSERT_SSL(ssl_role == TT_SSL_ROLE_CLIENT);
        protocolSide = kSSLClientSide;
    } else {
        TT_ASSERT_SSL((skt_role == TT_SKT_ROLE_TCP_LISTEN) ||
                      (skt_role == TT_SKT_ROLE_TCP_ACCEPT));
        TT_ASSERT_SSL(ssl_role == TT_SSL_ROLE_SERVER);
        protocolSide = kSSLServerSide;
    }

    if (skt_role != TT_SKT_ROLE_TCP_LISTEN) {
        ssl_ref = SSLCreateContext(NULL, protocolSide, kSSLStreamType);
        if (ssl_ref == NULL) {
            TT_ERROR("fail to create ssl context");
            return TT_FAIL;
        }

        osst = SSLSetConnection(ssl_ref, skt);
        if (osst != noErr) {
            CFRelease(ssl_ref);

            TT_ERROR("fail to set ssl connection");
            tt_osstatus_show(osst);
            return TT_FAIL;
        }

        osst = SSLSetIOFuncs(ssl_ref, __ssl_recv_io, __ssl_send_io);
        if (osst != noErr) {
            CFRelease(ssl_ref);

            TT_ERROR("fail to set ssl io funcs");
            tt_osstatus_show(osst);
            return TT_FAIL;
        }
    }
    // else listening ssl only save sslctx

    ssl->sys_ssl.ssl_ref = ssl_ref;

    ssl->skt = skt;
    ssl->sslctx = sslctx;

    tt_memcpy(&ssl->exit, exit, sizeof(tt_ssl_exit_t));

    if (attr != NULL) {
        tt_memcpy(&ssl->attr, attr, sizeof(tt_ssl_attr_t));
    } else {
        tt_ssl_attr_default(&ssl->attr);
    }

    ssl->session_mark = 0;

    // config openssl ssl according to ts ssl if it's not a
    // listening socket
    if ((skt_role != TT_SKT_ROLE_TCP_LISTEN) && !TT_OK(__ssl_ref_config(ssl))) {
        CFRelease(ssl_ref);
        return TT_FAIL;
    }

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

    if ((mode & TT_SSL_SHUTDOWN_RD) &&
        TT_OK(tt_async_skt_shutdown(ssl->skt, TT_SKT_SHUTDOWN_RD))) {
        result = TT_SUCCESS;
    }

    if ((mode & TT_SSL_SHUTDOWN_WR) &&
        TT_OK(tt_async_skt_shutdown(ssl->skt, TT_SKT_SHUTDOWN_WR))) {
        result = TT_SUCCESS;
    }

    return result;
}

tt_result_t tt_ssl_accept_async_ntv(IN struct tt_ssl_s *listening_ssl,
                                    IN struct tt_ssl_s *new_ssl,
                                    IN tt_skt_t *new_skt,
                                    IN OPT tt_skt_attr_t *new_skt_attr,
                                    IN OPT tt_ssl_attr_t *new_ssl_attr,
                                    IN tt_ssl_exit_t *new_ssl_exit,
                                    IN tt_ssl_on_accept_t on_accept,
                                    IN OPT void *cb_param)
{
    tt_skt_exit_t skt_exit;
    tt_ev_t *ev;
    __ssl_accept_t *aio;

    skt_exit.on_destroy = __ssl_on_destroy;
    skt_exit.cb_param = new_ssl;

    ev = tt_thread_ev_create(EV_SSL_ACCEPT, sizeof(__ssl_accept_t), NULL);
    if (ev == NULL) {
        TT_ERROR("fail to allocate ssl accept");
        return TT_FAIL;
    }
    aio = TT_EV_DATA(ev, __ssl_accept_t);

    tt_memset(aio, 0, sizeof(__ssl_accept_t));

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
    __ssl_connect_t *aio = NULL;

    TT_ASSERT(ssl != NULL);
    TT_ASSERT(remote_addr != NULL);
    TT_ASSERT(on_connect != NULL);

    ev = tt_thread_ev_create(EV_SSL_CONNECT, sizeof(__ssl_connect_t), NULL);
    if (ev == NULL) {
        TT_ERROR("fail to allocate ssl connect");
        return TT_FAIL;
    }
    aio = TT_EV_DATA(ev, __ssl_connect_t);

    tt_memset(aio, 0, sizeof(__ssl_connect_t));

    aio->ssl = ssl;

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

tt_result_t __ssl_ref_config(IN tt_ssl_t *ssl)
{
    tt_sslctx_ntv_t *sys_ctx = &ssl->sslctx->sys_ctx;
    tt_ssl_verify_t *ssl_verify = &ssl->sslctx->verify;

    SSLContextRef ssl_ref = ssl->sys_ssl.ssl_ref;
    OSStatus osst;

    // certificate
    if (sys_ctx->cert_privkey != NULL) {
        osst = SSLSetCertificate(ssl_ref, sys_ctx->cert_privkey->cert);
        if (osst != noErr) {
            TT_ERROR("fail to set ssl certificates");
            tt_osstatus_show(osst);
            return TT_FAIL;
        }
    }

    // peer certificate verify
    // to make verify proccess consistent on all platforms, we use
    // manually verify
    if (sys_ctx->role == TT_SSL_ROLE_CLIENT) {
        osst = SSLSetSessionOption(ssl_ref,
                                   kSSLSessionOptionBreakOnServerAuth,
                                   true);
        if (osst != noErr) {
            TT_ERROR("fail to set ssl server auth");
            tt_osstatus_show(osst);
            return TT_FAIL;
        }
    } else {
        osst = SSLSetSessionOption(ssl_ref,
                                   kSSLSessionOptionBreakOnClientAuth,
                                   true);
        if (osst != noErr) {
            TT_ERROR("fail to set ssl server auth");
            tt_osstatus_show(osst);
            return TT_FAIL;
        }

        if (ssl_verify->verify_peer) {
            osst = SSLSetClientSideAuthenticate(ssl_ref, kAlwaysAuthenticate);
            if (osst != noErr) {
                TT_ERROR("fail to set ssl server auth client");
                tt_osstatus_show(osst);
                return TT_FAIL;
            }
        }
    }

    // ssl version

    // cipher suite

    // private key/certificate
    // kSSLSessionOptionBreakOnServerAuth
    // SSLSetPeerDomainName
    // SSLSetPeerID

    return TT_SUCCESS;
}

void __ssl_on_destroy(IN tt_skt_t *skt, IN void *cb_param)
{
    tt_ssl_t *ssl = (tt_ssl_t *)cb_param;
    tt_ssl_exit_t *exit = &ssl->exit;
    tt_bool_t from_alloc = ssl->attr.from_alloc;

    // listening socket has no ssl_ref
    if (ssl->sys_ssl.ssl_ref != NULL) {
        // TT_CF_REF_SHOW(ssl->sys_ssl.ssl_ref);
        CFRelease(ssl->sys_ssl.ssl_ref);
        ssl->sys_ssl.ssl_ref = NULL;
    }

    exit->on_destroy(ssl, exit->cb_param);

    if (from_alloc) {
        tt_free(ssl);
    }
}

tt_result_t __ssl_destroy_aio(IN tt_ev_t *ev)
{
    switch (ev->ev_id) {
        case EV_SSL_CONNECT: {
            __ssl_connect_t *aio = TT_EV_DATA(ev, __ssl_connect_t);

        } break;
        case EV_SSL_ACCEPT: {
            __ssl_accept_t *aio = TT_EV_DATA(ev, __ssl_accept_t);

        } break;

        default: {
            TT_FATAL("invalid ssl tev[%x]", ev->ev_id);
        } break;
    }
    return TT_SUCCESS;
}

tt_result_t __ssl_close_notify(IN struct tt_ssl_s *ssl)
{
    SSLContextRef ssl_ref = ssl->sys_ssl.ssl_ref;
    OSStatus osst;

    osst = SSLClose(ssl_ref);
    if (osst == noErr) {
        return TT_SUCCESS;
    } else {
        tt_osstatus_show(osst);
        TT_ERROR("ssl close failed");

        return TT_FAIL;
    }
}

__ssl_accept_t *__ssl_accept_get(IN tt_skt_ntv_t *sys_skt)
{
    tt_lnode_t *node;
    tt_thread_ev_t *tev;

    TT_ASSERT_SSL(__SKT_ROLE_OF_TCP_ACCEPT(sys_skt->role));
    TT_ASSERT_SSL(sys_skt->ssl_rd_handshaking);
    TT_ASSERT_SSL(sys_skt->ssl_wr_handshaking);

    node = tt_list_head(&sys_skt->read_q);
    TT_ASSERT_SSL(node != NULL);

    tev = TT_CONTAINER(node, tt_thread_ev_t, node);
    TT_ASSERT_SSL(TT_EV_OF(tev)->ev_id == EV_SSL_ACCEPT);

    return TT_TEV_DATA(tev, __ssl_accept_t);
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

    tt_sslpeer_t *sslpeer;
    tt_ssl_t *new_ssl = aio->new_ssl;
    tt_sslctx_t *sslctx = aio->listening_ssl->sslctx;

    TT_ASSERT_SSL(listening_sys_skt->ssl == aio->listening_ssl);
    TT_ASSERT_SSL(listening_sys_skt == &aio->listening_ssl->skt->sys_socket);

    if (!TT_OK(aioctx->result)) {
        aio->result = aioctx->result;
        __ssl_accept_cb(aio);
        tt_ev_destroy(ev);
        return;
    }
    // later if error occur, the new_sys_skt->s should be closed

    if (!TT_OK(tt_async_ssl_create(new_ssl,
                                   new_skt,
                                   sslctx,
                                   &aio->new_ssl_attr,
                                   &aio->new_ssl_exit))) {
        __RETRY_IF_EINTR(close(new_sys_skt->s) != 0);
        new_sys_skt->s = -1;

        aio->result = TT_FAIL;
        __ssl_accept_cb(aio);
        tt_ev_destroy(ev);
        return;
    }

    if (new_ssl->attr.session_resume) {
        tt_sktaddr_t local_addr, remote_addr;

        tt_skt_local_addr(new_skt, &local_addr);
        tt_skt_remote_addr(new_skt, &remote_addr);

        // try session resume
        sslpeer = tt_sslcache_find(&sslctx->ssl_cache,
                                   sslctx,
                                   &local_addr,
                                   &remote_addr,
                                   new_ssl->session_mark);
        if (sslpeer != NULL) {
            SSLSetPeerID(new_ssl->sys_ssl.ssl_ref,
                         sslpeer->peer_id,
                         sslpeer->peer_id_len);
            tt_sslpeer_release(sslpeer);
        }
    }

    // start ssl handshake

    tt_list_push_head(&new_sys_skt->read_q, &tev->node);
    new_sys_skt->ssl_rd_handshaking = TT_TRUE;
    new_sys_skt->ssl_wr_handshaking = TT_TRUE;

    __ssl_handshake(aio->new_ssl, TT_SUCCESS);
}

void __ssl_accept_cb(IN __ssl_accept_t *aio)
{
    tt_ssl_aioctx_t ssl_env;

    ssl_env.result = aio->result;
    ssl_env.cb_param = aio->cb_param;

    // if something failed in __ssl_on_accept, then aio->new_ssl->skt
    // may be a null pointer
    if (aio->new_ssl->skt != NULL) {
        tt_skt_ntv_t *sys_skt = &aio->new_ssl->skt->sys_socket;

        sys_skt->ssl_rd_handshaking = TT_FALSE;
        sys_skt->ssl_want_rd = TT_FALSE;

        sys_skt->ssl_wr_handshaking = TT_FALSE;
        sys_skt->ssl_want_wr = TT_FALSE;
    }

    if (TT_OK(ssl_env.result)) {
        tt_ssl_t *ssl = aio->new_ssl;
        tt_sslctx_t *sslctx = ssl->sslctx;
        tt_sktaddr_t local_addr, remote_addr;

        tt_skt_local_addr(ssl->skt, &local_addr);
        tt_skt_remote_addr(ssl->skt, &remote_addr);

        if (ssl->attr.session_resume) {
            // cache the session if it's established successfully
            tt_sslcache_add(&sslctx->ssl_cache,
                            sslctx,
                            &local_addr,
                            &remote_addr,
                            ssl->session_mark,
                            NULL);
        }
    }

    aio->on_accept(aio->listening_ssl, aio->new_ssl, &ssl_env);
    // note aio->new_ssl not be valid now if ssl_env->result is not
    // TT_SUCCESS
}

__ssl_connect_t *__ssl_connect_get(IN tt_skt_ntv_t *sys_skt)
{
    tt_lnode_t *node;
    tt_thread_ev_t *tev;

    TT_ASSERT_SSL(__SKT_ROLE_OF_TCP_CONNECT(sys_skt->role));
    TT_ASSERT_SSL(sys_skt->ssl_rd_handshaking);
    TT_ASSERT_SSL(sys_skt->ssl_wr_handshaking);

    node = tt_list_head(&sys_skt->read_q);
    TT_ASSERT_SSL(node != NULL);

    tev = TT_CONTAINER(node, tt_thread_ev_t, node);
    TT_ASSERT_SSL(TT_EV_OF(tev)->ev_id == EV_SSL_CONNECT);

    return TT_TEV_DATA(tev, __ssl_connect_t);
}

void __ssl_on_connect(IN tt_skt_t *skt,
                      IN tt_sktaddr_t *remote_addr,
                      IN tt_skt_aioctx_t *aioctx)
{
    tt_ev_t *ev = (tt_ev_t *)aioctx->cb_param;
    tt_thread_ev_t *tev = TT_EV_HDR(ev, tt_thread_ev_t);
    __ssl_connect_t *aio = TT_EV_DATA(ev, __ssl_connect_t);

    tt_skt_ntv_t *sys_skt = &skt->sys_socket;

    tt_ssl_t *ssl = aio->ssl;
    tt_sslctx_t *sslctx = ssl->sslctx;
    tt_sslpeer_t *sslpeer;

    TT_ASSERT_SSL(sys_skt->ssl == ssl);
    TT_ASSERT_SSL(sys_skt == &aio->ssl->skt->sys_socket);

    tt_memcpy(&aio->remote_addr, remote_addr, sizeof(tt_sktaddr_t));

    if (!TT_OK(aioctx->result)) {
        aio->result = aioctx->result;
        __ssl_connect_cb(aio);
        tt_ev_destroy(ev);
        return;
    }

    if (ssl->attr.session_resume) {
        tt_sktaddr_t local_addr;

        tt_skt_local_addr(ssl->skt, &local_addr);

        sslpeer = tt_sslcache_find(&sslctx->ssl_cache,
                                   sslctx,
                                   &local_addr,
                                   remote_addr,
                                   ssl->session_mark);
        if (sslpeer != NULL) {
            SSLSetPeerID(ssl->sys_ssl.ssl_ref,
                         sslpeer->peer_id,
                         sslpeer->peer_id_len);
            tt_sslpeer_release(sslpeer);
        }
    }

    // start ssl handshake

    tt_list_push_head(&sys_skt->read_q, &tev->node);
    sys_skt->ssl_rd_handshaking = TT_TRUE;
    sys_skt->ssl_wr_handshaking = TT_TRUE;

    __ssl_handshake(ssl, TT_SUCCESS);
}

void __ssl_connect_cb(IN __ssl_connect_t *aio)
{
    tt_ssl_aioctx_t ssl_env;
    tt_skt_ntv_t *sys_skt = &aio->ssl->skt->sys_socket;

    ssl_env.result = aio->result;
    ssl_env.cb_param = aio->cb_param;

    sys_skt->ssl_rd_handshaking = TT_FALSE;
    sys_skt->ssl_want_rd = TT_FALSE;

    sys_skt->ssl_wr_handshaking = TT_FALSE;
    sys_skt->ssl_want_wr = TT_FALSE;

    if (TT_OK(ssl_env.result)) {
        tt_ssl_t *ssl = aio->ssl;
        tt_sslctx_t *sslctx = aio->ssl->sslctx;

        if (ssl->attr.session_resume) {
            tt_sktaddr_t local_addr;

            tt_skt_local_addr(ssl->skt, &local_addr);

            // cache the session if it's established successfully
            tt_sslcache_add(&sslctx->ssl_cache,
                            sslctx,
                            &local_addr,
                            &aio->remote_addr,
                            ssl->session_mark,
                            NULL);
        }
    }

    aio->on_connect(aio->ssl, &aio->remote_addr, &ssl_env);
    // note aio->ssl not be valid now if ssl_env->result is not
    // TT_SUCCESS
}

OSStatus __ssl_send_io(IN SSLConnectionRef connection,
                       IN const void *data,
                       IN OUT size_t *dataLength)
{
    tt_skt_t *skt = (tt_skt_t *)connection;
    tt_skt_ntv_t *sys_skt = &skt->sys_socket;

    tt_u8_t *pos = (tt_u8_t *)data;
    tt_u32_t len = (tt_u32_t)*dataLength;

    // security framework would maintain the data buffer status, here
    // it only need:
    //  - send data
    //  - set dataLength to the number of sent bytes
    //  - return errSSLWouldBlock if partial of data are sent, return
    //    noErr if all are sent return failure if error occurred

    do {
        ssize_t n = send(sys_skt->s, pos, len, 0);
        if (n > 0) {
            TT_ASSERT_SSL(n <= len);
            pos += n;
            len -= n;
        } else if (n == 0) {
            // not error?
            break;
        } else {
            if (errno == EINTR) {
                continue;
            } else if (errno == EAGAIN) {
                break;
            } else {
                // all other are considered error
                return errSSLInternal;
            }
        }
    } while (len > 0);

    *dataLength -= len;
    if (len == 0) {
        return noErr;
    } else {
        return errSSLWouldBlock;
    }
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

OSStatus __ssl_recv_io(IN SSLConnectionRef connection,
                       IN void *data,
                       IN OUT size_t *dataLength)
{
    tt_skt_t *skt = (tt_skt_t *)connection;
    tt_skt_ntv_t *sys_skt = &skt->sys_socket;

    tt_u8_t *pos = (tt_u8_t *)data;
    tt_u32_t len = (tt_u32_t)*dataLength;

    do {
        ssize_t n = recv(sys_skt->s, pos, len, 0);
        if (n > 0) {
            TT_ASSERT_SSL(n <= len);
            pos += n;
            len -= n;
        } else if (n == 0) {
            return errSSLClosedGraceful;
        } else {
            if (errno == EINTR) {
                continue;
            } else if (errno == EAGAIN) {
                break;
            }
#if 0
            else if (errno == ECONNRESET)
            {
                return errSSLClosedAbort;
            }
#endif
            else {
                // all other are considered error
                return errSSLInternal;
            }
        }
    } while (len > 0);

    *dataLength -= len;
    if (len == 0) {
        return noErr;
    } else {
        return errSSLWouldBlock;
    }
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

#if 0
static
tt_result_t __ssl_do_verify_cert_fld(IN SecTrustRef trust,
                                     IN tt_ssl_verify_t *ssl_verify)
{
    CFTypeRef fld[TT_SSL_CERT_FLD_NUM] = {0};
    CFIndex fld_num = 0, i;
    
    SecCertificateRef cert = NULL;
    CFArrayRef key = NULL;
    CFDictionaryRef val = NULL;
    CFDictionaryRef dict = NULL;
    CFArrayRef ar = NULL;

    if (ssl_verify->vfy_fld[TT_SSL_CERT_FLD_COMMON_NAME] &&
        (ssl_verify->vfy_common_name != NULL))
    {
        fld[fld_num++] = kSecOIDX509V1SubjectName;
    }
    
    // more field
    
    if (fld_num == 0)
    {
        // nothing to verify
        return TT_SUCCESS;
    }
    
    // use index 0?
    cert = SecTrustGetCertificateAtIndex(trust, 0);
    if (cert == NULL)
    {
        TT_ERROR("no cert in trust");
        goto __vf_fail;
    }
    
    key = CFArrayCreate(NULL, fld, fld_num, &kCFTypeArrayCallBacks);
    if (key == NULL)
    {
        goto __vf_fail;
    }
    
    val = SecCertificateCopyValues(cert, key, NULL);
    if (val == NULL)
    {
        goto __vf_fail;
    }

    for (i = 0; i < fld_num; ++i)
    {
        tt_uintptr_t fld_type = fld[i];
        
        dict = CFDictionaryGetValue(val, (const void*)fld_type);
        if (dict == NULL)
        {
            goto __vf_fail;
        }
        
        ar = CFDictionaryGetValue(dict, kSecPropertyKeyValue);
        if ((ar == NULL) || (CFGetTypeID(ar) != CFArrayGetTypeID()))
        {
            goto __vf_fail;
        }

        dict = CFArrayGetValueAtIndex(ar, 0);
        if ((dict == NULL) || (CFGetTypeID(dict) != CFDictionaryGetTypeID()))
        {
            goto __vf_fail;
        }
        
        switch (fld_type)
        {
            case TT_SSL_CERT_FLD_COMMON_NAME:
            {
                CFStringRef fld_val = \
                    CFDictionaryGetValue(dict, kSecPropertyKeyValue);
                if (dict == NULL)
                {
                    goto __vf_fail;
                }
                if (CFGetTypeID(fld_val) != CFStringGetTypeID())
                {
                   goto __vf_fail;
                }
                
                // check commmon name here??
            }
            break;
                
            default:
            {
            }
            break;
        }
    }
    
    return TT_SUCCESS;
    
  __vf_fail:
  
    if (val != NULL)
    {
        CFRelease(val);
    }

    if (key != NULL)
    {
        CFRelease(key);
    }

    return TT_FAIL;
}
#endif

tt_result_t __ssl_do_verify(IN tt_ssl_t *ssl)
{
    tt_lnode_t *node;
    tt_sslctx_t *sslctx = ssl->sslctx;
    tt_ssl_verify_t *ssl_verify = &sslctx->verify;

    SecTrustRef trust = NULL;
    OSStatus osst;
    tt_result_t result;
    SecTrustResultType trust_result;

    osst = SSLCopyPeerTrust(ssl->sys_ssl.ssl_ref, &trust);
    if ((osst != noErr) || (trust == NULL)) {
        TT_ERROR("fail to copy peer trust");
        tt_osstatus_show(osst);
        return TT_FAIL;
    }

    result = tt_sslctx_config_trust(ssl->sslctx, trust);
    if (!TT_OK(result)) {
        CFRelease(trust);
        return TT_FAIL;
    }

    osst = SecTrustEvaluate(trust, &trust_result);
    CFRelease(trust);
    if ((osst == noErr) && ((trust_result == kSecTrustResultProceed) ||
                            (trust_result == kSecTrustResultUnspecified))) {
        SecCertificateRef cert;

        // use cert at index 0?
        cert = SecTrustGetCertificateAtIndex(trust, 0);
        if (cert == NULL) {
            TT_ERROR("no cert in trust");
            return TT_FAIL;
        }

        // verify common name
        if (ssl_verify->common_name != NULL) {
            result = __ssl_vfy_common_name(cert, ssl_verify->common_name);
            if (!TT_OK(result)) {
                return TT_FAIL;
            }
        }

        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_result_t __ssl_vfy_common_name(IN SecCertificateRef cert,
                                  IN tt_char_t *common_name)
{
    // ios does not support get common name from a certificate, maybe we
    // have to rely on SecTrustSetPolicies to verify common name
    return TT_SUCCESS;
}

tt_result_t __ssl_handshake(IN tt_ssl_t *ssl, IN tt_result_t kq_result)
{
    tt_skt_ntv_t *sys_skt = &ssl->skt->sys_socket;
    OSStatus osst;
    tt_result_t result;

    if (TT_OK(kq_result)) {
    __hsk:
        osst = SSLHandshake(ssl->sys_ssl.ssl_ref);
        if (osst == errSSLWouldBlock) {
            // we don't know whether it really want to read, but as
            // either ssl connect or accept aio is always in read q
            // before handshaking done, it's safe to monitor kqueue
            // reading status
            __skt_kq_rd_enable(sys_skt);
            sys_skt->ssl_want_rd = TT_TRUE;

            return TT_PROCEEDING;
        } else if (osst == errSSLPeerAuthCompleted) {
            if (TT_OK(__ssl_verify(ssl))) {
                goto __hsk;
            }
        }
    } else {
        osst = errSSLInternal;
    }

    // todo: to handle:
    //  - errSSLServerAuthCompleted
    //  - errSSLClientAuthCompleted

    // here either handshake is completed or failed
    if (__SKT_ROLE_OF_TCP_CONNECT(sys_skt->role)) {
        __ssl_connect_t *aio;
        tt_thread_ev_t *tev;
        tt_ev_t *ev;

        aio = __ssl_connect_get(sys_skt);
        TT_ASSERT_SSL(aio != NULL);
        tev = TT_TEV_OF(aio);
        ev = TT_EV_OF(tev);

        if (osst == noErr) {
            aio->result = TT_SUCCESS;
            result = TT_SUCCESS;
        } else {
            aio->result = TT_FAIL;
            result = TT_END;
        }

        __ssl_connect_cb(aio);
        tt_list_remove(&tev->node);
        tt_ev_destroy(ev);
        return result;
    } else {
        __ssl_accept_t *aio;
        tt_thread_ev_t *tev;
        tt_ev_t *ev;

        TT_ASSERT_SSL(__SKT_ROLE_OF_TCP_ACCEPT(sys_skt->role));

        aio = __ssl_accept_get(sys_skt);
        TT_ASSERT_SSL(aio != NULL);
        tev = TT_TEV_OF(aio);
        ev = TT_EV_OF(tev);

        if (osst == noErr) {
            aio->result = TT_SUCCESS;
            result = TT_SUCCESS;
        } else {
            tt_skt_ntv_t *new_sys_skt;

            // something special to handle. to close ssl and socket
            // created in __ssl_on_accept

            new_sys_skt = &aio->new_ssl->skt->sys_socket;
            __RETRY_IF_EINTR(close(new_sys_skt->s) != 0);
            new_sys_skt->s = -1;

            CFRelease(aio->new_ssl->sys_ssl.ssl_ref);
            aio->new_ssl->sys_ssl.ssl_ref = NULL;

            aio->result = TT_FAIL;
            result = TT_END;
        }

        __ssl_accept_cb(aio);
        tt_list_remove(&tev->node);
        tt_ev_destroy(ev);
        return result;
    }
}

#endif
