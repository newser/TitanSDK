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

#include <math/mp/tt_mpn_exp.h>

#include <math/mp/tt_mpn_add.h>
#include <math/mp/tt_mpn_common.h>
#include <math/mp/tt_mpn_gcd.h>
#include <math/mp/tt_mpn_mod.h>
#include <math/mp/tt_mpn_sub.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

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
extern tt_mpn_unit_t __kmpn_addmul_1(IN tt_mpn_unit_t *ret_u,
                                     IN tt_mpn_unit_t *a_u,
                                     IN tt_u32_t a_u_num,
                                     IN tt_mpn_unit_t b_v);
extern tt_mpn_unit_t __kmpn_add_1(IN tt_mpn_unit_t *ret_u,
                                  IN tt_mpn_unit_t *a_u,
                                  IN tt_u32_t a_u_num,
                                  IN tt_mpn_unit_t b_v,
                                  IN tt_mpn_unit_t carry);

extern tt_u32_t __mpn_add(IN tt_mpn_unit_t *ret_u,
                          IN tt_mpn_unit_t *a_u,
                          IN tt_u32_t a_u_num,
                          IN tt_mpn_unit_t *b_u,
                          IN tt_u32_t b_u_num);

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_result_t __mpn_rdmont(OUT tt_mpn_t *ret,
                         IN tt_mpn_t *t,
                         IN tt_mpn_t *m,
                         IN tt_mpn_unit_t m1,
                         IN tt_mpn_cache_t *mpnc);

// - a*b*r^-1 mod m, 0 <= a,b < m
tt_result_t __mpn_mulmont(OUT tt_mpn_t *ret,
                          IN tt_mpn_t *a,
                          IN tt_mpn_t *b,
                          IN tt_mpn_t *m,
                          IN tt_mpn_unit_t m1,
                          IN tt_mpn_cache_t *mpnc);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_mpn_powmodmont(OUT tt_mpn_t *ret,
                              IN tt_mpn_t *a,
                              IN tt_mpn_t *e,
                              IN tt_mpn_t *m,
                              IN OPT tt_mpn_montconst_t *mcn,
                              IN tt_mpn_cache_t *mpnc)
{
#define __w_bitnum 5
#define __w_num (1 << __w_bitnum)
#define __w_num_half (1 << (__w_bitnum - 1))

    tt_result_t result = TT_FAIL;
    tt_mpn_montconst_t __mcn;
    tt_mpn_unit_t m1, u;
    tt_mpn_t *w[__w_num], *tmp = NULL;
    tt_u32_t i, k, w_i;

    tt_memset(w, 0, sizeof(w));

    // ========================================
    // param check
    // ========================================

    tt_mpn_normalize(a);
    if (a->negative) {
        return TT_FAIL;
    }
    if (tt_mpn_zero(a)) {
        tt_mpn_clear(ret);
        return TT_SUCCESS;
    }

    tt_mpn_normalize(e);
    if (e->negative) {
        return TT_FAIL;
    }

    tt_mpn_normalize(m);
    if (m->negative) {
        return TT_FAIL;
    }

    i = tt_mpn_cmp(a, m);
    if (((tt_s32_t)i) > 0) {
        // todo: a > m
        return TT_FAIL;
    } else if (((tt_s32_t)i) == 0) {
        tt_mpn_clear(ret);
        return TT_SUCCESS;
    }

    // ========================================
    // calculation
    // ========================================

    if (mcn == NULL) {
        if (!TT_OK(tt_mpn_montconst_create(&__mcn, m, mpnc))) {
            return TT_FAIL;
        }
        mcn = &__mcn;
    }
    m1 = mcn->m1;
    tt_mpn_normalize(mcn->r2mm);
    tt_mpn_normalize(mcn->rmm);

    // pre compute w
    TT_DO_G(mem_out, __mpn_mulmont(ret, a, mcn->r2mm, m, m1, mpnc));
    tt_mpn_normalize(ret);

    TT_DONN_G(mem_out, w[1] = tt_mpn_alloc(mpnc, 0));
    TT_DO_G(mem_out, tt_mpn_copy(w[1], ret, 0));
    TT_DONN_G(mem_out, tmp = tt_mpn_alloc(mpnc, 0));
    for (i = 2; i < __w_num; ++i) {
        TT_DONN_G(mem_out, w[i] = tt_mpn_alloc(mpnc, 0));
        TT_DO_G(mem_out, __mpn_mulmont(w[i], w[i - 1], ret, m, m1, mpnc));
        tt_mpn_normalize(w[i]);
    }

    // from e's highest bit to 0th bit
    u = e->unit[e->unit_num - 1];
    i = __kmpn_h0bitnum(u);
    i = TT_MPN_USIZE_BIT - 2 - i;
    TT_ASSERT_MPN((i < TT_MPN_USIZE_BIT) || (i == ~0));
    w_i = 0;

    for (; i != ~0; --i) {
        TT_DO_G(mem_out, __mpn_mulmont(tmp, ret, ret, m, m1, mpnc));
        tt_mpn_swap(ret, tmp);
        tt_mpn_normalize(ret);

        if ((u & (((tt_mpn_unit_t)1) << i)) != 0) {
            w_i <<= 1;
            w_i |= 1;
        } else if (w_i != 0) {
            w_i <<= 1;
        } else {
            continue;
        }

        if (w_i >= __w_num_half) {
            TT_DO_G(mem_out, __mpn_mulmont(tmp, ret, w[w_i], m, m1, mpnc));
            tt_mpn_swap(ret, tmp);
            tt_mpn_normalize(ret);

            w_i = 0;
        }
    }

    for (k = e->unit_num - 2; k != ~0; --k) {
        u = e->unit[k];
        for (i = TT_MPN_USIZE_BIT - 1; i != ~0; --i) {
            TT_DO_G(mem_out, __mpn_mulmont(tmp, ret, ret, m, m1, mpnc));
            tt_mpn_swap(ret, tmp);
            tt_mpn_normalize(ret);

            if ((u & (((tt_mpn_unit_t)1) << i)) != 0) {
                w_i <<= 1;
                w_i |= 1;
            } else if (w_i != 0) {
                w_i <<= 1;
            } else {
                continue;
            }

            if (w_i >= __w_num_half) {
                TT_DO_G(mem_out, __mpn_mulmont(tmp, ret, w[w_i], m, m1, mpnc));
                tt_mpn_swap(ret, tmp);
                tt_mpn_normalize(ret);

                w_i = 0;
            }
        }
    }

    if (w_i != 0) {
        TT_DO_G(mem_out, __mpn_mulmont(tmp, ret, w[w_i], m, m1, mpnc));
        tt_mpn_swap(ret, tmp);
        tt_mpn_normalize(ret);
    }

    // ret = mont(ret, 1)
    tt_mpn_set_u(w[1], 1, TT_FALSE, 0);
    TT_DO_G(mem_out, __mpn_mulmont(tmp, ret, w[1], m, m1, mpnc));
    tt_mpn_swap(ret, tmp);

    result = TT_SUCCESS;

mem_out:

    tt_mpn_free(mpnc, tmp);

    for (i = 0; i < __w_num; ++i) {
        tt_mpn_free(mpnc, w[i]);
    }

    if (mcn == &__mcn) {
        tt_mpn_montconst_destroy(&__mcn, mpnc);
    }

    return result;
}

tt_result_t tt_mpn_mont_m1(OUT tt_mpn_unit_t *mont_m1,
                           IN tt_mpn_t *m,
                           IN tt_mpn_cache_t *mpnc)
{
    tt_mpn_t *m1 = NULL, *r = NULL;
    tt_result_t result = TT_FAIL;

    // mont_m1 = -(m^-1 mod r)

    if (m->negative) {
        // negative modular is not allowed
        return TT_FAIL;
    }

    TT_DONN_G(mm1_out, m1 = tt_mpn_alloc(mpnc, 0));

    TT_DONN_G(mm1_out, r = tt_mpn_alloc(mpnc, 0));
    TT_DO_G(mm1_out, tt_mpn_ulshf_u(r, 1, TT_FALSE, 1));

    // m^-1
    TT_DO_G(mm1_out, tt_mpn_modminv(m1, m, r, mpnc));

    // -m^-1
    TT_DO_G(mm1_out, tt_mpn_sub(m1, r, m1));
    *mont_m1 = m1->unit[0];

    result = TT_SUCCESS;

mm1_out:

    tt_mpn_free(mpnc, m1);
    tt_mpn_free(mpnc, r);

    return result;
}

// m1 = - (m^-1 mod r)
tt_result_t __mpn_rdmont(OUT tt_mpn_t *ret,
                         IN tt_mpn_t *t,
                         IN tt_mpn_t *m,
                         IN tt_mpn_unit_t m1,
                         IN tt_mpn_cache_t *mpnc)
{
    tt_u32_t n, n2, i;
    tt_mpn_unit_t ui, cy;

    // assumes t and m have been normalized
    // tt_mpn_normalize(t);
    // tt_mpn_normalize(m);
    n = m->unit_num;
    n2 = n << 1;

    // be sure t < m*r^n
    TT_ASSERT_MPN((t->unit_num < n2) ||
                  ((t->unit_num == n2) &&
                   (t->unit[t->unit_num - 1] < m->unit[m->unit_num - 1])));
    // there may be a carry when calculating ret
    n2 += 1;

    // ret = t
    TT_DO(tt_mpn_reserve_u(ret, n2));
    __kmpn_copy_u(ret->unit, t->unit, t->unit_num);
    for (i = t->unit_num; i < n2; ++i) {
        ret->unit[i] = 0;
    }
    ret->unit_num = n2;

    // ui = ret[i]*m1 mod r
    // ret += ui*m*r^i
    for (i = 0; i < n; ++i) {
        ui = ret->unit[i] * m1;
        cy = __kmpn_addmul_1(&ret->unit[i], m->unit, n, ui);
        TT_ASSERT_MPN(ret->unit[i] == 0);
        cy = __kmpn_add_1(&ret->unit[n + i],
                          &ret->unit[n + i],
                          n2 - n - i,
                          cy,
                          0);
        TT_ASSERT_MPN(cy == 0);
    }

    // ret = ret/r^n
    tt_mpn_rshf_u(NULL, ret, n);

    // if ret >= m, then ret -= m
    if (tt_mpn_cmp(ret, m) >= 0) {
        tt_mpn_subeq(ret, m);
    }

    return TT_SUCCESS;
}

// - a*b*r^-1 mod m, 0 <= a,b < m
tt_result_t __mpn_mulmont(OUT tt_mpn_t *ret,
                          IN tt_mpn_t *a,
                          IN tt_mpn_t *b,
                          IN tt_mpn_t *m,
                          IN tt_mpn_unit_t m1,
                          IN tt_mpn_cache_t *mpnc)
{
    tt_u32_t a_un, b_un, n, n2, i;
    tt_mpn_unit_t ai, b0, ui, cy, *ret_u;

    if (a->unit_num > b->unit_num) {
        TT_SWAP(tt_mpn_t *, a, b);
    }
    a_un = a->unit_num;
    b_un = b->unit_num;
    b0 = b->unit[0];
    n = m->unit_num;
    n2 = (n << 1) + 1;

    // be sure 0 <= a,b < m
    TT_ASSERT_MPN(!a->negative && (tt_mpn_cmp(a, m) < 0));
    TT_ASSERT_MPN(!b->negative && (tt_mpn_cmp(b, m) < 0));

    // ret = 0
    TT_DO(tt_mpn_reserve_u(ret, n2));
    ret_u = ret->unit;
    for (i = 0; i < n2; ++i) {
        ret_u[i] = 0;
    }
    ret->unit_num = n2;
    ret->negative = TT_FALSE;

    // ui = (ret[i]+ai*b0) mod r
    // ret += (ai*b + ui*m)*r^i
    ai = a->unit[0];
    ui = ai * b0 * m1;
    cy = __kmpn_mul_1(ret_u, b->unit, b_un, ai);
    cy = __kmpn_add_1(ret_u + b_un, ret_u + b_un, n2 - b_un, cy, 0);
    TT_ASSERT_MPN(cy == 0);
    cy = __kmpn_addmul_1(ret_u, m->unit, n, ui);
    cy = __kmpn_add_1(ret_u + n, ret_u + n, n2 - n, cy, 0);
    TT_ASSERT_MPN(cy == 0);
    TT_ASSERT_MPN(ret->unit[0] == 0);

    for (i = 1; i < a_un; ++i) {
        ai = a->unit[i];
        ui = (ret_u[i] + ai * b0) * m1;
        cy = __kmpn_addmul_1(ret_u + i, b->unit, b_un, ai);
        cy = __kmpn_add_1(ret_u + i + b_un,
                          ret_u + i + b_un,
                          n2 - b_un - i,
                          cy,
                          0);
        TT_ASSERT_MPN(cy == 0);
        cy = __kmpn_addmul_1(ret_u + i, m->unit, n, ui);
        cy = __kmpn_add_1(ret_u + i + n, ret_u + i + n, n2 - n - i, cy, 0);
        TT_ASSERT_MPN(cy == 0);
        TT_ASSERT_MPN(ret->unit[i] == 0);
    }

    for (; i < n; ++i) {
        ui = ret_u[i] * m1;
        cy = __kmpn_addmul_1(ret_u + i, m->unit, n, ui);
        cy = __kmpn_add_1(ret_u + i + n, ret_u + i + n, n2 - n - i, cy, 0);
        TT_ASSERT_MPN(cy == 0);
        TT_ASSERT_MPN(ret->unit[i] == 0);
    }

    // ret = ret/r^n
    tt_mpn_rshf_u(NULL, ret, n);

    // if ret >= m, then ret -= m
    if (tt_mpn_cmp(ret, m) >= 0) {
        tt_mpn_subeq(ret, m);
    }
    TT_ASSERT_MPN(tt_mpn_cmp(ret, m) < 0);

    return TT_SUCCESS;
}

tt_result_t tt_mpn_montconst_create(OUT tt_mpn_montconst_t *mcn,
                                    IN tt_mpn_t *m,
                                    IN tt_mpn_cache_t *mpnc)

{
    tt_mpn_t *r2mm = NULL, *rmm = NULL;

    tt_mpn_normalize(m);

    TT_DO_G(mc_fail, tt_mpn_mont_m1(&mcn->m1, m, mpnc));

    // r = 1^unit_num
    // r^2 mod m
    TT_DONN_G(mc_fail, r2mm = tt_mpn_alloc(mpnc, 0));
    TT_DO_G(mc_fail, tt_mpn_ulshf_u(r2mm, 1, TT_FALSE, m->unit_num << 1));
    TT_DO_G(mc_fail, tt_mpn_modeq(r2mm, m, mpnc));
    mcn->r2mm = r2mm;

    // r mod m
    TT_DONN_G(mc_fail, rmm = tt_mpn_alloc(mpnc, 0));
    TT_DO_G(mc_fail, tt_mpn_ulshf_u(rmm, 1, TT_FALSE, m->unit_num));
    TT_DO_G(mc_fail, tt_mpn_modeq(rmm, m, mpnc));
    mcn->rmm = rmm;

    return TT_SUCCESS;

mc_fail:

    tt_mpn_free(mpnc, r2mm);
    tt_mpn_free(mpnc, rmm);

    return TT_FAIL;
}

void tt_mpn_montconst_destroy(OUT tt_mpn_montconst_t *mcn,
                              IN tt_mpn_cache_t *mpnc)
{
    tt_mpn_free(mpnc, mcn->r2mm);
    tt_mpn_free(mpnc, mcn->rmm);
}
