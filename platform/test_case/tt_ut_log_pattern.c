/* Copyright (C) 2017 haniu (niuhao.cn@gmail.com)
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
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
TT_TEST_ROUTINE_DECLARE(case_lpatn_sn)
TT_TEST_ROUTINE_DECLARE(case_lpatn_time)
TT_TEST_ROUTINE_DECLARE(case_lpatn_logger)
TT_TEST_ROUTINE_DECLARE(case_lpatn_level)
TT_TEST_ROUTINE_DECLARE(case_lpatn_content)
TT_TEST_ROUTINE_DECLARE(case_lpatn_func)
TT_TEST_ROUTINE_DECLARE(case_lpatn_line)
TT_TEST_ROUTINE_DECLARE(case_log_pattern)

TT_TEST_ROUTINE_DECLARE(case_log_syslog3164)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(lpatn_case)

TT_TEST_CASE("case_lpatn_sn",
             "testing log pattern: seq num",
             case_lpatn_sn,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("case_lpatn_time",
                 "testing log pattern: time",
                 case_lpatn_time,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_lpatn_logger",
                 "testing log pattern: logger",
                 case_lpatn_logger,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_lpatn_content",
                 "testing log pattern: content",
                 case_lpatn_content,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_lpatn_func",
                 "testing log pattern: function",
                 case_lpatn_func,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_lpatn_line",
                 "testing log pattern: line",
                 case_lpatn_line,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_log_pattern",
                 "testing log pattern",
                 case_log_pattern,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_log_syslog3164",
                 "testing log layout: syslog3164",
                 case_log_syslog3164,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

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
    TT_TEST_ROUTINE_DEFINE(case_log_syslog3164)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(case_lpatn_sn)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_logfld_t *lpf;
    tt_result_t ret;
    tt_buf_t buf;
    tt_log_entry_t entry = {0};

    // at least 3 chars
    tt_char_t f01[] = "{ }";
    tt_char_t f02[] = "{:}";
    tt_char_t f03[] = "{::";
    tt_char_t f04[] = "{seq_num";
    tt_char_t f05[] = "{seq_num:";
    tt_char_t f06[] = "{seq_num}";
    tt_char_t f07[] = "{seq_num1}";
    tt_char_t f1[] = "{seq_num:}";
    tt_char_t f2[] = "{seq_num:%+04d}";

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&buf, NULL);

    ret = tt_logfld_check(&f01[0], &f01[sizeof(f01) - 2]);
    TT_UT_FAIL(ret, "");

    ret = tt_logfld_check(&f02[0], &f02[sizeof(f02) - 2]);
    TT_UT_FAIL(ret, "");

    ret = tt_logfld_check(&f03[0], &f03[sizeof(f03) - 2]);
    TT_UT_FAIL(ret, "");

    ret = tt_logfld_check(&f04[0], &f04[sizeof(f04) - 2]);
    TT_UT_FAIL(ret, "");

    ret = tt_logfld_check(&f05[0], &f05[sizeof(f05) - 2]);
    TT_UT_FAIL(ret, "");

    ret = tt_logfld_check(&f07[0], &f07[sizeof(f07) - 2]);
    TT_UT_FAIL(ret, "");
    lpf = tt_logfld_create(&f07[0], &f07[sizeof(f07) - 2]);
    TT_UT_EQUAL(lpf, NULL, "");

    // {seq_num}
    ret = tt_logfld_check(&f06[0], &f06[sizeof(f06) - 2]);
    TT_UT_SUCCESS(ret, "");

    {
        lpf = tt_logfld_create(&f06[0], &f06[sizeof(f06) - 2]);
        TT_UT_NOT_EQUAL(lpf, NULL, "");

        tt_buf_clear(&buf);

        entry.seq_num = 123456;
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_buf_cmp_cstr(&buf, "123456"), 0, "");

        tt_logfld_destroy(lpf);
    }

    // {seq_num:}
    ret = tt_logfld_check(&f1[0], &f1[sizeof(f1) - 2]);
    TT_UT_SUCCESS(ret, "");

    {
        lpf = tt_logfld_create(&f1[0], &f1[sizeof(f1) - 2]);
        TT_UT_NOT_EQUAL(lpf, NULL, "");

        tt_buf_clear(&buf);

        tt_buf_clear(&buf);
        entry.seq_num = 2147483647;
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_buf_cmp_cstr(&buf, "2147483647"), 0, "");

        tt_buf_clear(&buf);
        entry.seq_num = (tt_u32_t)-2147483648LL;
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_buf_cmp_cstr(&buf, "-2147483648"), 0, "");

        tt_buf_clear(&buf);
        entry.seq_num = 100;
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_buf_cmp_cstr(&buf, "100"), 0, "");

        tt_logfld_destroy(lpf);
    }

    // {seq_num:%+04x}
    ret = tt_logfld_check(&f2[0], &f2[sizeof(f2) - 2]);
    TT_UT_SUCCESS(ret, "");

    {
        lpf = tt_logfld_create(&f2[0], &f2[sizeof(f2) - 2]);
        TT_UT_NOT_EQUAL(lpf, NULL, "");

        tt_buf_clear(&buf);

        tt_buf_clear(&buf);
        entry.seq_num = 1;
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_buf_cmp_cstr(&buf, "+001"), 0, "");

        tt_buf_clear(&buf);
        entry.seq_num = (tt_u32_t)-2147483648LL;
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_buf_cmp_cstr(&buf, "-2147483648"), 0, "");

        tt_buf_clear(&buf);
        entry.seq_num = 10;
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_buf_cmp_cstr(&buf, "+010"), 0, "");

        tt_logfld_destroy(lpf);
    }

    tt_buf_destroy(&buf);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_lpatn_time)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_logfld_t *lpf;
    tt_result_t ret;
    tt_buf_t buf;
    tt_log_entry_t entry = {0};

    // at least 3 chars
    tt_char_t f04[] = "{time";
    tt_char_t f05[] = "{time:";
    tt_char_t f06[] = "{time}";
    tt_char_t f07[] = "{time }";
    tt_char_t f1[] = "{time:}";
    tt_char_t f2[] = "{time:%Y-%m-%d-%H-%M-S}";

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&buf, NULL);

    ret = tt_logfld_check(&f04[0], &f04[sizeof(f04) - 2]);
    TT_UT_FAIL(ret, "");

    ret = tt_logfld_check(&f05[0], &f05[sizeof(f05) - 2]);
    TT_UT_FAIL(ret, "");

    ret = tt_logfld_check(&f07[0], &f07[sizeof(f07) - 2]);
    TT_UT_FAIL(ret, "");
    lpf = tt_logfld_create(&f07[0], &f07[sizeof(f07) - 2]);
    TT_UT_EQUAL(lpf, NULL, "");

    // {time}
    ret = tt_logfld_check(&f06[0], &f06[sizeof(f06) - 2]);
    TT_UT_SUCCESS(ret, "");

    {
        tt_char_t tmp[100] = {0};

        lpf = tt_logfld_create(&f06[0], &f06[sizeof(f06) - 2]);
        TT_UT_NOT_EQUAL(lpf, NULL, "");

        tt_buf_clear(&buf);

        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");

        // default format: "%Y-%N-%D %H:%M:%S"
        tt_date_render_now("%Y-%N-%D %H:%M:%S", tmp, sizeof(tmp));
        TT_UT_EQUAL(tt_buf_cmp_cstr(&buf, tmp), 0, "");

        tt_logfld_destroy(lpf);
    }

    // {time:}
    ret = tt_logfld_check(&f1[0], &f1[sizeof(f1) - 2]);
    TT_UT_SUCCESS(ret, "");

    {
        tt_char_t tmp[100] = {0};

        lpf = tt_logfld_create(&f1[0], &f1[sizeof(f1) - 2]);
        TT_UT_NOT_EQUAL(lpf, NULL, "");

        tt_buf_clear(&buf);

        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");

        // default format: "%Y-%N-%D %H:%M:%S"
        tt_date_render_now("%Y-%N-%D %H:%M:%S", tmp, sizeof(tmp));
        TT_UT_EQUAL(tt_buf_cmp_cstr(&buf, tmp), 0, "");

        tt_logfld_destroy(lpf);
    }

    // {time:%Y-%m-%d-%H-%M-S}
    ret = tt_logfld_check(&f2[0], &f2[sizeof(f2) - 2]);
    TT_UT_SUCCESS(ret, "");

    {
        tt_char_t tmp[100] = {0};

        lpf = tt_logfld_create(&f2[0], &f2[sizeof(f2) - 2]);
        TT_UT_NOT_EQUAL(lpf, NULL, "");

        tt_buf_clear(&buf);

        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");

        // format: "%Y-%m-%d-%H-%M-S"
        tt_date_render_now("%Y-%m-%d-%H-%M-S", tmp, sizeof(tmp));
        TT_UT_EQUAL(tt_buf_cmp_cstr(&buf, tmp), 0, "");

        tt_logfld_destroy(lpf);
    }

    tt_buf_destroy(&buf);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_lpatn_logger)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_logfld_t *lpf;
    tt_result_t ret;
    tt_buf_t buf;
    tt_log_entry_t entry = {0};

    // at least 3 chars
    tt_char_t f04[] = "{logger";
    tt_char_t f05[] = "{logger:";
    tt_char_t f06[] = "{logger}";
    tt_char_t f07[] = "{logger-}";
    tt_char_t f1[] = "{logger:}";
    tt_char_t f2[] = "{logger:at least 7 char: [%7s]. end}";

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&buf, NULL);

    ret = tt_logfld_check(&f04[0], &f04[sizeof(f04) - 2]);
    TT_UT_FAIL(ret, "");

    ret = tt_logfld_check(&f05[0], &f05[sizeof(f05) - 2]);
    TT_UT_FAIL(ret, "");

    ret = tt_logfld_check(&f07[0], &f07[sizeof(f07) - 2]);
    TT_UT_FAIL(ret, "");
    lpf = tt_logfld_create(&f07[0], &f07[sizeof(f07) - 2]);
    TT_UT_EQUAL(lpf, NULL, "");

    // {logger}
    ret = tt_logfld_check(&f06[0], &f06[sizeof(f06) - 2]);
    TT_UT_SUCCESS(ret, "");

    {
        lpf = tt_logfld_create(&f06[0], &f06[sizeof(f06) - 2]);
        TT_UT_NOT_EQUAL(lpf, NULL, "");

        tt_buf_clear(&buf);

        entry.logger = NULL;
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(TT_BUF_RLEN(&buf), 0, "");

        entry.logger = "";
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(TT_BUF_RLEN(&buf), 0, "");

        entry.logger = "123";
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_buf_cmp_cstr(&buf, "123"), 0, "");

        tt_logfld_destroy(lpf);
    }

    // {logger:}
    ret = tt_logfld_check(&f1[0], &f1[sizeof(f1) - 2]);
    TT_UT_SUCCESS(ret, "");

    {
        lpf = tt_logfld_create(&f1[0], &f1[sizeof(f1) - 2]);
        TT_UT_NOT_EQUAL(lpf, NULL, "");

        tt_buf_clear(&buf);

        entry.logger = NULL;
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(TT_BUF_RLEN(&buf), 0, "");

        entry.logger = "";
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(TT_BUF_RLEN(&buf), 0, "");

        entry.logger = "123";
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_buf_cmp_cstr(&buf, "123"), 0, "");

        tt_logfld_destroy(lpf);
    }

    // {logger:%07s}
    ret = tt_logfld_check(&f2[0], &f2[sizeof(f2) - 2]);
    TT_UT_SUCCESS(ret, "");

    {
        lpf = tt_logfld_create(&f2[0], &f2[sizeof(f2) - 2]);
        TT_UT_NOT_EQUAL(lpf, NULL, "");

        tt_buf_clear(&buf);

        entry.logger = NULL;
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(TT_BUF_RLEN(&buf), 0, "");

        entry.logger = "";
        tt_buf_clear(&buf);
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_buf_cmp_cstr(&buf, "at least 7 char: [       ]. end"),
                    0,
                    "");

        entry.logger = "1";
        tt_buf_clear(&buf);
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_buf_cmp_cstr(&buf, "at least 7 char: [      1]. end"),
                    0,
                    "");

        entry.logger = "12345678";
        tt_buf_clear(&buf);
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_buf_cmp_cstr(&buf, "at least 7 char: [12345678]. end"),
                    0,
                    "");

        tt_logfld_destroy(lpf);
    }

    tt_buf_destroy(&buf);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_lpatn_content)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_logfld_t *lpf;
    tt_result_t ret;
    tt_buf_t buf;
    tt_log_entry_t entry = {0};

    // at least 3 chars
    tt_char_t f04[] = "{content";
    tt_char_t f05[] = "{content:";
    tt_char_t f06[] = "{content}";
    tt_char_t f07[] = "{content-}";
    tt_char_t f1[] = "{content:}";
    tt_char_t f2[] = "{content:at least 7 char: [%7s]. end}";

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&buf, NULL);

    ret = tt_logfld_check(&f04[0], &f04[sizeof(f04) - 2]);
    TT_UT_FAIL(ret, "");

    ret = tt_logfld_check(&f05[0], &f05[sizeof(f05) - 2]);
    TT_UT_FAIL(ret, "");

    ret = tt_logfld_check(&f07[0], &f07[sizeof(f07) - 2]);
    TT_UT_FAIL(ret, "");
    lpf = tt_logfld_create(&f07[0], &f07[sizeof(f07) - 2]);
    TT_UT_EQUAL(lpf, NULL, "");

    // {content}
    ret = tt_logfld_check(&f06[0], &f06[sizeof(f06) - 2]);
    TT_UT_SUCCESS(ret, "");

    {
        lpf = tt_logfld_create(&f06[0], &f06[sizeof(f06) - 2]);
        TT_UT_NOT_EQUAL(lpf, NULL, "");

        tt_buf_clear(&buf);

        entry.content = NULL;
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(TT_BUF_RLEN(&buf), 0, "");

        entry.content = "";
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(TT_BUF_RLEN(&buf), 0, "");

        entry.content = "123";
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_buf_cmp_cstr(&buf, "123"), 0, "");

        tt_logfld_destroy(lpf);
    }

    // {content:}
    ret = tt_logfld_check(&f1[0], &f1[sizeof(f1) - 2]);
    TT_UT_SUCCESS(ret, "");

    {
        lpf = tt_logfld_create(&f1[0], &f1[sizeof(f1) - 2]);
        TT_UT_NOT_EQUAL(lpf, NULL, "");

        tt_buf_clear(&buf);

        entry.content = NULL;
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(TT_BUF_RLEN(&buf), 0, "");

        entry.content = "";
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(TT_BUF_RLEN(&buf), 0, "");

        entry.content = "123";
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_buf_cmp_cstr(&buf, "123"), 0, "");

        tt_logfld_destroy(lpf);
    }

    // {content:%07s}
    ret = tt_logfld_check(&f2[0], &f2[sizeof(f2) - 2]);
    TT_UT_SUCCESS(ret, "");

    {
        lpf = tt_logfld_create(&f2[0], &f2[sizeof(f2) - 2]);
        TT_UT_NOT_EQUAL(lpf, NULL, "");

        tt_buf_clear(&buf);

        entry.content = NULL;
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(TT_BUF_RLEN(&buf), 0, "");

        tt_buf_clear(&buf);
        entry.content = "";
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_buf_cmp_cstr(&buf, "at least 7 char: [       ]. end"),
                    0,
                    "");

        tt_buf_clear(&buf);
        entry.content = "1";
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_buf_cmp_cstr(&buf, "at least 7 char: [      1]. end"),
                    0,
                    "");

        tt_buf_clear(&buf);
        entry.content = "12345678";
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_buf_cmp_cstr(&buf, "at least 7 char: [12345678]. end"),
                    0,
                    "");

        tt_logfld_destroy(lpf);
    }

    tt_buf_destroy(&buf);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_lpatn_func)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_logfld_t *lpf;
    tt_result_t ret;
    tt_buf_t buf;
    tt_log_entry_t entry = {0};

    // at least 3 chars
    tt_char_t f04[] = "{function";
    tt_char_t f05[] = "{function:";
    tt_char_t f06[] = "{function}";
    tt_char_t f07[] = "{function-}";
    tt_char_t f1[] = "{function:}";
    tt_char_t f2[] = "{function:at least 7 char: [%7s]. end}";

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&buf, NULL);

    ret = tt_logfld_check(&f04[0], &f04[sizeof(f04) - 2]);
    TT_UT_FAIL(ret, "");

    ret = tt_logfld_check(&f05[0], &f05[sizeof(f05) - 2]);
    TT_UT_FAIL(ret, "");

    ret = tt_logfld_check(&f07[0], &f07[sizeof(f07) - 2]);
    TT_UT_FAIL(ret, "");
    lpf = tt_logfld_create(&f07[0], &f07[sizeof(f07) - 2]);
    TT_UT_EQUAL(lpf, NULL, "");

    // {function}
    ret = tt_logfld_check(&f06[0], &f06[sizeof(f06) - 2]);
    TT_UT_SUCCESS(ret, "");

    {
        lpf = tt_logfld_create(&f06[0], &f06[sizeof(f06) - 2]);
        TT_UT_NOT_EQUAL(lpf, NULL, "");

        tt_buf_clear(&buf);

        entry.function = NULL;
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(TT_BUF_RLEN(&buf), 0, "");

        entry.function = "";
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(TT_BUF_RLEN(&buf), 0, "");

        entry.function = "123";
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_buf_cmp_cstr(&buf, "123"), 0, "");

        tt_logfld_destroy(lpf);
    }

    // {function:}
    ret = tt_logfld_check(&f1[0], &f1[sizeof(f1) - 2]);
    TT_UT_SUCCESS(ret, "");

    {
        lpf = tt_logfld_create(&f1[0], &f1[sizeof(f1) - 2]);
        TT_UT_NOT_EQUAL(lpf, NULL, "");

        tt_buf_clear(&buf);

        entry.function = NULL;
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(TT_BUF_RLEN(&buf), 0, "");

        entry.function = "";
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(TT_BUF_RLEN(&buf), 0, "");

        entry.function = "123";
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_buf_cmp_cstr(&buf, "123"), 0, "");

        tt_logfld_destroy(lpf);
    }

    // {function:%07s}
    ret = tt_logfld_check(&f2[0], &f2[sizeof(f2) - 2]);
    TT_UT_SUCCESS(ret, "");

    {
        lpf = tt_logfld_create(&f2[0], &f2[sizeof(f2) - 2]);
        TT_UT_NOT_EQUAL(lpf, NULL, "");

        tt_buf_clear(&buf);

        entry.function = NULL;
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(TT_BUF_RLEN(&buf), 0, "");

        tt_buf_clear(&buf);
        entry.function = "";
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_buf_cmp_cstr(&buf, "at least 7 char: [       ]. end"),
                    0,
                    "");

        tt_buf_clear(&buf);
        entry.function = "1";
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_buf_cmp_cstr(&buf, "at least 7 char: [      1]. end"),
                    0,
                    "");

        tt_buf_clear(&buf);
        entry.function = "12345678";
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "1");
        TT_UT_EQUAL(tt_buf_cmp_cstr(&buf, "at least 7 char: [12345678]. end"),
                    0,
                    "");

        tt_logfld_destroy(lpf);
    }

    tt_buf_destroy(&buf);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_lpatn_line)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_logfld_t *lpf;
    tt_result_t ret;
    tt_buf_t buf;
    tt_log_entry_t entry = {0};

    // at least 3 chars
    tt_char_t f01[] = "{ }";
    tt_char_t f02[] = "{:}";
    tt_char_t f03[] = "{::";
    tt_char_t f04[] = "{line";
    tt_char_t f05[] = "{line:";
    tt_char_t f06[] = "{line}";
    tt_char_t f07[] = "{line1}";
    tt_char_t f1[] = "{line:}";
    tt_char_t f2[] = "{line:%+04d}";

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&buf, NULL);

    ret = tt_logfld_check(&f01[0], &f01[sizeof(f01) - 2]);
    TT_UT_FAIL(ret, "");

    ret = tt_logfld_check(&f02[0], &f02[sizeof(f02) - 2]);
    TT_UT_FAIL(ret, "");

    ret = tt_logfld_check(&f03[0], &f03[sizeof(f03) - 2]);
    TT_UT_FAIL(ret, "");

    ret = tt_logfld_check(&f04[0], &f04[sizeof(f04) - 2]);
    TT_UT_FAIL(ret, "");

    ret = tt_logfld_check(&f05[0], &f05[sizeof(f05) - 2]);
    TT_UT_FAIL(ret, "");

    ret = tt_logfld_check(&f07[0], &f07[sizeof(f07) - 2]);
    TT_UT_FAIL(ret, "");
    lpf = tt_logfld_create(&f07[0], &f07[sizeof(f07) - 2]);
    TT_UT_EQUAL(lpf, NULL, "");

    // {line}
    ret = tt_logfld_check(&f06[0], &f06[sizeof(f06) - 2]);
    TT_UT_SUCCESS(ret, "");

    {
        lpf = tt_logfld_create(&f06[0], &f06[sizeof(f06) - 2]);
        TT_UT_NOT_EQUAL(lpf, NULL, "");

        tt_buf_clear(&buf);

        entry.line = 0;
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_buf_cmp_cstr(&buf, "0"), 0, "");

        tt_logfld_destroy(lpf);
    }

    // {line:}
    ret = tt_logfld_check(&f1[0], &f1[sizeof(f1) - 2]);
    TT_UT_SUCCESS(ret, "");

    {
        lpf = tt_logfld_create(&f1[0], &f1[sizeof(f1) - 2]);
        TT_UT_NOT_EQUAL(lpf, NULL, "");

        tt_buf_clear(&buf);
        entry.line = 2147483647;
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_buf_cmp_cstr(&buf, "2147483647"), 0, "");

        tt_buf_clear(&buf);
        entry.line = (tt_u32_t)-2147483648LL;
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_buf_cmp_cstr(&buf, "-2147483648"), 0, "");

        tt_buf_clear(&buf);
        entry.line = 100;
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_buf_cmp_cstr(&buf, "100"), 0, "");

        tt_logfld_destroy(lpf);
    }

    // {line:%+04x}
    ret = tt_logfld_check(&f2[0], &f2[sizeof(f2) - 2]);
    TT_UT_SUCCESS(ret, "");

    {
        lpf = tt_logfld_create(&f2[0], &f2[sizeof(f2) - 2]);
        TT_UT_NOT_EQUAL(lpf, NULL, "");

        tt_buf_clear(&buf);

        tt_buf_clear(&buf);
        entry.line = 1;
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_buf_cmp_cstr(&buf, "+001"), 0, "");

        tt_buf_clear(&buf);
        entry.line = (tt_u32_t)-2147483648LL;
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_buf_cmp_cstr(&buf, "-2147483648"), 0, "");

        tt_buf_clear(&buf);
        entry.line = 10;
        ret = tt_logfld_output(lpf, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_buf_cmp_cstr(&buf, "+010"), 0, "");

        tt_logfld_destroy(lpf);
    }

    tt_buf_destroy(&buf);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_log_pattern)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_loglyt_t *ll;
    tt_result_t ret;
    tt_log_entry_t entry = {0};
    tt_buf_t buf;
    tt_s32_t cmp_ret;

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&buf, NULL);

    entry.seq_num = 100;
    entry.level = TT_LOG_FATAL;
    entry.function = __FUNCTION__;
    entry.content = "log content";
    entry.logger = "me";
    entry.line = 1234;

    // basic
    {
        ll = tt_loglyt_pattern_create(
            "$${seq_num:%6d} $ ${level:%.2s} $}${function:} ${content}"
            "${logger:$$%s} ${line}\n");
        TT_UT_NOT_EQUAL(ll, NULL, "");

        tt_buf_clear(&buf);
        ret = tt_loglyt_format(ll, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        cmp_ret = tt_buf_cmp_cstr(&buf,
                                  "$   100 $ FA "
                                  "$}case_log_pattern log "
                                  "content$$me "
                                  "1234\n");
        TT_UT_EQUAL(cmp_ret, 0, "");

        tt_loglyt_release(ll);
    }

    // wront format
    ll = tt_loglyt_pattern_create("${content} ${level:%d\n");
    TT_UT_EQUAL(ll, NULL, "");

    ll = tt_loglyt_pattern_create("-- ${level} ${ function}\n");
    TT_UT_EQUAL(ll, NULL, "");

    ll = tt_loglyt_pattern_create("-- ${level} ${}\n");
    TT_UT_EQUAL(ll, NULL, "");

    ll = tt_loglyt_pattern_create("${level } ${function}\n");
    TT_UT_EQUAL(ll, NULL, "");

    {
        ll = tt_loglyt_pattern_create(
            "$${level} $${function:{${}{}} $${content} 1234");
        TT_UT_NOT_EQUAL(ll, NULL, "");

        tt_buf_clear(&buf);
        ret = tt_loglyt_format(ll, &entry, &buf);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_buf_cmp_cstr(&buf, "$FATAL ${${{}} $log content 1234"),
                    0,
                    "");

        tt_loglyt_release(ll);
    }

    tt_buf_destroy(&buf);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_log_syslog3164)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_loglyt_t *ll;
    tt_result_t ret;
    tt_buf_t buf;
    tt_log_entry_t le = {0};
    const tt_char_t *p;
    tt_char_t dn[16] = {0};

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&buf, NULL);

    ll = tt_loglyt_syslog3164_create(TT_SYSLOG_LPR,
                                     TT_SYSLOG_CRIT,
                                     "testhost",
                                     NULL,
                                     "${level} ${content}\n");
    TT_UT_NOT_NULL(ll, "");

    le.content = "log content";
    le.level = TT_LOG_DEBUG;
    ret = tt_loglyt_format(ll, &le, &buf);
    TT_UT_SUCCESS(ret, "");

    p = (const tt_char_t *)TT_BUF_RPOS(&buf);
    TT_UT_EQUAL(tt_strncmp(p, "<50>", 4), 0, "");
#if TT_ENV_OS_IS_ANDROID
    // android can not get process name until 21
    TT_UT_NOT_NULL(tt_strstr(p, "testhost"), "");
#else
    TT_UT_NOT_NULL(tt_strstr(p, "testhost unit_test"), "");
#endif
    TT_UT_NOT_NULL(tt_strstr(p, "DEBUG log content"), "");

    tt_date_render_now("%b %d %H:", dn, sizeof(dn) - 1);
    TT_UT_NOT_NULL(tt_strstr(p, dn), "");

#if 0
    {
        tt_sktaddr_t addr;
        tt_logio_t *lio;
        tt_u32_t i;
        
        tt_sktaddr_init(&addr, TT_NET_AF_INET);
        tt_sktaddr_set_ip_p(&addr, "192.168.140.170");
        tt_sktaddr_set_port(&addr, 62000);

        lio = tt_logio_udp_create(TT_NET_AF_INET, &addr, NULL);
        
        i = 0;
        while (i++ < 10) {
            tt_logio_output(lio, (tt_char_t*)TT_BUF_RPOS(&buf), TT_BUF_RLEN(&buf));
            tt_sleep(100);
        }
        
        tt_logio_release(lio);
    }
#endif

    tt_loglyt_release(ll);
    tt_buf_destroy(&buf);

    // test end
    TT_TEST_CASE_LEAVE()
}
