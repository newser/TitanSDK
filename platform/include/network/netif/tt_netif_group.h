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

/**
@file tt_netif_group.h
@brief network interface group

this file defines network interface group APIs
*/

#ifndef __TT_NETIF_GROUP__
#define __TT_NETIF_GROUP__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_list.h>
#include <misc/tt_util.h>
#include <network/netif/tt_netif.h>

#include <tt_netif_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_netif_s;

typedef struct tt_netif_group_s
{
    tt_list_t netif_list;
    tt_netif_group_ntv_t sys_group;

    tt_u32_t flag;
#define TT_NIFGRP_NO_IPV6_LINK_LOCAL (1 << 0)

} tt_netif_group_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_result_t tt_netif_group_create(OUT tt_netif_group_t *group,
                                            IN tt_u32_t flag);

tt_export void tt_netif_group_destroy(IN tt_netif_group_t *group);

tt_export tt_result_t tt_netif_group_add(IN tt_netif_group_t *group,
                                         IN OPT const tt_char_t *netif_name);

tt_export void tt_netif_group_remove(IN tt_netif_group_t *group,
                                     IN OPT const tt_char_t *netif_name);

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
    tt_lnode_t *node = TT_COND(netif == NULL, tt_list_head(&group->netif_list),
                               netif->node.next);
    return TT_CONTAINER(node, tt_netif_t, node);
}

extern tt_netif_t *tt_netif_group_find(IN tt_netif_group_t *group,
                                       IN const tt_char_t *netif_name);

#endif /* __TT_NETIF_GROUP__ */
