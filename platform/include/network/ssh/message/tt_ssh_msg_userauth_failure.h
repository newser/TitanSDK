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
@file tt_ssh_msg_userauth_failure.h
@brief ssh hello msg: userauth failure
*/

#ifndef __TT_SSH_MSG_SERVICE_FAILURE__
#define __TT_SSH_MSG_SERVICE_FAILURE__

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

typedef struct
{
    tt_ssh_auth_t auth[TT_SSH_AUTH_NUM];
    tt_u32_t auth_num;

    tt_bool_t partial_success;
} tt_sshmsg_uaf_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export struct tt_sshmsg_s *tt_sshmsg_uaf_create();

tt_export void tt_sshmsg_uaf_add_auth(IN struct tt_sshmsg_s *msg,
                                      IN tt_ssh_auth_t auth);

tt_export void tt_sshmsg_uaf_set_parial_succ(IN struct tt_sshmsg_s *msg,
                                             IN tt_bool_t partial_success);

#endif /* __TT_SSH_MSG_SERVICE_FAILURE__ */
