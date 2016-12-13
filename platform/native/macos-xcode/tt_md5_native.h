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
@file tt_md5_native.h
@brief crypto: MD5 native

this file defines MD5 native APIs
*/

#ifndef __TT_MD5_NATIVE__
#define __TT_MD5_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

#ifdef TSCM_PLATFORM_CRYPTO_ENABLE
#include <CommonCrypto/CommonDigest.h>
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
    CC_MD5_CTX md5;
#else
    tt_u32_t reserved;
#endif
} tt_md5_ntv_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_inline tt_result_t tt_md5_component_init_ntv(IN struct tt_profile_s *profile)
{
    return TT_SUCCESS;
}

extern tt_result_t tt_md5_create_ntv(IN tt_md5_ntv_t *sys_md5);

extern void tt_md5_destroy_ntv(IN tt_md5_ntv_t *sys_md5);

extern tt_result_t tt_md5_update_ntv(IN tt_md5_ntv_t *sys_md5,
                                     IN tt_u8_t *input,
                                     IN tt_u32_t input_len);

extern tt_result_t tt_md5_final_ntv(IN tt_md5_ntv_t *sys_md5,
                                    OUT tt_u8_t *output);

#endif /* __TT_MD5_NATIVE__ */
