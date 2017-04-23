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
@file tt_bn_mul.h
@brief multiple precision integer mul

this file includes multiple precision integer multiply API
*/

#ifndef __TT_MPN_MUL__
#define __TT_MPN_MUL__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <math/mp/tt_mpn.h>
#include <math/mp/tt_mpn_cache.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_mpn_mul(IN tt_mpn_t *ret,
                              IN tt_mpn_t *a,
                              IN tt_mpn_t *b,
                              IN tt_mpn_cache_t *mpnc);

tt_inline tt_result_t tt_mpn_mul_u(IN tt_mpn_t *ret,
                                   IN tt_mpn_t *a,
                                   IN tt_mpn_unit_t u,
                                   IN tt_bool_t u_negative,
                                   IN tt_mpn_cache_t *mpnc)
{
    tt_mpn_t b;

    tt_mpn_init_u(&b, u, u_negative);
    return tt_mpn_mul(ret, a, &b, mpnc);
}

tt_inline tt_result_t tt_mpn_muleq(IN tt_mpn_t *a,
                                   IN tt_mpn_t *b,
                                   IN tt_mpn_cache_t *mpnc)
{
    return tt_mpn_mul(a, a, b, mpnc);
}

tt_inline tt_result_t tt_mpn_muleq_u(IN tt_mpn_t *a,
                                     IN tt_mpn_unit_t u,
                                     IN tt_bool_t u_negative,
                                     IN tt_mpn_cache_t *mpnc)
{
    tt_mpn_t b;

    tt_mpn_init_u(&b, u, u_negative);
    return tt_mpn_mul(a, a, &b, mpnc);
}

extern tt_result_t tt_mpn_sqr(OUT OPT tt_mpn_t *ret,
                              IN tt_mpn_t *mpn,
                              IN tt_mpn_cache_t *mpnc);

extern tt_result_t tt_umpn_mul(IN tt_mpn_t *ret,
                               IN tt_mpn_t *a,
                               IN tt_mpn_t *b,
                               IN tt_mpn_cache_t *mpnc);

#endif /* __TT_MPN_MUL__ */
