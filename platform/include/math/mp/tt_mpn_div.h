/* Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for divitional information regarding copyright ownership.
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
@file tt_bn_div.h
@brief multiple precision integer div

this file includes multiple precision integer divide API
*/

#ifndef __TT_MPN_DIV__
#define __TT_MPN_DIV__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <math/mp/tt_mpn.h>
#include <math/mp/tt_mpn_cache.h>

#include <tt_mpn_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define __mpn_div2by1(q1, r, n1, n0, d, d_rp)                                  \
    do {                                                                       \
        tt_mpn_unit_t q0, t;                                                   \
                                                                               \
        TT_ASSERT_MPN((d & (((tt_mpn_unit_t)1) << (TT_MPN_USIZE_BIT - 1))) !=  \
                      0);                                                      \
        TT_ASSERT_MPN(n1 < d);                                                 \
                                                                               \
        __MPN_MUL(n1, d_rp, q1, q0);                                           \
        q0 += n0;                                                              \
        q1 += TT_COND(q0 < n0, 1, 0);                                          \
        q1 += n1;                                                              \
        q1 += 1;                                                               \
                                                                               \
        __MPN_MUL(q1, d, t, r);                                                \
        (void)t;                                                               \
        r = n0 - r;                                                            \
        if (r > q0) {                                                          \
            q1 -= 1;                                                           \
            r += d;                                                            \
        }                                                                      \
        if (r >= d) {                                                          \
            q1 += 1;                                                           \
            r -= d;                                                            \
        }                                                                      \
    } while (0)

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

// - sign of quotient = (sign of a) ^ (sign of b), like multiply
// - sign of remainder is same with a
// - return fail if b is 0
extern tt_result_t tt_mpn_div(OUT OPT tt_mpn_t *q,
                              OUT OPT tt_mpn_t *r,
                              IN tt_mpn_t *a,
                              IN tt_mpn_t *b,
                              IN tt_mpn_cache_t *mpnc);

tt_inline tt_result_t tt_mpn_div_q(OUT OPT tt_mpn_t *q,
                                   IN tt_mpn_t *a,
                                   IN tt_mpn_t *b,
                                   IN tt_mpn_cache_t *mpnc)
{
    // todo: could it be optimized when only want quotient?
    return tt_mpn_div(q, NULL, a, b, mpnc);
}

tt_inline tt_result_t tt_mpn_div_r(OUT OPT tt_mpn_t *r,
                                   IN tt_mpn_t *a,
                                   IN tt_mpn_t *b,
                                   IN tt_mpn_cache_t *mpnc)
{
    // todo: could it be optimized when only want remainder?
    return tt_mpn_div(NULL, r, a, b, mpnc);
}

tt_inline tt_result_t tt_mpn_diveq(IN OUT tt_mpn_t *a,
                                   IN tt_mpn_t *b,
                                   IN tt_mpn_cache_t *mpnc)
{
    return tt_mpn_div(a, NULL, a, b, mpnc);
}

extern tt_result_t tt_umpn_div(OUT OPT tt_mpn_t *q,
                               OUT OPT tt_mpn_t *r,
                               IN tt_mpn_t *a,
                               IN tt_mpn_t *b,
                               IN tt_mpn_cache_t *mpnc);

#endif /* __TT_MPN_DIV__ */
