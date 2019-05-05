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

#include <network/netif/tt_netif.h>

#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <memory/tt_memory_alloc.h>
#include <network/netif/tt_netif_addr.h>

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

static tt_result_t __net_itf_component_init(IN tt_component_t *comp,
                                            IN tt_profile_t *profile);

static void __net_itf_component_exit(IN tt_component_t *comp);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_network_interface_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __net_itf_component_init,
        __net_itf_component_exit,
    };

    // init component
    tt_component_init(&comp, TT_COMPONENT_NETWORK_INTERFACE,
                      "Network Interface", NULL, &itf);

    // register component
    tt_component_register(&comp);
}

tt_result_t __net_itf_component_init(IN tt_component_t *comp,
                                     IN tt_profile_t *profile)
{
    if (!TT_OK(tt_network_interface_init_ntv())) { return TT_FAIL; }

    return TT_SUCCESS;
}

void __net_itf_component_exit(IN tt_component_t *comp)
{
    tt_network_interface_exit_ntv();
}

tt_netif_t *tt_netif_create(IN const tt_char_t *netif_name, IN tt_u32_t index)
{
    tt_netif_t *netif;
    tt_u32_t len;

    len = (tt_u32_t)tt_strlen(netif_name);
    if (len > TT_NETIF_MAX_NAME_LEN) {
        TT_ERROR("netif name is too long");
        return NULL;
    }

    netif = tt_malloc(sizeof(tt_netif_t));
    if (netif == NULL) {
        TT_ERROR("no mem for new netif");
        return NULL;
    }

    tt_lnode_init(&netif->node);
    netif->internal_flag = 0;

    if (!TT_OK(tt_netif_create_ntv(&netif->sys_netif))) {
        tt_free(netif);
        return NULL;
    }

    tt_list_init(&netif->addr_list);
    tt_memcpy(netif->name, netif_name, len + 1);
    netif->index = index;
    netif->mtu = 0;
    tt_macaddr_init(&netif->macaddr, NULL);
    netif->type = TT_NETIF_TYPE_OTHER;
    netif->status = TT_NETIF_STATUS_NOT_EXIST;
    netif->multicast = TT_FALSE;
    netif->broadcast = TT_FALSE;
    netif->loopback = TT_FALSE;
    netif->p2p = TT_FALSE;

    return netif;
}

void tt_netif_destroy(IN tt_netif_t *netif)
{
    tt_lnode_t *node;

    tt_netif_destroy_ntv(&netif->sys_netif);

    while ((node = tt_list_pop_head(&netif->addr_list)) != NULL) {
        tt_netif_addr_destroy(TT_CONTAINER(node, tt_netif_addr_t, node));
    }

    tt_free(netif);
}

void tt_netif_refresh_prepare(IN tt_netif_t *netif)
{
    tt_lnode_t *node;

    node = tt_list_head(&netif->addr_list);
    while (node != NULL) {
        tt_netif_addr_t *addr = TT_CONTAINER(node, tt_netif_addr_t, node);

        node = node->next;

        addr->internal_flag = 0;
    }
}

void tt_netif_refresh_done(IN tt_netif_t *netif)
{
    tt_lnode_t *node;

    node = tt_list_head(&netif->addr_list);
    while (node != NULL) {
        tt_netif_addr_t *addr = TT_CONTAINER(node, tt_netif_addr_t, node);

        node = node->next;

        if (!(addr->internal_flag & __NETIF_INTERNAL_TOUCHED)) {
            // this address does not appear in recent refreshing
            tt_list_remove(&addr->node);
            tt_netif_addr_destroy(addr);

            netif->internal_flag |= TT_NETIF_DIFF_ADDR;

            continue;
        }

        netif->internal_flag |= TT_NETIF_DIFF(addr->internal_flag);
    }
}

const tt_char_t *__netif_status_name(IN tt_netif_status_t status)
{
    switch (status) {
    case TT_NETIF_STATUS_NOT_EXIST: return "not exist";
    case TT_NETIF_STATUS_DOWN: return "down";
    case TT_NETIF_STATUS_UP: return "up";
    case TT_NETIF_STATUS_ACTIVE: return "active";
    default: return "unknown";
    }
}

void tt_netif_dump(IN struct tt_netif_s *netif, IN const tt_char_t *prefix)
{
    tt_lnode_t *node;

    TT_INFO("%s%s:<%s%s%s%s> mtu %d", prefix, netif->name,
            TT_COND(netif->multicast, "MULTICAST,", ""),
            TT_COND(netif->broadcast, "BROADCAST,", ""),
            TT_COND(netif->loopback, "LOOPBACK,", ""),
            TT_COND(netif->p2p, "POINT2POINT,", ""), netif->mtu);

    if (netif->type == TT_NETIF_TYPE_ETHERNET) {
        tt_char_t buf[20];
        tt_macaddr_n2p(&netif->macaddr, buf, sizeof(buf), 0);
        TT_INFO("%sether %s", prefix, buf);
    }

    node = tt_list_head(&netif->addr_list);
    while (node != NULL) {
        tt_netif_addr_dump(netif, TT_CONTAINER(node, tt_netif_addr_t, node),
                           prefix);
        node = node->next;
    }

    TT_INFO("%sstatus: %s", prefix, __netif_status_name(netif->status));
    TT_INFO("");
}
