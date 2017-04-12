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

#include <tt_socket_native.h>

#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <io/tt_io_event.h>
#include <io/tt_socket.h>
#include <log/tt_log.h>
#include <os/tt_task.h>

#include <tt_cstd_api.h>
#include <tt_util_native.h>
#include <tt_wchar.h>

#include <ifdef.h>
#include <mswsock.h>
#include <netioapi.h>
#include <ws2tcpip.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define TT_ASSERT_SKT TT_ASSERT

//#define __SIMU_FAIL_SOCKET
//#define __SIMU_FAIL_CLOSE
//#define __SIMU_FAIL_SHUTDOWN
//#define __SIMU_FAIL_BIND
//#define __SIMU_FAIL_LISTEN
//#define __SIMU_FAIL_ACCEPT
//#define __SIMU_FAIL_CONNECT
//#define __SIMU_FAIL_SEND
//#define __SIMU_FAIL_RECV
//#define __SIMU_FAIL_SENDTO
//#define __SIMU_FAIL_RECVFROM

#ifdef __SIMU_FAIL_SOCKET
#define socket __sf_socket
int __sf_socket(int domain, int type, int protocol);
#endif

#ifdef __SIMU_FAIL_CLOSE
#define close __sf_close
int __sf_close(int fildes);
#endif

#ifdef __SIMU_FAIL_SHUTDOWN
#define shutdown __sf_shutdown
int __sf_shutdown(int socket, int how);
#endif

#ifdef __SIMU_FAIL_BIND
#define bind __sf_bind
int __sf_bind(int socket,
              const struct sockaddr *address,
              socklen_t address_len);
#endif

#ifdef __SIMU_FAIL_LISTEN
#define listen __sf_listen
int __sf_listen(int socket, int backlog);
#endif

#ifdef __SIMU_FAIL_ACCEPT
#define accept __sf_accept
int __sf_accept(int socket,
                struct sockaddr *restrict address,
                socklen_t *restrict address_len);
#endif

#ifdef __SIMU_FAIL_CONNECT
#define connect __sf_connect
int __sf_connect(int socket,
                 const struct sockaddr *address,
                 socklen_t address_len);
#endif

#ifdef __SIMU_FAIL_SEND
#define send __sf_send
ssize_t __sf_send(int socket, const void *buffer, size_t length, int flags);
#endif

#ifdef __SIMU_FAIL_RECV
#define recv __sf_recv
ssize_t __sf_recv(int socket, void *buffer, size_t length, int flags);
#endif

#ifdef __SIMU_FAIL_SENDTO
#define sendto __sf_sendto
ssize_t __sf_sendto(int socket,
                    const void *buffer,
                    size_t length,
                    int flags,
                    const struct sockaddr *dest_addr,
                    socklen_t dest_len);
#endif

#ifdef __SIMU_FAIL_RECVFROM
#define recvfrom __sf_recvfrom
ssize_t __sf_recvfrom(int socket,
                      void *restrict buffer,
                      size_t length,
                      int flags,
                      struct sockaddr *restrict address,
                      socklen_t *restrict address_len);
#endif

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

enum
{
    __SKT_ACCEPT,
    __SKT_CONNECT,
    __SKT_SEND,
    __SKT_RECV,
    __SKT_SENDTO,
    __SKT_RECVFROM,

    __SKT_EV_NUM,
};

typedef struct
{
    tt_io_ev_t io_ev;

    tt_skt_ntv_t *skt;
    tt_skt_ntv_t *new_skt;
    tt_sktaddr_t *addr;

    tt_result_t result;
    HANDLE iocp;
    tt_u8_t buf[(sizeof(SOCKADDR_STORAGE) + 16) << 1];
} __skt_accept_t;

typedef struct
{
    tt_io_ev_t io_ev;

    tt_skt_ntv_t *skt;
    tt_sktaddr_t *addr;

    tt_result_t result;
} __skt_connect_t;

typedef struct
{
    tt_io_ev_t io_ev;

    tt_skt_ntv_t *skt;
    tt_u8_t *buf;
    tt_u32_t *sent;
    tt_u32_t len;

    tt_result_t result;
    tt_u32_t pos;
} __skt_send_t;

typedef struct
{
    tt_io_ev_t io_ev;

    tt_skt_ntv_t *skt;
    tt_u8_t *buf;
    tt_u32_t *recvd;
    tt_u32_t len;

    tt_result_t result;
} __skt_recv_t;

typedef struct
{
    tt_io_ev_t io_ev;

    tt_skt_ntv_t *skt;
    tt_u8_t *buf;
    tt_u32_t *sent;
    tt_sktaddr_t *addr;
    tt_u32_t len;
    tt_u32_t addr_len;

    tt_result_t result;
    tt_u32_t pos;
} __skt_sendto_t;

typedef struct
{
    tt_io_ev_t io_ev;

    tt_skt_ntv_t *skt;
    tt_u8_t *buf;
    tt_u32_t *recvd;
    tt_sktaddr_t *addr;
    tt_u32_t len;

    tt_result_t result;
} __skt_recvfrom_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static BOOL(PASCAL FAR *tt_ConnectEx)(SOCKET s,
                                      const struct sockaddr *name,
                                      int namelen,
                                      PVOID lpSendBuffer,
                                      DWORD dwSendDataLength,
                                      LPDWORD lpdwBytesSent,
                                      LPOVERLAPPED lpOverlapped);

static BOOL(PASCAL FAR *tt_AcceptEx)(SOCKET sListenSocket,
                                     SOCKET sAcceptSocket,
                                     PVOID lpOutputBuffer,
                                     DWORD dwReceiveDataLength,
                                     DWORD dwLocalAddressLength,
                                     DWORD dwRemoteAddressLength,
                                     LPDWORD lpdwBytesReceived,
                                     LPOVERLAPPED lpOverlapped);

static void(PASCAL FAR *tt_GetAcceptExSockaddrs)(PVOID lpOutputBuffer,
                                                 DWORD dwReceiveDataLength,
                                                 DWORD dwLocalAddressLength,
                                                 DWORD dwRemoteAddressLength,
                                                 LPSOCKADDR *LocalSockaddr,
                                                 LPINT LocalSockaddrLength,
                                                 LPSOCKADDR *RemoteSockaddr,
                                                 LPINT RemoteSockaddrLength);

static tt_bool_t __do_accept(IN tt_io_ev_t *io_ev);

static tt_bool_t __do_connect(IN tt_io_ev_t *io_ev);

static tt_bool_t __do_send(IN tt_io_ev_t *io_ev);

static tt_bool_t __do_recv(IN tt_io_ev_t *io_ev);

static tt_bool_t __do_sendto(IN tt_io_ev_t *io_ev);

static tt_bool_t __do_recvfrom(IN tt_io_ev_t *io_ev);

static tt_poller_io_t __skt_poller_io[__SKT_EV_NUM] = {
    __do_accept, __do_connect, __do_send, __do_recv, __do_sendto, __do_recvfrom,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __init_api();

static tt_result_t __load_api(IN SOCKET s, const IN GUID *guid, IN void **pfn);

static HANDLE __skt_ev_init(IN tt_io_ev_t *io_ev, IN tt_u32_t ev);

static tt_result_t __addr_to_mreq(IN tt_sktaddr_ip_t *addr,
                                  IN const tt_char_t *itf,
                                  OUT struct ip_mreq *mreq);

static tt_result_t __addr_to_mreq6(IN tt_sktaddr_ip_t *addr,
                                   IN const tt_char_t *itf,
                                   OUT struct ipv6_mreq *mreq);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_skt_component_init_ntv(IN tt_profile_t *profile)
{
    if (!TT_OK(__init_api())) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_skt_create_ntv(IN tt_skt_ntv_t *skt,
                              IN tt_net_family_t family,
                              IN tt_net_protocol_t protocol,
                              IN OPT tt_skt_attr_t *attr)
{
    int af, type, proto;
    SOCKET s;
    LINGER linger = {0};
    HANDLE iocp;

    if (family == TT_NET_AF_INET) {
        af = AF_INET;
    } else {
        TT_ASSERT_SKT(family == TT_NET_AF_INET6);
        af = AF_INET6;
    }

    if (protocol == TT_NET_PROTO_TCP) {
        type = SOCK_STREAM;
        proto = IPPROTO_TCP;
    } else {
        TT_ASSERT_SKT(protocol == TT_NET_PROTO_UDP);
        type = SOCK_DGRAM;
        proto = IPPROTO_UDP;
    }

    s = WSASocket(af, type, proto, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (s == INVALID_SOCKET) {
        TT_NET_ERROR_NTV("fail to create socket");
        return TT_FAIL;
    }

    if (protocol == TT_NET_PROTO_UDP) {
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
    } else {
        if (setsockopt(s,
                       SOL_SOCKET,
                       SO_LINGER,
                       (char *)&linger,
                       sizeof(LINGER)) != 0) {
            TT_NET_ERROR_NTV("fail to set SO_LINGER");
            goto fail;
        }
    }

    iocp = tt_current_fiber_sched()->thread->task->iop.sys_iop.iocp;
    if (CreateIoCompletionPort((HANDLE)s, iocp, (ULONG_PTR)skt, 0) == NULL) {
        TT_NET_ERROR_NTV("fail to bind socket to iocp");
        goto fail;
    }

    skt->s = s;
    skt->af = af;

    tt_skt_stat_inc_num();
    return TT_SUCCESS;

fail:

    closesocket(s);
    return TT_FAIL;
}

void tt_skt_destroy_ntv(IN tt_skt_ntv_t *skt)
{
    if (closesocket(skt->s) != 0) {
        TT_NET_ERROR_NTV("fail to close socket");
    }
}

tt_result_t tt_skt_shutdown_ntv(IN tt_skt_ntv_t *skt, IN tt_skt_shut_t shut)
{
    int how;

    if (shut == TT_SKT_SHUT_RD) {
        how = SD_RECEIVE;
    } else if (shut == TT_SKT_SHUT_WR) {
        how = SD_SEND;
    } else {
        TT_ASSERT_SKT(shut == TT_SKT_SHUT_RDWR);
        how = SD_BOTH;
    }

    if (shutdown(skt->s, how) == 0) {
        return TT_SUCCESS;
    } else {
        TT_NET_ERROR_NTV("fail to shutdown socket");
        return TT_FAIL;
    }
}

tt_result_t tt_skt_bind_ntv(IN tt_skt_ntv_t *skt, IN tt_sktaddr_t *addr)
{
    if (bind(skt->s, (struct sockaddr *)addr, sizeof(tt_sktaddr_t)) == 0) {
        return TT_SUCCESS;
    } else {
        TT_NET_ERROR_NTV("bind fail");
        return TT_FAIL;
    }
}

tt_result_t tt_skt_listen_ntv(IN tt_skt_ntv_t *skt)
{
    if (listen(skt->s, SOMAXCONN) == 0) {
        return TT_SUCCESS;
    } else {
        TT_NET_ERROR_NTV("listening fail");
        return TT_FAIL;
    }
}

tt_result_t tt_skt_accept_ntv(IN tt_skt_ntv_t *skt,
                              OUT tt_skt_ntv_t *new_skt,
                              OUT tt_sktaddr_t *addr)
{
    SOCKET new_s;
    __skt_accept_t skt_accept;
    HANDLE iocp;
    DWORD dwBytesReceived;

    new_s = WSASocket(skt->af,
                      SOCK_STREAM,
                      IPPROTO_TCP,
                      NULL,
                      0,
                      WSA_FLAG_OVERLAPPED);
    if (new_s == INVALID_SOCKET) {
        TT_NET_ERROR_NTV("fail to create accept socket");
        return TT_FAIL;
    }
    new_skt->s = new_s;
    new_skt->af = skt->af;

    iocp = __skt_ev_init(&skt_accept.io_ev, __SKT_ACCEPT);

    skt_accept.skt = skt;
    skt_accept.new_skt = new_skt;
    skt_accept.addr = addr;

    skt_accept.result = TT_FAIL;
    skt_accept.iocp = iocp;

    if (!tt_AcceptEx(skt->s,
                     new_s,
                     skt_accept.buf,
                     0,
                     sizeof(SOCKADDR_STORAGE) + 16,
                     sizeof(SOCKADDR_STORAGE) + 16,
                     &dwBytesReceived,
                     &skt_accept.io_ev.wov) &&
        (WSAGetLastError() != ERROR_IO_PENDING)) {
        TT_NET_ERROR_NTV("AcceptEx failed");
        closesocket(new_s);
        return TT_FAIL;
    }

    tt_fiber_yield();
    return skt_accept.result;
}

tt_result_t tt_skt_connect_ntv(IN tt_skt_ntv_t *skt, IN tt_sktaddr_t *addr)
{
    tt_sktaddr_t a;
    __skt_connect_t skt_connect;
    HANDLE iocp;

    // ConnectEx need a bind
    tt_sktaddr_init(&a,
                    TT_COND(skt->af == AF_INET,
                            TT_NET_AF_INET,
                            TT_NET_AF_INET6));
    tt_sktaddr_set_ip_n(&a, TT_IP_ANY);
    // on windows, port 0 means allocate a port
    tt_sktaddr_set_port(&a, 0);
    if (bind(skt->s, (struct sockaddr *)&a, sizeof(tt_sktaddr_t)) != 0) {
        TT_NET_ERROR_NTV("bind fail");
        return TT_FAIL;
    }

    iocp = __skt_ev_init(&skt_connect.io_ev, __SKT_CONNECT);

    skt_connect.skt = skt;
    skt_connect.addr = addr;

    skt_connect.result = TT_FAIL;

    if (!tt_ConnectEx(skt->s,
                      (const struct sockaddr *)addr,
                      TT_COND(tt_sktaddr_get_family(addr) == TT_NET_AF_INET,
                              sizeof(struct sockaddr_in),
                              sizeof(struct sockaddr_in6)),
                      NULL,
                      0,
                      NULL,
                      &skt_connect.io_ev.wov) &&
        (WSAGetLastError() != WSA_IO_PENDING)) {
        TT_NET_ERROR_NTV("ConnectEx fail");
        return TT_FAIL;
    }

    tt_fiber_yield();
    return skt_connect.result;
}

tt_result_t tt_skt_local_addr_ntv(IN tt_skt_ntv_t *skt, OUT tt_sktaddr_t *addr)
{
    int len = sizeof(tt_sktaddr_t);

    if (getsockname(skt->s, (struct sockaddr *)addr, &len) == 0) {
        return TT_SUCCESS;
    } else {
        TT_NET_ERROR_NTV("fail to get local address");
        return TT_FAIL;
    }
}

tt_result_t tt_skt_remote_addr_ntv(IN tt_skt_ntv_t *skt, OUT tt_sktaddr_t *addr)
{
    int len = sizeof(tt_sktaddr_t);

    if (getpeername(skt->s, (struct sockaddr *)addr, &len) == 0) {
        return TT_SUCCESS;
    } else {
        TT_NET_ERROR_NTV("fail to get remote address");
        return TT_FAIL;
    }
}

tt_result_t tt_skt_recvfrom_ntv(IN tt_skt_ntv_t *skt,
                                OUT tt_u8_t *buf,
                                IN tt_u32_t len,
                                OUT OPT tt_u32_t *recvd,
                                OUT tt_sktaddr_t *addr)
{
    __skt_recvfrom_t skt_recvfrom;
    WSABUF Buffers;
    DWORD Flags = 0;
    INT Fromlen = sizeof(tt_sktaddr_t);

    __skt_ev_init(&skt_recvfrom.io_ev, __SKT_RECVFROM);

    skt_recvfrom.skt = skt;
    skt_recvfrom.buf = buf;
    skt_recvfrom.recvd = recvd;
    skt_recvfrom.addr = addr;
    skt_recvfrom.len = len;

    skt_recvfrom.result = TT_FAIL;

    // If the message is larger than the buffers, the buffers are filled
    // with the first part of the message. If the MSG_PARTIAL feature is
    // supported by the underlying service provider, the MSG_PARTIAL flag
    // is set in lpFlags and subsequent receive operation(s) will retrieve
    // the rest of the message. If MSG_PARTIAL is not supported, but the
    // protocol is reliable, WSARecvFrom generates the error WSAEMSGSIZE
    // and a subsequent receive operation with a larger buffer can be
    // used to retrieve the entire message. Otherwise, (that is, the protocol
    // is unreliable and does not support MSG_PARTIAL), the excess data
    // is lost, and WSARecvFrom generates the error WSAEMSGSIZE

    Buffers.buf = (char *)buf;
    Buffers.len = len;
    if ((WSARecvFrom(skt->s,
                     &Buffers,
                     1,
                     NULL,
                     &Flags,
                     (struct sockaddr *)addr,
                     (LPINT)&Fromlen,
                     &skt_recvfrom.io_ev.wov,
                     NULL) == 0) ||
        (WSAGetLastError() == WSA_IO_PENDING)) {
        tt_fiber_yield();
        return skt_recvfrom.result;
    }

    TT_NET_ERROR_NTV("WSARecvFrom fail");
    return TT_FAIL;
}

tt_result_t tt_skt_sendto_ntv(IN tt_skt_ntv_t *skt,
                              IN tt_u8_t *buf,
                              IN tt_u32_t len,
                              OUT OPT tt_u32_t *sent,
                              IN tt_sktaddr_t *addr)
{
    __skt_sendto_t skt_sendto;
    WSABUF Buffers;

    __skt_ev_init(&skt_sendto.io_ev, __SKT_SENDTO);

    skt_sendto.skt = skt;
    skt_sendto.buf = buf;
    skt_sendto.sent = sent;
    skt_sendto.addr = addr;
    skt_sendto.len = len;
    if (tt_sktaddr_get_family(addr) == TT_NET_AF_INET) {
        skt_sendto.addr_len = sizeof(struct sockaddr_in);
    } else {
        skt_sendto.addr_len = sizeof(struct sockaddr_in6);
    }

    skt_sendto.result = TT_FAIL;
    skt_sendto.pos = 0;

    Buffers.buf = (char *)buf;
    Buffers.len = len;
    if ((WSASendTo(skt->s,
                   &Buffers,
                   1,
                   NULL,
                   0,
                   (struct sockaddr *)addr,
                   skt_sendto.addr_len,
                   &skt_sendto.io_ev.wov,
                   NULL) == 0) ||
        (WSAGetLastError() == WSA_IO_PENDING)) {
        tt_fiber_yield();
        return skt_sendto.result;
    }

    TT_NET_ERROR_NTV("WSASendTo fail");
    return TT_FAIL;
}

tt_result_t tt_skt_send_ntv(IN tt_skt_ntv_t *skt,
                            IN tt_u8_t *buf,
                            IN tt_u32_t len,
                            OUT OPT tt_u32_t *sent)
{
    __skt_send_t skt_send;
    WSABUF Buffers;
    DWORD dwError;

    __skt_ev_init(&skt_send.io_ev, __SKT_SEND);

    skt_send.skt = skt;
    skt_send.buf = buf;
    skt_send.sent = sent;
    skt_send.len = len;

    skt_send.result = TT_FAIL;
    skt_send.pos = 0;

    Buffers.buf = (char *)buf;
    Buffers.len = len;
    if ((WSASend(skt->s, &Buffers, 1, NULL, 0, &skt_send.io_ev.wov, NULL) ==
         0) ||
        ((dwError = WSAGetLastError()) == WSA_IO_PENDING)) {
        tt_fiber_yield();
        return skt_send.result;
    }

    if ((dwError == WSAECONNABORTED) || (dwError == WSAECONNRESET)) {
        return TT_END;
    } else {
        TT_NET_ERROR_NTV("WSASend fail");
        return TT_FAIL;
    }
}

tt_result_t tt_skt_recv_ntv(IN tt_skt_ntv_t *skt,
                            OUT tt_u8_t *buf,
                            IN tt_u32_t len,
                            OUT OPT tt_u32_t *recvd)
{
    __skt_recv_t skt_recv;
    WSABUF Buffers;
    DWORD Flags = 0, dwError;

    __skt_ev_init(&skt_recv.io_ev, __SKT_RECV);

    skt_recv.skt = skt;
    skt_recv.buf = buf;
    skt_recv.len = len;
    skt_recv.recvd = recvd;

    skt_recv.result = TT_FAIL;

    Buffers.buf = (char *)buf;
    Buffers.len = len;
    if ((WSARecv(skt->s,
                 &Buffers,
                 1,
                 NULL,
                 &Flags,
                 &skt_recv.io_ev.wov,
                 NULL) == 0) ||
        ((dwError = WSAGetLastError()) == WSA_IO_PENDING)) {
        tt_fiber_yield();
        return skt_recv.result;
    }

    if ((dwError == WSAECONNABORTED) || (dwError == WSAECONNRESET)) {
        return TT_END;
    } else {
        TT_NET_ERROR_NTV("WSARecv fail");
        return TT_FAIL;
    }
}

tt_result_t tt_skt_join_mcast_ntv(IN tt_skt_ntv_t *skt,
                                  IN tt_net_family_t family,
                                  IN tt_sktaddr_ip_t *addr,
                                  IN const tt_char_t *itf)
{
    if (family == TT_NET_AF_INET) {
        struct ip_mreq mreq;
        if (!TT_OK(__addr_to_mreq(addr, itf, &mreq))) {
            return TT_FAIL;
        }

        // join multicast group
        if (setsockopt(skt->s,
                       IPPROTO_IP,
                       IP_ADD_MEMBERSHIP,
                       (char *)&mreq,
                       (int)sizeof(struct ip_mreq)) == 0) {
            return TT_SUCCESS;
        } else {
            TT_NET_ERROR_NTV("fail to join multicast group");
            return TT_FAIL;
        }
    } else {
        struct ipv6_mreq mreq;

        TT_ASSERT_SKT(family == TT_NET_AF_INET6);

        if (!TT_OK(__addr_to_mreq6(addr, itf, &mreq))) {
            return TT_FAIL;
        }

        // join multicast group
        if (setsockopt(skt->s,
                       IPPROTO_IPV6,
                       IPV6_JOIN_GROUP,
                       (char *)&mreq,
                       (int)sizeof(struct ipv6_mreq)) == 0) {
            return TT_SUCCESS;
        } else {
            TT_NET_ERROR_NTV("fail to join multicast group");
            return TT_FAIL;
        }
    }
}

tt_result_t tt_skt_leave_mcast_ntv(IN tt_skt_ntv_t *skt,
                                   IN tt_net_family_t family,
                                   IN tt_sktaddr_ip_t *addr,
                                   IN const tt_char_t *itf)
{
    if (family == TT_NET_AF_INET) {
        struct ip_mreq mreq;
        if (!TT_OK(__addr_to_mreq(addr, itf, &mreq))) {
            return TT_FAIL;
        }

        // leave multicast group
        if (setsockopt(skt->s,
                       IPPROTO_IP,
                       IP_DROP_MEMBERSHIP,
                       (char *)&mreq,
                       (int)sizeof(struct ip_mreq)) == 0) {
            return TT_SUCCESS;
        } else {
            TT_NET_ERROR_NTV("fail to leave multicast group");
            return TT_FAIL;
        }
    } else {
        struct ipv6_mreq mreq;

        TT_ASSERT_SKT(family == TT_NET_AF_INET6);

        if (!TT_OK(__addr_to_mreq6(addr, itf, &mreq))) {
            return TT_FAIL;
        }

        // leave multicast group
        if (setsockopt(skt->s,
                       IPPROTO_IPV6,
                       IPV6_DROP_MEMBERSHIP,
                       (char *)&mreq,
                       (int)sizeof(struct ipv6_mreq)) == 0) {
            return TT_SUCCESS;
        } else {
            TT_NET_ERROR_NTV("fail to leave multicast group");
            return TT_FAIL;
        }
    }
}

void tt_skt_worker_io(IN tt_io_ev_t *io_ev)
{
}

tt_bool_t tt_skt_poller_io(IN tt_io_ev_t *io_ev)
{
    return __skt_poller_io[io_ev->ev](io_ev);
}

HANDLE __skt_ev_init(IN tt_io_ev_t *io_ev, IN tt_u32_t ev)
{
    io_ev->src = tt_current_fiber();
    io_ev->dst = NULL;
    tt_dnode_init(&io_ev->node);
    tt_memset(&io_ev->wov, 0, sizeof(WSAOVERLAPPED));
    io_ev->io_bytes = 0;
    io_ev->io_result = TT_FAIL;
    io_ev->io = TT_IO_SOCKET;
    io_ev->ev = ev;

    return io_ev->src->fs->thread->task->iop.sys_iop.iocp;
}

tt_result_t __addr_to_mreq(IN tt_sktaddr_ip_t *addr,
                           IN const tt_char_t *itf,
                           OUT struct ip_mreq *mreq)
{
    // address
    mreq->imr_multiaddr.s_addr = addr->a32.__u32;

    // interface
    if (itf != NULL) {
        WCHAR *InterfaceName;
        NET_LUID luid;
        NETIO_STATUS ns;
        NET_IFINDEX ifidx;

        InterfaceName = tt_wchar_create(itf, NULL);
        if (InterfaceName == NULL) {
            return TT_FAIL;
        }

        ns = ConvertInterfaceNameToLuidW(InterfaceName, &luid);
        tt_wchar_destroy(InterfaceName);
        if (ns != 0) {
            TT_ERROR("can not get luid of %s", itf);
            return TT_FAIL;
        }
        if (ConvertInterfaceLuidToIndex(&luid, &ifidx) != NO_ERROR) {
            TT_ERROR("can not get index from luid of %s", itf);
            return TT_FAIL;
        }

        // windows accept setting imr_interface to address like
        // "0.x.x.x" where "x.x.x" represent the interface index
        if (ifidx >= (1 << 24)) {
            TT_ERROR("invalid interface index: %x", ifidx);
            return TT_FAIL;
        }
        mreq->imr_interface.s_addr = htonl(ifidx);
    } else {
        // use default interface
        memset(&mreq->imr_interface, 0, sizeof(struct in_addr));
    }

    return TT_SUCCESS;
}

tt_result_t __addr_to_mreq6(IN tt_sktaddr_ip_t *addr,
                            IN const tt_char_t *itf,
                            OUT struct ipv6_mreq *mreq)
{
    // address
    tt_memcpy(mreq->ipv6mr_multiaddr.s6_bytes, addr->a128.__u8, 16);

    // interface
    if (itf != NULL) {
        WCHAR *InterfaceName;
        NET_LUID luid;
        NETIO_STATUS ns;
        NET_IFINDEX ifidx;

        InterfaceName = tt_wchar_create(itf, NULL);
        if (InterfaceName == NULL) {
            return TT_FAIL;
        }

        ns = ConvertInterfaceNameToLuidW(InterfaceName, &luid);
        tt_wchar_destroy(InterfaceName);
        if (ns != 0) {
            TT_ERROR("can not get luid of %s", itf);
            return TT_FAIL;
        }
        if (ConvertInterfaceLuidToIndex(&luid, &ifidx) != NO_ERROR) {
            TT_ERROR("can not get index from luid of %s", itf);
            return TT_FAIL;
        }
        mreq->ipv6mr_interface = ifidx;
    } else {
        // use default interface
        mreq->ipv6mr_interface = 0;
    }

    return TT_SUCCESS;
}

tt_bool_t __do_accept(IN tt_io_ev_t *io_ev)
{
    __skt_accept_t *skt_accept = (__skt_accept_t *)io_ev;

    SOCKET new_s = skt_accept->new_skt->s;
    LINGER linger = {0};
    LPSOCKADDR LocalSockaddr, RemoteSockaddr;
    INT LocalSockaddrLength, RemoteSockaddrLength;

    if (!TT_OK(io_ev->io_result)) {
        goto fail;
    }

    if (setsockopt(new_s,
                   SOL_SOCKET,
                   SO_UPDATE_ACCEPT_CONTEXT,
                   (char *)&skt_accept->skt->s,
                   sizeof(SOCKET)) != 0) {
        TT_NET_ERROR_NTV("fail to set SO_UPDATE_ACCEPT_CONTEXT");
        goto fail;
    }

    if (setsockopt(new_s,
                   SOL_SOCKET,
                   SO_LINGER,
                   (char *)&linger,
                   sizeof(LINGER)) != 0) {
        TT_NET_ERROR_NTV("fail to set SO_LINGER");
        goto fail;
    }

    if (CreateIoCompletionPort((HANDLE)new_s,
                               skt_accept->iocp,
                               (ULONG_PTR)skt_accept->new_skt,
                               0) == NULL) {
        TT_NET_ERROR_NTV("fail to bind accept socket to iocp");
        goto fail;
    }

    LocalSockaddrLength = sizeof(tt_sktaddr_ntv_t);
    RemoteSockaddrLength = sizeof(tt_sktaddr_t);
    tt_GetAcceptExSockaddrs(skt_accept->buf,
                            0,
                            sizeof(SOCKADDR_STORAGE) + 16,
                            sizeof(SOCKADDR_STORAGE) + 16,
                            &LocalSockaddr,
                            &LocalSockaddrLength,
                            &RemoteSockaddr,
                            &RemoteSockaddrLength);

    TT_ASSERT_SKT(RemoteSockaddrLength <= sizeof(tt_sktaddr_t));
    tt_memcpy(skt_accept->addr, RemoteSockaddr, RemoteSockaddrLength);

    skt_accept->result = TT_SUCCESS;
    return TT_TRUE;

fail:

    closesocket(new_s);

    skt_accept->result = TT_FAIL;
    return TT_TRUE;
}

tt_bool_t __do_connect(IN tt_io_ev_t *io_ev)
{
    __skt_connect_t *skt_connect = (__skt_connect_t *)io_ev;

    if (!TT_OK(io_ev->io_result)) {
        skt_connect->result = TT_FAIL;
        return TT_TRUE;
    }

    // The socket s does not enable previously set properties or
    // options until SO_UPDATE_CONNECT_CONTEXT is set on the socket
    if (setsockopt(skt_connect->skt->s,
                   SOL_SOCKET,
                   SO_UPDATE_CONNECT_CONTEXT,
                   NULL,
                   0) != 0) {
        TT_NET_ERROR_NTV("fail to set SO_UPDATE_CONNECT_CONTEXT");
        skt_connect->result = TT_FAIL;
        return TT_TRUE;
    }

    skt_connect->result = TT_SUCCESS;
    return TT_TRUE;
}

tt_bool_t __do_send(IN tt_io_ev_t *io_ev)
{
    __skt_send_t *skt_send = (__skt_send_t *)io_ev;

    WSABUF Buffers;
    DWORD dwError;

    skt_send->pos += io_ev->io_bytes;
    if (skt_send->pos == skt_send->len) {
        *skt_send->sent = skt_send->pos;
        skt_send->result = TT_SUCCESS;
        return TT_TRUE;
    }
    TT_ASSERT(skt_send->pos < skt_send->len);

    // return success whenever some data is sent
    if (!TT_OK(io_ev->io_result)) {
        if (skt_send->pos > 0) {
            *skt_send->sent = skt_send->pos;
            skt_send->result = TT_SUCCESS;
        } else {
            skt_send->result = io_ev->io_result;
        }
        return TT_TRUE;
    }

    // send left data
    Buffers.buf = TT_PTR_INC(char, skt_send->buf, skt_send->pos);
    Buffers.len = skt_send->len - skt_send->pos;
    if ((WSASend(skt_send->skt->s,
                 &Buffers,
                 1,
                 NULL,
                 0,
                 &skt_send->io_ev.wov,
                 NULL) != 0) &&
        ((dwError = WSAGetLastError()) == WSA_IO_PENDING)) {
        return TT_FALSE;
    }

    // error
    if (skt_send->pos > 0) {
        *skt_send->sent = skt_send->pos;
        skt_send->result = TT_SUCCESS;
    } else if ((dwError == WSAECONNABORTED) || (dwError == WSAECONNRESET)) {
        skt_send->result = TT_END;
    } else {
        TT_NET_ERROR_NTV("WSASend fail");
        skt_send->result = TT_FAIL;
    }
    return TT_TRUE;
}

tt_bool_t __do_recv(IN tt_io_ev_t *io_ev)
{
    __skt_recv_t *skt_recv = (__skt_recv_t *)io_ev;

    TT_ASSERT(io_ev->io_bytes <= skt_recv->len);

    if (io_ev->io_bytes > 0) {
        *skt_recv->recvd = io_ev->io_bytes;
        skt_recv->result = TT_SUCCESS;
    } else if (TT_OK(io_ev->io_result)) {
        skt_recv->result = TT_END;
    } else {
        skt_recv->result = io_ev->io_result;
    }
    return TT_TRUE;
}

tt_bool_t __do_sendto(IN tt_io_ev_t *io_ev)
{
    __skt_sendto_t *skt_sendto = (__skt_sendto_t *)io_ev;

    WSABUF Buffers;

    skt_sendto->pos += io_ev->io_bytes;
    if (skt_sendto->pos == skt_sendto->len) {
        *skt_sendto->sent = skt_sendto->pos;
        skt_sendto->result = TT_SUCCESS;
        return TT_TRUE;
    }
    TT_ASSERT(skt_sendto->pos < skt_sendto->len);

    // return success whenever some data is sent
    if (!TT_OK(io_ev->io_result)) {
        if (skt_sendto->pos > 0) {
            *skt_sendto->sent = skt_sendto->pos;
            skt_sendto->result = TT_SUCCESS;
        } else {
            skt_sendto->result = io_ev->io_result;
        }
        return TT_TRUE;
    }

    // send left data
    Buffers.buf = TT_PTR_INC(char, skt_sendto->buf, skt_sendto->pos);
    Buffers.len = skt_sendto->len - skt_sendto->pos;
    if ((WSASendTo(skt_sendto->skt->s,
                   &Buffers,
                   1,
                   NULL,
                   0,
                   (struct sockaddr *)skt_sendto->addr,
                   skt_sendto->addr_len,
                   &skt_sendto->io_ev.wov,
                   NULL) == 0) ||
        (WSAGetLastError() == WSA_IO_PENDING)) {
        return TT_FALSE;
    }

    // error
    if (skt_sendto->pos > 0) {
        *skt_sendto->sent = skt_sendto->pos;
        skt_sendto->result = TT_SUCCESS;
    } else {
        TT_NET_ERROR_NTV("WSASendTo fail");
        skt_sendto->result = TT_FAIL;
    }
    return TT_TRUE;
}

tt_bool_t __do_recvfrom(IN tt_io_ev_t *io_ev)
{
    __skt_recvfrom_t *skt_recvfrom = (__skt_recvfrom_t *)io_ev;

    if (io_ev->io_bytes > 0) {
        *skt_recvfrom->recvd = io_ev->io_bytes;
        skt_recvfrom->result = TT_SUCCESS;
    } else {
        skt_recvfrom->result = io_ev->io_result;
    }
    return TT_TRUE;
}

tt_result_t __init_api()
{
    SOCKET s;
    const GUID guid_ConnectEx = WSAID_CONNECTEX;
    const GUID guid_AcceptEx = WSAID_ACCEPTEX;
    const GUID guid_GetAcceptExSockaddrs = WSAID_GETACCEPTEXSOCKADDRS;
    const GUID guid_DisconnectEx = WSAID_DISCONNECTEX;

    s = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, 0);
    if (s == INVALID_SOCKET) {
        TT_NET_ERROR_NTV("can not create a udp socket");
        return TT_FAIL;
    }

    if (!TT_OK(__load_api(s, &guid_ConnectEx, (void **)&tt_ConnectEx)) ||
        !TT_OK(__load_api(s, &guid_AcceptEx, (void **)&tt_AcceptEx)) ||
        !TT_OK(__load_api(s,
                          &guid_GetAcceptExSockaddrs,
                          (void **)&tt_GetAcceptExSockaddrs))) {
        closesocket(s);
        return TT_FAIL;
    }

    closesocket(s);
    return TT_SUCCESS;
}

tt_result_t __load_api(IN SOCKET s, const IN GUID *guid, IN void **pfn)
{
    DWORD bytes;
    if ((WSAIoctl(s,
                  SIO_GET_EXTENSION_FUNCTION_POINTER,
                  (LPVOID)guid,
                  sizeof(GUID),
                  pfn,
                  sizeof(void *),
                  &bytes,
                  NULL,
                  NULL) == 0) &&
        (*pfn != NULL)) {
        return TT_SUCCESS;
    } else {
        TT_NET_ERROR_NTV("fail to load winsock api");
        return TT_FAIL;
    }
}

#ifdef __SIMU_FAIL_SOCKET
#undef socket
int __sf_socket(int domain, int type, int protocol)
{
    return -1;
}
#endif

#ifdef __SIMU_FAIL_CLOSE
#undef close
int __sf_close(int fildes)
{
    return -1;
}
#endif

#ifdef __SIMU_FAIL_SHUTDOWN
#undef shutdown
int __sf_shutdown(int socket, int how)

{
    return -1;
}
#endif

#ifdef __SIMU_FAIL_BIND
#undef bind
int __sf_bind(int socket, const struct sockaddr *address, socklen_t address_len)

{
    return -1;
}
#endif

#ifdef __SIMU_FAIL_LISTEN
#undef listen
int __sf_listen(int socket, int backlog)
{
    return -1;
}
#endif

#ifdef __SIMU_FAIL_ACCEPT
#undef accept
int __sf_accept(int socket,
                struct sockaddr *restrict address,
                socklen_t *restrict address_len)
{
    return -1;
}
#endif

#ifdef __SIMU_FAIL_CONNECT
#undef connect
int __sf_connect(int socket,
                 const struct sockaddr *address,
                 socklen_t address_len)
{
    return -1;
}
#endif

#ifdef __SIMU_FAIL_SEND
#undef send
ssize_t __sf_send(int socket, const void *buffer, size_t length, int flags)
{
    if (tt_rand_u32() % 2) {
        return -1;
    } else {
        return send(socket, buffer, tt_rand_u32() % length, flags);
    }
}
#endif

#ifdef __SIMU_FAIL_RECV
#undef recv
ssize_t __sf_recv(int socket, void *buffer, size_t length, int flags)
{
    if (tt_rand_u32() % 2) {
        return -1;
    } else {
        return recv(socket, buffer, tt_rand_u32() % length, flags);
    }
}
#endif

#ifdef __SIMU_FAIL_SENDTO
#undef sendto
ssize_t __sf_sendto(int socket,
                    const void *buffer,
                    size_t length,
                    int flags,
                    const struct sockaddr *dest_addr,
                    socklen_t dest_len)
{
    if (tt_rand_u32() % 2) {
        return -1;
    } else {
        return sendto(socket,
                      buffer,
                      tt_rand_u32() % length,
                      flags,
                      dest_addr,
                      dest_len);
    }
}
#endif

#ifdef __SIMU_FAIL_RECVFROM
#undef recvfrom
ssize_t __sf_recvfrom(int socket,
                      void *restrict buffer,
                      size_t length,
                      int flags,
                      struct sockaddr *restrict address,
                      socklen_t *restrict address_len)
{
    if (tt_rand_u32() % 2) {
        return -1;
    } else {
        return recvfrom(socket,
                        buffer,
                        tt_rand_u32() % length,
                        flags,
                        address,
                        address_len);
    }
}
#endif
