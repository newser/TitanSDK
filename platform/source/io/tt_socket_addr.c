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

#include <io/tt_socket_addr.h>

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

tt_result_t tt_sktaddr_set_addr_p(IN tt_sktaddr_t *addr, IN tt_char_t *p_addr)
{
    tt_net_family_t af;
    tt_sktaddr_addr_t __n;

    af = tt_sktaddr_get_family(addr);
    if (!TT_NET_AF_VALID(af)) {
        TT_ERROR("invalid family: %d", af);
        return TT_FAIL;
    }

    if (p_addr == TT_SKTADDR_ANY) {
        tt_sktaddr_set_addr_n(addr, TT_SKTADDR_ANY);
        return TT_SUCCESS;
    }

    if (!TT_OK(tt_sktaddr_addr_p2n(af, p_addr, &__n))) {
        return TT_FAIL;
    }
    tt_sktaddr_set_addr_n(addr, &__n);
    return TT_SUCCESS;
}

tt_result_t tt_sktaddr_get_addr_p(IN tt_sktaddr_t *addr,
                                  OUT tt_char_t *p_addr,
                                  IN tt_u32_t p_addr_len)
{
    tt_net_family_t af;
    tt_sktaddr_addr_t __n;

    af = tt_sktaddr_get_family(addr);
    if (!TT_NET_AF_VALID(af)) {
        TT_ERROR("invalid family: %d", af);
        return TT_FAIL;
    }

    tt_sktaddr_get_addr_n(addr, &__n);
    return tt_sktaddr_addr_n2p(af, &__n, p_addr, p_addr_len);
}

tt_sktaddr_t *tt_sktaddr_map4to6(IN tt_sktaddr_t *in4, OUT tt_sktaddr_t *in6)
{
    if (tt_sktaddr_get_family(in4) == TT_NET_AF_INET) {
        tt_sktaddr_map4to6_ntv(in4, in6);
        return in6;
    } else {
        return in4;
    }
}

tt_sktaddr_t *tt_sktaddr_ipv4map(IN tt_net_family_t skt_af,
                                 IN tt_sktaddr_t *addr,
                                 IN tt_sktaddr_t *ipv4mapped)
{
    if ((skt_af == TT_NET_AF_INET6) &&
        (tt_sktaddr_get_family(addr) == TT_NET_AF_INET)) {
        tt_sktaddr_map4to6(addr, ipv4mapped);
        return ipv4mapped;
    } else {
        return addr;
    }
}
