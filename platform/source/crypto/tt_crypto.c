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

#include <crypto/tt_crypto.h>

#include <crypto/tt_aes.h>
#include <crypto/tt_dh.h>
#include <crypto/tt_hmac.h>
#include <crypto/tt_rsa.h>
#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <misc/tt_assert.h>

#include <tt_crypto_native.h>

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

static tt_result_t __crypto_component_init(IN tt_component_t *comp,
                                           IN tt_profile_t *profile);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_crypto_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __crypto_component_init,
    };

    // init component
    tt_component_init(&comp, TT_COMPONENT_CRYPTO, "Crypto", NULL, &itf);

    // register component
    tt_component_register(&comp);
}

tt_result_t __crypto_component_init(IN tt_component_t *comp,
                                    IN tt_profile_t *profile)
{
    tt_result_t result;

    result = tt_crypto_init_ntv(profile);
    if (!TT_OK(result)) {
        return TT_FAIL;
    }

#if 0
    result = tt_rsa_component_init(profile);
    if (!TT_OK(result)) {
        return TT_FAIL;
    }
    TT_INFO("Intializing %-32s [Done]", "RSA");

    result = tt_md5_component_init(profile);
    if (!TT_OK(result)) {
        return TT_FAIL;
    }
    TT_INFO("Intializing %-32s [Done]", "MD5");

    result = tt_sha_component_init(profile);
    if (!TT_OK(result)) {
        return TT_FAIL;
    }
    TT_INFO("Intializing %-32s [Done]", "SHA");

    result = tt_hmac_component_init(profile);
    if (!TT_OK(result)) {
        return TT_FAIL;
    }
    TT_INFO("Intializing %-32s [Done]", "HMAC");
#endif

    /*
    result = tt_aes_component_init(profile);
    if (!TT_OK(result)) {
        return TT_FAIL;
    }
    TT_INFO("Intializing %-32s [Done]", "AES");
     */

    result = tt_dh_component_init(profile);
    if (!TT_OK(result)) {
        return TT_FAIL;
    }
    TT_INFO("Intializing %-32s [Done]", "DH");

    return TT_SUCCESS;
}
