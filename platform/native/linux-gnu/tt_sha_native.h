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
@file tt_sha_native.h
@brief crypto: SHA port layer

this file defines SHA port layer APIs
*/

#ifndef __TT_SHA_NATIVE__
#define __TT_SHA_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <crypto/tt_sha_def.h>
#include <tt_basic_type.h>

#ifdef TSCM_PLATFORM_CRYPTO_ENABLE
#include <openssl/sha.h>
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
#ifdef TSCM_PLATFORM_CRYPTO_ENABLE
    union
    {
        SHA_CTX sha1;
    };
#else
    tt_u32_t reserved;
#endif
} tt_sha_ntv_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_inline tt_result_t tt_sha_component_init_ntv(IN struct tt_profile_s *profile)
{
    return TT_SUCCESS;
}

extern tt_result_t tt_sha_create_ntv(IN tt_sha_ntv_t *sys_sha,
                                     IN tt_sha_ver_t version);

extern void tt_sha_destroy_ntv(IN tt_sha_ntv_t *sys_sha);

extern tt_result_t tt_sha_update_ntv(IN tt_sha_ntv_t *sys_sha,
                                     IN tt_u8_t *input,
                                     IN tt_u32_t input_len);

extern tt_result_t tt_sha_final_ntv(IN tt_sha_ntv_t *sys_sha,
                                    OUT tt_u8_t *output);

#endif /* __TT_SHA_NATIVE__ */
