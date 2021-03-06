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

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <crypto/tt_cipher.h>

#include <algorithm/tt_buffer.h>
#include <crypto/tt_crypto.h>

#include <tt_cstd_api.h>

#include <ccm.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#if 1
#define TT_ASSERT_CIPHER TT_ASSERT
#else
#define TT_ASSERT_CIPHER(...)
#endif

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static mbedtls_cipher_type_t ___cipher_type_map[TT_CIPHER_TYPE_NUM] = {
    MBEDTLS_CIPHER_NULL,
    MBEDTLS_CIPHER_AES_128_ECB,
    MBEDTLS_CIPHER_AES_192_ECB,
    MBEDTLS_CIPHER_AES_256_ECB,
    MBEDTLS_CIPHER_AES_128_CBC,
    MBEDTLS_CIPHER_AES_192_CBC,
    MBEDTLS_CIPHER_AES_256_CBC,
    MBEDTLS_CIPHER_AES_128_CFB128,
    MBEDTLS_CIPHER_AES_192_CFB128,
    MBEDTLS_CIPHER_AES_256_CFB128,
    MBEDTLS_CIPHER_AES_128_CTR,
    MBEDTLS_CIPHER_AES_192_CTR,
    MBEDTLS_CIPHER_AES_256_CTR,
    MBEDTLS_CIPHER_AES_128_GCM,
    MBEDTLS_CIPHER_AES_192_GCM,
    MBEDTLS_CIPHER_AES_256_GCM,
    MBEDTLS_CIPHER_CAMELLIA_128_ECB,
    MBEDTLS_CIPHER_CAMELLIA_192_ECB,
    MBEDTLS_CIPHER_CAMELLIA_256_ECB,
    MBEDTLS_CIPHER_CAMELLIA_128_CBC,
    MBEDTLS_CIPHER_CAMELLIA_192_CBC,
    MBEDTLS_CIPHER_CAMELLIA_256_CBC,
    MBEDTLS_CIPHER_CAMELLIA_128_CFB128,
    MBEDTLS_CIPHER_CAMELLIA_192_CFB128,
    MBEDTLS_CIPHER_CAMELLIA_256_CFB128,
    MBEDTLS_CIPHER_CAMELLIA_128_CTR,
    MBEDTLS_CIPHER_CAMELLIA_192_CTR,
    MBEDTLS_CIPHER_CAMELLIA_256_CTR,
    MBEDTLS_CIPHER_CAMELLIA_128_GCM,
    MBEDTLS_CIPHER_CAMELLIA_192_GCM,
    MBEDTLS_CIPHER_CAMELLIA_256_GCM,
    MBEDTLS_CIPHER_DES_ECB,
    MBEDTLS_CIPHER_DES_CBC,
    MBEDTLS_CIPHER_DES_EDE_ECB,
    MBEDTLS_CIPHER_DES_EDE_CBC,
    MBEDTLS_CIPHER_DES_EDE3_ECB,
    MBEDTLS_CIPHER_DES_EDE3_CBC,
    MBEDTLS_CIPHER_BLOWFISH_ECB,
    MBEDTLS_CIPHER_BLOWFISH_CBC,
    MBEDTLS_CIPHER_BLOWFISH_CFB64,
    MBEDTLS_CIPHER_BLOWFISH_CTR,
    MBEDTLS_CIPHER_ARC4_128,
    MBEDTLS_CIPHER_AES_128_CCM,
    MBEDTLS_CIPHER_AES_192_CCM,
    MBEDTLS_CIPHER_AES_256_CCM,
    MBEDTLS_CIPHER_CAMELLIA_128_CCM,
    MBEDTLS_CIPHER_CAMELLIA_192_CCM,
    MBEDTLS_CIPHER_CAMELLIA_256_CCM,
};

static mbedtls_cipher_padding_t ___cipher_padding_map[TT_PADDING_NUM] = {
    MBEDTLS_PADDING_PKCS7,
    MBEDTLS_PADDING_ONE_AND_ZEROS,
    MBEDTLS_PADDING_ZEROS_AND_LEN,
    MBEDTLS_PADDING_ZEROS,
    MBEDTLS_PADDING_NONE,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_cipher_setup(IN tt_cipher_t *cipher,
                            IN tt_cipher_type_t type,
                            IN tt_bool_t encrypt,
                            IN tt_u8_t *key,
                            IN tt_u32_t len)
{
    int e;

    TT_ASSERT(cipher != NULL);
    TT_ASSERT(TT_CIPHER_TYPE_VALID(type));

    e = mbedtls_cipher_setup(&cipher->ctx,
                             mbedtls_cipher_info_from_type(
                                 ___cipher_type_map[type]));
    if (e != 0) {
        tt_crypto_error("fail to setup cipher");
        return TT_FAIL;
    }

    e = mbedtls_cipher_setkey(&cipher->ctx,
                              key,
                              len << 3,
                              encrypt ? MBEDTLS_ENCRYPT : MBEDTLS_DECRYPT);
    if (e != 0) {
        tt_crypto_error("fail to set cipher key");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_cipher_set_iv(IN tt_cipher_t *cipher,
                             IN tt_u8_t *iv,
                             IN tt_u32_t len)
{
    int e;

    e = mbedtls_cipher_set_iv(&cipher->ctx, iv, len);
    if (e != 0) {
        tt_crypto_error("fail to set iv");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_cipher_set_pad(IN tt_cipher_t *cipher,
                              IN tt_cipher_padding_t padding)
{
    int e;

    e = mbedtls_cipher_set_padding_mode(&cipher->ctx,
                                        ___cipher_padding_map[padding]);
    if (e != 0) {
        tt_crypto_error("fail to set padding");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_cipher_set_aad(IN tt_cipher_t *cipher,
                              IN tt_u8_t *aad,
                              IN tt_u32_t len)
{
    int e;

    e = mbedtls_cipher_update_ad(&cipher->ctx, aad, len);
    if (e != 0) {
        tt_crypto_error("fail to set additional data");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_cipher_update(IN tt_cipher_t *cipher,
                             IN tt_u8_t *input,
                             IN tt_u32_t ilen,
                             IN tt_u8_t *output,
                             IN tt_u32_t *olen)
{
    size_t i_n = ilen, o_n = *olen;
    int e;

    e = mbedtls_cipher_update(&cipher->ctx, input, i_n, output, &o_n);
    if (e != 0) {
        tt_crypto_error("fail to update cipher");
        return TT_FAIL;
    }
    *olen = (tt_u32_t)o_n;

    return TT_SUCCESS;
}

tt_result_t tt_cipher_finish(IN tt_cipher_t *cipher,
                             IN tt_u8_t *output,
                             IN tt_u32_t *olen)
{
    size_t o_n = *olen;
    int e;

    e = mbedtls_cipher_finish(&cipher->ctx, output, &o_n);
    if (e != 0) {
        tt_crypto_error("fail to finish cipher");
        return TT_FAIL;
    }
    *olen = (tt_u32_t)o_n;

    return TT_SUCCESS;
}

tt_result_t tt_cipher_tag(IN tt_cipher_t *cipher,
                          IN tt_u8_t *tag,
                          IN tt_u32_t len)
{
    int e;

    e = mbedtls_cipher_write_tag(&cipher->ctx, tag, len);
    if (e != 0) {
        tt_crypto_error("fail to tag cipher");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_cipher_auth(IN tt_cipher_t *cipher,
                           IN tt_u8_t *tag,
                           IN tt_u32_t len)
{
    int e;

    e = mbedtls_cipher_check_tag(&cipher->ctx, tag, len);
    if (e != 0) {
        tt_crypto_error("fail to auth cipher");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_cipher_update_buf(IN tt_cipher_t *cipher,
                                 IN tt_u8_t *input,
                                 IN tt_u32_t ilen,
                                 IN tt_buf_t *obuf)
{
    tt_u8_t *output;
    size_t olen;
    int e;

    if (!TT_OK(tt_buf_reserve(obuf,
                              ilen + mbedtls_cipher_get_block_size(
                                         &cipher->ctx)))) {
        TT_ERROR("fail to reserve output space");
        return TT_FAIL;
    }

    output = TT_BUF_WPOS(obuf);
    olen = TT_BUF_WLEN(obuf);
    e = mbedtls_cipher_update(&cipher->ctx, input, ilen, output, &olen);
    if (e != 0) {
        tt_crypto_error("fail to update cipher");
        return TT_FAIL;
    }
    tt_buf_inc_wp(obuf, (tt_u32_t)olen);

    return TT_SUCCESS;
}

tt_result_t tt_cipher_finish_buf(IN tt_cipher_t *cipher, IN tt_buf_t *obuf)
{
    tt_u8_t *output;
    size_t olen;
    int e;

    if (!TT_OK(tt_buf_reserve(obuf,
                              mbedtls_cipher_get_block_size(&cipher->ctx)))) {
        TT_ERROR("fail to reserve output space");
        return TT_FAIL;
    }

    output = TT_BUF_WPOS(obuf);
    olen = TT_BUF_WLEN(obuf);
    e = mbedtls_cipher_finish(&cipher->ctx, output, &olen);
    if (e != 0) {
        tt_crypto_error("fail to finish cipher");
        return TT_FAIL;
    }
    tt_buf_inc_wp(obuf, (tt_u32_t)olen);

    return TT_SUCCESS;
}

tt_result_t tt_cipher_encrypt_tag(IN tt_cipher_t *cipher,
                                  IN tt_u8_t *iv,
                                  IN tt_u32_t iv_len,
                                  IN tt_u8_t *aad,
                                  IN tt_u32_t ad_len,
                                  IN tt_u8_t *input,
                                  IN tt_u32_t ilen,
                                  IN tt_u8_t *output,
                                  IN tt_u32_t *olen,
                                  IN tt_u8_t *tag,
                                  IN tt_u32_t tag_len)
{
    size_t o_n = *olen;
    int e;

    e = mbedtls_cipher_auth_encrypt(&cipher->ctx,
                                    iv,
                                    iv_len,
                                    aad,
                                    ad_len,
                                    input,
                                    ilen,
                                    output,
                                    &o_n,
                                    tag,
                                    tag_len);
    if (e != 0) {
        tt_crypto_error("fail to encrypt tag");
        return TT_FAIL;
    }
    *olen = (tt_u32_t)o_n;

    return TT_SUCCESS;
}

tt_result_t tt_cipher_decrypt_auth(IN tt_cipher_t *cipher,
                                   IN tt_u8_t *iv,
                                   IN tt_u32_t iv_len,
                                   IN tt_u8_t *aad,
                                   IN tt_u32_t ad_len,
                                   IN tt_u8_t *input,
                                   IN tt_u32_t ilen,
                                   IN tt_u8_t *output,
                                   IN tt_u32_t *olen,
                                   IN tt_u8_t *tag,
                                   IN tt_u32_t tag_len)
{
    size_t o_n = *olen;
    int e;

    e = mbedtls_cipher_auth_decrypt(&cipher->ctx,
                                    iv,
                                    iv_len,
                                    aad,
                                    ad_len,
                                    input,
                                    ilen,
                                    output,
                                    &o_n,
                                    tag,
                                    tag_len);
    if (e != 0) {
        tt_crypto_error("fail to decrypt auth");
        return TT_FAIL;
    }
    *olen = (tt_u32_t)o_n;

    return TT_SUCCESS;
}

tt_result_t tt_cipher_encrypt_tag_buf(IN tt_cipher_t *cipher,
                                      IN tt_u8_t *iv,
                                      IN tt_u32_t iv_len,
                                      IN tt_u8_t *aad,
                                      IN tt_u32_t ad_len,
                                      IN tt_u8_t *input,
                                      IN tt_u32_t ilen,
                                      IN tt_buf_t *obuf,
                                      IN tt_u8_t *tag,
                                      IN tt_u32_t tag_len)
{
    tt_u8_t *output;
    size_t olen;
    int e;

    if (!TT_OK(tt_buf_reserve(obuf,
                              ilen + mbedtls_cipher_get_block_size(
                                         &cipher->ctx)))) {
        TT_ERROR("fail to reserve output space");
        return TT_FAIL;
    }

    output = TT_BUF_WPOS(obuf);
    olen = TT_BUF_WLEN(obuf);
    e = mbedtls_cipher_auth_encrypt(&cipher->ctx,
                                    iv,
                                    iv_len,
                                    aad,
                                    ad_len,
                                    input,
                                    ilen,
                                    output,
                                    &olen,
                                    tag,
                                    tag_len);
    if (e != 0) {
        tt_crypto_error("fail to encrypt tag");
        return TT_FAIL;
    }
    tt_buf_inc_wp(obuf, (tt_u32_t)olen);

    return TT_SUCCESS;
}

tt_result_t tt_cipher_decrypt_auth_buf(IN tt_cipher_t *cipher,
                                       IN tt_u8_t *iv,
                                       IN tt_u32_t iv_len,
                                       IN tt_u8_t *aad,
                                       IN tt_u32_t ad_len,
                                       IN tt_u8_t *input,
                                       IN tt_u32_t ilen,
                                       IN tt_buf_t *obuf,
                                       IN tt_u8_t *tag,
                                       IN tt_u32_t tag_len)
{
    tt_u8_t *output;
    size_t olen;
    int e;

    if (!TT_OK(tt_buf_reserve(obuf, ilen))) {
        TT_ERROR("fail to reserve output space");
        return TT_FAIL;
    }

    output = TT_BUF_WPOS(obuf);
    olen = TT_BUF_WLEN(obuf);
    e = mbedtls_cipher_auth_decrypt(&cipher->ctx,
                                    iv,
                                    iv_len,
                                    aad,
                                    ad_len,
                                    input,
                                    ilen,
                                    output,
                                    &olen,
                                    tag,
                                    tag_len);
    if (e != 0) {
        tt_crypto_error("fail to decrypt auth");
        return TT_FAIL;
    }
    tt_buf_inc_wp(obuf, (tt_u32_t)olen);

    return TT_SUCCESS;
}
