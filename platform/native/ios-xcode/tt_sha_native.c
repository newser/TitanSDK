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

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_sha_native.h>

#include <crypto/tt_sha.h>
#include <misc/tt_util.h>

#ifdef TTCM_PLATFORM_CRYPTO_ENABLE

#include <CommonCrypto/CommonCrypto.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef tt_result_t (*__sha_update_t)(IN tt_sha_ntv_t *sys_sha,
                                      IN tt_u8_t *input,
                                      IN tt_u32_t input_len);
typedef tt_result_t (*__sha_final_t)(IN tt_sha_ntv_t *sys_sha,
                                     OUT tt_u8_t *output);

typedef struct
{
    __sha_update_t update;
    __sha_final_t final;
} __sha_itf_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_result_t __sha1_update(IN tt_sha_ntv_t *sys_sha,
                                 IN tt_u8_t *input,
                                 IN tt_u32_t input_len);
static tt_result_t __sha1_final(IN tt_sha_ntv_t *sys_sha, OUT tt_u8_t *output);

static __sha_itf_t tt_s_sha_itf[TT_SHA_VER_NUM] = {
    // TT_SHA_VER_SHA1
    {
        __sha1_update, __sha1_final,
    }};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_sha_create_ntv(IN tt_sha_ntv_t *sys_sha, IN tt_sha_ver_t version)
{
    switch (version) {
        case TT_SHA_VER_SHA1: {
            CC_SHA1_Init(&sys_sha->sha1);
            return TT_SUCCESS;
        } break;

        default: {
            return TT_FAIL;
        } break;
    }
}

void tt_sha_destroy_ntv(IN tt_sha_ntv_t *sys_sha)
{
}

tt_result_t tt_sha_update_ntv(IN tt_sha_ntv_t *sys_sha,
                              IN tt_u8_t *input,
                              IN tt_u32_t input_len)
{
    tt_sha_t *sha = TT_CONTAINER(sys_sha, tt_sha_t, sys_sha);
    return tt_s_sha_itf[sha->version].update(sys_sha, input, input_len);
}

tt_result_t tt_sha_final_ntv(IN tt_sha_ntv_t *sys_sha, OUT tt_u8_t *output)
{
    tt_sha_t *sha = TT_CONTAINER(sys_sha, tt_sha_t, sys_sha);
    return tt_s_sha_itf[sha->version].final(sys_sha, output);
}

// TT_SHA_VER_SHA1
tt_result_t __sha1_update(IN tt_sha_ntv_t *sys_sha,
                          IN tt_u8_t *input,
                          IN tt_u32_t input_len)
{
    CC_SHA1_Update(&sys_sha->sha1, input, input_len);
    return TT_SUCCESS;
}

tt_result_t __sha1_final(IN tt_sha_ntv_t *sys_sha, OUT tt_u8_t *output)
{
    CC_SHA1_Final(output, &sys_sha->sha1);
    return TT_SUCCESS;
}

#endif
