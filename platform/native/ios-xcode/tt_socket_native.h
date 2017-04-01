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

/**
@file tt_socket_native.h
@brief socket native apis

this file specifies socket native apis
*/

#ifndef __TT_SOCKET_NATIVE__
#define __TT_SOCKET_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_list.h>
#include <event/tt_event_base.h>
#include <io/tt_socket_addr.h>
#include <io/tt_socket_aio_cb.h>

#include <tt_sys_error.h>

#include <Security/SecureTransport.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_SKT_SHUT_RD_NTV SHUT_RD
#define TT_SKT_SHUT_WR_NTV SHUT_WR
#define TT_SKT_SHUT_RDWR_NTV SHUT_RDWR

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_profile_s;
struct tt_evcenter_s;
struct tt_evpoller_s;

struct tt_skt_attr_s;
struct tt_skt_ntv_s;

struct tt_ssl_s;

// socket event ranges
enum
{
    TT_SKT_RANGE_AIO,
    TT_SKT_RANGE_KQ_MARK,
};

enum
{
    TT_SKT_EV_START =
        TT_EV_MKID_SUB(TT_EV_RANGE_INTERNAL_SOCKET, TT_SKT_RANGE_KQ_MARK, 0),

    TT_SKT_MARK_KQ_RD,
    TT_SKT_MARK_KQ_WR,

    TT_SKT_EV_END,
};

typedef enum tt_skt_role_s {
    TT_SKT_ROLE_UDP,

    TT_SKT_ROLE_TCP_LISTEN,
    TT_SKT_ROLE_TCP_CONNECT,
    TT_SKT_ROLE_TCP_ACCEPT,

    TT_SKT_ROLE_NUM
} tt_skt_role_t;
#define TT_SKT_ROLE_VALID(r) ((r) < TT_SKT_ROLE_NUM)

typedef struct tt_skt_ntv_s
{
    int s;

    // ========================================
    // below for async aio
    // ========================================

    struct tt_evcenter_s *evc;
    tt_u32_t ev_mark_rd;
    tt_u32_t ev_mark_wr;
    tt_skt_role_t role;

    tt_skt_on_destroy_t on_destroy;
    void *on_destroy_param;

    tt_list_t read_q;
    tt_list_t write_q;

    // ssl
    struct tt_ssl_s *ssl;

    tt_bool_t rd_closing : 1;
    tt_bool_t rd_closed : 1;
    tt_bool_t ssl_rd_handshaking : 1;
    tt_bool_t ssl_want_rd : 1;
    tt_bool_t wr_closing : 1;
    tt_bool_t wr_closed : 1;
    tt_bool_t ssl_wr_handshaking : 1;
    tt_bool_t ssl_want_wr : 1;
    tt_bool_t connecting : 1;
    tt_bool_t connected : 1;
} tt_skt_ntv_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_skt_component_init_ntv(IN struct tt_profile_s *profile);

extern tt_result_t tt_skt_create_ntv(OUT tt_skt_ntv_t *skt,
                                     IN tt_net_family_t family,
                                     IN tt_net_protocol_t protocol,
                                     IN struct tt_skt_attr_s *attr);

tt_inline tt_result_t tt_skt_shutdown_ntv(IN tt_skt_ntv_t *skt,
                                          IN tt_u32_t mode)
{
    if (shutdown(skt->s, mode) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to shutdown socket");
        return TT_FAIL;
    }
}

extern tt_result_t tt_skt_destroy_ntv(IN tt_skt_ntv_t *skt);

tt_inline tt_result_t tt_skt_bind_ntv(IN tt_skt_ntv_t *skt,
                                      IN tt_sktaddr_ntv_t *local_addr)
{
    // mac need accurate value of the 3rd argument of bind()
    // local_addr->ss_len should have been set in tt_sktaddr_init()
    if (bind(skt->s, (struct sockaddr *)local_addr, local_addr->ss_len) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("binding fail");
        return TT_FAIL;
    }
}

extern tt_result_t tt_skt_listen_ntv(IN tt_skt_ntv_t *skt, IN tt_u32_t backlog);

extern tt_result_t tt_skt_accept_ntv(IN tt_skt_ntv_t *skt,
                                     OUT tt_skt_ntv_t *new_socket,
                                     OUT tt_sktaddr_ntv_t *remote_addr);

extern tt_result_t tt_skt_connect_ntv(IN tt_skt_ntv_t *skt,
                                      IN tt_sktaddr_ntv_t *remote_addr);

extern tt_result_t tt_skt_local_addr_ntv(IN tt_skt_ntv_t *skt,
                                         OUT tt_sktaddr_ntv_t *local_addr);
extern tt_result_t tt_skt_remote_addr_ntv(IN tt_skt_ntv_t *skt,
                                          OUT tt_sktaddr_ntv_t *remote_addr);

// recv_len stores how many bytes are received only when return value is
// TT_SUCCESS
extern tt_result_t tt_skt_recvfrom_ntv(IN tt_skt_ntv_t *skt,
                                       OUT tt_u8_t *buf,
                                       IN tt_u32_t buf_len,
                                       OUT tt_u32_t *recv_len,
                                       OUT tt_sktaddr_ntv_t *remote_addr);
// send_len stores how many bytes are sent only when return value is TT_SUCCESS
extern tt_result_t tt_skt_sendto_ntv(IN tt_skt_ntv_t *skt,
                                     IN tt_u8_t *buf,
                                     IN tt_u32_t buf_len,
                                     OUT tt_u32_t *send_len,
                                     IN tt_sktaddr_ntv_t *remote_addr);

// recv_len stores how many bytes are received only when return value is
// TT_SUCCESS
extern tt_result_t tt_skt_recv_ntv(IN tt_skt_ntv_t *skt,
                                   IN tt_u8_t *buf,
                                   IN tt_u32_t buf_len,
                                   OUT tt_u32_t *recv_len);
// send_len stores how many bytes are sent only when return value is TT_SUCCESS
extern tt_result_t tt_skt_send_ntv(IN tt_skt_ntv_t *skt,
                                   OUT tt_u8_t *buf,
                                   IN tt_u32_t buf_len,
                                   OUT tt_u32_t *send_len);

extern tt_result_t tt_skt_join_mcast_ntv(IN tt_skt_ntv_t *skt,
                                         IN tt_net_family_t family,
                                         IN tt_sktaddr_ip_t *mc_addr,
                                         IN tt_char_t *mcast_itf);
extern tt_result_t tt_skt_leave_mcast_ntv(IN tt_skt_ntv_t *skt,
                                          IN tt_net_family_t family,
                                          IN tt_sktaddr_ip_t *mc_addr,
                                          IN tt_char_t *mcast_itf);

#endif // __TT_SOCKET_NATIVE__
