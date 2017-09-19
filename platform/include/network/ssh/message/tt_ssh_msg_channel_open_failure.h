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
@file tt_ssh_msg_channel_open_failure.h
@brief ssh hello msg: channel open failure
*/

#ifndef __TT_SSH_MSGID_CHANNEL_OPEN_FAILURE__
#define __TT_SSH_MSGID_CHANNEL_OPEN_FAILURE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_blob.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_SSH_CHOPF_RC_ADMINISTRATIVELY_PROHIBITED 1
#define TT_SSH_CHOPF_RC_CONNECT_FAILED 2
#define TT_SSH_CHOPF_RC_UNKNOWN_CHANNEL_TYPE 3
#define TT_SSH_CHOPF_RC_RESOURCE_SHORTAGE 4

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_sshmsg_s;

typedef struct
{
    tt_u32_t rcv_chnum;
    tt_u32_t reason_code;
    tt_blob_t desc;
} tt_sshmsg_chopf_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export struct tt_sshmsg_s *tt_sshmsg_chopf_create();

tt_export void tt_sshmsg_chopf_set_rcvchnum(IN struct tt_sshmsg_s *msg,
                                            IN tt_u32_t rcv_chnum);

tt_export void tt_sshmsg_chopf_set_reason(IN struct tt_sshmsg_s *msg,
                                          IN tt_u32_t reason_code);

tt_export tt_result_t tt_sshmsg_chopf_set_desc(IN struct tt_sshmsg_s *msg,
                                               IN const tt_char_t *desc);

#endif /* __TT_SSH_MSGID_CHANNEL_OPEN_FAILURE__ */
