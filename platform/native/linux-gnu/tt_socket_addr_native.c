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

#include <tt_cstd_api.h>
#include <tt_sys_error.h>

#include <net/if.h>

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

void tt_sktaddr_set_addr_n_ntv(IN tt_sktaddr_ntv_t *addr,
                               IN union _tt_sktaddr_addr_t *addr_val)
{
    if (addr->ss_family == AF_INET) {
        if (addr_val == TT_SKTADDR_ANY) {
            ((struct sockaddr_in *)addr)->sin_addr.s_addr = INADDR_ANY;
        } else {
            ((struct sockaddr_in *)addr)->sin_addr.s_addr =
                addr_val->addr32.__u32;
        }
    } else if (addr->ss_family == AF_INET6) {
        if (addr_val == TT_SKTADDR_ANY) {
            ((struct sockaddr_in6 *)addr)->sin6_addr = in6addr_any;
        } else {
            tt_memcpy(((struct sockaddr_in6 *)addr)->sin6_addr.s6_addr,
                      addr_val->addr128.__u8,
                      16);
        }
    } else {
        TT_ERROR("invalid sys family: %d", addr->ss_family);
    }
}

void tt_sktaddr_get_addr_n_ntv(IN tt_sktaddr_ntv_t *addr,
                               OUT union _tt_sktaddr_addr_t *addr_val)
{
    tt_memset(addr_val, 0, sizeof(union _tt_sktaddr_addr_t));

    if (addr->ss_family == AF_INET) {
        addr_val->addr32.__u32 = ((struct sockaddr_in *)addr)->sin_addr.s_addr;
    } else if (addr->ss_family == AF_INET6) {
        tt_memcpy(addr_val->addr128.__u8,
                  ((struct sockaddr_in6 *)addr)->sin6_addr.s6_addr,
                  16);
    } else {
        TT_ERROR("invalid sys family: %d", addr->ss_family);
    }
}

tt_result_t tt_sktaddr_addr_n2p_ntv(IN tt_net_family_t family,
                                    IN union _tt_sktaddr_addr_t *addr_val,
                                    OUT tt_char_t *buf,
                                    IN tt_u32_t buf_len)
{
    if (family == TT_NET_AF_INET) {
        struct in_addr __n;
        __n.s_addr = addr_val->addr32.__u32;

        if (inet_ntop(AF_INET, &__n, buf, buf_len) != NULL) {
            return TT_SUCCESS;
        } else {
            TT_ERROR_NTV("invalid n addr");
            return TT_FAIL;
        }
    } else if (family == TT_NET_AF_INET6) {
        struct in6_addr __n;
        tt_memcpy(__n.s6_addr, addr_val->addr128.__u8, 16);

        if (inet_ntop(AF_INET6, &__n, buf, buf_len) != NULL) {
            return TT_SUCCESS;
        } else {
            TT_ERROR_NTV("invalid n addr");
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
        struct in_addr __n;

        ret = inet_pton(AF_INET, buf, &__n);
        if (ret == 1) {
            addr_val->addr32.__u32 = __n.s_addr;
            return TT_SUCCESS;
        } else if (ret == 0) {
            TT_ERROR("invalid format: %s", buf);
            return TT_FAIL;
        } else {
            TT_ERROR_NTV("invalid p address: %s", buf);
            return TT_FAIL;
        }
    } else if (family == TT_NET_AF_INET6) {
        int ret;
        struct in6_addr __n;

        ret = inet_pton(AF_INET6, buf, &__n);
        if (ret == 1) {
            tt_memcpy(addr_val->addr128.__u8, __n.s6_addr, 16);
            return TT_SUCCESS;
        } else if (ret == 0) {
            TT_ERROR("invalid format: %s", buf);
            return TT_FAIL;
        } else {
            TT_ERROR_NTV("invalid p address: %s", buf);
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
        ((struct sockaddr_in *)addr)->sin_port = htons(port);
        return TT_SUCCESS;
    } else if (addr->ss_family == AF_INET6) {
        ((struct sockaddr_in6 *)addr)->sin6_port = htons(port);
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
        *port = ntohs(((struct sockaddr_in *)addr)->sin_port);
        return TT_SUCCESS;
    } else if (addr->ss_family == AF_INET6) {
        *port = ntohs(((struct sockaddr_in6 *)addr)->sin6_port);
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
        ((struct sockaddr_in6 *)addr)->sin6_scope_id = scope_id;
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
        tt_u32_t scope_id = if_nametoindex(scope_name);
        if (scope_id != 0) {
            ((struct sockaddr_in6 *)addr)->sin6_scope_id = scope_id;
            return TT_SUCCESS;
        } else {
            TT_ERROR("invalid scope name: %s", scope_name);
            return TT_FAIL;
        }
    } else {
        TT_ERROR("invalid sys family[%d] to set scope id", addr->ss_family);
        return TT_FAIL;
    }
}

void tt_sktaddr_map4to6_ntv(IN tt_sktaddr_ntv_t *in4, OUT tt_sktaddr_ntv_t *in6)
{
    struct sockaddr_in *__in4 = (struct sockaddr_in *)in4;
    struct sockaddr_in6 *__in6 = (struct sockaddr_in6 *)in6;
    unsigned char *__uc6 = __in6->sin6_addr.s6_addr;

    // save ipv4 value as in4 and in6 may be same pointer
    uint32_t __s_addr = __in4->sin_addr.s_addr;
    in_port_t __sin_port = __in4->sin_port;
    // now change change in6
    tt_memset(__in6, 0, sizeof(struct sockaddr_in6));

    // family
    __in6->sin6_family = AF_INET6;

    // address, already in network endian
    *((uint32_t *)&__uc6[0]) = 0;
    *((uint32_t *)&__uc6[4]) = 0;
    *((uint16_t *)&__uc6[8]) = 0;
    *((uint16_t *)&__uc6[10]) = 0xFFFF;
    *((uint32_t *)&__uc6[12]) = __s_addr;

    // port
    __in6->sin6_port = __sin_port;

    // other
    //__in6->sin6_flowinfo = 0;
    //__in6->sin6_scope_id = 0;
}

tt_bool_t tt_sktaddr_ipv4mapped_ntv(IN tt_sktaddr_ntv_t *addr)
{
    unsigned char *__uc6 = ((struct sockaddr_in6 *)addr)->sin6_addr.s6_addr;

    // check family
    if (addr->ss_family != AF_INET6) {
        return TT_FALSE;
    }

    // check address
    if ((*((uint32_t *)&__uc6[0]) != 0) || (*((uint32_t *)&__uc6[4]) != 0) ||
        (*((uint16_t *)&__uc6[8]) != 0) ||
        (*((uint16_t *)&__uc6[10]) != 0xFFFF)) {
        return TT_FALSE;
    }

    return TT_TRUE;
}
