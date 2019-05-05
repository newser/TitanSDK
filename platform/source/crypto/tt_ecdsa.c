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

#include <crypto/tt_ecdsa.h>

#include <crypto/tt_crypto.h>
#include <crypto/tt_ctr_drbg.h>
#include <crypto/tt_public_key.h>
#include <misc/tt_assert.h>

#include <bignum.h>

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

void tt_ecdsa_init(IN tt_ecdsa_t *dsa)
{
    TT_ASSERT(dsa != NULL);

    mbedtls_ecdsa_init(&dsa->ctx);
}

extern void tt_ecdsa_destroy(IN tt_ecdsa_t *dsa)
{
    TT_ASSERT(dsa != NULL);

    mbedtls_ecdsa_free(&dsa->ctx);
}

tt_result_t tt_ecdsa_load(IN tt_ecdsa_t *dsa, IN tt_pk_t *pk)
{
    mbedtls_ecdsa_context *ctx;
    mbedtls_ecp_keypair *ec;
    int e;

    TT_ASSERT(dsa != NULL);
    TT_ASSERT(pk != NULL);

    if (tt_pk_get_type(pk) != TT_ECKEY) {
        TT_ERROR("pk is not ec key");
        return TT_FAIL;
    }

    ctx = &dsa->ctx;
    ec = mbedtls_pk_ec(pk->ctx);

    e = mbedtls_ecdsa_from_keypair(ctx, ec);
    if (e != 0) {
        tt_crypto_error("fail to copy ec key");
        mbedtls_ecdsa_free(ctx);
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_ecdsa_generate(IN tt_ecdsa_t *dsa, IN tt_ecgrp_t g)
{
    mbedtls_ecdsa_context *ctx;
    int e;

    TT_ASSERT(dsa != NULL);
    TT_ASSERT(TT_ECGRP_VALID(g));

    ctx = &dsa->ctx;

    e = mbedtls_ecdsa_genkey(ctx, tt_g_ecgrp_map[g], tt_ctr_drbg,
                             tt_current_ctr_drbg());
    if (e != 0) {
        tt_crypto_error("fail to generate ecdsa");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_ecdsa_sign(IN tt_ecdsa_t *dsa, IN tt_u8_t *input,
                          IN tt_u32_t len, IN tt_md_type_t md_type,
                          IN tt_md_type_t sig_md, OUT tt_u8_t *sig,
                          IN OUT tt_u32_t *sig_len)
{
    mbedtls_ecdsa_context *ctx = &dsa->ctx;
    mbedtls_md_type_t t;
    const mbedtls_md_info_t *md_type_info;
    tt_u8_t hash[MBEDTLS_MD_MAX_SIZE];
    tt_u32_t hashlen;
    size_t slen = *sig_len;
    int e;

    t = tt_g_md_type_map[md_type];
    md_type_info = mbedtls_md_info_from_type(t);
    mbedtls_md(md_type_info, input, len, hash);
    hashlen = mbedtls_md_get_size(md_type_info);

    e = mbedtls_ecdsa_write_signature(ctx, tt_g_md_type_map[sig_md], hash,
                                      hashlen, sig, &slen, tt_ctr_drbg,
                                      tt_current_ctr_drbg());
    if (e != 0) {
        tt_crypto_error("ecdsa sign failed");
        return TT_FAIL;
    }
    *sig_len = (tt_u32_t)slen;

    return TT_SUCCESS;
}

tt_result_t tt_ecdsa_verify(IN tt_ecdsa_t *dsa, IN tt_u8_t *input,
                            IN tt_u32_t len, IN tt_md_type_t md_type,
                            IN tt_u8_t *sig, IN tt_u32_t sig_len)
{
    mbedtls_ecdsa_context *ctx = &dsa->ctx;
    mbedtls_md_type_t t;
    const mbedtls_md_info_t *md_type_info;
    tt_u8_t hash[MBEDTLS_MD_MAX_SIZE];
    tt_u32_t hashlen;
    int e;

    t = tt_g_md_type_map[md_type];
    md_type_info = mbedtls_md_info_from_type(t);
    mbedtls_md(md_type_info, input, len, hash);
    hashlen = mbedtls_md_get_size(md_type_info);

    e = mbedtls_ecdsa_read_signature(ctx, hash, hashlen, sig, sig_len);
    if (e != 0) {
        tt_crypto_error("ecdsa verify failed");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}
