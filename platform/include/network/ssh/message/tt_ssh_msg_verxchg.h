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
@file tt_ssh_msg_verxchg.h
@brief ssh msg: software version exchange
*/

#ifndef __TT_SSH_MSGID_VERXCHG__
#define __TT_SSH_MSGID_VERXCHG__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_blob.h>
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
    // protocol version
    tt_ssh_ver_t protover;
    // software version
    tt_blob_t swver;
    // comments
    tt_blob_t comment;
} tt_sshms_verxchg_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export struct tt_sshmsg_s *tt_sshms_verxchg_create();

tt_export void tt_sshms_verxchg_set_protover(IN struct tt_sshmsg_s *msg,
                                             IN tt_ssh_ver_t ver);

tt_export tt_result_t tt_sshms_verxchg_set_swver(IN struct tt_sshmsg_s *msg,
                                                 IN const tt_char_t *ver);

// pass null to clear comment
tt_export tt_result_t tt_sshms_verxchg_set_comment(
    IN struct tt_sshmsg_s *msg, IN OPT const tt_char_t *comment);

#endif /* __TT_SSH_MSGID_VERXCHG__ */
