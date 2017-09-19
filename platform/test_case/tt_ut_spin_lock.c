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

#include <algorithm/tt_list.h>
#include <log/tt_log.h>
#include <os/tt_spinlock.h>
#include <os/tt_thread.h>
#include <time/tt_time_reference.h>

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
TT_TEST_ROUTINE_DECLARE(case_spin_lock_basic_mp)
TT_TEST_ROUTINE_DECLARE(case_spin_lock_basic_sp)
TT_TEST_ROUTINE_DECLARE(case_spin_lock_mt)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(spin_lock_case)

TT_TEST_CASE("case_spin_lock_basic_mp",
             "testing basic spin lock API for smp",
             case_spin_lock_basic_mp,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("case_spin_lock_basic_sp",
                 "basic spin lock API for single processor",
                 case_spin_lock_basic_sp,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_spin_lock_mt",
                 "testing spin lock API in multithread",
                 case_spin_lock_mt,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(spin_lock_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(TEST_UNIT_SPIN_LOCK, 0, spin_lock_case)

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

    TT_TEST_ROUTINE_DEFINE(case_spin_lock_basic_mp)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_spinlock_t lock;
    tt_result_t ret = TT_FAIL;
    tt_bool_t b_ret;

    TT_TEST_CASE_ENTER()
    // test start

    ret = tt_spinlock_create(&lock, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    // lock
    tt_spinlock_acquire(&lock);

    // trylock
    b_ret = tt_spinlock_try_acquire(&lock);
    TT_UT_EQUAL(b_ret, TT_FALSE, "");

    // unlock
    tt_spinlock_release(&lock);

    // trylock
    b_ret = tt_spinlock_try_acquire(&lock);
    TT_UT_EQUAL(ret, TT_TRUE, "");

    // unlock
    tt_spinlock_release(&lock);

    // unlock, assertion
    // tt_spinlock_release(&lock);

    tt_spinlock_destroy(&lock);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_spin_lock_basic_sp)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_spinlock_t lock;
    tt_result_t ret = TT_FAIL;
    tt_bool_t b_ret;

    // tt_u32_t real_cpu_num = tt_g_cpu_num;

    TT_TEST_CASE_ENTER()
    // test start

    // test spinlock on single processor platform
    //*((tt_u32_t*)&tt_g_cpu_num) = 1;

    ret = tt_spinlock_create(&lock, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    // lock
    tt_spinlock_acquire(&lock);

    // trylock
    b_ret = tt_spinlock_try_acquire(&lock);
    TT_UT_EQUAL(b_ret, TT_FALSE, "");

    // unlock
    tt_spinlock_release(&lock);

    // trylock
    b_ret = tt_spinlock_try_acquire(&lock);
    TT_UT_EQUAL(ret, TT_TRUE, "");

    // unlock
    tt_spinlock_release(&lock);

    // unlock, assertion
    // tt_spinlock_release(&lock);

    tt_spinlock_destroy(&lock);

    // restore cpu number
    //*((tt_u32_t*)&tt_g_cpu_num) = real_cpu_num;

    // test end
    TT_TEST_CASE_LEAVE()
}

#ifdef __UT_LITE__
static tt_thread_t *test_threads[10];
#else
static tt_thread_t *test_threads[100];
#endif

static tt_spinlock_t slock;
static int cnt;

#ifdef __UT_LITE__
#define __ACT_NUM 100
#else
#define __ACT_NUM 10000
#endif
unsigned char act[sizeof(test_threads) / sizeof(tt_thread_t *)][__ACT_NUM];

tt_list_t __test_list;

static tt_result_t test_routine_1(IN void *param)
{
    tt_ptrdiff_t idx = (tt_ptrdiff_t)param;
    int i = 0;
    // unsigned char *thread_act = act[idx];

    tt_lnode_t thread_node;

    // TT_ASSERT(thread == test_threads[idx]);

    tt_lnode_init(&thread_node);

    for (i = 0; i < __ACT_NUM; ++i) {
        int k = 0, k_end = /*thread_act[i]*/ 5 * 1000;

        tt_spinlock_acquire(&slock);
        ++cnt;

        if (thread_node.lst == NULL) {
            tt_list_push_head(&__test_list, &thread_node);
        } else {
            tt_list_remove(&thread_node);
        }

        tt_spinlock_release(&slock);

        // simulating processing
        while (k++ < k_end)
            ;
    }

    tt_spinlock_acquire(&slock);
    if (thread_node.lst != NULL) {
        tt_list_remove(&thread_node);
    }
    tt_spinlock_release(&slock);

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(case_spin_lock_mt)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_ptrdiff_t i;
    int j;
    int exp_cnt;

    tt_s64_t start_time, end_time;

    // tt_u32_t real_cpu_num = tt_g_cpu_num;

    TT_TEST_CASE_ENTER()
// test start

#if 0
    // test spinlock on smp platform
    *((tt_u32_t*)&tt_g_cpu_num) = 2;

    tt_spinlock_create(&slock, NULL);
    cnt = 0;
    
    for (i = 0; i < sizeof(test_threads)/sizeof(tt_thread_t*); ++i)
    {
        tt_thread_create(&test_threads[i], NULL, 
                         test_routine_1, (void*)i, NULL);
    }

    for (i = 0; i < sizeof(test_threads)/sizeof(tt_thread_t*); ++i)
    {
        tt_thread_wait(&test_threads[i]);
    }    

    tt_spinlock_destroy(&slock);

    // restore cpu number
    *((tt_u32_t*)&tt_g_cpu_num) = real_cpu_num;

    // test spinlock on single processor platform
    *((tt_u32_t*)&tt_g_cpu_num) = 1;
#endif

    tt_spinlock_create(&slock, NULL);
    cnt = 0;
    tt_list_init(&__test_list);

    exp_cnt = 0;
    srand((unsigned int)time(NULL));
    for (i = 0; i < sizeof(test_threads) / sizeof(tt_thread_t *); ++i) {
        for (j = 0; j < __ACT_NUM; ++j) {
            act[i][j] = (unsigned char)rand() % 10;

            ++exp_cnt;
        }
    }

    start_time = tt_time_ref();
    for (i = 0; i < sizeof(test_threads) / sizeof(tt_thread_t *); ++i) {
        test_threads[i] = tt_thread_create(test_routine_1, (void *)i, NULL);
    }

    for (i = 0; i < sizeof(test_threads) / sizeof(tt_thread_t *); ++i) {
        tt_thread_wait(test_threads[i]);
    }
    end_time = tt_time_ref();
    TT_INFO("time consumed: %d ms",
            (tt_u32_t)tt_time_ref2ms(end_time - start_time));

    tt_spinlock_destroy(&slock);

    // restore cpu number
    //*((tt_u32_t*)&tt_g_cpu_num) = real_cpu_num;

    TT_UT_EQUAL(cnt, exp_cnt, "");

    // test end
    TT_TEST_CASE_LEAVE()
}
