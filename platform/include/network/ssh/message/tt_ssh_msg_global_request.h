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
@file tt_ssh_msg_global_request.h
@brief ssh msg: global request
*/

#ifndef __TT_SSH_MSGID_GLOBAL_REQUEST__
#define __TT_SSH_MSGID_GLOBAL_REQUEST__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_blob.h>

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
    tt_blob_t req_name;
    tt_bool_t want_reply;
} tt_sshmsg_glbreq_t;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern struct tt_sshmsg_s *tt_sshmsg_glbreq_create();

extern tt_result_t tt_sshmsg_glbreq_set_reqname(IN struct tt_sshmsg_s *msg,
                                                IN const tt_char_t *req_name);

extern void tt_sshmsg_glbreq_set_wantreply(IN struct tt_sshmsg_s *msg,
                                           IN tt_bool_t want_reply);

#endif /* __TT_SSH_MSGID_GLOBAL_REQUEST__ */
