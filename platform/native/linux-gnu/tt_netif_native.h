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
@file tt_neif_native.h
@brief network interface native

this file includes network interface native
*/

#ifndef __TT_NETIF_NATIVE__
#define __TT_NETIF_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_netif_group_s;

typedef struct
{
    int skt;
} tt_netif_group_ntv_t;

typedef struct
{
    int skt;
} tt_netif_ntv_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_inline tt_result_t tt_network_interface_init_ntv()
{
    return TT_SUCCESS;
}

tt_inline void tt_network_interface_exit_ntv()
{
}

extern tt_result_t tt_netif_group_create_ntv(IN tt_netif_group_ntv_t *group);

extern void tt_netif_group_destroy_ntv(IN tt_netif_group_ntv_t *group);

extern tt_result_t tt_netif_group_refresh_ntv(IN struct tt_netif_group_s *group,
                                              IN tt_u32_t flag);

extern tt_result_t tt_netif_create_ntv(IN tt_netif_ntv_t *sys_netif);

extern void tt_netif_destroy_ntv(IN tt_netif_ntv_t *sys_netif);

extern tt_result_t tt_netif_name2idx_ntv(IN const tt_char_t *name,
                                         OUT tt_u32_t *idx);

extern tt_result_t tt_netif_idx2name_ntv(IN tt_u32_t idx,
                                         OUT tt_char_t *name,
                                         IN tt_u32_t len);

#endif /* __TT_NETIF_NATIVE__ */
