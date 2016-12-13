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

#include <tt_network_interface_native.h>

#include <log/tt_log.h>
#include <memory/tt_memory_alloc.h>
#include <network/tt_network_interface.h>
#include <os/tt_atomic.h>

#include <tt_sys_error.h>
#include <tt_util_native.h>

// clang-format off
#include <ws2tcpip.h>
#include <iphlpapi.h>
// clang-format on

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#ifndef NETIO_ERROR_SUCCESS
#define NETIO_ERROR_SUCCESS 0
#endif

#define __INIT_ADPT_ADDR_SIZE (3 * 1024)

#define __cmp_netif_addr(na, sa)                                               \
    tt_memcmp(&((SOCKADDR_IN *)&(na)->addr)->sin_addr,                         \
              &((SOCKADDR_IN *)(sa))->sin_addr,                                \
              sizeof(IN_ADDR))
#define __copy_netif_addr(na, sa)                                              \
    tt_memcpy(&(na)->addr, (sa), sizeof(SOCKADDR_IN))

#define __cmp_netif_addr6(na, sa)                                              \
    tt_memcmp(&((SOCKADDR_IN6 *)&(na)->addr)->sin6_addr,                       \
              &((SOCKADDR_IN6 *)(sa))->sin6_addr,                              \
              sizeof(IN6_ADDR))
#define __copy_netif_addr6(na, sa)                                             \
    tt_memcpy(&(na)->addr, (sa), sizeof(SOCKADDR_IN6))

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_atomic_s32_t tt_s_adpt_size;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __load_adpt(IN struct tt_netif_group_s *group,
                               IN tt_u32_t flag,
                               OUT PIP_ADAPTER_ADDRESSES *adpt);

// netif
static tt_result_t __netif_update(IN tt_netif_t *netif,
                                  IN PIP_ADAPTER_ADDRESSES adpt,
                                  IN tt_u32_t flag);
#define __NIF_UPDT_NO_IPV6_LINK_LOCAL (1 << 0)

static tt_netif_addr_t *__netif_find_addr(
    IN tt_netif_t *netif, IN PIP_ADAPTER_UNICAST_ADDRESS uni_addr);

// netif addr
static tt_result_t __netif_addr_update(IN tt_netif_addr_t *netif_addr,
                                       IN PIP_ADAPTER_ADDRESSES adpt,
                                       IN PIP_ADAPTER_UNICAST_ADDRESS uni_addr);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_network_interface_init_ntv()
{
    tt_atomic_s32_init(&tt_s_adpt_size, __INIT_ADPT_ADDR_SIZE);

    return TT_SUCCESS;
}

tt_result_t tt_netif_group_refresh_ntv(IN struct tt_netif_group_s *group,
                                       IN tt_u32_t flag)
{
    PIP_ADAPTER_ADDRESSES adpt = NULL;
    PIP_ADAPTER_ADDRESSES cur_adpt = NULL;
    tt_u32_t updt_flag = 0;

    if (!TT_OK(__load_adpt(group, flag, &adpt))) {
        return TT_FAIL;
    }

    if (group->flag & TT_NIFGRP_NO_IPV6_LINK_LOCAL) {
        updt_flag |= __NIF_UPDT_NO_IPV6_LINK_LOCAL;
    }

    for (cur_adpt = adpt; cur_adpt != NULL; cur_adpt = cur_adpt->Next) {
        tt_netif_t *netif;
        char itf_name[NDIS_IF_MAX_STRING_SIZE + 1] = {0};
        NETIO_STATUS nstatus;

        nstatus = ConvertInterfaceLuidToNameA(&cur_adpt->Luid,
                                              itf_name,
                                              sizeof(itf_name));
        if (nstatus != NETIO_ERROR_SUCCESS) {
            TT_WARN("fail to get itf name of [%s]", cur_adpt->AdapterName);
            continue;
        }
        TT_INFO("netif: %s", itf_name);

        netif = __netif_find(group, itf_name);
        if (netif == NULL) {
            continue;
        }

        if (!TT_OK(__netif_update(netif, cur_adpt, updt_flag))) {
            TT_ERROR("netif[%s] is removed due to updating failed",
                     netif->name);
            tt_list_remove(&netif->node);
            __netif_destroy(netif);

            continue;
        }
    }

    tt_mem_free(adpt);
    return TT_SUCCESS;
}

tt_result_t tt_netif_create_ntv(IN tt_netif_ntv_t *sys_netif)
{
    sys_netif->reserved = 0;

    return TT_SUCCESS;
}

void tt_netif_destroy_ntv(IN tt_netif_ntv_t *sys_netif)
{
}

tt_result_t __load_adpt(IN struct tt_netif_group_s *group,
                        IN tt_u32_t flag,
                        OUT PIP_ADAPTER_ADDRESSES *adpt)
{
    ULONG Family = AF_UNSPEC;
    ULONG Flags = 0;
    PIP_ADAPTER_ADDRESSES AdapterAddresses = NULL;
    tt_u32_t size = (tt_u32_t)tt_atomic_s32_get(&tt_s_adpt_size);

    ULONG ret, req_size;
    tt_bool_t retried = TT_FALSE;

    Flags = GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_DNS_SERVER |
            GAA_FLAG_SKIP_FRIENDLY_NAME | GAA_FLAG_SKIP_MULTICAST;

retry:
    TT_ASSERT(size != 0);
    AdapterAddresses = (PIP_ADAPTER_ADDRESSES)tt_mem_alloc(size);
    if (AdapterAddresses == NULL) {
        TT_ERROR("fail to alloc adpt addr");
        return TT_FAIL;
    }

    req_size = (ULONG)size;
    ret = GetAdaptersAddresses(Family, Flags, 0, AdapterAddresses, &req_size);
    if (ret == ERROR_SUCCESS) {
        *adpt = AdapterAddresses;
        return TT_SUCCESS;
    } else if ((ret == ERROR_BUFFER_OVERFLOW) && !retried) {
        if (AdapterAddresses != NULL) {
            tt_mem_free(AdapterAddresses);
        }

        size = (tt_u32_t)req_size;
        tt_atomic_s32_set(&tt_s_adpt_size, size);

        retried = TT_TRUE;
        goto retry;
    } else {
        TT_ERROR("fail to get adapters addresses");

        if (AdapterAddresses != NULL) {
            tt_mem_free(AdapterAddresses);
        }

        return TT_FAIL;
    }
}

tt_result_t __netif_update(IN tt_netif_t *netif,
                           IN PIP_ADAPTER_ADDRESSES adpt,
                           IN tt_u32_t flag)
{
    PIP_ADAPTER_UNICAST_ADDRESS uni_addr;

    // update addresses
    for (uni_addr = adpt->FirstUnicastAddress; uni_addr != NULL;
         uni_addr = uni_addr->Next) {
        tt_netif_addr_t *netif_addr;
        SOCKADDR *sockaddr = uni_addr->Address.lpSockaddr;

        if ((sockaddr->sa_family != AF_INET) &&
            (sockaddr->sa_family != AF_INET6)) {
            continue;
        }

        if ((sockaddr->sa_family == AF_INET6) &&
            (flag & __NIF_UPDT_NO_IPV6_LINK_LOCAL)) {
            tt_u8_t *__u8 = ((SOCKADDR_IN6 *)sockaddr)->sin6_addr.u.Byte;

            if ((__u8[0] == 0xFE) && (__u8[1] == 0x80)) {
                continue;
            }
        }

        netif_addr = __netif_find_addr(netif, uni_addr);
        if (netif_addr != NULL) {
            if (!TT_OK(__netif_addr_update(netif_addr, adpt, uni_addr))) {
                tt_list_remove(&netif_addr->node);
                __netif_addr_destroy(netif_addr);
                return TT_FAIL;
            }
        } else {
            if (sockaddr->sa_family == AF_INET) {
                netif_addr = __netif_addr_create(TT_NET_AF_INET);
            } else {
                TT_ASSERT(sockaddr->sa_family == AF_INET6);
                netif_addr = __netif_addr_create(TT_NET_AF_INET6);
            }
            if (netif_addr == NULL) {
                return TT_FAIL;
            }

            if (!TT_OK(__netif_addr_update(netif_addr, adpt, uni_addr))) {
                __netif_addr_destroy(netif_addr);
                return TT_FAIL;
            }

            tt_list_addtail(&netif->addr_list, &netif_addr->node);
        }
    }

    // update status
    if (adpt->OperStatus == IfOperStatusUp) {
        netif->internal_flag |= __NETIF_INTERNAL_UP;
    } else {
        netif->internal_flag &= ~__NETIF_INTERNAL_UP;
    }

    return TT_SUCCESS;
}

tt_netif_addr_t *__netif_find_addr(IN tt_netif_t *netif,
                                   IN PIP_ADAPTER_UNICAST_ADDRESS uni_addr)
{
    SOCKADDR *sockaddr = uni_addr->Address.lpSockaddr;
    tt_lnode_t *node = tt_list_head(&netif->addr_list);

    while (node != NULL) {
        tt_netif_addr_t *cur_adpt = TT_CONTAINER(node, tt_netif_addr_t, node);

        node = node->next;

        if (sockaddr->sa_family == AF_INET) {
            // ipv4, compare:
            //  - ip
            if (__cmp_netif_addr(cur_adpt, sockaddr) == 0) {
                return cur_adpt;
            }
        } else {
            TT_ASSERT(sockaddr->sa_family == AF_INET6);

            // ipv6, compare:
            //  - ip
            //  - scope id??
            if (__cmp_netif_addr6(cur_adpt, sockaddr) == 0) {
                return cur_adpt;
            }
        }
    }

    return NULL;
}

tt_result_t __netif_addr_update(IN tt_netif_addr_t *netif_addr,
                                IN PIP_ADAPTER_ADDRESSES adpt,
                                IN PIP_ADAPTER_UNICAST_ADDRESS uni_addr)
{
    SOCKADDR *sockaddr = uni_addr->Address.lpSockaddr;

    netif_addr->internal_flag |= __NETIF_INTERNAL_TOUCHED;

    if (sockaddr->sa_family == AF_INET) {
        if (__cmp_netif_addr(netif_addr, sockaddr) != 0) {
            __copy_netif_addr(netif_addr, sockaddr);
            netif_addr->internal_flag |= TT_NETIF_DIFF_ADDR;
        }

        return TT_SUCCESS;
    } else {
        TT_ASSERT(sockaddr->sa_family == AF_INET6);

        if (__cmp_netif_addr6(netif_addr, sockaddr) != 0) {
            __copy_netif_addr6(netif_addr, sockaddr);
            netif_addr->internal_flag |= TT_NETIF_DIFF_ADDR;
        }

        // any other fields

        return TT_SUCCESS;
    }
}
