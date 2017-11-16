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
@file tt_ssh_channel_manager.h
@brief ts ssh channel manager
*/

#ifndef __TT_SSH_CHANNEL_MANAGER__
#define __TT_SSH_CHANNEL_MANAGER__

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

struct tt_sshchmgr_s;
struct tt_sshmsg_s;
struct tt_sshch_s;

typedef tt_result_t (*tt_sshchmgr_send_t)(IN struct tt_sshchmgr_s *chmgr,
                                          IN struct tt_sshmsg_s *msg);

typedef tt_result_t (*tt_sshchmgr_extra_size_t)(IN struct tt_sshchmgr_s *chmgr,
                                                OUT tt_u32_t *head_mac,
                                                OUT tt_u32_t *block_size);

typedef void (*tt_sshchmgr_on_ch_destroy_t)(IN struct tt_sshchmgr_s *chmgr,
                                            IN struct tt_sshch_s *ch);

typedef struct tt_sshchmgr_itf_s
{
    tt_sshchmgr_send_t send;
    tt_sshchmgr_extra_size_t extra_size;

    tt_sshchmgr_on_ch_destroy_t on_ch_destroy;
} tt_sshchmgr_itf_t;

typedef struct tt_sshchmgr_s
{
    tt_sshchmgr_itf_t *itf;
} tt_sshchmgr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_result_t tt_sshchmgr_send(IN tt_sshchmgr_t *chmgr,
                                       IN struct tt_sshmsg_s *msg);

tt_export tt_result_t tt_sshchmgr_extra_size(IN struct tt_sshchmgr_s *chmgr,
                                             OUT tt_u32_t *head_mac,
                                             OUT tt_u32_t *block_size);

tt_export void tt_sshchmgr_on_ch_destroy(IN tt_sshchmgr_t *chmgr,
                                         IN struct tt_sshch_s *ch);

#endif /* __TT_SSH_CHANNEL_MANAGER__ */
