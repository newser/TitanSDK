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

#define _GNU_SOURCE

#include <tt_socket_aio_native.h>

#include <event/tt_event_center.h>
#include <event/tt_event_poller.h>
#include <init/tt_profile.h>
#include <io/tt_socket_aio.h>
#include <memory/tt_memory_alloc.h>
#include <misc/tt_assert.h>
#include <network/ssl/tt_ssl.h>

#include <tt_cstd_api.h>
#include <tt_sys_error.h>
#include <tt_util_native.h>

#include <fcntl.h>
#include <sys/event.h>
#include <sys/uio.h>
#include <unistd.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __SKTAIO_CHECK_EVC

#define __SKTAIO_DEBUG_FLAG

#ifdef __SKTAIO_DEBUG_FLAG
#define __SKTAIO_DEBUG_FLAG_DECLARE tt_u32_t __debug_flag;
#define __SKTAIO_DEBUG_FLAG_SET(aio, flag) (aio)->__debug_flag = (flag)
#define __SKTAIO_DEBUG_FLAG_OR(aio, n) (aio)->__debug_flag |= 1 << (n)
#else
#define __SKTAIO_DEBUG_FLAG_DECLARE
#define __SKTAIO_DEBUG_FLAG_SET(aio, flag)
#define __SKTAIO_DEBUG_FLAG_OR(aio, n)
#endif

#define TT_ASSERT_SKTAIO TT_ASSERT

#define __INLINE_SBUF_NUM 4
#define __SBUF_SIZE(n) ((tt_u32_t)(sizeof(__sbuf_t) * (n)))

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

enum
{
    EV_SKTAIO_START =
        TT_EV_MKID_SUB(TT_EV_RANGE_INTERNAL_SOCKET, TT_SKT_RANGE_AIO, 0),

    EV_SKT_CONNECT,
    EV_SKT_ACCEPT,
    EV_SKT_SEND,
    EV_SKT_RECV,
    EV_SKT_SENDTO,
    EV_SKT_RECVFROM,
    EV_SKT_SHUTDOWN_WRITE,
    EV_SKT_SHUTDOWN_READ,
    EV_SKT_DESTROY,

    EV_SKTAIO_END,
};

typedef union
{
    struct iovec w;
    tt_blob_t t;
} __sbuf_t;

typedef struct
{
    struct iovec *iov;
    tt_u32_t iov_num;
    tt_u32_t cur_idx;
    tt_u32_t cur_pos;
    tt_u32_t io_bytes;
} __sbuf_state_t;

typedef struct __skt_shutdown_s
{
    tt_skt_t *skt;
} __skt_shutdown_t;

typedef struct
{
    tt_skt_t *skt;
} __skt_destroy_t;

typedef struct
{
    tt_skt_t *skt;
    tt_sktaddr_t remote_addr;
    tt_skt_on_connect_t on_connect;

    tt_result_t result;
    void *cb_param;
} __skt_connect_t;

typedef struct
{
    tt_skt_t *listening_skt;
    tt_skt_t *new_skt;
    tt_skt_on_accept_t on_accept;

    tt_result_t result;
    void *cb_param;
} __skt_accept_t;

typedef struct
{
    tt_skt_t *skt;
    __sbuf_state_t sbuf_state;
    __sbuf_t sbuf[__INLINE_SBUF_NUM];
    tt_skt_on_send_t on_send;

    tt_result_t result;
    void *cb_param;
} __skt_send_t;

typedef struct
{
    tt_skt_t *skt;
    __sbuf_state_t sbuf_state;
    __sbuf_t sbuf[__INLINE_SBUF_NUM];
    tt_skt_on_recv_t on_recv;

    tt_result_t result;
    void *cb_param;

    __SKTAIO_DEBUG_FLAG_DECLARE
} __skt_recv_t;

typedef struct
{
    tt_skt_t *skt;
    __sbuf_state_t sbuf_state;
    __sbuf_t sbuf[__INLINE_SBUF_NUM];
    tt_u32_t total_len;
    tt_sktaddr_t remote_addr;
    tt_skt_on_sendto_t on_sendto;

    tt_result_t result;
    void *cb_param;
} __skt_sendto_t;

typedef struct
{
    tt_skt_t *skt;
    __sbuf_state_t sbuf_state;
    __sbuf_t sbuf[__INLINE_SBUF_NUM];
    tt_skt_on_recvfrom_t on_recvfrom;
    tt_sktaddr_t remote_addr;
    socklen_t addr_len;

    tt_result_t result;
    void *cb_param;
} __skt_recvfrom_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

extern tt_result_t __handle_skt_attr(IN tt_skt_t *skt,
                                     IN tt_net_family_t family,
                                     IN tt_net_protocol_t protocol,
                                     IN tt_skt_attr_t *attr);

extern tt_result_t __ssl_handshake(IN struct tt_ssl_s *ssl,
                                   IN tt_result_t kq_result);
extern tt_result_t __ssl_close_notify(IN struct tt_ssl_s *ssl);
extern tt_result_t __ssl_destroy_aio(IN tt_thread_ev_t *tev);

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

// send
static void __skt_send_on_destroy(IN struct tt_ev_s *ev);

static tt_ev_itf_t __skt_send_itf = {
    NULL, __skt_send_on_destroy,
};

// recv
static void __skt_recv_on_destroy(IN struct tt_ev_s *ev);

static tt_ev_itf_t __skt_recv_itf = {
    NULL, __skt_recv_on_destroy,
};

// sendto
static void __skt_sendto_on_destroy(IN struct tt_ev_s *ev);

static tt_ev_itf_t __skt_sendto_itf = {
    NULL, __skt_sendto_on_destroy,
};

// recvfrom
static void __skt_recvfrom_on_destroy(IN struct tt_ev_s *ev);

static tt_ev_itf_t __skt_recvfrom_itf = {
    NULL, __skt_recvfrom_on_destroy,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#define close __close_socket
static int __close_socket(int skt);

static tt_result_t __async_skt_create(OUT tt_skt_t *skt,
                                      IN tt_net_family_t family,
                                      IN tt_net_protocol_t protocol,
                                      IN tt_u32_t role,
                                      IN OPT tt_skt_attr_t *attr,
                                      IN tt_skt_exit_t *exit,
                                      IN tt_u32_t flag);
#define __ASC_ACCEPT (1 << 0)

// connect
static tt_bool_t __do_skt_connect(IN __skt_connect_t *aio,
                                  IN tt_result_t kq_result);
static void __do_skt_connect_cb(IN __skt_connect_t *aio);

// accept
static tt_bool_t __do_skt_accept(IN __skt_accept_t *aio,
                                 IN tt_result_t kq_result);
static void __do_skt_accept_cb(IN __skt_accept_t *aio);

// send
static tt_bool_t __do_skt_send(IN __skt_send_t *aio, IN tt_result_t kq_result);
static tt_result_t __do_skt_send_io(IN __skt_send_t *aio);
static tt_result_t __do_skt_send_io_ssl(IN __skt_send_t *aio);
static void __do_skt_send_cb(IN __skt_send_t *aio);

// recv
static tt_bool_t __do_skt_recv(IN __skt_recv_t *aio, IN tt_result_t kq_result);
static tt_result_t __do_skt_recv_io(IN __skt_recv_t *aio);
static tt_result_t __do_skt_recv_io_ssl(IN __skt_recv_t *aio);
static void __do_skt_recv_cb(IN __skt_recv_t *aio);

// sendto
static tt_bool_t __do_skt_sendto(IN __skt_sendto_t *aio,
                                 IN tt_result_t kq_result);
static tt_result_t __do_skt_sendto_io(IN __skt_sendto_t *aio);
static void __do_skt_sendto_cb(IN __skt_sendto_t *aio);

// recvfrom
static tt_bool_t __do_skt_recvfrom(IN __skt_recvfrom_t *aio,
                                   IN tt_result_t kq_result);
static tt_result_t __do_skt_recvfrom_io(IN __skt_recvfrom_t *aio);
static void __do_skt_recvfrom_cb(IN __skt_recvfrom_t *aio);

// destroy
static void __do_skt_destroy(IN __skt_destroy_t *aio);

// shutdown
static tt_result_t __skt_shutdown_wr(IN tt_skt_t *skt);
static tt_bool_t __do_skt_shutdown_wr(IN __skt_shutdown_t *aio,
                                      IN tt_result_t kq_result);

static tt_result_t __skt_shutdown_rd(IN tt_skt_t *skt);
static tt_bool_t __do_skt_shutdown_rd(IN __skt_shutdown_t *aio,
                                      IN tt_result_t kq_result);

// aio q
static void __do_skt_aio_q(IN tt_skt_ntv_t *sys_skt,
                           IN tt_list_t *aio_q,
                           IN tt_result_t kq_result);
static void __clear_skt_aio_q(IN tt_skt_ntv_t *sys_skt,
                              IN tt_list_t *aio_q,
                              IN tt_result_t aio_result);

// misc
static void __iovec2blob_inline(IN struct iovec *iovec, IN tt_u32_t iov_num);

static tt_bool_t __update_sbuf_state(IN __sbuf_state_t *sbuf_state,
                                     IN tt_u32_t io_bytes);

// ========================================
// simulate failure
// ========================================

#ifdef __SIMULATE_SKT_AIO_FAIL

#define tt_evc_sendto_thread tt_evc_sendto_thread_SF
static tt_result_t tt_evc_sendto_thread_SF(IN tt_evcenter_t *evc,
                                           IN tt_thread_ev_t *tev);

#define sendmsg sendmsg_SF
static ssize_t sendmsg_SF(int sockfd, const struct msghdr *msg, int flags);

#define recvmsg recvmsg_SF
static ssize_t recvmsg_SF(int sockfd, struct msghdr *msg, int flags);

#define kevent kevent_SF
int kevent_SF(int kq,
              const struct kevent *changelist,
              int nchanges,
              struct kevent *eventlist,
              int nevents,
              const struct timespec *timeout);

#endif

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_skt_aio_component_init_ntv(IN tt_profile_t *profile)
{
    return TT_SUCCESS;
}

tt_result_t tt_async_skt_create_ntv(OUT tt_skt_t *skt,
                                    IN tt_net_family_t family,
                                    IN tt_net_protocol_t protocol,
                                    IN tt_u32_t role,
                                    IN OPT tt_skt_attr_t *attr,
                                    IN tt_skt_exit_t *exit)
{
    return __async_skt_create(skt, family, protocol, role, attr, exit, 0);
}

void tt_async_skt_destroy_ntv(IN tt_skt_t *skt, IN tt_bool_t immediate)
{
    tt_skt_ntv_t *sys_skt;

    TT_ASSERT(skt != NULL);

    sys_skt = &skt->sys_skt;
    if (immediate) {
        tt_skt_on_destroy_t on_destroy =
            (tt_skt_on_destroy_t)sys_skt->on_destroy;
        tt_bool_t from_alloc = skt->attr.from_alloc;

        __clear_skt_aio_q(sys_skt, &sys_skt->read_q, TT_CANCELLED);
        __clear_skt_aio_q(sys_skt, &sys_skt->write_q, TT_CANCELLED);
        if (sys_skt->s != -1) {
            __RETRY_IF_EINTR(close(sys_skt->s) != 0);
            sys_skt->s = -1;
        }

        on_destroy(skt, sys_skt->on_destroy_param);
        // do not access skt from now on as application may freed it

        if (from_alloc) {
            tt_free(skt);
        }
    } else {
        tt_evcenter_t *evc = sys_skt->evc;
        tt_ev_t *ev;
        __skt_destroy_t *aio;

#ifdef __SKTAIO_CHECK_EVC
        if (evc != tt_evc_current()) {
            TT_ERROR("different socket evc");
            return;
        }
#endif

        if (sys_skt->rd_closing && sys_skt->wr_closing) {
            TT_ERROR("already being closed");
            return;
        }

        // aio
        ev = tt_thread_ev_create(EV_SKT_DESTROY, sizeof(__skt_destroy_t), NULL);
        if (ev == NULL) {
            TT_ERROR("fail to allocate destroy aio");
            return;
        }
        aio = TT_EV_DATA(ev, __skt_destroy_t);

        // init aio
        aio->skt = skt;

        // post aio as a thread tev rather than appending to
        // read/write queue, or the process would crash
        if (!TT_OK(tt_evc_sendto_thread(evc, ev))) {
            TT_ERROR("fail to post skt destroy");

            tt_ev_destroy(ev);
            return;
        }

        sys_skt->rd_closing = TT_TRUE;
        sys_skt->wr_closing = TT_TRUE;
    }
}

tt_result_t __async_skt_create(OUT tt_skt_t *skt,
                               IN tt_net_family_t family,
                               IN tt_net_protocol_t protocol,
                               IN tt_u32_t role,
                               IN OPT tt_skt_attr_t *attr,
                               IN tt_skt_exit_t *exit,
                               IN tt_u32_t flag)
{
    tt_skt_attr_t __attr;
    tt_evcenter_t *evc;
    tt_skt_ntv_t *sys_skt;

    int __skt = -1;
    int af;
    int type;
    int __protocol;
    long __skt_flags;
    int nosigpipe = 1;

    // ========================================
    // check params
    // ========================================

    TT_ASSERT(skt != NULL);
    TT_ASSERT(TT_NET_AF_VALID(family));
    TT_ASSERT(TT_NET_PROTO_VALID(protocol));
    TT_ASSERT(TT_SKT_ROLE_VALID(role));

    TT_ASSERT(exit != NULL);
    TT_ASSERT(exit->on_destroy != NULL);

    // attributes
    if (attr == NULL) {
        tt_skt_attr_default(&__attr);
        attr = &__attr;
    }

    // evc
    evc = tt_evc_current();
    if (evc == NULL) {
        TT_ERROR("not in evc");
        return TT_BAD_PARAM;
    }

    // ========================================
    // create system socket
    // ========================================

    sys_skt = &skt->sys_skt;

    // convert family
    if (family == TT_NET_AF_INET) {
        af = AF_INET;
    } else if (family == TT_NET_AF_INET6) {
        af = AF_INET6;
    } else {
        TT_ERROR("invalid family: %d", family);
        return TT_FAIL;
    }

    // convert type
    if (protocol == TT_NET_PROTO_TCP) {
        TT_ASSERT_SKTAIO(__SKT_ROLE_OF_TCP(role));

        type = SOCK_STREAM;
        __protocol = IPPROTO_TCP;
    } else if (protocol == TT_NET_PROTO_UDP) {
        TT_ASSERT_SKTAIO(__SKT_ROLE_OF_UDP(role));

        type = SOCK_DGRAM;
        __protocol = IPPROTO_UDP;
    } else {
        TT_ERROR("invalid protocol: %d", protocol);
        return TT_FAIL;
    }

    if (flag & __ASC_ACCEPT) {
        __skt = -1;
    } else {
        // create socket
        __skt = socket(af, type, __protocol);
        if (__skt == -1) {
            TT_ERROR_NTV("fail to create socket");
            return TT_FAIL;
        }
        do {
            tt_s64_t skt_num = tt_atomic_s64_inc(&tt_skt_stat_num);

            // the peek value is not accurate, just for reference
            if (skt_num > tt_atomic_s64_get(&tt_skt_stat_peek)) {
                tt_atomic_s64_set(&tt_skt_stat_peek, skt_num);
            }
        } while (0);

        // never use SIGPIPE
        if (setsockopt(__skt,
                       SOL_SOCKET,
                       SO_NOSIGPIPE,
                       &nosigpipe,
                       sizeof(nosigpipe)) != 0) {
            TT_ERROR_NTV("fail to set SO_NOSIGPIPE");

            __RETRY_IF_EINTR(close(__skt) != 0);
            __skt = -1;
            return TT_FAIL;
        }

        // set socket nonblock
        if (((__skt_flags = fcntl(__skt, F_GETFL, 0)) == -1) ||
            (fcntl(__skt, F_SETFL, __skt_flags | O_NONBLOCK) == -1)) {
            TT_ERROR_NTV("fail to set socket nonblock");

            __RETRY_IF_EINTR(close(__skt) != 0);
            __skt = -1;
            return TT_FAIL;
        }

        // set close-on-exec
        if (((__skt_flags = fcntl(__skt, F_GETFD, 0)) == -1) ||
            (fcntl(__skt, F_SETFD, __skt_flags | FD_CLOEXEC) == -1)) {
            TT_ERROR_NTV("fail to set socket close-on-exec");

            __RETRY_IF_EINTR(close(__skt) != 0);
            __skt = -1;
            return TT_FAIL;
        }

        // add to kqueue
        if (!TT_OK(__skt_kq_rd_add(evc->sys_evc.kq_fd,
                                   __skt,
                                   &sys_skt->ev_mark_rd)) ||
            !TT_OK(__skt_kq_wr_add(evc->sys_evc.kq_fd,
                                   __skt,
                                   &sys_skt->ev_mark_wr))) {
            TT_ERROR_NTV("fail to add socket to kqueue");

            __RETRY_IF_EINTR(close(__skt) != 0);
            __skt = -1;
            return TT_FAIL;
        }
    }

    sys_skt->s = __skt;
    sys_skt->evc = evc;
    sys_skt->ev_mark_rd = TT_SKT_MARK_KQ_RD;
    sys_skt->ev_mark_wr = TT_SKT_MARK_KQ_WR;
    sys_skt->role = role;

    sys_skt->on_destroy = exit->on_destroy;
    sys_skt->on_destroy_param = exit->cb_param;

    tt_list_init(&sys_skt->read_q);
    tt_list_init(&sys_skt->write_q);

    // left securty 0
    sys_skt->ssl = NULL;

    sys_skt->rd_closing = TT_FALSE;
    sys_skt->rd_closed = TT_FALSE;
    sys_skt->ssl_rd_handshaking = TT_FALSE;
    sys_skt->ssl_want_rd = TT_FALSE;
    sys_skt->wr_closing = TT_FALSE;
    sys_skt->wr_closed = TT_FALSE;
    sys_skt->ssl_wr_handshaking = TT_FALSE;
    sys_skt->ssl_want_wr = TT_FALSE;
    sys_skt->connecting = TT_FALSE;
    sys_skt->connected = TT_FALSE;

    // ========================================
    // create tt socket
    // ========================================

    // save data
    skt->family = family;
    skt->protocol = protocol;
    tt_memcpy(&skt->attr, attr, sizeof(tt_skt_attr_t));

    if (!(flag & __ASC_ACCEPT)) {
        // process attributes
        if (!TT_OK(__handle_skt_attr(skt, family, protocol, attr))) {
            __RETRY_IF_EINTR(close(sys_skt->s) != 0);
            sys_skt->s = -1;
            return TT_FAIL;
        }
    }
    // else the attribute would be set in __do_skt_accept

    // socket would not be put to kqueue until some aio occurs
    return TT_SUCCESS;
}

tt_result_t tt_async_skt_shutdown_ntv(IN tt_skt_t *skt, IN tt_u32_t mode)
{
    TT_ASSERT(skt != NULL);
    TT_ASSERT(TT_NET_PROTO_VALID(skt->protocol));

#ifdef __SKTAIO_CHECK_EVC
    if (skt->sys_skt.evc != tt_evc_current()) {
        TT_ERROR("different socket evc");
        return TT_BAD_PARAM;
    }
#endif

    if (!__SKT_ROLE_OF_TCP_DATA(skt->sys_skt.role)) {
        TT_ERROR("not tcp data socket");
        return TT_FAIL;
    }

    if (mode == TT_SKT_SHUT_WR) {
        return __skt_shutdown_wr(skt);
    } else if (mode == TT_SKT_SHUT_RD) {
        return __skt_shutdown_rd(skt);
    } else {
        tt_result_t sd_wr, sd_rd;

        TT_ASSERT(mode == TT_SKT_SHUT_RDWR);

        sd_wr = __skt_shutdown_wr(skt);
        sd_rd = __skt_shutdown_rd(skt);
        if (TT_OK(sd_wr) || TT_OK(sd_rd)) {
            return TT_SUCCESS;
        } else {
            return TT_FAIL;
        }
    }
}

tt_result_t tt_skt_accept_async_ntv(IN tt_skt_t *listening_skt,
                                    IN tt_skt_t *new_skt,
                                    IN OPT tt_skt_attr_t *new_skt_attr,
                                    IN tt_skt_exit_t *new_skt_exit,
                                    IN tt_skt_on_accept_t on_accept,
                                    IN OPT void *cb_param)
{
    tt_skt_ntv_t *sys_skt;
    tt_skt_ntv_t *new_sys_skt;
    tt_skt_attr_t __new_skt_attr;
    tt_ev_t *ev;
    tt_thread_ev_t *tev;
    __skt_accept_t *aio;

    TT_ASSERT(listening_skt != NULL);
    TT_ASSERT(TT_NET_AF_VALID(listening_skt->family));

    TT_ASSERT(new_skt != NULL);
    TT_ASSERT(new_skt_exit != NULL);
    TT_ASSERT(new_skt_exit->on_destroy != NULL);

    TT_ASSERT(on_accept != NULL);

    sys_skt = &listening_skt->sys_skt;
    new_sys_skt = &new_skt->sys_skt;

#ifdef __SKTAIO_CHECK_EVC
    if (sys_skt->evc != tt_evc_current()) {
        TT_ERROR("different socket evc");
        return TT_BAD_PARAM;
    }
#endif

    if (listening_skt->protocol != TT_NET_PROTO_TCP) {
        TT_ERROR("not tcp socket to accept");
        return TT_FAIL;
    }
    if (sys_skt->role != TT_SKT_ROLE_TCP_LISTEN) {
        TT_ERROR("seems not a listener socket");
        return TT_FAIL;
    }
    if (sys_skt->rd_closing) {
        TT_ERROR("skt is being closed");
        return TT_BAD_PARAM;
    }

    // attribute of new socket
    if (new_skt_attr == NULL) {
        tt_skt_attr_default(&__new_skt_attr);
        new_skt_attr = &__new_skt_attr;
    }

    // init new sys socket
    if (!TT_OK(__async_skt_create(new_skt,
                                  listening_skt->family,
                                  TT_NET_PROTO_TCP,
                                  TT_SKT_ROLE_TCP_ACCEPT,
                                  new_skt_attr,
                                  new_skt_exit,
                                  __ASC_ACCEPT))) {
        TT_ERROR("fail to init new skt");
        return TT_FAIL;
    }

    // aio
    ev = tt_thread_ev_create(EV_SKT_ACCEPT, sizeof(__skt_accept_t), NULL);
    if (ev == NULL) {
        TT_ERROR("fail to allocate aio");
        return TT_FAIL;
    }
    tev = TT_EV_HDR(ev, tt_thread_ev_t);
    aio = TT_EV_DATA(ev, __skt_accept_t);

    // init aio
    aio->listening_skt = listening_skt;
    aio->new_skt = new_skt;
    aio->on_accept = on_accept;

    aio->result = TT_PROCEEDING;
    aio->cb_param = cb_param;

    // add request
    tt_list_push_tail(&sys_skt->read_q, &tev->node);

    // start kqueue if it's head aio
    if ((tt_list_count(&sys_skt->read_q) == 1) &&
        !TT_OK(__skt_kq_rd_enable(sys_skt))) {
        tt_list_remove(&tev->node);
        tt_ev_destroy(ev);
        return TT_FAIL;
    }
    return TT_SUCCESS;
}

tt_result_t tt_skt_connect_async_ntv(IN tt_skt_t *skt,
                                     IN tt_sktaddr_t *remote_addr,
                                     IN tt_skt_on_connect_t on_connect,
                                     IN OPT void *cb_param)
{
    tt_skt_ntv_t *sys_skt;
    tt_ev_t *ev;
    tt_thread_ev_t *tev;
    __skt_connect_t *aio;

    TT_ASSERT(skt != NULL);
    TT_ASSERT(TT_NET_AF_VALID(skt->family));
    TT_ASSERT(remote_addr != NULL);
    TT_ASSERT(on_connect != NULL);

    sys_skt = &skt->sys_skt;

#ifdef __SKTAIO_CHECK_EVC
    if (sys_skt->evc != tt_evc_current()) {
        TT_ERROR("different socket evc");
        return TT_BAD_PARAM;
    }
#endif

    if (skt->protocol != TT_NET_PROTO_TCP) {
        TT_ERROR("not tcp socket to connect");
        return TT_FAIL;
    }
    if (!__SKT_ROLE_OF_TCP_CONNECT(sys_skt->role)) {
        TT_ERROR("seems not a client socket");
        return TT_FAIL;
    }
    if (sys_skt->wr_closing) {
        TT_ERROR("skt is being closed");
        return TT_BAD_PARAM;
    }
    if (sys_skt->connecting) {
        TT_ERROR("socket is connecting");
        return TT_FAIL;
    }

conn_ag:
    if (connect(sys_skt->s,
                (const struct sockaddr *)remote_addr,
                TT_COND(skt->family == TT_NET_AF_INET,
                        sizeof(struct sockaddr_in),
                        sizeof(struct sockaddr_in6))) != 0) {
        if (errno == EINTR) {
            goto conn_ag;
        } else if (errno != EINPROGRESS) {
            TT_ERROR_NTV("fail to connect");
            return TT_FAIL;
        }
    }
    // now connect is ongoing(or done...)

    // aio
    ev = tt_thread_ev_create(EV_SKT_CONNECT, sizeof(__skt_connect_t), NULL);
    if (ev == NULL) {
        TT_ERROR("fail to allocate aio");
        return TT_FAIL;
    }
    tev = TT_EV_HDR(ev, tt_thread_ev_t);
    aio = TT_EV_DATA(ev, __skt_connect_t);

    // init aio
    aio->skt = skt;
    tt_memcpy(&aio->remote_addr, remote_addr, sizeof(tt_sktaddr_t));
    aio->on_connect = on_connect;

    aio->result = TT_PROCEEDING;
    aio->cb_param = cb_param;

    // add request
    tt_list_push_tail(&sys_skt->write_q, &tev->node);

    // start kqueue if it's head aio
    if ((tt_list_count(&sys_skt->write_q) == 1) &&
        !TT_OK(__skt_kq_wr_enable(sys_skt))) {
        tt_list_remove(&tev->node);
        tt_ev_destroy(ev);
        return TT_FAIL;
    }

    sys_skt->connecting = TT_TRUE;
    return TT_SUCCESS;
}

tt_result_t tt_skt_send_async_ntv(IN tt_skt_t *skt,
                                  IN tt_blob_t *blob,
                                  IN tt_u32_t blob_num,
                                  IN tt_skt_on_send_t on_send,
                                  IN OPT void *cb_param)
{
    tt_skt_ntv_t *sys_skt;
    tt_ev_t *ev;
    tt_thread_ev_t *tev;
    __skt_send_t *aio;
    __sbuf_state_t *sbuf_state;
    tt_u32_t i;

    TT_ASSERT(skt != NULL);
    TT_ASSERT(TT_NET_AF_VALID(skt->family));
    TT_ASSERT(blob != NULL);
    TT_ASSERT(blob_num != 0);

    sys_skt = &skt->sys_skt;

#ifdef __SKTAIO_CHECK_EVC
    if (sys_skt->evc != tt_evc_current()) {
        TT_ERROR("different socket evc");
        return TT_BAD_PARAM;
    }
#endif

    if (skt->protocol != TT_NET_PROTO_TCP) {
        TT_ERROR("send is only for tcp socket");
        return TT_FAIL;
    }
    if (!__SKT_ROLE_OF_TCP_DATA(sys_skt->role)) {
        TT_ERROR("socket is not for transferring data");
        return TT_FAIL;
    }
    if (sys_skt->wr_closing) {
        TT_ERROR("skt is being closed");
        return TT_BAD_PARAM;
    }
    if (!sys_skt->connected) {
        TT_ERROR("skt is not connected");
        return TT_BAD_PARAM;
    }

    for (i = 0; i < blob_num; ++i) {
        if ((blob[i].addr == NULL) || (blob[i].len == 0)) {
            TT_ERROR("invalid buffer, none to write");
            return TT_FAIL;
        }
    }

    // aio
    ev =
        tt_thread_ev_create(EV_SKT_SEND, sizeof(__skt_send_t), &__skt_send_itf);
    if (ev == NULL) {
        TT_ERROR("fail to allocate aio");
        return TT_FAIL;
    }
    tev = TT_EV_HDR(ev, tt_thread_ev_t);
    aio = TT_EV_DATA(ev, __skt_send_t);
    sbuf_state = &aio->sbuf_state;

    // init aio
    aio->skt = skt;

    sbuf_state->iov_num = blob_num;
    if (blob_num <= __INLINE_SBUF_NUM) {
        sbuf_state->iov = (struct iovec *)aio->sbuf;
    } else {
        sbuf_state->iov = (struct iovec *)tt_malloc(__SBUF_SIZE(blob_num));
        if (sbuf_state->iov == NULL) {
            TT_ERROR("no mem for buf array");

            tt_free(ev);
            return TT_FAIL;
        }
    }
    for (i = 0; i < blob_num; ++i) {
        sbuf_state->iov[i].iov_base = blob[i].addr;
        sbuf_state->iov[i].iov_len = (size_t)blob[i].len;
    }
    sbuf_state->cur_idx = 0;
    sbuf_state->cur_pos = 0;
    sbuf_state->io_bytes = 0;

    aio->on_send = on_send;

    aio->result = TT_PROCEEDING;
    aio->cb_param = cb_param;

    // add request
    tt_list_push_tail(&sys_skt->write_q, &tev->node);

    // start kqueue if it's head aio
    if ((tt_list_count(&sys_skt->write_q) == 1) &&
        !TT_OK(__skt_kq_wr_enable(sys_skt))) {
        tt_list_remove(&tev->node);
        tt_ev_destroy(ev);
        return TT_FAIL;
    }
    return TT_SUCCESS;
}

tt_result_t tt_skt_recv_async_ntv(IN tt_skt_t *skt,
                                  IN tt_blob_t *blob,
                                  IN tt_u32_t blob_num,
                                  IN tt_skt_on_recv_t on_recv,
                                  IN OPT void *cb_param)
{
    tt_skt_ntv_t *sys_skt;
    tt_ev_t *ev;
    tt_thread_ev_t *tev;
    __skt_recv_t *aio;
    __sbuf_state_t *sbuf_state;
    tt_u32_t i;

    TT_ASSERT(skt != NULL);
    TT_ASSERT(TT_NET_AF_VALID(skt->family));
    TT_ASSERT(blob != NULL);
    TT_ASSERT(blob_num != 0);

    sys_skt = &skt->sys_skt;

#ifdef __SKTAIO_CHECK_EVC
    if (sys_skt->evc != tt_evc_current()) {
        TT_ERROR("different socket evc");
        return TT_BAD_PARAM;
    }
#endif

    if (skt->protocol != TT_NET_PROTO_TCP) {
        TT_ERROR("recv is only for tcp socket");
        return TT_FAIL;
    }
    if (!__SKT_ROLE_OF_TCP_DATA(sys_skt->role)) {
        TT_ERROR("socket is not for transferring data");
        return TT_FAIL;
    }
    if (sys_skt->rd_closing) {
        TT_ERROR("skt is being closed");
        return TT_BAD_PARAM;
    }
    if (!sys_skt->connected) {
        TT_ERROR("skt is not connected");
        return TT_BAD_PARAM;
    }

    for (i = 0; i < blob_num; ++i) {
        if ((blob[i].addr == NULL) || (blob[i].len == 0)) {
            TT_ERROR("invalid buffer, none to write");
            return TT_FAIL;
        }
    }

    // aio
    ev =
        tt_thread_ev_create(EV_SKT_RECV, sizeof(__skt_recv_t), &__skt_recv_itf);
    if (ev == NULL) {
        TT_ERROR("fail to allocate aio");
        return TT_FAIL;
    }
    tev = TT_EV_HDR(ev, tt_thread_ev_t);
    aio = TT_EV_DATA(ev, __skt_recv_t);
    sbuf_state = &aio->sbuf_state;

    // init aio
    aio->skt = skt;

    sbuf_state->iov_num = blob_num;
    if (blob_num <= __INLINE_SBUF_NUM) {
        sbuf_state->iov = (struct iovec *)aio->sbuf;
    } else {
        sbuf_state->iov = (struct iovec *)tt_malloc(__SBUF_SIZE(blob_num));
        if (sbuf_state->iov == NULL) {
            TT_ERROR("no mem for buf array");

            tt_free(ev);
            return TT_FAIL;
        }
    }
    for (i = 0; i < blob_num; ++i) {
        sbuf_state->iov[i].iov_base = blob[i].addr;
        sbuf_state->iov[i].iov_len = (size_t)blob[i].len;
    }
    sbuf_state->cur_idx = 0;
    sbuf_state->cur_pos = 0;
    sbuf_state->io_bytes = 0;

    aio->on_recv = on_recv;

    aio->result = TT_PROCEEDING;
    aio->cb_param = cb_param;

    __SKTAIO_DEBUG_FLAG_SET(aio, 0);

    // add request
    tt_list_push_tail(&sys_skt->read_q, &tev->node);

    // start kqueue if it's head aio
    if (tt_list_count(&sys_skt->read_q) == 1) {
#ifdef TT_PLATFORM_SSL_ENABLE
        if (sys_skt->ssl != NULL) {
            size_t bufSize = 0;

            if (SSLGetBufferedReadSize(sys_skt->ssl->sys_ssl.ssl_ref,
                                       &bufSize) != noErr) {
                tt_list_remove(&tev->node);
                tt_ev_destroy(ev);
                return TT_FAIL;
            }

            // if there are buffered ssl data, we should not polling
            // socket, or the aio would pend if no new data come
            if (bufSize > 0) {
                if (TT_OK(tt_evc_sendto_thread(sys_skt->evc, ev))) {
                    return TT_SUCCESS;
                } else {
                    tt_list_remove(&tev->node);
                    tt_ev_destroy(ev);
                    return TT_FAIL;
                }
            }
            // else to poll socket
        }
#endif

        if (!TT_OK(__skt_kq_rd_enable(sys_skt))) {
            tt_list_remove(&tev->node);
            tt_ev_destroy(ev);
            return TT_FAIL;
        }
    }
    return TT_SUCCESS;
}

tt_result_t tt_skt_sendto_async_ntv(IN tt_skt_t *skt,
                                    IN tt_blob_t *blob,
                                    IN tt_u32_t blob_num,
                                    IN tt_sktaddr_t *remote_addr,
                                    IN tt_skt_on_sendto_t on_sendto,
                                    IN OPT void *cb_param)
{
    tt_skt_ntv_t *sys_skt;
    tt_ev_t *ev;
    tt_thread_ev_t *tev;
    __skt_sendto_t *aio;
    __sbuf_state_t *sbuf_state;
    tt_u32_t i;

    TT_ASSERT(skt != NULL);
    TT_ASSERT(TT_NET_AF_VALID(skt->family));
    TT_ASSERT(blob != NULL);
    TT_ASSERT(blob_num != 0);
    TT_ASSERT(remote_addr != 0);

    sys_skt = &skt->sys_skt;

#ifdef __SKTAIO_CHECK_EVC
    if (sys_skt->evc != tt_evc_current()) {
        TT_ERROR("different socket evc");
        return TT_BAD_PARAM;
    }
#endif

    if (skt->protocol != TT_NET_PROTO_UDP) {
        TT_ERROR("sendto is only for udp socket");
        return TT_FAIL;
    }
    if (!__SKT_ROLE_OF_UDP(sys_skt->role)) {
        TT_ERROR("incorrect socket role: %d", sys_skt->role);
        return TT_FAIL;
    }
    if (sys_skt->wr_closing) {
        TT_ERROR("skt is being closed");
        return TT_BAD_PARAM;
    }

    for (i = 0; i < blob_num; ++i) {
        if ((blob[i].addr == NULL) || (blob[i].len == 0)) {
            TT_ERROR("invalid buffer, none to sendto");
            return TT_FAIL;
        }
    }

    // aio
    ev = tt_thread_ev_create(EV_SKT_SENDTO,
                             sizeof(__skt_sendto_t),
                             &__skt_sendto_itf);
    if (ev == NULL) {
        TT_ERROR("fail to allocate aio");
        return TT_FAIL;
    }
    tev = TT_EV_HDR(ev, tt_thread_ev_t);
    aio = TT_EV_DATA(ev, __skt_sendto_t);
    sbuf_state = &aio->sbuf_state;

    // init aio
    aio->skt = skt;

    sbuf_state->iov_num = blob_num;
    if (blob_num <= __INLINE_SBUF_NUM) {
        sbuf_state->iov = (struct iovec *)aio->sbuf;
    } else {
        sbuf_state->iov = (struct iovec *)tt_malloc(__SBUF_SIZE(blob_num));
        if (sbuf_state->iov == NULL) {
            TT_ERROR("no mem for buf array");

            tt_free(ev);
            return TT_FAIL;
        }
    }
    aio->total_len = 0;
    for (i = 0; i < blob_num; ++i) {
        sbuf_state->iov[i].iov_base = blob[i].addr;
        sbuf_state->iov[i].iov_len = (size_t)blob[i].len;
        aio->total_len += (tt_u32_t)blob[i].len;
    }
    sbuf_state->cur_idx = 0;
    sbuf_state->cur_pos = 0;
    sbuf_state->io_bytes = 0;

    tt_memcpy(&aio->remote_addr, remote_addr, sizeof(tt_sktaddr_t));
    aio->on_sendto = on_sendto;

    aio->result = TT_PROCEEDING;
    aio->cb_param = cb_param;

    // add request
    tt_list_push_tail(&sys_skt->write_q, &tev->node);

    // start kqueue if it's head aio
    if ((tt_list_count(&sys_skt->write_q) == 1) &&
        !TT_OK(__skt_kq_wr_enable(sys_skt))) {
        tt_list_remove(&tev->node);
        tt_ev_destroy(ev);
        return TT_FAIL;
    }
    return TT_SUCCESS;
}

tt_result_t tt_skt_recvfrom_async_ntv(IN tt_skt_t *skt,
                                      IN tt_blob_t *blob,
                                      IN tt_u32_t blob_num,
                                      IN tt_skt_on_recvfrom_t on_recvfrom,
                                      IN OPT void *cb_param)
{
    tt_skt_ntv_t *sys_skt;
    tt_ev_t *ev;
    tt_thread_ev_t *tev;
    __skt_recvfrom_t *aio;
    __sbuf_state_t *sbuf_state;
    tt_u32_t i;

    TT_ASSERT(skt != NULL);
    TT_ASSERT(TT_NET_AF_VALID(skt->family));
    TT_ASSERT(blob != NULL);
    TT_ASSERT(blob_num != 0);

    sys_skt = &skt->sys_skt;

#ifdef __SKTAIO_CHECK_EVC
    if (sys_skt->evc != tt_evc_current()) {
        TT_ERROR("different socket evc");
        return TT_BAD_PARAM;
    }
#endif

    if (skt->protocol != TT_NET_PROTO_UDP) {
        TT_ERROR("recvfrom is only for udp socket");
        return TT_FAIL;
    }
    if (!__SKT_ROLE_OF_UDP(sys_skt->role)) {
        TT_ERROR("incorrect socket role: %d", sys_skt->role);
        return TT_FAIL;
    }
    if (sys_skt->rd_closing) {
        TT_ERROR("skt is being closed");
        return TT_BAD_PARAM;
    }

    for (i = 0; i < blob_num; ++i) {
        if ((blob[i].addr == NULL) || (blob[i].len == 0)) {
            TT_ERROR("invalid buffer, none to write");
            return TT_FAIL;
        }
    }

    // aio
    ev = tt_thread_ev_create(EV_SKT_RECVFROM,
                             sizeof(__skt_recvfrom_t),
                             &__skt_recvfrom_itf);
    if (ev == NULL) {
        TT_ERROR("fail to allocate aio");
        return TT_FAIL;
    }
    tev = TT_EV_HDR(ev, tt_thread_ev_t);
    aio = TT_EV_DATA(ev, __skt_recvfrom_t);
    sbuf_state = &aio->sbuf_state;

    // init aio
    aio->skt = skt;

    sbuf_state->iov_num = blob_num;
    if (blob_num <= __INLINE_SBUF_NUM) {
        sbuf_state->iov = (struct iovec *)aio->sbuf;
    } else {
        sbuf_state->iov = (struct iovec *)tt_malloc(__SBUF_SIZE(blob_num));
        if (sbuf_state->iov == NULL) {
            TT_ERROR("no mem for buf array");

            tt_free(ev);
            return TT_FAIL;
        }
    }
    for (i = 0; i < blob_num; ++i) {
        sbuf_state->iov[i].iov_base = blob[i].addr;
        sbuf_state->iov[i].iov_len = (size_t)blob[i].len;
    }
    sbuf_state->cur_idx = 0;
    sbuf_state->cur_pos = 0;
    sbuf_state->io_bytes = 0;

    aio->on_recvfrom = on_recvfrom;
    tt_memset(&aio->remote_addr, 0, sizeof(tt_sktaddr_t));
    aio->addr_len = sizeof(aio->remote_addr);

    aio->result = TT_PROCEEDING;
    aio->cb_param = cb_param;

    // add request
    tt_list_push_tail(&sys_skt->read_q, &tev->node);

    // start kqueue if it's head aio
    if ((tt_list_count(&sys_skt->read_q) == 1) &&
        !TT_OK(__skt_kq_rd_enable(sys_skt))) {
        tt_list_remove(&tev->node);
        tt_ev_destroy(ev);
        return TT_FAIL;
    }
    return TT_SUCCESS;
}

tt_result_t tt_skt_kq_rd_handler(IN tt_skt_t *skt,
                                 IN tt_u32_t flags,
                                 IN tt_u32_t fflags,
                                 IN tt_uintptr_t data)
{
    tt_skt_ntv_t *sys_skt = &skt->sys_skt;

    // data indicates how many bytes could be read out
    if (data != 0) {
        __do_skt_aio_q(sys_skt, &sys_skt->read_q, TT_SUCCESS);
        return TT_SUCCESS;
    }

    if (flags & EV_EOF) {
        __do_skt_aio_q(sys_skt, &sys_skt->read_q, TT_END);
        return TT_SUCCESS;
    }

    if (flags & EV_ERROR) {
        __do_skt_aio_q(sys_skt, &sys_skt->read_q, TT_FAIL);
        return TT_SUCCESS;
    }

    return TT_SUCCESS;
}

tt_result_t tt_skt_kq_wr_handler(IN tt_skt_t *skt,
                                 IN tt_u32_t flags,
                                 IN tt_u32_t fflags,
                                 IN tt_uintptr_t data)
{
    tt_skt_ntv_t *sys_skt = &skt->sys_skt;

    // data indicates how many bytes could be written
    if (data != 0) {
        __do_skt_aio_q(sys_skt, &sys_skt->write_q, TT_SUCCESS);
        return TT_SUCCESS;
    }

    if (flags & EV_ERROR) {
        __do_skt_aio_q(sys_skt, &sys_skt->write_q, TT_FAIL);
        return TT_SUCCESS;
    }

    return TT_SUCCESS;
}

tt_result_t tt_skt_tev_handler(IN struct tt_evpoller_s *evp, IN tt_ev_t *ev)
{
    TT_ASSERT(ev != NULL);
    TT_ASSERT(TT_EV_RANGE(ev->ev_id) == TT_EV_RANGE_INTERNAL_SOCKET);

    switch (ev->ev_id) {
        case EV_SKT_DESTROY: {
            __do_skt_destroy(TT_EV_DATA(ev, __skt_destroy_t));
            tt_ev_destroy(ev);
        } break;

        // posted as there are buffered ssl data to be read
        case EV_SKT_RECV: {
            __skt_recv_t *aio = TT_EV_DATA(ev, __skt_recv_t);
            tt_skt_ntv_t *sys_skt = &aio->skt->sys_skt;

            __do_skt_aio_q(sys_skt, &sys_skt->read_q, TT_SUCCESS);
            // __do_skt_aio_q will remove and destroy the recv event
        } break;

        // unknown
        default: {
            TT_ERROR("unknown event: %x", ev->ev_id);
            tt_ev_destroy(ev);
        } break;
    }
    return TT_SUCCESS;
}

tt_result_t tt_tcp_server_async_ntv(OUT tt_skt_t *skt,
                                    IN tt_net_family_t family,
                                    IN tt_skt_attr_t *attr,
                                    IN tt_sktaddr_t *local_addr,
                                    IN tt_u32_t backlog,
                                    IN tt_skt_exit_t *exit)
{
    skt->sys_skt.s = -1;

    if (!TT_OK(tt_async_skt_create(skt,
                                   family,
                                   TT_NET_PROTO_TCP,
                                   TT_SKT_ROLE_TCP_LISTEN,
                                   attr,
                                   exit))) {
        return TT_FAIL;
    }

    if (!TT_OK(tt_skt_bind(skt, local_addr))) {
        goto tssa_fail;
    }

    if (!TT_OK(tt_skt_listen(skt, backlog))) {
        goto tssa_fail;
    }

    // now could start accept()
    return TT_SUCCESS;

tssa_fail:

    // nothing except skt->sys_skt.s need be released
    if (skt->sys_skt.s != -1) {
        __RETRY_IF_EINTR(close(skt->sys_skt.s) != 0);
        skt->sys_skt.s = -1;
    }

    return TT_FAIL;
}

tt_result_t tt_udp_server_async_ntv(OUT tt_skt_t *skt,
                                    IN tt_net_family_t family,
                                    IN OPT tt_skt_attr_t *attr,
                                    IN tt_sktaddr_t *local_addr,
                                    IN tt_skt_exit_t *exit)
{
    skt->sys_skt.s = -1;

    if (!TT_OK(tt_async_skt_create(skt,
                                   family,
                                   TT_NET_PROTO_UDP,
                                   TT_SKT_ROLE_UDP,
                                   attr,
                                   exit))) {
        return TT_FAIL;
    }

    if (!TT_OK(tt_skt_bind(skt, local_addr))) {
        goto ussa_fail;
    }

    // now could start recvfrom()
    return TT_SUCCESS;

ussa_fail:

    // nothing except skt->sys_skt.s need be released
    if (skt->sys_skt.s != -1) {
        __RETRY_IF_EINTR(close(skt->sys_skt.s) != 0);
        skt->sys_skt.s = -1;
    }
    return TT_FAIL;
}

tt_result_t __skt_shutdown_wr(IN tt_skt_t *skt)
{
    tt_skt_ntv_t *sys_skt = &skt->sys_skt;
    tt_ev_t *ev;
    tt_thread_ev_t *tev;
    __skt_shutdown_t *aio;

    if (sys_skt->wr_closing) {
        // TT_ERROR("already closing write");
        return TT_BAD_PARAM;
    }

    // aio
    ev = tt_thread_ev_create(EV_SKT_SHUTDOWN_WRITE,
                             sizeof(__skt_shutdown_t),
                             NULL);
    if (ev == NULL) {
        TT_ERROR("fail to allocate shutdown wr aio");
        return TT_FAIL;
    }
    tev = TT_EV_HDR(ev, tt_thread_ev_t);
    aio = TT_EV_DATA(ev, __skt_shutdown_t);

    // it can only shutdown socket write when all write aio are
    // submitted or these aio are lost

    aio->skt = skt;

    // add request
    tt_list_push_tail(&sys_skt->write_q, &tev->node);
    sys_skt->wr_closing = TT_TRUE;

    // start kqueue if it's head aio
    if ((tt_list_count(&sys_skt->write_q) == 1) &&
        !TT_OK(__skt_kq_wr_enable(sys_skt))) {
        tt_list_remove(&tev->node);
        tt_ev_destroy(ev);
        return TT_FAIL;
    }
    return TT_SUCCESS;
}

tt_bool_t __do_skt_shutdown_wr(IN __skt_shutdown_t *aio,
                               IN tt_result_t kq_result)
{
    tt_skt_ntv_t *sys_skt = &aio->skt->sys_skt;

    // all write aio must have been done and this aio should be removed
    TT_ASSERT_SKTAIO(tt_list_count(&sys_skt->write_q) == 0);

#ifdef TT_PLATFORM_SSL_ENABLE
    if (sys_skt->ssl != NULL) {
        __ssl_close_notify(sys_skt->ssl);
    }
#endif

    // all data are sent to os, shutdown write direction
    if (shutdown(sys_skt->s, SHUT_WR) != 0) {
        if (errno == ENOTCONN) {
            // remote site closed connection, ignore it
        } else {
            // failing to shutdown SHUT_WR is possible when remote
            // side forced a connection closing
            TT_ERROR_NTV("fail to shutdown write");
        }
    }

    // mark as closed
    TT_ASSERT_SKTAIO(!sys_skt->wr_closed);
    sys_skt->wr_closed = TT_TRUE;

    // check if read q is already closed
    if (sys_skt->rd_closed) {
        tt_skt_on_destroy_t on_destroy =
            (tt_skt_on_destroy_t)sys_skt->on_destroy;
        tt_bool_t from_alloc = aio->skt->attr.from_alloc;

        __RETRY_IF_EINTR(close(sys_skt->s) != 0);
        sys_skt->s = -1;

        on_destroy(aio->skt, sys_skt->on_destroy_param);

        if (from_alloc) {
            tt_free(aio->skt);
        }
    }

    return TT_TRUE;
}

tt_result_t __skt_shutdown_rd(IN tt_skt_t *skt)
{
    tt_skt_ntv_t *sys_skt = &skt->sys_skt;
    tt_ev_t *ev;
    tt_thread_ev_t *tev;
    __skt_shutdown_t *aio;

    if (sys_skt->rd_closing) {
        // TT_ERROR("already closing read");
        return TT_BAD_PARAM;
    }

    // shutdown read direction immediately as caller does not want
    // any more data
    if (shutdown(sys_skt->s, SHUT_RD) != 0) {
        if (errno == ENOTCONN) {
            // ignore the error that remote site has closed connection
        } else {
            TT_ERROR_NTV("fail to shutdown");
            return TT_FAIL;
        }
    }

    // aio
    ev = tt_thread_ev_create(EV_SKT_SHUTDOWN_READ,
                             sizeof(__skt_shutdown_t),
                             NULL);
    if (ev == NULL) {
        TT_ERROR("fail to allocate shutdown wr aio");
        return TT_FAIL;
    }
    tev = TT_EV_HDR(ev, tt_thread_ev_t);
    aio = TT_EV_DATA(ev, __skt_shutdown_t);

    aio->skt = skt;

    // add request
    tt_list_push_tail(&sys_skt->read_q, &tev->node);
    sys_skt->rd_closing = TT_TRUE;

    // start kqueue if it's head aio
    if ((tt_list_count(&sys_skt->read_q) == 1) &&
        !TT_OK(__skt_kq_rd_enable(sys_skt))) {
        tt_list_remove(&tev->node);
        tt_ev_destroy(ev);
        return TT_FAIL;
    }
    return TT_SUCCESS;
}

tt_bool_t __do_skt_shutdown_rd(IN __skt_shutdown_t *aio,
                               IN tt_result_t kq_result)
{
    tt_skt_ntv_t *sys_skt = &aio->skt->sys_skt;

    // all read aio must have been done
    TT_ASSERT_SKTAIO(tt_list_count(&sys_skt->read_q) == 0);

    // mark as closed
    TT_ASSERT_SKTAIO(!sys_skt->rd_closed);
    sys_skt->rd_closed = TT_TRUE;

    // check if write q is already closed
    if (sys_skt->wr_closed) {
        tt_skt_on_destroy_t on_destroy =
            (tt_skt_on_destroy_t)sys_skt->on_destroy;
        tt_bool_t from_alloc = aio->skt->attr.from_alloc;

        __RETRY_IF_EINTR(close(sys_skt->s) != 0);
        sys_skt->s = -1;

        on_destroy(aio->skt, sys_skt->on_destroy_param);

        if (from_alloc) {
            tt_free(aio->skt);
        }
    }

    return TT_TRUE;
}

tt_bool_t __do_skt_accept(IN __skt_accept_t *aio, IN tt_result_t kq_result)
{
    tt_skt_t *listening_skt = aio->listening_skt;
    tt_skt_ntv_t *listening_sys_skt = &listening_skt->sys_skt;
    tt_skt_t *new_skt = aio->new_skt;
    tt_skt_ntv_t *new_sys_skt = &new_skt->sys_skt;

    struct sockaddr_storage remote_addr;
    socklen_t remote_addr_len = sizeof(struct sockaddr_storage);

    TT_ASSERT_SKTAIO(new_sys_skt->s == -1);

    if (TT_OK(kq_result)) {
        long __skt_flags;
        tt_evcenter_ntv_t *sys_evc = &new_sys_skt->evc->sys_evc;

    accept_ag:
        new_sys_skt->s = accept(listening_sys_skt->s,
                                (struct sockaddr *)&remote_addr,
                                &remote_addr_len);
        if (new_sys_skt->s == -1) {
            if (errno == EINTR) {
                goto accept_ag;
            }

            // EAGAIN or EWOULDBLOCK may occur when listening socket has
            // no new socket to accept, so left the accept aio in q
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                if (!TT_OK(__skt_kq_rd_enable(listening_sys_skt))) {
                    TT_FATAL("listening may never work");
                }
                return TT_FALSE;
            }

            TT_ERROR_NTV("fail to accept");
            aio->result = TT_FAIL;
        } else if (((__skt_flags = fcntl(new_sys_skt->s, F_GETFL, 0)) == -1) ||
                   (fcntl(new_sys_skt->s, F_SETFL, __skt_flags | O_NONBLOCK) ==
                    -1)) {
            TT_ERROR_NTV("fail to set accepted socket nonblock");
            aio->result = TT_FAIL;
        } else if (!TT_OK(__handle_skt_attr(new_skt,
                                            new_skt->family,
                                            TT_NET_PROTO_TCP,
                                            &new_skt->attr))) {
            TT_ERROR("fail to set accepted socket attribute");
            aio->result = TT_FAIL;
        } else if (!TT_OK(__skt_kq_rd_add(sys_evc->kq_fd,
                                          new_sys_skt->s,
                                          &new_sys_skt->ev_mark_rd)) ||
                   !TT_OK(__skt_kq_wr_add(sys_evc->kq_fd,
                                          new_sys_skt->s,
                                          &new_sys_skt->ev_mark_wr))) {
            TT_ERROR_NTV("fail to add socket to kqueue");
            aio->result = TT_FAIL;
        } else {
            aio->result = TT_SUCCESS;
            tt_skt_stat_inc_num();
        }
    } else {
        aio->result = kq_result;
    }
    TT_ASSERT_SKTAIO(aio->result != TT_PROCEEDING);

    if (!TT_OK(aio->result)) {
        if (new_sys_skt->s != -1) {
            __RETRY_IF_EINTR(close(new_sys_skt->s) != 0);
            new_sys_skt->s = -1;
        }
    }

    __do_skt_accept_cb(aio);
    return TT_TRUE;
}

void __do_skt_accept_cb(IN __skt_accept_t *aio)
{
    tt_skt_aioctx_t aioctx;
    tt_skt_t *new_skt = aio->new_skt;
    tt_bool_t from_alloc = new_skt->attr.from_alloc;

    new_skt->sys_skt.connected = TT_BOOL(TT_OK(aio->result));

    // callback
    aioctx.result = aio->result;
    aioctx.cb_param = aio->cb_param;
    aio->on_accept(aio->listening_skt, aio->new_skt, &aioctx);

    if (!TT_OK(aioctx.result) && from_alloc) {
        tt_free(new_skt);
    }
}

tt_bool_t __do_skt_connect(IN __skt_connect_t *aio, IN tt_result_t kq_result)
{
    // connecting is done when socket becomes writable
    aio->result = kq_result;

    __do_skt_connect_cb(aio);
    return TT_TRUE;
}

void __do_skt_connect_cb(IN __skt_connect_t *aio)
{
    tt_skt_t *skt = aio->skt;
    tt_skt_aioctx_t aioctx;

    TT_ASSERT(aio->result != TT_PROCEEDING);

    // connecting done, success or fail
    skt->sys_skt.connecting = TT_FALSE;
    skt->sys_skt.connected = TT_BOOL(TT_OK(aio->result));

    // callback
    aioctx.result = aio->result;
    aioctx.cb_param = aio->cb_param;
    aio->on_connect(aio->skt, &aio->remote_addr, &aioctx);
}

tt_bool_t __do_skt_send(IN __skt_send_t *aio, IN tt_result_t kq_result)
{
    if (TT_OK(kq_result)) {
#ifdef TT_PLATFORM_SSL_ENABLE
        if (aio->skt->sys_skt.ssl != NULL) {
            aio->result = __do_skt_send_io_ssl(aio);
        } else {
            aio->result = __do_skt_send_io(aio);
        }
#else
        aio->result = __do_skt_send_io(aio);
#endif
    } else {
        aio->result = kq_result;
    }

    if (aio->result != TT_PROCEEDING) {
        __do_skt_send_cb(aio);
        return TT_TRUE;
    } else {
        return TT_FALSE;
    }
}

tt_result_t __do_skt_send_io(IN __skt_send_t *aio)
{
    tt_skt_ntv_t *sys_skt = &aio->skt->sys_skt;
    __sbuf_state_t *sbuf_state = &aio->sbuf_state;

    // do until pending
    while (sbuf_state->cur_idx < sbuf_state->iov_num) {
        struct iovec *cur_v = &sbuf_state->iov[sbuf_state->cur_idx];
        struct iovec *iov;
        int iov_cnt;
        struct iovec __iov;
        ssize_t write_num = 0;

        struct msghdr msg;

        TT_ASSERT_SKTAIO(sbuf_state->cur_pos < cur_v->iov_len);

        // init iov
        if ((sbuf_state->cur_idx == 0) && (sbuf_state->cur_pos == 0)) {
            iov = sbuf_state->iov;
            iov_cnt = sbuf_state->iov_num;
        } else {
            __iov.iov_base =
                TT_PTR_INC(char, cur_v->iov_base, sbuf_state->cur_pos);
            __iov.iov_len = cur_v->iov_len - sbuf_state->cur_pos;

            iov = &__iov;
            iov_cnt = 1;
        }

        // init msg
        msg.msg_name = NULL;
        msg.msg_namelen = 0;
        msg.msg_iov = iov;
        msg.msg_iovlen = iov_cnt;
        msg.msg_control = NULL;
        msg.msg_controllen = 0;
        msg.msg_flags = 0;

        write_num = sendmsg(sys_skt->s, &msg, 0);
        if (write_num > 0) {
            __update_sbuf_state(sbuf_state, (tt_u32_t)write_num);
            continue;
            // while loop would check if all are written
        } else if (write_num == 0) {
            TT_ERROR("send 0 bytes");
            return TT_END;
        } else {
            if (errno == EINTR) {
                continue;
            } else if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                // can not send now, start polling
                if (TT_OK(__skt_kq_wr_enable(sys_skt))) {
                    return TT_PROCEEDING;
                } else {
                    return TT_FAIL;
                }
            } else if ((errno == ECONNRESET) || (errno == EPIPE)
                       // || (errno == ENETDOWN)
                       ) {
                return TT_END;
            } else {
                TT_ERROR_NTV("fail to send");
                return TT_FAIL;
            }
        }
    }
    TT_ASSERT_SKTAIO(sbuf_state->cur_idx == sbuf_state->iov_num);
    return TT_SUCCESS;
}

#ifdef TT_PLATFORM_SSL_ENABLE
tt_result_t __do_skt_send_io_ssl(IN __skt_send_t *aio)
{
    tt_skt_ntv_t *sys_skt = &aio->skt->sys_skt;
    __sbuf_state_t *sbuf_state = &aio->sbuf_state;

    // do until pending
    while (sbuf_state->cur_idx < sbuf_state->iov_num) {
        struct iovec *cur_v = &sbuf_state->iov[sbuf_state->cur_idx];
        tt_u8_t *pos;
        tt_u32_t len;
        OSStatus err;
        size_t write_num = 0;

        TT_ASSERT_SKTAIO(sbuf_state->cur_pos < cur_v->iov_len);

        pos = TT_PTR_INC(tt_u8_t, cur_v->iov_base, sbuf_state->cur_pos);
        len = (tt_u32_t)(cur_v->iov_len - sbuf_state->cur_pos);

        err = SSLWrite(sys_skt->ssl->sys_ssl.ssl_ref, pos, len, &write_num);
        if ((err == noErr) || (err == errSSLWouldBlock)) {
            if (write_num > 0) {
                __update_sbuf_state(sbuf_state, (tt_u32_t)write_num);
                continue;
                // while loop would check if all are written
            } else {
                // can not send now, start polling
                if (TT_OK(__skt_kq_wr_enable(sys_skt))) {
                    return TT_PROCEEDING;
                } else {
                    return TT_FAIL;
                }
            }
        } else if ((err == errSSLClosedGraceful) ||
                   (err == errSSLClosedNoNotify)) {
            return TT_END;
        } else {
            return TT_FAIL;
        }
    }
    TT_ASSERT_SKTAIO(sbuf_state->cur_idx == sbuf_state->iov_num);
    return TT_SUCCESS;
}
#endif

void __do_skt_send_cb(IN __skt_send_t *aio)
{
    tt_skt_aioctx_t aioctx;
    __sbuf_state_t *sbuf_state = &aio->sbuf_state;

    if (sbuf_state->io_bytes > 0) {
        aio->result = TT_SUCCESS;
    }

    __iovec2blob_inline(sbuf_state->iov, sbuf_state->iov_num);

    // callback
    aioctx.result = aio->result;
    aioctx.cb_param = aio->cb_param;
    aio->on_send(aio->skt,
                 (tt_blob_t *)sbuf_state->iov,
                 sbuf_state->iov_num,
                 &aioctx,
                 sbuf_state->io_bytes);
}

void __skt_send_on_destroy(IN struct tt_ev_s *ev)
{
    __skt_send_t *aio = TT_EV_DATA(ev, __skt_send_t);
    __sbuf_state_t *sbuf_state = &aio->sbuf_state;

    if (sbuf_state->iov != (struct iovec *)aio->sbuf) {
        tt_free(sbuf_state->iov);
    }
}

tt_bool_t __do_skt_recv(IN __skt_recv_t *aio, IN tt_result_t kq_result)
{
    if (TT_OK(kq_result)) {
#ifdef TT_PLATFORM_SSL_ENABLE
        if (aio->skt->sys_skt.ssl != NULL) {
            aio->result = __do_skt_recv_io_ssl(aio);
        } else {
            aio->result = __do_skt_recv_io(aio);
        }
#else
        aio->result = __do_skt_recv_io(aio);
#endif
    } else {
        aio->result = kq_result;
    }

    if (aio->result != TT_PROCEEDING) {
        __do_skt_recv_cb(aio);
        return TT_TRUE;
    } else {
        return TT_FALSE;
    }
}

tt_result_t __do_skt_recv_io(IN __skt_recv_t *aio)
{
    tt_skt_ntv_t *sys_skt = &aio->skt->sys_skt;
    __sbuf_state_t *sbuf_state = &aio->sbuf_state;

    // do until io_pending
    while (sbuf_state->cur_idx < sbuf_state->iov_num) {
        struct iovec *cur_v = &sbuf_state->iov[sbuf_state->cur_idx];
        struct iovec *iov;
        int iov_cnt;
        struct iovec __iov;
        ssize_t read_num = 0;

        struct msghdr msg;

        TT_ASSERT_SKTAIO(sbuf_state->cur_pos < cur_v->iov_len);

        // init iov
        if ((sbuf_state->cur_idx == 0) && (sbuf_state->cur_pos == 0)) {
            iov = sbuf_state->iov;
            iov_cnt = sbuf_state->iov_num;
        } else {
            __iov.iov_len = cur_v->iov_len - sbuf_state->cur_pos;
            __iov.iov_base =
                TT_PTR_INC(char, cur_v->iov_base, sbuf_state->cur_pos);

            iov = &__iov;
            iov_cnt = 1;
        }

        // init msg
        msg.msg_name = NULL;
        msg.msg_namelen = 0;
        msg.msg_iov = iov;
        msg.msg_iovlen = iov_cnt;
        msg.msg_control = NULL;
        msg.msg_controllen = 0;
        msg.msg_flags = 0;

        read_num = recvmsg(sys_skt->s, &msg, 0);
        if (read_num > 0) {
            __update_sbuf_state(sbuf_state, (tt_u32_t)read_num);
            continue;
            // while loop would check if all are written
        } else if (read_num == 0) {
            return TT_END;
        } else {
            if (errno == EINTR) {
                continue;
            } else if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                // some data are received and not a full recv
                if (sbuf_state->io_bytes > 0) {
                    return TT_SUCCESS;
                }

                // here it's a full receive or no data received, so keep
                // doing io
                if (TT_OK(__skt_kq_rd_enable(sys_skt))) {
                    return TT_PROCEEDING;
                } else {
                    return TT_FAIL;
                }
            } else if (errno == ECONNRESET
                       // || (errno == ENETDOWN)
                       ) {
                return TT_END;
            } else {
                TT_ERROR_NTV("fail to read");
                return TT_FAIL;
            }
        }
    }
    TT_ASSERT_SKTAIO(sbuf_state->cur_idx == sbuf_state->iov_num);
    return TT_SUCCESS;
}

#ifdef TT_PLATFORM_SSL_ENABLE
tt_result_t __do_skt_recv_io_ssl(IN __skt_recv_t *aio)
{
    tt_skt_ntv_t *sys_skt = &aio->skt->sys_skt;
    __sbuf_state_t *sbuf_state = &aio->sbuf_state;

    // do until io_pending
    while (sbuf_state->cur_idx < sbuf_state->iov_num) {
        struct iovec *cur_v = &sbuf_state->iov[sbuf_state->cur_idx];
        tt_u8_t *pos;
        tt_u32_t len;
        OSStatus err;
        size_t read_num = 0;

        TT_ASSERT_SKTAIO(sbuf_state->cur_pos < cur_v->iov_len);

        pos = TT_PTR_INC(tt_u8_t, cur_v->iov_base, sbuf_state->cur_pos);
        len = (tt_u32_t)(cur_v->iov_len - sbuf_state->cur_pos);

        err = SSLRead(sys_skt->ssl->sys_ssl.ssl_ref, pos, len, &read_num);
        if (err == noErr) {
            if (read_num > 0) {
                __update_sbuf_state(sbuf_state, (tt_u32_t)read_num);
                continue;
                // while loop would check if all are written
            } else {
                return TT_END;
            }
        } else if (err == errSSLWouldBlock) {
            if (read_num > 0) {
                __update_sbuf_state(sbuf_state, (tt_u32_t)read_num);
                continue;
                // while loop would check if all are written
            } else {
                // some data are received and not a full recv
                if (sbuf_state->io_bytes > 0) {
                    return TT_SUCCESS;
                }

                // here it's a full receive or no data received, so keep
                // doing io
                if (TT_OK(__skt_kq_rd_enable(sys_skt))) {
                    return TT_PROCEEDING;
                } else {
                    return TT_FAIL;
                }
            }
        } else if ((err == errSSLClosedGraceful) ||
                   (err == errSSLClosedNoNotify)) {
            return TT_END;
        } else {
            return TT_FAIL;
        }
    }
    TT_ASSERT_SKTAIO(sbuf_state->cur_idx == sbuf_state->iov_num);
    return TT_SUCCESS;
}
#endif

void __do_skt_recv_cb(IN __skt_recv_t *aio)
{
    tt_skt_aioctx_t aioctx;
    __sbuf_state_t *sbuf_state = &aio->sbuf_state;

    if (sbuf_state->io_bytes > 0) {
        aio->result = TT_SUCCESS;
    }

    __iovec2blob_inline(sbuf_state->iov, sbuf_state->iov_num);

    // callback
    aioctx.result = aio->result;
    aioctx.cb_param = aio->cb_param;
    aio->on_recv(aio->skt,
                 (tt_blob_t *)sbuf_state->iov,
                 sbuf_state->iov_num,
                 &aioctx,
                 sbuf_state->io_bytes);
}

void __skt_recv_on_destroy(IN struct tt_ev_s *ev)
{
    __skt_recv_t *aio = TT_EV_DATA(ev, __skt_recv_t);
    __sbuf_state_t *sbuf_state = &aio->sbuf_state;

    if (sbuf_state->iov != (struct iovec *)aio->sbuf) {
        tt_free(sbuf_state->iov);
    }
}

tt_bool_t __do_skt_sendto(IN __skt_sendto_t *aio, IN tt_result_t kq_result)
{
    if (TT_OK(kq_result)) {
        aio->result = __do_skt_sendto_io(aio);
    } else {
        aio->result = kq_result;
    }

    if (aio->result != TT_PROCEEDING) {
        __do_skt_sendto_cb(aio);
        return TT_TRUE;
    } else {
        return TT_FALSE;
    }
}

tt_result_t __do_skt_sendto_io(IN __skt_sendto_t *aio)
{
    tt_skt_ntv_t *sys_skt = &aio->skt->sys_skt;
    __sbuf_state_t *sbuf_state = &aio->sbuf_state;

    // do until io_pending
    while (sbuf_state->cur_idx < sbuf_state->iov_num) {
        struct iovec *cur_v = &sbuf_state->iov[sbuf_state->cur_idx];
        struct iovec *iov;
        int iov_cnt;
        struct iovec __iov;

        struct msghdr msg;
        ssize_t write_num;

        TT_ASSERT_SKTAIO(sbuf_state->cur_pos < cur_v->iov_len);

        // init iov
        if ((sbuf_state->cur_idx == 0) && (sbuf_state->cur_pos == 0)) {
            iov = sbuf_state->iov;
            iov_cnt = sbuf_state->iov_num;
        } else {
            __iov.iov_len = cur_v->iov_len - sbuf_state->cur_pos;
            __iov.iov_base =
                TT_PTR_INC(char, cur_v->iov_base, sbuf_state->cur_pos);

            iov = &__iov;
            iov_cnt = 1;
        }

        // init msg
        msg.msg_name = &aio->remote_addr;
        msg.msg_namelen = aio->remote_addr.ss_len;
        msg.msg_iov = iov;
        msg.msg_iovlen = iov_cnt;
        msg.msg_control = NULL;
        msg.msg_controllen = 0;
        msg.msg_flags = 0;

        write_num = sendmsg(sys_skt->s, &msg, 0);
        if (write_num > 0) {
            __update_sbuf_state(sbuf_state, (tt_u32_t)write_num);
            // while loop would check if all are written

            // sent length is expected to be the total aio length
            if (write_num != aio->total_len) {
                TT_WARN("total packet: %d bytes, sent: %d",
                        aio->total_len,
                        write_num);
                // return error?
            }

            continue;
        } else if (write_num == 0) {
            return TT_END;
        } else {
            if (errno == EINTR) {
                continue;
            } else if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                // can not send now, start polling
                if (TT_OK(__skt_kq_wr_enable(sys_skt))) {
                    return TT_PROCEEDING;
                } else {
                    return TT_FAIL;
                }
            } else if ((errno == ECONNRESET) || (errno == EPIPE)
                       // || (errno == ENETDOWN)
                       ) {
                return TT_END;
            } else {
                TT_ERROR_NTV("fail to send");
                return TT_FAIL;
            }
        }
    }
    TT_ASSERT_SKTAIO(sbuf_state->cur_idx == sbuf_state->iov_num);
    return TT_SUCCESS;
}

void __do_skt_sendto_cb(IN __skt_sendto_t *aio)
{
    tt_skt_aioctx_t aioctx;
    __sbuf_state_t *sbuf_state = &aio->sbuf_state;

    if (sbuf_state->io_bytes > 0) {
        aio->result = TT_SUCCESS;
    }

    __iovec2blob_inline(sbuf_state->iov, sbuf_state->iov_num);

    // callback
    aioctx.result = aio->result;
    aioctx.cb_param = aio->cb_param;
    aio->on_sendto(aio->skt,
                   (tt_blob_t *)sbuf_state->iov,
                   sbuf_state->iov_num,
                   &aio->remote_addr,
                   &aioctx,
                   sbuf_state->io_bytes);
}

void __skt_sendto_on_destroy(IN struct tt_ev_s *ev)
{
    __skt_sendto_t *aio = TT_EV_DATA(ev, __skt_sendto_t);
    __sbuf_state_t *sbuf_state = &aio->sbuf_state;

    if (sbuf_state->iov != (struct iovec *)aio->sbuf) {
        tt_free(sbuf_state->iov);
    }
}

tt_bool_t __do_skt_recvfrom(IN __skt_recvfrom_t *aio, IN tt_result_t kq_result)
{
    if (TT_OK(kq_result)) {
        aio->result = __do_skt_recvfrom_io(aio);
    } else {
        aio->result = kq_result;
    }

    if (aio->result != TT_PROCEEDING) {
        __do_skt_recvfrom_cb(aio);
        return TT_TRUE;
    } else {
        return TT_FALSE;
    }
}

tt_result_t __do_skt_recvfrom_io(IN __skt_recvfrom_t *aio)
{
    tt_skt_ntv_t *sys_skt = &aio->skt->sys_skt;
    __sbuf_state_t *sbuf_state = &aio->sbuf_state;

    struct iovec *cur_v = &sbuf_state->iov[sbuf_state->cur_idx];
    struct iovec *iov;
    int iov_cnt;
    struct iovec __iov;

    struct msghdr msg;
    ssize_t read_num;

    TT_ASSERT_SKTAIO(sbuf_state->cur_pos < cur_v->iov_len);

    // init iov
    if ((sbuf_state->cur_idx == 0) && (sbuf_state->cur_pos == 0)) {
        iov = sbuf_state->iov;
        iov_cnt = sbuf_state->iov_num;
    } else {
        __iov.iov_len = cur_v->iov_len - sbuf_state->cur_pos;
        __iov.iov_base = TT_PTR_INC(char, cur_v->iov_base, sbuf_state->cur_pos);

        iov = &__iov;
        iov_cnt = 1;
    }

    // init msg
    msg.msg_name = &aio->remote_addr;
    msg.msg_namelen = sizeof(struct sockaddr_storage);
    msg.msg_iov = iov;
    msg.msg_iovlen = iov_cnt;
    msg.msg_control = NULL;
    msg.msg_controllen = 0;
    msg.msg_flags = 0;

urf_retry:
    read_num = recvmsg(sys_skt->s, &msg, 0);
    if (read_num > 0) {
        __update_sbuf_state(sbuf_state, (tt_u32_t)read_num);

        if (msg.msg_flags & MSG_TRUNC) {
            TT_WARN("udp packet truncated");
            // return TT_FAIL;
        }

        return TT_SUCCESS;
    } else if (read_num == 0) {
        return TT_END;
    } else {
        if (errno == EINTR) {
            goto urf_retry;
        } else if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
            TT_ASSERT_SKTAIO(sbuf_state->io_bytes == 0);

            if (TT_OK(__skt_kq_rd_enable(sys_skt))) {
                return TT_PROCEEDING;
            } else {
                return TT_FAIL;
            }
        } else if (errno == ECONNRESET
                   // || (errno == ENETDOWN)
                   ) {
            return TT_END;
        } else {
            TT_ERROR_NTV("fail to read");
            return TT_FAIL;
        }
    }
}

void __do_skt_recvfrom_cb(IN __skt_recvfrom_t *aio)
{
    tt_skt_aioctx_t aioctx;
    __sbuf_state_t *sbuf_state = &aio->sbuf_state;

    if (sbuf_state->io_bytes > 0) {
        aio->result = TT_SUCCESS;
    }

    __iovec2blob_inline(sbuf_state->iov, sbuf_state->iov_num);

    // callback
    aioctx.result = aio->result;
    aioctx.cb_param = aio->cb_param;
    aio->on_recvfrom(aio->skt,
                     (tt_blob_t *)sbuf_state->iov,
                     sbuf_state->iov_num,
                     &aioctx,
                     sbuf_state->io_bytes,
                     &aio->remote_addr);
}

void __skt_recvfrom_on_destroy(IN struct tt_ev_s *ev)
{
    __skt_recvfrom_t *aio = TT_EV_DATA(ev, __skt_recvfrom_t);
    __sbuf_state_t *sbuf_state = &aio->sbuf_state;

    if (sbuf_state->iov != (struct iovec *)aio->sbuf) {
        tt_free(sbuf_state->iov);
    }
}

void __do_skt_destroy(IN __skt_destroy_t *aio)
{
    tt_skt_t *skt = aio->skt;
    tt_skt_ntv_t *sys_skt = &skt->sys_skt;
    tt_skt_on_destroy_t on_destroy = (tt_skt_on_destroy_t)sys_skt->on_destroy;
    tt_bool_t from_alloc = skt->attr.from_alloc;

    __clear_skt_aio_q(sys_skt, &sys_skt->read_q, TT_CANCELLED);
    __clear_skt_aio_q(sys_skt, &sys_skt->write_q, TT_CANCELLED);
    __RETRY_IF_EINTR(close(sys_skt->s) != 0);
    sys_skt->s = -1;

    on_destroy(aio->skt, sys_skt->on_destroy_param);
    // do not access aio->skt from now on as application may freed aio->skt

    if (from_alloc) {
        tt_free(skt);
    }
}

void __do_skt_aio_q(IN tt_skt_ntv_t *sys_skt,
                    IN tt_list_t *aio_q,
                    IN tt_result_t kq_result)
{
    tt_lnode_t *node;

#ifdef TT_PLATFORM_SSL_ENABLE
    if (sys_skt->ssl_rd_handshaking || sys_skt->ssl_wr_handshaking) {
        tt_result_t hsk_result = __ssl_handshake(sys_skt->ssl, kq_result);
        if (TT_OK(hsk_result)) {
            if ((aio_q == &sys_skt->read_q) &&
                !tt_list_empty(&sys_skt->read_q)) {
                __skt_kq_rd_enable(sys_skt);
            }

            if ((aio_q == &sys_skt->write_q) &&
                !tt_list_empty(&sys_skt->write_q)) {
                __skt_kq_wr_enable(sys_skt);
            }

            // can not do io directly, or read aio would read 0 byte and
            // the socket is thus considered shutdown
            return;
        } else if (hsk_result == TT_PROCEEDING) {
            return;
        } else {
            kq_result = TT_END;
        }
    }
#endif

    while ((node = tt_list_head(aio_q)) != NULL) {
        tt_thread_ev_t *tev = TT_CONTAINER(node, tt_thread_ev_t, node);
        tt_ev_t *ev = TT_EV_OF(tev);
        tt_bool_t aio_done = TT_TRUE;

        switch (ev->ev_id) {
            case EV_SKT_SHUTDOWN_WRITE: {
                __skt_shutdown_t *aio = TT_EV_DATA(ev, __skt_shutdown_t);

                // it should be the head aio in write q
                TT_ASSERT_SKTAIO(&tev->node ==
                                 tt_list_head(&aio->skt->sys_skt.write_q));

                // __do_skt_shutdown_wr may destroy socket
                tt_list_remove(&tev->node);
                __do_skt_shutdown_wr(aio, kq_result);
                // do not access aio from now on as it's already freed

                // no more aio in write q and skt should not be accessed
                tt_ev_destroy(ev);
                return;
            } break;
            case EV_SKT_SHUTDOWN_READ: {
                __skt_shutdown_t *aio = TT_EV_DATA(ev, __skt_shutdown_t);

                // it should be the head aio in read q
                TT_ASSERT_SKTAIO(&tev->node ==
                                 tt_list_head(&aio->skt->sys_skt.read_q));

                // __do_skt_shutdown_wr may destroy socket
                tt_list_remove(&tev->node);
                __do_skt_shutdown_rd(aio, kq_result);
                // do not access aio from now on as it's already freed

                // no more aio in read q and skt should not be accessed
                tt_ev_destroy(ev);
                return;
            } break;
            case EV_SKT_ACCEPT: {
                __skt_accept_t *aio = TT_EV_DATA(ev, __skt_accept_t);

                // it should be the head aio in read q
                TT_ASSERT_SKTAIO(
                    &tev->node ==
                    tt_list_head(&aio->listening_skt->sys_skt.read_q));

                aio_done = __do_skt_accept(aio, kq_result);
            } break;
            case EV_SKT_CONNECT: {
                __skt_connect_t *aio = TT_EV_DATA(ev, __skt_connect_t);

                // it should be the head aio in write q
                TT_ASSERT_SKTAIO(&tev->node ==
                                 tt_list_head(&aio->skt->sys_skt.write_q));

                aio_done = __do_skt_connect(aio, kq_result);
            } break;
            case EV_SKT_SEND: {
                __skt_send_t *aio = TT_EV_DATA(ev, __skt_send_t);

                TT_ASSERT_SKTAIO(&tev->node ==
                                 tt_list_head(&aio->skt->sys_skt.write_q));

                aio_done = __do_skt_send(aio, kq_result);
            } break;
            case EV_SKT_RECV: {
                __skt_recv_t *aio = TT_EV_DATA(ev, __skt_recv_t);

                TT_ASSERT_SKTAIO(&tev->node ==
                                 tt_list_head(&aio->skt->sys_skt.read_q));

                aio_done = __do_skt_recv(aio, kq_result);
            } break;
            case EV_SKT_SENDTO: {
                __skt_sendto_t *aio = TT_EV_DATA(ev, __skt_sendto_t);

                TT_ASSERT_SKTAIO(&tev->node ==
                                 tt_list_head(&aio->skt->sys_skt.write_q));

                aio_done = __do_skt_sendto(aio, kq_result);
            } break;
            case EV_SKT_RECVFROM: {
                __skt_recvfrom_t *aio = TT_EV_DATA(ev, __skt_recvfrom_t);

                TT_ASSERT_SKTAIO(&tev->node ==
                                 tt_list_head(&aio->skt->sys_skt.read_q));

                aio_done = __do_skt_recvfrom(aio, kq_result);
            } break;

            default: {
                TT_ERROR("unknown aio: %x", ev->ev_id);
            } break;
        }

        if (!aio_done) {
            return;
        }
        tt_list_remove(&tev->node);
        tt_ev_destroy(ev);
        // next
    }

    // - if an aio is completed and it is the last aio in q and
    //   nothing is done in callback then we should stop polling
    //   the corresponding io status
    if ((aio_q == &sys_skt->read_q) && (sys_skt->s != -1) &&
        !sys_skt->ssl_want_rd) {
        __skt_kq_rd_disable(sys_skt);
    }
    if ((aio_q == &sys_skt->write_q) && (sys_skt->s != -1) &&
        !sys_skt->ssl_want_wr) {
        __skt_kq_wr_disable(sys_skt);
    }
}

void __clear_skt_aio_q(IN tt_skt_ntv_t *sys_skt,
                       IN tt_list_t *aio_q,
                       IN tt_result_t aio_result)
{
    tt_lnode_t *node;

    while ((node = tt_list_head(aio_q)) != NULL) {
        tt_thread_ev_t *tev = TT_CONTAINER(node, tt_thread_ev_t, node);
        tt_ev_t *ev = TT_EV_OF(tev);

        switch (ev->ev_id) {
            case EV_SKT_SHUTDOWN_WRITE: {
                __skt_shutdown_t *aio = TT_EV_DATA(ev, __skt_shutdown_t);

                // it should be the head aio in write q
                TT_ASSERT_SKTAIO(&tev->node ==
                                 tt_list_head(&aio->skt->sys_skt.write_q));

                // ignore
            } break;
            case EV_SKT_SHUTDOWN_READ: {
                __skt_shutdown_t *aio = TT_EV_DATA(ev, __skt_shutdown_t);

                // it should be the head aio in read q
                TT_ASSERT_SKTAIO(&tev->node ==
                                 tt_list_head(&aio->skt->sys_skt.read_q));

                // ignore
            } break;
            case EV_SKT_ACCEPT: {
                __skt_accept_t *aio = TT_EV_DATA(ev, __skt_accept_t);

                // it should be the head aio in read q
                TT_ASSERT_SKTAIO(
                    &tev->node ==
                    tt_list_head(&aio->listening_skt->sys_skt.read_q));

                aio->result = aio_result;
                __do_skt_accept_cb(aio);
            } break;
            case EV_SKT_CONNECT: {
                __skt_connect_t *aio = TT_EV_DATA(ev, __skt_connect_t);

                // it should be the head aio in write q
                TT_ASSERT_SKTAIO(&tev->node ==
                                 tt_list_head(&aio->skt->sys_skt.write_q));

                aio->result = aio_result;
                __do_skt_connect_cb(aio);
            } break;
            case EV_SKT_SEND: {
                __skt_send_t *aio = TT_EV_DATA(ev, __skt_send_t);

                TT_ASSERT_SKTAIO(&tev->node ==
                                 tt_list_head(&aio->skt->sys_skt.write_q));

                aio->result = aio_result;
                __do_skt_send_cb(aio);
            } break;
            case EV_SKT_RECV: {
                __skt_recv_t *aio = TT_EV_DATA(ev, __skt_recv_t);

                TT_ASSERT_SKTAIO(&tev->node ==
                                 tt_list_head(&aio->skt->sys_skt.read_q));

                aio->result = aio_result;
                __do_skt_recv_cb(aio);
            } break;
            case EV_SKT_SENDTO: {
                __skt_sendto_t *aio = TT_EV_DATA(ev, __skt_sendto_t);

                TT_ASSERT_SKTAIO(&tev->node ==
                                 tt_list_head(&aio->skt->sys_skt.write_q));

                aio->result = aio_result;
                __do_skt_sendto_cb(aio);
            } break;
            case EV_SKT_RECVFROM: {
                __skt_recvfrom_t *aio = TT_EV_DATA(ev, __skt_recvfrom_t);

                TT_ASSERT_SKTAIO(&tev->node ==
                                 tt_list_head(&aio->skt->sys_skt.read_q));

                aio->result = aio_result;
                __do_skt_recvfrom_cb(aio);
            } break;

            default: {
                tt_u32_t ev_range = TT_EV_RANGE(ev->ev_id);
                tt_result_t result = TT_FAIL;

#ifdef TT_PLATFORM_SSL_ENABLE
                if (ev_range == TT_EV_RANGE_INTERNAL_SSL) {
                    result = __ssl_destroy_aio(tev);
                }
#endif

                if (!TT_OK(result)) {
                    TT_FATAL("unknown aio: %x", ev->ev_id);
                }
            } break;
        }

        tt_list_remove(&tev->node);
        tt_ev_destroy(ev);
        // next
    }
}

// return true if buffers are full
tt_bool_t __update_sbuf_state(IN __sbuf_state_t *sbuf_state,
                              IN tt_u32_t io_bytes)
{
    // update send_len
    sbuf_state->io_bytes += io_bytes;

    // update buf array
    while (sbuf_state->cur_idx < sbuf_state->iov_num) {
        struct iovec *cur_v = &sbuf_state->iov[sbuf_state->cur_idx];
        tt_u32_t left_space;

        TT_ASSERT_SKTAIO(sbuf_state->cur_pos < cur_v->iov_len);
        left_space = (tt_u32_t)(cur_v->iov_len - sbuf_state->cur_pos);

        if (io_bytes < left_space) {
            sbuf_state->cur_pos += io_bytes;
            break;
        }
        io_bytes -= left_space;

        // buf full, move to next
        ++sbuf_state->cur_idx;
        sbuf_state->cur_pos = 0;
    };
    TT_ASSERT_SKTAIO(sbuf_state->cur_idx <= sbuf_state->iov_num);

    if (sbuf_state->cur_idx == sbuf_state->iov_num) {
        TT_ASSERT_SKTAIO(io_bytes == 0);
        return TT_TRUE;
    } else {
        return TT_FALSE;
    }
}

void __iovec2blob_inline(IN struct iovec *iov, IN tt_u32_t iov_num)
{
    tt_blob_t *blob = (tt_blob_t *)iov;
    tt_u32_t i;

    TT_ASSERT_SKTAIO(sizeof(struct iovec) >= sizeof(tt_blob_t));

    for (i = 0; i < iov_num; ++i) {
        struct iovec v;
        v.iov_base = iov[i].iov_base;
        v.iov_len = iov[i].iov_len;

        blob[i].addr = (tt_u8_t *)v.iov_base;
        blob[i].len = (tt_u32_t)v.iov_len;
    }
}

// ========================================
// simulate failure
// ========================================

#ifdef __SIMULATE_SKT_AIO_FAIL

tt_result_t tt_evc_sendto_thread_SF(IN tt_evcenter_t *evc,
                                    IN tt_thread_ev_t *tev)
{
    if (rand() % 2) {
        return TT_FAIL;
    } else {
#undef tt_evc_sendto_thread
        return tt_evc_sendto_thread(evc, tev);
    }
}

enum
{
    __SF_IO_END,
    __SF_PARTIAL,
    __SF_FAIL,
    __SF_FAIL_EINTR,
    __SF_FAIL_AG,
    __SF_SYS,

    __SF_NUM
};

ssize_t sendmsg_SF(int sockfd, const struct msghdr *msg, int flags)
{
    int v = rand() % __SF_NUM;
    int ret;
    struct msghdr __sf_msg;

    memcpy(&__sf_msg, msg, sizeof(struct msghdr));

    if (__sf_msg.msg_iov[0].iov_len < 50) {
        v = __SF_SYS;
    }

    if (v == __SF_IO_END) {
        return 0;
    } else if (v == __SF_PARTIAL) {
        __sf_msg.msg_iov[0].iov_len -= (rand() % 50);
        __sf_msg.msg_iovlen = 1;
    } else if (v == __SF_FAIL_EINTR) {
        errno = EINTR;
        return -1;
    } else if (v == __SF_FAIL_AG) {
        errno = EAGAIN;
        return -1;
    } else if (v == __SF_FAIL) {
        return -1;
    }

#undef sendmsg
    return sendmsg(sockfd, &__sf_msg, flags);
}

ssize_t recvmsg_SF(int sockfd, struct msghdr *msg, int flags)
{
    int v = rand() % __SF_NUM;
    int ret;
    struct msghdr __sf_msg;

    memcpy(&__sf_msg, msg, sizeof(struct msghdr));

    if (__sf_msg.msg_iov[0].iov_len < 50) {
        v = __SF_SYS;
    }

    if (v == __SF_IO_END) {
        return 0;
    } else if (v == __SF_PARTIAL) {
        __sf_msg.msg_iov[0].iov_len -= (rand() % 50);
        __sf_msg.msg_iovlen = 1;
    } else if (v == __SF_FAIL_EINTR) {
        errno = EINTR;
        return -1;
    } else if (v == __SF_FAIL_AG) {
        errno = EAGAIN;
        return -1;
    } else if (v == __SF_FAIL) {
        return -1;
    }

#undef recvmsg
    return recvmsg(sockfd, msg, flags);
}

int kevent_SF(int kq,
              const struct kevent *changelist,
              int nchanges,
              struct kevent *eventlist,
              int nevents,
              const struct timespec *timeout)
{
    if (rand() % 4 == 0) {
        return -1;
    } else {
#undef kevent
        return kevent(kq, changelist, nchanges, eventlist, nevents, timeout);
    }
}

#endif

#undef close
int __close_socket(int skt)
{
    if (skt >= 0) {
        tt_atomic_s64_dec(&tt_skt_stat_num);
    }
    return close(skt);
}
