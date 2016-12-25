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

#include <tt_md5_native.h>

#include <crypto/tt_md5.h>
#include <init/tt_profile.h>
#include <memory/tt_memory_alloc.h>
#include <misc/tt_assert.h>

#include <tt_crypto_native.h>

#ifdef TSCM_PLATFORM_CRYPTO_ENABLE

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

static BCRYPT_ALG_HANDLE tt_s_crypt_prov_md5;
static ULONG tt_s_crypt_md5_size;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_md5_component_init_ntv(IN tt_profile_t *profile)
{
    NTSTATUS ntst;
    ULONG val, n;

    // provider
    ntst = BCryptOpenAlgorithmProvider(&tt_s_crypt_prov_md5,
                                       BCRYPT_MD5_ALGORITHM,
                                       MS_PRIMITIVE_PROVIDER,
                                       0);
    if ((ntst != STATUS_SUCCESS) || (tt_s_crypt_prov_md5 == NULL)) {
        TT_ERROR("fail to create md5 provider");
        return TT_FAIL;
    }

    // md5 object size
    ntst = BCryptGetProperty(tt_s_crypt_prov_md5,
                             BCRYPT_OBJECT_LENGTH,
                             (PUCHAR)&tt_s_crypt_md5_size,
                             sizeof(tt_s_crypt_md5_size),
                             &n,
                             0);
    if ((ntst != STATUS_SUCCESS) || (tt_s_crypt_md5_size == 0)) {
        TT_ERROR("fail to get md5 obj size");
        return TT_FAIL;
    }

    // check md5 hash size, should be 16
    ntst = BCryptGetProperty(tt_s_crypt_prov_md5,
                             BCRYPT_HASH_LENGTH,
                             (PUCHAR)&val,
                             sizeof(val),
                             &n,
                             0);
    if ((ntst != STATUS_SUCCESS) || (val != TT_MD5_DIGEST_LENGTH)) {
        TT_ERROR("fail to get md5 hash size");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_md5_create_ntv(IN tt_md5_ntv_t *sys_md5)
{
    tt_u8_t *p;
    NTSTATUS ntst;
    BCRYPT_HASH_HANDLE h_md5;

    TT_ASSERT(tt_s_crypt_md5_size != 0);
    p = tt_malloc(tt_s_crypt_md5_size);
    if (p == NULL) {
        TT_ERROR("no mem for md5 obj");
        return TT_FAIL;
    }

    ntst = BCryptCreateHash(tt_s_crypt_prov_md5,
                            &h_md5,
                            p,
                            tt_s_crypt_md5_size,
                            NULL,
                            0,
                            0);
    if (ntst != STATUS_SUCCESS) {
        TT_ERROR("fail to create md5 obj");

        tt_free(p);
        return TT_FAIL;
    }

    sys_md5->h_md5 = h_md5;
    sys_md5->mem = p;
    return TT_SUCCESS;
}

void tt_md5_destroy_ntv(IN tt_md5_ntv_t *sys_md5)
{
    BCryptDestroyHash(sys_md5->h_md5);
    tt_free(sys_md5->mem);
}

tt_result_t tt_md5_update_ntv(IN tt_md5_ntv_t *sys_md5,
                              IN tt_u8_t *input,
                              IN tt_u32_t input_len)
{
    NTSTATUS ntst;

    ntst = BCryptHashData(sys_md5->h_md5, input, input_len, 0);
    if (ntst != STATUS_SUCCESS) {
        TT_ERROR("fail to hash md5 data");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_md5_final_ntv(IN tt_md5_ntv_t *sys_md5, OUT tt_u8_t *output)
{
    NTSTATUS ntst;

    ntst = BCryptFinishHash(sys_md5->h_md5, output, TT_MD5_DIGEST_LENGTH, 0);
    if (ntst != STATUS_SUCCESS) {
        TT_ERROR("fail to calc md5 data hash val");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

#endif
