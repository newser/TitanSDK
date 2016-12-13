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

#include <network/tt_network_interface.h>

#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <memory/tt_memory_alloc.h>

#include <tt_cstd_api.h>

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

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_network_interface_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __net_itf_component_init,
    };

    // init component
    tt_component_init(&comp,
                      TT_COMPONENT_NETWORK_INTERFACE,
                      "Network Interface",
                      NULL,
                      &itf);

    // register component
    tt_component_register(&comp);
}

tt_result_t __net_itf_component_init(IN tt_component_t *comp,
                                     IN tt_profile_t *profile)
{
    tt_result_t result = TT_FAIL;

    result = tt_network_interface_init_ntv();
    if (!TT_OK(result)) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

// ========================================
// netif group
// ========================================

void tt_netif_group_init(OUT tt_netif_group_t *group, IN tt_u32_t flag)
{
    TT_ASSERT(group != NULL);

    tt_list_init(&group->netif_list);

    group->flag = flag;
}

void tt_netif_group_destroy(IN tt_netif_group_t *group)
{
    tt_lnode_t *node;

    TT_ASSERT(group != NULL);

    while ((node = tt_list_pophead(&group->netif_list)) != NULL) {
        __netif_destroy(TT_CONTAINER(node, tt_netif_t, node));
    }
}

tt_result_t tt_netif_group_add(IN tt_netif_group_t *group,
                               IN const tt_char_t *netif_name)
{
    tt_netif_t *netif;

    TT_ASSERT(group != NULL);
    TT_ASSERT(netif_name != NULL);

    netif = __netif_find(group, netif_name);
    if (netif != NULL) {
        TT_INFO("netif[%s] already in group", netif_name);
        return TT_SUCCESS;
    }

    netif = __netif_create(netif_name);
    if (netif == NULL) {
        return TT_FAIL;
    }

    tt_list_addtail(&group->netif_list, &netif->node);

    return TT_SUCCESS;
}

void tt_netif_group_remove(IN tt_netif_group_t *group,
                           IN const tt_char_t *netif_name)
{
    tt_netif_t *netif;

    TT_ASSERT(group != NULL);
    TT_ASSERT(netif_name != NULL);

    netif = __netif_find(group, netif_name);
    if (netif != NULL) {
        tt_list_remove(&netif->node);
        __netif_destroy(netif);
    }
}

void tt_netif_group_refresh_prepare(IN tt_netif_group_t *group)
{
    tt_lnode_t *node;

    TT_ASSERT(group != NULL);

    node = tt_list_head(&group->netif_list);
    while (node != NULL) {
        tt_netif_t *netif = TT_CONTAINER(node, tt_netif_t, node);

        node = node->next;

        netif->internal_flag = 0;
        __netif_refresh_prepare(netif);
    }
}

tt_u32_t tt_netif_group_refresh_done(IN tt_netif_group_t *group)
{
    tt_u32_t internal_flag = 0;
    tt_lnode_t *node;

    TT_ASSERT(group != NULL);

    node = tt_list_head(&group->netif_list);
    while (node != NULL) {
        tt_netif_t *netif = TT_CONTAINER(node, tt_netif_t, node);

        node = node->next;

        __netif_refresh_done(netif);
        internal_flag |= TT_NETIF_DIFF(netif->internal_flag);
    }

    return internal_flag;
}

void tt_netif_group_dump(IN tt_netif_group_t *group)
{
    tt_lnode_t *node;

    TT_ASSERT(group != NULL);

    TT_INFO("network interface group: \n");

    node = tt_list_head(&group->netif_list);
    while (node != NULL) {
        __netif_dump(TT_CONTAINER(node, tt_netif_t, node), "  ");

        node = node->next;
    }
}

// ========================================
// netif
// ========================================

tt_netif_t *__netif_create(IN const tt_char_t *netif_name)
{
    tt_netif_t *netif;
    tt_u32_t name_len;

    TT_ASSERT(netif_name != NULL);

    name_len = (tt_u32_t)tt_strlen(netif_name);
    if (name_len > TT_NETIF_MAX_NAME_LEN) {
        TT_ERROR("netif name is too long");
        return NULL;
    }

    netif = (tt_netif_t *)tt_mem_alloc(sizeof(tt_netif_t));
    if (netif == NULL) {
        TT_ERROR("no mem for new netif");
        return NULL;
    }
    // tt_memset(netif, 0, sizeof(tt_netif_t));

    tt_lnode_init(&netif->node);
    netif->internal_flag = 0;

    if (!TT_OK(tt_netif_create_ntv(&netif->sys_netif))) {
        tt_mem_free(netif);
        return NULL;
    }

    tt_memcpy(netif->name, netif_name, name_len + 1);
    netif->status = TT_NETIF_STATUS_NOT_EXIST;
    tt_list_init(&netif->addr_list);

    return netif;
}

void __netif_destroy(IN tt_netif_t *netif)
{
    tt_lnode_t *node;

    tt_netif_destroy_ntv(&netif->sys_netif);

    while ((node = tt_list_pophead(&netif->addr_list)) != NULL) {
        __netif_addr_destroy(TT_CONTAINER(node, tt_netif_addr_t, node));
    }

    tt_mem_free(netif);
}

tt_netif_t *__netif_find(IN tt_netif_group_t *group,
                         IN const tt_char_t *netif_name)
{
    tt_lnode_t *node;

    TT_ASSERT(group != NULL);
    TT_ASSERT(netif_name != NULL);

    node = tt_list_head(&group->netif_list);
    while (node != NULL) {
        tt_netif_t *netif = TT_CONTAINER(node, tt_netif_t, node);

        node = node->next;

        if (tt_strncmp(netif->name, netif_name, TT_NETIF_MAX_NAME_LEN) == 0) {
            return netif;
        }
    }

    return NULL;
}

void __netif_refresh_prepare(IN tt_netif_t *netif)
{
    tt_lnode_t *node;

    TT_ASSERT(netif != NULL);

    node = tt_list_head(&netif->addr_list);
    while (node != NULL) {
        tt_netif_addr_t *addr = TT_CONTAINER(node, tt_netif_addr_t, node);

        node = node->next;

        addr->internal_flag = 0;
    }
}

void __netif_refresh_done(IN tt_netif_t *netif)
{
    tt_lnode_t *node;
    tt_netif_status_t new_status = TT_NETIF_STATUS_DOWN;

    TT_ASSERT(netif != NULL);

    node = tt_list_head(&netif->addr_list);
    while (node != NULL) {
        tt_netif_addr_t *addr = TT_CONTAINER(node, tt_netif_addr_t, node);

        node = node->next;

        if (!(addr->internal_flag & __NETIF_INTERNAL_TOUCHED)) {
            // this address does not appear in recent refreshing
            tt_list_remove(&addr->node);
            __netif_addr_destroy(addr);

            netif->internal_flag |= TT_NETIF_DIFF_ADDR;

            continue;
        }

        // 3 scenarios will all be covered:
        //  - new address appear
        //  - network changes(new gateway, new dns server, etc.)
        //  - network down
        netif->internal_flag |= TT_NETIF_DIFF(addr->internal_flag);
    }

    if (netif->internal_flag & __NETIF_INTERNAL_UP) {
        if (!tt_list_empty(&netif->addr_list)) {
            new_status = TT_NETIF_STATUS_ACTIVE;
        } else {
            // system indicates netif is up but not ip associated
            new_status = TT_NETIF_STATUS_UP;
        }
    }
    if (netif->status != new_status) {
        netif->status = new_status;
        netif->internal_flag |= TT_NETIF_DIFF_STATUS;
    }
}

const tt_char_t *__netif_status_str(IN tt_netif_status_t status)
{
    switch (status) {
        case TT_NETIF_STATUS_NOT_EXIST:
            return "not exist";
        case TT_NETIF_STATUS_NOT_CONFIGURED:
            return "not configured";
        case TT_NETIF_STATUS_DOWN:
            return "down";
        case TT_NETIF_STATUS_UP:
            return "up";
        case TT_NETIF_STATUS_ACTIVE:
            return "active";
        default:
            return "unknown";
    }
}

void __netif_dump(IN struct tt_netif_s *netif, IN const tt_char_t *prefix)
{
    tt_lnode_t *node;

    TT_INFO("%s[%s]", prefix, netif->name);
    TT_INFO("%sstatus: %s", prefix, __netif_status_str(netif->status));

    TT_INFO("%saddress:", prefix);
    node = tt_list_head(&netif->addr_list);
    while (node != NULL) {
        __netif_addr_dump(TT_CONTAINER(node, tt_netif_addr_t, node), "    ");

        node = node->next;
    }

    TT_INFO("");
}

tt_netif_addr_t *__netif_addr_create(IN tt_net_family_t family)
{
    tt_netif_addr_t *addr;

    addr = (tt_netif_addr_t *)tt_mem_alloc(sizeof(tt_netif_addr_t));
    if (addr == NULL) {
        TT_ERROR("no mem for new netif addr");
        return NULL;
    }
    // tt_memset(addr, 0, sizeof(tt_netif_addr_t));

    tt_lnode_init(&addr->node);
    addr->internal_flag = 0;

    tt_sktaddr_init(&addr->addr, family);

    return addr;
}

void __netif_addr_destroy(IN tt_netif_addr_t *netif_addr)
{
    TT_ASSERT(netif_addr != NULL);

    tt_mem_free(netif_addr);
}

void __netif_addr_dump(IN tt_netif_addr_t *netif_addr,
                       IN const tt_char_t *prefix)
{
    tt_char_t buf[200] = {0};

    tt_sktaddr_get_addr_p(&netif_addr->addr, buf, sizeof(buf) - 1);
    TT_INFO("%s- %-10s%s", prefix, "ip", buf);
    // TT_INFO("%s  %-10s", prefix, "gateway");

    // more?
}
