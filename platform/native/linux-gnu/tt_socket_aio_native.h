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
@file tt_socket_aio_native.h
@brief async socket io

this file defines async socket io APIs
*/

#ifndef __TT_SKT_AIO_NATIVE__
#define __TT_SKT_AIO_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <io/tt_socket.h>
#include <io/tt_socket_aio_cb.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

// test macro
//#define __SIMULATE_SKT_AIO_FAIL

// check socket role
#define __SKT_ROLE_OF_UDP(r)                                                   \
    (((r) >= TT_SKT_ROLE_UDP) && ((r) <= TT_SKT_ROLE_UDP))
#define __SKT_ROLE_OF_TCP(r)                                                   \
    (((r) >= TT_SKT_ROLE_TCP_LISTEN) && ((r) <= TT_SKT_ROLE_TCP_ACCEPT))
#define __SKT_ROLE_OF_TCP_DATA(r)                                              \
    (((r) >= TT_SKT_ROLE_TCP_CONNECT) && ((r) <= TT_SKT_ROLE_TCP_ACCEPT))
#define __SKT_ROLE_OF_TCP_CONNECT(r)                                           \
    (((r) >= TT_SKT_ROLE_TCP_CONNECT) && ((r) <= TT_SKT_ROLE_TCP_CONNECT))
#define __SKT_ROLE_OF_TCP_ACCEPT(r)                                            \
    (((r) >= TT_SKT_ROLE_TCP_ACCEPT) && ((r) <= TT_SKT_ROLE_TCP_ACCEPT))

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_profile_s;
struct tt_ev_s;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_skt_aio_component_init_ntv(
    IN struct tt_profile_s *profile);

extern tt_result_t tt_async_skt_create_ntv(OUT tt_skt_t *skt,
                                           IN tt_net_family_t family,
                                           IN tt_net_protocol_t protocol,
                                           IN tt_u32_t role,
                                           IN OPT tt_skt_attr_t *attr,
                                           IN tt_skt_exit_t *exit);

extern void tt_async_skt_destroy_ntv(IN tt_skt_t *skt, IN tt_bool_t immediate);

extern tt_result_t tt_async_skt_shutdown_ntv(IN tt_skt_t *skt,
                                             IN tt_u32_t mode);

extern tt_result_t tt_skt_accept_async_ntv(IN tt_skt_t *listening_skt,
                                           IN tt_skt_t *new_skt,
                                           IN OPT tt_skt_attr_t *new_skt_attr,
                                           IN tt_skt_exit_t *new_skt_exit,
                                           IN tt_skt_on_accept_t on_accept,
                                           IN OPT void *cb_param);

extern tt_result_t tt_skt_connect_async_ntv(IN tt_skt_t *skt,
                                            IN tt_sktaddr_t *remote_addr,
                                            IN tt_skt_on_connect_t on_connect,
                                            IN OPT void *cb_param);

extern tt_result_t tt_skt_send_async_ntv(IN tt_skt_t *skt,
                                         IN tt_blob_t *blob,
                                         IN tt_u32_t blob_num,
                                         IN tt_skt_on_send_t on_send,
                                         IN OPT void *cb_param);

extern tt_result_t tt_skt_recv_async_ntv(IN tt_skt_t *skt,
                                         IN tt_blob_t *blob,
                                         IN tt_u32_t blob_num,
                                         IN tt_skt_on_recv_t on_recv,
                                         IN OPT void *cb_param);

extern tt_result_t tt_skt_sendto_async_ntv(IN tt_skt_t *skt,
                                           IN tt_blob_t *blob,
                                           IN tt_u32_t blob_num,
                                           IN tt_sktaddr_t *remote_addr,
                                           IN tt_skt_on_sendto_t on_sendto,
                                           IN OPT void *cb_param);

extern tt_result_t tt_skt_recvfrom_async_ntv(IN tt_skt_t *skt,
                                             IN tt_blob_t *blob,
                                             IN tt_u32_t blob_num,
                                             IN tt_skt_on_recvfrom_t
                                                 on_recvfrom,
                                             IN OPT void *cb_param);

extern tt_result_t tt_skt_ep_handler(IN tt_skt_t *skt, IN tt_u32_t ep_events);

extern tt_result_t tt_skt_tev_handler(IN struct tt_evpoller_s *evp,
                                      IN struct tt_ev_s *ev);

extern tt_result_t tt_tcp_server_async_ntv(OUT tt_skt_t *skt,
                                           IN tt_net_family_t family,
                                           IN tt_skt_attr_t *attr,
                                           IN tt_sktaddr_t *local_addr,
                                           IN tt_u32_t backlog,
                                           IN tt_skt_exit_t *exit);

extern tt_result_t tt_udp_server_async_ntv(OUT tt_skt_t *skt,
                                           IN tt_net_family_t family,
                                           IN OPT tt_skt_attr_t *attr,
                                           IN tt_sktaddr_t *local_addr,
                                           IN tt_skt_exit_t *exit);

#endif /* __TT_SKT_AIO_NATIVE__ */
