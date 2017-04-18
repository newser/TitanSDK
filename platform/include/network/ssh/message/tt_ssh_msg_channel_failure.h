/* Licensed to the Apache Software Foundation (ASF) under one or more
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
 @file tt_ssh_msg_channel_failure.h
 @brief ssh msg: channel failure
 */

#ifndef __TT_SSH_MSGID_CHANNEL_FAILURE__
#define __TT_SSH_MSGID_CHANNEL_FAILURE__

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

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

typedef struct
{
    tt_u32_t rcv_chnum;
} tt_sshmsg_chfail_t;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern struct tt_sshmsg_s *tt_sshmsg_chfail_create();

extern void tt_sshmsg_chfail_set_rcvchnum(IN struct tt_sshmsg_s *msg,
                                          IN tt_u32_t rcv_chnum);

#endif /* __TT_SSH_MSGID_CHANNEL_FAILURE__ */
