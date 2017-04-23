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

//#include <event/tt_event_base.h>
//#include <event/tt_thread_event.h>
//#include <io/tt_socket_aio.h>
#include <memory/tt_memory_alloc.h>
#include <misc/tt_reference_counter.h>
#include <network/ssl/tt_ssl.h>
#include <network/ssl/tt_ssl_aio.h>
#include <network/ssl/tt_ssl_context.h>

//#include <tt_socket_aio_native.h>
#include <tt_ssl_context_native.h>
#include <tt_util_native.h>

#ifdef TT_PLATFORM_SSL_ENABLE

#include <openssl/err.h>
#include <openssl/ssl.h>
#include <unistd.h>

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

extern tt_result_t __skt_start_epoll(IN tt_skt_ntv_t *sys_skt);

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __openssl_ssl_config(IN tt_ssl_t *ssl);

// static
tt_result_t __ssl_handshake(IN tt_ssl_t *ssl, IN tt_result_t ep_result);

static void __ssl_on_destroy(IN tt_skt_t *skt, IN void *cb_param);

// static
tt_result_t __ssl_destroy_aio(IN tt_ev_t *ev);

// static
tt_result_t __ssl_close_notify(IN tt_ssl_t *ssl);

// accept
static __ssl_accept_t *__ssl_accept_pop(IN tt_skt_ntv_t *sys_skt);
static void __ssl_on_accept(IN tt_skt_t *listening_skt,
                            IN tt_skt_t *new_skt,
                            IN tt_skt_aioctx_t *aioctx);
static tt_result_t __ssl_accept_io(IN __ssl_accept_t *aio,
                                   IN tt_skt_ntv_t *sys_skt);
static void __ssl_accept_cb(IN __ssl_accept_t *aio);

// connect
static __ssl_connect_t *__ssl_connect_pop(IN tt_skt_ntv_t *sys_skt);
static void __ssl_on_connect(IN tt_skt_t *skt,
                             IN tt_sktaddr_t *remote_addr,
                             IN tt_skt_aioctx_t *aioctx);
static tt_result_t __ssl_connect_io(IN __ssl_connect_t *aio,
                                    IN tt_skt_ntv_t *sys_skt);
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

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_async_ssl_create_ntv(IN tt_ssl_t *ssl,
                                    IN TO tt_skt_t *skt,
                                    IN tt_sslctx_t *sslctx,
                                    IN OPT tt_ssl_attr_t *attr,
                                    IN tt_ssl_exit_t *exit)
{
    SSL *new_ssl = NULL;
    tt_u32_t skt_role;
    tt_ssl_role_t ssl_role;

    TT_ASSERT(ssl != NULL);
    TT_ASSERT(skt != NULL);
    TT_ASSERT(skt->sys_skt.ssl == NULL);
    TT_ASSERT(sslctx != NULL);
    TT_ASSERT(exit != NULL);
    TT_ASSERT(exit->on_destroy != NULL);

    skt_role = skt->sys_skt.role;
    ssl_role = sslctx->sys_ctx.role;
    if (skt_role == TT_SKT_ROLE_TCP_CONNECT) {
        TT_ASSERT_SSL(ssl_role == TT_SSL_ROLE_CLIENT);
    } else {
        TT_ASSERT_SSL((skt_role == TT_SKT_ROLE_TCP_LISTEN) ||
                      (skt_role == TT_SKT_ROLE_TCP_ACCEPT));
        TT_ASSERT_SSL(ssl_role == TT_SSL_ROLE_SERVER);
    }

    if (skt_role != TT_SKT_ROLE_TCP_LISTEN) {
        // openssl ssl
        new_ssl = SSL_new(sslctx->sys_ctx.openssl_ctx);
        if (new_ssl == NULL) {
            __SSL_ERROR("fail to new ssl");
            return TT_FAIL;
        }

        if (SSL_set_fd(new_ssl, skt->sys_skt.s) != 1) {
            SSL_free(new_ssl);

            __SSL_ERROR("fail to set ssl fd");
            return TT_FAIL;
        }
    }
    // else listening ssl only save sslctx

    tt_memset(ssl, 0, sizeof(tt_ssl_t));

    ssl->sys_ssl.ssl = new_ssl;

    ssl->skt = skt;
    ssl->sslctx = sslctx;

    tt_memcpy(&ssl->exit, exit, sizeof(tt_ssl_exit_t));

    if (attr != NULL) {
        tt_memcpy(&ssl->attr, attr, sizeof(tt_ssl_attr_t));
    } else {
        tt_ssl_attr_default(&ssl->attr);
    }

    // config openssl ssl according to ts ssl if it's not a
    // listening socket
    if ((skt_role != TT_SKT_ROLE_TCP_LISTEN) &&
        !TT_OK(__openssl_ssl_config(ssl))) {
        SSL_free(new_ssl);
        return TT_FAIL;
    }

    // be sure skt is not doing io
    skt->sys_skt.on_destroy = __ssl_on_destroy;
    skt->sys_skt.on_destroy_param = ssl;
    skt->sys_skt.ssl = ssl;

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
        TT_OK(tt_async_skt_shutdown(ssl->skt, TT_SKT_SHUT_RD))) {
        result = TT_SUCCESS;
    }

    if ((mode & TT_SSL_SHUTDOWN_WR) &&
        TT_OK(tt_async_skt_shutdown(ssl->skt, TT_SKT_SHUT_WR))) {
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

tt_result_t __openssl_ssl_config(IN tt_ssl_t *ssl)
{
    SSL *openssl = ssl->sys_ssl.ssl;

    // set private data
    if (SSL_set_ex_data(openssl, tt_g_ssl_private_idx, ssl) != 1) {
        __SSL_ERROR("fail to bind ssl to openssl");
        return TT_FAIL;
    }

    SSL_set_mode(openssl, SSL_MODE_ENABLE_PARTIAL_WRITE);

    return TT_SUCCESS;
}

tt_result_t __ssl_handshake(IN tt_ssl_t *ssl, IN tt_result_t ep_result)
{
    tt_skt_ntv_t *sys_skt = &ssl->skt->sys_skt;
    if (__SKT_ROLE_OF_TCP_CONNECT(sys_skt->role)) {
        __ssl_connect_t *aio;
        tt_thread_ev_t *tev;
        tt_ev_t *ev;
        tt_result_t result;

        aio = __ssl_connect_pop(sys_skt);
        TT_ASSERT_SSL(aio != NULL);
        tev = TT_TEV_OF(aio);
        ev = TT_EV_OF(tev);

        // do handshaking only when epoll succeeds
        if (TT_OK(ep_result)) {
            result = __ssl_connect_io(aio, sys_skt);
        } else {
            result = ep_result;
        }

        if (result == TT_PROCEEDING) {
            // handshake ongoing
            TT_ASSERT_SSL(sys_skt->ssl_want_rd || sys_skt->ssl_want_wr);
            return TT_PROCEEDING;
        }

        if (TT_OK(result)) {
            aio->result = TT_SUCCESS;
        } else {
            aio->result = TT_FAIL;
            result = TT_END;
        }

        __ssl_connect_cb(aio);
        TT_ASSERT_SSL(tev->node.lst == NULL);
        // tt_list_remove(&tev->node);
        tt_ev_destroy(ev);
        return result;
    } else {
        __ssl_accept_t *aio;
        tt_thread_ev_t *tev;
        tt_ev_t *ev;
        tt_result_t result;

        aio = __ssl_accept_pop(sys_skt);
        TT_ASSERT_SSL(aio != NULL);
        tev = TT_TEV_OF(aio);
        ev = TT_EV_OF(tev);

        // do handshaking only when epoll succeeds
        if (TT_OK(ep_result)) {
            result = __ssl_accept_io(aio, sys_skt);
        } else {
            result = ep_result;
        }

        if (result == TT_PROCEEDING) {
            // handshake ongoing
            TT_ASSERT_SSL(sys_skt->ssl_want_rd || sys_skt->ssl_want_wr);
            return TT_PROCEEDING;
        }

        if (TT_OK(result)) {
            aio->result = TT_SUCCESS;
        } else {
            tt_skt_ntv_t *new_sys_skt;

            new_sys_skt = &aio->new_ssl->skt->sys_skt;
            __RETRY_IF_EINTR(close(new_sys_skt->s) != 0);
            new_sys_skt->s = -1;

            SSL_free(ssl->sys_ssl.ssl);
            ssl->sys_ssl.ssl = NULL;

            aio->result = TT_FAIL;
            result = TT_END;
        }

        __ssl_accept_cb(aio);
        TT_ASSERT_SSL(tev->node.lst == NULL);
        // tt_list_remove(&tev->node);
        tt_ev_destroy(ev);
        return result;
    }
}

void __ssl_on_destroy(IN tt_skt_t *skt, IN void *cb_param)
{
    tt_ssl_t *ssl = (tt_ssl_t *)cb_param;
    tt_ssl_exit_t *exit = &ssl->exit;
    tt_bool_t from_alloc = ssl->attr.from_alloc;

    // listening socket has null sys_ssl.ssl
    if (ssl->sys_ssl.ssl != NULL) {
        SSL_free(ssl->sys_ssl.ssl);
        ssl->sys_ssl.ssl = NULL;
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

tt_result_t __ssl_close_notify(IN tt_ssl_t *ssl)
{
    SSL *openssl = ssl->sys_ssl.ssl;
    int ret, err;

    ret = SSL_shutdown(openssl);
    if (ret >= 0) {
        return TT_SUCCESS;
    }

    err = SSL_get_error(openssl, ret);
    switch (err) {
        case SSL_ERROR_WANT_WRITE: {
            // should wait until close_notify could be sent
            return TT_PROCEEDING;
        } break;
#if 0
        case SSL_ERROR_WANT_READ:
        {
            // whether wait for close_notify is decided by caller
            return TT_SUCCESS;
        }
        break;
#endif

        default: {
            __SSL_ERROR_STRING(err);
            return TT_FAIL;
        } break;
    }
}

__ssl_accept_t *__ssl_accept_pop(IN tt_skt_ntv_t *sys_skt)
{
    tt_lnode_t *node;
    tt_thread_ev_t *tev;

    TT_ASSERT_SSL(__SKT_ROLE_OF_TCP_ACCEPT(sys_skt->role));
    TT_ASSERT_SSL(sys_skt->ssl_rd_handshaking);
    TT_ASSERT_SSL(sys_skt->ssl_wr_handshaking);
    TT_ASSERT_SSL(sys_skt->ssl_want_rd || sys_skt->ssl_want_wr);

    if (sys_skt->ssl_want_rd) {
        node = tt_list_pop_head(&sys_skt->read_q);
    } else {
        node = tt_list_pop_head(&sys_skt->write_q);
    }
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

    tt_skt_ntv_t *listening_sys_skt = &listening_skt->sys_skt;
    tt_skt_ntv_t *new_sys_skt = &new_skt->sys_skt;

    tt_sslpeer_t *sslpeer;
    tt_ssl_t *new_ssl = aio->new_ssl;
    tt_sslctx_t *sslctx = aio->listening_ssl->sslctx;

    TT_ASSERT_SSL(listening_sys_skt->ssl == aio->listening_ssl);
    TT_ASSERT_SSL(listening_sys_skt == &aio->listening_ssl->skt->sys_skt);

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

#if 0 // openssl server does not need SSL_set_session
    if (new_ssl->attr.session_resume) {
        // try session resume
        sslpeer = tt_sslcache_find(&sslctx->ssl_cache,
                                    sslctx,
                                    tt_skt_local_addr(new_skt),
                                    tt_skt_remote_addr(new_skt),
                                    new_ssl->session_mark);
        if (sslpeer != NULL) {
            if (SSL_set_session(new_ssl->sys_ssl.ssl, 
                                sslpeer->sys_peer.openssl_session) != 1) {
                __SSL_ERROR("fail to set ssl session");
            }

            tt_sslpeer_release(sslpeer);
        }
    }
#endif

    // start ssl handshake
    new_sys_skt->ssl_rd_handshaking = TT_TRUE;
    new_sys_skt->ssl_wr_handshaking = TT_TRUE;

    if (__ssl_accept_io(aio, new_sys_skt) != TT_PROCEEDING) {
        // if not TT_PROCEEDING, __ssl_accept_io must fail
        __RETRY_IF_EINTR(close(new_sys_skt->s) != 0);
        new_sys_skt->s = -1;

        SSL_free(aio->new_ssl->sys_ssl.ssl);
        aio->new_ssl->sys_ssl.ssl = NULL;

        aio->result = TT_FAIL;
        __ssl_accept_cb(aio);
        tt_ev_destroy(ev);
        return;
    }
    TT_ASSERT_SSL(new_sys_skt->ssl_want_rd || new_sys_skt->ssl_want_wr);
}

tt_result_t __ssl_accept_io(IN __ssl_accept_t *aio, IN tt_skt_ntv_t *sys_skt)
{
    SSL *ssl = aio->new_ssl->sys_ssl.ssl;
    int ret;
    tt_thread_ev_t *tev = TT_TEV_OF(aio);

    TT_ASSERT_SSL(sys_skt == &aio->new_ssl->skt->sys_skt);
    TT_ASSERT_SSL(sys_skt->ssl_rd_handshaking);
    TT_ASSERT_SSL(sys_skt->ssl_wr_handshaking);

    ret = SSL_accept(ssl);
    if (ret == 1) {
        return TT_SUCCESS;
    } else {
        int err = SSL_get_error(ssl, ret);
        switch (err) {
            case SSL_ERROR_WANT_READ: {
                tt_list_push_head(&sys_skt->read_q, &tev->node);
                sys_skt->ssl_want_rd = TT_TRUE;
                sys_skt->ssl_want_wr = TT_FALSE;

                __skt_start_epoll(sys_skt);
                return TT_PROCEEDING;
            } break;
            case SSL_ERROR_WANT_WRITE: {
                tt_list_push_head(&sys_skt->write_q, &tev->node);
                sys_skt->ssl_want_rd = TT_FALSE;
                sys_skt->ssl_want_wr = TT_TRUE;

                __skt_start_epoll(sys_skt);
                return TT_PROCEEDING;
            } break;

            default: {
                __SSL_ERROR_STRING(err);
                return TT_FAIL;
            } break;
        }
    }
}

void __ssl_accept_cb(IN __ssl_accept_t *aio)
{
    tt_ssl_aioctx_t ssl_env;
    tt_skt_ntv_t *sys_skt = &aio->new_ssl->skt->sys_skt;

    ssl_env.result = aio->result;
    ssl_env.cb_param = aio->cb_param;

    // if something failed in __ssl_on_accept, then aio->new_ssl->skt
    // may be a null pointer
    if (aio->new_ssl->skt != NULL) {
        tt_skt_ntv_t *sys_skt = &aio->new_ssl->skt->sys_skt;

        sys_skt->ssl_rd_handshaking = TT_FALSE;
        sys_skt->ssl_want_rd = TT_FALSE;

        sys_skt->ssl_wr_handshaking = TT_FALSE;
        sys_skt->ssl_want_wr = TT_FALSE;
    }

    if (TT_OK(ssl_env.result)) {
        tt_ssl_t *ssl = aio->new_ssl;
        tt_sslctx_t *sslctx = ssl->sslctx;

#if 0 // openssl server does not need it
        if (ssl->attr.session_resume) {
            SSL_SESSION *openssl_session = \
                SSL_get1_session(ssl->sys_ssl.ssl);
            tt_sslpeer_ntv_t sys_peer = { openssl_session };
            
            if (openssl_session != NULL) {
                // cache the session if it's established successfully
                tt_sslcache_add(&sslctx->ssl_cache,
                                 sslctx,
                                 tt_skt_local_addr(ssl->skt),
                                 tt_skt_remote_addr(ssl->skt),
                                 ssl->session_mark,
                                 &sys_peer);
            } else {
                __SSL_ERROR("fail to get ssl session");
            }
        }
#endif
    }

    aio->on_accept(aio->listening_ssl, aio->new_ssl, &ssl_env);
    // note aio->new_ssl not be valid now if ssl_env->result is not
    // TT_SUCCESS
}

__ssl_connect_t *__ssl_connect_pop(IN tt_skt_ntv_t *sys_skt)
{
    tt_lnode_t *node;
    tt_thread_ev_t *tev;

    TT_ASSERT_SSL(__SKT_ROLE_OF_TCP_CONNECT(sys_skt->role));
    TT_ASSERT_SSL(sys_skt->ssl_rd_handshaking);
    TT_ASSERT_SSL(sys_skt->ssl_wr_handshaking);
    TT_ASSERT_SSL(sys_skt->ssl_want_rd || sys_skt->ssl_want_wr);

    if (sys_skt->ssl_want_rd) {
        node = tt_list_pop_head(&sys_skt->read_q);
    } else {
        node = tt_list_pop_head(&sys_skt->write_q);
    }
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

    tt_skt_ntv_t *sys_skt = &skt->sys_skt;

    tt_ssl_t *ssl = aio->ssl;
    tt_sslctx_t *sslctx = ssl->sslctx;
    tt_sslpeer_t *sslpeer;

    TT_ASSERT_SSL(sys_skt->ssl == aio->ssl);
    TT_ASSERT_SSL(sys_skt == &aio->ssl->skt->sys_skt);

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
            if (SSL_set_session(ssl->sys_ssl.ssl,
                                sslpeer->sys_peer.openssl_session) != 1) {
                __SSL_ERROR("fail to set ssl session");
            }

            tt_sslpeer_release(sslpeer);
        }
    }

    sys_skt->ssl_rd_handshaking = TT_TRUE;
    sys_skt->ssl_wr_handshaking = TT_TRUE;

    if (__ssl_connect_io(aio, sys_skt) != TT_PROCEEDING) {
        // if not TT_PROCEEDING, __ssl_connect_io must fail
        aio->result = aioctx->result;
        __ssl_connect_cb(aio);
        tt_ev_destroy(ev);
        return;
    }
    TT_ASSERT_SSL(sys_skt->ssl_want_rd || sys_skt->ssl_want_wr);
}

tt_result_t __ssl_connect_io(IN __ssl_connect_t *aio, IN tt_skt_ntv_t *sys_skt)
{
    SSL *ssl = aio->ssl->sys_ssl.ssl;
    int ret;
    tt_thread_ev_t *tev = TT_TEV_OF(aio);

    TT_ASSERT_SSL(sys_skt == &aio->ssl->skt->sys_skt);
    TT_ASSERT_SSL(sys_skt->ssl_rd_handshaking);
    TT_ASSERT_SSL(sys_skt->ssl_wr_handshaking);

    ret = SSL_connect(ssl);
    if (ret == 1) {
        return TT_SUCCESS;
    } else {
        int err = SSL_get_error(ssl, ret);
        switch (err) {
            case SSL_ERROR_WANT_READ: {
                tt_list_push_head(&sys_skt->read_q, &tev->node);
                sys_skt->ssl_want_rd = TT_TRUE;
                sys_skt->ssl_want_wr = TT_FALSE;

                __skt_start_epoll(sys_skt);
                return TT_PROCEEDING;
            } break;
            case SSL_ERROR_WANT_WRITE: {
                tt_list_push_head(&sys_skt->write_q, &tev->node);
                sys_skt->ssl_want_rd = TT_FALSE;
                sys_skt->ssl_want_wr = TT_TRUE;

                __skt_start_epoll(sys_skt);
                return TT_PROCEEDING;
            } break;

            default: {
                __SSL_ERROR_STRING(err);
                return TT_FAIL;
            } break;
        }
    }
}

void __ssl_connect_cb(IN __ssl_connect_t *aio)
{
    tt_ssl_aioctx_t ssl_env;
    tt_skt_ntv_t *sys_skt = &aio->ssl->skt->sys_skt;

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
            SSL_SESSION *openssl_session = SSL_get1_session(ssl->sys_ssl.ssl);

            // cache the session if it's established successfully
            if (openssl_session != NULL) {
                tt_sktaddr_t local_addr;
                tt_sslpeer_ntv_t sys_peer = {openssl_session};

                tt_skt_local_addr(ssl->skt, &local_addr);

                tt_sslcache_add(&sslctx->ssl_cache,
                                sslctx,
                                &local_addr,
                                &aio->remote_addr,
                                ssl->session_mark,
                                &sys_peer);
            } else {
                __SSL_ERROR("fail to get ssl session");
            }
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

#endif
