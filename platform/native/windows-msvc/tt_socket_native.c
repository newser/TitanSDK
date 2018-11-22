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

#include <tt_socket_native.h>

#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <io/tt_file_system.h>
#include <io/tt_io_event.h>
#include <io/tt_socket.h>
#include <log/tt_log.h>
#include <os/tt_fiber_event.h>
#include <os/tt_task.h>
#include <time/tt_timer.h>

#include <tt_cstd_api.h>
#include <tt_ntdll.h>
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

//#define __SIMU_FAIL_WSASocketW
//#define __SIMU_FAIL_closesocket
//#define __SIMU_FAIL_shutdown
//#define __SIMU_FAIL_bind
//#define __SIMU_FAIL_listen
//#define __SIMU_FAIL_AcceptEx
//#define __SIMU_FAIL_ConnectEx
//#define __SIMU_FAIL_WSASend
//#define __SIMU_FAIL_WSARecv
//#define __SIMU_FAIL_WSASendTo
//#define __SIMU_FAIL_WSARecvFrom

#ifdef __SIMU_FAIL_WSASocketW
#define WSASocketW __sf_WSASocketW
SOCKET __sf_WSASocketW(int af,
                       int type,
                       int protocol,
                       LPWSAPROTOCOL_INFOW lpProtocolInfo,
                       GROUP g,
                       DWORD dwFlags);
#endif

#ifdef __SIMU_FAIL_closesocket
#define closesocket __sf_closesocket
int __sf_closesocket(SOCKET s);
#endif

#ifdef __SIMU_FAIL_shutdown
#define shutdown __sf_shutdown
int __sf_shutdown(SOCKET s, int how);
#endif

#ifdef __SIMU_FAIL_bind
#define bind __sf_bind
int __sf_bind(SOCKET s, const struct sockaddr *name, int namelen);
#endif

#ifdef __SIMU_FAIL_listen
#define listen __sf_listen
int __sf_listen(SOCKET s, int backlog);
#endif

#ifdef __SIMU_FAIL_AcceptEx
#define AcceptEx __sf_AcceptEx
BOOL __sf_AcceptEx(SOCKET sListenSocket,
                   SOCKET sAcceptSocket,
                   PVOID lpOutputBuffer,
                   DWORD dwReceiveDataLength,
                   DWORD dwLocalAddressLength,
                   DWORD dwRemoteAddressLength,
                   LPDWORD lpdwBytesReceived,
                   LPOVERLAPPED lpOverlapped);
#endif

#ifdef __SIMU_FAIL_ConnectEx
#define ConnectEx __sf_ConnectEx
BOOL PASCAL __sf_ConnectEx(SOCKET s,
                           const struct sockaddr *name,
                           int namelen,
                           __in_opt PVOID lpSendBuffer,
                           DWORD dwSendDataLength,
                           LPDWORD lpdwBytesSent,
                           LPOVERLAPPED lpOverlapped);
#endif

#ifdef __SIMU_FAIL_WSASend
#define WSASend __sf_WSASend
int __sf_WSASend(SOCKET s,
                 LPWSABUF lpBuffers,
                 DWORD dwBufferCount,
                 LPDWORD lpNumberOfBytesSent,
                 DWORD dwFlags,
                 LPWSAOVERLAPPED lpOverlapped,
                 LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
#endif

#ifdef __SIMU_FAIL_WSARecv
#define WSARecv __sf_WSARecv
int __sf_WSARecv(SOCKET s,
                 LPWSABUF lpBuffers,
                 DWORD dwBufferCount,
                 LPDWORD lpNumberOfBytesRecvd,
                 LPDWORD lpFlags,
                 LPWSAOVERLAPPED lpOverlapped,
                 LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
#endif

#ifdef __SIMU_FAIL_WSASendTo
#define WSASendTo __sf_WSASendTo
int __sf_WSASendTo(SOCKET s,
                   LPWSABUF lpBuffers,
                   DWORD dwBufferCount,
                   LPDWORD lpNumberOfBytesSent,
                   DWORD dwFlags,
                   const struct sockaddr *lpTo,
                   int iToLen,
                   LPWSAOVERLAPPED lpOverlapped,
                   LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
#endif

#ifdef __SIMU_FAIL_WSARecvFrom
#define WSARecvFrom __sf_WSARecvFrom
int __sf_WSARecvFrom(SOCKET s,
                     LPWSABUF lpBuffers,
                     DWORD dwBufferCount,
                     LPDWORD lpNumberOfBytesRecvd,
                     LPDWORD lpFlags,
                     struct sockaddr *lpFrom,
                     LPINT lpFromlen,
                     LPWSAOVERLAPPED lpOverlapped,
                     LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
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
    __SKT_SENDFILE,

    __SKT_EV_NUM,
};

typedef struct
{
    tt_io_ev_t io_ev;

    tt_skt_ntv_t *skt;
    SOCKET new_s;
    tt_sktaddr_t *addr;

    tt_skt_t *new_skt;
    HANDLE iocp;
    tt_u8_t buf[(sizeof(SOCKADDR_STORAGE) + 16) << 1];
    tt_bool_t done : 1;
    tt_bool_t canceled : 1;
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

    tt_u64_t *sent;

    tt_result_t result;
} __skt_sendfile_t;

typedef struct
{
    tt_io_ev_t io_ev;

    tt_skt_ntv_t *skt;
    tt_u8_t *buf;
    tt_u32_t *recvd;
    tt_u32_t len;

    tt_result_t result;
    tt_bool_t done : 1;
    tt_bool_t canceled : 1;
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
    tt_sktaddr_t __addr;
    tt_u32_t len;

    tt_result_t result;
    tt_bool_t done : 1;
    tt_bool_t canceled : 1;
} __skt_recvfrom_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

BOOL(PASCAL FAR *tt_ConnectEx)
(SOCKET s,
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

static BOOL(PASCAL FAR *tt_TransmitFile)(
    SOCKET hSocket,
    HANDLE hFile,
    DWORD nNumberOfBytesToWrite,
    DWORD nNumberOfBytesPerSend,
    LPOVERLAPPED lpOverlapped,
    LPTRANSMIT_FILE_BUFFERS lpTransmitBuffers,
    DWORD dwFlags);

static tt_bool_t __do_accept(IN tt_io_ev_t *io_ev);

static tt_bool_t __do_connect(IN tt_io_ev_t *io_ev);

static tt_bool_t __do_send(IN tt_io_ev_t *io_ev);

static tt_bool_t __do_recv(IN tt_io_ev_t *io_ev);

static tt_bool_t __do_sendto(IN tt_io_ev_t *io_ev);

static tt_bool_t __do_recvfrom(IN tt_io_ev_t *io_ev);

static tt_bool_t __do_sendfile(IN tt_io_ev_t *io_ev);

static tt_poller_io_t __skt_poller_io[__SKT_EV_NUM] = {
    __do_accept,
    __do_connect,
    __do_send,
    __do_recv,
    __do_sendto,
    __do_recvfrom,
    __do_sendfile,
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

static tt_result_t __bind_iocp(IN tt_skt_ntv_t *skt, IN HANDLE iocp);

tt_inline tt_result_t __set_nonblock(IN SOCKET s, IN tt_bool_t nonblock)
{
    u_long nbio = TT_COND(nonblock, 1, 0);
    return TT_COND(ioctlsocket(s, FIONBIO, &nbio) == 0, TT_SUCCESS, TT_FAIL);
}

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

void tt_skt_component_exit_ntv()
{
}

void tt_skt_status_dump_ntv(IN tt_u32_t flag)
{
    tt_ntdll_dump_skt(flag);
}

tt_result_t tt_skt_create_ntv(IN tt_skt_ntv_t *skt,
                              IN tt_net_family_t family,
                              IN tt_net_protocol_t protocol,
                              IN OPT tt_skt_attr_t *attr)
{
    int af, type, proto;
    SOCKET s;

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

    s = WSASocketW(af, type, proto, NULL, 0, WSA_FLAG_OVERLAPPED);
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
    }

    if (!TT_OK(__set_nonblock(s, TT_TRUE))) {
        TT_NET_ERROR_NTV("fail to set nonblock");
        closesocket(s);
        return TT_FAIL;
    }

    skt->s = s;
    skt->af = af;
    skt->iocp = TT_FALSE;

    return TT_SUCCESS;
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

tt_skt_t *tt_skt_accept_ntv(IN tt_skt_ntv_t *skt,
                            OUT tt_sktaddr_t *addr,
                            OUT tt_fiber_ev_t **p_fev,
                            OUT tt_tmr_t **p_tmr)
{
    __skt_accept_t skt_accept;
    HANDLE iocp;
    tt_fiber_t *cfb;
    SOCKET new_s;
    DWORD dwBytesReceived;

    *p_fev = NULL;
    *p_tmr = NULL;

    iocp = __skt_ev_init(&skt_accept.io_ev, __SKT_ACCEPT);
    if (!TT_OK(__bind_iocp(skt, iocp))) {
        return NULL;
    }

    cfb = skt_accept.io_ev.src;
    if (tt_fiber_recv(cfb, TT_FALSE, p_fev, p_tmr)) {
        return NULL;
    }

    new_s = WSASocketW(skt->af,
                       SOCK_STREAM,
                       IPPROTO_TCP,
                       NULL,
                       0,
                       WSA_FLAG_OVERLAPPED);
    if (new_s == INVALID_SOCKET) {
        TT_NET_ERROR_NTV("fail to create accept socket");
        return NULL;
    }

    skt_accept.skt = skt;
    skt_accept.new_s = new_s;
    skt_accept.addr = addr;

    skt_accept.new_skt = NULL;
    skt_accept.iocp = iocp;
    skt_accept.done = TT_FALSE;
    skt_accept.canceled = TT_FALSE;

    if (tt_AcceptEx(skt->s,
                    new_s,
                    skt_accept.buf,
                    0,
                    sizeof(SOCKADDR_STORAGE) + 16,
                    sizeof(SOCKADDR_STORAGE) + 16,
                    &dwBytesReceived,
                    &skt_accept.io_ev.u.wov) ||
        (WSAGetLastError() == ERROR_IO_PENDING)) {
        cfb->recving = TT_TRUE;
        while (!skt_accept.done) {
            tt_fiber_suspend();
            cfb->recving = TT_FALSE;

            if (!skt_accept.done && !skt_accept.canceled) {
                if (CancelIoEx((HANDLE)skt->s, &skt_accept.io_ev.u.wov) ||
                    (GetLastError() == ERROR_NOT_FOUND)) {
                    skt_accept.canceled = TT_TRUE;
                } else {
                    TT_ERROR("fail to cancel AcceptEx");
                }
            }
        }

        tt_fiber_recv(cfb, TT_FALSE, p_fev, p_tmr);

        return skt_accept.new_skt;
    }

    TT_NET_ERROR_NTV("AcceptEx failed");
    closesocket(new_s);
    return NULL;
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
    if (!TT_OK(__bind_iocp(skt, iocp))) {
        return TT_FAIL;
    }

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
                      &skt_connect.io_ev.u.wov) &&
        (WSAGetLastError() != WSA_IO_PENDING)) {
        TT_NET_ERROR_NTV("ConnectEx fail");
        return TT_FAIL;
    }

    tt_fiber_suspend();
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
                                OUT tt_u32_t *recvd,
                                OUT tt_sktaddr_t *addr,
                                OUT tt_fiber_ev_t **p_fev,
                                OUT tt_tmr_t **p_tmr)
{
    __skt_recvfrom_t skt_recvfrom;
    HANDLE iocp;
    tt_fiber_t *cfb;
    WSABUF Buffers;
    DWORD Flags = 0;
    INT Fromlen = sizeof(tt_sktaddr_t);

    *recvd = 0;
    *p_fev = NULL;
    *p_tmr = NULL;

    iocp = __skt_ev_init(&skt_recvfrom.io_ev, __SKT_RECVFROM);
    if (!TT_OK(__bind_iocp(skt, iocp))) {
        return TT_FAIL;
    }

    cfb = skt_recvfrom.io_ev.src;
    if (tt_fiber_recv(cfb, TT_FALSE, p_fev, p_tmr)) {
        return TT_SUCCESS;
    }

    skt_recvfrom.skt = skt;
    skt_recvfrom.buf = buf;
    skt_recvfrom.recvd = recvd;
    skt_recvfrom.addr = TT_COND(addr != NULL, addr, &skt_recvfrom.__addr);
    skt_recvfrom.len = len;

    skt_recvfrom.result = TT_FAIL;
    skt_recvfrom.done = TT_FALSE;
    skt_recvfrom.canceled = TT_FALSE;

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
                     (struct sockaddr *)skt_recvfrom.addr,
                     (LPINT)&Fromlen,
                     &skt_recvfrom.io_ev.u.wov,
                     NULL) == 0) ||
        (WSAGetLastError() == WSA_IO_PENDING)) {
        cfb->recving = TT_TRUE;
        while (!skt_recvfrom.done) {
            tt_fiber_suspend();
            cfb->recving = TT_FALSE;

            if (!skt_recvfrom.done && !skt_recvfrom.canceled) {
                if (CancelIoEx((HANDLE)skt->s, &skt_recvfrom.io_ev.u.wov) ||
                    (GetLastError() == ERROR_NOT_FOUND)) {
                    skt_recvfrom.canceled = TT_TRUE;
                } else {
                    TT_ERROR("fail to cancel WSARecvFrom");
                }
            }
        }

        if (tt_fiber_recv(cfb, TT_FALSE, p_fev, p_tmr)) {
            skt_recvfrom.result = TT_SUCCESS;
        }

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
#if 0
    __skt_sendto_t skt_sendto;
    HANDLE iocp;
    WSABUF Buffers;

    iocp = __skt_ev_init(&skt_sendto.io_ev, __SKT_SENDTO);
    if (!TT_OK(__bind_iocp(skt, iocp))) {
        return TT_FAIL;
    }

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
                   &skt_sendto.io_ev.u.wov,
                   NULL) == 0) ||
        (WSAGetLastError() == WSA_IO_PENDING)) {
        tt_fiber_suspend();
        return skt_sendto.result;
    }

    TT_NET_ERROR_NTV("WSASendTo fail");
    return TT_FAIL;
#else
    WSABUF Buffers;
    int iToLen;
    DWORD NumberOfBytesSent;
    DWORD dwError;

    if (tt_sktaddr_get_family(addr) == TT_NET_AF_INET) {
        iToLen = sizeof(struct sockaddr_in);
    } else {
        iToLen = sizeof(struct sockaddr_in6);
    }

    Buffers.buf = (char *)buf;
    Buffers.len = len;
    if (WSASendTo(skt->s,
                  &Buffers,
                  1,
                  &NumberOfBytesSent,
                  0,
                  (struct sockaddr *)addr,
                  iToLen,
                  NULL,
                  NULL) == 0) {
        TT_SAFE_ASSIGN(sent, NumberOfBytesSent);
        return TT_SUCCESS;
    }

    dwError = WSAGetLastError();
    if (dwError == WSAEWOULDBLOCK) {
        // NumberOfBytesSent is not set when returning WSAEWOULDBLOCK
        TT_SAFE_ASSIGN(sent, 0);
        return TT_E_AGAIN;
    } else {
        TT_NET_ERROR_NTV("WSASendTo fail");
        return TT_FAIL;
    }
#endif
}

tt_result_t tt_skt_send_ntv(IN tt_skt_ntv_t *skt,
                            IN tt_u8_t *buf,
                            IN tt_u32_t len,
                            OUT OPT tt_u32_t *sent)
{
#if 0
    __skt_send_t skt_send;
    HANDLE iocp;
    WSABUF Buffers;
    DWORD dwError;

    iocp = __skt_ev_init(&skt_send.io_ev, __SKT_SEND);
    if (!TT_OK(__bind_iocp(skt, iocp))) {
        return TT_FAIL;
    }

    skt_send.skt = skt;
    skt_send.buf = buf;
    skt_send.sent = sent;
    skt_send.len = len;

    skt_send.result = TT_FAIL;
    skt_send.pos = 0;

    Buffers.buf = (char *)buf;
    Buffers.len = len;
    if ((WSASend(skt->s, &Buffers, 1, NULL, 0, &skt_send.io_ev.u.wov, NULL) ==
         0) ||
        ((dwError = WSAGetLastError()) == WSA_IO_PENDING)) {
        tt_fiber_suspend();
        return skt_send.result;
    }

    if ((dwError == WSAECONNABORTED) || (dwError == WSAECONNRESET)) {
        return TT_E_END;
    } else {
        TT_NET_ERROR_NTV("WSASend fail");
        return TT_FAIL;
    }
#else
    WSABUF Buffers;
    DWORD NumberOfBytesSent;
    DWORD dwError;

    /*
    we do not use iocp to send data, as it's possible that the remote peer
    had stopped reading, in that case caller(the current fiber) would be
    blocked at tt_skt_send() forever without any oppotunity to receive fiber
    and timer events.

    a possible solution is to implement tt_skt_send(..., &fev, &tmr) to
    enable receiving fiber and timer events during sending but the difficulty
    is that we are not able to retrieve byte count that are already sent
    once WSASend() has been called. by experiment, WSASend() can be cancelled
    by CancelIoEx() and GQCS() then returned with 0 io_bytes BUT those data
    are actually delivered to tcp stack, if remote peer start receiving those
    data that are believed "cancelled" would actually arrive at remote peer

    instead we send data in nonblocking mode, just return how many bytes
    are sent immediately

    refer MSDN:
    If the socket is non-blocking and stream-oriented, and there is
    not sufficient space in the transport's buffer, WSASend will return
    with only part of the application's buffers having been consumed.

    the purpose is prevent from some malicious behaviors, if remote peer
    is intentionally paused then local fiber hangs and then resources can
    never be released. for other write() functions like tt_fwrite(), we
    do not have such consideration as os is trusty.

    for other write() api, like tt_fwrite(), we do not make use of nonblock
    io
    */

    Buffers.buf = (char *)buf;
    Buffers.len = len;
    if (WSASend(skt->s, &Buffers, 1, &NumberOfBytesSent, 0, NULL, NULL) == 0) {
        TT_SAFE_ASSIGN(sent, NumberOfBytesSent);
        return TT_SUCCESS;
    }

    dwError = WSAGetLastError();
    if (dwError == WSAEWOULDBLOCK) {
        // NumberOfBytesSent is not set when returning WSAEWOULDBLOCK
        TT_SAFE_ASSIGN(sent, 0);
        return TT_E_AGAIN;
    } else {
        TT_NET_ERROR_NTV("WSASend fail");
        return TT_FAIL;
    }
#endif
}

tt_result_t tt_skt_send_oob_ntv(IN tt_skt_ntv_t *skt, IN tt_u8_t b)
{
#if 0
    __skt_send_t skt_send;
    HANDLE iocp;
    WSABUF Buffers;
    DWORD dwError;

    iocp = __skt_ev_init(&skt_send.io_ev, __SKT_SEND);
    if (!TT_OK(__bind_iocp(skt, iocp))) {
        return TT_FAIL;
    }

    skt_send.skt = skt;
    skt_send.buf = &b;
    skt_send.sent = NULL;
    skt_send.len = 1;

    skt_send.result = TT_FAIL;
    skt_send.pos = 0;

    Buffers.buf = (char *)&b;
    Buffers.len = 1;
    if ((WSASend(skt->s,
                 &Buffers,
                 1,
                 NULL,
                 MSG_OOB,
                 &skt_send.io_ev.u.wov,
                 NULL) == 0) ||
        ((dwError = WSAGetLastError()) == WSA_IO_PENDING)) {
        tt_fiber_suspend();
        return skt_send.result;
    }

    if ((dwError == WSAECONNABORTED) || (dwError == WSAECONNRESET)) {
        return TT_E_END;
    } else {
        TT_NET_ERROR_NTV("WSASend fail");
        return TT_FAIL;
    }
#else
    WSABUF Buffers;
    DWORD NumberOfBytesSent;
    DWORD dwError;

    Buffers.buf = (char *)&b;
    Buffers.len = 1;
    if (WSASend(skt->s, &Buffers, 1, &NumberOfBytesSent, 0, NULL, NULL) == 0) {
        return TT_SUCCESS;
    }

    dwError = WSAGetLastError();
    if (dwError == WSAEWOULDBLOCK) {
        return TT_E_AGAIN;
    } else {
        TT_NET_ERROR_NTV("WSASend fail");
        return TT_FAIL;
    }
#endif
}

tt_result_t tt_skt_sendfile_ntv(IN tt_skt_ntv_t *skt,
                                IN tt_file_t *f,
                                IN tt_u64_t offset,
                                IN tt_u32_t len,
                                OUT tt_u64_t *sent)
{
    __skt_sendfile_t skt_sendfile;
    HANDLE iocp;
    DWORD dwError;

    iocp = __skt_ev_init(&skt_sendfile.io_ev, __SKT_SENDFILE);
    if (!TT_OK(__bind_iocp(skt, iocp))) {
        return TT_FAIL;
    }

    skt_sendfile.io_ev.u.wov.OffsetHigh = (DWORD)(offset >> 32);
    skt_sendfile.io_ev.u.wov.Offset = (DWORD)offset;

    skt_sendfile.sent = sent;

    skt_sendfile.result = TT_FAIL;

    // by experiment, TransmitFile() does not block even if the local
    // socket send buffer was full, but outgoing data order may need
    // be confirmed in case: send(), sendfile(), send(), ...
    if (tt_TransmitFile(skt->s,
                        f->sys_file.hf,
                        len,
                        0,
                        &skt_sendfile.io_ev.u.wov,
                        NULL,
                        0) ||
        ((dwError = WSAGetLastError()) == WSA_IO_PENDING)) {
        tt_fiber_suspend();
        return skt_sendfile.result;
    }

    TT_NET_ERROR_NTV("TransmitFile fail");
    return TT_FAIL;
}

tt_result_t tt_skt_recv_ntv(IN tt_skt_ntv_t *skt,
                            OUT tt_u8_t *buf,
                            IN tt_u32_t len,
                            OUT tt_u32_t *recvd,
                            OUT tt_fiber_ev_t **p_fev,
                            OUT tt_tmr_t **p_tmr)
{
    __skt_recv_t skt_recv;
    HANDLE iocp;
    tt_fiber_t *cfb;
    WSABUF Buffers;
    DWORD Flags = 0, dwError;

    *recvd = 0;
    *p_fev = NULL;
    *p_tmr = NULL;

    iocp = __skt_ev_init(&skt_recv.io_ev, __SKT_RECV);
    if (!TT_OK(__bind_iocp(skt, iocp))) {
        return TT_FAIL;
    }

    cfb = skt_recv.io_ev.src;
    if (tt_fiber_recv(cfb, TT_FALSE, p_fev, p_tmr)) {
        return TT_SUCCESS;
    }

    skt_recv.skt = skt;
    skt_recv.buf = buf;
    skt_recv.len = len;
    skt_recv.recvd = recvd;

    skt_recv.result = TT_FAIL;
    skt_recv.done = TT_FALSE;
    skt_recv.canceled = TT_FALSE;

    Buffers.buf = (char *)buf;
    Buffers.len = len;
    if ((WSARecv(skt->s,
                 &Buffers,
                 1,
                 NULL,
                 &Flags,
                 &skt_recv.io_ev.u.wov,
                 NULL) == 0) ||
        ((dwError = WSAGetLastError()) == WSA_IO_PENDING)) {
        cfb->recving = TT_TRUE;
        while (!skt_recv.done) {
            tt_fiber_suspend();
            cfb->recving = TT_FALSE;

            if (!skt_recv.done && !skt_recv.canceled) {
                // if CancelIoEx() succeeds, wait for notification. or
                // GetLastError() may be ERROR_NOT_FOUND which means io
                // is completed and has been queued
                if (CancelIoEx((HANDLE)skt->s, &skt_recv.io_ev.u.wov) ||
                    (GetLastError() == ERROR_NOT_FOUND)) {
                    skt_recv.canceled = TT_TRUE;
                } else {
                    TT_ERROR("fail to cancel WSARecv");
                }
            }
        }

        if (tt_fiber_recv(cfb, TT_FALSE, p_fev, p_tmr)) {
            skt_recv.result = TT_SUCCESS;
        }

        return skt_recv.result;
    }

    if ((dwError == WSAECONNABORTED) || (dwError == WSAECONNRESET)) {
        return TT_E_END;
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
    tt_io_ev_init(io_ev, TT_IO_SOCKET, ev);
    io_ev->src = tt_current_fiber();

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

        InterfaceName = tt_wchar_create(itf, 0, NULL);
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

        InterfaceName = tt_wchar_create(itf, 0, NULL);
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

    SOCKET new_s = skt_accept->new_s;
    tt_skt_t *new_skt = NULL;
    LPSOCKADDR LocalSockaddr, RemoteSockaddr;
    INT LocalSockaddrLength, RemoteSockaddrLength;

    skt_accept->done = TT_TRUE;

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

    if (!TT_OK(__set_nonblock(new_s, TT_TRUE))) {
        TT_NET_ERROR_NTV("fail to set nonblock");
        goto fail;
    }

    new_skt = tt_malloc(sizeof(tt_skt_t));
    if (new_skt == NULL) {
        TT_ERROR("no mem for new skt");
        goto fail;
    }
    new_skt->sys_skt.s = new_s;
    new_skt->sys_skt.af = skt_accept->skt->af;
    new_skt->sys_skt.iocp = TT_FALSE;

    skt_accept->new_skt = new_skt;

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

    return TT_TRUE;

fail:

    closesocket(new_s);

    if (new_skt != NULL) {
        tt_free(new_skt);
    }

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
        TT_SAFE_ASSIGN(skt_send->sent, skt_send->pos);
        skt_send->result = TT_SUCCESS;
        return TT_TRUE;
    }
    TT_ASSERT_SKT(skt_send->pos < skt_send->len);

    // return success whenever some data is sent
    if (!TT_OK(io_ev->io_result)) {
        if (skt_send->pos > 0) {
            TT_SAFE_ASSIGN(skt_send->sent, skt_send->pos);
            skt_send->result = TT_SUCCESS;
        } else {
            skt_send->result = io_ev->io_result;
        }
        return TT_TRUE;
    }

    // send left data
    TT_INFO("continue WSASend");
    Buffers.buf = TT_PTR_INC(char, skt_send->buf, skt_send->pos);
    Buffers.len = skt_send->len - skt_send->pos;
    tt_memset(&skt_send->io_ev.u.wov, 0, sizeof(WSAOVERLAPPED));
    if ((WSASend(skt_send->skt->s,
                 &Buffers,
                 1,
                 NULL,
                 0,
                 &skt_send->io_ev.u.wov,
                 NULL) == 0) ||
        ((dwError = WSAGetLastError()) == WSA_IO_PENDING)) {
        return TT_FALSE;
    }

    // error
    if (skt_send->pos > 0) {
        TT_SAFE_ASSIGN(skt_send->sent, skt_send->pos);
        skt_send->result = TT_SUCCESS;
    } else if ((dwError == WSAECONNABORTED) || (dwError == WSAECONNRESET)) {
        skt_send->result = TT_E_END;
    } else {
        TT_NET_ERROR_NTV("WSASend fail");
        skt_send->result = TT_FAIL;
    }
    return TT_TRUE;
}

tt_bool_t __do_recv(IN tt_io_ev_t *io_ev)
{
    __skt_recv_t *skt_recv = (__skt_recv_t *)io_ev;

    TT_ASSERT_SKT(io_ev->io_bytes <= skt_recv->len);

    if (io_ev->io_bytes > 0) {
        TT_SAFE_ASSIGN(skt_recv->recvd, io_ev->io_bytes);
        skt_recv->result = TT_SUCCESS;
    } else if (TT_OK(io_ev->io_result)) {
        skt_recv->result = TT_E_END;
    } else {
        skt_recv->result = io_ev->io_result;
    }

    skt_recv->done = TT_TRUE;
    return TT_TRUE;
}

tt_bool_t __do_sendto(IN tt_io_ev_t *io_ev)
{
    __skt_sendto_t *skt_sendto = (__skt_sendto_t *)io_ev;

    WSABUF Buffers;

    skt_sendto->pos += io_ev->io_bytes;
    if (skt_sendto->pos == skt_sendto->len) {
        TT_SAFE_ASSIGN(skt_sendto->sent, skt_sendto->pos);
        skt_sendto->result = TT_SUCCESS;
        return TT_TRUE;
    }
    TT_ASSERT_SKT(skt_sendto->pos < skt_sendto->len);

    // return success whenever some data is sent
    if (!TT_OK(io_ev->io_result)) {
        if (skt_sendto->pos > 0) {
            TT_SAFE_ASSIGN(skt_sendto->sent, skt_sendto->pos);
            skt_sendto->result = TT_SUCCESS;
        } else {
            skt_sendto->result = io_ev->io_result;
        }
        return TT_TRUE;
    }

    // send left data
    Buffers.buf = TT_PTR_INC(char, skt_sendto->buf, skt_sendto->pos);
    Buffers.len = skt_sendto->len - skt_sendto->pos;
    tt_memset(&skt_sendto->io_ev.u.wov, 0, sizeof(WSAOVERLAPPED));
    if ((WSASendTo(skt_sendto->skt->s,
                   &Buffers,
                   1,
                   NULL,
                   0,
                   (struct sockaddr *)skt_sendto->addr,
                   skt_sendto->addr_len,
                   &skt_sendto->io_ev.u.wov,
                   NULL) == 0) ||
        (WSAGetLastError() == WSA_IO_PENDING)) {
        return TT_FALSE;
    }

    // error
    if (skt_sendto->pos > 0) {
        TT_SAFE_ASSIGN(skt_sendto->sent, skt_sendto->pos);
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
        TT_SAFE_ASSIGN(skt_recvfrom->recvd, io_ev->io_bytes);
        skt_recvfrom->result = TT_SUCCESS;
    } else {
        skt_recvfrom->result = io_ev->io_result;
    }

    skt_recvfrom->done = TT_TRUE;
    return TT_TRUE;
}

tt_bool_t __do_sendfile(IN tt_io_ev_t *io_ev)
{
    __skt_sendfile_t *skt_sendfile = (__skt_sendfile_t *)io_ev;

    if (TT_OK(io_ev->io_result)) {
        TT_SAFE_ASSIGN(skt_sendfile->sent, io_ev->io_bytes);
        skt_sendfile->result = TT_SUCCESS;
    } else {
        skt_sendfile->result = TT_FAIL;
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
    const GUID guid_TransmitFile = WSAID_TRANSMITFILE;

    s = WSASocketW(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, 0);
    if (s == INVALID_SOCKET) {
        TT_NET_ERROR_NTV("can not create a udp socket");
        return TT_FAIL;
    }

    if (!TT_OK(__load_api(s, &guid_ConnectEx, (void **)&tt_ConnectEx)) ||
        !TT_OK(__load_api(s, &guid_AcceptEx, (void **)&tt_AcceptEx)) ||
        !TT_OK(__load_api(s,
                          &guid_GetAcceptExSockaddrs,
                          (void **)&tt_GetAcceptExSockaddrs)) ||
        !TT_OK(__load_api(s, &guid_TransmitFile, (void **)&tt_TransmitFile))) {
        closesocket(s);
        return TT_FAIL;
    }
#ifdef __SIMU_FAIL_AcceptEx
    tt_AcceptEx = __sf_AcceptEx;
#endif
#ifdef __SIMU_FAIL_ConnectEx
    tt_ConnectEx = __sf_ConnectEx;
#endif

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

tt_result_t __bind_iocp(IN tt_skt_ntv_t *skt, IN HANDLE iocp)
{
    if (skt->iocp) {
        return TT_SUCCESS;
    }

    if (iocp == NULL) {
        iocp = tt_current_fiber()->fs->thread->task->iop.sys_iop.iocp;
    }

    if (CreateIoCompletionPort((HANDLE)skt->s, iocp, (ULONG_PTR)skt, 0) !=
        NULL) {
        skt->iocp = TT_TRUE;
        return TT_SUCCESS;
    } else {
        TT_NET_ERROR_NTV("fail to bind socket to iocp");
        return TT_FAIL;
    }
}

#ifdef __SIMU_FAIL_WSASocketW
#undef WSASocketW
SOCKET __sf_WSASocketW(int af,
                       int type,
                       int protocol,
                       LPWSAPROTOCOL_INFOW lpProtocolInfo,
                       GROUP g,
                       DWORD dwFlags)
{
    return INVALID_SOCKET;
}
#endif

#ifdef __SIMU_FAIL_closesocket
#undef closesocket
int __sf_closesocket(int fildes)
{
    return SOCKET_ERROR;
}
#endif

#ifdef __SIMU_FAIL_shutdown
#undef shutdown
int __sf_shutdown(SOCKET s, int how)
{
    return SOCKET_ERROR;
}
#endif

#ifdef __SIMU_FAIL_bind
#undef bind
int __sf_bind(SOCKET s, const struct sockaddr *name, int namelen)

{
    return SOCKET_ERROR;
}
#endif

#ifdef __SIMU_FAIL_listen
#undef listen
int __sf_listen(SOCKET s, int backlog)
{
    return SOCKET_ERROR;
}
#endif

#ifdef __SIMU_FAIL_AcceptEx
#undef AcceptEx
BOOL __sf_AcceptEx(SOCKET sListenSocket,
                   SOCKET sAcceptSocket,
                   PVOID lpOutputBuffer,
                   DWORD dwReceiveDataLength,
                   DWORD dwLocalAddressLength,
                   DWORD dwRemoteAddressLength,
                   LPDWORD lpdwBytesReceived,
                   LPOVERLAPPED lpOverlapped)
{
    return FALSE;
}
#endif

#ifdef __SIMU_FAIL_ConnectEx
#undef ConnectEx
BOOL PASCAL __sf_ConnectEx(SOCKET s,
                           const struct sockaddr *name,
                           int namelen,
                           __in_opt PVOID lpSendBuffer,
                           DWORD dwSendDataLength,
                           LPDWORD lpdwBytesSent,
                           LPOVERLAPPED lpOverlapped)
{
    return FALSE;
}
#endif

#ifdef __SIMU_FAIL_WSASend
#undef WSASend
int __sf_WSASend(SOCKET s,
                 LPWSABUF lpBuffers,
                 DWORD dwBufferCount,
                 LPDWORD lpNumberOfBytesSent,
                 DWORD dwFlags,
                 LPWSAOVERLAPPED lpOverlapped,
                 LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    if (tt_rand_u32() % 10 == 0) {
        WSASetLastError(WSA_NOT_ENOUGH_MEMORY);
        return SOCKET_ERROR;
    } else {
        // TT_INFO("org send: %d", lpBuffers[0].len);
        lpBuffers[0].len = tt_rand_u32() % lpBuffers[0].len + 1;
        // TT_INFO("but send: %d", lpBuffers[0].len);
        return WSASend(s,
                       lpBuffers,
                       dwBufferCount,
                       lpNumberOfBytesSent,
                       dwFlags,
                       lpOverlapped,
                       lpCompletionRoutine);
    }
}
#endif

#ifdef __SIMU_FAIL_WSARecv
#undef WSARecv
int __sf_WSARecv(SOCKET s,
                 LPWSABUF lpBuffers,
                 DWORD dwBufferCount,
                 LPDWORD lpNumberOfBytesRecvd,
                 LPDWORD lpFlags,
                 LPWSAOVERLAPPED lpOverlapped,
                 LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    if (tt_rand_u32() % 10 == 0) {
        WSASetLastError(WSA_NOT_ENOUGH_MEMORY);
        return SOCKET_ERROR;
    } else {
        // TT_INFO("org recv: %d", lpBuffers[0].len);
        lpBuffers[0].len = tt_rand_u32() % lpBuffers[0].len + 1;
        // TT_INFO("org recv: %d", lpBuffers[0].len);
        return WSARecv(s,
                       lpBuffers,
                       dwBufferCount,
                       lpNumberOfBytesRecvd,
                       lpFlags,
                       lpOverlapped,
                       lpCompletionRoutine);
    }
}
#endif

#ifdef __SIMU_FAIL_WSASendTo
#undef WSASendTo
int __sf_WSASendTo(SOCKET s,
                   LPWSABUF lpBuffers,
                   DWORD dwBufferCount,
                   LPDWORD lpNumberOfBytesSent,
                   DWORD dwFlags,
                   const struct sockaddr *lpTo,
                   int iToLen,
                   LPWSAOVERLAPPED lpOverlapped,
                   LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    if (tt_rand_u32() % 10 == 0) {
        WSASetLastError(WSA_NOT_ENOUGH_MEMORY);
        return SOCKET_ERROR;
    } else {
        lpBuffers[0].len = tt_rand_u32() % lpBuffers[0].len + 1;
        return WSASendTo(s,
                         lpBuffers,
                         dwBufferCount,
                         lpNumberOfBytesSent,
                         dwFlags,
                         lpTo,
                         iToLen,
                         lpOverlapped,
                         lpCompletionRoutine);
    }
}
#endif

#ifdef __SIMU_FAIL_WSARecvFrom
#undef WSARecvFrom
int __sf_WSARecvFrom(SOCKET s,
                     LPWSABUF lpBuffers,
                     DWORD dwBufferCount,
                     LPDWORD lpNumberOfBytesRecvd,
                     LPDWORD lpFlags,
                     struct sockaddr *lpFrom,
                     LPINT lpFromlen,
                     LPWSAOVERLAPPED lpOverlapped,
                     LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    if (tt_rand_u32() % 10 == 0) {
        WSASetLastError(WSA_NOT_ENOUGH_MEMORY);
        return SOCKET_ERROR;
    } else {
        lpBuffers[0].len = tt_rand_u32() % lpBuffers[0].len + 1;
        return WSARecvFrom(s,
                           lpBuffers,
                           dwBufferCount,
                           lpNumberOfBytesRecvd,
                           lpFlags,
                           lpFrom,
                           lpFromlen,
                           lpOverlapped,
                           lpCompletionRoutine);
    }
}
#endif
