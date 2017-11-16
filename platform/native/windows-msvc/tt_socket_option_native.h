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
@file tt_socket_option_native.h
@brief get/set socket option

this file specifies apis to get or set socket option
*/

#ifndef __TT_SOCKET_OPTION_NATIVE__
#define __TT_SOCKET_OPTION_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <misc/tt_util.h>

#include <tt_socket_native.h>
#include <tt_sys_error.h>

#include <ws2tcpip.h>

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
    DWORD val = ipv6only ? 1 : 0;
    if (setsockopt(skt->s,
                   IPPROTO_IPV6,
                   IPV6_V6ONLY,
                   (char *)&val,
                   (int)sizeof(DWORD)) == 0) {
        return TT_SUCCESS;
    } else {
        TT_NET_ERROR_NTV("fail to set ipv6 only to %d", ipv6only);
        return TT_FAIL;
    }
}

tt_inline tt_result_t tt_skt_get_ipv6only_ntv(IN tt_skt_ntv_t *skt,
                                              OUT tt_bool_t *ipv6only)
{
    DWORD val = 0;
    int len = (int)sizeof(DWORD);
    if (getsockopt(skt->s, IPPROTO_IPV6, IPV6_V6ONLY, (char *)&val, &len) ==
        0) {
        *ipv6only = TT_BOOL(val);
        return TT_SUCCESS;
    } else {
        TT_NET_ERROR_NTV("fail to get ipv6 only");
        return TT_FAIL;
    }
}

tt_inline tt_result_t tt_skt_set_reuseaddr_ntv(IN tt_skt_ntv_t *skt,
                                               IN tt_bool_t reuse_addr)
{
    BOOL val = reuse_addr ? TRUE : FALSE;
    if (setsockopt(skt->s,
                   SOL_SOCKET,
                   SO_REUSEADDR,
                   (char *)&val,
                   (int)sizeof(BOOL)) == 0) {
        return TT_SUCCESS;
    } else {
        TT_NET_ERROR_NTV("fail to set reuse addr to %d", reuse_addr);
        return TT_FAIL;
    }
}

tt_inline tt_result_t tt_skt_get_reuseaddr_ntv(IN tt_skt_ntv_t *skt,
                                               OUT tt_bool_t *reuse_addr)
{
    BOOL val = 0;
    int len = (int)sizeof(BOOL);
    if (getsockopt(skt->s, SOL_SOCKET, SO_REUSEADDR, (char *)&val, &len) == 0) {
        *reuse_addr = TT_BOOL(val);
        return TT_SUCCESS;
    } else {
        TT_NET_ERROR_NTV("fail to get reuse addr");
        return TT_FAIL;
    }
}

tt_inline tt_result_t tt_skt_set_reuseport_ntv(IN tt_skt_ntv_t *skt,
                                               IN tt_bool_t reuse_port)
{
    // on windows, enabling SO_REUSEADDR implies reuse port
    // care windows has another option to disable reusing
    return TT_SUCCESS;
}

tt_inline tt_result_t tt_skt_get_reuseport_ntv(IN tt_skt_ntv_t *skt,
                                               OUT tt_bool_t *reuse_port)
{
    *reuse_port = TT_TRUE;
    return TT_SUCCESS;
}

tt_inline tt_result_t tt_skt_set_tcp_nodelay_ntv(IN tt_skt_ntv_t *skt,
                                                 IN tt_bool_t nodelay)
{
    BOOL val = nodelay ? TRUE : FALSE;
    if (setsockopt(skt->s,
                   IPPROTO_TCP,
                   TCP_NODELAY,
                   (char *)&val,
                   (int)sizeof(int)) == 0) {
        return TT_SUCCESS;
    } else {
        TT_NET_ERROR_NTV("fail to set reuse addr to %d", nodelay);
        return TT_FAIL;
    }
}

tt_inline tt_result_t tt_skt_get_tcp_nodelay_ntv(IN tt_skt_ntv_t *skt,
                                                 OUT tt_bool_t *nodelay)
{
    BOOL val = 0;
    int len = (int)sizeof(BOOL);
    if (getsockopt(skt->s, IPPROTO_TCP, TCP_NODELAY, (char *)&val, &len) == 0) {
        *nodelay = TT_BOOL(val);
        return TT_SUCCESS;
    } else {
        TT_NET_ERROR_NTV("fail to get reuse addr");
        return TT_FAIL;
    }
}

tt_inline tt_result_t tt_skt_set_nonblock_ntv(IN tt_skt_ntv_t *skt,
                                              IN tt_bool_t nonblock)
{
    u_long val = nonblock ? 1 : 0;
    if (ioctlsocket(skt->s, FIONBIO, &val) == 0) {
        return TT_SUCCESS;
    } else {
        TT_NET_ERROR_NTV("fail to set nonblock to %d", nonblock);
        return TT_FAIL;
    }
}

tt_inline tt_result_t tt_skt_set_linger_ntv(IN tt_skt_ntv_t *skt,
                                            IN tt_bool_t enable,
                                            IN tt_u16_t linger_sec)
{
    LINGER linger;
    linger.l_onoff = TT_COND(enable, 1, 0);
    linger.l_linger = linger_sec;
    if (setsockopt(skt->s,
                   SOL_SOCKET,
                   SO_LINGER,
                   (char *)&linger,
                   (int)sizeof(LINGER)) == 0) {
        return TT_SUCCESS;
    } else {
        TT_NET_ERROR_NTV("fail to set linger");
        return TT_FAIL;
    }
}

#endif // __TT_SOCKET_OPTION_NATIVE__
