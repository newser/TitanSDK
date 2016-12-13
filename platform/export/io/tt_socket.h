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
@file tt_socket.h
@brief socket system

this file specifies socket APIs
*/

#ifndef __TT_SOCKET__
#define __TT_SOCKET__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <io/tt_socket_addr.h>
#include <os/tt_atomic.h>

#include <tt_socket_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct tt_skt_attr_s
{
    tt_bool_t config_ipv6only : 1;
    tt_bool_t ipv6only : 1;

    tt_bool_t config_reuse_addr : 1;
    tt_bool_t reuse_addr : 1;

    tt_bool_t config_reuse_port : 1;
    tt_bool_t reuse_port : 1;

    tt_bool_t config_tcp_nodelay : 1;
    tt_bool_t tcp_nodelay : 1;

    tt_bool_t from_alloc : 1;
} tt_skt_attr_t;

typedef struct tt_skt_s
{
    tt_skt_ntv_t sys_socket;

    tt_net_family_t family;
    tt_net_protocol_t protocol;
    tt_skt_attr_t attr;
} tt_skt_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

extern tt_atomic_s64_t tt_stat_socket_num;

extern tt_atomic_s64_t tt_stat_socket_peek;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/**
@fn void tt_skt_component_register()
register socket system
*/
extern void tt_skt_component_register();

extern tt_result_t tt_skt_create(OUT tt_skt_t *skt,
                                 IN tt_net_family_t family,
                                 IN tt_net_protocol_t protocol,
                                 IN OPT tt_skt_attr_t *attr);

extern tt_result_t tt_skt_destroy(IN tt_skt_t *skt);

tt_inline tt_result_t tt_skt_shutdown(IN tt_skt_t *skt, IN tt_u32_t mode)
{
    TT_ASSERT(skt != NULL);

    return tt_skt_shutdown_ntv(&skt->sys_socket, mode);
}
#define TT_SKT_SHUTDOWN_RD TT_SKT_SHUTDOWN_RD_NTV
#define TT_SKT_SHUTDOWN_WR TT_SKT_SHUTDOWN_WR_NTV
#define TT_SKT_SHUTDOWN_RDWR TT_SKT_SHUTDOWN_RDWR_NTV

// behavior of binding ipv4/ipv6 address to ipv4/ipv6 socket depends
// on platform
extern tt_result_t tt_skt_bind(IN tt_skt_t *skt, IN tt_sktaddr_t *local_addr);

extern tt_result_t tt_skt_bind_n(IN tt_skt_t *skt,
                                 IN tt_net_family_t family,
                                 IN tt_sktaddr_addr_t *addr,
                                 IN tt_u16_t port);

extern tt_result_t tt_skt_bind_p(IN tt_skt_t *skt,
                                 IN tt_net_family_t family,
                                 IN tt_char_t *addr,
                                 IN tt_u16_t port);

extern tt_result_t tt_skt_listen(IN tt_skt_t *skt, IN tt_u32_t backlog);
// backlog
#define TT_SKT_BACKLOG_DEFAULT (~0)

// this function does not guarantee that the new socket would inherite
// attributes of listening_skt, although os may do some about it). caller
// should explicitly set options by api defined in tt_socket_option.h
extern tt_result_t tt_skt_accept(IN tt_skt_t *listening_skt,
                                 OUT tt_skt_t *new_socket,
                                 IN OPT tt_skt_attr_t *new_skt_attr);

extern tt_result_t tt_skt_connect(IN tt_skt_t *skt,
                                  IN tt_sktaddr_t *remote_addr);

extern tt_result_t tt_skt_connect_n(IN tt_skt_t *skt,
                                    IN tt_net_family_t family,
                                    IN tt_sktaddr_addr_t *addr,
                                    IN tt_u16_t port);

extern tt_result_t tt_skt_connect_p(IN tt_skt_t *skt,
                                    IN tt_net_family_t family,
                                    IN tt_char_t *addr,
                                    IN tt_u16_t port);

/**
@fn tt_result_t tt_skt_send(IN tt_skt_t *skt,
                               OUT tt_u8_t *buf,
                               IN tt_u32_t buf_len,
                               OUT tt_u32_t *send_len);
send data through socket

@param [in] skt socket
@param [out] buf data to be sent
@param [in] buf_len size of buf
@param [out] send_len stores how many bytes are sent, can be NULL

@return
- TT_SUCCESS some content of buffer is sent
- TT_FAIL none is sent due to some error occur

@note
- the bytes returned by send_len may be less than buf_len
- this function return TT_SUCCESS even if the operation is not completed, e.g.
  operation is interrupted, but send_len returns how many bytes are sent
  accurately.
- send_len return how many bytes read only if return value is TT_SUCCESS
*/
extern tt_result_t tt_skt_send(IN tt_skt_t *skt,
                               OUT tt_u8_t *buf,
                               IN tt_u32_t buf_len,
                               OUT OPT tt_u32_t *send_len);

extern tt_result_t tt_skt_recv(IN tt_skt_t *skt,
                               IN tt_u8_t *buf,
                               IN tt_u32_t buf_len,
                               OUT OPT tt_u32_t *recv_len);

extern tt_result_t tt_skt_recvfrom(IN tt_skt_t *skt,
                                   OUT tt_u8_t *buf,
                                   IN tt_u32_t buf_len,
                                   OUT OPT tt_u32_t *recv_len,
                                   OUT OPT tt_sktaddr_t *remote_addr);

extern tt_result_t tt_skt_sendto(IN tt_skt_t *skt,
                                 IN tt_u8_t *buf,
                                 IN tt_u32_t buf_len,
                                 OUT OPT tt_u32_t *send_len,
                                 IN tt_sktaddr_t *remote_addr);

extern tt_result_t tt_skt_local_addr(IN tt_skt_t *skt, IN tt_sktaddr_t *addr);

extern tt_result_t tt_skt_remote_addr(IN tt_skt_t *skt, IN tt_sktaddr_t *addr);

extern void tt_skt_stat_show(IN tt_u32_t flag);

extern void tt_skt_stat_inc_num();
extern void tt_skt_stat_dec_num();

// ========================================
// multicast APIs
// ========================================

// must bind to address to receive multicast packets
extern tt_result_t tt_skt_join_mcast(IN tt_skt_t *skt,
                                     IN tt_sktaddr_addr_t *mc_addr,
                                     IN OPT tt_char_t *mcast_itf);

extern tt_result_t tt_skt_leave_mcast(IN tt_skt_t *skt,
                                      IN tt_sktaddr_addr_t *mc_addr,
                                      IN OPT tt_char_t *mcast_itf);

// ========================================
// server socket APIs
// ========================================

// - use tt_skt_destroy() to destroy created server socket
// - created socket could be passed to tt_skt_accept()
extern tt_result_t tt_tcp_server(OUT tt_skt_t *skt,
                                 IN tt_net_family_t family,
                                 IN tt_skt_attr_t *attr,
                                 IN tt_sktaddr_t *local_addr,
                                 IN tt_u32_t backlog);

extern tt_result_t tt_udp_server(OUT tt_skt_t *skt,
                                 IN tt_net_family_t family,
                                 IN OPT tt_skt_attr_t *attr,
                                 IN tt_sktaddr_t *local_addr);

extern tt_result_t tt_udp_server_mcastast(OUT tt_skt_t *skt,
                                          IN tt_net_family_t family,
                                          IN tt_skt_attr_t *attr,
                                          IN tt_sktaddr_t *local_addr,
                                          IN OPT tt_sktaddr_addr_t *mcast_addr,
                                          IN OPT tt_char_t *mcast_itf);

// ========================================
// socket attributes
// ========================================

extern void tt_skt_attr_default(IN tt_skt_attr_t *attr);

tt_inline void tt_skt_attr_set_ipv6only(IN tt_skt_attr_t *attr,
                                        IN tt_bool_t ipv6only)
{
    attr->config_ipv6only = TT_TRUE;
    attr->ipv6only = ipv6only;
}

tt_inline void tt_skt_attr_set_reuseaddr(IN tt_skt_attr_t *attr,
                                         IN tt_bool_t reuse_addr)
{
    attr->config_reuse_addr = TT_TRUE;
    attr->reuse_addr = reuse_addr;
}

tt_inline void tt_skt_attr_set_reuseport(IN tt_skt_attr_t *attr,
                                         IN tt_bool_t reuse_port)
{
    attr->config_reuse_port = TT_TRUE;
    attr->reuse_port = reuse_port;
}

tt_inline void tt_skt_attr_set_nodelay(IN tt_skt_attr_t *attr,
                                       IN tt_bool_t tcp_nodelay)
{
    attr->config_tcp_nodelay = TT_TRUE;
    attr->tcp_nodelay = tcp_nodelay;
}

#endif // __TT_SOCKET__
