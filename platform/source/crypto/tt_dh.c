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

#include <crypto/tt_dh.h>

#include <algorithm/tt_buffer_format.h>
#include <crypto/tt_crypto.h>
#include <crypto/tt_ctr_drbg.h>
#include <crypto/tt_public_key.h>
#include <io/tt_file_system.h>
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

void tt_dh_init(IN tt_dh_t *dh)
{
    TT_ASSERT(dh != NULL);

    mbedtls_dhm_init(&dh->ctx);
}

void tt_dh_destroy(IN tt_dh_t *dh)
{
    TT_ASSERT(dh != NULL);

    mbedtls_dhm_free(&dh->ctx);
}

tt_result_t tt_dh_load_param(IN tt_dh_t *dh, IN tt_u8_t *param, IN tt_u32_t len)
{
    int e;

    TT_ASSERT(dh != NULL);
    TT_ASSERT(param != NULL);

    e = mbedtls_dhm_parse_dhm(&dh->ctx, param, len);
    if (e != 0) {
        tt_crypto_error("fail to parse dh");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_dh_load_param_file(IN tt_dh_t *dh, IN const tt_char_t *path)
{
    tt_buf_t buf;
    int e;

    TT_ASSERT(dh != NULL);
    TT_ASSERT(path != NULL);

    tt_buf_init(&buf, NULL);
    if (!TT_OK(tt_fcontent_buf(path, &buf)) || !TT_OK(tt_buf_put_u8(&buf, 0))) {
        tt_buf_destroy(&buf);
        return TT_FAIL;
    }

    e = mbedtls_dhm_parse_dhm(&dh->ctx, TT_BUF_RPOS(&buf), TT_BUF_RLEN(&buf));
    tt_buf_destroy(&buf);
    if (e != 0) {
        tt_crypto_error("fail to parse dh param");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_dh_generate_pub(IN tt_dh_t *dh,
                               IN tt_u32_t priv_size,
                               OUT tt_u8_t *pub,
                               IN OUT tt_u32_t len)
{
    int e;

    TT_ASSERT(dh != NULL);
    TT_ASSERT(pub != NULL);

    e = mbedtls_dhm_make_public(&dh->ctx,
                                priv_size,
                                pub,
                                len,
                                tt_ctr_drbg,
                                tt_current_ctr_drbg());
    if (e != 0) {
        tt_crypto_error("fail to generate dh pub");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_dh_get_pub(IN tt_dh_t *dh,
                          IN tt_bool_t local,
                          OUT tt_u8_t *pub,
                          IN tt_u32_t len)
{
    int e;

    TT_ASSERT(dh != NULL);
    TT_ASSERT(pub != NULL);

    e = mbedtls_mpi_write_binary(TT_COND(local, &dh->ctx.GX, &dh->ctx.GY),
                                 pub,
                                 len);
    if (e != 0) {
        tt_crypto_error("fail to get dh pub");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_dh_set_pub(IN tt_dh_t *dh, IN tt_u8_t *pub, IN tt_u32_t len)
{
    int e;

    TT_ASSERT(dh != NULL);
    TT_ASSERT(pub != NULL);

    e = mbedtls_dhm_read_public(&dh->ctx, pub, len);
    if (e != 0) {
        tt_crypto_error("fail to set dh pub");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_dh_derive(IN tt_dh_t *dh,
                         OUT tt_u8_t *secret,
                         IN OUT tt_u32_t *len)
{
    size_t olen;
    int e;

    TT_ASSERT(dh != NULL);
    TT_ASSERT(secret != NULL);
    TT_ASSERT(len != NULL);

    e = mbedtls_dhm_calc_secret(&dh->ctx,
                                secret,
                                (size_t)len,
                                &olen,
                                tt_ctr_drbg,
                                tt_current_ctr_drbg());
    if (e != 0) {
        tt_crypto_error("fail to drive dh secret");
        return TT_FAIL;
    }
    *len = (tt_u32_t)olen;

    return TT_SUCCESS;
}

tt_result_t tt_dh_get_secret(IN tt_dh_t *dh,
                             OUT tt_u8_t *secret,
                             IN tt_u32_t len)
{
    int e;

    TT_ASSERT(dh != NULL);
    TT_ASSERT(secret != NULL);

    e = mbedtls_mpi_write_binary(&dh->ctx.K, secret, len);
    if (e != 0) {
        tt_crypto_error("fail to get dh secret");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}
