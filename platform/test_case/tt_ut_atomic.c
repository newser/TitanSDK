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

#include <log/tt_log.h>
#include <os/tt_atomic.h>
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
TT_TEST_ROUTINE_DECLARE(case_atomic_basic)
TT_TEST_ROUTINE_DECLARE(case_atomic_mt)
TT_TEST_ROUTINE_DECLARE(case_atomic_cas)
TT_TEST_ROUTINE_DECLARE(case_atomic_ptr)
TT_TEST_ROUTINE_DECLARE(case_atomic_fence)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(atomic_case)

TT_TEST_CASE("case_atomic_basic", "testing basic atomic API", case_atomic_basic,
             NULL, NULL, NULL, NULL, NULL)
,

    TT_TEST_CASE("case_atomic_mt", "testing atomic API in multithread",
                 case_atomic_mt, NULL, NULL, NULL, NULL, NULL),

    TT_TEST_CASE("case_atomic_cas", "testing cas API in multithread",
                 case_atomic_cas, NULL, NULL, NULL, NULL, NULL),

    TT_TEST_CASE("case_atomic_ptr", "testing atomic ptr API", case_atomic_ptr,
                 NULL, NULL, NULL, NULL, NULL),

    TT_TEST_CASE("case_atomic_fence", "testing atomic fence", case_atomic_fence,
                 NULL, NULL, NULL, NULL, NULL),

    TT_TEST_CASE_LIST_DEFINE_END(atomic_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(TEST_UNIT_ATOMIC, 0, atomic_case)

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

    TT_TEST_ROUTINE_DEFINE(case_atomic_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_atomic_s32_t v32;
    tt_s32_t ret32;
    tt_atomic_s64_t v64;
    tt_s64_t ret64;
    tt_bool_t b_ret;

    TT_TEST_CASE_ENTER()
    // test start

    // create
    tt_atomic_s32_set(&v32, 0xefffffff);

    // inc
    ret32 = tt_atomic_s32_inc(&v32);
    TT_UT_EQUAL(ret32, 0xf0000000, "");

    // read
    ret32 = tt_atomic_s32_get(&v32);
    TT_UT_EQUAL(ret32, 0xf0000000, "");

    // swap
    ret32 = tt_atomic_s32_swap(&v32, 0);
    TT_UT_EQUAL(ret32, 0xf0000000, "");

    // dec
    ret32 = tt_atomic_s32_dec(&v32);
    TT_UT_EQUAL(ret32, -1, "");

    // read
    ret32 = tt_atomic_s32_get(&v32);
    TT_UT_EQUAL(ret32, -1, "");

    // write
    tt_atomic_s32_set(&v32, 0x12345679);
    ret32 = tt_atomic_s32_get(&v32);
    TT_UT_EQUAL(ret32, 0x12345679, "");

    // exchange
    tt_atomic_s32_set(&v32, 0xF2345679);
    ret32 = tt_atomic_s32_get(&v32);
    TT_UT_EQUAL(ret32, 0xF2345679, "");

    ret32 = tt_atomic_s32_swap(&v32, 0);
    TT_UT_EQUAL(ret32, 0xF2345679, "");
    ret32 = tt_atomic_s32_get(&v32);
    TT_UT_EQUAL(ret32, 0, "");

    tt_atomic_s32_set(&v32, 0x12345679);
    ret32 = tt_atomic_s32_get(&v32);
    TT_UT_EQUAL(ret32, 0x12345679, "");

    // cmp exchange
    v32 = 0x12345679;
    b_ret = tt_atomic_s32_cas(&v32, 0x12345679, 0x12345689);
    TT_UT_EQUAL(b_ret, TT_TRUE, "");
    ret32 = tt_atomic_s32_get(&v32);
    TT_UT_EQUAL(ret32, 0x12345689, "");

    // exchange fail
    tt_atomic_s32_set(&v32, 0x12345679);

    b_ret = tt_atomic_s32_cas(&v32, 0x1, 0x2);
    TT_UT_EQUAL(b_ret, TT_FALSE, "");
    ret32 = tt_atomic_s32_get(&v32);
    TT_UT_EQUAL(ret32, 0x12345679, "");

    // inc64
    tt_atomic_s64_set(&v64, 0x12345678ffffffffLL);

    ret64 = tt_atomic_s64_inc(&v64);
    TT_UT_EQUAL(ret64, 0x1234567900000000LL, "");
    ret64 = tt_atomic_s64_get(&v64);
    TT_UT_EQUAL(ret64, 0x1234567900000000LL, "");

    // dec64
    tt_atomic_s64_set(&v64, 0x1234567900000000LL);
    ret64 = tt_atomic_s64_get(&v64);
    TT_UT_EQUAL(ret64, 0x1234567900000000LL, "");

    // dec
    ret64 = tt_atomic_s64_dec(&v64);
    TT_UT_EQUAL(ret64, 0x12345678ffffffffLL, "");
    ret64 = tt_atomic_s64_get(&v64);
    TT_UT_EQUAL(ret64, 0x12345678ffffffffLL, "");

    // write
    tt_atomic_s64_set(&v64, 0x1234567900000000LL);
    ret64 = tt_atomic_s64_get(&v64);
    TT_UT_EQUAL(ret64, 0x1234567900000000LL, "");

    // exchange
    tt_atomic_s64_set(&v64, 0xFFFFFFFFFFFFFFFFLL);
    ret64 = tt_atomic_s64_get(&v64);
    TT_UT_EQUAL(ret64, 0xFFFFFFFFFFFFFFFFLL, "");

    ret64 = tt_atomic_s64_swap(&v64, 0);
    TT_UT_EQUAL(ret64, 0xFFFFFFFFFFFFFFFFLL, "");
    ret64 = tt_atomic_s64_get(&v64);
    TT_UT_EQUAL(ret64, 0, "");

    ret64 = tt_atomic_s64_swap(&v64, 0x1234567900000000LL);
    TT_UT_EQUAL(ret64, 0, "");
    ret64 = tt_atomic_s64_get(&v64);
    TT_UT_EQUAL(ret64, 0x1234567900000000LL, "");

    // cmp exchange
    b_ret = tt_atomic_s64_cas(&v64, 0x1234567900000000LL, 0x1234568900000001LL);
    TT_UT_EQUAL(b_ret, TT_TRUE, "");
    ret64 = tt_atomic_s64_get(&v64);
    TT_UT_EQUAL(ret64, 0x1234568900000001LL, "");

    // exchange fail
    tt_atomic_s64_set(&v64, 0x1234567900000000LL);
    b_ret = tt_atomic_s64_cas(&v64, 0x1, 0x2);
    TT_UT_EQUAL(b_ret, TT_FALSE, "");
    ret64 = tt_atomic_s64_get(&v64);
    TT_UT_EQUAL(ret64, 0x1234567900000000LL, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_thread_t *test_threads[100];
static tt_atomic_s32_t test_counter;
static tt_atomic_s64_t test_counter_2;

#ifdef __UT_LITE__
#define INC_NUM 100
#define DEC_NUM 50
#else
#define INC_NUM 10000
#define DEC_NUM 5000
#endif

static tt_result_t test_routine_1(IN void *param)
{
    tt_ptrdiff_t idx = (tt_ptrdiff_t)param;
    int i = 0;

    // TT_ASSERT(thread == test_threads[idx]);

    for (i = 0; i < INC_NUM; ++i) { tt_atomic_s32_inc(&test_counter); }
    for (i = 0; i < DEC_NUM; ++i) { tt_atomic_s32_dec(&test_counter); }

    for (i = 0; i < INC_NUM; ++i) { tt_atomic_s64_inc(&test_counter_2); }
    for (i = 0; i < DEC_NUM; ++i) { tt_atomic_s64_dec(&test_counter_2); }

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(case_atomic_mt)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_ptrdiff_t i = 0;
    tt_u32_t ret32;
    tt_u64_t ret64;

    TT_TEST_CASE_ENTER()
    // test start

    tt_atomic_s32_set(&test_counter, 0);
    tt_atomic_s64_set(&test_counter_2, 0);

    for (i = 0; i < sizeof(test_threads) / sizeof(tt_thread_t *); ++i) {
        test_threads[i] = tt_thread_create(test_routine_1, (void *)i, NULL);
    }

    for (i = 0; i < sizeof(test_threads) / sizeof(tt_thread_t *); ++i) {
        tt_thread_wait(test_threads[i]);
    }

    ret32 = tt_atomic_s32_get(&test_counter);
    TT_UT_EQUAL(ret32,
                (INC_NUM - DEC_NUM) * sizeof(test_threads) /
                    sizeof(tt_thread_t *),
                "");

    ret64 = tt_atomic_s64_get(&test_counter_2);
    TT_UT_EQUAL(ret64,
                (INC_NUM - DEC_NUM) * sizeof(test_threads) /
                    sizeof(tt_thread_t *),
                "");

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_u32_t test_v1 = 0;
static tt_u32_t test_v1_64 = 0;
static const int loop_num = 1000;

static tt_result_t test_routine_2(IN void *param)
{
    tt_ptrdiff_t idx = (tt_ptrdiff_t)param;
    int i;

    // TT_ASSERT(thread == test_threads[idx]);

    for (i = 0; i < loop_num; ++i) {
        while (!TT_OK(tt_atomic_s32_cas(&test_counter, 0, 1)))
            ;
        ++test_v1;
        TT_ASSERT_ALWAYS(tt_atomic_s32_cas(&test_counter, 1, 0));
    }

    for (i = 0; i < loop_num; ++i) {
        while (!TT_OK(tt_atomic_s64_cas(&test_counter_2, 0x12345678ababababLL,
                                        0xabababab12345678LL)))
            ;
        ++test_v1_64;
        TT_ASSERT_ALWAYS(tt_atomic_s64_cas(&test_counter_2,
                                           0xabababab12345678LL,
                                           0x12345678ababababLL));
    }

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(case_atomic_cas)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_ptrdiff_t i = 0;
    tt_u32_t ret32;
    tt_u64_t ret64;

    TT_TEST_CASE_ENTER()
    // test start

    tt_atomic_s32_set(&test_counter, 0);
    tt_atomic_s64_set(&test_counter_2, 0x12345678ababababLL);

    for (i = 0; i < sizeof(test_threads) / sizeof(tt_thread_t *); ++i) {
        test_threads[i] = tt_thread_create(test_routine_2, (void *)i, NULL);
    }

    for (i = 0; i < sizeof(test_threads) / sizeof(tt_thread_t *); ++i) {
        tt_thread_wait(test_threads[i]);
    }

    ret32 = tt_atomic_s32_get(&test_counter);
    TT_UT_EQUAL(ret32, 0, "");

    ret64 = tt_atomic_s64_get(&test_counter_2);
    TT_UT_EQUAL(ret64, 0x12345678ababababLL, "");

    TT_UT_EQUAL(test_v1,
                (sizeof(test_threads) / sizeof(tt_thread_t *)) * loop_num, "");
    TT_UT_EQUAL(test_v1_64,
                (sizeof(test_threads) / sizeof(tt_thread_t *)) * loop_num, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

// static tt_ptr_t test_ptr;

typedef struct _test_node_t
{
    tt_ptr_t next;
} test_node_t;
static test_node_t *test_head;
static test_node_t test_node[sizeof(test_threads) / sizeof(tt_thread_t *)]
                            [1000];

static tt_result_t test_routine_ptr_xchg(IN void *param)
{
    tt_ptrdiff_t idx = (tt_ptrdiff_t)param;
    int i;

    // TT_ASSERT(thread == test_threads[idx]);

    for (i = 0; i < sizeof(test_node[0]) / sizeof(tt_ptr_t); ++i) {
        test_node_t *p = &test_node[idx][i];
        tt_ptr_t next = NULL;

        next = tt_atomic_ptr_swap((tt_ptr_t *)&test_head, p);
        p->next = next;
    }

    return TT_SUCCESS;
}

static tt_result_t test_routine_ptr_cas(IN void *param)
{
    tt_ptrdiff_t idx = (tt_ptrdiff_t)param;
    int i;

    // TT_ASSERT(thread == test_threads[idx]);

    for (i = 0; i < sizeof(test_node[0]) / sizeof(tt_ptr_t); ++i) {
        test_node_t *p = &test_node[idx][i];
        test_node_t *next = test_head;

        while (1) {
            next = test_head;
            if (!tt_atomic_ptr_cas((tt_ptr_t *)&test_head, next, p)) {
                continue;
            }

            p->next = next;
            break;
        }
    }

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(case_atomic_ptr)
{
    tt_ptr_t ptr;
    tt_ptr_t ret_ptr;
    tt_ptrdiff_t i, v1, v2;
    tt_bool_t b_ret;

    TT_TEST_CASE_ENTER()
    // test start

    ptr = (tt_ptr_t)NULL;

    // exchange
    ret_ptr = tt_atomic_ptr_swap(&ptr, &v1);
    TT_UT_EQUAL(ptr, &v1, "");
    TT_UT_EQUAL(ret_ptr, NULL, "");

    tt_atomic_ptr_set(&ptr, &v2);
    ret_ptr = tt_atomic_ptr_get(&ptr);
    TT_UT_EQUAL(ptr, &v2, "");
    TT_UT_EQUAL(ret_ptr, &v2, "");

    ret_ptr = tt_atomic_ptr_swap(&ptr, NULL);
    TT_UT_EQUAL(ptr, NULL, "");
    TT_UT_EQUAL(ret_ptr, &v2, "");

    // cas
    ptr = (tt_ptr_t)NULL;

    b_ret = tt_atomic_ptr_cas(&ptr, NULL, &v1);
    TT_UT_EQUAL(ptr, &v1, "");
    TT_UT_EQUAL(b_ret, TT_TRUE, "");

    b_ret = tt_atomic_ptr_cas(&ptr, &v2, &v1);
    TT_UT_EQUAL(ptr, &v1, "");
    TT_UT_EQUAL(b_ret, TT_FALSE, "");

    b_ret = tt_atomic_ptr_cas(&ptr, &v1, &v2);
    TT_UT_EQUAL(ptr, &v2, "");
    TT_UT_EQUAL(b_ret, TT_TRUE, "");

    // xchg in multi thread
    test_head = NULL;

    for (i = 0; i < sizeof(test_threads) / sizeof(tt_thread_t *); ++i) {
        test_threads[i] =
            tt_thread_create(test_routine_ptr_xchg, (void *)i, NULL);
    }

    for (i = 0; i < sizeof(test_threads) / sizeof(tt_thread_t *); ++i) {
        tt_thread_wait(test_threads[i]);
    }

    i = 0;
    while (test_head != NULL) {
        test_head = test_head->next;
        ++i;
    }
    TT_UT_EQUAL(i, sizeof(test_node) / sizeof(test_node_t), "");

    // cas in multi thread
    test_head = NULL;
    tt_memset(test_node, 0, sizeof(test_node));

    for (i = 0; i < sizeof(test_threads) / sizeof(tt_thread_t *); ++i) {
        test_threads[i] =
            tt_thread_create(test_routine_ptr_cas, (void *)i, NULL);
    }

    for (i = 0; i < sizeof(test_threads) / sizeof(tt_thread_t *); ++i) {
        tt_thread_wait(test_threads[i]);
    }

    i = 0;
    while (test_head != NULL) {
        test_head = test_head->next;
        ++i;
    }
    TT_UT_EQUAL(i, sizeof(test_node) / sizeof(test_node_t), "");

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_result_t __ut_ret_fence;
static tt_atomic_s32_t __done, __done2, __tmp[5];

static tt_result_t test_routine_3_1(IN void *param)
{
    tt_atomic_s32_set(&__tmp[0], 0x123);
    tt_atomic_s32_set(&__tmp[1], 0x4556);
    tt_atomic_s32_set(&__tmp[2], 0x1);
    tt_atomic_s32_set(&__tmp[3], 0x9999999);
    tt_atomic_s32_set(&__tmp[4], 0xabcd);

    tt_atomic_s32_set(&__done, 1);

    return TT_SUCCESS;
}

static tt_result_t test_routine_3_2(IN void *param)
{
    while (tt_atomic_s32_get(&__done) != 1) {}
    tt_atomic_s32_set(&__done2, 1);

    return TT_SUCCESS;
}

static tt_result_t test_routine_3_3(IN void *param)
{
    while (tt_atomic_s32_get(&__done2) != 1) {}

    if (tt_atomic_s32_get(&__tmp[0]) != 0x123) {
        __ut_ret_fence = TT_FAIL;
        return TT_FAIL;
    }
    if (tt_atomic_s32_get(&__tmp[1]) != 0x4556) {
        __ut_ret_fence = TT_FAIL;
        return TT_FAIL;
    }
    if (tt_atomic_s32_get(&__tmp[2]) != 0x1) {
        __ut_ret_fence = TT_FAIL;
        return TT_FAIL;
    }
    if (tt_atomic_s32_get(&__tmp[3]) != 0x9999999) {
        __ut_ret_fence = TT_FAIL;
        return TT_FAIL;
    }
    if (tt_atomic_s32_get(&__tmp[4]) != 0xabcd) {
        __ut_ret_fence = TT_FAIL;
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

#ifdef __UT_LITE__
#define F_NUM 10
#else
#define F_NUM 100
#endif

TT_TEST_ROUTINE_DEFINE(case_atomic_fence)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_ptrdiff_t i = 0;

    TT_TEST_CASE_ENTER()
    // test start

    __ut_ret_fence = TT_SUCCESS;

    for (i = 0; i < F_NUM; ++i) {
        tt_atomic_s32_set(&__done, 0);

        test_threads[0] = tt_thread_create(test_routine_3_1, (void *)i, NULL);
        test_threads[1] = tt_thread_create(test_routine_3_2, (void *)i, NULL);
        test_threads[2] = tt_thread_create(test_routine_3_3, (void *)i, NULL);

        tt_thread_wait(test_threads[0]);
        tt_thread_wait(test_threads[1]);
        tt_thread_wait(test_threads[2]);
    }

    // test end
    TT_TEST_CASE_LEAVE()
}
