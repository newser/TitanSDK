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

/**
@file tt_rng.h
@brief pseudo random number genrator

this file includes pseudo random number genrator
*/

#ifndef __TT_RNG__
#define __TT_RNG__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <misc/tt_util.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_RNG_CAST(rng, type) TT_PTR_INC(type, rng, sizeof(tt_rng_t))

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_rng_s;

typedef tt_u64_t (*tt_rng_u64_t)(IN struct tt_rng_s *rng);

typedef void (*tt_rng_destroy_t)(IN struct tt_rng_s *rng);

typedef struct
{
    tt_rng_u64_t rng_u64;
    tt_rng_destroy_t destroy;
} tt_rng_itf_t;

typedef struct tt_rng_s
{
    tt_rng_itf_t *itf;
} tt_rng_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export void tt_rng_component_register();

tt_export tt_rng_t *tt_rng_create(IN tt_u32_t size, IN tt_rng_itf_t *itf);

tt_export void tt_rng_destroy(IN tt_rng_t *rng);

tt_inline tt_u64_t tt_rng_u64(IN tt_rng_t *rng)
{
    return rng->itf->rng_u64(rng);
}

#endif /* __TT_RNG__ */
