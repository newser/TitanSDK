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
@file tt_ssh_msg_userauth_request.h
@brief ssh hello msg: userauth request
*/

#ifndef __TT_SSH_MSGID_USERAUTH_REQUEST__
#define __TT_SSH_MSGID_USERAUTH_REQUEST__

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
    tt_ssh_pubkey_alg_t pubkey_alg;
    tt_blob_t pubkey;
    tt_blob_t signature;
} tt_ssh_auth_pubkey_t;

typedef struct
{
    tt_blob_t pwd;
    tt_blob_t new_pwd;
} tt_ssh_auth_pwd_t;

typedef struct
{
    tt_blob_t user;
    tt_ssh_service_t service;
    tt_ssh_auth_t auth;
    union
    {
        tt_ssh_auth_pubkey_t pubkey;
        tt_ssh_auth_pwd_t pwd;
    } auth_u;
} tt_sshmsg_uar_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern struct tt_sshmsg_s *tt_sshmsg_uar_create();

extern tt_result_t tt_sshmsg_uar_set_user(IN struct tt_sshmsg_s *msg,
                                          IN const tt_char_t *user);

extern void tt_sshmsg_uar_set_service(IN struct tt_sshmsg_s *msg,
                                      IN tt_ssh_service_t service);

extern void tt_sshmsg_uar_set_auth(IN struct tt_sshmsg_s *msg,
                                   IN tt_ssh_auth_t auth);

// ========================================
// auth: public key
// ========================================

extern tt_result_t tt_sshmsg_uar_set_pubkey_alg(IN struct tt_sshmsg_s *msg,
                                                IN tt_ssh_pubkey_alg_t alg);

extern tt_result_t tt_sshmsg_uar_set_pubkey(IN struct tt_sshmsg_s *msg,
                                            IN tt_u8_t *pubkey,
                                            IN tt_u32_t pubkey_len);

extern tt_result_t tt_sshmsg_uar_set_signature(IN struct tt_sshmsg_s *msg,
                                               IN tt_u8_t *sig,
                                               IN tt_u32_t sig_len);

// ========================================
// auth: password
// ========================================

extern tt_result_t tt_sshmsg_uar_set_pwd(IN struct tt_sshmsg_s *msg,
                                         IN const tt_char_t *pwd);

extern tt_result_t tt_sshmsg_uar_set_newpwd(IN struct tt_sshmsg_s *msg,
                                            IN const tt_char_t *pwd);

#endif /* __TT_SSH_MSGID_USERAUTH_REQUEST__ */
