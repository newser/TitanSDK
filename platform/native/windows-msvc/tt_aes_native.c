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

#include <init/tt_profile.h>
#include <log/tt_log.h>
#include <memory/tt_memory_alloc.h>
#include <misc/tt_assert.h>
#include <misc/tt_util.h>

#include <tt_crypto_native.h>

#ifdef PLATFORM_CRYPTO_ENABLE

#include <bcrypt.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define TT_ASSERT_AES TT_ASSERT

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef struct
{
    BCRYPT_KEY_DATA_BLOB_HEADER header;
    tt_u8_t data[__AES_MAX_KEY_SIZE];
} __aes_key_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static BCRYPT_ALG_HANDLE tt_s_crypt_prov_aes_cbc;
static ULONG tt_s_crypt_aes_size;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_aes_component_init_ntv(IN tt_profile_t *profile)
{
    NTSTATUS ntst;
    ULONG val, n;

    // ========================================
    // AES CBC
    // ========================================

    // provider
    ntst = BCryptOpenAlgorithmProvider(&tt_s_crypt_prov_aes_cbc,
                                       BCRYPT_AES_ALGORITHM,
                                       MS_PRIMITIVE_PROVIDER,
                                       0);
    if ((ntst != STATUS_SUCCESS) || (tt_s_crypt_prov_aes_cbc == NULL)) {
        TT_ERROR("fail to create aes provider");
        return TT_FAIL;
    }

    // aes object size
    ntst = BCryptGetProperty(tt_s_crypt_prov_aes_cbc,
                             BCRYPT_OBJECT_LENGTH,
                             (PUCHAR)&tt_s_crypt_aes_size,
                             sizeof(tt_s_crypt_aes_size),
                             &n,
                             0);
    if ((ntst != STATUS_SUCCESS) || (tt_s_crypt_aes_size == 0)) {
        TT_ERROR("fail to get aes obj size");
        return TT_FAIL;
    }

    // check aes block size, should be 16
    ntst = BCryptGetProperty(tt_s_crypt_prov_aes_cbc,
                             BCRYPT_BLOCK_LENGTH,
                             (PUCHAR)&val,
                             sizeof(val),
                             &n,
                             0);
    if ((ntst != STATUS_SUCCESS) || (val != TT_AES_BLOCK_SIZE)) {
        TT_ERROR("fail to get aes block size");
        return TT_FAIL;
    }

    // mode
    ntst = BCryptSetProperty(tt_s_crypt_prov_aes_cbc,
                             BCRYPT_CHAINING_MODE,
                             (PUCHAR)BCRYPT_CHAIN_MODE_CBC,
                             sizeof(BCRYPT_CHAIN_MODE_CBC),
                             0);
    if ((ntst != STATUS_SUCCESS) || (val != TT_AES_BLOCK_SIZE)) {
        TT_ERROR("fail to SET aes cbc mode");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_aes_create_ntv(IN tt_aes_ntv_t *sys_aes,
                              IN tt_bool_t encrypt,
                              IN tt_blob_t *key,
                              IN tt_aes_size_t size,
                              IN tt_aes_attr_t *attr)
{
    __aes_key_t aligned_key;
    BCRYPT_ALG_HANDLE *prov = NULL;
    NTSTATUS ntst;

    // align aes key
    aligned_key.header.dwMagic = BCRYPT_KEY_DATA_BLOB_MAGIC;
    aligned_key.header.dwVersion = BCRYPT_KEY_DATA_BLOB_VERSION1;
    switch (size) {
        case TT_AES_SIZE_128: {
            aligned_key.header.cbKeyData = 16;
        } break;
        case TT_AES_SIZE_256: {
            aligned_key.header.cbKeyData = 32;
        } break;

        default: {
            TT_ERROR("invalid aes size[%d]", size);
            return TT_FAIL;
        } break;
    }
    if (key->len > aligned_key.header.cbKeyData) {
        TT_ERROR("aes key length[%d] can not exceed[%d]",
                 key->len,
                 aligned_key.header.cbKeyData);
        return TT_FAIL;
    }
    tt_memset(aligned_key.data, 0, sizeof(aligned_key.data));
    tt_memcpy(aligned_key.data, key->addr, key->len);

    // align aes iv
    switch (attr->mode) {
        case TT_AES_MODE_CBC: {
            tt_blob_t *ivec = &attr->cbc.ivec;
            if (ivec->len > TT_AES_BLOCK_SIZE) {
                TT_ERROR("cbc iv length[%d] can not exceed[%d]",
                         ivec->len,
                         TT_AES_BLOCK_SIZE);
                return TT_FAIL;
            }
            tt_memset(sys_aes->cbc.iv, 0, sizeof(sys_aes->cbc.iv));
            tt_memcpy(sys_aes->cbc.iv, ivec->addr, ivec->len);

            prov = tt_s_crypt_prov_aes_cbc;
        } break;

        default: {
            TT_ERROR("invalid aes mode[%d]", attr->mode);
            return TT_FAIL;
        } break;
    }

    // create aes key
    sys_aes->key_mem = (tt_u8_t *)tt_malloc(tt_s_crypt_aes_size);
    if (sys_aes->key_mem == NULL) {
        TT_ERROR("no mem for aes key");
        return TT_FAIL;
    }

    ntst = BCryptImportKey(prov,
                           NULL,
                           BCRYPT_KEY_DATA_BLOB,
                           &sys_aes->key,
                           sys_aes->key_mem,
                           tt_s_crypt_aes_size,
                           (PUCHAR)&aligned_key,
                           sizeof(__aes_key_t),
                           0);
    if (ntst != STATUS_SUCCESS) {
        TT_ERROR("fail to import aes key");
        return TT_FAIL;
    }

    sys_aes->padding = attr->padding;

    return TT_SUCCESS;
}

void tt_aes_destroy_ntv(IN tt_aes_ntv_t *sys_aes)
{
    BCryptDestroyKey(sys_aes->key);
    tt_free(sys_aes->key_mem);
}

tt_result_t tt_aes_encrypt_ntv(IN tt_aes_ntv_t *sys_aes,
                               IN tt_u8_t *input,
                               IN tt_u32_t input_len,
                               OUT tt_u8_t *output,
                               IN OUT tt_u32_t *output_len)
{
    tt_u8_t tail[TT_AES_BLOCK_SIZE];
    tt_u32_t in_len = 0, out_len = 0;
    ULONG n;
    NTSTATUS ntst;

    switch (sys_aes->padding) {
        case TT_AES_PADDING_NONE: {
            if ((input_len & 0xF) != 0) {
                TT_ERROR("input_len must be multiple of 16");
                return TT_FAIL;
            }

            in_len = input_len;
        } break;

        case TT_AES_PADDING_PKCS7: {
            in_len = input_len / TT_AES_BLOCK_SIZE;
            in_len *= TT_AES_BLOCK_SIZE;

            if (in_len == input_len) {
                tt_memset(tail, TT_AES_BLOCK_SIZE, TT_AES_BLOCK_SIZE);
            } else {
                tt_u32_t tail_data;
                tt_u8_t padding_val;

                TT_ASSERT_AES(in_len < input_len);
                tail_data = input_len - in_len;
                TT_ASSERT_AES(tail_data < TT_AES_BLOCK_SIZE);
                padding_val = (tt_u8_t)(TT_AES_BLOCK_SIZE - tail_data);
                tt_memcpy(tail, TT_PTR_INC(tt_u8_t, input, in_len), tail_data);
                tt_memset(TT_PTR_INC(tt_u8_t, tail, tail_data),
                          padding_val,
                          padding_val);
            }

            // check output length
            if (*output_len < (in_len + TT_AES_BLOCK_SIZE)) {
                TT_ERROR("no enough space for encrypted data");
                return TT_FAIL;
            }
        } break;

        default: {
            TT_ERROR("invalid aes padding[%d]", sys_aes->padding);
            return TT_FAIL;
        } break;
    }

    // encrypt data
    ntst = BCryptEncrypt(sys_aes->key,
                         input,
                         in_len,
                         NULL,
                         sys_aes->cbc.iv,
                         TT_AES_BLOCK_SIZE,
                         output + out_len,
                         *output_len - out_len,
                         &n,
                         0);
    if (ntst != STATUS_SUCCESS) {
        TT_ERROR("fail to do aes encryption");
        return TT_FAIL;
    }
    out_len += n;

    // encrypt tail
    if (sys_aes->padding != TT_AES_PADDING_NONE) {
        ntst = BCryptEncrypt(sys_aes->key,
                             tail,
                             TT_AES_BLOCK_SIZE,
                             NULL,
                             sys_aes->cbc.iv,
                             TT_AES_BLOCK_SIZE,
                             output + out_len,
                             *output_len - out_len,
                             &n,
                             0);
        if (ntst != STATUS_SUCCESS) {
            TT_ERROR("fail to do aes encryption");
            return TT_FAIL;
        }

        in_len += TT_AES_BLOCK_SIZE;
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
    ULONG n;
    NTSTATUS ntst;

    if (((input_len % TT_AES_BLOCK_SIZE) != 0) || (input_len == 0)) {
        TT_ERROR("invalid input length[%d]", input_len);
        return TT_FAIL;
    }
    if ((*output_len < input_len) || (*output_len == 0)) {
        TT_ERROR("invalid output length[%d]", *output_len);
        return TT_FAIL;
    }

    // decrypt data
    ntst = BCryptDecrypt(sys_aes->key,
                         input,
                         input_len,
                         NULL,
                         sys_aes->cbc.iv,
                         TT_AES_BLOCK_SIZE,
                         output,
                         *output_len,
                         &n,
                         0);
    if (ntst != STATUS_SUCCESS) {
        TT_ERROR("fail to do aes encryption");
        return TT_FAIL;
    }

    // decoding
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
