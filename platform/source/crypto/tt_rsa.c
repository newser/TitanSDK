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

#include <crypto/tt_rsa.h>

#include <crypto/tt_crypto.h>
#include <crypto/tt_ctr_drbg.h>
#include <crypto/tt_public_key.h>
#include <misc/tt_assert.h>
#include <misc/tt_util.h>

#include <rsa.h>

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

void tt_rsa_init(IN tt_rsa_t *rsa)
{
    TT_ASSERT(rsa != NULL);

    mbedtls_rsa_init(&rsa->ctx, 0, 0);
}

extern void tt_rsa_destroy(IN tt_rsa_t *rsa)
{
    TT_ASSERT(rsa != NULL);

    mbedtls_rsa_free(&rsa->ctx);
}

tt_result_t tt_rsa_load(IN tt_rsa_t *rsa, IN tt_pk_t *pk)
{
    mbedtls_rsa_context *ctx;
    int e;

    TT_ASSERT(rsa != NULL);
    TT_ASSERT(pk != NULL);

    if (tt_pk_get_type(pk) != TT_RSA) {
        TT_ERROR("rsa is not rsa key");
        return TT_FAIL;
    }

    ctx = mbedtls_pk_rsa(pk->ctx);

    e = mbedtls_rsa_copy(&rsa->ctx, ctx);
    if (e != 0) {
        tt_crypto_error("fail to copy rsa");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_rsa_generate(OUT tt_rsa_t *rsa,
                            IN tt_u32_t bit_num,
                            IN tt_u32_t exponent)
{
    mbedtls_rsa_context *ctx;
    int e;

    TT_ASSERT(rsa != NULL);

    ctx = &rsa->ctx;

    e = mbedtls_rsa_gen_key(ctx,
                            tt_ctr_drbg,
                            tt_current_ctr_drbg(),
                            bit_num,
                            exponent);
    if (e != 0) {
        tt_crypto_error("fail to generate rsa");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_rsa_topub(IN tt_rsa_t *rsa, OUT tt_rsa_t *pub)
{
    mbedtls_rsa_context *ctx, *pub_ctx;
    int e;

    TT_ASSERT(rsa != NULL);
    TT_ASSERT(pub != NULL);

    ctx = &rsa->ctx;
    pub_ctx = &pub->ctx;

    mbedtls_rsa_free(pub_ctx);
    pub_ctx->ver = ctx->ver;
    pub_ctx->len = ctx->len;
    if (((e = mbedtls_mpi_copy(&pub_ctx->N, &ctx->N)) != 0) ||
        ((e = mbedtls_mpi_copy(&pub_ctx->E, &ctx->E)) != 0)) {
        tt_crypto_error("fail to copy pub rsa");
        mbedtls_rsa_free(pub_ctx);
        return TT_FAIL;
    }
    pub_ctx->padding = ctx->padding;
    pub_ctx->hash_id = ctx->hash_id;

    return TT_SUCCESS;
}

tt_result_t tt_rsa_check(IN tt_rsa_t *pub, IN tt_rsa_t *priv)
{
    int e;

    TT_ASSERT(pub != NULL);
    TT_ASSERT(priv != NULL);

    e = mbedtls_rsa_check_pub_priv(&pub->ctx, &priv->ctx);
    if (e != 0) {
        tt_crypto_error("rsa pub and priv does match");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_rsa_encrypt_pkcs1(IN tt_rsa_t *rsa,
                                 IN tt_u8_t *input,
                                 IN tt_u32_t ilen,
                                 IN tt_u8_t *output)
{
    mbedtls_rsa_context *ctx = &rsa->ctx;
    int e;

    mbedtls_rsa_set_padding(ctx, MBEDTLS_RSA_PKCS_V15, 0);

    e = mbedtls_rsa_rsaes_pkcs1_v15_encrypt(ctx,
                                            tt_ctr_drbg,
                                            tt_current_ctr_drbg(),
                                            MBEDTLS_RSA_PUBLIC,
                                            ilen,
                                            input,
                                            output);
    if (e != 0) {
        tt_crypto_error("rsa rsacs1 encrypt fail");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_rsa_decrypt_pkcs1(IN tt_rsa_t *rsa,
                                 IN tt_u8_t *input,
                                 IN tt_u8_t *output,
                                 IN tt_u32_t *olen)
{
    mbedtls_rsa_context *ctx = &rsa->ctx;
    size_t n = *olen;
    int e;

    mbedtls_rsa_set_padding(ctx, MBEDTLS_RSA_PKCS_V15, 0);

    e = mbedtls_rsa_rsaes_pkcs1_v15_decrypt(ctx,
                                            tt_ctr_drbg,
                                            tt_current_ctr_drbg(),
                                            MBEDTLS_RSA_PRIVATE,
                                            &n,
                                            input,
                                            output,
                                            n);
    if (e != 0) {
        tt_crypto_error("rsa rsacs1 decrypt fail");
        return TT_FAIL;
    }
    *olen = (tt_u32_t)n;

    return TT_SUCCESS;
}

tt_result_t tt_rsa_sign_pkcs1(IN tt_rsa_t *rsa,
                              IN tt_u8_t *input,
                              IN tt_u32_t ilen,
                              IN tt_md_type_t md_type,
                              IN tt_u8_t *sig)
{
    mbedtls_rsa_context *ctx = &rsa->ctx;
    mbedtls_md_type_t t;
    const mbedtls_md_info_t *md_info;
    tt_u8_t hash[MBEDTLS_MD_MAX_SIZE];
    tt_u32_t hashlen;
    int e;

    mbedtls_rsa_set_padding(ctx, MBEDTLS_RSA_PKCS_V15, 0);

    t = tt_g_md_type_map[md_type];
    md_info = mbedtls_md_info_from_type(t);
    mbedtls_md(md_info, input, ilen, hash);
    hashlen = mbedtls_md_get_size(md_info);

    e = mbedtls_rsa_rsassa_pkcs1_v15_sign(ctx,
                                          tt_ctr_drbg,
                                          tt_current_ctr_drbg(),
                                          MBEDTLS_RSA_PRIVATE,
                                          t,
                                          hashlen,
                                          hash,
                                          sig);
    if (e != 0) {
        tt_crypto_error("rsa rsacs1 sign fail");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_rsa_verify_pkcs1(IN tt_rsa_t *rsa,
                                IN tt_u8_t *input,
                                IN tt_u32_t ilen,
                                IN tt_md_type_t md_type,
                                IN tt_u8_t *sig)
{
    mbedtls_rsa_context *ctx = &rsa->ctx;
    mbedtls_md_type_t t;
    const mbedtls_md_info_t *md_info;
    tt_u8_t hash[MBEDTLS_MD_MAX_SIZE];
    tt_u32_t hashlen;
    int e;

    mbedtls_rsa_set_padding(ctx, MBEDTLS_RSA_PKCS_V15, 0);

    t = tt_g_md_type_map[md_type];
    md_info = mbedtls_md_info_from_type(t);
    mbedtls_md(md_info, input, ilen, hash);
    hashlen = mbedtls_md_get_size(md_info);

    e = mbedtls_rsa_rsassa_pkcs1_v15_verify(ctx,
                                            tt_ctr_drbg,
                                            tt_current_ctr_drbg(),
                                            MBEDTLS_RSA_PUBLIC,
                                            t,
                                            hashlen,
                                            hash,
                                            sig);
    if (e != 0) {
        tt_crypto_error("rsa rsacs1 verify fail");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_rsa_encrypt_oaep(IN tt_rsa_t *rsa,
                                IN tt_u8_t *input,
                                IN tt_u32_t ilen,
                                IN const tt_u8_t *label,
                                IN tt_u32_t label_len,
                                IN tt_md_type_t md_type,
                                IN tt_u8_t *output)
{
    mbedtls_rsa_context *ctx = &rsa->ctx;
    int e;

    mbedtls_rsa_set_padding(ctx,
                            MBEDTLS_RSA_PKCS_V21,
                            tt_g_md_type_map[md_type]);

    e = mbedtls_rsa_rsaes_oaep_encrypt(ctx,
                                       tt_ctr_drbg,
                                       tt_current_ctr_drbg(),
                                       MBEDTLS_RSA_PUBLIC,
                                       label,
                                       label_len,
                                       ilen,
                                       input,
                                       output);
    if (e != 0) {
        tt_crypto_error("rsa oaep encrypt fail");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_rsa_decrypt_oaep(IN tt_rsa_t *rsa,
                                IN tt_u8_t *input,
                                IN const tt_u8_t *label,
                                IN tt_u32_t label_len,
                                IN tt_md_type_t md_type,
                                IN tt_u8_t *output,
                                IN tt_u32_t *olen)
{
    mbedtls_rsa_context *ctx = &rsa->ctx;
    size_t n = *olen;
    int e;

    mbedtls_rsa_set_padding(ctx,
                            MBEDTLS_RSA_PKCS_V21,
                            tt_g_md_type_map[md_type]);

    e = mbedtls_rsa_rsaes_oaep_decrypt(ctx,
                                       tt_ctr_drbg,
                                       tt_current_ctr_drbg(),
                                       MBEDTLS_RSA_PRIVATE,
                                       label,
                                       label_len,
                                       &n,
                                       input,
                                       output,
                                       n);
    if (e != 0) {
        tt_crypto_error("rsa oaep decrypt fail");
        return TT_FAIL;
    }
    *olen = (tt_u32_t)n;

    return TT_SUCCESS;
}

tt_result_t tt_rsa_sign_pss(IN tt_rsa_t *rsa,
                            IN tt_u8_t *input,
                            IN tt_u32_t ilen,
                            IN tt_md_type_t md_type,
                            IN tt_u8_t *sig)
{
    mbedtls_rsa_context *ctx = &rsa->ctx;
    mbedtls_md_type_t t;
    const mbedtls_md_info_t *md_info;
    tt_u8_t hash[MBEDTLS_MD_MAX_SIZE];
    tt_u32_t hashlen;
    int e;

    t = tt_g_md_type_map[md_type];
    md_info = mbedtls_md_info_from_type(t);
    mbedtls_md(md_info, input, ilen, hash);
    hashlen = mbedtls_md_get_size(md_info);

    mbedtls_rsa_set_padding(ctx, MBEDTLS_RSA_PKCS_V21, t);

    e = mbedtls_rsa_rsassa_pss_sign(ctx,
                                    tt_ctr_drbg,
                                    tt_current_ctr_drbg(),
                                    MBEDTLS_RSA_PRIVATE,
                                    t,
                                    hashlen,
                                    hash,
                                    sig);
    if (e != 0) {
        tt_crypto_error("rsa pss sign fail");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_rsa_verify_pss(IN tt_rsa_t *rsa,
                              IN tt_u8_t *input,
                              IN tt_u32_t ilen,
                              IN tt_md_type_t md_type,
                              IN tt_u8_t *sig)
{
    mbedtls_rsa_context *ctx = &rsa->ctx;
    mbedtls_md_type_t t;
    const mbedtls_md_info_t *md_info;
    tt_u8_t hash[MBEDTLS_MD_MAX_SIZE];
    tt_u32_t hashlen;
    int e;

    t = tt_g_md_type_map[md_type];
    md_info = mbedtls_md_info_from_type(t);
    mbedtls_md(md_info, input, ilen, hash);
    hashlen = mbedtls_md_get_size(md_info);

    mbedtls_rsa_set_padding(ctx, MBEDTLS_RSA_PKCS_V21, t);

    e = mbedtls_rsa_rsassa_pss_verify(ctx,
                                      tt_ctr_drbg,
                                      tt_current_ctr_drbg(),
                                      MBEDTLS_RSA_PUBLIC,
                                      t,
                                      hashlen,
                                      hash,
                                      sig);
    if (e != 0) {
        tt_crypto_error("rsa pss verify fail");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}
