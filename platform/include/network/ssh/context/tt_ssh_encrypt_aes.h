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
@file tt_ssh_encrypt_aes.h
@brief ts ssh encrypt & decrypt: AES
*/

#ifndef __TT_SSH_AES__
#define __TT_SSH_AES__

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

struct tt_sshenc_s;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_result_t tt_sshenc_aes_create_128cbc(
    IN struct tt_sshenc_s *enc, IN tt_bool_t encrypt, IN tt_u8_t *iv,
    IN tt_u32_t iv_len, IN tt_u8_t *key, IN tt_u32_t key_leny);

tt_export tt_result_t tt_sshenc_aes_create_256cbc(
    IN struct tt_sshenc_s *enc, IN tt_bool_t encrypt, IN tt_u8_t *iv,
    IN tt_u32_t iv_len, IN tt_u8_t *key, IN tt_u32_t key_len);

tt_export void tt_sshenc_aes_destroy(IN struct tt_sshenc_s *enc);

tt_export tt_result_t tt_sshenc_aes_encrypt(IN struct tt_sshenc_s *enc,
                                            IN OUT tt_u8_t *data,
                                            IN tt_u32_t data_len);

tt_export tt_result_t tt_sshenc_aes_decrypt(IN struct tt_sshenc_s *enc,
                                            IN OUT tt_u8_t *data,
                                            IN tt_u32_t data_len);

#endif /* __TT_SSH_AES__ */
