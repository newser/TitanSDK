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
@file tt_netif_addr.h
@brief network interface address

this file defines network interface address APIs
*/

#ifndef __TT_NETIF_ADDR__
#define __TT_NETIF_ADDR__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_list.h>
#include <io/tt_socket_addr.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_netif_s;

typedef struct tt_netif_addr_s
{
    tt_lnode_t node;
    tt_u32_t internal_flag;

    tt_sktaddr_t addr;
    tt_sktaddr_t netmask;
    tt_sktaddr_t dstaddr;
} tt_netif_addr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_netif_addr_t *tt_netif_addr_create(IN tt_net_family_t family);

tt_export void tt_netif_addr_destroy(IN tt_netif_addr_t *netif_addr);

tt_export void tt_netif_addr_dump(IN struct tt_netif_s *netif,
                                  IN tt_netif_addr_t *netif_addr,
                                  IN const tt_char_t *prefix);

#endif /* __TT_NETIF_ADDR__ */
