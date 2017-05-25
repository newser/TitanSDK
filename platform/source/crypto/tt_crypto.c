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

#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <os/tt_thread.h>

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
    return TT_SUCCESS;
}

int tt_crypto_rng(IN void *param, IN unsigned char *buf, IN size_t len)
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
