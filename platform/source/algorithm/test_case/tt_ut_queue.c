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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_queue)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_ptr_queue)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(queue_case)

TT_TEST_CASE("tt_unit_test_queue",
             "testing queue",
             tt_unit_test_queue,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_ptr_queue",
                 "testing ptr queue",
                 tt_unit_test_ptr_queue,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(queue_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(ALG_UT_QUEUE, 0, queue_case)

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

        TT_TEST_ROUTINE_DEFINE(tt_unit_test_queue)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_u32_t v[__q_size] = {0};
    tt_u32_t i, n, val;
    tt_result_t ret;
    tt_queue_t q;
    tt_queue_attr_t attr;

    TT_TEST_CASE_ENTER()
    // test start

    for (i = 0; i < __q_size; ++i) {
        v[i] = i;
    }

    tt_queue_attr_default(&attr);
    attr.obj_per_frame = __qf_size;

    tt_queue_init(&q, sizeof(tt_u32_t), &attr);
    tt_queue_clear(&q);
    TT_TEST_CHECK_EQUAL(tt_queue_count(&q), 0, "");
    TT_TEST_CHECK_EQUAL(tt_queue_empty(&q), TT_TRUE, "");
    TT_TEST_CHECK_EQUAL(tt_queue_head(&q), NULL, "");
    TT_TEST_CHECK_EQUAL(tt_queue_tail(&q), NULL, "");

    ret = tt_queue_pop(&q, &i);
    TT_TEST_CHECK_FAIL(ret, "");

    {
        tt_queue_iter_t pos;
        tt_queue_iter(&q, &pos);
        tt_u32_t *pu32;
        i = 0;
        while ((pu32 = tt_queue_iter_next(&pos)) != NULL) {
            TT_TEST_CHECK_EQUAL(*pu32, v[i++], "");
        }
        TT_TEST_CHECK_EQUAL(i, 0, "");
    }

    for (i = 0; i < __q_size; ++i) {
        ret = tt_queue_push(&q, &v[i]);
        TT_TEST_CHECK_SUCCESS(ret, "");
        TT_TEST_CHECK_EQUAL(*(tt_u32_t *)tt_queue_head(&q), v[0], "");
        TT_TEST_CHECK_EQUAL(*(tt_u32_t *)tt_queue_tail(&q), v[i], "");
    }
    TT_TEST_CHECK_EQUAL(tt_queue_count(&q), __q_size, "");

    {
        tt_queue_iter_t pos;
        tt_queue_iter(&q, &pos);
        tt_u32_t *pu32;
        i = 0;
        while ((pu32 = tt_queue_iter_next(&pos)) != NULL) {
            TT_TEST_CHECK_EQUAL(*pu32, v[i++], "");
        }
        TT_TEST_CHECK_EQUAL(i, __q_size, "");
    }

    n = tt_rand_u32() % __q_size;
    for (i = 0; i < n; ++i) {
        ret = tt_queue_pop(&q, &val);
        TT_TEST_CHECK_SUCCESS(ret, "");
        TT_TEST_CHECK_EQUAL(val, v[i], "");
        TT_TEST_CHECK_EQUAL(*(tt_u32_t *)tt_queue_head(&q), v[i + 1], "");
        TT_TEST_CHECK_EQUAL(*(tt_u32_t *)tt_queue_tail(&q),
                            v[__q_size - 1],
                            "");
    }
    TT_TEST_CHECK_EQUAL(tt_queue_count(&q), __q_size - n, "");

    for (i = 0; i < n; ++i) {
        ret = tt_queue_push(&q, &v[i]);
        TT_TEST_CHECK_SUCCESS(ret, "");
        TT_TEST_CHECK_EQUAL(*(tt_u32_t *)tt_queue_head(&q), v[n], "");
        TT_TEST_CHECK_EQUAL(*(tt_u32_t *)tt_queue_tail(&q), v[i], "");
    }
    TT_TEST_CHECK_EQUAL(tt_queue_count(&q), __q_size, "");

    for (i = 0; i < __q_size - __qf_size; ++i) {
        ret = tt_queue_pop(&q, &val);
        if (i < (__q_size - n)) {
            TT_TEST_CHECK_EQUAL(val, v[i + n], "");
        } else {
            TT_TEST_CHECK_EQUAL(val, v[i + n - __q_size], "");
        }
    }
    TT_TEST_CHECK_EQUAL(tt_queue_count(&q), __qf_size, "");

    tt_queue_clear(&q);
    TT_TEST_CHECK_EQUAL(tt_queue_count(&q), 0, "");
    TT_TEST_CHECK_EQUAL(tt_queue_empty(&q), TT_TRUE, "");
    TT_TEST_CHECK_EQUAL(tt_queue_head(&q), NULL, "");
    TT_TEST_CHECK_EQUAL(tt_queue_tail(&q), NULL, "");

    ret = tt_queue_pop(&q, &i);
    TT_TEST_CHECK_FAIL(ret, "");

    tt_queue_destroy(&q);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_ptr_queue)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_u32_t v[__q_size] = {0};
    tt_u32_t i, n, val;
    tt_result_t ret;
    tt_ptrq_t q;
    tt_ptrq_attr_t attr;

    TT_TEST_CASE_ENTER()
    // test start

    for (i = 0; i < __q_size; ++i) {
        v[i] = i;
    }

    tt_ptrq_init(&q, NULL);
    tt_ptrq_clear(&q);
    TT_TEST_CHECK_EQUAL(tt_ptrq_count(&q), 0, "");
    TT_TEST_CHECK_EQUAL(tt_ptrq_empty(&q), TT_TRUE, "");
    TT_TEST_CHECK_EQUAL(tt_ptrq_head(&q), NULL, "");
    TT_TEST_CHECK_EQUAL(tt_ptrq_tail(&q), NULL, "");

    TT_TEST_CHECK_EQUAL(tt_ptrq_pop(&q), NULL, "");

    {
        tt_ptrq_iter_t pos;
        tt_ptrq_iter(&q, &pos);
        i = 0;
        while (tt_ptrq_iter_next(&pos) != NULL) {
            ++i;
        }
        TT_TEST_CHECK_EQUAL(i, 0, "");
    }

    for (i = 0; i < __q_size; ++i) {
        ret = tt_ptrq_push(&q, &v[i]);
        TT_TEST_CHECK_SUCCESS(ret, "");
        TT_TEST_CHECK_EQUAL(tt_ptrq_head(&q), &v[0], "");
        TT_TEST_CHECK_EQUAL(tt_ptrq_tail(&q), &v[i], "");
    }
    TT_TEST_CHECK_EQUAL(tt_ptrq_count(&q), __q_size, "");

    {
        tt_ptrq_iter_t pos;
        tt_ptrq_iter(&q, &pos);
        tt_ptr_t p;
        i = 0;
        while ((p = tt_ptrq_iter_next(&pos)) != NULL) {
            TT_TEST_CHECK_EQUAL(p, &v[i++], "");
        }
        TT_TEST_CHECK_EQUAL(i, __q_size, "");
    }

    n = tt_rand_u32() % __q_size;
    for (i = 0; i < n; ++i) {
        TT_TEST_CHECK_EQUAL(tt_ptrq_pop(&q), &v[i], "");
        TT_TEST_CHECK_EQUAL(tt_ptrq_head(&q), &v[i + 1], "");
        TT_TEST_CHECK_EQUAL(tt_ptrq_tail(&q), &v[__q_size - 1], "");
    }
    TT_TEST_CHECK_EQUAL(tt_ptrq_count(&q), __q_size - n, "");

    for (i = 0; i < n; ++i) {
        ret = tt_ptrq_push(&q, &v[i]);
        TT_TEST_CHECK_SUCCESS(ret, "");
    }
    TT_TEST_CHECK_EQUAL(tt_ptrq_count(&q), __q_size, "");

    for (i = 0; i < __q_size - q.ptr_per_frame; ++i) {
        if (i < (__q_size - n)) {
            TT_TEST_CHECK_EQUAL(tt_ptrq_pop(&q), &v[i + n], "");
        } else {
            TT_TEST_CHECK_EQUAL(tt_ptrq_pop(&q), &v[i + n - __q_size], "");
        }
    }
    TT_TEST_CHECK_EQUAL(tt_ptrq_count(&q), q.ptr_per_frame, "");

    tt_ptrq_clear(&q);
    TT_TEST_CHECK_EQUAL(tt_ptrq_pop(&q), NULL, "");
    TT_TEST_CHECK_EQUAL(tt_ptrq_head(&q), NULL, "");
    TT_TEST_CHECK_EQUAL(tt_ptrq_tail(&q), NULL, "");

    tt_ptrq_destroy(&q);

    // test end
    TT_TEST_CASE_LEAVE()
}
