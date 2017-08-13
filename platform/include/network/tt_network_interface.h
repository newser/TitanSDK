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
@file tt_network_interface.h
@brief network interface

this file defines network interface APIs
*/

#ifndef __TT_NETWORK_INTERFACE__
#define __TT_NETWORK_INTERFACE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_list.h>
#include <io/tt_socket_addr.h>
#include <misc/tt_util.h>

#include <tt_network_interface_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_NETIF_MAX_NAME_LEN 23

#define TT_NETIF_DIFF_STATUS (1 << 0)
#define TT_NETIF_DIFF_ADDR (1 << 1)
#define TT_NETIF_DIFF(c) ((c)&0xFFFFFF)

#define __NETIF_INTERNAL_TOUCHED (1 << 24)
#define __NETIF_INTERNAL_UP (1 << 25)

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_netif_s;

// netif status
typedef enum {
    TT_NETIF_STATUS_NOT_EXIST,
    TT_NETIF_STATUS_NOT_CONFIGURED, // exist, but not configured
    TT_NETIF_STATUS_DOWN, // configured, not enabled
    TT_NETIF_STATUS_UP, // enabled, not operating
    TT_NETIF_STATUS_ACTIVE,

    TT_NETIF_STATUS_NUM
} tt_netif_status_t;
#define TT_NETIF_STATUS_VALID(s) ((s) < TT_NETIF_STATUS_NUM)

// netif address, this includes ip address information of
// a netif, e.g. ip, netmask or dns server
typedef struct tt_netif_addr_s
{
    tt_lnode_t node;
    tt_u32_t internal_flag;

    tt_sktaddr_t addr;
    // may add more info such as netmask, gateway, dns, etc.
} tt_netif_addr_t;

// network interface
typedef struct tt_netif_s
{
    tt_lnode_t node;
    tt_u32_t internal_flag;
    tt_netif_ntv_t sys_netif;

    // info
    tt_char_t name[TT_NETIF_MAX_NAME_LEN + 1];
    tt_netif_status_t status;
    tt_list_t addr_list;
} tt_netif_t;

typedef struct tt_netif_group_s
{
    tt_list_t netif_list;

    tt_u32_t flag;
#define TT_NIFGRP_NO_IPV6_LINK_LOCAL (1 << 0)

} tt_netif_group_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export void tt_network_interface_component_register();

// ========================================
// netif group
// ========================================

tt_export void tt_netif_group_init(OUT tt_netif_group_t *group,
                                   IN tt_u32_t flag);

tt_export void tt_netif_group_destroy(IN tt_netif_group_t *group);

tt_export tt_result_t tt_netif_group_add(IN tt_netif_group_t *group,
                                         IN const tt_char_t *netif_name);

tt_export void tt_netif_group_remove(IN tt_netif_group_t *group,
                                     IN const tt_char_t *netif_name);

tt_export void tt_netif_group_refresh_prepare(IN tt_netif_group_t *group);

tt_inline void tt_netif_group_refresh(IN tt_netif_group_t *group,
                                      IN tt_u32_t flag)
{
    tt_netif_group_refresh_ntv(group, flag);
}

tt_export tt_u32_t tt_netif_group_refresh_done(IN tt_netif_group_t *group);

tt_export void tt_netif_group_dump(IN tt_netif_group_t *group);

tt_inline tt_netif_t *tt_netif_group_next(IN tt_netif_group_t *group,
                                          IN tt_netif_t *netif)
{
    tt_lnode_t *node = TT_COND(netif == NULL,
                               tt_list_head(&group->netif_list),
                               netif->node.next);
    return TT_CONTAINER(node, tt_netif_t, node);
}

// ========================================
// netif, auxiliary functions
// ========================================

tt_export tt_netif_t *__netif_create(IN const tt_char_t *netif_name);

tt_export void __netif_destroy(IN tt_netif_t *netif);

tt_export tt_netif_t *__netif_find(IN tt_netif_group_t *group,
                                   IN const tt_char_t *netif_name);

tt_export void __netif_refresh_prepare(IN tt_netif_t *netif);
tt_export void __netif_refresh_done(IN tt_netif_t *netif);

tt_export void __netif_dump(IN tt_netif_t *netif, IN const tt_char_t *prefix);

// netif address
tt_export tt_netif_addr_t *__netif_addr_create(IN tt_net_family_t family);

tt_export void __netif_addr_destroy(IN tt_netif_addr_t *netif_addr);

tt_export void __netif_addr_dump(IN tt_netif_addr_t *netif_addr,
                                 IN const tt_char_t *prefix);

#endif /* __TT_NETWORK_INTERFACE__ */
