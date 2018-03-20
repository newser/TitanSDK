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

#include <algorithm/tt_rng_xorshift.h>

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

static tt_u64_t __rx_u64(IN tt_rng_t *rng);

static tt_rng_itf_t __rx_itf = {
    __rx_u64, NULL,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_rng_t *tt_rng_xorshift_create()
{
    tt_rng_t *rng;

    rng = tt_rng_create(sizeof(tt_rng_xorshift_t), &__rx_itf);
    if (rng != NULL) {
        tt_rng_xorshift_t *rx = TT_RNG_CAST(rng, tt_rng_xorshift_t);

        tt_rng_ntv((tt_u8_t *)&rx->s[0], sizeof(rx->s[0]));
        tt_rng_ntv((tt_u8_t *)&rx->s[1], sizeof(rx->s[1]));
    }

    return rng;
}

tt_u64_t __rx_u64(IN tt_rng_t *rng)
{
    tt_rng_xorshift_t *rx = TT_RNG_CAST(rng, tt_rng_xorshift_t);
    tt_u64_t s1 = rx->s[0];
    const tt_u64_t s0 = rx->s[1];

    rx->s[0] = s0;
    s1 ^= s1 << 23;
    return (rx->s[1] = (s1 ^ s0 ^ (s1 >> 17) ^ (s0 >> 26))) + s0;
}
