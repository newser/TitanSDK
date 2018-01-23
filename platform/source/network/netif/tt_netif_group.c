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

#include <network/netif/tt_netif_group.h>

#include <algorithm/tt_algorithm_def.h>

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

    while ((node = tt_list_pop_head(&group->netif_list)) != NULL) {
        tt_netif_destroy(TT_CONTAINER(node, tt_netif_t, node));
    }
}

tt_result_t tt_netif_group_add(IN tt_netif_group_t *group,
                               IN OPT const tt_char_t *netif_name)
{
    tt_netif_t *netif;
    tt_u32_t idx;

    TT_ASSERT(group != NULL);

    if (netif_name == NULL) {
        // todo: enum all netif name and add to group
        return TT_FAIL;
    }

    netif = tt_netif_group_find(group, netif_name);
    if (netif != NULL) {
        TT_ERROR("netif[%s] already in group", netif_name);
        return TT_E_EXIST;
    }

    if (!TT_OK(tt_netif_name2idx(netif_name, &idx))) {
#if 0
        TT_ERROR("invalid interface name: %s", netif_name);
        return TT_E_BADARG;
#else
        idx = TT_POS_NULL;
#endif
    }

    netif = tt_netif_create(netif_name, idx);
    if (netif == NULL) {
        return TT_FAIL;
    }

    tt_list_push_tail(&group->netif_list, &netif->node);

    return TT_SUCCESS;
}

void tt_netif_group_remove(IN tt_netif_group_t *group,
                           IN OPT const tt_char_t *netif_name)
{
    tt_netif_t *netif;

    TT_ASSERT(group != NULL);

    if (netif_name != NULL) {
        netif = tt_netif_group_find(group, netif_name);
        if (netif != NULL) {
            tt_list_remove(&netif->node);
            tt_netif_destroy(netif);
        }
    } else {
        tt_lnode_t *node;
        while ((node = tt_list_pop_head(&group->netif_list)) != NULL) {
            tt_netif_destroy(TT_CONTAINER(node, tt_netif_t, node));
        }
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
        tt_netif_refresh_prepare(netif);
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

        tt_netif_refresh_done(netif);
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
        tt_netif_dump(TT_CONTAINER(node, tt_netif_t, node), "  ");

        node = node->next;
    }
}

tt_netif_t *tt_netif_group_find(IN tt_netif_group_t *group,
                                IN const tt_char_t *netif_name)
{
    tt_lnode_t *node;

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
