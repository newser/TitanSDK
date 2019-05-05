/* Copyright (C) 2017 haniu (niuhao.cn@gmail.com)
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
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

static mbedtls_md_type_t __hmac_type_map[TT_HMAC_TYPE_NUM] = {
    MBEDTLS_MD_MD2,    MBEDTLS_MD_MD4,    MBEDTLS_MD_MD5,
    MBEDTLS_MD_SHA1,   MBEDTLS_MD_SHA224, MBEDTLS_MD_SHA256,
    MBEDTLS_MD_SHA384, MBEDTLS_MD_SHA512, MBEDTLS_MD_RIPEMD160,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_hmac_create(IN tt_hmac_t *hm, IN tt_hmac_type_t type,
                           IN tt_u8_t *key, IN tt_u32_t key_len)
{
    TT_ASSERT(hm != NULL);
    TT_ASSERT(TT_HMAC_TYPE_VALID(type));

    mbedtls_md_init(&hm->ctx);

    if (mbedtls_md_setup(&hm->ctx,
                         mbedtls_md_info_from_type(__hmac_type_map[type]),
                         1) != 0) {
        TT_ERROR("fail to setup hm");
        return TT_FAIL;
    }

    if (mbedtls_md_hmac_starts(&hm->ctx, key, key_len) != 0) {
        TT_ERROR("fail to start hm");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_hmac_destroy(IN tt_hmac_t *hm)
{
    TT_ASSERT(hm != NULL);

    mbedtls_md_free(&hm->ctx);
}

tt_result_t tt_hmac_final_buf(IN tt_hmac_t *hm, OUT tt_buf_t *output)
{
    tt_u32_t size = tt_hmac_size(hm);

    if (!TT_OK(tt_buf_reserve(output, size))) { return TT_FAIL; }

    if (mbedtls_md_hmac_finish(&hm->ctx, TT_BUF_RPOS(output)) != 0) {
        TT_ERROR("hm final buf failed");
        return TT_FAIL;
    }
    tt_buf_inc_wp(output, size);

    return TT_SUCCESS;
}

tt_result_t tt_hmac_gather(IN tt_hmac_type_t type, IN tt_u8_t *key,
                           IN tt_u32_t key_len, IN tt_blob_t *input,
                           IN tt_u32_t input_num, OUT tt_u8_t *output)
{
    tt_hmac_t hm;
    tt_u32_t i;

    if (!TT_OK(tt_hmac_create(&hm, type, key, key_len))) { return TT_FAIL; }

    for (i = 0; i < input_num; ++i) {
        if (!TT_OK(tt_hmac_update(&hm, input[i].addr, input[i].len))) {
            tt_hmac_destroy(&hm);
            return TT_FAIL;
        }
    }

    if (!TT_OK(tt_hmac_final(&hm, output))) {
        tt_hmac_destroy(&hm);
        return TT_FAIL;
    }

    tt_hmac_destroy(&hm);
    return TT_SUCCESS;
}
