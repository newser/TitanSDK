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

//#define TT_VERSION_REQUIRE_MAJOR 256
//#define TT_VERSION_REQUIRE_MINOR 256

#include <time/tt_date_def.h>
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
TT_TEST_ROUTINE_DECLARE(case_date_def)
TT_TEST_ROUTINE_DECLARE(case_date_render)
TT_TEST_ROUTINE_DECLARE(case_date_parse)
TT_TEST_ROUTINE_DECLARE(case_date_year)
TT_TEST_ROUTINE_DECLARE(case_date_month)
TT_TEST_ROUTINE_DECLARE(case_date_day)
TT_TEST_ROUTINE_DECLARE(case_date_time)
TT_TEST_ROUTINE_DECLARE(case_date_cjdn)
TT_TEST_ROUTINE_DECLARE(case_date_inc_dec)
TT_TEST_ROUTINE_DECLARE(case_date_julian)
TT_TEST_ROUTINE_DECLARE(case_date_diff)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(date_case)

TT_TEST_CASE("case_date_def",
             "testing date definitions",
             case_date_def,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("case_date_render",
                 "testing date rendering",
                 case_date_render,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_date_parse",
                 "testing date parsing",
                 case_date_parse,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_date_year",
                 "testing date year",
                 case_date_year,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_date_month",
                 "testing date month",
                 case_date_month,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_date_day",
                 "testing date day",
                 case_date_day,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_date_time",
                 "testing date time",
                 case_date_time,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_date_cjdn",
                 "testing date cjdn",
                 case_date_cjdn,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_date_inc_dec",
                 "testing date inc/dec",
                 case_date_inc_dec,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_date_julian",
                 "testing date to/from julian day",
                 case_date_julian,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_date_diff",
                 "testing date difference",
                 case_date_diff,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(date_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(TEST_UNIT_DATE, 0, date_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(case_date_diff)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(case_date_def)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_date_t d;

    TT_TEST_CASE_ENTER()
    // test start

    TT_UT_EQUAL(tt_strcmp(tt_tmzone_name[TT_UTC_MINUS_12_00], "UTC-12:00"),
                0,
                "");
    TT_UT_EQUAL(tt_strcmp(tt_tmzone_name_iso8601[TT_UTC_MINUS_12_00], "-12:00"),
                0,
                "");
    TT_UT_EQUAL(tt_strcmp(tt_tmzone_name_rfc1123[TT_UTC_MINUS_12_00], "-1200"),
                0,
                "");

    TT_UT_EQUAL(tt_strcmp(tt_tmzone_name[TT_UTC_00_00], "UTC+00:00"), 0, "");
    TT_UT_EQUAL(tt_strcmp(tt_tmzone_name_rfc1123[TT_UTC_00_00], "GMT"), 0, "");
    TT_UT_EQUAL(tt_strcmp(tt_tmzone_name_iso8601[TT_UTC_00_00], "Z"), 0, "");

    TT_UT_EQUAL(tt_strcmp(tt_tmzone_name[TT_UTC_14_00], "UTC+14:00"), 0, "");
    TT_UT_EQUAL(tt_strcmp(tt_tmzone_name_rfc1123[TT_UTC_14_00], "+1400"),
                0,
                "");
    TT_UT_EQUAL(tt_strcmp(tt_tmzone_name_iso8601[TT_UTC_14_00], "+14:00"),
                0,
                "");

    TT_UT_EQUAL(tt_strcmp(tt_month_name[TT_JANUARY], "January"), 0, "");
    TT_UT_EQUAL(tt_strcmp(tt_month_name[TT_DECEMBER], "December"), 0, "");

    TT_UT_EQUAL(tt_strcmp(tt_weekday_name[TT_SUNDAY], "Sunday"), 0, "");
    TT_UT_EQUAL(tt_strcmp(tt_weekday_name[TT_SATURDAY], "Saturday"), 0, "");

    TT_UT_EQUAL(tt_offsec2tmzone(-100000000), TT_UTC_MINUS_12_00, "");
    TT_UT_EQUAL(tt_offsec2tmzone(-43200), TT_UTC_MINUS_12_00, "");
    TT_UT_EQUAL(tt_offsec2tmzone(-43200 + 1), TT_UTC_MINUS_11_00, "");
    TT_UT_EQUAL(tt_offsec2tmzone(-12600), TT_UTC_MINUS_03_30, "");
    TT_UT_EQUAL(tt_offsec2tmzone(-12600 + 1), TT_UTC_MINUS_03_00, "");
    TT_UT_EQUAL(tt_offsec2tmzone(-1), TT_UTC_00_00, "");
    TT_UT_EQUAL(tt_offsec2tmzone(0), TT_UTC_00_00, "");
    TT_UT_EQUAL(tt_offsec2tmzone(1), TT_UTC_01_00, "");
    TT_UT_EQUAL(tt_offsec2tmzone(31500), TT_UTC_08_45, "");
    TT_UT_EQUAL(tt_offsec2tmzone(50400 - 1), TT_UTC_14_00, "");
    TT_UT_EQUAL(tt_offsec2tmzone(50400), TT_UTC_14_00, "");
    TT_UT_EQUAL(tt_offsec2tmzone(50400 + 1), TT_UTC_14_00, "");

    TT_UT_EQUAL(tt_tmzone2offsec(TT_UTC_MINUS_12_00), (-43200), "");
    TT_UT_EQUAL(tt_tmzone2offsec(TT_UTC_00_00), 0, "");
    TT_UT_EQUAL(tt_tmzone2offsec(TT_UTC_08_45), 31500, "");
    TT_UT_EQUAL(tt_tmzone2offsec(TT_UTC_14_00), 50400, "");

    tt_date_init(&d, TT_UTC_08_00);
    TT_UT_SUCCESS(tt_date_set(&d, 2017, TT_OCTOBER, 7, 20, 0, 0), "");

    tt_date_change_tmzone(&d, TT_UTC_MINUS_07_00);
    TT_UT_EQUAL(tt_date_get_month(&d), TT_OCTOBER, "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 7, "");
    TT_UT_EQUAL(tt_date_get_hour(&d), 5, "");
    TT_UT_EQUAL(tt_date_get_minute(&d), 0, "");
    TT_UT_EQUAL(tt_date_get_second(&d), 0, "");

    tt_date_change_tmzone(&d, TT_UTC_09_00);
    TT_UT_EQUAL(tt_date_get_month(&d), TT_OCTOBER, "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 7, "");
    TT_UT_EQUAL(tt_date_get_hour(&d), 21, "");
    TT_UT_EQUAL(tt_date_get_minute(&d), 0, "");
    TT_UT_EQUAL(tt_date_get_second(&d), 0, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_date_render)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_date_t d;
    tt_char_t buf[100];
    tt_u32_t ret;

    TT_TEST_CASE_ENTER()
    // test start

    tt_date_init(&d, TT_UTC_08_00);

    // %Y
    tt_date_set_year(&d, 1923);
    ret = tt_date_render(&d, "%Y", buf, sizeof(buf));
    TT_UT_EQUAL(ret, 2, "");
    TT_UT_EQUAL(tt_strcmp(buf, "23"), 0, "");

    // %y
    tt_date_set_year(&d, 1900);
    ret = tt_date_render(&d, "%y", buf, sizeof(buf));
    TT_UT_EQUAL(ret, 1, "");
    TT_UT_EQUAL(tt_strcmp(buf, "0"), 0, "");

    // %C
    tt_date_set_year(&d, 1900);
    ret = tt_date_render(&d, "%C", buf, sizeof(buf));
    TT_UT_EQUAL(ret, 4, "");
    TT_UT_EQUAL(tt_strcmp(buf, "1900"), 0, "");

    // %B
    tt_date_set_month(&d, TT_JANUARY);
    ret = tt_date_render(&d, "%B", buf, sizeof(buf));
    TT_UT_EQUAL(ret, sizeof("January") - 1, "");
    TT_UT_EQUAL(tt_strcmp(buf, "January"), 0, "");
    tt_date_set_month(&d, TT_DECEMBER);
    ret = tt_date_render(&d, "%B", buf, sizeof(buf));
    TT_UT_EQUAL(ret, sizeof("December") - 1, "");
    TT_UT_EQUAL(tt_strcmp(buf, "December"), 0, "");

    // %b
    tt_date_set_month(&d, TT_JANUARY);
    ret = tt_date_render(&d, "%b", buf, sizeof(buf));
    TT_UT_EQUAL(ret, sizeof("Jan") - 1, "");
    TT_UT_EQUAL(tt_strcmp(buf, "Jan"), 0, "");
    tt_date_set_month(&d, TT_DECEMBER);
    ret = tt_date_render(&d, "%b", buf, sizeof(buf));
    TT_UT_EQUAL(ret, sizeof("Dec") - 1, "");
    TT_UT_EQUAL(tt_strcmp(buf, "Dec"), 0, "");

    // %D
    tt_date_set_monthday(&d, 9);
    ret = tt_date_render(&d, "%D", buf, sizeof(buf));
    TT_UT_EQUAL(ret, 2, "");
    TT_UT_EQUAL(tt_strcmp(buf, "09"), 0, "");

    // %d
    tt_date_set_monthday(&d, 9);
    ret = tt_date_render(&d, "%d", buf, sizeof(buf));
    TT_UT_EQUAL(ret, 2, "");
    TT_UT_EQUAL(tt_strcmp(buf, " 9"), 0, "");

    // %H
    tt_date_set_hour(&d, 12);
    ret = tt_date_render(&d, "%H", buf, sizeof(buf));
    TT_UT_EQUAL(ret, 2, "");
    TT_UT_EQUAL(tt_strcmp(buf, "12"), 0, "");

    // %A/%a
    tt_date_set_hour(&d, 12);
    ret = tt_date_render(&d, "%A", buf, sizeof(buf));
    TT_UT_EQUAL(ret, 2, "");
    TT_UT_EQUAL(tt_strcmp(buf, "PM"), 0, "");
    ret = tt_date_render(&d, "%a", buf, sizeof(buf));
    TT_UT_EQUAL(ret, 2, "");
    TT_UT_EQUAL(tt_strcmp(buf, "pm"), 0, "");

    tt_date_set_hour(&d, 0);
    ret = tt_date_render(&d, "%A", buf, sizeof(buf));
    TT_UT_EQUAL(ret, 2, "");
    TT_UT_EQUAL(tt_strcmp(buf, "AM"), 0, "");
    ret = tt_date_render(&d, "%a", buf, sizeof(buf));
    TT_UT_EQUAL(ret, 2, "");
    TT_UT_EQUAL(tt_strcmp(buf, "am"), 0, "");

    // %h
    tt_date_set_monthday(&d, 12);
    ret = tt_date_render(&d, "%h", buf, sizeof(buf));
    TT_UT_EQUAL(ret, 2, "");
    TT_UT_EQUAL(tt_strcmp(buf, "00"), 0, "");

    // %M
    tt_date_set_minute(&d, 9);
    ret = tt_date_render(&d, "%M", buf, sizeof(buf));
    TT_UT_EQUAL(ret, 2, "");
    TT_UT_EQUAL(tt_strcmp(buf, "09"), 0, "");

    // %S
    tt_date_set_second(&d, 9);
    ret = tt_date_render(&d, "%S", buf, sizeof(buf));
    TT_UT_EQUAL(ret, 2, "");
    TT_UT_EQUAL(tt_strcmp(buf, "09"), 0, "");

    // %Z
    tt_date_set_tmzone(&d, TT_UTC_04_30);
    ret = tt_date_render(&d, "%Z", buf, sizeof(buf));
    TT_UT_EQUAL(ret, sizeof("+0430") - 1, "");
    TT_UT_EQUAL(tt_strcmp(buf, "+0430"), 0, "");
    ret = tt_date_render(&d, "%z", buf, sizeof(buf));
    TT_UT_EQUAL(ret, sizeof("+04:30") - 1, "");
    TT_UT_EQUAL(tt_strcmp(buf, "+04:30"), 0, "");

    // %z
    tt_date_set_tmzone(&d, TT_UTC_00_00);
    ret = tt_date_render(&d, "%Z", buf, sizeof(buf));
    TT_UT_EQUAL(ret, sizeof("GMT") - 1, "");
    TT_UT_EQUAL(tt_strcmp(buf, "GMT"), 0, "");
    ret = tt_date_render(&d, "%z", buf, sizeof(buf));
    TT_UT_EQUAL(ret, sizeof("Z") - 1, "");
    TT_UT_EQUAL(tt_strcmp(buf, "Z"), 0, "");

    tt_date_set(&d, 2017, TT_OCTOBER, 8, 23, 59, 01);
    ret = tt_date_render(&d, "%W", buf, sizeof(buf));
    TT_UT_EQUAL(ret, sizeof("Sunday") - 1, "");
    TT_UT_EQUAL(tt_strcmp(buf, "Sunday"), 0, "");
    ret = tt_date_render(&d, "%w", buf, sizeof(buf));
    TT_UT_EQUAL(ret, sizeof("Sun") - 1, "");
    TT_UT_EQUAL(tt_strcmp(buf, "Sun"), 0, "");

    // combination, RFC1123
    tt_date_set(&d, 2017, TT_OCTOBER, 8, 23, 59, 01);
    tt_date_set_tmzone(&d, TT_UTC_MINUS_03_30);
    ret =
        tt_date_render(&d, "--- %w, %D %b %C %H:%M:%S %Z%%%", buf, sizeof(buf));
    TT_UT_EQUAL(ret, sizeof("--- Sun, 08 Oct 2017 23:59:01 -0330%%%") - 1, "");
    TT_UT_EQUAL(tt_strcmp(buf, "--- Sun, 08 Oct 2017 23:59:01 -0330%%%"),
                0,
                "");

    // combination, RFC850
    tt_date_set(&d, 2017, TT_OCTOBER, 8, 23, 59, 01);
    tt_date_set_tmzone(&d, TT_UTC_MINUS_04_00);
    ret = tt_date_render(&d, "%W, %D-%b-%C %H:%M:%S %z %%%", buf, sizeof(buf));
    TT_UT_EQUAL(ret, sizeof("Sunday, 08-Oct-2017 23:59:01 -04:00 %%%") - 1, "");
    TT_UT_EQUAL(tt_strcmp(buf, "Sunday, 08-Oct-2017 23:59:01 -04:00 %%%"),
                0,
                "");

    // combination, ascii
    tt_date_set(&d, 2017, TT_OCTOBER, 8, 23, 59, 01);
    tt_date_set_tmzone(&d, TT_UTC_00_00);
    ret = tt_date_render(&d, "--- %w %b %d %H:%M:%S %C %Z", buf, sizeof(buf));
    TT_UT_EQUAL(ret, sizeof("--- Sun Oct  8 23:59:01 2017 GMT") - 1, "");
    TT_UT_EQUAL(tt_strcmp(buf, "--- Sun Oct  8 23:59:01 2017 GMT"), 0, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_date_parse)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_date_t d;
    tt_result_t ret;

    TT_TEST_CASE_ENTER()
    // test start

    tt_date_init(&d, tt_g_local_tmzone);

    ret = tt_date_parse(&d,
                        "%Y-%m-%d %H-%M-%S",
                        "2020-11-31 23-59-59",
                        sizeof("2020-11-31 23-59-59"));
    TT_UT_NOT_EQUAL(ret, 0, "");
    TT_UT_EQUAL(ret, sizeof("2020-11-31 23-59-59") - 1, "");
    TT_UT_EQUAL(tt_date_get_year(&d), 2020, "");
    TT_UT_EQUAL(tt_date_get_month(&d), TT_NOVEMBER, "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 31, "");
    TT_UT_EQUAL(tt_date_get_hour(&d), 23, "");
    TT_UT_EQUAL(tt_date_get_minute(&d), 59, "");
    TT_UT_EQUAL(tt_date_get_second(&d), 59, "");

    ret = tt_date_parse(
        &d,
        "%Y-%m-%d %H-%M-%S",
        "2020-11-31 23-59-59 2020-11-31 23-59-59 2020-11-31 23-59-59"
        "2020-11-31 23-59-59 2020-11-31 23-59-59 2020-11-31 23-59-59"
        "2020-11-31 23-59-59 2020-11-31 23-59-59 2020-11-31 23-59-59"
        "2020-11-31 23-59-59 2020-11-31 23-59-59 2020-11-31 23-59-59",
        127);
    TT_UT_NOT_EQUAL(ret, 0, "");
    TT_UT_EQUAL(ret, sizeof("2020-11-31 23-59-59") - 1, "");
    TT_UT_EQUAL(tt_date_get_year(&d), 2020, "");

    ret = tt_date_parse(
        &d,
        "%Y-%m-%d %H-%M-%S",
        "2020-11-31 23-59-59 2020-11-31 23-59-59 2020-11-31 23-59-59"
        "2020-11-31 23-59-59 2020-11-31 23-59-59 2020-11-31 23-59-59"
        "2020-11-31 23-59-59 2020-11-31 23-59-59 2020-11-31 23-59-59"
        "2020-11-31 23-59-59 2020-11-31 23-59-59 2020-11-31 23-59-59",
        128);
    TT_UT_NOT_EQUAL(ret, 0, "");
    TT_UT_EQUAL(ret, sizeof("2020-11-31 23-59-59") - 1, "");
    TT_UT_EQUAL(tt_date_get_year(&d), 2020, "");

    ret = tt_date_parse(
        &d,
        "%Y-%m-%d %H-%M-%S",
        "2020-11-31 23-59-59 2020-11-31 23-59-59 2020-11-31 23-59-59"
        "2020-11-31 23-59-59 2020-11-31 23-59-59 2020-11-31 23-59-59"
        "2020-11-31 23-59-59 2020-11-31 23-59-59 2020-11-31 23-59-59"
        "2020-11-31 23-59-59 2020-11-31 23-59-59 2020-11-31 23-59-59",
        130);
    TT_UT_NOT_EQUAL(ret, 0, "");
    TT_UT_EQUAL(ret, sizeof("2020-11-31 23-59-59") - 1, "");
    TT_UT_EQUAL(tt_date_get_second(&d), 59, "");

    ret = tt_date_parse(&d,
                        "%Y-%m-%d %H-%M-%S",
                        "2020-11-31",
                        sizeof("2020-11-31"));
    TT_UT_EQUAL(ret, 0, "");

    ret = tt_date_parse(&d,
                        "%Y-%m-%d %H-%M-%S",
                        "2020-13-31 23-59-59",
                        sizeof("2020-13-31 23-59-59"));
    TT_UT_EQUAL(ret, 0, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

extern tt_u32_t __year_daynum(IN tt_u32_t year);

TT_TEST_ROUTINE_DEFINE(case_date_year)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_date_t d;

    TT_TEST_CASE_ENTER()
    // test start

    tt_date_init(&d, tt_g_local_tmzone);

    TT_UT_EQUAL(tt_date_get_year(&d), 1583, "");
    TT_UT_EQUAL(tt_date_is_leapyear(&d), TT_FALSE, "");

    TT_UT_FAIL(tt_date_set_year(&d, 1582), "");
    TT_UT_EQUAL(tt_date_get_year(&d), 1583, "");
    TT_UT_EQUAL(tt_date_is_leapyear(&d), TT_FALSE, "");

    TT_UT_SUCCESS(tt_date_set_year(&d, 1584), "");
    TT_UT_EQUAL(tt_date_get_year(&d), 1584, "");
    TT_UT_EQUAL(tt_date_is_leapyear(&d), TT_TRUE, "");

    TT_UT_SUCCESS(tt_date_set_year(&d, 1700), "");
    TT_UT_EQUAL(tt_date_get_year(&d), 1700, "");
    TT_UT_EQUAL(tt_date_is_leapyear(&d), TT_FALSE, "");

    TT_UT_SUCCESS(tt_date_set_year(&d, 2000), "");
    TT_UT_EQUAL(tt_date_get_year(&d), 2000, "");
    TT_UT_EQUAL(tt_date_is_leapyear(&d), TT_TRUE, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_date_month)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_date_t d;

    TT_TEST_CASE_ENTER()
    // test start

    tt_date_init(&d, tt_g_local_tmzone);

    tt_date_set_year(&d, 1587);

    tt_date_set_month(&d, TT_JANUARY);
    TT_UT_EQUAL(tt_date_get_month(&d), TT_JANUARY, "");
    TT_UT_SUCCESS(tt_date_set_monthday(&d, 1), "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 1, "");
    TT_UT_SUCCESS(tt_date_set_monthday(&d, 31), "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 31, "");
    TT_UT_FAIL(tt_date_set_monthday(&d, 32), "");

    tt_date_set_month(&d, TT_FEBRUARY);
    TT_UT_EQUAL(tt_date_get_month(&d), TT_FEBRUARY, "");
    TT_UT_SUCCESS(tt_date_set_monthday(&d, 1), "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 1, "");
    TT_UT_SUCCESS(tt_date_set_monthday(&d, 28), "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 28, "");
    TT_UT_FAIL(tt_date_set_monthday(&d, 29), "");

    tt_date_set_month(&d, TT_MARCH);
    TT_UT_SUCCESS(tt_date_set_monthday(&d, 31), "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 31, "");
    TT_UT_FAIL(tt_date_set_monthday(&d, 32), "");
    tt_date_set_month(&d, TT_APRIL);
    TT_UT_SUCCESS(tt_date_set_monthday(&d, 30), "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 30, "");
    TT_UT_FAIL(tt_date_set_monthday(&d, 31), "");
    tt_date_set_month(&d, TT_MAY);
    TT_UT_SUCCESS(tt_date_set_monthday(&d, 31), "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 31, "");
    TT_UT_FAIL(tt_date_set_monthday(&d, 32), "");
    tt_date_set_month(&d, TT_JUNE);
    TT_UT_SUCCESS(tt_date_set_monthday(&d, 30), "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 30, "");
    TT_UT_FAIL(tt_date_set_monthday(&d, 31), "");
    tt_date_set_month(&d, TT_JULY);
    TT_UT_SUCCESS(tt_date_set_monthday(&d, 31), "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 31, "");
    TT_UT_FAIL(tt_date_set_monthday(&d, 32), "");
    tt_date_set_month(&d, TT_AUGUST);
    TT_UT_SUCCESS(tt_date_set_monthday(&d, 31), "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 31, "");
    TT_UT_FAIL(tt_date_set_monthday(&d, 32), "");
    tt_date_set_month(&d, TT_SEPTEMBER);
    TT_UT_SUCCESS(tt_date_set_monthday(&d, 30), "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 30, "");
    TT_UT_FAIL(tt_date_set_monthday(&d, 31), "");
    tt_date_set_month(&d, TT_OCTOBER);
    TT_UT_SUCCESS(tt_date_set_monthday(&d, 31), "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 31, "");
    TT_UT_FAIL(tt_date_set_monthday(&d, 32), "");
    tt_date_set_month(&d, TT_NOVEMBER);
    TT_UT_SUCCESS(tt_date_set_monthday(&d, 30), "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 30, "");
    TT_UT_FAIL(tt_date_set_monthday(&d, 31), "");
    tt_date_set_month(&d, TT_DECEMBER);
    TT_UT_SUCCESS(tt_date_set_monthday(&d, 31), "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 31, "");
    TT_UT_FAIL(tt_date_set_monthday(&d, 32), "");

    // leap year
    tt_date_set_year(&d, 2000);
    tt_date_set_month(&d, TT_FEBRUARY);
    TT_UT_EQUAL(tt_date_get_month(&d), TT_FEBRUARY, "");
    TT_UT_SUCCESS(tt_date_set_monthday(&d, 1), "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 1, "");
    TT_UT_SUCCESS(tt_date_set_monthday(&d, 28), "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 28, "");
    TT_UT_SUCCESS(tt_date_set_monthday(&d, 29), "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 29, "");
    TT_UT_FAIL(tt_date_set_monthday(&d, 30), "");

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_date_day)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_date_t d;

    TT_TEST_CASE_ENTER()
    // test start

    tt_date_init(&d, tt_g_local_tmzone);

    tt_date_set_year(&d, 2017);
    tt_date_set_month(&d, TT_OCTOBER);
    tt_date_set_monthday(&d, 5);
    TT_UT_EQUAL(tt_date_get_weekday(&d), TT_THURSDAY, "");

    TT_UT_SUCCESS(tt_date_set_date(&d, 1901, TT_JANUARY, 1), "");
    TT_UT_EQUAL(tt_date_get_weekday(&d), TT_TUESDAY, "");

    TT_UT_SUCCESS(tt_date_set_date(&d, 1584, TT_FEBRUARY, 29), "");
    TT_UT_EQUAL(tt_date_get_weekday(&d), TT_WEDNESDAY, "");

    TT_UT_SUCCESS(tt_date_set_date(&d, 1583, TT_JANUARY, 1), "");
    TT_UT_EQUAL(tt_date_get_weekday(&d), TT_SATURDAY, "");

    // get year day
    TT_UT_SUCCESS(tt_date_set_date(&d, 1999, TT_JANUARY, 1), "");
    TT_UT_EQUAL(tt_date_get_yearday(&d), 0, "");
    TT_UT_SUCCESS(tt_date_set_date(&d, 1999, TT_FEBRUARY, 28), "");
    TT_UT_EQUAL(tt_date_get_yearday(&d), 58, "");
    TT_UT_SUCCESS(tt_date_set_date(&d, 1999, TT_MARCH, 1), "");
    TT_UT_EQUAL(tt_date_get_yearday(&d), 59, "");
    TT_UT_SUCCESS(tt_date_set_date(&d, 1999, TT_DECEMBER, 31), "");
    TT_UT_EQUAL(tt_date_get_yearday(&d), 364, "");

    // get leap year day
    TT_UT_SUCCESS(tt_date_set_date(&d, 2000, TT_JANUARY, 1), "");
    TT_UT_EQUAL(tt_date_get_yearday(&d), 0, "");
    TT_UT_SUCCESS(tt_date_set_date(&d, 2000, TT_FEBRUARY, 29), "");
    TT_UT_EQUAL(tt_date_get_yearday(&d), 59, "");
    TT_UT_SUCCESS(tt_date_set_date(&d, 2000, TT_MARCH, 1), "");
    TT_UT_EQUAL(tt_date_get_yearday(&d), 60, "");
    TT_UT_SUCCESS(tt_date_set_date(&d, 2000, TT_DECEMBER, 31), "");
    TT_UT_EQUAL(tt_date_get_yearday(&d), 365, "");

    // set year day
    TT_UT_SUCCESS(tt_date_set_date(&d, 1999, TT_JANUARY, 1), "");
    TT_UT_FAIL(tt_date_set_yearday(&d, 365), "");

    TT_UT_SUCCESS(tt_date_set_yearday(&d, 364), "");
    TT_UT_EQUAL(tt_date_get_year(&d), 1999, "");
    TT_UT_EQUAL(tt_date_get_month(&d), TT_DECEMBER, "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 31, "");

    TT_UT_SUCCESS(tt_date_set_yearday(&d, 0), "");
    TT_UT_EQUAL(tt_date_get_year(&d), 1999, "");
    TT_UT_EQUAL(tt_date_get_month(&d), TT_JANUARY, "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 1, "");

    TT_UT_SUCCESS(tt_date_set_yearday(&d, 58), "");
    TT_UT_EQUAL(tt_date_get_year(&d), 1999, "");
    TT_UT_EQUAL(tt_date_get_month(&d), TT_FEBRUARY, "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 28, "");

    TT_UT_SUCCESS(tt_date_set_yearday(&d, 59), "");
    TT_UT_EQUAL(tt_date_get_year(&d), 1999, "");
    TT_UT_EQUAL(tt_date_get_month(&d), TT_MARCH, "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 1, "");

    // set leap year day
    TT_UT_SUCCESS(tt_date_set_date(&d, 2000, TT_JANUARY, 1), "");
    TT_UT_SUCCESS(tt_date_set_yearday(&d, 365), "");

    TT_UT_SUCCESS(tt_date_set_yearday(&d, 365), "");
    TT_UT_EQUAL(tt_date_get_year(&d), 2000, "");
    TT_UT_EQUAL(tt_date_get_month(&d), TT_DECEMBER, "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 31, "");

    TT_UT_SUCCESS(tt_date_set_yearday(&d, 0), "");
    TT_UT_EQUAL(tt_date_get_year(&d), 2000, "");
    TT_UT_EQUAL(tt_date_get_month(&d), TT_JANUARY, "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 1, "");

    TT_UT_SUCCESS(tt_date_set_yearday(&d, 59), "");
    TT_UT_EQUAL(tt_date_get_year(&d), 2000, "");
    TT_UT_EQUAL(tt_date_get_month(&d), TT_FEBRUARY, "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 29, "");

    TT_UT_SUCCESS(tt_date_set_yearday(&d, 60), "");
    TT_UT_EQUAL(tt_date_get_year(&d), 2000, "");
    TT_UT_EQUAL(tt_date_get_month(&d), TT_MARCH, "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 1, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_date_time)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_date_t d;

    TT_TEST_CASE_ENTER()
    // test start

    tt_date_init(&d, tt_g_local_tmzone);

    tt_date_set_hour(&d, 0);
    TT_UT_EQUAL(tt_date_get_hour(&d), 0, "");
    TT_UT_EQUAL(tt_date_get_hour_ampm(&d), 0, "");
    TT_UT_EQUAL(tt_date_is_am(&d), TT_TRUE, "");
    TT_UT_EQUAL(tt_date_is_pm(&d), TT_FALSE, "");

    tt_date_set_hour(&d, 12);
    TT_UT_EQUAL(tt_date_get_hour(&d), 12, "");
    TT_UT_EQUAL(tt_date_get_hour_ampm(&d), 12, "");
    TT_UT_EQUAL(tt_date_is_am(&d), TT_FALSE, "");
    TT_UT_EQUAL(tt_date_is_pm(&d), TT_TRUE, "");

    tt_date_set_hour(&d, 23);
    TT_UT_EQUAL(tt_date_get_hour(&d), 23, "");
    TT_UT_EQUAL(tt_date_get_hour_ampm(&d), 11, "");
    TT_UT_EQUAL(tt_date_is_am(&d), TT_FALSE, "");
    TT_UT_EQUAL(tt_date_is_pm(&d), TT_TRUE, "");

    tt_date_set_minute(&d, 0);
    TT_UT_EQUAL(tt_date_get_minute(&d), 0, "");
    tt_date_set_minute(&d, 30);
    TT_UT_EQUAL(tt_date_get_minute(&d), 30, "");
    tt_date_set_minute(&d, 59);
    TT_UT_EQUAL(tt_date_get_minute(&d), 59, "");

    tt_date_set_second(&d, 0);
    TT_UT_EQUAL(tt_date_get_second(&d), 0, "");
    tt_date_set_second(&d, 30);
    TT_UT_EQUAL(tt_date_get_second(&d), 30, "");
    tt_date_set_second(&d, 59);
    TT_UT_EQUAL(tt_date_get_second(&d), 59, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

extern tt_u32_t __ymd2cjdn(IN tt_u16_t year,
                           IN tt_month_t month,
                           IN tt_u8_t day);

extern void __cjdn2ymd(IN tt_u32_t cjdn,
                       OUT tt_u16_t *year,
                       OUT tt_month_t *month,
                       OUT tt_u8_t *day);

TT_TEST_ROUTINE_DEFINE(case_date_cjdn)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_date_t d;
    tt_u32_t cjdn;
    tt_month_t m;

    TT_TEST_CASE_ENTER()
    // test start

    tt_date_init(&d, tt_g_local_tmzone);

    // 2000/2/29
    TT_UT_SUCCESS(tt_date_set_date(&d, 2000, TT_FEBRUARY, 29), "");
    cjdn = __ymd2cjdn(d.year, d.month, d.mday);
    TT_UT_EQUAL(cjdn, 2451604, "");
    __cjdn2ymd(cjdn, &d.year, &m, &d.mday);
    d.month = m;
    TT_UT_EQUAL(tt_date_get_year(&d), 2000, "");
    TT_UT_EQUAL(tt_date_get_month(&d), TT_FEBRUARY, "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 29, "");

    // 2000/3/1
    TT_UT_SUCCESS(tt_date_set_date(&d, 2000, TT_MARCH, 1), "");
    cjdn = __ymd2cjdn(d.year, d.month, d.mday);
    TT_UT_EQUAL(cjdn, 2451605, "");
    __cjdn2ymd(cjdn, &d.year, &m, &d.mday);
    d.month = m;
    TT_UT_EQUAL(tt_date_get_year(&d), 2000, "");
    TT_UT_EQUAL(tt_date_get_month(&d), TT_MARCH, "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 1, "");

    // 2001/2/28
    TT_UT_SUCCESS(tt_date_set_date(&d, 2001, TT_FEBRUARY, 28), "");
    cjdn = __ymd2cjdn(d.year, d.month, d.mday);
    TT_UT_EQUAL(cjdn, 2451969, "");
    __cjdn2ymd(cjdn, &d.year, &m, &d.mday);
    d.month = m;
    TT_UT_EQUAL(tt_date_get_year(&d), 2001, "");
    TT_UT_EQUAL(tt_date_get_month(&d), TT_FEBRUARY, "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 28, "");

    // 2001/3/1
    TT_UT_SUCCESS(tt_date_set_date(&d, 2001, TT_MARCH, 1), "");
    cjdn = __ymd2cjdn(d.year, d.month, d.mday);
    TT_UT_EQUAL(cjdn, 2451970, "");
    __cjdn2ymd(cjdn, &d.year, &m, &d.mday);
    d.month = m;
    TT_UT_EQUAL(tt_date_get_year(&d), 2001, "");
    TT_UT_EQUAL(tt_date_get_month(&d), TT_MARCH, "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 1, "");

    // 2100/2/28
    TT_UT_SUCCESS(tt_date_set_date(&d, 2100, TT_FEBRUARY, 28), "");
    cjdn = __ymd2cjdn(d.year, d.month, d.mday);
    TT_UT_EQUAL(cjdn, 2488128, "");
    __cjdn2ymd(cjdn, &d.year, &m, &d.mday);
    d.month = m;
    TT_UT_EQUAL(tt_date_get_year(&d), 2100, "");
    TT_UT_EQUAL(tt_date_get_month(&d), TT_FEBRUARY, "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 28, "");

    // 2100/3/1
    TT_UT_SUCCESS(tt_date_set_date(&d, 2100, TT_MARCH, 1), "");
    cjdn = __ymd2cjdn(d.year, d.month, d.mday);
    TT_UT_EQUAL(cjdn, 2488129, "");
    __cjdn2ymd(cjdn, &d.year, &m, &d.mday);
    d.month = m;
    TT_UT_EQUAL(tt_date_get_year(&d), 2100, "");
    TT_UT_EQUAL(tt_date_get_month(&d), TT_MARCH, "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 1, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_date_inc_dec)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_date_t d, d2;

    TT_TEST_CASE_ENTER()
    // test start

    tt_date_init(&d, tt_g_local_tmzone);

    TT_UT_FAIL(tt_date_inc_day(&d, ~0), "");
    TT_UT_EQUAL(tt_date_cmp(&d, &d), 0, "");

    // 1999/1/1
    TT_UT_SUCCESS(tt_date_set_date(&d, 1999, TT_DECEMBER, 31), "");

    // cmp date
    TT_UT_SUCCESS(tt_date_set_date(&d2, 2000, TT_DECEMBER, 31), "");
    TT_UT_EQUAL(tt_date_cmp(&d, &d2), -1, "");
    TT_UT_EQUAL(tt_date_cmp(&d2, &d), 1, "");

    TT_UT_SUCCESS(tt_date_set_date(&d2, 1999, TT_NOVEMBER, 30), "");
    TT_UT_EQUAL(tt_date_cmp(&d, &d2), 1, "");
    TT_UT_EQUAL(tt_date_cmp(&d2, &d), -1, "");

    TT_UT_SUCCESS(tt_date_set_date(&d2, 1999, TT_DECEMBER, 30), "");
    TT_UT_EQUAL(tt_date_cmp(&d, &d2), 1, "");
    TT_UT_EQUAL(tt_date_cmp(&d2, &d), -1, "");

    // day
    TT_UT_SUCCESS(tt_date_inc_day(&d, 0), "");
    TT_UT_EQUAL(tt_date_get_year(&d), 1999, "");
    TT_UT_EQUAL(tt_date_get_month(&d), TT_DECEMBER, "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 31, "");

    TT_UT_SUCCESS(tt_date_inc_day(&d, 1), "");
    TT_UT_EQUAL(tt_date_get_year(&d), 2000, "");
    TT_UT_EQUAL(tt_date_get_month(&d), TT_JANUARY, "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 1, "");

    TT_UT_SUCCESS(tt_date_dec_day(&d, 1), "");
    TT_UT_EQUAL(tt_date_get_year(&d), 1999, "");
    TT_UT_EQUAL(tt_date_get_month(&d), TT_DECEMBER, "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 31, "");

    // hour
    tt_date_set_time(&d, 23, 59, 59);

    // cmp time
    tt_date_copy(&d2, &d);
    tt_date_set_time(&d2, 21, 59, 59);
    TT_UT_EQUAL(tt_date_cmp(&d, &d2), 1, "");
    TT_UT_EQUAL(tt_date_cmp(&d2, &d), -1, "");
    tt_date_set_time(&d2, 23, 58, 59);
    TT_UT_EQUAL(tt_date_cmp(&d, &d2), 1, "");
    TT_UT_EQUAL(tt_date_cmp(&d2, &d), -1, "");
    tt_date_set_time(&d2, 23, 59, 58);
    TT_UT_EQUAL(tt_date_cmp(&d, &d2), 1, "");
    TT_UT_EQUAL(tt_date_cmp(&d2, &d), -1, "");

    TT_UT_SUCCESS(tt_date_set_date(&d2, 1999, TT_NOVEMBER, 30), "");
    TT_UT_EQUAL(tt_date_cmp(&d, &d2), 1, "");
    TT_UT_EQUAL(tt_date_cmp(&d2, &d), -1, "");

    TT_UT_SUCCESS(tt_date_set_date(&d2, 1999, TT_DECEMBER, 30), "");
    TT_UT_EQUAL(tt_date_cmp(&d, &d2), 1, "");
    TT_UT_EQUAL(tt_date_cmp(&d2, &d), -1, "");

    TT_UT_SUCCESS(tt_date_inc_hour(&d, 0), "");
    TT_UT_EQUAL(tt_date_get_hour(&d), 23, "");
    TT_UT_EQUAL(tt_date_get_minute(&d), 59, "");
    TT_UT_EQUAL(tt_date_get_second(&d), 59, "");

    TT_UT_SUCCESS(tt_date_inc_hour(&d, 1), "");
    TT_UT_EQUAL(tt_date_get_year(&d), 2000, "");
    TT_UT_EQUAL(tt_date_get_month(&d), TT_JANUARY, "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 1, "");
    TT_UT_EQUAL(tt_date_get_hour(&d), 0, "");
    TT_UT_EQUAL(tt_date_get_minute(&d), 59, "");
    TT_UT_EQUAL(tt_date_get_second(&d), 59, "");

    TT_UT_SUCCESS(tt_date_dec_hour(&d, 1), "");
    TT_UT_EQUAL(tt_date_get_year(&d), 1999, "");
    TT_UT_EQUAL(tt_date_get_month(&d), TT_DECEMBER, "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 31, "");
    TT_UT_EQUAL(tt_date_get_hour(&d), 23, "");
    TT_UT_EQUAL(tt_date_get_minute(&d), 59, "");
    TT_UT_EQUAL(tt_date_get_second(&d), 59, "");

    TT_UT_SUCCESS(tt_date_inc_hour(&d, 25), "");
    TT_UT_EQUAL(tt_date_get_year(&d), 2000, "");
    TT_UT_EQUAL(tt_date_get_month(&d), TT_JANUARY, "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 2, "");
    TT_UT_EQUAL(tt_date_get_hour(&d), 0, "");
    TT_UT_EQUAL(tt_date_get_minute(&d), 59, "");
    TT_UT_EQUAL(tt_date_get_second(&d), 59, "");

    TT_UT_SUCCESS(tt_date_dec_hour(&d, 25), "");
    TT_UT_EQUAL(tt_date_get_year(&d), 1999, "");
    TT_UT_EQUAL(tt_date_get_month(&d), TT_DECEMBER, "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 31, "");
    TT_UT_EQUAL(tt_date_get_hour(&d), 23, "");
    TT_UT_EQUAL(tt_date_get_minute(&d), 59, "");
    TT_UT_EQUAL(tt_date_get_second(&d), 59, "");

    // min
    TT_UT_SUCCESS(tt_date_set(&d, 2000, TT_FEBRUARY, 29, 23, 59, 59), "");

    TT_UT_SUCCESS(tt_date_inc_minute(&d, 0), "");
    TT_UT_EQUAL(tt_date_get_year(&d), 2000, "");
    TT_UT_EQUAL(tt_date_get_month(&d), TT_FEBRUARY, "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 29, "");
    TT_UT_EQUAL(tt_date_get_hour(&d), 23, "");
    TT_UT_EQUAL(tt_date_get_minute(&d), 59, "");
    TT_UT_EQUAL(tt_date_get_second(&d), 59, "");

    TT_UT_SUCCESS(tt_date_inc_minute(&d, 1501), "");
    TT_UT_EQUAL(tt_date_get_year(&d), 2000, "");
    TT_UT_EQUAL(tt_date_get_month(&d), TT_MARCH, "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 2, "");
    TT_UT_EQUAL(tt_date_get_hour(&d), 1, "");
    TT_UT_EQUAL(tt_date_get_minute(&d), 0, "");
    TT_UT_EQUAL(tt_date_get_second(&d), 59, "");

    TT_UT_SUCCESS(tt_date_dec_minute(&d, 1501), "");
    TT_UT_EQUAL(tt_date_get_year(&d), 2000, "");
    TT_UT_EQUAL(tt_date_get_month(&d), TT_FEBRUARY, "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 29, "");
    TT_UT_EQUAL(tt_date_get_hour(&d), 23, "");
    TT_UT_EQUAL(tt_date_get_minute(&d), 59, "");
    TT_UT_EQUAL(tt_date_get_second(&d), 59, "");

    // second
    TT_UT_SUCCESS(tt_date_set(&d, 1999, TT_FEBRUARY, 28, 23, 59, 59), "");

    TT_UT_SUCCESS(tt_date_inc_second(&d, 0), "");
    TT_UT_EQUAL(tt_date_get_year(&d), 1999, "");
    TT_UT_EQUAL(tt_date_get_month(&d), TT_FEBRUARY, "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 28, "");
    TT_UT_EQUAL(tt_date_get_hour(&d), 23, "");
    TT_UT_EQUAL(tt_date_get_minute(&d), 59, "");
    TT_UT_EQUAL(tt_date_get_second(&d), 59, "");

    TT_UT_SUCCESS(tt_date_inc_second(&d, 90001), "");
    TT_UT_EQUAL(tt_date_get_year(&d), 1999, "");
    TT_UT_EQUAL(tt_date_get_month(&d), TT_MARCH, "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 2, "");
    TT_UT_EQUAL(tt_date_get_hour(&d), 1, "");
    TT_UT_EQUAL(tt_date_get_minute(&d), 0, "");
    TT_UT_EQUAL(tt_date_get_second(&d), 0, "");

    TT_UT_SUCCESS(tt_date_dec_second(&d, 90001), "");
    TT_UT_EQUAL(tt_date_get_year(&d), 1999, "");
    TT_UT_EQUAL(tt_date_get_month(&d), TT_FEBRUARY, "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 28, "");
    TT_UT_EQUAL(tt_date_get_hour(&d), 23, "");
    TT_UT_EQUAL(tt_date_get_minute(&d), 59, "");
    TT_UT_EQUAL(tt_date_get_second(&d), 59, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_date_julian)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_date_t d;
    tt_double_t jd;
    tt_u32_t w;

    TT_TEST_CASE_ENTER()
    // test start

    tt_date_init(&d, TT_UTC_00_00);

    // test get week
    TT_UT_SUCCESS(tt_date_set_date(&d, 2017, TT_JANUARY, 3), "");
    TT_UT_EQUAL(tt_date_get_weekday(&d), TT_TUESDAY, "");
    TT_UT_EQUAL(tt_date_get_week(&d, TT_MONDAY), 1, "");
    TT_UT_EQUAL(tt_date_get_week(&d, TT_FRIDAY), 0, "");
    TT_UT_EQUAL(tt_date_get_week(&d, TT_TUESDAY), 1, "");

    TT_UT_SUCCESS(tt_date_set_date(&d, 2017, TT_DECEMBER, 30), "");
    TT_UT_EQUAL(tt_date_get_week(&d, TT_MONDAY), 52, "");

    TT_UT_SUCCESS(tt_date_set_date(&d, 2017, TT_DECEMBER, 18), "");
    TT_UT_EQUAL(tt_date_get_week(&d, TT_MONDAY), 51, "");

    TT_UT_SUCCESS(tt_date_set_date(&d, 2016, TT_FEBRUARY, 28), "");
    TT_UT_EQUAL(tt_date_get_week(&d, TT_MONDAY), 8, "");

    TT_UT_SUCCESS(tt_date_set_date(&d, 2016, TT_FEBRUARY, 29), "");
    TT_UT_EQUAL(tt_date_get_week(&d, TT_MONDAY), 9, "");
    // test get week end

    TT_UT_SUCCESS(tt_date_set_date(&d, 1840, TT_DECEMBER, 31), "");
    TT_UT_SUCCESS(tt_date_to_julian(&d, &jd), "");
    TT_UT_EXP((jd - 2393470.5) < 0.1, "");
    TT_UT_SUCCESS(tt_date_from_julian(&d, jd, TT_UTC_00_00), "");
    TT_UT_EQUAL(tt_date_get_year(&d), 1840, "");
    TT_UT_EQUAL(tt_date_get_month(&d), TT_DECEMBER, "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 31, "");
    TT_UT_EQUAL(tt_date_get_hour(&d), 0, "");

    TT_UT_SUCCESS(tt_date_set_date(&d, 1858, TT_NOVEMBER, 17), "");
    TT_UT_SUCCESS(tt_date_to_julian(&d, &jd), "");
    TT_UT_EXP((jd - 2400000.5) < 0.1, "");
    TT_UT_EQUAL(tt_date_get_year(&d), 1858, "");
    TT_UT_EQUAL(tt_date_get_month(&d), TT_NOVEMBER, "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 17, "");
    TT_UT_EQUAL(tt_date_get_hour(&d), 0, "");

    TT_UT_SUCCESS(tt_date_set_date(&d, 1899, TT_DECEMBER, 30), "");
    TT_UT_SUCCESS(tt_date_to_julian(&d, &jd), "");
    TT_UT_EXP((jd - 2415018.5) < 0.1, "");

    TT_UT_SUCCESS(tt_date_set_date(&d, 1901, TT_JANUARY, 1), "");
    TT_UT_SUCCESS(tt_date_to_julian(&d, &jd), "");
    TT_UT_EXP((jd - 2415385.5) < 0.1, "");

    TT_UT_SUCCESS(tt_date_set_date(&d, 1970, TT_JANUARY, 1), "");
    TT_UT_SUCCESS(tt_date_to_julian(&d, &jd), "");
    TT_UT_EXP((jd - 2440587.5) < 0.1, "");

    TT_UT_SUCCESS(tt_date_set_date(&d, 1980, TT_JANUARY, 1), "");
    TT_UT_SUCCESS(tt_date_to_julian(&d, &jd), "");
    TT_UT_EXP((jd - 2444239.5) < 0.1, "");
    TT_UT_SUCCESS(tt_date_from_julian(&d, jd, TT_UTC_00_00), "");
    TT_UT_EQUAL(tt_date_get_year(&d), 1980, "");
    TT_UT_EQUAL(tt_date_get_month(&d), TT_JANUARY, "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 1, "");
    TT_UT_EQUAL(tt_date_get_hour(&d), 0, "");

    TT_UT_SUCCESS(tt_date_from_julian(&d, 2455620.56318, TT_UTC_00_00), "");
    TT_UT_EQUAL(tt_date_get_year(&d), 2011, "");
    TT_UT_EQUAL(tt_date_get_month(&d), TT_FEBRUARY, "");
    TT_UT_EQUAL(tt_date_get_monthday(&d), 28, "");
    TT_UT_EQUAL(tt_date_get_hour(&d), 1, "");
    TT_UT_EQUAL(tt_date_get_minute(&d), 30, "");
    // TT_UT_EQUAL(tt_date_get_second(&d), 59, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_date_diff)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_date_t a, b;

    TT_TEST_CASE_ENTER()
    // test start

    tt_date_init(&a, tt_g_local_tmzone);
    tt_date_init(&b, tt_g_local_tmzone);

    // diff day
    tt_date_set_date(&a, 2016, TT_FEBRUARY, 28);
    tt_date_copy(&b, &a);
    TT_UT_EQUAL(tt_date_diff_day(&a, &b), 0, "");

    tt_date_set_date(&b, 2016, TT_FEBRUARY, 29);
    TT_UT_EQUAL(tt_date_diff_day(&a, &b), -1, "");
    TT_UT_EQUAL(tt_date_diff_day(&b, &a), 1, "");

    tt_date_set_date(&b, 2017, TT_FEBRUARY, 28);
    TT_UT_EQUAL(tt_date_diff_day(&a, &b), -366, "");
    TT_UT_EQUAL(tt_date_diff_day(&b, &a), 366, "");

    // diff time
    tt_date_set_time(&a, 12, 0, 0);
    tt_date_set_time(&b, 12, 0, 0);
    TT_UT_EQUAL(tt_date_diff_second(&a, &b), -366 * 86400, "");
    TT_UT_EQUAL(tt_date_diff_second(&b, &a), 366 * 86400, "");

    tt_date_set_time(&a, 12, 0, 1);
    TT_UT_EQUAL(tt_date_diff_second(&a, &b), -366 * 86400 + 1, "");
    TT_UT_EQUAL(tt_date_diff_second(&b, &a), 366 * 86400 - 1, "");

    tt_date_set_time(&a, 11, 59, 59);
    TT_UT_EQUAL(tt_date_diff_second(&a, &b), -366 * 86400 - 1, "");
    TT_UT_EQUAL(tt_date_diff_second(&b, &a), 366 * 86400 + 1, "");

    // test end
    TT_TEST_CASE_LEAVE()
}
