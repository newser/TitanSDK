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

#include <math/mp/tt_mpn_common.h>

#include <memory/tt_memory_alloc.h>
#include <misc/tt_assert.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __bitn2un(bitnum)                                                      \
    (((bit_num) >> TT_MPN_USIZE_BIT_ORDER) +                                   \
     TT_COND((((bit_num) & ((1 << TT_MPN_USIZE_BIT_ORDER) - 1)) != 0), 1, 0))

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_inline void __mpn_abs(IN tt_mpn_t *mpn)
{
    mpn->negative = TT_FALSE;
}

tt_inline void __mpn_neg(IN tt_mpn_t *mpn)
{
    mpn->negative = !mpn->negative;
}

static tt_result_t __mpn_lshf(IN tt_mpn_t *mpn, IN tt_u32_t bit_num);

tt_u32_t __kmpn_lshf(IN tt_mpn_unit_t *u,
                     IN tt_u32_t u_num,
                     IN tt_u32_t bit_num);
tt_u32_t __kmpn_lshf_u(IN tt_mpn_unit_t *u,
                       IN tt_u32_t u_num,
                       IN tt_u32_t unit_num);

static tt_result_t __mpn_rshf(IN tt_mpn_t *mpn, IN tt_u32_t bit_num);
tt_u32_t __kmpn_rshf(IN tt_mpn_unit_t *u,
                     IN tt_u32_t u_num,
                     IN tt_u32_t bit_num);
tt_u32_t __kmpn_rshf_u(IN tt_mpn_unit_t *u,
                       IN tt_u32_t u_num,
                       IN tt_u32_t unit_num);

static void __mpn_truncate(IN tt_mpn_t *mpn, IN tt_u32_t bit_num);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_mpn_copy(IN tt_mpn_t *dst,
                        IN tt_mpn_t *src,
                        IN tt_u32_t reserve_um)
{
    tt_u32_t i;

    TT_DO(tt_mpn_reserve_u(dst, src->unit_num + reserve_um));

    if (dst == src) {
        return TT_SUCCESS;
    }

    for (i = 0; i < src->unit_num; ++i) {
        dst->unit[i] = src->unit[i];
    }
    dst->unit_num = src->unit_num;
    dst->negative = src->negative;
    dst->invalid = src->invalid;

    return TT_SUCCESS;
}

void tt_mpn_swap(IN tt_mpn_t *a, IN tt_mpn_t *b)
{
    if ((a->unit == &a->unit_inline) && (b->unit == &b->unit_inline)) {
        TT_SWAP(tt_mpn_unit_t, a->unit_inline, b->unit_inline);
        TT_SWAP(tt_bool_t, a->negative, b->negative);
        return;
    } else if (a->unit == &a->unit_inline) {
        a->unit = b->unit;
        b->unit_inline = a->unit_inline;
        b->unit = &b->unit_inline;
    } else if (b->unit == &b->unit_inline) {
        b->unit = a->unit;
        a->unit_inline = b->unit_inline;
        a->unit = &a->unit_inline;
    } else {
        TT_SWAP(tt_mpn_unit_t *, a->unit, b->unit);
    }

    TT_SWAP_U32(a->unit_size, b->unit_size);
    TT_SWAP_U32(a->unit_num, b->unit_num);
    TT_SWAP(tt_bool_t, a->negative, b->negative);
}

tt_result_t tt_mpn_abs(OUT OPT tt_mpn_t *ret, IN tt_mpn_t *mpn)
{
    if (ret != NULL) {
        TT_DO(tt_mpn_copy(ret, mpn, 0));
        mpn = ret;
    }

    __mpn_abs(mpn);
    return TT_SUCCESS;
}

tt_result_t tt_mpn_reverse(OUT OPT tt_mpn_t *ret, IN tt_mpn_t *mpn)
{
    if (ret != NULL) {
        TT_DO(tt_mpn_copy(ret, mpn, 0));
        mpn = ret;
    }

    mpn->negative = !mpn->negative;
    return TT_SUCCESS;
}

tt_s32_t __kmpn_cmp(IN tt_mpn_unit_t *a_u, IN tt_mpn_unit_t *b_u, IN tt_u32_t n)
{
    tt_u32_t i;
    for (i = n - 1; i != ~0; --i) {
        tt_mpn_unit_t a_v = a_u[i];
        tt_mpn_unit_t b_v = b_u[i];
        if (a_v < b_v) {
            return -1;
        } else if (a_v == b_v) {
            continue;
        } else {
            return 1;
        }
    }
    return 0;
}

tt_s32_t tt_mpn_cmp_abs(IN tt_mpn_t *a, IN tt_mpn_t *b)
{
    tt_mpn_normalize(a);
    tt_mpn_normalize(b);

    if (a->unit_num < b->unit_num) {
        return -1;
    } else if (a->unit_num == b->unit_num) {
        return __kmpn_cmp(a->unit, b->unit, a->unit_num);
    } else {
        return 1;
    }
}

tt_s32_t tt_mpn_cmp_uabs(IN tt_mpn_t *a, IN tt_mpn_unit_t u)
{
    tt_mpn_unit_t a0;

    tt_mpn_normalize(a);

    if (a->unit_num > 1) {
        return 1;
    }

    a0 = a->unit[0];
    if (a0 > u) {
        return 1;
    } else if (a0 == u) {
        return 0;
    } else {
        return -1;
    }
}

tt_s32_t tt_mpn_cmp_u(IN tt_mpn_t *a, IN tt_mpn_unit_t u, IN tt_bool_t negative)
{
    tt_s32_t ret;

    tt_mpn_normalize(a);

    if (tt_mpn_zero(a) && (u == 0)) {
        return 0;
    }

    if (a->negative != negative) {
        if (a->negative) {
            return -1;
        } else {
            return 1;
        }
    }

    ret = tt_mpn_cmp_uabs(a, u);
    if (a->negative) {
        return -ret;
    } else {
        return ret;
    }
}

tt_s32_t tt_mpn_cmp(IN tt_mpn_t *a, IN tt_mpn_t *b)
{
    tt_s32_t ret;

    tt_mpn_normalize(a);
    tt_mpn_normalize(b);

    if (tt_mpn_zero(a) && tt_mpn_zero(b)) {
        return 0;
    }

    if (a->negative != b->negative) {
        if (a->negative) {
            return -1;
        } else {
            return 1;
        }
    }

    ret = tt_mpn_cmp_abs(a, b);
    if (a->negative) {
        return -ret;
    } else {
        return ret;
    }
}

tt_result_t tt_mpn_lshf(OUT OPT tt_mpn_t *ret,
                        IN tt_mpn_t *mpn,
                        IN tt_u32_t bit_num)
{
    if (ret != NULL) {
        TT_DO(tt_mpn_copy(ret, mpn, __bitn2un(bit_num)));
        mpn = ret;
    }

    return __mpn_lshf(mpn, bit_num);
}

tt_result_t __mpn_lshf(IN tt_mpn_t *mpn, IN tt_u32_t bit_num)
{
    tt_u32_t unit_num;

    tt_mpn_normalize(mpn);

    unit_num = bit_num >> TT_MPN_USIZE_BIT_ORDER;
    bit_num &= (1 << TT_MPN_USIZE_BIT_ORDER) - 1;

    TT_DO(tt_mpn_reserve_um(mpn, unit_num + 1));

    // todo, shift units and bits in one loop if necessary

    mpn->unit_num = __kmpn_lshf(mpn->unit, mpn->unit_num, bit_num);
    mpn->unit_num = __kmpn_lshf_u(mpn->unit, mpn->unit_num, unit_num);
    return TT_SUCCESS;
}

tt_result_t tt_mpn_rshf(OUT OPT tt_mpn_t *ret,
                        IN tt_mpn_t *mpn,
                        IN tt_u32_t bit_num)
{
    if (ret != NULL) {
        TT_DO(tt_mpn_copy(ret, mpn, __bitn2un(bit_num)));
        mpn = ret;
    }

    return __mpn_rshf(mpn, bit_num);
}

tt_result_t tt_mpn_ulshf_u(OUT tt_mpn_t *ret,
                           IN tt_mpn_unit_t u,
                           IN tt_bool_t negative,
                           IN tt_u32_t lshf_u_num)
{
    TT_DO(tt_mpn_reserve_u(ret, 1 + lshf_u_num));
    tt_mpn_set_u(ret, u, negative, 0);
    TT_DO(tt_mpn_lshf_u(NULL, ret, lshf_u_num));

    return TT_SUCCESS;
}

tt_result_t __mpn_rshf(IN tt_mpn_t *mpn, IN tt_u32_t bit_num)
{
    tt_u32_t unit_num;

    tt_mpn_normalize(mpn);

    unit_num = bit_num >> TT_MPN_USIZE_BIT_ORDER;
    bit_num &= (1 << TT_MPN_USIZE_BIT_ORDER) - 1;

    // todo, shift units and bits in one loop if necessary

    mpn->unit_num = __kmpn_rshf(mpn->unit, mpn->unit_num, bit_num);
    mpn->unit_num = __kmpn_rshf_u(mpn->unit, mpn->unit_num, unit_num);
    return TT_SUCCESS;
}

tt_u32_t __kmpn_lshf(IN tt_mpn_unit_t *u,
                     IN tt_u32_t u_num,
                     IN tt_u32_t bit_num)
{
    tt_u32_t i, hb_num;

    TT_ASSERT_MPN(u_num != 0);

    if (bit_num == 0) {
        return u_num;
    }

    TT_ASSERT_MPN(bit_num < (1 << TT_MPN_USIZE_BIT_ORDER));
    hb_num = (1 << TT_MPN_USIZE_BIT_ORDER) - bit_num;

    u[u_num] = 0;
    for (i = u_num - 1; i != ~0; --i) {
        tt_mpn_unit_t u_v = u[i];
        u[i + 1] += u_v >> hb_num;
        u[i] = u_v << bit_num;
    }

    return TT_COND(u[u_num] != 0, u_num + 1, u_num);
}

tt_u32_t __kmpn_lshf_u(IN tt_mpn_unit_t *u,
                       IN tt_u32_t u_num,
                       IN tt_u32_t unit_num)
{
    tt_u32_t i;

    TT_ASSERT_MPN(u_num != 0);

    if (unit_num == 0) {
        return u_num;
    }

    for (i = u_num + unit_num - 1; i >= unit_num; --i) {
        u[i] = u[i - unit_num];
    }
    for (; i != ~0; --i) {
        u[i] = 0;
    }

    return u_num + unit_num;
}

tt_u32_t __kmpn_rshf(IN tt_mpn_unit_t *u,
                     IN tt_u32_t u_num,
                     IN tt_u32_t bit_num)
{
    tt_u32_t i, hb_num;

    TT_ASSERT_MPN(u_num != 0);

    if (bit_num == 0) {
        return u_num;
    }

    TT_ASSERT_MPN(bit_num < (1 << TT_MPN_USIZE_BIT_ORDER));
    hb_num = (1 << TT_MPN_USIZE_BIT_ORDER) - bit_num;

    u[0] >>= bit_num;
    for (i = 1; i < u_num; ++i) {
        tt_mpn_unit_t u_v = u[i];
        u[i - 1] += u_v << hb_num;
        u[i] = u_v >> bit_num;
    }

    if ((u[u_num - 1] != 0) || (u_num == 1)) {
        return u_num;
    } else {
        return u_num - 1;
    }
}

tt_u32_t __kmpn_rshf_u(IN tt_mpn_unit_t *u,
                       IN tt_u32_t u_num,
                       IN tt_u32_t unit_num)
{
    tt_u32_t i;

    TT_ASSERT_MPN(u_num != 0);

    if (unit_num == 0) {
        return u_num;
    }
    if (u_num <= unit_num) {
        u[0] = 0;
        return 1;
    }

    for (i = unit_num; i < u_num; ++i) {
        u[i - unit_num] = u[i];
    }

    return u_num - unit_num;
}

tt_result_t tt_mpn_prepend_un(IN OPT tt_mpn_t *ret,
                              IN tt_mpn_t *mpn,
                              IN tt_mpn_unit_t u,
                              IN tt_u32_t u_num)
{
    tt_u32_t i, n;

    if (ret != NULL) {
        TT_DO(tt_mpn_copy(ret, mpn, u_num));
        mpn = ret;
    } else {
        TT_DO(tt_mpn_reserve_um(mpn, u_num));
    }

    n = mpn->unit_num + u_num;
    for (i = mpn->unit_num; i < n; ++i) {
        mpn->unit[i] = u;
    }
    mpn->unit_num = n;

    return TT_SUCCESS;
}

tt_result_t tt_mpn_append_un(IN OPT tt_mpn_t *ret,
                             IN tt_mpn_t *mpn,
                             IN tt_mpn_unit_t u,
                             IN tt_u32_t u_num)
{
    tt_u32_t i;

    TT_DO(tt_mpn_lshf_u(ret, mpn, u_num));
    if (ret != NULL) {
        mpn = ret;
    }

    for (i = 0; i < u_num; ++i) {
        mpn->unit[i] = u;
    }

    return TT_SUCCESS;
}

tt_result_t tt_mpn_truncate(OUT tt_mpn_t *ret,
                            IN tt_mpn_t *mpn,
                            IN tt_u32_t bit_num)
{
    if (ret != NULL) {
        // may not need copy all units
        TT_DO(tt_mpn_copy(ret, mpn, 0));
        mpn = ret;
    }

    __mpn_truncate(mpn, bit_num);
    return TT_SUCCESS;
}

void __mpn_truncate(IN tt_mpn_t *mpn, IN tt_u32_t bit_num)
{
    tt_u32_t u_num;

    u_num = bit_num >> TT_MPN_USIZE_BIT_ORDER;
    bit_num &= (1 << TT_MPN_USIZE_BIT_ORDER) - 1;
    if (bit_num != 0) {
        ++u_num;
    }

    if (u_num == 0) {
        tt_mpn_clear(mpn);
    } else if (u_num <= mpn->unit_num) {
        mpn->unit_num = u_num;

        if (bit_num != 0) {
            mpn->unit[mpn->unit_num - 1] &= (((tt_mpn_unit_t)1) << bit_num) - 1;
        }
    }
}
