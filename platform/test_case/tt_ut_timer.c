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

#include <os/tt_fiber.h>
#include <os/tt_fiber_event.h>
#include <os/tt_task.h>
#include <os/tt_thread.h>
#include <time/tt_time_reference.h>
#include <time/tt_timer.h>
#include <time/tt_timer_manager.h>

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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_tmr_basic)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_tmr_fiber)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_tmr_accuracy)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_tmr_stable)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(tmr_case)

TT_TEST_CASE("tt_unit_test_tmr_basic",
             "testing tmr mgr basic api",
             tt_unit_test_tmr_basic,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_tmr_fiber",
                 "testing tmr mgr with fiber",
                 tt_unit_test_tmr_fiber,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_tmr_accuracy",
                 "testing tmr mgr accuracy",
                 tt_unit_test_tmr_accuracy,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_tmr_stable",
                 "testing tmr mgr stability",
                 tt_unit_test_tmr_stable,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(tmr_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(TEST_UNIT_TIMER, 0, tmr_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(tt_unit_test_tmr_fiber)
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

TT_TEST_ROUTINE_DEFINE(tt_unit_test_tmr_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    tt_tmr_t *tmr, *tt;
    tt_s64_t t1, t2;
    tt_fiber_t *cfb = tt_current_fiber();

    TT_TEST_CASE_ENTER()
    // test start

    // create, destroy
    tmr = tt_tmr_create(20, 1, &_n1);
    TT_UT_NOT_EQUAL(tmr, NULL, "");
    tt_tmr_destroy(tmr);

    // create, start, destroy
    tmr = tt_tmr_create(20, 1, &_n1);
    TT_UT_NOT_EQUAL(tmr, NULL, "");
    ret = tt_tmr_start(tmr);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    tt_tmr_destroy(tmr);

    // create, start, stop, destroy
    tmr = tt_tmr_create(20, 1, &_n1);
    TT_UT_NOT_EQUAL(tmr, NULL, "");
    ret = tt_tmr_start(tmr);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    tt_tmr_stop(tmr);
    tt_tmr_destroy(tmr);

    // create timer
    tmr = tt_tmr_create(20, 1, &_n1);
    TT_UT_NOT_EQUAL(tmr, NULL, "");

    t1 = tt_time_ref();
    ret = tt_tmr_start(tmr);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    // normal timer expired
    tt = tt_fiber_recv_timer(cfb, TT_FALSE);
    TT_UT_NULL(tt, "");
    tt = tt_fiber_recv_timer(cfb, TT_TRUE);
    t2 = tt_time_ref();
    TT_UT_EQUAL(tt, tmr, "");
    TT_UT_EQUAL(tt->ev, 1, "");
    TT_UT_EQUAL(tt->param, &_n1, "");
    TT_UT_EXP(labs((long)tt_time_ref2ms(t2 - t1) - 20) < 5, "");
    tt = tt_fiber_recv_timer(cfb, TT_FALSE);
    TT_UT_NULL(tt, "");
    tt_tmr_stop(tmr);

    // stop timer before expired
    do {
        tt_tmr_set_delay(tmr, 30);
        tt_tmr_set_param(tmr, &_n1);
        ret = tt_tmr_start(tmr);
    } while (0);
    tt = tt_fiber_recv_timer(cfb, TT_FALSE);
    TT_UT_NULL(tt, "");
    tt_sleep(10);
    tt_tmr_stop(tmr);
    tt_sleep(20);
    tt = tt_fiber_recv_timer(cfb, TT_FALSE);
    TT_UT_NULL(tt, "");
    tt_sleep(20);
    tt = tt_fiber_recv_timer(cfb, TT_FALSE);
    TT_UT_NULL(tt, "");

    tt_tmr_destroy(tmr);
    tmr = NULL;

    // restart then destroy before expired
    tmr = tt_tmr_create(20, 2, &_n1);
    TT_UT_NOT_EQUAL(tmr, NULL, "");
    ret = tt_tmr_start(tmr);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    tt = tt_fiber_recv_timer(cfb, TT_TRUE);
    TT_UT_EQUAL(tt, tmr, "");

    do {
        tt_tmr_set_delay(tmr, 50);
        tt_tmr_set_param(tmr, NULL);
        t1 = tt_time_ref();
        ret = tt_tmr_start(tmr);
    } while (0);
    tt = tt_fiber_recv_timer(cfb, TT_TRUE);
    t2 = tt_time_ref();
    TT_UT_EQUAL(tt, tmr, "");
    TT_UT_EQUAL(tt->ev, 2, "");
    TT_UT_EQUAL(tt->param, NULL, "");
    TT_UT_EXP(labs((long)tt_time_ref2ms(t2 - t1) - 50) < 5, "");

    tt_tmr_destroy(tmr);

    // test end
    TT_TEST_CASE_LEAVE()
}

tt_result_t __fb_tmr_1(IN void *param)
{
    tt_tmr_t *t = tt_tmr_create(1000, 0, NULL);

    return TT_SUCCESS;
}

tt_result_t __fb_tmr_2(IN void *param)
{
    tt_tmr_t *t = tt_tmr_create(1000, 0, NULL);

    tt_tmr_destroy(t);
    return TT_SUCCESS;
}

tt_result_t __fb_tmr_3(IN void *param)
{
    tt_tmr_t *t = tt_tmr_create(1000, 0, NULL);

    tt_tmr_start(t);
    return TT_SUCCESS;
}

tt_result_t __fb_tmr_4(IN void *param)
{
    tt_tmr_t *t = tt_tmr_create(100, 0, NULL);

    tt_tmr_stop(t);
    return TT_SUCCESS;
}

tt_result_t __fb_tmr_5(IN void *param)
{
    tt_tmr_t *t = tt_tmr_create(100, 0, NULL), *e;

    tt_tmr_start(t);
    e = tt_fiber_recv_timer(tt_current_fiber(), TT_TRUE);
    TT_ASSERT(e == t);
    tt_tmr_stop(t);

    return TT_SUCCESS;
}

tt_result_t __fb_tmr_6(IN void *param)
{
    tt_tmr_t *t = tt_tmr_create(100, 0, NULL), *e;
    tt_s64_t now = tt_time_ref();
    tt_s64_t ms;

    now = tt_time_ref();
    tt_tmr_start(t);
    e = tt_fiber_recv_timer(tt_current_fiber(), TT_TRUE);
    TT_ASSERT(e == t);
    ms = tt_time_ref2ms(tt_time_ref() - now);
    TT_INFO("timer 100ms, elapsed %dms", ms);

    now = tt_time_ref();
    tt_tmr_start(t);
    e = tt_fiber_recv_timer(tt_current_fiber(), TT_TRUE);
    TT_ASSERT(e == t);
    ms = tt_time_ref2ms(tt_time_ref() - now);
    TT_INFO("timer 100ms, elapsed %dms", ms);

    now = tt_time_ref();
    tt_tmr_start(t);
    e = tt_fiber_recv_timer(tt_current_fiber(), TT_TRUE);
    TT_ASSERT(e == t);
    ms = tt_time_ref2ms(tt_time_ref() - now);
    TT_INFO("timer 100ms, elapsed %dms", ms);

    tt_tmr_set_delay(t, 200);
    now = tt_time_ref();
    tt_tmr_start(t);
    e = tt_fiber_recv_timer(tt_current_fiber(), TT_TRUE);
    TT_ASSERT(e == t);
    ms = tt_time_ref2ms(tt_time_ref() - now);
    TT_INFO("timer 200ms, elapsed %dms", ms);

    tt_tmr_set_delay(t, 400);
    now = tt_time_ref();
    tt_tmr_start(t);
    e = tt_fiber_recv_timer(tt_current_fiber(), TT_TRUE);
    TT_ASSERT(e == t);
    ms = tt_time_ref2ms(tt_time_ref() - now);
    TT_INFO("timer 400ms, elapsed %dms", ms);

    tt_tmr_set_delay(e, 1000);
    tt_tmr_start(e);

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_tmr_fiber)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_task_t t;
    tt_result_t ret;
    tt_u32_t n, i;

    TT_TEST_CASE_ENTER()
    // test start

    ret = tt_task_create(&t, NULL);
    TT_UT_SUCCESS(ret, "");
    tt_task_add_fiber(&t, "f1", __fb_tmr_1, NULL, NULL);
    tt_task_run(&t);
    tt_task_wait(&t);
    TT_INFO("timer fiber 1");

    ret = tt_task_create(&t, NULL);
    TT_UT_SUCCESS(ret, "");
    tt_task_add_fiber(&t, "f1", __fb_tmr_2, NULL, NULL);
    tt_task_run(&t);
    tt_task_wait(&t);
    TT_INFO("timer fiber 2");

    ret = tt_task_create(&t, NULL);
    TT_UT_SUCCESS(ret, "");
    tt_task_add_fiber(&t, "f1", __fb_tmr_3, NULL, NULL);
    tt_task_run(&t);
    tt_task_wait(&t);
    TT_INFO("timer fiber 3");

    ret = tt_task_create(&t, NULL);
    TT_UT_SUCCESS(ret, "");
    tt_task_add_fiber(&t, "f1", __fb_tmr_4, NULL, NULL);
    tt_task_run(&t);
    tt_task_wait(&t);
    TT_INFO("timer fiber 4");

    ret = tt_task_create(&t, NULL);
    TT_UT_SUCCESS(ret, "");
    tt_task_add_fiber(&t, "f1", __fb_tmr_5, NULL, NULL);
    tt_task_run(&t);
    tt_task_wait(&t);
    TT_INFO("timer fiber 5");

    ret = tt_task_create(&t, NULL);
    TT_UT_SUCCESS(ret, "");
    tt_task_add_fiber(&t, "f1", __fb_tmr_6, NULL, NULL);
    tt_task_run(&t);
    tt_task_wait(&t);
    TT_INFO("timer fiber 6");

    ret = tt_task_create(&t, NULL);
    TT_UT_SUCCESS(ret, "");

    n = tt_rand_u32() % 10;
    for (i = 0; i < n; ++i) {
        tt_task_add_fiber(&t, NULL, __fb_tmr_1, NULL, NULL);
    }
    n = tt_rand_u32() % 10;
    for (i = 0; i < n; ++i) {
        tt_task_add_fiber(&t, NULL, __fb_tmr_2, NULL, NULL);
    }
    n = tt_rand_u32() % 10;
    for (i = 0; i < n; ++i) {
        tt_task_add_fiber(&t, NULL, __fb_tmr_3, NULL, NULL);
    }
    n = tt_rand_u32() % 10;
    for (i = 0; i < n; ++i) {
        tt_task_add_fiber(&t, NULL, __fb_tmr_4, NULL, NULL);
    }
    n = tt_rand_u32() % 10;
    for (i = 0; i < n; ++i) {
        tt_task_add_fiber(&t, NULL, __fb_tmr_5, NULL, NULL);
    }
    n = tt_rand_u32() % 10;
    for (i = 0; i < n; ++i) {
        tt_task_add_fiber(&t, NULL, __fb_tmr_6, NULL, NULL);
    }
    tt_task_run(&t);
    tt_task_wait(&t);
    TT_INFO("timer fiber N");

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_s64_t max_diff;

TT_TEST_ROUTINE_DEFINE(tt_unit_test_tmr_accuracy)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    tt_fiber_t *cfb = tt_current_fiber();

#define __case2_tn 1000
    static tt_tmr_t *__tmrs2[__case2_tn];
    static tt_s64_t __start[__case2_tn];
    int i;

    TT_TEST_CASE_ENTER()
    // test start

    for (i = 0; i < __case2_tn; ++i) {
        tt_s64_t exp = (rand() % 100) * 10; // 1s exp at most

        __tmrs2[i] = tt_tmr_create(exp, i, NULL);
        TT_UT_NOT_EQUAL(__tmrs2[i], NULL, "");
    }

    _n1 = 0;
    // start all timers
    for (i = 0; i < __case2_tn; ++i) {
        // tt_sleep(rand()%5);
        __start[i] = tt_time_ref();
        ret = tt_tmr_start(__tmrs2[i]);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
    }

    // run all timers
    i = 0;
    max_diff = 0;
    do {
        tt_tmr_t *tmr = tt_fiber_recv_timer(cfb, TT_TRUE);
        tt_s64_t now = tt_time_ref();
        TT_UT_NOT_NULL(tmr, "");
        if (++i == __case2_tn) {
            break;
        }
        now -= __start[tmr->ev];
        now = tt_time_ref2ms(now);
        now = labs((long)(now - tmr->delay_ms));
        if (now > max_diff) {
            max_diff = now;
        }
    } while (1);
    TT_INFO("max_diff: %dms", max_diff);

    // start all timers
    for (i = 0; i < __case2_tn; ++i) {
        tt_tmr_destroy(__tmrs2[i]);
    }

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_s64_t max_diff3;

TT_TEST_ROUTINE_DEFINE(tt_unit_test_tmr_stable)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    tt_fiber_t *cfb = tt_current_fiber();

#define __case3_tn 5000
#define __case3_op 10000
    static tt_tmr_t *__tmrs3[__case3_tn];
    static tt_u8_t destroyed[__case3_tn];
    static tt_s64_t __start3[__case3_tn];

    int i;
    tt_s64_t now;
    tt_tmr_t *tmr;

    TT_TEST_CASE_ENTER()
    // test start

    for (i = 0; i < __case3_tn; ++i) {
        tt_s64_t exp = (rand() % 100) * 10; // 1s exp at most

        __tmrs3[i] = tt_tmr_create(exp, i, NULL);
        TT_UT_NOT_EQUAL(__tmrs3[i], NULL, "");
    }

    for (i = 0; i < __case3_op; ++i) {
        int idx = rand() % __case3_tn;

        // keep the first timer running
        if (idx == 0) {
            __start3[idx] = tt_time_ref();
            tt_tmr_start(__tmrs3[idx]);
            continue;
        }

        if (!destroyed[idx]) {
            int op__ = rand() % 10;
            if (op__ < 3) { // 30% start
                __start3[idx] = tt_time_ref();
                tt_tmr_start(__tmrs3[idx]);
            } else if (op__ < 6) { // 30% stop
                __start3[idx] = 0;
                tt_tmr_stop(__tmrs3[idx]);
            } else if (op__ < 9) { // 20% restart
                tt_s64_t e1 = (rand() % 100) * 10;
                __start3[idx] = tt_time_ref();
                do {
                    tt_tmr_set_delay(__tmrs3[idx], e1);
                    tt_tmr_set_param(__tmrs3[idx], &__start3[idx]);
                    ret = tt_tmr_start(__tmrs3[idx]);
                } while (0);
                TT_UT_EQUAL(ret, TT_SUCCESS, "");
            } else { // 10% destroy
                tt_tmr_destroy(__tmrs3[idx]);
                destroyed[idx] = 1;
            }
        }

        if (i % 10 == 0) {
            tmr = tt_fiber_recv_timer(cfb, TT_TRUE);
            TT_UT_NOT_NULL(tmr, "");
            now = tt_time_ref();
            now -= __start3[tmr->ev];
            now = tt_time_ref2ms(now);
            now = labs((long)(now - tmr->delay_ms));
            if (now > max_diff) {
                max_diff = now;
            }

            if (tmr->ev == 0) {
                tt_tmr_set_delay(tmr, (rand() % 100) * 10);
                __start3[0] = tt_time_ref();
                ret = tt_tmr_start(tmr);
                TT_UT_EQUAL(ret, TT_SUCCESS, "");
            }
        }
    }
    TT_INFO("max_diff: %dms", max_diff);

    // start all timers
    for (i = 0; i < __case3_tn; ++i) {
        if (!destroyed[i])
            tt_tmr_destroy(__tmrs3[i]);
    }

    // test end
    TT_TEST_CASE_LEAVE()
}
