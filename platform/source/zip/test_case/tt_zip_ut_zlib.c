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
TT_TEST_ROUTINE_DECLARE(case_zip_def)
TT_TEST_ROUTINE_DECLARE(case_zip_def_null)
TT_TEST_ROUTINE_DECLARE(case_zip_gzipdef)
TT_TEST_ROUTINE_DECLARE(case_zip_gzipdef_null)
TT_TEST_ROUTINE_DECLARE(case_zip_zlibdef)
TT_TEST_ROUTINE_DECLARE(case_zip_zlibdef_null)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(zip_zlib_case)

TT_TEST_CASE("case_zip_def", "zip: deflate", case_zip_def, NULL, NULL, NULL,
             NULL, NULL)
,

    TT_TEST_CASE("case_zip_def_null", "zip: deflate, null input/output",
                 case_zip_def_null, NULL, NULL, NULL, NULL, NULL),

    TT_TEST_CASE("case_zip_gzipdef", "zip: gzip deflate", case_zip_gzipdef,
                 NULL, NULL, NULL, NULL, NULL),

    TT_TEST_CASE("case_zip_gzipdef_null",
                 "zip: gzip deflate, null input/output", case_zip_gzipdef_null,
                 NULL, NULL, NULL, NULL, NULL),

    TT_TEST_CASE("case_zip_zlibdef", "zip: zlib deflate", case_zip_zlibdef,
                 NULL, NULL, NULL, NULL, NULL),

    TT_TEST_CASE("case_zip_zlibdef_null",
                 "zip: zlib deflate, null input/output", case_zip_zlibdef_null,
                 NULL, NULL, NULL, NULL, NULL),

    TT_TEST_CASE_LIST_DEFINE_END(zip_zlib_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(ZIP_UT_ZLIB, 0, zip_zlib_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(case_zip_def_null)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(case_zip_def)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_deflate_t dfl;
    tt_inflate_t ifl;
    tt_result_t ret;
    tt_u8_t ibuf[100], ibuf2[sizeof(ibuf)], obuf[700];
    tt_u32_t i_n, o_n, olen, i, j;

    TT_TEST_CASE_ENTER()
    // test start

    // do nothing
    ret = tt_deflate_create(&dfl, NULL);
    TT_UT_SUCCESS(ret, "");
    tt_deflate_reset(&dfl);
    tt_deflate_destroy(&dfl);

    ret = tt_inflate_create(&ifl, NULL);
    TT_UT_SUCCESS(ret, "");
    tt_inflate_reset(&ifl);
    tt_inflate_destroy(&ifl);

    // 0 buf
    ret = tt_deflate_create(&dfl, NULL);
    TT_UT_SUCCESS(ret, "");

    ret = tt_deflate_run(&dfl, ibuf, 0, &i_n, obuf, 0, &o_n, TT_FALSE);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(i_n, 0, "");
    TT_UT_EQUAL(o_n, 0, "");

    ret = tt_inflate_create(&ifl, NULL);
    TT_UT_SUCCESS(ret, "");

    ret = tt_inflate_run(&ifl, ibuf, 0, &i_n, obuf, 0, &o_n, TT_FALSE);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(i_n, 0, "");
    TT_UT_EQUAL(o_n, 0, "");

    tt_deflate_reset(&dfl);
    ret = tt_deflate_run(&dfl, ibuf, 0, &i_n, obuf, 0, &o_n, TT_TRUE);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(i_n, 0, "");
    TT_UT_EQUAL(o_n, 0, "");

    tt_deflate_reset(&dfl);
    ret =
        tt_deflate_run(&dfl, ibuf, 0, &i_n, obuf, sizeof(obuf), &o_n, TT_FALSE);
    // TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(i_n, 0, "");
    TT_INFO("o_n: %d", o_n);

    tt_inflate_reset(&ifl);
    i_n = 123;
    ret =
        tt_inflate_run(&ifl, ibuf, 0, &i_n, obuf, sizeof(obuf), &o_n, TT_FALSE);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(i_n, 0, "");

    tt_deflate_reset(&dfl);
    ret =
        tt_deflate_run(&dfl, ibuf, 0, &i_n, obuf, sizeof(obuf), &o_n, TT_TRUE);
    TT_UT_EQUAL(ret, TT_E_END, "");
    TT_UT_EQUAL(i_n, 0, "");
    olen = o_n;

    tt_inflate_reset(&ifl);
    ret = tt_inflate_run(&ifl, obuf, olen, &o_n, ibuf, sizeof(ibuf), &i_n,
                         TT_TRUE);
    TT_UT_EQUAL(ret, TT_E_END, "");
    TT_UT_EQUAL(i_n, 0, "");
    TT_UT_EQUAL(o_n, olen, "");

    // has content;
    tt_deflate_reset(&dfl);
    tt_inflate_reset(&ifl);

    for (i = 0; i < sizeof(ibuf); ++i) { ibuf[i] = (tt_u8_t)tt_rand_u32(); }
    i = 0;
    j = 0;
    while (i < sizeof(ibuf)) {
        tt_u32_t n = tt_rand_u32() % 10 + 1;
        tt_bool_t finish = TT_FALSE;
        if (i + n >= sizeof(ibuf)) {
            n = sizeof(ibuf) - i;
            finish = TT_TRUE;
        }

        ret = tt_deflate_run(&dfl, ibuf + i, n, &i_n, obuf + j,
                             sizeof(obuf) - j, &o_n, finish);
        i += i_n;
        j += o_n;
        if (ret == TT_E_END) { break; }
        TT_UT_SUCCESS(ret, "");
    }
    TT_ASSERT(i == sizeof(ibuf));
    olen = j;

    i = 0;
    j = 0;
    while (i < olen) {
        tt_u32_t n = tt_rand_u32() % 10 + 1;
        tt_bool_t finish = TT_FALSE;
        if (i + n >= olen) {
            n = olen - i;
            finish = TT_TRUE;
        }

        ret = tt_inflate_run(&ifl, obuf + i, n, &i_n, ibuf2 + j,
                             sizeof(ibuf2) - j, &o_n, finish);
        i += i_n;
        j += o_n;
        if (ret == TT_E_END) { break; }
        TT_UT_SUCCESS(ret, "");
    }
    TT_ASSERT(i == olen);

    TT_UT_EQUAL(j, sizeof(ibuf2), "");
    TT_UT_EQUAL(tt_memcmp(ibuf, ibuf2, sizeof(ibuf)), 0, "");

    tt_deflate_destroy(&dfl);
    tt_inflate_destroy(&ifl);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_zip_def_null)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_deflate_t dfl;
    tt_inflate_t ifl;
    tt_result_t ret;
    tt_u8_t ibuf[100], ibuf2[sizeof(ibuf)], obuf[700];
    tt_u32_t i_n, o_n, olen, i, j;
    tt_deflate_attr_t da;
    tt_inflate_attr_t ia;

    TT_TEST_CASE_ENTER()
    // test start

    tt_deflate_attr_default(&da);
    da.level = 9;
    da.window_bits = 9;
    da.mem_level = 9;
    ret = tt_deflate_create(&dfl, &da);

    tt_inflate_attr_default(&ia);
    ia.window_bits = 15;
    ret = tt_inflate_create(&ifl, &ia);
    TT_UT_SUCCESS(ret, "");

    // deflate
    for (i = 0; i < sizeof(ibuf); ++i) { ibuf[i] = (tt_u8_t)tt_rand_u32(); }

    ret =
        tt_deflate_run(&dfl, ibuf, sizeof(ibuf), &i_n, obuf, 10, &o_n, TT_TRUE);
    TT_UT_SUCCESS(ret, "");

    i = i_n;
    j = 10;
    while (j < sizeof(obuf)) {
        tt_u32_t n = tt_rand_u32() % 10 + 10;
        if (j + n >= sizeof(obuf)) { n = sizeof(obuf) - j; }

        ret = tt_deflate_run(&dfl, NULL, 0, &i_n, obuf + j, n, &o_n, TT_TRUE);
        i += i_n;
        j += o_n;
        if (ret == TT_E_END) { break; }
        TT_UT_SUCCESS(ret, "");
    }
    TT_ASSERT(i == sizeof(ibuf));
    olen = j;

    // inflate
    ret = tt_inflate_run(&ifl, obuf, olen, &i_n, ibuf2, 10, &o_n, TT_TRUE);
    TT_UT_SUCCESS(ret, "");

    i = i_n;
    j = 10;
    while (i < olen) {
        tt_u32_t n = tt_rand_u32() % 10 + 1;
        if (j + n >= sizeof(ibuf2)) { n = sizeof(ibuf2) - j; }

        ret = tt_inflate_run(&ifl, NULL, 0, &i_n, ibuf2 + j, n, &o_n, TT_TRUE);
        i += i_n;
        j += o_n;
        if (ret == TT_E_END) { break; }
        TT_UT_SUCCESS(ret, "");
    }
    TT_ASSERT(i == olen);

    TT_UT_EQUAL(j, sizeof(ibuf2), "");
    TT_UT_EQUAL(tt_memcmp(ibuf, ibuf2, sizeof(ibuf)), 0, "");

    tt_deflate_destroy(&dfl);
    tt_inflate_destroy(&ifl);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_zip_gzipdef)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_gzipdef_t dfl;
    tt_gzipinf_t ifl;
    tt_result_t ret;
    tt_u8_t ibuf[100], ibuf2[sizeof(ibuf)], obuf[700];
    tt_u32_t i_n, o_n, olen, i, j;

    TT_TEST_CASE_ENTER()
    // test start

    // do nothing
    ret = tt_gzipdef_create(&dfl, NULL);
    TT_UT_SUCCESS(ret, "");
    tt_gzipdef_reset(&dfl);
    tt_gzipdef_destroy(&dfl);

    ret = tt_gzipinf_create(&ifl, NULL);
    TT_UT_SUCCESS(ret, "");
    tt_gzipinf_reset(&ifl);
    tt_gzipinf_destroy(&ifl);

    // 0 buf
    ret = tt_gzipdef_create(&dfl, NULL);
    TT_UT_SUCCESS(ret, "");

    ret = tt_gzipdef_run(&dfl, ibuf, 0, &i_n, obuf, 0, &o_n, TT_FALSE);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(i_n, 0, "");
    TT_UT_EQUAL(o_n, 0, "");

    ret = tt_gzipinf_create(&ifl, NULL);
    TT_UT_SUCCESS(ret, "");

    ret = tt_gzipinf_run(&ifl, ibuf, 0, &i_n, obuf, 0, &o_n, TT_FALSE);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(i_n, 0, "");
    TT_UT_EQUAL(o_n, 0, "");

    tt_gzipdef_reset(&dfl);
    ret = tt_gzipdef_run(&dfl, ibuf, 0, &i_n, obuf, 0, &o_n, TT_TRUE);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(i_n, 0, "");
    TT_UT_EQUAL(o_n, 0, "");

    tt_gzipdef_reset(&dfl);
    ret =
        tt_gzipdef_run(&dfl, ibuf, 0, &i_n, obuf, sizeof(obuf), &o_n, TT_FALSE);
    // TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(i_n, 0, "");
    TT_INFO("o_n: %d", o_n);

    tt_gzipinf_reset(&ifl);
    i_n = 123;
    ret =
        tt_gzipinf_run(&ifl, ibuf, 0, &i_n, obuf, sizeof(obuf), &o_n, TT_FALSE);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(i_n, 0, "");

    tt_gzipdef_reset(&dfl);
    ret =
        tt_gzipdef_run(&dfl, ibuf, 0, &i_n, obuf, sizeof(obuf), &o_n, TT_TRUE);
    TT_UT_EQUAL(ret, TT_E_END, "");
    TT_UT_EQUAL(i_n, 0, "");
    olen = o_n;

    tt_gzipinf_reset(&ifl);
    ret = tt_gzipinf_run(&ifl, obuf, olen, &o_n, ibuf, sizeof(ibuf), &i_n,
                         TT_TRUE);
    TT_UT_EQUAL(ret, TT_E_END, "");
    TT_UT_EQUAL(i_n, 0, "");
    TT_UT_EQUAL(o_n, olen, "");

    // has content;
    tt_gzipdef_reset(&dfl);
    tt_gzipinf_reset(&ifl);

    for (i = 0; i < sizeof(ibuf); ++i) { ibuf[i] = (tt_u8_t)tt_rand_u32(); }
    i = 0;
    j = 0;
    while (i < sizeof(ibuf)) {
        tt_u32_t n = tt_rand_u32() % 10 + 1;
        tt_bool_t finish = TT_FALSE;
        if (i + n >= sizeof(ibuf)) {
            n = sizeof(ibuf) - i;
            finish = TT_TRUE;
        }

        ret = tt_gzipdef_run(&dfl, ibuf + i, n, &i_n, obuf + j,
                             sizeof(obuf) - j, &o_n, finish);
        i += i_n;
        j += o_n;
        if (ret == TT_E_END) { break; }
        TT_UT_SUCCESS(ret, "");
    }
    TT_ASSERT(i == sizeof(ibuf));
    olen = j;

    i = 0;
    j = 0;
    while (i < olen) {
        tt_u32_t n = tt_rand_u32() % 10 + 1;
        tt_bool_t finish = TT_FALSE;
        if (i + n >= olen) {
            n = olen - i;
            finish = TT_TRUE;
        }

        ret = tt_gzipinf_run(&ifl, obuf + i, n, &i_n, ibuf2 + j,
                             sizeof(ibuf2) - j, &o_n, finish);
        i += i_n;
        j += o_n;
        if (ret == TT_E_END) { break; }
        TT_UT_SUCCESS(ret, "");
    }
    TT_ASSERT(i == olen);

    TT_UT_EQUAL(j, sizeof(ibuf2), "");
    TT_UT_EQUAL(tt_memcmp(ibuf, ibuf2, sizeof(ibuf)), 0, "");

    tt_gzipdef_destroy(&dfl);
    tt_gzipinf_destroy(&ifl);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_zip_gzipdef_null)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_gzipdef_t dfl;
    tt_gzipinf_t ifl;
    tt_result_t ret;
    tt_u8_t ibuf[100], ibuf2[sizeof(ibuf)], obuf[700];
    tt_u32_t i_n, o_n, olen, i, j;
    tt_gzipdef_attr_t da;
    tt_gzipinf_attr_t ia;

    TT_TEST_CASE_ENTER()
    // test start

    tt_gzipdef_attr_default(&da);
    da.level = 9;
    da.window_bits = 9;
    da.mem_level = 9;
    ret = tt_gzipdef_create(&dfl, &da);

    tt_gzipinf_attr_default(&ia);
    ia.window_bits = 15;
    ret = tt_gzipinf_create(&ifl, &ia);
    TT_UT_SUCCESS(ret, "");

    // deflate
    for (i = 0; i < sizeof(ibuf); ++i) { ibuf[i] = (tt_u8_t)tt_rand_u32(); }

    ret =
        tt_gzipdef_run(&dfl, ibuf, sizeof(ibuf), &i_n, obuf, 10, &o_n, TT_TRUE);
    TT_UT_SUCCESS(ret, "");

    i = i_n;
    j = 10;
    while (j < sizeof(obuf)) {
        tt_u32_t n = tt_rand_u32() % 10 + 10;
        if (j + n >= sizeof(obuf)) { n = sizeof(obuf) - j; }

        ret = tt_gzipdef_run(&dfl, NULL, 0, &i_n, obuf + j, n, &o_n, TT_TRUE);
        i += i_n;
        j += o_n;
        if (ret == TT_E_END) { break; }
        TT_UT_SUCCESS(ret, "");
    }
    TT_ASSERT(i == sizeof(ibuf));
    olen = j;

    // inflate
    ret = tt_gzipinf_run(&ifl, obuf, olen, &i_n, ibuf2, 10, &o_n, TT_TRUE);
    TT_UT_SUCCESS(ret, "");

    i = i_n;
    j = 10;
    while (i < olen) {
        tt_u32_t n = tt_rand_u32() % 10 + 1;
        if (j + n >= sizeof(ibuf2)) { n = sizeof(ibuf2) - j; }

        ret = tt_gzipinf_run(&ifl, NULL, 0, &i_n, ibuf2 + j, n, &o_n, TT_TRUE);
        i += i_n;
        j += o_n;
        if (ret == TT_E_END) { break; }
        TT_UT_SUCCESS(ret, "");
    }
    TT_ASSERT(i == olen);

    TT_UT_EQUAL(j, sizeof(ibuf2), "");
    TT_UT_EQUAL(tt_memcmp(ibuf, ibuf2, sizeof(ibuf)), 0, "");

    tt_gzipdef_destroy(&dfl);
    tt_gzipinf_destroy(&ifl);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_zip_zlibdef)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_zlibdef_t dfl;
    tt_zlibinf_t ifl;
    tt_result_t ret;
    tt_u8_t ibuf[100], ibuf2[sizeof(ibuf)], obuf[700];
    tt_u32_t i_n, o_n, olen, i, j;

    TT_TEST_CASE_ENTER()
    // test start

    // do nothing
    ret = tt_zlibdef_create(&dfl, NULL);
    TT_UT_SUCCESS(ret, "");
    tt_zlibdef_reset(&dfl);
    tt_zlibdef_destroy(&dfl);

    ret = tt_zlibinf_create(&ifl, NULL);
    TT_UT_SUCCESS(ret, "");
    tt_zlibinf_reset(&ifl);
    tt_zlibinf_destroy(&ifl);

    // 0 buf
    ret = tt_zlibdef_create(&dfl, NULL);
    TT_UT_SUCCESS(ret, "");

    ret = tt_zlibdef_run(&dfl, ibuf, 0, &i_n, obuf, 0, &o_n, TT_FALSE);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(i_n, 0, "");
    TT_UT_EQUAL(o_n, 0, "");

    ret = tt_zlibinf_create(&ifl, NULL);
    TT_UT_SUCCESS(ret, "");

    ret = tt_zlibinf_run(&ifl, ibuf, 0, &i_n, obuf, 0, &o_n, TT_FALSE);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(i_n, 0, "");
    TT_UT_EQUAL(o_n, 0, "");

    tt_zlibdef_reset(&dfl);
    ret = tt_zlibdef_run(&dfl, ibuf, 0, &i_n, obuf, 0, &o_n, TT_TRUE);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(i_n, 0, "");
    TT_UT_EQUAL(o_n, 0, "");

    tt_zlibdef_reset(&dfl);
    ret =
        tt_zlibdef_run(&dfl, ibuf, 0, &i_n, obuf, sizeof(obuf), &o_n, TT_FALSE);
    // TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(i_n, 0, "");
    TT_INFO("o_n: %d", o_n);

    tt_zlibinf_reset(&ifl);
    i_n = 123;
    ret =
        tt_zlibinf_run(&ifl, ibuf, 0, &i_n, obuf, sizeof(obuf), &o_n, TT_FALSE);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(i_n, 0, "");

    tt_zlibdef_reset(&dfl);
    ret =
        tt_zlibdef_run(&dfl, ibuf, 0, &i_n, obuf, sizeof(obuf), &o_n, TT_TRUE);
    TT_UT_EQUAL(ret, TT_E_END, "");
    TT_UT_EQUAL(i_n, 0, "");
    olen = o_n;

    tt_zlibinf_reset(&ifl);
    ret = tt_zlibinf_run(&ifl, obuf, olen, &o_n, ibuf, sizeof(ibuf), &i_n,
                         TT_TRUE);
    TT_UT_EQUAL(ret, TT_E_END, "");
    TT_UT_EQUAL(i_n, 0, "");
    TT_UT_EQUAL(o_n, olen, "");

    // has content;
    tt_zlibdef_reset(&dfl);
    tt_zlibinf_reset(&ifl);

    for (i = 0; i < sizeof(ibuf); ++i) { ibuf[i] = (tt_u8_t)tt_rand_u32(); }
    i = 0;
    j = 0;
    while (i < sizeof(ibuf)) {
        tt_u32_t n = tt_rand_u32() % 10 + 1;
        tt_bool_t finish = TT_FALSE;
        if (i + n >= sizeof(ibuf)) {
            n = sizeof(ibuf) - i;
            finish = TT_TRUE;
        }

        ret = tt_zlibdef_run(&dfl, ibuf + i, n, &i_n, obuf + j,
                             sizeof(obuf) - j, &o_n, finish);
        i += i_n;
        j += o_n;
        if (ret == TT_E_END) { break; }
        TT_UT_SUCCESS(ret, "");
    }
    TT_ASSERT(i == sizeof(ibuf));
    olen = j;

    i = 0;
    j = 0;
    while (i < olen) {
        tt_u32_t n = tt_rand_u32() % 10 + 1;
        tt_bool_t finish = TT_FALSE;
        if (i + n >= olen) {
            n = olen - i;
            finish = TT_TRUE;
        }

        ret = tt_zlibinf_run(&ifl, obuf + i, n, &i_n, ibuf2 + j,
                             sizeof(ibuf2) - j, &o_n, finish);
        i += i_n;
        j += o_n;
        if (ret == TT_E_END) { break; }
        TT_UT_SUCCESS(ret, "");
    }
    TT_ASSERT(i == olen);

    TT_UT_EQUAL(j, sizeof(ibuf2), "");
    TT_UT_EQUAL(tt_memcmp(ibuf, ibuf2, sizeof(ibuf)), 0, "");

    tt_zlibdef_destroy(&dfl);
    tt_zlibinf_destroy(&ifl);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_zip_zlibdef_null)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_zlibdef_t dfl;
    tt_zlibinf_t ifl;
    tt_result_t ret;
    tt_u8_t ibuf[100], ibuf2[sizeof(ibuf)], obuf[700];
    tt_u32_t i_n, o_n, olen, i, j;
    tt_zlibdef_attr_t da;
    tt_zlibinf_attr_t ia;

    TT_TEST_CASE_ENTER()
    // test start

    tt_zlibdef_attr_default(&da);
    da.level = 9;
    da.window_bits = 9;
    da.mem_level = 9;
    ret = tt_zlibdef_create(&dfl, &da);

    tt_zlibinf_attr_default(&ia);
    ia.window_bits = 15;
    ret = tt_zlibinf_create(&ifl, &ia);
    TT_UT_SUCCESS(ret, "");

    // deflate
    for (i = 0; i < sizeof(ibuf); ++i) { ibuf[i] = (tt_u8_t)tt_rand_u32(); }

    ret =
        tt_zlibdef_run(&dfl, ibuf, sizeof(ibuf), &i_n, obuf, 10, &o_n, TT_TRUE);
    TT_UT_SUCCESS(ret, "");

    i = i_n;
    j = 10;
    while (j < sizeof(obuf)) {
        tt_u32_t n = tt_rand_u32() % 10 + 10;
        if (j + n >= sizeof(obuf)) { n = sizeof(obuf) - j; }

        ret = tt_zlibdef_run(&dfl, NULL, 0, &i_n, obuf + j, n, &o_n, TT_TRUE);
        i += i_n;
        j += o_n;
        if (ret == TT_E_END) { break; }
        TT_UT_SUCCESS(ret, "");
    }
    TT_ASSERT(i == sizeof(ibuf));
    olen = j;

    // inflate
    ret = tt_zlibinf_run(&ifl, obuf, olen, &i_n, ibuf2, 10, &o_n, TT_TRUE);
    TT_UT_SUCCESS(ret, "");

    i = i_n;
    j = 10;
    while (i < olen) {
        tt_u32_t n = tt_rand_u32() % 10 + 1;
        if (j + n >= sizeof(ibuf2)) { n = sizeof(ibuf2) - j; }

        ret = tt_zlibinf_run(&ifl, NULL, 0, &i_n, ibuf2 + j, n, &o_n, TT_TRUE);
        i += i_n;
        j += o_n;
        if (ret == TT_E_END) { break; }
        TT_UT_SUCCESS(ret, "");
    }
    TT_ASSERT(i == olen);

    TT_UT_EQUAL(j, sizeof(ibuf2), "");
    TT_UT_EQUAL(tt_memcmp(ibuf, ibuf2, sizeof(ibuf)), 0, "");

    tt_zlibdef_destroy(&dfl);
    tt_zlibinf_destroy(&ifl);

    // test end
    TT_TEST_CASE_LEAVE()
}
