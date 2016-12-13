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

#include <tt_mpn_native.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

extern tt_mpn_unit_t __mpn_reciprocal_v(tt_mpn_unit_t d);

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

// === routine declarations ================
TT_TEST_ROUTINE_DECLARE(tt_unit_test_mpn_div_reciprocal)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_mpn_div_2by1)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_mpn_div_basic)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_mpn_div_stress)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(bndiv_case)

TT_TEST_CASE("tt_unit_test_mpn_div_reciprocal",
             "mp integer div: reciprocal",
             tt_unit_test_mpn_div_reciprocal,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_mpn_div_2by1",
                 "mp integer div: 2units by 1unit",
                 tt_unit_test_mpn_div_2by1,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_mpn_div_basic",
                 "mp integer div: basic",
                 tt_unit_test_mpn_div_basic,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_mpn_div_stress",
                 "mp integer div: stress",
                 tt_unit_test_mpn_div_stress,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(bndiv_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(MPN_DIV, 0, bndiv_case)

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

    //////////////////////////////////////////////////////

    TT_TEST_ROUTINE_DEFINE(tt_unit_test_mpn_div_reciprocal)
{
    tt_u32_t i = 0;
    tt_mpn_t right, left, radix;
    tt_mpn_unit_t d, rp;
    tt_mpn_cache_t bc;
    tt_result_t ret;

    // d's reciprocal rp:  rp = (r * r - 1)/d - r
    // so (r * r - 1) - (rp + r)*d < d

    TT_TEST_CASE_ENTER()
    // test start

    tt_mpn_init(&right);
    tt_mpn_reserve_u(&right, 2);
    right.unit[0] = ~0;
    right.unit[1] = ~0;
    right.unit_num = 2;

    tt_mpn_init(&radix);
    tt_mpn_reserve_u(&radix, 2);
    radix.unit[0] = 0;
    radix.unit[1] = 1;
    radix.unit_num = 2;

    tt_mpn_init(&left);

    ret = tt_mpn_cache_create(&bc);
    TT_TEST_CHECK_SUCCESS(ret, "");

    d = (tt_mpn_unit_t)0xFFFFFFFFFFFFFFFF;
    rp = __mpn_reciprocal_v(d);
    tt_mpn_set_u(&left, rp, TT_FALSE, 0);
    tt_mpn_addeq(&left, &radix);
    tt_mpn_muleq_u(&left, d, TT_FALSE, &bc);
    tt_mpn_subeq(&left, &right);
    tt_mpn_reverse(NULL, &left);
    TT_TEST_CHECK_EQUAL(tt_mpn_cmp_u(&left, d, TT_FALSE), -1, "");

    d = (tt_mpn_unit_t)0x8000000000000000;
    rp = __mpn_reciprocal_v(d);
    tt_mpn_set_u(&left, rp, TT_FALSE, 0);
    tt_mpn_addeq(&left, &radix);
    tt_mpn_muleq_u(&left, d, TT_FALSE, &bc);
    tt_mpn_subeq(&left, &right);
    tt_mpn_reverse(NULL, &left);
    TT_TEST_CHECK_EQUAL(tt_mpn_cmp_u(&left, d, TT_FALSE), -1, "");

    d = (tt_mpn_unit_t)0x8000000000000001;
    rp = __mpn_reciprocal_v(d);
    tt_mpn_set_u(&left, rp, TT_FALSE, 0);
    tt_mpn_addeq(&left, &radix);
    tt_mpn_muleq_u(&left, d, TT_FALSE, &bc);
    tt_mpn_subeq(&left, &right);
    tt_mpn_reverse(NULL, &left);
    TT_TEST_CHECK_EQUAL(tt_mpn_cmp_u(&left, d, TT_FALSE), -1, "");

    while (i++ < 10) {
        // d = (tt_mpn_unit_t)tt_rand_u64();
        d = 0xa70931da18182951;
        d |= ((tt_mpn_unit_t)1) << (TT_MPN_USIZE_BIT - 1);

        rp = __mpn_reciprocal_v(d);

        tt_mpn_set_u(&left, rp, TT_FALSE, 0);
        tt_mpn_addeq(&left, &radix);
        tt_mpn_muleq_u(&left, d, TT_FALSE, &bc);
        tt_mpn_subeq(&left, &right);
        tt_mpn_reverse(NULL, &left);
        TT_TEST_CHECK_EQUAL(tt_mpn_cmp_u(&left, d, TT_FALSE), -1, "");
    }

    tt_mpn_destroy(&right);
    tt_mpn_destroy(&left);
    tt_mpn_destroy(&radix);

    tt_mpn_cache_destroy(&bc);

    // test end
    TT_TEST_CASE_LEAVE()
}

////////////////////////////////////////////

TT_TEST_ROUTINE_DEFINE(tt_unit_test_mpn_div_2by1)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_mpn_unit_t n1, n0, d, d_rp;
    tt_mpn_unit_t q, r;
    tt_u32_t i;

#ifndef __mpn_div2by1
    extern void __mpn_div2by1(OUT tt_mpn_unit_t * q,
                              OUT tt_mpn_unit_t * r,
                              IN tt_mpn_unit_t n1,
                              IN tt_mpn_unit_t n0,
                              IN tt_mpn_unit_t d,
                              IN tt_mpn_unit_t d_rp);
#endif

    TT_TEST_CASE_ENTER()
    // test start

    i = 0;
    while (i++ < 100) {
        tt_mpn_unit_t r1, r0;

        n1 = (tt_mpn_unit_t)tt_rand_u64();
        n0 = (tt_mpn_unit_t)tt_rand_u64();
        d = (tt_mpn_unit_t)tt_rand_u64();

        d |= ((tt_mpn_unit_t)1) << (TT_MPN_USIZE_BIT - 1);
        if (n1 > d) {
            n1 -= d;
        }

        d_rp = __mpn_reciprocal_v(d);

#ifndef __mpn_div2by1
        __mpn_div2by1(&q, &r, n1, n0, d, d_rp);
#else
        __mpn_div2by1(q, r, n1, n0, d, d_rp);
#endif
        TT_TEST_CHECK_EXP(r < d, "");

        // check
        __MPN_MUL(q, d, r1, r0);
        r0 += r;
        r1 += TT_COND(r0 < r, 1, 0);
        TT_TEST_CHECK_EQUAL(r1, n1, "");
        TT_TEST_CHECK_EQUAL(r0, n0, "");
    }


    // test end
    TT_TEST_CASE_LEAVE()
}

///////////////////////////////////////////////////////////////////////

typedef struct
{
    const tt_char_t *n;
    const tt_char_t *d;
    const tt_char_t *correct_q;
    const tt_char_t *correct_r;
} __bn_div_case_t;

__bn_div_case_t __bn_div_case[] = {
    // simple cases
    {" -0x123", "0x8000111122223334123", "0x0", " -0x123"},
    {"0x8000111122223333123",
     "-0x8000111122223334123",
     "0x0",
     "0x8000111122223333123"},
    {"0x8000111122223333123", "-0x8000111122223333123", "-0x1", "0x0"},
    {"-0x8000111122223333123", "-0x1", "0x8000111122223333123", "0x0"},

    // no bit shift
    {"0x80001111222233334444555566667777",
     "0x8000111122223334",
     "  FFFFFFFFFFFFFFFE",
     "  44447777AAAADDDF"},
    {"0x8000111122223333444455556666777788881111222233334444555566667777",
     "0x80001111222233338000111122223333",
     "0xFFFFFFFFFFFFFFFF88889876620032CD",
     "0x5C8DD04E547AF917DC8E58D6DD0381A0"},

    // with bit shift
    {"0x800011112222333344445555666677778888111122223333",
     "0x1111222233338000111122223333",
     "0x77FF97FFFFFE5C030FFE9",
     "0x44A687F1DE20443D10A2671CCC8"},
};

TT_TEST_ROUTINE_DEFINE(tt_unit_test_mpn_div_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_u32_t i;
    tt_mpn_t n, d, q, r, correct_q, correct_r;
    tt_result_t ret;
    tt_s32_t cmp_ret;
    tt_mpn_cache_t bc;

    TT_TEST_CASE_ENTER()
    // test start

    tt_mpn_init(&n);
    tt_mpn_init(&d);
    tt_mpn_init(&q);
    tt_mpn_init(&r);
    tt_mpn_init(&correct_q);
    tt_mpn_init(&correct_r);

    ret = tt_mpn_cache_create(&bc);
    TT_TEST_CHECK_SUCCESS(ret, "");

    // div by 0
    ret = tt_mpn_div(NULL, NULL, &n, &d, &bc);
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");

    for (i = 0; i < sizeof(__bn_div_case) / sizeof(__bn_div_case[0]); ++i) {
        __bn_div_case_t *tc = &__bn_div_case[i];

        ret = tt_mpn_set_cstr(&n, tc->n, TT_MPN_FMT_AUTO, 0);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_mpn_set_cstr(&d, tc->d, TT_MPN_FMT_AUTO, 0);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_mpn_set_cstr(&correct_q, tc->correct_q, TT_MPN_FMT_AUTO, 0);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_mpn_set_cstr(&correct_r, tc->correct_r, TT_MPN_FMT_AUTO, 0);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        ret = tt_mpn_div(&q, &r, &n, &d, &bc);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        cmp_ret = tt_mpn_cmp(&r, &correct_r);
        if (cmp_ret != 0) {
            tt_char_t c_idx[30] = {0};
            tt_snprintf(c_idx, sizeof(c_idx) - 1, "bn mul fail case: %d", i);
            TT_RECORD_ERROR(c_idx);
        }
        TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

        cmp_ret = tt_mpn_cmp(&q, &correct_q);
        if (cmp_ret != 0) {
            tt_char_t c_idx[30] = {0};
            tt_snprintf(c_idx, sizeof(c_idx) - 1, "bn mul fail case: %d", i);
            TT_RECORD_ERROR(c_idx);
        }
        TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    }

    tt_mpn_destroy(&n);
    tt_mpn_destroy(&d);
    tt_mpn_destroy(&q);
    tt_mpn_destroy(&r);
    tt_mpn_destroy(&correct_q);
    tt_mpn_destroy(&correct_r);

    tt_mpn_cache_destroy(&bc);

    // test end
    TT_TEST_CASE_LEAVE()
}

/////////////////////////////////////////////////////////////

#define __BDS_SIZE 10000
#define __BDS_CNT 100
//#define __BDS_CNT 1000

static tt_u8_t __bds_n[__BDS_SIZE];
static tt_u8_t __bds_d[__BDS_SIZE];

TT_TEST_ROUTINE_DEFINE(tt_unit_test_mpn_div_stress)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_u32_t i, j, n_len, d_len, cmp;
    tt_mpn_t n, d, q, r, tmp;
    tt_result_t ret;
    tt_s64_t start, end, t = 0;
    tt_mpn_cache_t bc;

    TT_TEST_CASE_ENTER()
    // test start

    tt_mpn_init(&n);
    tt_mpn_init(&d);
    tt_mpn_init(&q);
    tt_mpn_init(&r);
    tt_mpn_init(&tmp);

    ret = tt_mpn_cache_create(&bc);
    TT_TEST_CHECK_SUCCESS(ret, "");

    for (i = 0; i < __BDS_CNT; ++i) {
        for (j = 0; j < __BDS_SIZE; ++j) {
            __bds_n[j] = (tt_u8_t)tt_rand_u32();
            __bds_d[j] = (tt_u8_t)tt_rand_u32();
        }

        n_len = tt_rand_u32() % (__BDS_SIZE / 2) + __BDS_SIZE / 2;
        d_len = tt_rand_u32() % __BDS_SIZE + 1;

        ret = tt_mpn_set(&n, __bds_n, n_len, TT_FALSE, 0);
        TT_TEST_CHECK_SUCCESS(ret, "");
        ret = tt_mpn_set(&d, __bds_d, d_len, TT_FALSE, 0);
        TT_TEST_CHECK_SUCCESS(ret, "");
        if (tt_mpn_zero(&d))
            tt_mpn_set_u(&d, 3, TT_FALSE, 0);

        ret = tt_mpn_div(&q, &r, &n, &d, &bc);
        TT_TEST_CHECK_SUCCESS(ret, "");

        start = tt_time_ref();
        ret = tt_mpn_mul(&tmp, &q, &d, &bc);
        end = tt_time_ref();
        t += end - start;

        TT_TEST_CHECK_SUCCESS(ret, "");
        ret = tt_mpn_addeq(&tmp, &r);
        TT_TEST_CHECK_SUCCESS(ret, "");

        cmp = tt_mpn_cmp(&tmp, &n);
        TT_ASSERT(cmp == 0);
        TT_TEST_CHECK_EQUAL(cmp, 0, "");

        // div q
        ret = tt_mpn_div_q(&tmp, &n, &d, &bc);
        TT_TEST_CHECK_SUCCESS(ret, "");
        TT_TEST_CHECK_EQUAL(tt_mpn_cmp(&tmp, &q), 0, "");

        // div r
        ret = tt_mpn_div_r(&tmp, &n, &d, &bc);
        TT_TEST_CHECK_SUCCESS(ret, "");
        TT_TEST_CHECK_EQUAL(tt_mpn_cmp(&tmp, &r), 0, "");

        // diveq
        ret = tt_mpn_copy(&tmp, &n, 0);
        TT_TEST_CHECK_SUCCESS(ret, "");
        ret = tt_mpn_diveq(&tmp, &d, &bc);
        TT_TEST_CHECK_SUCCESS(ret, "");
        TT_TEST_CHECK_EQUAL(tt_mpn_cmp(&tmp, &q), 0, "");
    }

    tt_mpn_destroy(&n);
    tt_mpn_destroy(&d);
    tt_mpn_destroy(&q);
    tt_mpn_destroy(&r);
    tt_mpn_destroy(&tmp);

    tt_mpn_cache_destroy(&bc);

    TT_RECORD_INFO("mul div time: %dms", tt_time_ref2ms(t));

    // test end
    TT_TEST_CASE_LEAVE()
}
