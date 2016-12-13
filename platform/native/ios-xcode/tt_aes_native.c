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

#include <tt_aes_native.h>

#include <log/tt_log.h>
#include <misc/tt_assert.h>
#include <misc/tt_util.h>

#include <tt_cstd_api.h>

#ifdef TSCM_PLATFORM_CRYPTO_ENABLE

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define TT_ASSERT_AES TT_ASSERT

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

tt_result_t tt_aes_create_ntv(IN tt_aes_ntv_t *sys_aes,
                              IN tt_bool_t encrypt,
                              IN tt_blob_t *key,
                              IN tt_aes_size_t size,
                              IN tt_aes_attr_t *attr)
{
    tt_u8_t aligned_key[__AES_MAX_KEY_SIZE], aligned_iv[__AES_BLOCK_SIZE];
    tt_u32_t aligned_key_len;
    CCOptions options = 0;
    CCCryptorStatus cstatus;

    // key
    if (size == TT_AES_SIZE_128) {
        if (key->len > __AES128_KEY_SIZE) {
            TT_ERROR("aes128 key length[%d] can not exceed[%d]",
                     key->len,
                     __AES128_KEY_SIZE);
            return TT_FAIL;
        }

        aligned_key_len = __AES128_KEY_SIZE;
    } else {
        TT_ASSERT(size == TT_AES_SIZE_256);
        if (key->len > __AES256_KEY_SIZE) {
            TT_ERROR("aes256 key length[%d] can not exceed[%d]",
                     key->len,
                     __AES256_KEY_SIZE);
            return TT_FAIL;
        }

        aligned_key_len = __AES256_KEY_SIZE;
    }
    tt_memset(aligned_key, 0, sizeof(aligned_key));
    tt_memcpy(aligned_key, key->addr, key->len);

    // padding
    sys_aes->padding = attr->padding;

    // mode
    if (attr->mode == TT_AES_MODE_CBC) {
        tt_blob_t *ivec = &attr->cbc.ivec;
        if (ivec->len > __AES_BLOCK_SIZE) {
            TT_ERROR("cbc iv length[%d] can not exceed[%d]",
                     ivec->len,
                     __AES_BLOCK_SIZE);
            return TT_FAIL;
        }
        tt_memset(aligned_iv, 0, sizeof(aligned_iv));
        tt_memcpy(aligned_iv, ivec->addr, ivec->len);
    }

    // create
    cstatus = CCCryptorCreate(TT_COND(encrypt, kCCEncrypt, kCCDecrypt),
                              kCCAlgorithmAES,
                              options,
                              aligned_key,
                              aligned_key_len,
                              aligned_iv,
                              &sys_aes->cref);
    if (cstatus != kCCSuccess) {
        TT_ERROR("fail to create enc ref");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_aes_destroy_ntv(IN tt_aes_ntv_t *sys_aes)
{
    CCCryptorRelease(sys_aes->cref);
}

tt_result_t tt_aes_encrypt_ntv(IN tt_aes_ntv_t *sys_aes,
                               IN tt_u8_t *input,
                               IN tt_u32_t input_len,
                               OUT tt_u8_t *output,
                               IN OUT tt_u32_t *output_len)
{
    tt_u8_t tail[__AES_BLOCK_SIZE];
    tt_u32_t in_len = 0;

    CCCryptorStatus cstatus;
    size_t dataOutMoved = 0;

    switch (sys_aes->padding) {
        case TT_AES_PADDING_NONE: {
            if ((input_len & 0xF) != 0) {
                TT_ERROR("input_len must be multiple of 16");
                return TT_FAIL;
            }

            in_len = input_len;
        } break;

        case TT_AES_PADDING_PKCS7: {
            in_len = input_len / __AES_BLOCK_SIZE;
            in_len *= __AES_BLOCK_SIZE;

            if (in_len == input_len) {
                tt_memset(tail, __AES_BLOCK_SIZE, __AES_BLOCK_SIZE);
            } else {
                tt_u32_t tail_data;
                tt_u8_t padding_val;

                TT_ASSERT_AES(in_len < input_len);
                tail_data = input_len - in_len;
                TT_ASSERT_AES(tail_data < __AES_BLOCK_SIZE);
                padding_val = (tt_u8_t)(__AES_BLOCK_SIZE - tail_data);
                tt_memcpy(tail, TT_PTR_INC(tt_u8_t, input, in_len), tail_data);
                tt_memset(TT_PTR_INC(tt_u8_t, tail, tail_data),
                          padding_val,
                          padding_val);
            }

            // check output length
            if (*output_len < (in_len + __AES_BLOCK_SIZE)) {
                TT_ERROR("no enough space for encrypted data");
                return TT_FAIL;
            }
        } break;

        default: {
            TT_ERROR("invalid aes padding[%d]", sys_aes->padding);
            return TT_FAIL;
        } break;
    }

    cstatus = CCCryptorUpdate(sys_aes->cref,
                              input,
                              in_len,
                              output,
                              in_len,
                              &dataOutMoved);
    if (cstatus != kCCSuccess) {
        TT_ERROR("aes encrypt failed");
        return TT_FAIL;
    }

    if (sys_aes->padding != TT_AES_PADDING_NONE) {
        cstatus = CCCryptorUpdate(sys_aes->cref,
                                  tail,
                                  __AES_BLOCK_SIZE,
                                  TT_PTR_INC(tt_u8_t, output, in_len),
                                  __AES_BLOCK_SIZE,
                                  &dataOutMoved);
        if (cstatus != kCCSuccess) {
            TT_ERROR("aes encrypt failed");
            return TT_FAIL;
        }

        in_len += __AES_BLOCK_SIZE;
    }

    *output_len = in_len;
    return TT_SUCCESS;
}

tt_result_t tt_aes_decrypt_ntv(IN tt_aes_ntv_t *sys_aes,
                               IN tt_u8_t *input,
                               IN tt_u32_t input_len,
                               OUT tt_u8_t *output,
                               IN OUT tt_u32_t *output_len)
{
    CCCryptorStatus cstatus;
    size_t dataOutMoved = 0;

    if (((input_len % __AES_BLOCK_SIZE) != 0) || (input_len == 0)) {
        TT_ERROR("invalid input length[%d]", input_len);
        return TT_FAIL;
    }
    if ((*output_len < input_len) || (*output_len == 0)) {
        TT_ERROR("invalid output length[%d]", *output_len);
        return TT_FAIL;
    }

    cstatus = CCCryptorUpdate(sys_aes->cref,
                              input,
                              input_len,
                              output,
                              *output_len,
                              &dataOutMoved);
    if (cstatus != kCCSuccess) {
        TT_ERROR("aes decrypt failed");
        return TT_FAIL;
    }

    switch (sys_aes->padding) {
        case TT_AES_PADDING_NONE: {
            *output_len = input_len;
            return TT_SUCCESS;
        } break;

        case TT_AES_PADDING_PKCS7: {
            tt_u8_t padding_val = output[input_len - 1];

            if ((padding_val == 0) || (padding_val > 0x10) ||
                (padding_val > input_len)) {
                TT_ERROR("aes pkcs7 decoding failed");
                return TT_FAIL;
            }

            *output_len = input_len - padding_val;
            return TT_SUCCESS;
        } break;

        default: {
            TT_ERROR("invalid aes padding[%d]", sys_aes->padding);
            return TT_FAIL;
        } break;
    }
}

#endif
