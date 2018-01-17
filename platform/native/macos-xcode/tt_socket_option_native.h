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
@file tt_socket_option.h
@brief get/set socket option

this file specifies apis to get or set socket option
*/

#ifndef __TT_SOCKET_OPTION_NATIVE__
#define __TT_SOCKET_OPTION_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_socket_native.h>
#include <tt_sys_error.h>

#include <fcntl.h>
#include <netinet/tcp.h>
#include <sys/socket.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_inline tt_result_t tt_skt_set_ipv6only_ntv(IN tt_skt_ntv_t *skt,
                                              IN tt_bool_t ipv6only)
{
    int val = ipv6only ? 1 : 0;
    if (setsockopt(skt->s, IPPROTO_IPV6, IPV6_V6ONLY, &val, sizeof(int)) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to set ipv6 only to %d", ipv6only);
        return TT_FAIL;
    }
}

tt_inline tt_result_t tt_skt_get_ipv6only_ntv(IN tt_skt_ntv_t *skt,
                                              OUT tt_bool_t *ipv6only)
{
    int val = 0;
    socklen_t len = (int)sizeof(int);
    if (getsockopt(skt->s, IPPROTO_IPV6, IPV6_V6ONLY, &val, &len) == 0) {
        *ipv6only = TT_BOOL(val);
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to get ipv6 only");
        return TT_FAIL;
    }
}

tt_inline tt_result_t tt_skt_set_reuseaddr_ntv(IN tt_skt_ntv_t *skt,
                                               IN tt_bool_t reuse_addr)
{
    int val = reuse_addr ? 1 : 0;
    if (setsockopt(skt->s, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(int)) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to set reuse addr to %d", reuse_addr);
        return TT_FAIL;
    }
}

tt_inline tt_result_t tt_skt_get_reuseaddr_ntv(IN tt_skt_ntv_t *skt,
                                               OUT tt_bool_t *reuse_addr)
{
    int val = 0;
    socklen_t len = (int)sizeof(int);
    if (getsockopt(skt->s, SOL_SOCKET, SO_REUSEADDR, &val, &len) == 0) {
        *reuse_addr = TT_BOOL(val);
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to get reuse addr");
        return TT_FAIL;
    }
}

tt_inline tt_result_t tt_skt_set_reuseport_ntv(IN tt_skt_ntv_t *skt,
                                               IN tt_bool_t reuse_port)
{
    int val = reuse_port ? 1 : 0;
    if (setsockopt(skt->s, SOL_SOCKET, SO_REUSEPORT, &val, sizeof(int)) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to set reuse port to %d", reuse_port);
        return TT_FAIL;
    }
}

tt_inline tt_result_t tt_skt_get_reuseport_ntv(IN tt_skt_ntv_t *skt,
                                               OUT tt_bool_t *reuse_port)
{
    int val = 0;
    socklen_t len = (int)sizeof(int);
    if (getsockopt(skt->s, SOL_SOCKET, SO_REUSEPORT, &val, &len) == 0) {
        *reuse_port = TT_BOOL(val);
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to get reuse port");
        return TT_FAIL;
    }
}

tt_inline tt_result_t tt_skt_set_tcp_nodelay_ntv(IN tt_skt_ntv_t *skt,
                                                 IN tt_bool_t nodelay)
{
    int val = nodelay ? 1 : 0;
    if (setsockopt(skt->s, IPPROTO_TCP, TCP_NODELAY, &val, sizeof(int)) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to set reuse addr to %d", nodelay);
        return TT_FAIL;
    }
}

tt_inline tt_result_t tt_skt_get_tcp_nodelay_ntv(IN tt_skt_ntv_t *skt,
                                                 OUT tt_bool_t *nodelay)
{
    int val = 0;
    socklen_t len = (int)sizeof(int);
    if (getsockopt(skt->s, IPPROTO_TCP, TCP_NODELAY, &val, &len) == 0) {
        *nodelay = TT_BOOL(val);
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to get reuse addr");
        return TT_FAIL;
    }
}

tt_inline tt_result_t tt_skt_set_nonblock_ntv(IN tt_skt_ntv_t *skt,
                                              IN tt_bool_t nonblock)
{
    long flags = fcntl(skt->s, F_GETFL, 0);

    if (flags == -1) {
        TT_ERROR_NTV("fail to get socket flags");
        return TT_FAIL;
    }

    if (nonblock) {
        flags |= O_NONBLOCK;
    } else {
        flags &= ~O_NONBLOCK;
    }

    if (fcntl(skt->s, F_SETFL, flags) == -1) {
        TT_ERROR_NTV("fail to set socket nonblock");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_inline tt_result_t tt_skt_set_linger_ntv(IN tt_skt_ntv_t *skt,
                                            IN tt_bool_t enable,
                                            IN tt_u16_t linger_sec)
{
    struct linger linger;
    linger.l_onoff = TT_COND(enable, 1, 0);
    linger.l_linger = linger_sec;
    if (setsockopt(skt->s,
                   SOL_SOCKET,
                   SO_LINGER,
                   &linger,
                   sizeof(struct linger)) == 0) {
        return TT_SUCCESS;
    } else {
        TT_NET_ERROR_NTV("fail to set linger");
        return TT_FAIL;
    }
}

extern tt_result_t tt_skt_set_mcast_loop_ntv(IN tt_skt_ntv_t *skt,
                                             IN tt_net_family_t family,
                                             IN tt_bool_t loop);

extern tt_result_t tt_skt_get_mcast_loop_ntv(IN tt_skt_ntv_t *skt,
                                             IN tt_net_family_t family,
                                             OUT tt_bool_t *loop);

extern tt_result_t tt_skt_set_mcast_ttl_ntv(IN tt_skt_ntv_t *skt,
                                            IN tt_net_family_t family,
                                            IN tt_u8_t ttl);

extern tt_result_t tt_skt_get_mcast_ttl_ntv(IN tt_skt_ntv_t *skt,
                                            IN tt_net_family_t family,
                                            OUT tt_u8_t *ttl);

extern tt_result_t tt_skt_set_mcast_if_ntv(IN tt_skt_ntv_t *skt,
                                           IN tt_sktaddr_ip_t *addr);

extern tt_result_t tt_skt_get_mcast_if_ntv(IN tt_skt_ntv_t *skt,
                                           OUT tt_sktaddr_ip_t *addr);

extern tt_result_t tt_skt_set_mcast_ifidx_ntv(IN tt_skt_ntv_t *skt,
                                              IN tt_u32_t ifidx);

extern tt_result_t tt_skt_get_mcast_ifidx_ntv(IN tt_skt_ntv_t *skt,
                                              OUT tt_u32_t *ifidx);

#endif // __TT_SOCKET_OPTION_NATIVE__
