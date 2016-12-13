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

#include <tt_rsa_native.h>

#include <algorithm/tt_buffer_format.h>
#include <crypto/tt_rsa.h>
#include <init/tt_profile.h>
#include <log/tt_log.h>
#include <memory/tt_memory_alloc.h>

#ifdef TSCM_PLATFORM_CRYPTO_ENABLE

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/sha.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __BN2BUF(bn, buf, fail_label)                                          \
    do {                                                                       \
        tt_u8_t *p;                                                            \
        tt_u32_t len;                                                          \
                                                                               \
        if ((bn) == NULL) {                                                    \
            TT_ERROR("null " #bn);                                             \
            goto fail_label;                                                   \
        }                                                                      \
                                                                               \
        len = BN_num_bytes((bn));                                              \
        p = (tt_u8_t *)tt_mem_alloc(len);                                      \
        if (p == NULL) {                                                       \
            TT_ERROR("no mem for " #bn);                                       \
            goto fail_label;                                                   \
        }                                                                      \
        tt_memset(p, 0, len);                                                  \
        BN_bn2bin((bn), p);                                                    \
                                                                               \
        (buf)->addr = p;                                                       \
        (buf)->len = len;                                                      \
    } while (0)

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef \
tt_result_t (*__rsa_encrypt_t)(IN tt_rsa_ntv_t *sys_rsa,
                               IN tt_u8_t *input,
                               IN tt_u32_t input_len,
                               OUT tt_u8_t *output,
                               IN OUT tt_u32_t *output_len);
typedef \
tt_result_t (*__rsa_decrypt_t)(IN tt_rsa_ntv_t *sys_rsa,
                               IN tt_u8_t *input,
                               IN tt_u32_t input_len,
                               OUT tt_u8_t *output,
                               IN OUT tt_u32_t *output_len);

typedef \
tt_result_t (*__rsa_sign_t)(IN tt_rsa_ntv_t *sys_rsa,
                            IN tt_u8_t *input,
                            IN tt_u32_t input_len,
                            OUT tt_u8_t *signature,
                            IN OUT tt_u32_t *signature_len);
typedef \
tt_result_t (*__rsa_verify_t)(IN tt_rsa_ntv_t *sys_rsa,
                              IN tt_u8_t *input,
                              IN tt_u32_t input_len,
                              IN tt_u8_t *signature,
                              IN tt_u32_t signature_len);

typedef struct
{
    __rsa_encrypt_t encrypt;
    __rsa_decrypt_t decrypt;

    __rsa_sign_t sign;
    __rsa_verify_t verify;
} __rsa_itf_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_result_t __rsa_public(IN tt_rsa_ntv_t *sys_rsa,
                                IN tt_blob_t *key_data,
                                IN struct tt_rsa_attr_s *attr);
static tt_result_t __rsa_private_pkcs1(IN tt_rsa_ntv_t *sys_rsa,
                                       IN tt_blob_t *key_data,
                                       IN struct tt_rsa_attr_s *attr);
static tt_result_t __rsa_private_pkcs8(IN tt_rsa_ntv_t *sys_rsa,
                                       IN tt_blob_t *key_data,
                                       IN struct tt_rsa_attr_s *attr);

// TT_RSA_PADDING_NONE
static tt_result_t __rsa_encrypt_none(IN tt_rsa_ntv_t *sys_rsa,
                                      IN tt_u8_t *input,
                                      IN tt_u32_t input_len,
                                      OUT tt_u8_t *output,
                                      IN OUT tt_u32_t *output_len);
static tt_result_t __rsa_decrypt_none(IN tt_rsa_ntv_t *sys_rsa,
                                      IN tt_u8_t *input,
                                      IN tt_u32_t input_len,
                                      OUT tt_u8_t *output,
                                      IN OUT tt_u32_t *output_len);

// TT_RSA_PADDING_OAEP
static tt_result_t __rsa_encrypt_oaep(IN tt_rsa_ntv_t *sys_rsa,
                                      IN tt_u8_t *input,
                                      IN tt_u32_t input_len,
                                      OUT tt_u8_t *output,
                                      IN OUT tt_u32_t *output_len);
static tt_result_t __rsa_decrypt_oaep(IN tt_rsa_ntv_t *sys_rsa,
                                      IN tt_u8_t *input,
                                      IN tt_u32_t input_len,
                                      OUT tt_u8_t *output,
                                      IN OUT tt_u32_t *output_len);

// TT_RSA_PADDING_PKCS1
static tt_result_t __rsa_encrypt_pkcs1(IN tt_rsa_ntv_t *sys_rsa,
                                       IN tt_u8_t *input,
                                       IN tt_u32_t input_len,
                                       OUT tt_u8_t *output,
                                       IN OUT tt_u32_t *output_len);
static tt_result_t __rsa_decrypt_pkcs1(IN tt_rsa_ntv_t *sys_rsa,
                                       IN tt_u8_t *input,
                                       IN tt_u32_t input_len,
                                       OUT tt_u8_t *output,
                                       IN OUT tt_u32_t *output_len);
static tt_result_t __rsa_sign_pkcs1(IN tt_rsa_ntv_t *sys_rsa,
                                    IN tt_u8_t *input,
                                    IN tt_u32_t input_len,
                                    OUT tt_u8_t *signature,
                                    IN OUT tt_u32_t *signature_len);
static tt_result_t __rsa_verify_pkcs1(IN tt_rsa_ntv_t *sys_rsa,
                                      IN tt_u8_t *input,
                                      IN tt_u32_t input_len,
                                      IN tt_u8_t *signature,
                                      IN tt_u32_t signature_len);

static __rsa_itf_t tt_s_rsa_itf[TT_RSA_PADDING_NUM] = {
    // TT_RSA_PADDING_NONE
    {
        __rsa_encrypt_none,
        __rsa_decrypt_none,

        NULL,
        NULL,
    },

    // TT_RSA_PADDING_OAEP
    {
        __rsa_encrypt_oaep,
        __rsa_decrypt_oaep,

        NULL,
        NULL,
    },

    // TT_RSA_PADDING_PKCS1
    {
        __rsa_encrypt_pkcs1,
        __rsa_decrypt_pkcs1,

        __rsa_sign_pkcs1,
        __rsa_verify_pkcs1,
    }};

static tt_u32_t __rsa_size_byte(tt_rsa_size_t size);

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __rsa_number_get_public(IN RSA *openssl_rsa,
                                           IN tt_rsa_pubnum_t *pub_n);
static tt_result_t __rsa_number_get_private(IN RSA *openssl_rsa,
                                            IN tt_rsa_privnum_t *priv_n);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_rsa_component_init_ntv(IN tt_profile_t *profile)
{
    return TT_SUCCESS;
}

tt_result_t tt_rsa_generate_ntv(OUT tt_rsa_ntv_t *sys_rsa,
                                IN tt_rsa_size_t size,
                                IN struct tt_rsa_attr_s *attr)
{
    int bits = 0;
    BIGNUM *rsa_exp = NULL;
    RSA *openssl_rsa = NULL;

    switch (size) {
        case TT_RSA_SIZE_1024BIT: {
            bits = 1024;
        } break;
        case TT_RSA_SIZE_2048BIT: {
            bits = 2048;
        } break;

        default: {
            TT_ERROR("unsupported rsa size: %d", size);
            return TT_FAIL;
        } break;
    }

    rsa_exp = BN_new();
    if (rsa_exp == NULL) {
        TT_ERROR("fail to new rsa exp");
        return TT_FAIL;
    }

    // always use 65537
    if (BN_set_word(rsa_exp, RSA_F4) != 1) {
        TT_ERROR("fail to set rsa exp");
        goto rg_fail;
    }

    openssl_rsa = RSA_new();
    if (openssl_rsa == NULL) {
        TT_ERROR("fail to set rsa exp");
        goto rg_fail;
    }

    if (RSA_generate_key_ex(openssl_rsa, bits, rsa_exp, NULL) != 1) {
        TT_ERROR("fail to generate rsa");
        goto rg_fail;
    }

    sys_rsa->rsa = openssl_rsa;
    sys_rsa->type = TT_RSA_TYPE_PRIVATE;

    return TT_SUCCESS;

rg_fail:

    if (openssl_rsa != NULL) {
        RSA_free(openssl_rsa);
    }

    if (rsa_exp != NULL) {
        BN_free(rsa_exp);
    }

    return TT_FAIL;
}

tt_result_t tt_rsa_create_ntv(IN tt_rsa_ntv_t *sys_rsa,
                              IN tt_rsa_format_t key_format,
                              IN tt_rsa_type_t key_type,
                              IN tt_blob_t *key_data,
                              IN struct tt_rsa_attr_s *attr)
{
    if (key_type == TT_RSA_TYPE_PUBLIC) {
        return __rsa_public(sys_rsa, key_data, attr);
    }
    TT_ASSERT(key_type == TT_RSA_TYPE_PRIVATE);

    switch (key_format) {
        case TT_RSA_FORMAT_PKCS1: {
            return __rsa_private_pkcs1(sys_rsa, key_data, attr);
        } break;
        case TT_RSA_FORMAT_PKCS8: {
            return __rsa_private_pkcs8(sys_rsa, key_data, attr);
        } break;

        default: {
            TT_ERROR("invalid rsa format[%d]", key_format);
            return TT_FAIL;
        } break;
    }
}

void tt_rsa_destroy_ntv(IN tt_rsa_ntv_t *sys_rsa)
{
    if (sys_rsa->rsa != NULL) {
        RSA_free(sys_rsa->rsa);
    }
}

tt_result_t tt_rsa_encrypt_ntv(IN tt_rsa_ntv_t *sys_rsa,
                               IN tt_u8_t *input,
                               IN tt_u32_t input_len,
                               OUT tt_u8_t *output,
                               IN OUT tt_u32_t *output_len)
{
    tt_rsa_t *rsa;
    __rsa_encrypt_t encrypt;

    rsa = TT_CONTAINER(sys_rsa, tt_rsa_t, sys_rsa);
    encrypt = tt_s_rsa_itf[rsa->attr.padding.mode].encrypt;
    if (encrypt != NULL) {
        return encrypt(sys_rsa, input, input_len, output, output_len);
    } else {
        TT_ERROR("can not encrypt in padding: %d", rsa->attr.padding.mode);
        return TT_FAIL;
    }
}

tt_result_t tt_rsa_decrypt_ntv(IN tt_rsa_ntv_t *sys_rsa,
                               IN tt_u8_t *input,
                               IN tt_u32_t input_len,
                               OUT tt_u8_t *output,
                               IN OUT tt_u32_t *output_len)
{
    tt_rsa_t *rsa;
    __rsa_decrypt_t decrypt;

    rsa = TT_CONTAINER(sys_rsa, tt_rsa_t, sys_rsa);
    decrypt = tt_s_rsa_itf[rsa->attr.padding.mode].decrypt;
    if (decrypt != NULL) {
        return decrypt(sys_rsa, input, input_len, output, output_len);
    } else {
        TT_ERROR("can not decrypt in padding: %d", rsa->attr.padding.mode);
        return TT_FAIL;
    }
}

tt_result_t tt_rsa_sign_ntv(IN tt_rsa_ntv_t *sys_rsa,
                            IN tt_u8_t *input,
                            IN tt_u32_t input_len,
                            OUT tt_u8_t *signature,
                            IN OUT tt_u32_t *signature_len)
{
    tt_rsa_t *rsa;
    __rsa_sign_t sign;

    rsa = TT_CONTAINER(sys_rsa, tt_rsa_t, sys_rsa);
    sign = tt_s_rsa_itf[rsa->attr.padding.mode].sign;
    if (sign != NULL) {
        return sign(sys_rsa, input, input_len, signature, signature_len);
    } else {
        TT_ERROR("can not sign in padding: %d", rsa->attr.padding.mode);
        return TT_FAIL;
    }
}

tt_result_t tt_rsa_verify_ntv(IN tt_rsa_ntv_t *sys_rsa,
                              IN tt_u8_t *input,
                              IN tt_u32_t input_len,
                              IN tt_u8_t *signature,
                              IN tt_u32_t signature_len)
{
    tt_rsa_t *rsa;
    __rsa_verify_t verify;

    rsa = TT_CONTAINER(sys_rsa, tt_rsa_t, sys_rsa);
    verify = tt_s_rsa_itf[rsa->attr.padding.mode].verify;
    if (verify != NULL) {
        return verify(sys_rsa, input, input_len, signature, signature_len);
    } else {
        TT_ERROR("can not verify in padding: %d", rsa->attr.padding.mode);
        return TT_FAIL;
    }
}

tt_result_t __rsa_public(IN tt_rsa_ntv_t *sys_rsa,
                         IN tt_blob_t *key_data,
                         IN struct tt_rsa_attr_s *attr)
{
    tt_rsa_t *rsa = TT_CONTAINER(sys_rsa, tt_rsa_t, sys_rsa);

    RSA *openssl_rsa = NULL;
    int openssl_rsa_size = 0;

    if (attr->pem_armor) {
        BIO *bio = BIO_new_mem_buf(key_data->addr, key_data->len);
        if (bio == NULL) {
            TT_ERROR("fail to new mem bio");
            return TT_FAIL;
        }

        openssl_rsa =
            PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, (void *)attr->password);
        BIO_free_all(bio);
    } else {
        const unsigned char *p = key_data->addr;

        openssl_rsa = d2i_RSAPublicKey(NULL, &p, key_data->len);
    }
    if (openssl_rsa == NULL) {
        __SSL_ERROR("fail to read rsa key");
        return TT_FAIL;
    }

    openssl_rsa_size = RSA_size(openssl_rsa);
    if (openssl_rsa_size == 128) {
        rsa->size = TT_RSA_SIZE_1024BIT;
    } else if (openssl_rsa_size == 256) {
        rsa->size = TT_RSA_SIZE_2048BIT;
    } else {
        TT_ERROR("rsa size[%d] is not supported", openssl_rsa_size);

        RSA_free(openssl_rsa);
        return TT_FAIL;
    }

    sys_rsa->rsa = openssl_rsa;
    sys_rsa->type = TT_RSA_TYPE_PUBLIC;

    return TT_SUCCESS;
}


tt_result_t __rsa_private_pkcs1(IN tt_rsa_ntv_t *sys_rsa,
                                IN tt_blob_t *key_data,
                                IN struct tt_rsa_attr_s *attr)
{
    tt_rsa_t *rsa = TT_CONTAINER(sys_rsa, tt_rsa_t, sys_rsa);

    RSA *openssl_rsa = NULL;
    int openssl_rsa_size = 0;

    if (attr->pem_armor) {
        BIO *bio = BIO_new_mem_buf(key_data->addr, key_data->len);
        if (bio == NULL) {
            TT_ERROR("fail to new mem bio");
            return TT_FAIL;
        }

        openssl_rsa =
            PEM_read_bio_RSAPrivateKey(bio, NULL, NULL, (void *)attr->password);
        BIO_free_all(bio);
    } else {
        const unsigned char *p = key_data->addr;

        openssl_rsa = d2i_RSAPrivateKey(NULL, &p, key_data->len);
    }
    if (openssl_rsa == NULL) {
        __SSL_ERROR("fail to read rsa key");
        return TT_FAIL;
    }

    openssl_rsa_size = RSA_size(openssl_rsa);
    if (openssl_rsa_size == 128) {
        rsa->size = TT_RSA_SIZE_1024BIT;
    } else if (openssl_rsa_size == 256) {
        rsa->size = TT_RSA_SIZE_2048BIT;
    } else {
        TT_ERROR("rsa size[%d] is not supported", openssl_rsa_size);

        RSA_free(openssl_rsa);
        return TT_FAIL;
    }

    sys_rsa->rsa = openssl_rsa;
    sys_rsa->type = TT_RSA_TYPE_PRIVATE;

    return TT_SUCCESS;
}

tt_result_t __rsa_private_pkcs8(IN tt_rsa_ntv_t *sys_rsa,
                                IN tt_blob_t *key_data,
                                IN struct tt_rsa_attr_s *attr)
{
    tt_rsa_t *rsa = TT_CONTAINER(sys_rsa, tt_rsa_t, sys_rsa);

    BIO *bio = NULL;
    RSA *openssl_rsa = NULL;
    int openssl_rsa_size = 0;

    bio = BIO_new_mem_buf(key_data->addr, key_data->len);
    if (bio == NULL) {
        TT_ERROR("fail to new mem bio");
        return TT_FAIL;
    }

    if (attr->pem_armor) {
        openssl_rsa =
            PEM_read_bio_RSAPrivateKey(bio, NULL, NULL, (void *)attr->password);
    } else {
        EVP_PKEY *epk = NULL;

        if (attr->password) {
            epk = d2i_PKCS8PrivateKey_bio(bio,
                                          NULL,
                                          NULL,
                                          (void *)attr->password);
        } else {
            PKCS8_PRIV_KEY_INFO *p8inf;

            // even there is no official docs of
            // d2i_PKCS8_PRIV_KEY_INFO_bio, if this api is removed,
            // we'll always use encrypted pkcs8 file, and do not
            // decrypte pkcs8 file in app layer, see
            // __rsa_pkcs8_decrypt()

            p8inf = d2i_PKCS8_PRIV_KEY_INFO_bio(bio, NULL);
            if (p8inf == NULL) {
                TT_ERROR("fail to open unencrypted pkcs8 key");
            } else {
                epk = EVP_PKCS82PKEY(p8inf);
                PKCS8_PRIV_KEY_INFO_free(p8inf);
            }
        }
        if (epk == NULL) {
            TT_ERROR("fail to open pkcs8 key");
        } else {
            openssl_rsa = EVP_PKEY_get1_RSA(epk);
            EVP_PKEY_set1_RSA(epk, NULL);
            EVP_PKEY_free(epk);
        }
    }
    BIO_free_all(bio);
    if (openssl_rsa == NULL) {
        __SSL_ERROR("fail to read rsa key");
        return TT_FAIL;
    }

    openssl_rsa_size = RSA_size(openssl_rsa);
    if (openssl_rsa_size == 128) {
        rsa->size = TT_RSA_SIZE_1024BIT;
    } else if (openssl_rsa_size == 256) {
        rsa->size = TT_RSA_SIZE_2048BIT;
    } else {
        TT_ERROR("rsa size[%d] is not supported", openssl_rsa_size);

        RSA_free(openssl_rsa);
        return TT_FAIL;
    }

    sys_rsa->rsa = openssl_rsa;
    sys_rsa->type = TT_RSA_TYPE_PRIVATE;

    return TT_SUCCESS;
}


// TT_RSA_PADDING_NONE
tt_result_t __rsa_encrypt_none(IN tt_rsa_ntv_t *sys_rsa,
                               IN tt_u8_t *input,
                               IN tt_u32_t input_len,
                               OUT tt_u8_t *output,
                               IN OUT tt_u32_t *output_len)
{
    int n = RSA_public_encrypt(input_len,
                               input,
                               output,
                               sys_rsa->rsa,
                               RSA_NO_PADDING);
    if (n < 0) {
        __SSL_ERROR("rsa encrypt failed");
        return TT_FAIL;
    }
    *output_len = n;

    return TT_SUCCESS;
}

tt_result_t __rsa_decrypt_none(IN tt_rsa_ntv_t *sys_rsa,
                               IN tt_u8_t *input,
                               IN tt_u32_t input_len,
                               OUT tt_u8_t *output,
                               IN OUT tt_u32_t *output_len)
{
    int n = RSA_private_decrypt(input_len,
                                input,
                                output,
                                sys_rsa->rsa,
                                RSA_NO_PADDING);
    if (n < 0) {
        __SSL_ERROR("rsa decrypt failed");
        return TT_FAIL;
    }
    *output_len = n;

    return TT_SUCCESS;
}

// TT_RSA_PADDING_OAEP
tt_result_t __rsa_encrypt_oaep(IN tt_rsa_ntv_t *sys_rsa,
                               IN tt_u8_t *input,
                               IN tt_u32_t input_len,
                               OUT tt_u8_t *output,
                               IN OUT tt_u32_t *output_len)
{
    int n = RSA_public_encrypt(input_len,
                               input,
                               output,
                               sys_rsa->rsa,
                               RSA_PKCS1_OAEP_PADDING);
    if (n < 0) {
        __SSL_ERROR("rsa encrypt failed");
        return TT_FAIL;
    }
    *output_len = n;

    return TT_SUCCESS;
}

tt_result_t __rsa_decrypt_oaep(IN tt_rsa_ntv_t *sys_rsa,
                               IN tt_u8_t *input,
                               IN tt_u32_t input_len,
                               OUT tt_u8_t *output,
                               IN OUT tt_u32_t *output_len)
{
    int n = RSA_private_decrypt(input_len,
                                input,
                                output,
                                sys_rsa->rsa,
                                RSA_PKCS1_OAEP_PADDING);
    if (n < 0) {
        __SSL_ERROR("rsa decrypt failed");
        return TT_FAIL;
    }
    *output_len = n;

    return TT_SUCCESS;
}

// TT_RSA_PADDING_PKCS1
tt_result_t __rsa_encrypt_pkcs1(IN tt_rsa_ntv_t *sys_rsa,
                                IN tt_u8_t *input,
                                IN tt_u32_t input_len,
                                OUT tt_u8_t *output,
                                IN OUT tt_u32_t *output_len)
{
    int n = RSA_public_encrypt(input_len,
                               input,
                               output,
                               sys_rsa->rsa,
                               RSA_PKCS1_PADDING);
    if (n < 0) {
        __SSL_ERROR("rsa encrypt failed");
        return TT_FAIL;
    }
    *output_len = n;

    return TT_SUCCESS;
}

tt_result_t __rsa_decrypt_pkcs1(IN tt_rsa_ntv_t *sys_rsa,
                                IN tt_u8_t *input,
                                IN tt_u32_t input_len,
                                OUT tt_u8_t *output,
                                IN OUT tt_u32_t *output_len)
{
    int n = RSA_private_decrypt(input_len,
                                input,
                                output,
                                sys_rsa->rsa,
                                RSA_PKCS1_PADDING);
    if (n < 0) {
        __SSL_ERROR("rsa decrypt failed");
        return TT_FAIL;
    }
    *output_len = n;

    return TT_SUCCESS;
}

tt_result_t __rsa_sign_pkcs1(IN tt_rsa_ntv_t *sys_rsa,
                             IN tt_u8_t *input,
                             IN tt_u32_t input_len,
                             OUT tt_u8_t *signature,
                             IN OUT tt_u32_t *signature_len)
{
    unsigned char hash[SHA_DIGEST_LENGTH];
    unsigned int siglen = *signature;

    SHA1(input, input_len, hash);

    // tt_rsa_sign() should have already checked
    if (RSA_sign(NID_sha1,
                 hash,
                 sizeof(hash),
                 signature,
                 &siglen,
                 sys_rsa->rsa) != 1) {
        __SSL_ERROR("rsa sign failed");
        return TT_FAIL;
    }
    *signature_len = siglen;

    return TT_SUCCESS;
}

tt_result_t __rsa_verify_pkcs1(IN tt_rsa_ntv_t *sys_rsa,
                               IN tt_u8_t *input,
                               IN tt_u32_t input_len,
                               IN tt_u8_t *signature,
                               IN tt_u32_t signature_len)
{
    unsigned char hash[SHA_DIGEST_LENGTH];
    unsigned int siglen = signature_len;

    SHA1(input, input_len, hash);

    if (RSA_verify(NID_sha1,
                   hash,
                   sizeof(hash),
                   signature,
                   siglen,
                   sys_rsa->rsa) != 1) {
        __SSL_ERROR("rsa verify failed");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_rsa_show_ntv(IN tt_rsa_ntv_t *sys_rsa)
{
    tt_rsa_number_t rsa_number;
    tt_buf_t printable;

    if (!TT_OK(tt_rsa_get_number_ntv(sys_rsa, &rsa_number))) {
        return TT_FAIL;
    }

    tt_buf_init(&printable, NULL);

    if (sys_rsa->type == TT_RSA_TYPE_PUBLIC) {
        tt_rsa_pubnum_t *pub_num = &rsa_number.pubnum;

        tt_buf_reset_rwp(&printable);
        tt_buf_put_hex2cstr(&printable,
                            pub_num->modulus.addr,
                            pub_num->modulus.len);
        tt_buf_put_u8(&printable, 0);
        TT_INFO("  modulus: %s", TT_BUF_RPOS(&printable));

        tt_buf_reset_rwp(&printable);
        tt_buf_put_hex2cstr(&printable,
                            pub_num->pub_exp.addr,
                            pub_num->pub_exp.len);
        tt_buf_put_u8(&printable, 0);
        TT_INFO("  pub_exp: %s", TT_BUF_RPOS(&printable));
    } else {
        tt_rsa_privnum_t *priv_num = &rsa_number.privnum;

        tt_buf_reset_rwp(&printable);
        tt_buf_put_hex2cstr(&printable,
                            priv_num->modulus.addr,
                            priv_num->modulus.len);
        tt_buf_put_u8(&printable, 0);
        TT_INFO("  modulus: %s", TT_BUF_RPOS(&printable));

        tt_buf_reset_rwp(&printable);
        tt_buf_put_hex2cstr(&printable,
                            priv_num->pub_exp.addr,
                            priv_num->pub_exp.len);
        tt_buf_put_u8(&printable, 0);
        TT_INFO("  pub_exp: %s", TT_BUF_RPOS(&printable));

        tt_buf_reset_rwp(&printable);
        tt_buf_put_hex2cstr(&printable,
                            priv_num->priv_exp.addr,
                            priv_num->priv_exp.len);
        tt_buf_put_u8(&printable, 0);
        TT_INFO("  priv_exp: %s", TT_BUF_RPOS(&printable));

        tt_buf_reset_rwp(&printable);
        tt_buf_put_hex2cstr(&printable,
                            priv_num->prime1.addr,
                            priv_num->prime1.len);
        tt_buf_put_u8(&printable, 0);
        TT_INFO("  prime1: %s", TT_BUF_RPOS(&printable));

        tt_buf_reset_rwp(&printable);
        tt_buf_put_hex2cstr(&printable,
                            priv_num->prime2.addr,
                            priv_num->prime2.len);
        tt_buf_put_u8(&printable, 0);
        TT_INFO("  prime2: %s", TT_BUF_RPOS(&printable));

        tt_buf_reset_rwp(&printable);
        tt_buf_put_hex2cstr(&printable,
                            priv_num->exp1.addr,
                            priv_num->exp1.len);
        tt_buf_put_u8(&printable, 0);
        TT_INFO("  exp1: %s", TT_BUF_RPOS(&printable));

        tt_buf_reset_rwp(&printable);
        tt_buf_put_hex2cstr(&printable,
                            priv_num->exp2.addr,
                            priv_num->exp2.len);
        tt_buf_put_u8(&printable, 0);
        TT_INFO("  exp2: %s", TT_BUF_RPOS(&printable));

        tt_buf_reset_rwp(&printable);
        tt_buf_put_hex2cstr(&printable,
                            priv_num->coefficient.addr,
                            priv_num->coefficient.len);
        tt_buf_put_u8(&printable, 0);
        TT_INFO("  coefficient: %s", TT_BUF_RPOS(&printable));
    }

    tt_rsa_number_destroy(&rsa_number);
    tt_buf_destroy(&printable);

    return TT_SUCCESS;
}

tt_result_t tt_rsa_get_number_ntv(IN tt_rsa_ntv_t *sys_rsa,
                                  IN struct tt_rsa_number_s *rsa_number)
{
    if (sys_rsa->type == TT_RSA_TYPE_PUBLIC) {
        rsa_number->type = TT_RSA_TYPE_PUBLIC;
        return __rsa_number_get_public(sys_rsa->rsa, &rsa_number->pubnum);
    } else {
        TT_ASSERT(sys_rsa->type == TT_RSA_TYPE_PRIVATE);

        rsa_number->type = TT_RSA_TYPE_PRIVATE;
        return __rsa_number_get_private(sys_rsa->rsa, &rsa_number->privnum);
    }
}

tt_result_t __rsa_number_get_public(IN RSA *openssl_rsa,
                                    IN tt_rsa_pubnum_t *pub_n)
{
    /*
     kSecFormatBSAFE:

     RSAPublicKey ::= SEQUENCE {
        modulus INTEGER, -- n
        publicExponent INTEGER -- e
     }
     */

    tt_memset(pub_n, 0, sizeof(tt_rsa_pubnum_t));

    // modulus
    __BN2BUF(openssl_rsa->n, &pub_n->modulus, gp_fail);

    // pub exp
    __BN2BUF(openssl_rsa->e, &pub_n->pub_exp, gp_fail);

    return TT_SUCCESS;

gp_fail:

    if (pub_n->modulus.addr != NULL) {
        tt_mem_free(pub_n->modulus.addr);
    }

    if (pub_n->pub_exp.addr != NULL) {
        tt_mem_free(pub_n->pub_exp.addr);
    }

    return TT_FAIL;
}

tt_result_t __rsa_number_get_private(IN RSA *openssl_rsa,
                                     IN tt_rsa_privnum_t *priv_n)
{
    /*
     kSecFormatBSAFE:

     RSAPrivateKey ::= SEQUENCE {
        version Version,
        modulus INTEGER, -- n
        publicExponent INTEGER, -- e
        privateExponent INTEGER, -- d
        prime1 INTEGER, -- p
        prime2 INTEGER, -- q
        exponent1 INTEGER, -- d mod (p-1)
        exponent2 INTEGER, -- d mod (q-1)
        coefficient INTEGER, -- (inverse of q) mod p
        otherPrimeInfos OtherPrimeInfos OPTIONAL
     }
     */

    tt_memset(priv_n, 0, sizeof(tt_rsa_privnum_t));

    // modulus
    __BN2BUF(openssl_rsa->n, &priv_n->modulus, gp_fail);

    // pub exp
    __BN2BUF(openssl_rsa->e, &priv_n->pub_exp, gp_fail);

    // priv exp
    __BN2BUF(openssl_rsa->d, &priv_n->priv_exp, gp_fail);

    // prime1
    __BN2BUF(openssl_rsa->p, &priv_n->prime1, gp_fail);

    // prime2
    __BN2BUF(openssl_rsa->q, &priv_n->prime2, gp_fail);

    // exp1
    __BN2BUF(openssl_rsa->dmp1, &priv_n->exp1, gp_fail);

    // exp2
    __BN2BUF(openssl_rsa->dmq1, &priv_n->exp2, gp_fail);

    // coefficient
    __BN2BUF(openssl_rsa->iqmp, &priv_n->coefficient, gp_fail);

    return TT_SUCCESS;

gp_fail:

    if (priv_n->modulus.addr != NULL) {
        tt_mem_free(priv_n->modulus.addr);
    }

    if (priv_n->pub_exp.addr != NULL) {
        tt_mem_free(priv_n->pub_exp.addr);
    }

    if (priv_n->priv_exp.addr != NULL) {
        tt_mem_free(priv_n->priv_exp.addr);
    }

    if (priv_n->prime1.addr != NULL) {
        tt_mem_free(priv_n->prime1.addr);
    }

    if (priv_n->prime2.addr != NULL) {
        tt_mem_free(priv_n->prime2.addr);
    }

    if (priv_n->exp1.addr != NULL) {
        tt_mem_free(priv_n->exp1.addr);
    }

    if (priv_n->exp2.addr != NULL) {
        tt_mem_free(priv_n->exp2.addr);
    }

    if (priv_n->coefficient.addr != NULL) {
        tt_mem_free(priv_n->coefficient.addr);
    }

    return TT_FAIL;
}

tt_u32_t __rsa_size_byte(tt_rsa_size_t size)
{
    switch (size) {
        case TT_RSA_SIZE_1024BIT: {
            return 128;
        }
        case TT_RSA_SIZE_2048BIT: {
            return 256;
        }

        default: {
            return 0;
        }
    }
}

#endif
