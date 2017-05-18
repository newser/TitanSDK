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

#include <crypto/tt_public_key.h>

#include <algorithm/tt_buffer.h>
#include <algorithm/tt_buffer_format.h>
#include <io/tt_file_system.h>
#include <memory/tt_memory_alloc.h>
#include <os/tt_thread.h>

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
    MBEDTLS_PK_RSA, MBEDTLS_PK_ECKEY_DH, MBEDTLS_PK_ECDSA,
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

tt_result_t tt_pk_setup_public(IN tt_pk_t *pk, IN tt_u8_t *key, IN tt_u32_t len)
{
    int e;

    TT_ASSERT(pk != NULL);
    TT_ASSERT(key != NULL);

    e = mbedtls_pk_parse_public_key(&pk->ctx, key, len);
    if (e != 0) {
        TT_ERROR("fail to parse public key: %s", tt_pk_strerror(e));
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_pk_setup_private(IN tt_pk_t *pk,
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
        TT_ERROR("fail to parse private key: %s", tt_pk_strerror(e));
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_pk_setup_public_file(IN tt_pk_t *pk, IN const tt_char_t *path)
{
    tt_buf_t buf;
    int e;

    TT_ASSERT(pk != NULL);
    TT_ASSERT(path != NULL);

    tt_buf_init(&buf, NULL);
    if (!TT_OK(tt_fcontent_buf(path, &buf)) || !TT_OK(tt_buf_put_u8(&buf, 0))) {
        return TT_FAIL;
    }

    e = mbedtls_pk_parse_public_key(&pk->ctx,
                                    TT_BUF_RPOS(&buf),
                                    TT_BUF_RLEN(&buf));
    tt_buf_destroy(&buf);
    if (e != 0) {
        TT_ERROR("fail to parse public key: %s", tt_pk_strerror(e));
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_pk_setup_private_file(IN tt_pk_t *pk,
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
        return TT_FAIL;
    }

    e = mbedtls_pk_parse_key(&pk->ctx,
                             TT_BUF_RPOS(&buf),
                             TT_BUF_RLEN(&buf),
                             pwd,
                             pwd_len);
    tt_buf_destroy(&buf);
    if (e != 0) {
        TT_ERROR("fail to parse private key: %s", tt_pk_strerror(e));
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
        TT_ERROR("pub and priv does not match: %s", tt_pk_strerror(e));
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

int tt_pk_rng(IN void *param, unsigned char *buf, size_t len)
{
    tt_u32_t n;

    n = 0;
    while ((n + sizeof(tt_u64_t)) <= len) {
        *TT_PTR_INC(tt_u64_t, buf, n) = tt_rand_u64();
        n += sizeof(tt_u64_t);
    }
    while (n < len) {
        buf[n] = (tt_u8_t)tt_rand_u32();
        n += sizeof(tt_u8_t);
    }

    return 0;
}

const tt_char_t *tt_pk_strerror(IN int err)
{
    switch (err) {
        case MBEDTLS_ERR_PK_ALLOC_FAILED:
            return "Memory allocation failed.";
        case MBEDTLS_ERR_PK_TYPE_MISMATCH:
            return "Type mismatch, eg attempt to encrypt with an ECDSA key";
        case MBEDTLS_ERR_PK_BAD_INPUT_DATA:
            return "Bad input parameters to function.";
        case MBEDTLS_ERR_PK_FILE_IO_ERROR:
            return "Read/write of file failed.";
        case MBEDTLS_ERR_PK_KEY_INVALID_VERSION:
            return "Unsupported key version";
        case MBEDTLS_ERR_PK_KEY_INVALID_FORMAT:
            return "Invalid key tag or value.";
        case MBEDTLS_ERR_PK_UNKNOWN_PK_ALG:
            return "Key algorithm is unsupported (only RSA and EC are "
                   "supported).";
        case MBEDTLS_ERR_PK_PASSWORD_REQUIRED:
            return "Private key password can't be empty.";
        case MBEDTLS_ERR_PK_PASSWORD_MISMATCH:
            return "Given private key password does not allow for correct "
                   "decryption.";
        case MBEDTLS_ERR_PK_INVALID_PUBKEY:
            return "The pubkey tag or value is invalid (only RSA and EC are "
                   "supported).";
        case MBEDTLS_ERR_PK_INVALID_ALG:
            return "The algorithm tag or value is invalid.";
        case MBEDTLS_ERR_PK_UNKNOWN_NAMED_CURVE:
            return "Elliptic curve is unsupported (only NIST curves are "
                   "supported).";
        case MBEDTLS_ERR_PK_FEATURE_UNAVAILABLE:
            return "Unavailable feature, e.g. RSA disabled for RSA key.";
        case MBEDTLS_ERR_PK_SIG_LEN_MISMATCH:
            return "The signature is valid but its length is less than "
                   "expected.";

        case MBEDTLS_ERR_PEM_NO_HEADER_FOOTER_PRESENT:
            return "No PEM header or footer found";
        case MBEDTLS_ERR_PEM_INVALID_DATA:
            return "PEM string is not as expected";
        case MBEDTLS_ERR_PEM_ALLOC_FAILED:
            return "Failed to allocate memory";
        case MBEDTLS_ERR_PEM_INVALID_ENC_IV:
            return "RSA IV is not in hex-format";
        case MBEDTLS_ERR_PEM_UNKNOWN_ENC_ALG:
            return "Unsupported key encryption algorithm";
        case MBEDTLS_ERR_PEM_PASSWORD_REQUIRED:
            return "Private key password can't be empty";
        case MBEDTLS_ERR_PEM_PASSWORD_MISMATCH:
            return "Given private key password does not allow for correct "
                   "decryption";
        case MBEDTLS_ERR_PEM_FEATURE_UNAVAILABLE:
            return "Unavailable feature, e.g. hashing/encryption combination";
        case MBEDTLS_ERR_PEM_BAD_INPUT_DATA:
            return "Bad input parameters to function";

        default:
            return "Unknown error";
    }
}
