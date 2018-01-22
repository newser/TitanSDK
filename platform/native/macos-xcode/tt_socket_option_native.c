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

#include <tt_socket_option_native.h>

#include <log/tt_log.h>
#include <misc/tt_assert.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_skt_set_ttl_ntv(IN tt_skt_ntv_t *skt,
                               IN tt_net_family_t family,
                               IN tt_u8_t ttl)
{
    int val = ttl;
    if (setsockopt(skt->s,
                   TT_COND(family == TT_NET_AF_INET, IPPROTO_IP, IPPROTO_IPV6),
                   TT_COND(family == TT_NET_AF_INET, IP_TTL, IPV6_UNICAST_HOPS),
                   &val,
                   sizeof(val)) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to set ttl to %d", ttl);
        return TT_FAIL;
    }
}

tt_result_t tt_skt_get_ttl_ntv(IN tt_skt_ntv_t *skt,
                               IN tt_net_family_t family,
                               OUT tt_u8_t *ttl)
{
    int val;
    socklen_t len = sizeof(val);
    if (getsockopt(skt->s,
                   TT_COND(family == TT_NET_AF_INET, IPPROTO_IP, IPPROTO_IPV6),
                   TT_COND(family == TT_NET_AF_INET, IP_TTL, IPV6_UNICAST_HOPS),
                   &val,
                   &len) == 0) {
        *ttl = (tt_u8_t)val;
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to get ttl");
        return TT_FAIL;
    }
}

tt_result_t tt_skt_set_broadcast_ntv(IN tt_skt_ntv_t *skt,
                                     IN tt_bool_t broadcast)
{
    int val = broadcast ? 1 : 0;
    if (setsockopt(skt->s, SOL_SOCKET, SO_BROADCAST, &val, sizeof(int)) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to set broadcast to %d", broadcast);
        return TT_FAIL;
    }
}

tt_result_t tt_skt_get_broadcast_ntv(IN tt_skt_ntv_t *skt,
                                     OUT tt_bool_t *broadcast)
{
    int val;
    socklen_t len = sizeof(val);
    if (getsockopt(skt->s, SOL_SOCKET, SO_BROADCAST, &val, &len) == 0) {
        *broadcast = TT_BOOL(val);
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to get broadcast");
        return TT_FAIL;
    }
}

tt_result_t tt_skt_set_oobinline_ntv(IN tt_skt_ntv_t *skt,
                                     IN tt_bool_t oobinline)
{
    int val = oobinline ? 1 : 0;
    if (setsockopt(skt->s, SOL_SOCKET, SO_OOBINLINE, &val, sizeof(int)) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to set oobinline to %d", oobinline);
        return TT_FAIL;
    }
}

tt_result_t tt_skt_get_oobinline_ntv(IN tt_skt_ntv_t *skt,
                                     OUT tt_bool_t *oobinline)
{
    int val;
    socklen_t len = sizeof(val);
    if (getsockopt(skt->s, SOL_SOCKET, SO_OOBINLINE, &val, &len) == 0) {
        *oobinline = TT_BOOL(val);
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to get oobinline");
        return TT_FAIL;
    }
}

tt_result_t tt_skt_set_sendtime_ntv(IN tt_skt_ntv_t *skt, IN tt_u32_t ms)
{
    struct timeval val = {ms / 1000, (ms % 1000) * 1000};
    if (setsockopt(skt->s, SOL_SOCKET, SO_SNDTIMEO, &val, sizeof(val)) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to set send time out to %d", ms);
        return TT_FAIL;
    }
}

tt_result_t tt_skt_get_sendtime_ntv(IN tt_skt_ntv_t *skt, OUT tt_u32_t *ms)
{
    struct timeval val;
    socklen_t len = sizeof(val);
    if (getsockopt(skt->s, SOL_SOCKET, SO_SNDTIMEO, &val, &len) == 0) {
        *ms = val.tv_sec * 1000 + val.tv_usec / 1000;
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to get send time out");
        return TT_FAIL;
    }
}

tt_result_t tt_skt_set_recvtime_ntv(IN tt_skt_ntv_t *skt, IN tt_u32_t ms)
{
    struct timeval val = {ms / 1000, (ms % 1000) * 1000};
    if (setsockopt(skt->s, SOL_SOCKET, SO_RCVTIMEO, &val, sizeof(val)) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to set recv time out to %d", ms);
        return TT_FAIL;
    }
}

tt_result_t tt_skt_get_recvtime_ntv(IN tt_skt_ntv_t *skt, OUT tt_u32_t *ms)
{
    struct timeval val;
    socklen_t len = sizeof(val);
    if (getsockopt(skt->s, SOL_SOCKET, SO_RCVTIMEO, &val, &len) == 0) {
        *ms = val.tv_sec * 1000 + val.tv_usec / 1000;
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to get recv time out");
        return TT_FAIL;
    }
}

tt_result_t tt_skt_set_sendbuf_ntv(IN tt_skt_ntv_t *skt, IN tt_u32_t size)
{
    int val = size;
    if (setsockopt(skt->s, SOL_SOCKET, SO_SNDBUF, &val, sizeof(int)) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to set send buf size to %d", size);
        return TT_FAIL;
    }
}

tt_result_t tt_skt_get_sendbuf_ntv(IN tt_skt_ntv_t *skt, OUT tt_u32_t *size)
{
    int val;
    socklen_t len = sizeof(val);
    if (getsockopt(skt->s, SOL_SOCKET, SO_SNDBUF, &val, &len) == 0) {
        *size = (tt_u32_t)val;
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to get send buf size");
        return TT_FAIL;
    }
}

tt_result_t tt_skt_set_recvbuf_ntv(IN tt_skt_ntv_t *skt, IN tt_u32_t size)
{
    int val = size;
    if (setsockopt(skt->s, SOL_SOCKET, SO_RCVBUF, &val, sizeof(int)) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to set recv buf size to %d", size);
        return TT_FAIL;
    }
}

tt_result_t tt_skt_get_recvbuf_ntv(IN tt_skt_ntv_t *skt, OUT tt_u32_t *size)
{
    int val;
    socklen_t len = sizeof(val);
    if (getsockopt(skt->s, SOL_SOCKET, SO_RCVBUF, &val, &len) == 0) {
        *size = (tt_u32_t)val;
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to get recv buf size");
        return TT_FAIL;
    }
}

tt_result_t tt_skt_set_mcast_loop_ntv(IN tt_skt_ntv_t *skt,
                                      IN tt_net_family_t family,
                                      IN tt_bool_t loop)
{
    int val = loop ? 1 : 0;
    if (setsockopt(skt->s,
                   TT_COND(family == TT_NET_AF_INET, IPPROTO_IP, IPPROTO_IPV6),
                   TT_COND(family == TT_NET_AF_INET,
                           IP_MULTICAST_LOOP,
                           IPV6_MULTICAST_LOOP),
                   &val,
                   sizeof(val)) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to set mcast loop to %d", loop);
        return TT_FAIL;
    }
}

tt_result_t tt_skt_get_mcast_loop_ntv(IN tt_skt_ntv_t *skt,
                                      IN tt_net_family_t family,
                                      OUT tt_bool_t *loop)
{
    int val;
    socklen_t len = sizeof(val);
    if (getsockopt(skt->s,
                   TT_COND(family == TT_NET_AF_INET, IPPROTO_IP, IPPROTO_IPV6),
                   TT_COND(family == TT_NET_AF_INET,
                           IP_MULTICAST_LOOP,
                           IPV6_MULTICAST_LOOP),
                   &val,
                   &len) == 0) {
        *loop = TT_BOOL(val);
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to get mcast loop");
        return TT_FAIL;
    }
}

tt_result_t tt_skt_set_mcast_ttl_ntv(IN tt_skt_ntv_t *skt,
                                     IN tt_net_family_t family,
                                     IN tt_u8_t ttl)
{
    int val = ttl;
    if (setsockopt(skt->s,
                   TT_COND(family == TT_NET_AF_INET, IPPROTO_IP, IPPROTO_IPV6),
                   TT_COND(family == TT_NET_AF_INET,
                           IP_MULTICAST_TTL,
                           IPV6_MULTICAST_HOPS),
                   &val,
                   sizeof(val)) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to set mcast ttl to %d", ttl);
        return TT_FAIL;
    }
}

tt_result_t tt_skt_get_mcast_ttl_ntv(IN tt_skt_ntv_t *skt,
                                     IN tt_net_family_t family,
                                     OUT tt_u8_t *ttl)
{
    int val;
    socklen_t len = sizeof(val);
    if (getsockopt(skt->s,
                   TT_COND(family == TT_NET_AF_INET, IPPROTO_IP, IPPROTO_IPV6),
                   TT_COND(family == TT_NET_AF_INET,
                           IP_MULTICAST_TTL,
                           IPV6_MULTICAST_HOPS),
                   &val,
                   &len) == 0) {
        *ttl = (tt_u8_t)val;
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to get mcast ttl");
        return TT_FAIL;
    }
}

tt_result_t tt_skt_set_mcast_if_ntv(IN tt_skt_ntv_t *skt,
                                    IN tt_sktaddr_ip_t *addr)
{
    struct in_addr a;
    a.s_addr = addr->a32.__u32;

    if (setsockopt(skt->s, IPPROTO_IP, IP_MULTICAST_IF, &a, sizeof(a)) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to set mcast interface");
        return TT_FAIL;
    }
}

tt_result_t tt_skt_get_mcast_if_ntv(IN tt_skt_ntv_t *skt,
                                    OUT tt_sktaddr_ip_t *addr)
{
    struct in_addr a;
    socklen_t len = sizeof(a);
    if (getsockopt(skt->s, IPPROTO_IP, IP_MULTICAST_IF, &a, &len) == 0) {
        addr->a32.__u32 = a.s_addr;
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to get mcast interface");
        return TT_FAIL;
    }
}

tt_result_t tt_skt_set_mcast_ifidx_ntv(IN tt_skt_ntv_t *skt, IN tt_u32_t ifidx)
{
    int val = (int)ifidx;
    if (setsockopt(skt->s,
                   IPPROTO_IPV6,
                   IPV6_MULTICAST_IF,
                   &val,
                   sizeof(val)) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to set mcast interface idx");
        return TT_FAIL;
    }
}

tt_result_t tt_skt_get_mcast_ifidx_ntv(IN tt_skt_ntv_t *skt,
                                       OUT tt_u32_t *ifidx)
{
    int val;
    socklen_t len = sizeof(val);
    if (getsockopt(skt->s, IPPROTO_IPV6, IPV6_MULTICAST_IF, &val, &len) == 0) {
        *ifidx = val;
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to get mcast interface idx");
        return TT_FAIL;
    }
}
