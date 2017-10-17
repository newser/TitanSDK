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
#include <zip/tt_zip_source.h>
#include <zip/tt_zip_source_blob.h>

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
TT_TEST_ROUTINE_DECLARE(case_zsrc_blob)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(zip_zsrc_case)

TT_TEST_CASE("case_zsrc_blob",
             "zip source: blob",
             case_zsrc_blob,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE_LIST_DEFINE_END(zip_zsrc_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(ZIP_UT_ZSOURCE, 0, zip_zsrc_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(case_zsrc_blob)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(case_zsrc_blob)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_zipsrc_t *zs;
    tt_char_t buf[100] = "123", obuf[200];
    tt_result_t ret;
    tt_u32_t n;

    TT_TEST_CASE_ENTER()
    // test start

    zs = tt_zipsrc_blob_create(buf, sizeof(buf), TT_FALSE);
    TT_UT_NOT_NULL(zs, "");

    tt_zipsrc_ref(zs);
    tt_zipsrc_ref(zs);
    tt_zipsrc_release(zs);
    tt_zipsrc_release(zs);
    // still has 1 ref

    ret = tt_zipsrc_open(zs);
    TT_UT_SUCCESS(ret, "");

    ret = tt_zipsrc_read(zs, (tt_u8_t *)obuf, sizeof(obuf), &n);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(n, 100, "");
    TT_UT_EQUAL(tt_memcmp(buf, obuf, n), 0, "");
    ret = tt_zipsrc_read(zs, (tt_u8_t *)obuf, sizeof(obuf), &n);
    TT_UT_EQUAL(ret, TT_E_END, "");
    TT_UT_EQUAL(n, 0, "");

    tt_zipsrc_release(zs);

    // test end
    TT_TEST_CASE_LEAVE()
}
