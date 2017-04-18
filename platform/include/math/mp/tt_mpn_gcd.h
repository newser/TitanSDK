/* Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for subitional information regarding copyright ownership.
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
@file tt_bn_gcd.h
@brief multiple precision integer: greatest common divisor

this file includes multiple precision integer greatest common
divisor API
*/

#ifndef __TT_MPN_GCD__
#define __TT_MPN_GCD__

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

extern tt_result_t tt_mpn_gcd(OUT tt_mpn_t *divisor,
                              IN tt_mpn_t *u,
                              IN tt_mpn_t *v,
                              IN tt_mpn_cache_t *mpnc);

// - returned divisor is always positive
// - u*u_cof + v*v_cof = divisor is always satisfied
extern tt_result_t tt_mpn_extgcd(OUT tt_mpn_t *divisor,
                                 OUT OPT tt_mpn_t *u_cof,
                                 OUT OPT tt_mpn_t *v_cof,
                                 IN tt_mpn_t *u,
                                 IN tt_mpn_t *v,
                                 IN tt_mpn_cache_t *mpnc);

// - return fail when: a <= 0 or m <= 1, so a and m can not
//   be negative
// - when return: a*ret mod m = 1 and ret < m
// - if negative a or m is allowed, then a*ret mod m would
//   be 1-m or 1 or 1+m
extern tt_result_t tt_mpn_modminv(OUT tt_mpn_t *ret,
                                  IN tt_mpn_t *a,
                                  IN tt_mpn_t *m,
                                  IN tt_mpn_cache_t *mpnc);

#endif /* __TT_MPN_GCD__ */
