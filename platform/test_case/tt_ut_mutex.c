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
#include <os/tt_mutex.h>
#include <os/tt_thread.h>

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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_mutex_basic)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_mutex_mt)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(mutex_case)

TT_TEST_CASE("tt_unit_test_mutex_basic",
             "testing basic mutex API",
             tt_unit_test_mutex_basic,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_mutex_mt",
                 "testing mutex API in multithread",
                 tt_unit_test_mutex_mt,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(mutex_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(TEST_UNIT_MUTEX, 0, mutex_case)

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

    TT_TEST_ROUTINE_DEFINE(tt_unit_test_mutex_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_mutex_t lock;
    tt_result_t ret = TT_FAIL;

    TT_TEST_CASE_ENTER()
    // test start

    ret = tt_mutex_create(&lock, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // lock
    tt_mutex_acquire(&lock);

    // trylock
    ret = tt_mutex_try_acquire(&lock);
    // TT_TEST_CHECK_EQUAL(ret, TT_TIME_OUT, "");

    // unlock
    tt_mutex_release(&lock);

    // trylock
    ret = tt_mutex_try_acquire(&lock);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // unlock
    tt_mutex_release(&lock);

    // unlock, assertion
    // tt_mutex_release(&lock);

    tt_mutex_destroy(&lock);

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_thread_t *test_threads[10];
static tt_mutex_t mutex;
static int cnt;

static tt_result_t test_routine_1(IN void *param)
{
    tt_ptrdiff_t idx = (tt_ptrdiff_t)param;
    int i = 0;

    // TT_ASSERT(thread == test_threads[idx]);

    for (i = 0; i < 10000; ++i) {
        tt_mutex_acquire(&mutex);
        ++cnt;
        tt_mutex_release(&mutex);
    }

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_mutex_mt)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_ptrdiff_t i;

    TT_TEST_CASE_ENTER()
    // test start

    tt_mutex_create(&mutex, NULL);
    cnt = 0;

    for (i = 0; i < sizeof(test_threads) / sizeof(tt_thread_t *); ++i) {
        test_threads[i] = tt_thread_create(test_routine_1, (void *)i, NULL);
    }

    for (i = 0; i < sizeof(test_threads) / sizeof(tt_thread_t *); ++i) {
        tt_thread_wait(test_threads[i]);
    }

    tt_mutex_destroy(&mutex);

    TT_TEST_CHECK_EQUAL(cnt,
                        10000 * sizeof(test_threads) / sizeof(tt_thread_t *),
                        "");

    // test end
    TT_TEST_CASE_LEAVE()
}
