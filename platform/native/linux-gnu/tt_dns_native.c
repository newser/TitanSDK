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

#include <tt_dns_native.h>

#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <log/tt_log.h>
#include <memory/tt_memory_alloc.h>
#include <os/tt_task.h>

#include <tt_util_native.h>

#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>

// clang-format off
#define HAVE_CONFIG_H
#include <ares_setup.h>
#include <ares.h>
#include <ares_private.h>
// clang-format on

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

// each server has 2 sockets
#define __DSKT_NUM(ch) (((ch)->nservers) << 1)

#define __DSKT(ch, i) (&(((__dskt_t *)((ch)->sock_create_cb_data))[(i)]))

#define __DSKT_IDX(dskt)                                                       \
    ((dskt) - (__dskt_t *)((dskt)->ch->sock_create_cb_data))

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

enum
{
    __DNS_NULL,
    __DNS_READ,
    __DNS_WRITE,
    __DNS_READ_WRITE,

    __DNS_EV_NUM,
};

typedef struct
{
    ares_channel ch;
    tt_io_ev_t ev;
    int s;
    tt_bool_t status : 2;
} __dskt_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static ares_socket_t __dskt_socket(IN int af,
                                   IN int type,
                                   IN int protocol,
                                   IN void *param);

static int __dskt_close(IN ares_socket_t s, IN void *param);

static int __dskt_connect(IN ares_socket_t s,
                          IN const struct sockaddr *addr,
                          IN ares_socklen_t addrlen,
                          IN void *param);

static ares_ssize_t __dskt_recvfrom(IN ares_socket_t s,
                                    IN void *data,
                                    IN size_t data_len,
                                    IN int flags,
                                    OUT struct sockaddr *from,
                                    IN OUT ares_socklen_t *from_len,
                                    IN void *param);

static ares_ssize_t __dskt_sendv(IN ares_socket_t s,
                                 IN const struct iovec *vec,
                                 IN int len,
                                 IN void *param);

static struct ares_socket_functions __dskt_itf = {
    __dskt_socket, __dskt_close, __dskt_connect, __dskt_recvfrom, __dskt_sendv,
};

static tt_bool_t __do_null(IN tt_io_ev_t *io_ev);

static tt_bool_t __do_read(IN tt_io_ev_t *io_ev);

static tt_bool_t __do_write(IN tt_io_ev_t *io_ev);

static tt_bool_t __do_read_write(IN tt_io_ev_t *io_ev);

static tt_poller_io_t __dns_poller_io[__DNS_EV_NUM] = {
    __do_null, __do_read, __do_write, __do_read_write,
};

static tt_io_ev_t __s_null_io_ev;

static struct epoll_event __s_epev;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static void __dskt_init(IN __dskt_t *dskt, IN ares_channel ch);

static void __dskt_destroy(IN __dskt_t *dskt);

static __dskt_t *__dskt_avail(IN ares_channel ch);

static int __dskt_reset(IN __dskt_t *dskt);

static void __dskt_ev_init(IN tt_io_ev_t *io_ev, IN tt_u32_t ev);

static tt_result_t __dskt_config(IN int s, IN int af, IN ares_channel ch);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_dns_component_init_ntv(IN tt_profile_t *profile)
{
    __s_null_io_ev.src = NULL;
    __s_null_io_ev.dst = NULL;
    tt_dnode_init(&__s_null_io_ev.node);
    __s_null_io_ev.epev = &__s_epev;
    __s_null_io_ev.io = TT_IO_DNS;
    __s_null_io_ev.ev = __DNS_NULL;

    __s_epev.events = 0;
    __s_epev.data.ptr = &__s_null_io_ev;

    return TT_SUCCESS;
}

tt_result_t tt_dns_create_ntv(IN ares_channel ch)
{
    if (ch->nservers != 0) {
        __dskt_t *dskt;
        int i;

        dskt = tt_malloc(sizeof(__dskt_t) * __DSKT_NUM(ch));
        if (dskt == NULL) {
            TT_ERROR("no mem for dns wov");
            return TT_FAIL;
        }

        for (i = 0; i < __DSKT_NUM(ch); ++i) {
            __dskt_init(&dskt[i], ch);
        }

        // save dskt in ch->sock_create_cb_data. note ares_dup()
        // would copy the pointer, then two ares_channels would
        // share same dskts, which is not expected, so must do a
        // "deep copy" if ares_dup() is called
        ares_set_socket_callback(ch, NULL, dskt);
    }

    ares_set_socket_functions(ch, &__dskt_itf, ch);

    return TT_SUCCESS;
}

void tt_dns_destroy_ntv(IN ares_channel ch)
{
    __dskt_t *dskt = (__dskt_t *)ch->sock_create_cb_data;
    int n = __DSKT_NUM(ch);

    // ares_destroy() need access dskt array as it would close
    // all sockets. so ares_destroy() must be called before
    // __dskt_destroy()
    ares_destroy(ch);

    if (dskt != NULL) {
        int i;
        for (i = 0; i < n; ++i) {
            // no matter whether the dskt is in reading or writing
            // as this function is generally called when io poller
            // thread is terminated, __do_read/write/connect won't
            // be called
            __dskt_destroy(&dskt[i]);
        }

        tt_free(dskt);
    }
}

tt_s64_t tt_dns_run_ntv(IN ares_channel ch)
{
    fd_set rfds, wfds;
    int nfds, i;
    int ep = tt_current_task()->iop.sys_iop.ep;
    struct timeval tv;

    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    nfds = ares_fds(ch, &rfds, &wfds);
    for (i = 0; i < nfds; ++i) {
        __dskt_t *dskt = __DSKT(ch, i);
        tt_bool_t r = TT_BOOL(FD_ISSET(i, &rfds));
        tt_bool_t w = TT_BOOL(FD_ISSET(i, &wfds));

        dskt = __DSKT(ch, i);
        if (r && w) {
            if (dskt->status != __DNS_READ_WRITE) {
                dskt->ev.ev = __DNS_READ_WRITE;
                tt_ep_read_write(ep, dskt->s, &dskt->ev);
                dskt->status = __DNS_READ_WRITE;
            }
        } else if (r) {
            if (dskt->status != __DNS_READ) {
                dskt->ev.ev = __DNS_READ;
                tt_ep_read(ep, dskt->s, &dskt->ev);
                dskt->status = __DNS_READ;
            }
        } else if (w) {
            if (dskt->status != __DNS_WRITE) {
                dskt->ev.ev = __DNS_WRITE;
                tt_ep_write(ep, dskt->s, &dskt->ev);
                dskt->status = __DNS_WRITE;
            }
        } else {
            if ((dskt->s != -1) && (dskt->status != __DNS_NULL)) {
                dskt->ev.ev = __DNS_NULL;
                tt_ep_unread(ep, dskt->s, &__s_null_io_ev);
                dskt->status = __DNS_NULL;
            }
        }
    }
    // to process timer
    ares_process(ch, NULL, NULL);

    if (ares_timeout(ch, NULL, &tv) != NULL) {
        return (tt_s64_t)tv.tv_sec * 1000 + (tt_s64_t)tv.tv_usec / 1000;
    } else {
        return TT_TIME_INFINITE;
    }
}

tt_bool_t tt_dns_poller_io(IN tt_io_ev_t *io_ev)
{
    return __dns_poller_io[io_ev->ev](io_ev);
}

void __dskt_init(IN __dskt_t *dskt, IN ares_channel ch)
{
    dskt->ch = ch;
    __dskt_ev_init(&dskt->ev, __DNS_NULL);
    dskt->s = -1;
    dskt->status = __DNS_NULL;
}

void __dskt_destroy(IN __dskt_t *dskt)
{
}

__dskt_t *__dskt_avail(IN ares_channel ch)
{
    int i;
    for (i = 0; i < __DSKT_NUM(ch); ++i) {
        if (__DSKT(ch, i)->s == -1) {
            return __DSKT(ch, i);
        }
    }
    return NULL;
}

int __dskt_reset(IN __dskt_t *dskt)
{
    int e;

    // keep dskt->ch
    __dskt_ev_init(&dskt->ev, __DNS_NULL);
    e = close(dskt->s);
    dskt->s = -1;
    dskt->status = __DNS_NULL;

    return e;
}

void __dskt_ev_init(IN tt_io_ev_t *io_ev, IN tt_u32_t ev)
{
    io_ev->src = NULL;
    io_ev->dst = NULL;
    tt_dnode_init(&io_ev->node);
    io_ev->epev = NULL;
    io_ev->io = TT_IO_DNS;
    io_ev->ev = ev;
}

tt_result_t __dskt_config(IN int s, IN int af, IN ares_channel ch)
{
    int flag, n;
    tt_sktaddr_t addr;

    // refer configure_socket()

    if (((flag = fcntl(s, F_GETFL, 0)) == -1) ||
        (fcntl(s, F_SETFL, flag | O_NONBLOCK) == -1)) {
        TT_ERROR_NTV("fail to set O_NONBLOCK");
        return TT_FAIL;
    }

    if (((flag = fcntl(s, F_GETFD, 0)) == -1) ||
        (fcntl(s, F_SETFD, flag | FD_CLOEXEC) == -1)) {
        TT_ERROR_NTV("fail to set FD_CLOEXEC");
        return TT_FAIL;
    }

    n = ch->socket_send_buffer_size;
    if ((n > 0) && (setsockopt(s, SOL_SOCKET, SO_SNDBUF, &n, sizeof(n)) != 0)) {
        TT_ERROR_NTV("fail to set SO_SNDBUF");
        return TT_FAIL;
    }

    n = ch->socket_receive_buffer_size;
    if ((n > 0) && (setsockopt(s, SOL_SOCKET, SO_RCVBUF, &n, sizeof(n)) != 0)) {
        TT_ERROR_NTV("fail to set SO_RCVBUF");
        return TT_FAIL;
    }

    if (af == AF_INET) {
        if (ch->local_ip4 != 0) {
            tt_sktaddr_ip_t ip;

            tt_sktaddr_init(&addr, TT_NET_AF_INET);

            ip.a32.__u32 = htonl(ch->local_ip4);
            tt_sktaddr_set_ip_n(&addr, &ip);
        } else {
            tt_sktaddr_init_any(&addr, TT_NET_AF_INET);
        }
    } else {
        TT_ASSERT(af == AF_INET6);
        if (tt_memcmp(ch->local_ip6,
                      &ares_in6addr_any,
                      sizeof(ch->local_ip6)) != 0) {
            tt_sktaddr_ip_t ip;

            tt_sktaddr_init(&addr, TT_NET_AF_INET6);

            tt_memcpy(ip.a128.__u8, ch->local_ip6, 16);
            tt_sktaddr_set_ip_n(&addr, &ip);
        } else {
            tt_sktaddr_init_any(&addr, TT_NET_AF_INET6);
        }
    }
    if (bind(s, (struct sockaddr *)&addr, sizeof(tt_sktaddr_t)) != 0) {
        TT_ERROR_NTV("bind fail");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

// ========================================
// dskt interface
// ========================================

ares_socket_t __dskt_socket(IN int af,
                            IN int type,
                            IN int protocol,
                            IN void *param)
{
    ares_channel ch = (ares_channel)param;
    __dskt_t *dskt;
    int s, epfd;
    struct epoll_event event;

    dskt = __dskt_avail(ch);
    if (dskt == NULL) {
        TT_ERROR("no available dns skt");
        return ARES_SOCKET_BAD;
    }

    s = socket(af, type, protocol);
    if (s == -1) {
        TT_ERROR_NTV("fail to create socket");
        return ARES_SOCKET_BAD;
    }

    if (!TT_OK(__dskt_config(s, af, ch))) {
        goto fail;
    }

    epfd = tt_current_task()->iop.sys_iop.ep;
    event.events = EPOLLRDHUP | EPOLLONESHOT;
    event.data.ptr = &__s_null_io_ev;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, s, &event) != 0) {
        TT_ERROR_NTV("fail to add skt to epoll");
        goto fail;
    }

    dskt->s = s;
    return __DSKT_IDX(dskt);

fail:

    close(s);
    return ARES_SOCKET_BAD;
}

int __dskt_close(IN ares_socket_t s, IN void *param)
{
    ares_channel ch = (ares_channel)param;
    __dskt_t *dskt = __DSKT(ch, s);

    return __dskt_reset(__DSKT(ch, s));
}

int __dskt_connect(IN ares_socket_t s,
                   IN const struct sockaddr *addr,
                   IN ares_socklen_t addrlen,
                   IN void *param)
{
    ares_channel ch = (ares_channel)param;
    __dskt_t *dskt = __DSKT(ch, s);
    int e;

    TT_ASSERT(dskt->status == __DNS_NULL);

again:
    e = connect(dskt->s, (const struct sockaddr *)addr, addrlen);
    if (e == 0) {
        return 0;
    } else if (errno == EINTR) {
        goto again;
    } else {
        if (errno != EINPROGRESS) {
            TT_ERROR_NTV("fail to connect");
        }
        return e;
    }
}

ares_ssize_t __dskt_recvfrom(IN ares_socket_t s,
                             IN void *data,
                             IN size_t data_len,
                             IN int flags,
                             IN struct sockaddr *from,
                             IN ares_socklen_t *from_len,
                             IN void *param)
{
    ares_channel ch = (ares_channel)param;
    __dskt_t *dskt = __DSKT(ch, s);

    if (from != NULL) {
        return recvfrom(dskt->s, data, data_len, flags, from, from_len);
    } else {
        return recv(dskt->s, data, data_len, 0);
    }
}

ares_ssize_t __dskt_sendv(IN ares_socket_t s,
                          IN const struct iovec *vec,
                          IN int len,
                          IN void *param)
{
    ares_channel ch = (ares_channel)param;
    __dskt_t *dskt = __DSKT(ch, s);

    // sendmsg??
    return writev(dskt->s, vec, len);
}

// ========================================
// io handler
// ========================================

tt_bool_t __do_null(IN tt_io_ev_t *io_ev)
{
    return TT_TRUE;
}

tt_bool_t __do_read(IN tt_io_ev_t *io_ev)
{
    __dskt_t *dskt = TT_CONTAINER(io_ev, __dskt_t, ev);

    TT_ASSERT(dskt->status = __DNS_READ);
    dskt->status = __DNS_NULL;

    ares_process_fd(dskt->ch, __DSKT_IDX(dskt), ARES_SOCKET_BAD);

    return TT_TRUE;
}

tt_bool_t __do_write(IN tt_io_ev_t *io_ev)
{
    __dskt_t *dskt = TT_CONTAINER(io_ev, __dskt_t, ev);

    TT_ASSERT(dskt->status = __DNS_WRITE);
    dskt->status = __DNS_NULL;

    ares_process_fd(dskt->ch, ARES_SOCKET_BAD, __DSKT_IDX(dskt));

    return TT_TRUE;
}

tt_bool_t __do_read_write(IN tt_io_ev_t *io_ev)
{
    __dskt_t *dskt = TT_CONTAINER(io_ev, __dskt_t, ev);
    uint32_t events = io_ev->epev->events;
    ares_socket_t rs, ws;

    TT_ASSERT(dskt->status = __DNS_READ_WRITE);
    dskt->status = __DNS_NULL;

    if (events & ~EPOLLOUT) {
        // read if it sees any ev other than EPOLLOUT(EPOLLIN, error, etc.)
        rs = __DSKT_IDX(dskt);
    } else {
        rs = ARES_SOCKET_BAD;
    }

    if (events & EPOLLOUT) {
        ws = __DSKT_IDX(dskt);
    } else {
        ws = ARES_SOCKET_BAD;
    }

    ares_process_fd(dskt->ch, rs, ws);

    return TT_TRUE;
}
