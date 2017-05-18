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
@file tt_message_digest.h
@brief message digest

this file defines message digest APIs
*/

#ifndef __TT_MESSAGE_DIGEST__
#define __TT_MESSAGE_DIGEST__

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

typedef enum {
    TT_MD2,
    TT_MD4,
    TT_MD5,
    TT_SHA1,
    TT_SHA224,
    TT_SHA256,
    TT_SHA384,
    TT_SHA512,
    TT_RIPEMD160,

    TT_MD_TYPE_NUM
} tt_md_type_t;
#define TT_MD_TYPE_VALID(t) ((t) < TT_MD_TYPE_NUM)

typedef struct
{
    mbedtls_md_context_t ctx;
} tt_md_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

extern mbedtls_md_type_t tt_g_md_type_map[TT_MD_TYPE_NUM];

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_md_create(IN tt_md_t *md, IN tt_md_type_t type);

extern void tt_md_destroy(IN tt_md_t *md);

tt_inline tt_u32_t tt_md_size(IN tt_md_t *md)
{
    return (tt_u32_t)mbedtls_md_get_size(md->ctx.md_info);
}

tt_inline tt_result_t tt_md_update(IN tt_md_t *md,
                                   IN tt_u8_t *input,
                                   IN tt_u32_t len)
{
    if (mbedtls_md_update(&md->ctx, input, len) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR("md update failed");
        return TT_FAIL;
    }
}

tt_inline tt_result_t tt_md_final(IN tt_md_t *md, OUT tt_u8_t *output)
{
    if (mbedtls_md_finish(&md->ctx, output) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR("md final failed");
        return TT_FAIL;
    }
}

extern tt_result_t tt_md_final_buf(IN tt_md_t *md, OUT struct tt_buf_s *output);

tt_inline tt_result_t tt_md_reset(IN tt_md_t *md)
{
    if (mbedtls_md_starts(&md->ctx) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR("md reset failed");
        return TT_FAIL;
    }
}

extern tt_result_t tt_md_gather(IN tt_md_type_t type,
                                IN tt_blob_t *input,
                                IN tt_u32_t input_num,
                                OUT tt_u8_t *output);

tt_inline tt_result_t tt_md(IN tt_md_type_t type,
                            IN tt_u8_t *input,
                            IN tt_u32_t input_len,
                            OUT tt_u8_t *output)
{
    tt_blob_t b = {input, input_len};
    return tt_md_gather(type, &b, 1, output);
}

#endif /* __TT_MESSAGE_DIGEST__ */
