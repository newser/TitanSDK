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

#include <crypto/tt_md.h>

#include <algorithm/tt_buffer.h>
#include <misc/tt_assert.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static mbedtls_md_type_t __s_md_type_map[TT_MD_TYPE_NUM] = {
    MBEDTLS_MD_MD2,
    MBEDTLS_MD_MD4,
    MBEDTLS_MD_MD5,
    MBEDTLS_MD_SHA1,
    MBEDTLS_MD_SHA224,
    MBEDTLS_MD_SHA256,
    MBEDTLS_MD_SHA384,
    MBEDTLS_MD_SHA512,
    MBEDTLS_MD_RIPEMD160,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_md_create(IN tt_md_t *md, IN tt_md_type_t type)
{
    TT_ASSERT(md != NULL);
    TT_ASSERT(TT_MD_TYPE_VALID(type));

    if (mbedtls_md_setup(&md->ctx,
                         mbedtls_md_info_from_type(__s_md_type_map[type]),
                         0) != 0) {
        TT_ERROR("fail to setup md");
        return TT_FAIL;
    }

    if (mbedtls_md_starts(&md->ctx) != 0) {
        TT_ERROR("fail to start md");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_md_destroy(IN tt_md_t *md)
{
    TT_ASSERT(md != NULL);

    mbedtls_md_free(&md->ctx);
}

tt_result_t tt_md_final_buf(IN tt_md_t *md, OUT tt_buf_t *output)
{
    tt_u32_t size = tt_md_size(md);

    if (!TT_OK(tt_buf_reserve(output, size))) {
        return TT_FAIL;
    }

    if (mbedtls_md_finish(&md->ctx, TT_BUF_RPOS(output)) != 0) {
        TT_ERROR("md final buf failed");
        return TT_FAIL;
    }
    tt_buf_inc_wp(output, size);

    return TT_SUCCESS;
}
