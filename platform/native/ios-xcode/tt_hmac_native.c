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
#include <log/tt_log.h>
#include <misc/tt_util.h>

#ifdef TSCM_PLATFORM_CRYPTO_ENABLE

#include <CommonCrypto/CommonCrypto.h>

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

static tt_result_t __hmac_sha1_update(IN tt_hmac_ntv_t *sys_hmac,
                                      IN tt_u8_t *input,
                                      IN tt_u32_t input_len);
static tt_result_t __hmac_sha1_final(IN tt_hmac_ntv_t *sys_hmac,
                                     OUT tt_u8_t *output);
static tt_result_t __hmac_sha1_reset(IN tt_hmac_ntv_t *sys_hmac);

static __hmac_itf_t tt_s_hmac_itf[TT_HMAC_VER_NUM] = {
    // TT_HMAC_VER_SHA1
    {__hmac_sha1_update, __hmac_sha1_final, __hmac_sha1_reset}};

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
    switch (version) {
        case TT_HMAC_VER_SHA1: {
            CCHmacInit(&sys_hmac->hmac, kCCHmacAlgSHA1, key->addr, key->len);
        } break;

        default: {
            return TT_FAIL;
        } break;
    }

    // save hmac key
    if (!TT_OK(tt_blob_create(&sys_hmac->key, key->addr, key->len))) {
        TT_ERROR("fail to save hmac key");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_hmac_destroy_ntv(IN tt_hmac_ntv_t *sys_hmac)
{
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
    CCHmacUpdate(&sys_hmac->hmac, input, input_len);
    return TT_SUCCESS;
}

tt_result_t __hmac_sha1_final(IN tt_hmac_ntv_t *sys_hmac, OUT tt_u8_t *output)
{
    CCHmacFinal(&sys_hmac->hmac, output);
    return TT_SUCCESS;
}

tt_result_t __hmac_sha1_reset(IN tt_hmac_ntv_t *sys_hmac)
{
    CCHmacInit(&sys_hmac->hmac,
               kCCHmacAlgSHA1,
               sys_hmac->key.addr,
               sys_hmac->key.len);

    return TT_SUCCESS;
}

#endif
