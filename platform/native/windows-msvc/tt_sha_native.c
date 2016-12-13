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

#include <tt_sha_native.h>

#include <crypto/tt_sha.h>
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

typedef tt_result_t (*__sha_update_t)(IN tt_sha_ntv_t *sys_sha,
                                      IN tt_u8_t *input,
                                      IN tt_u32_t input_len);
typedef tt_result_t (*__sha_final_t)(IN tt_sha_ntv_t *sys_sha,
                                     OUT tt_u8_t *output);

typedef struct
{
    __sha_update_t update;
    __sha_final_t final;
} __sha_itf_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

// TT_SHA_VER_SHA1
static BCRYPT_ALG_HANDLE tt_s_crypt_prov_sha1;
static ULONG tt_s_crypt_sha1_size;

static tt_result_t __sha1_update(IN tt_sha_ntv_t *sys_sha,
                                 IN tt_u8_t *input,
                                 IN tt_u32_t input_len);
static tt_result_t __sha1_final(IN tt_sha_ntv_t *sys_sha, OUT tt_u8_t *output);

static __sha_itf_t tt_s_sha_itf[TT_SHA_VER_NUM] = {
    // TT_SHA_VER_SHA1
    {
        __sha1_update, __sha1_final,
    }};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_sha_component_init_ntv(IN tt_profile_t *profile)
{
    NTSTATUS ntst;
    ULONG val, n;

    // ========================================
    // SHA1
    // ========================================

    // provider
    ntst = BCryptOpenAlgorithmProvider(&tt_s_crypt_prov_sha1,
                                       BCRYPT_SHA1_ALGORITHM,
                                       MS_PRIMITIVE_PROVIDER,
                                       0);
    if ((ntst != STATUS_SUCCESS) || (tt_s_crypt_prov_sha1 == NULL)) {
        TT_ERROR("fail to create sha1 provider");
        return TT_FAIL;
    }

    // sha1 object size
    ntst = BCryptGetProperty(tt_s_crypt_prov_sha1,
                             BCRYPT_OBJECT_LENGTH,
                             (PUCHAR)&tt_s_crypt_sha1_size,
                             sizeof(tt_s_crypt_sha1_size),
                             &n,
                             0);
    if ((ntst != STATUS_SUCCESS) || (tt_s_crypt_sha1_size == 0)) {
        TT_ERROR("fail to get sha1 obj size");
        return TT_FAIL;
    }

    // check sha1 hash size, should be 16
    ntst = BCryptGetProperty(tt_s_crypt_prov_sha1,
                             BCRYPT_HASH_LENGTH,
                             (PUCHAR)&val,
                             sizeof(val),
                             &n,
                             0);
    if ((ntst != STATUS_SUCCESS) || (val != TT_SHA1_DIGEST_LENGTH)) {
        TT_ERROR("fail to get sha1 hash size");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_sha_create_ntv(IN tt_sha_ntv_t *sys_sha, IN tt_sha_ver_t version)
{
    tt_u8_t *p;
    tt_u32_t size;

    NTSTATUS ntst;
    BCRYPT_ALG_HANDLE sha_prov;
    BCRYPT_HASH_HANDLE h_sha;

    switch (version) {
        case TT_SHA_VER_SHA1: {
            size = tt_s_crypt_sha1_size;
            sha_prov = tt_s_crypt_prov_sha1;
        } break;

        default: {
            return TT_FAIL;
        } break;
    }
    TT_ASSERT(size != 0);

    p = tt_mem_alloc(size);
    if (p == NULL) {
        TT_ERROR("no mem for sha obj");
        return TT_FAIL;
    }

    ntst = BCryptCreateHash(sha_prov, &h_sha, p, size, NULL, 0, 0);
    if (ntst != STATUS_SUCCESS) {
        TT_ERROR("fail to create sha obj");

        tt_mem_free(p);
        return TT_FAIL;
    }

    sys_sha->h_sha = h_sha;
    sys_sha->mem = p;
    sys_sha->size = size;
    return TT_SUCCESS;
}

void tt_sha_destroy_ntv(IN tt_sha_ntv_t *sys_sha)
{
    BCryptDestroyHash(sys_sha->h_sha);
    tt_mem_free(sys_sha->mem);
}

tt_result_t tt_sha_update_ntv(IN tt_sha_ntv_t *sys_sha,
                              IN tt_u8_t *input,
                              IN tt_u32_t input_len)
{
    tt_sha_t *sha = TT_CONTAINER(sys_sha, tt_sha_t, sys_sha);
    return tt_s_sha_itf[sha->version].update(sys_sha, input, input_len);
}

tt_result_t tt_sha_final_ntv(IN tt_sha_ntv_t *sys_sha, OUT tt_u8_t *output)
{
    tt_sha_t *sha = TT_CONTAINER(sys_sha, tt_sha_t, sys_sha);
    return tt_s_sha_itf[sha->version].final(sys_sha, output);
}

tt_result_t tt_sha1_ntv(IN tt_u8_t *data,
                        IN tt_u32_t data_len,
                        OUT tt_u8_t *hash_val)
{
    tt_u8_t *p;

    NTSTATUS ntst;
    BCRYPT_HASH_HANDLE h_sha;

    p = (tt_u8_t *)tt_mem_alloc(tt_s_crypt_sha1_size);
    if (p == NULL) {
        TT_ERROR("fail to alloc sha obj");
        return TT_FAIL;
    }

    // create
    ntst = BCryptCreateHash(tt_s_crypt_prov_sha1,
                            &h_sha,
                            p,
                            tt_s_crypt_sha1_size,
                            NULL,
                            0,
                            0);
    if (ntst != STATUS_SUCCESS) {
        TT_ERROR("fail to create sha obj");

        tt_mem_free(p);
        return TT_FAIL;
    }

    // update
    ntst = BCryptHashData(h_sha, data, data_len, 0);
    if (ntst != STATUS_SUCCESS) {
        TT_ERROR("fail to update sha1 data");

        BCryptDestroyHash(h_sha);
        tt_mem_free(p);
        return TT_FAIL;
    }

    // final
    ntst = BCryptFinishHash(h_sha, hash_val, TT_SHA1_DIGEST_LENGTH, 0);
    BCryptDestroyHash(h_sha);
    tt_mem_free(p);
    if (ntst != STATUS_SUCCESS) {
        TT_ERROR("fail to calc sha1 data hash val");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

// TT_SHA_VER_SHA1
tt_result_t __sha1_update(IN tt_sha_ntv_t *sys_sha,
                          IN tt_u8_t *input,
                          IN tt_u32_t input_len)
{
    NTSTATUS ntst;

    ntst = BCryptHashData(sys_sha->h_sha, input, input_len, 0);
    if (ntst != STATUS_SUCCESS) {
        TT_ERROR("fail to hash sha1 data");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t __sha1_final(IN tt_sha_ntv_t *sys_sha, OUT tt_u8_t *output)
{
    NTSTATUS ntst;

    ntst = BCryptFinishHash(sys_sha->h_sha, output, TT_SHA1_DIGEST_LENGTH, 0);
    if (ntst != STATUS_SUCCESS) {
        TT_ERROR("fail to calc sha1 data hash val");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

#endif
