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

#include <math/mp/tt_mpn_gcd.h>

#include <log/tt_log.h>
#include <math/mp/tt_mpn_add.h>
#include <math/mp/tt_mpn_common.h>
#include <math/mp/tt_mpn_div.h>
#include <math/mp/tt_mpn_mod.h>
#include <math/mp/tt_mpn_mul.h>
#include <math/mp/tt_mpn_sub.h>
#include <misc/tt_assert.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __GCD_THRSH_BASE 3 // to be tuned

#define __hb2(v) (((tt_mpn_unit_t)(v)) >> (TT_MPN_USIZE_BIT - 2))
#define __s2bn(s, bn)                                                          \
    do {                                                                       \
        if ((s) >= 0) {                                                        \
            tt_mpn_set_u((bn), (tt_mpn_unit_t)(s), TT_FALSE, 0);               \
        } else {                                                               \
            tt_mpn_set_u((bn), (tt_mpn_unit_t)(-(s)), TT_TRUE, 0);             \
        }                                                                      \
    } while (0)

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

tt_mpn_unit_t __gcd_u(IN tt_mpn_unit_t u, IN tt_mpn_unit_t v);
tt_mpn_unit_t __extgcd_u(IN tt_mpn_unit_t u,
                         IN tt_mpn_unit_t v,
                         OUT tt_mpn_unit_t *u_cof,
                         OUT tt_mpn_unit_t *v_cof);

tt_result_t __mpn_gcd(OUT tt_mpn_t *divisor,
                      IN tt_mpn_t *u,
                      IN tt_mpn_t *v,
                      IN tt_mpn_cache_t *mpnc);
tt_result_t __mpn_extgcd(OUT tt_mpn_t *divisor,
                         OUT OPT tt_mpn_t *u_cof,
                         OUT OPT tt_mpn_t *v_cof,
                         IN tt_mpn_t *u,
                         IN tt_mpn_t *v,
                         IN tt_mpn_cache_t *mpnc);

tt_result_t __mpn_gcd_base(IN tt_mpn_t *u,
                           IN tt_mpn_t *v,
                           OUT tt_mpn_t *divisor,
                           IN tt_mpn_cache_t *mpnc);
tt_result_t __mpn_extgcd_base(IN tt_mpn_t *u,
                              IN tt_mpn_t *v,
                              OUT tt_mpn_t *divisor,
                              OUT OPT tt_mpn_t *u_cof,
                              OUT OPT tt_mpn_t *v_cof,
                              IN tt_mpn_cache_t *mpnc);

tt_result_t __mpn_gcd_lm(IN tt_mpn_t *u,
                         IN tt_mpn_t *v,
                         OUT tt_mpn_t *divisor,
                         IN tt_mpn_cache_t *mpnc);
tt_result_t __mpn_extgcd_lm(IN tt_mpn_t *u,
                            IN tt_mpn_t *v,
                            OUT tt_mpn_t *divisor,
                            OUT OPT tt_mpn_t *u_cof,
                            OUT OPT tt_mpn_t *v_cof,
                            IN tt_mpn_cache_t *mpnc);

tt_bool_t __verify_extgcd(IN tt_mpn_t *u,
                          IN tt_mpn_t *v,
                          IN tt_mpn_t *divisor,
                          IN tt_mpn_t *u_cof,
                          IN tt_mpn_t *v_cof,
                          IN tt_mpn_cache_t *mpnc);

static tt_result_t __lm_core_1(IN tt_mpn_t *u,
                               IN tt_mpn_t *v,
                               OUT tt_mpn_t *ma,
                               OUT tt_mpn_t *mb,
                               OUT tt_mpn_t *mc,
                               OUT tt_mpn_t *md);
static tt_result_t __lm_core_n(IN tt_mpn_t *u,
                               IN tt_mpn_t *v,
                               IN tt_u32_t u_num,
                               OUT tt_mpn_t *ma,
                               OUT tt_mpn_t *mb,
                               OUT tt_mpn_t *mc,
                               OUT tt_mpn_t *md,
                               IN tt_mpn_cache_t *mpnc);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_mpn_gcd(OUT tt_mpn_t *divisor,
                       IN tt_mpn_t *u,
                       IN tt_mpn_t *v,
                       IN tt_mpn_cache_t *mpnc)
{
    tt_mpn_t *d = NULL, *uc = NULL, *vc = NULL;
    tt_result_t result = TT_FAIL;

    if ((divisor == u) || (divisor == v)) {
        TT_DONN_G(eg_out, d = tt_mpn_alloc(mpnc, 0));
    } else {
        d = divisor;
    }

    TT_DO_G(eg_out, __mpn_gcd(d, u, v, mpnc));

    if (d != divisor) {
        tt_mpn_swap(d, divisor);
    }

    result = TT_SUCCESS;

eg_out:

    if (d != divisor) {
        tt_mpn_free(mpnc, d);
    }

    return result;
}

tt_result_t __mpn_gcd(OUT tt_mpn_t *divisor,
                      IN tt_mpn_t *u,
                      IN tt_mpn_t *v,
                      IN tt_mpn_cache_t *mpnc)
{
    tt_bool_t u_neg, v_neg;
    tt_result_t result = TT_FALSE;
    tt_s32_t cmp_ret;

    u_neg = u->negative;
    u->negative = TT_FALSE;
    v_neg = v->negative;
    v->negative = TT_FALSE;

    cmp_ret = tt_mpn_cmp(u, v);
    if (cmp_ret == 0) {
        TT_DO(tt_mpn_copy(divisor, u, 0));
        divisor->negative = TT_FALSE;

        result = TT_SUCCESS;
        goto gcd_out;
    } else if (cmp_ret < 0) {
        TT_SWAP(tt_mpn_t *, u, v);
        TT_SWAP(tt_bool_t, u_neg, v_neg);
    }

    // todo: remove ending 0s by lshift, and do rshift
    // on result

    // ========================================
    // gcd start

    if (v->unit_num <= __GCD_THRSH_BASE) {
        TT_DO_G(gcd_out, __mpn_gcd_base(u, v, divisor, mpnc));
    } else {
        // todo:
        //  - use half GCD(HGCD) algorithm when v->unit_num is too large
        //  - implement __lm_core_2, by simd instructions, which could
        //    operate on 128bit integer

        TT_DO_G(gcd_out, __mpn_gcd_lm(u, v, divisor, mpnc));
    }
    divisor->negative = TT_FALSE;

    // gcd end
    // ========================================

    result = TT_SUCCESS;

gcd_out:

    u->negative = u_neg;
    v->negative = v_neg;

    return result;
}

tt_result_t tt_mpn_extgcd(OUT tt_mpn_t *divisor,
                          OUT OPT tt_mpn_t *u_cof,
                          OUT OPT tt_mpn_t *v_cof,
                          IN tt_mpn_t *u,
                          IN tt_mpn_t *v,
                          IN tt_mpn_cache_t *mpnc)
{
    tt_mpn_t *d = NULL, *uc = NULL, *vc = NULL;
    tt_result_t result = TT_FAIL;

    if ((divisor == u_cof) || (divisor == v_cof) ||
        ((u_cof == v_cof) && (u_cof != NULL))) {
        TT_ERROR("divisor, u_cof and v_cof must be different ptr");
        return TT_FAIL;
    }

    if ((divisor == u) || (divisor == v)) {
        TT_DONN_G(eg_out, d = tt_mpn_alloc(mpnc, 0));
    } else {
        d = divisor;
    }

    if ((u_cof != NULL) && ((u_cof == u) || (u_cof == v))) {
        TT_DONN_G(eg_out, uc = tt_mpn_alloc(mpnc, 0));
    } else {
        uc = u_cof;
    }

    if ((v_cof != NULL) && ((v_cof == u) || (v_cof == v))) {
        TT_DONN_G(eg_out, vc = tt_mpn_alloc(mpnc, 0));
    } else {
        vc = v_cof;
    }

    TT_DO_G(eg_out, __mpn_extgcd(d, uc, vc, u, v, mpnc));

    if (d != divisor) {
        tt_mpn_swap(d, divisor);
    }

    if (uc != u_cof) {
        tt_mpn_swap(uc, u_cof);
    }

    if (vc != v_cof) {
        tt_mpn_swap(vc, v_cof);
    }

    result = TT_SUCCESS;

eg_out:

    if (d != divisor) {
        tt_mpn_free(mpnc, d);
    }

    if (uc != u_cof) {
        tt_mpn_free(mpnc, uc);
    }

    if (vc != v_cof) {
        tt_mpn_free(mpnc, vc);
    }

    return result;
}

tt_result_t __mpn_extgcd(OUT tt_mpn_t *divisor,
                         OUT OPT tt_mpn_t *u_cof,
                         OUT OPT tt_mpn_t *v_cof,
                         IN tt_mpn_t *u,
                         IN tt_mpn_t *v,
                         IN tt_mpn_cache_t *mpnc)
{
    tt_bool_t u_neg, v_neg;
    tt_result_t result = TT_FALSE;
    tt_s32_t cmp_ret;

    u_neg = u->negative;
    u->negative = TT_FALSE;
    v_neg = v->negative;
    v->negative = TT_FALSE;

    cmp_ret = tt_mpn_cmp(u, v);
    if (cmp_ret == 0) {
        TT_DO(tt_mpn_copy(divisor, u, 0));
        divisor->negative = TT_FALSE;

        if (u_cof != NULL) {
            tt_mpn_set_u(u_cof, 1, TT_FALSE, 0);
        }

        if (v_cof != NULL) {
            tt_mpn_set_u(v_cof, 0, TT_FALSE, 0);
        }

        result = TT_SUCCESS;
        goto eg_out;
    } else if (cmp_ret < 0) {
        TT_SWAP(tt_mpn_t *, u, v);
        TT_SWAP(tt_bool_t, u_neg, v_neg);
        TT_SWAP(tt_mpn_t *, u_cof, v_cof);
    }

    // todo: remove ending 0s by lshift, and do rshift
    // on result

    // ========================================
    // gcd start

    if (v->unit_num <= __GCD_THRSH_BASE) {
        TT_DO_G(eg_out, __mpn_extgcd_base(u, v, divisor, u_cof, v_cof, mpnc));
    } else {
        // todo:
        //  - use half GCD(HGCD) algorithm when v->unit_num is too large
        //  - implement __lm_core_2, by simd instructions, which could
        //    operate on 128bit integer

        TT_DO_G(eg_out, __mpn_extgcd_lm(u, v, divisor, u_cof, v_cof, mpnc));
    }
    divisor->negative = TT_FALSE;

    // gcd end
    // ========================================

    result = TT_SUCCESS;

    if ((u_cof != NULL) && u_neg) {
        u_cof->negative = !u_cof->negative;
    }

    if ((v_cof != NULL) && v_neg) {
        v_cof->negative = !v_cof->negative;
    }

eg_out:

    u->negative = u_neg;
    v->negative = v_neg;

    return result;
}

tt_result_t tt_mpn_modminv(OUT tt_mpn_t *ret,
                           IN tt_mpn_t *a,
                           IN tt_mpn_t *m,
                           IN tt_mpn_cache_t *mpnc)
{
    tt_result_t result = TT_FAIL;
    tt_mpn_t *d = NULL;

    if ((tt_mpn_cmp_u(a, 0, TT_FALSE) <= 0) ||
        (tt_mpn_cmp_u(m, 1, TT_FALSE) <= 0)) {
        return TT_FAIL;
    }

    TT_DONN(d = tt_mpn_alloc(mpnc, 0));

    TT_DO_G(mm_out, tt_mpn_extgcd(d, ret, NULL, a, m, mpnc));

    if (tt_mpn_cmp_u(d, 1, TT_FALSE) != 0) {
        // TT_ERROR("mod mul inv does not exist");
        goto mm_out;
    }

    if (ret->negative) {
        TT_DO_G(mm_out, tt_mpn_addeq(ret, m));
    }

    result = TT_SUCCESS;

mm_out:

    tt_mpn_free(mpnc, d);

    return result;
}

tt_mpn_unit_t __gcd_u(IN tt_mpn_unit_t u, IN tt_mpn_unit_t v)
{
    while (v != 0) {
        tt_mpn_unit_t r = u % v;
        u = v;
        v = r;
    }
    return u;
}

tt_mpn_unit_t __extgcd_u(IN tt_mpn_unit_t u,
                         IN tt_mpn_unit_t v,
                         OUT tt_mpn_unit_t *u_cof,
                         OUT tt_mpn_unit_t *v_cof)
{
    tt_mpn_unit_t u1 = 1, u2 = 0, u3 = u;
    tt_mpn_unit_t v1 = 0, v2 = 1, v3 = v;
    tt_mpn_unit_t t1, t2, t3;

    // u*t1 + v*t2 = t3
    // u*u_cof + v*u2 = u3
    // u*v_cof + v*v2 = v3
    while (v3 != 0) {
        tt_mpn_unit_t q = u3 / v3;

        t1 = u1 - q * v1;
        t2 = u2 - q * v2;
        t3 = u3 - q * v3;

        u1 = v1;
        u2 = v2;
        u3 = v3;

        v1 = t1;
        v2 = t2;
        v3 = t3;
    }

    *u_cof = u1;
    *v_cof = u2;
    return u3;
}

tt_result_t __mpn_gcd_base(IN tt_mpn_t *u,
                           IN tt_mpn_t *v,
                           OUT tt_mpn_t *divisor,
                           IN tt_mpn_cache_t *mpnc)
{
    tt_mpn_t *u3 = NULL, *v3 = NULL, *r = NULL;
    tt_result_t result = TT_FAIL;

    TT_DONN_G(beb_out, u3 = tt_mpn_alloc(mpnc, 0));
    TT_DONN_G(beb_out, v3 = tt_mpn_alloc(mpnc, 0));
    TT_DONN_G(beb_out, r = tt_mpn_alloc(mpnc, 0));

    TT_DO_G(beb_out, tt_mpn_copy(u3, u, 0));
    TT_DO_G(beb_out, tt_mpn_copy(v3, v, 0));

    while (!tt_mpn_zero(v3)) {
        TT_DO_G(beb_out, tt_mpn_mod(r, u3, v3, mpnc));
        TT_DO_G(beb_out, tt_mpn_copy(u3, v3, 0));
        TT_DO_G(beb_out, tt_mpn_copy(v3, r, 0));
    }

    tt_mpn_swap(divisor, u3);

    result = TT_SUCCESS;

beb_out:

    tt_mpn_free(mpnc, u3);
    tt_mpn_free(mpnc, v3);
    tt_mpn_free(mpnc, r);

    return result;
}

tt_result_t __mpn_extgcd_base(IN tt_mpn_t *u,
                              IN tt_mpn_t *v,
                              OUT tt_mpn_t *divisor,
                              OUT OPT tt_mpn_t *u_cof,
                              OUT OPT tt_mpn_t *v_cof,
                              IN tt_mpn_cache_t *mpnc)
{
    tt_mpn_t *u1 = NULL, *u2 = NULL, *u3 = NULL;
    tt_mpn_t *t1 = NULL, *t2 = NULL, *t3 = NULL;
    tt_mpn_t *v1 = NULL, *v2 = NULL, *v3 = NULL, *q = NULL;
    tt_result_t result = TT_FAIL;

    TT_DONN_G(beb_out, u1 = tt_mpn_alloc(mpnc, 0));
    TT_DONN_G(beb_out, u2 = tt_mpn_alloc(mpnc, 0));
    TT_DONN_G(beb_out, u3 = tt_mpn_alloc(mpnc, 0));
    TT_DONN_G(beb_out, v1 = tt_mpn_alloc(mpnc, 0));
    TT_DONN_G(beb_out, v2 = tt_mpn_alloc(mpnc, 0));
    TT_DONN_G(beb_out, v3 = tt_mpn_alloc(mpnc, 0));
    TT_DONN_G(beb_out, t1 = tt_mpn_alloc(mpnc, 0));
    TT_DONN_G(beb_out, t2 = tt_mpn_alloc(mpnc, 0));
    TT_DONN_G(beb_out, t3 = tt_mpn_alloc(mpnc, 0));
    TT_DONN_G(beb_out, q = tt_mpn_alloc(mpnc, 0));

    tt_mpn_set_u(u1, 1, TT_FALSE, 0);
    tt_mpn_set_u(u2, 0, TT_FALSE, 0);
    TT_DO_G(beb_out, tt_mpn_copy(u3, u, 0));
    tt_mpn_set_u(v1, 0, TT_FALSE, 0);
    tt_mpn_set_u(v2, 1, TT_FALSE, 0);
    TT_DO_G(beb_out, tt_mpn_copy(v3, v, 0));

    // u*t1 + v*t2 = t3
    // u*u1(u_cof) + v*u2(v_cof) = u3
    // u*v1 + v*v2 = v3
    while (!tt_mpn_zero(v3)) {
        TT_DO_G(beb_out, tt_mpn_div(q, NULL, u3, v3, mpnc));

        // t1 = u1 - q * v1;
        TT_DO_G(beb_out, tt_mpn_mul(t1, q, v1, mpnc));
        TT_DO_G(beb_out, tt_mpn_sub(t1, u1, t1));
        // t2 = u2 - q * v2;
        TT_DO_G(beb_out, tt_mpn_mul(t2, q, v2, mpnc));
        TT_DO_G(beb_out, tt_mpn_sub(t2, u2, t2));
        // t3 = u3 - q * v3;
        TT_DO_G(beb_out, tt_mpn_mul(t3, q, v3, mpnc));
        TT_DO_G(beb_out, tt_mpn_sub(t3, u3, t3));

        // u1 = v1;
        tt_mpn_swap(u1, v1);
        // u2 = v2;
        tt_mpn_swap(u2, v2);
        // u3 = v3;
        tt_mpn_swap(u3, v3);

        // v1 = t1;
        tt_mpn_swap(v1, t1);
        // v2 = t2;
        tt_mpn_swap(v2, t2);
        // v3 = t3;
        tt_mpn_swap(v3, t3);

        // TT_ASSERT_MPN(__verify_extgcd(u, v, u3, u1, u2, mpnc));
    }

    tt_mpn_swap(divisor, u3);

    if (u_cof != NULL) {
        tt_mpn_swap(u_cof, u1);
    }

    if (v_cof != NULL) {
        tt_mpn_swap(v_cof, u2);
    }

    // TT_ASSERT_MPN(__verify_extgcd(u, v, &u3, &u1, &u2));

    result = TT_SUCCESS;

beb_out:

    tt_mpn_free(mpnc, u1);
    tt_mpn_free(mpnc, u2);
    tt_mpn_free(mpnc, u3);
    tt_mpn_free(mpnc, v1);
    tt_mpn_free(mpnc, v2);
    tt_mpn_free(mpnc, v3);
    tt_mpn_free(mpnc, t1);
    tt_mpn_free(mpnc, t2);
    tt_mpn_free(mpnc, t3);
    tt_mpn_free(mpnc, q);

    return result;
}

tt_result_t __lm_core_1(IN tt_mpn_t *u,
                        IN tt_mpn_t *v,
                        OUT tt_mpn_t *ma,
                        OUT tt_mpn_t *mb,
                        OUT tt_mpn_t *mc,
                        OUT tt_mpn_t *md)
{
    tt_mpn_sunit_t u1, v1, a, b, c, d;
    tt_mpn_sunit_t u1a, u1b, v1c, v1d;
    tt_mpn_sunit_t q, q1, r;

    TT_ASSERT_MPN(u->unit_num >= v->unit_num);
    if (u->unit_num != v->unit_num) {
        return TT_PROCEEDING;
    }

    // reserve highest two bits:
    //  - store sign
    //  - prevent from overflow
    u1 = (tt_mpn_sunit_t)(u->unit[u->unit_num - 1]);
    v1 = (tt_mpn_sunit_t)(v->unit[v->unit_num - 1]);
    if ((__hb2(u1) != 0) || (__hb2(v1) != 0)) {
        return TT_PROCEEDING;
    }
    TT_ASSERT_MPN(u1 >= v1);

    a = 1;
    b = 0;
    c = 0;
    d = 1;

    // won't overflow?
    do {
        u1a = u1 + a;
        u1b = u1 + b;
        v1c = v1 + c;
        v1d = v1 + d;
        if ((v1c == 0) || (v1d == 0)) {
            break;
        }

        q = u1a / v1c;
        q1 = u1b / v1d;
        if (q != q1) {
            break;
        }

        r = a - q * c;
        a = c;
        c = r;

        r = b - q * d;
        b = d;
        d = r;

        r = u1 - q * v1;
        u1 = v1;
        v1 = r;
    } while (1);

    if (b == 0) {
        return TT_PROCEEDING;
    }

    __s2bn(a, ma);
    __s2bn(b, mb);
    __s2bn(c, mc);
    __s2bn(d, md);

    return TT_SUCCESS;
}

// __lm_core_n seems of too much constant overhead, slower
// than __lm_core_1
tt_result_t __lm_core_n(IN tt_mpn_t *u,
                        IN tt_mpn_t *v,
                        IN tt_u32_t u_num,
                        OUT tt_mpn_t *ma,
                        OUT tt_mpn_t *mb,
                        OUT tt_mpn_t *mc,
                        OUT tt_mpn_t *md,
                        IN tt_mpn_cache_t *mpnc)
{
    tt_mpn_t u1, v1;
    tt_mpn_t u1a, u1b, v1c, v1d;
    tt_mpn_t q, q1, r;
    tt_u32_t shift_num;

    tt_result_t result = TT_FAIL;

    TT_ASSERT_MPN(u->unit_num >= v->unit_num);
    shift_num = u->unit_num - v->unit_num;
    if (shift_num >= u_num) {
        return TT_PROCEEDING;
    }
    shift_num = u->unit_num - u_num;

    tt_mpn_init(&u1);
    tt_mpn_init(&v1);
    tt_mpn_init(&u1a);
    tt_mpn_init(&u1b);
    tt_mpn_init(&v1c);
    tt_mpn_init(&v1d);
    tt_mpn_init(&q);
    tt_mpn_init(&q1);
    tt_mpn_init(&r);

    TT_DO_G(lcd_out, tt_mpn_rshf_u(&u1, u, shift_num));
    TT_DO_G(lcd_out, tt_mpn_rshf_u(&v1, v, shift_num));

    tt_mpn_set_u(ma, 1, TT_FALSE, 0);
    tt_mpn_set_u(mb, 0, TT_FALSE, 0);
    tt_mpn_set_u(mc, 0, TT_FALSE, 0);
    tt_mpn_set_u(md, 1, TT_FALSE, 0);

    do {
        TT_DO_G(lcd_out, tt_mpn_add(&u1a, &u1, ma));
        TT_DO_G(lcd_out, tt_mpn_add(&u1b, &u1, mb));
        TT_DO_G(lcd_out, tt_mpn_add(&v1c, &v1, mc));
        TT_DO_G(lcd_out, tt_mpn_add(&v1d, &v1, md));
        if (tt_mpn_zero(&v1c) || tt_mpn_zero(&v1d)) {
            break;
        }

        TT_DO_G(lcd_out, tt_mpn_div(&q, NULL, &u1a, &v1c, mpnc));
        TT_DO_G(lcd_out, tt_mpn_div(&q1, NULL, &u1b, &v1d, mpnc));
        if (tt_mpn_cmp(&q, &q1) != 0) {
            break;
        }

        TT_DO_G(lcd_out, tt_mpn_mul(&r, &q, mc, mpnc));
        TT_DO_G(lcd_out, tt_mpn_sub(&r, ma, &r));
        tt_mpn_swap(ma, mc);
        tt_mpn_swap(mc, &r);

        TT_DO_G(lcd_out, tt_mpn_mul(&r, &q, md, mpnc));
        TT_DO_G(lcd_out, tt_mpn_sub(&r, mb, &r));
        tt_mpn_swap(mb, md);
        tt_mpn_swap(md, &r);

        TT_DO_G(lcd_out, tt_mpn_mul(&r, &q, &v1, mpnc));
        TT_DO_G(lcd_out, tt_mpn_sub(&r, &u1, &r));
        tt_mpn_swap(&u1, &v1);
        tt_mpn_swap(&v1, &r);
    } while (1);

    if (tt_mpn_zero(mb)) {
        result = TT_PROCEEDING;
        goto lcd_out;
    }

    result = TT_SUCCESS;

lcd_out:

    tt_mpn_destroy(&u1);
    tt_mpn_destroy(&v1);
    tt_mpn_destroy(&u1a);
    tt_mpn_destroy(&u1b);
    tt_mpn_destroy(&v1c);
    tt_mpn_destroy(&v1d);
    tt_mpn_destroy(&q);
    tt_mpn_destroy(&q1);
    tt_mpn_destroy(&r);

    return result;
}

tt_result_t __mpn_gcd_lm(IN tt_mpn_t *u,
                         IN tt_mpn_t *v,
                         OUT tt_mpn_t *divisor,
                         IN tt_mpn_cache_t *mpnc)
{
    tt_mpn_t *a = NULL, *b = NULL;
    tt_mpn_t *t1 = NULL, *t2 = NULL, *t3 = NULL;
    tt_mpn_t *ma = NULL, *mb = NULL, *mc = NULL, *md = NULL;

    tt_result_t result = TT_FAIL;

    TT_DONN_G(bel_out, a = tt_mpn_alloc(mpnc, 0));
    TT_DONN_G(bel_out, b = tt_mpn_alloc(mpnc, 0));
    TT_DONN_G(bel_out, t1 = tt_mpn_alloc(mpnc, 0));
    TT_DONN_G(bel_out, t2 = tt_mpn_alloc(mpnc, 0));
    TT_DONN_G(bel_out, t3 = tt_mpn_alloc(mpnc, 0));
    TT_DONN_G(bel_out, ma = tt_mpn_alloc(mpnc, 0));
    TT_DONN_G(bel_out, mb = tt_mpn_alloc(mpnc, 0));
    TT_DONN_G(bel_out, mc = tt_mpn_alloc(mpnc, 0));
    TT_DONN_G(bel_out, md = tt_mpn_alloc(mpnc, 0));

    TT_DO_G(bel_out, tt_mpn_copy(a, u, 0));
    TT_DO_G(bel_out, tt_mpn_copy(b, v, 0));

    while (b->unit_num > __GCD_THRSH_BASE) {
        tt_result_t rt;

        rt = __lm_core_1(a, b, ma, mb, mc, md);
        if (TT_OK(rt)) {
            // a = ma*a + mb*b
            // b = mc*a + md*b
            TT_DO_G(bel_out, tt_mpn_mul(t1, a, ma, mpnc));
            TT_DO_G(bel_out, tt_mpn_mul(t2, b, mb, mpnc));
            TT_DO_G(bel_out, tt_mpn_addeq(t1, t2));

            TT_DO_G(bel_out, tt_mpn_mul(t3, a, mc, mpnc));
            TT_DO_G(bel_out, tt_mpn_mul(t2, b, md, mpnc));
            TT_DO_G(bel_out, tt_mpn_addeq(t3, t2));

            tt_mpn_swap(a, t1);
            tt_mpn_swap(b, t3);
        } else if (rt == TT_PROCEEDING) {
            TT_DO_G(bel_out, tt_mpn_div(t1, NULL, a, b, mpnc));

            // a = b
            // b = a - q*b
            TT_DO_G(bel_out, tt_mpn_mul(t2, t1, b, mpnc));
            TT_DO_G(bel_out, tt_mpn_sub(t2, a, t2));
            tt_mpn_swap(a, b);
            tt_mpn_swap(b, t2);
        } else {
            goto bel_out;
        }

        // unit num of a and b would be checked in next loop,
        // so need normalize them
        tt_mpn_normalize(a);
        tt_mpn_normalize(b);
    }

    if (tt_mpn_zero(b)) {
        tt_mpn_swap(divisor, a);
    } else {
        TT_DO_G(bel_out, __mpn_gcd_base(a, b, divisor, mpnc));
    }

    result = TT_SUCCESS;

bel_out:

    tt_mpn_free(mpnc, a);
    tt_mpn_free(mpnc, b);
    tt_mpn_free(mpnc, t1);
    tt_mpn_free(mpnc, t2);
    tt_mpn_free(mpnc, t3);
    tt_mpn_free(mpnc, ma);
    tt_mpn_free(mpnc, mb);
    tt_mpn_free(mpnc, mc);
    tt_mpn_free(mpnc, md);

    return result;
}

tt_result_t __mpn_extgcd_lm(IN tt_mpn_t *u,
                            IN tt_mpn_t *v,
                            OUT tt_mpn_t *divisor,
                            OUT OPT tt_mpn_t *u_cof,
                            OUT OPT tt_mpn_t *v_cof,
                            IN tt_mpn_cache_t *mpnc)
{
    tt_mpn_t *a = NULL, *b = NULL;
    tt_mpn_t *ua = NULL, *ub = NULL, *va = NULL, *vb = NULL;
    tt_mpn_t *t1 = NULL, *t2 = NULL, *t3 = NULL;
    tt_mpn_t *ma = NULL, *mb = NULL, *mc = NULL, *md = NULL;

    tt_result_t result = TT_FAIL;

    TT_DONN_G(bel_out, a = tt_mpn_alloc(mpnc, 0));
    TT_DONN_G(bel_out, b = tt_mpn_alloc(mpnc, 0));
    TT_DONN_G(bel_out, ua = tt_mpn_alloc(mpnc, 0));
    TT_DONN_G(bel_out, ub = tt_mpn_alloc(mpnc, 0));
    TT_DONN_G(bel_out, va = tt_mpn_alloc(mpnc, 0));
    TT_DONN_G(bel_out, vb = tt_mpn_alloc(mpnc, 0));
    TT_DONN_G(bel_out, t1 = tt_mpn_alloc(mpnc, 0));
    TT_DONN_G(bel_out, t2 = tt_mpn_alloc(mpnc, 0));
    TT_DONN_G(bel_out, t3 = tt_mpn_alloc(mpnc, 0));
    TT_DONN_G(bel_out, ma = tt_mpn_alloc(mpnc, 0));
    TT_DONN_G(bel_out, mb = tt_mpn_alloc(mpnc, 0));
    TT_DONN_G(bel_out, mc = tt_mpn_alloc(mpnc, 0));
    TT_DONN_G(bel_out, md = tt_mpn_alloc(mpnc, 0));

    TT_DO_G(bel_out, tt_mpn_copy(a, u, 0));
    TT_DO_G(bel_out, tt_mpn_copy(b, v, 0));
    tt_mpn_set_u(ua, 0, TT_FALSE, 0);
    tt_mpn_set_u(ub, 1, TT_FALSE, 0);
    tt_mpn_set_u(va, 1, TT_FALSE, 0);
    tt_mpn_set_u(vb, 0, TT_FALSE, 0);

    while (b->unit_num > __GCD_THRSH_BASE) {
        tt_result_t rt;

        rt = __lm_core_1(a, b, ma, mb, mc, md);
        // rt = TT_FAIL;
        if (TT_OK(rt)) {
            // a = ma*a + mb*b
            // b = mc*a + md*b
            TT_DO_G(bel_out, tt_mpn_mul(t1, a, ma, mpnc));
            TT_DO_G(bel_out, tt_mpn_mul(t2, b, mb, mpnc));
            TT_DO_G(bel_out, tt_mpn_addeq(t1, t2));

            TT_DO_G(bel_out, tt_mpn_mul(t3, a, mc, mpnc));
            TT_DO_G(bel_out, tt_mpn_mul(t2, b, md, mpnc));
            TT_DO_G(bel_out, tt_mpn_addeq(t3, t2));

            tt_mpn_swap(a, t1);
            tt_mpn_swap(b, t3);

            // ua = ma*ua + mb*ub
            // ub = mc*ua + md*ub
            TT_DO_G(bel_out, tt_mpn_mul(t1, ua, ma, mpnc));
            TT_DO_G(bel_out, tt_mpn_mul(t2, ub, mb, mpnc));
            TT_DO_G(bel_out, tt_mpn_addeq(t1, t2));

            TT_DO_G(bel_out, tt_mpn_mul(t3, ua, mc, mpnc));
            TT_DO_G(bel_out, tt_mpn_mul(t2, ub, md, mpnc));
            TT_DO_G(bel_out, tt_mpn_addeq(t3, t2));

            tt_mpn_swap(ua, t1);
            tt_mpn_swap(ub, t3);

            // va = ma*va + mb*vb
            // vb = mc*va + md*vb
            TT_DO_G(bel_out, tt_mpn_mul(t1, va, ma, mpnc));
            TT_DO_G(bel_out, tt_mpn_mul(t2, vb, mb, mpnc));
            TT_DO_G(bel_out, tt_mpn_addeq(t1, t2));

            TT_DO_G(bel_out, tt_mpn_mul(t3, va, mc, mpnc));
            TT_DO_G(bel_out, tt_mpn_mul(t2, vb, md, mpnc));
            TT_DO_G(bel_out, tt_mpn_addeq(t3, t2));

            tt_mpn_swap(va, t1);
            tt_mpn_swap(vb, t3);
        } else if (rt == TT_PROCEEDING) {
            TT_DO_G(bel_out, tt_mpn_div(t1, NULL, a, b, mpnc));

            // a = b
            // b = a - q*b
            TT_DO_G(bel_out, tt_mpn_mul(t2, t1, b, mpnc));
            TT_DO_G(bel_out, tt_mpn_sub(t2, a, t2));
            tt_mpn_swap(a, b);
            tt_mpn_swap(b, t2);

            // ua = ub
            // ub = ua - q*ub
            TT_DO_G(bel_out, tt_mpn_mul(t2, t1, ub, mpnc));
            TT_DO_G(bel_out, tt_mpn_sub(t2, ua, t2));
            tt_mpn_swap(ua, ub);
            tt_mpn_swap(ub, t2);

            // va = vb
            // vb = va - q*vb
            TT_DO_G(bel_out, tt_mpn_mul(t2, t1, vb, mpnc));
            TT_DO_G(bel_out, tt_mpn_sub(t2, va, t2));
            tt_mpn_swap(va, vb);
            tt_mpn_swap(vb, t2);
        } else {
            goto bel_out;
        }

        // unit num of a and b would be checked in next loop,
        // so need normalize them
        tt_mpn_normalize(a);
        tt_mpn_normalize(b);

#if 0
        // u*va + v*ua = a
        do
        {
            TT_DO_G(bel_out, tt_mpn_mul(t1, u, va, mpnc));
            TT_DO_G(bel_out, tt_mpn_mul(t2, v, ua, mpnc));
            TT_DO_G(bel_out, tt_mpn_addeq(t1, t2));
            TT_ASSERT_MPN(tt_mpn_cmp(t1, a) == 0);
        }while(0);
#endif
    }

    if (tt_mpn_zero(b)) {
        tt_mpn_swap(divisor, a);

        if (u_cof != NULL) {
            tt_mpn_swap(u_cof, va);
        }

        if (v_cof != NULL) {
            tt_mpn_swap(v_cof, ua);
        }
    } else {
        TT_DO_G(bel_out, __mpn_extgcd_base(a, b, divisor, ma, mb, mpnc));

        if (u_cof != NULL) {
            TT_DO_G(bel_out, tt_mpn_mul(u_cof, ma, va, mpnc));
            TT_DO_G(bel_out, tt_mpn_mul(t1, mb, vb, mpnc));
            TT_DO_G(bel_out, tt_mpn_addeq(u_cof, t1));
        }

        if (v_cof != NULL) {
            TT_DO_G(bel_out, tt_mpn_mul(v_cof, ma, ua, mpnc));
            TT_DO_G(bel_out, tt_mpn_mul(t1, mb, ub, mpnc));
            TT_DO_G(bel_out, tt_mpn_addeq(v_cof, t1));
        }
    }

    result = TT_SUCCESS;

bel_out:

    tt_mpn_free(mpnc, a);
    tt_mpn_free(mpnc, b);
    tt_mpn_free(mpnc, ua);
    tt_mpn_free(mpnc, ub);
    tt_mpn_free(mpnc, va);
    tt_mpn_free(mpnc, vb);
    tt_mpn_free(mpnc, t1);
    tt_mpn_free(mpnc, t2);
    tt_mpn_free(mpnc, t3);
    tt_mpn_free(mpnc, ma);
    tt_mpn_free(mpnc, mb);
    tt_mpn_free(mpnc, mc);
    tt_mpn_free(mpnc, md);

    return result;
}

tt_bool_t __verify_extgcd(IN tt_mpn_t *u,
                          IN tt_mpn_t *v,
                          IN tt_mpn_t *divisor,
                          IN tt_mpn_t *u_cof,
                          IN tt_mpn_t *v_cof,
                          IN tt_mpn_cache_t *mpnc)
{
    tt_mpn_t test1, test2;
    tt_bool_t ret;

    tt_mpn_init(&test1);
    tt_mpn_init(&test2);

    tt_mpn_mul(&test1, u, u_cof, mpnc);
    tt_mpn_mul(&test2, v, v_cof, mpnc);
    tt_mpn_add(&test1, &test1, &test2);
    ret = TT_BOOL(tt_mpn_cmp(&test1, divisor) == 0);

#if 1
    if (!ret) {
        tt_mpn_show("u: \n", u, 0);
        tt_mpn_show("v: \n", v, 0);
        tt_mpn_show("divisor: \n", divisor, 0);
        tt_mpn_show("u_cof: \n", u_cof, 0);
        tt_mpn_show("v_cof: \n", v_cof, 0);
    }
#endif

    tt_mpn_destroy(&test1);
    tt_mpn_destroy(&test2);

    return ret;
}
