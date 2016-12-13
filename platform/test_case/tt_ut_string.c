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

#include <algorithm/tt_string.h>
#include <algorithm/tt_string_common.h>
#include <tt_cstd_api.h>
#include <unit_test/tt_test_framework.h>
#include <unit_test/tt_unit_test.h>

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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_str_null)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_str_basic)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_str_getchar)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_str_cmp)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_str_cpswap)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_str_find)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_str_join)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_str_case)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_str_insert)

// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(string_case)

TT_TEST_CASE("tt_unit_test_str_null",
             "testing basic string api, null string",
             tt_unit_test_str_null,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_str_basic",
                 "testing basic string api",
                 tt_unit_test_str_basic,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_str_getchar",
                 "testing basic string getchar",
                 tt_unit_test_str_getchar,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_str_cmp",
                 "testing basic string compare",
                 tt_unit_test_str_cmp,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_str_copy_swap",
                 "testing basic string copy&swap",
                 tt_unit_test_str_cpswap,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_str_find",
                 "testing basic string find",
                 tt_unit_test_str_find,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_str_join",
                 "testing basic string join",
                 tt_unit_test_str_join,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_str_case",
                 "testing string case convert",
                 tt_unit_test_str_case,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_str_insert",
                 "testing string insert",
                 tt_unit_test_str_insert,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(string_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(TEST_UNIT_STRING, 0, string_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(tt_unit_test_str_insert)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    tt_u32_t __s_err_line;

tt_result_t __ut_str_checknull(tt_string_t *s)
{
    TT_STR_CHECK(s);

    if (tt_string_len(s) != 0) {
        __s_err_line = __LINE__;
        return TT_FAIL;
    }

    if (tt_strncmp(tt_string_cstr(s), "", 1) != 0 || tt_string_len(s) != 0) {
        __s_err_line = __LINE__;
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_str_null)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_string_t s;
    tt_result_t ret;

    TT_TEST_CASE_ENTER()
    // test start

    // init
    tt_string_init(&s, NULL);
    tt_string_print(&s, 0);

    ret = __ut_str_checknull(&s);
    TT_TEST_CHECK_SUCCESS(ret, "");

    tt_string_destroy(&s);

    // create
    ret = tt_string_create(&s, "", NULL);
    TT_TEST_CHECK_SUCCESS(ret, "");

    ret = __ut_str_checknull(&s);
    TT_TEST_CHECK_SUCCESS(ret, "");

    tt_string_destroy(&s);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_str_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_string_t s;
    tt_result_t ret;
    tt_u32_t len;
    const tt_char_t *p;
    tt_s32_t cmp_ret;

    TT_TEST_CASE_ENTER()
    // test start

    // create
    ret = tt_string_create(&s, "123", NULL);
    TT_TEST_CHECK_SUCCESS(ret, "");
    tt_string_print(&s, 0);

    TT_TEST_CHECK_EQUAL(tt_string_len(&s), 3, "");

    p = tt_string_cstr(&s);
    TT_TEST_CHECK_EQUAL(tt_strncmp(p, "123", 3), 0, "");

    p = tt_string_subcstr(&s, 0, &len);
    TT_TEST_CHECK_EQUAL(len, 3, "");
    TT_TEST_CHECK_EQUAL(tt_strncmp(p, "123", 3), 0, "");
    p = tt_string_subcstr(&s, 1, &len);
    TT_TEST_CHECK_EQUAL(len, 2, "");
    TT_TEST_CHECK_EQUAL(tt_strncmp(p, "23", 2), 0, "");
    p = tt_string_subcstr(&s, 3, &len);
    TT_TEST_CHECK_EQUAL(len, 0, "");
    TT_TEST_CHECK_EQUAL(tt_strncmp(p, "", 1), 0, "");
    p = tt_string_subcstr(&s, 4, &len);
    TT_TEST_CHECK_EQUAL(len, 0, "");
    TT_TEST_CHECK_EQUAL(p, NULL, "");

    tt_string_clear(&s);
    ret = __ut_str_checknull(&s);
    TT_TEST_CHECK_SUCCESS(ret, "");

    tt_string_destroy(&s);

    // create n
    ret = tt_string_create_n(&s, "", 0, NULL);
    TT_TEST_CHECK_SUCCESS(ret, "");
    cmp_ret = tt_string_cmp(&s, "");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    tt_string_destroy(&s);

    ret = tt_string_create_n(&s, "", 1, NULL);
    TT_TEST_CHECK_SUCCESS(ret, "");
    cmp_ret = tt_string_cmp(&s, "");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    tt_string_destroy(&s);

    ret = tt_string_create_n(&s, "", 10, NULL);
    TT_TEST_CHECK_SUCCESS(ret, "");
    cmp_ret = tt_string_cmp(&s, "");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    tt_string_destroy(&s);

    ret = tt_string_create_n(&s, "123", 2, NULL);
    TT_TEST_CHECK_SUCCESS(ret, "");
    cmp_ret = tt_string_cmp(&s, "12");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    tt_string_destroy(&s);

    ret = tt_string_create_n(&s, "123", 3, NULL);
    TT_TEST_CHECK_SUCCESS(ret, "");
    cmp_ret = tt_string_cmp(&s, "123");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    tt_string_destroy(&s);

    ret = tt_string_create_n(&s, "123", 30, NULL);
    TT_TEST_CHECK_SUCCESS(ret, "");
    cmp_ret = tt_string_cmp(&s, "123");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    tt_string_destroy(&s);

    // create nocpy
    ret = tt_string_create_nocopy(&s, "123", NULL);
    TT_TEST_CHECK_SUCCESS(ret, "");
    tt_string_print(&s, 0);

    TT_TEST_CHECK_EQUAL(tt_string_len(&s), 3, "");

    p = tt_string_cstr(&s);
    TT_TEST_CHECK_EQUAL(tt_strncmp(p, "123", 3), 0, "");

    tt_string_clear(&s);
    ret = __ut_str_checknull(&s);
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");

    tt_string_destroy(&s);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_str_getchar)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_string_t s;
    tt_result_t ret;
    tt_char_t c;

    TT_TEST_CASE_ENTER()
    // test start

    // getchar
    tt_string_init(&s, NULL);

    c = tt_string_getchar(&s, 0);
    TT_TEST_CHECK_EQUAL(c, 0, "");
    c = tt_string_getchar(&s, 1);
    TT_TEST_CHECK_EQUAL(c, 0, "");
    c = tt_string_getchar(&s, 2);
    TT_TEST_CHECK_EQUAL(c, 0, "");

    ret = tt_string_create_nocopy(&s, "0123456789abcedf", NULL);
    TT_TEST_CHECK_SUCCESS(ret, "");

    c = tt_string_getchar(&s, 0);
    TT_TEST_CHECK_EQUAL(c, '0', "");
    c = tt_string_getchar(&s, 10);
    TT_TEST_CHECK_EQUAL(c, 'a', "");
    c = tt_string_getchar(&s, 15);
    TT_TEST_CHECK_EQUAL(c, 'f', "");
    c = tt_string_getchar(&s, 16);
    TT_TEST_CHECK_EQUAL(c, 0, "");
    c = tt_string_getchar(&s, 17);
    TT_TEST_CHECK_EQUAL(c, 0, "");
    c = tt_string_getchar(&s, ~0);
    TT_TEST_CHECK_EQUAL(c, 0, "");

    tt_string_destroy(&s);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_str_cmp)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_string_t s, s2;
    tt_result_t ret;
    tt_s32_t cmp_ret;

    TT_TEST_CASE_ENTER()
    // test start

    tt_string_init(&s, NULL);
    tt_string_init(&s2, NULL);
    cmp_ret = tt_string_cmp(&s, tt_string_cstr(&s2));
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    tt_string_destroy(&s);

    ret = tt_string_create(&s, "", NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_cmp(&s, tt_string_cstr(&s2));
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    cmp_ret = tt_string_cmp(&s2, tt_string_cstr(&s2));
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    cmp_ret = tt_string_cmp(&s2, "");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    tt_string_destroy(&s);
    tt_string_destroy(&s2);

    // diff len
    ret = tt_string_create(&s, "1234567890aaaaafdsafasfdas", NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_string_create(&s2, "1234567890aaaaafdsafasfda", NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_cmp(&s, tt_string_cstr(&s2));
    TT_TEST_CHECK_EXP(cmp_ret > 0, "");
    cmp_ret = tt_string_cmp(&s2, tt_string_cstr(&s));
    TT_TEST_CHECK_EXP(cmp_ret < 0, "");
    cmp_ret = tt_string_cmp(&s, "1234567890aaaaafdsafasfdas");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    // same len
    tt_string_destroy(&s2);
    ret = tt_string_create(&s2, "1234567890aaaaafdsafasfdat", NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_cmp(&s, tt_string_cstr(&s2));
    TT_TEST_CHECK_EXP(cmp_ret < 0, "");
    cmp_ret = tt_string_cmp(&s2, tt_string_cstr(&s));
    TT_TEST_CHECK_EXP(cmp_ret > 0, "");
    cmp_ret = tt_string_cmp(&s, "1234567890aaaaafdsafasfdas");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    // string cmp empty
    tt_string_destroy(&s2);
    tt_string_init(&s2, NULL);
    cmp_ret = tt_string_cmp(&s, tt_string_cstr(&s2));
    TT_TEST_CHECK_EXP(cmp_ret > 0, "");
    cmp_ret = tt_string_cmp(&s2, tt_string_cstr(&s));
    TT_TEST_CHECK_EXP(cmp_ret < 0, "");

    tt_string_destroy(&s);
    tt_string_destroy(&s2);

    // ncasecmp
    tt_string_init(&s, NULL);
    tt_string_init(&s2, NULL);
    cmp_ret = tt_string_ncasecmp(&s, tt_string_cstr(&s2));
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    tt_string_destroy(&s);

    ret = tt_string_create(&s, "", NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_ncasecmp(&s, tt_string_cstr(&s2));
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    cmp_ret = tt_string_ncasecmp(&s2, tt_string_cstr(&s2));
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    cmp_ret = tt_string_ncasecmp(&s, "");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    tt_string_destroy(&s);
    tt_string_destroy(&s2);

    // diff len
    ret = tt_string_create(&s, "1234567890aAaAaFdSaFaszZ0fdas", NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_string_create(&s2, "1234567890aAaAaFdSaFaszZ0fda", NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_ncasecmp(&s, tt_string_cstr(&s2));
    TT_TEST_CHECK_EXP(cmp_ret > 0, "");
    cmp_ret = tt_string_ncasecmp(&s2, tt_string_cstr(&s));
    TT_TEST_CHECK_EXP(cmp_ret < 0, "");
    cmp_ret = tt_string_ncasecmp(&s, "1234567890aAaAaFdSaFaszZ0fdas");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    // same len
    tt_string_destroy(&s2);
    ret = tt_string_create(&s2, "1234567890aAaAaFdSaFaszZ0fdat", NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_ncasecmp(&s, tt_string_cstr(&s2));
    TT_TEST_CHECK_EXP(cmp_ret < 0, "");
    cmp_ret = tt_string_ncasecmp(&s2, tt_string_cstr(&s));
    TT_TEST_CHECK_EXP(cmp_ret > 0, "");

    // same len, diff case
    tt_string_destroy(&s2);
    ret = tt_string_create(&s2, "1234567890AAaaaFdSaFasZz0fdaS", NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_ncasecmp(&s, tt_string_cstr(&s2));
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    cmp_ret = tt_string_ncasecmp(&s, "1234567890AaaaaFdSaFasZZ0fdaS");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    // string cmp empty
    tt_string_destroy(&s2);
    tt_string_init(&s2, NULL);
    cmp_ret = tt_string_ncasecmp(&s, tt_string_cstr(&s2));
    TT_TEST_CHECK_EXP(cmp_ret > 0, "");
    cmp_ret = tt_string_ncasecmp(&s2, tt_string_cstr(&s));
    TT_TEST_CHECK_EXP(cmp_ret < 0, "");

    tt_string_destroy(&s);
    tt_string_destroy(&s2);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_str_cpswap)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_string_t s, s2;
    tt_result_t ret;
    tt_s32_t cmp_ret;

    TT_TEST_CASE_ENTER()
    // test start

    // empty
    tt_string_init(&s, NULL);
    tt_string_init(&s2, NULL);
    ret = tt_string_copy(&s, &s2);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_cmp(&s, "");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    tt_string_swap(&s, &s2);
    cmp_ret = tt_string_cmp(&s, "");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    cmp_ret = tt_string_cmp(&s2, "");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    // test come remove api
    tt_string_remove(&s, 0, 1);
    cmp_ret = tt_string_cmp(&s, "");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    tt_string_remove(&s, 0, 0);
    cmp_ret = tt_string_cmp(&s, "");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    tt_string_remove(&s, 100, 1);
    cmp_ret = tt_string_cmp(&s, "");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    tt_string_remove(&s, 100, 0);
    cmp_ret = tt_string_cmp(&s, "");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    // empty <--> string
    tt_string_destroy(&s2);
    ret = tt_string_create(&s2, "1234567890abcedf", NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_string_copy(&s, &s2);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_cmp(&s, "1234567890abcedf");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    cmp_ret = tt_string_cmp(&s, tt_string_cstr(&s2));
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    tt_string_clear(&s2);
    // now s: "123...", s2: ""
    tt_string_swap(&s, &s2);
    // now s2: "123...", s: ""
    cmp_ret = tt_string_cmp(&s, "");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    cmp_ret = tt_string_cmp(&s2, "1234567890abcedf");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    // string <--> string
    tt_string_destroy(&s);
    tt_string_init(&s, NULL);
    ret = tt_string_set(&s, "!");
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    tt_string_swap(&s, &s2);
    // now s: "123...", s2: "!"
    cmp_ret = tt_string_cmp(&s, "1234567890abcedf");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    cmp_ret = tt_string_cmp(&s2, "!");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    // test some removing op
    tt_string_remove(&s, 0, 1);
    cmp_ret = tt_string_cmp(&s, "234567890abcedf");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    tt_string_remove(&s, tt_string_len(&s), 1);
    cmp_ret = tt_string_cmp(&s, "234567890abcedf");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    tt_string_remove(&s, 9, 3);
    cmp_ret = tt_string_cmp(&s, "234567890edf");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    tt_string_remove(&s, 9, 100);
    cmp_ret = tt_string_cmp(&s, "234567890");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    tt_string_remove(&s, 0, 100);
    cmp_ret = tt_string_cmp(&s, "");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    tt_string_destroy(&s);
    tt_string_destroy(&s2);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_str_find)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_string_t s;
    tt_s32_t pos;

    TT_TEST_CASE_ENTER()
    // test start

    // empty
    tt_string_init(&s, NULL);
    pos = tt_string_find(&s, "");
    TT_TEST_CHECK_EQUAL(pos, 0, "");
    pos = tt_string_find(&s, "1");
    TT_TEST_CHECK_EQUAL(pos, TT_STRPOS_NULL, "");

    pos = tt_string_findfrom(&s, 0, "");
    TT_TEST_CHECK_EQUAL(pos, 0, "");
    pos = tt_string_findfrom(&s, 1, "");
    TT_TEST_CHECK_EQUAL(pos, TT_STRPOS_NULL, "");
    pos = tt_string_findfrom(&s, 0, "1");
    TT_TEST_CHECK_EQUAL(pos, TT_STRPOS_NULL, "");

    pos = tt_string_find_c(&s, 0);
    TT_TEST_CHECK_EQUAL(pos, 0, "");
    pos = tt_string_find_c(&s, 'a');
    TT_TEST_CHECK_EQUAL(pos, TT_STRPOS_NULL, "");

    pos = tt_string_findfrom_c(&s, 0, 0);
    TT_TEST_CHECK_EQUAL(pos, 0, "");
    pos = tt_string_findfrom_c(&s, 1, 0);
    TT_TEST_CHECK_EQUAL(pos, TT_STRPOS_NULL, "");
    pos = tt_string_findfrom_c(&s, 0, 'x');
    TT_TEST_CHECK_EQUAL(pos, TT_STRPOS_NULL, "");
    pos = tt_string_findfrom_c(&s, 1, 'x');
    TT_TEST_CHECK_EQUAL(pos, TT_STRPOS_NULL, "");

    tt_string_destroy(&s);

    // string
    tt_string_init(&s, NULL);
    tt_string_set(&s, "0123456789abcdef0123456789abcdefg"); // 33 bytes

    pos = tt_string_find(&s, "");
    TT_TEST_CHECK_EQUAL(pos, 0, "");
    pos = tt_string_find(&s, "0123");
    TT_TEST_CHECK_EQUAL(pos, 0, "");
    pos = tt_string_find(&s, "abcdefg");
    TT_TEST_CHECK_EQUAL(pos, 26, "");
    pos = tt_string_find(&s, "ef01234");
    TT_TEST_CHECK_EQUAL(pos, 14, "");
    pos = tt_string_find(&s, "ef01234aa");
    TT_TEST_CHECK_EQUAL(pos, TT_STRPOS_NULL, "");

    pos = tt_string_findfrom(&s, 0, "");
    TT_TEST_CHECK_EQUAL(pos, 0, "");
    pos = tt_string_findfrom(&s, 5, "");
    TT_TEST_CHECK_EQUAL(pos, 5, "");
    pos = tt_string_findfrom(&s, tt_string_len(&s), "");
    TT_TEST_CHECK_EQUAL(pos, tt_string_len(&s), "");

    pos = tt_string_findfrom(&s, 0, "0123");
    TT_TEST_CHECK_EQUAL(pos, 0, "");
    pos = tt_string_findfrom(&s, 1, "0123");
    TT_TEST_CHECK_EQUAL(pos, 16, "");
    pos = tt_string_findfrom(&s, 16, "0123");
    TT_TEST_CHECK_EQUAL(pos, 16, "");
    pos = tt_string_findfrom(&s, 17, "0123");
    TT_TEST_CHECK_EQUAL(pos, TT_STRPOS_NULL, "");
    pos = tt_string_findfrom(&s, tt_string_len(&s), "0123");
    TT_TEST_CHECK_EQUAL(pos, TT_STRPOS_NULL, "");
    pos = tt_string_findfrom(&s, tt_string_len(&s) + 10, "0123");
    TT_TEST_CHECK_EQUAL(pos, TT_STRPOS_NULL, "");

    pos = tt_string_findfrom(&s, 0, "def");
    TT_TEST_CHECK_EQUAL(pos, 13, "");
    pos = tt_string_findfrom(&s, 13, "def");
    TT_TEST_CHECK_EQUAL(pos, 13, "");
    pos = tt_string_findfrom(&s, 14, "def");
    TT_TEST_CHECK_EQUAL(pos, 29, "");
    pos = tt_string_findfrom(&s, 29, "def");
    TT_TEST_CHECK_EQUAL(pos, 29, "");
    pos = tt_string_findfrom(&s, 30, "def");
    TT_TEST_CHECK_EQUAL(pos, TT_STRPOS_NULL, "");
    pos = tt_string_findfrom(&s, tt_string_len(&s), "def");
    TT_TEST_CHECK_EQUAL(pos, TT_STRPOS_NULL, "");
    pos = tt_string_findfrom(&s, tt_string_len(&s) + 10, "def");
    TT_TEST_CHECK_EQUAL(pos, TT_STRPOS_NULL, "");

    pos = tt_string_find_c(&s, 0);
    TT_TEST_CHECK_EQUAL(pos, tt_string_len(&s), "");
    pos = tt_string_find_c(&s, '0');
    TT_TEST_CHECK_EQUAL(pos, 0, "");
    pos = tt_string_find_c(&s, 'g');
    TT_TEST_CHECK_EQUAL(pos, tt_string_len(&s) - 1, "");
    pos = tt_string_find_c(&s, 'f');
    TT_TEST_CHECK_EQUAL(pos, 15, "");

    pos = tt_string_findfrom_c(&s, 0, 0);
    TT_TEST_CHECK_EQUAL(pos, tt_string_len(&s), "");
    pos = tt_string_findfrom_c(&s, 0, '0');
    TT_TEST_CHECK_EQUAL(pos, 0, "");
    pos = tt_string_findfrom_c(&s, 0, 'g');
    TT_TEST_CHECK_EQUAL(pos, tt_string_len(&s) - 1, "");
    pos = tt_string_findfrom_c(&s, 0, 'f');
    TT_TEST_CHECK_EQUAL(pos, 15, "");

    pos = tt_string_findfrom_c(&s, 15, 0);
    TT_TEST_CHECK_EQUAL(pos, tt_string_len(&s), "");
    pos = tt_string_findfrom_c(&s, 15, '0');
    TT_TEST_CHECK_EQUAL(pos, 16, "");
    pos = tt_string_findfrom_c(&s, 15, 'g');
    TT_TEST_CHECK_EQUAL(pos, tt_string_len(&s) - 1, "");
    pos = tt_string_findfrom_c(&s, 16, 'f');
    TT_TEST_CHECK_EQUAL(pos, 31, "");

    tt_string_destroy(&s);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_str_join)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_string_t s, s2;
    tt_result_t ret;
    tt_s32_t cmp_ret;
    tt_bool_t br;

    TT_TEST_CASE_ENTER()
    // test start

    tt_string_init(&s, NULL);
    tt_string_init(&s2, NULL);
    ret = tt_string_append(&s, "");
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_cmp(&s, "");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    ret = tt_string_append_sub(&s, "", 0);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_cmp(&s, "");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    ret = tt_string_append_sub(&s, "", 10);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_cmp(&s, "");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    br = tt_string_startwith(&s, "");
    TT_TEST_CHECK_EQUAL(br, TT_TRUE, "");
    br = tt_string_startwith(&s, "aaaaaaaaaaaaa");
    TT_TEST_CHECK_EQUAL(br, TT_FALSE, "");

    br = tt_string_endwith(&s, "");
    TT_TEST_CHECK_EQUAL(br, TT_TRUE, "");
    br = tt_string_endwith(&s, "aaaaaaaaaaaaa");
    TT_TEST_CHECK_EQUAL(br, TT_FALSE, "");

    ret = tt_string_set(&s, "123");
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_string_append(&s, "");
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_cmp(&s, "123");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    ret = tt_string_append_sub(&s, "", 0);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_cmp(&s, "123");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    ret = tt_string_append_sub(&s, "", 10);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_cmp(&s, "123");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    ret = tt_string_append(&s, "4567890abcdefxxxxxxxxx");
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_cmp(&s, "1234567890abcdefxxxxxxxxx");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    br = tt_string_startwith(&s, "");
    TT_TEST_CHECK_EQUAL(br, TT_TRUE, "");
    br = tt_string_startwith(&s, "1");
    TT_TEST_CHECK_EQUAL(br, TT_TRUE, "");
    br = tt_string_startwith(&s, "123");
    TT_TEST_CHECK_EQUAL(br, TT_TRUE, "");
    br = tt_string_startwith(&s, "1234567890abcdefxxxxxxxxx");
    TT_TEST_CHECK_EQUAL(br, TT_TRUE, "");
    br = tt_string_startwith(&s, "1234567890abcdefxxxxxxxxxy");
    TT_TEST_CHECK_EQUAL(br, TT_FALSE, "");

    br = tt_string_endwith(&s, "");
    TT_TEST_CHECK_EQUAL(br, TT_TRUE, "");
    br = tt_string_startwith(&s, "1");
    TT_TEST_CHECK_EQUAL(br, TT_TRUE, "");
    br = tt_string_startwith(&s, "123");
    TT_TEST_CHECK_EQUAL(br, TT_TRUE, "");
    br = tt_string_startwith(&s, "1234567890abcdefxxxxxxxxx");
    TT_TEST_CHECK_EQUAL(br, TT_TRUE, "");
    br = tt_string_startwith(&s, "1234567890abcdefxxxxxxxxxy");
    TT_TEST_CHECK_EQUAL(br, TT_FALSE, "");

    // substr from 0
    ret = tt_string_substr(&s, 0, 0, &s2);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_cmp(&s2, "");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    ret = tt_string_substr(&s, 0, 1, &s2);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_cmp(&s2, "1");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    ret = tt_string_substr(&s, 0, 16, &s2);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_cmp(&s2, "1234567890abcdef");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    ret = tt_string_substr(&s, 0, tt_string_len(&s), &s2);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_cmp(&s2, "1234567890abcdefxxxxxxxxx");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    ret = tt_string_substr(&s, 0, ~0, &s2);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_cmp(&s2, "1234567890abcdefxxxxxxxxx");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    // substr from middle
    ret = tt_string_substr(&s, 9, 0, &s2);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_cmp(&s2, "");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    ret = tt_string_substr(&s, 9, 1, &s2);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_cmp(&s2, "0");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    ret = tt_string_substr(&s, 9, 6, &s2);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_cmp(&s2, "0abcde");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    ret = tt_string_substr(&s, 9, tt_string_len(&s) - 9, &s2);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_cmp(&s2, "0abcdefxxxxxxxxx");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    ret = tt_string_substr(&s, 9, ~0, &s2);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_cmp(&s2, "0abcdefxxxxxxxxx");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    // join c
    ret = tt_string_set(&s, "");
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_cmp(&s, "");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    // join 0
    ret = tt_string_append_c(&s, 0);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_cmp(&s, "");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    // join char
    ret = tt_string_append_c(&s, '1');
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_cmp(&s, "1");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    // join char ag
    ret = tt_string_append_c(&s, '1');
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_cmp(&s, "11");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    ret = tt_string_append_sub(&s, "22", 0);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_cmp(&s, "11");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    ret = tt_string_append_sub(&s, "22", 1);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_cmp(&s, "112");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    ret = tt_string_append_sub(&s, "333", 3);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_cmp(&s, "112333");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    ret = tt_string_append_sub(&s, "4444", 333);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_cmp(&s, "1123334444");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    tt_string_destroy(&s);
    tt_string_destroy(&s2);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_str_case)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_string_t s;
    tt_result_t ret;
    tt_s32_t cmp_ret;

    TT_TEST_CASE_ENTER()
    // test start

    tt_string_init(&s, NULL);

    // empty string
    tt_string_tolower(&s);
    cmp_ret = tt_string_cmp(&s, "");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    tt_string_toupper(&s);
    cmp_ret = tt_string_cmp(&s, "");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    tt_string_trim(&s);
    cmp_ret = tt_string_cmp(&s, "");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    // string
    ret = tt_string_set(&s, "123aaazzzAAAZZZ     ");
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    tt_string_toupper(&s);
    cmp_ret = tt_string_cmp(&s, "123AAAZZZAAAZZZ     ");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    tt_string_tolower(&s);
    cmp_ret = tt_string_cmp(&s, "123aaazzzaaazzz     ");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    tt_string_trim(&s);
    cmp_ret = tt_string_cmp(&s, "123aaazzzaaazzz");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    // more spaces
    ret = tt_string_set(&s, "    123a  aazzz  AAA ZZZ     ");
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    tt_string_trim(&s);
    cmp_ret = tt_string_cmp(&s, "123a  aazzz  AAA ZZZ");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    tt_string_destroy(&s);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_str_insert)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_string_t s;
    tt_result_t ret;
    tt_s32_t cmp_ret;

    TT_TEST_CASE_ENTER()
    // test start

    tt_string_init(&s, NULL);

    ret = tt_string_insert(&s, 0, "");
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_cmp(&s, "");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    ret = tt_string_insert(&s, TT_STRPOS_NULL, "");
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_cmp(&s, "");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    ret = tt_string_insert(&s, 0, "abc");
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_cmp(&s, "abc");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    ret = tt_string_insert(&s, TT_STRPOS_NULL, "123");
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_cmp(&s, "abc123");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    ret = tt_string_insert(&s, 2, "zzz");
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_cmp(&s, "abzzzc123");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    ret = tt_string_insert(&s, 2, "");
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_cmp(&s, "abzzzc123");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    ret = tt_string_insert(&s, 9, "xyz");
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_cmp(&s, "abzzzc123xyz");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    tt_string_destroy(&s);

    /////////////////////////////////////////////
    // insert c

    tt_string_init(&s, NULL);

    ret = tt_string_insert_c(&s, 0, 0);
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");
    cmp_ret = tt_string_cmp(&s, "");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    ret = tt_string_insert_c(&s, TT_STRPOS_NULL, 0);
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");
    cmp_ret = tt_string_cmp(&s, "");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    ret = tt_string_insert_c(&s, 0, 'a');
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_cmp(&s, "a");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    ret = tt_string_insert_c(&s, TT_STRPOS_NULL, 'b');
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_cmp(&s, "ab");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    ret = tt_string_insert_c(&s, 2, 'c');
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_cmp(&s, "abc");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    ret = tt_string_insert_c(&s, 2, 'z');
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_string_cmp(&s, "abzc");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    ret = tt_string_insert_c(&s, 2, 0);
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");
    cmp_ret = tt_string_cmp(&s, "abzc");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    tt_string_destroy(&s);

    // test end
    TT_TEST_CASE_LEAVE()
}
