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

//#define __VS_OPENSSL
#ifdef __VS_OPENSSL
#include <openssl/bn.h>
#endif

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

extern tt_result_t __mpn_rdmont(OUT tt_mpn_t *ret,
                                IN tt_mpn_t *t,
                                IN tt_mpn_t *m,
                                IN tt_mpn_unit_t m1,
                                IN tt_mpn_cache_t *mpnc);

extern tt_result_t __mpn_mulmont(OUT tt_mpn_t *ret,
                                 IN tt_mpn_t *a,
                                 IN tt_mpn_t *b,
                                 IN tt_mpn_t *m,
                                 IN tt_mpn_unit_t m1,
                                 IN tt_mpn_cache_t *mpnc);

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

// === routine declarations ================
TT_TEST_ROUTINE_DECLARE(tt_unit_test_mpn_mont_m1)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_mpn_rdmont)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_mpn_mulmont)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_mpn_modexp)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(mpexp_case)
#if 1
TT_TEST_CASE("tt_unit_test_mpn_mont_m1",
             "mp integer montgomery mul inv",
             tt_unit_test_mpn_mont_m1,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_mpn_rdmont",
                 "mp integer montgomery redunction",
                 tt_unit_test_mpn_rdmont,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),
#endif

    TT_TEST_CASE("tt_unit_test_mpn_mulmont",
                 "mp integer montgomery multiply",
                 tt_unit_test_mpn_mulmont,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_mpn_modexp",
                 "mp integer modular exponentiation",
                 tt_unit_test_mpn_modexp,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(mpexp_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(MPN_EXP, 0, mpexp_case)

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

    TT_TEST_ROUTINE_DEFINE(tt_unit_test_mpn_mont_m1)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_mpn_t m, t1, t2, r;
    tt_mpn_cache_t mc;
    tt_u32_t i, k;
    tt_result_t ret;
    tt_mpn_unit_t m1;

    TT_TEST_CASE_ENTER()
    // test start

    tt_mpn_init(&m);
    tt_mpn_init(&r);
    tt_mpn_init(&t1);
    tt_mpn_init(&t2);

    tt_mpn_set_u(&r, 1, TT_FALSE, 0);
    ret = tt_mpn_lshf_u(NULL, &r, 1);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_mpn_cache_create(&mc);

    //==============================================
    ret = tt_mpn_set_cstr(
        &m,
        "0xE3D47BC677E0FE89EB995040AFEB7BB627B3E85101C8D3C37C59C"
        "4967C264E3D8ED97FD965585843A0C9E6AD8D6001ACB809FC6D1E08"
        "2A973D6ADFB90999FC680AA6D027BE2CBE52A3A41E6469D2BE88D60"
        "C0392E80C3084F5D194CFC43BD50135230DA9ACECFDD62B41614D03"
        "34A35172CBD216B3F2536D1A26EFDA5BA8D90B",
        TT_MPN_FMT_HEX,
        0);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_mpn_mont_m1(&m1, &m, &mc);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_mpn_sub_u(&t1, &r, m1, TT_FALSE);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_mpn_muleq(&t1, &m, &mc);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_mpn_modeq(&t1, &r, &mc);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(tt_mpn_cmp_u(&t1, 1, TT_FALSE), 0, "");
    //==============================================

    for (i = 0; i < 100; ++i) {
        tt_u8_t u_byte[1000];
        tt_u32_t u_n;

        u_n = tt_rand_u32() % sizeof(u_byte);
        if (u_n < 500)
            u_n += 400;
        for (k = 0; k < u_n; ++k)
            u_byte[k] = (tt_u8_t)tt_rand_u32();

        ret = tt_mpn_set(&m, u_byte, u_n, TT_FALSE, 0);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");

        // m1 = -(m^-1 mod r)
        ret = tt_mpn_mont_m1(&m1, &m, &mc);
        if (!TT_OK(ret)) {
            ret = tt_mpn_gcd(&t1, &m, &r, &mc);
            TT_UT_EQUAL(ret, TT_SUCCESS, "");

            TT_UT_NOT_EQUAL(tt_mpn_cmp_u(&t1, 1, TT_FALSE), 0, "");

            continue;
        }

        // mod neg
        ret = tt_mpn_sub_u(&t1, &r, m1, TT_FALSE);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");

        ret = tt_mpn_muleq(&t1, &m, &mc);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_mpn_modeq(&t1, &r, &mc);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");

        // expect (-m1)*m mod r = 1
        TT_UT_EQUAL(tt_mpn_cmp_u(&t1, 1, TT_FALSE), 0, "");
    }

    tt_mpn_destroy(&m);
    tt_mpn_destroy(&t1);
    tt_mpn_destroy(&t2);

    tt_mpn_cache_destroy(&mc);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_mpn_rdmont)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_mpn_t m, a, x, t2, t3;
    tt_mpn_cache_t mc;
    tt_u32_t i, k;
    tt_result_t ret;

    TT_TEST_CASE_ENTER()
    // test start

    tt_mpn_init(&m);
    tt_mpn_init(&a);
    tt_mpn_init(&x);
    tt_mpn_init(&t2);
    tt_mpn_init(&t3);

    tt_mpn_cache_create(&mc);

    for (i = 0; i < 100; ++i) {
        tt_u8_t u_byte[1000], v_byte[1000];
        tt_u32_t u_n, v_n;
        tt_mpn_unit_t m1;
        tt_s32_t cmp_ret;

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

        ret = tt_mpn_set(&m, u_byte, u_n, TT_FALSE, 0);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");

        ret = tt_mpn_set(&a, v_byte, v_n, TT_FALSE, 0);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");

        if (a.unit_num > m.unit_num * 2)
            a.unit_num = m.unit_num * 2;

        if ((a.unit_num == (m.unit_num * 2)) &&
            (a.unit[a.unit_num - 1] >= m.unit[m.unit_num - 1])) {
            if (m.unit[m.unit_num - 1] != 0)
                a.unit[a.unit_num - 1] = m.unit[m.unit_num - 1] - 1;
            else {
                a.unit[a.unit_num - 1] = 1;
                m.unit[m.unit_num - 1] = 2;
            }
        }

        // m1 = -(m^-1 mod r)
        ret = tt_mpn_mont_m1(&m1, &m, &mc);
        if (!TT_OK(ret)) {
            continue;
        }

        // mont:
        //  m has n units, x=r^n
        //  return a*x^-1 mod m
        ret = __mpn_rdmont(&t2, &a, &m, m1, &mc);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");

        // x=r^n
        ret = tt_mpn_ulshf_u(&x, 1, TT_FALSE, m.unit_num);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        // x^-1 mod m
        ret = tt_mpn_modminv(&x, &x, &m, &mc);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        // a*x^-1
        ret = tt_mpn_muleq(&x, &a, &mc);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        // a*x^-1 mod m
        ret = tt_mpn_modeq(&x, &m, &mc);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");

        cmp_ret = tt_mpn_cmp(&x, &t2);
        if (cmp_ret != 0) {
            tt_mpn_show("x: \n", &x, 0);
            tt_mpn_show("t2: \n", &t2, 0);
        }
        TT_UT_EXP(cmp_ret == 0, "");
    }

    tt_mpn_destroy(&m);
    tt_mpn_destroy(&a);
    tt_mpn_destroy(&x);
    tt_mpn_destroy(&t3);
    tt_mpn_destroy(&t2);

    tt_mpn_cache_destroy(&mc);

    // test end
    TT_TEST_CASE_LEAVE()
}

//////////////////////////////////////////////////////////////

typedef struct
{
    const tt_char_t *a;
    const tt_char_t *b;
    const tt_char_t *m;
} __mulmont_case_t;

__mulmont_case_t __mulmont_case[] = {
    {
        "0x1111222233334444aaaabbbbccccdddd",
        "0x5555666677778888ccccddddeeeeffff",
        "0xffff666677778888ccccddddeeeeffff",
    },
    {
        "0x1111222233334444aaaabbbbccccdddd",
        "0x5555666677778888ccccddddeeeeffff",
        "0x123ffff666677778888ccccddddeeeeffff",
    },
};

TT_TEST_ROUTINE_DEFINE(tt_unit_test_mpn_mulmont)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_mpn_t m, a, b, rn, x, t2, t3;
    tt_mpn_unit_t m1;
    tt_mpn_cache_t mc;
    tt_u32_t i, k;
    tt_result_t ret;
    tt_s64_t start, end, t = 0;

    TT_TEST_CASE_ENTER()
    // test start

    tt_mpn_init(&m);
    tt_mpn_init(&a);
    tt_mpn_init(&b);
    tt_mpn_init(&rn);
    tt_mpn_init(&x);
    tt_mpn_init(&t2);
    tt_mpn_init(&t3);

    tt_mpn_cache_create(&mc);

    for (i = 0; i < sizeof(__mulmont_case) / sizeof(__mulmont_case[0]); ++i) {
        __mulmont_case_t *c = &__mulmont_case[i];

        ret = tt_mpn_set_cstr(&a, c->a, TT_MPN_FMT_HEX, 0);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_mpn_set_cstr(&b, c->b, TT_MPN_FMT_HEX, 0);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_mpn_set_cstr(&m, c->m, TT_MPN_FMT_HEX, 0);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_mpn_mont_m1(&m1, &m, &mc);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        ret = __mpn_mulmont(&x, &a, &b, &m, m1, &mc);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");

        // a*b*rn^-1 mod m
        ret = tt_mpn_ulshf_u(&rn, 1, TT_FALSE, m.unit_num);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_mpn_modminv(&rn, &rn, &m, &mc);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_mpn_mul(&t2, &a, &b, &mc);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_mpn_muleq(&t2, &rn, &mc);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_mpn_modeq(&t2, &m, &mc);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");

        TT_UT_EQUAL(tt_mpn_cmp(&t2, &x), 0, "");
    }

    for (i = 0; i < 100; ++i) {
        tt_u8_t u_byte[1000], v_byte[1000], w_byte[1500];
        tt_u32_t u_n, v_n, w_n;

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

        w_n = TT_MAX(u_n, v_n);
        w_n += tt_rand_u32() % (sizeof(w_byte) - w_n);
        for (k = 0; k < w_n; ++k)
            w_byte[k] = (tt_u8_t)tt_rand_u32();
        if ((w_n == u_n) && (u_byte[0] >= w_byte[0])) {
            u_byte[0] = 1;
            w_byte[0] = 2;
        }
        if ((w_n == v_n) && (v_byte[0] >= w_byte[0])) {
            v_byte[0] = 1;
            w_byte[0] = 2;
        }

        ret = tt_mpn_set(&a, u_byte, u_n, TT_FALSE, 0);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_mpn_set(&b, v_byte, v_n, TT_FALSE, 0);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_mpn_set(&m, w_byte, w_n, TT_FALSE, 0);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        start = tt_time_ref();
        ret = tt_mpn_mont_m1(&m1, &m, &mc);
        end = tt_time_ref();
        t += end - start;
        if (!TT_OK(ret)) {
            continue;
        }
        ret = __mpn_mulmont(&x, &a, &b, &m, m1, &mc);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");

        // a*b*rn^-1 mod m
        ret = tt_mpn_ulshf_u(&rn, 1, TT_FALSE, m.unit_num);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_mpn_modminv(&rn, &rn, &m, &mc);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_mpn_mul(&t2, &a, &b, &mc);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_mpn_muleq(&t2, &rn, &mc);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_mpn_modeq(&t2, &m, &mc);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");

        TT_UT_EQUAL(tt_mpn_cmp(&t2, &x), 0, "");
    }
    TT_RECORD_INFO("mulmont time: %dms", tt_time_ref2ms(t));

    tt_mpn_destroy(&m);
    tt_mpn_destroy(&a);
    tt_mpn_destroy(&b);
    tt_mpn_destroy(&rn);
    tt_mpn_destroy(&x);
    tt_mpn_destroy(&t3);
    tt_mpn_destroy(&t2);

    tt_mpn_cache_destroy(&mc);

    // test end
    TT_TEST_CASE_LEAVE()
}

typedef struct
{
    const tt_char_t *a;
    const tt_char_t *e;
    const tt_char_t *m;
    const tt_char_t *aem;
} __modexp_case_t;

__modexp_case_t __modexp_case[] = {
    {"0x123afffffaaabbcdeadccf234567811223345aaabbccddeeff1234",
     "0x1",
     "0x123aaadefccdefcdefcdefcaaabcdef123412341223456789fffffff",
     "0x123afffffaaabbcdeadccf234567811223345aaabbccddeeff1234"},
    {"0x123afffffaaabbcdeadccf234567811223345aaabbccddeeff1234",
     "0x2",
     "0x123aaadefccdefcdefcdefcaaabcdef123412341223456789fffffff",
     "0xEA0A57D7EBE1C47662E353858F0855D48F4860B204BC39054C79AC6"},
    {"0x123afffffaaabbcdeadccf234567811223345aaabbccddeeff1234",
     "0x20000000000000002",
     "0x123aaadefccdefcdefcdefcaaabcdef123412341223456789fffffff",
     "0xC7256572869FDD2810528630ADAB8583863365BC96FD8C423BBB0F4"},
    {"0x123afffffaaabbcdeadccf234567811223345aaabbccddeeff1234",
     "0x123afffffaaabbcdeadccf234567811223345aaabbccddeeff1234",
     "0x123aaadefccdefcdefcdefcaaabcdef123412341223456789fffffff",
     "0x883F458F92A6DE6E52516E9C40980D569C637DA8D4EAB6F837A6A0C"},
    {"0x123afffffaaabbcdeadccf234567811223345aaabbccddeeff123423345aaabbccdde45"
     "67811223345aa23345aaccf234567811223345aaabbccddeeff123423345aaabbccdde456"
     "7811223345a4567811223345aaabbccddeeff123423345aaabbccd23345a4567811223345"
     "aaabbccdd4567811223345aaa5aaccf234567811223345aaabbccddeeff123423345aaabb"
     "ccdde45423345aaabbccdde4567811223345a4567811223345aaabbccddeeff123423345a"
     "aabbccd23345a4567811223345aaabbccdd456781122334bcdeadccf234567811223345aa"
     "abbccddeeff123423345aaabbccdde4567811223345aa23345aaccf234567811223345aaa"
     "bbccddeeff123423345aaabbccdde4567811223345a4567811223345aaabbccddeeff1234"
     "23345aaabbccd23345a4567811223345aaabbccdd4567811223",
     "0x123afffffaaabbcdeadccf234567811223345aaabbccddeeff123423345aaabbccdde45"
     "67811223345aa23345aaccf234567811223345aaabbccddeeff123423345aaabbccdde456"
     "7811223345a4567811223345aaabbccddeeff123423345aaabbccd23345a4567811223345"
     "aaabbccdd4567811223345aaa5aaccf234567811223345aaabbccddeeff123423345aaabb"
     "ccdde45423345aaabbccdde4567811223345a4567811223345aaabbccddeeff123423345a"
     "aabbccd23345a4567811223345aaabbccdd456781122334bcdeadccf234567811223345aa"
     "abbccddeeff123423345aaabbccdde4567811223345aa23345aaccf234567811223345aaa"
     "bbccddeeff123423345aaabbccdde4567811223345a4567811223345aaabbccddeeff1234"
     "23345aaabbccd23345a4567811223345aaabbccdd4567811223",
     "0x123aaadefccdefcdefcdefcaaabcdef123412341223456789fffffffccf234567811223"
     "345aaabbccddeeff1234aacdef1234123456789011aaedcbaaffeedcba12345678aaa1123"
     "4567890987654321aabcdeffedacbba123411233bccddeeff1234aacdef12341234567890"
     "11aaedcbaaffeedccdefcdefcaaabcdef123412341223456789fffffffccf234567811223"
     "345aaabbccddeeff1234aacdef1234123456789011aaedcbaaffeedcba1231aabcdeffeda"
     "cbba123411233bccddeeff1234aacdef1234123456789011aaedcbaaffeedccdefcdefcaa"
     "abcdef123412341223456789fffffffccf011aaedcbaaffeedccdefcdefcaaabcdef12341"
     "2341223456789fffffffccf234567811223345aaabbccddeeff1234aacdef123412345678"
     "9011aae234aacdef1234123456789011aaedcbaaffeedcba1231aabcdeffedacbba123411"
     "233bccddeeff1234aacdef1234123456789011aaedcbaaffeedccdefcdefcaaabcdef1234"
     "12341223456789fffffffccf011aaed",
     "0xC1E95F68ADE3833AD7DA43D385E890520A9AAB3E860C3584AF806DD02D8A0DA7588AA96"
     "6C51DFE253A2725DDD90B9143A8EFBE03D2B69304BD68215896BDA32BBF57AB580387AD66"
     "C2E6D9A837A5E7B1B330AA3A34588333442A2ABA19591B4C4A4E2C7475F21356BFD09C9C2"
     "AA5E3C7F02A55DBE674C297EF4EFB961C980CFB7C063A586BEA26E8B4D42FBE400E8A0851"
     "BB20AD0CD0ED907FDD3409DC24B5FDB3DCDC0429E3BB7BC2329FAA73B62DA547B247BF178"
     "247239B3970D47A61F1AAD2396DC3A916DD3ADBCF4E5A0D3A0088EBBFD50F76BC84DC3BF9"
     "0E326118C14A4159BB5232E381E4E7B8E2C43A14255E40325C599CDF2417B14EE11F8CCD9"
     "C611CD4D552885911A3E3A3F24C0C55A1FD4589AA2156368FC0CD41390CF26852BB95E27C"
     "0D2A6EA3B95275082CD87074A2DF6C520CACCD930A9AECBA658738E40A54E23D0DA075854"
     "CB4AF7D4A6ED2F9FD8D7EF980A99CEFE6BC616374CACFF8222D6B58EFCE7C1E6BD43EBE21"
     "6D37F076DA6C3E8279DEA100DF3B8C"},
    {"0x2", "0x5", "0x1f", "0x1"},
    {"0x2",
     "0x7C6B8E00088C15BD5EDDEA136DFB63B65CDF0198387423209B86FB3FEA0E4039BDC8DBD"
     "18788751A26CC1FE57B503F19A2E5A152D35D45B3AA78537887712279F4BDE49448101485"
     "F7D58951F8E66DDFF48DD9CD119F9FA6A0C4B6AB3553E14248CE13E6D6ECBFD3D9C6D3BEA"
     "5DC9C018A43697E9041B0FBF317098B43CF017B",
     "0xE3D47BC677E0FE89EB995040AFEB7BB627B3E85101C8D3C37C59C4967C264E3D8ED97FD"
     "965585843A0C9E6AD8D6001ACB809FC6D1E082A973D6ADFB90999FC680AA6D027BE2CBE52"
     "A3A41E6469D2BE88D60C0392E80C3084F5D194CFC43BD50135230DA9ACECFDD62B41614D0"
     "334A35172CBD216B3F2536D1A26EFDA5BA8D90B",
     "0xCD991373D465988001937927B751CDF46A7C92D4DCDDB42267BAEAC0E9BF2818A9DE3DD"
     "F1732C3CD5B3DA98E27DED2762A07EC00424DE4F15D35BAED8D6DB36DE363CCB849642673"
     "5E1F3556B85604ECF1AD007519872ADAB385090B43CF623D07DB6546944146678B08BF026"
     "57C20E9B4B1CB24B7501D4AE89959B55FF6B7D4"},
};

TT_TEST_ROUTINE_DEFINE(tt_unit_test_mpn_modexp)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_mpn_t m, a, e, r, correct_r;
    // tt_mpn_montconst_t mcn;
    tt_mpn_cache_t mc;
    tt_u32_t i, k;
    tt_result_t ret;
    tt_s64_t start, end, t = 0;

#ifdef __VS_OPENSSL
    BIGNUM * or, *oa, *oe, *om;
    BN_CTX *octx;
#endif

    TT_TEST_CASE_ENTER()
    // test start

    tt_mpn_init(&m);
    tt_mpn_init(&a);
    tt_mpn_init(&e);
    tt_mpn_init(&r);
    tt_mpn_init(&correct_r);

    tt_mpn_cache_create(&mc);

    for (i = 0; i < sizeof(__modexp_case) / sizeof(__modexp_case[0]); ++i) {
        __modexp_case_t *c = &__modexp_case[i];

        ret = tt_mpn_set_cstr(&a, c->a, TT_MPN_FMT_HEX, 0);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_mpn_set_cstr(&e, c->e, TT_MPN_FMT_HEX, 0);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_mpn_set_cstr(&m, c->m, TT_MPN_FMT_HEX, 0);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_mpn_set_cstr(&correct_r, c->aem, TT_MPN_FMT_HEX, 0);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");

        ret = tt_mpn_powmod(&r, &a, &e, &m, &mc);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(tt_mpn_cmp(&correct_r, &r), 0, "");
    }

#if 1
#ifdef __VS_OPENSSL
    or = BN_new();
    oa = BN_new();
    oe = BN_new();
    om = BN_new();
    octx = BN_CTX_new();
#endif
    for (i = 0; i < 10; ++i) {
        tt_u8_t u_byte[1000], v_byte[1000], w_byte[1500];
        tt_u32_t u_n, v_n, w_n;

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

        w_n = TT_MAX(u_n, v_n);
        w_n += tt_rand_u32() % (sizeof(w_byte) - w_n);
        for (k = 0; k < w_n; ++k)
            w_byte[k] = (tt_u8_t)tt_rand_u32();
        if ((w_n == u_n) && (u_byte[0] >= w_byte[0])) {
            u_byte[0] = 1;
            w_byte[0] = 2;
        }
        if ((w_n == v_n) && (v_byte[0] >= w_byte[0])) {
            v_byte[0] = 1;
            w_byte[0] = 2;
        }

        ret = tt_mpn_set(&a, u_byte, u_n, TT_FALSE, 0);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_mpn_set(&e, v_byte, v_n, TT_FALSE, 0);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_mpn_set(&m, w_byte, w_n, TT_FALSE, 0);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        start = tt_time_ref();
        ret = tt_mpn_powmod(&r, &a, &e, &m, &mc);
        end = tt_time_ref();
        TT_INFO("a[%dbits] ^ e[%dbits] mod m[%dbits]: %dms",
                TT_MPN_USIZE_N(a.unit_num) << 3,
                TT_MPN_USIZE_N(e.unit_num) << 3,
                TT_MPN_USIZE_N(m.unit_num) << 3,
                tt_time_ref2ms(end - start));
        t += end - start;
        if (!TT_OK(ret)) {
            continue;
        }

#ifdef __VS_OPENSSL
        BN_bin2bn(u_byte, u_n, oa);
        BN_bin2bn(v_byte, v_n, oe);
        BN_bin2bn(w_byte, w_n, om);
        start = tt_time_ref();
        BN_mod_exp(or, oa, oe, om, octx);
        end = tt_time_ref();
        TT_INFO("openssl: %dms", tt_time_ref2ms(end - start, NULL));
        {
            tt_u8_t t_byte[2000];
            tt_u32_t t_n;

            t_n = BN_num_bytes(or);
            BN_bn2bin(or, t_byte);
            tt_mpn_set(&a, t_byte, t_n, TT_FALSE, 0);
            TT_UT_EQUAL(tt_mpn_cmp(&a, &r), 0, "");
            TT_INFO("equal");
        }
#endif
    }
    TT_RECORD_INFO("mulmont time: %dms", tt_time_ref2ms(t));
#endif

    tt_mpn_destroy(&m);
    tt_mpn_destroy(&a);
    tt_mpn_destroy(&e);
    tt_mpn_destroy(&r);
    tt_mpn_destroy(&correct_r);

    tt_mpn_cache_destroy(&mc);

    // test end
    TT_TEST_CASE_LEAVE()
}
