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

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <math/mp/tt_mpn_sub.h>

#include <log/tt_log.h>
#include <math/mp/tt_mpn_common.h>
#include <misc/tt_assert.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_umpn_add(IN tt_mpn_t *ret,
                               IN tt_mpn_t *a,
                               IN tt_mpn_t *b);

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_u32_t __mmpn_sub(IN tt_mpn_unit_t *ret_u,
                    IN tt_mpn_unit_t *a_u,
                    IN tt_u32_t a_u_num,
                    IN tt_mpn_unit_t *b_u,
                    IN tt_u32_t b_u_num);

tt_mpn_unit_t __kmpn_sub_n(IN tt_mpn_unit_t *ret_u,
                           IN tt_mpn_unit_t *a_u,
                           IN tt_u32_t a_u_num,
                           IN tt_mpn_unit_t *b_u,
                           IN tt_mpn_unit_t borrow);
tt_mpn_unit_t __kmpn_sub_1(IN tt_mpn_unit_t *ret_u,
                           IN tt_mpn_unit_t *a_u,
                           IN tt_u32_t a_u_num,
                           IN tt_mpn_unit_t b_v,
                           IN tt_mpn_unit_t borrow);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_mpn_sub(IN tt_mpn_t *ret, IN tt_mpn_t *a, IN tt_mpn_t *b)
{
    if (a->negative == b->negative) {
        tt_bool_t neg;

        TT_DO(tt_umpn_sub(ret, a, b, &neg));
        ret->negative = a->negative ^ neg;

        return TT_SUCCESS;
    } else {
        TT_DO(tt_umpn_add(ret, a, b));
        ret->negative = a->negative;

        return TT_SUCCESS;
    }
}

tt_result_t tt_mpn_sub_ulshf_u(IN tt_mpn_t *ret,
                               IN tt_mpn_t *a,
                               IN tt_mpn_unit_t unit,
                               IN tt_bool_t negative,
                               IN tt_u32_t lshf_u_num)
{
    tt_mpn_t b;
    tt_result_t result;

    tt_mpn_init(&b);
    TT_DO(tt_mpn_ulshf_u(&b, unit, negative, lshf_u_num));

    result = tt_mpn_sub(ret, a, &b);

    tt_mpn_destroy(&b);
    return result;
}

tt_result_t tt_umpn_sub(IN tt_mpn_t *ret,
                        IN tt_mpn_t *a,
                        IN tt_mpn_t *b,
                        OUT OPT tt_bool_t *negative)
{
    tt_bool_t swapped = TT_FALSE;

    if (tt_mpn_cmp_abs(a, b) < 0) {
        TT_SWAP(tt_mpn_t *, a, b);
        swapped = TT_TRUE;
    }

    TT_DO(tt_mpn_reserve_u(ret, a->unit_num));

    ret->unit_num =
        __mmpn_sub(ret->unit, a->unit, a->unit_num, b->unit, b->unit_num);
    if (negative != NULL) {
        *negative = swapped;
    }

    return TT_SUCCESS;
}

tt_u32_t __mmpn_sub(IN tt_mpn_unit_t *ret_u,
                    IN tt_mpn_unit_t *a_u,
                    IN tt_u32_t a_u_num,
                    IN tt_mpn_unit_t *b_u,
                    IN tt_u32_t b_u_num)
{
    tt_mpn_unit_t borrow;

    TT_ASSERT_MPN(a_u_num >= b_u_num);

    borrow = __kmpn_sub_n(ret_u, a_u, b_u_num, b_u, 0);
    if (a_u_num > b_u_num) {
        borrow = __kmpn_sub_1(ret_u + b_u_num,
                              a_u + b_u_num,
                              a_u_num - b_u_num,
                              0,
                              borrow);
    }

    TT_ASSERT_MPN(borrow == 0);
    return a_u_num;
}

tt_mpn_unit_t __kmpn_sub_n(IN tt_mpn_unit_t *ret_u,
                           IN tt_mpn_unit_t *a_u,
                           IN tt_u32_t a_u_num,
                           IN tt_mpn_unit_t *b_u,
                           IN tt_mpn_unit_t borrow)
{
    tt_u32_t i;

    for (i = 0; i < a_u_num; ++i) {
        tt_mpn_unit_t a_v = a_u[i];
        tt_mpn_unit_t b_v = b_u[i];
        tt_mpn_unit_t ret_v = b_v + borrow;
        borrow = TT_COND(ret_v < b_v, 1, 0);
        ret_v = a_v - ret_v;
        borrow += TT_COND(ret_v > a_v, 1, 0);
        ret_u[i] = ret_v;
    }

    return borrow;
}

tt_mpn_unit_t __kmpn_sub_1(IN tt_mpn_unit_t *ret_u,
                           IN tt_mpn_unit_t *a_u,
                           IN tt_u32_t a_u_num,
                           IN tt_mpn_unit_t b_v,
                           IN tt_mpn_unit_t borrow)
{
    tt_mpn_unit_t a_v, ret_v;
    tt_u32_t i;

    a_v = a_u[0];
    ret_v = b_v + borrow;
    borrow = TT_COND(ret_v < b_v, 1, 0);
    ret_v = a_v - ret_v;
    borrow += TT_COND(ret_v > a_v, 1, 0);
    ret_u[0] = ret_v;

    for (i = 1; i < a_u_num && (borrow != 0); ++i) {
        a_v = a_u[i];
        ret_v = a_v - borrow;
        borrow = TT_COND(ret_v > a_v, 1, 0);
        ret_u[i] = ret_v;
    }
    for (; i < a_u_num; ++i) {
        ret_u[i] = a_u[i];
    }

    return borrow;
}
