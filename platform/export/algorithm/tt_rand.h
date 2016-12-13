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

/**
@file tt_rand.h
@brief pseudo random rngerator

this file includes pseudo random rngerator
*/

#ifndef __TT_RAND__
#define __TT_RAND__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_rand_xorshift.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_RNG_XORSHIFT 0

// use macro to choose random number rngerator
#define TT_RNG TT_RNG_XORSHIFT

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef union
{
    tt_rng_xorshift_t xorshift;
} tt_rng_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern void tt_rng_component_register();

// ========================================
// choose rngerator
// ========================================

#if (TT_RNG == TT_RNG_XORSHIFT)
// xorshift

tt_inline void tt_rng_init(IN tt_rng_t *rng)
{
    tt_rng_xorshift_init(&rng->xorshift);
}

tt_inline tt_u64_t tt_rng_u64(IN tt_rng_t *rng)
{
    return tt_rng_xorshift_u64(&rng->xorshift);
}
#define tt_rng_u32(r) ((tt_u32_t)tt_rng_u64((r)))

#else
// unknown

#error unknown random number rngerator

#endif

#endif /* __TT_RAND__ */
