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
@file tt_ssh_msg_channel_open.h
@brief ssh hello msg: channel open
*/

#ifndef __TT_SSH_MSGID_CHANNEL_OPEN__
#define __TT_SSH_MSGID_CHANNEL_OPEN__

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

struct tt_sshmsg_s;

typedef struct tt_sshmsg_chopen_s
{
    tt_ssh_chtype_t type;
    tt_u32_t snd_chnum;
    tt_u32_t window_size;
    tt_u32_t packet_size;
} tt_sshmsg_chopen_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export struct tt_sshmsg_s *tt_sshmsg_chopen_create();

tt_export void tt_sshmsg_chopen_set_type(IN struct tt_sshmsg_s *msg,
                                      IN tt_ssh_chtype_t type);

tt_export void tt_sshmsg_chopen_set_sndchnum(IN struct tt_sshmsg_s *msg,
                                          IN tt_u32_t snd_chnum);

tt_export void tt_sshmsg_chopen_set_winsize(IN struct tt_sshmsg_s *msg,
                                         IN tt_u32_t window_size);

tt_export void tt_sshmsg_chopen_set_pktsize(IN struct tt_sshmsg_s *msg,
                                         IN tt_u32_t packet_size);

#endif /* __TT_SSH_MSGID_CHANNEL_OPEN__ */
