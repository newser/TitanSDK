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

#include <crypto/tt_sha.h>

#include <algorithm/tt_buffer.h>
#include <misc/tt_assert.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_u32_t __sha_digest_len[TT_SHA_VER_NUM] = {
    TT_SHA1_DIGEST_LENGTH,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_sha_create(IN tt_sha_t *sha, IN tt_sha_ver_t version)
{
    TT_ASSERT(sha != NULL);
    TT_ASSERT(TT_SHA_VER_VALID(version));

    sha->version = version;

    return tt_sha_create_ntv(&sha->sys_sha, version);
}

void tt_sha_destroy(IN tt_sha_t *sha)
{
    tt_sha_destroy_ntv(&sha->sys_sha);
}

tt_result_t tt_sha_update(IN tt_sha_t *sha,
                          IN tt_u8_t *input,
                          IN tt_u32_t input_len)
{
    TT_ASSERT(sha != NULL);
    TT_ASSERT(input != NULL);

    if (input_len > 0) {
        return tt_sha_update_ntv(&sha->sys_sha, input, input_len);
    } else {
        return TT_SUCCESS;
    }
}

tt_result_t tt_sha_final_buf(IN tt_sha_t *sha, OUT tt_buf_t *output)
{
    tt_u32_t len = __sha_digest_len[sha->version];

    TT_ASSERT(sha != NULL);
    TT_ASSERT(output != NULL);

    if (!TT_OK(tt_buf_reserve(output, len))) {
        return TT_FAIL;
    }

    if (!TT_OK(tt_sha_final_ntv(&sha->sys_sha, TT_BUF_WPOS(output)))) {
        tt_buf_destroy(output);
        return TT_FAIL;
    }
    tt_buf_inc_wp(output, len);

    return TT_SUCCESS;
}

tt_result_t tt_sha_final(IN tt_sha_t *sha, OUT tt_u8_t *output)
{
    TT_ASSERT(sha != NULL);
    TT_ASSERT(output != NULL);

    return tt_sha_final_ntv(&sha->sys_sha, output);
}
