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
TT_TEST_ROUTINE_DECLARE(case_entropy)
TT_TEST_ROUTINE_DECLARE(case_ctr_drbg)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(crypto_rand_case)

TT_TEST_CASE("case_entropy",
             "crypto: entropy",
             case_entropy,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("case_ctr_drbg",
                 "crypto: ctr drbg",
                 case_ctr_drbg,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(crypto_rand_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(CRYPTO_UT_RAND, 0, crypto_rand_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(case_entropy)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(case_entropy)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_entropy_t *e;

    TT_TEST_CASE_ENTER()
    // test start

    e = tt_entropy_create();
    TT_UT_NOT_NULL(e, "");

    tt_entropy_destroy(e);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_ctr_drbg)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_entropy_t *e;
    tt_ctr_drbg_t *d;
    tt_result_t ret;
    tt_u8_t buf[2000];

    TT_TEST_CASE_ENTER()
    // test start

    e = tt_entropy_create();
    TT_UT_NOT_NULL(e, "");

    d = tt_ctr_drbg_create(e, (tt_u8_t *)&ret, 1);
    TT_UT_NOT_NULL(d, "");
    ret = tt_ctr_drbg_rand(d, buf, 100);
    TT_UT_SUCCESS(ret, "");
    ret = tt_ctr_drbg_rand(d, buf, 100);
    TT_UT_SUCCESS(ret, "");
    tt_ctr_drbg_destroy(d);

    d = tt_ctr_drbg_create(e, NULL, 0);
    TT_UT_NOT_NULL(d, "");
    ret = tt_ctr_drbg_rand(d, buf, 100);
    TT_UT_SUCCESS(ret, "");
    ret = tt_ctr_drbg_rand(d, buf, 2000);
    // TT_UT_SUCCESS(ret, "");
    tt_ctr_drbg_destroy(d);

    tt_entropy_destroy(e);

    d = tt_current_ctr_drbg();
    TT_UT_NOT_NULL(e, "");
    ret = tt_ctr_drbg_rand(d, buf, 100);
    TT_UT_SUCCESS(ret, "");

    // test end
    TT_TEST_CASE_LEAVE()
}
