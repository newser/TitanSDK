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

#include <crypto/tt_ecdh.h>

#include <crypto/tt_crypto.h>
#include <crypto/tt_public_key.h>
#include <misc/tt_assert.h>
#include <misc/tt_util.h>

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

void tt_ecdh_init(IN tt_ecdh_t *ecdh)
{
    TT_ASSERT(ecdh != NULL);

    mbedtls_ecdh_init(&ecdh->ctx);
}

extern void tt_ecdh_destroy(IN tt_ecdh_t *ecdh)
{
    TT_ASSERT(ecdh != NULL);

    mbedtls_ecdh_free(&ecdh->ctx);
}

tt_result_t tt_ecdh_load(IN tt_ecdh_t *ecdh, IN tt_pk_t *pk)
{
    mbedtls_ecdh_context *ctx;
    mbedtls_ecp_keypair *ec;
    int e;

    TT_ASSERT(ecdh != NULL);
    TT_ASSERT(pk != NULL);

    if (tt_pk_get_type(pk) != TT_ECKEY) {
        TT_ERROR("pk is not ec key");
        return TT_FAIL;
    }

    ctx = &ecdh->ctx;
    ec = mbedtls_pk_ec(pk->ctx);

    if (((e = mbedtls_ecp_group_copy(&ctx->grp, &ec->grp)) != 0) ||
        ((e = mbedtls_mpi_copy(&ctx->d, &ec->d)) != 0) ||
        ((e = mbedtls_ecp_copy(&ctx->Q, &ec->Q)) != 0)) {
        tt_crypto_error("fail to copy ec key");
        mbedtls_ecdh_free(ctx);
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_ecdh_generate(IN tt_ecdh_t *ecdh, IN tt_ecgrp_t g)
{
    mbedtls_ecdh_context *ctx;
    int e;

    TT_ASSERT(ecdh != NULL);
    TT_ASSERT(TT_ECGRP_VALID(g));

    ctx = &ecdh->ctx;

    e = mbedtls_ecp_group_load(&ctx->grp, tt_g_ecgrp_map[g]);
    if (e != 0) {
        tt_crypto_error("fail to load ec group[%d]", g);
        mbedtls_ecdh_free(ctx);
        return TT_FAIL;
    }

    e = mbedtls_ecdh_gen_public(&ctx->grp,
                                &ctx->d,
                                &ctx->Q,
                                tt_crypto_rng,
                                NULL);
    if (e != 0) {
        tt_crypto_error("fail to generate ecdh pub");
        mbedtls_ecdh_free(ctx);
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_ecdh_get_pub(IN tt_ecdh_t *ecdh,
                            IN tt_bool_t local,
                            IN tt_bool_t compress,
                            OUT tt_u8_t *pub,
                            IN OUT tt_u32_t *len)
{
    mbedtls_ecdh_context *ctx;
    size_t olen;
    int e;

    TT_ASSERT(ecdh != NULL);
    TT_ASSERT(pub != NULL);
    TT_ASSERT(len != NULL);

    ctx = &ecdh->ctx;

    e = mbedtls_ecp_point_write_binary(&ctx->grp,
                                       TT_COND(local, &ctx->Q, &ctx->Qp),
                                       TT_COND(compress,
                                               MBEDTLS_ECP_PF_COMPRESSED,
                                               MBEDTLS_ECP_PF_UNCOMPRESSED),
                                       &olen,
                                       pub,
                                       len);
    if (e != 0) {
        tt_crypto_error("fail to get ec pub");
        return TT_FAIL;
    }
    *len = (tt_u32_t)olen;

    return TT_SUCCESS;
}

tt_result_t tt_ecdh_set_pub(IN tt_ecdh_t *ecdh,
                            IN tt_bool_t local,
                            IN tt_u8_t *pub,
                            IN tt_u32_t len)
{
    mbedtls_ecdh_context *ctx;
    int e;

    TT_ASSERT(ecdh != NULL);
    TT_ASSERT(pub != NULL);

    ctx = &ecdh->ctx;

    e = mbedtls_ecp_point_read_binary(&ctx->grp,
                                      TT_COND(local, &ctx->Q, &ctx->Qp),
                                      pub,
                                      len);
    if (e != 0) {
        tt_crypto_error("fail to set ec pub");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_ecdh_derive(IN tt_ecdh_t *ecdh)
{
    mbedtls_ecdh_context *ctx;
    int e;

    TT_ASSERT(ecdh != NULL);

    ctx = &ecdh->ctx;

    e = mbedtls_ecdh_compute_shared(&ctx->grp,
                                    &ctx->z,
                                    &ctx->Qp,
                                    &ctx->d,
                                    tt_crypto_rng,
                                    NULL);
    if (e != 0) {
        tt_crypto_error("fail to drive ecdh secret");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_ecdh_get_secret(IN tt_ecdh_t *ecdh,
                               OUT tt_u8_t *secret,
                               IN tt_u32_t len)
{
    mbedtls_ecdh_context *ctx;
    int e;

    TT_ASSERT(ecdh != NULL);
    TT_ASSERT(secret != NULL);

    ctx = &ecdh->ctx;

    e = mbedtls_mpi_write_binary(&ctx->z, secret, len);
    if (e != 0) {
        tt_crypto_error("fail to write ec secret");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}
