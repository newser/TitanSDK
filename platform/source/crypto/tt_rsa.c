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

#include <crypto/tt_rsa.h>

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

tt_result_t tt_rsa_generate(OUT tt_pk_t *pub,
                            OUT tt_pk_t *priv,
                            IN tt_u32_t bit_num,
                            IN tt_u32_t exponent)
{
    int ret;
    mbedtls_rsa_context *pub_ctx, *priv_ctx;

    tt_u32_t __done = 0;
#define __RG_PUB (1 << 0)
#define __RG_PRIV (1 << 1)

    TT_ASSERT(pub != NULL);
    TT_ASSERT(priv != NULL);

    ret =
        mbedtls_pk_setup(&pub->ctx, mbedtls_pk_info_from_type(MBEDTLS_PK_RSA));
    if (ret != 0) {
        TT_ERROR("fail to setup pub rsa: %s", tt_pk_strerror(ret));
        goto cleanup;
    }
    pub_ctx = mbedtls_pk_rsa(pub->ctx);
    mbedtls_rsa_init(pub_ctx, 0, 0);
    __done |= __RG_PUB;

    ret =
        mbedtls_pk_setup(&priv->ctx, mbedtls_pk_info_from_type(MBEDTLS_PK_RSA));
    if (ret != 0) {
        TT_ERROR("fail to setup priv rsa: %s", tt_pk_strerror(ret));
        goto cleanup;
    }
    priv_ctx = mbedtls_pk_rsa(priv->ctx);
    mbedtls_rsa_init(priv_ctx, 0, 0);
    __done |= __RG_PRIV;

    ret = mbedtls_rsa_gen_key(priv_ctx, tt_pk_rng, NULL, bit_num, exponent);
    if (ret != 0) {
        TT_ERROR("fail to generate priv rsa: %s", tt_pk_strerror(ret));
        goto cleanup;
    }

    pub_ctx->ver = priv_ctx->ver;
    pub_ctx->len = priv_ctx->len;
    MBEDTLS_MPI_CHK(mbedtls_mpi_copy(&pub_ctx->N, &priv_ctx->N));
    MBEDTLS_MPI_CHK(mbedtls_mpi_copy(&pub_ctx->E, &priv_ctx->E));
    pub_ctx->padding = priv_ctx->padding;
    pub_ctx->hash_id = priv_ctx->hash_id;

    return TT_SUCCESS;

cleanup:

    if (__done & __RG_PUB) {
        tt_pk_destroy(pub);
        tt_pk_init(pub);
    }

    if (__done & __RG_PRIV) {
        tt_pk_destroy(priv);
        tt_pk_init(priv);
    }

    return TT_FAIL;
}

tt_result_t tt_rsa_encrypt_pkcs1(IN tt_pk_t *pk,
                                 IN tt_u8_t *input,
                                 IN tt_u32_t ilen,
                                 IN tt_u8_t *output)
{
    mbedtls_rsa_context *ctx = mbedtls_pk_rsa(pk->ctx);
    int e;

    mbedtls_rsa_set_padding(ctx, MBEDTLS_RSA_PKCS_V15, 0);

    e = mbedtls_rsa_rsaes_pkcs1_v15_encrypt(ctx,
                                            tt_pk_rng,
                                            NULL,
                                            MBEDTLS_RSA_PUBLIC,
                                            ilen,
                                            input,
                                            output);
    if (e != 0) {
        TT_ERROR("rsa pkcs1 encrypt fail: %s", tt_pk_strerror(e));
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_rsa_decrypt_pkcs1(IN tt_pk_t *pk,
                                 IN tt_u8_t *input,
                                 IN tt_u8_t *output,
                                 IN tt_u32_t *olen)
{
    mbedtls_rsa_context *ctx = mbedtls_pk_rsa(pk->ctx);
    size_t n = *olen;
    int e;

    mbedtls_rsa_set_padding(ctx, MBEDTLS_RSA_PKCS_V15, 0);

    e = mbedtls_rsa_rsaes_pkcs1_v15_decrypt(ctx,
                                            tt_pk_rng,
                                            NULL,
                                            MBEDTLS_RSA_PRIVATE,
                                            &n,
                                            input,
                                            output,
                                            n);
    if (e != 0) {
        TT_ERROR("rsa pkcs1 decrypt fail: %s", tt_pk_strerror(e));
        return TT_FAIL;
    }
    *olen = (tt_u32_t)n;

    return TT_SUCCESS;
}

tt_result_t tt_rsa_sign_pkcs1(IN tt_pk_t *pk,
                              IN tt_u8_t *input,
                              IN tt_u32_t ilen,
                              IN tt_md_type_t md_type,
                              IN tt_u8_t *sig)
{
    mbedtls_rsa_context *ctx = mbedtls_pk_rsa(pk->ctx);
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
                                          tt_pk_rng,
                                          NULL,
                                          MBEDTLS_RSA_PRIVATE,
                                          t,
                                          hashlen,
                                          hash,
                                          sig);
    if (e != 0) {
        TT_ERROR("rsa pkcs1 sign fail: %s", tt_pk_strerror(e));
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_rsa_verify_pkcs1(IN tt_pk_t *pk,
                                IN tt_u8_t *input,
                                IN tt_u32_t ilen,
                                IN tt_md_type_t md_type,
                                IN tt_u8_t *sig)
{
    mbedtls_rsa_context *ctx = mbedtls_pk_rsa(pk->ctx);
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
                                            tt_pk_rng,
                                            NULL,
                                            MBEDTLS_RSA_PUBLIC,
                                            t,
                                            hashlen,
                                            hash,
                                            sig);
    if (e != 0) {
        TT_ERROR("rsa pkcs1 verify fail: %s", tt_pk_strerror(e));
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_rsa_encrypt_oaep(IN tt_pk_t *pk,
                                IN tt_u8_t *input,
                                IN tt_u32_t ilen,
                                IN const tt_u8_t *label,
                                IN tt_u32_t label_len,
                                IN tt_md_type_t md_type,
                                IN tt_u8_t *output)
{
    mbedtls_rsa_context *ctx = mbedtls_pk_rsa(pk->ctx);
    int e;

    mbedtls_rsa_set_padding(ctx,
                            MBEDTLS_RSA_PKCS_V21,
                            tt_g_md_type_map[md_type]);

    e = mbedtls_rsa_rsaes_oaep_encrypt(ctx,
                                       tt_pk_rng,
                                       NULL,
                                       MBEDTLS_RSA_PUBLIC,
                                       label,
                                       label_len,
                                       ilen,
                                       input,
                                       output);
    if (e != 0) {
        TT_ERROR("rsa oaep encrypt fail: %s", tt_pk_strerror(e));
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_rsa_decrypt_oaep(IN tt_pk_t *pk,
                                IN tt_u8_t *input,
                                IN const tt_u8_t *label,
                                IN tt_u32_t label_len,
                                IN tt_md_type_t md_type,
                                IN tt_u8_t *output,
                                IN tt_u32_t *olen)
{
    mbedtls_rsa_context *ctx = mbedtls_pk_rsa(pk->ctx);
    size_t n = *olen;
    int e;

    mbedtls_rsa_set_padding(ctx,
                            MBEDTLS_RSA_PKCS_V21,
                            tt_g_md_type_map[md_type]);

    e = mbedtls_rsa_rsaes_oaep_decrypt(ctx,
                                       tt_pk_rng,
                                       NULL,
                                       MBEDTLS_RSA_PRIVATE,
                                       label,
                                       label_len,
                                       &n,
                                       input,
                                       output,
                                       n);
    if (e != 0) {
        TT_ERROR("rsa oaep decrypt fail: %s", tt_pk_strerror(e));
        return TT_FAIL;
    }
    *olen = (tt_u32_t)n;

    return TT_SUCCESS;
}

tt_result_t tt_rsa_sign_pss(IN tt_pk_t *pk,
                            IN tt_u8_t *input,
                            IN tt_u32_t ilen,
                            IN tt_md_type_t md_type,
                            IN tt_u8_t *sig)
{
    mbedtls_rsa_context *ctx = mbedtls_pk_rsa(pk->ctx);
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
                                    tt_pk_rng,
                                    NULL,
                                    MBEDTLS_RSA_PRIVATE,
                                    t,
                                    hashlen,
                                    hash,
                                    sig);
    if (e != 0) {
        TT_ERROR("rsa pss sign fail: %s", tt_pk_strerror(e));
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_rsa_verify_pss(IN tt_pk_t *pk,
                              IN tt_u8_t *input,
                              IN tt_u32_t ilen,
                              IN tt_md_type_t md_type,
                              IN tt_u8_t *sig)
{
    mbedtls_rsa_context *ctx = mbedtls_pk_rsa(pk->ctx);
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
                                      tt_pk_rng,
                                      NULL,
                                      MBEDTLS_RSA_PUBLIC,
                                      t,
                                      hashlen,
                                      hash,
                                      sig);
    if (e != 0) {
        TT_ERROR("rsa pss verify fail: %s", tt_pk_strerror(e));
        return TT_FAIL;
    }

    return TT_SUCCESS;
}
