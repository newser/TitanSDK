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
    } else if (family == TT_NET_AF_INET6) {
        addr->ss_family = AF_INET6;
    } else {
        TT_ERROR("invalid family: %d", family);
        addr->ss_family = -1;
    }
}

tt_net_family_t tt_sktaddr_get_family_ntv(IN tt_sktaddr_ntv_t *addr)
{
    if (addr->ss_family == AF_INET) {
        return TT_NET_AF_INET;
    } else if (addr->ss_family == AF_INET6) {
        return TT_NET_AF_INET6;
    } else {
        TT_ERROR("invalid sys family: %d", addr->ss_family);
        return TT_NET_AF_INVALID;
    }
}

tt_result_t tt_sktaddr_set_addr_n_ntv(IN tt_sktaddr_ntv_t *addr,
                                      IN union _tt_sktaddr_addr_t *addr_val)
{
    if (addr->ss_family == AF_INET) {
        if (addr_val == TT_SKTADDR_ANY) {
            ((SOCKADDR_IN *)addr)->sin_addr.s_addr = INADDR_ANY;
        } else {
            ((SOCKADDR_IN *)addr)->sin_addr.s_addr = addr_val->addr32.__u32;
        }
        return TT_SUCCESS;
    } else if (addr->ss_family == AF_INET6) {
        if (addr_val == TT_SKTADDR_ANY) {
            ((SOCKADDR_IN6 *)addr)->sin6_addr = in6addr_any;
        } else {
            tt_memcpy(((SOCKADDR_IN6 *)addr)->sin6_addr.s6_bytes,
                      addr_val->addr128.__u8,
                      16);
        }
        return TT_SUCCESS;
    } else {
        TT_ERROR("invalid sys family: %d", addr->ss_family);
        return TT_FAIL;
    }
}

tt_result_t tt_sktaddr_get_addr_n_ntv(IN tt_sktaddr_ntv_t *addr,
                                      OUT union _tt_sktaddr_addr_t *addr_val)
{
    // tt_memset(addr_val, 0, sizeof(union _tt_sktaddr_addr_t));

    if (addr->ss_family == AF_INET) {
        addr_val->addr32.__u32 = ((SOCKADDR_IN *)addr)->sin_addr.s_addr;
        return TT_SUCCESS;
    } else if (addr->ss_family == AF_INET6) {
        tt_memcpy(addr_val->addr128.__u8,
                  ((SOCKADDR_IN6 *)addr)->sin6_addr.s6_bytes,
                  16);
        return TT_SUCCESS;
    } else {
        TT_ERROR("invalid sys family: %d", addr->ss_family);
        return TT_FAIL;
    }
}

tt_result_t tt_sktaddr_addr_n2p_ntv(IN tt_net_family_t family,
                                    IN union _tt_sktaddr_addr_t *addr_val,
                                    OUT tt_char_t *buf,
                                    IN tt_u32_t buf_len)
{
    if (family == TT_NET_AF_INET) {
        IN_ADDR __n;
        __n.s_addr = addr_val->addr32.__u32;

        if (InetNtopA(AF_INET, &__n, buf, buf_len) != NULL) {
            return TT_SUCCESS;
        } else {
            TT_NET_ERROR_NTV("invalid n addr");
            return TT_FAIL;
        }
    } else if (family == TT_NET_AF_INET6) {
        IN6_ADDR __n;
        tt_memcpy(__n.s6_bytes, addr_val->addr128.__u8, 16);

        if (InetNtopA(AF_INET6, &__n, buf, buf_len) != NULL) {
            return TT_SUCCESS;
        } else {
            TT_NET_ERROR_NTV("invalid n addr");
            return TT_FAIL;
        }
    } else {
        TT_ERROR("invalid family: %d", family);
        return TT_FAIL;
    }
}

tt_result_t tt_sktaddr_addr_p2n_ntv(IN tt_net_family_t family,
                                    IN tt_char_t *buf,
                                    OUT union _tt_sktaddr_addr_t *addr_val)
{
    if (family == TT_NET_AF_INET) {
        int ret;
        IN_ADDR __n;

        ret = InetPtonA(AF_INET, buf, &__n);
        if (ret == 1) {
            addr_val->addr32.__u32 = __n.s_addr;
            return TT_SUCCESS;
        } else if (ret == 0) {
            TT_ERROR("invalid format: %ls", buf);
            return TT_FAIL;
        } else {
            TT_NET_ERROR_NTV("invalid p address: %ls", buf);
            return TT_FAIL;
        }
    } else if (family == TT_NET_AF_INET6) {
        int ret;
        IN6_ADDR __n;

        ret = InetPtonA(AF_INET6, buf, &__n);
        if (ret == 1) {
            tt_memcpy(addr_val->addr128.__u8, __n.s6_bytes, 16);
            return TT_SUCCESS;
        } else if (ret == 0) {
            TT_ERROR("invalid format: %ls", buf);
            return TT_FAIL;
        } else {
            TT_NET_ERROR_NTV("invalid p address: %ls", buf);
            return TT_FAIL;
        }
    } else {
        TT_ERROR("invalid family: %d", family);
        return TT_FAIL;
    }
}

tt_result_t tt_sktaddr_set_port_ntv(IN tt_sktaddr_ntv_t *addr, IN tt_u16_t port)
{
    if (addr->ss_family == AF_INET) {
        ((SOCKADDR_IN *)addr)->sin_port = htons(port);
        return TT_SUCCESS;
    } else if (addr->ss_family == AF_INET6) {
        ((SOCKADDR_IN6 *)addr)->sin6_port = htons(port);
        return TT_SUCCESS;
    } else {
        TT_ERROR("invalid sys family: %d", addr->ss_family);
        return TT_FAIL;
    }
}

tt_result_t tt_sktaddr_get_port_ntv(IN tt_sktaddr_ntv_t *addr,
                                    IN tt_u16_t *port)
{
    if (addr->ss_family == AF_INET) {
        *port = ntohs(((SOCKADDR_IN *)addr)->sin_port);
        return TT_SUCCESS;
    } else if (addr->ss_family == AF_INET6) {
        *port = ntohs(((SOCKADDR_IN6 *)addr)->sin6_port);
        return TT_SUCCESS;
    } else {
        TT_ERROR("invalid sys family: %d", addr->ss_family);
        return TT_FAIL;
    }
}

tt_result_t tt_sktaddr_set_scope_ntv(IN tt_sktaddr_ntv_t *addr,
                                     IN tt_u32_t scope_id)
{
    if (addr->ss_family == AF_INET6) {
        ((SOCKADDR_IN6 *)addr)->sin6_scope_id = (u_long)scope_id;
        return TT_SUCCESS;
    } else {
        TT_ERROR("invalid sys family[%d] to set scope id", addr->ss_family);
        return TT_FAIL;
    }
}

tt_result_t tt_sktaddr_set_scope_p_ntv(IN tt_sktaddr_ntv_t *addr,
                                       IN tt_char_t *scope_name)
{
    if (addr->ss_family == AF_INET6) {
        WCHAR *InterfaceName;
        NET_LUID luid;
        NET_IFINDEX ifidx;
        NETIO_STATUS ns;

        InterfaceName = tt_wchar_create(scope_name, NULL);
        if (InterfaceName == NULL) {
            return TT_FAIL;
        }

        // get network interface index
        ns = ConvertInterfaceNameToLuidW(InterfaceName, &luid);
        tt_wchar_destroy(InterfaceName);
        if (ns != 0) {
            TT_ERROR_NTV("can not get luid of %ls", scope_name);
            return TT_FAIL;
        }
        if (ConvertInterfaceLuidToIndex(&luid, &ifidx) != NO_ERROR) {
            TT_ERROR_NTV("can not get index from luid of %ls", scope_name);
            return TT_FAIL;
        }

        ((SOCKADDR_IN6 *)addr)->sin6_scope_id = (u_long)ifidx;
        return TT_SUCCESS;
    } else {
        TT_ERROR("invalid sys family[%d] to set scope id", addr->ss_family);
        return TT_FAIL;
    }
}

void tt_sktaddr_map4to6_ntv(IN tt_sktaddr_ntv_t *in4, OUT tt_sktaddr_ntv_t *in6)
{
    SOCKADDR_IN *__in4 = (SOCKADDR_IN *)in4;
    SOCKADDR_IN6 *__in6 = (SOCKADDR_IN6 *)in6;
    u_char *__uc6 = __in6->sin6_addr.s6_bytes;

    // save ipv4 value as in4 and in6 may be same pointer
    u_long __s_addr = __in4->sin_addr.s_addr;
    u_short __sin_port = __in4->sin_port;
    // now change change in6
    tt_memset(__in6, 0, sizeof(SOCKADDR_IN6));

    // family
    __in6->sin6_family = AF_INET6;

    // address, already in network endian
    *((u_long *)&__uc6[0]) = 0;
    *((u_long *)&__uc6[4]) = 0;
    *((u_short *)&__uc6[8]) = 0;
    *((u_short *)&__uc6[10]) = 0xFFFF;
    *((u_long *)&__uc6[12]) = __s_addr;

    // port
    __in6->sin6_port = __sin_port;

    // other
    //__in6->sin6_flowinfo = 0;
    //__in6->sin6_scope_id = 0;
}

tt_bool_t tt_sktaddr_ipv4mapped_ntv(IN tt_sktaddr_ntv_t *addr)
{
    u_char *__uc6 = ((SOCKADDR_IN6 *)addr)->sin6_addr.s6_bytes;

    // check family
    if (addr->ss_family != AF_INET6) {
        return TT_FALSE;
    }

    // check address
    if ((*((u_long *)&__uc6[0]) != 0) || (*((u_long *)&__uc6[4]) != 0) ||
        (*((u_short *)&__uc6[8]) != 0) ||
        (*((u_short *)&__uc6[10]) != 0xFFFF)) {
        return TT_FALSE;
    }

    return TT_TRUE;
}
