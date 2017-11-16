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

#include <crypto/tt_public_key.h>

#include <algorithm/tt_buffer.h>
#include <algorithm/tt_buffer_format.h>
#include <crypto/tt_crypto.h>
#include <io/tt_file_system.h>
#include <misc/tt_assert.h>

#include <pem.h>

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

static mbedtls_pk_type_t __pk_type_map[TT_PK_TYPE_NUM] = {
    MBEDTLS_PK_RSA, MBEDTLS_PK_ECKEY,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_pk_init(IN tt_pk_t *pk)
{
    TT_ASSERT(pk != NULL);

    mbedtls_pk_init(&pk->ctx);
}

void tt_pk_destroy(IN tt_pk_t *pk)
{
    TT_ASSERT(pk != NULL);

    mbedtls_pk_free(&pk->ctx);
}

tt_result_t tt_pk_load_public(IN tt_pk_t *pk, IN tt_u8_t *key, IN tt_u32_t len)
{
    int e;

    TT_ASSERT(pk != NULL);
    TT_ASSERT(key != NULL);

    e = mbedtls_pk_parse_public_key(&pk->ctx, key, len);
    if (e != 0) {
        tt_crypto_error("fail to parse public key");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_pk_load_private(IN tt_pk_t *pk,
                               IN tt_u8_t *key,
                               IN tt_u32_t key_len,
                               IN OPT tt_u8_t *pwd,
                               IN tt_u32_t pwd_len)
{
    int e;

    TT_ASSERT(pk != NULL);
    TT_ASSERT(key != NULL);

    e = mbedtls_pk_parse_key(&pk->ctx, key, key_len, pwd, pwd_len);
    if (e != 0) {
        tt_crypto_error("fail to parse private key");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_pk_load_public_file(IN tt_pk_t *pk, IN const tt_char_t *path)
{
    tt_buf_t buf;
    int e;

    TT_ASSERT(pk != NULL);
    TT_ASSERT(path != NULL);

    tt_buf_init(&buf, NULL);
    if (!TT_OK(tt_fcontent_buf(path, &buf)) || !TT_OK(tt_buf_put_u8(&buf, 0))) {
        tt_buf_destroy(&buf);
        return TT_FAIL;
    }

    e = mbedtls_pk_parse_public_key(&pk->ctx,
                                    TT_BUF_RPOS(&buf),
                                    TT_BUF_RLEN(&buf));
    tt_buf_destroy(&buf);
    if (e != 0) {
        tt_crypto_error("fail to parse public key");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_pk_load_private_file(IN tt_pk_t *pk,
                                    IN const tt_char_t *path,
                                    IN OPT const tt_u8_t *pwd,
                                    IN tt_u32_t pwd_len)
{
    tt_buf_t buf;
    int e;

    TT_ASSERT(pk != NULL);
    TT_ASSERT(path != NULL);

    tt_buf_init(&buf, NULL);
    if (!TT_OK(tt_fcontent_buf(path, &buf)) || !TT_OK(tt_buf_put_u8(&buf, 0))) {
        tt_buf_destroy(&buf);
        return TT_FAIL;
    }

    e = mbedtls_pk_parse_key(&pk->ctx,
                             TT_BUF_RPOS(&buf),
                             TT_BUF_RLEN(&buf),
                             pwd,
                             pwd_len);
    tt_buf_destroy(&buf);
    if (e != 0) {
        tt_crypto_error("fail to parse private key");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_pk_check(IN tt_pk_t *pub, IN tt_pk_t *priv)
{
    int e;

    TT_ASSERT(pub != NULL);
    TT_ASSERT(priv != NULL);

    e = mbedtls_pk_check_pair(&pub->ctx, &priv->ctx);
    if (e != 0) {
        tt_crypto_error("pub and priv does not match");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_pk_type_t tt_pk_get_type(IN tt_pk_t *pk)
{
    mbedtls_pk_type_t t;
    tt_pk_type_t i;

    TT_ASSERT(pk != NULL);

    t = mbedtls_pk_get_type(&pk->ctx);
    for (i = 0; i < TT_PK_TYPE_NUM; ++i) {
        if (__pk_type_map[i] == t) {
            break;
        }
    }
    return i;
}
