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

#include "tt_unit_test_case_config.h"
#include <unit_test/tt_unit_test.h>

#include <io/tt_io_worker_group.h>
#include <log/tt_log.h>
#include <os/tt_fiber.h>
#include <os/tt_fiber_event.h>
#include <os/tt_semaphore.h>
#include <os/tt_task.h>
#include <os/tt_thread.h>
#include <time/tt_time_reference.h>

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
TT_TEST_ROUTINE_DECLARE(case_fiber_basic)
TT_TEST_ROUTINE_DECLARE(case_fiber_3fibers)
TT_TEST_ROUTINE_DECLARE(case_fiber_sanity)
TT_TEST_ROUTINE_DECLARE(case_fiber_sanity2)
TT_TEST_ROUTINE_DECLARE(case_fiber_event)

TT_TEST_ROUTINE_DECLARE(case_worker_group)

TT_TEST_ROUTINE_DECLARE(case_task)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(fiber_case)

TT_TEST_CASE("case_fiber_basic",
             "testing basic fiber API",
             case_fiber_basic,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("case_fiber_3fibers",
                 "testing basic fiber API, 3 fibers",
                 case_fiber_3fibers,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_fiber_sanity",
                 "testing fiber sanity",
                 case_fiber_sanity,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_fiber_sanity2",
                 "testing fiber sanity more",
                 case_fiber_sanity2,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_worker_group",
                 "testing worker group",
                 case_worker_group,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_task",
                 "testing task basic",
                 case_task,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_fiber_event",
                 "testing fiber event send/recv",
                 case_fiber_event,
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
    TT_TEST_ROUTINE_DEFINE(case_task)
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

static tt_result_t __test_fiber_main_yield(IN void *param)
{
    tt_u32_t i = 0;
    while (i++ < 100) {
        tt_fiber_suspend();
    }

    return TT_SUCCESS;
}

static tt_result_t __fiber_1(IN void *param)
{
    if (param != (void *)1) {
        return TT_FAIL;
    }

    while (__ut_num < 100) {
        ++__ut_num;
        tt_fiber_suspend();
    }

    return TT_SUCCESS;
}

static tt_result_t __fiber_name(IN void *param)
{
    if ((tt_fiber_find("1") != NULL) && (tt_fiber_find("") != NULL) &&
        (tt_fiber_find("12") == NULL)) {
        __ut_ret = TT_SUCCESS;
    }

    return TT_SUCCESS;
}

static tt_result_t __test_fiber_2f_yield_resume(IN void *param)
{
    tt_fiber_t *f1;
    tt_u32_t last_num;

    f1 = tt_fiber_create(NULL, __fiber_1, (void *)1, NULL);
    if (f1 == NULL) {
        __ut_ret = TT_FAIL;
        __err_line = __LINE__;
        return TT_FAIL;
    }

    __ut_num = 0;
    tt_fiber_resume(f1, TT_FALSE);

    last_num = __ut_num;
    while (__ut_num < 100) {
        tt_fiber_resume(f1, TT_FALSE);

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

static tt_result_t __fiber_1_resume(IN void *param)
{
    tt_fiber_sched_t *cfs = tt_current_fiber_sched();

    if (param != (void *)1) {
        return TT_FAIL;
    }

    while (__ut_num < 100) {
        ++__ut_num;
        tt_fiber_resume(cfs->__main, TT_TRUE);
    }

    return TT_SUCCESS;
}

static tt_result_t __test_fiber_2f_resume(IN void *param)
{
    tt_fiber_t *f1;
    tt_u32_t last_num;
    tt_fiber_attr_t fattr;

    tt_fiber_attr_default(&fattr);
    fattr.stack_size = 1;

    f1 = tt_fiber_create(NULL, __fiber_1_resume, (void *)1, &fattr);
    if (f1 == NULL) {
        __ut_ret = TT_FAIL;
        __err_line = __LINE__;
        return TT_FAIL;
    }

    __ut_num = 0;
    tt_fiber_resume(f1, TT_FALSE);

    last_num = __ut_num;
    while (__ut_num < 100) {
        tt_fiber_resume(f1, TT_FALSE);

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

TT_TEST_ROUTINE_DEFINE(case_fiber_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_thread_t *t;
    tt_thread_attr_t tattr;
    tt_fiber_t *fb1, *fb2, *fb3;

    TT_TEST_CASE_ENTER()
    // test start

    __ut_ret = TT_FAIL;
    fb1 = tt_fiber_create("1", __fiber_name, NULL, NULL);
    TT_UT_NOT_EQUAL(fb1, NULL, "");
    TT_UT_EQUAL(tt_fiber_find("1"), fb1, "");

    // duplicated name
    fb2 = tt_fiber_create("1", __fiber_name, NULL, NULL);
    TT_UT_EQUAL(fb2, NULL, "");

    fb2 = tt_fiber_create("", __fiber_name, NULL, NULL);
    TT_UT_NOT_EQUAL(fb2, NULL, "");
    fb3 = tt_fiber_create("", __fiber_name, NULL, NULL);
    TT_UT_EQUAL(fb3, NULL, "");
    tt_fiber_resume(fb1, TT_FALSE);
    tt_fiber_resume(fb2, TT_FALSE);

    tt_thread_attr_default(&tattr);
    tattr.enable_fiber = TT_TRUE;

    __ut_ret = TT_FAIL;

    // 1 main fiber, yield
    t = tt_thread_create(__test_fiber_main_yield, NULL, &tattr);
    tt_thread_wait(t);

    // 2 fibers, 1 yield, main resume
    __ut_num = 0;
    t = tt_thread_create(__test_fiber_2f_yield_resume, NULL, &tattr);
    tt_thread_wait(t);
    TT_UT_EQUAL(__ut_ret, TT_SUCCESS, "");

    // 2 fibers, 1 resume, main resume
    __ut_num = 0;
    t = tt_thread_create(__test_fiber_2f_resume, NULL, &tattr);
    tt_thread_wait(t);
    TT_UT_EQUAL(__ut_ret, TT_SUCCESS, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

#define FIBER_NUM 2
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
        tt_fiber_suspend();

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
        tt_fiber_attr_t fattr;
        tt_fiber_attr_default(&fattr);
        fattr.stack_size = tt_rand_u32() % (1 << 6) + 1;

        __fb_ar[i] =
            tt_fiber_create(NULL, __fiber_2, (void *)(tt_uintptr_t)i, &fattr);
        if (__fb_ar[i] == NULL) {
            __err_line = __LINE__;
            return TT_FAIL;
        }
        tt_fiber_resume(__fb_ar[i], TT_FALSE);
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
            tt_fiber_resume(__fb_ar[idx], TT_FALSE);

            if (__err_line != 0) {
                return TT_FAIL;
            }
        }
    }
    __ut_ret = TT_SUCCESS;

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(case_fiber_sanity)
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

    TT_UT_EQUAL(__ut_ret, TT_SUCCESS, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_result_t __fiber_3(IN void *param)
{
    tt_u32_t idx = (tt_u32_t)(tt_uintptr_t)param;
    tt_fiber_sched_t *cfs = tt_current_fiber_sched();

    TT_INFO("entered fiber[%d]", idx);
    while (1) {
        tt_u32_t i = tt_rand_u32() % 4;
        TT_INFO("fiber[%d] => [%d]", idx, i);
        if (i == 3) {
            tt_fiber_resume(cfs->__main, TT_TRUE);
        } else {
            tt_fiber_resume(__fb_ar[i], TT_FALSE);
        }
        TT_INFO("fiber[%d]", idx);
    }

    return TT_SUCCESS;
}

static tt_result_t __test_fiber_3(IN void *param)
{
    tt_u32_t i, num = 0;

    for (i = 0; i < 3; ++i) {
        __fb_ar[i] =
            tt_fiber_create(NULL, __fiber_3, (void *)(tt_uintptr_t)i, NULL);
        if (__fb_ar[i] == NULL) {
            __err_line = __LINE__;
            return TT_FAIL;
        }
    }
    tt_fiber_resume(__fb_ar[0], TT_TRUE);

    while (num++ < 100) {
        tt_u32_t idx = tt_rand_u32() % 3;
        tt_fiber_resume(__fb_ar[idx], TT_TRUE);
        TT_INFO("main num: %d", num);
    }
    __ut_ret = TT_SUCCESS;

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(case_fiber_3fibers)
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

    t = tt_thread_create(__test_fiber_3, NULL, &tattr);
    tt_thread_wait(t);

    TT_UT_EQUAL(__ut_ret, TT_SUCCESS, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_result_t __fiber_san2(IN void *param)
{
    tt_u32_t idx = (tt_u32_t)(tt_uintptr_t)param;
    tt_fiber_sched_t *cfs = tt_current_fiber_sched();

    if (idx < FIBER_NUM - 1) {
        tt_fiber_attr_t fattr;
        tt_fiber_attr_default(&fattr);
        fattr.stack_size = tt_rand_u32() % (1 << 6) + 1;

        __fb_ar[idx + 1] = tt_fiber_create(NULL,
                                           __fiber_san2,
                                           (void *)(tt_uintptr_t)(idx + 1),
                                           &fattr);
        tt_fiber_resume(__fb_ar[idx + 1], TT_TRUE);
    }

    while (1) {
        tt_u32_t action = tt_rand_u32() % 4;
        if (action == 0) {
            tt_fiber_suspend();
        } else if (action == 1) {
            tt_u32_t k = tt_rand_u32() % FIBER_NUM;
            if (__fb_ar[k] != NULL) {
                // TT_INFO("resuming %p", __fb_ar[idx]);
                tt_fiber_resume(__fb_ar[k], TT_TRUE);
            }
        } else if (action == 2) {
            // TT_INFO("exiting %p", __fb_ar[idx]);
            __fb_ar[idx] = NULL;
            break;
        } else {
            tt_fiber_resume(cfs->__main, TT_TRUE);
        }
    }

    return TT_SUCCESS;
}

static tt_result_t __test_fiber_san2(IN void *param)
{
    tt_u32_t num = 0;

#if 0
    for (i = 0; i < FIBER_NUM; ++i) {
        tt_fiber_attr_t fattr;
        tt_fiber_attr_default(&fattr);
        fattr.stack_size = tt_rand_u32() % (1 << 6) + 1;

        __fb_ar[i] =
            tt_fiber_create(NULL, __fiber_san2, (void *)(tt_uintptr_t)i, &fattr);
        if (__fb_ar[i] == NULL) {
            __err_line = __LINE__;
            return TT_FAIL;
        }
    }
    tt_fiber_resume(__fb_ar[tt_rand_u32() % FIBER_NUM]);
#else
    {
        tt_fiber_attr_t fattr;
        tt_fiber_attr_default(&fattr);
        fattr.stack_size = tt_rand_u32() % (1 << 6) + 1;

        __fb_ar[0] = tt_fiber_create(NULL,
                                     __fiber_san2,
                                     (void *)(tt_uintptr_t)0,
                                     &fattr);
        if (__fb_ar[0] == NULL) {
            __err_line = __LINE__;
            return TT_FAIL;
        }
    }
    tt_fiber_resume(__fb_ar[0], TT_TRUE);
#endif

    while (num++ < FIBER_NUM * 100) {
        tt_u32_t idx = tt_rand_u32() % FIBER_NUM;
        if (__fb_ar[idx] != NULL) {
            // TT_INFO("resuming %p", __fb_ar[idx]);
            tt_fiber_resume(__fb_ar[idx], TT_TRUE);
        }
    }
    __ut_ret = TT_SUCCESS;

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(case_fiber_sanity2)
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

    t = tt_thread_create(__test_fiber_san2, NULL, &tattr);
    tt_thread_wait(t);

    TT_UT_EQUAL(__ut_ret, TT_SUCCESS, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_worker_group)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_iowg_t wg;
    tt_result_t ret;
    tt_u32_t n;

    TT_TEST_CASE_ENTER()
    // test start

    ret = tt_iowg_create(&wg, 0, 0, NULL);
    TT_UT_SUCCESS(ret, "");
    tt_iowg_destroy(&wg);

    n = tt_rand_u32() % 10;
    ret = tt_iowg_create(&wg, n, n + tt_rand_u32() % 10, NULL);
    TT_UT_SUCCESS(ret, "");
    tt_iowg_destroy(&wg);

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_u32_t __tb_cnt;
static tt_u32_t __tb_cnt_expect;

static tt_result_t __task_fiber_1(IN void *param)
{
    tt_u32_t idx = (tt_u32_t)(tt_uintptr_t)param;

    ++__tb_cnt;

    if (idx % 2) {
        tt_fiber_suspend();
    }

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(case_task)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_task_t t1;
    tt_result_t ret;
    tt_u32_t n, i;

    TT_TEST_CASE_ENTER()
    // test start

    ret = tt_task_create(&t1, NULL);
    TT_UT_SUCCESS(ret, "");
    tt_task_exit(&t1);
    tt_task_wait(&t1);

    __tb_cnt = 0;
    ret = tt_task_create(&t1, NULL);
    TT_UT_SUCCESS(ret, "");
    tt_task_add_fiber(&t1, NULL, __task_fiber_1, (void *)(tt_uintptr_t)1, NULL);
    tt_task_run(&t1);
    tt_task_exit(&t1);
    tt_task_wait(&t1);
    TT_UT_EQUAL(__tb_cnt, 1, "");

    __tb_cnt = 0;
    ret = tt_task_create(&t1, NULL);
    TT_UT_SUCCESS(ret, "");
    n = 10; // tt_rand_u32() % 10 + 1;
    for (i = 0; i < n; ++i) {
        tt_task_add_fiber(&t1,
                          NULL,
                          __task_fiber_1,
                          (void *)(tt_uintptr_t)i,
                          NULL);
    }
    tt_task_run(&t1);
    tt_task_exit(&t1);
    tt_task_wait(&t1);
    TT_UT_EQUAL(__tb_cnt, n, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

struct __ut_fev
{
    tt_fiber_ev_t fev;
    tt_u32_t resp;
};

static tt_result_t __fiber_ev_send(IN void *param)
{
    tt_u32_t i = 0;
    struct __ut_fev uev;
    tt_fiber_t *recv = tt_fiber_find("recv");

    while (i < 1000) {
        tt_fiber_ev_init(&uev.fev, i);
        uev.resp = i;

        tt_fiber_send_ev(recv, &uev.fev, TT_TRUE);
        if (uev.resp != i + 100) {
            __err_line = __LINE__;
        }

        ++i;
    }

    return TT_SUCCESS;
}

static tt_result_t __fiber_ev_recv(IN void *param)
{
    tt_u32_t i = 0;
    tt_fiber_t *cfb = tt_current_fiber();

    while (i < 1000) {
        tt_fiber_ev_t *fev = tt_fiber_recv_ev(cfb, TT_TRUE);
        struct __ut_fev *uev;

        TT_ASSERT(fev != NULL);
        uev = TT_CONTAINER(fev, struct __ut_fev, fev);
        uev->resp += 100;
        tt_fiber_finish(fev);

        ++i;
    }

    return TT_SUCCESS;
}

static tt_result_t __fiber_ev_send_nw(IN void *param)
{
    tt_u32_t i = 0;
    tt_fiber_t *recv = tt_fiber_find("recv");

    while (i < 1000) {
        if (i % 2 == 0) {
            struct __ut_fev *uev =
                (struct __ut_fev *)tt_fiber_ev_create(i,
                                                      sizeof(struct __ut_fev) -
                                                          sizeof(
                                                              tt_fiber_ev_t));
            if (uev == NULL) {
                __err_line = __LINE__;
                return TT_FAIL;
            }
            uev->resp = i;

            tt_fiber_send_ev(recv, &uev->fev, TT_FALSE);
        } else {
            struct __ut_fev uev;

            tt_fiber_ev_init(&uev.fev, i);
            uev.resp = i;

            tt_fiber_send_ev(recv, &uev.fev, TT_TRUE);
            if (uev.resp != i + 200) {
                __err_line = __LINE__;
            }
        }

        ++i;
    }

    return TT_SUCCESS;
}

static tt_u32_t __ut_sum;

static tt_result_t __fiber_ev_recv_nw(IN void *param)
{
    tt_u32_t i = 0;
    tt_fiber_t *cfb = tt_current_fiber();

    while (i < 1000) {
        tt_fiber_ev_t *fev = tt_fiber_recv_ev(cfb, TT_FALSE);
        struct __ut_fev *uev;

        if (fev != NULL) {
            uev = TT_CONTAINER(fev, struct __ut_fev, fev);
            __ut_sum += uev->resp;
            uev->resp += 200;
            tt_fiber_finish(fev);

            ++i;
        } else {
            cfb->recving = TT_TRUE;
            tt_fiber_suspend();
            cfb->recving = TT_FALSE;
        }
    }

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(case_fiber_event)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_task_t t1;
    tt_result_t ret;

    TT_TEST_CASE_ENTER()
    // test start

    __err_line = 0;
    ret = tt_task_create(&t1, NULL);
    TT_UT_SUCCESS(ret, "");
    tt_task_add_fiber(&t1, "send", __fiber_ev_send, NULL, NULL);
    tt_task_add_fiber(&t1, "recv", __fiber_ev_recv, NULL, NULL);
    tt_task_run(&t1);
    tt_task_wait(&t1);
    TT_UT_EQUAL(__err_line, 0, "");

    __err_line = 0;
    __ut_sum = 0;
    ret = tt_task_create(&t1, NULL);
    TT_UT_SUCCESS(ret, "");
    tt_task_add_fiber(&t1, "send", __fiber_ev_send_nw, NULL, NULL);
    tt_task_add_fiber(&t1, "recv", __fiber_ev_recv_nw, NULL, NULL);
    tt_task_run(&t1);
    tt_task_wait(&t1);
    TT_UT_EQUAL(__err_line, 0, "");
    TT_UT_EQUAL(__ut_sum, 499500, ""); // (0+999)*500

    // test end
    TT_TEST_CASE_LEAVE()
}
