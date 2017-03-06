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
@file tt_hmac_native.h
@brief crypto: HMAC native

this file defines HMAC native APIs
*/

#ifndef __TT_HMAC_NATIVE__
#define __TT_HMAC_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <crypto/tt_hmac_def.h>
#include <tt_basic_type.h>

#ifdef TTCM_PLATFORM_CRYPTO_ENABLE
#include <bcrypt.h>
#endif

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_profile_s;

typedef struct
{
#ifdef TTCM_PLATFORM_CRYPTO_ENABLE
    BCRYPT_HASH_HANDLE h_hmac;
#endif
    tt_u8_t *mem;
    tt_u32_t size;

    tt_blob_t key;
} tt_hmac_ntv_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_hmac_component_init_ntv(IN struct tt_profile_s *profile);

extern tt_result_t tt_hmac_create_ntv(IN tt_hmac_ntv_t *sys_hmac,
                                      IN tt_hmac_ver_t version,
                                      IN tt_blob_t *key);

extern void tt_hmac_destroy_ntv(IN tt_hmac_ntv_t *sys_hmac);

extern tt_result_t tt_hmac_update_ntv(IN tt_hmac_ntv_t *sys_hmac,
                                      IN tt_u8_t *input,
                                      IN tt_u32_t input_len);

extern tt_result_t tt_hmac_final_ntv(IN tt_hmac_ntv_t *sys_hmac,
                                     OUT tt_u8_t *output);

extern tt_result_t tt_hmac_reset_ntv(IN tt_hmac_ntv_t *sys_hmac);

#endif /* __TT_HMAC_NATIVE__ */
