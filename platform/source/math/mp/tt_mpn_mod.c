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

#include <math/mp/tt_mpn_mod.h>

#include <log/tt_log.h>
#include <math/mp/tt_mpn_add.h>
#include <math/mp/tt_mpn_common.h>
#include <math/mp/tt_mpn_div.h>
#include <math/mp/tt_mpn_mul.h>
#include <math/mp/tt_mpn_sub.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __BRP_V(brpr) ((brpr)&0x7FFFFFFF)
#define __BRP_SET_MARK(brpr) ((brpr) |= 0x80000000)
#define __BRP_CLR_MARK(brpr) ((brpr) &= 0x7FFFFFFF)
#define _BRP_MARKED(brpr) ((brpr)&0x80000000)

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

static tt_result_t __mpn_mod(OUT tt_mpn_t *ret,
                             IN tt_mpn_t *a,
                             IN tt_mpn_t *m,
                             IN tt_mpn_t *brp,
                             IN tt_u32_t brp_range,
                             IN tt_mpn_cache_t *mpnc);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_mpn_modbr(OUT tt_mpn_t *ret,
                         IN tt_mpn_t *a,
                         IN tt_mpn_t *m,
                         IN OPT tt_mpn_t *brp,
                         IN OPT tt_u32_t brp_range,
                         IN tt_mpn_cache_t *mpnc)
{
    tt_mpn_t *p_ret = NULL;
    tt_bool_t a_neg, m_neg, brp_neg;
    tt_result_t result = TT_FAIL;

    if (brp == NULL) {
        return tt_mpn_div_r(ret, a, m, mpnc);
    }

    if (tt_mpn_zero(m)) {
        TT_ERROR("modular can not be 0");
        return TT_FAIL;
    }

    tt_mpn_normalize(a);
    a_neg = a->negative;
    a->negative = TT_FALSE;

    tt_mpn_normalize(m);
    m_neg = m->negative;
    m->negative = TT_FALSE;

    tt_mpn_normalize(brp);
    brp_neg = brp->negative;
    brp->negative = TT_FALSE;

    if ((ret == a) || (ret == m)) {
        TT_DONN_G(m_out, p_ret = tt_mpn_alloc(mpnc, 0));
    } else {
        p_ret = ret;
    }

    // ========================================
    // mod start

    // todo:
    //  - may use div directly if brp is null
    //  - optimize when module = b^k - l

    TT_DO_G(m_out, __mpn_mod(p_ret, a, m, brp, brp_range, mpnc));

    p_ret->negative = a_neg;

    // mod end
    // ========================================

    if (p_ret != ret) {
        tt_mpn_swap(p_ret, ret);
    }

    result = TT_SUCCESS;

m_out:

    if (p_ret != ret) {
        tt_mpn_free(mpnc, p_ret);
    }

    a->negative = a_neg;
    m->negative = m_neg;
    brp->negative = brp_neg;

    return result;
}

tt_result_t __mpn_mod(OUT tt_mpn_t *ret,
                      IN tt_mpn_t *a,
                      IN tt_mpn_t *m,
                      IN tt_mpn_t *brp,
                      IN tt_u32_t brp_range,
                      IN tt_mpn_cache_t *mpnc)
{
    tt_mpn_t *q = NULL;
    tt_u32_t k;
    tt_result_t result = TT_FAIL;

    TT_DONN_G(bm_out, q = tt_mpn_alloc(mpnc, 0));

    k = __BRP_V(brp_range) >> 1;
    TT_ASSERT_MPN(k >= 1);

    if (_BRP_MARKED(brp_range)) {
        // a is too large:
        // q = a * brp
        // q = q / b^2k
        TT_DO_G(bm_out, tt_mpn_mul(q, a, brp, mpnc));
        TT_DO_G(bm_out, tt_mpn_rshf_u(NULL, q, k << 1));
    } else {
        // a is not too larger than modular:
        // q = a / b^(k-1)
        // q = q * brp
        // q = q / b^(k+1)
        TT_DO_G(bm_out, tt_mpn_rshf_u(q, a, k - 1));
        TT_DO_G(bm_out, tt_mpn_muleq(q, brp, mpnc));
        TT_DO_G(bm_out, tt_mpn_rshf_u(NULL, q, k + 1));
    }

    // ret = a mod b^(k+1)
    // q = (q * m) mod b^(k+1)
    // ret = ret - q
    TT_DO_G(bm_out, tt_mpn_copy(ret, a, 0));
    ret->unit_num = TT_MIN(ret->unit_num, k + 1);
    TT_DO_G(bm_out, tt_mpn_muleq(q, m, mpnc));
    q->unit_num = TT_MIN(q->unit_num, k + 1);
    TT_DO_G(bm_out, tt_mpn_subeq(ret, q));
    // now we have: ret = (Q-q)*m+R or ret = (Q-q)*m+R-b^(k+1)

    // if ret < 0, ret = ret + b^(k+1)
    if (ret->negative) {
        TT_DO_G(bm_out, tt_mpn_addeq_ulshfu(ret, 1, TT_FALSE, k + 1));
    }
    TT_ASSERT_MPN(!ret->negative);

    // now Q-2 <= q < Q, so R < ret <= R+2*m
    // while ret >= m, ret = ret - m
    if (tt_mpn_cmp(ret, m) >= 0) {
        tt_mpn_subeq(ret, m);
    }
    if (tt_mpn_cmp(ret, m) >= 0) {
        tt_mpn_subeq(ret, m);
    }
    TT_ASSERT_MPN(tt_mpn_cmp(ret, m) < 0);

    result = TT_SUCCESS;

bm_out:

    tt_mpn_free(mpnc, q);

    return result;
}

tt_result_t tt_mpn_mod_brp(OUT tt_mpn_t *brp,
                           IN OUT tt_u32_t *brp_range,
                           IN tt_mpn_t *m,
                           IN tt_mpn_cache_t *mpnc)
{
    tt_u32_t brpr;
    tt_mpn_t n;
    tt_result_t result = TT_FAIL;

    if (tt_mpn_zero(m)) {
        TT_ERROR("modular can not be 0");
        return TT_FAIL;
    }

    // highest 1 bit is reserved
    brpr = __BRP_V(*brp_range);
    if (brpr < (m->unit_num << 1)) {
        brpr = m->unit_num << 1;
    } else {
        if ((brpr & 0x1) == 0x1) {
            brpr += 1;
        }
        __BRP_SET_MARK(brpr);
    }
    *brp_range = brpr;

    // construct b^brpr
    tt_mpn_init(&n);
    TT_DO_G(mb_out, tt_mpn_ulshf_u(&n, 1, TT_FALSE, __BRP_V(brpr)));

    // floor(b^brpr / modular)
    TT_DO_G(mb_out, tt_mpn_div(brp, NULL, &n, m, mpnc));

    result = TT_SUCCESS;

mb_out:

    tt_mpn_destroy(&n);

    return result;
}
