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

#include <tt_network_interface_native.h>

#include <log/tt_log.h>
#include <memory/tt_memory_alloc.h>
#include <network/tt_network_interface.h>

#include <tt_cstd_api.h>
#include <tt_sys_error.h>
#include <tt_util_native.h>

#include <ifaddrs.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __cmp_addr(na, sa)                                                     \
    tt_memcmp(&((struct sockaddr_in *)&(na)->addr)->sin_addr,                  \
              &((struct sockaddr_in *)(sa))->sin_addr,                         \
              sizeof(struct in_addr))
#define __copy_addr(na, sa)                                                    \
    tt_memcpy(&(na)->addr, (sa), sizeof(struct sockaddr_in))

#define __cmp_addr6(na, sa)                                                    \
    tt_memcmp(&((struct sockaddr_in6 *)&(na)->addr)->sin6_addr,                \
              &((struct sockaddr_in6 *)(sa))->sin6_addr,                       \
              sizeof(struct in6_addr))
#define __copy_addr6(na, sa)                                                   \
    tt_memcpy(&(na)->addr, (sa), sizeof(struct sockaddr_in6))

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

// netif
static tt_result_t __netif_update(IN tt_netif_t *netif, IN struct ifaddrs *ifa);
static tt_netif_addr_t *tt_netif_group_find_addr(IN tt_netif_t *netif,
                                                 IN struct sockaddr *sockaddr);

// netif addr
static tt_result_t __netif_addr_update(IN tt_netif_addr_t *netif_addr,
                                       IN struct ifaddrs *ifa);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_netif_group_refresh_ntv(IN struct tt_netif_group_s *group,
                                       IN tt_u32_t flag)
{
    struct ifaddrs *ifaddrs = NULL;
    struct ifaddrs *ifa = NULL;
    tt_u32_t internal_flag = 0;

    if (getifaddrs(&ifaddrs) != 0) {
        TT_ERROR_NTV("getifaddrs failed");
        return TT_FAIL;
    }

    ifa = ifaddrs;
    while (ifa != NULL) {
        struct ifaddrs *cur_ifa;
        struct sockaddr *sa;
        tt_netif_t *netif;

        cur_ifa = ifa;
        ifa = ifa->ifa_next;

        if (cur_ifa->ifa_name == NULL) {
            continue;
        }

        sa = cur_ifa->ifa_addr;
        if ((sa == NULL) ||
            ((sa->sa_family != AF_INET) && (sa->sa_family != AF_INET6))) {
            continue;
        }

        if (group->flag & TT_NIFGRP_NO_IPV6_LINK_LOCAL) {
            tt_u8_t *__u8 = (tt_u8_t *)(((struct sockaddr_in6 *)sa)
                                            ->sin6_addr.__u6_addr.__u6_addr8);

            if ((sa->sa_family == AF_INET6) && (__u8[0] == 0xFE) &&
                (__u8[1] == 0x80)) {
                continue;
            }
        }

        netif = tt_netif_group_find(group, cur_ifa->ifa_name);
        if (netif == NULL) {
            continue;
        }

        if (!TT_OK(__netif_update(netif, cur_ifa))) {
            TT_ERROR("netif[%s] is removed due to updating failed",
                     netif->name);
            tt_list_remove(&netif->node);
            tt_netif_destroy(netif);

            continue;
        }
    }

    freeifaddrs(ifa);

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

tt_result_t __netif_update(IN tt_netif_t *netif, IN struct ifaddrs *ifa)
{
    tt_netif_addr_t *netif_addr;

    // update addresses

    netif_addr = tt_netif_group_find_addr(netif, ifa->ifa_addr);
    if (netif_addr != NULL) {
        if (!TT_OK(__netif_addr_update(netif_addr, ifa))) {
            tt_list_remove(&netif_addr->node);
            tt_netif_addr_destroy(netif_addr);
            return TT_FAIL;
        }
    } else {
        if (ifa->ifa_addr->sa_family == AF_INET) {
            netif_addr = tt_netif_addr_create(TT_NET_AF_INET);
        } else {
            TT_ASSERT(ifa->ifa_addr->sa_family == AF_INET6);
            netif_addr = tt_netif_addr_create(TT_NET_AF_INET6);
        }
        if (netif_addr == NULL) {
            return TT_FAIL;
        }

        if (!TT_OK(__netif_addr_update(netif_addr, ifa))) {
            tt_netif_addr_destroy(netif_addr);
            return TT_FAIL;
        }

        tt_list_push_tail(&netif->addr_list, &netif_addr->node);
    }

    // update status
    if (ifa->ifa_flags & IFF_UP) {
        netif->internal_flag |= __NETIF_INTERNAL_UP;
    } else {
        netif->internal_flag &= ~__NETIF_INTERNAL_UP;
    }

    return TT_SUCCESS;
}

tt_netif_addr_t *tt_netif_group_find_addr(IN tt_netif_t *netif,
                                          IN struct sockaddr *sockaddr)
{
    tt_lnode_t *node = tt_list_head(&netif->addr_list);
    while (node != NULL) {
        tt_netif_addr_t *cur_addr = TT_CONTAINER(node, tt_netif_addr_t, node);

        node = node->next;

        if (sockaddr->sa_family == AF_INET) {
            // ipv4, compare:
            //  - ip
            if (__cmp_addr(cur_addr, sockaddr) == 0) {
                return cur_addr;
            }
        } else {
            TT_ASSERT(sockaddr->sa_family == AF_INET6);

            // ipv6, compare:
            //  - ip
            //  - scope id??
            if (__cmp_addr6(cur_addr, sockaddr) == 0) {
                return cur_addr;
            }
        }
    }

    return NULL;
}

tt_result_t __netif_addr_update(IN tt_netif_addr_t *netif_addr,
                                IN struct ifaddrs *ifa)
{
    sa_family_t sa_family = ifa->ifa_addr->sa_family;

    netif_addr->internal_flag |= __NETIF_INTERNAL_TOUCHED;

    if (sa_family == AF_INET) {
        if (__cmp_addr(netif_addr, ifa->ifa_addr) != 0) {
            __copy_addr(netif_addr, ifa->ifa_addr);
            netif_addr->internal_flag |= TT_NETIF_DIFF_ADDR;
        }

        return TT_SUCCESS;
    } else {
        TT_ASSERT(sa_family == AF_INET6);

        if (__cmp_addr6(netif_addr, ifa->ifa_addr) != 0) {
            __copy_addr6(netif_addr, ifa->ifa_addr);
            netif_addr->internal_flag |= TT_NETIF_DIFF_ADDR;
        }

        // any other fields

        return TT_SUCCESS;
    }
}
