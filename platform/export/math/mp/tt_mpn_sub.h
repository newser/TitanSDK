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
@file tt_bn_sub.h
@brief multiple precision integer substract

this file includes multiple precision integer substract API
*/

#ifndef __TT_MPN_SUB__
#define __TT_MPN_SUB__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <math/mp/tt_mpn.h>

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

extern tt_result_t tt_mpn_sub(IN tt_mpn_t *ret, IN tt_mpn_t *a, IN tt_mpn_t *b);

tt_inline tt_result_t tt_mpn_subeq(IN OUT tt_mpn_t *a, IN tt_mpn_t *b)
{
    return tt_mpn_sub(a, a, b);
}

tt_inline tt_result_t tt_mpn_sub_u(IN tt_mpn_t *ret,
                                   IN tt_mpn_t *a,
                                   IN tt_mpn_unit_t unit,
                                   IN tt_bool_t negative)
{
    tt_mpn_t b;

    tt_mpn_init_u(&b, unit, negative);
    return tt_mpn_sub(ret, a, &b);
}

tt_inline tt_result_t tt_mpn_subeq_u(IN tt_mpn_t *ret,
                                     IN tt_mpn_t *a,
                                     IN tt_mpn_unit_t unit,
                                     IN tt_bool_t negative)
{
    tt_mpn_t b;

    tt_mpn_init_u(&b, unit, negative);
    return tt_mpn_subeq(a, &b);
}

tt_inline tt_result_t tt_mpn_dec(IN tt_mpn_t *mpn)
{
    return tt_mpn_sub_u(mpn, mpn, 1, TT_FALSE);
}

extern tt_result_t tt_mpn_sub_ulshf_u(IN tt_mpn_t *ret,
                                      IN tt_mpn_t *a,
                                      IN tt_mpn_unit_t unit,
                                      IN tt_bool_t negative,
                                      IN tt_u32_t lshf_u_num);

tt_inline tt_result_t tt_mpn_subeq_ulshf_u(IN tt_mpn_t *a,
                                           IN tt_mpn_unit_t unit,
                                           IN tt_bool_t negative,
                                           IN tt_u32_t lshf_u_num)
{
    return tt_mpn_sub_ulshf_u(a, a, unit, negative, lshf_u_num);
}

extern tt_result_t tt_umpn_sub(IN tt_mpn_t *ret,
                               IN tt_mpn_t *a,
                               IN tt_mpn_t *b,
                               OUT OPT tt_bool_t *negative);

#endif /* __TT_MPN_SUB__ */
