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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_date_def)

TT_TEST_ROUTINE_DECLARE(tt_unit_test_date_render)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(date_case)

TT_TEST_CASE("tt_unit_test_date_def",
             "testing date definitions",
             tt_unit_test_date_def,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL),

TT_TEST_CASE("tt_unit_test_date_render",
             "testing date rendering",
             tt_unit_test_date_render,
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
    TT_TEST_ROUTINE_DEFINE(tt_unit_test_date_render)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(tt_unit_test_date_def)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

    TT_TEST_CASE_ENTER()
    // test start

    TT_UT_EQUAL(tt_strcmp(tt_tmzone_name[TT_UTC_MINUS_12_00], "UTC-12:00"),
                0,
                "");
    TT_UT_EQUAL(tt_strcmp(tt_tmzone_name[TT_UTC_14_00], "UTC+14:00"), 0, "");

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

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_date_render)
{
    //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_date_t d;
    tt_char_t buf[100];
    tt_result_t ret;
    
    TT_TEST_CASE_ENTER()
    // test start
    
    tt_date_init(&d, TT_UTC_08_00);
    
    tt_date_now(&d);
    ret = tt_date_render(&d, "%Y-%m-%d %H-%M-%S", buf, sizeof(buf));
    TT_UT_SUCCESS(ret, "");
    TT_INFO("now: %s", buf);

    ret = tt_date_render(&d, "%Y-%m-%d %H-%M-%S", buf, 0);
    TT_UT_FAIL(ret, "");

    tt_date_init(&d, TT_UTC_08_00);
    tt_date_set_year(&d, 2020);
    tt_date_set_month(&d, TT_NOVEMBER);
    tt_date_set_mday(&d, 31); // it can ...
    tt_date_set_hour(&d, 23);
    tt_date_set_minute(&d, 59);
    tt_date_set_second(&d, 60);
    ret = tt_date_render(&d, "%Y-%m-%d %H-%M-%S", buf, sizeof(buf));
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_strcmp(buf, "2020-11-31 23-59-60"), 0, "");

    ret = tt_date_render(&d, "%Y-%m-%d %H-%M-%S", buf, sizeof("2020-11-31 23-59-60") - 1);
    TT_UT_FAIL(ret, "");
    ret = tt_date_render(&d, "%Y-%m-%d %H-%M-%S", buf, sizeof("2020-11-31 23-59-60"));
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_strcmp(buf, "2020-11-31 23-59-60"), 0, "");
    
    // test end
    TT_TEST_CASE_LEAVE()
}
