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
@file tt_ssh_mac_sha.h
@brief ts ssh mac: SHA
*/

#ifndef __TT_SSH_MAC_SHA__
#define __TT_SSH_MAC_SHA__

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

struct tt_sshmac_s;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_result_t tt_sshmac_sha1_create(IN struct tt_sshmac_s *mac,
                                         IN tt_u8_t *key,
                                         IN tt_u32_t key_len);

tt_export void tt_sshmac_sha1_destroy(IN struct tt_sshmac_s *mac);

tt_export tt_result_t tt_sshmac_sha1_sign(IN struct tt_sshmac_s *mac,
                                       IN tt_u32_t seq_number,
                                       IN tt_u8_t *data,
                                       IN tt_u32_t data_len,
                                       OUT tt_u8_t *signature,
                                       IN tt_u32_t signature_len);
tt_export tt_result_t tt_sshmac_sha1_verify(IN struct tt_sshmac_s *mac,
                                         IN tt_u32_t seq_number,
                                         IN tt_u8_t *data,
                                         IN tt_u32_t data_len,
                                         OUT tt_u8_t *signature,
                                         IN tt_u32_t signature_len);

#endif /* __TT_SSH_MAC_SHA__ */
