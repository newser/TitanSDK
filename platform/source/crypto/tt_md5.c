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

#include <crypto/tt_md5.h>

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

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_md5_create(IN tt_md5_t *md5)
{
    TT_ASSERT(md5 != NULL);

    return tt_md5_create_ntv(&md5->sys_md5);
}

void tt_md5_destroy(IN tt_md5_t *md5)
{
    tt_md5_destroy_ntv(&md5->sys_md5);
}

tt_result_t tt_md5_update(IN tt_md5_t *md5,
                          IN tt_u8_t *input,
                          IN tt_u32_t input_len)
{
    TT_ASSERT(md5 != NULL);
    TT_ASSERT(input != NULL);

    if (input_len > 0) {
        return tt_md5_update_ntv(&md5->sys_md5, input, input_len);
    } else {
        return TT_SUCCESS;
    }
}

tt_result_t tt_md5_final_buf(IN tt_md5_t *md5, OUT tt_buf_t *output)
{
    TT_ASSERT(md5 != NULL);
    TT_ASSERT(output != NULL);

    // md5 digest length can only be 16 bytes
    if (!TT_OK(tt_buf_reserve(output, TT_MD5_DIGEST_LENGTH))) {
        return TT_FAIL;
    }

    if (!TT_OK(tt_md5_final_ntv(&md5->sys_md5, TT_BUF_WPOS(output)))) {
        tt_buf_destroy(output);
        return TT_FAIL;
    }
    tt_buf_inc_wp(output, TT_MD5_DIGEST_LENGTH);

    return TT_SUCCESS;
}

tt_result_t tt_md5_final(IN tt_md5_t *md5, OUT tt_u8_t *output)
{
    TT_ASSERT(md5 != NULL);
    TT_ASSERT(output != NULL);

    return tt_md5_final_ntv(&md5->sys_md5, output);
}
