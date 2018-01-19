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

#include <network/netif/tt_netif_addr.h>

#include <memory/tt_memory_alloc.h>
#include <network/netif/tt_netif.h>

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

tt_netif_addr_t *tt_netif_addr_create(IN tt_net_family_t family)
{
    tt_netif_addr_t *addr;

    addr = (tt_netif_addr_t *)tt_malloc(sizeof(tt_netif_addr_t));
    if (addr == NULL) {
        TT_ERROR("no mem for new netif addr");
        return NULL;
    }

    tt_lnode_init(&addr->node);
    addr->internal_flag = 0;

    tt_sktaddr_init(&addr->addr, TT_NET_AF_INET);
    tt_sktaddr_init(&addr->netmask, TT_NET_AF_INET);
    tt_sktaddr_init(&addr->dstaddr, TT_NET_AF_INET);

    return addr;
}

void tt_netif_addr_destroy(IN tt_netif_addr_t *netif_addr)
{
    tt_free(netif_addr);
}

void tt_netif_addr_dump(IN tt_netif_t *netif,
                        IN tt_netif_addr_t *netif_addr,
                        IN const tt_char_t *prefix)
{
    tt_net_family_t af = tt_sktaddr_get_family(&netif_addr->addr);
    tt_char_t buf1[200], buf2[200], buf3[200];

    if (af == TT_NET_AF_INET) {
        tt_sktaddr_get_ip_p(&netif_addr->addr, buf1, sizeof(buf1) - 1);
        tt_sktaddr_get_ip_p(&netif_addr->netmask, buf2, sizeof(buf2) - 1);
        tt_sktaddr_get_ip_p(&netif_addr->dstaddr, buf3, sizeof(buf3) - 1);
    } else if (af == TT_NET_AF_INET6) {
        tt_sktaddr_get_ip_p(&netif_addr->addr, buf1, sizeof(buf1) - 1);
    }

    if (netif->p2p) {
        if (af == TT_NET_AF_INET) {
            TT_INFO("%sinet %s --> %s netmask %s", prefix, buf1, buf3, buf2);
        } else if (af == TT_NET_AF_INET6) {
            struct sockaddr_in6 *sa6 = (struct sockaddr_in6 *)&netif_addr->addr;
            TT_INFO("%sinet6 %s scopeid %x", prefix, buf1, sa6->sin6_scope_id);
        }
    } else {
        if (af == TT_NET_AF_INET) {
            TT_INFO("%sinet %s netmask %s broadcast %s",
                    prefix,
                    buf1,
                    buf2,
                    buf3);
        } else if (af == TT_NET_AF_INET6) {
            struct sockaddr_in6 *sa6 = (struct sockaddr_in6 *)&netif_addr->addr;
            TT_INFO("%sinet6 %s scopeid %x", prefix, buf1, sa6->sin6_scope_id);
        }
    }
}
