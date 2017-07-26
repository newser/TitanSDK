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
@file tt_ssh_msg_disconnect.h
@brief ssh hello msg: disconnect
*/

#ifndef __TT_SSH_MSGID_DISCONNECT__
#define __TT_SSH_MSGID_DISCONNECT__

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

typedef struct
{
    tt_u32_t reason_code;

#define TT_SSHMSG_DISC_DESC_LEN 64
    tt_char_t description[TT_SSHMSG_DISC_DESC_LEN + 1];

} tt_sshmsg_disconnect_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export struct tt_sshmsg_s *tt_sshmsg_disconnect_create();

tt_export void tt_sshmsg_disconnect_set_reason(IN struct tt_sshmsg_s *msg,
                                            IN tt_u32_t reason_code);

tt_export void tt_sshmsg_disconnect_set_desc(IN struct tt_sshmsg_s *msg,
                                          IN const tt_char_t *description);

#endif /* __TT_SSH_MSGID_DISCONNECT__ */
