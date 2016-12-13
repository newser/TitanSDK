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

#include <tt_platform.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

// === routine declarations ================
TT_TEST_ROUTINE_DECLARE(tt_unit_test_mpn_mod_brp)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_mpn_mod)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_mpn_mod_perf)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(mpmod_case)

TT_TEST_CASE("tt_unit_test_mpn_mod_brp",
             "mp integer: barrett reciprocal",
             tt_unit_test_mpn_mod_brp,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_mpn_mod",
                 "mp integer mod",
                 tt_unit_test_mpn_mod,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_mpn_mod_perf",
                 "mp integer mod: perf vs",
                 tt_unit_test_mpn_mod_perf,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(mpmod_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(MPN_MOD, 0, mpmod_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(name)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    typedef struct
{
    const tt_char_t *a;
    const tt_char_t *ret;
} __mp_brp_case_t;

//////////////////////////////////////////////////////
__mp_brp_case_t __mp_minv_case[] = {
    {
        "0x1", "0x100000000000000000000000000000000",
    },
    {
        "0x2", "0x80000000000000000000000000000000",
    },
    {
        "0x3", "0x55555555555555555555555555555555",
    },
    {
        "0x31111222233334444",
        "0x537a6d7d54d9372d9287c612edfdd6bddd33d60d226402e5",
    },
    {
        "0x11112222333344445555666677778888aaaabbbbccc",
        "0xefff0fffffffffffffffffffffffffff1001dfff10b4ee9710b400",
    },
};

//////////////////////////////////////////////////////

static tt_u8_t __mc_mod[1000];

TT_TEST_ROUTINE_DEFINE(tt_unit_test_mpn_mod_brp)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_u32_t i;
    tt_mpn_t a, r, answer, tmp;
    tt_result_t ret;
    tt_s32_t cmp_ret;
    tt_u32_t mr = 0;
    tt_mpn_cache_t bc;

    TT_TEST_CASE_ENTER()
    // test start

    tt_mpn_init(&a);
    tt_mpn_init(&r);
    tt_mpn_init(&answer);
    tt_mpn_init(&tmp);

    ret = tt_mpn_cache_create(&bc);
    TT_TEST_CHECK_SUCCESS(ret, "");

    for (i = 0; i < sizeof(__mp_minv_case) / sizeof(__mp_minv_case[0]); ++i) {
        __mp_brp_case_t *tc = &__mp_minv_case[i];

        ret = tt_mpn_set_cstr(&a, tc->a, TT_MPN_FMT_AUTO, 0);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_mpn_set_cstr(&answer, tc->ret, TT_MPN_FMT_AUTO, 0);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        ret = tt_mpn_mod_brp(&r, &mr, &a, &bc);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        cmp_ret = tt_mpn_cmp(&r, &answer);
        if (cmp_ret != 0) {
            tt_char_t c_idx[30] = {0};
            tt_snprintf(c_idx,
                        sizeof(c_idx) - 1,
                        "mp mod coeff fail case: %d",
                        i);
            TT_RECORD_ERROR(c_idx);
        }
        TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    }

    for (i = 0; i < 10; ++i) {
        tt_u32_t j, n;

        n = tt_rand_u32() % sizeof(__mc_mod);
        if (n == 0)
            n = 1;
        for (j = 0; j < n; ++j) {
            __mc_mod[j] = (tt_u8_t)tt_rand_u32();
        }

        tt_mpn_set(&a, __mc_mod, n, TT_FALSE, 0);

        ret = tt_mpn_mod_brp(&r, &mr, &a, &bc);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        ret = tt_mpn_mul(&tmp, &r, &a, &bc);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_mpn_subeq_ulshf_u(&tmp, 1, TT_FALSE, mr & 0x7FFFFFFF);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        tmp.negative = TT_FALSE;
        TT_TEST_CHECK_EXP(tt_mpn_cmp(&tmp, &a) < 0, "");
    }

    tt_mpn_destroy(&a);
    tt_mpn_destroy(&r);
    tt_mpn_destroy(&answer);
    tt_mpn_destroy(&tmp);

    tt_mpn_cache_destroy(&bc);

    // test end
    TT_TEST_CASE_LEAVE()
}

typedef struct
{
    const tt_char_t *a;
    const tt_char_t *m;
    const tt_char_t *ret;
} __mp_mod_case_t;

//////////////////////////////////////////////////////
__mp_mod_case_t __mp_mod_case[] = {
    {
        "0x1", "-0x1", "0x0",
    },
    {
        "-0x111122223333444455556", "0x1", "0x0",
    },
    {
        "-0x0", "-0x111122223333444455556", "0x0",
    },
    {
        "-0x1111222233334444", "0x33331111", "-0x1E5A8C5",
    },
    {
        "-0x450893adf2ea15630e15d5ed569fdf69e8959261a935d20f71e66afb54e410c5635"
        "591aa83c94c740b383a20b3f2bc8503f73ce3ee436aa929af71e91efb69e39b0bc2c97"
        "62184ab723bc184460bce2b13b55e1db191cfc56b61eedddc4a2ac5b84d962f90",
        "-410ae47f012ce9bd1a20508c9a1918c94c210e6e58acb9a23bdeda1333d855fccc3d0"
        "267d47bd782ad4af4633d0700366ce3f2ea9c2222800c536989a3b9515775993260b8b"
        "4",
        "-17F767E89AFFF95646E9E3240715CC4BAA00F3610EACE7AB1096C43855CD7FAA33CA7"
        "AA0388A0FC80FEA4353D715B560AE6A47FB12DE1A5B31A09AAB41582734DC90D0E2788"
        "C",
        // q: 10FB53B680EE846B43464575861D1D25E8FDB0E68F472DA5330BA4678F54E65
    },
    {
        "0x4135f8e7d3ce69f16c0b581836cd0d3a48572f1e4aef0151cfca87c95c4bef2ee6de"
        "ccadc2b09f85b9489f44db3671fe57e2c92f5c0716a2595ed4da933fd65e8ad2e97b6a"
        "202929759d188b83155df9144ec0f87f9552be88c1bed56decbb45a0b54e188b9aadf0"
        "5663a30e915f15",
        "-b13d73",
        "6A406",
        // q:
        // 5E304139498DF5FBB70DF534D05D8B556B998F76C7DBE0649F1D86E7579AF44B5425796B0545D83DEF33C84821585DF35C1F1D7CE5FECFB105A07F656AD1774859AC2D162ACBF095B8EE5089C924F143F860F4D9D144DC5124CF94BBA5752B6D6852DF6F7A75C30FD8EE64F5
    },
};

static tt_u8_t __m[300];

TT_TEST_ROUTINE_DEFINE(tt_unit_test_mpn_mod)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_u32_t i;
    tt_mpn_t a, m, r, answer, tmp;
    tt_result_t ret;
    tt_s32_t cmp_ret;
    tt_mpn_cache_t bc;

    TT_TEST_CASE_ENTER()
    // test start

    tt_mpn_init(&a);
    tt_mpn_init(&m);
    tt_mpn_init(&r);
    tt_mpn_init(&answer);
    tt_mpn_init(&tmp);

    ret = tt_mpn_cache_create(&bc);
    TT_TEST_CHECK_SUCCESS(ret, "");

    for (i = 0; i < sizeof(__mp_mod_case) / sizeof(__mp_mod_case[0]); ++i) {
        __mp_mod_case_t *tc = &__mp_mod_case[i];

        ret = tt_mpn_set_cstr(&a, tc->a, TT_MPN_FMT_AUTO, 0);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_mpn_set_cstr(&m, tc->m, TT_MPN_FMT_AUTO, 0);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_mpn_set_cstr(&answer, tc->ret, TT_MPN_FMT_AUTO, 0);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        ret = tt_mpn_mod(&r, &a, &m, &bc);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        cmp_ret = tt_mpn_cmp(&r, &answer);
        if (cmp_ret != 0) {
            tt_char_t c_idx[30] = {0};
            tt_snprintf(c_idx,
                        sizeof(c_idx) - 1,
                        "mp mod coeff fail case: %d",
                        i);
            TT_RECORD_ERROR(c_idx);
        }
        TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

        ret = tt_mpn_modeq(&a, &m, &bc);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_mpn_cmp(&r, &answer);
        TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    }

    for (i = 0; i < 10; ++i) {
        tt_u32_t j, n;

        n = tt_rand_u32() % sizeof(__mc_mod);
        if (n == 0)
            n = 1;
        for (j = 0; j < n; ++j) {
            __mc_mod[j] = (tt_u8_t)tt_rand_u32();
        }
        tt_mpn_set(&a, __mc_mod, n, TT_FALSE, 0);
        a.negative = TT_BOOL(i % 3 == 0);

        n = tt_rand_u32() % sizeof(__m);
        if (n == 0)
            n = 1;
        for (j = 0; j < n; ++j) {
            __m[j] = (tt_u8_t)tt_rand_u32();
        }
        tt_mpn_set(&m, __m, n, TT_FALSE, 0);
        m.negative = TT_BOOL(i % 2 == 0);

        // tt_mpn_show("a: \n", &a, 0);
        // tt_mpn_show("m: \n", &m, 0);
        ret = tt_mpn_mod(&r, &a, &m, &bc);
        // tt_mpn_show("r: \n", &r, 0);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        TT_TEST_CHECK_EQUAL(r.negative, a.negative, "");

        ret = tt_mpn_sub(&tmp, &a, &r);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_mpn_div(NULL, &tmp, &tmp, &m, &bc);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        // TT_ASSERT_ALWAYS(tt_mpn_zero(&tmp));
        TT_TEST_CHECK_EXP(tt_mpn_zero(&tmp), "");

        ret = tt_mpn_modeq(&a, &m, &bc);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_mpn_cmp(&r, &a);
        TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    }

    tt_mpn_destroy(&a);
    tt_mpn_destroy(&m);
    tt_mpn_destroy(&r);
    tt_mpn_destroy(&answer);
    tt_mpn_destroy(&tmp);

    tt_mpn_cache_destroy(&bc);

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_u8_t __mc_mod_perf[10000];
static tt_u8_t __m_perf[5000];

TT_TEST_ROUTINE_DEFINE(tt_unit_test_mpn_mod_perf)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_mpn_t a, m, r, minv;
    tt_u32_t i, j, n1, n2, minv_range;
    tt_s64_t t_div, t_minv, start, end;
    tt_mpn_cache_t bc;
    tt_result_t ret;

    TT_TEST_CASE_ENTER()
    // test start

    tt_mpn_init(&a);
    tt_mpn_init(&m);
    tt_mpn_init(&r);
    tt_mpn_init(&minv);

    ret = tt_mpn_cache_create(&bc);
    TT_TEST_CHECK_SUCCESS(ret, "");

    // a
    n1 = tt_rand_u32() % sizeof(__mc_mod_perf);
    if (n1 == 0)
        n1 = 1;
    for (j = 0; j < n1; ++j) {
        __mc_mod_perf[j] = (tt_u8_t)tt_rand_u32();
    }
    tt_mpn_set(&a, __mc_mod_perf, n1, TT_FALSE, 0);

    // m
    n2 = tt_rand_u32() % sizeof(__m_perf);
    if (n2 == 0)
        n2 = 1;
    for (j = 0; j < n2; ++j) {
        __m_perf[j] = (tt_u8_t)tt_rand_u32();
    }
    tt_mpn_set(&m, __m_perf, n2, TT_FALSE, 0);

    // minv
    minv_range = a.unit_num;
    tt_mpn_mod_brp(&minv, &minv_range, &m, &bc);

#define __perf_cnt 100
    start = tt_time_ref();
    for (i = 0; i < __perf_cnt; ++i) {
        tt_mpn_mod(&r, &a, &m, &bc);
    }
    end = tt_time_ref();
    t_div = end - start;

    start = tt_time_ref();
    for (i = 0; i < __perf_cnt; ++i) {
        a.negative = TT_BOOL(i % 3);
        m.negative = TT_BOOL(i % 2);

        tt_mpn_modbr(&r, &a, &m, &minv, minv_range, &bc);

        TT_TEST_CHECK_EQUAL(r.negative, a.negative, "");
    }
    end = tt_time_ref();
    t_minv = end - start;

    TT_RECORD_INFO("[%d]B mod [%d]B, div: %dms, minv: %dms",
                   n1,
                   n2,
                   tt_time_ref2ms(t_div),
                   tt_time_ref2ms(t_minv));

    tt_mpn_destroy(&a);
    tt_mpn_destroy(&m);
    tt_mpn_destroy(&r);
    tt_mpn_destroy(&minv);

    tt_mpn_cache_destroy(&bc);

    // test end
    TT_TEST_CASE_LEAVE()
}
