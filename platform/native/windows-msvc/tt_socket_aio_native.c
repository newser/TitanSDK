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

#include <tt_socket_aio_native.h>

#include <event/tt_event_center.h>
#include <event/tt_event_poller.h>
#include <init/tt_profile.h>
#include <io/tt_socket_aio.h>
#include <memory/tt_memory_alloc.h>
#include <misc/tt_assert.h>
#include <network/ssl/tt_ssl_aio.h>

#include <tt_sys_error.h>

#include <mswsock.h>
#include <ws2tcpip.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

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

#define __SKTAIO_CHECK_EVC

#define TT_ASSERT_SKTAIO TT_ASSERT

#define __INLINE_SBUF_NUM 4
#define __SBUF_SIZE(n) ((tt_u32_t)(sizeof(__sbuf_t) * (n)))

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

enum
{
    TT_SKT_RANGE_AIO,
};

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
    EV_SKT_DESTROY_WRITE,
    EV_SKT_DESTROY_READ,
    EV_SKT_Q_READ,
    EV_SKT_Q_WRITE,

    EV_SKTAIO_END,
};

typedef union
{
    WSABUF w;
    tt_blob_t t;
} __sbuf_t;

typedef struct
{
    WSABUF *wbuf;
    tt_u32_t wbuf_num;
    tt_u32_t cur_idx;
    tt_u32_t cur_pos;
    tt_u32_t io_bytes;
} __sbuf_state_t;

typedef struct
{
    // must be put at head
    WSAOVERLAPPED wov;

    tt_skt_t *skt;
    tt_sktaddr_t remote_addr;
    tt_skt_on_connect_t on_connect;
    tt_bool_t submitted;

    tt_result_t result;
    void *cb_param;
} __skt_connect_t;

typedef struct
{
    // must be put at head
    WSAOVERLAPPED wov;

    tt_skt_t *listening_skt;
    tt_skt_t *new_skt;
    tt_skt_on_accept_t on_accept;
    tt_u8_t addr[(sizeof(SOCKADDR_STORAGE) + 16) * 2];
    tt_bool_t submitted;

    tt_result_t result;
    void *cb_param;
} __skt_accept_t;

typedef struct
{
    // must be put at head
    WSAOVERLAPPED wov;

    tt_skt_t *skt;
    __sbuf_state_t sbuf_state;
    __sbuf_t sbuf[__INLINE_SBUF_NUM];
    tt_skt_on_send_t on_send;

    tt_result_t result;
    void *cb_param;
} __skt_send_t;

typedef struct
{
    // must be put at head
    WSAOVERLAPPED wov;

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
    // must be put at head
    WSAOVERLAPPED wov;

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
    // must be put at head
    WSAOVERLAPPED wov;

    tt_skt_t *skt;
    __sbuf_state_t sbuf_state;
    __sbuf_t sbuf[__INLINE_SBUF_NUM];
    tt_skt_on_recvfrom_t on_recvfrom;
    tt_sktaddr_t remote_addr;
    INT addr_len;

    tt_result_t result;
    void *cb_param;
} __skt_recvfrom_t;

typedef struct __skt_shutdown_s
{
    // must be put at head
    WSAOVERLAPPED wov;

    tt_skt_t *skt;
} __skt_shutdown_t;

typedef struct
{
    // must be put at head
    WSAOVERLAPPED wov;

    tt_skt_t *skt;
} __skt_destroy_t;

typedef struct
{
    tt_skt_t *skt;
} __skt_q_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

extern tt_result_t __handle_skt_attr(IN tt_skt_t *skt);

extern tt_result_t __ssl_destroy_aio(IN tt_ev_t *ev);
extern tt_result_t __ssl_handshake(IN tt_ssl_t *ssl,
                                   IN tt_ev_t *tev,
                                   IN DWORD NumberOfBytes,
                                   IN tt_result_t iocp_result);
extern tt_result_t __ssl_encrypt(IN tt_ssl_t *ssl,
                                 IN tt_u8_t *data,
                                 IN OUT tt_u32_t *data_len);
extern tt_result_t __ssl_decrypt(IN tt_ssl_t *ssl,
                                 IN tt_u32_t last_io_bytes,
                                 IN tt_ssl_on_decrypt_t decrypt_cb,
                                 IN void *cb_param);

extern tt_result_t __ssl_close_notify(IN struct tt_ssl_s *ssl);

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

// ConnectEx()
static BOOL(PASCAL FAR *tt_ConnectEx)(SOCKET s,
                                      const struct sockaddr *name,
                                      int namelen,
                                      PVOID lpSendBuffer,
                                      DWORD dwSendDataLength,
                                      LPDWORD lpdwBytesSent,
                                      LPOVERLAPPED lpOverlapped);
// AcceptEx()
static BOOL(PASCAL FAR *tt_AcceptEx)(SOCKET sListenSocket,
                                     SOCKET sAcceptSocket,
                                     PVOID lpOutputBuffer,
                                     DWORD dwReceiveDataLength,
                                     DWORD dwLocalAddressLength,
                                     DWORD dwRemoteAddressLength,
                                     LPDWORD lpdwBytesReceived,
                                     LPOVERLAPPED lpOverlapped);
// GetAcceptExSockaddrs()
static void(PASCAL FAR *tt_GetAcceptExSockaddrs)(PVOID lpOutputBuffer,
                                                 DWORD dwReceiveDataLength,
                                                 DWORD dwLocalAddressLength,
                                                 DWORD dwRemoteAddressLength,
                                                 LPSOCKADDR *LocalSockaddr,
                                                 LPINT LocalSockaddrLength,
                                                 LPSOCKADDR *RemoteSockaddr,
                                                 LPINT RemoteSockaddrLength);

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

static tt_result_t __load_wsa();
static tt_result_t __load_wsa_guid(IN SOCKET s,
                                   const IN GUID *guid,
                                   IN void **pfn);

// connect
static tt_bool_t __do_skt_connect(IN __skt_connect_t *aio,
                                  IN tt_result_t iocp_result);
static tt_result_t __do_skt_connect_io(IN __skt_connect_t *aio);
static void __do_skt_connect_cb(IN __skt_connect_t *aio);

// accept
static tt_bool_t __do_skt_accept(IN __skt_accept_t *aio,
                                 IN tt_result_t iocp_result);
static tt_result_t __do_skt_accept_io(IN __skt_accept_t *aio);
static void __do_skt_accept_cb(IN __skt_accept_t *aio);

// send
static tt_bool_t __do_skt_send(IN __skt_send_t *aio,
                               IN tt_result_t iocp_result,
                               IN tt_u32_t send_num);
static tt_result_t __do_skt_send_io(IN __skt_send_t *aio,
                                    IN tt_u32_t last_io_bytes);
static tt_result_t __do_skt_send_io_ssl(IN __skt_send_t *aio,
                                        IN tt_u32_t last_io_bytes);
static void __do_skt_send_cb(IN __skt_send_t *aio);

// recv
static tt_bool_t __do_skt_recv(IN __skt_recv_t *aio,
                               IN tt_result_t iocp_result,
                               IN tt_u32_t recv_num);
static tt_result_t __do_skt_recv_io(IN __skt_recv_t *aio,
                                    IN tt_u32_t last_io_bytes);
static tt_result_t __do_skt_recv_io_ssl(IN __skt_recv_t *aio,
                                        IN tt_u32_t last_io_bytes);
static void __do_skt_recv_cb(IN __skt_recv_t *aio);

// sendto
static tt_bool_t __do_skt_sendto(IN __skt_sendto_t *aio,
                                 IN tt_result_t iocp_result,
                                 IN tt_u32_t send_num);
static tt_result_t __do_skt_sendto_io(IN __skt_sendto_t *aio,
                                      IN tt_u32_t send_num);
static void __do_skt_sendto_cb(IN __skt_sendto_t *aio);

// recvfrom
static tt_bool_t __do_skt_recvfrom(IN __skt_recvfrom_t *aio,
                                   IN tt_result_t iocp_result,
                                   IN tt_u32_t recv_num);
static tt_result_t __do_skt_recvfrom_io(IN __skt_recvfrom_t *aio,
                                        IN tt_u32_t recv_num);
static void __do_skt_recvfrom_cb(IN __skt_recvfrom_t *aio);

// shutdown
static tt_result_t __skt_shutdown_wr(IN tt_skt_t *skt);
static tt_bool_t __do_skt_shutdown_wr(IN __skt_shutdown_t *aio);

static tt_result_t __skt_shutdown_rd(IN tt_skt_t *skt);
static tt_bool_t __do_skt_shutdown_rd(IN __skt_shutdown_t *aio);

// destroy
static tt_result_t __skt_destroy_wr(IN tt_skt_t *skt);
static tt_bool_t __do_skt_destroy_wr(IN __skt_destroy_t *aio);

static tt_result_t __skt_destroy_rd(IN tt_skt_t *skt);
static tt_bool_t __do_skt_destroy_rd(IN __skt_destroy_t *aio);

static void __destroy_skt(IN tt_skt_t *skt,
                          IN tt_bool_t do_cb,
                          IN tt_bool_t check_free);

static tt_result_t __create_aio_ev(IN tt_skt_ntv_t *sys_skt);
static void __destroy_aio_ev(IN tt_skt_ntv_t *sys_skt);

// aio q
static void __do_skt_aio_q(IN tt_skt_ntv_t *sys_skt,
                           IN tt_list_t *aio_q,
                           IN tt_result_t iocp_result,
                           IN DWORD NumberOfBytes);
static void __clear_skt_aio_q(IN tt_skt_ntv_t *sys_skt,
                              IN tt_list_t *aio_q,
                              IN tt_result_t aio_result);

static tt_bool_t __update_sbuf_state(IN __sbuf_state_t *sbuf_state,
                                     IN tt_u32_t io_bytes);
static tt_bool_t __update_sbuf_state_data(IN __sbuf_state_t *sbuf_state,
                                          IN tt_u8_t *data,
                                          IN tt_u32_t data_len,
                                          OUT tt_u32_t *updated_len);

static void __wsabuf2blob_inline(IN WSABUF *blob_array, IN tt_u32_t blob_num);

static tt_result_t __skt_aio_do_ssl_send_io(IN __skt_send_t *aio,
                                            IN tt_u32_t last_io_bytes);
static tt_result_t __skt_aio_do_ssl_recv_io(IN __skt_recv_t *aio,
                                            IN tt_u32_t last_io_bytes);

// ========================================
// simulate failure
// ========================================

#ifdef __SIMULATE_SKT_AIO_FAIL

#define tt_evc_sendto_thread tt_evc_sendto_thread_SF
static tt_result_t tt_evc_sendto_thread_SF(IN tt_evcenter_t *evc,
                                           IN tt_thread_ev_t *tev);

#define WSASend WSASend_SF
static int WSASend_SF(SOCKET s,
                      LPWSABUF lpBuffers,
                      DWORD dwBufferCount,
                      LPDWORD lpNumberOfBytesSent,
                      DWORD dwFlags,
                      LPWSAOVERLAPPED lpOverlapped,
                      LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

#define WSARecv WSARecv_SF
static int WSARecv_SF(SOCKET s,
                      LPWSABUF lpBuffers,
                      DWORD dwBufferCount,
                      LPDWORD lpNumberOfBytesRecvd,
                      LPDWORD lpFlags,
                      LPWSAOVERLAPPED lpOverlapped,
                      LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

#define WSASendTo WSASendTo_SF
static int WSASendTo_SF(SOCKET s,
                        LPWSABUF lpBuffers,
                        DWORD dwBufferCount,
                        LPDWORD lpNumberOfBytesSent,
                        DWORD dwFlags,
                        const struct sockaddr *lpTo,
                        int iToLen,
                        LPWSAOVERLAPPED lpOverlapped,
                        LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

#define WSARecvFrom WSARecvFrom_SF
static int WSARecvFrom_SF(
    SOCKET s,
    LPWSABUF lpBuffers,
    DWORD dwBufferCount,
    LPDWORD lpNumberOfBytesRecvd,
    LPDWORD lpFlags,
    struct sockaddr *lpFrom,
    LPINT lpFromlen,
    LPWSAOVERLAPPED lpOverlapped,
    LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

#endif

#if 0 //#ifdef __SIMULATE_SSL_AIO_FAIL

#define WSASend WSASend_SSL
static
int WSASend_SSL(SOCKET s,
                LPWSABUF lpBuffers,
                DWORD dwBufferCount,
                LPDWORD lpNumberOfBytesSent,
                DWORD dwFlags,
                LPWSAOVERLAPPED lpOverlapped,
                LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

#define WSARecv WSARecv_SSL
static
int WSARecv_SSL(SOCKET s,
                LPWSABUF lpBuffers,
                DWORD dwBufferCount,
                LPDWORD lpNumberOfBytesRecvd,
                LPDWORD lpFlags,
                LPWSAOVERLAPPED lpOverlapped,
                LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

#endif

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_skt_aio_component_init_ntv(IN tt_profile_t *profile)
{
    if (sizeof(WSABUF) < sizeof(tt_blob_t)) {
        TT_ERROR("sizeof(WSABUF): %d, less than sizeof(tt_blob_t): %d",
                 sizeof(WSABUF),
                 sizeof(tt_blob_t));
        return TT_FAIL;
    }

    if (!TT_OK(__load_wsa())) {
        TT_ERROR("fail to load WSA APIs");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_async_skt_create_ntv(OUT tt_skt_t *skt,
                                    IN tt_net_family_t family,
                                    IN tt_net_protocol_t protocol,
                                    IN tt_u32_t role,
                                    IN OPT tt_skt_attr_t *attr,
                                    IN tt_skt_exit_t *exit)
{
    tt_skt_attr_t __attr;
    tt_evcenter_t *evc;
    tt_skt_ntv_t *sys_skt;

    SOCKET __skt = INVALID_SOCKET;
    int af;
    int type;
    int __protocol;

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

    sys_skt = &skt->sys_socket;

    // family
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

    // create socket
    __skt = socket(af, type, __protocol);
    if (__skt == INVALID_SOCKET) {
        TT_NET_ERROR_NTV("fail to create socket");
        return TT_FAIL;
    }

    // ignore icmp port unreachable
    if (protocol == TT_NET_PROTO_UDP) {
        BOOL bNewBehavior = FALSE;
        DWORD dwBytesReturned = 0;
        if (WSAIoctl(__skt,
                     SIO_UDP_CONNRESET,
                     &bNewBehavior,
                     sizeof(bNewBehavior),
                     NULL,
                     0,
                     &dwBytesReturned,
                     NULL,
                     NULL) == SOCKET_ERROR) {
            TT_NET_ERROR_NTV("fail to disable SIO_UDP_CONNRESET");
            // ignore such error?
        }
    }

    // FILE_SKIP_COMPLETION_PORT_ON_SUCCESS lead to some bug

    // associating socket with iocp
    if (CreateIoCompletionPort((HANDLE)__skt,
                               evc->sys_evc.iocp,
                               (ULONG_PTR)(&sys_skt->ev_mark),
                               0) == NULL) {
        TT_ERROR_NTV("fail to associate socket with iocp");

        closesocket(__skt);
        return TT_FAIL;
    }

    sys_skt->s = __skt;
    sys_skt->evc = evc;
    sys_skt->ev_mark = TT_EV_RANGE_INTERNAL_SOCKET;
    sys_skt->role = role;

    if (!TT_OK(__create_aio_ev(sys_skt))) {
        closesocket(__skt);
        return TT_FAIL;
    }

    sys_skt->on_destroy = exit->on_destroy;
    sys_skt->on_destroy_param = exit->cb_param;

    tt_list_init(&sys_skt->read_q);
    tt_list_init(&sys_skt->write_q);

    sys_skt->ssl = NULL;

    sys_skt->rd_closing = TT_FALSE;
    sys_skt->rd_closed = TT_FALSE;
    sys_skt->ssl_handshaking = TT_FALSE;
    sys_skt->wr_closing = TT_FALSE;
    sys_skt->wr_closed = TT_FALSE;
    sys_skt->ssl_connected = TT_FALSE;
    sys_skt->connecting = TT_FALSE;
    sys_skt->connected = TT_FALSE;
    sys_skt->ssl_closing = TT_FALSE;
    sys_skt->rd_submitted = TT_FALSE;
    sys_skt->wr_closed = TT_FALSE;

    // ========================================
    // create tt socket
    // ========================================

    // save data
    skt->family = family;
    skt->protocol = protocol;
    tt_memcpy(&skt->attr, attr, sizeof(tt_skt_attr_t));

    // process attributes
    if (!TT_OK(__handle_skt_attr(skt))) {
        __destroy_aio_ev(sys_skt);
        closesocket(sys_skt->s);
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_async_skt_destroy_ntv(IN tt_skt_t *skt, IN tt_bool_t immediate)
{
    tt_skt_ntv_t *sys_skt;

    TT_ASSERT(skt != NULL);

    sys_skt = &skt->sys_socket;
    if (immediate) {
        tt_skt_on_destroy_t on_destroy =
            (tt_skt_on_destroy_t)sys_skt->on_destroy;
        tt_bool_t from_alloc = skt->attr.from_alloc;

        __clear_skt_aio_q(sys_skt, &sys_skt->read_q, TT_CANCELLED);
        __clear_skt_aio_q(sys_skt, &sys_skt->write_q, TT_CANCELLED);

        __destroy_skt(skt, TT_TRUE, TT_TRUE);
    } else {
        tt_result_t des_rd, des_wr;

#ifdef __SKTAIO_CHECK_EVC
        if (sys_skt->evc != tt_evc_current()) {
            TT_ERROR("different socket evc");
            return;
        }
#endif

        des_rd = __skt_destroy_rd(skt);
        des_wr = __skt_destroy_wr(skt);
        if (TT_OK(des_rd) || TT_OK(des_wr)) {
            closesocket(sys_skt->s);
            sys_skt->s = INVALID_SOCKET;
        }
    }
}

tt_result_t tt_async_skt_shutdown_ntv(IN tt_skt_t *skt, IN tt_u32_t mode)
{
    TT_ASSERT(skt != NULL);
    TT_ASSERT(TT_NET_PROTO_VALID(skt->protocol));

#ifdef __SKTAIO_CHECK_EVC
    if (skt->sys_socket.evc != tt_evc_current()) {
        TT_ERROR("different socket evc");
        return TT_BAD_PARAM;
    }
#endif

    if (!__SKT_ROLE_OF_TCP_DATA(skt->sys_socket.role)) {
        TT_ERROR("not tcp data socket");
        return TT_FAIL;
    }

    if (mode == TT_SKT_SHUTDOWN_WR) {
        return __skt_shutdown_wr(skt);
    } else if (mode == TT_SKT_SHUTDOWN_RD) {
        return __skt_shutdown_rd(skt);
    } else {
        tt_result_t sd_wr, sd_rd;

        TT_ASSERT(mode == TT_SKT_SHUTDOWN_RDWR);

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

    sys_skt = &listening_skt->sys_socket;
    new_sys_skt = &new_skt->sys_socket;

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

    // create new socket
    if (!TT_OK(tt_async_skt_create_ntv(new_skt,
                                       listening_skt->family,
                                       TT_NET_PROTO_TCP,
                                       TT_SKT_ROLE_TCP_ACCEPT,
                                       new_skt_attr,
                                       new_skt_exit))) {
        TT_ERROR("fail to create new socket");
        return TT_FAIL;
    }

    // aio
    ev = tt_thread_ev_create(EV_SKT_ACCEPT, sizeof(__skt_accept_t), NULL);
    if (ev == NULL) {
        TT_ERROR("fail to allocate aio");

        closesocket(new_skt->sys_socket.s);
        return TT_FAIL;
    }
    tev = TT_EV_HDR(ev, tt_thread_ev_t);
    aio = TT_EV_DATA(ev, __skt_accept_t);

    // init aio
    tt_memset(&aio->wov, 0, sizeof(WSAOVERLAPPED));
    aio->listening_skt = listening_skt;
    aio->new_skt = new_skt;
    aio->on_accept = on_accept;
    tt_memset(aio->addr, 0, sizeof(aio->addr));
    aio->submitted = TT_FALSE;

    aio->result = TT_PROCEEDING;
    aio->cb_param = cb_param;

    // add request
    tt_list_addtail(&sys_skt->read_q, &tev->node);

    // start iocp if it's head aio
    if (tt_list_count(&sys_skt->read_q) == 1) {
        tt_result_t result;

        // if this is the first aio, send event rather than submitting an ov, so
        // that
        // caller is still able to destroy skt immediately
        if (sys_skt->rd_submitted) {
            result = TT_COND(__do_skt_accept_io(aio) == TT_PROCEEDING,
                             TT_SUCCESS,
                             TT_FAIL);
        } else {
            tt_ev_t *aio_ev = sys_skt->aio_rd;

            TT_ASSERT_SKTAIO((aio_ev != NULL) &&
                             (aio_ev->ev_id == EV_SKT_Q_READ));
            TT_ASSERT_SKTAIO(TT_EV_HDR(aio_ev, tt_thread_ev_t)->node.lst ==
                             NULL);
            result = tt_evc_sendto_thread(sys_skt->evc, aio_ev);
        }

        if (!TT_OK(result)) {
            tt_list_remove(&tev->node);
            tt_ev_destroy(ev);
            closesocket(new_skt->sys_socket.s);
            return TT_FAIL;
        }
    }

    sys_skt->rd_submitted = TT_TRUE;
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

    sys_skt = &skt->sys_socket;

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

    // aio
    ev = tt_thread_ev_create(EV_SKT_CONNECT, sizeof(__skt_connect_t), NULL);
    if (ev == NULL) {
        TT_ERROR("fail to allocate aio");
        return TT_FAIL;
    }
    tev = TT_EV_HDR(ev, tt_thread_ev_t);
    aio = TT_EV_DATA(ev, __skt_connect_t);

    // init aio
    tt_memset(&aio->wov, 0, sizeof(WSAOVERLAPPED));
    aio->skt = skt;
    tt_memcpy(&aio->remote_addr, remote_addr, sizeof(tt_sktaddr_t));
    aio->on_connect = on_connect;
    aio->submitted = TT_FALSE;

    aio->result = TT_PROCEEDING;
    aio->cb_param = cb_param;

    // add request
    tt_list_addtail(&sys_skt->write_q, &tev->node);

    // start iocp if it's head aio
    if (tt_list_count(&sys_skt->write_q) == 1) {
        tt_result_t result;

        // if this is the first aio, send event rather than submitting an ov, so
        // that
        // caller is still able to destroy skt immediately
        if (sys_skt->wr_submitted) {
            result = TT_COND(__do_skt_connect_io(aio) == TT_PROCEEDING,
                             TT_SUCCESS,
                             TT_FAIL);
        } else {
            tt_ev_t *aio_ev = sys_skt->aio_wr;

            TT_ASSERT_SKTAIO((aio_ev != NULL) &&
                             (aio_ev->ev_id == EV_SKT_Q_WRITE));
            TT_ASSERT_SKTAIO(TT_EV_HDR(aio_ev, tt_thread_ev_t)->node.lst ==
                             NULL);
            result = tt_evc_sendto_thread(sys_skt->evc, aio_ev);
        }

        if (!TT_OK(result)) {
            tt_list_remove(&tev->node);
            tt_ev_destroy(ev);
            return TT_FAIL;
        }
    }

    sys_skt->connecting = TT_TRUE;
    sys_skt->wr_submitted = TT_TRUE;
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

    sys_skt = &skt->sys_socket;

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
    tt_memset(&aio->wov, 0, sizeof(WSAOVERLAPPED));
    aio->skt = skt;

    sbuf_state->wbuf_num = blob_num;
    if (blob_num <= __INLINE_SBUF_NUM) {
        sbuf_state->wbuf = (WSABUF *)aio->sbuf;
    } else {
        sbuf_state->wbuf = (WSABUF *)tt_mem_alloc(__SBUF_SIZE(blob_num));
        if (sbuf_state->wbuf == NULL) {
            TT_ERROR("no mem for buf array");

            // do not call tt_ev_destroy, as aio has not finished
            // initialization
            tt_mem_free(ev);
            return TT_FAIL;
        }
    }
    for (i = 0; i < blob_num; ++i) {
        sbuf_state->wbuf[i].buf = (char *)blob[i].addr;
        sbuf_state->wbuf[i].len = (u_long)blob[i].len;
    }
    sbuf_state->cur_idx = 0;
    sbuf_state->cur_pos = 0;
    sbuf_state->io_bytes = 0;

    aio->on_send = on_send;

    aio->result = TT_PROCEEDING;
    aio->cb_param = cb_param;

    // add request
    tt_list_addtail(&sys_skt->write_q, &tev->node);

    // start iocp if it's head aio
    if ((tt_list_count(&sys_skt->write_q) == 1)) {
        tt_result_t result;
#ifdef TT_PLATFORM_SSL_ENABLE
        if (sys_skt->ssl != NULL) {
            result = __do_skt_send_io_ssl(aio, 0);
        } else {
            result = __do_skt_send_io(aio, 0);
        }
#else
        result = __do_skt_send_io(aio, 0);
#endif
        if (result != TT_PROCEEDING) {
            tt_list_remove(&tev->node);
            tt_ev_destroy(ev);
            return TT_FAIL;
        }
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

    sys_skt = &skt->sys_socket;

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
    tt_memset(&aio->wov, 0, sizeof(WSAOVERLAPPED));
    aio->skt = skt;

    sbuf_state->wbuf_num = blob_num;
    if (blob_num <= __INLINE_SBUF_NUM) {
        sbuf_state->wbuf = (WSABUF *)aio->sbuf;
    } else {
        sbuf_state->wbuf = (WSABUF *)tt_mem_alloc(__SBUF_SIZE(blob_num));
        if (sbuf_state->wbuf == NULL) {
            TT_ERROR("no mem for buf array");

            // do not call tt_ev_destroy, as aio has not finished
            // initialization
            tt_mem_free(ev);
            return TT_FAIL;
        }
    }
    for (i = 0; i < blob_num; ++i) {
        sbuf_state->wbuf[i].buf = (char *)blob[i].addr;
        sbuf_state->wbuf[i].len = (u_long)blob[i].len;
    }
    sbuf_state->cur_idx = 0;
    sbuf_state->cur_pos = 0;
    sbuf_state->io_bytes = 0;

    aio->on_recv = on_recv;

    aio->result = TT_PROCEEDING;
    aio->cb_param = cb_param;

    __SKTAIO_DEBUG_FLAG_SET(aio, 0);

    // add request
    tt_list_addtail(&sys_skt->read_q, &tev->node);

    // start iocp if it's head aio
    if (tt_list_count(&sys_skt->read_q) == 1) {
        tt_result_t result;
#ifdef TT_PLATFORM_SSL_ENABLE
        if (sys_skt->ssl != NULL) {
            if (sys_skt->ssl->sys_ssl.input_plain_len > 0) {
                // if there are decrypted data, then we use PQCS
                if (PostQueuedCompletionStatus(sys_skt->evc->sys_evc.iocp,
                                               0,
                                               (ULONG_PTR)&sys_skt->ev_mark,
                                               &aio->wov)) {
                    result = TT_PROCEEDING;
                } else {
                    TT_ERROR_NTV("fail to post recv");
                    result = TT_FAIL;
                }
            } else {
                result = __do_skt_recv_io_ssl(aio, 0);
            }
        } else {
            result = __do_skt_recv_io(aio, 0);
        }
#else
        result = __do_skt_recv_io(aio, 0);
#endif
        if (result != TT_PROCEEDING) {
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

    sys_skt = &skt->sys_socket;

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
    tt_memset(&aio->wov, 0, sizeof(WSAOVERLAPPED));
    aio->skt = skt;

    sbuf_state->wbuf_num = blob_num;
    if (blob_num <= __INLINE_SBUF_NUM) {
        sbuf_state->wbuf = (WSABUF *)aio->sbuf;
    } else {
        sbuf_state->wbuf = (WSABUF *)tt_mem_alloc(__SBUF_SIZE(blob_num));
        if (sbuf_state->wbuf == NULL) {
            TT_ERROR("no mem for buf array");

            // do not call tt_ev_destroy, as aio has not finished
            // initialization
            tt_mem_free(ev);
            return TT_FAIL;
        }
    }
    aio->total_len = 0;
    for (i = 0; i < blob_num; ++i) {
        sbuf_state->wbuf[i].buf = (char *)blob[i].addr;
        sbuf_state->wbuf[i].len = (u_long)blob[i].len;
        aio->total_len += (u_long)blob[i].len;
    }
    sbuf_state->cur_idx = 0;
    sbuf_state->cur_pos = 0;
    sbuf_state->io_bytes = 0;

    tt_memcpy(&aio->remote_addr, remote_addr, sizeof(tt_sktaddr_t));
    aio->on_sendto = on_sendto;

    aio->result = TT_PROCEEDING;
    aio->cb_param = cb_param;

    // add request
    tt_list_addtail(&sys_skt->write_q, &tev->node);

    // start iocp if it's head aio
    if (tt_list_count(&sys_skt->write_q) == 1) {
        tt_result_t result;

        // if this is the first aio, send event rather than submitting an ov, so
        // that
        // caller is still able to destroy skt immediately
        if (sys_skt->wr_submitted) {
            result = TT_COND(__do_skt_sendto_io(aio, 0) == TT_PROCEEDING,
                             TT_SUCCESS,
                             TT_FAIL);
        } else {
            tt_ev_t *aio_ev = sys_skt->aio_wr;

            TT_ASSERT_SKTAIO((aio_ev != NULL) &&
                             (aio_ev->ev_id == EV_SKT_Q_WRITE));
            TT_ASSERT_SKTAIO(TT_EV_HDR(aio_ev, tt_thread_ev_t)->node.lst ==
                             NULL);
            result = tt_evc_sendto_thread(sys_skt->evc, aio_ev);
        }

        if (!TT_OK(result)) {
            tt_list_remove(&tev->node);
            tt_ev_destroy(ev);
            return TT_FAIL;
        }
    }

    sys_skt->wr_submitted = TT_TRUE;
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

    sys_skt = &skt->sys_socket;

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
    tt_memset(&aio->wov, 0, sizeof(WSAOVERLAPPED));
    aio->skt = skt;

    sbuf_state->wbuf_num = blob_num;
    if (blob_num <= __INLINE_SBUF_NUM) {
        sbuf_state->wbuf = (WSABUF *)aio->sbuf;
    } else {
        sbuf_state->wbuf = (WSABUF *)tt_mem_alloc(__SBUF_SIZE(blob_num));
        if (sbuf_state->wbuf == NULL) {
            TT_ERROR("no mem for buf array");

            // do not call tt_ev_destroy, as aio has not finished
            // initialization
            tt_mem_free(ev);
            return TT_FAIL;
        }
    }
    for (i = 0; i < blob_num; ++i) {
        sbuf_state->wbuf[i].buf = (char *)blob[i].addr;
        sbuf_state->wbuf[i].len = (u_long)blob[i].len;
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
    tt_list_addtail(&sys_skt->read_q, &tev->node);

    // start iocp if it's head aio
    if (tt_list_count(&sys_skt->read_q) == 1) {
        tt_result_t result;

        // if this is the first aio, send event rather than submitting an ov, so
        // that
        // caller is still able to destroy skt immediately
        if (sys_skt->rd_submitted) {
            result = TT_COND(__do_skt_recvfrom_io(aio, 0) == TT_PROCEEDING,
                             TT_SUCCESS,
                             TT_FAIL);
        } else {
            tt_ev_t *aio_ev = sys_skt->aio_rd;

            TT_ASSERT_SKTAIO((aio_ev != NULL) &&
                             (aio_ev->ev_id == EV_SKT_Q_READ));
            TT_ASSERT_SKTAIO(TT_EV_HDR(aio_ev, tt_thread_ev_t)->node.lst ==
                             NULL);
            result = tt_evc_sendto_thread(sys_skt->evc, aio_ev);
        }

        if (!TT_OK(result)) {
            tt_list_remove(&tev->node);
            tt_ev_destroy(ev);
            return TT_FAIL;
        }
    }

    sys_skt->rd_submitted = TT_TRUE;
    return TT_SUCCESS;
}

tt_result_t tt_skt_iocp_handler(IN tt_skt_t *skt,
                                IN DWORD NumberOfBytes,
                                IN WSAOVERLAPPED *Overlapped,
                                IN tt_result_t iocp_result)
{
    tt_skt_ntv_t *sys_skt = &skt->sys_socket;
    tt_thread_ev_t *tev = TT_TEV_OF(Overlapped);
    tt_lnode_t *node = &tev->node;
    tt_ev_t *ev = TT_EV_OF(tev);

    TT_ASSERT(iocp_result != TT_PROCEEDING);
    TT_ASSERT((node->lst == &sys_skt->read_q) ||
              (node->lst == &sys_skt->write_q));
    TT_ASSERT(node == tt_list_head(node->lst));

    switch (ev->ev_id) {
        case EV_SKT_RECV: {
            if ((NumberOfBytes == 0) && TT_OK(iocp_result)) {
                if ((sys_skt->ssl == NULL) ||
                    (sys_skt->ssl->sys_ssl.input_plain_len == 0)) {
                    iocp_result = TT_END;
                }
            }
        } break;
        default: {
        } break;
    }

    __do_skt_aio_q(sys_skt, node->lst, iocp_result, NumberOfBytes);
    return TT_SUCCESS;
}

tt_result_t tt_skt_tev_handler(IN struct tt_evpoller_s *evp,
                               IN struct tt_ev_s *ev)
{
    TT_ASSERT(ev != NULL);
    TT_ASSERT(TT_EV_RANGE(ev->ev_id) == TT_EV_RANGE_INTERNAL_SOCKET);

    switch (ev->ev_id) {
        case EV_SKT_Q_READ: {
            __skt_q_t *aio = TT_EV_DATA(ev, __skt_q_t);
            tt_skt_ntv_t *sys_skt = &aio->skt->sys_socket;

            __do_skt_aio_q(sys_skt, &sys_skt->read_q, TT_SUCCESS, 0);
            // no need to destroy ev
        } break;
        case EV_SKT_Q_WRITE: {
            __skt_q_t *aio = TT_EV_DATA(ev, __skt_q_t);
            tt_skt_ntv_t *sys_skt = &aio->skt->sys_socket;

            __do_skt_aio_q(sys_skt, &sys_skt->write_q, TT_SUCCESS, 0);
            // no need to destroy ev
        } break;

        default: {
            TT_FATAL("unknown event: %x", ev->ev_id);
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
    skt->sys_socket.s = INVALID_SOCKET;

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

    return TT_SUCCESS;

tssa_fail:

    // nothing except skt->sys_socket.s need be released
    if (skt->sys_socket.s != INVALID_SOCKET) {
        closesocket(skt->sys_socket.s);
        skt->sys_socket.s = INVALID_SOCKET;
    }

    return TT_FAIL;
}

tt_result_t tt_udp_server_async_ntv(OUT tt_skt_t *skt,
                                    IN tt_net_family_t family,
                                    IN OPT tt_skt_attr_t *attr,
                                    IN tt_sktaddr_t *local_addr,
                                    IN tt_skt_exit_t *exit)
{
    skt->sys_socket.s = INVALID_SOCKET;

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

    return TT_SUCCESS;

ussa_fail:

    // nothing except skt->sys_socket.s need be released
    if (skt->sys_socket.s != INVALID_SOCKET) {
        closesocket(skt->sys_socket.s);
        skt->sys_socket.s = INVALID_SOCKET;
    }

    return TT_FAIL;
}

tt_bool_t __do_skt_connect(IN __skt_connect_t *aio, IN tt_result_t iocp_result)
{
    tt_skt_t *skt = aio->skt;
    tt_skt_ntv_t *sys_skt = &skt->sys_socket;

    if (TT_OK(iocp_result)) {
        if (aio->submitted) {
            aio->result = TT_SUCCESS;
        } else {
            aio->result = __do_skt_connect_io(aio);
            if (aio->result == TT_PROCEEDING) {
                return TT_FALSE;
            }
        }
    } else {
        aio->result = iocp_result;
    }
    TT_ASSERT_SKTAIO(aio->result != TT_PROCEEDING);

    if (TT_OK(aio->result)) {
        // The socket s does not enable previously set properties or
        // options until SO_UPDATE_CONNECT_CONTEXT is set on the socket
        if (setsockopt(sys_skt->s,
                       SOL_SOCKET,
                       SO_UPDATE_CONNECT_CONTEXT,
                       NULL,
                       0) != 0) {
            TT_NET_ERROR_NTV("fail to set SO_UPDATE_CONNECT_CONTEXT");
        }
    }

    __do_skt_connect_cb(aio);
    return TT_TRUE;
}

tt_result_t __do_skt_connect_io(IN __skt_connect_t *aio)
{
    tt_skt_t *skt = aio->skt;
    tt_skt_ntv_t *sys_skt = &skt->sys_socket;

    if (sys_skt->s == INVALID_SOCKET) {
        return TT_FAIL;
    }

    // ConnectEx need a bind. on windows, port 0 means allocate a port
    if (!TT_OK(tt_skt_bind_n(skt, skt->family, TT_SKTADDR_ANY, 0))) {
        return TT_FAIL;
    }

    if (!tt_ConnectEx(sys_skt->s,
                      (const struct sockaddr *)&aio->remote_addr,
                      TT_COND(skt->family == TT_NET_AF_INET,
                              sizeof(struct sockaddr_in),
                              sizeof(struct sockaddr_in6)),
                      NULL,
                      0,
                      NULL,
                      &aio->wov) &&
        (WSAGetLastError() != WSA_IO_PENDING)) {
        TT_NET_ERROR_NTV("async connect fail");
        return TT_FAIL;
    }
    aio->submitted = TT_TRUE;

    return TT_PROCEEDING;
}

void __do_skt_connect_cb(IN __skt_connect_t *aio)
{
    tt_skt_t *skt = aio->skt;
    tt_skt_aioctx_t aioctx;

    TT_ASSERT(aio->result != TT_PROCEEDING);

    // connecting done, success or fail
    skt->sys_socket.connecting = TT_FALSE;

    skt->sys_socket.connected = TT_BOOL(TT_OK(aio->result));

    // callback
    aioctx.result = aio->result;
    aioctx.cb_param = aio->cb_param;
    aio->on_connect(aio->skt, &aio->remote_addr, &aioctx);
}

tt_bool_t __do_skt_accept(IN __skt_accept_t *aio, IN tt_result_t iocp_result)
{
    tt_skt_t *listening_skt = aio->listening_skt;
    tt_skt_t *new_skt = aio->new_skt;
    tt_skt_ntv_t *new_sys_skt = &new_skt->sys_socket;

    if (TT_OK(iocp_result)) {
        if (aio->submitted) {
            aio->result = TT_SUCCESS;
        } else {
            aio->result = __do_skt_accept_io(aio);
            if (aio->result == TT_PROCEEDING) {
                return TT_FALSE;
            }
        }
    } else {
        aio->result = iocp_result;
    }
    TT_ASSERT_SKTAIO(aio->result != TT_PROCEEDING);

    if (TT_OK(aio->result)) {
        if (setsockopt(new_skt->sys_socket.s,
                       SOL_SOCKET,
                       SO_UPDATE_ACCEPT_CONTEXT,
                       (char *)&listening_skt->sys_socket.s,
                       sizeof(SOCKET)) != 0) {
            TT_NET_ERROR_NTV("fail to set SO_UPDATE_ACCEPT_CONTEXT");
        }
    } else {
        TT_ASSERT_SKTAIO(new_skt->sys_socket.s != INVALID_SOCKET);
        closesocket(new_skt->sys_socket.s);
        new_skt->sys_socket.s = INVALID_SOCKET;
    }

    __do_skt_accept_cb(aio);
    return TT_TRUE;
}

tt_result_t __do_skt_accept_io(IN __skt_accept_t *aio)
{
    SOCKET s = aio->listening_skt->sys_socket.s;
    DWORD dwBytesReceived;

    if (s == INVALID_SOCKET) {
        return TT_FAIL;
    }

    if (!tt_AcceptEx(s,
                     aio->new_skt->sys_socket.s,
                     aio->addr,
                     0,
                     sizeof(SOCKADDR_STORAGE) + 16,
                     sizeof(SOCKADDR_STORAGE) + 16,
                     &dwBytesReceived,
                     &aio->wov) &&
        (WSAGetLastError() != WSA_IO_PENDING)) {
        TT_NET_ERROR_NTV("async accept fail");
        return TT_FAIL;
    }
    aio->submitted = TT_TRUE;

    return TT_PROCEEDING;
}

void __do_skt_accept_cb(IN __skt_accept_t *aio)
{
    tt_skt_aioctx_t aioctx;
    tt_skt_t *new_skt = aio->new_skt;
    tt_bool_t from_alloc = new_skt->attr.from_alloc;

    new_skt->sys_socket.connected = TT_BOOL(TT_OK(aio->result));

    // callback
    aioctx.result = aio->result;
    aioctx.cb_param = aio->cb_param;
    aio->on_accept(aio->listening_skt, aio->new_skt, &aioctx);

    if (!TT_OK(aioctx.result) && from_alloc) {
        tt_mem_free(new_skt);
    }
}

tt_bool_t __do_skt_send(IN __skt_send_t *aio,
                        IN tt_result_t iocp_result,
                        IN tt_u32_t send_num)
{
    if (TT_OK(iocp_result)) {
#ifdef TT_PLATFORM_SSL_ENABLE
        if (aio->skt->sys_socket.ssl != NULL) {
            aio->result = __do_skt_send_io_ssl(aio, send_num);
        } else {
            aio->result = __do_skt_send_io(aio, send_num);
        }
#else
        aio->result = __do_skt_send_io(aio, send_num);
#endif
    } else {
        aio->result = iocp_result;
    }

    if (aio->result != TT_PROCEEDING) {
        __do_skt_send_cb(aio);
        return TT_TRUE;
    } else {
        return TT_FALSE;
    }
}

tt_result_t __do_skt_send_io(IN __skt_send_t *aio, IN tt_u32_t last_io_bytes)
{
    tt_skt_ntv_t *sys_skt = &aio->skt->sys_socket;
    __sbuf_state_t *sbuf_state = &aio->sbuf_state;

    WSABUF *cur_buf;
    WSABUF *Buffers;
    DWORD dwBufferCount;
    WSABUF __Buffer;
    DWORD NumberOfBytesSent = 0;
    DWORD dwError;

    if (__update_sbuf_state(sbuf_state, last_io_bytes)) {
        return TT_SUCCESS;
    }

    if (sys_skt->s == INVALID_SOCKET) {
        return TT_END;
    }

    cur_buf = &sbuf_state->wbuf[sbuf_state->cur_idx];
    TT_ASSERT_SKTAIO(sbuf_state->cur_idx < sbuf_state->wbuf_num);
    TT_ASSERT_SKTAIO(sbuf_state->cur_pos < cur_buf->len);

    if ((sbuf_state->cur_idx == 0) && (sbuf_state->cur_pos == 0)) {
        Buffers = sbuf_state->wbuf;
        dwBufferCount = sbuf_state->wbuf_num;
    } else {
        __Buffer.len = cur_buf->len - sbuf_state->cur_pos;
        __Buffer.buf = TT_PTR_INC(char, cur_buf->buf, sbuf_state->cur_pos);

        Buffers = &__Buffer;
        dwBufferCount = 1;
    }

    if ((WSASend(sys_skt->s,
                 Buffers,
                 dwBufferCount,
                 &NumberOfBytesSent,
                 0,
                 &aio->wov,
                 NULL) != 0) &&
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

#ifdef TT_PLATFORM_SSL_ENABLE
tt_result_t __do_skt_send_io_ssl(IN __skt_send_t *aio,
                                 IN tt_u32_t last_io_bytes)
{
    tt_skt_ntv_t *sys_skt = &aio->skt->sys_socket;
    __sbuf_state_t *sbuf_state = &aio->sbuf_state;
    tt_ssl_t *ssl = sys_skt->ssl;
    tt_ssl_ntv_t *sys_ssl = &ssl->sys_ssl;

    WSABUF Buffer;
    DWORD NumberOfBytesSent = 0;
    DWORD dwError;

    if (sys_skt->s == INVALID_SOCKET) {
        return TT_END;
    }

    // TT_DETAIL("sent[%d]", last_io_bytes);
    sys_ssl->output_pos += last_io_bytes;
    TT_ASSERT_SKTAIO(sys_ssl->output_pos <= sys_ssl->output_data_len);
    if (sys_ssl->output_pos < sys_ssl->output_data_len) {
        // partial encrypted data sent
        Buffer.buf =
            TT_PTR_INC(char, sys_ssl->output.addr, sys_ssl->output_pos);
        Buffer.len = sys_ssl->output_data_len - sys_ssl->output_pos;
    } else {
        // choose next plain data to encrypt and sent
        WSABUF *cur_buf;
        tt_u8_t *data;
        tt_u32_t data_len;

        // update recv buf
        if (last_io_bytes > 0) {
            last_io_bytes = sys_ssl->output_data_len;
            TT_ASSERT_SKTAIO(last_io_bytes >= sys_ssl->extra_len);
            last_io_bytes -= sys_ssl->extra_len;

            if (__update_sbuf_state(sbuf_state, last_io_bytes)) {
                return TT_SUCCESS;
            }
        }
        cur_buf = &sbuf_state->wbuf[sbuf_state->cur_idx];
        TT_ASSERT_SKTAIO(sbuf_state->cur_idx < sbuf_state->wbuf_num);
        TT_ASSERT_SKTAIO(sbuf_state->cur_pos < cur_buf->len);

        // encrypt plain data
        data = TT_PTR_INC(char, cur_buf->buf, sbuf_state->cur_pos);
        data_len = cur_buf->len - sbuf_state->cur_pos;
        if (!TT_OK(__ssl_encrypt(ssl, data, &data_len))) {
            // can not do more things if ssl encrypt failed
            return TT_END;
        }

        Buffer.buf = (char *)sys_ssl->output.addr;
        Buffer.len = sys_ssl->output_data_len;
    }

    // TT_DETAIL("encrypted[%d]", Buffer.len);
    if ((WSASend(sys_skt->s,
                 &Buffer,
                 1,
                 &NumberOfBytesSent,
                 0,
                 &aio->wov,
                 NULL) != 0) &&
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

#endif

void __do_skt_send_cb(IN __skt_send_t *aio)
{
    tt_skt_aioctx_t aioctx;
    __sbuf_state_t *sbuf_state = &aio->sbuf_state;

    if (sbuf_state->io_bytes > 0) {
        aio->result = TT_SUCCESS;
    }

    __wsabuf2blob_inline(sbuf_state->wbuf, sbuf_state->wbuf_num);

    // callback
    aioctx.result = aio->result;
    aioctx.cb_param = aio->cb_param;
    aio->on_send(aio->skt,
                 (tt_blob_t *)sbuf_state->wbuf,
                 sbuf_state->wbuf_num,
                 &aioctx,
                 sbuf_state->io_bytes);
}

void __skt_send_on_destroy(IN struct tt_ev_s *ev)
{
    __skt_send_t *aio = TT_EV_DATA(ev, __skt_send_t);
    __sbuf_state_t *sbuf_state = &aio->sbuf_state;

    if (sbuf_state->wbuf != (WSABUF *)aio->sbuf) {
        tt_mem_free(sbuf_state->wbuf);
    }
}

tt_bool_t __do_skt_recv(IN __skt_recv_t *aio,
                        IN tt_result_t iocp_result,
                        IN tt_u32_t recv_num)
{
    if (TT_OK(iocp_result)) {
#ifdef TT_PLATFORM_SSL_ENABLE
        if (aio->skt->sys_socket.ssl != NULL) {
            aio->result = __do_skt_recv_io_ssl(aio, recv_num);
        } else {
            aio->result = __do_skt_recv_io(aio, recv_num);
        }
#else
        aio->result = __do_skt_recv_io(aio, recv_num);
#endif
    } else {
        aio->result = iocp_result;
    }

    if (aio->result != TT_PROCEEDING) {
        __do_skt_recv_cb(aio);
        return TT_TRUE;
    } else {
        return TT_FALSE;
    }
}

tt_result_t __do_skt_recv_io(IN __skt_recv_t *aio, IN tt_u32_t last_io_bytes)
{
    tt_skt_ntv_t *sys_skt = &aio->skt->sys_socket;
    __sbuf_state_t *sbuf_state = &aio->sbuf_state;

    WSABUF *cur_buf;
    WSABUF *Buffers;
    DWORD dwBufferCount;
    WSABUF __Buffer;
    DWORD NumberOfBytesRecvd = 0;
    DWORD Flags = 0;
    DWORD dwError;

    if (__update_sbuf_state(sbuf_state, last_io_bytes)) {
        return TT_SUCCESS;
    }
    if (sbuf_state->io_bytes > 0) {
        return TT_SUCCESS;
    }

    if (sys_skt->s == INVALID_SOCKET) {
        return TT_END;
    }

    cur_buf = &sbuf_state->wbuf[sbuf_state->cur_idx];
    TT_ASSERT_SKTAIO(sbuf_state->cur_idx < sbuf_state->wbuf_num);
    TT_ASSERT_SKTAIO(sbuf_state->cur_pos < cur_buf->len);

    if ((sbuf_state->cur_idx == 0) && (sbuf_state->cur_pos == 0)) {
        Buffers = sbuf_state->wbuf;
        dwBufferCount = sbuf_state->wbuf_num;
    } else {
        __Buffer.len = cur_buf->len - sbuf_state->cur_pos;
        __Buffer.buf = TT_PTR_INC(char, cur_buf->buf, sbuf_state->cur_pos);

        Buffers = &__Buffer;
        dwBufferCount = 1;
    }

    if ((WSARecv(sys_skt->s,
                 Buffers,
                 dwBufferCount,
                 &NumberOfBytesRecvd,
                 &Flags,
                 &aio->wov,
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

#ifdef TT_PLATFORM_SSL_ENABLE
static tt_u32_t __ssl_on_decrypt(IN tt_u8_t *pos,
                                 IN tt_u32_t len,
                                 IN void *cb_param)
{
    __skt_recv_t *aio = (__skt_recv_t *)cb_param;
    __sbuf_state_t *sbuf_state = &aio->sbuf_state;
    tt_u32_t updated_len = 0;

    __update_sbuf_state_data(sbuf_state, pos, len, &updated_len);

    return updated_len;
}

tt_result_t __do_skt_recv_io_ssl(IN __skt_recv_t *aio,
                                 IN tt_u32_t last_io_bytes)
{
    tt_skt_ntv_t *sys_skt = &aio->skt->sys_socket;
    tt_ssl_t *ssl = sys_skt->ssl;
    tt_ssl_ntv_t *sys_ssl = &ssl->sys_ssl;
    tt_result_t result;

    WSABUF Buffer;
    DWORD NumberOfBytesRecvd = 0;
    DWORD Flags = 0;
    DWORD dwError;

    result = __ssl_decrypt(ssl, last_io_bytes, __ssl_on_decrypt, aio);
    if (TT_OK(result)) {
        return TT_SUCCESS;
    } else if (result != TT_PROCEEDING) {
        return result;
    }
    // else continue receiving

    Buffer.buf = TT_PTR_INC(char, sys_ssl->input.addr, sys_ssl->input_pos);
    TT_ASSERT_SKTAIO(sys_ssl->input_pos < sys_ssl->input.len);
    Buffer.len = sys_ssl->input.len - sys_ssl->input_pos;

    if ((WSARecv(sys_skt->s,
                 &Buffer,
                 1,
                 &NumberOfBytesRecvd,
                 &Flags,
                 &aio->wov,
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

#endif

void __do_skt_recv_cb(IN __skt_recv_t *aio)
{
    tt_skt_aioctx_t aioctx;
    __sbuf_state_t *sbuf_state = &aio->sbuf_state;

    if (sbuf_state->io_bytes > 0) {
        aio->result = TT_SUCCESS;
    }

    __wsabuf2blob_inline(sbuf_state->wbuf, sbuf_state->wbuf_num);

    // callback
    aioctx.result = aio->result;
    aioctx.cb_param = aio->cb_param;
    aio->on_recv(aio->skt,
                 (tt_blob_t *)sbuf_state->wbuf,
                 sbuf_state->wbuf_num,
                 &aioctx,
                 sbuf_state->io_bytes);
}

void __skt_recv_on_destroy(IN struct tt_ev_s *ev)
{
    __skt_recv_t *aio = TT_EV_DATA(ev, __skt_recv_t);
    __sbuf_state_t *sbuf_state = &aio->sbuf_state;

    if (sbuf_state->wbuf != (WSABUF *)aio->sbuf) {
        tt_mem_free(sbuf_state->wbuf);
    }
}

tt_bool_t __do_skt_sendto(IN __skt_sendto_t *aio,
                          IN tt_result_t iocp_result,
                          IN tt_u32_t send_num)
{
    if (TT_OK(iocp_result)) {
        aio->result = __do_skt_sendto_io(aio, send_num);
    } else {
        aio->result = iocp_result;
    }

    if (aio->result != TT_PROCEEDING) {
        __do_skt_sendto_cb(aio);
        return TT_TRUE;
    } else {
        return TT_FALSE;
    }
}

tt_result_t __do_skt_sendto_io(IN __skt_sendto_t *aio, IN tt_u32_t send_num)
{
    tt_skt_ntv_t *sys_skt = &aio->skt->sys_socket;
    __sbuf_state_t *sbuf_state = &aio->sbuf_state;

    WSABUF *cur_buf;
    int iToLen = TT_COND(aio->skt->family == TT_NET_AF_INET,
                         sizeof(struct sockaddr_in),
                         sizeof(struct sockaddr_in6));
    WSABUF *Buffers;
    DWORD dwBufferCount;
    WSABUF __Buffer;
    DWORD NumberOfBytesSent = 0;
    DWORD dwError;

    if (__update_sbuf_state(sbuf_state, send_num)) {
        return TT_SUCCESS;
    }

    if (sys_skt->s == INVALID_SOCKET) {
        return TT_END;
    }

    cur_buf = &sbuf_state->wbuf[sbuf_state->cur_idx];
    TT_ASSERT_SKTAIO(sbuf_state->cur_idx < sbuf_state->wbuf_num);
    TT_ASSERT_SKTAIO(sbuf_state->cur_pos < cur_buf->len);

    if ((sbuf_state->cur_idx == 0) && (sbuf_state->cur_pos == 0)) {
        Buffers = sbuf_state->wbuf;
        dwBufferCount = sbuf_state->wbuf_num;
    } else {
        __Buffer.len = cur_buf->len - sbuf_state->cur_pos;
        __Buffer.buf = TT_PTR_INC(char, cur_buf->buf, sbuf_state->cur_pos);

        Buffers = &__Buffer;
        dwBufferCount = 1;
    }

    if ((WSASendTo(sys_skt->s,
                   Buffers,
                   dwBufferCount,
                   &NumberOfBytesSent,
                   0,
                   (const struct sockaddr *)&aio->remote_addr,
                   iToLen,
                   &aio->wov,
                   NULL) != 0) &&
        ((dwError = WSAGetLastError()) != WSA_IO_PENDING)) {
        TT_NET_ERROR_NTV("sendto fail");
        return TT_FAIL;
    }

    return TT_PROCEEDING;
}

void __do_skt_sendto_cb(IN __skt_sendto_t *aio)
{
    tt_skt_aioctx_t aioctx;
    __sbuf_state_t *sbuf_state = &aio->sbuf_state;

    if (sbuf_state->io_bytes > 0) {
        aio->result = TT_SUCCESS;
    }

    __wsabuf2blob_inline(sbuf_state->wbuf, sbuf_state->wbuf_num);

    // callback
    aioctx.result = aio->result;
    aioctx.cb_param = aio->cb_param;
    aio->on_sendto(aio->skt,
                   (tt_blob_t *)sbuf_state->wbuf,
                   sbuf_state->wbuf_num,
                   &aio->remote_addr,
                   &aioctx,
                   sbuf_state->io_bytes);
}

void __skt_sendto_on_destroy(IN struct tt_ev_s *ev)
{
    __skt_sendto_t *aio = TT_EV_DATA(ev, __skt_sendto_t);
    __sbuf_state_t *sbuf_state = &aio->sbuf_state;

    if (sbuf_state->wbuf != (WSABUF *)aio->sbuf) {
        tt_mem_free(sbuf_state->wbuf);
    }
}

tt_bool_t __do_skt_recvfrom(IN __skt_recvfrom_t *aio,
                            IN tt_result_t iocp_result,
                            IN tt_u32_t recv_num)
{
    if (TT_OK(iocp_result)) {
        aio->result = __do_skt_recvfrom_io(aio, recv_num);
    } else {
        aio->result = iocp_result;
    }

    if (aio->result != TT_PROCEEDING) {
        __do_skt_recvfrom_cb(aio);
        return TT_TRUE;
    } else {
        return TT_FALSE;
    }
}

tt_result_t __do_skt_recvfrom_io(IN __skt_recvfrom_t *aio, IN tt_u32_t recv_num)
{
    tt_skt_ntv_t *sys_skt = &aio->skt->sys_socket;
    __sbuf_state_t *sbuf_state = &aio->sbuf_state;

    WSABUF *cur_buf;
    WSABUF *Buffers;
    DWORD dwBufferCount;
    WSABUF __Buffer;
    DWORD Flags = 0;
    DWORD dwError;

    DWORD NumberOfBytesRecvd = 0;

    if (__update_sbuf_state(sbuf_state, recv_num)) {
        return TT_SUCCESS;
    }
    if (sbuf_state->io_bytes > 0) {
        return TT_SUCCESS;
    }

    if (sys_skt->s == INVALID_SOCKET) {
        return TT_END;
    }

    cur_buf = &sbuf_state->wbuf[sbuf_state->cur_idx];
    TT_ASSERT_SKTAIO(sbuf_state->cur_idx < sbuf_state->wbuf_num);
    TT_ASSERT_SKTAIO(sbuf_state->cur_pos < cur_buf->len);

    if ((sbuf_state->cur_idx == 0) && (sbuf_state->cur_pos == 0)) {
        Buffers = sbuf_state->wbuf;
        dwBufferCount = sbuf_state->wbuf_num;
    } else {
        __Buffer.len = cur_buf->len - sbuf_state->cur_pos;
        __Buffer.buf = TT_PTR_INC(char, cur_buf->buf, sbuf_state->cur_pos);

        Buffers = &__Buffer;
        dwBufferCount = 1;
    }

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

    if ((WSARecvFrom(sys_skt->s,
                     Buffers,
                     dwBufferCount,
                     &NumberOfBytesRecvd,
                     &Flags,
                     (struct sockaddr *)&aio->remote_addr,
                     (LPINT)&aio->addr_len,
                     &aio->wov,
                     NULL) != 0) &&
        ((dwError = WSAGetLastError()) != WSA_IO_PENDING)) {
        TT_NET_ERROR_NTV("recvfrom fail");
        return TT_FAIL;
    }

    return TT_PROCEEDING;
}

void __do_skt_recvfrom_cb(IN __skt_recvfrom_t *aio)
{
    tt_skt_aioctx_t aioctx;
    __sbuf_state_t *sbuf_state = &aio->sbuf_state;

    if (sbuf_state->io_bytes > 0) {
        aio->result = TT_SUCCESS;
    }

    __wsabuf2blob_inline(sbuf_state->wbuf, sbuf_state->wbuf_num);

    // callback
    aioctx.result = aio->result;
    aioctx.cb_param = aio->cb_param;
    aio->on_recvfrom(aio->skt,
                     (tt_blob_t *)sbuf_state->wbuf,
                     sbuf_state->wbuf_num,
                     &aioctx,
                     sbuf_state->io_bytes,
                     &aio->remote_addr);
}

void __skt_recvfrom_on_destroy(IN struct tt_ev_s *ev)
{
    __skt_recvfrom_t *aio = TT_EV_DATA(ev, __skt_recvfrom_t);
    __sbuf_state_t *sbuf_state = &aio->sbuf_state;

    if (sbuf_state->wbuf != (WSABUF *)aio->sbuf) {
        tt_mem_free(sbuf_state->wbuf);
    }
}

tt_result_t __skt_shutdown_wr(IN tt_skt_t *skt)
{
    tt_skt_ntv_t *sys_skt = &skt->sys_socket;
    tt_ev_t *ev;
    tt_thread_ev_t *tev;
    __skt_shutdown_t *aio;

    if (sys_skt->wr_closing) {
        // TT_ERROR("already closing write");
        return TT_FAIL;
    }

    // aio
    ev = tt_thread_ev_create(EV_SKT_SHUTDOWN_WRITE,
                             sizeof(__skt_shutdown_t),
                             NULL);
    if (ev == NULL) {
        TT_ERROR("fail to allocate shutdown aio");
        return TT_FAIL;
    }
    tev = TT_EV_HDR(ev, tt_thread_ev_t);
    aio = TT_EV_DATA(ev, __skt_shutdown_t);

    // init aio
    tt_memset(&aio->wov, 0, sizeof(WSAOVERLAPPED));
    aio->skt = skt;

    // add request
    tt_list_addtail(&sys_skt->write_q, &tev->node);

    // start iocp if it's head aio
    if ((tt_list_count(&sys_skt->write_q) == 1) &&
        !PostQueuedCompletionStatus(sys_skt->evc->sys_evc.iocp,
                                    0,
                                    (ULONG_PTR)&sys_skt->ev_mark,
                                    &aio->wov)) {
        TT_ERROR_NTV("fail to post shutdown wr");

        tt_list_remove(&tev->node);
        tt_ev_destroy(ev);
        return TT_FAIL;
    }

    sys_skt->wr_closing = TT_TRUE;
    return TT_SUCCESS;
}

tt_bool_t __do_skt_shutdown_wr(IN __skt_shutdown_t *aio)
{
    tt_skt_ntv_t *sys_skt = &aio->skt->sys_socket;

    // all write aio must have been done
    TT_ASSERT_SKTAIO(tt_list_count(&sys_skt->write_q) == 0);

#ifdef TT_PLATFORM_SSL_ENABLE
    // to send ssl close notify if:
    //  - this is an ssl socket
    //  - ssl close notify is not sent
    //  - ssl close notify is generated successfully
    // otherwise directly do tcp shutdown
    if ((sys_skt->ssl != NULL) && !sys_skt->ssl_closing &&
        TT_OK(__ssl_close_notify(sys_skt->ssl))) {
        tt_ssl_ntv_t *sys_ssl = &sys_skt->ssl->sys_ssl;
        WSABUF Buffer;
        DWORD NumberOfBytesSent = 0;

        // set the ssl closing without caring whether ssl
        // close notify is sent, we only try once and even
        // how much bytes of the ssl close notify packet
        // are sent is also ignored.
        sys_skt->ssl_closing = TT_TRUE;

        Buffer.buf =
            TT_PTR_INC(char, sys_ssl->output.addr, sys_ssl->output_pos);
        Buffer.len = sys_ssl->output_data_len - sys_ssl->output_pos;
        if ((WSASend(sys_skt->s,
                     &Buffer,
                     1,
                     &NumberOfBytesSent,
                     0,
                     &aio->wov,
                     NULL) == 0) ||
            (WSAGetLastError() == WSA_IO_PENDING)) {
            return TT_FALSE;
        } else {
            TT_NET_ERROR_NTV("send fail");
            // continue
        }
    }
#endif

    if (sys_skt->s != INVALID_SOCKET) {
        if (shutdown(sys_skt->s, SD_SEND) != 0) {
            TT_NET_ERROR_NTV("fail to shutdown write");
        }
        // failing to shutdown SHUT_WR is possible when remote
        // side forced a connection closing. it make no sense to
        // tell caller shuting down failed, who can do nothing
    }

    // mark as closed
    TT_ASSERT_SKTAIO(!sys_skt->wr_closed);
    sys_skt->wr_closed = TT_TRUE;

    // check if read q is already closed
    if (sys_skt->rd_closed) {
        __destroy_skt(aio->skt, TT_TRUE, TT_TRUE);
    }

    return TT_TRUE;
}

tt_result_t __skt_shutdown_rd(IN tt_skt_t *skt)
{
    tt_skt_ntv_t *sys_skt = &skt->sys_socket;
    tt_ev_t *ev;
    tt_thread_ev_t *tev;
    __skt_shutdown_t *aio;

    if (sys_skt->rd_closing) {
        // TT_ERROR("already closing read");
        return TT_FAIL;
    }

    // aio
    ev = tt_thread_ev_create(EV_SKT_SHUTDOWN_READ,
                             sizeof(__skt_shutdown_t),
                             NULL);
    if (ev == NULL) {
        TT_ERROR("fail to allocate shutdown aio");
        return TT_FAIL;
    }
    tev = TT_EV_HDR(ev, tt_thread_ev_t);
    aio = TT_EV_DATA(ev, __skt_shutdown_t);

    // init aio
    tt_memset(&aio->wov, 0, sizeof(WSAOVERLAPPED));
    aio->skt = skt;

    // add request
    tt_list_addtail(&sys_skt->read_q, &tev->node);

    // start iocp if it's head aio
    if ((tt_list_count(&sys_skt->read_q) == 1) &&
        !PostQueuedCompletionStatus(sys_skt->evc->sys_evc.iocp,
                                    0,
                                    (ULONG_PTR)&sys_skt->ev_mark,
                                    &aio->wov)) {
        TT_ERROR_NTV("fail to post shutdown rd");

        tt_list_remove(&tev->node);
        tt_ev_destroy(ev);
        return TT_FAIL;
    }

    sys_skt->rd_closing = TT_TRUE;
    return TT_SUCCESS;
}

tt_bool_t __do_skt_shutdown_rd(IN __skt_shutdown_t *aio)
{
    tt_skt_ntv_t *sys_skt = &aio->skt->sys_socket;

    // all read aio must have been done
    TT_ASSERT_SKTAIO(tt_list_count(&sys_skt->read_q) == 0);

    // mark as closed
    TT_ASSERT_SKTAIO(!sys_skt->rd_closed);
    sys_skt->rd_closed = TT_TRUE;

    // check if write q is already closed
    if (sys_skt->wr_closed) {
        __destroy_skt(aio->skt, TT_TRUE, TT_TRUE);
    }

    return TT_TRUE;
}

tt_result_t __skt_destroy_wr(IN tt_skt_t *skt)
{
    tt_skt_ntv_t *sys_skt = &skt->sys_socket;
    tt_ev_t *ev;
    tt_thread_ev_t *tev;
    __skt_destroy_t *aio;

    if (sys_skt->wr_closing) {
        // TT_ERROR("already closing write");
        return TT_FAIL;
    }

    // aio
    ev = tt_thread_ev_create(EV_SKT_DESTROY_WRITE,
                             sizeof(__skt_destroy_t),
                             NULL);
    if (ev == NULL) {
        TT_ERROR("fail to allocate shutdown aio");
        return TT_FAIL;
    }
    tev = TT_EV_HDR(ev, tt_thread_ev_t);
    aio = TT_EV_DATA(ev, __skt_destroy_t);

    // init aio
    tt_memset(&aio->wov, 0, sizeof(WSAOVERLAPPED));
    aio->skt = skt;

    // add request
    tt_list_addtail(&sys_skt->write_q, &tev->node);

    // start iocp if it's head aio
    if ((tt_list_count(&sys_skt->write_q) == 1) &&
        !PostQueuedCompletionStatus(sys_skt->evc->sys_evc.iocp,
                                    0,
                                    (ULONG_PTR)&sys_skt->ev_mark,
                                    &aio->wov)) {
        TT_ERROR_NTV("fail to post destroy wr");

        tt_list_remove(&tev->node);
        tt_ev_destroy(ev);
        return TT_FAIL;
    }

    sys_skt->wr_closing = TT_TRUE;
    return TT_SUCCESS;
}

tt_bool_t __do_skt_destroy_wr(IN __skt_destroy_t *aio)
{
    tt_skt_ntv_t *sys_skt = &aio->skt->sys_socket;

    // all write aio must have been done
    TT_ASSERT_SKTAIO(tt_list_count(&sys_skt->write_q) == 0);

    // mark as closed
    TT_ASSERT_SKTAIO(!sys_skt->wr_closed);
    sys_skt->wr_closed = TT_TRUE;

    // check if read q is already closed
    if (sys_skt->rd_closed) {
        __destroy_skt(aio->skt, TT_TRUE, TT_TRUE);
    }

    return TT_TRUE;
}

tt_result_t __skt_destroy_rd(IN tt_skt_t *skt)
{
    tt_skt_ntv_t *sys_skt = &skt->sys_socket;
    tt_ev_t *ev;
    tt_thread_ev_t *tev;
    __skt_destroy_t *aio;

    if (sys_skt->rd_closing) {
        // TT_ERROR("already closing read");
        return TT_FAIL;
    }

    // aio
    ev =
        tt_thread_ev_create(EV_SKT_DESTROY_READ, sizeof(__skt_destroy_t), NULL);
    if (ev == NULL) {
        TT_ERROR("fail to allocate shutdown aio");
        return TT_FAIL;
    }
    tev = TT_EV_HDR(ev, tt_thread_ev_t);
    aio = TT_EV_DATA(ev, __skt_destroy_t);

    // init aio
    tt_memset(&aio->wov, 0, sizeof(WSAOVERLAPPED));
    aio->skt = skt;

    // add request
    tt_list_addtail(&sys_skt->read_q, &tev->node);

    // start iocp if it's head aio
    if ((tt_list_count(&sys_skt->read_q) == 1) &&
        !PostQueuedCompletionStatus(sys_skt->evc->sys_evc.iocp,
                                    0,
                                    (ULONG_PTR)&sys_skt->ev_mark,
                                    &aio->wov)) {
        TT_ERROR_NTV("fail to post destroy rd");

        tt_list_remove(&tev->node);
        tt_ev_destroy(ev);
        return TT_FAIL;
    }

    sys_skt->rd_closing = TT_TRUE;
    return TT_SUCCESS;
}

tt_bool_t __do_skt_destroy_rd(IN __skt_destroy_t *aio)
{
    tt_skt_ntv_t *sys_skt = &aio->skt->sys_socket;

    // all read aio must have been done
    TT_ASSERT_SKTAIO(tt_list_count(&sys_skt->read_q) == 0);

    // mark as closed
    TT_ASSERT_SKTAIO(!sys_skt->rd_closed);
    sys_skt->rd_closed = TT_TRUE;

    // check if write q is already closed
    if (sys_skt->wr_closed) {
        __destroy_skt(aio->skt, TT_TRUE, TT_TRUE);
    }

    return TT_TRUE;
}

void __destroy_skt(IN tt_skt_t *skt,
                   IN tt_bool_t do_cb,
                   IN tt_bool_t check_free)
{
    tt_skt_ntv_t *sys_skt = &skt->sys_socket;
    tt_skt_on_destroy_t on_destroy = (tt_skt_on_destroy_t)sys_skt->on_destroy;
    tt_bool_t from_alloc = skt->attr.from_alloc;

    if (sys_skt->s != INVALID_SOCKET) {
        closesocket(sys_skt->s);
        sys_skt->s = INVALID_SOCKET;
    }

    __destroy_aio_ev(sys_skt);

    if (!tt_list_empty(&sys_skt->read_q)) {
        TT_WARN("still aio in skt read q");
    }
    if (!tt_list_empty(&sys_skt->write_q)) {
        TT_WARN("still aio in skt write q");
    }

    on_destroy(skt, sys_skt->on_destroy_param);
    // it should guarantee skt is not accessed after on_destroy

    if (from_alloc) {
        tt_mem_free(skt);
    }
}

tt_result_t __create_aio_ev(IN tt_skt_ntv_t *sys_skt)
{
    tt_ev_t *ev;
    __skt_q_t *aio;

    // aio read ev
    ev = tt_thread_ev_create(EV_SKT_Q_READ, sizeof(__skt_q_t), NULL);
    if (ev == NULL) {
        TT_ERROR("fail to create EV_SKT_Q_READ");
        return TT_FAIL;
    }
    aio = TT_EV_DATA(ev, __skt_q_t);
    aio->skt = TT_CONTAINER(sys_skt, tt_skt_t, sys_socket);
    sys_skt->aio_rd = ev;

    // aio write ev
    ev = tt_thread_ev_create(EV_SKT_Q_WRITE, sizeof(__skt_q_t), NULL);
    if (ev == NULL) {
        TT_ERROR("fail to create EV_SKT_Q_WRITE");
        tt_ev_destroy(sys_skt->aio_rd);
        return TT_FAIL;
    }
    aio = TT_EV_DATA(ev, __skt_q_t);
    aio->skt = TT_CONTAINER(sys_skt, tt_skt_t, sys_socket);
    sys_skt->aio_wr = ev;

    return TT_SUCCESS;
}

void __destroy_aio_ev(IN tt_skt_ntv_t *sys_skt)
{
    if (sys_skt->aio_rd != NULL) {
        tt_thread_ev_t *tev = TT_EV_HDR(sys_skt->aio_rd, tt_thread_ev_t);

        // here the aio_ev may be removing from ev q of some evc without
        // holding any lock so the aio_ev must in the local evc
        TT_ASSERT((tev->node.lst == NULL) ||
                  (tev->node.lst == &tt_evc_current()->ev_q));
        tt_list_remove(&tev->node);
        tt_ev_destroy(sys_skt->aio_rd);
    }
    sys_skt->aio_rd = NULL;

    if (sys_skt->aio_wr != NULL) {
        tt_thread_ev_t *tev = TT_EV_HDR(sys_skt->aio_wr, tt_thread_ev_t);

        // here the aio_ev may be removing from ev q of some evc without
        // holding any lock so the aio_ev must in the local evc
        TT_ASSERT((tev->node.lst == NULL) ||
                  (tev->node.lst == &tt_evc_current()->ev_q));
        tt_list_remove(&tev->node);
        tt_ev_destroy(sys_skt->aio_wr);
    }
    sys_skt->aio_wr = NULL;
}

tt_result_t __load_wsa()
{
    SOCKET s = INVALID_SOCKET;

    const GUID guid_ConnectEx = WSAID_CONNECTEX;
    const GUID guid_AcceptEx = WSAID_ACCEPTEX;
    const GUID guid_GetAcceptExSockaddrs = WSAID_GETACCEPTEXSOCKADDRS;
    const GUID guid_DisconnectEx = WSAID_DISCONNECTEX;

    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s == INVALID_SOCKET) {
        TT_ERROR("can not create a udp socket");
        goto salw_out;
    }

    // load ConnectEx
    if (!TT_OK(__load_wsa_guid(s, &guid_ConnectEx, (void **)&tt_ConnectEx)) ||
        (tt_ConnectEx == NULL)) {
        goto salw_out;
    }

    // load AcceptEx
    if (!TT_OK(__load_wsa_guid(s, &guid_AcceptEx, (void **)&tt_AcceptEx)) ||
        (tt_AcceptEx == NULL)) {
        goto salw_out;
    }

    // load GetAcceptExSockaddrs
    if (!TT_OK(__load_wsa_guid(s,
                               &guid_GetAcceptExSockaddrs,
                               (void **)&tt_GetAcceptExSockaddrs)) ||
        (tt_GetAcceptExSockaddrs == NULL)) {
        goto salw_out;
    }

    closesocket(s);
    return TT_SUCCESS;

salw_out:

    if (s != INVALID_SOCKET) {
        closesocket(s);
    }

    return TT_FAIL;
}

tt_result_t __load_wsa_guid(IN SOCKET s, const IN GUID *guid, IN void **pfn)
{
    DWORD bytes;
    if (WSAIoctl(s,
                 SIO_GET_EXTENSION_FUNCTION_POINTER,
                 (LPVOID)guid,
                 sizeof(GUID),
                 pfn,
                 sizeof(void *),
                 &bytes,
                 NULL,
                 NULL) == 0) {
        return TT_SUCCESS;
    } else {
        TT_NET_ERROR_NTV("fail to load winsock api");
        return TT_FAIL;
    }
}

// return true if buffers are full
tt_bool_t __update_sbuf_state(IN __sbuf_state_t *sbuf_state,
                              IN tt_u32_t io_bytes)
{
    // update send_len
    sbuf_state->io_bytes += io_bytes;

    // update buf array
    while (sbuf_state->cur_idx < sbuf_state->wbuf_num) {
        WSABUF *cur_buf = &sbuf_state->wbuf[sbuf_state->cur_idx];
        tt_u32_t left_space;

        TT_ASSERT_SKTAIO(sbuf_state->cur_pos < cur_buf->len);
        left_space = cur_buf->len - sbuf_state->cur_pos;

        if (io_bytes < left_space) {
            sbuf_state->cur_pos += io_bytes;
            break;
        }
        io_bytes -= left_space;

        // buf full, move to next
        ++sbuf_state->cur_idx;
        sbuf_state->cur_pos = 0;
    };
    TT_ASSERT_SKTAIO(sbuf_state->cur_idx <= sbuf_state->wbuf_num);

    if (sbuf_state->cur_idx == sbuf_state->wbuf_num) {
        TT_ASSERT_SKTAIO(io_bytes == 0);
        return TT_TRUE;
    } else {
        return TT_FALSE;
    }
}

tt_bool_t __update_sbuf_state_data(IN __sbuf_state_t *sbuf_state,
                                   IN tt_u8_t *data,
                                   IN tt_u32_t data_len,
                                   OUT tt_u32_t *updated_len)
{
    tt_u32_t io_bytes = 0;

    // update buf array
    while (sbuf_state->cur_idx < sbuf_state->wbuf_num) {
        WSABUF *cur_buf = &sbuf_state->wbuf[sbuf_state->cur_idx];
        tt_u32_t left_space;

        TT_ASSERT_SKTAIO(sbuf_state->cur_pos < cur_buf->len);
        left_space = cur_buf->len - sbuf_state->cur_pos;

        if (data_len < left_space) {
            tt_memcpy(TT_PTR_INC(tt_u8_t, cur_buf->buf, sbuf_state->cur_pos),
                      data,
                      data_len);
            sbuf_state->cur_pos += data_len;
            io_bytes += data_len;
            break;
        }
        tt_memcpy(TT_PTR_INC(tt_u8_t, cur_buf->buf, sbuf_state->cur_pos),
                  data,
                  left_space);
        data_len -= left_space;
        io_bytes += left_space;

        // buf full, move to next
        ++sbuf_state->cur_idx;
        sbuf_state->cur_pos = 0;
    };
    TT_ASSERT_SKTAIO(sbuf_state->cur_idx <= sbuf_state->wbuf_num);

    sbuf_state->io_bytes = io_bytes;
    *updated_len = io_bytes;
    if (sbuf_state->cur_idx == sbuf_state->wbuf_num) {
        // for ssl receiving, it's possible that the bytes of
        // received data is more than that delivered in recv aio

        // TT_ASSERT_SKTAIO(data_len == 0);
        return TT_TRUE;
    } else {
        return TT_FALSE;
    }
}

void __do_skt_aio_q(IN tt_skt_ntv_t *sys_skt,
                    IN tt_list_t *aio_q,
                    IN tt_result_t iocp_result,
                    IN DWORD NumberOfBytes)
{
    tt_lnode_t *node;

#ifdef TT_PLATFORM_SSL_ENABLE
    if (sys_skt->ssl_handshaking) {
        tt_result_t hsk_result;
        tt_thread_ev_t *tev;
        tt_ev_t *ev;

        node = tt_list_head(aio_q);
        TT_ASSERT_SKTAIO(node != NULL);
        tev = TT_CONTAINER(node, tt_thread_ev_t, node);
        ev = TT_EV_OF(tev);

        hsk_result =
            __ssl_handshake(sys_skt->ssl, ev, NumberOfBytes, iocp_result);
        if (TT_OK(hsk_result)) {
            iocp_result = TT_SUCCESS;
            NumberOfBytes = 0;
        } else if (hsk_result == TT_PROCEEDING) {
            return;
        } else {
            iocp_result = TT_END;
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
                                 tt_list_head(&aio->skt->sys_socket.write_q));

                tt_list_remove(node);
                aio_done = __do_skt_shutdown_wr(aio);
                if (aio_done) {
                    tt_ev_destroy(ev);
                } else {
                    TT_ASSERT_SKTAIO(
                        tt_list_count(&aio->skt->sys_socket.write_q) == 0);
                    tt_list_addtail(&aio->skt->sys_socket.write_q, node);
                }
                return;
            } break;
            case EV_SKT_SHUTDOWN_READ: {
                __skt_shutdown_t *aio = TT_EV_DATA(ev, __skt_shutdown_t);

                // it should be the head aio in read q
                TT_ASSERT_SKTAIO(&tev->node ==
                                 tt_list_head(&aio->skt->sys_socket.read_q));

                tt_list_remove(node);
                __do_skt_shutdown_rd(aio);
                tt_ev_destroy(ev);
                return;
            } break;
            case EV_SKT_DESTROY_WRITE: {
                __skt_destroy_t *aio = TT_EV_DATA(ev, __skt_destroy_t);

                // it should be the head aio in write q
                TT_ASSERT_SKTAIO(&tev->node ==
                                 tt_list_head(&aio->skt->sys_socket.write_q));

                tt_list_remove(node);
                __do_skt_destroy_wr(aio);
                tt_ev_destroy(ev);
                return;
            } break;
            case EV_SKT_DESTROY_READ: {
                __skt_destroy_t *aio = TT_EV_DATA(ev, __skt_destroy_t);

                // it should be the head aio in read q
                TT_ASSERT_SKTAIO(&tev->node ==
                                 tt_list_head(&aio->skt->sys_socket.read_q));

                tt_list_remove(node);
                __do_skt_destroy_rd(aio);
                tt_ev_destroy(ev);
                return;
            } break;
            case EV_SKT_CONNECT: {
                __skt_connect_t *aio = TT_EV_DATA(ev, __skt_connect_t);

                TT_ASSERT_SKTAIO(sys_skt == &aio->skt->sys_socket);

                aio_done = __do_skt_connect(aio, iocp_result);
            } break;
            case EV_SKT_ACCEPT: {
                __skt_accept_t *aio = TT_EV_DATA(ev, __skt_accept_t);

                TT_ASSERT_SKTAIO(sys_skt == &aio->listening_skt->sys_socket);

                aio_done = __do_skt_accept(aio, iocp_result);
            } break;
            case EV_SKT_SEND: {
                __skt_send_t *aio = TT_EV_DATA(ev, __skt_send_t);

                TT_ASSERT_SKTAIO(sys_skt == &aio->skt->sys_socket);

                aio_done = __do_skt_send(aio, iocp_result, NumberOfBytes);
            } break;
            case EV_SKT_RECV: {
                __skt_recv_t *aio = TT_EV_DATA(ev, __skt_recv_t);

                TT_ASSERT_SKTAIO(sys_skt == &aio->skt->sys_socket);

                aio_done = __do_skt_recv(aio, iocp_result, NumberOfBytes);
            } break;
            case EV_SKT_SENDTO: {
                __skt_sendto_t *aio = TT_EV_DATA(ev, __skt_sendto_t);

                TT_ASSERT_SKTAIO(sys_skt == &aio->skt->sys_socket);

                aio_done = __do_skt_sendto(aio, iocp_result, NumberOfBytes);
            } break;
            case EV_SKT_RECVFROM: {
                __skt_recvfrom_t *aio = TT_EV_DATA(ev, __skt_recvfrom_t);

                TT_ASSERT_SKTAIO(sys_skt == &aio->skt->sys_socket);

                aio_done = __do_skt_recvfrom(aio, iocp_result, NumberOfBytes);
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

        // pass to next aio
        NumberOfBytes = 0;
        iocp_result = TT_SUCCESS;
    }
}

void __clear_skt_aio_q(IN tt_skt_ntv_t *sys_skt,
                       IN tt_list_t *aio_q,
                       IN tt_result_t aio_result)
{
    tt_lnode_t *node;

    // it would crash there are still some pending overlapped operation
    while ((node = tt_list_head(aio_q)) != NULL) {
        tt_thread_ev_t *tev = TT_CONTAINER(node, tt_thread_ev_t, node);
        tt_ev_t *ev = TT_EV_OF(tev);

        switch (ev->ev_id) {
            case EV_SKT_SHUTDOWN_WRITE: {
                __skt_shutdown_t *aio = TT_EV_DATA(ev, __skt_shutdown_t);

                // it should be the head aio in write q
                TT_ASSERT_SKTAIO(&tev->node ==
                                 tt_list_head(&aio->skt->sys_socket.write_q));

                // ignore
            } break;
            case EV_SKT_SHUTDOWN_READ: {
                __skt_shutdown_t *aio = TT_EV_DATA(ev, __skt_shutdown_t);

                // it should be the head aio in read q
                TT_ASSERT_SKTAIO(&tev->node ==
                                 tt_list_head(&aio->skt->sys_socket.read_q));

                // ignore
            } break;
            case EV_SKT_DESTROY_WRITE: {
                __skt_destroy_t *aio = TT_EV_DATA(ev, __skt_destroy_t);

                // it should be the head aio in write q
                TT_ASSERT_SKTAIO(&tev->node ==
                                 tt_list_head(&aio->skt->sys_socket.write_q));

                // ignore
            } break;
            case EV_SKT_DESTROY_READ: {
                __skt_destroy_t *aio = TT_EV_DATA(ev, __skt_destroy_t);

                // it should be the head aio in write q
                TT_ASSERT_SKTAIO(&tev->node ==
                                 tt_list_head(&aio->skt->sys_socket.read_q));

                // ignore
            } break;
            case EV_SKT_CONNECT: {
                __skt_connect_t *aio = TT_EV_DATA(ev, __skt_connect_t);

                // it should be the head aio in write q
                TT_ASSERT_SKTAIO(&tev->node ==
                                 tt_list_head(&aio->skt->sys_socket.write_q));

                aio->result = aio_result;
                __do_skt_connect_cb(aio);
            } break;
            case EV_SKT_ACCEPT: {
                __skt_accept_t *aio = TT_EV_DATA(ev, __skt_accept_t);

                // it should be the head aio in read q
                TT_ASSERT_SKTAIO(
                    &tev->node ==
                    tt_list_head(&aio->listening_skt->sys_socket.read_q));

                aio->result = aio_result;
                __do_skt_accept_cb(aio);
            } break;
            case EV_SKT_SEND: {
                __skt_send_t *aio = TT_EV_DATA(ev, __skt_send_t);

                TT_ASSERT_SKTAIO(&tev->node ==
                                 tt_list_head(&aio->skt->sys_socket.write_q));

                aio->result = aio_result;
                __do_skt_send_cb(aio);
            } break;
            case EV_SKT_RECV: {
                __skt_recv_t *aio = TT_EV_DATA(ev, __skt_recv_t);

                TT_ASSERT_SKTAIO(&tev->node ==
                                 tt_list_head(&aio->skt->sys_socket.read_q));

                aio->result = aio_result;
                __do_skt_recv_cb(aio);
            } break;
            case EV_SKT_SENDTO: {
                __skt_sendto_t *aio = TT_EV_DATA(ev, __skt_sendto_t);

                TT_ASSERT_SKTAIO(&tev->node ==
                                 tt_list_head(&aio->skt->sys_socket.write_q));

                aio->result = aio_result;
                __do_skt_sendto_cb(aio);
            } break;
            case EV_SKT_RECVFROM: {
                __skt_recvfrom_t *aio = TT_EV_DATA(ev, __skt_recvfrom_t);

                TT_ASSERT_SKTAIO(&tev->node ==
                                 tt_list_head(&aio->skt->sys_socket.read_q));

                aio->result = aio_result;
                __do_skt_recvfrom_cb(aio);
            } break;
            default: {
                tt_u32_t ev_range = TT_EV_RANGE(ev->ev_id);
                tt_result_t result = TT_FAIL;

#ifdef TT_PLATFORM_SSL_ENABLE
                if (ev_range == TT_EV_RANGE_INTERNAL_SSL) {
                    result = __ssl_destroy_aio(ev);
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

void __wsabuf2blob_inline(IN WSABUF *wbuf, IN tt_u32_t wbuf_num)
{
    tt_blob_t *blob = (tt_blob_t *)wbuf;
    tt_u32_t i;

    TT_ASSERT_SKTAIO(sizeof(WSABUF) >= sizeof(tt_blob_t));

    for (i = 0; i < wbuf_num; ++i) {
        WSABUF tmp;
        tmp.buf = wbuf[i].buf;
        tmp.len = wbuf[i].len;

        blob[i].addr = (tt_u8_t *)tmp.buf;
        blob[i].len = (tt_u32_t)tmp.len;
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
        return tt_evc_sendto_thread(evc, tev);
    }
}

enum
{
    __SF_IO_END,
    __SF_PARTIAL,
    __SF_FAIL,
    __SF_SYS,

    __SF_NUM
};

int WSASend_SF(SOCKET s,
               LPWSABUF lpBuffers,
               DWORD dwBufferCount,
               LPDWORD lpNumberOfBytesSent,
               DWORD dwFlags,
               LPWSAOVERLAPPED lpOverlapped,
               LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    int v = rand() % __SF_NUM;
    int ret;

    LPWSABUF buf = (LPWSABUF)malloc(sizeof(WSABUF) * dwBufferCount);
    tt_memcpy(buf, lpBuffers, sizeof(WSABUF) * dwBufferCount);

    if (buf[0].len < 50) {
        v = __SF_SYS;
    }

    if (v == __SF_IO_END) {
        buf[0].len = 0;
    } else if (v == __SF_PARTIAL) {
        buf[0].len -= (rand() % 50);
        dwBufferCount = 1;
    } else if (v == __SF_FAIL) {
        s = INVALID_SOCKET;
    }

#undef WSASend
    ret = WSASend(s,
                  lpBuffers,
                  dwBufferCount,
                  lpNumberOfBytesSent,
                  dwFlags,
                  lpOverlapped,
                  lpCompletionRoutine);

    free(buf);
    return ret;
}

int WSARecv_SF(SOCKET s,
               LPWSABUF lpBuffers,
               DWORD dwBufferCount,
               LPDWORD lpNumberOfBytesRecvd,
               LPDWORD lpFlags,
               LPWSAOVERLAPPED lpOverlapped,
               LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    int v = rand() % __SF_NUM;
    int ret;

    LPWSABUF buf = (LPWSABUF)malloc(sizeof(WSABUF) * dwBufferCount);
    tt_memcpy(buf, lpBuffers, sizeof(WSABUF) * dwBufferCount);

    if (buf[0].len < 50) {
        v = __SF_SYS;
    }

    if (v == __SF_IO_END) {
        buf[0].len = 0;
    } else if (v == __SF_PARTIAL) {
        buf[0].len -= (rand() % 50);
        dwBufferCount = 1;
    } else if (v == __SF_FAIL) {
        s = INVALID_SOCKET;
    }

#undef WSARecv
    ret = WSARecv(s,
                  buf,
                  dwBufferCount,
                  lpNumberOfBytesRecvd,
                  lpFlags,
                  lpOverlapped,
                  lpCompletionRoutine);

    free(buf);
    return ret;
}

int WSASendTo_SF(SOCKET s,
                 LPWSABUF lpBuffers,
                 DWORD dwBufferCount,
                 LPDWORD lpNumberOfBytesSent,
                 DWORD dwFlags,
                 const struct sockaddr *lpTo,
                 int iToLen,
                 LPWSAOVERLAPPED lpOverlapped,
                 LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    int v = rand() % __SF_NUM;
    int ret;

    LPWSABUF buf = (LPWSABUF)malloc(sizeof(WSABUF) * dwBufferCount);
    tt_memcpy(buf, lpBuffers, sizeof(WSABUF) * dwBufferCount);

    if (buf[0].len < 50) {
        v = __SF_SYS;
    }

    if (v == __SF_IO_END) {
        buf[0].len = 0;
    } else if (v == __SF_PARTIAL) {
        buf[0].len -= (rand() % 50);
        dwBufferCount = 1;
    } else if (v == __SF_FAIL) {
        s = INVALID_SOCKET;
    }

#undef WSASendTo
    ret = WSASendTo(s,
                    buf,
                    dwBufferCount,
                    lpNumberOfBytesSent,
                    dwFlags,
                    lpTo,
                    iToLen,
                    lpOverlapped,
                    lpCompletionRoutine);

    free(buf);
    return ret;
}

int WSARecvFrom_SF(SOCKET s,
                   LPWSABUF lpBuffers,
                   DWORD dwBufferCount,
                   LPDWORD lpNumberOfBytesRecvd,
                   LPDWORD lpFlags,
                   struct sockaddr *lpFrom,
                   LPINT lpFromlen,
                   LPWSAOVERLAPPED lpOverlapped,
                   LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    int v = rand() % __SF_NUM;
    int ret;

    LPWSABUF buf = (LPWSABUF)malloc(sizeof(WSABUF) * dwBufferCount);
    tt_memcpy(buf, lpBuffers, sizeof(WSABUF) * dwBufferCount);

    if (buf[0].len < 50) {
        v = __SF_SYS;
    }

    if (v == __SF_IO_END) {
        // buf[0].len = 0;
        // set to 0 would discard some data and makeing test routines hang
    } else if (v == __SF_PARTIAL) {
        // change nothing or some udp packet is discarded
    } else if (v == __SF_FAIL) {
        s = INVALID_SOCKET;
    }

#undef WSARecvFrom
    ret = WSARecvFrom(s,
                      buf,
                      dwBufferCount,
                      lpNumberOfBytesRecvd,
                      lpFlags,
                      lpFrom,
                      lpFromlen,
                      lpOverlapped,
                      lpCompletionRoutine);

    free(buf);
    return ret;
}

#endif

#ifdef __SIMULATE_SSL_AIO_FAIL

int WSASend_SSL(SOCKET s,
                LPWSABUF lpBuffers,
                DWORD dwBufferCount,
                LPDWORD lpNumberOfBytesSent,
                DWORD dwFlags,
                LPWSAOVERLAPPED lpOverlapped,
                LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    int ret;

    LPWSABUF buf = (LPWSABUF)malloc(sizeof(WSABUF) * dwBufferCount);
    tt_memcpy(buf, lpBuffers, sizeof(WSABUF) * dwBufferCount);

    if (buf[0].len > 50) {
        buf[0].len -= (rand() % 50);
        dwBufferCount = 1;
    }

#undef WSASend
    ret = WSASend(s,
                  lpBuffers,
                  dwBufferCount,
                  lpNumberOfBytesSent,
                  dwFlags,
                  lpOverlapped,
                  lpCompletionRoutine);

    free(buf);
    return ret;
}

int WSARecv_SSL(SOCKET s,
                LPWSABUF lpBuffers,
                DWORD dwBufferCount,
                LPDWORD lpNumberOfBytesRecvd,
                LPDWORD lpFlags,
                LPWSAOVERLAPPED lpOverlapped,
                LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    int ret;

    LPWSABUF buf = (LPWSABUF)malloc(sizeof(WSABUF) * dwBufferCount);
    tt_memcpy(buf, lpBuffers, sizeof(WSABUF) * dwBufferCount);

    if (buf[0].len > 50) {
        buf[0].len -= (rand() % 50);
        dwBufferCount = 1;
    }

#undef WSARecv
    ret = WSARecv(s,
                  buf,
                  dwBufferCount,
                  lpNumberOfBytesRecvd,
                  lpFlags,
                  lpOverlapped,
                  lpCompletionRoutine);

    free(buf);
    return ret;
}

#endif
