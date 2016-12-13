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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_mpn_mul_basic)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_mpn_sqr)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(bnmul_case)

TT_TEST_CASE("tt_unit_test_mpn_mul_basic",
             "mp integer multiply: simple",
             tt_unit_test_mpn_mul_basic,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_mpn_sqr",
                 "mp integer squaring",
                 tt_unit_test_mpn_sqr,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(bnmul_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(MPN_MUL, 0, bnmul_case)

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
    const tt_char_t *b;
    const tt_char_t *ret;
} __bn_mul_case_t;

//////////////////////////////////////////////////////
// case 1

tt_u8_t c1_a[] = {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0};
tt_u8_t c1_b[] = {0x1f};
tt_u8_t c1_r[] = {0x2, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xFF, 0x10};

//////////////////////////////////////////////////////
// case 1

//////////////////////////////////////////////////////
__bn_mul_case_t __bn_mul_case[] = {
    // case 0
    {
        "0x11112222333344445555666677778888",
        "0x22223333444455556666777788881111",
        "0x2468E38E81B543216C1740DB05B06D38BF24C3B1C71B851DB9741FDA7411908",
    },
    {
        "0x11112222333344445555666677778888",
        "0x6666777788881111",
        "0x6D3A86428F5C3B2A1907F6E5D4C3B29DB9741FDA7411908",
    },

    // case 1
    {
        "+0x123456789abcdef0", "-1f", "-23456789abcdEFf10",
    },

    // case 2
    {"0x789abcdef0"
     "123456789abcdef0"
     "123456789abcdef0"
     "123456789abcdef0"
     "123456789abcdef0",

     "-f0"
     "123456789abcdef0"
     "123456789abcdef0",

     "   -7119A49A73E8"
     "1C99E086CB3641C4"
     "C3C74FEB5E489CCA"
     "5F6BEFEB5E489CCA"
     "5F6BEFEB5E489CB9"
     "4D0F77FE1940EEDC"
     "A5E20890F2A52100"},

    // case 3: set __BM_THRESHOLD_A_SP_BLOCK to 20 and __BM_THRESHOLD_B_SP to 20
    // to do test
    {"  "
     "789abcdef0aaaa123456789abcdef0123456789abcdef0123456789abcdef0123456789ab"
     "cdef0"
     "123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789"
     "abcdef0"
     "123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789"
     "abcdef0"
     "123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789"
     "abcdef0"

     "123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789"
     "abcdef0"
     "123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789"
     "abcdef0"
     "123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789"
     "abcdef0"
     "123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789"
     "abcdef0"

     "123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789"
     "abcdef0"
     "123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789"
     "abcdef0"
     "123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789"
     "abcdef0"
     "123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789"
     "abcdef0",

     "             "
     "-f0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0"
     "123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789"
     "abcdef0"
     "123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789"
     "abcdef0",
     "-7119A49A747715D1532380F406562FADFA50F0612CF1FD8AA17E5FCE538DCB6748ABCF3B"
     "7A299943EFD93EA8A0C567209706AE15C76134FD3E341D82EDFD02D9E5618CF01498D0B68"
     "C8EFC5D3B349E9333BC6BCA61D06C6FDAE9DB37886C3A4C82174AA4AF0808292944BA11D5"
     "A3D605D06995F589688D82358D29092D964322358D29092D964322358D29092D964322358"
     "D29092D964322358D29092D964322358D29092D964322358D29092D964322358D29092D96"
     "4322358D29092D964322358D29092D964322358D29092D964322358D29092D964322358D2"
     "9092D964322358D29092D964322358D29092D964322358D29092D964322358D29092D9643"
     "22358D29092D964322358D29092D964322358D29092D964322358D29092D964322358D290"
     "92D964322358D29092D964322358D29092D964322358D29092D964322358D29092D964322"
     "358D29092D964322358D29092D964322358D29092D964322358D29092D964322358D29092"
     "D964322358D29092D964322358D29092D964322358D29092D964322358D29092D96432235"
     "8D29092D964322358D29092D964322358D29092D964322358D29092D964322358D29092D9"
     "64322358D29092D964322358D29092D964322358D29092D964322358D29092D964322358D"
     "29092D9643112330B11BE88E95347C0341AEC1F2C757D4D5D2419B56F97B2DA862D474BB2"
     "B9E867AF3674E1F5DC1DF4D83FA27838FE53820148D00E7C20890F2A51FDA4BF42BE9C535"
     "B2B3B0264F4297C6458D1458729B6A56D866788A95F43CE76B3FDCBCB94D0F77FE1940EED"
     "CA5E20890F2A52100"},

    // case 4: set __BM_THRESHOLD_A_SP_BLOCK to 20 and __BM_THRESHOLD_B_SP to 20
    // to do test
    {"f0123456789abcdef0"
     "123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789"
     "abcdef0"

     "123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789"
     "abcdef0"
     "123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789"
     "abcdef0"
     "123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789"
     "abcdef0"
     "123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789"
     "abcdef0"

     "123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789"
     "abcdef0"
     "123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789"
     "abcdef0"
     "123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789"
     "abcdef0"
     "123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789"
     "abcdef0",

     "             "
     "-f0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0"
     "123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789"
     "abcdef0"
     "123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789"
     "abcdef0",
     "-E122236D88FE5618AEFECA9AF86B7CB47CDB71C867D8A3504AB818F5D745C9EC1894C023"
     "46B2F087E6716750B6201723B44E0E7E258D3DBF822AB5AB94FA645B50075CD904678AF71"
     "DE4040673D4B192EBC0AB33E341D82EB99D526152AEFECA8779F98EC21C25665556A0BC31"
     "894C022322358D29092D964322358D29092D964322358D29092D964322358D29092D96432"
     "2358D29092D964322358D29092D964322358D29092D964322358D29092D964322358D2909"
     "2D964322358D29092D964322358D29092D964322358D29092D964322358D29092D9643223"
     "58D29092D964322358D29092D964322358D29092D964322358D29092D964322358D29092D"
     "964322358D29092D964322358D29092D964322358D29092D964322358D29092D964322358"
     "D29092D964322358D29092D964322358D29092D964322358D29092D964322358D29092D96"
     "4322358D29092D964322358D29092D964322358D29092D964322358D29092D964322358D2"
     "9092D9643112330B11BE88E95347C0341AEC1F2C757D4D5D2419B56F97B2DA862D474BB2B"
     "9E867AF3674E1F5DC1DF4D83FA27838FE53820148D00E7C20890F2A51FDA4BF42BE9C535B"
     "2B3B0264F4297C6458D1458729B6A56D866788A95F43CE76B3FDCBCB94D0F77FE1940EEDC"
     "A5E20890F2A52100"},

    // case 5: set __BM_THRESHOLD_A_SP_BLOCK to 20 and __BM_THRESHOLD_B_SP to 20
    // to do test
    {"-123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef012345678"
     "9abcdef0"
     "123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789"
     "abcdef0"
     "123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789"
     "abcdef0"
     "123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789"
     "abcdef0"

     "123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789"
     "abcdef0"
     "123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789"
     "abcdef0"
     "123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789"
     "abcdef0"
     "123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789"
     "abcdef0"

     "123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789"
     "abcdef0"
     "123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789"
     "abcdef0"
     "123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789"
     "abcdef0"
     "123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789"
     "abcdef0",

     "             "
     "-f0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0"
     "123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789"
     "abcdef0"
     "123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789"
     "abcdef0",
     "11125C77ED4507ADEDB989E75A6BA37BCA60B756C7923F49A707E4C634B8DB1783AF1235A"
     "1DF76E560563FA50F0612B33CFD6D147C2CAE8119A49A83E9534A4EF64BC7F35679E61CD2"
     "F2F562C3A081EAAF9A22D230C71DB88C4150419DEDB98668E87DB10B145554458FAB20783"
     "AF12222358D29092D964322358D29092D964322358D29092D964322358D29092D96432235"
     "8D29092D964322358D29092D964322358D29092D964322358D29092D964322358D29092D9"
     "64322358D29092D964322358D29092D964322358D29092D964322358D29092D964322358D"
     "29092D964322358D29092D964322358D29092D964322358D29092D964322358D29092D964"
     "322358D29092D964322358D29092D964322358D29092D964322358D29092D964322358D29"
     "092D964322358D29092D964322358D29092D964322358D29092D964322358D29092D96432"
     "2358D29092D964322358D29092D964322358D29092D964322358D29092D964322358D2909"
     "2D964322358D29092D964322358D29092D964322358D29092D964322358D29092D9643223"
     "58D29092D964322358D29092D964322358D29092D964322358D29092D964322358D29092D"
     "964322358D29092D964322358D29092D964322358D29092D964322358D29092D964322358"
     "D29092D9643112330B11BE88E95347C0341AEC1F2C757D4D5D2419B56F97B2DA862D474BB"
     "2B9E867AF3674E1F5DC1DF4D83FA27838FE53820148D00E7C20890F2A51FDA4BF42BE9C53"
     "5B2B3B0264F4297C6458D1458729B6A56D866788A95F43CE76B3FDCBCB94D0F77FE1940EE"
     "DCA5E20890F2A52100"},
};

//////////////////////////////////////////////////////

TT_TEST_ROUTINE_DEFINE(tt_unit_test_mpn_mul_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_u32_t i;
    tt_mpn_t a, b, r, answer;
    tt_result_t ret;
    tt_s32_t cmp_ret;
    tt_mpn_cache_t bc;

    TT_TEST_CASE_ENTER()
    // test start

    tt_mpn_init(&a);
    tt_mpn_init(&b);
    tt_mpn_init(&r);
    tt_mpn_init(&answer);

    ret = tt_mpn_cache_create(&bc);
    TT_TEST_CHECK_SUCCESS(ret, "");

    ret = tt_mpn_mul(&a, &a, &a, &bc);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(tt_mpn_zero(&a), TT_TRUE, "");

    for (i = 0; i < 1; // sizeof(__bn_mul_case)/sizeof(__bn_mul_case[0]);
         ++i) {
        __bn_mul_case_t *tc = &__bn_mul_case[i];

        ret = tt_mpn_set_cstr(&a, tc->a, TT_MPN_FMT_AUTO, 0);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_mpn_set_cstr(&b, tc->b, TT_MPN_FMT_AUTO, 0);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_mpn_set_cstr(&answer, tc->ret, TT_MPN_FMT_AUTO, 0);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        ret = tt_mpn_mul(&r, &a, &b, &bc);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        cmp_ret = tt_mpn_cmp(&r, &answer);
        if (cmp_ret != 0) {
            tt_char_t c_idx[30] = {0};
            tt_snprintf(c_idx, sizeof(c_idx) - 1, "bn mul fail case: %d", i);
            TT_RECORD_ERROR(c_idx);
        }
        TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

        ret = tt_mpn_mul(&a, &a, &a, &bc);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    }

    tt_mpn_destroy(&a);
    tt_mpn_destroy(&b);
    tt_mpn_destroy(&r);
    tt_mpn_destroy(&answer);

    tt_mpn_cache_destroy(&bc);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_mpn_sqr)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_mpn_t a, s, correct;
    tt_result_t ret;
    tt_mpn_cache_t bc;

    TT_TEST_CASE_ENTER()
    // test start

    tt_mpn_init(&a);
    tt_mpn_init(&s);
    tt_mpn_init(&correct);

    ret = tt_mpn_cache_create(&bc);
    TT_TEST_CHECK_SUCCESS(ret, "");

    ret = tt_mpn_set_cstr(&a, "0x0", TT_MPN_FMT_AUTO, 0);
    TT_TEST_CHECK_SUCCESS(ret, "");
    ret = tt_mpn_set_cstr(&correct, "0x0", TT_MPN_FMT_AUTO, 0);
    TT_TEST_CHECK_SUCCESS(ret, "");
    ret = tt_mpn_sqr(&s, &a, &bc);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_EXP(tt_mpn_cmp(&s, &correct) == 0, "");

    ret = tt_mpn_set_cstr(&a, "0x1", TT_MPN_FMT_AUTO, 0);
    TT_TEST_CHECK_SUCCESS(ret, "");
    ret = tt_mpn_set_cstr(&correct, "0x1", TT_MPN_FMT_AUTO, 0);
    TT_TEST_CHECK_SUCCESS(ret, "");
    ret = tt_mpn_sqr(&s, &a, &bc);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_EXP(tt_mpn_cmp(&s, &correct) == 0, "");

    ret = tt_mpn_set_cstr(&a,
                          "-0xffffffffffffffffffffffffffffffff",
                          TT_MPN_FMT_AUTO,
                          0);
    TT_TEST_CHECK_SUCCESS(ret, "");
    ret = tt_mpn_set_cstr(&correct,
                          "0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFE000000000000000000"
                          "000"
                          "00000000001",
                          TT_MPN_FMT_AUTO,
                          0);
    TT_TEST_CHECK_SUCCESS(ret, "");
    ret = tt_mpn_sqr(NULL, &a, &bc);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_EXP(tt_mpn_cmp(&a, &correct) == 0, "");

    ret = tt_mpn_set_cstr(&a,
                          "0xffff222233334444ffff222233334444ffff222233334444",
                          TT_MPN_FMT_AUTO,
                          0);
    TT_TEST_CHECK_SUCCESS(ret, "");
    ret = tt_mpn_set_cstr(&correct,
                          "0xFFFE444526AF3C4EB9723211AAAAAAAC72E61FDE2EA619082C"
                          "5D8"
                          "52065430ECA72E99753E147A06CB975A9875D4C3210",
                          TT_MPN_FMT_AUTO,
                          0);
    TT_TEST_CHECK_SUCCESS(ret, "");
    ret = tt_mpn_sqr(NULL, &a, &bc);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_EXP(tt_mpn_cmp(&a, &correct) == 0, "");

    do {
        tt_u8_t __p_a[150];
        tt_u32_t i;
        tt_s64_t start, e, t_mul = 0, t_sqr = 0;
        tt_mpn_t another;

        for (i = 0; i < sizeof(__p_a); ++i) {
            __p_a[i] = (tt_u8_t)tt_rand_u32();
        }

        tt_mpn_set(&a, __p_a, sizeof(__p_a), TT_FALSE, 0);

        tt_mpn_init(&another);
        tt_mpn_copy(&another, &a, 0);

        start = tt_time_ref();
        for (i = 0; i < 1000; ++i) {
            tt_mpn_mul(&correct, &a, &another, &bc);
        }
        e = tt_time_ref();
        t_mul += e - start;

        start = tt_time_ref();
        for (i = 0; i < 1000; ++i) {
            tt_mpn_sqr(&s, &a, &bc);
        }
        e = tt_time_ref();
        t_sqr += e - start;

        TT_TEST_CHECK_EQUAL(tt_mpn_cmp(&s, &correct), 0, "");

        TT_RECORD_INFO("t_mul: %dms, t_sqr: %dms",
                       tt_time_ref2ms(t_mul),
                       tt_time_ref2ms(t_sqr));
    } while (0);

    tt_mpn_destroy(&a);
    tt_mpn_destroy(&s);
    tt_mpn_destroy(&correct);

    tt_mpn_cache_destroy(&bc);

    // test end
    TT_TEST_CASE_LEAVE()
}
