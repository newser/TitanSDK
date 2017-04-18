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
@file tt_bn_mod.h
@brief multiple precision modular arithmetic

this file includes multiple precision modular arithmetic
*/

#ifndef __TT_MPN_MOD__
#define __TT_MPN_MOD__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <math/mp/tt_mpn.h>
#include <math/mp/tt_mpn_cache.h>
#include <math/mp/tt_mpn_div.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct
{
    tt_mpn_t *minv;
} tt_mpn_minv_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

// - as a remainder, sign of ret is same as a
tt_inline tt_result_t tt_mpn_mod(OUT tt_mpn_t *ret,
                                 IN tt_mpn_t *a,
                                 IN tt_mpn_t *m,
                                 IN tt_mpn_cache_t *mpnc)
{
    return tt_mpn_div_r(ret, a, m, mpnc);
}

extern tt_result_t tt_mpn_modbr(OUT tt_mpn_t *ret,
                                IN tt_mpn_t *a,
                                IN tt_mpn_t *m,
                                IN OPT tt_mpn_t *brp,
                                IN OPT tt_u32_t brp_range,
                                IN tt_mpn_cache_t *mpnc);

// barrett reciprocal
// - b^brp_range / modular
// - brp_range is the max unit num of number to be modulo,
//   can be calculated by (byte << TT_MPN_USIZE)
// - set brp_range to 0 to use 2*modular->unit_num
extern tt_result_t tt_mpn_mod_brp(OUT tt_mpn_t *brp,
                                  IN OUT tt_u32_t *brp_range,
                                  IN tt_mpn_t *m,
                                  IN tt_mpn_cache_t *mpnc);

tt_inline tt_result_t tt_mpn_modeq(IN OUT tt_mpn_t *a,
                                   IN tt_mpn_t *m,
                                   IN tt_mpn_cache_t *mpnc)
{
    return tt_mpn_mod(a, a, m, mpnc);
}

#endif /* __TT_MPN_MOD__ */
