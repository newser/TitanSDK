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

/**
@file tt_unit_test.h
@brief unit test apis

unit test apis

*/

#ifndef __TT_UNIT_TEST__
#define __TT_UNIT_TEST__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <unit_test/tt_test_framework.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

// make case list name
#define TT_MAKE_TEST_CASE_LIST_NAME(name) tt_g_test_case_list_##name

// define test case list
#define TT_TEST_CASE_LIST_DEFINE_BEGIN(name)                                   \
    tt_test_case_t TT_MAKE_TEST_CASE_LIST_NAME(name)[] = {
#define TT_TEST_CASE_LIST_DEFINE_END(name)                                     \
    }                                                                          \
    ;

// define a case
#define TT_TEST_CASE(name,                                                     \
                     comment,                                                  \
                     routine,                                                  \
                     routine_param,                                            \
                     routine_enter,                                            \
                     enter_param,                                              \
                     routine_leave,                                            \
                     leave_param)                                              \
    {                                                                          \
        name, comment, routine, routine_param, routine_enter, enter_param,     \
            routine_leave, leave_param, TT_CASE_SANITY                         \
    }

#define TT_TEST_CASE_ADVANCED(name,                                            \
                              comment,                                         \
                              routine,                                         \
                              routine_param,                                   \
                              routine_enter,                                   \
                              enter_param,                                     \
                              routine_leave,                                   \
                              leave_param)                                     \
    {                                                                          \
        name, comment, routine, routine_param, routine_enter, enter_param,     \
            routine_leave, leave_param, TT_CASE_ADVANCED                       \
    }

#define TT_MAKE_TEST_UNIT_NAME(name) tt_g_test_unit_##name

// define test unit
#define TT_TEST_UNIT_DEFINE(name, attribute, case_list)                        \
    tt_test_unit_t TT_MAKE_TEST_UNIT_NAME(                                     \
        name) = {#name,                                                        \
                 attribute,                                                    \
                 TT_MAKE_TEST_CASE_LIST_NAME(case_list),                       \
                 sizeof(TT_MAKE_TEST_CASE_LIST_NAME(case_list)) /              \
                     sizeof(tt_test_case_t)};

#define TT_TEST_CASE_ENTER() tt_test_item_t *item = (tt_test_item_t *)test_item;

#define TT_TEST_CASE_LEAVE() return TT_SUCCESS;

#define TT_UT_EQUAL(a, b, info_if_not_match)                                   \
    do {                                                                       \
        if ((a) != (b)) {                                                      \
            item->test_result = TT_FAIL;                                       \
            TT_RECORD_ERROR((info_if_not_match));                              \
            return TT_FAIL;                                                    \
        }                                                                      \
    } while (0)

#define TT_UT_NOT_EQUAL(a, b, info_if_not_match)                               \
    do {                                                                       \
        if ((a) == (b)) {                                                      \
            item->test_result = TT_FAIL;                                       \
            TT_RECORD_ERROR((info_if_not_match));                              \
            return TT_FAIL;                                                    \
        }                                                                      \
    } while (0)

#define TT_UT_EXP(e, info_if_not_match)                                        \
    do {                                                                       \
        if (!(e)) {                                                            \
            item->test_result = TT_FAIL;                                       \
            TT_RECORD_ERROR((info_if_not_match));                              \
            return TT_FAIL;                                                    \
        }                                                                      \
    } while (0)

#define TT_UT_SUCCESS(a, info) TT_UT_EQUAL((a), TT_SUCCESS, (info))
#define TT_UT_FAIL(a, info) TT_UT_NOT_EQUAL((a), TT_SUCCESS, (info))

#define TT_UT_NULL(a, info) TT_UT_EQUAL((a), NULL, (info))
#define TT_UT_NOT_NULL(a, info) TT_UT_NOT_EQUAL((a), NULL, (info))

#define TT_UT_TRUE(a, info) TT_UT_EQUAL((a), TT_TRUE, (info))
#define TT_UT_FALSE(a, info) TT_UT_EQUAL((a), TT_FALSE, (info))

#define TT_UT_STREQ(a, b, info)                                                \
    TT_UT_EQUAL(tt_strcmp((char *)(a), (char *)(b)), 0, (info))
#define TT_UT_NSTREQ(a, b, n, info)                                            \
    TT_UT_EQUAL(tt_strncmp((char *)(a), (char *)(b), (n)), 0, (info))
#define TT_UT_MEMEQ(a, b, n, info)                                             \
    TT_UT_EQUAL(tt_memcmp((a), (b), (n)), 0, (info))

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef enum {
    TEST_UNIT_BEGIN = 0,

    TEST_UNIT_LOG = TEST_UNIT_BEGIN, // id for tt_trace
    TEST_UNIT_LOG_PATTERN,
    TEST_UNIT_ATOMIC,
    TEST_UNIT_LIST,
    TEST_UNIT_QUEUE,
    TEST_UNIT_SPIN_LOCK,
    TEST_UNIT_THREAD, // id for tt_thread
    TEST_UNIT_MUTEX, // id for ts mutex
    TEST_UNIT_SEM, // id for ts semaphore
    TEST_UNIT_RWLOCK, // id for ts rwlock
    TEST_UNIT_SLAB, // id for ts memory cache slab
    TEST_UNIT_MISC, // id for misc
    TEST_UNIT_MEMPOOL, // id for memory pool
    TEST_UNIT_TIME_REF,
    TEST_UNIT_HEAP,
    TEST_UNIT_VECTOR,
    TEST_UNIT_MAP,
    TEST_UNIT_FS,
    TEST_UNIT_TIMER,
    TEST_UNIT_SOCKET,
    TEST_UNIT_BUF,
    TEST_UNIT_IPC,
    TEST_UNIT_FIBER,
    TEST_UNIT_CHARSET,
    TEST_UNIT_CFGNODE,
    TEST_UNIT_CFGPATH,
    TEST_UNIT_CFGSHELL,
    TEST_UNIT_DATE,
    TEST_UNIT_FPATH,

    TEST_UNIT_NUM // number of test units
} tt_test_unit_id_t;

typedef enum {
    TT_CASE_SANITY,
    TT_CASE_ADVANCED,
} tt_ut_case_level_t;

typedef struct
{
    const tt_char_t *name;
    const tt_char_t *comment;

    tt_test_routine_t routine;
    void *param;
    tt_test_routine_enter_t routine_enter;
    void *enter_param;
    tt_test_routine_leave_t routine_leave;
    void *leave_param;
    tt_ut_case_level_t level;
} tt_test_case_t;

typedef struct
{
    // for test class
    const tt_char_t *name;
    tt_u32_t attribute;

    // for test item
    tt_test_case_t *case_list;
    tt_u32_t case_num;
} tt_test_unit_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

tt_export tt_test_unit_t *tt_g_test_unit_list[TEST_UNIT_NUM];

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_result_t tt_test_unit_init(IN tt_ptr_t reserved);

tt_export tt_result_t tt_test_unit_run(IN const tt_char_t *unit_name);

tt_export tt_result_t tt_test_unit_list(IN const tt_char_t *unit_name);

tt_export tt_result_t tt_test_unit_to_class(IN tt_test_unit_t *unit);

#endif /* __TT_UNIT_TEST__ */
