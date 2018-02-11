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
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_rng.h>

#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <memory/tt_memory_alloc.h>

#include <tt_rng_native.h>

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

static tt_result_t __rng_component_init(IN tt_component_t *comp,
                                        IN tt_profile_t *profile);

static void __rng_component_exit(IN tt_component_t *comp);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_rng_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __rng_component_init, __rng_component_exit,
    };

    // init component
    tt_component_init(&comp,
                      TT_COMPONENT_RAND,
                      "Random Number Generator",
                      NULL,
                      &itf);

    // register component
    tt_component_register(&comp);
}

tt_rng_t *tt_rng_create(IN tt_u32_t size, IN tt_rng_itf_t *itf)
{
    tt_rng_t *rng;

    TT_ASSERT(itf != NULL);
    TT_ASSERT(itf->rng_u64 != NULL);

    rng = tt_malloc(sizeof(tt_rng_t) + size);
    if (rng == NULL) {
        TT_ERROR("no mem for rng");
        return NULL;
    }

    rng->itf = itf;

    return rng;
}

void tt_rng_destroy(IN tt_rng_t *rng)
{
    TT_ASSERT(rng != NULL);

    if (rng->itf->destroy != NULL) {
        rng->itf->destroy(rng);
    }

    tt_free(rng);
}

tt_result_t __rng_component_init(IN tt_component_t *comp,
                                 IN tt_profile_t *profile)
{
    // init platform specific
    if (!TT_OK(tt_rng_component_init_ntv())) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void __rng_component_exit(IN tt_component_t *comp)
{
    tt_rng_component_exit_ntv();
}
