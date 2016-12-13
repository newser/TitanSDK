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
@file tt_rng_xorshit.h
@brief pseudo random generator: xorshift

this file defines xorshift pseudo random generator
*/

#ifndef __TT_RNG_XORSHIFT__
#define __TT_RNG_XORSHIFT__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_rand_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct
{
    tt_u64_t s[2];
} tt_rng_xorshift_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_inline void tt_rng_xorshift_init(IN tt_rng_xorshift_t *gen)
{
    tt_rng_ntv((tt_u8_t *)&gen->s[0], sizeof(gen->s[0]));
    tt_rng_ntv((tt_u8_t *)&gen->s[1], sizeof(gen->s[1]));
}

tt_inline tt_u64_t tt_rng_xorshift_u64(IN tt_rng_xorshift_t *gen)
{
    tt_u64_t s1 = gen->s[0];
    const tt_u64_t s0 = gen->s[1];
    gen->s[0] = s0;
    s1 ^= s1 << 23;
    return (gen->s[1] = (s1 ^ s0 ^ (s1 >> 17) ^ (s0 >> 26))) + s0;
}

#endif /* __TT_RNG_XORSHIFT__ */
