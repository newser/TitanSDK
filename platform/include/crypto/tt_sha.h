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
@file tt_sha.h
@brief crypto: SHA

this file defines SHA APIs
*/

#ifndef __TT_SHA__
#define __TT_SHA__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <crypto/tt_sha_def.h>

#include <tt_sha_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_profile_s;
struct tt_buf_s;

typedef struct
{
    tt_sha_ntv_t sys_sha;
    tt_sha_ver_t version;
} tt_sha_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_inline tt_result_t tt_sha_component_init(IN struct tt_profile_s *profile)
{
    return tt_sha_component_init_ntv(profile);
}

extern tt_result_t tt_sha_create(IN tt_sha_t *sha, IN tt_sha_ver_t version);

extern void tt_sha_destroy(IN tt_sha_t *sha);

extern tt_result_t tt_sha_update(IN tt_sha_t *sha,
                                 IN tt_u8_t *input,
                                 IN tt_u32_t input_len);

// size of ouput must be at least the digest length
extern tt_result_t tt_sha_final(IN tt_sha_t *sha, OUT tt_u8_t *output);
extern tt_result_t tt_sha_final_buf(IN tt_sha_t *sha,
                                    OUT struct tt_buf_s *output);

#endif /* __TT_SHA__ */
