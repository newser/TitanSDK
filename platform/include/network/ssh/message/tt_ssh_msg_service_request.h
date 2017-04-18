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
@file tt_ssh_msg_service_request.h
@brief ssh hello msg: service request
*/

#ifndef __TT_SSH_MSGID_SERVICE_REQUEST__
#define __TT_SSH_MSGID_SERVICE_REQUEST__

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
    tt_ssh_service_t service;
} tt_sshmsg_servreq_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern struct tt_sshmsg_s *tt_sshmsg_servreq_create();

extern void tt_sshmsg_servreq_set_service(IN struct tt_sshmsg_s *msg,
                                          IN tt_ssh_service_t service);

extern tt_ssh_service_t tt_sshmsg_servreq_get_service(
    IN struct tt_sshmsg_s *msg);

#endif /* __TT_SSH_MSGID_SERVICE_REQUEST__ */
