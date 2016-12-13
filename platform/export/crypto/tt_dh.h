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
@file tt_dh.h
@brief crypto: diffie-hellman

this file defines diffie-hellman APIs
*/

#ifndef __TT_DH__
#define __TT_DH__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <crypto/tt_dh_def.h>

#include <tt_dh_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_profile_s;
struct tt_buf_s;

typedef struct tt_dh_attr_s
{
    const tt_char_t *password;

    tt_bool_t pem_armor : 1;
} tt_dh_attr_t;

typedef struct
{
    tt_dh_ntv_t sys_dh;

    tt_dh_attr_t attr;
} tt_dh_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_inline tt_result_t tt_dh_component_init(IN struct tt_profile_s *profile)
{
    return tt_dh_component_init_ntv(profile);
}

extern tt_result_t tt_dh_create(IN tt_dh_t *dh,
                                IN tt_dh_format_t format,
                                IN tt_dh_keydata_t *keydata,
                                IN tt_dh_attr_t *attr);

extern void tt_dh_destroy(IN tt_dh_t *dh);

extern void tt_dh_attr_default(IN tt_dh_attr_t *attr);

extern tt_result_t tt_dh_get_pubkey(IN tt_dh_t *dh,
                                    OUT OPT tt_u8_t *pubkey,
                                    IN OUT tt_u32_t *pubkey_len,
                                    IN tt_u32_t flag);
extern tt_result_t tt_dh_get_pubkey_buf(IN tt_dh_t *dh,
                                        OUT struct tt_buf_s *pubkey,
                                        IN tt_u32_t flag);

extern tt_result_t tt_dh_compute(IN tt_dh_t *dh,
                                 IN tt_u8_t *peer_pub,
                                 IN tt_u32_t peer_pub_len);

extern tt_result_t tt_dh_get_secret(IN tt_dh_t *dh,
                                    OUT OPT tt_u8_t *secret,
                                    IN OUT tt_u32_t *secret_len,
                                    IN tt_u32_t flag);
extern tt_result_t tt_dh_get_secret_buf(IN tt_dh_t *dh,
                                        OUT struct tt_buf_s *secret,
                                        IN tt_u32_t flag);

#endif
