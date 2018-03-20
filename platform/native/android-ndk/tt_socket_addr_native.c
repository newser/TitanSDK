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

#include <tt_socket_addr_native.h>

#include <io/tt_socket_addr.h>

#include <tt_cstd_api.h>
#include <tt_sys_error.h>

#include <net/if.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define TT_ASSERT_SA TT_ASSERT

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

void tt_sktaddr_init_ntv(IN tt_sktaddr_ntv_t *addr, IN tt_net_family_t family)
{
    tt_memset(addr, 0, sizeof(tt_sktaddr_ntv_t));
    if (family == TT_NET_AF_INET) {
        addr->ss_family = AF_INET;
    } else {
        TT_ASSERT_SA(family == TT_NET_AF_INET6);
        addr->ss_family = AF_INET6;
    }
}

tt_net_family_t tt_sktaddr_get_family_ntv(IN tt_sktaddr_ntv_t *addr)
{
    if (addr->ss_family == AF_INET) {
        return TT_NET_AF_INET;
    } else {
        TT_ASSERT_SA(addr->ss_family == AF_INET6);
        return TT_NET_AF_INET6;
    }
}

void tt_sktaddr_set_ip_n_ntv(IN tt_sktaddr_ntv_t *addr, IN tt_sktaddr_ip_t *na)
{
    if (addr->ss_family == AF_INET) {
        if (na == TT_IP_ANY) {
            ((struct sockaddr_in *)addr)->sin_addr.s_addr = INADDR_ANY;
        } else {
            ((struct sockaddr_in *)addr)->sin_addr.s_addr = na->a32.__u32;
        }
    } else {
        TT_ASSERT_SA(addr->ss_family == AF_INET6);
        if (na == TT_IP_ANY) {
            ((struct sockaddr_in6 *)addr)->sin6_addr = in6addr_any;
        } else {
            tt_memcpy(((struct sockaddr_in6 *)addr)->sin6_addr.s6_addr,
                      na->a128.__u8,
                      16);
        }
    }
}

void tt_sktaddr_get_ip_n_ntv(IN tt_sktaddr_ntv_t *addr, OUT tt_sktaddr_ip_t *na)
{
    tt_memset(na, 0, sizeof(tt_sktaddr_ip_t));
    if (addr->ss_family == AF_INET) {
        na->a32.__u32 = ((struct sockaddr_in *)addr)->sin_addr.s_addr;
    } else {
        TT_ASSERT_SA(addr->ss_family == AF_INET6);
        tt_memcpy(na->a128.__u8,
                  ((struct sockaddr_in6 *)addr)->sin6_addr.s6_addr,
                  16);
    }
}

tt_result_t tt_sktaddr_ip_n2p_ntv(IN tt_net_family_t family,
                                  IN tt_sktaddr_ip_t *na,
                                  OUT tt_char_t *buf,
                                  IN tt_u32_t buf_len)
{
    if (family == TT_NET_AF_INET) {
        struct in_addr n;

        n.s_addr = na->a32.__u32;
        if (inet_ntop(AF_INET, &n, buf, buf_len) != NULL) {
            return TT_SUCCESS;
        } else {
            TT_ERROR_NTV("invalid n addr");
            return TT_FAIL;
        }
    } else {
        struct in6_addr n;

        TT_ASSERT_SA(family == TT_NET_AF_INET6);

        tt_memcpy(n.s6_addr, na->a128.__u8, 16);
        if (inet_ntop(AF_INET6, &n, buf, buf_len) != NULL) {
            return TT_SUCCESS;
        } else {
            TT_ERROR_NTV("invalid n addr");
            return TT_FAIL;
        }
    }
}

tt_result_t tt_sktaddr_ip_p2n_ntv(IN tt_net_family_t family,
                                  IN const tt_char_t *buf,
                                  OUT tt_sktaddr_ip_t *na)
{
    if (family == TT_NET_AF_INET) {
        int ret;
        struct in_addr n;

        ret = inet_pton(AF_INET, buf, &n);
        if (ret == 1) {
            na->a32.__u32 = n.s_addr;
            return TT_SUCCESS;
        } else if (ret == 0) {
            TT_ERROR("invalid format: %s", buf);
            return TT_FAIL;
        } else {
            TT_ERROR_NTV("invalid p address: %s", buf);
            return TT_FAIL;
        }
    } else {
        int ret;
        struct in6_addr n;

        TT_ASSERT_SA(family == TT_NET_AF_INET6);

        ret = inet_pton(AF_INET6, buf, &n);
        if (ret == 1) {
            tt_memcpy(na->a128.__u8, n.s6_addr, 16);
            return TT_SUCCESS;
        } else if (ret == 0) {
            TT_ERROR("invalid format: %s", buf);
            return TT_FAIL;
        } else {
            TT_ERROR_NTV("invalid p address: %s", buf);
            return TT_FAIL;
        }
    }
}

void tt_sktaddr_set_port_ntv(IN tt_sktaddr_ntv_t *addr, IN tt_u16_t port)
{
    if (addr->ss_family == AF_INET) {
        ((struct sockaddr_in *)addr)->sin_port = htons(port);
    } else {
        TT_ASSERT_SA(addr->ss_family == AF_INET6);
        ((struct sockaddr_in6 *)addr)->sin6_port = htons(port);
    }
}

tt_u16_t tt_sktaddr_get_port_ntv(IN tt_sktaddr_ntv_t *addr)
{
    if (addr->ss_family == AF_INET) {
        return ntohs(((struct sockaddr_in *)addr)->sin_port);
    } else {
        TT_ASSERT_SA(addr->ss_family == AF_INET6);
        return ntohs(((struct sockaddr_in6 *)addr)->sin6_port);
    }
}

void tt_sktaddr_set_scope_ntv(IN tt_sktaddr_ntv_t *addr, IN tt_u32_t scope_id)
{
    if (addr->ss_family != AF_INET6) {
        TT_WARN("can no set scope id to ipv4 addr");
        return;
    }

    ((struct sockaddr_in6 *)addr)->sin6_scope_id = scope_id;
}

tt_u32_t tt_sktaddr_get_scope_ntv(IN tt_sktaddr_ntv_t *addr)
{
    return ((struct sockaddr_in6 *)addr)->sin6_scope_id;
}

void tt_sktaddr_set_scope_p_ntv(IN tt_sktaddr_ntv_t *addr,
                                IN tt_char_t *scope_name)
{
    tt_u32_t scope_id;

    if (addr->ss_family != AF_INET6) {
        TT_WARN("can no set scope id to ipv4 addr");
        return;
    }

    scope_id = if_nametoindex(scope_name);
    if (scope_id == 0) {
        TT_ERROR("invalid scope name: %s", scope_name);
        return;
    }

    ((struct sockaddr_in6 *)addr)->sin6_scope_id = scope_id;
}

void tt_sktaddr_map4to6_ntv(IN tt_sktaddr_ntv_t *in4, OUT tt_sktaddr_ntv_t *in6)
{
    struct sockaddr_in *i4 = (struct sockaddr_in *)in4;
    struct sockaddr_in6 *i6 = (struct sockaddr_in6 *)in6;
    unsigned char *uc6 = i6->sin6_addr.s6_addr;

    // save ipv4 value as in4 and in6 may be same pointer
    uint32_t __s_addr = i4->sin_addr.s_addr;
    uint16_t __sin_port = i4->sin_port;
    // now change change in6
    tt_memset(i6, 0, sizeof(struct sockaddr_in6));

    i6->sin6_family = AF_INET6;

    *((uint32_t *)&uc6[0]) = 0;
    *((uint32_t *)&uc6[4]) = 0;
    *((uint16_t *)&uc6[8]) = 0;
    *((uint16_t *)&uc6[10]) = 0xFFFF;
    *((uint32_t *)&uc6[12]) = __s_addr;

    i6->sin6_port = __sin_port;

    // i6->sin6_flowinfo = 0;
    // i6->sin6_scope_id = 0;
}

tt_bool_t tt_sktaddr_ipv4mapped_ntv(IN tt_sktaddr_ntv_t *addr)
{
    unsigned char *uc6 = ((struct sockaddr_in6 *)addr)->sin6_addr.s6_addr;

    if (addr->ss_family != AF_INET6) {
        return TT_FALSE;
    }

    if ((*((uint32_t *)&uc6[0]) != 0) || (*((uint32_t *)&uc6[4]) != 0) ||
        (*((uint16_t *)&uc6[8]) != 0) || (*((uint16_t *)&uc6[10]) != 0xFFFF)) {
        return TT_FALSE;
    }

    return TT_TRUE;
}
