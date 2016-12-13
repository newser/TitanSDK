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

#include <crypto/tt_aes.h>

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

tt_result_t tt_aes_create(IN tt_aes_t *aes,
                          IN tt_bool_t encrypt,
                          IN tt_blob_t *key,
                          IN tt_aes_size_t size,
                          IN tt_aes_attr_t *attr)
{
    TT_ASSERT(aes != NULL);
    TT_ASSERT(TT_AES_SIZE_VALID(size));
    TT_ASSERT(attr != NULL);
    TT_ASSERT(TT_AES_PADDING_VALID(attr->padding));
    TT_ASSERT(TT_AES_MODE_VALID(attr->mode));

    return tt_aes_create_ntv(&aes->sys_aes, encrypt, key, size, attr);
}

void tt_aes_destroy(IN tt_aes_t *aes)
{
    TT_ASSERT(aes != NULL);

    tt_aes_destroy_ntv(&aes->sys_aes);
}

tt_result_t tt_aes(IN tt_bool_t encrypt,
                   IN tt_blob_t *key,
                   IN tt_aes_size_t size,
                   IN tt_aes_attr_t *attr,
                   IN tt_blob_t *input,
                   OUT tt_u8_t *output,
                   IN OUT tt_u32_t *output_len)
{
    tt_aes_t aes;
    tt_result_t result;

    result = tt_aes_create(&aes, encrypt, key, size, attr);
    if (!TT_OK(result)) {
        return TT_FAIL;
    }

    if (encrypt) {
        result =
            tt_aes_encrypt(&aes, input->addr, input->len, output, output_len);
    } else {
        result =
            tt_aes_decrypt(&aes, input->addr, input->len, output, output_len);
    }
    if (!TT_OK(result)) {
        tt_aes_destroy(&aes);
        return TT_FAIL;
    }

    tt_aes_destroy(&aes);
    return TT_SUCCESS;
}
