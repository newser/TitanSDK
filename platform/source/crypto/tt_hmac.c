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

#include <crypto/tt_hmac.h>

#include <algorithm/tt_buffer.h>
#include <init/tt_component.h>
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

static tt_u32_t __hmac_digest_len[TT_HMAC_VER_NUM] = {
    TT_HMAC_SHA1_DIGEST_LENGTH,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_hmac_create(IN tt_hmac_t *hmac,
                           IN tt_hmac_ver_t version,
                           IN tt_blob_t *key)
{
    TT_ASSERT(hmac != NULL);
    TT_ASSERT(TT_HMAC_VER_VALID(version));
    TT_ASSERT((key != NULL) && (key->addr != NULL) && (key->len != 0));

    hmac->version = version;

    return tt_hmac_create_ntv(&hmac->sys_hmac, version, key);
}

void tt_hmac_destroy(IN tt_hmac_t *hmac)
{
    tt_hmac_destroy_ntv(&hmac->sys_hmac);
}

tt_result_t tt_hmac_update(IN tt_hmac_t *hmac,
                           IN tt_u8_t *input,
                           IN tt_u32_t input_len)
{
    TT_ASSERT(hmac != NULL);
    TT_ASSERT(input != NULL);

    if (input > 0) {
        return tt_hmac_update_ntv(&hmac->sys_hmac, input, input_len);
    } else {
        return TT_SUCCESS;
    }
}

tt_result_t tt_hmac_final_buf(IN tt_hmac_t *hmac, OUT tt_buf_t *output)
{
    tt_u32_t len = __hmac_digest_len[hmac->version];

    TT_ASSERT(hmac != NULL);
    TT_ASSERT(output != NULL);

    if (!TT_OK(tt_buf_reserve(output, len))) {
        return TT_FAIL;
    }

    if (!TT_OK(tt_hmac_final_ntv(&hmac->sys_hmac, TT_BUF_WPOS(output)))) {
        tt_buf_destroy(output);
        return TT_FAIL;
    }
    tt_buf_inc_wp(output, len);

    return TT_SUCCESS;
}

tt_result_t tt_hmac_reset(IN tt_hmac_t *hmac)
{
    TT_ASSERT(hmac != NULL);

    return tt_hmac_reset_ntv(&hmac->sys_hmac);
}

tt_result_t tt_hmac_final(IN tt_hmac_t *hmac, OUT tt_u8_t *output)
{
    TT_ASSERT(hmac != NULL);
    TT_ASSERT(output != NULL);

    return tt_hmac_final_ntv(&hmac->sys_hmac, output);
}

tt_result_t tt_hmac_gather(IN tt_hmac_ver_t version,
                           IN tt_blob_t *key,
                           IN tt_blob_t *input,
                           IN tt_u32_t input_num,
                           OUT tt_u8_t *output)
{
    tt_hmac_t hmac;
    tt_result_t result;
    tt_u32_t i;

    result = tt_hmac_create(&hmac, version, key);
    if (!TT_OK(result)) {
        return TT_FAIL;
    }

    for (i = 0; i < input_num; ++i) {
        result = tt_hmac_update(&hmac, input[i].addr, input[i].len);
        if (!TT_OK(result)) {
            tt_hmac_destroy(&hmac);
            return TT_FAIL;
        }
    }

    result = tt_hmac_final(&hmac, output);
    if (!TT_OK(result)) {
        tt_hmac_destroy(&hmac);
        return TT_FAIL;
    }

    tt_hmac_destroy(&hmac);
    return TT_SUCCESS;
}
