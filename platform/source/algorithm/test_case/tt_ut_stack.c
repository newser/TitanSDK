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

#include <unit_test/tt_unit_test.h>

#include <tt_platform.h>

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
TT_TEST_ROUTINE_DECLARE(case_stack)
TT_TEST_ROUTINE_DECLARE(case_ptr_stack)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(stack_case)

TT_TEST_CASE("case_stack", "testing stack", case_stack, NULL, NULL, NULL, NULL,
             NULL)
,

    TT_TEST_CASE("case_ptr_stack", "testing ptr stack", case_ptr_stack, NULL,
                 NULL, NULL, NULL, NULL),

    TT_TEST_CASE_LIST_DEFINE_END(stack_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(ALG_UT_STACK, 0, stack_case)

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

#define __q_size 100
#define __qf_size 10

        TT_TEST_ROUTINE_DEFINE(case_stack)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_u32_t v[__q_size] = {0};
    tt_u32_t i, n, val;
    tt_result_t ret;
    tt_stack_t q;
    tt_stack_attr_t attr;

    TT_TEST_CASE_ENTER()
    // test start

    for (i = 0; i < __q_size; ++i) { v[i] = i; }

    tt_stack_attr_default(&attr);
    attr.obj_per_frame = __qf_size;

    tt_stack_init(&q, sizeof(tt_u32_t), &attr);
    tt_stack_clear(&q);
    TT_UT_EQUAL(tt_stack_count(&q), 0, "");
    TT_UT_EQUAL(tt_stack_empty(&q), TT_TRUE, "");
    TT_UT_EQUAL(tt_stack_top(&q), NULL, "");

    ret = tt_stack_pop(&q, &i);
    TT_UT_FAIL(ret, "");

    {
        tt_stack_iter_t pos;
        tt_u32_t *pu32;

        tt_stack_iter(&q, &pos);
        i = 0;
        while ((pu32 = tt_stack_iter_next(&pos)) != NULL) {
            TT_UT_EQUAL(*pu32, v[i++], "");
        }
        TT_UT_EQUAL(i, 0, "");
    }

    for (i = 0; i < __q_size; ++i) {
        ret = tt_stack_push(&q, &v[i]);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(*(tt_u32_t *)tt_stack_top(&q), v[i], "");
    }
    TT_UT_EQUAL(tt_stack_count(&q), __q_size, "");

    {
        tt_stack_iter_t pos;
        tt_u32_t *pu32;

        tt_stack_iter(&q, &pos);
        i = 0;
        while ((pu32 = tt_stack_iter_next(&pos)) != NULL) {
            TT_UT_EQUAL(*pu32, v[i++], "");
        }
        TT_UT_EQUAL(i, __q_size, "");
    }

    n = tt_rand_u32() % __q_size;
    for (i = __q_size - 1; i >= (__q_size - n); --i) {
        ret = tt_stack_pop(&q, &val);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(val, v[i], "");
        TT_UT_EQUAL(*(tt_u32_t *)tt_stack_top(&q), v[i - 1], "");
    }
    TT_UT_EQUAL(tt_stack_count(&q), __q_size - n, "");

    for (i = __q_size - n; i < __q_size; ++i) {
        ret = tt_stack_push(&q, &v[i]);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(*(tt_u32_t *)tt_stack_top(&q), v[i], "");
    }
    TT_UT_EQUAL(tt_stack_count(&q), __q_size, "");

    for (i = __q_size - 1; i >= (__q_size - __qf_size); --i) {
        ret = tt_stack_pop(&q, &val);
        TT_UT_EQUAL(val, v[i], "");
    }
    TT_UT_EQUAL(tt_stack_count(&q), (__q_size - __qf_size), "");

    tt_stack_clear(&q);
    TT_UT_EQUAL(tt_stack_count(&q), 0, "");
    TT_UT_EQUAL(tt_stack_empty(&q), TT_TRUE, "");
    TT_UT_EQUAL(tt_stack_top(&q), NULL, "");

    ret = tt_stack_pop(&q, &i);
    TT_UT_FAIL(ret, "");

    tt_stack_destroy(&q);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_ptr_stack)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_u32_t v[__q_size] = {0};
    tt_u32_t i, n;
    tt_result_t ret;
    tt_ptrstk_t q;

    TT_TEST_CASE_ENTER()
    // test start

    for (i = 0; i < __q_size; ++i) { v[i] = i; }

    tt_ptrstk_init(&q, NULL);
    tt_ptrstk_clear(&q);
    TT_UT_EQUAL(tt_ptrstk_count(&q), 0, "");
    TT_UT_EQUAL(tt_ptrstk_empty(&q), TT_TRUE, "");
    TT_UT_EQUAL(tt_ptrstk_top(&q), NULL, "");

    TT_UT_EQUAL(tt_ptrstk_pop(&q), NULL, "");

    {
        tt_ptrstk_iter_t pos;
        tt_ptrstk_iter(&q, &pos);
        i = 0;
        while (tt_ptrstk_iter_next(&pos) != NULL) { ++i; }
        TT_UT_EQUAL(i, 0, "");
    }

    for (i = 0; i < __q_size; ++i) {
        ret = tt_ptrstk_push(&q, &v[i]);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_ptrstk_top(&q), &v[i], "");
    }
    TT_UT_EQUAL(tt_ptrstk_count(&q), __q_size, "");

    {
        tt_ptrstk_iter_t pos;
        tt_ptr_t p;

        tt_ptrstk_iter(&q, &pos);
        i = 0;
        while ((p = tt_ptrstk_iter_next(&pos)) != NULL) {
            TT_UT_EQUAL(p, &v[i++], "");
        }
        TT_UT_EQUAL(i, __q_size, "");
    }

    n = tt_rand_u32() % __q_size;
    for (i = __q_size - 1; i >= (__q_size - n); --i) {
        TT_UT_EQUAL(tt_ptrstk_pop(&q), &v[i], "");
        TT_UT_EQUAL(tt_ptrstk_top(&q), &v[i - 1], "");
    }
    TT_UT_EQUAL(tt_ptrstk_count(&q), __q_size - n, "");

    for (i = __q_size - n; i < __q_size; ++i) {
        ret = tt_ptrstk_push(&q, &v[i]);
        TT_UT_SUCCESS(ret, "");
    }
    TT_UT_EQUAL(tt_ptrstk_count(&q), __q_size, "");

    for (i = __q_size - 1; i >= (__q_size - q.ptr_per_frame); --i) {
        TT_UT_EQUAL(tt_ptrstk_pop(&q), &v[i], "");
    }
    TT_UT_EQUAL(tt_ptrstk_count(&q), (__q_size - q.ptr_per_frame), "");

    tt_ptrstk_clear(&q);
    TT_UT_EQUAL(tt_ptrstk_pop(&q), NULL, "");
    TT_UT_EQUAL(tt_ptrstk_top(&q), NULL, "");

    tt_ptrstk_destroy(&q);

    // test end
    TT_TEST_CASE_LEAVE()
}
