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
#include <os/tt_fiber_event.h>

#include <tt_socket_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_tmr_s;
struct tt_file_s;

typedef struct tt_skt_s
{
    tt_skt_ntv_t sys_skt;
} tt_skt_t;

typedef struct tt_skt_attr_s
{
    tt_u32_t reserved;
} tt_skt_attr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

tt_export tt_atomic_s64_t tt_skt_stat_num;

tt_export tt_atomic_s64_t tt_skt_stat_peek;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/**
@fn void tt_skt_component_register()
register socket system
*/
tt_export void tt_skt_component_register();

tt_export tt_skt_t *tt_skt_create(IN tt_net_family_t family,
                                  IN tt_net_protocol_t protocol,
                                  IN OPT tt_skt_attr_t *attr);

tt_export void tt_skt_destroy(IN tt_skt_t *skt);

tt_export void tt_skt_attr_default(IN tt_skt_attr_t *attr);

tt_export tt_result_t tt_skt_shutdown(IN tt_skt_t *skt, IN tt_skt_shut_t shut);

tt_export tt_result_t tt_skt_bind(IN tt_skt_t *skt, IN tt_sktaddr_t *addr);

tt_export tt_result_t tt_skt_bind_n(IN tt_skt_t *skt,
                                    IN tt_net_family_t family,
                                    IN tt_sktaddr_ip_t *ip,
                                    IN tt_u16_t port);

tt_export tt_result_t tt_skt_bind_p(IN tt_skt_t *skt,
                                    IN tt_net_family_t family,
                                    IN const tt_char_t *ip_str,
                                    IN tt_u16_t port);

tt_export tt_result_t tt_skt_listen(IN tt_skt_t *skt);

// this function does not guarantee that the new socket would inherite
// attributes of listening_skt, although os may do some about it). caller
// should explicitly set options by api defined in tt_socket_option.h
tt_export tt_skt_t *tt_skt_accept(IN tt_skt_t *skt,
                                  IN OPT tt_skt_attr_t *new_attr,
                                  OUT OPT tt_sktaddr_t *addr);

tt_export tt_result_t tt_skt_connect(IN tt_skt_t *skt, IN tt_sktaddr_t *addr);

tt_export tt_result_t tt_skt_connect_n(IN tt_skt_t *skt,
                                       IN tt_net_family_t family,
                                       IN tt_sktaddr_ip_t *ip,
                                       IN tt_u16_t port);

tt_export tt_result_t tt_skt_connect_p(IN tt_skt_t *skt,
                                       IN tt_net_family_t family,
                                       IN const tt_char_t *ip_str,
                                       IN tt_u16_t port);

/**
@fn tt_result_t tt_skt_send(IN tt_skt_t *skt,
                               OUT tt_u8_t *buf,
                               IN tt_u32_t len,
                               OUT tt_u32_t *send_len);
send data through socket

@param [in] skt socket
@param [out] buf data to be sent
@param [in] len size of buf
@param [out] send_len stores how many bytes are sent, can be NULL

@return
- TT_SUCCESS some content of buffer is sent
- TT_FAIL none is sent due to some error occur

@note
- the bytes returned by send_len may be less than len
- this function return TT_SUCCESS even if the operation is not completed, e.g.
  operation is interrupted, but send_len returns how many bytes are sent
  accurately.
- send_len return how many bytes read only if return value is TT_SUCCESS
*/
tt_inline tt_result_t tt_skt_send(IN tt_skt_t *skt,
                                  IN tt_u8_t *buf,
                                  IN tt_u32_t len,
                                  OUT OPT tt_u32_t *sent)
{
    if (len != 0) {
        return tt_skt_send_ntv(&skt->sys_skt, buf, len, sent);
    } else {
        *sent = 0;
        return TT_SUCCESS;
    }
}

tt_inline tt_result_t tt_skt_send_all(IN tt_skt_t *skt,
                                      IN tt_u8_t *buf,
                                      IN tt_u32_t len)
{
    tt_u32_t n = 0;
    while (n < len) {
        tt_u32_t sent;
        tt_result_t result;
        if (!TT_OK(result = tt_skt_send(skt, buf + n, len - n, &sent))) {
            return result;
        }
        n += sent;
    }
    return TT_SUCCESS;
}

tt_inline tt_result_t tt_skt_sendfile(IN tt_skt_t *skt, IN struct tt_file_s *f)
{
    return tt_skt_sendfile_ntv(&skt->sys_skt, f);
}

tt_export tt_result_t tt_skt_sendfile_path(IN tt_skt_t *skt,
                                           IN const tt_char_t *path);

tt_inline tt_result_t tt_skt_recv(IN tt_skt_t *skt,
                                  OUT tt_u8_t *buf,
                                  IN tt_u32_t len,
                                  OUT OPT tt_u32_t *recvd,
                                  OUT tt_fiber_ev_t **p_fev,
                                  OUT struct tt_tmr_s **p_tmr)
{
    if (len != 0) {
        return tt_skt_recv_ntv(&skt->sys_skt, buf, len, recvd, p_fev, p_tmr);
    } else {
        TT_ERROR("recv buf len can not be 0");
        return TT_FAIL;
    }
}

tt_inline tt_result_t tt_skt_recvfrom(IN tt_skt_t *skt,
                                      OUT tt_u8_t *buf,
                                      IN tt_u32_t len,
                                      OUT OPT tt_u32_t *recvd,
                                      OUT OPT tt_sktaddr_t *addr,
                                      OUT tt_fiber_ev_t **p_fev,
                                      OUT struct tt_tmr_s **p_tmr)
{
    if (len != 0) {
        return tt_skt_recvfrom_ntv(&skt->sys_skt,
                                   buf,
                                   len,
                                   recvd,
                                   addr,
                                   p_fev,
                                   p_tmr);
    } else {
        TT_ERROR("recvfrom buf len can not be 0");
        return TT_FAIL;
    }
}

tt_inline tt_result_t tt_skt_sendto(IN tt_skt_t *skt,
                                    IN tt_u8_t *buf,
                                    IN tt_u32_t len,
                                    OUT OPT tt_u32_t *sent,
                                    IN tt_sktaddr_t *addr)
{
    if (len != 0) {
        return tt_skt_sendto_ntv(&skt->sys_skt, buf, len, sent, addr);
    } else {
        *sent = 0;
        return TT_SUCCESS;
    }
}

tt_inline tt_result_t tt_skt_sendto_all(IN tt_skt_t *skt,
                                        IN tt_u8_t *buf,
                                        IN tt_u32_t len,
                                        IN tt_sktaddr_t *addr)
{
    tt_u32_t n = 0;
    while (n < len) {
        tt_u32_t sent;
        tt_result_t result;
        if (!TT_OK(result =
                       tt_skt_sendto(skt, buf + n, len - n, &sent, addr))) {
            return result;
        }
        n += sent;
    }
    return TT_SUCCESS;
}

tt_export tt_result_t tt_skt_local_addr(IN tt_skt_t *skt,
                                        IN tt_sktaddr_t *addr);

tt_export tt_result_t tt_skt_remote_addr(IN tt_skt_t *skt,
                                         IN tt_sktaddr_t *addr);

tt_export void tt_skt_stat_inc_num();

tt_export void tt_skt_stat_dec_num();

#if TT_ENV_OS_IS_WINDOWS
tt_inline HANDLE tt_skt_handle(IN tt_skt_t *skt)
{
    return skt->sys_skt.hf;
}
#else
tt_inline int tt_skt_fd(IN tt_skt_t *skt)
{
    return skt->sys_skt.s;
}
#endif

// ========================================
// multicast
// ========================================

// must bind to address to receive multicast packets
tt_export tt_result_t tt_skt_join_mcast(IN tt_skt_t *skt,
                                        IN tt_net_family_t family,
                                        IN tt_sktaddr_ip_t *ip,
                                        IN OPT const tt_char_t *itf);

tt_export tt_result_t tt_skt_leave_mcast(IN tt_skt_t *skt,
                                         IN tt_net_family_t family,
                                         IN tt_sktaddr_ip_t *ip,
                                         IN OPT const tt_char_t *itf);

// ========================================
// server
// ========================================

// - use tt_skt_destroy() to destroy created server socket
// - created socket could be passed to tt_skt_accept()
tt_export tt_skt_t *tt_tcp_server(IN tt_net_family_t family,
                                  IN OPT tt_skt_attr_t *attr,
                                  IN tt_sktaddr_t *addr);

tt_export tt_skt_t *tt_tcp_server_p(IN tt_net_family_t family,
                                    IN OPT tt_skt_attr_t *attr,
                                    IN const tt_char_t *ip_str,
                                    IN tt_u16_t port);

tt_export tt_skt_t *tt_udp_server(IN tt_net_family_t family,
                                  IN OPT tt_skt_attr_t *attr,
                                  IN tt_sktaddr_t *addr);

tt_export tt_skt_t *tt_udp_server_p(IN tt_net_family_t family,
                                    IN OPT tt_skt_attr_t *attr,
                                    IN const tt_char_t *ip_str,
                                    IN tt_u16_t port);

#endif // __TT_SOCKET__
