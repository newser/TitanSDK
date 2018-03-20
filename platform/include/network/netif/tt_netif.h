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
@file tt_netif.h
@brief network interface

this file defines network interface APIs
*/

#ifndef __TT_NETIF__
#define __TT_NETIF__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_list.h>
#include <io/tt_mac_addr.h>

#include <tt_netif_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_NETIF_MAX_NAME_LEN 31

#define TT_NETIF_DIFF_STATUS (1 << 0)
#define TT_NETIF_DIFF_ADDR (1 << 1)
#define TT_NETIF_DIFF(c) ((c)&0xFFFFFF)

#define __NETIF_INTERNAL_TOUCHED (1 << 24)
#define __NETIF_INTERNAL_UP (1 << 25)
#define __NETIF_INTERNAL_RUNNING (1 << 26)

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_netif_s;

typedef enum {
    TT_NETIF_TYPE_OTHER,
    TT_NETIF_TYPE_ETHERNET,
    TT_NETIF_TYPE_CSMA_CD,
    TT_NETIF_TYPE_TOKEN_RING,
    TT_NETIF_TYPE_FRELAY,
    TT_NETIF_TYPE_FDDI,
    TT_NETIF_TYPE_PPP,
    TT_NETIF_TYPE_LOOPBACK,
    TT_NETIF_TYPE_ATM,
    TT_NETIF_TYPE_IEEE80211,
    TT_NETIF_TYPE_IEEE1394,
    TT_NETIF_TYPE_TUNNEL,

    TT_NETIF_TYPE_NUM
} tt_netif_type_t;
#define TT_NETIF_TYPE_VALID(t) ((t) < TT_NETIF_TYPE_NUM)

typedef enum {
    TT_NETIF_STATUS_NOT_EXIST,
    TT_NETIF_STATUS_DOWN,
    TT_NETIF_STATUS_UP,
    TT_NETIF_STATUS_ACTIVE,

    TT_NETIF_STATUS_NUM
} tt_netif_status_t;
#define TT_NETIF_STATUS_VALID(s) ((s) < TT_NETIF_STATUS_NUM)

typedef struct tt_netif_s
{
    tt_lnode_t node;
    tt_u32_t internal_flag;
    tt_netif_ntv_t sys_netif;

    // info
    tt_list_t addr_list;
    tt_char_t name[TT_NETIF_MAX_NAME_LEN + 1];
    tt_u32_t index;
    tt_u32_t mtu;
    tt_macaddr_t macaddr;
    tt_netif_type_t type : 8;
    tt_netif_status_t status : 4;
    tt_bool_t multicast : 1;
    tt_bool_t broadcast : 1;
    tt_bool_t loopback : 1;
    tt_bool_t p2p : 1;
} tt_netif_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export void tt_network_interface_component_register();

tt_export tt_netif_t *tt_netif_create(IN const tt_char_t *netif_name,
                                      IN tt_u32_t index);

tt_export void tt_netif_destroy(IN tt_netif_t *netif);

tt_export void tt_netif_refresh_prepare(IN tt_netif_t *netif);

tt_export void tt_netif_refresh_done(IN tt_netif_t *netif);

tt_export void tt_netif_dump(IN tt_netif_t *netif, IN const tt_char_t *prefix);

tt_inline tt_result_t tt_netif_name2idx(IN const tt_char_t *name,
                                        OUT tt_u32_t *idx)
{
    return tt_netif_name2idx_ntv(name, idx);
}

tt_inline tt_result_t tt_netif_idx2name(IN tt_u32_t idx,
                                        OUT tt_char_t *name,
                                        IN tt_u32_t len)
{
    return tt_netif_idx2name_ntv(idx, name, len);
}

#endif /* __TT_NETIF__ */
