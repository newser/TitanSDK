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
@file tt_ssh_channel_cb.h
@brief ts ssh channel callback
*/

#ifndef __TT_SSH_CHANNEL_CB__
#define __TT_SSH_CHANNEL_CB__

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

struct tt_sshch_s;
struct tt_sshmsg_chopen_s;

typedef void (*tt_sshch_on_connect_t)(IN struct tt_sshch_s *ch,
                                      IN struct tt_sshmsg_chopen_s *chopen);

typedef void (*tt_sshch_on_accept_t)(IN struct tt_sshch_s *ch,
                                     IN struct tt_sshmsg_chopen_s *chopen);

typedef void (*tt_sshch_on_recv_t)(IN struct tt_sshch_s *ch, IN tt_u8_t *data,
                                   IN tt_u32_t data_len);

typedef struct tt_sshch_cb_s
{
    tt_sshch_on_connect_t on_connect;
    tt_sshch_on_accept_t on_accept;
    tt_sshch_on_recv_t on_recv;
} tt_sshch_cb_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif /* __TT_SSH_CHANNEL_CB__ */
