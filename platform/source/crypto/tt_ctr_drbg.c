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

#include <crypto/tt_ctr_drbg.h>

#include <crypto/tt_crypto.h>
#include <crypto/tt_entropy.h>
#include <memory/tt_memory_alloc.h>
#include <misc/tt_assert.h>
#include <misc/tt_util.h>
#include <os/tt_thread.h>

#include <entropy.h>

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

tt_ctr_drbg_t *tt_ctr_drbg_create(IN tt_entropy_t *entropy,
                                  IN OPT tt_u8_t *opaque,
                                  IN tt_u32_t len)
{
    tt_ctr_drbg_t *drbg;
    int e;

    TT_ASSERT(entropy != NULL);

    drbg = tt_malloc(sizeof(tt_ctr_drbg_t));
    if (drbg == NULL) {
        TT_ERROR("no mem for ctr drbg");
        return NULL;
    }

    mbedtls_ctr_drbg_init(&drbg->ctx);

    e = mbedtls_ctr_drbg_seed(&drbg->ctx,
                              mbedtls_entropy_func,
                              &entropy->ctx,
                              opaque,
                              len);
    if (e != 0) {
        tt_crypto_error("fail to seed ctr drbg");
        tt_free(drbg);
        return NULL;
    }

    return drbg;
}

void tt_ctr_drbg_destroy(IN tt_ctr_drbg_t *drbg)
{
    TT_ASSERT(drbg != NULL);

    mbedtls_ctr_drbg_free(&drbg->ctx);

    tt_free(drbg);
}

tt_ctr_drbg_t *tt_current_ctr_drbg()
{
    tt_thread_t *t = tt_current_thread();
    if (t->ctr_drbg == NULL) {
        tt_entropy_t *entropy = tt_current_entropy();
        if (entropy != NULL) {
            t->ctr_drbg =
                tt_ctr_drbg_create(entropy,
                                   (tt_u8_t *)t->name,
                                   TT_COND(t->name != NULL,
                                           (tt_u32_t)tt_strlen(t->name),
                                           0));
        }
    }
    return t->ctr_drbg;
}

tt_result_t tt_ctr_drbg_rand(IN tt_ctr_drbg_t *drbg,
                             OUT tt_u8_t *buf,
                             IN tt_u32_t len)
{
    int e;

    TT_ASSERT(drbg != NULL);
    TT_ASSERT(buf != NULL);

    e = mbedtls_ctr_drbg_random(&drbg->ctx, buf, len);
    if (e != 0) {
        tt_crypto_error("ctr drbg rand fail");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

int tt_ctr_drbg(IN void *param, IN unsigned char *buf, IN size_t len)
{
    return mbedtls_ctr_drbg_random(&((tt_ctr_drbg_t *)param)->ctx, buf, len);
}
