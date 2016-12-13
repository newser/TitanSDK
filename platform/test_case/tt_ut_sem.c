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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_sem_basic)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_sem_mt)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_sem_pc)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(sem_case)

TT_TEST_CASE("tt_unit_test_sem_basic",
             "testing basic sem API",
             tt_unit_test_sem_basic,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_sem_mt",
                 "testing sem API in multithread",
                 tt_unit_test_sem_mt,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_sem_pc",
                 "testing sem API in producer-consumer model",
                 tt_unit_test_sem_pc,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(sem_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(TEST_UNIT_SEM, 0, sem_case)

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

    TT_TEST_ROUTINE_DEFINE(tt_unit_test_sem_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_sem_t lock;
    tt_result_t ret = TT_FAIL;
    tt_sem_attr_t attr;

    tt_s64_t begin, end;
    tt_s32_t t;

    TT_TEST_CASE_ENTER()
    // test start

    ret = tt_sem_create(&lock, 0, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // lock, wait for 100ms
    begin = tt_time_ref();
    ret = tt_sem_acquire(&lock, 100);
    end = tt_time_ref();
    TT_TEST_CHECK_EQUAL(ret, TT_TIME_OUT, "");

    t = (tt_s32_t)tt_time_ref2ms(end - begin);
    TT_TEST_CHECK_EXP(abs(t - 100) < 10, "");

    tt_sem_destroy(&lock);

    // create with counter value 2
    tt_memset(&attr, 0, sizeof(tt_sem_attr_t));
    ret = tt_sem_create(&lock, 2, &attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // lock for two times
    ret = tt_sem_acquire(&lock, TT_TIME_INFINITE);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_sem_acquire(&lock, TT_TIME_INFINITE);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // test wait infinite
    // ret = tt_sem_acquire(&lock, TT_TIME_INFINITE);

    // trylock
    ret = tt_sem_try_acquire(&lock);
    // TT_TEST_CHECK_EQUAL(ret, TT_TIME_OUT, "");

    // unlock, relock
    tt_sem_release(&lock);
    ret = tt_sem_acquire(&lock, TT_TIME_INFINITE);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // unlock all
    tt_sem_release(&lock);
    tt_sem_release(&lock);

    // trylock
    ret = tt_sem_try_acquire(&lock);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // unlock
    tt_sem_release(&lock);

    // unlock, assertion
    // tt_sem_release(&lock);

    tt_sem_destroy(&lock);

    // create
    ret = tt_sem_create(&lock, 0, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    // with time specified
    ret = tt_sem_acquire(&lock, 100);
    TT_TEST_CHECK_EQUAL(ret, TT_TIME_OUT, "");
    ret = tt_sem_acquire(&lock, 100);
    TT_TEST_CHECK_EQUAL(ret, TT_TIME_OUT, "");
    tt_sem_release(&lock);
    tt_sem_acquire(&lock, 100);
    // destroy
    tt_sem_destroy(&lock);

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_thread_t *test_threads[10];
static tt_sem_t sem;
static int cnt;

static tt_result_t test_routine_1(IN tt_thread_t *thread, IN void *param)
{
    tt_ptrdiff_t idx = (tt_ptrdiff_t)param;
    int i = 0;
    tt_result_t ret;

    // TT_ASSERT(thread == test_threads[idx]);

    for (i = 0; i < 10000; ++i) {
        ret = tt_sem_acquire(&sem, 10);
        if (ret == TT_TIME_OUT) {
            --i;
            continue;
        }

        ++cnt;
        tt_sem_release(&sem);
    }

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_sem_mt)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_ptrdiff_t i;
    tt_sem_attr_t attr;

    TT_TEST_CASE_ENTER()
    // test start

    tt_memset(&attr, 0, sizeof(tt_sem_attr_t));

    tt_sem_create(&sem, 1, &attr);
    cnt = 0;

    for (i = 0; i < sizeof(test_threads) / sizeof(tt_thread_t *); ++i) {
        test_threads[i] =
            tt_thread_create(NULL, test_routine_1, (void *)i, NULL);
    }

    for (i = 0; i < sizeof(test_threads) / sizeof(tt_thread_t *); ++i) {
        tt_thread_wait(test_threads[i]);
    }

    tt_sem_destroy(&sem);

    TT_TEST_CHECK_EQUAL(cnt,
                        10000 * sizeof(test_threads) / sizeof(tt_thread_t *),
                        "");

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_thread_t *test_consumer_thread;

#define __PRODUCER_CAP 10000

static tt_result_t test_producer(IN tt_thread_t *thread, IN void *param)
{
    tt_ptrdiff_t idx = (tt_ptrdiff_t)param;
    int i = 0;

    // TT_ASSERT(thread == test_threads[idx]);

    for (i = 0; i < __PRODUCER_CAP; ++i) {
        // produce
        tt_sem_release(&sem);
    }

    return TT_SUCCESS;
}

static tt_result_t test_consumer(IN tt_thread_t *thread, IN void *param)
{
    int i = 0;
    tt_result_t ret;

    while (i < __PRODUCER_CAP * sizeof(test_threads) / sizeof(tt_thread_t *)) {
        // produce
        ret = tt_sem_acquire(&sem, 10);
        if (TT_OK(ret)) {
            ++i;
        }
    }

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_sem_pc)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_ptrdiff_t i;
    tt_sem_attr_t attr;
    tt_thread_attr_t tattr;

    TT_TEST_CASE_ENTER()
    // test start

    tt_memset(&attr, 0, sizeof(tt_sem_attr_t));

    tt_sem_create(&sem, 1, &attr);
    cnt = 0;

    // start producer
    tt_thread_attr_default(&tattr);
    tattr.detached = TT_TRUE;
    for (i = 0; i < sizeof(test_threads) / sizeof(tt_thread_t *); ++i) {
        test_threads[i] =
            tt_thread_create(NULL, test_producer, (void *)i, &tattr);
        TT_INFO("%d thread created", i);
    }

    // start consumer
    test_consumer_thread = tt_thread_create(NULL, test_consumer, NULL, NULL);
    tt_thread_wait(test_consumer_thread);

    tt_sem_destroy(&sem);

    TT_RECORD_INFO("%d", 0); // remove warnings

    // test end
    TT_TEST_CASE_LEAVE()
}
