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

#include <os/tt_thread.h>
#include <timer/tt_time_reference.h>
#include <timer/tt_timer.h>
#include <timer/tt_timer_manager.h>

// portlayer header files
#include <tt_cstd_api.h>

#include <time.h>
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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_tmrm_basic)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_tmrm_accuracy)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_tmrm_stable)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(tmrm_case)

TT_TEST_CASE("tt_unit_test_tmrm_basic",
             "testing tmr mgr basic api",
             tt_unit_test_tmrm_basic,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_tmrm_accuracy",
                 "testing tmr mgr accuracy",
                 tt_unit_test_tmrm_accuracy,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_tmrm_stable",
                 "testing tmr mgr stability",
                 tt_unit_test_tmrm_stable,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(tmrm_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(TEST_UNIT_TIMER_MGR, 0, tmrm_case)

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

    static int _n1;
void __ta1(IN struct tt_tmr_s *timer, IN void *param, IN tt_u32_t reason)
{
    int *pn1 = (int *)param;

    *pn1 += 1;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_tmrm_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_tmr_mgr_t tmrm;
    tt_tmr_mgr_attr_t attr;
    tt_result_t ret;
    tt_tmr_t *tmr;
    tt_s64_t wait;

    TT_TEST_CASE_ENTER()
    // test start

    tt_tmr_mgr_attr_default(&attr);

    ret = tt_tmr_mgr_create(&tmrm, &attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // create timer
    tmr = tt_tmr_create(&tmrm, 20, 0, __ta1, &_n1, 0);
    TT_TEST_CHECK_NOT_EQUAL(tmr, NULL, "");

    ret = tt_tmr_start(tmr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // normal timer expired
    TT_TEST_CHECK_EQUAL(_n1, 0, "");
    wait = tt_tmr_mgr_run(&tmrm);
    TT_TEST_CHECK_EXP(abs((int)wait - 20) < 5, "");
    tt_sleep(25);
    wait = tt_tmr_mgr_run(&tmrm);
    TT_TEST_CHECK_EQUAL(wait, TT_TIME_INFINITE, "");
    TT_TEST_CHECK_EQUAL(_n1, 1, "");
    tt_tmr_stop(tmr);

    // stop timer before expired
    do {
        tt_tmr_set_delay(tmr, 30);
        tt_tmr_set_cb(tmr, __ta1);
        tt_tmr_set_cbparam(tmr, &_n1);
        ret = tt_tmr_start(tmr);
    } while (0);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    wait = tt_tmr_mgr_run(&tmrm);
    TT_TEST_CHECK_EXP(abs((int)wait - 30) < 5, "");
    tt_tmr_stop(tmr);
    tt_sleep(35);
    wait = tt_tmr_mgr_run(&tmrm);
    TT_TEST_CHECK_EQUAL(wait, TT_TIME_INFINITE, "");
    TT_TEST_CHECK_EQUAL(_n1, 1, "");

    tt_tmr_destroy(tmr);
    tmr = NULL;

    // restart then destroy before expired
    tmr = tt_tmr_create(&tmrm, 20, 0, __ta1, &_n1, 0);
    TT_TEST_CHECK_NOT_EQUAL(tmr, NULL, "");
    ret = tt_tmr_start(tmr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    wait = tt_tmr_mgr_run(&tmrm);
    TT_TEST_CHECK_EXP(abs((int)wait - 20) < 5, "");

    do {
        tt_tmr_set_delay(tmr, 50);
        tt_tmr_set_cb(tmr, __ta1);
        tt_tmr_set_cbparam(tmr, &_n1);
        ret = tt_tmr_start(tmr);
    } while (0);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    wait = tt_tmr_mgr_run(&tmrm);
    TT_TEST_CHECK_EXP(abs((int)wait - 50) < 5, "");

    tt_tmr_destroy(tmr);
    wait = tt_tmr_mgr_run(&tmrm);
    TT_TEST_CHECK_EQUAL(wait, TT_TIME_INFINITE, "");
    TT_TEST_CHECK_EQUAL(_n1, 1, "");

    tt_tmr_mgr_destroy(&tmrm);

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_bool_t __c2_ok = TT_TRUE;
static tt_s64_t max_diff;

void __ta2(IN struct tt_tmr_s *timer, IN void *param, IN tt_u32_t reason)
{
    tt_s64_t start = *((tt_s64_t *)param);
    tt_s64_t now = tt_time_ref();

    tt_s64_t diff = tt_time_ref2ms(now - start);
    if (diff > timer->delay_ms)
        diff -= timer->delay_ms;
    else
        diff = timer->delay_ms - diff;

    if (diff > max_diff)
        max_diff = diff;

    if (diff > 5) {
        // TT_ERROR("diff: %dms", diff);
        __c2_ok = TT_FALSE;
    }

    _n1 += 1;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_tmrm_accuracy)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_tmr_mgr_t tmrm;
    tt_result_t ret;

#define __case2_tn 1000
    static tt_tmr_t *__tmrs2[__case2_tn];
    static tt_s64_t __start[__case2_tn];
    int i;
    tt_s64_t wait;

    TT_TEST_CASE_ENTER()
    // test start

    srand((int)time(NULL));

    ret = tt_tmr_mgr_create(&tmrm, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    for (i = 0; i < __case2_tn; ++i) {
        tt_s64_t exp = (rand() % 100) * 10; // 1s exp at most

        __tmrs2[i] = tt_tmr_create(&tmrm, exp, 0, __ta2, &__start[i], 0);
        TT_TEST_CHECK_NOT_EQUAL(__tmrs2[i], NULL, "");
    }

    _n1 = 0;
    // start all timers
    for (i = 0; i < __case2_tn; ++i) {
        // tt_sleep(rand()%5);
        __start[i] = tt_time_ref();
        ret = tt_tmr_start(__tmrs2[i]);
        tt_tmr_mgr_run(&tmrm);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    }

    // run all timers
    do {
        wait = tt_tmr_mgr_run(&tmrm);
        if (wait == TT_TIME_INFINITE) {
            break;
        } else {
            tt_sleep((tt_u32_t)wait);
        }
    } while (1);
    TT_INFO("max_diff: %dms", max_diff);
    TT_TEST_CHECK_EQUAL(__c2_ok, TT_TRUE, "");
    TT_TEST_CHECK_EQUAL(_n1, 1000, "");

    // start all timers
    for (i = 0; i < __case2_tn; ++i) {
        tt_tmr_destroy(__tmrs2[i]);
    }

    tt_tmr_mgr_destroy(&tmrm);

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_bool_t __c3_ok = TT_TRUE;
static tt_s64_t max_diff3;

void __ta3(IN void *param, IN struct tt_tmr_s *timer)
{
    tt_s64_t start = *((tt_s64_t *)param);
    tt_s64_t now = tt_time_ref();

    tt_s64_t diff = tt_time_ref2ms(now - start);
    if (diff > timer->delay_ms)
        diff -= timer->delay_ms;
    else
        diff = timer->delay_ms - diff;

    if (diff > max_diff3)
        max_diff3 = diff;

    if (diff > 5) {
        __c3_ok = TT_FALSE;
    }

    _n1 += 1;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_tmrm_stable)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_tmr_mgr_t tmrm;
    tt_result_t ret;

#define __case3_tn 5000
#define __case3_op 10000
    static tt_tmr_t *__tmrs3[__case3_tn];
    static tt_u8_t destroyed[__case3_tn];
    static tt_s64_t __start3[__case3_tn];

    int i;
    tt_s64_t wait;

    TT_TEST_CASE_ENTER()
    // test start

    srand((int)time(NULL));

    ret = tt_tmr_mgr_create(&tmrm, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    for (i = 0; i < __case3_tn; ++i) {
        tt_s64_t exp = (rand() % 100) * 10; // 1s exp at most

        __tmrs3[i] = tt_tmr_create(&tmrm, exp, 0, __ta2, &__start3[i], 0);
        TT_TEST_CHECK_NOT_EQUAL(__tmrs3[i], NULL, "");
    }


    for (i = 0; i < __case3_op; ++i) {
        int idx = rand() % __case3_tn;

        if (!destroyed[idx]) {
            int op__ = rand() % 10;
            if (op__ < 3) { // 30% start
                __start3[idx] = tt_time_ref();
                tt_tmr_start(__tmrs3[idx]);
            } else if (op__ < 6) { // 30% stop
                tt_tmr_stop(__tmrs3[idx]);
            } else if (op__ < 9) { // 20% restart
                tt_s64_t e1 = (rand() % 100) * 10;
                __start3[idx] = tt_time_ref();
                do {
                    tt_tmr_set_delay(__tmrs3[idx], e1);
                    tt_tmr_set_cb(__tmrs3[idx], __ta2);
                    tt_tmr_set_cbparam(__tmrs3[idx], &__start3[idx]);
                    ret = tt_tmr_start(__tmrs3[idx]);
                } while (0);
                TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
            } else { // 10% destroy
                tt_tmr_destroy(__tmrs3[idx]);
                destroyed[idx] = 1;
            }
        }
        tt_tmr_mgr_run(&tmrm);
    }

    // run all timers
    do {
        wait = tt_tmr_mgr_run(&tmrm);
        if (wait == TT_TIME_INFINITE) {
            break;
        } else {
            tt_sleep((tt_u32_t)wait);
        }
    } while (1);
    TT_TEST_CHECK_EQUAL(__c3_ok, TT_TRUE, "");

    // start all timers
    for (i = 0; i < __case3_tn; ++i) {
        if (!destroyed[i])
            tt_tmr_destroy(__tmrs3[i]);
    }

    tt_tmr_mgr_destroy(&tmrm);

    // test end
    TT_TEST_CASE_LEAVE()
}
