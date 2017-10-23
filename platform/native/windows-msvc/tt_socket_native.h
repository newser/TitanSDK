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

#include <io/tt_socket_addr.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_profile_s;
struct tt_skt_attr_s;
struct tt_io_ev_s;
struct tt_fiber_ev_t;
struct tt_tmr_s;

typedef struct tt_skt_ntv_s
{
    SOCKET s;
    int af;
} tt_skt_ntv_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_skt_component_init_ntv(IN struct tt_profile_s *profile);

extern tt_result_t tt_skt_create_ntv(IN tt_skt_ntv_t *skt,
                                     IN tt_net_family_t family,
                                     IN tt_net_protocol_t protocol,
                                     IN OPT struct tt_skt_attr_s *attr);

extern void tt_skt_destroy_ntv(IN tt_skt_ntv_t *skt);

extern tt_result_t tt_skt_shutdown_ntv(IN tt_skt_ntv_t *skt,
                                       IN tt_skt_shut_t shut);

extern tt_result_t tt_skt_bind_ntv(IN tt_skt_ntv_t *skt, IN tt_sktaddr_t *addr);

extern tt_result_t tt_skt_listen_ntv(IN tt_skt_ntv_t *skt);

extern tt_result_t tt_skt_accept_ntv(IN tt_skt_ntv_t *skt,
                                     OUT tt_skt_ntv_t *new_skt,
                                     OUT tt_sktaddr_t *addr);

extern tt_result_t tt_skt_connect_ntv(IN tt_skt_ntv_t *skt,
                                      IN tt_sktaddr_t *addr);

extern tt_result_t tt_skt_local_addr_ntv(IN tt_skt_ntv_t *skt,
                                         OUT tt_sktaddr_t *addr);

extern tt_result_t tt_skt_remote_addr_ntv(IN tt_skt_ntv_t *skt,
                                          OUT tt_sktaddr_t *addr);

// recv_len stores how many bytes are received only when return value is
// TT_SUCCESS
extern tt_result_t tt_skt_recvfrom_ntv(IN tt_skt_ntv_t *skt,
                                       OUT tt_u8_t *buf,
                                       IN tt_u32_t len,
                                       OUT tt_u32_t *recvd,
                                       OUT tt_sktaddr_t *addr,
                                       OUT struct tt_fiber_ev_t **p_fev,
                                       OUT struct tt_tmr_s **p_tmr);

// send_len stores how many bytes are sent only when return value is TT_SUCCESS
extern tt_result_t tt_skt_sendto_ntv(IN tt_skt_ntv_t *skt,
                                     IN tt_u8_t *buf,
                                     IN tt_u32_t len,
                                     OUT OPT tt_u32_t *sent,
                                     IN tt_sktaddr_t *addr);

// recv_len stores how many bytes are received only when return value is
// TT_SUCCESS
extern tt_result_t tt_skt_recv_ntv(IN tt_skt_ntv_t *skt,
                                   OUT tt_u8_t *buf,
                                   IN tt_u32_t len,
                                   OUT tt_u32_t *recvd,
                                   OUT struct tt_fiber_ev_t **p_fev,
                                   OUT struct tt_tmr_s **p_tmr);

// send_len stores how many bytes are sent only when return value is TT_SUCCESS
extern tt_result_t tt_skt_send_ntv(IN tt_skt_ntv_t *skt,
                                   IN tt_u8_t *buf,
                                   IN tt_u32_t len,
                                   OUT OPT tt_u32_t *sent);

extern tt_result_t tt_skt_join_mcast_ntv(IN tt_skt_ntv_t *skt,
                                         IN tt_net_family_t family,
                                         IN tt_sktaddr_ip_t *addr,
                                         IN const tt_char_t *itf);

extern tt_result_t tt_skt_leave_mcast_ntv(IN tt_skt_ntv_t *skt,
                                          IN tt_net_family_t family,
                                          IN tt_sktaddr_ip_t *addr,
                                          IN const tt_char_t *itf);

extern void tt_skt_worker_io(IN struct tt_io_ev_s *io_ev);

extern tt_bool_t tt_skt_poller_io(IN struct tt_io_ev_s *io_ev);

#endif // __TT_SOCKET_NATIVE__
