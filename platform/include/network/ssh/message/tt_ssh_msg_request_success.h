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
@file tt_ssh_msg_request_success.h
@brief ssh msg: request success
*/

#ifndef __TT_SSH_MSGID_REQUEST_SUCCESS__
#define __TT_SSH_MSGID_REQUEST_SUCCESS__

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

struct tt_sshmsg_s;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

typedef struct
{
    tt_u32_t reserved;
} tt_sshmsg_reqsucc_t;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export struct tt_sshmsg_s *tt_sshmsg_reqsucc_create();

#endif /* __TT_SSH_MSGID_REQUEST_SUCCESS__ */
