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
@file tt_ssh_msg_channel_request.h
@brief ssh msg: channel request
*/

#ifndef __TT_SSH_MSGID_CHANNEL_REQUEST__
#define __TT_SSH_MSGID_CHANNEL_REQUEST__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <network/ssh/tt_ssh_def.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

struct tt_sshmsg_s;

typedef struct
{
    tt_u32_t rcv_chnum;
    tt_ssh_chreqtype_t req_type;
    tt_bool_t want_reply;
} tt_sshmsg_chreq_t;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export struct tt_sshmsg_s *tt_sshmsg_chreq_create();

tt_export void tt_sshmsg_chreq_set_rcvchnum(IN struct tt_sshmsg_s *msg,
                                            IN tt_u32_t rcv_chnum);

tt_export void tt_sshmsg_chreq_set_reqtype(IN struct tt_sshmsg_s *msg,
                                           IN tt_ssh_chreqtype_t req_type);

tt_export void tt_sshmsg_chreq_set_wantreply(IN struct tt_sshmsg_s *msg,
                                             IN tt_bool_t want_reply);

#endif /* __TT_SSH_MSGID_CHANNEL_REQUEST__ */
