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
#include <unit_test/tt_unit_test.h>

#include <log/tt_log.h>
#include <os/tt_thread.h>
#include <timer/tt_time_reference.h>

// portlayer header files
#include <tt_cstd_api.h>

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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_time_ref_basic)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(time_ref_case)

TT_TEST_CASE("tt_unit_test_time_abs_basic",
             "testing basic time ref API",
             tt_unit_test_time_ref_basic,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE_LIST_DEFINE_END(time_ref_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(TEST_UNIT_TIME_REF, 0, time_ref_case)

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

#ifdef _WIN32
        void __bm()
{
    int i;
    DWORD s, e, t1, t2, t3, t4;
    int __t;
    tt_s64_t d = 0;
    LARGE_INTEGER li;

    i = 0;
    s = timeGetTime();
    while (i++ < 10000000) {
        __t = GetTickCount();
        ++__t;
    }
    e = timeGetTime();
    t1 = e - s;

    i = 0;
    s = timeGetTime();
    while (i++ < 10000000) {
        __t = timeGetTime();
        ++__t;
    }
    e = timeGetTime();
    t2 = e - s;

    i = 0;
    s = timeGetTime();
    while (i++ < 10000000) {
        QueryPerformanceCounter(&li);
        ++li.QuadPart;
    }
    e = timeGetTime();
    t3 = e - s;

    i = 0;
    s = timeGetTime();
    while (i++ < 10000000) {
        tt_time_ref();
        // d = tt_time_ref2ms(tt_time_ref(), NULL);
        //++d;
    }
    e = timeGetTime();
    t4 = e - s;

    TT_INFO("%d, %d, %d, %d", t1, t2, t3, t4);
    TT_INFO("%d, %d, %d", __t, d, li.QuadPart);
}
#endif

TT_TEST_ROUTINE_DEFINE(tt_unit_test_time_ref_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_s64_t t, last;
    tt_u32_t i;

    TT_TEST_CASE_ENTER()
// test start

#ifdef _WIN32
    __bm();
#endif

    t = tt_time_ref();
    last = t;

    i = 0;
    while (i++ < 10) {
        tt_s64_t d = 0;

        TT_INFO("sleep 50 ms", t);
        tt_sleep(50);
        t = tt_time_ref();
        TT_UT_EXP(t >= last, "");

        d = t - last;
        d = tt_time_ref2ms(d);
        last = t;

        // error is expected less than 5ms
        d -= 50; // slept 50ms
        TT_INFO("difference: %d ms", d);
        // TT_UT_EXP(d <= 5, "");
        TT_UT_EXP(d >= -5, "");
    }

    // test end
    TT_TEST_CASE_LEAVE()
}
