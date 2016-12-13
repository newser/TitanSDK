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

#include <math/mp/tt_mpn_div.h>

#include <math/mp/tt_mpn_common.h>
#include <math/mp/tt_mpn_sub.h>

#include <tt_mpn_native.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __BD_LS(v, b) (((tt_mpn_unit_t)(v)) << (b))

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

extern tt_mpn_unit_t __kmpn_mul_1(IN tt_mpn_unit_t *ret_u,
                                  IN tt_mpn_unit_t *a_u,
                                  IN tt_u32_t a_u_num,
                                  IN tt_mpn_unit_t b_v);
extern tt_mpn_unit_t __kmpn_sub_n(IN tt_mpn_unit_t *ret_u,
                                  IN tt_mpn_unit_t *a_u,
                                  IN tt_u32_t a_u_num,
                                  IN tt_mpn_unit_t *b_u,
                                  IN tt_mpn_unit_t borrow);
extern tt_s32_t __kmpn_cmp(IN tt_mpn_unit_t *a_u,
                           IN tt_mpn_unit_t *b_u,
                           IN tt_u32_t n);

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __mpn_div_trivial(OUT tt_mpn_t *q,
                                     OUT tt_mpn_t *r,
                                     IN tt_mpn_t *a,
                                     IN tt_mpn_t *b);

tt_result_t __mpn_div_base(OUT tt_mpn_t *q,
                           OUT tt_mpn_t *r,
                           IN tt_mpn_t *a,
                           IN tt_mpn_t *b,
                           IN tt_mpn_cache_t *mpnc);

tt_mpn_unit_t __mpn_reciprocal_v(IN tt_mpn_unit_t d);

tt_u32_t __mmpn_div_base(IN tt_mpn_unit_t *n_u,
                         IN OUT tt_u32_t *n_u_num,
                         IN tt_mpn_unit_t *d_u,
                         IN tt_u32_t d_u_num,
                         OUT tt_mpn_unit_t *q_u,
                         IN tt_mpn_unit_t *tmp_u);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_mpn_div(OUT OPT tt_mpn_t *q,
                       OUT OPT tt_mpn_t *r,
                       IN tt_mpn_t *a,
                       IN tt_mpn_t *b,
                       IN tt_mpn_cache_t *mpnc)
{
    TT_DO(tt_umpn_div(q, r, a, b, mpnc));

    if (q != NULL) {
        q->negative = a->negative ^ b->negative;
    }

    if (r != NULL) {
        r->negative = a->negative;
    }

    return TT_SUCCESS;
}

tt_result_t tt_umpn_div(OUT OPT tt_mpn_t *q,
                        OUT OPT tt_mpn_t *r,
                        IN tt_mpn_t *a,
                        IN tt_mpn_t *b,
                        IN tt_mpn_cache_t *mpnc)
{
    tt_mpn_t *__q = NULL, *__r = NULL;
    tt_result_t result = TT_FAIL;

    tt_mpn_normalize(a);
    tt_mpn_normalize(b);

    if (tt_mpn_zero(b)) {
        TT_ERROR("0 denominator");
        return TT_FAIL;
    }

    if ((q == NULL) || (q == a) || (q == b)) {
        TT_DONN_G(div_done, __q = tt_mpn_alloc(mpnc, 0));
    } else {
        __q = q;
    }

    if ((r == NULL) || (r == a) || (r == b)) {
        TT_DONN_G(div_done, __r = tt_mpn_alloc(mpnc, 0));
    } else {
        __r = r;
    }

    result = __mpn_div_trivial(__q, __r, a, b);
    if (TT_OK(result) || (result != TT_PROCEEDING)) {
        // it's ok to go to div_done even if something error
        // occur during __mpn_div_trivial
        goto div_done;
    }
    result = TT_FAIL;

    // ========================================
    // div start

    // todo:
    //  - if b only has 1 or 2 units, use some optimized algorithm
    //  - if number is too large, calc [radix^k/b] first, i.e
    //    use Newton-Raphson alg, and then use Barrett's method
    //    to get q and r

    TT_DO(__mpn_div_base(__q, __r, a, b, mpnc));

    // div end
    // ========================================

    result = TT_SUCCESS;

div_done:

    if (__q != q) {
        if (q != NULL) {
            tt_mpn_swap(q, __q);
        }
        tt_mpn_free(mpnc, __q);
    }

    if (__r != r) {
        if (r != NULL) {
            tt_mpn_swap(r, __r);
        }
        tt_mpn_free(mpnc, __r);
    }

    return result;
}

tt_result_t __mpn_div_base(OUT tt_mpn_t *q,
                           OUT tt_mpn_t *r,
                           IN tt_mpn_t *a,
                           IN tt_mpn_t *b,
                           IN tt_mpn_cache_t *mpnc)
{
    tt_mpn_t *p_b = NULL, *tmp = NULL;
    tt_u32_t b_h0bitnum;
    tt_result_t result = TT_FAIL;

    // denominator:
    //  - make b[n-1] >= radix/2 => highest bit is 1
    b_h0bitnum = tt_mpn_h0bitnum(b);
    if (b_h0bitnum != 0) {
        TT_DONN_G(bdb_out, p_b = tt_mpn_alloc(mpnc, 0));
        TT_DO_G(bdb_out, tt_mpn_lshf(p_b, b, b_h0bitnum));
    } else {
        p_b = b;
    }

    // numerator:
    //  - must have one more unit than denominator
    TT_DO_G(bdb_out, tt_mpn_lshf(r, a, b_h0bitnum));
    a = r;
    TT_ASSERT_MPN(a->unit_num >= p_b->unit_num);
    if (a->unit_num == p_b->unit_num) {
        TT_DO_G(bdb_out, tt_mpn_reserve_um(a, 1));
        a->unit[a->unit_num++] = 0;
    }

    // quotient
    TT_DO_G(bdb_out, tt_mpn_reserve_u(q, a->unit_num - p_b->unit_num + 1));

    TT_DONN_G(bdb_out, tmp = tt_mpn_alloc(mpnc, 0));
    TT_DO_G(bdb_out, tt_mpn_reserve_u(tmp, p_b->unit_num + 1));

    // div
    q->unit_num = __mmpn_div_base(a->unit,
                                  &a->unit_num,
                                  p_b->unit,
                                  p_b->unit_num,
                                  q->unit,
                                  tmp->unit);

    if (b_h0bitnum != 0) {
        tt_mpn_rshf(NULL, r, b_h0bitnum);
    }

    result = TT_SUCCESS;

bdb_out:

    if (p_b != b) {
        tt_mpn_free(mpnc, p_b);
    }

    tt_mpn_free(mpnc, tmp);

    return result;
}

tt_result_t __mpn_div_trivial(OUT tt_mpn_t *q,
                              OUT tt_mpn_t *r,
                              IN tt_mpn_t *a,
                              IN tt_mpn_t *b)
{
    if (a->unit_num < b->unit_num) {
        // q = 0, r = a
        tt_mpn_clear(q);
        if (!TT_OK(tt_mpn_copy(r, a, 0))) {
            return TT_FAIL;
        }

        return TT_SUCCESS;
    } else if (a->unit_num == b->unit_num) {
        tt_s32_t cmp_ret = tt_mpn_cmp_abs(a, b);
        if (cmp_ret < 0) {
            // q = 0, r = a
            tt_mpn_clear(q);
            if (!TT_OK(tt_mpn_copy(r, a, 0))) {
                return TT_FAIL;
            }

            return TT_SUCCESS;
        } else if (cmp_ret == 0) {
            // q = 1, r = 0
            tt_mpn_set_u(q, 1, TT_FALSE, 0);
            tt_mpn_clear(r);

            return TT_SUCCESS;
        } else {
            return TT_PROCEEDING;
        }
    } else {
        // a and b have been normalized
        if ((b->unit_num == 1) && (b->unit[0] == 1)) {
            // q = a, r = 0
            if (!TT_OK(tt_mpn_copy(q, a, 0))) {
                return TT_FAIL;
            }
            tt_mpn_clear(r);

            return TT_SUCCESS;
        }

        return TT_PROCEEDING;
    }
}

tt_mpn_unit_t __mpn_reciprocal_v(IN tt_mpn_unit_t d)
{
    tt_u32_t d1, d0;
    tt_u32_t n2, n1, n0;
    tt_mpn_unit_t d_r, m, mh, ml, n1n0;

#define __bdr_ls(v, b) (((tt_u64_t)v) << b)
#define __bdr_mk(n2, n1) (__bdr_ls(n2, 32) + ((tt_u64_t)n1))
#define __bdr_div(n2, n1, d) (__bdr_mk(n2, n1) / d)
#define __bdr_gt(l1, l0, r1, r0) ((l1 > r1) || ((l1 == r1) && (l0 > r0)))

    // d = (floor((r*r - 1) / d) - r), r = (1 << TT_MPN_USIZE_ORDER)

    TT_ASSERT_MPN(d >= __bdr_ls(1, 63));

    d1 = d >> 32;
    d0 = d & 0xFFFFFFFF;

    // ========================================

    n2 = ~d1; // 0xFFFFFFFF - d1
    n1 = ~d0;
    n0 = ~0;

    // m = n2n1 / d1
    m = __bdr_div(n2, n1, d1);

    // m * d -2 <= [real d] <= m * d
    __MPN_MUL(d, m, mh, ml);
    n1n0 = __bdr_mk(n1, n0);
    if (__bdr_gt(mh, ml, n2, n1n0)) {
        m -= 1;
        mh -= TT_COND(ml < d, 1, 0);
        ml -= d;
    }
    if (__bdr_gt(mh, ml, n2, n1n0)) {
        m -= 1;
        mh -= TT_COND(ml < d, 1, 0);
        ml -= d;
    }
    TT_ASSERT_MPN(!__bdr_gt(mh, ml, n2, n1n0));

    // quotient
    d_r = m << 32;

    // remainder, borrow is ignored
    n1n0 -= ml;

    // ========================================

    n2 = n1n0 >> 32;
    n1 = n1n0 & 0xFFFFFFFF;
    n0 = ~0;

    // m = n2n1 / d1
    m = __bdr_div(n2, n1, d1);

    // m * d -2 <= [real d] <= m * d
    __MPN_MUL(d, m, mh, ml);
    n1n0 = __bdr_mk(n1, n0);
    if (__bdr_gt(mh, ml, n2, n1n0)) {
        m -= 1;
        mh -= TT_COND(ml < d, 1, 0);
        ml -= d;
    }
    if (__bdr_gt(mh, ml, n2, n1n0)) {
        m -= 1;
        mh -= TT_COND(ml < d, 1, 0);
        ml -= d;
    }
    TT_ASSERT_MPN(!__bdr_gt(mh, ml, n2, n1n0));

    // quotient
    d_r += m;

    // ========================================

    return d_r;
}

tt_u32_t __mmpn_div_base(IN tt_mpn_unit_t *n_u,
                         IN OUT tt_u32_t *n_u_num,
                         IN tt_mpn_unit_t *d_u,
                         IN tt_u32_t d_u_num,
                         OUT tt_mpn_unit_t *q_u,
                         IN tt_mpn_unit_t *tmp_u)
{
    tt_u32_t __n_u_num, q_u_num;
    tt_mpn_unit_t *__n_u, *__q_u;
    tt_mpn_unit_t d_rp, d_v, n_v1, n_v0, q_v, r_v;

    // assume:
    //  - n_u_num is larger than d_u_num
    //  - highest bit of d_u is 1
    //  - q has enough space: q_u_num > n_u_num - d_u_num
    //  - size of tmp_u is larger than d_u_num

    __n_u_num = *n_u_num;
    TT_ASSERT_MPN(__n_u_num > d_u_num);
    TT_ASSERT_MPN((d_u[d_u_num - 1] & __BD_LS(1, TT_MPN_USIZE_BIT - 1)) != 0);

    TT_ASSERT_MPN(__n_u_num > d_u_num);
    d_v = d_u[d_u_num - 1];

    __n_u = n_u + __n_u_num - d_u_num;
    __q_u = q_u + __n_u_num - d_u_num;

    d_rp = __mpn_reciprocal_v(d_u[d_u_num - 1]);

    // first division is special
    if (__kmpn_cmp(__n_u, d_u, d_u_num) >= 0) {
        __kmpn_sub_n(__n_u, __n_u, d_u_num, d_u, 0);
        *__q_u = 1;
        q_u_num = __n_u_num - d_u_num + 1;
    } else {
        *__q_u = 0;
        q_u_num = __n_u_num - d_u_num;
    }
    --__n_u;
    --__q_u;

    while (__q_u >= q_u) {
        // now __n_u has d_u_num + 1 units
        n_v1 = __n_u[d_u_num];

        // todo: ignore if n is 0

        TT_ASSERT_MPN(__n_u >= n_u);
        TT_ASSERT_MPN(n_v1 <= d_v);
        if (n_v1 == d_v) {
            q_v = ~0;
        } else {
            n_v0 = __n_u[d_u_num - 1];
            __mpn_div2by1(q_v, r_v, n_v1, n_v0, d_v, d_rp);
        }

        tmp_u[d_u_num] = __kmpn_mul_1(tmp_u, d_u, d_u_num, q_v);
        if (__kmpn_cmp(tmp_u, __n_u, d_u_num + 1) > 0) {
            tmp_u[d_u_num] -= __kmpn_sub_n(tmp_u, tmp_u, d_u_num, d_u, 0);
            q_v -= 1;
        }
        if (__kmpn_cmp(tmp_u, __n_u, d_u_num + 1) > 0) {
            tmp_u[d_u_num] -= __kmpn_sub_n(tmp_u, tmp_u, d_u_num, d_u, 0);
            q_v -= 1;
        }
        TT_ASSERT_MPN(__kmpn_cmp(tmp_u, __n_u, d_u_num + 1) <= 0);

        *__q_u = q_v;
        --__q_u;

        __kmpn_sub_n(__n_u, __n_u, d_u_num + 1, tmp_u, 0);
        --__n_u;
    }
    TT_ASSERT_MPN(__n_u + 1 == n_u);
    TT_ASSERT_MPN(__q_u + 1 == q_u);

    return q_u_num;
}
