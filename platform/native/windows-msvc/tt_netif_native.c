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

#include <tt_netif_native.h>

#include <algorithm/tt_buffer.h>
#include <log/tt_log.h>
#include <memory/tt_memory_alloc.h>
#include <network/netif/tt_netif.h>
#include <network/netif/tt_netif_addr.h>
#include <network/netif/tt_netif_group.h>
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

#define __cmp_addr(na, sa)                                                     \
    tt_memcmp(&((SOCKADDR_IN *)(na))->sin_addr,                                \
              &((SOCKADDR_IN *)(sa))->sin_addr,                                \
              sizeof(IN_ADDR))
#define __copy_addr(na, sa) tt_memcpy((na), (sa), sizeof(SOCKADDR_IN))

#define __cmp_addr6(na, sa)                                                    \
    tt_memcmp(&((SOCKADDR_IN6 *)(na))->sin6_addr,                              \
              &((SOCKADDR_IN6 *)(sa))->sin6_addr,                              \
              sizeof(IN6_ADDR))
#define __copy_addr6(na, sa) tt_memcpy((na), (sa), sizeof(SOCKADDR_IN6))

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

static tt_result_t __load_adpt(IN tt_netif_group_t *group,
                               IN tt_u32_t flag,
                               OUT PIP_ADAPTER_ADDRESSES *adpt);

static tt_netif_t *__luid2netif(IN tt_netif_group_t *group, IN NET_LUID *luid);

static tt_result_t __netif_update(IN tt_netif_t *netif,
                                  IN PIP_ADAPTER_ADDRESSES adpt,
                                  IN tt_u32_t flag);
#define __NIF_UPDT_NO_IPV6_LINK_LOCAL (1 << 0)

static void __type_map(IN DWORD IfType, IN DWORD flags, IN tt_netif_t *netif);

// inet
static tt_netif_addr_t *__find_addr(IN tt_netif_t *netif,
                                    IN PIP_ADAPTER_UNICAST_ADDRESS uni_addr);

static void __addr_update(IN tt_netif_addr_t *netif_addr,
                          IN PIP_ADAPTER_ADDRESSES adpt,
                          IN PIP_ADAPTER_UNICAST_ADDRESS uni_addr);

// inet6
static tt_netif_addr_t *__find_addr6(IN tt_netif_t *netif,
                                     IN PIP_ADAPTER_UNICAST_ADDRESS uni_addr);

static void __addr6_update(IN tt_netif_addr_t *netif_addr,
                           IN PIP_ADAPTER_ADDRESSES adpt,
                           IN PIP_ADAPTER_UNICAST_ADDRESS uni_addr);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_network_interface_init_ntv()
{
    tt_atomic_s32_set(&tt_s_adpt_size, __INIT_ADPT_ADDR_SIZE);

    return TT_SUCCESS;
}

tt_result_t tt_netif_group_refresh_ntv(IN tt_netif_group_t *group,
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

        netif = __luid2netif(group, &cur_adpt->Luid);
        if (netif == NULL) {
            continue;
        }

        if (!TT_OK(__netif_update(netif, cur_adpt, updt_flag))) {
            TT_ERROR("netif[%s] is removed due to updating failed",
                     netif->name);
            tt_list_remove(&netif->node);
            tt_netif_destroy(netif);

            continue;
        }
    }

    tt_free(adpt);
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

tt_result_t tt_netif_name2idx_ntv(IN const tt_char_t *name, OUT tt_u32_t *idx)
{
    NET_LUID luid;
    NET_IFINDEX index;
    NETIO_STATUS nstatus;

    nstatus = ConvertInterfaceNameToLuidA(name, &luid);
    if (nstatus != NETIO_ERROR_SUCCESS) {
        TT_ERROR("fail to convert name[%s] to luid", name);
        return TT_FAIL;
    }

    nstatus = ConvertInterfaceLuidToIndex(&luid, &index);
    if (nstatus != NETIO_ERROR_SUCCESS) {
        TT_ERROR("fail to convert luid to index");
        return TT_FAIL;
    }

    *idx = (tt_u32_t)index;
    return TT_SUCCESS;
}

tt_result_t tt_netif_idx2name_ntv(IN tt_u32_t idx,
                                  OUT tt_char_t *name,
                                  IN tt_u32_t len)
{
    NET_IFINDEX index;
    NET_LUID luid;
    char ifname[NDIS_IF_MAX_STRING_SIZE + 1];
    NETIO_STATUS nstatus;
    tt_u32_t n;

    index = (NET_IFINDEX)idx;
    nstatus = ConvertInterfaceIndexToLuid(index, &luid);
    if (nstatus != NETIO_ERROR_SUCCESS) {
        TT_ERROR("fail to convert index to luid");
        return TT_FAIL;
    }

    nstatus = ConvertInterfaceLuidToNameA(&luid, ifname, sizeof(ifname));
    if (nstatus != NETIO_ERROR_SUCCESS) {
        TT_ERROR("fail to convert luid to name");
        return TT_FAIL;
    }

    n = (tt_u32_t)strlen(ifname);
    if (len <= n) {
        TT_ERROR("no enough space for netif name");
        return TT_E_NOSPC;
    }

    memcpy(name, ifname, n);
    name[n] = 0;
    return TT_SUCCESS;
}

tt_result_t __load_adpt(IN tt_netif_group_t *group,
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
            GAA_FLAG_SKIP_FRIENDLY_NAME | GAA_FLAG_SKIP_MULTICAST |
            GAA_FLAG_INCLUDE_ALL_INTERFACES;

retry:
    TT_ASSERT(size != 0);
    AdapterAddresses = (PIP_ADAPTER_ADDRESSES)tt_malloc(size);
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
            tt_free(AdapterAddresses);
        }

        size = (tt_u32_t)req_size;
        tt_atomic_s32_set(&tt_s_adpt_size, size);

        retried = TT_TRUE;
        goto retry;
    } else {
        TT_ERROR("fail to get adapters addresses");

        if (AdapterAddresses != NULL) {
            tt_free(AdapterAddresses);
        }

        return TT_FAIL;
    }
}

tt_netif_t *__luid2netif(IN tt_netif_group_t *group, IN NET_LUID *luid)
{
    char name[NDIS_IF_MAX_STRING_SIZE + 1] = {0};
    NETIO_STATUS nstatus;

    nstatus = ConvertInterfaceLuidToNameA(luid, name, sizeof(name));
    if (nstatus != NETIO_ERROR_SUCCESS) {
        return NULL;
    }

    return tt_netif_group_find(group, name);
}

tt_result_t __netif_update(IN tt_netif_t *netif,
                           IN PIP_ADAPTER_ADDRESSES adpt,
                           IN tt_u32_t flag)
{
    PIP_ADAPTER_UNICAST_ADDRESS uni_addr;
    tt_netif_status_t status;

    // mtu
    netif->mtu = adpt->Mtu;

    // mac address
    if (adpt->PhysicalAddressLength >= TT_MACADDR_LEN) {
        tt_macaddr_init(&netif->macaddr, adpt->PhysicalAddress);
    }

    // type and flags
    __type_map(adpt->IfType, adpt->Flags, netif);

    // address
    for (uni_addr = adpt->FirstUnicastAddress; uni_addr != NULL;
         uni_addr = uni_addr->Next) {
        tt_netif_addr_t *netif_addr;
        SOCKADDR *sockaddr = uni_addr->Address.lpSockaddr;

        switch (sockaddr->sa_family) {
            case AF_INET: {
                netif_addr = __find_addr(netif, uni_addr);
                if (netif_addr != NULL) {
                    __addr_update(netif_addr, adpt, uni_addr);
                    netif_addr->internal_flag |= __NETIF_INTERNAL_TOUCHED;
                } else {
                    netif_addr = tt_netif_addr_create(TT_NET_AF_INET);
                    if (netif_addr != NULL) {
                        __addr_update(netif_addr, adpt, uni_addr);
                        netif_addr->internal_flag |= __NETIF_INTERNAL_TOUCHED;
                        tt_list_push_tail(&netif->addr_list, &netif_addr->node);
                    }
                }
            } break;
            case AF_INET6: {
                if ((sockaddr->sa_family == AF_INET6) &&
                    (flag & __NIF_UPDT_NO_IPV6_LINK_LOCAL)) {
                    tt_u8_t *__u8 =
                        ((SOCKADDR_IN6 *)sockaddr)->sin6_addr.u.Byte;

                    if ((__u8[0] == 0xFE) && (__u8[1] == 0x80)) {
                        break;
                    }
                }

                netif_addr = __find_addr6(netif, uni_addr);
                if (netif_addr != NULL) {
                    __addr6_update(netif_addr, adpt, uni_addr);
                    netif_addr->internal_flag |= __NETIF_INTERNAL_TOUCHED;
                } else {
                    netif_addr = tt_netif_addr_create(TT_NET_AF_INET6);
                    if (netif_addr != NULL) {
                        __addr6_update(netif_addr, adpt, uni_addr);
                        netif_addr->internal_flag |= __NETIF_INTERNAL_TOUCHED;
                        tt_list_push_tail(&netif->addr_list, &netif_addr->node);
                    }
                }
            } break;

            default: {
            } break;
        }
    }

    // status
    if (adpt->OperStatus == IfOperStatusUp) {
        if (!tt_list_empty(&netif->addr_list)) {
            status = TT_NETIF_STATUS_ACTIVE;
        } else {
            status = TT_NETIF_STATUS_UP;
        }
    } else {
        status = TT_NETIF_STATUS_DOWN;
    }
    if (netif->status != status) {
        netif->internal_flag |= TT_NETIF_DIFF_STATUS;
    }
    netif->status = status;

    return TT_SUCCESS;
}

void __type_map(IN DWORD IfType, IN DWORD flags, IN tt_netif_t *netif)
{
    switch (IfType) {
        case IF_TYPE_ETHERNET_CSMACD: {
            netif->type = TT_NETIF_TYPE_ETHERNET;
            netif->broadcast = TT_TRUE;
            netif->loopback = TT_FALSE;
            netif->p2p = TT_FALSE;
        } break;
        case IF_TYPE_ISO88025_TOKENRING: {
            netif->type = TT_NETIF_TYPE_TOKEN_RING;
            netif->broadcast = TT_TRUE;
            netif->loopback = TT_FALSE;
            netif->p2p = TT_FALSE;
        } break;
        case IF_TYPE_PPP: {
            netif->type = TT_NETIF_TYPE_PPP;
            netif->broadcast = TT_FALSE;
            netif->loopback = TT_FALSE;
            netif->p2p = TT_TRUE;
        } break;
        case IF_TYPE_SOFTWARE_LOOPBACK: {
            netif->type = TT_NETIF_TYPE_LOOPBACK;
            netif->broadcast = TT_FALSE;
            netif->loopback = TT_TRUE;
            netif->p2p = TT_FALSE;
        } break;
        case IF_TYPE_ATM: {
            netif->type = TT_NETIF_TYPE_ATM;
            netif->broadcast = TT_FALSE;
            netif->loopback = TT_FALSE;
            netif->p2p = TT_TRUE;
        } break;
        case IF_TYPE_IEEE80211: {
            netif->type = TT_NETIF_TYPE_IEEE80211;
            netif->broadcast = TT_TRUE;
            netif->loopback = TT_FALSE;
            netif->p2p = TT_FALSE;
        } break;
        case IF_TYPE_IEEE1394: {
            netif->type = TT_NETIF_TYPE_IEEE1394;
            netif->broadcast = TT_FALSE;
            netif->loopback = TT_FALSE;
            netif->p2p = TT_TRUE;
        } break;
        case IF_TYPE_TUNNEL: {
            netif->type = TT_NETIF_TYPE_TUNNEL;
            netif->broadcast = TT_FALSE;
            netif->loopback = TT_FALSE;
            netif->p2p = TT_TRUE;
        } break;
        default: {
            netif->type = TT_NETIF_TYPE_OTHER;
            netif->broadcast = TT_FALSE;
            netif->loopback = TT_FALSE;
            netif->p2p = TT_FALSE;
        } break;
    }

    netif->multicast = TT_BOOL(!(flags & IP_ADAPTER_NO_MULTICAST));
}

// inet
tt_netif_addr_t *__find_addr(IN tt_netif_t *netif,
                             IN PIP_ADAPTER_UNICAST_ADDRESS uni_addr)
{
    SOCKADDR *sockaddr = uni_addr->Address.lpSockaddr;
    tt_lnode_t *node = tt_list_head(&netif->addr_list);

    while (node != NULL) {
        tt_netif_addr_t *cur_addr = TT_CONTAINER(node, tt_netif_addr_t, node);

        node = node->next;

        if ((sockaddr->sa_family == AF_INET) &&
            (__cmp_addr(&cur_addr->addr, sockaddr) == 0)) {
            return cur_addr;
        }
    }

    return NULL;
}

void __addr_update(IN tt_netif_addr_t *netif_addr,
                   IN PIP_ADAPTER_ADDRESSES adpt,
                   IN PIP_ADAPTER_UNICAST_ADDRESS uni_addr)
{
    SOCKADDR *sockaddr = uni_addr->Address.lpSockaddr;

    if (__cmp_addr(&netif_addr->addr, sockaddr) != 0) {
        __copy_addr(&netif_addr->addr, sockaddr);
        netif_addr->internal_flag |= TT_NETIF_DIFF_ADDR;
    }

    if (uni_addr->OnLinkPrefixLength <= 32) {
        tt_u32_t v;
        tt_sktaddr_ip_t ip;

        v = tt_hton32(~0 << (32 - uni_addr->OnLinkPrefixLength));
        ip.a32.__u32 = v;
        tt_sktaddr_set_ip_n(&netif_addr->netmask, &ip);

        tt_sktaddr_get_ip_n(&netif_addr->addr, &ip);
        ip.a32.__u32 &= (tt_u32_t)v;
        ip.a32.__u32 |= (tt_u32_t)~v;
        tt_sktaddr_set_ip_n(&netif_addr->dstaddr, &ip);
    }
}

// inet6
tt_netif_addr_t *__find_addr6(IN tt_netif_t *netif,
                              IN PIP_ADAPTER_UNICAST_ADDRESS uni_addr)
{
    SOCKADDR *sockaddr = uni_addr->Address.lpSockaddr;
    tt_lnode_t *node = tt_list_head(&netif->addr_list);

    while (node != NULL) {
        tt_netif_addr_t *cur_addr = TT_CONTAINER(node, tt_netif_addr_t, node);

        node = node->next;

        if ((sockaddr->sa_family == AF_INET6) &&
            (__cmp_addr6(&cur_addr->addr, sockaddr) == 0)) {
            return cur_addr;
        }
    }

    return NULL;
}

void __addr6_update(IN tt_netif_addr_t *netif_addr,
                    IN PIP_ADAPTER_ADDRESSES adpt,
                    IN PIP_ADAPTER_UNICAST_ADDRESS uni_addr)
{
    SOCKADDR *sockaddr = uni_addr->Address.lpSockaddr;

    if (__cmp_addr6(&netif_addr->addr, sockaddr) != 0) {
        __copy_addr6(&netif_addr->addr, sockaddr);
        netif_addr->internal_flag |= TT_NETIF_DIFF_ADDR;
    }
}
