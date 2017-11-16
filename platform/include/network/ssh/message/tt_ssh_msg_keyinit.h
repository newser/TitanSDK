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
@file tt_ssh_msg_keyinit.h
@brief ssh hello msg: key init
*/

#ifndef __TT_SSH_MSG_KEYINIT__
#define __TT_SSH_MSG_KEYINIT__

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

struct _tt_sshmsg_;

typedef struct
{
    // kex_algorithms
    tt_ssh_kex_alg_t kex_alg[TT_SSH_KEX_ALG_NUM];
    tt_u32_t kex_alg_num;

    // server_host_key_algorithms
    tt_ssh_pubkey_alg_t pubkey_alg[TT_SSH_PUBKEY_ALG_NUM];
    tt_u32_t pubkey_alg_num;

    // encryption_algorithms_client_to_server
    tt_ssh_enc_alg_t enc_c2s[TT_SSH_ENC_ALG_NUM];
    tt_u32_t enc_c2s_num;
    // encryption_algorithms_server_to_client
    tt_ssh_enc_alg_t enc_s2c[TT_SSH_ENC_ALG_NUM];
    tt_u32_t enc_s2c_num;

    // mac_algorithms_client_to_server
    tt_ssh_mac_alg_t mac_c2s[TT_SSH_MAC_ALG_NUM];
    tt_u32_t mac_c2s_num;
    // mac_algorithms_server_to_client
    tt_ssh_mac_alg_t mac_s2c[TT_SSH_MAC_ALG_NUM];
    tt_u32_t mac_s2c_num;

    // compression_algorithms_client_to_server
    tt_ssh_cmprs_alg_t comp_c2s[TT_SSH_CMPRS_ALG_NUM];
    tt_u32_t comp_c2s_num;
    // compression_algorithms_server_to_client
    tt_ssh_cmprs_alg_t comp_s2c[TT_SSH_CMPRS_ALG_NUM];
    tt_u32_t comp_s2c_num;

} tt_sshms_keyinit_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export struct tt_sshmsg_s *tt_sshms_keyinit_create();

tt_export tt_result_t tt_sshms_keyinit_addalg_kex(IN struct tt_sshmsg_s *msg,
                                                  IN tt_ssh_kex_alg_t kex);

tt_export tt_result_t tt_sshms_keyinit_addalg_pubkey(
    IN struct tt_sshmsg_s *msg, IN tt_ssh_pubkey_alg_t pubkey);

tt_export tt_result_t tt_sshms_keyinit_addalg_enc(IN struct tt_sshmsg_s *msg,
                                                  IN tt_ssh_enc_alg_t enc,
                                                  IN tt_bool_t client2server);

tt_export tt_result_t tt_sshms_keyinit_addalg_mac(IN struct tt_sshmsg_s *msg,
                                                  IN tt_ssh_mac_alg_t mac,
                                                  IN tt_bool_t client2server);

tt_export tt_result_t tt_sshms_keyinit_addalg_comp(IN struct tt_sshmsg_s *msg,
                                                   IN tt_ssh_cmprs_alg_t comp,
                                                   IN tt_bool_t client2server);

#endif /* __TT_SSH_MSG_KEYINIT__ */
