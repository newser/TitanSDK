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

#include "tt_unit_test_case_config.h"
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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_lpatn_sn)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(lpatn_case)

TT_TEST_CASE("tt_unit_test_lpatn_sn",
             "testing log pattern: seq num",
             tt_unit_test_lpatn_sn,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE_LIST_DEFINE_END(lpatn_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(TEST_UNIT_LOG_PATTERN, 0, lpatn_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(tt_unit_test_lpatn_sn)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(tt_unit_test_lpatn_sn)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_lpfld_t *lpf;
    tt_result_t ret;
    tt_buf_t buf;

    // at least 3 chars
    tt_char_t f01[] = "{ }";
    tt_char_t f02[] = "{:}";
    tt_char_t f03[] = "{::";
    tt_char_t f04[] = "{seq_num";
    tt_char_t f05[] = "{seq_num:";
    tt_char_t f06[] = "{seq_num}";
    tt_char_t f1[] = "{seq_num:}";
    tt_char_t f2[] = "{seq_num:%+04d}";

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&buf, NULL);

    ret = tt_lpfld_check(&f01[0], &f01[sizeof(f01) - 2]);
    TT_TEST_CHECK_FAIL(ret, "");

    ret = tt_lpfld_check(&f02[0], &f02[sizeof(f02) - 2]);
    TT_TEST_CHECK_FAIL(ret, "");

    ret = tt_lpfld_check(&f03[0], &f03[sizeof(f03) - 2]);
    TT_TEST_CHECK_FAIL(ret, "");

    ret = tt_lpfld_check(&f04[0], &f04[sizeof(f04) - 2]);
    TT_TEST_CHECK_FAIL(ret, "");

    ret = tt_lpfld_check(&f05[0], &f05[sizeof(f05) - 2]);
    TT_TEST_CHECK_FAIL(ret, "");

    // {seq_num}
    ret = tt_lpfld_check(&f06[0], &f06[sizeof(f06) - 2]);
    TT_TEST_CHECK_SUCCESS(ret, "");

    {
        lpf = tt_lpfld_create(&f06[0], &f06[sizeof(f06) - 2]);
        TT_TEST_CHECK_NOT_EQUAL(lpf, NULL, "");

        tt_buf_clear(&buf);

        ret = tt_lpfld_output(lpf, &buf);
        TT_TEST_CHECK_FAIL(ret, "");
        TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&buf), 0, "");

        ret = tt_lpfld_output_cstr(lpf, "123", &buf);
        TT_TEST_CHECK_FAIL(ret, "");
        TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&buf), 0, "");

        ret = tt_lpfld_output_s32(lpf, 0, &buf);
        TT_TEST_CHECK_SUCCESS(ret, "");
        TT_TEST_CHECK_EQUAL(tt_buf_cmp_cstr(&buf, "0"), 0, "");

        tt_lpfld_destroy(lpf);
    }

    // {seq_num:}
    ret = tt_lpfld_check(&f1[0], &f1[sizeof(f1) - 2]);
    TT_TEST_CHECK_SUCCESS(ret, "");

    {
        lpf = tt_lpfld_create(&f1[0], &f1[sizeof(f1) - 2]);
        TT_TEST_CHECK_NOT_EQUAL(lpf, NULL, "");

        tt_buf_clear(&buf);

        ret = tt_lpfld_output(lpf, &buf);
        TT_TEST_CHECK_FAIL(ret, "");
        TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&buf), 0, "");

        ret = tt_lpfld_output_cstr(lpf, "123", &buf);
        TT_TEST_CHECK_FAIL(ret, "");
        TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&buf), 0, "");

        tt_buf_clear(&buf);
        ret = tt_lpfld_output_s32(lpf, 2147483647, &buf);
        TT_TEST_CHECK_SUCCESS(ret, "");
        TT_TEST_CHECK_EQUAL(tt_buf_cmp_cstr(&buf, "2147483647"), 0, "");

        tt_buf_clear(&buf);
        ret = tt_lpfld_output_s32(lpf, -2147483648, &buf);
        TT_TEST_CHECK_SUCCESS(ret, "");
        TT_TEST_CHECK_EQUAL(tt_buf_cmp_cstr(&buf, "-2147483648"), 0, "");

        tt_buf_clear(&buf);
        ret = tt_lpfld_output_s32(lpf, 100, &buf);
        TT_TEST_CHECK_SUCCESS(ret, "");
        TT_TEST_CHECK_EQUAL(tt_buf_cmp_cstr(&buf, "100"), 0, "");

        tt_lpfld_destroy(lpf);
    }

    // {seq_num:%+04x}
    ret = tt_lpfld_check(&f2[0], &f2[sizeof(f2) - 2]);
    TT_TEST_CHECK_SUCCESS(ret, "");

    {
        lpf = tt_lpfld_create(&f2[0], &f2[sizeof(f2) - 2]);
        TT_TEST_CHECK_NOT_EQUAL(lpf, NULL, "");

        tt_buf_clear(&buf);

        ret = tt_lpfld_output(lpf, &buf);
        TT_TEST_CHECK_FAIL(ret, "");
        TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&buf), 0, "");

        ret = tt_lpfld_output_cstr(lpf, "123", &buf);
        TT_TEST_CHECK_FAIL(ret, "");
        TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&buf), 0, "");

        tt_buf_clear(&buf);
        ret = tt_lpfld_output_s32(lpf, 1, &buf);
        TT_TEST_CHECK_SUCCESS(ret, "");
        TT_TEST_CHECK_EQUAL(tt_buf_cmp_cstr(&buf, "+001"), 0, "");

        tt_buf_clear(&buf);
        ret = tt_lpfld_output_s32(lpf, -2147483648, &buf);
        TT_TEST_CHECK_SUCCESS(ret, "");
        TT_TEST_CHECK_EQUAL(tt_buf_cmp_cstr(&buf, "-2147483648"), 0, "");

        tt_buf_clear(&buf);
        ret = tt_lpfld_output_s32(lpf, 10, &buf);
        TT_TEST_CHECK_SUCCESS(ret, "");
        TT_TEST_CHECK_EQUAL(tt_buf_cmp_cstr(&buf, "+010"), 0, "");

        tt_lpfld_destroy(lpf);
    }

    tt_buf_destroy(&buf);

    // test end
    TT_TEST_CASE_LEAVE()
}
