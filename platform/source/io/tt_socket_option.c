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

tt_result_t tt_skt_set_ipv6only(IN struct tt_skt_s *skt, IN tt_bool_t ipv6only)
{
    TT_ASSERT(skt != NULL);

    return tt_skt_set_ipv6only_ntv(&skt->sys_skt, ipv6only);
}

tt_result_t tt_skt_get_ipv6only(IN struct tt_skt_s *skt,
                                OUT tt_bool_t *ipv6only)
{
    TT_ASSERT(skt != NULL);
    TT_ASSERT(ipv6only != NULL);

    return tt_skt_get_ipv6only_ntv(&skt->sys_skt, ipv6only);
}

tt_result_t tt_skt_set_reuseaddr(IN struct tt_skt_s *skt,
                                 IN tt_bool_t reuse_addr)
{
    TT_ASSERT(skt != NULL);

    return tt_skt_set_reuseaddr_ntv(&skt->sys_skt, reuse_addr);
}

tt_result_t tt_skt_get_reuseaddr(IN struct tt_skt_s *skt,
                                 OUT tt_bool_t *reuse_addr)
{
    TT_ASSERT(skt != NULL);
    TT_ASSERT(reuse_addr != NULL);

    return tt_skt_get_reuseaddr_ntv(&skt->sys_skt, reuse_addr);
}

tt_result_t tt_skt_set_reuseport(IN struct tt_skt_s *skt,
                                 IN tt_bool_t reuse_port)
{
    TT_ASSERT(skt != NULL);

    return tt_skt_set_reuseport_ntv(&skt->sys_skt, reuse_port);
}

tt_result_t tt_skt_get_reuseport(IN struct tt_skt_s *skt,
                                 OUT tt_bool_t *reuse_port)
{
    TT_ASSERT(skt != NULL);
    TT_ASSERT(reuse_port != NULL);

    return tt_skt_get_reuseport_ntv(&skt->sys_skt, reuse_port);
}

tt_result_t tt_skt_set_tcp_nodelay(IN struct tt_skt_s *skt,
                                   IN tt_bool_t nodelay)
{
    TT_ASSERT(skt != NULL);

    return tt_skt_set_tcp_nodelay_ntv(&skt->sys_skt, nodelay);
}

tt_result_t tt_skt_get_tcp_nodelay(IN struct tt_skt_s *skt,
                                   OUT tt_bool_t *nodelay)
{
    TT_ASSERT(skt != NULL);
    TT_ASSERT(nodelay != NULL);

    return tt_skt_get_tcp_nodelay_ntv(&skt->sys_skt, nodelay);
}

tt_result_t tt_skt_set_nonblock(IN struct tt_skt_s *skt, IN tt_bool_t nonblock)
{
    TT_ASSERT(skt != NULL);

    return tt_skt_set_nonblock_ntv(&skt->sys_skt, nonblock);
}

tt_result_t tt_skt_set_linger(IN struct tt_skt_s *skt,
                              IN tt_bool_t enable,
                              IN tt_u16_t linger_sec)
{
    TT_ASSERT(skt != NULL);

    return tt_skt_set_linger_ntv(&skt->sys_skt, enable, linger_sec);
}
