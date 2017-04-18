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
@file tt_hmac.h
@brief crypto: HMAC

this file defines HMAC APIs
*/

#ifndef __TT_HMAC__
#define __TT_HMAC__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <crypto/tt_hmac_def.h>

#include <tt_hmac_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_buf_s;

typedef struct
{
    tt_hmac_ntv_t sys_hmac;
    tt_hmac_ver_t version;
} tt_hmac_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_inline tt_result_t tt_hmac_component_init(IN struct tt_profile_s *profile)
{
    return tt_hmac_component_init_ntv(profile);
}

extern tt_result_t tt_hmac_create(IN tt_hmac_t *hmac,
                                  IN tt_hmac_ver_t version,
                                  IN tt_blob_t *key);

extern void tt_hmac_destroy(IN tt_hmac_t *hmac);

extern tt_result_t tt_hmac_update(IN tt_hmac_t *hmac,
                                  IN tt_u8_t *input,
                                  IN tt_u32_t input_len);

// length of output must be at least of digest length
extern tt_result_t tt_hmac_final(IN tt_hmac_t *hmac, OUT tt_u8_t *output);
extern tt_result_t tt_hmac_final_buf(IN tt_hmac_t *hmac,
                                     OUT struct tt_buf_s *output);

extern tt_result_t tt_hmac_reset(IN tt_hmac_t *hmac);

extern tt_result_t tt_hmac_gather(IN tt_hmac_ver_t version,
                                  IN tt_blob_t *key,
                                  IN tt_blob_t *input,
                                  IN tt_u32_t input_num,
                                  OUT tt_u8_t *output);

tt_inline tt_result_t tt_hmac(IN tt_hmac_ver_t version,
                              IN tt_blob_t *key,
                              IN tt_blob_t *input,
                              OUT tt_u8_t *output)
{
    return tt_hmac_gather(version, key, input, 1, output);
}

#endif /* __TT_HMAC__ */
