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
@file tt_mpn_exp.h
@brief multiple precision integer exponentiation

multiple precision integer exponentiation API
*/

#ifndef __TT_MPN_EXP__
#define __TT_MPN_EXP__

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

typedef struct
{
    tt_mpn_unit_t m1;
    tt_mpn_t *r2mm;
    tt_mpn_t *rmm;
} tt_mpn_montconst_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_mpn_powmodmont(OUT tt_mpn_t *ret,
                                     IN tt_mpn_t *a,
                                     IN tt_mpn_t *e,
                                     IN tt_mpn_t *m,
                                     IN OPT tt_mpn_montconst_t *mcn,
                                     IN tt_mpn_cache_t *mpnc);

// - both a and m must be positive
tt_inline tt_result_t tt_mpn_powmod(OUT tt_mpn_t *ret,
                                    IN tt_mpn_t *a,
                                    IN tt_mpn_t *e,
                                    IN tt_mpn_t *m,
                                    IN tt_mpn_cache_t *mpnc)
{
    return tt_mpn_powmodmont(ret, a, e, m, NULL, mpnc);
}

// - m must be positive
// - mont_m1 = -(m^-1 mod r)
// - may fail when m is not a prime
extern tt_result_t tt_mpn_mont_m1(OUT tt_mpn_unit_t *mont_m1,
                                  IN tt_mpn_t *m,
                                  IN tt_mpn_cache_t *mpnc);

extern tt_result_t tt_mpn_montconst_create(OUT tt_mpn_montconst_t *mcn,
                                           IN tt_mpn_t *m,
                                           IN tt_mpn_cache_t *mpnc);

extern void tt_mpn_montconst_destroy(OUT tt_mpn_montconst_t *mcn,
                                     IN tt_mpn_cache_t *mpnc);

#endif /* __TT_MPN_EXP__ */
