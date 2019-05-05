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
 * See the License for the specific language governing entropyermissions and
 * limitations under the License.
 */

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <crypto/tt_entropy.h>

#include <crypto/tt_crypto.h>
#include <memory/tt_memory_alloc.h>
#include <misc/tt_assert.h>
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

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_entropy_t *tt_entropy_create()
{
    tt_entropy_t *entropy;
    int e;

    entropy = tt_malloc(sizeof(tt_entropy_t));
    if (entropy == NULL) {
        TT_ERROR("no mem for entropy");
        return NULL;
    }

    mbedtls_entropy_init(&entropy->ctx);

    e = mbedtls_entropy_gather(&entropy->ctx);
    if (e != 0) {
        tt_crypto_error("fail to gather entropy");
        tt_free(entropy);
        return NULL;
    }

    return entropy;
}

void tt_entropy_destroy(IN tt_entropy_t *entropy)
{
    TT_ASSERT(entropy != NULL);

    mbedtls_entropy_free(&entropy->ctx);

    tt_free(entropy);
}

tt_entropy_t *tt_current_entropy()
{
    tt_thread_t *t = tt_current_thread();
    if (t->entropy == NULL) { t->entropy = tt_entropy_create(); }
    return t->entropy;
}
