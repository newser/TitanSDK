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

#include <log/tt_log.h>
#include <memory/tt_memory_alloc.h>
#include <network/netif/tt_netif.h>
#include <network/netif/tt_netif_addr.h>
#include <network/netif/tt_netif_group.h>

#include <tt_cstd_api.h>
#include <tt_sys_error.h>
#include <tt_util_native.h>

#include <ifaddrs.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __cmp_addr(na, sa)                                                     \
    tt_memcmp(&((struct sockaddr_in *)(na))->sin_addr,                         \
              &((struct sockaddr_in *)(sa))->sin_addr,                         \
              sizeof(struct in_addr))
#define __copy_addr(na, sa) tt_memcpy((na), (sa), sizeof(struct sockaddr_in))

#define __cmp_addr6(na, sa)                                                    \
    tt_memcmp(&((struct sockaddr_in6 *)(na))->sin6_addr,                       \
              &((struct sockaddr_in6 *)(sa))->sin6_addr,                       \
              sizeof(struct in6_addr))
#define __copy_addr6(na, sa) tt_memcpy((na), (sa), sizeof(struct sockaddr_in6))

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

static tt_result_t __netif_group_link_update(IN tt_netif_group_t *group);

static tt_result_t __netif_link_update(IN tt_netif_t *netif, IN int skt);

static tt_result_t __netif_inet_update(IN tt_netif_t *netif,
                                       IN struct ifaddrs *ifa);

static tt_result_t __netif_inet6_update(IN tt_netif_t *netif,
                                        IN struct ifaddrs *ifa);

static tt_netif_type_t __type_map(IN sa_family_t hw_af);

// inet
static tt_netif_addr_t *__find_addr(IN tt_netif_t *netif,
                                    IN struct sockaddr *sockaddr);

static void __addr_update(IN tt_netif_addr_t *netif_addr,
                          IN struct ifaddrs *ifa);

// inet6
static tt_netif_addr_t *__find_addr6(IN tt_netif_t *netif,
                                     IN struct sockaddr *sockaddr);

static void __addr6_update(IN tt_netif_addr_t *netif_addr,
                           IN struct ifaddrs *ifa);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_netif_group_create_ntv(IN tt_netif_group_ntv_t *group)
{
    group->skt = socket(AF_INET, SOCK_DGRAM, 0);
    if (group->skt >= 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to create netif group socket");
        return TT_FAIL;
    }
}

void tt_netif_group_destroy_ntv(IN tt_netif_group_ntv_t *group)
{
    if (group->skt >= 0) {
        __RETRY_IF_EINTR(close(group->skt));
    }
}

tt_result_t tt_netif_group_refresh_ntv(IN tt_netif_group_t *group,
                                       IN tt_u32_t flag)
{
    struct ifaddrs *ifaddrs = NULL;
    struct ifaddrs *ifa = NULL;
    tt_u32_t internal_flag = 0;

    if (!TT_OK(__netif_group_link_update(group))) {
        return TT_FAIL;
    }

    if (getifaddrs(&ifaddrs) != 0) {
        TT_ERROR_NTV("getifaddrs failed");
        return TT_FAIL;
    }

    ifa = ifaddrs;
    while (ifa != NULL) {
        struct ifaddrs *cur_ifa;
        struct sockaddr *sa;
        tt_netif_t *netif;
        tt_result_t result;

        cur_ifa = ifa;
        ifa = ifa->ifa_next;

        if ((cur_ifa->ifa_name == NULL) || ((sa = cur_ifa->ifa_addr) == NULL) ||
            ((netif = tt_netif_group_find(group, cur_ifa->ifa_name)) == NULL)) {
            continue;
        }

        switch (sa->sa_family) {
            case AF_INET: {
                result = __netif_inet_update(netif, cur_ifa);
            } break;
            case AF_INET6: {
                if (group->flag & TT_NIFGRP_NO_IPV6_LINK_LOCAL) {
                    tt_u8_t *__u8 = (tt_u8_t *)(((struct sockaddr_in6 *)sa)
                                                    ->sin6_addr.s6_addr);
                    if ((__u8[0] == 0xFE) && (__u8[1] == 0x80)) {
                        continue;
                    }
                }

                result = __netif_inet6_update(netif, cur_ifa);
            } break;

            default: {
                continue;
            } break;
        }
        if (!TT_OK(result)) {
            TT_ERROR("netif[%s] is removed due to updating failed",
                     netif->name);
            tt_list_remove(&netif->node);
            tt_netif_destroy(netif);
        }
    }

    freeifaddrs(ifaddrs);

    return TT_SUCCESS;
}

tt_result_t tt_netif_create_ntv(IN tt_netif_ntv_t *sys_netif)
{
    sys_netif->skt = -1;

    return TT_SUCCESS;
}

void tt_netif_destroy_ntv(IN tt_netif_ntv_t *sys_netif)
{
    if (sys_netif->skt != -1) {
        __RETRY_IF_EINTR(close(sys_netif->skt));
    }
}

tt_result_t tt_netif_name2idx_ntv(IN const tt_char_t *name, OUT tt_u32_t *idx)
{
    unsigned int val = if_nametoindex(name);
    if (val != 0) {
        *idx = val;
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("invalid interface name: %s", name);
        return TT_E_BADARG;
    }
}

tt_result_t tt_netif_idx2name_ntv(IN tt_u32_t idx,
                                  OUT tt_char_t *name,
                                  IN tt_u32_t len)
{
    char ifname[IFNAMSIZ + 1] = {0};
    if (if_indextoname(idx, ifname) != NULL) {
        tt_u32_t n = (tt_u32_t)strlen(ifname);
        if (len > n) {
            tt_memcpy(name, ifname, n);
            name[n] = 0;
            return TT_SUCCESS;
        } else {
            TT_ERROR("not enough space for ifname");
            return TT_E_NOSPC;
        }
    } else {
        TT_ERROR_NTV("fail to get interface name: %d", idx);
        return TT_E_BADARG;
    }
}

tt_result_t __netif_group_link_update(IN tt_netif_group_t *group)
{
    tt_lnode_t *node;

    node = tt_list_head(&group->netif_list);
    while (node != NULL) {
        tt_netif_t *netif = TT_CONTAINER(node, tt_netif_t, node);

        node = node->next;

        if (!TT_OK(__netif_link_update(netif, group->sys_group.skt))) {
            TT_ERROR("fail to update link info of %s", netif->name);
            return TT_FAIL;
        }
    }

    return TT_SUCCESS;
}

tt_result_t __netif_link_update(IN tt_netif_t *netif, IN int skt)
{
    struct ifreq ifr;

    tt_strncpy(ifr.ifr_name, netif->name, sizeof(ifr.ifr_name) - 1);

    if (ioctl(skt, SIOCGIFMTU, &ifr, sizeof(ifr)) >= 0) {
        netif->mtu = ifr.ifr_mtu;
    }

    if (ioctl(skt, SIOCGIFHWADDR, &ifr, sizeof(ifr)) >= 0) {
        netif->type = __type_map(ifr.ifr_hwaddr.sa_family);
        tt_macaddr_init(&netif->macaddr, (tt_u8_t *)ifr.ifr_hwaddr.sa_data);
    }

    if (ioctl(skt, SIOCGIFFLAGS, &ifr, sizeof(ifr)) >= 0) {
        short flags = ifr.ifr_flags;
        tt_u32_t status;

        if (flags & IFF_RUNNING) {
            status = TT_NETIF_STATUS_ACTIVE;
        } else if (flags & IFF_UP) {
            status = TT_NETIF_STATUS_UP;
        } else {
            status = TT_NETIF_STATUS_DOWN;
        }
        if (netif->status != status) {
            netif->internal_flag |= TT_NETIF_DIFF_STATUS;
        }
        netif->status = status;

        netif->multicast = TT_BOOL(flags & IFF_MULTICAST);
        netif->loopback = TT_BOOL(flags & IFF_LOOPBACK);
        netif->broadcast = TT_BOOL(flags & IFF_BROADCAST);
        netif->p2p = TT_BOOL(flags & IFF_POINTOPOINT);
    }

    return TT_SUCCESS;
}

tt_result_t __netif_inet_update(IN tt_netif_t *netif, IN struct ifaddrs *ifa)
{
    tt_netif_addr_t *netif_addr;

    TT_ASSERT(ifa->ifa_addr->sa_family == AF_INET);

    netif_addr = __find_addr(netif, ifa->ifa_addr);
    if (netif_addr != NULL) {
        __addr_update(netif_addr, ifa);
        netif_addr->internal_flag |= __NETIF_INTERNAL_TOUCHED;
    } else {
        netif_addr = tt_netif_addr_create(TT_NET_AF_INET);
        if (netif_addr != NULL) {
            __addr_update(netif_addr, ifa);
            netif_addr->internal_flag |= __NETIF_INTERNAL_TOUCHED;
            tt_list_push_tail(&netif->addr_list, &netif_addr->node);
        } else {
            return TT_FAIL;
        }
    }

    return TT_SUCCESS;
}

tt_result_t __netif_inet6_update(IN tt_netif_t *netif, IN struct ifaddrs *ifa)
{
    tt_netif_addr_t *netif_addr;

    TT_ASSERT(ifa->ifa_addr->sa_family == AF_INET6);

    netif_addr = __find_addr6(netif, ifa->ifa_addr);
    if (netif_addr != NULL) {
        __addr6_update(netif_addr, ifa);
        netif_addr->internal_flag |= __NETIF_INTERNAL_TOUCHED;
    } else {
        netif_addr = tt_netif_addr_create(TT_NET_AF_INET6);
        if (netif_addr != NULL) {
            __addr6_update(netif_addr, ifa);
            netif_addr->internal_flag |= __NETIF_INTERNAL_TOUCHED;
            tt_list_push_tail(&netif->addr_list, &netif_addr->node);
        } else {
            return TT_FAIL;
        }
    }

    return TT_SUCCESS;
}

tt_netif_type_t __type_map(IN sa_family_t hw_af)
{
    switch (hw_af) {
        case ARPHRD_ETHER:
            return TT_NETIF_TYPE_ETHERNET;
        case ARPHRD_IEEE802:
            return TT_NETIF_TYPE_TOKEN_RING;
        case ARPHRD_DLCI:
            return TT_NETIF_TYPE_FRELAY;
        case ARPHRD_PPP:
            return TT_NETIF_TYPE_PPP;
        case ARPHRD_LOOPBACK:
            return TT_NETIF_TYPE_LOOPBACK;
        case ARPHRD_ATM:
            return TT_NETIF_TYPE_ATM;
        case ARPHRD_IEEE80211:
            return TT_NETIF_TYPE_IEEE80211;
        case ARPHRD_IEEE1394:
            return TT_NETIF_TYPE_IEEE1394;
        case ARPHRD_TUNNEL:
        case ARPHRD_TUNNEL6:
            return TT_NETIF_TYPE_TUNNEL;
        default:
            return TT_NETIF_TYPE_OTHER;
    }
}

// inet
tt_netif_addr_t *__find_addr(IN tt_netif_t *netif, IN struct sockaddr *sockaddr)
{
    tt_lnode_t *node = tt_list_head(&netif->addr_list);
    while (node != NULL) {
        tt_netif_addr_t *cur_addr = TT_CONTAINER(node, tt_netif_addr_t, node);

        node = node->next;

        if (sockaddr->sa_family == AF_INET) {
            if (__cmp_addr(cur_addr, sockaddr) == 0) {
                return cur_addr;
            }
        }
    }
    return NULL;
}

void __addr_update(IN tt_netif_addr_t *netif_addr, IN struct ifaddrs *ifa)
{
    if (__cmp_addr(&netif_addr->addr, ifa->ifa_addr) != 0) {
        __copy_addr(&netif_addr->addr, ifa->ifa_addr);
        netif_addr->internal_flag |= TT_NETIF_DIFF_ADDR;
    }

    if (__cmp_addr(&netif_addr->netmask, ifa->ifa_netmask) != 0) {
        __copy_addr(&netif_addr->netmask, ifa->ifa_netmask);
        netif_addr->internal_flag |= TT_NETIF_DIFF_ADDR;
    }

    if (__cmp_addr(&netif_addr->dstaddr, ifa->ifa_dstaddr) != 0) {
        __copy_addr(&netif_addr->dstaddr, ifa->ifa_dstaddr);
        netif_addr->internal_flag |= TT_NETIF_DIFF_ADDR;
    }
}

// inet6
tt_netif_addr_t *__find_addr6(IN tt_netif_t *netif,
                              IN struct sockaddr *sockaddr)
{
    tt_lnode_t *node = tt_list_head(&netif->addr_list);
    while (node != NULL) {
        tt_netif_addr_t *cur_addr = TT_CONTAINER(node, tt_netif_addr_t, node);

        node = node->next;

        if (sockaddr->sa_family == AF_INET6) {
            if (__cmp_addr6(cur_addr, sockaddr) == 0) {
                return cur_addr;
            }
        }
    }
    return NULL;
}

void __addr6_update(IN tt_netif_addr_t *netif_addr, IN struct ifaddrs *ifa)
{
    if (__cmp_addr6(&netif_addr->addr, ifa->ifa_addr) != 0) {
        __copy_addr6(&netif_addr->addr, ifa->ifa_addr);
        netif_addr->internal_flag |= TT_NETIF_DIFF_ADDR;
    }

    if (__cmp_addr6(&netif_addr->netmask, ifa->ifa_netmask) != 0) {
        __copy_addr6(&netif_addr->netmask, ifa->ifa_netmask);
        netif_addr->internal_flag |= TT_NETIF_DIFF_ADDR;
    }

    // ipv6, no broadcast addr
}
