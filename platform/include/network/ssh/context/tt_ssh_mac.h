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
@file tt_ssh_mac.h
@brief ts ssh mac
*/

#ifndef __TT_SSH_MAC__
#define __TT_SSH_MAC__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <crypto/tt_hmac.h>
#include <network/ssh/tt_ssh_def.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct tt_sshmac_s
{
    tt_ssh_mac_alg_t alg;
    tt_hmac_t hmac;

    tt_u32_t mac_len;
} tt_sshmac_t;

typedef tt_result_t (*tt_sshmac_create_t)(IN tt_sshmac_t *mac, IN tt_u8_t *key,
                                          IN tt_u32_t key_len);
typedef void (*tt_sshmac_destroy_t)(IN tt_sshmac_t *mac);

typedef tt_result_t (*tt_sshmac_sign_t)(IN tt_sshmac_t *mac,
                                        IN tt_u32_t seq_number,
                                        IN tt_u8_t *data, IN tt_u32_t data_len,
                                        OUT tt_u8_t *signature,
                                        IN tt_u32_t signature_len);
typedef tt_result_t (*tt_sshmac_verify_t)(
    IN tt_sshmac_t *mac, IN tt_u32_t seq_number, IN tt_u8_t *data,
    IN tt_u32_t data_len, OUT tt_u8_t *signature, IN tt_u32_t signature_len);

typedef struct
{
    tt_sshmac_create_t create;
    tt_sshmac_destroy_t destroy;

    tt_sshmac_sign_t sign;
    tt_sshmac_verify_t verify;
} tt_sshmac_itf_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export void tt_sshmac_init(IN tt_sshmac_t *mac);

tt_export void tt_sshmac_destroy(IN tt_sshmac_t *mac);

tt_export tt_result_t tt_sshmac_setalg(IN tt_sshmac_t *mac,
                                       IN tt_ssh_mac_alg_t alg, IN tt_u8_t *key,
                                       IN tt_u32_t key_len);

tt_export tt_result_t tt_sshmac_sign(IN tt_sshmac_t *mac,
                                     IN tt_u32_t seq_number, IN tt_u8_t *data,
                                     IN tt_u32_t data_len,
                                     OUT tt_u8_t *signature,
                                     IN tt_u32_t signature_len);

tt_export tt_result_t tt_sshmac_verify(IN tt_sshmac_t *mac,
                                       IN tt_u32_t seq_number, IN tt_u8_t *data,
                                       IN tt_u32_t data_len,
                                       IN tt_u8_t *signature,
                                       IN tt_u32_t signature_len);

#endif /* __TT_SSH_MAC__ */
