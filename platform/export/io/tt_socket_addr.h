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
@file tt_socket_addr.h
@brief socket address definitions

this file specifies socket address definitions
*/

#ifndef __TT_SOCKET_ADDR__
#define __TT_SOCKET_ADDR__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <io/tt_network_io_def.h>
#include <misc/tt_assert.h>

#include <tt_socket_addr_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_SKTADDR_ANY NULL

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef tt_sktaddr_ntv_t tt_sktaddr_t;

typedef union
{
    tt_u8_t __u8[4];
    tt_u16_t __u16[2];
    tt_u32_t __u32;
} tt_sktaddr_addr32_t;

typedef union
{
    tt_u8_t __u8[16];
    tt_u16_t __u16[8];
    tt_u32_t __u32[4];
} tt_sktaddr_addr128_t;

typedef union tt_sktaddr_addr_s
{
    tt_sktaddr_addr32_t a32;
    tt_sktaddr_addr128_t a128;
} tt_sktaddr_addr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_inline void tt_sktaddr_init(IN tt_sktaddr_t *addr, IN tt_net_family_t family)
{
    TT_ASSERT(TT_NET_AF_VALID(family));
    tt_sktaddr_init_ntv(addr, family);
}

tt_inline tt_net_family_t tt_sktaddr_get_family(IN tt_sktaddr_t *addr)
{
    return tt_sktaddr_get_family_ntv(addr);
}

// addr_val can be TT_SKTADDR_ANY
tt_inline void tt_sktaddr_set_addr_n(IN tt_sktaddr_t *addr,
                                     IN tt_sktaddr_addr_t *na)
{
    tt_sktaddr_set_addr_n_ntv(addr, na);
}

tt_inline void tt_sktaddr_get_addr_n(IN tt_sktaddr_t *addr,
                                     OUT tt_sktaddr_addr_t *na)
{
    tt_sktaddr_get_addr_n_ntv(addr, na);
}

// addr_val can be TT_SKTADDR_ANY
extern tt_result_t tt_sktaddr_set_addr_p(IN tt_sktaddr_t *addr,
                                         IN tt_char_t *pa);

extern tt_result_t tt_sktaddr_get_addr_p(IN tt_sktaddr_t *addr,
                                         OUT tt_char_t *pa,
                                         IN tt_u32_t pa_len);

tt_inline void tt_sktaddr_set_port(IN tt_sktaddr_t *addr, IN tt_u16_t port)
{
    tt_sktaddr_set_port_ntv(addr, port);
}

tt_inline tt_u16_t tt_sktaddr_get_port(IN tt_sktaddr_t *addr)
{
    return tt_sktaddr_get_port_ntv(addr);
}

tt_inline void tt_sktaddr_set_scope(IN tt_sktaddr_t *addr, IN tt_u32_t scope_id)
{
    tt_sktaddr_set_scope_ntv(addr, scope_id);
}

tt_inline void tt_sktaddr_set_scope_p(IN tt_sktaddr_t *addr,
                                      IN tt_char_t *scope_name)
{
    tt_sktaddr_set_scope_p_ntv(addr, scope_name);
}

tt_inline tt_result_t tt_sktaddr_addr_n2p(IN tt_net_family_t family,
                                          IN tt_sktaddr_addr_t *addr_val,
                                          OUT tt_char_t *buf,
                                          IN tt_u32_t buf_len)
{
    return tt_sktaddr_addr_n2p_ntv(family, addr_val, buf, buf_len);
}

tt_inline tt_result_t tt_sktaddr_addr_p2n(IN tt_net_family_t family,
                                          IN tt_char_t *buf,
                                          OUT tt_sktaddr_addr_t *addr_val)
{
    return tt_sktaddr_addr_p2n_ntv(family, buf, addr_val);
}

tt_inline void tt_sktaddr_init_any(IN tt_sktaddr_t *addr,
                                   IN tt_net_family_t family)
{
    tt_sktaddr_init_ntv(addr, family);
    tt_sktaddr_set_addr_n(addr, TT_SKTADDR_ANY);
    tt_sktaddr_set_port(addr, 0);
}

// convert ipv4 address to ipv4-mapped ipv6 address
// - in4 and in6 can be same value
// - return mapped address if in4 and in6 are different
extern tt_sktaddr_t *tt_sktaddr_map4to6(IN tt_sktaddr_t *in4,
                                        IN tt_sktaddr_t *in6);

tt_inline tt_bool_t tt_sktaddr_ipv4mapped(IN tt_sktaddr_t *addr)
{
    return tt_sktaddr_ipv4mapped_ntv(addr);
}

#endif // __TT_SOCKET_ADDR__
