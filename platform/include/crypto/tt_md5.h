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
@file tt_md5.h
@brief crypto: SHA

this file defines SHA APIs
*/

#ifndef __TT_MD5__
#define __TT_MD5__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_md5_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_MD5_DIGEST_LENGTH 16

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_profile_s;
struct tt_buf_s;

typedef struct
{
    tt_md5_ntv_t sys_md5;
} tt_md5_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_inline tt_result_t tt_md5_component_init(IN struct tt_profile_s *profile)
{
    return tt_md5_component_init_ntv(profile);
}

extern tt_result_t tt_md5_create(IN tt_md5_t *md5);

extern void tt_md5_destroy(IN tt_md5_t *md5);

extern tt_result_t tt_md5_update(IN tt_md5_t *md5,
                                 IN tt_u8_t *input,
                                 IN tt_u32_t input_len);

// size of ouput must be at least TT_MD5_DIGEST_LENGTH
extern tt_result_t tt_md5_final(IN tt_md5_t *md5, OUT tt_u8_t *output);
extern tt_result_t tt_md5_final_buf(IN tt_md5_t *md5,
                                    OUT struct tt_buf_s *output);

#endif /* __TT_MD5__ */
