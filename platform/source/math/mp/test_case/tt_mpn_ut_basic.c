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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_mpn_create)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_mpn_misc)

TT_TEST_ROUTINE_DECLARE(tt_unit_test_mpn_add)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_mpn_addeq)

TT_TEST_ROUTINE_DECLARE(tt_unit_test_mpn_sub)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_mpn_subeq)

TT_TEST_ROUTINE_DECLARE(tt_unit_test_mpn_lshift)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_mpn_rshift)

TT_TEST_ROUTINE_DECLARE(tt_unit_test_mpn_prepend)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_mpn_append)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(mpnbasic_case)

TT_TEST_CASE("tt_unit_test_mpn_create",
             "mp integer basic",
             tt_unit_test_mpn_create,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_mpn_misc",
                 "mp integer basic, misc",
                 tt_unit_test_mpn_misc,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_mpn_add",
                 "mp integer add",
                 tt_unit_test_mpn_add,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),
    TT_TEST_CASE("tt_unit_test_mpn_addeq",
                 "mp integer add equal",
                 tt_unit_test_mpn_addeq,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_mpn_sub",
                 "mp integer substract",
                 tt_unit_test_mpn_sub,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),
    TT_TEST_CASE("tt_unit_test_mpn_subeq",
                 "mp integer substract equal",
                 tt_unit_test_mpn_subeq,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_mpn_lshift",
                 "mp integer left shift",
                 tt_unit_test_mpn_lshift,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),
    TT_TEST_CASE("tt_unit_test_mpn_rshift",
                 "mp integer right shift",
                 tt_unit_test_mpn_rshift,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_mpn_prepend",
                 "mp integer prepend",
                 tt_unit_test_mpn_prepend,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),
    TT_TEST_CASE("tt_unit_test_mpn_append",
                 "mp integer append",
                 tt_unit_test_mpn_append,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(mpnbasic_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(MPN_BASIC, 0, mpnbasic_case)

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

    TT_TEST_ROUTINE_DEFINE(tt_unit_test_mpn_create)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_mpn_t mpn;
    tt_result_t ret;

    tt_u8_t v1[] = {0x0};
    tt_u8_t v2[] = {0x12, 0x34};
    tt_u8_t v3[] = {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0};
    tt_u8_t v4[] = {0x00, 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0};
    tt_u8_t v5[] = {
        0x00, 0x12, 0x34, 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde,
        0xf0, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x11, 0x22,
    };
    tt_u8_t v6[] = {
        0x00, 0x00, 0x00, 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde,
        0xf0, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x11, 0x22,
    };
    tt_u8_t getbuf[100];
    tt_u32_t getbuf_len;

    TT_TEST_CASE_ENTER()
    // test start

    ret = tt_mpn_create(&mpn, NULL, 11, TT_TRUE, 0);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_EQUAL(mpn.unit[0], 0, "");
    TT_TEST_CHECK_EQUAL(mpn.unit_num, 1, "");
    TT_TEST_CHECK_EQUAL(tt_mpn_neg(&mpn), TT_FALSE, "");

    ret = tt_mpn_create(&mpn, v1, 0, TT_TRUE, 0);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_EQUAL(mpn.unit[0], 0, "");
    TT_TEST_CHECK_EQUAL(mpn.unit_num, 1, "");
    TT_TEST_CHECK_EQUAL(tt_mpn_neg(&mpn), TT_FALSE, "");

    //// v1
    ret = tt_mpn_set(&mpn, v1, sizeof(v1), TT_FALSE, 0);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_EQUAL(mpn.unit[0], 0, "");
    TT_TEST_CHECK_EQUAL(mpn.unit_num, 1, "");
    TT_TEST_CHECK_EQUAL(mpn.negative, TT_FALSE, "");

    TT_TEST_CHECK_EQUAL(tt_mpn_bitnum(&mpn), 0, "");
    TT_TEST_CHECK_EQUAL(tt_mpn_bytenum(&mpn), 0, "");

    getbuf_len = sizeof(getbuf);
    ret = tt_mpn_get(&mpn, getbuf, &getbuf_len, NULL, 0);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_EQUAL(getbuf_len, 1, "");
    TT_TEST_CHECK_EQUAL(getbuf[0], 0, "");
    ret = tt_mpn_get(&mpn, getbuf, &getbuf_len, NULL, TT_MPN_GET_SIGNED);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_EQUAL(getbuf_len, 1, "");
    TT_TEST_CHECK_EQUAL(getbuf[0], 0, "");

    //// v2
    ret = tt_mpn_set(&mpn, v2, sizeof(v2), TT_FALSE, 0);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_EQUAL(mpn.unit[0], 0x1234, "");
    TT_TEST_CHECK_EQUAL(mpn.unit_num, 1, "");
    TT_TEST_CHECK_EQUAL(mpn.negative, TT_FALSE, "");

    getbuf_len = sizeof(getbuf);
    ret = tt_mpn_get(&mpn, getbuf, &getbuf_len, NULL, 0);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_EQUAL(getbuf_len, sizeof(v2), "");
    TT_TEST_CHECK_EQUAL(tt_memcmp(getbuf, v2, getbuf_len), 0, "");
    ret = tt_mpn_get(&mpn, getbuf, &getbuf_len, NULL, TT_MPN_GET_SIGNED);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_EQUAL(getbuf_len, sizeof(v2), "");
    TT_TEST_CHECK_EQUAL(tt_memcmp(getbuf, v2, getbuf_len), 0, "");

    //// v3
    ret = tt_mpn_set(&mpn, v3, sizeof(v3), TT_TRUE, 0);
    TT_TEST_CHECK_SUCCESS(ret, "");
#if TT_ENV_IS_64BIT
    TT_TEST_CHECK_EQUAL(mpn.unit[0], 0x123456789abcdef0, "");
    TT_TEST_CHECK_EQUAL(mpn.unit_num, 1, "");
    TT_TEST_CHECK_EQUAL(mpn.negative, TT_TRUE, "");
#else
    TT_TEST_CHECK_EQUAL(mpn.unit[0], 0x9abcdef0, "");
    TT_TEST_CHECK_EQUAL(mpn.unit[1], 0x12345678, "");
    TT_TEST_CHECK_EQUAL(mpn.unit_num, 2, "");
    TT_TEST_CHECK_EQUAL(mpn.negative, TT_TRUE, "");
#endif

    getbuf_len = sizeof(getbuf);
    ret = tt_mpn_get(&mpn, getbuf, &getbuf_len, NULL, 0);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_EQUAL(getbuf_len, sizeof(v3), "");
    TT_TEST_CHECK_EQUAL(tt_memcmp(getbuf, v3, getbuf_len), 0, "");
    ret = tt_mpn_get(&mpn, getbuf, &getbuf_len, NULL, TT_MPN_GET_SIGNED);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_EQUAL(getbuf_len, sizeof(v3), "");
    TT_TEST_CHECK_EQUAL(tt_memcmp(getbuf, v3, getbuf_len), 0, "");

    //// v4
    ret = tt_mpn_set(&mpn, v4, sizeof(v4), TT_FALSE, 0);
    TT_TEST_CHECK_SUCCESS(ret, "");
#if TT_ENV_IS_64BIT
    TT_TEST_CHECK_EQUAL(mpn.unit[0], 0x123456789abcdef0, "");
    TT_TEST_CHECK_EQUAL(mpn.unit_num, 1, "");
    TT_TEST_CHECK_EQUAL(mpn.negative, TT_FALSE, "");
#else
    TT_TEST_CHECK_EQUAL(mpn.unit[0], 0x9abcdef0, "");
    TT_TEST_CHECK_EQUAL(mpn.unit[1], 0x12345678, "");
    TT_TEST_CHECK_EQUAL(mpn.unit[2], 0, "");
    TT_TEST_CHECK_EQUAL(mpn.unit_num, 2, "");
    TT_TEST_CHECK_EQUAL(mpn.negative, TT_FALSE, "");
#endif

    TT_TEST_CHECK_EQUAL(tt_mpn_bitnum(&mpn), ((sizeof(v4) - 1) << 3) - 3, "");
    TT_TEST_CHECK_EQUAL(tt_mpn_bytenum(&mpn), (sizeof(v4) - 1), "");

    //// v5
    ret = tt_mpn_set(&mpn, v5, sizeof(v5), TT_FALSE, 0);
    TT_TEST_CHECK_SUCCESS(ret, "");
#if TT_ENV_IS_64BIT
    TT_TEST_CHECK_EQUAL(mpn.unit[0], 0xaabbccddeeff1122, "");
    TT_TEST_CHECK_EQUAL(mpn.unit[1], 0x123456789abcdef0, "");
    TT_TEST_CHECK_EQUAL(mpn.unit[2], 0x1234, "");
    TT_TEST_CHECK_EQUAL(mpn.unit_num, 3, "");
    TT_TEST_CHECK_EQUAL(mpn.negative, TT_FALSE, "");
#else
    TT_TEST_CHECK_EQUAL(mpn.unit[0], 0xeeff1122, "");
    TT_TEST_CHECK_EQUAL(mpn.unit[1], 0xaabbccdd, "");
    TT_TEST_CHECK_EQUAL(mpn.unit[2], 0x9abcdef0, "");
    TT_TEST_CHECK_EQUAL(mpn.unit[3], 0x12345678, "");
    TT_TEST_CHECK_EQUAL(mpn.unit[4], 0x1234, "");
    TT_TEST_CHECK_EQUAL(mpn.unit_num, 5, "");
    TT_TEST_CHECK_EQUAL(mpn.negative, TT_FALSE, "");
#endif

    TT_TEST_CHECK_EQUAL(tt_mpn_bitnum(&mpn), ((sizeof(v5) - 1) << 3) - 3, "");
    TT_TEST_CHECK_EQUAL(tt_mpn_bytenum(&mpn), (sizeof(v5) - 1), "");

    getbuf_len = sizeof(getbuf);
    ret = tt_mpn_get(&mpn, getbuf, &getbuf_len, NULL, 0);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_EQUAL(getbuf_len, sizeof(v5) - 1, "");
    TT_TEST_CHECK_EQUAL(tt_memcmp(getbuf, v5 + 1, getbuf_len), 0, "");
    ret = tt_mpn_get(&mpn, getbuf, &getbuf_len, NULL, TT_MPN_GET_SIGNED);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_EQUAL(getbuf_len, sizeof(v5) - 1, "");
    TT_TEST_CHECK_EQUAL(tt_memcmp(getbuf, v5 + 1, getbuf_len), 0, "");

    {
        tt_u8_t x1[] = {0xff, 0, 0, 0, 0, 0, 0, 0};
        tt_u8_t x2[] = {0x80, 0, 0, 0, 0, 0, 0, 0, 0xff, 0, 0, 0, 0, 0, 0, 0};

        ret = tt_mpn_set(&mpn, x1, sizeof(x1), TT_FALSE, 0);
        TT_TEST_CHECK_SUCCESS(ret, "");
        getbuf_len = sizeof(getbuf);
        ret = tt_mpn_get(&mpn, getbuf, &getbuf_len, NULL, TT_MPN_GET_SIGNED);
        TT_TEST_CHECK_SUCCESS(ret, "");
        TT_TEST_CHECK_EQUAL(getbuf_len, 9, "");
        TT_TEST_CHECK_EQUAL(getbuf[0], 0, "");
        TT_TEST_CHECK_EQUAL(tt_memcmp(getbuf + 1, x1, getbuf_len - 1), 0, "");

        ret = tt_mpn_set(&mpn, x2, sizeof(x2), TT_FALSE, 0);
        TT_TEST_CHECK_SUCCESS(ret, "");
        getbuf_len = sizeof(getbuf);
        ret = tt_mpn_get(&mpn, getbuf, &getbuf_len, NULL, TT_MPN_GET_SIGNED);
        TT_TEST_CHECK_SUCCESS(ret, "");
        TT_TEST_CHECK_EQUAL(getbuf_len, 17, "");
        TT_TEST_CHECK_EQUAL(getbuf[0], 0, "");
        TT_TEST_CHECK_EQUAL(tt_memcmp(getbuf + 1, x2, getbuf_len - 1), 0, "");
    }

    //// v6
    ret = tt_mpn_set(&mpn, v6, sizeof(v6), TT_FALSE, 0);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_EQUAL(mpn.unit_num, 2, "");

    ret = tt_mpn_set(&mpn, v2, sizeof(v2), TT_TRUE, 0);
    TT_TEST_CHECK_EQUAL(mpn.unit[0], 0x1234, "");
    TT_TEST_CHECK_EQUAL(mpn.unit_num, 1, "");
    TT_TEST_CHECK_EQUAL(mpn.negative, TT_TRUE, "");

    // do shrink
    tt_mpn_normalize(&mpn);
    TT_TEST_CHECK_EQUAL(mpn.unit[0], 0x1234, "");
    TT_TEST_CHECK_EQUAL(mpn.unit_num, 1, "");
    TT_TEST_CHECK_EQUAL(mpn.negative, TT_TRUE, "");

    tt_mpn_normalize(&mpn);
    TT_TEST_CHECK_EQUAL(mpn.unit[0], 0x1234, "");
    TT_TEST_CHECK_EQUAL(mpn.unit_num, 1, "");
    TT_TEST_CHECK_EQUAL(mpn.negative, TT_TRUE, "");

    // zero
    ret = tt_mpn_set(&mpn, v6, sizeof(v6), TT_FALSE, 0);
    TT_TEST_CHECK_SUCCESS(ret, "");
    tt_mpn_clear(&mpn);
    tt_mpn_normalize(&mpn);
    TT_TEST_CHECK_EQUAL(mpn.unit[0], 0, "");
    TT_TEST_CHECK_EQUAL(mpn.unit_num, 1, "");
    TT_TEST_CHECK_NOT_EQUAL(mpn.unit, &mpn.unit_inline, "");
    tt_mpn_refine(&mpn);
    TT_TEST_CHECK_EQUAL(mpn.unit[0], 0, "");
    TT_TEST_CHECK_EQUAL(mpn.unit_num, 1, "");
    TT_TEST_CHECK_EQUAL(mpn.unit, &mpn.unit_inline, "");
    tt_mpn_refine(&mpn);
    TT_TEST_CHECK_EQUAL(mpn.unit[0], 0, "");
    TT_TEST_CHECK_EQUAL(mpn.unit_num, 1, "");

    tt_mpn_destroy(&mpn);

    // test end
    TT_TEST_CASE_LEAVE()
}

///////////////////////////
static tt_u8_t __a1[] = {
    0x92, 0x34, 0x56, 0x78, 0x56, 0x78, 0x78, 0x92, 0x34, 0x56, 0x78, 0x56,
    0x78, 0x78, 0xaa, 0x92, 0x34, 0x56, 0x78, 0x56, 0x78, 0x78, 0xbb,
};
static tt_u8_t __b1[] = {
    0x92, 0x92, 0x34, 0x56, 0x78, 0x56, 0x78, 0x78, 0xbb,
};
static tt_u8_t __b2[] = {
    0xFF, 0x92, 0x34, 0x56, 0x78, 0x56, 0x78, 0x78, 0x92, 0x34, 0x56, 0x78,
    0x56, 0x78, 0x78, 0xaa, 0x92, 0x34, 0x56, 0x78, 0x56, 0x78, 0x78, 0xbb,
};
static tt_u8_t __b3[] = {
    0x92, 0x34, 0x56, 0x78, 0x56, 0x78, 0x78, 0x92, 0x34, 0x56, 0x78, 0x56,
    0x78, 0x78, 0xaa, 0x93, 0x34, 0x56, 0x78, 0x56, 0x78, 0x78, 0xbb,
};

static tt_u8_t __c1[] = {
    0x92,
    0x34,
    0x56,
    0x78,
    0x56,
    0x78,
    0x78,
    0xaa,
    0x92,
    0x34,
    0x56,
    0x78,
    0x56,
    0x78,
    0x78,
    0xbc,
};
static tt_u8_t __c2[] = {
    0x91, 0x34, 0x56, 0x78, 0x56, 0x78, 0x78, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0x92, 0x34, 0x56, 0x78, 0x56, 0x78, 0x78, 0xbb,
};

///////////////////////////

TT_TEST_ROUTINE_DEFINE(tt_unit_test_mpn_misc)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_mpn_t b, ret, correct;
    tt_result_t result;
    tt_s32_t s;

    TT_TEST_CASE_ENTER()
    // test start

    tt_mpn_init(&b);
    tt_mpn_init(&ret);
    tt_mpn_init(&correct);

    //// cmp

    tt_mpn_reverse(NULL, &ret);
    s = tt_mpn_cmp(&b, &ret);
    TT_TEST_CHECK_EQUAL(s, 0, "");
    TT_TEST_CHECK_EQUAL(tt_mpn_h0bitnum(&b), TT_MPN_USIZE_BIT, "");

    //// tt_mpn_copy

    result = tt_mpn_copy(&ret, &b, 0);
    TT_TEST_CHECK_SUCCESS(result, "");
    TT_TEST_CHECK_EQUAL(ret.unit[0], 0, "");
    TT_TEST_CHECK_EQUAL(ret.unit_num, 1, "");

    b.unit[0] = 1;
    TT_TEST_CHECK_EQUAL(tt_mpn_h0bitnum(&b), TT_MPN_USIZE_BIT - 1, "");

    b.unit[0] = ~0;
    TT_TEST_CHECK_EQUAL(tt_mpn_h0bitnum(&b), 0, "");
    b.unit[0] = 0x10;
    TT_TEST_CHECK_EQUAL(tt_mpn_h0bitnum(&b), TT_MPN_USIZE_BIT - 5, "");
    b.unit[0] = 0x201;
    TT_TEST_CHECK_EQUAL(tt_mpn_h0bitnum(&b), TT_MPN_USIZE_BIT - 10, "");
    b.unit[0] = 0x4110;
    TT_TEST_CHECK_EQUAL(tt_mpn_h0bitnum(&b), TT_MPN_USIZE_BIT - 15, "");
    b.unit[0] = 0x801020;
    TT_TEST_CHECK_EQUAL(tt_mpn_h0bitnum(&b), TT_MPN_USIZE_BIT - 24, "");
    b.unit[0] = 0x83030110;
    TT_TEST_CHECK_EQUAL(tt_mpn_h0bitnum(&b), TT_MPN_USIZE_BIT - 32, "");
#if TT_ENV_IS_64BIT
    b.unit[0] = 0x80f001001;
    TT_TEST_CHECK_EQUAL(tt_mpn_h0bitnum(&b), TT_MPN_USIZE_BIT - 36, "");
    b.unit[0] = 0x40203001fff;
    TT_TEST_CHECK_EQUAL(tt_mpn_h0bitnum(&b), TT_MPN_USIZE_BIT - 43, "");
    b.unit[0] = 0x40ffe0ff0eeccd;
    TT_TEST_CHECK_EQUAL(tt_mpn_h0bitnum(&b), TT_MPN_USIZE_BIT - 55, "");
#endif

    result = tt_mpn_set(&b, __c2, sizeof(__c2), TT_TRUE, 0);
    TT_TEST_CHECK_SUCCESS(result, "");

    // b is neg, ret is 0
    s = tt_mpn_cmp(&b, &ret);
    TT_TEST_CHECK_EQUAL(s, -1, "");
    s = tt_mpn_cmp(&ret, &b);
    TT_TEST_CHECK_EQUAL(s, 1, "");
    s = tt_mpn_cmp(&b, &b);
    TT_TEST_CHECK_EQUAL(s, 0, "");

    result = tt_mpn_copy(&ret, &b, 1);
    TT_TEST_CHECK_SUCCESS(result, "");
#if TT_ENV_IS_64BIT
    TT_TEST_CHECK_EQUAL(ret.unit[0], 0x92345678567878bb, "");
    TT_TEST_CHECK_EQUAL(ret.unit[1], 0xffffffffffffffff, "");
    TT_TEST_CHECK_EQUAL(ret.unit[2], 0x91345678567878, "");
    TT_TEST_CHECK_EQUAL(ret.unit_num, 3, "");
    TT_TEST_CHECK_EQUAL(ret.negative, TT_TRUE, "");
#else
    TT_TEST_CHECK_SUCCESS(TT_FAIL, "");
#endif

    tt_mpn_clear(&b);
    result = tt_mpn_copy(&ret, &b, 2);
    TT_TEST_CHECK_SUCCESS(result, "");
    TT_TEST_CHECK_EQUAL(ret.unit[0], 0, "");
    TT_TEST_CHECK_EQUAL(ret.unit_num, 1, "");

    //// tt_mpn_abs

    result = tt_mpn_set(&b, __c2, sizeof(__c2), TT_TRUE, 0);
    TT_TEST_CHECK_SUCCESS(result, "");
    result = tt_mpn_abs(&ret, &b);
    TT_TEST_CHECK_SUCCESS(result, "");
#if TT_ENV_IS_64BIT
    TT_TEST_CHECK_EQUAL(ret.unit[0], 0x92345678567878bb, "");
    TT_TEST_CHECK_EQUAL(ret.unit[1], 0xffffffffffffffff, "");
    TT_TEST_CHECK_EQUAL(ret.unit[2], 0x91345678567878, "");
    TT_TEST_CHECK_EQUAL(ret.unit_num, 3, "");
    TT_TEST_CHECK_EQUAL(ret.negative, TT_FALSE, "");
#else
    TT_TEST_CHECK_SUCCESS(TT_FAIL, "");
#endif

    result = tt_mpn_abs(NULL, &b);
    TT_TEST_CHECK_SUCCESS(result, "");
    TT_TEST_CHECK_EQUAL(b.negative, TT_FALSE, "");

    ///////////////////////////
    result = tt_mpn_set_cstr(&b, "", TT_MPN_FMT_AUTO, 0);
    TT_TEST_CHECK_SUCCESS(result, "");
    TT_TEST_CHECK_EQUAL(b.unit[0], 0, "");
    TT_TEST_CHECK_EQUAL(b.unit_num, 1, "");

    result = tt_mpn_set_cstr(&b,
                             "0xaaaacccc2222333344445555ffffffffffffffff",
                             TT_MPN_FMT_AUTO,
                             0);
    TT_TEST_CHECK_SUCCESS(result, "");

    // trunc 0
    result = tt_mpn_set_cstr(&correct, "0x0", TT_MPN_FMT_AUTO, 0);
    TT_TEST_CHECK_SUCCESS(result, "");
    result = tt_mpn_truncate(&ret, &b, 0);
    TT_TEST_CHECK_SUCCESS(result, "");
    TT_TEST_CHECK_EQUAL(tt_mpn_cmp(&ret, &correct), 0, "");

    // trunc 16b
    result = tt_mpn_set_cstr(&correct, "0xffff", TT_MPN_FMT_AUTO, 0);
    TT_TEST_CHECK_SUCCESS(result, "");
    result = tt_mpn_truncate(&ret, &b, 16);
    TT_TEST_CHECK_SUCCESS(result, "");
    TT_TEST_CHECK_EQUAL(tt_mpn_cmp(&ret, &correct), 0, "");

    // trunc 64b
    result =
        tt_mpn_set_cstr(&correct, "0xffffffffffffffff", TT_MPN_FMT_AUTO, 0);
    TT_TEST_CHECK_SUCCESS(result, "");
    result = tt_mpn_truncate(&ret, &b, 64);
    TT_TEST_CHECK_SUCCESS(result, "");
    TT_TEST_CHECK_EQUAL(tt_mpn_cmp(&ret, &correct), 0, "");

    // trunc 65b
    result =
        tt_mpn_set_cstr(&correct, "0x1ffffffffffffffff", TT_MPN_FMT_AUTO, 0);
    TT_TEST_CHECK_SUCCESS(result, "");
    result = tt_mpn_truncate(&ret, &b, 65);
    TT_TEST_CHECK_SUCCESS(result, "");
    TT_TEST_CHECK_EQUAL(tt_mpn_cmp(&ret, &correct), 0, "");

    // trunc 128b
    result = tt_mpn_set_cstr(&correct,
                             "0x2222333344445555ffffffffffffffff",
                             TT_MPN_FMT_AUTO,
                             0);
    TT_TEST_CHECK_SUCCESS(result, "");
    result = tt_mpn_truncate(&ret, &b, 128);
    TT_TEST_CHECK_SUCCESS(result, "");
    TT_TEST_CHECK_EQUAL(tt_mpn_cmp(&ret, &correct), 0, "");

    // trunc 129b
    result = tt_mpn_set_cstr(&correct,
                             "0x2222333344445555ffffffffffffffff",
                             TT_MPN_FMT_AUTO,
                             0);
    TT_TEST_CHECK_SUCCESS(result, "");
    result = tt_mpn_truncate(&ret, &b, 129);
    TT_TEST_CHECK_SUCCESS(result, "");
    TT_TEST_CHECK_EQUAL(tt_mpn_cmp(&ret, &correct), 0, "");

    // trunc 190b
    result = tt_mpn_set_cstr(&correct,
                             "0xaaaacccc2222333344445555ffffffffffffffff",
                             TT_MPN_FMT_AUTO,
                             0);
    TT_TEST_CHECK_SUCCESS(result, "");
    result = tt_mpn_truncate(&ret, &b, 190);
    TT_TEST_CHECK_SUCCESS(result, "");
    TT_TEST_CHECK_EQUAL(tt_mpn_cmp(&ret, &correct), 0, "");

    // trunc 250b
    result = tt_mpn_set_cstr(&correct,
                             "0xaaaacccc2222333344445555ffffffffffffffff",
                             TT_MPN_FMT_AUTO,
                             0);
    TT_TEST_CHECK_SUCCESS(result, "");
    result = tt_mpn_truncate(&ret, &b, 250);
    TT_TEST_CHECK_SUCCESS(result, "");
    TT_TEST_CHECK_EQUAL(tt_mpn_cmp(&ret, &correct), 0, "");

    ///////////////////////////

    // rand
    {
        tt_u32_t i;

        result = tt_mpn_rand(&b, 0, TT_MPN_RAND_FULL);
        TT_TEST_CHECK_SUCCESS(result, "");
        TT_TEST_CHECK_EQUAL(tt_mpn_zero(&b), TT_TRUE, "");

        result = tt_mpn_rand_lessthan(&ret, &b, TT_MPN_RAND_FULL);
        TT_TEST_CHECK_FAIL(result, "");

        for (i = 1; i < 100; ++i) {
            result = tt_mpn_rand(&b, i, TT_MPN_RAND_FULL);
            TT_TEST_CHECK_SUCCESS(result, "");
            TT_TEST_CHECK_EQUAL(tt_mpn_bytenum(&b), i, "");

            result = tt_mpn_rand_lessthan(&ret, &b, TT_MPN_RAND_FULL);
            TT_TEST_CHECK_SUCCESS(result, "");
            TT_TEST_CHECK_EQUAL(tt_mpn_cmp(&ret, &b), -1, "");
            TT_TEST_CHECK_EQUAL(tt_mpn_bytenum(&ret), tt_mpn_bytenum(&b), "");
        }
    }

    ///////////////////////////

    tt_mpn_destroy(&b);
    tt_mpn_destroy(&ret);
    tt_mpn_destroy(&correct);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_mpn_add)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_mpn_t a, b, r;
    tt_result_t ret;

    TT_TEST_CASE_ENTER()
    // test start

    tt_mpn_init(&a);
    tt_mpn_init(&b);
    tt_mpn_init(&r);

    ret = tt_mpn_add(&r, &b, &a);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(r.unit[0], 0, "");
    TT_TEST_CHECK_EQUAL(r.unit_num, 1, "");

    tt_mpn_set(&a, __a1, sizeof(__a1), TT_FALSE, 0);
    ret = tt_mpn_add(&r, &b, &a);
    tt_mpn_normalize(&r);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
#if TT_ENV_IS_64BIT
    TT_TEST_CHECK_EQUAL(r.unit[0], 0x92345678567878bb, "");
    TT_TEST_CHECK_EQUAL(r.unit[1], 0x92345678567878aa, "");
    TT_TEST_CHECK_EQUAL(r.unit[2], 0x92345678567878, "");
    TT_TEST_CHECK_EQUAL(r.unit_num, 3, "");
#else
    TT_TEST_CHECK_SUCCESS(TT_FAIL, "");
#endif

    tt_mpn_set(&b, __b1, sizeof(__b1), TT_FALSE, 0);
    ret = tt_mpn_add(&r, &b, &a);
    tt_mpn_normalize(&r);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
#if TT_ENV_IS_64BIT
    TT_TEST_CHECK_EQUAL(r.unit[0], 0x2468acf0acf0f176, "");
    TT_TEST_CHECK_EQUAL(r.unit[1], 0x923456785678793d, "");
    TT_TEST_CHECK_EQUAL(r.unit[2], 0x92345678567878, "");
    TT_TEST_CHECK_EQUAL(r.unit_num, 3, "");
#else
    TT_TEST_CHECK_SUCCESS(TT_FAIL, "");
#endif

    tt_mpn_set(&b, __b2, sizeof(__b2), TT_FALSE, 0);
    ret = tt_mpn_add(&r, &b, &a);
    tt_mpn_normalize(&r);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
#if TT_ENV_IS_64BIT
    TT_TEST_CHECK_EQUAL(r.unit[0], 0x2468acf0acf0f176, "");
    TT_TEST_CHECK_EQUAL(r.unit[1], 0x2468acf0acf0f155, "");
    TT_TEST_CHECK_EQUAL(r.unit[2], 0x002468acf0acf0f1, "");
    TT_TEST_CHECK_EQUAL(r.unit[3], 1, "");
    TT_TEST_CHECK_EQUAL(r.unit_num, 4, "");
#else
    TT_TEST_CHECK_SUCCESS(TT_FAIL, "");
#endif

    // a1 - b3
    tt_mpn_set(&b, __b3, sizeof(__b3), TT_TRUE, 0);
    ret = tt_mpn_add(&r, &a, &b);
    tt_mpn_normalize(&r);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
#if TT_ENV_IS_64BIT
    TT_TEST_CHECK_EQUAL(r.unit[0], 0x100000000000000, "");
    TT_TEST_CHECK_EQUAL(r.unit_num, 1, "");
    TT_TEST_CHECK_EQUAL(r.negative, TT_TRUE, "");
#else
    TT_TEST_CHECK_SUCCESS(TT_FAIL, "");
#endif
    ret = tt_mpn_add(&r, &b, &a);
    tt_mpn_normalize(&r);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
#if TT_ENV_IS_64BIT
    TT_TEST_CHECK_EQUAL(r.unit[0], 0x100000000000000, "");
    TT_TEST_CHECK_EQUAL(r.unit_num, 1, "");
    TT_TEST_CHECK_EQUAL(r.negative, TT_TRUE, "");
#else
    TT_TEST_CHECK_SUCCESS(TT_FAIL, "");
#endif

    tt_mpn_set(&b, __a1, sizeof(__a1), TT_TRUE, 0);
    ret = tt_mpn_add(&r, &a, &b);
    tt_mpn_normalize(&r);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(r.unit[0], 0, "");
    TT_TEST_CHECK_EQUAL(r.unit_num, 1, "");

    tt_mpn_destroy(&a);
    tt_mpn_destroy(&b);
    tt_mpn_destroy(&r);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_mpn_addeq)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_mpn_t a, b;
    tt_result_t ret;

    TT_TEST_CASE_ENTER()
    // test start

    tt_mpn_init(&a);
    tt_mpn_init(&b);

    ret = tt_mpn_add(&a, &b, &a);
    tt_mpn_normalize(&a);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(a.unit[0], 0, "");
    TT_TEST_CHECK_EQUAL(a.unit_num, 1, "");

    // __a1 + 0 => a
    tt_mpn_set(&a, __a1, sizeof(__a1), TT_FALSE, 0);
    ret = tt_mpn_add(&a, &b, &a);
    tt_mpn_normalize(&a);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
#if TT_ENV_IS_64BIT
    TT_TEST_CHECK_EQUAL(a.unit[0], 0x92345678567878bb, "");
    TT_TEST_CHECK_EQUAL(a.unit[1], 0x92345678567878aa, "");
    TT_TEST_CHECK_EQUAL(a.unit[2], 0x92345678567878, "");
    TT_TEST_CHECK_EQUAL(a.unit_num, 3, "");
#else
    TT_TEST_CHECK_SUCCESS(TT_FAIL, "");
#endif

    // __b1 + __a1 => b
    tt_mpn_set(&b, __b1, sizeof(__b1), TT_FALSE, 0);
    ret = tt_mpn_add(&b, &b, &a);
    tt_mpn_normalize(&b);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
#if TT_ENV_IS_64BIT
    TT_TEST_CHECK_EQUAL(b.unit[0], 0x2468acf0acf0f176, "");
    TT_TEST_CHECK_EQUAL(b.unit[1], 0x923456785678793d, "");
    TT_TEST_CHECK_EQUAL(b.unit[2], 0x92345678567878, "");
    TT_TEST_CHECK_EQUAL(b.unit_num, 3, "");
#else
    TT_TEST_CHECK_SUCCESS(TT_FAIL, "");
#endif

    tt_mpn_set(&b, __b2, sizeof(__b2), TT_FALSE, 0);
    ret = tt_mpn_add(&b, &a, &b);
    tt_mpn_normalize(&b);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
#if TT_ENV_IS_64BIT
    TT_TEST_CHECK_EQUAL(b.unit[0], 0x2468acf0acf0f176, "");
    TT_TEST_CHECK_EQUAL(b.unit[1], 0x2468acf0acf0f155, "");
    TT_TEST_CHECK_EQUAL(b.unit[2], 0x002468acf0acf0f1, "");
    TT_TEST_CHECK_EQUAL(b.unit[3], 1, "");
    TT_TEST_CHECK_EQUAL(b.unit_num, 4, "");
#else
    TT_TEST_CHECK_SUCCESS(TT_FAIL, "");
#endif

    // a1 - b3
    tt_mpn_set(&b, __b3, sizeof(__b3), TT_TRUE, 0);
    ret = tt_mpn_add(&b, &a, &b);
    tt_mpn_normalize(&b);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
#if TT_ENV_IS_64BIT
    TT_TEST_CHECK_EQUAL(b.unit[0], 0x100000000000000, "");
    TT_TEST_CHECK_EQUAL(b.unit_num, 1, "");
    TT_TEST_CHECK_EQUAL(b.negative, TT_TRUE, "");
#else
    TT_TEST_CHECK_SUCCESS(TT_FAIL, "");
#endif
    tt_mpn_set(&b, __b3, sizeof(__b3), TT_TRUE, 0);
    ret = tt_mpn_add(&b, &b, &a);
    tt_mpn_normalize(&b);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
#if TT_ENV_IS_64BIT
    TT_TEST_CHECK_EQUAL(b.unit[0], 0x100000000000000, "");
    TT_TEST_CHECK_EQUAL(b.unit_num, 1, "");
    TT_TEST_CHECK_EQUAL(b.negative, TT_TRUE, "");
#else
    TT_TEST_CHECK_SUCCESS(TT_FAIL, "");
#endif

    // now b is negative
    ret = tt_mpn_abs(&a, &b);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(b.negative, TT_TRUE, "");
    TT_TEST_CHECK_EQUAL(a.negative, TT_FALSE, "");
    ret = tt_mpn_add(&b, &a, &b);
    tt_mpn_normalize(&b);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(b.unit[0], 0, "");
    TT_TEST_CHECK_EQUAL(b.unit_num, 1, "");

    // b + b => b
    ret = tt_mpn_set(&b, __b2, sizeof(__b2), TT_TRUE, 0);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_mpn_addeq(&b, &b);
    tt_mpn_normalize(&b);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
#if TT_ENV_IS_64BIT
    TT_TEST_CHECK_EQUAL(b.unit[0], 0x2468acf0acf0f176, "");
    TT_TEST_CHECK_EQUAL(b.unit[1], 0x2468acf0acf0f155, "");
    TT_TEST_CHECK_EQUAL(b.unit[2], 0xff2468acf0acf0f1, "");
    TT_TEST_CHECK_EQUAL(b.unit[3], 1, "");
    TT_TEST_CHECK_EQUAL(b.unit_num, 4, "");
    TT_TEST_CHECK_EQUAL(b.negative, TT_TRUE, "");
#else
    TT_TEST_CHECK_SUCCESS(TT_FAIL, "");
#endif

    tt_mpn_destroy(&a);
    tt_mpn_destroy(&b);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_mpn_sub)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_mpn_t a, b, r;
    tt_result_t ret;

    TT_TEST_CASE_ENTER()
    // test start

    tt_mpn_init(&a);
    tt_mpn_init(&b);
    tt_mpn_init(&r);

    // 0 - 0
    ret = tt_mpn_sub(&r, &b, &a);
    tt_mpn_normalize(&r);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(r.unit[0], 0, "");
    TT_TEST_CHECK_EQUAL(r.unit_num, 1, "");

    // 0 - a1
    tt_mpn_set(&a, __a1, sizeof(__a1), TT_FALSE, 0);
    ret = tt_mpn_sub(&r, &b, &a);
    tt_mpn_normalize(&r);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
#if TT_ENV_IS_64BIT
    TT_TEST_CHECK_EQUAL(r.unit[0], 0x92345678567878bb, "");
    TT_TEST_CHECK_EQUAL(r.unit[1], 0x92345678567878aa, "");
    TT_TEST_CHECK_EQUAL(r.unit[2], 0x92345678567878, "");
    TT_TEST_CHECK_EQUAL(r.unit_num, 3, "");
    TT_TEST_CHECK_EQUAL(r.negative, TT_TRUE, "");
#else
    TT_TEST_CHECK_SUCCESS(TT_FAIL, "");
#endif

    // a1 - 0
    ret = tt_mpn_sub(&r, &a, &b);
    tt_mpn_normalize(&r);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
#if TT_ENV_IS_64BIT
    TT_TEST_CHECK_EQUAL(r.unit[0], 0x92345678567878bb, "");
    TT_TEST_CHECK_EQUAL(r.unit[1], 0x92345678567878aa, "");
    TT_TEST_CHECK_EQUAL(r.unit[2], 0x92345678567878, "");
    TT_TEST_CHECK_EQUAL(r.unit_num, 3, "");
    TT_TEST_CHECK_EQUAL(r.negative, TT_FALSE, "");
#else
    TT_TEST_CHECK_SUCCESS(TT_FAIL, "");
#endif

    // c1 - a1
    tt_mpn_set(&b, __c1, sizeof(__c1), TT_FALSE, 0);
    ret = tt_mpn_sub(&r, &b, &a);
    tt_mpn_normalize(&r);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
#if TT_ENV_IS_64BIT
    TT_TEST_CHECK_EQUAL(r.unit[0], 0xffffffffffffffff, "");
    TT_TEST_CHECK_EQUAL(r.unit[1], 0xffffffffffffffff, "");
    TT_TEST_CHECK_EQUAL(r.unit[2], 0x92345678567877, "");
    TT_TEST_CHECK_EQUAL(r.unit_num, 3, "");
    TT_TEST_CHECK_EQUAL(r.negative, TT_TRUE, "");
#else
    TT_TEST_CHECK_SUCCESS(TT_FAIL, "");
#endif

    // a1 - c1
    ret = tt_mpn_sub(&r, &a, &b);
    tt_mpn_normalize(&r);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
#if TT_ENV_IS_64BIT
    TT_TEST_CHECK_EQUAL(r.unit[0], 0xffffffffffffffff, "");
    TT_TEST_CHECK_EQUAL(r.unit[1], 0xffffffffffffffff, "");
    TT_TEST_CHECK_EQUAL(r.unit[2], 0x92345678567877, "");
    TT_TEST_CHECK_EQUAL(r.unit_num, 3, "");
    TT_TEST_CHECK_EQUAL(r.negative, TT_FALSE, "");
#else
    TT_TEST_CHECK_SUCCESS(TT_FAIL, "");
#endif

    // c2 - a1
    tt_mpn_set(&b, __c2, sizeof(__c2), TT_FALSE, 0);
    ret = tt_mpn_sub(&r, &b, &a);
    tt_mpn_normalize(&r);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
#if TT_ENV_IS_64BIT
    TT_TEST_CHECK_EQUAL(r.unit[0], 0, "");
    TT_TEST_CHECK_EQUAL(r.unit[1], 0x92345678567878ab, "");
    TT_TEST_CHECK_EQUAL(r.unit[2], 0x0000ffffffffffff, "");
    TT_TEST_CHECK_EQUAL(r.unit_num, 3, "");
    TT_TEST_CHECK_EQUAL(r.negative, TT_TRUE, "");
#else
    TT_TEST_CHECK_SUCCESS(TT_FAIL, "");
#endif

    // a1 - c2
    ret = tt_mpn_sub(&r, &a, &b);
    tt_mpn_normalize(&r);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
#if TT_ENV_IS_64BIT
    TT_TEST_CHECK_EQUAL(r.unit[0], 0, "");
    TT_TEST_CHECK_EQUAL(r.unit[1], 0x92345678567878ab, "");
    TT_TEST_CHECK_EQUAL(r.unit[2], 0x0000ffffffffffff, "");
    TT_TEST_CHECK_EQUAL(r.unit_num, 3, "");
    TT_TEST_CHECK_EQUAL(r.negative, TT_FALSE, "");
#else
    TT_TEST_CHECK_SUCCESS(TT_FAIL, "");
#endif

    tt_mpn_set(&b, __a1, sizeof(__a1), TT_FALSE, 0);
    ret = tt_mpn_sub(&r, &a, &b);
    tt_mpn_normalize(&r);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(r.unit[0], 0, "");
    TT_TEST_CHECK_EQUAL(r.unit_num, 1, "");

    tt_mpn_destroy(&a);
    tt_mpn_destroy(&b);
    tt_mpn_destroy(&r);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_mpn_subeq)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_mpn_t a, b;
    tt_result_t ret;

    TT_TEST_CASE_ENTER()
    // test start

    tt_mpn_init(&a);
    tt_mpn_init(&b);

    // 0 - 0
    ret = tt_mpn_sub(&b, &b, &a);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(b.unit[0], 0, "");
    TT_TEST_CHECK_EQUAL(b.unit_num, 1, "");

    // 0 - a1
    tt_mpn_set(&a, __a1, sizeof(__a1), TT_FALSE, 0);
    ret = tt_mpn_sub(&a, &b, &a);
    tt_mpn_normalize(&a);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
#if TT_ENV_IS_64BIT
    TT_TEST_CHECK_EQUAL(a.unit[0], 0x92345678567878bb, "");
    TT_TEST_CHECK_EQUAL(a.unit[1], 0x92345678567878aa, "");
    TT_TEST_CHECK_EQUAL(a.unit[2], 0x92345678567878, "");
    TT_TEST_CHECK_EQUAL(a.unit_num, 3, "");
    TT_TEST_CHECK_EQUAL(a.negative, TT_TRUE, "");
#else
    TT_TEST_CHECK_SUCCESS(TT_FAIL, "");
#endif

    // a1 - 0
    tt_mpn_abs(NULL, &a);
    ret = tt_mpn_sub(&b, &a, &b);
    tt_mpn_normalize(&b);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
#if TT_ENV_IS_64BIT
    TT_TEST_CHECK_EQUAL(b.unit[0], 0x92345678567878bb, "");
    TT_TEST_CHECK_EQUAL(b.unit[1], 0x92345678567878aa, "");
    TT_TEST_CHECK_EQUAL(b.unit[2], 0x92345678567878, "");
    TT_TEST_CHECK_EQUAL(b.unit_num, 3, "");
    TT_TEST_CHECK_EQUAL(b.negative, TT_FALSE, "");
#else
    TT_TEST_CHECK_SUCCESS(TT_FAIL, "");
#endif

    // c1 - a1
    tt_mpn_set(&b, __c1, sizeof(__c1), TT_FALSE, 0);
    tt_mpn_set(&a, __a1, sizeof(__a1), TT_FALSE, 0);
    ret = tt_mpn_sub(&b, &b, &a);
    tt_mpn_normalize(&b);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
#if TT_ENV_IS_64BIT
    TT_TEST_CHECK_EQUAL(b.unit[0], 0xffffffffffffffff, "");
    TT_TEST_CHECK_EQUAL(b.unit[1], 0xffffffffffffffff, "");
    TT_TEST_CHECK_EQUAL(b.unit[2], 0x92345678567877, "");
    TT_TEST_CHECK_EQUAL(b.unit_num, 3, "");
    TT_TEST_CHECK_EQUAL(b.negative, TT_TRUE, "");
#else
    TT_TEST_CHECK_SUCCESS(TT_FAIL, "");
#endif

    // a1 - c1
    tt_mpn_set(&a, __a1, sizeof(__a1), TT_FALSE, 0);
    tt_mpn_set(&b, __c1, sizeof(__c1), TT_FALSE, 0);
    ret = tt_mpn_sub(&a, &a, &b);
    tt_mpn_normalize(&b);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
#if TT_ENV_IS_64BIT
    TT_TEST_CHECK_EQUAL(a.unit[0], 0xffffffffffffffff, "");
    TT_TEST_CHECK_EQUAL(a.unit[1], 0xffffffffffffffff, "");
    TT_TEST_CHECK_EQUAL(a.unit[2], 0x92345678567877, "");
    TT_TEST_CHECK_EQUAL(a.unit_num, 3, "");
    TT_TEST_CHECK_EQUAL(a.negative, TT_FALSE, "");
#else
    TT_TEST_CHECK_SUCCESS(TT_FAIL, "");
#endif

    // c2 - a1
    tt_mpn_set(&a, __a1, sizeof(__a1), TT_FALSE, 0);
    tt_mpn_set(&b, __c2, sizeof(__c2), TT_FALSE, 0);
    ret = tt_mpn_sub(&a, &b, &a);
    tt_mpn_normalize(&b);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
#if TT_ENV_IS_64BIT
    TT_TEST_CHECK_EQUAL(a.unit[0], 0, "");
    TT_TEST_CHECK_EQUAL(a.unit[1], 0x92345678567878ab, "");
    TT_TEST_CHECK_EQUAL(a.unit[2], 0x0000ffffffffffff, "");
    TT_TEST_CHECK_EQUAL(a.unit_num, 3, "");
    TT_TEST_CHECK_EQUAL(a.negative, TT_TRUE, "");
#else
    TT_TEST_CHECK_SUCCESS(TT_FAIL, "");
#endif

    // a1 - c2
    tt_mpn_set(&a, __a1, sizeof(__a1), TT_FALSE, 0);
    tt_mpn_set(&b, __c2, sizeof(__c2), TT_FALSE, 0);
    ret = tt_mpn_sub(&a, &a, &b);
    tt_mpn_normalize(&a);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
#if TT_ENV_IS_64BIT
    TT_TEST_CHECK_EQUAL(a.unit[0], 0, "");
    TT_TEST_CHECK_EQUAL(a.unit[1], 0x92345678567878ab, "");
    TT_TEST_CHECK_EQUAL(a.unit[2], 0x0000ffffffffffff, "");
    TT_TEST_CHECK_EQUAL(a.unit_num, 3, "");
    TT_TEST_CHECK_EQUAL(a.negative, TT_FALSE, "");
#else
    TT_TEST_CHECK_SUCCESS(TT_FAIL, "");
#endif

    // b - b => b
    tt_mpn_set(&b, __a1, sizeof(__a1), TT_FALSE, 0);
    ret = tt_mpn_subeq(&b, &b);
    tt_mpn_normalize(&b);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(b.unit[0], 0, "");
    TT_TEST_CHECK_EQUAL(b.unit_num, 1, "");

    // b - (-b) => b
    tt_mpn_set(&b, __b2, sizeof(__b2), TT_TRUE, 0);
    tt_mpn_reverse(&a, &b);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(a.unit_num, b.unit_num, "");
    TT_TEST_CHECK_EQUAL(a.unit[a.unit_num - 1], b.unit[b.unit_num - 1], "");
    TT_TEST_CHECK_EQUAL(a.negative, TT_FALSE, "");
    ret = tt_mpn_sub(&b, &b, &a);
    tt_mpn_normalize(&b);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
#if TT_ENV_IS_64BIT
    TT_TEST_CHECK_EQUAL(b.unit[0], 0x2468acf0acf0f176, "");
    TT_TEST_CHECK_EQUAL(b.unit[1], 0x2468acf0acf0f155, "");
    TT_TEST_CHECK_EQUAL(b.unit[2], 0xff2468acf0acf0f1, "");
    TT_TEST_CHECK_EQUAL(b.unit[3], 1, "");
    TT_TEST_CHECK_EQUAL(b.unit_num, 4, "");
    TT_TEST_CHECK_EQUAL(b.negative, TT_TRUE, "");
#else
    TT_TEST_CHECK_SUCCESS(TT_FAIL, "");
#endif

    tt_mpn_destroy(&a);
    tt_mpn_destroy(&b);

    // test end
    TT_TEST_CASE_LEAVE()
}

//////////////////////////////////////////////////////////////////

struct
{
    const tt_char_t *input;
    tt_u32_t lshift_bit;
    const tt_char_t *output;
} __mpn_lsfht_case[] = {
    {"0x1", 0, "0x1"},
    {"0x1", 1, "0x2"},
    {"0x1", 63, "0x8000000000000000"},
    {"0x1", 64, "0x10000000000000000"},

    {"0x18000000000000003", 0, "0x18000000000000003"},
    {"0x18000000000000003", 1, "0x30000000000000006"},
    {"0x18000000000000003", 63, " 0xc0000000000000018000000000000000"},
    {"0x18000000000000003", 64, "0x180000000000000030000000000000000"},

    {"0x18000000000000003c000000000000005",
     0,
     "0x18000000000000003c000000000000005"},
    {"0x1e000000000000003c000000000000005",
     1,
     "0x3C000000000000007800000000000000a"},
    {"0x18000000000000003c000000000000005",
     63,
     "0xc000000000000001e0000000000000028000000000000000"},
    {"0x180000000000000037000000000000004",
     64,
     "0x1800000000000000370000000000000040000000000000000"},

    {"0x18000000000000003c000000000000005",
     65,
     "0x30000000000000007800000000000000a0000000000000000"},
    {"0x1e000000000000003c000000000000005",
     127,
     "0xf000000000000001e00000000000000280000000000000000000000000000000"},

    {"0x18000000000000003c000000000000005",
     128,
     "0x18000000000000003c00000000000000500000000000000000000000000000000"},
    {"0x1e000000000000003c000000000000005",
     191,
     "0xf000000000000001e000000000000002800000000000000000000000000000000000000"
     "000000000"},
};

TT_TEST_ROUTINE_DEFINE(tt_unit_test_mpn_lshift)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_mpn_t in, out, calc_out;
    tt_result_t ret;
    tt_u32_t i;
    tt_s32_t cmp_ret;

    TT_TEST_CASE_ENTER()
    // test start

    tt_mpn_init(&in);
    tt_mpn_init(&out);
    tt_mpn_init(&calc_out);

    for (i = 0; i < sizeof(__mpn_lsfht_case) / sizeof(__mpn_lsfht_case[0]);
         ++i) {
        ret =
            tt_mpn_set_cstr(&in, __mpn_lsfht_case[i].input, TT_MPN_FMT_HEX, 0);
        TT_TEST_CHECK_SUCCESS(ret, "");
        ret = tt_mpn_set_cstr(&out,
                              __mpn_lsfht_case[i].output,
                              TT_MPN_FMT_HEX,
                              0);
        TT_TEST_CHECK_SUCCESS(ret, "");

        ret = tt_mpn_lshf(&calc_out, &in, __mpn_lsfht_case[i].lshift_bit);
        TT_TEST_CHECK_SUCCESS(ret, "");

        cmp_ret = tt_mpn_cmp(&calc_out, &out);
        TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    }

    for (i = 0; i < sizeof(__mpn_lsfht_case) / sizeof(__mpn_lsfht_case[0]);
         ++i) {
        ret =
            tt_mpn_set_cstr(&in, __mpn_lsfht_case[i].input, TT_MPN_FMT_HEX, 0);
        TT_TEST_CHECK_SUCCESS(ret, "");
        ret = tt_mpn_set_cstr(&out,
                              __mpn_lsfht_case[i].output,
                              TT_MPN_FMT_HEX,
                              0);
        TT_TEST_CHECK_SUCCESS(ret, "");

        ret = tt_mpn_lshf(NULL, &in, __mpn_lsfht_case[i].lshift_bit);
        TT_TEST_CHECK_SUCCESS(ret, "");

        cmp_ret = tt_mpn_cmp(&in, &out);
        TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    }

    tt_mpn_destroy(&in);
    tt_mpn_destroy(&out);
    tt_mpn_destroy(&calc_out);

    // test end
    TT_TEST_CASE_LEAVE()
}

struct
{
    const tt_char_t *input;
    tt_u32_t lshift_bit;
    const tt_char_t *output;
} __mpn_rsfht_case[] = {
    {"0x1", 0, "0x1"},
    {"0x1", 1, "0x0"},

    {"0x18000000000000003", 0, "0x18000000000000003"},
    {"0x18000000000000003", 1, "0xc000000000000001"},
    {"0x18000000000000003", 63, " 0x3"},
    {"0x18000000000000003", 64, "0x1"},

    {"0x18000000000000003c000000000000005",
     0,
     "0x18000000000000003c000000000000005"},
    {"0x1e000000000000003c000000000000005",
     1,
     " 0xf000000000000001e000000000000002"},
    {"0x18000000000000003c000000000000005", 63, "0x30000000000000007"},
    {"0x180000000000000037000000000000004", 64, "0x18000000000000003"},

    {"0x18000000000000003c000000000000005", 65, "0xc000000000000001"},
    {"0x1e000000000000003c000000000000005", 127, "0x3"},

    {"0x18000000000000003c00000000000000500000000000000000000000000000000",
     128,
     "0x18000000000000003c000000000000005"},
    {"0xf000000000000001e000000000000002800000005000000040000000300000002000000"
     "010000000",
     191,
     "0x1e000000000000003c000000000000005"},
};

TT_TEST_ROUTINE_DEFINE(tt_unit_test_mpn_rshift)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_mpn_t in, out, calc_out;
    tt_result_t ret;
    tt_u32_t i;
    tt_s32_t cmp_ret;

    TT_TEST_CASE_ENTER()
    // test start

    tt_mpn_init(&in);
    tt_mpn_init(&out);
    tt_mpn_init(&calc_out);

    for (i = 0; i < sizeof(__mpn_rsfht_case) / sizeof(__mpn_rsfht_case[0]);
         ++i) {
        ret =
            tt_mpn_set_cstr(&in, __mpn_rsfht_case[i].input, TT_MPN_FMT_HEX, 0);
        TT_TEST_CHECK_SUCCESS(ret, "");
        ret = tt_mpn_set_cstr(&out,
                              __mpn_rsfht_case[i].output,
                              TT_MPN_FMT_HEX,
                              0);
        TT_TEST_CHECK_SUCCESS(ret, "");

        ret = tt_mpn_rshf(&calc_out, &in, __mpn_rsfht_case[i].lshift_bit);
        TT_TEST_CHECK_SUCCESS(ret, "");

        cmp_ret = tt_mpn_cmp(&calc_out, &out);
        TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    }

    for (i = 0; i < sizeof(__mpn_rsfht_case) / sizeof(__mpn_rsfht_case[0]);
         ++i) {
        ret =
            tt_mpn_set_cstr(&in, __mpn_rsfht_case[i].input, TT_MPN_FMT_HEX, 0);
        TT_TEST_CHECK_SUCCESS(ret, "");
        ret = tt_mpn_set_cstr(&out,
                              __mpn_rsfht_case[i].output,
                              TT_MPN_FMT_HEX,
                              0);
        TT_TEST_CHECK_SUCCESS(ret, "");

        ret = tt_mpn_rshf(NULL, &in, __mpn_rsfht_case[i].lshift_bit);
        TT_TEST_CHECK_SUCCESS(ret, "");

        cmp_ret = tt_mpn_cmp(&in, &out);
        TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    }

    tt_mpn_destroy(&in);
    tt_mpn_destroy(&out);
    tt_mpn_destroy(&calc_out);

    // test end
    TT_TEST_CASE_LEAVE()
}

struct
{
    const tt_char_t *in;
    tt_mpn_unit_t u;
    tt_u32_t u_num;
    const tt_char_t *out;
} __bu_prepend_un[] = {
    {"0x1111222233334444", 0x1b, 1, "0x1b1111222233334444"},
    {"0x1111222233334444", 0x1b, 0, "0x1111222233334444"},
    {"0x1111222233334444", 0x1b, 2, "0x1b000000000000001b1111222233334444"},
};

TT_TEST_ROUTINE_DEFINE(tt_unit_test_mpn_prepend)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_mpn_t in, out, correct_out;
    tt_u32_t i;
    tt_result_t ret;

    TT_TEST_CASE_ENTER()
    // test start

    tt_mpn_init(&in);
    tt_mpn_init(&out);
    tt_mpn_init(&correct_out);

    for (i = 0; i < sizeof(__bu_prepend_un) / sizeof(__bu_prepend_un[0]); ++i) {
        ret = tt_mpn_set_cstr(&in, __bu_prepend_un[i].in, TT_FALSE, 0);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        ret =
            tt_mpn_set_cstr(&correct_out, __bu_prepend_un[i].out, TT_FALSE, 0);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        ret = tt_mpn_prepend_un(&out,
                                &in,
                                __bu_prepend_un[i].u,
                                __bu_prepend_un[i].u_num);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        TT_TEST_CHECK_EQUAL(tt_mpn_cmp(&correct_out, &out), 0, "");
    }

    for (i = 0; i < sizeof(__bu_prepend_un) / sizeof(__bu_prepend_un[0]); ++i) {
        ret = tt_mpn_set_cstr(&in, __bu_prepend_un[i].in, TT_FALSE, 0);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        ret =
            tt_mpn_set_cstr(&correct_out, __bu_prepend_un[i].out, TT_FALSE, 0);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        ret = tt_mpn_prepend_un(NULL,
                                &in,
                                __bu_prepend_un[i].u,
                                __bu_prepend_un[i].u_num);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        TT_TEST_CHECK_EQUAL(tt_mpn_cmp(&correct_out, &in), 0, "");
    }

    tt_mpn_destroy(&in);
    tt_mpn_destroy(&out);
    tt_mpn_destroy(&correct_out);

    // test end
    TT_TEST_CASE_LEAVE()
}

struct
{
    const tt_char_t *in;
    tt_mpn_unit_t u;
    tt_u32_t u_num;
    const tt_char_t *out;
} __bu_append_un[] = {
    {"0x1111222233334444", 0x1b, 1, "0x1111222233334444000000000000001b"},
    {"0x1111222233334444", 0x1b, 0, "0x1111222233334444"},
    {"0x1111222233334444",
     0x1b,
     2,
     "0x1111222233334444000000000000001b000000000000001b"},
};

TT_TEST_ROUTINE_DEFINE(tt_unit_test_mpn_append)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_mpn_t in, out, correct_out;
    tt_u32_t i;
    tt_result_t ret;

    TT_TEST_CASE_ENTER()
    // test start

    tt_mpn_init(&in);
    tt_mpn_init(&out);
    tt_mpn_init(&correct_out);

    for (i = 0; i < sizeof(__bu_append_un) / sizeof(__bu_append_un[0]); ++i) {
        ret = tt_mpn_set_cstr(&in, __bu_append_un[i].in, TT_FALSE, 0);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        ret = tt_mpn_set_cstr(&correct_out, __bu_append_un[i].out, TT_FALSE, 0);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        ret = tt_mpn_append_un(&out,
                               &in,
                               __bu_append_un[i].u,
                               __bu_append_un[i].u_num);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        TT_TEST_CHECK_EQUAL(tt_mpn_cmp(&correct_out, &out), 0, "");
    }

    for (i = 0; i < sizeof(__bu_append_un) / sizeof(__bu_append_un[0]); ++i) {
        ret = tt_mpn_set_cstr(&in, __bu_append_un[i].in, TT_FALSE, 0);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        ret = tt_mpn_set_cstr(&correct_out, __bu_append_un[i].out, TT_FALSE, 0);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        ret = tt_mpn_append_un(NULL,
                               &in,
                               __bu_append_un[i].u,
                               __bu_append_un[i].u_num);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        TT_TEST_CHECK_EQUAL(tt_mpn_cmp(&correct_out, &in), 0, "");
    }

    tt_mpn_destroy(&in);
    tt_mpn_destroy(&out);
    tt_mpn_destroy(&correct_out);

    // test end
    TT_TEST_CASE_LEAVE()
}
