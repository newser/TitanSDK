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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_vector_basic)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(vec_case)

TT_TEST_CASE("tt_unit_test_vector_basic",
             "testing vector basic api",
             tt_unit_test_vector_basic,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE_LIST_DEFINE_END(vec_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(TEST_UNIT_VECTOR, 0, vec_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(tt_unit_test_vector_basic)
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

    if (tt_vec_head(v) != NULL)
        __utv_fail_false();
    if (tt_vec_tail(v) != NULL)
        __utv_fail_false();
    if (tt_vec_count(v) != 0)
        __utv_fail_false();
    if (tt_vec_empty(v) != TT_TRUE)
        __utv_fail_false();

    if (TT_OK(tt_vec_pop_head(v, &val_out))) {
        __utv_fail_false();
    }
    if (TT_OK(tt_vec_pop_tail(v, &val_out))) {
        __utv_fail_false();
    }

    val = 123;
    if (!TT_OK(tt_vec_push_head(v, &val))) {
        __utv_fail_false();
    }
    if (!TT_OK(tt_vec_pop_head(v, &val_out)) || (val_out != val)) {
        __utv_fail_false();
    }
    if (!tt_vec_empty(v))
        __utv_fail_false();

    if (!TT_OK(tt_vec_push_tail(v, &val))) {
        __utv_fail_false();
    }
    if (!TT_OK(tt_vec_pop_tail(v, &val_out)) || (val_out != val)) {
        __utv_fail_false();
    }
    if (!tt_vec_empty(v))
        __utv_fail_false();

    if (tt_vec_comtain(v, &val)) {
        __utv_fail_false();
    }

    if (tt_vec_get(v, 0) != NULL) {
        __utv_fail_false();
    }

    if (TT_OK(tt_vec_set(v, 0, &val))) {
        __utv_fail_false();
    }

    if (tt_vec_find(v, &val) != TT_POS_NULL) {
        __utv_fail_false();
    }
    if (tt_vec_find_last(v, &val) != TT_POS_NULL) {
        __utv_fail_false();
    }
    if (tt_vec_find_from(v, 0, &val) != TT_POS_NULL) {
        __utv_fail_false();
    }
    if (tt_vec_find_from(v, &val, ~0) != TT_POS_NULL) {
        __utv_fail_false();
    }
    if (tt_vec_find_range(v, &val, 0, 0) != TT_POS_NULL) {
        __utv_fail_false();
    }
    if (tt_vec_find_range(v, &val, 0, ~0) != TT_POS_NULL) {
        __utv_fail_false();
    }

    tt_vec_remove(v, 0);
    tt_vec_remove(v, ~0);
    if (tt_vec_remove_equal(v, &val) != TT_POS_NULL) {
        __utv_fail_false();
    }
    tt_vec_remove_range(v, 0, 0);
    tt_vec_remove_range(v, 0, ~0);

    return TT_TRUE;
}

static tt_bool_t __utv_test_u16_1(IN tt_vec_t *v)
{
    tt_u16_t val, val_out, *pval;

    if (((pval = tt_vec_head(v)) == NULL) || (*pval != 1))
        __utv_fail_false();
    if (((pval = tt_vec_tail(v)) == NULL) || (*pval != 1))
        __utv_fail_false();
    if (tt_vec_count(v) != 1)
        __utv_fail_false();
    if (tt_vec_empty(v))
        __utv_fail_false();

    if (!TT_OK(tt_vec_pop_head(v, &val_out)) || (val_out != 1)) {
        __utv_fail_false();
    }
    tt_vec_push_head(v, &val_out);

    if (!TT_OK(tt_vec_pop_tail(v, &val_out)) || (val_out != 1)) {
        __utv_fail_false();
    }
    tt_vec_push_tail(v, &val_out);

    val = 2;
    if (!TT_OK(tt_vec_push_head(v, &val))) {
        __utv_fail_false();
    }
    if (*((tt_u16_t *)tt_vec_head(v)) != 2) {
        __utv_fail_false();
    }
    if (*((tt_u16_t *)tt_vec_tail(v)) != 1) {
        __utv_fail_false();
    }
    if (!TT_OK(tt_vec_pop_head(v, &val)) || val != 2) {
        __utv_fail_false();
    }

    if (!TT_OK(tt_vec_push_tail(v, &val))) {
        __utv_fail_false();
    }
    if (*((tt_u16_t *)tt_vec_head(v)) != 1) {
        __utv_fail_false();
    }
    if (*((tt_u16_t *)tt_vec_tail(v)) != 2) {
        __utv_fail_false();
    }
    if (!TT_OK(tt_vec_pop_tail(v, &val)) || val != 2) {
        __utv_fail_false();
    }

    val = 2;
    if (tt_vec_comtain(v, &val)) {
        __utv_fail_false();
    }
    val = 1;
    if (!tt_vec_comtain(v, &val)) {
        __utv_fail_false();
    }

    if (*((tt_u16_t *)tt_vec_get(v, 0)) != 1) {
        __utv_fail_false();
    }
    if (tt_vec_get(v, 1) != NULL) {
        __utv_fail_false();
    }
    if (tt_vec_get(v, ~0) != NULL) {
        __utv_fail_false();
    }

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
    if (tt_vec_find(v, &val) != TT_POS_NULL) {
        __utv_fail_false();
    }
    if (tt_vec_find_last(v, &val) != TT_POS_NULL) {
        __utv_fail_false();
    }
    if (tt_vec_find_from(v, &val, 0) != TT_POS_NULL) {
        __utv_fail_false();
    }
    if (tt_vec_find_from(v, &val, ~0) != TT_POS_NULL) {
        __utv_fail_false();
    }
    if (tt_vec_find_range(v, &val, 0, 0) != TT_POS_NULL) {
        __utv_fail_false();
    }
    if (tt_vec_find_range(v, &val, 0, ~0) != TT_POS_NULL) {
        __utv_fail_false();
    }

    val = 1;
    if (tt_vec_find(v, &val) != 0) {
        __utv_fail_false();
    }
    if (tt_vec_find_last(v, &val) != 0) {
        __utv_fail_false();
    }
    if (tt_vec_find_from(v, &val, 0) != 0) {
        __utv_fail_false();
    }
    if (tt_vec_find_from(v, &val, 1) != TT_POS_NULL) {
        __utv_fail_false();
    }
    if (tt_vec_find_from(v, &val, ~0) != TT_POS_NULL) {
        __utv_fail_false();
    }
    if (tt_vec_find_range(v, &val, 0, 0) != TT_POS_NULL) {
        __utv_fail_false();
    }
    if (tt_vec_find_range(v, &val, 0, ~0) != 0) {
        __utv_fail_false();
    }

    tt_vec_remove(v, 0);
    if (tt_vec_get(v, 0) != NULL) {
        __utv_fail_false();
    }
    val = 1;
    tt_vec_push_tail(v, &val);

    tt_vec_remove(v, ~0);
    if (*((tt_u16_t *)tt_vec_get(v, 0)) != 1) {
        __utv_fail_false();
    }

    val = 2;
    if (tt_vec_remove_equal(v, &val) != TT_POS_NULL) {
        __utv_fail_false();
    }
    val = 1;
    if (tt_vec_remove_equal(v, &val) != 0) {
        __utv_fail_false();
    }
    tt_vec_push_tail(v, &val);

    tt_vec_remove_range(v, 0, 0);
    if (*((tt_u16_t *)tt_vec_get(v, 0)) != 1) {
        __utv_fail_false();
    }

    tt_vec_remove_range(v, 0, ~0);
    if (tt_vec_get(v, 0) != NULL) {
        __utv_fail_false();
    }
    tt_vec_push_tail(v, &val);

    return TT_TRUE;
}

static tt_bool_t __utv_test_u16_123(IN tt_vec_t *v)
{
    tt_u16_t val, val_out, *pval;

    if (((pval = tt_vec_head(v)) == NULL) || (*pval != 1))
        __utv_fail_false();
    if (((pval = tt_vec_tail(v)) == NULL) || (*pval != 3))
        __utv_fail_false();
    if (tt_vec_count(v) != 3)
        __utv_fail_false();
    if (tt_vec_empty(v))
        __utv_fail_false();

    if (!TT_OK(tt_vec_pop_head(v, &val_out)) || (val_out != 1)) {
        __utv_fail_false();
    }
    tt_vec_push_head(v, &val_out);

    if (!TT_OK(tt_vec_pop_tail(v, &val_out)) || (val_out != 3)) {
        __utv_fail_false();
    }
    tt_vec_push_tail(v, &val_out);

    val = 4;
    if (!TT_OK(tt_vec_push_head(v, &val))) {
        __utv_fail_false();
    }
    // 4,1,2,3
    if (*((tt_u16_t *)tt_vec_head(v)) != 4) {
        __utv_fail_false();
    }
    if (*((tt_u16_t *)tt_vec_tail(v)) != 3) {
        __utv_fail_false();
    }
    if (!TT_OK(tt_vec_pop_head(v, &val)) || val != 4) {
        __utv_fail_false();
    }

    if (!TT_OK(tt_vec_push_tail(v, &val))) {
        __utv_fail_false();
    }
    // 1,2,3,4
    if (*((tt_u16_t *)tt_vec_get(v, 1)) != 2) {
        __utv_fail_false();
    }
    if (*((tt_u16_t *)tt_vec_tail(v)) != 4) {
        __utv_fail_false();
    }
    if (!TT_OK(tt_vec_pop_tail(v, &val)) || val != 4) {
        __utv_fail_false();
    }

    val = 4;
    if (tt_vec_comtain(v, &val)) {
        __utv_fail_false();
    }
    val = 3;
    if (!tt_vec_comtain(v, &val)) {
        __utv_fail_false();
    }

    if (*((tt_u16_t *)tt_vec_get(v, 0)) != 1) {
        __utv_fail_false();
    }
    if (*((tt_u16_t *)tt_vec_get(v, 2)) != 3) {
        __utv_fail_false();
    }
    if (tt_vec_get(v, 3) != NULL) {
        __utv_fail_false();
    }
    if (tt_vec_get(v, ~0) != NULL) {
        __utv_fail_false();
    }

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
    if (tt_vec_find(v, &val) != 1) {
        __utv_fail_false();
    }
    if (tt_vec_find_last(v, &val) != 1) {
        __utv_fail_false();
    }
    if (tt_vec_find_from(v, &val, 0) != 1) {
        __utv_fail_false();
    }
    if (tt_vec_find_from(v, &val, 1) != 1) {
        __utv_fail_false();
    }
    if (tt_vec_find_from(v, &val, 2) != TT_POS_NULL) {
        __utv_fail_false();
    }
    if (tt_vec_find_from(v, &val, ~0) != TT_POS_NULL) {
        __utv_fail_false();
    }
    if (tt_vec_find_range(v, &val, 0, 0) != TT_POS_NULL) {
        __utv_fail_false();
    }
    if (tt_vec_find_range(v, &val, 0, 1) != TT_POS_NULL) {
        __utv_fail_false();
    }
    if (tt_vec_find_range(v, &val, 1, 2) != 1) {
        __utv_fail_false();
    }
    if (tt_vec_find_range(v, &val, 2, 3) != TT_POS_NULL) {
        __utv_fail_false();
    }
    if (tt_vec_find_range(v, &val, 0, ~0) != 1) {
        __utv_fail_false();
    }

    tt_vec_remove(v, 1);
    // 1, 3
    if (tt_vec_get(v, 2) != NULL) {
        __utv_fail_false();
    }
    val = 2;
    tt_vec_insert(v, 1, &val);

    tt_vec_remove(v, ~0);
    if (*((tt_u16_t *)tt_vec_get(v, 2)) != 3) {
        __utv_fail_false();
    }

    val = 4;
    if (tt_vec_remove_equal(v, &val) != TT_POS_NULL) {
        __utv_fail_false();
    }
    val = 2;
    if (tt_vec_remove_equal(v, &val) != 1) {
        __utv_fail_false();
    }
    val = 2;
    tt_vec_insert(v, 1, &val);

    tt_vec_remove_range(v, 0, 0);
    if (*((tt_u16_t *)tt_vec_get(v, 2)) != 3) {
        __utv_fail_false();
    }

    tt_vec_remove_range(v, 0, ~0);
    if (!tt_vec_empty(v)) {
        __utv_fail_false();
    }
    val = 1;
    tt_vec_push_tail(v, &val);
    val = 2;
    tt_vec_push_tail(v, &val);
    val = 3;
    tt_vec_push_tail(v, &val);

    val = 2;
    tt_vec_push_tail(v, &val);
    if (tt_vec_find_last(v, &val) != 3) {
        __utv_fail_false();
    }
    tt_vec_pop_tail(v, NULL);
    
    return TT_TRUE;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_vector_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_vec_t v_u16, v2;
    tt_result_t ret;
    tt_bool_t b_ret;
    tt_u16_t val16_1 = 1;

    TT_TEST_CASE_ENTER()
    // test start

    tt_vec_init(&v_u16, sizeof(tt_u16_t), NULL);
    tt_vec_trim(&v_u16);
    tt_vec_destroy(&v_u16);

    tt_vec_init(&v_u16, sizeof(tt_u16_t), NULL);

    // empty
    {
        b_ret = __utv_test_u16_0(&v_u16);
        TT_TEST_CHECK_EQUAL(b_ret, TT_SUCCESS, "");
    }

    // 1 element
    {
        ret = tt_vec_push_tail(&v_u16, &val16_1);
        TT_TEST_CHECK_SUCCESS(ret, "");

        b_ret = __utv_test_u16_1(&v_u16);
        TT_TEST_CHECK_EQUAL(b_ret, TT_SUCCESS, "");
    }

    // 3 element
    {
        val16_1 = 2;
        ret = tt_vec_push_tail(&v_u16, &val16_1);
        TT_TEST_CHECK_SUCCESS(ret, "");

        val16_1 = 3;
        ret = tt_vec_push_tail(&v_u16, &val16_1);
        TT_TEST_CHECK_SUCCESS(ret, "");

        b_ret = __utv_test_u16_123(&v_u16);
        TT_TEST_CHECK_EQUAL(b_ret, TT_SUCCESS, "");

        TT_TEST_CHECK_EQUAL(tt_vec_count(&v_u16), 3, "");
    }

    // move
    {
        tt_vec_init(&v2, sizeof(tt_u16_t), NULL);

        ret = tt_vec_move_all(&v_u16, &v2);
        TT_TEST_CHECK_SUCCESS(ret, "");
        TT_TEST_CHECK_EQUAL(tt_vec_count(&v_u16), 3, "");

        ret = tt_vec_move_from(&v_u16, &v2, 0);
        TT_TEST_CHECK_SUCCESS(ret, "");
        TT_TEST_CHECK_EQUAL(tt_vec_count(&v_u16), 3, "");

        ret = tt_vec_move_from(&v_u16, &v2, ~0);
        TT_TEST_CHECK_SUCCESS(ret, "");
        TT_TEST_CHECK_EQUAL(tt_vec_count(&v_u16), 3, "");

        ret = tt_vec_move_range(&v_u16, &v2, 0, 0);
        TT_TEST_CHECK_SUCCESS(ret, "");
        TT_TEST_CHECK_EQUAL(tt_vec_count(&v_u16), 3, "");

        ret = tt_vec_move_range(&v_u16, &v2, 0, ~0);
        TT_TEST_CHECK_SUCCESS(ret, "");
        TT_TEST_CHECK_EQUAL(tt_vec_count(&v_u16), 3, "");

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
        TT_TEST_CHECK_EQUAL(tt_vec_count(&v_u16), 4, "");
        TT_TEST_CHECK_EQUAL(*(tt_u16_t *)tt_vec_get(&v_u16, 3), 6, "");

        // 1,2,3,6 <= 4,5
        tt_vec_move_range(&v_u16, &v2, 1, ~0);
        // 1,2,3,6,5 <= 4
        TT_TEST_CHECK_EQUAL(tt_vec_count(&v_u16), 5, "");
        TT_TEST_CHECK_EQUAL(*(tt_u16_t *)tt_vec_get(&v_u16, 4), 5, "");

        tt_vec_move_all(&v_u16, &v2);
        // 1,2,3,6,5,4
        TT_TEST_CHECK_EQUAL(tt_vec_count(&v_u16), 6, "");
        TT_TEST_CHECK_EQUAL(*(tt_u16_t *)tt_vec_get(&v_u16, 5), 4, "");
        TT_TEST_CHECK_EQUAL(tt_vec_count(&v2), 0, "");

        tt_vec_trim(&v2);
        tt_vec_destroy(&v2);
    }

    tt_vec_destroy(&v_u16);

    // test end
    TT_TEST_CASE_LEAVE()
}
