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

#include <crypto/tt_crypto_pad.h>

#include <misc/tt_assert.h>
#include <misc/tt_util.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef tt_result_t (*__pad_t)(IN tt_u8_t block,
                               IN tt_u8_t *data,
                               IN OUT tt_u32_t *data_len,
                               IN OUT tt_u8_t *tail,
                               IN OUT tt_u32_t *tail_len);

typedef tt_result_t (*__unpad_t)(IN tt_u8_t block,
                                 IN tt_u8_t *data,
                                 IN OUT tt_u32_t *data_len);

typedef struct
{
    __pad_t pad;
    __unpad_t unpad;
} __pad_itf_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

// TT_CRYPTO_PAD_NONE
static tt_result_t __pad_none(IN tt_u8_t block,
                              IN tt_u8_t *data,
                              IN OUT tt_u32_t *data_len,
                              IN OUT tt_u8_t *tail,
                              IN OUT tt_u32_t *tail_len);
static tt_result_t __unpad_none(IN tt_u8_t block,
                                IN tt_u8_t *data,
                                IN OUT tt_u32_t *data_len);

// TT_CRYPTO_PAD_PKCS7
static tt_result_t __pad_pkcs7(IN tt_u8_t block,
                               IN tt_u8_t *data,
                               IN OUT tt_u32_t *data_len,
                               IN OUT tt_u8_t *tail,
                               IN OUT tt_u32_t *tail_len);
static tt_result_t __unpad_pkcs7(IN tt_u8_t block,
                                 IN tt_u8_t *data,
                                 IN OUT tt_u32_t *data_len);

static __pad_itf_t tt_s_pad_itf[TT_CRYPTO_PAD_NUM] = {
    // TT_CRYPTO_PAD_NONE
    {__pad_none, __unpad_none},

    // TT_CRYPTO_PAD_PKCS7
    {__pad_pkcs7, __unpad_pkcs7},
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_crypto_pad(IN tt_crypto_pad_t pad,
                          IN tt_u8_t block,
                          IN tt_u8_t *data,
                          IN OUT tt_u32_t *data_len,
                          IN OUT tt_u8_t *tail,
                          IN OUT tt_u32_t *tail_len)
{
    TT_ASSERT(TT_CRYPTO_PAD_VALID(pad));
    TT_ASSERT(block != 0);
    TT_ASSERT(data != NULL);
    TT_ASSERT(data_len != NULL);
    TT_ASSERT(tail != NULL);
    TT_ASSERT(tail_len != NULL);

    return tt_s_pad_itf[pad].pad(block, data, data_len, tail, tail_len);
}

tt_result_t tt_crypto_unpad(IN tt_crypto_pad_t pad,
                            IN tt_u8_t block,
                            IN tt_u8_t *data,
                            IN OUT tt_u32_t *data_len)
{
    TT_ASSERT(TT_CRYPTO_PAD_VALID(pad));
    TT_ASSERT(block != 0);
    TT_ASSERT(data != NULL);
    TT_ASSERT(data_len != NULL);

    return tt_s_pad_itf[pad].unpad(block, data, data_len);
}

tt_result_t __pad_none(IN tt_u8_t block,
                       IN tt_u8_t *data,
                       IN OUT tt_u32_t *data_len,
                       IN OUT tt_u8_t *tail,
                       IN OUT tt_u32_t *tail_len)
{
    *tail_len = 0;
    return TT_SUCCESS;
}

tt_result_t __unpad_none(IN tt_u8_t block,
                         IN tt_u8_t *data,
                         IN OUT tt_u32_t *data_len)
{
    return TT_SUCCESS;
}

tt_result_t __pad_pkcs7(IN tt_u8_t block,
                        IN tt_u8_t *data,
                        IN OUT tt_u32_t *data_len,
                        IN OUT tt_u8_t *tail,
                        IN OUT tt_u32_t *tail_len)
{
    tt_u32_t dlen = *data_len;
    tt_u32_t n = (dlen / block) * block;

    if (n < dlen) {
        tt_u32_t tlen;
        tt_u8_t pad;

        *data_len = n;

        tlen = dlen - n;
        pad = (tt_u8_t)(block - tlen);
        tt_memcpy(tail, TT_PTR_INC(tt_u8_t, data, n), tlen);
        tt_memset(TT_PTR_INC(tt_u8_t, tail, tlen), pad, pad);
        *tail_len = block;
    } else {
        tt_memset(tail, block, block);
        *tail_len = block;
    }

    return TT_SUCCESS;
}

tt_result_t __unpad_pkcs7(IN tt_u8_t block,
                          IN tt_u8_t *data,
                          IN OUT tt_u32_t *data_len)
{
    tt_u32_t dlen = *data_len;
    tt_u8_t pad;

    if ((dlen == 0) || (dlen % block != 0)) {
        TT_ERROR("data length[%d] must be multiple of block[%d]", dlen, block);
        return TT_FAIL;
    }

    pad = data[dlen - 1];
    if ((pad == 0) || (pad > dlen)) {
        TT_ERROR("pkcs7 unpadding failed");
        return TT_FAIL;
    }

    *data_len -= pad;
    return TT_SUCCESS;
}
