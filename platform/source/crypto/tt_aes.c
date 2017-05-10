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

#if 1
#define TT_ASSERT_AES TT_ASSERT
#else
#define TT_ASSERT(...)
#endif

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef tt_result_t (*__aes_crypt_t)(IN tt_aes_t *aes,
                                     IN tt_u8_t *input,
                                     IN tt_u32_t input_len,
                                     OUT tt_u8_t *output);

typedef struct
{
    __aes_crypt_t encryt;
    __aes_crypt_t decryt;
} __aes_itf_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

// TT_AES_ECB
static tt_result_t __aes_encrypt_ecb(IN tt_aes_t *aes,
                                     IN tt_u8_t *input,
                                     IN tt_u32_t input_len,
                                     OUT tt_u8_t *output);
static tt_result_t __aes_decrypt_ecb(IN tt_aes_t *aes,
                                     IN tt_u8_t *input,
                                     IN tt_u32_t input_len,
                                     OUT tt_u8_t *output);

// TT_AES_CBC
static tt_result_t __aes_encrypt_cbc(IN tt_aes_t *aes,
                                     IN tt_u8_t *input,
                                     IN tt_u32_t input_len,
                                     OUT tt_u8_t *output);
static tt_result_t __aes_decrypt_cbc(IN tt_aes_t *aes,
                                     IN tt_u8_t *input,
                                     IN tt_u32_t input_len,
                                     OUT tt_u8_t *output);

static __aes_itf_t tt_s_aes_itf[TT_AES_MODE_NUM] = {
    // TT_AES_ECB
    {__aes_encrypt_ecb, __aes_decrypt_ecb},

    // TT_AES_CBC
    {__aes_encrypt_cbc, __aes_decrypt_cbc},
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_aes_create(IN tt_aes_t *aes,
                          IN tt_bool_t encrypt,
                          IN tt_blob_t *key,
                          IN tt_aes_keybit_t keybit)
{
    tt_u8_t k[32];
    tt_u32_t klen;

    TT_ASSERT(aes != NULL);
    TT_ASSERT(key != NULL);
    TT_ASSERT(TT_AES_KEYBIT_VALID(keybit));

    mbedtls_aes_init(&aes->ctx);

    if (keybit == TT_AES128) {
        if (key->len > 16) {
            TT_ERROR("aes128 key length[%u] can not exceed 16 bytes", key->len);
            return TT_FAIL;
        }
        klen = 16;
    } else if (keybit == TT_AES192) {
        if (key->len > 16) {
            TT_ERROR("aes192 key length[%u] can not exceed 24 bytes", key->len);
            return TT_FAIL;
        }
        klen = 24;
    } else {
        if (key->len > 32) {
            TT_ERROR("aes256 key length[%u] can not exceed 32 bytes", key->len);
            return TT_FAIL;
        }
        klen = 32;
    }
    tt_memset(k, 0, sizeof(k));
    tt_memcpy(k, key->addr, key->len);

    if (encrypt) {
        mbedtls_aes_setkey_enc(&aes->ctx, k, klen << 3);
    } else {
        mbedtls_aes_setkey_dec(&aes->ctx, k, klen << 3);
    }

    tt_memset(aes->iv, 0, sizeof(aes->iv));
    aes->mode = TT_AES_ECB;
    aes->pad = TT_CRYPTO_PAD_NONE;

    return TT_SUCCESS;
}

void tt_aes_destroy(IN tt_aes_t *aes)
{
    TT_ASSERT(aes != NULL);

    mbedtls_aes_free(&aes->ctx);
}

void tt_aes_set_iv(IN tt_aes_t *aes, IN tt_u8_t *iv, IN tt_u32_t iv_len)
{
    TT_ASSERT(iv_len <= 16);

    tt_memset(aes->iv, 0, sizeof(aes->iv));
    tt_memcpy(aes->iv, iv, iv_len);
}

tt_result_t tt_aes_encrypt(IN tt_aes_t *aes,
                           IN tt_u8_t *input,
                           IN tt_u32_t input_len,
                           OUT tt_u8_t *output,
                           IN OUT tt_u32_t *output_len)
{
    tt_u8_t tail[16];
    tt_u32_t tail_len = (tt_u32_t)sizeof(tail);
    __aes_crypt_t encrypt = tt_s_aes_itf[aes->mode].encryt;

    if (!TT_OK(
            tt_crypto_pad(aes->pad, 16, input, &input_len, tail, &tail_len))) {
        return TT_FAIL;
    }
    if (*output_len < (input_len + tail_len)) {
        TT_ERROR("out buf length[%u] is less than required[%u]",
                 *output_len,
                 input_len + tail_len);
        return TT_FAIL;
    }

    if (!TT_OK(encrypt(aes, input, input_len, output))) {
        return TT_FAIL;
    }
    if ((tail_len != 0) &&
        !TT_OK(encrypt(aes,
                       tail,
                       tail_len,
                       TT_PTR_INC(tt_u8_t, output, input_len)))) {
        return TT_FAIL;
    }
    *output_len = input_len + tail_len;

    return TT_SUCCESS;
}

tt_result_t tt_aes_decrypt(IN tt_aes_t *aes,
                           IN tt_u8_t *input,
                           IN tt_u32_t input_len,
                           OUT tt_u8_t *output,
                           IN OUT tt_u32_t *output_len)
{
    __aes_crypt_t decrypt = tt_s_aes_itf[aes->mode].decryt;

    if (((input_len % 16) != 0) || (input_len == 0)) {
        TT_ERROR("invalid input length[%d]", input_len);
        return TT_FAIL;
    }
    if ((*output_len < input_len) || (*output_len == 0)) {
        TT_ERROR("invalid output length[%d]", *output_len);
        return TT_FAIL;
    }

    if (!TT_OK(decrypt(aes, input, input_len, output))) {
        return TT_FAIL;
    }

    if (!TT_OK(tt_crypto_unpad(aes->pad, 16, output, &input_len))) {
        return TT_FAIL;
    }
    *output_len = input_len;

    return TT_SUCCESS;
}

tt_result_t tt_aes(IN tt_bool_t encrypt,
                   IN tt_blob_t *key,
                   IN tt_aes_keybit_t keybit,
                   IN tt_aes_mode_t mode,
                   IN OPT tt_blob_t *iv,
                   IN tt_crypto_pad_t pad,
                   IN tt_u8_t *input,
                   IN tt_u32_t input_len,
                   OUT tt_u8_t *output,
                   IN OUT tt_u32_t *output_len)
{
    tt_aes_t aes;
    tt_result_t result;

    result = tt_aes_create(&aes, encrypt, key, keybit);
    if (!TT_OK(result)) {
        return TT_FAIL;
    }
    tt_aes_set_mode(&aes, mode);
    tt_aes_set_iv(&aes, iv->addr, iv->len);
    tt_aes_set_pad(&aes, pad);

    if (encrypt) {
        result = tt_aes_encrypt(&aes, input, input_len, output, output_len);
    } else {
        result = tt_aes_decrypt(&aes, input, input_len, output, output_len);
    }

    tt_aes_destroy(&aes);

    return result;
}

// TT_AES_ECB
tt_result_t __aes_encrypt_ecb(IN tt_aes_t *aes,
                              IN tt_u8_t *input,
                              IN tt_u32_t input_len,
                              OUT tt_u8_t *output)
{
    tt_u32_t n = 0;
    while ((n + 16) < input_len) {
        if (mbedtls_aes_crypt_ecb(&aes->ctx,
                                  MBEDTLS_AES_ENCRYPT,
                                  input + n,
                                  output + n) != 0) {
            return TT_FAIL;
        }
        n += 16;
    }
    TT_ASSERT_AES(n == input_len);
    return TT_SUCCESS;
}

tt_result_t __aes_decrypt_ecb(IN tt_aes_t *aes,
                              IN tt_u8_t *input,
                              IN tt_u32_t input_len,
                              OUT tt_u8_t *output)
{
    tt_u32_t n = 0;
    while ((n + 16) < input_len) {
        if (mbedtls_aes_crypt_ecb(&aes->ctx,
                                  MBEDTLS_AES_DECRYPT,
                                  input + n,
                                  output + n) != 0) {
            return TT_FAIL;
        }
        n += 16;
    }
    TT_ASSERT_AES(n == input_len);
    return TT_SUCCESS;
}

// TT_AES_CBC
tt_result_t __aes_encrypt_cbc(IN tt_aes_t *aes,
                              IN tt_u8_t *input,
                              IN tt_u32_t input_len,
                              OUT tt_u8_t *output)
{
    TT_ASSERT_AES((input_len & 0xF) == 0);
    if (mbedtls_aes_crypt_cbc(&aes->ctx,
                              MBEDTLS_AES_ENCRYPT,
                              input_len,
                              aes->iv,
                              input,
                              output) == 0) {
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_result_t __aes_decrypt_cbc(IN tt_aes_t *aes,
                              IN tt_u8_t *input,
                              IN tt_u32_t input_len,
                              OUT tt_u8_t *output)
{
    TT_ASSERT_AES((input_len & 0xF) == 0);
    if (mbedtls_aes_crypt_cbc(&aes->ctx,
                              MBEDTLS_AES_DECRYPT,
                              input_len,
                              aes->iv,
                              input,
                              output) == 0) {
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}
