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

#include <io/tt_socket_option.h>

#include <io/tt_socket.h>
#include <misc/tt_assert.h>

#include <tt_socket_option_native.h>

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

tt_result_t tt_skt_set_ipv6only(IN tt_skt_t *skt, IN tt_bool_t ipv6only)
{
    TT_ASSERT(skt != NULL);

    return tt_skt_set_ipv6only_ntv(&skt->sys_skt, ipv6only);
}

tt_result_t tt_skt_get_ipv6only(IN tt_skt_t *skt, OUT tt_bool_t *ipv6only)
{
    TT_ASSERT(skt != NULL);
    TT_ASSERT(ipv6only != NULL);

    return tt_skt_get_ipv6only_ntv(&skt->sys_skt, ipv6only);
}

tt_result_t tt_skt_set_reuseaddr(IN tt_skt_t *skt, IN tt_bool_t reuse_addr)
{
    TT_ASSERT(skt != NULL);

    return tt_skt_set_reuseaddr_ntv(&skt->sys_skt, reuse_addr);
}

tt_result_t tt_skt_get_reuseaddr(IN tt_skt_t *skt, OUT tt_bool_t *reuse_addr)
{
    TT_ASSERT(skt != NULL);
    TT_ASSERT(reuse_addr != NULL);

    return tt_skt_get_reuseaddr_ntv(&skt->sys_skt, reuse_addr);
}

tt_result_t tt_skt_set_reuseport(IN tt_skt_t *skt, IN tt_bool_t reuse_port)
{
    TT_ASSERT(skt != NULL);

    return tt_skt_set_reuseport_ntv(&skt->sys_skt, reuse_port);
}

tt_result_t tt_skt_get_reuseport(IN tt_skt_t *skt, OUT tt_bool_t *reuse_port)
{
    TT_ASSERT(skt != NULL);
    TT_ASSERT(reuse_port != NULL);

    return tt_skt_get_reuseport_ntv(&skt->sys_skt, reuse_port);
}

tt_result_t tt_skt_set_nodelay(IN tt_skt_t *skt, IN tt_bool_t nodelay)
{
    TT_ASSERT(skt != NULL);

    return tt_skt_set_nodelay_ntv(&skt->sys_skt, nodelay);
}

tt_result_t tt_skt_get_nodelay(IN tt_skt_t *skt, OUT tt_bool_t *nodelay)
{
    TT_ASSERT(skt != NULL);
    TT_ASSERT(nodelay != NULL);

    return tt_skt_get_nodelay_ntv(&skt->sys_skt, nodelay);
}

tt_result_t tt_skt_set_nonblock(IN tt_skt_t *skt, IN tt_bool_t nonblock)
{
    TT_ASSERT(skt != NULL);

    return tt_skt_set_nonblock_ntv(&skt->sys_skt, nonblock);
}

tt_result_t tt_skt_set_linger(IN tt_skt_t *skt, IN tt_bool_t enable,
                              IN tt_u16_t linger_sec)
{
    TT_ASSERT(skt != NULL);

    return tt_skt_set_linger_ntv(&skt->sys_skt, enable, linger_sec);
}

tt_result_t tt_skt_get_linger(IN tt_skt_t *skt, OUT tt_bool_t *enable,
                              OUT tt_u16_t *linger_sec)
{
    TT_ASSERT(skt != NULL);

    return tt_skt_get_linger_ntv(&skt->sys_skt, enable, linger_sec);
}

tt_result_t tt_skt_set_keepalive(IN tt_skt_t *skt, IN tt_bool_t keepalive)
{
    TT_ASSERT(skt != NULL);

    return tt_skt_set_keepalive_ntv(&skt->sys_skt, keepalive);
}

tt_result_t tt_skt_get_keepalive(IN tt_skt_t *skt, OUT tt_bool_t *keepalive)
{
    TT_ASSERT(skt != NULL);
    TT_ASSERT(keepalive != NULL);

    return tt_skt_get_keepalive_ntv(&skt->sys_skt, keepalive);
}

tt_result_t tt_skt_set_ttl(IN tt_skt_t *skt, IN tt_net_family_t family,
                           IN tt_u8_t ttl)
{
    TT_ASSERT(skt != NULL);
    TT_ASSERT(TT_NET_AF_VALID(family));

    return tt_skt_set_ttl_ntv(&skt->sys_skt, family, ttl);
}

tt_result_t tt_skt_get_ttl(IN tt_skt_t *skt, IN tt_net_family_t family,
                           OUT tt_u8_t *ttl)
{
    TT_ASSERT(skt != NULL);
    TT_ASSERT(TT_NET_AF_VALID(family));

    return tt_skt_get_ttl_ntv(&skt->sys_skt, family, ttl);
}

tt_result_t tt_skt_set_broadcast(IN tt_skt_t *skt, IN tt_bool_t broadcast)
{
    TT_ASSERT(skt != NULL);

    return tt_skt_set_broadcast_ntv(&skt->sys_skt, broadcast);
}

tt_result_t tt_skt_get_broadcast(IN tt_skt_t *skt, OUT tt_bool_t *broadcast)
{
    TT_ASSERT(skt != NULL);

    return tt_skt_get_broadcast_ntv(&skt->sys_skt, broadcast);
}

tt_result_t tt_skt_set_oobinline(IN tt_skt_t *skt, IN tt_bool_t oobinline)
{
    TT_ASSERT(skt != NULL);

    return tt_skt_set_oobinline_ntv(&skt->sys_skt, oobinline);
}

tt_result_t tt_skt_get_oobinline(IN tt_skt_t *skt, OUT tt_bool_t *oobinline)
{
    TT_ASSERT(skt != NULL);

    return tt_skt_get_oobinline_ntv(&skt->sys_skt, oobinline);
}

tt_result_t tt_skt_set_sendbuf(IN tt_skt_t *skt, IN tt_u32_t size)
{
    TT_ASSERT(skt != NULL);

    return tt_skt_set_sendbuf_ntv(&skt->sys_skt, size);
}

tt_result_t tt_skt_get_sendbuf(IN tt_skt_t *skt, OUT tt_u32_t *size)
{
    TT_ASSERT(skt != NULL);
    TT_ASSERT(size != NULL);

    return tt_skt_get_sendbuf_ntv(&skt->sys_skt, size);
}

tt_result_t tt_skt_set_recvbuf(IN tt_skt_t *skt, IN tt_u32_t size)
{
    TT_ASSERT(skt != NULL);

    return tt_skt_set_recvbuf_ntv(&skt->sys_skt, size);
}

tt_result_t tt_skt_get_recvbuf(IN tt_skt_t *skt, OUT tt_u32_t *size)
{
    TT_ASSERT(skt != NULL);

    return tt_skt_get_recvbuf_ntv(&skt->sys_skt, size);
}

tt_result_t tt_skt_set_sendtime(IN tt_skt_t *skt, IN tt_u32_t ms)
{
    TT_ASSERT(skt != NULL);

    return tt_skt_set_sendtime_ntv(&skt->sys_skt, ms);
}

tt_result_t tt_skt_get_sendtime(IN tt_skt_t *skt, OUT tt_u32_t *ms)
{
    TT_ASSERT(skt != NULL);
    TT_ASSERT(ms != NULL);

    return tt_skt_get_sendtime_ntv(&skt->sys_skt, ms);
}

tt_result_t tt_skt_set_recvtime(IN tt_skt_t *skt, IN tt_u32_t ms)
{
    TT_ASSERT(skt != NULL);

    return tt_skt_set_recvtime_ntv(&skt->sys_skt, ms);
}

tt_result_t tt_skt_get_recvtime(IN tt_skt_t *skt, OUT tt_u32_t *ms)
{
    TT_ASSERT(skt != NULL);
    TT_ASSERT(ms != NULL);

    return tt_skt_get_recvtime_ntv(&skt->sys_skt, ms);
}

tt_result_t tt_skt_set_mcast_loop(IN tt_skt_t *skt, IN tt_net_family_t family,
                                  IN tt_bool_t loop)
{
    TT_ASSERT(skt != NULL);
    TT_ASSERT(TT_NET_AF_VALID(family));

    return tt_skt_set_mcast_loop_ntv(&skt->sys_skt, family, loop);
}

tt_result_t tt_skt_get_mcast_loop(IN tt_skt_t *skt, IN tt_net_family_t family,
                                  OUT tt_bool_t *loop)
{
    TT_ASSERT(skt != NULL);
    TT_ASSERT(TT_NET_AF_VALID(family));

    return tt_skt_get_mcast_loop_ntv(&skt->sys_skt, family, loop);
}

tt_result_t tt_skt_set_mcast_ttl(IN tt_skt_t *skt, IN tt_net_family_t family,
                                 IN tt_u8_t ttl)
{
    TT_ASSERT(skt != NULL);
    TT_ASSERT(TT_NET_AF_VALID(family));

    return tt_skt_set_mcast_ttl_ntv(&skt->sys_skt, family, ttl);
}

tt_result_t tt_skt_get_mcast_ttl(IN tt_skt_t *skt, IN tt_net_family_t family,
                                 OUT tt_u8_t *ttl)
{
    TT_ASSERT(skt != NULL);
    TT_ASSERT(TT_NET_AF_VALID(family));

    return tt_skt_get_mcast_ttl_ntv(&skt->sys_skt, family, ttl);
}

tt_result_t tt_skt_set_mcast_if(IN tt_skt_t *skt, IN tt_sktaddr_ip_t *addr)
{
    TT_ASSERT(skt != NULL);

    return tt_skt_set_mcast_if_ntv(&skt->sys_skt, addr);
}

tt_result_t tt_skt_get_mcast_if(IN tt_skt_t *skt, OUT tt_sktaddr_ip_t *addr)

{
    TT_ASSERT(skt != NULL);

    return tt_skt_get_mcast_if_ntv(&skt->sys_skt, addr);
}

tt_result_t tt_skt_set_mcast_ifidx(IN tt_skt_t *skt, IN tt_u32_t ifidx)
{
    TT_ASSERT(skt != NULL);

    return tt_skt_set_mcast_ifidx_ntv(&skt->sys_skt, ifidx);
}

tt_result_t tt_skt_get_mcast_ifidx(IN tt_skt_t *skt, OUT tt_u32_t *ifidx)
{
    TT_ASSERT(skt != NULL);
    TT_ASSERT(ifidx != NULL);

    return tt_skt_get_mcast_ifidx_ntv(&skt->sys_skt, ifidx);
}
