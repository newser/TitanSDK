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

extern tt_mpn_unit_t __gcd_u(IN tt_mpn_unit_t u, IN tt_mpn_unit_t v);
extern tt_mpn_unit_t __extgcd_u(IN tt_mpn_unit_t u,
                                IN tt_mpn_unit_t v,
                                OUT tt_mpn_unit_t *u1,
                                OUT tt_mpn_unit_t *v1);
extern tt_bool_t __verify_extgcd(IN tt_mpn_t *u,
                                 IN tt_mpn_t *v,
                                 IN tt_mpn_t *divisor,
                                 IN tt_mpn_t *u_cof,
                                 IN tt_mpn_t *v_cof,
                                 IN tt_mpn_cache_t *bcache);

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

// === routine declarations ================
TT_TEST_ROUTINE_DECLARE(tt_unit_test_mpn_gcd_u)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_mpn_extgcd_u)

TT_TEST_ROUTINE_DECLARE(tt_unit_test_mpn_gcd)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_mpn_extgcd)

TT_TEST_ROUTINE_DECLARE(tt_unit_test_mpn_minv)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(bngcd_case)
#if 0
    TT_TEST_CASE("tt_unit_test_mpn_gcd_u",
                 "mp integer gcd: unit",
                 tt_unit_test_mpn_gcd_u, NULL,
                 NULL, NULL,
                 NULL, NULL),

    TT_TEST_CASE("tt_unit_test_mpn_extgcd_u",
                 "mp integer extgcd: unit",
                 tt_unit_test_mpn_extgcd_u, NULL,
                 NULL, NULL,
                 NULL, NULL),

    TT_TEST_CASE("tt_unit_test_mpn_gcd",
                 "mp integer gcd",
                 tt_unit_test_mpn_gcd, NULL,
                 NULL, NULL,
                 NULL, NULL),
#endif
TT_TEST_CASE("tt_unit_test_mpn_extgcd",
             "mp integer extgcd",
             tt_unit_test_mpn_extgcd,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_mpn_minv",
                 "mp integer modular mul inverse",
                 tt_unit_test_mpn_minv,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(bngcd_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(MPN_GCD, 0, bngcd_case)

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

    TT_TEST_ROUTINE_DEFINE(tt_unit_test_mpn_gcd_u)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_mpn_unit_t u, v, g;
    tt_u32_t i;

    TT_TEST_CASE_ENTER()
    // test start

    u = 0;
    v = 100;
    g = __gcd_u(u, v);
    TT_TEST_CHECK_EQUAL(g, 100, "");

    u = 1;
    v = 100;
    g = __gcd_u(u, v);
    TT_TEST_CHECK_EQUAL(g, 1, "");

    u = 0x123456677;
    v = 0x12213454312AAA;
    g = __gcd_u(u, v);
    TT_TEST_CHECK_EQUAL(g, 1, "");

    u = 0x123456677;
    v = 0x14A06D269357;
    g = __gcd_u(u, v);
    TT_TEST_CHECK_EQUAL(g, 0x123456677, "");

    for (i = 0; i < 100; ++i) {
        u = (tt_mpn_unit_t)tt_rand_u64();
        v = (tt_mpn_unit_t)tt_rand_u64();
        g = __gcd_u(u, v);

        TT_TEST_CHECK_EQUAL(u % g, 0, "");
        TT_TEST_CHECK_EQUAL(v % g, 0, "");
    }

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_mpn_extgcd_u)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_mpn_unit_t u, v, g, u1, v1, a;
    tt_u32_t i;

    TT_TEST_CASE_ENTER()
    // test start

    u = 0;
    v = 100;
    g = __extgcd_u(u, v, &u1, &v1);
    TT_TEST_CHECK_EQUAL(g, 100, "");
    a = u * u1 + v * v1;
    TT_TEST_CHECK_EQUAL(a, 100, "");

    u = 1;
    v = 100;
    g = __extgcd_u(u, v, &u1, &v1);
    TT_TEST_CHECK_EQUAL(g, 1, "");
    a = u * u1 + v * v1;
    TT_TEST_CHECK_EQUAL(a, 1, "");

    u = 0x123456677;
    v = 0x12213454312AAA;
    g = __extgcd_u(u, v, &u1, &v1);
    TT_TEST_CHECK_EQUAL(g, 1, "");
    a = u * u1 + v * v1;
    TT_TEST_CHECK_EQUAL(a, 1, "");

    u = 0x123456677;
    v = 0x14A06D269357;
    g = __extgcd_u(u, v, &u1, &v1);
    TT_TEST_CHECK_EQUAL(g, 0x123456677, "");
    a = u * u1 + v * v1;
    TT_TEST_CHECK_EQUAL(a, 0x123456677, "");

    for (i = 0; i < 100; ++i) {
        u = (tt_mpn_unit_t)tt_rand_u64();
        v = (tt_mpn_unit_t)tt_rand_u64();
        g = __extgcd_u(u, v, &u1, &v1);
        a = u * u1 + v * v1;

        TT_TEST_CHECK_EQUAL(u % g, 0, "");
        TT_TEST_CHECK_EQUAL(v % g, 0, "");
        TT_TEST_CHECK_EQUAL(a, g, "");
    }

    // test end
    TT_TEST_CASE_LEAVE()
}

///////////////////////////////////////////////////////////////
typedef struct
{
    const tt_char_t *u;
    const tt_char_t *v;
    const tt_char_t *d;
    const tt_char_t *u_cof;
    const tt_char_t *v_cof;
} __egcd_case_t;

__egcd_case_t egcd_case[] = {
    {
        "0x1", "-0x0", "0x1", "0x1", "0x0",
    },
    {
        "-0x0", "0x0", "0x0", "-0x1", "0x0",
    },
    {
        "-0x14B3FB0B7179CACBDDDE215C7B86F28E1ADA974D49790863983A317433E3566AC4"
        "8",
        "-0x12355436543765478765853427D511BA37AB4FFDBFBBD669C6B394A20CBFB783702"
        "A2D84635EE4",
        "0x246AA86CA86ECA8F0ECB0A685B5FB4",
    },
};

TT_TEST_ROUTINE_DEFINE(tt_unit_test_mpn_extgcd)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_mpn_t u, v, u_cof, v_cof, d;
    tt_mpn_t correct_u_cof, correct_v_cof, correct_d, tmp1, tmp2;
    tt_u32_t i;
    tt_result_t ret;
    tt_mpn_cache_t bc;

    tt_s64_t start, end, t = 0;

    TT_TEST_CASE_ENTER()
    // test start

    tt_mpn_init(&u);
    tt_mpn_init(&v);
    tt_mpn_init(&u_cof);
    tt_mpn_init(&v_cof);
    tt_mpn_init(&d);
    tt_mpn_init(&correct_u_cof);
    tt_mpn_init(&correct_v_cof);
    tt_mpn_init(&correct_d);
    tt_mpn_init(&tmp1);
    tt_mpn_init(&tmp2);

    ret = tt_mpn_cache_create(&bc);
    TT_TEST_CHECK_SUCCESS(ret, "");

    for (i = 0; i < sizeof(egcd_case) / sizeof(egcd_case[0]); ++i) {
        __egcd_case_t *ec = &egcd_case[i];

        ret = tt_mpn_set_cstr(&u, ec->u, TT_MPN_FMT_AUTO, 0);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_mpn_set_cstr(&v, ec->v, TT_MPN_FMT_AUTO, 0);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        // ret = tt_mpn_set_cstr(&correct_u_cof,ec->u_cof,TT_MPN_FMT_AUTO,0);
        // TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        // ret = tt_mpn_set_cstr(&correct_v_cof,ec->v_cof,TT_MPN_FMT_AUTO,0);
        // TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_mpn_set_cstr(&correct_d, ec->d, TT_MPN_FMT_AUTO, 0);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        ret = tt_mpn_extgcd(&d, &u_cof, &v_cof, &u, &v, &bc);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        TT_TEST_CHECK_EQUAL(tt_mpn_cmp(&d, &correct_d), 0, "");

        TT_TEST_CHECK_EQUAL(__verify_extgcd(&u, &v, &d, &u_cof, &v_cof, &bc),
                            TT_TRUE,
                            "");

        ret = tt_mpn_mul(&tmp1, &u, &u_cof, &bc);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_mpn_mul(&tmp2, &v, &v_cof, &bc);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_mpn_add(&tmp2, &tmp1, &tmp2);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        TT_TEST_CHECK_EQUAL(tt_mpn_cmp(&tmp2, &d), 0, "");
    }

//#define __gcd_forever
#ifdef __gcd_forever
gcd_ag:
    for (i = 0; i < 1000; ++i)
#else
    for (i = 0; i < 10; ++i)
#endif
    {
        tt_u8_t u_byte[1000], v_byte[1000];
        tt_u32_t u_n, v_n, k;
        tt_bool_t u_neg = TT_FALSE, v_neg = TT_FALSE;

        u_n = tt_rand_u32() % sizeof(u_byte);
        if (u_n < 500)
            u_n += 400;
        for (k = 0; k < u_n; ++k)
            u_byte[k] = (tt_u8_t)tt_rand_u32();

        v_n = tt_rand_u32() % sizeof(v_byte);
        if (v_n < 500)
            v_n += 400;
        for (k = 0; k < v_n; ++k)
            v_byte[k] = (tt_u8_t)tt_rand_u32();

        if (i % 3 == 0)
            u_neg = TT_TRUE;
        ret = tt_mpn_set(&u, u_byte, u_n, u_neg, 0);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        if (i % 2 == 0)
            v_neg = TT_TRUE;
        ret = tt_mpn_set(&v, v_byte, v_n, v_neg, 0);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        start = tt_time_ref();
        ret = tt_mpn_extgcd(&d, &u_cof, &v_cof, &u, &v, &bc);
        end = tt_time_ref();
        t += end - start;

        TT_TEST_CHECK_EQUAL(u.negative, u_neg, "");
        TT_TEST_CHECK_EQUAL(v.negative, v_neg, "");

        TT_TEST_CHECK_EQUAL(d.negative, TT_FALSE, "");

#ifndef __gcd_forever
        TT_TEST_CHECK_EQUAL(__verify_extgcd(&u, &v, &d, &u_cof, &v_cof, &bc),
                            TT_TRUE,
                            "");
#else
// TT_ASSERT_ALWAYS(__verify_extgcd(&u,&v,&d,&u_cof,&v_cof,&bc));
#endif

        if (i % 3 == 0) {
            ret = tt_mpn_extgcd(&d, &u, &v, &u, &v, &bc);
            TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
            TT_ASSERT(tt_mpn_cmp(&u, &u_cof) == 0);
            TT_TEST_CHECK_EQUAL(tt_mpn_cmp(&u, &u_cof), 0, "");
            TT_ASSERT(tt_mpn_cmp(&v, &v_cof) == 0);
            TT_TEST_CHECK_EQUAL(tt_mpn_cmp(&v, &v_cof), 0, "");
        } else if (i % 3 == 1) {
            ret = tt_mpn_extgcd(&u, &u_cof, &v, &u, &v, &bc);
            TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
            TT_ASSERT(tt_mpn_cmp(&u, &d) == 0);
            TT_TEST_CHECK_EQUAL(tt_mpn_cmp(&u, &d), 0, "");
            TT_ASSERT(tt_mpn_cmp(&v, &v_cof) == 0);
            TT_TEST_CHECK_EQUAL(tt_mpn_cmp(&v, &v_cof), 0, "");
        } else {
            ret = tt_mpn_extgcd(&u, &v, &v_cof, &u, &v, &bc);
            TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
            TT_ASSERT(tt_mpn_cmp(&u, &d) == 0);
            TT_TEST_CHECK_EQUAL(tt_mpn_cmp(&u, &d), 0, "");
            TT_ASSERT(tt_mpn_cmp(&v, &u_cof) == 0);
            TT_TEST_CHECK_EQUAL(tt_mpn_cmp(&v, &u_cof), 0, "");
        }
    }
    TT_RECORD_INFO("extgcd time: %dms", tt_time_ref2ms(t));

#ifdef __gcd_forever
    do {
        static int vn = 0;
        TT_INFO("[%d] verify extgcd...", (++vn) * i);
        tt_sleep(1000);
        goto gcd_ag;
    } while (0);
#endif

    tt_mpn_destroy(&u);
    tt_mpn_destroy(&v);
    tt_mpn_destroy(&u_cof);
    tt_mpn_destroy(&v_cof);
    tt_mpn_destroy(&d);
    tt_mpn_destroy(&correct_u_cof);
    tt_mpn_destroy(&correct_v_cof);
    tt_mpn_destroy(&correct_d);
    tt_mpn_destroy(&tmp1);
    tt_mpn_destroy(&tmp2);

    tt_mpn_cache_destroy(&bc);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_mpn_gcd)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_mpn_t u, v, d, r;
    tt_mpn_t correct_d, tmp1, tmp2;
    tt_u32_t i;
    tt_result_t ret;
    tt_mpn_cache_t bc;

    tt_s64_t start, end, t = 0;

    TT_TEST_CASE_ENTER()
    // test start

    tt_mpn_init(&u);
    tt_mpn_init(&v);
    tt_mpn_init(&d);
    tt_mpn_init(&r);
    tt_mpn_init(&correct_d);
    tt_mpn_init(&tmp1);
    tt_mpn_init(&tmp2);

    ret = tt_mpn_cache_create(&bc);
    TT_TEST_CHECK_SUCCESS(ret, "");

    for (i = 0; i < sizeof(egcd_case) / sizeof(egcd_case[0]); ++i) {
        __egcd_case_t *ec = &egcd_case[i];

        ret = tt_mpn_set_cstr(&u, ec->u, TT_MPN_FMT_AUTO, 0);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_mpn_set_cstr(&v, ec->v, TT_MPN_FMT_AUTO, 0);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        // ret = tt_mpn_set_cstr(&correct_u_cof,ec->u_cof,TT_MPN_FMT_AUTO,0);
        // TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        // ret = tt_mpn_set_cstr(&correct_v_cof,ec->v_cof,TT_MPN_FMT_AUTO,0);
        // TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_mpn_set_cstr(&correct_d, ec->d, TT_MPN_FMT_AUTO, 0);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        ret = tt_mpn_gcd(&d, &u, &v, &bc);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        TT_TEST_CHECK_EQUAL(tt_mpn_cmp(&d, &correct_d), 0, "");

        if (tt_mpn_zero(&d))
            continue;

        ret = tt_mpn_div(&tmp1, &r, &u, &d, &bc);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        TT_TEST_CHECK_EQUAL(tt_mpn_zero(&r), TT_TRUE, "");

        ret = tt_mpn_div(&tmp2, &r, &v, &d, &bc);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        TT_TEST_CHECK_EQUAL(tt_mpn_zero(&r), TT_TRUE, "");

        // should be pairwise prime
        ret = tt_mpn_gcd(&r, &tmp1, &tmp2, &bc);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        TT_TEST_CHECK_EQUAL(tt_mpn_cmp_u(&r, 1, TT_FALSE), 0, "");
    }

    for (i = 0; i < 10; ++i) {
        tt_u8_t u_byte[1000], v_byte[1000];
        tt_u32_t u_n, v_n, k;

        u_n = tt_rand_u32() % sizeof(u_byte);
        if (u_n < 500)
            u_n += 400;
        for (k = 0; k < u_n; ++k)
            u_byte[k] = (tt_u8_t)tt_rand_u32();

        v_n = tt_rand_u32() % sizeof(v_byte);
        if (v_n < 500)
            v_n += 400;
        for (k = 0; k < v_n; ++k)
            v_byte[k] = (tt_u8_t)tt_rand_u32();

        ret = tt_mpn_set(&u, u_byte, u_n, TT_FALSE, 0);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        if (i % 3 == 0)
            tt_mpn_reverse(NULL, &u);

        ret = tt_mpn_set(&v, v_byte, v_n, TT_FALSE, 0);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        if (i % 2 == 0)
            tt_mpn_reverse(NULL, &v);

        start = tt_time_ref();
        ret = tt_mpn_gcd(&d, &u, &v, &bc);
        end = tt_time_ref();
        t += end - start;

        if (tt_mpn_zero(&d))
            continue;

        TT_TEST_CHECK_EQUAL(d.negative, TT_FALSE, "");

        ret = tt_mpn_div(&tmp1, &r, &u, &d, &bc);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        TT_TEST_CHECK_EQUAL(tt_mpn_zero(&r), TT_TRUE, "");

        ret = tt_mpn_div(&tmp2, &r, &v, &d, &bc);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        TT_TEST_CHECK_EQUAL(tt_mpn_zero(&r), TT_TRUE, "");

        // should be pairwise prime
        ret = tt_mpn_gcd(&r, &tmp1, &tmp2, &bc);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        TT_TEST_CHECK_EQUAL(tt_mpn_cmp_u(&r, 1, TT_FALSE), 0, "");

        if (i % 1) {
            ret = tt_mpn_gcd(&tmp1, &tmp1, &tmp2, &bc);
            TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
            TT_TEST_CHECK_EQUAL(tt_mpn_cmp(&tmp1, &r), 0, "");
        } else {
            ret = tt_mpn_gcd(&tmp2, &tmp1, &tmp2, &bc);
            TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
            TT_TEST_CHECK_EQUAL(tt_mpn_cmp(&tmp2, &r), 0, "");
        }
    }
    TT_RECORD_INFO("gcd time: %dms", tt_time_ref2ms(t));

    tt_mpn_destroy(&u);
    tt_mpn_destroy(&v);
    tt_mpn_destroy(&d);
    tt_mpn_destroy(&r);
    tt_mpn_destroy(&correct_d);
    tt_mpn_destroy(&tmp1);
    tt_mpn_destroy(&tmp2);

    tt_mpn_cache_destroy(&bc);

    // test end
    TT_TEST_CASE_LEAVE()
}

typedef struct
{
    const tt_char_t *a;
    const tt_char_t *m;
    tt_result_t ret;
} __mi_case_t;

__mi_case_t mi_case[] = {
    {
        "0x100",
        "0x0",
        TT_FAIL // m should > 1
    },
    {
        "0x100",
        "0x1",
        TT_FAIL // m should > 1
    },
    {"0x1",
     "-0xAB", // m should > 1
     TT_FAIL},
    {
        "-0x1",
        "0x100",
        TT_FAIL // a should > 0
    },
    {
        "0x0",
        "0x100",
        TT_FAIL // a should > 0
    },
    {"0x2", "0xAB", TT_SUCCESS},
    {"0x1b0e72c08e02b3000000000",
     "0x20753adafd1e8c000000000000000000",
     TT_FAIL},
    {"0x1B0E72C08E02B2911A05561",
     "0x20753ADAFD1E8B9F53686D013143EE01",
     TT_SUCCESS},
};

TT_TEST_ROUTINE_DEFINE(tt_unit_test_mpn_minv)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_mpn_t a, m, inv, tmp;
    tt_result_t ret;
    tt_u32_t i;
    tt_mpn_cache_t mc;

    TT_TEST_CASE_ENTER()
    // test start

    tt_mpn_init(&a);
    tt_mpn_init(&m);
    tt_mpn_init(&inv);
    tt_mpn_init(&tmp);

    ret = tt_mpn_cache_create(&mc);
    TT_TEST_CHECK_SUCCESS(ret, "");

    for (i = 0; i < sizeof(mi_case) / sizeof(mi_case[0]); ++i) {
        ret = tt_mpn_set_cstr(&a, mi_case[i].a, TT_MPN_FMT_HEX, 0);
        TT_TEST_CHECK_SUCCESS(ret, "");
        ret = tt_mpn_set_cstr(&m, mi_case[i].m, TT_MPN_FMT_HEX, 0);
        TT_TEST_CHECK_SUCCESS(ret, "");

        ret = tt_mpn_modminv(&inv, &a, &m, &mc);
        TT_TEST_CHECK_EQUAL(ret, mi_case[i].ret, "");
        // tt_mpn_show("", &inv, 0);
        if (TT_OK(ret)) {
            ret = tt_mpn_mul(&tmp, &a, &inv, &mc);
            TT_TEST_CHECK_SUCCESS(ret, "");

            ret = tt_mpn_mod(&tmp, &tmp, &m, &mc);
            TT_TEST_CHECK_SUCCESS(ret, "");

            TT_TEST_CHECK_EQUAL(tt_mpn_cmp_u(&tmp, 1, TT_FALSE), 0, "");
        } else if (!((tt_mpn_cmp_u(&a, 0, TT_FALSE) <= 0) ||
                     (tt_mpn_cmp_u(&m, 1, TT_FALSE) <= 0))) {
            ret = tt_mpn_gcd(&tmp, &a, &m, &mc);
            TT_TEST_CHECK_NOT_EQUAL(tt_mpn_cmp_u(&tmp, 1, TT_FALSE), 0, "");
        }
    }

    for (i = 0; i < 100; ++i) {
        tt_u8_t u_byte[1000], v_byte[1000];
        tt_u32_t u_n, v_n, k;

        u_n = tt_rand_u32() % sizeof(u_byte);
        if (u_n < 500)
            u_n += 400;
        for (k = 0; k < u_n; ++k)
            u_byte[k] = (tt_u8_t)tt_rand_u32();

        v_n = tt_rand_u32() % sizeof(v_byte);
        if (v_n < 500)
            v_n += 400;
        for (k = 0; k < v_n; ++k)
            v_byte[k] = (tt_u8_t)tt_rand_u32();

        ret = tt_mpn_set(&a, u_byte, u_n, TT_FALSE, 0);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        ret = tt_mpn_set(&m, v_byte, v_n, TT_FALSE, 0);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        ret = tt_mpn_modminv(&inv, &a, &m, &mc);
        if (TT_OK(ret)) {
            ret = tt_mpn_mul(&tmp, &a, &inv, &mc);
            TT_TEST_CHECK_SUCCESS(ret, "");

            ret = tt_mpn_mod(&tmp, &tmp, &m, &mc);
            TT_TEST_CHECK_SUCCESS(ret, "");

            TT_TEST_CHECK_EQUAL(tt_mpn_cmp_u(&tmp, 1, TT_FALSE), 0, "");
        } else if (!tt_mpn_zero(&a) && (tt_mpn_cmp_uabs(&m, 1) > 0)) {
            ret = tt_mpn_gcd(&tmp, &a, &m, &mc);
            TT_TEST_CHECK_NOT_EQUAL(tt_mpn_cmp_u(&tmp, 1, TT_FALSE), 0, "");
        }
    }

    tt_mpn_destroy(&a);
    tt_mpn_destroy(&m);
    tt_mpn_destroy(&inv);
    tt_mpn_destroy(&tmp);

    // test end
    TT_TEST_CASE_LEAVE()
}
