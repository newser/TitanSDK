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
#include <memory/tt_memory_alloc.h>
#include <os/tt_mutex.h>
#include <os/tt_rwlock.h>
#include <os/tt_semaphore.h>
#include <os/tt_spinlock.h>
#include <os/tt_thread.h>
#include <timer/tt_time_reference.h>

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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_rwlock_basic)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_rwlock_mt)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(rwlock_case)

TT_TEST_CASE("tt_unit_test_rwlock_basic",
             "testing basic rwlock API",
             tt_unit_test_rwlock_basic,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_rwlock_mt",
                 "testing rwlock API in multithread",
                 tt_unit_test_rwlock_mt,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(rwlock_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(TEST_UNIT_RWLOCK, 0, rwlock_case)

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

    TT_TEST_ROUTINE_DEFINE(tt_unit_test_rwlock_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_rwlock_t lock;
    tt_result_t ret;

    TT_TEST_CASE_ENTER()
    // test start

    // create
    ret = tt_rwlock_create(&lock, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // read acquire
    tt_rwlock_acquire_r(&lock);
    // read acquire
    tt_rwlock_acquire_r(&lock);

    // try read acquire
    ret = tt_rwlock_try_acquire_r(&lock);

    // try write acquire
    ret = tt_rwlock_try_acquire_w(&lock);
    TT_TEST_CHECK_EQUAL(ret, TT_TIME_OUT, "");

    // read release
    tt_rwlock_release_r(&lock);
    // read release
    tt_rwlock_release_r(&lock);

    // try write acquire
    ret = tt_rwlock_try_acquire_w(&lock);
    TT_TEST_CHECK_EQUAL(ret, TT_TIME_OUT, "");

    // read release
    tt_rwlock_release_r(&lock);

    // write acquire
    tt_rwlock_acquire_w(&lock);

    // try read acquire
    // ret= tt_rwlock_try_acquire_r(&lock);
    // TT_TEST_CHECK_EQUAL(ret, TT_TIME_OUT, "");

    // try write acquire
    // ret= tt_rwlock_try_acquire_w(&lock);
    // TT_TEST_CHECK_EQUAL(ret, TT_TIME_OUT, "");

    // write release
    tt_rwlock_release_w(&lock);

    // destroy
    tt_rwlock_destroy(&lock);

    // test end
    TT_TEST_CASE_LEAVE()
}

#define ACT_NUM 10000
#define THREAD_NUM 16
#define SIM_WRITING(k, n) while (k++ < n)
//#define SIM_WRITING(k, n) do {}while(0)

static tt_thread_t *test_threads[THREAD_NUM];

static tt_rwlock_t rwlock;
static tt_u8_t action[THREAD_NUM][ACT_NUM];
static tt_u32_t write_counter;

// static LONG _comp_lock;

static tt_result_t test_routine_1(IN void *param)
{
    tt_ptrdiff_t idx = (tt_ptrdiff_t)param;
    int i = 0;
    tt_u8_t *this_act;

    // TT_ASSERT(thread == test_threads[idx]);
    this_act = action[idx];

    for (i = 0; i < ACT_NUM; ++i) {
        if (this_act[i] == 0) {
            tt_u32_t tmp;
            tt_rwlock_acquire_r(&rwlock);
            tmp = write_counter;
            tmp++;
            tt_rwlock_release_r(&rwlock);
        } else {
            int k = 0;
            int n = (rand() % 5) * 1000;
            tt_rwlock_acquire_w(&rwlock);
            ++write_counter;
            SIM_WRITING(k, n); // simulate writing
            tt_rwlock_release_w(&rwlock);
        }
    }

    return TT_SUCCESS;
}

#ifdef _WIN32
static CRITICAL_SECTION __cs1;

static tt_result_t test_routine_2(IN tt_thread_t *thread, IN void *param)
{
    tt_ptrdiff_t idx = (tt_ptrdiff_t)param;
    int i = 0;
    tt_u8_t *this_act;

    // TT_ASSERT(thread == &test_threads[idx]);
    this_act = action[idx];

    for (i = 0; i < ACT_NUM; ++i) {
        if (this_act[i] == 0) {
            tt_u32_t tmp;
            EnterCriticalSection(&__cs1);
            tmp = write_counter;
            LeaveCriticalSection(&__cs1);
        } else {
            int k = 0;
            int n = (rand() % 5) * 1000;
            EnterCriticalSection(&__cs1);
            ++write_counter;
            SIM_WRITING(k, n); // simulate writing
            LeaveCriticalSection(&__cs1);
        }
    }

    return TT_SUCCESS;
}
#endif

static tt_mutex_t __mutex1;

static tt_result_t test_routine_3(IN void *param)
{
    tt_ptrdiff_t idx = (tt_ptrdiff_t)param;
    int i = 0;
    tt_u8_t *this_act;

    // TT_ASSERT(thread == test_threads[idx]);
    this_act = action[idx];

    for (i = 0; i < ACT_NUM; ++i) {
        if (this_act[i] == 0) {
            tt_u32_t tmp;
            tt_mutex_acquire(&__mutex1);
            tmp = write_counter;
            ++tmp;
            tt_mutex_release(&__mutex1);
        } else {
            int k = 0;
            int n = (rand() % 5) * 1000;
            tt_mutex_acquire(&__mutex1);
            ++write_counter;
            SIM_WRITING(k, n); // simulate writing
            tt_mutex_release(&__mutex1);
        }
    }

    return TT_SUCCESS;
}

static tt_spinlock_t __spinlock;

static tt_result_t test_routine_4(IN void *param)
{
    tt_ptrdiff_t idx = (tt_ptrdiff_t)param;
    int i = 0;
    tt_u8_t *this_act;

    // TT_ASSERT(thread == test_threads[idx]);
    this_act = action[idx];

    for (i = 0; i < ACT_NUM; ++i) {
        if (this_act[i] == 0) {
            tt_u32_t tmp;
            tt_spinlock_acquire(&__spinlock);
            tmp = write_counter;
            ++tmp;
            tt_spinlock_release(&__spinlock);
        } else {
            int k = 0;
            int n = (rand() % 5) * 1000;
            tt_spinlock_acquire(&__spinlock);
            ++write_counter;
            SIM_WRITING(k, n); // simulate writing
            tt_spinlock_release(&__spinlock);
        }
    }

    return TT_SUCCESS;
}

static tt_sem_t __sem;

static tt_result_t test_routine_5(IN void *param)
{
    tt_ptrdiff_t idx = (tt_ptrdiff_t)param;
    int i = 0;
    tt_u8_t *this_act;

    // TT_ASSERT(thread == test_threads[idx]);
    this_act = action[idx];

    for (i = 0; i < ACT_NUM; ++i) {
        if (this_act[i] == 0) {
            tt_u32_t tmp;
            tt_sem_acquire(&__sem, TT_TIME_INFINITE);
            tmp = write_counter;
            ++tmp;
            tt_sem_release(&__sem);
        } else {
            int k = 0;
            int n = (rand() % 5) * 1000;
            tt_sem_acquire(&__sem, TT_TIME_INFINITE);
            ++write_counter;
            SIM_WRITING(k, n); // simulate writing
            tt_sem_release(&__sem);
        }
    }

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_rwlock_mt)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_ptrdiff_t i = 0, j = 0;
    tt_u32_t write_num = 0;

    tt_s64_t begin, end;
    tt_u32_t t1, t2, t3, t4, t5;

    tt_sem_attr_t sem_attr = {0};

    TT_TEST_CASE_ENTER()
    // test start

    tt_rwlock_create(&rwlock, NULL);
#ifdef _WIN32
    InitializeCriticalSectionAndSpinCount(&__cs1, 4000);
#endif
    tt_mutex_create(&__mutex1, NULL);
    tt_spinlock_create(&__spinlock, NULL);

    tt_sem_create(&__sem, 1, &sem_attr);

    srand((unsigned int)time(NULL));
    for (i = 0; i < sizeof(test_threads) / sizeof(tt_thread_t *); ++i) {
        for (j = 0; j < ACT_NUM; ++j) {
            int tmp = rand() % 100;
            if (tmp >= 50) {
                action[i][j] = 0; // 85% read
            } else {
                action[i][j] = 1; // 15% write
                ++write_num;
            }
        }
    }

    begin = tt_time_ref();
    for (i = 0; i < sizeof(test_threads) / sizeof(tt_thread_t *); ++i) {
        test_threads[i] = tt_thread_create(test_routine_1, (void *)i, NULL);
    }
    for (i = 0; i < sizeof(test_threads) / sizeof(tt_thread_t *); ++i) {
        tt_thread_wait(test_threads[i]);
    }
    end = tt_time_ref();
    t1 = (tt_u32_t)tt_time_ref2ms(end - begin);

    TT_TEST_CHECK_EQUAL(write_num, write_counter, "");

#ifdef _WIN32
    // compare with cs with spin
    begin = tt_time_ref();
    for (i = 0; i < sizeof(test_threads) / sizeof(tt_thread_t *); ++i) {
        test_threads[i] =
            tt_thread_create(NULL, test_routine_2, (void *)i, NULL);
    }
    for (i = 0; i < sizeof(test_threads) / sizeof(tt_thread_t *); ++i) {
        tt_thread_wait(test_threads[i]);
    }
    end = tt_time_ref();
    t2 = (tt_u32_t)tt_time_ref2ms(end - begin);
#else
    t2 = 0;
#endif

    // compare with mutex
    begin = tt_time_ref();
    for (i = 0; i < sizeof(test_threads) / sizeof(tt_thread_t *); ++i) {
        test_threads[i] = tt_thread_create(test_routine_3, (void *)i, NULL);
    }
    for (i = 0; i < sizeof(test_threads) / sizeof(tt_thread_t *); ++i) {
        tt_thread_wait(test_threads[i]);
    }
    end = tt_time_ref();
    t3 = (tt_u32_t)tt_time_ref2ms(end - begin);

    // compare with spin lock
    begin = tt_time_ref();
    for (i = 0; i < sizeof(test_threads) / sizeof(tt_thread_t *); ++i) {
        test_threads[i] = tt_thread_create(test_routine_4, (void *)i, NULL);
    }
    for (i = 0; i < sizeof(test_threads) / sizeof(tt_thread_t *); ++i) {
        tt_thread_wait(test_threads[i]);
    }
    end = tt_time_ref();
    t4 = (tt_u32_t)tt_time_ref2ms(end - begin);

    // compare with semaphore
    begin = tt_time_ref();
    for (i = 0; i < sizeof(test_threads) / sizeof(tt_thread_t *); ++i) {
        test_threads[i] = tt_thread_create(test_routine_5, (void *)i, NULL);
    }
    for (i = 0; i < sizeof(test_threads) / sizeof(tt_thread_t *); ++i) {
        tt_thread_wait(test_threads[i]);
    }
    end = tt_time_ref();
    t5 = (tt_u32_t)tt_time_ref2ms(end - begin);

    tt_rwlock_destroy(&rwlock);
#ifdef _WIN32
    DeleteCriticalSection(&__cs1);
#endif
    tt_mutex_destroy(&__mutex1);
    tt_spinlock_destroy(&__spinlock);
    tt_sem_destroy(&__sem);

    TT_INFO("rwlock: %d ms", t1);
    TT_INFO("critical section: %d ms", t2);
    TT_INFO("mutex: %d ms", t3);
    TT_INFO("spin lock: %d ms", t4);
    TT_INFO("semaphore: %d ms", t5);

    // test end
    TT_TEST_CASE_LEAVE()
}
