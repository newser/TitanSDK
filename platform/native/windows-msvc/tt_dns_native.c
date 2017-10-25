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

// each server has 2 sockets, and reserve extra 2 sockets
// for async tcp closing
#define __DSKT_NUM(ch) (((ch)->nservers) << 2)

#define __DSKT(ch, i) (&(((__dskt_t *)((ch)->sock_create_cb_data))[(i)]))

#define __DSKT_IDX(dskt)                                                       \
    ((dskt) - (__dskt_t *)((dskt)->ch->sock_create_cb_data))

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

enum
{
    __DNS_CONNECT,
    __DNS_READ,
    __DNS_WRITE,

    __DNS_EV_NUM,
};

typedef struct
{
    ares_channel ch;
    SOCKET s;
    SOCKADDR_STORAGE addr;
    WSABUF w_buf[__WBUF_NUM];
    tt_io_ev_t r_ev;
    tt_io_ev_t w_ev;
    tt_buf_t r_buf;
    ares_socklen_t addrlen;
    tt_u32_t w_len;
    tt_result_t r_result;
    tt_result_t w_result;
    tt_bool_t udp : 1;
    tt_bool_t reading : 1;
    tt_bool_t writing : 1;
    tt_bool_t closing : 1;
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

static tt_bool_t __do_connect(IN tt_io_ev_t *io_ev);

static tt_bool_t __do_read(IN tt_io_ev_t *io_ev);

static tt_bool_t __do_write(IN tt_io_ev_t *io_ev);

static tt_poller_io_t __dns_poller_io[__DNS_EV_NUM] = {
    __do_connect, __do_read, __do_write,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static void __dskt_init(IN __dskt_t *dskt, IN ares_channel ch);

static void __dskt_destroy(IN __dskt_t *dskt);

static __dskt_t *__dskt_avail(IN ares_channel ch);

static void __dskt_reset(IN __dskt_t *dskt);

static void __dskt_ev_init(IN tt_io_ev_t *io_ev, IN tt_u32_t ev);

static tt_result_t __dskt_config(IN SOCKET s, IN int af, IN ares_channel ch);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

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
    __dskt_t *p = (__dskt_t *)ch->sock_create_cb_data;
    int n = __DSKT_NUM(ch);

    // ares_destroy() need access dskt array as it would close
    // all sockets. so ares_destroy() must be called before
    // __dskt_destroy()
    ares_destroy(ch);

    if (p != NULL) {
        int i;
        for (i = 0; i < n; ++i) {
            // no matter whether the dskt is in reading or writing
            // as this function is generally called when io poller
            // thread is terminated, __do_read/write/connect won't
            // be called
            __dskt_destroy(&p[i]);
        }

        tt_free(p);
    }
}

tt_s64_t tt_dns_run_ntv(IN ares_channel ch)
{
    fd_set rfds, wfds;
    u_int i;
    struct timeval tv;

again:
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    ares_fds(ch, &rfds, &wfds);
    ares_process(ch, &rfds, &wfds);
    for (i = 0; i < rfds.fd_count; ++i) {
        __dskt_t *dskt = __DSKT(ch, rfds.fd_array[i]);
        if (!dskt->reading) {
            // a socket want read but not reading
            goto again;
        }
    }
    for (i = 0; i < wfds.fd_count; ++i) {
        __dskt_t *dskt = __DSKT(ch, rfds.fd_array[i]);
        if (!dskt->udp && !dskt->writing) {
            // a tcp socket want write but not writing
            goto again;
        }
    }

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
    dskt->udp = TT_FALSE;
    dskt->reading = TT_FALSE;
    dskt->writing = TT_FALSE;
    dskt->closing = TT_FALSE;
}

void __dskt_destroy(IN __dskt_t *dskt)
{
    tt_buf_destroy(&dskt->r_buf);
}

__dskt_t *__dskt_avail(IN ares_channel ch)
{
    int i;
    for (i = 0; i < __DSKT_NUM(ch); ++i) {
        if (__DSKT(ch, i)->s == INVALID_SOCKET) {
            return __DSKT(ch, i);
        }
    }
    return NULL;
}

void __dskt_reset(IN __dskt_t *dskt)
{
    // keep dskt->ch

    dskt->s = INVALID_SOCKET;
    tt_memset(&dskt->addr, 0, sizeof(SOCKADDR_STORAGE));
    tt_memset(&dskt->w_buf, 0, sizeof(dskt->w_buf));
    __dskt_ev_init(&dskt->r_ev, __DNS_READ);
    __dskt_ev_init(&dskt->w_ev, __DNS_WRITE);
    tt_buf_clear(&dskt->r_buf);
    dskt->addrlen = 0;
    dskt->w_len = 0;
    dskt->r_result = TT_SUCCESS;
    dskt->w_result = TT_SUCCESS;
    dskt->udp = TT_FALSE;
    dskt->reading = TT_FALSE;
    dskt->writing = TT_FALSE;
    dskt->closing = TT_FALSE;
}

void __dskt_ev_init(IN tt_io_ev_t *io_ev, IN tt_u32_t ev)
{
	tt_io_ev_init(io_ev, TT_IO_DNS, ev);
}

tt_result_t __dskt_config(IN SOCKET s, IN int af, IN ares_channel ch)
{
    u_long nonblock = 1;
    int n;
    tt_sktaddr_t addr;

    // refer configure_socket()

    if (ioctlsocket(s, FIONBIO, &nonblock) != 0) {
        TT_NET_ERROR_NTV("fail to set FIONBIO");
        return TT_FAIL;
    }

    n = ch->socket_send_buffer_size;
    if ((n > 0) && (setsockopt(s,
                               SOL_SOCKET,
                               SO_SNDBUF,
                               (const char *)&n,
                               (int)sizeof(int)) != 0)) {
        TT_NET_ERROR_NTV("fail to set SO_SNDBUF");
        return TT_FAIL;
    }

    n = ch->socket_receive_buffer_size;
    if ((n > 0) && (setsockopt(s,
                               SOL_SOCKET,
                               SO_RCVBUF,
                               (const char *)&n,
                               (int)sizeof(int)) != 0)) {
        TT_NET_ERROR_NTV("fail to set SO_RCVBUF");
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
    ares_channel ch = (ares_channel)param;
    __dskt_t *dskt;
    SOCKET s;
    HANDLE iocp;

    dskt = __dskt_avail(ch);
    if (dskt == NULL) {
        TT_ERROR("no available dns skt");
        return ARES_SOCKET_BAD;
    }

    s = WSASocketW(af, type, protocol, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (s == INVALID_SOCKET) {
        TT_NET_ERROR_NTV("fail to create socket");
        return ARES_SOCKET_BAD;
    }

    if (!TT_OK(__dskt_config(s, af, ch))) {
        goto fail;
    }

    if (type == SOCK_DGRAM) {
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

    iocp = tt_current_task()->iop.sys_iop.iocp;
    if (CreateIoCompletionPort((HANDLE)s, iocp, (ULONG_PTR)ch, 0) == NULL) {
        TT_NET_ERROR_NTV("fail to bind socket to iocp");
        goto fail;
    }

    dskt->s = s;
    dskt->udp = TT_BOOL(type == SOCK_DGRAM);
    return __DSKT_IDX(dskt);

fail:

    closesocket(s);
    return ARES_SOCKET_BAD;
}

int __dskt_close(IN ares_socket_t s, IN void *param)
{
    ares_channel ch = (ares_channel)param;
    __dskt_t *dskt = __DSKT(ch, s);

    TT_ASSERT(!dskt->closing);
    if (closesocket(dskt->s) != 0) {
        TT_NET_ERROR_NTV("fail to close socket");
    }
    dskt->closing = TT_TRUE;

    if (!dskt->reading && !dskt->writing) {
        __dskt_reset(__DSKT(ch, s));
    }

    return 0;
}

int __dskt_connect(IN ares_socket_t s,
                   IN const struct sockaddr *addr,
                   IN ares_socklen_t addrlen,
                   IN void *param)
{
    ares_channel ch = (ares_channel)param;
    __dskt_t *dskt = __DSKT(ch, s);

#if 0
    if (dskt->reading || dskt->writing || dskt->closing) {
        WSASetLastError(WSAEWOULDBLOCK);
        return -1;
    }
#else
    TT_ASSERT(!dskt->reading && !dskt->writing && !dskt->closing);
#endif

    tt_memcpy(&dskt->addr, addr, addrlen);
    dskt->addrlen = addrlen;

    if (dskt->udp) {
        // connecting udp socket won't block
        if (WSAConnect(dskt->s,
                       (const struct sockaddr *)&dskt->addr,
                       dskt->addrlen,
                       NULL,
                       NULL,
                       NULL,
                       NULL) == 0) {
            return 0;
        } else {
            TT_NET_ERROR_NTV("WSAConnect fail");
            WSASetLastError(WSAECONNABORTED);
            return -1;
        }
    } else {
        tt_memset(&dskt->w_ev.u.wov, 0, sizeof(WSAOVERLAPPED));
        if (tt_ConnectEx(dskt->s,
                         (const struct sockaddr *)&dskt->addr,
                         dskt->addrlen,
                         NULL,
                         0,
                         NULL,
                         &dskt->w_ev.u.wov) ||
            (WSAGetLastError() == WSA_IO_PENDING)) {
            dskt->w_ev.ev = __DNS_CONNECT;
            dskt->reading = TT_TRUE;
            dskt->writing = TT_TRUE;
            WSASetLastError(WSAEWOULDBLOCK);
            return -1;
        } else {
            TT_NET_ERROR_NTV("ConnectEx fail");
            WSASetLastError(WSAECONNABORTED);
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
    ares_channel ch = (ares_channel)param;
    __dskt_t *dskt = __DSKT(ch, s);
    tt_buf_t *buf;
    tt_u32_t len;
    WSABUF Buffers;
    DWORD Flags;

    TT_ASSERT(!dskt->closing);
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
        tt_buf_try_refine(buf, 1024);
        if (from != NULL) {
            tt_u32_t n = TT_MIN(dskt->addrlen, *from_len);
            tt_memcpy(from, &dskt->addr, n);
            *from_len = n;
        }
        return len;
    }

    if (!TT_OK(dskt->r_result)) {
        WSASetLastError(WSAECONNABORTED);
        return -1;
    }

    if (!TT_OK(tt_buf_reserve(buf, (tt_u32_t)data_len))) {
        TT_ERROR("fail to reserve dns recv buf");
        WSASetLastError(WSA_NOT_ENOUGH_MEMORY);
        return -1;
    }

    Buffers.buf = (char *)TT_BUF_WPOS(buf);
    Buffers.len = (ULONG)data_len;
    Flags = 0;
    tt_memset(&dskt->r_ev.u.wov, 0, sizeof(WSAOVERLAPPED));
    if ((WSARecv(dskt->s, &Buffers, 1, NULL, &Flags, &dskt->r_ev.u.wov, NULL) ==
         0) ||
        (WSAGetLastError() == WSA_IO_PENDING)) {
        dskt->reading = TT_TRUE;
        WSASetLastError(WSAEWOULDBLOCK);
    } else {
        TT_NET_ERROR_NTV("fail to deliver WSARecv");
        WSASetLastError(WSAECONNABORTED);
    }
    return -1;
}

ares_ssize_t __dskt_sendv(IN ares_socket_t s,
                          IN const struct iovec *vec,
                          IN int len,
                          IN void *param)
{
    ares_channel ch = (ares_channel)param;
    __dskt_t *dskt = __DSKT(ch, s);
    int n, i;

    TT_ASSERT(!dskt->closing);
    if (dskt->writing) {
        WSASetLastError(WSAEWOULDBLOCK);
        return -1;
    }

    if (dskt->w_len > 0) {
        tt_u32_t w_len = dskt->w_len;
        dskt->w_len = 0;
        return w_len;
    }

    if (!TT_OK(dskt->w_result)) {
        WSASetLastError(WSAECONNABORTED);
        return -1;
    }

    n = TT_MIN(__WBUF_NUM, len);
    for (i = 0; i < n; ++i) {
        dskt->w_buf[i].len = (u_long)vec[i].iov_len;
        dskt->w_buf[i].buf = (char *)vec[i].iov_base;
    }
    if (dskt->udp) {
        DWORD sent = 0;
        if ((WSASend(dskt->s, dskt->w_buf, n, &sent, 0, NULL, NULL) == 0) ||
            (WSAGetLastError() == WSA_IO_PENDING)) {
            return sent;
        } else {
            TT_NET_ERROR_NTV("fail to deliver WSASend");
            WSASetLastError(WSAECONNABORTED);
            return -1;
        }
    } else {
        tt_memset(&dskt->w_ev.u.wov, 0, sizeof(WSAOVERLAPPED));
        if ((WSASend(dskt->s, dskt->w_buf, n, NULL, 0, &dskt->w_ev.u.wov, NULL) ==
             0) ||
            (WSAGetLastError() == WSA_IO_PENDING)) {
            dskt->writing = TT_TRUE;
            WSASetLastError(WSAEWOULDBLOCK);
        } else {
            TT_NET_ERROR_NTV("fail to deliver WSASend");
            WSASetLastError(WSAECONNABORTED);
        }
        return -1;
    }
}

// ========================================
// io handler
// ========================================

tt_bool_t __do_connect(IN tt_io_ev_t *io_ev)
{
    __dskt_t *dskt = TT_CONTAINER(io_ev, __dskt_t, w_ev);

    dskt->w_ev.ev = __DNS_WRITE;
    dskt->reading = TT_FALSE;
    dskt->writing = TT_FALSE;

    if (!TT_OK(io_ev->io_result) ||
        (setsockopt(dskt->s, SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, NULL, 0) !=
         0)) {
        TT_NET_ERROR_NTV("connect failed");
        dskt->r_result = TT_FAIL;
        dskt->w_result = TT_FAIL;
    }

    return TT_TRUE;
}

tt_bool_t __do_read(IN tt_io_ev_t *io_ev)
{
    __dskt_t *dskt = TT_CONTAINER(io_ev, __dskt_t, r_ev);

    if (dskt->closing && !dskt->writing) {
        __dskt_reset(dskt);
        return TT_TRUE;
    }

    dskt->reading = TT_FALSE;

    if (io_ev->io_bytes > 0) {
        tt_buf_inc_wp(&dskt->r_buf, io_ev->io_bytes);
        dskt->r_result = TT_SUCCESS;
    } else if (TT_OK(io_ev->io_result)) {
        dskt->r_result = TT_E_END;
    } else {
        dskt->r_result = io_ev->io_result;
    }

    ares_process_fd(dskt->ch, __DSKT_IDX(dskt), ARES_SOCKET_BAD);

    return TT_TRUE;
}

tt_bool_t __do_write(IN tt_io_ev_t *io_ev)
{
    __dskt_t *dskt = TT_CONTAINER(io_ev, __dskt_t, w_ev);

    if (dskt->closing && !dskt->reading) {
        __dskt_reset(dskt);
        return TT_TRUE;
    }

    dskt->writing = TT_FALSE;

    if (TT_OK(io_ev->io_result)) {
        TT_ASSERT(dskt->w_len == 0);
        dskt->w_len = io_ev->io_bytes;
        dskt->w_result = TT_SUCCESS;
    } else {
        dskt->w_result = io_ev->io_result;
    }

    ares_process_fd(dskt->ch, ARES_SOCKET_BAD, __DSKT_IDX(dskt));

    return TT_TRUE;
}
