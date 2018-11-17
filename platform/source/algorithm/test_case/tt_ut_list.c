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
TT_TEST_ROUTINE_DECLARE(case_list)
TT_TEST_ROUTINE_DECLARE(case_dl_list)
TT_TEST_ROUTINE_DECLARE(case_sl_list)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(list_case)

TT_TEST_CASE(
    "case_list", "testing: list", case_list, NULL, NULL, NULL, NULL, NULL)
,

    TT_TEST_CASE("case_dl_list",
                 "testing: double linked list",
                 case_dl_list,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_sl_list",
                 "testing: single linked list",
                 case_sl_list,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(list_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(ALG_UT_LIST, 0, list_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(case_list)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(case_list)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_list_t l1;
    tt_lnode_t n1, n2, n3, n4, n5;

    TT_TEST_CASE_ENTER()
    // test start

    tt_list_init(&l1);
    tt_list_clear(&l1);
    TT_UT_EQUAL(tt_list_count(&l1), 0, "");
    TT_UT_EQUAL(tt_list_empty(&l1), TT_TRUE, "");
    TT_UT_EQUAL(tt_list_head(&l1), NULL, "");
    TT_UT_EQUAL(tt_list_tail(&l1), NULL, "");
    TT_UT_EQUAL(tt_list_pop_head(&l1), NULL, "");
    TT_UT_EQUAL(tt_list_pop_tail(&l1), NULL, "");

    tt_lnode_init(&n1);
    tt_lnode_init(&n2);
    tt_lnode_init(&n3);
    tt_lnode_init(&n4);
    tt_lnode_init(&n5);

    tt_list_push_head(&l1, &n1);
    TT_UT_EQUAL(tt_list_count(&l1), 1, "");
    TT_UT_EQUAL(tt_list_empty(&l1), TT_FALSE, "");
    TT_UT_EQUAL(tt_list_head(&l1), &n1, "");
    TT_UT_EQUAL(tt_list_tail(&l1), &n1, "");
    TT_UT_EQUAL(tt_list_pop_tail(&l1), &n1, "");
    // now empty:
    TT_UT_EQUAL(tt_list_count(&l1), 0, "");
    TT_UT_EQUAL(tt_list_empty(&l1), TT_TRUE, "");
    TT_UT_EQUAL(tt_list_head(&l1), NULL, "");
    TT_UT_EQUAL(tt_list_tail(&l1), NULL, "");
    TT_UT_EQUAL(tt_list_pop_head(&l1), NULL, "");
    TT_UT_EQUAL(tt_list_pop_tail(&l1), NULL, "");

    tt_list_push_tail(&l1, &n1);
    TT_UT_EQUAL(tt_list_count(&l1), 1, "");
    TT_UT_EQUAL(tt_list_empty(&l1), TT_FALSE, "");
    TT_UT_EQUAL(tt_list_head(&l1), &n1, "");
    TT_UT_EQUAL(tt_list_tail(&l1), &n1, "");
    TT_UT_EQUAL(tt_list_pop_head(&l1), &n1, "");
    // now empty:
    TT_UT_EQUAL(tt_list_count(&l1), 0, "");
    TT_UT_EQUAL(tt_list_empty(&l1), TT_TRUE, "");
    TT_UT_EQUAL(tt_list_head(&l1), NULL, "");
    TT_UT_EQUAL(tt_list_tail(&l1), NULL, "");
    TT_UT_EQUAL(tt_list_pop_head(&l1), NULL, "");
    TT_UT_EQUAL(tt_list_pop_tail(&l1), NULL, "");

    // multiple nodes:
    tt_list_push_head(&l1, &n1);
    tt_list_insert_before(&n1, &n2);
    tt_list_insert_before(&n1, &n5);
    // n2, n5, n1
    tt_list_insert_after(&n1, &n3);
    tt_list_insert_after(&n1, &n4);
    // n2, n5, n1, n4, n3
    TT_UT_EQUAL(tt_list_count(&l1), 5, "");
    TT_UT_EQUAL(tt_list_empty(&l1), TT_FALSE, "");
    TT_UT_EQUAL(tt_list_head(&l1), &n2, "");
    TT_UT_EQUAL(tt_list_tail(&l1), &n3, "");

    // remove n5: n2,n1,n4,n3
    TT_UT_EQUAL(tt_list_remove(&n5), &n1, "");
    TT_UT_EQUAL(tt_list_count(&l1), 4, "");
    TT_UT_EQUAL(tt_list_empty(&l1), TT_FALSE, "");
    TT_UT_EQUAL(tt_list_head(&l1), &n2, "");
    TT_UT_EQUAL(tt_list_tail(&l1), &n3, "");

    TT_UT_EQUAL(tt_list_remove(&n5), NULL, "");

    // remove n2: n1,n4,n3
    TT_UT_EQUAL(tt_list_remove(&n2), &n1, "");
    TT_UT_EQUAL(tt_list_count(&l1), 3, "");
    TT_UT_EQUAL(tt_list_empty(&l1), TT_FALSE, "");
    TT_UT_EQUAL(tt_list_head(&l1), &n1, "");
    TT_UT_EQUAL(tt_list_tail(&l1), &n3, "");

    // remove n4: n1,n3
    TT_UT_EQUAL(tt_list_remove(&n4), &n3, "");
    TT_UT_EQUAL(tt_list_count(&l1), 2, "");
    TT_UT_EQUAL(tt_list_empty(&l1), TT_FALSE, "");
    TT_UT_EQUAL(tt_list_head(&l1), &n1, "");
    TT_UT_EQUAL(tt_list_tail(&l1), &n3, "");

    // remove n3: n1
    TT_UT_EQUAL(tt_list_remove(&n3), NULL, "");
    TT_UT_EQUAL(tt_list_count(&l1), 1, "");
    TT_UT_EQUAL(tt_list_empty(&l1), TT_FALSE, "");
    TT_UT_EQUAL(tt_list_head(&l1), &n1, "");
    TT_UT_EQUAL(tt_list_tail(&l1), &n1, "");

    tt_list_clear(&l1);
    TT_UT_EQUAL(tt_list_remove(&n1), NULL, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_dl_list)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_dlist_t l1, l2;
    tt_dnode_t n1, n2, n3, n4, n5;

    TT_TEST_CASE_ENTER()
    // test start

    tt_dlist_init(&l1);
    tt_dlist_init(&l2);
    tt_dlist_clear(&l1);
    TT_UT_EQUAL(tt_dlist_count(&l1), 0, "");
    TT_UT_EQUAL(tt_dlist_empty(&l1), TT_TRUE, "");
    TT_UT_EQUAL(tt_dlist_head(&l1), NULL, "");
    TT_UT_EQUAL(tt_dlist_tail(&l1), NULL, "");
    TT_UT_EQUAL(tt_dlist_pop_head(&l1), NULL, "");
    TT_UT_EQUAL(tt_dlist_pop_tail(&l1), NULL, "");

    tt_dnode_init(&n1);
    tt_dnode_init(&n2);
    tt_dnode_init(&n3);
    tt_dnode_init(&n4);
    tt_dnode_init(&n5);

    tt_dlist_push_head(&l1, &n1);
    TT_UT_EQUAL(tt_dlist_count(&l1), 1, "");
    TT_UT_EQUAL(tt_dlist_empty(&l1), TT_FALSE, "");
    TT_UT_EQUAL(tt_dlist_head(&l1), &n1, "");
    TT_UT_EQUAL(tt_dlist_tail(&l1), &n1, "");
    TT_UT_EQUAL(tt_dlist_pop_tail(&l1), &n1, "");
    // now empty:
    TT_UT_EQUAL(tt_dlist_count(&l1), 0, "");
    TT_UT_EQUAL(tt_dlist_empty(&l1), TT_TRUE, "");
    TT_UT_EQUAL(tt_dlist_head(&l1), NULL, "");
    TT_UT_EQUAL(tt_dlist_tail(&l1), NULL, "");
    TT_UT_EQUAL(tt_dlist_pop_head(&l1), NULL, "");
    TT_UT_EQUAL(tt_dlist_pop_tail(&l1), NULL, "");

    tt_dlist_push_tail(&l1, &n1);
    TT_UT_EQUAL(tt_dlist_count(&l1), 1, "");
    TT_UT_EQUAL(tt_dlist_empty(&l1), TT_FALSE, "");
    TT_UT_EQUAL(tt_dlist_head(&l1), &n1, "");
    TT_UT_EQUAL(tt_dlist_tail(&l1), &n1, "");
    TT_UT_EQUAL(tt_dlist_pop_head(&l1), &n1, "");
    // now empty:
    TT_UT_EQUAL(tt_dlist_count(&l1), 0, "");
    TT_UT_EQUAL(tt_dlist_empty(&l1), TT_TRUE, "");
    TT_UT_EQUAL(tt_dlist_head(&l1), NULL, "");
    TT_UT_EQUAL(tt_dlist_tail(&l1), NULL, "");
    TT_UT_EQUAL(tt_dlist_pop_head(&l1), NULL, "");
    TT_UT_EQUAL(tt_dlist_pop_tail(&l1), NULL, "");

    // multiple nodes:
    tt_dlist_push_head(&l1, &n1);
    tt_dlist_insert_before(&l1, &n1, &n2);
    tt_dlist_insert_before(&l1, &n1, &n5);
    // n2, n5, n1
    tt_dlist_insert_after(&l1, &n1, &n3);
    tt_dlist_insert_after(&l1, &n1, &n4);
    // n2, n5, n1, n4, n3
    TT_UT_EQUAL(tt_dlist_count(&l1), 5, "");
    TT_UT_EQUAL(tt_dlist_empty(&l1), TT_FALSE, "");
    TT_UT_EQUAL(tt_dlist_head(&l1), &n2, "");
    TT_UT_EQUAL(tt_dlist_tail(&l1), &n3, "");

    // remove n5: n2,n1,n4,n3
    TT_UT_TRUE(tt_dnode_in_dlist(&n5), "");
    TT_UT_EQUAL(tt_dlist_remove(&l1, &n5), &n1, "");
    TT_UT_FALSE(tt_dnode_in_dlist(&n5), "");
    TT_UT_EQUAL(tt_dlist_count(&l1), 4, "");
    TT_UT_EQUAL(tt_dlist_empty(&l1), TT_FALSE, "");
    TT_UT_EQUAL(tt_dlist_head(&l1), &n2, "");
    TT_UT_EQUAL(tt_dlist_tail(&l1), &n3, "");

    tt_dlist_remove(&l1, &n5);
    TT_UT_EQUAL(tt_dlist_remove(&l1, &n5), NULL, "");

    // remove n2: n1,n4,n3
    TT_UT_EQUAL(tt_dlist_remove(&l1, &n2), &n1, "");
    TT_UT_EQUAL(tt_dlist_count(&l1), 3, "");
    TT_UT_EQUAL(tt_dlist_empty(&l1), TT_FALSE, "");
    TT_UT_EQUAL(tt_dlist_head(&l1), &n1, "");
    TT_UT_EQUAL(tt_dlist_tail(&l1), &n3, "");

    // remove n4: n1,n3
    TT_UT_EQUAL(tt_dlist_remove(&l1, &n4), &n3, "");
    TT_UT_EQUAL(tt_dlist_count(&l1), 2, "");
    TT_UT_EQUAL(tt_dlist_empty(&l1), TT_FALSE, "");
    TT_UT_EQUAL(tt_dlist_head(&l1), &n1, "");
    TT_UT_EQUAL(tt_dlist_tail(&l1), &n3, "");

    // remove n3: n1
    TT_UT_EQUAL(tt_dlist_remove(&l1, &n3), NULL, "");
    TT_UT_EQUAL(tt_dlist_count(&l1), 1, "");
    TT_UT_EQUAL(tt_dlist_empty(&l1), TT_FALSE, "");
    TT_UT_EQUAL(tt_dlist_head(&l1), &n1, "");
    TT_UT_EQUAL(tt_dlist_tail(&l1), &n1, "");

    tt_dlist_clear(&l1);
    TT_UT_EQUAL(tt_dlist_remove(&l1, &n1), NULL, "");

    // move
    tt_dlist_move(&l1, &l2);
    TT_UT_EQUAL(tt_dlist_empty(&l1), TT_TRUE, "");
    TT_UT_EQUAL(tt_dlist_empty(&l2), TT_TRUE, "");

    // move to empty
    tt_dlist_push_tail(&l2, &n1);
    tt_dlist_push_tail(&l2, &n2);
    tt_dlist_push_tail(&l2, &n3);
    tt_dlist_move(&l1, &l2);
    TT_UT_EQUAL(tt_dlist_count(&l1), 3, "");
    TT_UT_EQUAL(tt_dlist_head(&l1), &n1, "");
    TT_UT_EQUAL(tt_dlist_tail(&l1), &n3, "");
    TT_UT_EQUAL(tt_dlist_empty(&l2), TT_TRUE, "");
    tt_dlist_clear(&l1);

    // move from empty
    tt_dlist_push_tail(&l1, &n1);
    tt_dlist_push_tail(&l1, &n2);
    tt_dlist_move(&l1, &l2);
    TT_UT_EQUAL(tt_dlist_count(&l1), 2, "");
    TT_UT_EQUAL(tt_dlist_head(&l1), &n1, "");
    TT_UT_EQUAL(tt_dlist_tail(&l1), &n2, "");
    TT_UT_EQUAL(tt_dlist_empty(&l2), TT_TRUE, "");

    // move
    tt_dlist_push_tail(&l2, &n3);
    tt_dlist_push_tail(&l2, &n4);
    tt_dlist_move(&l1, &l2);
    TT_UT_EQUAL(tt_dlist_count(&l1), 4, "");
    TT_UT_EQUAL(tt_dlist_head(&l1), &n1, "");
    TT_UT_EQUAL(tt_dlist_tail(&l1), &n4, "");
    TT_UT_EQUAL(tt_dlist_empty(&l2), TT_TRUE, "");

    // swap
    tt_dlist_clear(&l1);
    tt_dlist_clear(&l2);
    tt_dlist_swap(&l1, &l2);
    TT_UT_TRUE(tt_dlist_empty(&l1), "");
    TT_UT_TRUE(tt_dlist_empty(&l2), "");

    tt_dlist_push_tail(&l1, &n1);
    tt_dlist_push_tail(&l1, &n2);
    tt_dlist_push_tail(&l1, &n3);
    tt_dlist_swap(&l1, &l2);
    TT_UT_EQUAL(tt_dlist_count(&l2), 3, "");
    TT_UT_EQUAL(tt_dlist_head(&l2), &n1, "");
    TT_UT_EQUAL(tt_dlist_tail(&l2), &n3, "");
    TT_UT_EQUAL(tt_dlist_empty(&l1), TT_TRUE, "");

    tt_dlist_push_tail(&l1, &n4);
    tt_dlist_swap(&l1, &l2);
    TT_UT_EQUAL(tt_dlist_count(&l1), 3, "");
    TT_UT_EQUAL(tt_dlist_head(&l1), &n1, "");
    TT_UT_EQUAL(tt_dlist_tail(&l1), &n3, "");
    TT_UT_EQUAL(tt_dlist_count(&l2), 1, "");
    TT_UT_EQUAL(tt_dlist_head(&l2), &n4, "");
    TT_UT_EQUAL(tt_dlist_tail(&l2), &n4, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_sl_list)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_slist_t l1, l2;
    tt_snode_t n1, n2, n3, n4, n5;
    tt_u32_t num;

    TT_TEST_CASE_ENTER()
    // test start

    tt_slist_init(&l1);
    tt_slist_init(&l2);
    tt_slist_clear(&l1);
    TT_UT_EQUAL(tt_slist_count(&l1), 0, "");
    TT_UT_EQUAL(tt_slist_empty(&l1), TT_TRUE, "");
    TT_UT_EQUAL(tt_slist_head(&l1), NULL, "");
    TT_UT_EQUAL(tt_slist_tail(&l1), NULL, "");
    TT_UT_EQUAL(tt_slist_pop_head(&l1), NULL, "");
    TT_UT_EQUAL(tt_slist_pop_tail(&l1), NULL, "");

    tt_snode_init(&n1);
    tt_snode_init(&n2);
    tt_snode_init(&n3);
    tt_snode_init(&n4);
    tt_snode_init(&n5);

    tt_slist_push_head(&l1, &n1);
    TT_UT_EQUAL(tt_slist_count(&l1), 1, "");
    TT_UT_EQUAL(tt_slist_empty(&l1), TT_FALSE, "");
    TT_UT_EQUAL(tt_slist_head(&l1), &n1, "");
    TT_UT_EQUAL(tt_slist_tail(&l1), &n1, "");
    TT_UT_EQUAL(n1.next, NULL, "");
    TT_UT_EQUAL(tt_slist_pop_tail(&l1), &n1, "");
    // now empty:
    TT_UT_EQUAL(tt_slist_count(&l1), 0, "");
    TT_UT_EQUAL(tt_slist_empty(&l1), TT_TRUE, "");
    TT_UT_EQUAL(tt_slist_head(&l1), NULL, "");
    TT_UT_EQUAL(tt_slist_tail(&l1), NULL, "");
    TT_UT_EQUAL(tt_slist_pop_head(&l1), NULL, "");
    TT_UT_EQUAL(tt_slist_pop_tail(&l1), NULL, "");

    tt_slist_push_tail(&l1, &n1);
    TT_UT_EQUAL(tt_slist_count(&l1), 1, "");
    TT_UT_EQUAL(tt_slist_empty(&l1), TT_FALSE, "");
    TT_UT_EQUAL(tt_slist_head(&l1), &n1, "");
    TT_UT_EQUAL(tt_slist_tail(&l1), &n1, "");
    TT_UT_EQUAL(tt_slist_pop_head(&l1), &n1, "");
    TT_UT_EQUAL(n1.next, NULL, "");
    // now empty:
    TT_UT_EQUAL(tt_slist_count(&l1), 0, "");
    TT_UT_EQUAL(tt_slist_empty(&l1), TT_TRUE, "");
    TT_UT_EQUAL(tt_slist_head(&l1), NULL, "");
    TT_UT_EQUAL(tt_slist_tail(&l1), NULL, "");
    TT_UT_EQUAL(tt_slist_pop_head(&l1), NULL, "");
    TT_UT_EQUAL(tt_slist_pop_tail(&l1), NULL, "");

    // multiple nodes:
    tt_slist_push_head(&l1, &n1);
    tt_slist_push_head(&l1, &n2);
    tt_slist_insert_after(&n2, &n5);
    // n2, n5, n1
    tt_slist_insert_after(&n1, &n3);
    tt_slist_insert_after(&n1, &n4);
    // n2, n5, n1, n4, n3
    TT_UT_EQUAL(tt_slist_count(&l1), 5, "");
    TT_UT_EQUAL(tt_slist_empty(&l1), TT_FALSE, "");
    TT_UT_EQUAL(tt_slist_head(&l1), &n2, "");
    TT_UT_EQUAL(tt_slist_tail(&l1), &n3, "");
    TT_UT_EQUAL(n2.next, &n5, "");
    TT_UT_EQUAL(n1.next, &n4, "");
    TT_UT_EQUAL(n3.next, NULL, "");

    // remove n5: n2,n1,n4,n3
    TT_UT_EQUAL(tt_slist_fast_remove(&l1, &n2, &n5), &n1, "");
    TT_UT_EQUAL(tt_slist_count(&l1), 4, "");
    TT_UT_EQUAL(tt_slist_empty(&l1), TT_FALSE, "");
    TT_UT_EQUAL(tt_slist_head(&l1), &n2, "");
    TT_UT_EQUAL(tt_slist_tail(&l1), &n3, "");

    // remove n2: n1,n4,n3
    TT_UT_EQUAL(tt_slist_remove(&l1, &n2), &n1, "");
    TT_UT_EQUAL(tt_slist_count(&l1), 3, "");
    TT_UT_EQUAL(tt_slist_empty(&l1), TT_FALSE, "");
    TT_UT_EQUAL(tt_slist_head(&l1), &n1, "");
    TT_UT_EQUAL(tt_slist_tail(&l1), &n3, "");

    // remove n4: n1,n3
    TT_UT_EQUAL(tt_slist_fast_remove(&l1, &n1, &n4), &n3, "");
    TT_UT_EQUAL(tt_slist_count(&l1), 2, "");
    TT_UT_EQUAL(tt_slist_empty(&l1), TT_FALSE, "");
    TT_UT_EQUAL(tt_slist_head(&l1), &n1, "");
    TT_UT_EQUAL(tt_slist_tail(&l1), &n3, "");
    TT_UT_EQUAL(n3.next, NULL, "");
    TT_UT_EQUAL(n1.next, &n3, "");

    // remove n1: n3
    TT_UT_EQUAL(tt_slist_remove(&l1, &n1), &n3, "");
    TT_UT_EQUAL(tt_slist_count(&l1), 1, "");
    TT_UT_EQUAL(tt_slist_empty(&l1), TT_FALSE, "");
    TT_UT_EQUAL(tt_slist_head(&l1), &n3, "");
    TT_UT_EQUAL(tt_slist_tail(&l1), &n3, "");

    TT_UT_EQUAL(tt_slist_fast_remove(&l1, NULL, &n3), NULL, "");
    TT_UT_EQUAL(tt_slist_count(&l1), 0, "");
    TT_UT_EQUAL(tt_slist_empty(&l1), TT_TRUE, "");
    TT_UT_EQUAL(tt_slist_head(&l1), NULL, "");
    TT_UT_EQUAL(tt_slist_tail(&l1), NULL, "");

    tt_slist_clear(&l1);

    // move
    tt_slist_move(&l1, &l2);
    TT_UT_EQUAL(tt_slist_empty(&l1), TT_TRUE, "");
    TT_UT_EQUAL(tt_slist_empty(&l2), TT_TRUE, "");

    // move to empty
    tt_slist_push_tail(&l2, &n1);
    tt_slist_push_tail(&l2, &n2);
    tt_slist_push_tail(&l2, &n3);
    tt_slist_move(&l1, &l2);
    TT_UT_EQUAL(tt_slist_count(&l1), 3, "");
    TT_UT_EQUAL(tt_slist_head(&l1), &n1, "");
    TT_UT_EQUAL(tt_slist_tail(&l1), &n3, "");
    TT_UT_EQUAL(tt_slist_empty(&l2), TT_TRUE, "");
    tt_slist_clear(&l1);

    // move from empty
    tt_slist_push_tail(&l1, &n1);
    tt_slist_push_tail(&l1, &n2);
    tt_slist_move(&l1, &l2);
    TT_UT_EQUAL(tt_slist_count(&l1), 2, "");
    TT_UT_EQUAL(tt_slist_head(&l1), &n1, "");
    TT_UT_EQUAL(tt_slist_tail(&l1), &n2, "");
    TT_UT_EQUAL(tt_slist_empty(&l2), TT_TRUE, "");

    // move
    tt_slist_push_tail(&l2, &n3);
    tt_slist_push_tail(&l2, &n4);
    tt_slist_move(&l1, &l2);
    TT_UT_EQUAL(tt_slist_count(&l1), 4, "");
    TT_UT_EQUAL(tt_slist_head(&l1), &n1, "");
    TT_UT_EQUAL(tt_slist_tail(&l1), &n4, "");
    TT_UT_EQUAL(tt_slist_empty(&l2), TT_TRUE, "");

    // swap
    tt_slist_swap(&l1, &l2);
    TT_UT_EQUAL(tt_slist_count(&l2), 4, "");
    TT_UT_EQUAL(tt_slist_head(&l2), &n1, "");
    TT_UT_EQUAL(tt_slist_tail(&l2), &n4, "");
    TT_UT_EQUAL(tt_slist_empty(&l1), TT_TRUE, "");

    tt_slist_clear(&l2);
    tt_slist_swap(&l1, &l2);
    TT_UT_EQUAL(tt_slist_empty(&l1), TT_TRUE, "");
    TT_UT_EQUAL(tt_slist_empty(&l2), TT_TRUE, "");

    // move count
    tt_slist_push_tail(&l2, &n1);
    tt_slist_push_tail(&l2, &n2);
    tt_slist_push_tail(&l2, &n3);
    tt_slist_push_tail(&l2, &n4);

    num = tt_slist_move_count(&l1, &l2, 0);
    TT_UT_EQUAL(num, 0, "");
    TT_UT_EQUAL(tt_slist_count(&l1), 0, "");
    TT_UT_EQUAL(tt_slist_count(&l2), 4, "");

    num = tt_slist_move_count(&l1, &l2, 1);
    TT_UT_EQUAL(num, 1, "");
    TT_UT_EQUAL(tt_slist_count(&l1), 1, "");
    TT_UT_EQUAL(tt_slist_head(&l1), &n1, "");
    TT_UT_EQUAL(tt_slist_tail(&l1), &n1, "");
    TT_UT_EQUAL(tt_slist_count(&l2), 3, "");
    TT_UT_EQUAL(tt_slist_head(&l2), &n2, "");
    TT_UT_EQUAL(tt_slist_tail(&l2), &n4, "");

    num = tt_slist_move_count(&l1, &l2, 100);
    TT_UT_EQUAL(num, 3, "");
    TT_UT_EQUAL(tt_slist_count(&l1), 4, "");
    TT_UT_EQUAL(tt_slist_head(&l1), &n1, "");
    TT_UT_EQUAL(tt_slist_tail(&l1), &n4, "");
    TT_UT_EQUAL(tt_slist_count(&l2), 0, "");
    TT_UT_EQUAL(tt_slist_head(&l2), NULL, "");
    TT_UT_EQUAL(tt_slist_tail(&l2), NULL, "");

    // move empty to non_empty
    num = tt_slist_move_count(&l1, &l2, 100);
    TT_UT_EQUAL(num, 0, "");
    TT_UT_EQUAL(tt_slist_count(&l1), 4, "");
    TT_UT_EQUAL(tt_slist_head(&l1), &n1, "");
    TT_UT_EQUAL(tt_slist_tail(&l1), &n4, "");
    TT_UT_EQUAL(tt_slist_count(&l2), 0, "");
    TT_UT_EQUAL(tt_slist_head(&l2), NULL, "");
    TT_UT_EQUAL(tt_slist_tail(&l2), NULL, "");

    num = tt_slist_move_count(&l1, &l1, 100);
    TT_UT_EQUAL(num, 0, "");
    TT_UT_EQUAL(tt_slist_count(&l1), 4, "");
    TT_UT_EQUAL(tt_slist_head(&l1), &n1, "");
    TT_UT_EQUAL(tt_slist_tail(&l1), &n4, "");

    // test end
    TT_TEST_CASE_LEAVE()
}
