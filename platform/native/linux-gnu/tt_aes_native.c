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

#ifdef PLATFORM_CRYPTO_ENABLE

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

static tt_result_t __aes_cbc_encrypt(IN tt_aes_ntv_t *sys_aes,
                                     IN tt_u8_t *input,
                                     IN tt_u32_t input_len,
                                     OUT tt_u8_t *output,
                                     IN OUT tt_u32_t *output_len);
static tt_result_t __aes_cbc_decrypt(IN tt_aes_ntv_t *sys_aes,
                                     IN tt_u8_t *input,
                                     IN tt_u32_t input_len,
                                     OUT tt_u8_t *output,
                                     IN OUT tt_u32_t *output_len);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_aes_create_ntv(IN tt_aes_ntv_t *sys_aes,
                              IN tt_bool_t encrypt,
                              IN tt_blob_t *key,
                              IN tt_aes_size_t size,
                              IN tt_aes_attr_t *attr)
{
    tt_u8_t aligned_key[__AES_MAX_KEY_SIZE];
    tt_u32_t aligned_key_len;

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
    if (encrypt) {
        if (AES_set_encrypt_key(aligned_key,
                                aligned_key_len << 3,
                                &sys_aes->key) != 0) {
            TT_ERROR("fail to set aes encrypt key");
            return TT_FAIL;
        }
    } else {
        if (AES_set_decrypt_key(aligned_key,
                                aligned_key_len << 3,
                                &sys_aes->key) != 0) {
            TT_ERROR("fail to set aes decrypt key");
            return TT_FAIL;
        }
    }

    // size
    sys_aes->size = size;

    // padding
    sys_aes->padding = attr->padding;

    // mode
    sys_aes->mode = attr->mode;
    if (attr->mode == TT_AES_MODE_CBC) {
        tt_blob_t *ivec = &attr->cbc.ivec;
        if (ivec->len > __AES_BLOCK_SIZE) {
            TT_ERROR("cbc iv length[%d] can not exceed[%d]",
                     ivec->len,
                     __AES_BLOCK_SIZE);
            return TT_FAIL;
        }
        tt_memset(sys_aes->cbc.iv, 0, sizeof(sys_aes->cbc.iv));
        tt_memcpy(sys_aes->cbc.iv, ivec->addr, ivec->len);
    }

    return TT_SUCCESS;
}

void tt_aes_destroy_ntv(IN tt_aes_ntv_t *sys_aes)
{
}

tt_result_t tt_aes_encrypt_ntv(IN tt_aes_ntv_t *sys_aes,
                               IN tt_u8_t *input,
                               IN tt_u32_t input_len,
                               OUT tt_u8_t *output,
                               IN OUT tt_u32_t *output_len)
{
    switch (sys_aes->mode) {
        case TT_AES_MODE_CBC: {
            return __aes_cbc_encrypt(sys_aes,
                                     input,
                                     input_len,
                                     output,
                                     output_len);
        } break;

        default: {
            TT_ERROR("invalid aes mode[%d]", sys_aes->mode);
            return TT_FAIL;
        } break;
    }
}

tt_result_t tt_aes_decrypt_ntv(IN tt_aes_ntv_t *sys_aes,
                               IN tt_u8_t *input,
                               IN tt_u32_t input_len,
                               OUT tt_u8_t *output,
                               IN OUT tt_u32_t *output_len)
{
    switch (sys_aes->mode) {
        case TT_AES_MODE_CBC: {
            return __aes_cbc_decrypt(sys_aes,
                                     input,
                                     input_len,
                                     output,
                                     output_len);
        } break;

        default: {
            TT_ERROR("invalid aes mode[%d]", sys_aes->mode);
            return TT_FAIL;
        } break;
    }
}

tt_result_t __aes_cbc_encrypt(IN tt_aes_ntv_t *sys_aes,
                              IN tt_u8_t *input,
                              IN tt_u32_t input_len,
                              OUT tt_u8_t *output,
                              IN OUT tt_u32_t *output_len)
{
    tt_u8_t tail[__AES_BLOCK_SIZE];
    tt_u32_t in_len = 0;

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

    AES_cbc_encrypt(input, output, in_len, &sys_aes->key, sys_aes->cbc.iv, 1);

    if (sys_aes->padding != TT_AES_PADDING_NONE) {
        AES_cbc_encrypt(tail,
                        TT_PTR_INC(tt_u8_t, output, in_len),
                        __AES_BLOCK_SIZE,
                        &sys_aes->key,
                        sys_aes->cbc.iv,
                        1);
        in_len += __AES_BLOCK_SIZE;
    }

    *output_len = in_len;
    return TT_SUCCESS;
}

tt_result_t __aes_cbc_decrypt(IN tt_aes_ntv_t *sys_aes,
                              IN tt_u8_t *input,
                              IN tt_u32_t input_len,
                              OUT tt_u8_t *output,
                              IN OUT tt_u32_t *output_len)
{
    if (((input_len % __AES_BLOCK_SIZE) != 0) || (input_len == 0)) {
        TT_ERROR("invalid input length[%d]", input_len);
        return TT_FAIL;
    }
    if ((*output_len < input_len) || (*output_len == 0)) {
        TT_ERROR("invalid output length[%d]", *output_len);
        return TT_FAIL;
    }

    AES_cbc_encrypt(input,
                    output,
                    input_len,
                    &sys_aes->key,
                    sys_aes->cbc.iv,
                    0);

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
