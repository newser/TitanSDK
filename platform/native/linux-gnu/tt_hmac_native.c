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

#include <tt_hmac_native.h>

#include <crypto/tt_hmac.h>
#include <memory/tt_memory_alloc.h>
#include <misc/tt_util.h>

#ifdef PLATFORM_CRYPTO_ENABLE

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef \
tt_result_t (*__hmac_update_t)(IN tt_hmac_ntv_t *sys_hmac,
                               IN tt_u8_t *input,
                               IN tt_u32_t input_len);
typedef \
tt_result_t (*__hmac_final_t)(IN tt_hmac_ntv_t *sys_hmac, OUT tt_u8_t *output);
typedef \
tt_result_t (*__hmac_reset_t)(IN tt_hmac_ntv_t *sys_hmac);

typedef struct
{
    __hmac_update_t update;
    __hmac_final_t final;
    __hmac_reset_t reset;
} __hmac_itf_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_result_t __hmac_sha1_update(IN tt_hmac_ntv_t *sys_hmac,
                                      IN tt_u8_t *input,
                                      IN tt_u32_t input_len);
static tt_result_t __hmac_sha1_final(IN tt_hmac_ntv_t *sys_hmac,
                                     OUT tt_u8_t *output);
static tt_result_t __hmac_sha1_reset(IN tt_hmac_ntv_t *sys_hmac);

static __hmac_itf_t tt_s_hmac_itf[TT_HMAC_VER_NUM] = {
    // TT_HMAC_VER_SHA1
    {
        __hmac_sha1_update, __hmac_sha1_final, __hmac_sha1_reset,
    }};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_hmac_create_ntv(IN tt_hmac_ntv_t *sys_hmac,
                               IN tt_hmac_ver_t version,
                               IN tt_blob_t *key)
{
    HMAC_CTX *hmac = NULL;

#if OPENSSL_VERSION_NUMBER < 0x10100000L
    hmac = (HMAC_CTX *)tt_malloc(sizeof(HMAC_CTX));
    if (hmac == NULL) {
        TT_ERROR("no mem for hmac ctx");
        return TT_FAIL;
    }

    HMAC_CTX_init(hmac);
#else
    // not tested
    TT_ASSERT(0);

    hmac = HMAC_CTX_new();
    if (hmac == NULL) {
        TT_ERROR("no mem for hmac ctx");
        return TT_FAIL;
    }

    // need?
    if (HMAC_CTX_reset() != 1) {
        TT_ERROR("fail to reset hmac ctx");
        goto h_fail;
    }
#endif

    switch (version) {
        case TT_HMAC_VER_SHA1: {
#if OPENSSL_VERSION_NUMBER < 0x10000000L
            HMAC_Init_ex(hmac, key->addr, key->len, EVP_sha1(), NULL);
#else
            if (HMAC_Init_ex(hmac, key->addr, key->len, EVP_sha1(), NULL) !=
                1) {
                TT_ERROR("fail to init hmac ctx");
                goto h_fail;
            }
#endif
        } break;

        default: {
            goto h_fail;
        } break;
    }

    sys_hmac->hmac = hmac;
    return TT_SUCCESS;

h_fail:

    if (hmac != NULL) {
#if OPENSSL_VERSION_NUMBER < 0x10100000L
        HMAC_CTX_cleanup(hmac);
        tt_free(hmac);
#else
        HMAC_CTX_free(hmac);
#endif
    }

    return TT_FAIL;
}

void tt_hmac_destroy_ntv(IN tt_hmac_ntv_t *sys_hmac)
{
#if OPENSSL_VERSION_NUMBER < 0x10100000L
    HMAC_CTX_cleanup(sys_hmac->hmac);
    tt_free(sys_hmac->hmac);
#else
    HMAC_CTX_free(sys_hmac->hmac);
#endif
}

tt_result_t tt_hmac_update_ntv(IN tt_hmac_ntv_t *sys_hmac,
                               IN tt_u8_t *input,
                               IN tt_u32_t input_len)
{
    tt_hmac_t *hmac = TT_CONTAINER(sys_hmac, tt_hmac_t, sys_hmac);
    return tt_s_hmac_itf[hmac->version].update(sys_hmac, input, input_len);
}

tt_result_t tt_hmac_final_ntv(IN tt_hmac_ntv_t *sys_hmac, OUT tt_u8_t *output)
{
    tt_hmac_t *hmac = TT_CONTAINER(sys_hmac, tt_hmac_t, sys_hmac);
    return tt_s_hmac_itf[hmac->version].final(sys_hmac, output);
}

tt_result_t tt_hmac_reset_ntv(IN tt_hmac_ntv_t *sys_hmac)
{
    tt_hmac_t *hmac = TT_CONTAINER(sys_hmac, tt_hmac_t, sys_hmac);
    return tt_s_hmac_itf[hmac->version].reset(sys_hmac);
}

// TT_HMAC_VER_SHA1
tt_result_t __hmac_sha1_update(IN tt_hmac_ntv_t *sys_hmac,
                               IN tt_u8_t *input,
                               IN tt_u32_t input_len)
{
#if OPENSSL_VERSION_NUMBER < 0x10000000L
    HMAC_Update(sys_hmac->hmac, input, input_len);
#else
    if (HMAC_Update(sys_hmac->hmac, input, input_len) != 1) {
        TT_ERROR("fail: hmac update");
        return TT_FAIL;
    }
#endif

    return TT_SUCCESS;
}

tt_result_t __hmac_sha1_final(IN tt_hmac_ntv_t *sys_hmac, OUT tt_u8_t *output)
{
    unsigned int len = TT_HMAC_SHA1_DIGEST_LENGTH;

#if OPENSSL_VERSION_NUMBER < 0x10000000L
    HMAC_Final(sys_hmac->hmac, output, &len);
#else
    if (HMAC_Final(sys_hmac->hmac, output, &len) != 1) {
        TT_ERROR("fail: hmac final");
        return TT_FAIL;
    }
#endif

    return TT_SUCCESS;
}

tt_result_t __hmac_sha1_reset(IN tt_hmac_ntv_t *sys_hmac)
{
#if OPENSSL_VERSION_NUMBER < 0x10000000L
    HMAC_Init_ex(sys_hmac->hmac, NULL, 0, NULL, NULL);
#else
    if (HMAC_Init_ex(sys_hmac->hmac, NULL, 0, NULL, NULL) != 1) {
        TT_ERROR("fail: hmac init");
        return TT_FAIL;
    }
#endif

    return TT_SUCCESS;
}

#endif
