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
#include <init/tt_profile.h>
#include <memory/tt_memory_alloc.h>
#include <misc/tt_assert.h>
#include <misc/tt_util.h>

#include <tt_crypto_native.h>

#ifdef TSCM_PLATFORM_CRYPTO_ENABLE

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef tt_result_t (*__hmac_update_t)(IN tt_hmac_ntv_t *sys_hmac,
                                       IN tt_u8_t *input,
                                       IN tt_u32_t input_len);
typedef tt_result_t (*__hmac_final_t)(IN tt_hmac_ntv_t *sys_hmac,
                                      OUT tt_u8_t *output);
typedef tt_result_t (*__hmac_reset_t)(IN tt_hmac_ntv_t *sys_hmac);

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

// TT_HMAC_VER_SHA1
static BCRYPT_ALG_HANDLE tt_s_crypt_prov_hmac_sha1;
static ULONG tt_s_crypt_hmac_sha1_size;

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

tt_result_t tt_hmac_component_init_ntv(IN tt_profile_t *profile)
{
    NTSTATUS ntst;
    ULONG val, n;

    // ========================================
    // SHA1
    // ========================================

    // provider
    ntst = BCryptOpenAlgorithmProvider(&tt_s_crypt_prov_hmac_sha1,
                                       BCRYPT_SHA1_ALGORITHM,
                                       MS_PRIMITIVE_PROVIDER,
                                       BCRYPT_ALG_HANDLE_HMAC_FLAG);
    if ((ntst != STATUS_SUCCESS) || (tt_s_crypt_prov_hmac_sha1 == NULL)) {
        TT_ERROR("fail to create hmac sha1 provider");
        return TT_FAIL;
    }

    // hmac_sha1 object size
    ntst = BCryptGetProperty(tt_s_crypt_prov_hmac_sha1,
                             BCRYPT_OBJECT_LENGTH,
                             (PUCHAR)&tt_s_crypt_hmac_sha1_size,
                             sizeof(tt_s_crypt_hmac_sha1_size),
                             &n,
                             0);
    if ((ntst != STATUS_SUCCESS) || (tt_s_crypt_hmac_sha1_size == 0)) {
        TT_ERROR("fail to get hmac_sha1 obj size");
        return TT_FAIL;
    }

    // check hmac_sha1 hash size, should be 16
    ntst = BCryptGetProperty(tt_s_crypt_prov_hmac_sha1,
                             BCRYPT_HASH_LENGTH,
                             (PUCHAR)&val,
                             sizeof(val),
                             &n,
                             0);
    if ((ntst != STATUS_SUCCESS) || (val != TT_HMAC_SHA1_DIGEST_LENGTH)) {
        TT_ERROR("fail to get hmac_sha1 hash size");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_hmac_create_ntv(IN tt_hmac_ntv_t *sys_hmac,
                               IN tt_hmac_ver_t version,
                               IN tt_blob_t *key)
{
    tt_u8_t *p;
    tt_u32_t size;

    NTSTATUS ntst;
    BCRYPT_ALG_HANDLE hmac_prov;
    BCRYPT_HASH_HANDLE h_hmac;

    switch (version) {
        case TT_HMAC_VER_SHA1: {
            size = tt_s_crypt_hmac_sha1_size;
            hmac_prov = tt_s_crypt_prov_hmac_sha1;
        } break;

        default: {
            return TT_FAIL;
        } break;
    }
    TT_ASSERT(size != 0);

    p = tt_malloc(size);
    if (p == NULL) {
        TT_ERROR("no mem for hmac obj");
        return TT_FAIL;
    }

    ntst =
        BCryptCreateHash(hmac_prov, &h_hmac, p, size, key->addr, key->len, 0);
    if (ntst != STATUS_SUCCESS) {
        TT_ERROR("fail to create hmac obj");

        tt_free(p);
        return TT_FAIL;
    }

    if (!TT_OK(tt_blob_create(&sys_hmac->key, key->addr, key->len))) {
        TT_ERROR("fail to save hmac key");

        BCryptDestroyHash(h_hmac);
        tt_free(p);
        return TT_FAIL;
    }

    sys_hmac->h_hmac = h_hmac;
    sys_hmac->mem = p;
    sys_hmac->size = size;
    return TT_SUCCESS;
}

void tt_hmac_destroy_ntv(IN tt_hmac_ntv_t *sys_hmac)
{
    BCryptDestroyHash(sys_hmac->h_hmac);
    tt_free(sys_hmac->mem);

    tt_blob_destroy(&sys_hmac->key);
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
    NTSTATUS ntst;

    ntst = BCryptHashData(sys_hmac->h_hmac, input, input_len, 0);
    if (ntst != STATUS_SUCCESS) {
        TT_ERROR("fail to hash hmac_sha1 data");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t __hmac_sha1_final(IN tt_hmac_ntv_t *sys_hmac, OUT tt_u8_t *output)
{
    NTSTATUS ntst;

    ntst = BCryptFinishHash(sys_hmac->h_hmac,
                            output,
                            TT_HMAC_SHA1_DIGEST_LENGTH,
                            0);
    if (ntst != STATUS_SUCCESS) {
        TT_ERROR("fail to calc hmac_sha1 data hash val");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t __hmac_sha1_reset(IN tt_hmac_ntv_t *sys_hmac)
{
    tt_hmac_t *hmac = TT_CONTAINER(sys_hmac, tt_hmac_t, sys_hmac);

    NTSTATUS ntst;
    BCRYPT_ALG_HANDLE hmac_prov;
    BCRYPT_HASH_HANDLE h_hmac;

    BCryptDestroyHash(sys_hmac->h_hmac);
    sys_hmac->h_hmac = NULL;

    switch (hmac->version) {
        case TT_HMAC_VER_SHA1: {
            hmac_prov = tt_s_crypt_prov_hmac_sha1;
        } break;

        default: {
            return TT_FAIL;
        } break;
    }

    ntst = BCryptCreateHash(hmac_prov,
                            &h_hmac,
                            sys_hmac->mem,
                            sys_hmac->size,
                            sys_hmac->key.addr,
                            sys_hmac->key.len,
                            0);
    if (ntst != STATUS_SUCCESS) {
        TT_ERROR("fail to create hmac obj");
        return TT_FAIL;
    }

    sys_hmac->h_hmac = h_hmac;
    return TT_SUCCESS;
};

#endif
