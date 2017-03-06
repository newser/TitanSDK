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
#include <os/tt_fiber.h>
#include <os/tt_semaphore.h>
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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_fiber_basic)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_fiber_sanity)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(fiber_case)

TT_TEST_CASE("tt_unit_test_fiber_basic",
             "testing basic fiber API",
             tt_unit_test_fiber_basic,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_fiber_sanity",
                 "testing fiber sanity",
                 tt_unit_test_fiber_sanity,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(fiber_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(TEST_UNIT_FIBER, 0, fiber_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(tt_unit_test_fiber_sanity)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    static tt_result_t __ut_ret;
static tt_u32_t __err_line;
static tt_u32_t __ut_num;

static tt_result_t __fiber_1(IN void *param)
{
    if (param != (void *)1) {
        return TT_FAIL;
    }

    while (__ut_num < 100) {
        ++__ut_num;
        tt_fiber_yield();
    }

    return TT_SUCCESS;
}

static tt_result_t __test_fiber_1(IN void *param)
{
    tt_fiber_t *f1;
    tt_u32_t last_num;

    f1 = tt_fiber_create(__fiber_1, (void *)1, NULL);
    if (f1 == NULL) {
        __ut_ret = TT_FAIL;
        __err_line = __LINE__;
        return TT_FAIL;
    }

    __ut_num = 0;
    last_num = __ut_num;

    while (__ut_num < 100) {
        tt_fiber_resume(f1);

        if (__ut_num != (last_num + 1)) {
            __ut_ret = TT_FAIL;
            __err_line = __LINE__;
            return TT_FAIL;
        }
        last_num = __ut_num;
    }
    __ut_ret = TT_SUCCESS;

    return TT_SUCCESS;
}

static tt_result_t __fiber_1_1(IN void *param)
{
    if (param != (void *)2) {
        return TT_FAIL;
    }

    while (__ut_num < 100) {
        ++__ut_num;
        tt_fiber_yield();
    }

    ++__ut_num;
    return TT_SUCCESS;
}

static tt_result_t __test_fiber_1_1(IN void *param)
{
    tt_fiber_t *f1;
    tt_u32_t last_num;

    f1 = tt_fiber_create(__fiber_1_1, (void *)2, NULL);
    if (f1 == NULL) {
        __ut_ret = TT_FAIL;
        __err_line = __LINE__;
        return TT_FAIL;
    }

    __ut_num = 0;
    last_num = __ut_num;

    while (__ut_num < 101) {
        tt_fiber_resume(f1);

        if (__ut_num != (last_num + 1)) {
            __ut_ret = TT_FAIL;
            __err_line = __LINE__;
            return TT_FAIL;
        }
        last_num = __ut_num;
    }
    __ut_ret = TT_SUCCESS;

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_fiber_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_thread_t *t;
    tt_thread_attr_t tattr;

    TT_TEST_CASE_ENTER()
    // test start

    tt_thread_attr_default(&tattr);
    tattr.enable_fiber = TT_TRUE;

    __ut_ret = TT_FAIL;

    __ut_num = 0;
    t = tt_thread_create(__test_fiber_1, NULL, &tattr);
    tt_thread_wait(t);

    TT_TEST_CHECK_EQUAL(__ut_ret, TT_SUCCESS, "");

    __ut_num = 0;
    t = tt_thread_create(__test_fiber_1_1, NULL, &tattr);
    tt_thread_wait(t);

    TT_TEST_CHECK_EQUAL(__ut_ret, TT_SUCCESS, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

#define FIBER_NUM 100
static tt_fiber_t *__fb_ar[FIBER_NUM];

static tt_u32_t __ques[FIBER_NUM], __ans[FIBER_NUM], __waiting[FIBER_NUM];

static tt_result_t __fiber_2(IN void *param)
{
    tt_u32_t idx = (tt_u32_t)(tt_uintptr_t)param;

    while (1) {
        // tt_sleep(tt_rand_u32() % 5);

        // raise a question
        __ques[idx] = tt_rand_u32();
        __waiting[idx] = 1;
        tt_fiber_yield();

        // check answer
        if (__ques[idx] != __ans[idx] - 1) {
            __err_line = __LINE__;
            return TT_FAIL;
        }
    }

    return TT_SUCCESS;
}

static tt_result_t __test_fiber_2(IN void *param)
{
    tt_u32_t i, num = 0;

    for (i = 0; i < FIBER_NUM; ++i) {
        __fb_ar[i] = tt_fiber_create(__fiber_2, (void *)(tt_uintptr_t)i, NULL);
        if (__fb_ar[i] == NULL) {
            __err_line = __LINE__;
            return TT_FAIL;
        }
        tt_fiber_resume(__fb_ar[i]);
    }

    __ques[0] = 1314;
    __waiting[0] = 1;
    while (num++ < FIBER_NUM * 100) {
        tt_u32_t idx = tt_rand_u32() % FIBER_NUM;

        if (__waiting[idx]) {
            // answer its question
            // tt_sleep(tt_rand_u32()%10);
            __ans[idx] = __ques[idx] + 1;
            __waiting[idx] = 0;
            tt_fiber_resume(__fb_ar[idx]);

            if (__err_line != 0) {
                return TT_FAIL;
            }
        }
    }
    __ut_ret = TT_SUCCESS;

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_fiber_sanity)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_thread_t *t;
    tt_thread_attr_t tattr;

    TT_TEST_CASE_ENTER()
    // test start

    tt_thread_attr_default(&tattr);
    tattr.enable_fiber = TT_TRUE;

    __ut_ret = TT_FAIL;
    __err_line = 0;

    t = tt_thread_create(__test_fiber_2, NULL, &tattr);
    tt_thread_wait(t);

    TT_TEST_CHECK_EQUAL(__ut_ret, TT_SUCCESS, "");

    // test end
    TT_TEST_CASE_LEAVE()
}
