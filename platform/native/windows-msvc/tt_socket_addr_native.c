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
#include <misc/tt_util.h>

#include <tt_cstd_api.h>
#include <tt_sys_error.h>
#include <tt_wchar.h>

#include <iphlpapi.h>
#include <ws2tcpip.h>

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
            ((SOCKADDR_IN *)addr)->sin_addr.s_addr = INADDR_ANY;
        } else {
            ((SOCKADDR_IN *)addr)->sin_addr.s_addr = na->a32.__u32;
        }
    } else {
        TT_ASSERT_SA(addr->ss_family == AF_INET6);
        if (na == TT_IP_ANY) {
            ((SOCKADDR_IN6 *)addr)->sin6_addr = in6addr_any;
        } else {
            tt_memcpy(((SOCKADDR_IN6 *)addr)->sin6_addr.s6_bytes, na->a128.__u8,
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
                  ((struct sockaddr_in6 *)addr)->sin6_addr.s6_bytes, 16);
    }
}

tt_result_t tt_sktaddr_ip_n2p_ntv(IN tt_net_family_t family,
                                  IN tt_sktaddr_ip_t *na, OUT tt_char_t *buf,
                                  IN tt_u32_t buf_len)
{
    if (family == TT_NET_AF_INET) {
        IN_ADDR n;

        n.s_addr = na->a32.__u32;
        if (InetNtopA(AF_INET, &n, buf, buf_len) != NULL) {
            return TT_SUCCESS;
        } else {
            TT_NET_ERROR_NTV("invalid n addr");
            return TT_FAIL;
        }
    } else {
        IN6_ADDR n;

        TT_ASSERT_SA(family == TT_NET_AF_INET6);

        tt_memcpy(n.s6_bytes, na->a128.__u8, 16);
        if (InetNtopA(AF_INET6, &n, buf, buf_len) != NULL) {
            return TT_SUCCESS;
        } else {
            TT_NET_ERROR_NTV("invalid n addr");
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
        IN_ADDR n;

        ret = InetPtonA(AF_INET, buf, &n);
        if (ret == 1) {
            na->a32.__u32 = n.s_addr;
            return TT_SUCCESS;
        } else if (ret == 0) {
            TT_ERROR("invalid format: %ls", buf);
            return TT_FAIL;
        } else {
            TT_NET_ERROR_NTV("invalid p address: %ls", buf);
            return TT_FAIL;
        }
    } else {
        int ret;
        IN6_ADDR n;

        TT_ASSERT_SA(family == TT_NET_AF_INET6);

        ret = InetPtonA(AF_INET6, buf, &n);
        if (ret == 1) {
            tt_memcpy(na->a128.__u8, n.s6_bytes, 16);
            return TT_SUCCESS;
        } else if (ret == 0) {
            TT_ERROR("invalid format: %ls", buf);
            return TT_FAIL;
        } else {
            TT_NET_ERROR_NTV("invalid p address: %ls", buf);
            return TT_FAIL;
        }
    }
}

void tt_sktaddr_set_port_ntv(IN tt_sktaddr_ntv_t *addr, IN tt_u16_t port)
{
    if (addr->ss_family == AF_INET) {
        ((SOCKADDR_IN *)addr)->sin_port = htons(port);
    } else {
        TT_ASSERT_SA(addr->ss_family == AF_INET6);
        ((SOCKADDR_IN6 *)addr)->sin6_port = htons(port);
    }
}

tt_u16_t tt_sktaddr_get_port_ntv(IN tt_sktaddr_ntv_t *addr)
{
    if (addr->ss_family == AF_INET) {
        return ntohs(((SOCKADDR_IN *)addr)->sin_port);
    } else {
        TT_ASSERT_SA(addr->ss_family == AF_INET6);
        return ntohs(((SOCKADDR_IN6 *)addr)->sin6_port);
    }
}

void tt_sktaddr_set_scope_ntv(IN tt_sktaddr_ntv_t *addr, IN tt_u32_t scope_id)
{
    if (addr->ss_family != AF_INET6) {
        TT_WARN("can no set scope id to ipv4 addr");
        return;
    }

    ((SOCKADDR_IN6 *)addr)->sin6_scope_id = scope_id;
}

tt_u32_t tt_sktaddr_get_scope_ntv(IN tt_sktaddr_ntv_t *addr)
{
    return (tt_u32_t)((SOCKADDR_IN6 *)addr)->sin6_scope_id;
}

void tt_sktaddr_set_scope_p_ntv(IN tt_sktaddr_ntv_t *addr,
                                IN tt_char_t *scope_name)
{
    WCHAR *InterfaceName;
    NET_LUID luid;
    NET_IFINDEX ifidx;

    if (addr->ss_family != AF_INET6) {
        TT_WARN("can no set scope id to ipv4 addr");
        return;
    }

    InterfaceName = tt_wchar_create(scope_name, 0, NULL);
    if (InterfaceName == NULL) { return; }

    if ((ConvertInterfaceNameToLuidW(InterfaceName, &luid) != 0) ||
        (ConvertInterfaceLuidToIndex(&luid, &ifidx) != NO_ERROR)) {
        TT_ERROR_NTV("can not get ifidx of %ls", scope_name);
        tt_wchar_destroy(InterfaceName);
        return;
    }

    ((SOCKADDR_IN6 *)addr)->sin6_scope_id = (u_long)ifidx;
}

void tt_sktaddr_map4to6_ntv(IN tt_sktaddr_ntv_t *in4, OUT tt_sktaddr_ntv_t *in6)
{
    SOCKADDR_IN *i4 = (SOCKADDR_IN *)in4;
    SOCKADDR_IN6 *i6 = (SOCKADDR_IN6 *)in6;
    u_char *uc6 = i6->sin6_addr.s6_bytes;

    // save ipv4 value as in4 and in6 may be same pointer
    u_long __s_addr = i4->sin_addr.s_addr;
    u_short __sin_port = i4->sin_port;
    // now change change in6
    tt_memset(i6, 0, sizeof(SOCKADDR_IN6));

    i6->sin6_family = AF_INET6;

    *((u_long *)&uc6[0]) = 0;
    *((u_long *)&uc6[4]) = 0;
    *((u_short *)&uc6[8]) = 0;
    *((u_short *)&uc6[10]) = 0xFFFF;
    *((u_long *)&uc6[12]) = __s_addr;

    i6->sin6_port = __sin_port;

    // i6->sin6_flowinfo = 0;
    // i6->sin6_scope_id = 0;
}

tt_bool_t tt_sktaddr_ipv4mapped_ntv(IN tt_sktaddr_ntv_t *addr)
{
    u_char *uc6 = ((SOCKADDR_IN6 *)addr)->sin6_addr.s6_bytes;

    if (addr->ss_family != AF_INET6) { return TT_FALSE; }

    if ((*((u_long *)&uc6[0]) != 0) || (*((u_long *)&uc6[4]) != 0) ||
        (*((u_short *)&uc6[8]) != 0) || (*((u_short *)&uc6[10]) != 0xFFFF)) {
        return TT_FALSE;
    }

    return TT_TRUE;
}
