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

#include <math/mp/tt_mpn_mul.h>

#include <math/mp/tt_mpn_cache.h>
#include <math/mp/tt_mpn_common.h>

#include <tt_mpn_native.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __MUL_THRSH_A_BASE 500 // to be tuned
//#define __MUL_THRSH_A_BASE 20 // test

#define __MUL_THRSH_B_KRTSB 20 // to be tuned
//#define __MUL_THRSH_B_KRTSB 1 // all use krtsb

#if __MUL_THRSH_A_BASE < __MUL_THRSH_B_KRTSB
#error __MUL_THRSH_A_BASE < __MUL_THRSH_B_KRTSB
#endif

#define __SQR_THRSH_BASE 20 // to be tuned

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

extern tt_u32_t __mmpn_add(IN tt_mpn_unit_t *ret_u,
                           IN tt_mpn_unit_t *a_u,
                           IN tt_u32_t a_u_num,
                           IN tt_mpn_unit_t *b_u,
                           IN tt_u32_t b_u_num);
extern tt_u32_t __mmpn_sub(IN tt_mpn_unit_t *ret_u,
                           IN tt_mpn_unit_t *a_u,
                           IN tt_u32_t a_u_num,
                           IN tt_mpn_unit_t *b_u,
                           IN tt_u32_t b_u_num);

extern tt_s32_t __kmpn_cmp(IN tt_mpn_unit_t *a_u,
                           IN tt_mpn_unit_t *b_u,
                           IN tt_u32_t n);

extern tt_u32_t __kmpn_lshf(IN tt_mpn_unit_t *u,
                            IN tt_u32_t u_num,
                            IN tt_u32_t bit_num);

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_u32_t __mmpn_mul_base(IN tt_mpn_unit_t *ret_u,
                         IN tt_mpn_unit_t *a_u,
                         IN tt_u32_t a_u_num,
                         IN tt_mpn_unit_t *b_u,
                         IN tt_u32_t b_u_num,
                         IN tt_mpn_unit_t *tmp);
tt_u32_t __mmpn_mul_krtsb(IN tt_mpn_unit_t *ret_u,
                          IN tt_mpn_unit_t *a_u,
                          IN tt_u32_t a_u_num,
                          IN tt_mpn_unit_t *b_u,
                          IN tt_u32_t b_u_num,
                          IN tt_mpn_unit_t *tmp_u,
                          IN tt_u32_t tmp_u_num);

tt_mpn_unit_t __kmpn_mul_1(IN tt_mpn_unit_t *ret_u,
                           IN tt_mpn_unit_t *a_u,
                           IN tt_u32_t a_u_num,
                           IN tt_mpn_unit_t b_v);
tt_mpn_unit_t __kmpn_addmul_1(IN tt_mpn_unit_t *ret_u,
                              IN tt_mpn_unit_t *a_u,
                              IN tt_u32_t a_u_num,
                              IN tt_mpn_unit_t b_v);

// ret must have at lease u_num*2 units
// tmp must have at lease u_num*2 + 1 units
tt_u32_t __kmpn_sqr(OUT tt_mpn_unit_t *ret,
                    IN tt_mpn_unit_t *u,
                    IN tt_u32_t u_num,
                    IN tt_mpn_unit_t *tmp);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_mpn_mul(IN tt_mpn_t *ret,
                       IN tt_mpn_t *a,
                       IN tt_mpn_t *b,
                       IN tt_mpn_cache_t *mpnc)
{
    TT_DO(tt_umpn_mul(ret, a, b, mpnc));

    ret->negative = a->negative ^ b->negative;

    return TT_SUCCESS;
}

tt_result_t tt_umpn_mul(IN tt_mpn_t *ret,
                        IN tt_mpn_t *a,
                        IN tt_mpn_t *b,
                        IN tt_mpn_cache_t *mpnc)
{
    tt_mpn_t *p_ret = NULL, *tmp_bn = NULL;
    tt_u32_t u_num;
    tt_result_t result = TT_FAIL;

    tt_mpn_normalize(a);
    tt_mpn_normalize(b);
    if (a->unit_num < b->unit_num) {
        TT_SWAP(tt_mpn_t *, a, b);
    }

    // ret can not be a or b
    if ((ret == a) || (ret == b)) {
        TT_DONN_G(bm_out, p_ret = tt_mpn_alloc(mpnc, 0));
    } else {
        p_ret = ret;
    }

    u_num = a->unit_num + b->unit_num;
    TT_DO_G(bm_out, tt_mpn_reserve_u(p_ret, u_num));

    // ========================================
    // multiply start

    // always use b->unit_num to identify multiplication scale
    if (a == b) {
        TT_DONN_G(bm_out, tmp_bn = tt_mpn_alloc(mpnc, 0));
        TT_DO_G(bm_out, tt_mpn_reserve_u(tmp_bn, u_num + 1));

        p_ret->unit_num =
            __kmpn_sqr(p_ret->unit, a->unit, a->unit_num, tmp_bn->unit);
    } else if (b->unit_num <= __MUL_THRSH_B_KRTSB) {
        // b is small, use simple multiplication

        // todo: the threshold is to be tuned

        TT_DONN_G(bm_out, tmp_bn = tt_mpn_alloc(mpnc, 0));
        TT_DO_G(bm_out, tt_mpn_reserve_u(tmp_bn, u_num));

        p_ret->unit_num = __mmpn_mul_base(p_ret->unit,
                                          a->unit,
                                          a->unit_num,
                                          b->unit,
                                          b->unit_num,
                                          tmp_bn->unit);
    } else {
#if 0
        tt_mpn_unit_t *ret_u = p_ret->unit;
        tt_mpn_unit_t *a_u = a->unit;
        tt_u32_t a_u_num = a->unit_num;
        tt_mpn_unit_t *b_u = b->unit;
        tt_u32_t b_u_num = b->unit_num;
        tt_u32_t i;
        
        // todo:
        //  - use platform addmul instruction
        //  - put ret, a, b to a memory block of cache size, so
        //    that data accessing during multiply are all in data
        //    cache
        //  - toom-cook
        //  - fft
        
        ret_u[a_u_num] = __kmpn_mul_1(ret_u, a_u, a_u_num, b_u[0]);
        
        for (i = 1; i < b_u_num; ++i)
        {
            ret_u[a_u_num + i] = __kmpn_addmul_1(ret_u + i,
                                                 a_u,
                                                 a_u_num,
                                                 b_u[i]);
        }
        
        p_ret->unit_num = a_u_num + b_u_num;
#else
        TT_DONN_G(bm_out, tmp_bn = tt_mpn_alloc(mpnc, 0));
        TT_DO_G(bm_out, tt_mpn_reserve_u(tmp_bn, a->unit_num << 2));

        p_ret->unit_num =
            __mmpn_mul_krtsb(p_ret->unit,
                             a->unit,
                             a->unit_num,
                             b->unit,
                             b->unit_num,
                             tmp_bn->unit,
                             tmp_bn->unit_size >> TT_MPN_USIZE_ORDER);
#endif
    }

    // multiply end
    // ========================================

    if (p_ret != ret) {
        tt_mpn_swap(p_ret, ret);
    }

    result = TT_SUCCESS;

bm_out:

    tt_mpn_free(mpnc, tmp_bn);

    if (p_ret != ret) {
        tt_mpn_free(mpnc, p_ret);
    }

    return result;
}

tt_result_t tt_mpn_sqr(OUT OPT tt_mpn_t *ret,
                       IN tt_mpn_t *a,
                       IN tt_mpn_cache_t *mpnc)
{
    tt_mpn_t *p_ret = NULL, *tmp = NULL;
    tt_result_t result = TT_FAIL;

    if (ret == NULL) {
        TT_DONN_G(sqr_out, p_ret = tt_mpn_alloc(mpnc, 0));
    } else {
        p_ret = ret;
    }
    TT_DO_G(sqr_out, tt_mpn_reserve_u(p_ret, a->unit_num << 1));

    // ========================================
    // squaring start

    if (a->unit_num <= __SQR_THRSH_BASE) {
        TT_DONN_G(sqr_out, tmp = tt_mpn_alloc(mpnc, 0));
        TT_DO_G(sqr_out, tt_mpn_reserve_u(tmp, (a->unit_num << 1) + 1));

        p_ret->unit_num =
            __kmpn_sqr(p_ret->unit, a->unit, a->unit_num, tmp->unit);
    } else {
        TT_DONN_G(sqr_out, tmp = tt_mpn_alloc(mpnc, 0));
        TT_DO_G(sqr_out, tt_mpn_reserve_u(tmp, a->unit_num << 2));

        p_ret->unit_num =
            __mmpn_mul_krtsb(p_ret->unit,
                             a->unit,
                             a->unit_num,
                             a->unit,
                             a->unit_num,
                             tmp->unit,
                             tmp->unit_size >> TT_MPN_USIZE_ORDER);
    }
    p_ret->negative = TT_FALSE;

    // squaring end
    // ========================================

    if (p_ret != ret) {
        tt_mpn_swap(p_ret, a);
    }

    result = TT_SUCCESS;

sqr_out:

    tt_mpn_free(mpnc, tmp);

    if (p_ret != ret) {
        tt_mpn_free(mpnc, p_ret);
    }

    return result;
}

tt_u32_t __mmpn_mul_base(IN tt_mpn_unit_t *ret_u,
                         IN tt_mpn_unit_t *a_u,
                         IN tt_u32_t a_u_num,
                         IN tt_mpn_unit_t *b_u,
                         IN tt_u32_t b_u_num,
                         IN tt_mpn_unit_t *tmp)
{
    tt_u32_t i;

    TT_ASSERT_MPN(a_u_num >= b_u_num);

    if (a_u_num <= __MUL_THRSH_A_BASE) {
        TT_ASSERT_MPN(b_u_num != 0);
        ret_u[a_u_num] = __kmpn_mul_1(ret_u, a_u, a_u_num, b_u[0]);

        for (i = 1; i < b_u_num; ++i) {
            ret_u[a_u_num + i] =
                __kmpn_addmul_1(ret_u + i, a_u, a_u_num, b_u[i]);
        }
    } else {
        tt_u32_t u_num = a_u_num;

#define block_u_num __MUL_THRSH_A_BASE

        // todo: make the a_u seg and b_u all in cache at same time

        __mmpn_mul_base(ret_u, a_u, block_u_num, b_u, b_u_num, NULL);
        ret_u += block_u_num;
        __kmpn_copy_u(tmp, ret_u, b_u_num);
        a_u += block_u_num;
        u_num -= block_u_num;

        while (u_num > block_u_num) {
            __mmpn_mul_base(ret_u, a_u, block_u_num, b_u, b_u_num, NULL);
            __mmpn_add(ret_u, ret_u, block_u_num, tmp, b_u_num);
            ret_u += block_u_num;
            __kmpn_copy_u(tmp, ret_u, b_u_num);
            a_u += block_u_num;
            u_num -= block_u_num;
        }

        if (u_num > b_u_num) {
            __mmpn_mul_base(ret_u, a_u, u_num, b_u, b_u_num, NULL);
        } else {
            TT_ASSERT_MPN(u_num > 0);
            __mmpn_mul_base(ret_u, b_u, b_u_num, a_u, u_num, NULL);
        }
        __mmpn_add(ret_u, ret_u, u_num + b_u_num, tmp, b_u_num);
    }

    return a_u_num + b_u_num;
}

tt_mpn_unit_t __kmpn_mul_1(IN tt_mpn_unit_t *ret_u,
                           IN tt_mpn_unit_t *a_u,
                           IN tt_u32_t a_u_num,
                           IN tt_mpn_unit_t b_v)
{
    tt_mpn_unit_t h_ret_v, l_ret_v, a_v, carry;
    tt_u32_t i;

    carry = 0;
    for (i = 0; i < a_u_num; ++i) {
        a_v = a_u[i];
        __MPN_MUL(a_v, b_v, h_ret_v, l_ret_v);
        l_ret_v += carry;
        carry = TT_COND(l_ret_v < carry, 1, 0) + h_ret_v;
        ret_u[i] = l_ret_v;
    }

    return carry;
}

tt_mpn_unit_t __kmpn_addmul_1(IN tt_mpn_unit_t *ret_u,
                              IN tt_mpn_unit_t *a_u,
                              IN tt_u32_t a_u_num,
                              IN tt_mpn_unit_t b_v)
{
    tt_mpn_unit_t h_ret_v, l_ret_v, a_v, ret_v, carry;
    tt_u32_t i;

    carry = 0;
    for (i = 0; i < a_u_num; ++i) {
        a_v = a_u[i];
        __MPN_MUL(a_v, b_v, h_ret_v, l_ret_v);
        l_ret_v += carry;
        carry = TT_COND(l_ret_v < carry, 1, 0) + h_ret_v;

        ret_v = ret_u[i];
        ret_v += l_ret_v;
        carry += TT_COND(ret_v < l_ret_v, 1, 0);
        ret_u[i] = ret_v;
    }

    return carry;
}

// ========================================
// karatsuba multiplication
// ========================================

#define __rm_0(p, n)                                                           \
    do {                                                                       \
        tt_u32_t i;                                                            \
        for (i = n - 1; i != ~0; --i) {                                        \
            if (p[i] != 0) {                                                   \
                break;                                                         \
            }                                                                  \
        }                                                                      \
        n = i + 1;                                                             \
        if (n == 0) {                                                          \
            p = NULL;                                                          \
        }                                                                      \
    } while (0)

#define __salloc(p, u_n, from_u, from_u_n)                                     \
    do {                                                                       \
        TT_ASSERT_MPN((from_u_n) >= (u_n));                                    \
        (p) = (from_u);                                                        \
        (from_u) += (u_n);                                                     \
        (from_u_n) -= (u_n);                                                   \
    } while (0)
#define __sfree(u_n, from_u, from_u_n)                                         \
    do {                                                                       \
        (from_u) -= (u_n);                                                     \
        (from_u_n) += (u_n);                                                   \
    } while (0)

#define __fill_v(p, from, to, v)                                               \
    do {                                                                       \
        tt_u32_t i;                                                            \
        TT_ASSERT_MPN((from) <= (to));                                         \
        for (i = (from); i < (to); ++i) {                                      \
            (p)[(i)] = (v);                                                    \
        }                                                                      \
    } while (0)

tt_u32_t __mmpn_mul_krtsb(IN tt_mpn_unit_t *ret_u,
                          IN tt_mpn_unit_t *a_u,
                          IN tt_u32_t a_u_num,
                          IN tt_mpn_unit_t *b_u,
                          IN tt_u32_t b_u_num,
                          IN tt_mpn_unit_t *tmp_u,
                          IN tt_u32_t tmp_u_num)
{
    tt_mpn_unit_t *x1, *x0, *y1, *y0;
    tt_u32_t x1_un, x0_un, y1_un, y0_un, radix_un;

    tt_mpn_unit_t *x1ax0, *y1ay0, *z2, *z1, *z0;
    tt_u32_t x1ax0_un, y1ay0_un, z2_un, z1_un, z0_un;
    tt_u32_t x1ax0_m, y1ay0_m, z2_m, z1_m, z0_m;

    // assumes:
    //  - ret_u has space of at least a_u_num + b_u_num

    // Karatsuba's algorithm:
    //  x = x1 * b + x0
    //  y = y1 * b + y0
    //  x*y = z2 * b^2 + z1 * b + z0
    //  z2 = x1 * y1
    //  z1 = (x1 + x0) * (y1 + y0) - x1 * y1 - x0 * y0
    //  z0 = x0 * y0

    if ((a_u_num == 0) || (b_u_num == 0)) {
        return 0;
    }

    if (a_u_num < b_u_num) {
        TT_SWAP(tt_mpn_unit_t *, a_u, b_u);
        TT_SWAP(tt_u32_t, a_u_num, b_u_num);
    }

    if (b_u_num <= __MUL_THRSH_B_KRTSB) {
        return __mmpn_mul_base(ret_u, a_u, a_u_num, b_u, b_u_num, tmp_u);
    }

    radix_un = a_u_num >> 1;

    // split x
    x0 = a_u;
    x0_un = radix_un;
    x1 = a_u + radix_un;
    x1_un = a_u_num - radix_un;
    TT_ASSERT_MPN(x1_un >= x0_un);
    __rm_0(x0, x0_un);

    // split y
    y0 = b_u;
    if (b_u_num <= radix_un) {
        y0_un = b_u_num;
        y1 = NULL;
        y1_un = 0;
    } else {
        y0_un = radix_un;
        y1 = b_u + radix_un;
        y1_un = b_u_num - radix_un;
        __rm_0(y0, y0_un);
    }

    // z0
    z0_m = (radix_un << 1);
    z0 = ret_u;
    z0_un = __mmpn_mul_krtsb(z0, x0, x0_un, y0, y0_un, tmp_u, tmp_u_num);
    __fill_v(z0, z0_un, z0_m, 0);

    // z2
    z2_m = a_u_num + b_u_num - z0_m;
    z2 = ret_u + z0_m;
    z2_un = __mmpn_mul_krtsb(z2, x1, x1_un, y1, y1_un, tmp_u, tmp_u_num);
    __fill_v(z2, z2_un, z2_m, 0);

    // z1
    z1_m = x1_un + 1 + TT_MAX(y1_un, y0_un) + 1;
    __salloc(z1, z1_m, tmp_u, tmp_u_num);

    // (x1 + x0)
    x1ax0_m = x1_un + 1;
    __salloc(x1ax0, x1ax0_m, tmp_u, tmp_u_num);
    x1ax0_un = __mmpn_add(x1ax0, x1, x1_un, x0, x0_un);

    // (y1 + y0)
    if (y1_un == 0) {
        y1ay0_m = 0;
        y1ay0 = y0;
        y1ay0_un = y0_un;
    } else if (y1_un >= y0_un) {
        y1ay0_m = y1_un + 1;
        __salloc(y1ay0, y1ay0_m, tmp_u, tmp_u_num);
        y1ay0_un = __mmpn_add(y1ay0, y1, y1_un, y0, y0_un);
    } else {
        y1ay0_m = y0_un + 1;
        __salloc(y1ay0, y1ay0_m, tmp_u, tmp_u_num);
        y1ay0_un = __mmpn_add(y1ay0, y0, y0_un, y1, y1_un);
    }

    // (x1 + x0) * (y1 + y0) - x0 * y0 - x1 * y1
    z1_un = __mmpn_mul_krtsb(z1,
                             x1ax0,
                             x1ax0_un,
                             y1ay0,
                             y1ay0_un,
                             tmp_u,
                             tmp_u_num);
    z1_un = __mmpn_sub(z1, z1, z1_un, z0, z0_un);
    z1_un = __mmpn_sub(z1, z1, z1_un, z2, z2_un);
    __rm_0(z1, z1_un);

    __sfree(z1_m + x1ax0_m + y1ay0_m, tmp_u, tmp_u_num);

    // z2 * b^2 + z0 + z1 * b
    if (z2_un + radix_un > z1_un) {
        return radix_un + __mmpn_add(ret_u + radix_un,
                                     ret_u + radix_un,
                                     z2_un + radix_un,
                                     z1,
                                     z1_un);
    } else {
        return radix_un + __mmpn_add(ret_u + radix_un,
                                     z1,
                                     z1_un,
                                     ret_u + radix_un,
                                     z2_un + radix_un);
    }
}

tt_u32_t __kmpn_sqr(OUT tt_mpn_unit_t *ret,
                    IN tt_mpn_unit_t *u,
                    IN tt_u32_t u_num,
                    IN tt_mpn_unit_t *tmp)
{
    tt_u32_t i, n;

    // ret[0...2u-1]
    for (i = 0; i < u_num; ++i) {
        tt_mpn_unit_t u_v, h_v, l_v;
        u_v = u[i];
        __MPN_MUL(u_v, u_v, h_v, l_v);
        n = i << 1;
        ret[n] = l_v;
        ret[n + 1] = h_v;
    }

    if (u_num > 1) {
        // tmp[0...2u-1]
        n = u_num << 1;
        for (i = 0; i < n; ++i) {
            tmp[i] = 0;
        }

        tmp[u_num] = __kmpn_mul_1(tmp + 1, u + 1, u_num - 1, u[0]);
        for (i = 2; i < u_num; ++i) {
            tmp[u_num + i - 1] =
                __kmpn_addmul_1(tmp + (i << 1) - 1, u + i, u_num - i, u[i - 1]);
        }

        // tmp *= 2
        i = __kmpn_lshf(tmp, n, 1);
        TT_ASSERT_MPN(i <= n);

        // ret += tmp
        i = __mmpn_add(ret, ret, n, tmp, n);
        TT_ASSERT_MPN(i <= n);
    }

    return u_num << 1;
}
