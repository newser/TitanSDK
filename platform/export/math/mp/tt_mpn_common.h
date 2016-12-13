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
@file tt_mpn_common.h
@brief multiple precision integer, common arithmetic

this file defines multiple precision integer common arithmetic
*/

#ifndef __TT_MPN_COMMON__
#define __TT_MPN_COMMON__

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

extern tt_result_t tt_mpn_copy(IN tt_mpn_t *dst,
                               IN tt_mpn_t *src,
                               IN tt_u32_t reserve_um);

extern void tt_mpn_swap(IN tt_mpn_t *a, IN tt_mpn_t *b);

extern tt_result_t tt_mpn_abs(OUT OPT tt_mpn_t *ret, IN tt_mpn_t *mpn);

extern tt_result_t tt_mpn_reverse(OUT OPT tt_mpn_t *ret, IN tt_mpn_t *mpn);

extern tt_s32_t tt_mpn_cmp_abs(IN tt_mpn_t *a, IN tt_mpn_t *b);
extern tt_s32_t tt_mpn_cmp_uabs(IN tt_mpn_t *a, IN tt_mpn_unit_t u);

extern tt_s32_t tt_mpn_cmp(IN tt_mpn_t *a, IN tt_mpn_t *b);
extern tt_s32_t tt_mpn_cmp_u(IN tt_mpn_t *a,
                             IN tt_mpn_unit_t u,
                             IN tt_bool_t negative);

extern tt_result_t tt_mpn_lshf(OUT OPT tt_mpn_t *ret,
                               IN tt_mpn_t *mpn,
                               IN tt_u32_t bit_num);

tt_inline tt_result_t tt_mpn_lshf_u(OUT OPT tt_mpn_t *ret,
                                    IN tt_mpn_t *mpn,
                                    IN tt_u32_t unit_num)
{
    return tt_mpn_lshf(ret, mpn, unit_num << TT_MPN_USIZE_BIT_ORDER);
}

extern tt_result_t tt_mpn_rshf(OUT OPT tt_mpn_t *ret,
                               IN tt_mpn_t *mpn,
                               IN tt_u32_t bit_num);

tt_inline tt_result_t tt_mpn_rshf_u(OUT OPT tt_mpn_t *ret,
                                    IN tt_mpn_t *mpn,
                                    IN tt_u32_t unit_num)
{
    return tt_mpn_rshf(ret, mpn, unit_num << TT_MPN_USIZE_BIT_ORDER);
}

extern tt_result_t tt_mpn_ulshf_u(OUT tt_mpn_t *ret,
                                  IN tt_mpn_unit_t u,
                                  IN tt_bool_t negative,
                                  IN tt_u32_t lshf_u_num);

tt_inline void __kmpn_copy_u(IN tt_mpn_unit_t *dst,
                             IN tt_mpn_unit_t *src,
                             IN tt_u32_t u_num)
{
    tt_u32_t i;
    for (i = 0; i < u_num; ++i) {
        dst[i] = src[i];
    }
}

extern tt_result_t tt_mpn_prepend_un(IN OPT tt_mpn_t *ret,
                                     IN tt_mpn_t *mpn,
                                     IN tt_mpn_unit_t u,
                                     IN tt_u32_t u_num);

extern tt_result_t tt_mpn_append_un(IN OPT tt_mpn_t *ret,
                                    IN tt_mpn_t *mpn,
                                    IN tt_mpn_unit_t u,
                                    IN tt_u32_t u_num);

// keep bit_num LSB
extern tt_result_t tt_mpn_truncate(OUT tt_mpn_t *ret,
                                   IN tt_mpn_t *mpn,
                                   IN tt_u32_t bit_num);

#endif /* __TT_MPN_COMMON__ */
