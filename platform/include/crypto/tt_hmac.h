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

#include <algorithm/tt_blob.h>
#include <log/tt_log.h>

#include <md.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_buf_s;

typedef enum
{
    TT_HMAC_MD2,
    TT_HMAC_MD4,
    TT_HMAC_MD5,
    TT_HMAC_SHA1,
    TT_HMAC_SHA224,
    TT_HMAC_SHA256,
    TT_HMAC_SHA384,
    TT_HMAC_SHA512,
    TT_HMAC_RIPEMD160,

    TT_HMAC_TYPE_NUM
} tt_hmac_type_t;
#define TT_HMAC_TYPE_VALID(t) ((t) < TT_HMAC_TYPE_NUM)

typedef struct
{
    mbedtls_md_context_t ctx;
} tt_hmac_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_result_t tt_hmac_create(IN tt_hmac_t *hm, IN tt_hmac_type_t type,
                                     IN tt_u8_t *key, IN tt_u32_t key_len);

tt_export void tt_hmac_destroy(IN tt_hmac_t *hm);

tt_inline tt_u32_t tt_hmac_size(IN tt_hmac_t *hm)
{
    return (tt_u32_t)mbedtls_md_get_size(hm->ctx.md_info);
}

tt_inline tt_result_t tt_hmac_update(IN tt_hmac_t *hm, IN tt_u8_t *input,
                                     IN tt_u32_t len)
{
    if (mbedtls_md_hmac_update(&hm->ctx, input, len) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR("hmac update failed");
        return TT_FAIL;
    }
}

tt_inline tt_result_t tt_hmac_final(IN tt_hmac_t *hm, OUT tt_u8_t *output)
{
    if (mbedtls_md_hmac_finish(&hm->ctx, output) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR("hmac final failed");
        return TT_FAIL;
    }
}

tt_export tt_result_t tt_hmac_final_buf(IN tt_hmac_t *hm,
                                        OUT struct tt_buf_s *output);

tt_inline tt_result_t tt_hmac_reset(IN tt_hmac_t *hm)
{
    if (mbedtls_md_hmac_reset(&hm->ctx) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR("hm reset failed");
        return TT_FAIL;
    }
}

tt_export tt_result_t tt_hmac_gather(IN tt_hmac_type_t type, IN tt_u8_t *key,
                                     IN tt_u32_t key_len, IN tt_blob_t *input,
                                     IN tt_u32_t input_num,
                                     OUT tt_u8_t *output);

tt_inline tt_result_t tt_hmac(IN tt_hmac_type_t type, IN tt_u8_t *key,
                              IN tt_u32_t key_len, IN tt_u8_t *input,
                              IN tt_u32_t input_len, OUT tt_u8_t *output)
{
    tt_blob_t b = {input, input_len};
    return tt_hmac_gather(type, key, key_len, &b, 1, output);
}

#endif /* __TT_HMAC__ */
