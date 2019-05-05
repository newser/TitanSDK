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
TT_TEST_ROUTINE_DECLARE(case_vector)
TT_TEST_ROUTINE_DECLARE(case_ptr_vector)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(vec_case)

TT_TEST_CASE("case_vector", "testing vector basic api", case_vector, NULL, NULL,
             NULL, NULL, NULL)
,

    TT_TEST_CASE("case_ptr_vector", "testing pointer vector", case_ptr_vector,
                 NULL, NULL, NULL, NULL, NULL),

    TT_TEST_CASE_LIST_DEFINE_END(vec_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(ALG_UT_VECTOR, 0, vec_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(case_vector)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    static tt_result_t __utv_ret;
static tt_u32_t __utv_err_line;
#define __utv_fail_false()                                                     \
    do {                                                                       \
        __utv_ret = TT_FAIL;                                                   \
        __utv_err_line = __LINE__;                                             \
        return TT_FALSE;                                                       \
    } while (0)

static tt_bool_t __utv_test_u16_0(IN tt_vec_t *v)
{
    tt_u16_t val, val_out;

    if (tt_vec_head(v) != NULL) __utv_fail_false();
    if (tt_vec_tail(v) != NULL) __utv_fail_false();
    if (tt_vec_count(v) != 0) __utv_fail_false();
    if (tt_vec_empty(v) != TT_TRUE) __utv_fail_false();

    if (TT_OK(tt_vec_pop_head(v, &val_out))) { __utv_fail_false(); }
    if (TT_OK(tt_vec_pop_tail(v, &val_out))) { __utv_fail_false(); }

    val = 123;
    if (!TT_OK(tt_vec_push_head(v, &val))) { __utv_fail_false(); }
    if (!TT_OK(tt_vec_pop_head(v, &val_out)) || (val_out != val)) {
        __utv_fail_false();
    }
    if (!tt_vec_empty(v)) __utv_fail_false();

    if (!TT_OK(tt_vec_push_tail(v, &val))) { __utv_fail_false(); }
    if (!TT_OK(tt_vec_pop_tail(v, &val_out)) || (val_out != val)) {
        __utv_fail_false();
    }
    if (!tt_vec_empty(v)) __utv_fail_false();

    if (tt_vec_comtain(v, &val)) { __utv_fail_false(); }

    if (tt_vec_get(v, 0) != NULL) { __utv_fail_false(); }

    if (TT_OK(tt_vec_set(v, 0, &val))) { __utv_fail_false(); }

    if (tt_vec_find(v, &val) != TT_POS_NULL) { __utv_fail_false(); }
    if (tt_vec_find_last(v, &val) != TT_POS_NULL) { __utv_fail_false(); }
    if (tt_vec_find_from(v, &val, 0) != TT_POS_NULL) { __utv_fail_false(); }
    if (tt_vec_find_from(v, &val, ~0) != TT_POS_NULL) { __utv_fail_false(); }
    if (tt_vec_find_range(v, &val, 0, 0) != TT_POS_NULL) { __utv_fail_false(); }
    if (tt_vec_find_range(v, &val, 0, ~0) != TT_POS_NULL) {
        __utv_fail_false();
    }

    tt_vec_remove(v, 0);
    tt_vec_remove(v, ~0);
    if (tt_vec_remove_equal(v, &val) != TT_POS_NULL) { __utv_fail_false(); }
    tt_vec_remove_range(v, 0, 0);
    tt_vec_remove_range(v, 0, ~0);

    return TT_TRUE;
}

static tt_bool_t __utv_test_u16_1(IN tt_vec_t *v)
{
    tt_u16_t val, val_out, *pval;

    if (((pval = tt_vec_head(v)) == NULL) || (*pval != 1)) __utv_fail_false();
    if (((pval = tt_vec_tail(v)) == NULL) || (*pval != 1)) __utv_fail_false();
    if (tt_vec_count(v) != 1) __utv_fail_false();
    if (tt_vec_empty(v)) __utv_fail_false();

    if (!TT_OK(tt_vec_pop_head(v, &val_out)) || (val_out != 1)) {
        __utv_fail_false();
    }
    tt_vec_push_head(v, &val_out);

    if (!TT_OK(tt_vec_pop_tail(v, &val_out)) || (val_out != 1)) {
        __utv_fail_false();
    }
    tt_vec_push_tail(v, &val_out);

    val = 2;
    if (!TT_OK(tt_vec_push_head(v, &val))) { __utv_fail_false(); }
    if (*((tt_u16_t *)tt_vec_head(v)) != 2) { __utv_fail_false(); }
    if (*((tt_u16_t *)tt_vec_tail(v)) != 1) { __utv_fail_false(); }
    if (!TT_OK(tt_vec_pop_head(v, &val)) || val != 2) { __utv_fail_false(); }

    if (!TT_OK(tt_vec_push_tail(v, &val))) { __utv_fail_false(); }
    if (*((tt_u16_t *)tt_vec_head(v)) != 1) { __utv_fail_false(); }
    if (*((tt_u16_t *)tt_vec_tail(v)) != 2) { __utv_fail_false(); }
    if (!TT_OK(tt_vec_pop_tail(v, &val)) || val != 2) { __utv_fail_false(); }

    val = 2;
    if (tt_vec_comtain(v, &val)) { __utv_fail_false(); }
    val = 1;
    if (!tt_vec_comtain(v, &val)) { __utv_fail_false(); }

    if (*((tt_u16_t *)tt_vec_get(v, 0)) != 1) { __utv_fail_false(); }
    if (tt_vec_get(v, 1) != NULL) { __utv_fail_false(); }
    if (tt_vec_get(v, ~0) != NULL) { __utv_fail_false(); }

    val = 2;
    if (!TT_OK(tt_vec_set(v, 0, &val)) ||
        (*((tt_u16_t *)tt_vec_get(v, 0)) != 2)) {
        __utv_fail_false();
    }
    val = 1;
    if (!TT_OK(tt_vec_set(v, 0, &val)) ||
        (*((tt_u16_t *)tt_vec_get(v, 0)) != 1)) {
        __utv_fail_false();
    }
    if (TT_OK(tt_vec_set(v, 1, &val)) || TT_OK(tt_vec_set(v, ~0, &val))) {
        __utv_fail_false();
    }

    val = 2;
    if (tt_vec_find(v, &val) != TT_POS_NULL) { __utv_fail_false(); }
    if (tt_vec_find_last(v, &val) != TT_POS_NULL) { __utv_fail_false(); }
    if (tt_vec_find_from(v, &val, 0) != TT_POS_NULL) { __utv_fail_false(); }
    if (tt_vec_find_from(v, &val, ~0) != TT_POS_NULL) { __utv_fail_false(); }
    if (tt_vec_find_range(v, &val, 0, 0) != TT_POS_NULL) { __utv_fail_false(); }
    if (tt_vec_find_range(v, &val, 0, ~0) != TT_POS_NULL) {
        __utv_fail_false();
    }

    val = 1;
    if (tt_vec_find(v, &val) != 0) { __utv_fail_false(); }
    if (tt_vec_find_last(v, &val) != 0) { __utv_fail_false(); }
    if (tt_vec_find_from(v, &val, 0) != 0) { __utv_fail_false(); }
    if (tt_vec_find_from(v, &val, 1) != TT_POS_NULL) { __utv_fail_false(); }
    if (tt_vec_find_from(v, &val, ~0) != TT_POS_NULL) { __utv_fail_false(); }
    if (tt_vec_find_range(v, &val, 0, 0) != TT_POS_NULL) { __utv_fail_false(); }
    if (tt_vec_find_range(v, &val, 0, ~0) != 0) { __utv_fail_false(); }

    tt_vec_remove(v, 0);
    if (tt_vec_get(v, 0) != NULL) { __utv_fail_false(); }
    val = 1;
    tt_vec_push_tail(v, &val);

    tt_vec_remove(v, ~0);
    if (*((tt_u16_t *)tt_vec_get(v, 0)) != 1) { __utv_fail_false(); }

    val = 2;
    if (tt_vec_remove_equal(v, &val) != TT_POS_NULL) { __utv_fail_false(); }
    val = 1;
    if (tt_vec_remove_equal(v, &val) != 0) { __utv_fail_false(); }
    tt_vec_push_tail(v, &val);

    tt_vec_remove_range(v, 0, 0);
    if (*((tt_u16_t *)tt_vec_get(v, 0)) != 1) { __utv_fail_false(); }

    tt_vec_remove_range(v, 0, ~0);
    if (tt_vec_get(v, 0) != NULL) { __utv_fail_false(); }
    tt_vec_push_tail(v, &val);

    return TT_TRUE;
}

static tt_bool_t __utv_test_u16_123(IN tt_vec_t *v)
{
    tt_u16_t val, val_out, *pval;

    if (((pval = tt_vec_head(v)) == NULL) || (*pval != 1)) __utv_fail_false();
    if (((pval = tt_vec_tail(v)) == NULL) || (*pval != 3)) __utv_fail_false();
    if (tt_vec_count(v) != 3) __utv_fail_false();
    if (tt_vec_empty(v)) __utv_fail_false();

    if (!TT_OK(tt_vec_pop_head(v, &val_out)) || (val_out != 1)) {
        __utv_fail_false();
    }
    tt_vec_push_head(v, &val_out);

    if (!TT_OK(tt_vec_pop_tail(v, &val_out)) || (val_out != 3)) {
        __utv_fail_false();
    }
    tt_vec_push_tail(v, &val_out);

    val = 4;
    if (!TT_OK(tt_vec_push_head(v, &val))) { __utv_fail_false(); }
    // 4,1,2,3
    if (*((tt_u16_t *)tt_vec_head(v)) != 4) { __utv_fail_false(); }
    if (*((tt_u16_t *)tt_vec_tail(v)) != 3) { __utv_fail_false(); }
    if (!TT_OK(tt_vec_pop_head(v, &val)) || val != 4) { __utv_fail_false(); }

    if (!TT_OK(tt_vec_push_tail(v, &val))) { __utv_fail_false(); }
    // 1,2,3,4
    if (*((tt_u16_t *)tt_vec_get(v, 1)) != 2) { __utv_fail_false(); }
    if (*((tt_u16_t *)tt_vec_tail(v)) != 4) { __utv_fail_false(); }
    if (!TT_OK(tt_vec_pop_tail(v, &val)) || val != 4) { __utv_fail_false(); }

    val = 4;
    if (tt_vec_comtain(v, &val)) { __utv_fail_false(); }
    val = 3;
    if (!tt_vec_comtain(v, &val)) { __utv_fail_false(); }

    if (*((tt_u16_t *)tt_vec_get(v, 0)) != 1) { __utv_fail_false(); }
    if (*((tt_u16_t *)tt_vec_get(v, 2)) != 3) { __utv_fail_false(); }
    if (tt_vec_get(v, 3) != NULL) { __utv_fail_false(); }
    if (tt_vec_get(v, ~0) != NULL) { __utv_fail_false(); }

    val = 9;
    if (!TT_OK(tt_vec_set(v, 2, &val)) ||
        (*((tt_u16_t *)tt_vec_get(v, 2)) != 9)) {
        __utv_fail_false();
    }
    val = 3;
    if (!TT_OK(tt_vec_set(v, 2, &val)) ||
        (*((tt_u16_t *)tt_vec_get(v, 2)) != 3)) {
        __utv_fail_false();
    }
    if (TT_OK(tt_vec_set(v, 4, &val)) || TT_OK(tt_vec_set(v, ~0, &val))) {
        __utv_fail_false();
    }

    // 1,2,3
    val = 2;
    if (tt_vec_find(v, &val) != 1) { __utv_fail_false(); }
    if (tt_vec_find_last(v, &val) != 1) { __utv_fail_false(); }
    if (tt_vec_find_from(v, &val, 0) != 1) { __utv_fail_false(); }
    if (tt_vec_find_from(v, &val, 1) != 1) { __utv_fail_false(); }
    if (tt_vec_find_from(v, &val, 2) != TT_POS_NULL) { __utv_fail_false(); }
    if (tt_vec_find_from(v, &val, ~0) != TT_POS_NULL) { __utv_fail_false(); }
    if (tt_vec_find_range(v, &val, 0, 0) != TT_POS_NULL) { __utv_fail_false(); }
    if (tt_vec_find_range(v, &val, 0, 1) != TT_POS_NULL) { __utv_fail_false(); }
    if (tt_vec_find_range(v, &val, 1, 2) != 1) { __utv_fail_false(); }
    if (tt_vec_find_range(v, &val, 2, 3) != TT_POS_NULL) { __utv_fail_false(); }
    if (tt_vec_find_range(v, &val, 0, ~0) != 1) { __utv_fail_false(); }

    tt_vec_remove(v, 1);
    // 1, 3
    if (tt_vec_get(v, 2) != NULL) { __utv_fail_false(); }
    val = 2;
    tt_vec_insert(v, 1, &val);

    tt_vec_remove(v, ~0);
    if (*((tt_u16_t *)tt_vec_get(v, 2)) != 3) { __utv_fail_false(); }

    val = 4;
    if (tt_vec_remove_equal(v, &val) != TT_POS_NULL) { __utv_fail_false(); }
    val = 2;
    if (tt_vec_remove_equal(v, &val) != 1) { __utv_fail_false(); }
    val = 2;
    tt_vec_insert(v, 1, &val);

    tt_vec_remove_range(v, 0, 0);
    if (*((tt_u16_t *)tt_vec_get(v, 2)) != 3) { __utv_fail_false(); }

    tt_vec_remove_range(v, 0, ~0);
    if (!tt_vec_empty(v)) { __utv_fail_false(); }
    val = 1;
    tt_vec_push_tail(v, &val);
    val = 2;
    tt_vec_push_tail(v, &val);
    val = 3;
    tt_vec_push_tail(v, &val);

    val = 2;
    tt_vec_push_tail(v, &val);
    if (tt_vec_find_last(v, &val) != 3) { __utv_fail_false(); }
    tt_vec_pop_tail(v, NULL);

    return TT_TRUE;
}

TT_TEST_ROUTINE_DEFINE(case_vector)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_vec_t v_u16, v2;
    tt_result_t ret;
    tt_bool_t b_ret;
    tt_u16_t val16_1 = 1;

    TT_TEST_CASE_ENTER()
    // test start

    tt_vec_init(&v_u16, sizeof(tt_u16_t), NULL, NULL);
    tt_vec_trim(&v_u16);
    tt_vec_destroy(&v_u16);

    tt_vec_init(&v_u16, sizeof(tt_u16_t), NULL, NULL);

    // empty
    {
        b_ret = __utv_test_u16_0(&v_u16);
        TT_UT_EQUAL(b_ret, TT_SUCCESS, "");
    }

    // 1 element
    {
        ret = tt_vec_push_tail(&v_u16, &val16_1);
        TT_UT_SUCCESS(ret, "");

        b_ret = __utv_test_u16_1(&v_u16);
        TT_UT_EQUAL(b_ret, TT_SUCCESS, "");
    }

    // 3 element
    {
        val16_1 = 2;
        ret = tt_vec_push_tail(&v_u16, &val16_1);
        TT_UT_SUCCESS(ret, "");

        val16_1 = 3;
        ret = tt_vec_push_tail(&v_u16, &val16_1);
        TT_UT_SUCCESS(ret, "");

        b_ret = __utv_test_u16_123(&v_u16);
        TT_UT_EQUAL(b_ret, TT_SUCCESS, "");

        TT_UT_EQUAL(tt_vec_count(&v_u16), 3, "");
    }

    // move
    {
        tt_vec_init(&v2, sizeof(tt_u16_t), NULL, NULL);

        ret = tt_vec_move_all(&v_u16, &v2);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_vec_count(&v_u16), 3, "");

        ret = tt_vec_move_from(&v_u16, &v2, 0);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_vec_count(&v_u16), 3, "");

        ret = tt_vec_move_from(&v_u16, &v2, ~0);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_vec_count(&v_u16), 3, "");

        ret = tt_vec_move_range(&v_u16, &v2, 0, 0);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_vec_count(&v_u16), 3, "");

        ret = tt_vec_move_range(&v_u16, &v2, 0, ~0);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_vec_count(&v_u16), 3, "");

        val16_1 = 4;
        tt_vec_push_tail(&v2, &val16_1);
        val16_1 = 5;
        tt_vec_push_tail(&v2, &val16_1);
        val16_1 = 6;
        tt_vec_push_tail(&v2, &val16_1);

        tt_vec_trim(&v_u16);

        // 1,2,3 <= 4,5,6
        tt_vec_move_from(&v_u16, &v2, 2);
        // 1,2,3,6 <= 4,5
        TT_UT_EQUAL(tt_vec_count(&v_u16), 4, "");
        TT_UT_EQUAL(*(tt_u16_t *)tt_vec_get(&v_u16, 3), 6, "");

        // 1,2,3,6 <= 4,5
        tt_vec_move_range(&v_u16, &v2, 1, ~0);
        // 1,2,3,6,5 <= 4
        TT_UT_EQUAL(tt_vec_count(&v_u16), 5, "");
        TT_UT_EQUAL(*(tt_u16_t *)tt_vec_get(&v_u16, 4), 5, "");

        tt_vec_move_all(&v_u16, &v2);
        // 1,2,3,6,5,4
        TT_UT_EQUAL(tt_vec_count(&v_u16), 6, "");
        TT_UT_EQUAL(*(tt_u16_t *)tt_vec_get(&v_u16, 5), 4, "");
        TT_UT_EQUAL(tt_vec_count(&v2), 0, "");

        tt_vec_trim(&v2);
        tt_vec_destroy(&v2);
    }

    tt_vec_destroy(&v_u16);

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_u16_t pv1, pv2, pv3, pv4, pv5, pv6;

static tt_bool_t __utv_test_ptr_0(IN tt_ptrvec_t *v)
{
    tt_u16_t tmp = 0;

    if (tt_ptrvec_head(v) != NULL) __utv_fail_false();
    if (tt_ptrvec_tail(v) != NULL) __utv_fail_false();
    if (tt_ptrvec_count(v) != 0) __utv_fail_false();
    if (tt_ptrvec_empty(v) != TT_TRUE) __utv_fail_false();

    if (tt_ptrvec_pop_head(v) != NULL) { __utv_fail_false(); }
    if (tt_ptrvec_pop_tail(v) != NULL) { __utv_fail_false(); }

    if (!TT_OK(tt_ptrvec_push_head(v, &tmp))) { __utv_fail_false(); }
    if (tt_ptrvec_pop_head(v) != &tmp) { __utv_fail_false(); }
    if (!tt_ptrvec_empty(v)) __utv_fail_false();

    if (!TT_OK(tt_ptrvec_push_tail(v, &tmp))) { __utv_fail_false(); }
    if (tt_ptrvec_pop_tail(v) != &tmp) { __utv_fail_false(); }
    if (!tt_ptrvec_empty(v)) __utv_fail_false();

    if (tt_ptrvec_comtain(v, &tmp)) { __utv_fail_false(); }

    if (tt_ptrvec_get(v, 0) != NULL) { __utv_fail_false(); }

    if (TT_OK(tt_ptrvec_set(v, 0, &tmp))) { __utv_fail_false(); }

    if (tt_ptrvec_find(v, &tmp) != TT_POS_NULL) { __utv_fail_false(); }
    if (tt_ptrvec_find_last(v, &tmp) != TT_POS_NULL) { __utv_fail_false(); }
    if (tt_ptrvec_find_from(v, &tmp, 0) != TT_POS_NULL) { __utv_fail_false(); }
    if (tt_ptrvec_find_from(v, &tmp, ~0) != TT_POS_NULL) { __utv_fail_false(); }
    if (tt_ptrvec_find_range(v, &tmp, 0, 0) != TT_POS_NULL) {
        __utv_fail_false();
    }
    if (tt_ptrvec_find_range(v, &tmp, 0, ~0) != TT_POS_NULL) {
        __utv_fail_false();
    }

    tt_ptrvec_remove(v, 0);
    tt_ptrvec_remove(v, ~0);
    if (tt_ptrvec_remove_equal(v, &tmp) != TT_POS_NULL) { __utv_fail_false(); }
    tt_ptrvec_remove_range(v, 0, 0);
    tt_ptrvec_remove_range(v, 0, ~0);

    return TT_TRUE;
}

static tt_bool_t __utv_test_ptr_1(IN tt_ptrvec_t *v)
{
    if (tt_ptrvec_head(v) != &pv1) __utv_fail_false();
    if (tt_ptrvec_tail(v) != &pv1) __utv_fail_false();
    if (tt_ptrvec_count(v) != 1) __utv_fail_false();
    if (tt_ptrvec_empty(v)) __utv_fail_false();

    if (tt_ptrvec_pop_head(v) != &pv1) { __utv_fail_false(); }
    tt_ptrvec_push_head(v, &pv1);

    if (tt_ptrvec_pop_tail(v) != &pv1) { __utv_fail_false(); }
    tt_ptrvec_push_tail(v, &pv1);

    if (!TT_OK(tt_ptrvec_push_head(v, &pv2))) { __utv_fail_false(); }
    if (tt_ptrvec_head(v) != &pv2) { __utv_fail_false(); }
    if (tt_ptrvec_tail(v) != &pv1) { __utv_fail_false(); }
    if (tt_ptrvec_pop_head(v) != &pv2) { __utv_fail_false(); }

    if (!TT_OK(tt_ptrvec_push_tail(v, &pv2))) { __utv_fail_false(); }
    if (tt_ptrvec_head(v) != &pv1) { __utv_fail_false(); }
    if (tt_ptrvec_tail(v) != &pv2) { __utv_fail_false(); }
    if (tt_ptrvec_pop_tail(v) != &pv2) { __utv_fail_false(); }

    if (tt_ptrvec_comtain(v, &pv3)) { __utv_fail_false(); }
    if (!tt_ptrvec_comtain(v, &pv1)) { __utv_fail_false(); }

    if (tt_ptrvec_get(v, 0) != &pv1) { __utv_fail_false(); }
    if (tt_ptrvec_get(v, 1) != NULL) { __utv_fail_false(); }
    if (tt_ptrvec_get(v, ~0) != NULL) { __utv_fail_false(); }

    if (!TT_OK(tt_ptrvec_set(v, 0, &pv2)) || (tt_ptrvec_get(v, 0) != &pv2)) {
        __utv_fail_false();
    }
    if (!TT_OK(tt_ptrvec_set(v, 0, &pv1)) || (tt_ptrvec_get(v, 0) != &pv1)) {
        __utv_fail_false();
    }
    if (TT_OK(tt_ptrvec_set(v, 1, &pv3)) || TT_OK(tt_ptrvec_set(v, ~0, &pv3))) {
        __utv_fail_false();
    }

    if (tt_ptrvec_find(v, &pv2) != TT_POS_NULL) { __utv_fail_false(); }
    if (tt_ptrvec_find_last(v, &pv2) != TT_POS_NULL) { __utv_fail_false(); }
    if (tt_ptrvec_find_from(v, &pv2, 0) != TT_POS_NULL) { __utv_fail_false(); }
    if (tt_ptrvec_find_from(v, &pv2, ~0) != TT_POS_NULL) { __utv_fail_false(); }
    if (tt_ptrvec_find_range(v, &pv2, 0, 0) != TT_POS_NULL) {
        __utv_fail_false();
    }
    if (tt_ptrvec_find_range(v, &pv2, 0, ~0) != TT_POS_NULL) {
        __utv_fail_false();
    }

    if (tt_ptrvec_find(v, &pv1) != 0) { __utv_fail_false(); }
    if (tt_ptrvec_find_last(v, &pv1) != 0) { __utv_fail_false(); }
    if (tt_ptrvec_find_from(v, &pv1, 0) != 0) { __utv_fail_false(); }
    if (tt_ptrvec_find_from(v, &pv1, 1) != TT_POS_NULL) { __utv_fail_false(); }
    if (tt_ptrvec_find_from(v, &pv1, ~0) != TT_POS_NULL) { __utv_fail_false(); }
    if (tt_ptrvec_find_range(v, &pv1, 0, 0) != TT_POS_NULL) {
        __utv_fail_false();
    }
    if (tt_ptrvec_find_range(v, &pv1, 0, ~0) != 0) { __utv_fail_false(); }

    tt_ptrvec_remove(v, 0);
    if (tt_ptrvec_get(v, 0) != NULL) { __utv_fail_false(); }
    tt_ptrvec_push_tail(v, &pv1);

    tt_vec_remove(v, ~0);
    if (tt_ptrvec_get(v, 0) != &pv1) { __utv_fail_false(); }

    if (tt_ptrvec_remove_equal(v, &pv2) != TT_POS_NULL) { __utv_fail_false(); }
    if (tt_ptrvec_remove_equal(v, &pv1) != 0) { __utv_fail_false(); }
    tt_ptrvec_push_tail(v, &pv1);

    tt_ptrvec_remove_range(v, 0, 0);
    if (tt_ptrvec_get(v, 0) == NULL) { __utv_fail_false(); }

    tt_ptrvec_remove_range(v, 0, ~0);
    if (tt_ptrvec_get(v, 0) != NULL) { __utv_fail_false(); }
    tt_ptrvec_push_tail(v, &pv1);

    return TT_TRUE;
}

static tt_bool_t __utv_test_ptr_123(IN tt_vec_t *v)
{
    if (tt_ptrvec_head(v) != &pv1) __utv_fail_false();
    if (tt_ptrvec_tail(v) != &pv3) __utv_fail_false();
    if (tt_ptrvec_count(v) != 3) __utv_fail_false();
    if (tt_ptrvec_empty(v)) __utv_fail_false();

    if (tt_ptrvec_pop_head(v) != &pv1) { __utv_fail_false(); }
    tt_ptrvec_push_head(v, &pv1);

    if (tt_ptrvec_pop_tail(v) != &pv3) { __utv_fail_false(); }
    tt_ptrvec_push_tail(v, &pv3);

    if (!TT_OK(tt_ptrvec_push_head(v, &pv4))) { __utv_fail_false(); }
    // 4,1,2,3
    if (tt_ptrvec_head(v) != &pv4) { __utv_fail_false(); }
    if (tt_ptrvec_tail(v) != &pv3) { __utv_fail_false(); }
    if (tt_ptrvec_pop_head(v) != &pv4) { __utv_fail_false(); }

    if (!TT_OK(tt_ptrvec_push_tail(v, &pv4))) { __utv_fail_false(); }
    // 1,2,3,4
    if (tt_ptrvec_head(v) != &pv1) { __utv_fail_false(); }
    if (tt_ptrvec_tail(v) != &pv4) { __utv_fail_false(); }
    if (tt_ptrvec_pop_tail(v) != &pv4) { __utv_fail_false(); }

    if (tt_ptrvec_comtain(v, &pv4)) { __utv_fail_false(); }
    if (!tt_ptrvec_comtain(v, &pv2)) { __utv_fail_false(); }

    if (tt_ptrvec_get(v, 0) != &pv1) { __utv_fail_false(); }
    if (tt_ptrvec_get(v, 1) != &pv2) { __utv_fail_false(); }
    if (tt_ptrvec_get(v, 3) != NULL) { __utv_fail_false(); }
    if (tt_ptrvec_get(v, ~0) != NULL) { __utv_fail_false(); }

    if (!TT_OK(tt_ptrvec_set(v, 2, &pv6)) || (tt_ptrvec_get(v, 2) != &pv6)) {
        __utv_fail_false();
    }
    if (!TT_OK(tt_ptrvec_set(v, 2, &pv3)) || (tt_ptrvec_get(v, 2) != &pv3)) {
        __utv_fail_false();
    }
    if (TT_OK(tt_ptrvec_set(v, 4, &pv6)) || TT_OK(tt_ptrvec_set(v, ~0, &pv6))) {
        __utv_fail_false();
    }

    // 1,2,3
    if (tt_ptrvec_find(v, &pv2) != 1) { __utv_fail_false(); }
    if (tt_ptrvec_find_last(v, &pv2) != 1) { __utv_fail_false(); }
    if (tt_ptrvec_find_from(v, &pv2, 0) != 1) { __utv_fail_false(); }
    if (tt_ptrvec_find_from(v, &pv2, 1) != 1) { __utv_fail_false(); }
    if (tt_ptrvec_find_from(v, &pv2, 2) != TT_POS_NULL) { __utv_fail_false(); }
    if (tt_ptrvec_find_from(v, &pv2, ~0) != TT_POS_NULL) { __utv_fail_false(); }
    if (tt_ptrvec_find_range(v, &pv2, 0, 0) != TT_POS_NULL) {
        __utv_fail_false();
    }
    if (tt_ptrvec_find_range(v, &pv2, 0, 1) != TT_POS_NULL) {
        __utv_fail_false();
    }
    if (tt_ptrvec_find_range(v, &pv2, 1, 2) != 1) { __utv_fail_false(); }
    if (tt_ptrvec_find_range(v, &pv2, 2, 3) != TT_POS_NULL) {
        __utv_fail_false();
    }
    if (tt_ptrvec_find_range(v, &pv2, 0, ~0) != 1) { __utv_fail_false(); }

    tt_ptrvec_remove(v, 1);
    // 1, 3
    if (tt_ptrvec_get(v, 2) != NULL) { __utv_fail_false(); }
    tt_ptrvec_insert(v, 1, &pv2);

    tt_ptrvec_remove(v, ~0);
    if (tt_ptrvec_get(v, 2) != &pv3) { __utv_fail_false(); }

    if (tt_ptrvec_remove_equal(v, &pv5) != TT_POS_NULL) { __utv_fail_false(); }
    if (tt_ptrvec_remove_equal(v, &pv2) != 1) { __utv_fail_false(); }
    tt_ptrvec_insert(v, 1, &pv2);

    tt_ptrvec_remove_range(v, 0, 0);
    if (tt_ptrvec_get(v, 2) != &pv3) { __utv_fail_false(); }

    tt_ptrvec_remove_range(v, 0, ~0);
    if (!tt_ptrvec_empty(v)) { __utv_fail_false(); }
    tt_ptrvec_push_tail(v, &pv1);
    tt_ptrvec_push_tail(v, &pv2);
    tt_ptrvec_push_tail(v, &pv3);

    tt_ptrvec_push_tail(v, &pv2);
    if (tt_ptrvec_find_last(v, &pv2) != 3) { __utv_fail_false(); }
    tt_ptrvec_pop_tail(v);

    return TT_TRUE;
}

TT_TEST_ROUTINE_DEFINE(case_ptr_vector)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_ptrvec_t pv, v2;
    tt_result_t ret;
    tt_bool_t b_ret;

    TT_TEST_CASE_ENTER()
    // test start

    tt_ptrvec_init(&pv, NULL, NULL);
    tt_ptrvec_trim(&pv);
    tt_ptrvec_destroy(&pv);

    tt_ptrvec_init(&pv, NULL, NULL);

    // empty
    {
        b_ret = __utv_test_ptr_0(&pv);
        TT_UT_EQUAL(b_ret, TT_SUCCESS, "");
    }

    // 1 element
    {
        ret = tt_ptrvec_push_tail(&pv, &pv1);
        TT_UT_SUCCESS(ret, "");

        b_ret = __utv_test_ptr_1(&pv);
        TT_UT_EQUAL(b_ret, TT_SUCCESS, "");
    }

    // 3 element
    {
        ret = tt_ptrvec_push_tail(&pv, &pv2);
        TT_UT_SUCCESS(ret, "");
        ret = tt_ptrvec_push_tail(&pv, &pv3);
        TT_UT_SUCCESS(ret, "");

        b_ret = __utv_test_ptr_123(&pv);
        TT_UT_EQUAL(b_ret, TT_SUCCESS, "");

        TT_UT_EQUAL(tt_ptrvec_count(&pv), 3, "");
    }

    // move
    {
        tt_ptrvec_init(&v2, NULL, NULL);

        ret = tt_ptrvec_move_all(&pv, &v2);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_ptrvec_count(&pv), 3, "");

        ret = tt_ptrvec_move_from(&pv, &v2, 0);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_ptrvec_count(&pv), 3, "");

        ret = tt_ptrvec_move_from(&pv, &v2, ~0);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_ptrvec_count(&pv), 3, "");

        ret = tt_ptrvec_move_range(&pv, &v2, 0, 0);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_ptrvec_count(&pv), 3, "");

        ret = tt_ptrvec_move_range(&pv, &v2, 0, ~0);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_ptrvec_count(&pv), 3, "");

        tt_ptrvec_push_tail(&v2, &pv4);
        tt_ptrvec_push_tail(&v2, &pv5);
        tt_ptrvec_push_tail(&v2, &pv6);

        tt_ptrvec_trim(&pv);

        // 1,2,3 <= 4,5,6
        tt_ptrvec_move_from(&pv, &v2, 2);
        // 1,2,3,6 <= 4,5
        TT_UT_EQUAL(tt_ptrvec_count(&pv), 4, "");
        TT_UT_EQUAL(tt_ptrvec_get(&pv, 3), &pv6, "");

        // 1,2,3,6 <= 4,5
        tt_ptrvec_move_range(&pv, &v2, 1, ~0);
        // 1,2,3,6,5 <= 4
        TT_UT_EQUAL(tt_ptrvec_count(&pv), 5, "");
        TT_UT_EQUAL(tt_ptrvec_get(&pv, 4), &pv5, "");

        tt_ptrvec_move_all(&pv, &v2);
        // 1,2,3,6,5,4
        TT_UT_EQUAL(tt_ptrvec_count(&pv), 6, "");
        TT_UT_EQUAL(tt_ptrvec_get(&pv, 5), &pv4, "");
        TT_UT_EQUAL(tt_ptrvec_count(&v2), 0, "");

        tt_ptrvec_trim(&v2);
        tt_ptrvec_destroy(&v2);
    }

    tt_ptrvec_destroy(&pv);

    // test end
    TT_TEST_CASE_LEAVE()
}
