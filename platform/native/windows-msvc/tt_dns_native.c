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

#include <algorithm/tt_buffer.h>
#include <io/tt_socket_addr.h>
#include <log/tt_log.h>
#include <memory/tt_memory_alloc.h>
#include <os/tt_task.h>

#include <tt_cstd_api.h>
#include <tt_sys_error.h>

#include <mswsock.h>

// clang-format off
#include <ares_setup.h>
#include <ares.h>
#include <ares_private.h>
// clang-format on

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __WBUF_NUM 16

#define __DSKT_NUM(c) (((c)->nservers) << 1)

#define __DSKT(c, i) (&(((__dskt_t *)((c)->sock_create_cb_data))[(i)]))

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

enum
{
    __DNS_READ,
    __DNS_WRITE,

    __DNS_EV_NUM,
};

typedef struct
{
    ares_channel c;
    SOCKET s;
    SOCKADDR_STORAGE addr;
    WSABUF w_buf[__WBUF_NUM];
    tt_io_ev_t r_ev;
    tt_io_ev_t w_ev;
    tt_buf_t r_buf;
    // tt_buf_t w_buf;
    ares_socklen_t addrlen;
    tt_u32_t w_len;
    tt_result_t r_result;
    tt_result_t w_result;
    tt_u16_t idx;
    tt_bool_t udp : 1;
    tt_bool_t reading : 1;
    tt_bool_t writing : 1;
} __dskt_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

extern BOOL(PASCAL FAR *tt_ConnectEx)(SOCKET s,
                                      const struct sockaddr *name,
                                      int namelen,
                                      PVOID lpSendBuffer,
                                      DWORD dwSendDataLength,
                                      LPDWORD lpdwBytesSent,
                                      LPOVERLAPPED lpOverlapped);

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
                                    IN struct sockaddr *from,
                                    IN ares_socklen_t *from_len,
                                    IN void *param);

static ares_ssize_t __dskt_sendv(IN ares_socket_t s,
                                 IN const struct iovec *vec,
                                 IN int len,
                                 IN void *param);

static struct ares_socket_functions __dskt_itf = {
    __dskt_socket, __dskt_close, __dskt_connect, __dskt_recvfrom, __dskt_sendv,
};

static tt_bool_t __do_read(IN tt_io_ev_t *io_ev);

static tt_bool_t __do_write(IN tt_io_ev_t *io_ev);

static tt_poller_io_t __dns_poller_io[__DNS_EV_NUM] = {
    __do_read, __do_write,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static void __dskt_init(IN __dskt_t *dskt, IN ares_channel c, IN tt_u16_t idx);

static void __dskt_destroy(IN __dskt_t *dskt);

static __dskt_t *__dskt_avail(IN ares_channel c);

static int __dskt_reset(IN __dskt_t *dskt);

static void __dskt_ev_init(IN tt_io_ev_t *io_ev, IN tt_u32_t ev);

static tt_result_t __dskt_config(IN SOCKET s, IN int af, IN ares_channel c);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_dns_create_ntv(IN ares_channel c)
{
    if (c->nservers != 0) {
        __dskt_t *dskt;
        int i;

        dskt = tt_malloc(sizeof(__dskt_t) * __DSKT_NUM(c));
        if (dskt == NULL) {
            TT_ERROR("no mem for dns wov");
            return TT_FAIL;
        }

        for (i = 0; i < __DSKT_NUM(c); ++i) {
            __dskt_init(&dskt[i], c, i);
        }

        // save dskt in c->sock_create_cb_data
        ares_set_socket_callback(c, NULL, dskt);
    }

    ares_set_socket_functions(c, &__dskt_itf, c);

    return TT_SUCCESS;
}

void tt_dns_destroy_ntv(IN ares_channel c)
{
    if (c->sock_create_cb_data != NULL) {
        int i;
        for (i = 0; i < __DSKT_NUM(c); ++i) {
            __dskt_destroy(__DSKT(c, i));
        }

        tt_free(c->sock_create_cb_data);
    }
}

tt_bool_t tt_dns_poller_io(IN tt_io_ev_t *io_ev)
{
    return __dns_poller_io[io_ev->ev](io_ev);
}

void __dskt_init(IN __dskt_t *dskt, IN ares_channel c, IN tt_u16_t idx)
{
    dskt->c = c;
    dskt->s = INVALID_SOCKET;
    tt_memset(&dskt->addr, 0, sizeof(SOCKADDR_STORAGE));
    tt_memset(&dskt->w_buf, 0, sizeof(dskt->w_buf));
    __dskt_ev_init(&dskt->r_ev, __DNS_READ);
    __dskt_ev_init(&dskt->w_ev, __DNS_WRITE);
    tt_buf_init(&dskt->r_buf, NULL);
    dskt->addrlen = 0;
    dskt->w_len = 0;
    dskt->r_result = TT_SUCCESS;
    dskt->w_result = TT_SUCCESS;
    dskt->idx = idx;
    dskt->udp = TT_FALSE;
    dskt->reading = TT_FALSE;
    dskt->writing = TT_FALSE;
}

void __dskt_destroy(IN __dskt_t *dskt)
{
    tt_buf_destroy(&dskt->r_buf);
}

__dskt_t *__dskt_avail(IN ares_channel c)
{
    int i;
    for (i = 0; i < __DSKT_NUM(c); ++i) {
        if (__DSKT(c, i)->s == INVALID_SOCKET) {
            return __DSKT(c, i);
        }
    }
    return NULL;
}

int __dskt_reset(IN __dskt_t *dskt)
{
    int e = 0;

    // keep dskt->c

    if (dskt->s != INVALID_SOCKET) {
        e = closesocket(dskt->s);
        if (e != 0) {
            TT_NET_ERROR_NTV("fail to close socket");
        }
        dskt->s = INVALID_SOCKET;
    }

    tt_memset(&dskt->addr, 0, sizeof(SOCKADDR_STORAGE));
    tt_memset(&dskt->w_buf, 0, sizeof(dskt->w_buf));
    __dskt_ev_init(&dskt->r_ev, __DNS_READ);
    __dskt_ev_init(&dskt->w_ev, __DNS_WRITE);
    tt_buf_clear(&dskt->r_buf);
    dskt->addrlen = 0;
    dskt->w_len = 0;
    dskt->r_result = TT_SUCCESS;
    dskt->w_result = TT_SUCCESS;
    // keep dskt->idx
    dskt->udp = TT_FALSE;
    dskt->reading = TT_FALSE;
    dskt->writing = TT_FALSE;

    return e;
}

void __dskt_ev_init(IN tt_io_ev_t *io_ev, IN tt_u32_t ev)
{
    io_ev->src = NULL;
    io_ev->dst = NULL;
    tt_dnode_init(&io_ev->node);
    tt_memset(&io_ev->wov, 0, sizeof(WSAOVERLAPPED));
    io_ev->io_bytes = 0;
    io_ev->io_result = TT_FAIL;
    io_ev->io = TT_IO_DNS;
    io_ev->ev = ev;
}

tt_result_t __dskt_config(IN SOCKET s, IN int af, IN ares_channel c)
{
    int n;
    tt_sktaddr_t addr;

    // refer configure_socket()

    n = c->socket_send_buffer_size;
    if ((n > 0) && (setsockopt(s,
                               SOL_SOCKET,
                               SO_SNDBUF,
                               (const char *)&n,
                               (int)sizeof(int)) != 0)) {
        TT_NET_ERROR_NTV("fail to set SO_SNDBUF");
        return TT_FAIL;
    }

    n = c->socket_receive_buffer_size;
    if ((n > 0) && (setsockopt(s,
                               SOL_SOCKET,
                               SO_RCVBUF,
                               (const char *)&n,
                               (int)sizeof(int)) != 0)) {
        TT_NET_ERROR_NTV("fail to set SO_RCVBUF");
        return TT_FAIL;
    }

    if (af == AF_INET) {
        if (c->local_ip4 != 0) {
            tt_sktaddr_ip_t ip;

            tt_sktaddr_init(&addr, TT_NET_AF_INET);

            ip.a32.__u32 = htonl(c->local_ip4);
            tt_sktaddr_set_ip_n(&addr, &ip);
        } else {
            tt_sktaddr_init_any(&addr, TT_NET_AF_INET);
        }
    } else {
        TT_ASSERT(af == AF_INET6);
        if (tt_memcmp(c->local_ip6, &ares_in6addr_any, sizeof(c->local_ip6)) !=
            0) {
            tt_sktaddr_ip_t ip;

            tt_sktaddr_init(&addr, TT_NET_AF_INET6);

            tt_memcpy(ip.a128.__u8, c->local_ip6, 16);
            tt_sktaddr_set_ip_n(&addr, &ip);
        } else {
            tt_sktaddr_init_any(&addr, TT_NET_AF_INET6);
        }
    }
    if (bind(s, (struct sockaddr *)&addr, sizeof(tt_sktaddr_t)) != 0) {
        TT_NET_ERROR_NTV("bind fail");
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
    ares_channel c = (ares_channel)param;
    __dskt_t *dskt;
    SOCKET s;
    HANDLE iocp;

    dskt = __dskt_avail(c);
    if (dskt == NULL) {
        TT_ERROR("no available dns skt");
        return ARES_SOCKET_BAD;
    }

    s = WSASocketW(af, type, protocol, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (s == INVALID_SOCKET) {
        TT_NET_ERROR_NTV("fail to create socket");
        return ARES_SOCKET_BAD;
    }

    if (!TT_OK(__dskt_config(s, af, c))) {
        goto fail;
    }

    if (protocol == IPPROTO_UDP) {
        BOOL bNewBehavior = FALSE;
        DWORD dwBytesReturned = 0;
        if (WSAIoctl(s,
                     SIO_UDP_CONNRESET,
                     &bNewBehavior,
                     sizeof(bNewBehavior),
                     NULL,
                     0,
                     &dwBytesReturned,
                     NULL,
                     NULL) == SOCKET_ERROR) {
            TT_NET_ERROR_NTV("fail to disable SIO_UDP_CONNRESET");
            // ignore?
        }
    }

    iocp = tt_current_fiber_sched()->thread->task->iop.sys_iop.iocp;
    if (CreateIoCompletionPort((HANDLE)s, iocp, (ULONG_PTR)param, 0) == NULL) {
        TT_NET_ERROR_NTV("fail to bind socket to iocp");
        goto fail;
    }

    dskt->s = s;
    dskt->udp = TT_BOOL(protocol == IPPROTO_UDP);
    return (ares_socket_t)dskt->idx;

fail:

    closesocket(s);
    return ARES_SOCKET_BAD;
}

int __dskt_close(IN ares_socket_t s, IN void *param)
{
    ares_channel c = (ares_channel)param;

    return __dskt_reset(__DSKT(c, s));
}

int __dskt_connect(IN ares_socket_t s,
                   IN const struct sockaddr *addr,
                   IN ares_socklen_t addrlen,
                   IN void *param)
{
    ares_channel c = (ares_channel)param;
    __dskt_t *dskt = __DSKT(c, s);

#if 0
    if (dskt->writing) {
        WSASetLastError(WSAEWOULDBLOCK);
        return -1;
    }
#else
    // expecting connect() once for each socket
    TT_ASSERT(!dskt->writing);
#endif

    tt_memcpy(&dskt->addr, addr, addrlen);
    dskt->addrlen = addrlen;

    if (dskt->udp) {
        // connecting udp socket won't block
        if (WSAConnect(s,
                       (const struct sockaddr *)&dskt->addr,
                       dskt->addrlen,
                       NULL,
                       NULL,
                       NULL,
                       NULL) == 0) {
            return 0;
        } else {
            TT_NET_ERROR_NTV("WSAConnect fail");
            return -1;
        }
    } else {
        tt_memset(&dskt->w_ev.wov, 0, sizeof(WSAOVERLAPPED));
        if (tt_ConnectEx(s,
                         (const struct sockaddr *)&dskt->addr,
                         dskt->addrlen,
                         NULL,
                         0,
                         NULL,
                         &dskt->w_ev.wov) ||
            (WSAGetLastError() == WSA_IO_PENDING)) {
            dskt->writing = TT_TRUE;
            WSASetLastError(WSAEWOULDBLOCK);
            return -1;
        } else {
            TT_NET_ERROR_NTV("ConnectEx fail");
            return -1;
        }
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
    ares_channel c = (ares_channel)param;
    __dskt_t *dskt = __DSKT(c, s);
    tt_buf_t *buf;
    tt_u32_t len;
    WSABUF Buffers;
    DWORD Flags;

    if (dskt->reading) {
        WSASetLastError(WSAEWOULDBLOCK);
        return -1;
    }

    buf = &dskt->r_buf;
    len = TT_BUF_RLEN(buf);
    if (len > 0) {
        len = TT_MIN(len, (tt_u32_t)data_len);
        tt_memcpy(data, TT_BUF_RPOS(buf), len);
        tt_buf_inc_rp(buf, len);
        if (from != NULL) {
            tt_u32_t n = TT_MIN(dskt->addrlen, *from_len);
            tt_memcpy(from, &dskt->addr, n);
            *from_len = n;
        }
        return len;
    }

    if (!TT_OK(tt_buf_reserve(buf, (tt_u32_t)data_len))) {
        TT_ERROR("fail to reserve dns recv buf");
        return -1;
    }

    Buffers.buf = (char *)TT_BUF_WPOS(buf);
    Buffers.len = (ULONG)data_len;
    Flags = 0;
    tt_memset(&dskt->r_ev.wov, 0, sizeof(WSAOVERLAPPED));
    if ((WSARecv(s, &Buffers, 1, NULL, &Flags, &dskt->r_ev.wov, NULL) == 0) ||
        (WSAGetLastError() == WSA_IO_PENDING)) {
        dskt->reading = TT_TRUE;
        WSASetLastError(WSAEWOULDBLOCK);
    }
    return -1;
}

ares_ssize_t __dskt_sendv(IN ares_socket_t s,
                          IN const struct iovec *vec,
                          IN int len,
                          IN void *param)
{
    ares_channel c = (ares_channel)param;
    __dskt_t *dskt = __DSKT(c, s);
    int n, i;

    if (dskt->writing) {
        WSASetLastError(WSAEWOULDBLOCK);
        return -1;
    }

    if (dskt->w_len > 0) {
        return dskt->w_len;
    }

    n = TT_MIN(__WBUF_NUM, len);
    for (i = 0; i < n; ++i) {
        dskt->w_buf[i].len = (u_long)vec[i].iov_len;
        dskt->w_buf[i].buf = (char *)vec[i].iov_base;
    }
    tt_memset(&dskt->w_ev.wov, 0, sizeof(WSAOVERLAPPED));
    if ((WSASend(s, dskt->w_buf, n, NULL, 0, &dskt->w_ev.wov, NULL) == 0) ||
        (WSAGetLastError() == WSA_IO_PENDING)) {
        dskt->writing = TT_TRUE;
        WSASetLastError(WSAEWOULDBLOCK);
    }
    return -1;
}

// ========================================
// io handler
// ========================================

tt_bool_t __do_read(IN tt_io_ev_t *io_ev)
{
    __dskt_t *dskt = TT_CONTAINER(io_ev, __dskt_t, r_ev);

    if (io_ev->io_bytes > 0) {
        tt_buf_inc_rp(&dskt->r_buf, io_ev->io_bytes);
        dskt->r_result = TT_SUCCESS;
    } else if (TT_OK(io_ev->io_result)) {
        dskt->r_result = TT_END;
    } else {
        dskt->r_result = io_ev->io_result;
    }
    dskt->reading = TT_FALSE;

    ares_process_fd(dskt->c, dskt->s, ARES_SOCKET_BAD);

    return TT_TRUE;
}


tt_bool_t __do_write(IN tt_io_ev_t *io_ev)
{
    __dskt_t *dskt = TT_CONTAINER(io_ev, __dskt_t, r_ev);

    if (TT_OK(io_ev->io_result)) {
        TT_ASSERT(dskt->w_len == 0);
        dskt->w_len = io_ev->io_bytes;
        dskt->w_result = TT_SUCCESS;
    } else {
        dskt->w_result = io_ev->io_result;
    }
    dskt->writing = TT_FALSE;

    ares_process_fd(dskt->c, ARES_SOCKET_BAD, dskt->s);

    return TT_TRUE;
}
