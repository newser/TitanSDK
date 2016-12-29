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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_list)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_dl_list)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(list_case)

TT_TEST_CASE("tt_unit_test_list",
             "testing: list",
             tt_unit_test_list,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_dl_list",
                 "testing: double linked list",
                 tt_unit_test_dl_list,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(list_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(TEST_UNIT_LIST, 0, list_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(tt_unit_test_list)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(tt_unit_test_list)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_list_t l1;
    tt_lnode_t n1, n2, n3, n4, n5;

    TT_TEST_CASE_ENTER()
    // test start

    tt_list_init(&l1);
    tt_list_clear(&l1);
    TT_TEST_CHECK_EQUAL(tt_list_count(&l1), 0, "");
    TT_TEST_CHECK_EQUAL(tt_list_empty(&l1), TT_TRUE, "");
    TT_TEST_CHECK_EQUAL(tt_list_head(&l1), NULL, "");
    TT_TEST_CHECK_EQUAL(tt_list_tail(&l1), NULL, "");
    TT_TEST_CHECK_EQUAL(tt_list_pop_head(&l1), NULL, "");
    TT_TEST_CHECK_EQUAL(tt_list_pop_tail(&l1), NULL, "");

    tt_lnode_init(&n1);
    tt_lnode_init(&n2);
    tt_lnode_init(&n3);
    tt_lnode_init(&n4);
    tt_lnode_init(&n5);

    tt_list_push_head(&l1, &n1);
    TT_TEST_CHECK_EQUAL(tt_list_count(&l1), 1, "");
    TT_TEST_CHECK_EQUAL(tt_list_empty(&l1), TT_FALSE, "");
    TT_TEST_CHECK_EQUAL(tt_list_head(&l1), &n1, "");
    TT_TEST_CHECK_EQUAL(tt_list_tail(&l1), &n1, "");
    TT_TEST_CHECK_EQUAL(tt_list_pop_tail(&l1), &n1, "");
    // now empty:
    TT_TEST_CHECK_EQUAL(tt_list_count(&l1), 0, "");
    TT_TEST_CHECK_EQUAL(tt_list_empty(&l1), TT_TRUE, "");
    TT_TEST_CHECK_EQUAL(tt_list_head(&l1), NULL, "");
    TT_TEST_CHECK_EQUAL(tt_list_tail(&l1), NULL, "");
    TT_TEST_CHECK_EQUAL(tt_list_pop_head(&l1), NULL, "");
    TT_TEST_CHECK_EQUAL(tt_list_pop_tail(&l1), NULL, "");

    tt_list_push_tail(&l1, &n1);
    TT_TEST_CHECK_EQUAL(tt_list_count(&l1), 1, "");
    TT_TEST_CHECK_EQUAL(tt_list_empty(&l1), TT_FALSE, "");
    TT_TEST_CHECK_EQUAL(tt_list_head(&l1), &n1, "");
    TT_TEST_CHECK_EQUAL(tt_list_tail(&l1), &n1, "");
    TT_TEST_CHECK_EQUAL(tt_list_pop_head(&l1), &n1, "");
    // now empty:
    TT_TEST_CHECK_EQUAL(tt_list_count(&l1), 0, "");
    TT_TEST_CHECK_EQUAL(tt_list_empty(&l1), TT_TRUE, "");
    TT_TEST_CHECK_EQUAL(tt_list_head(&l1), NULL, "");
    TT_TEST_CHECK_EQUAL(tt_list_tail(&l1), NULL, "");
    TT_TEST_CHECK_EQUAL(tt_list_pop_head(&l1), NULL, "");
    TT_TEST_CHECK_EQUAL(tt_list_pop_tail(&l1), NULL, "");

    // multiple nodes:
    tt_list_push_head(&l1, &n1);
    tt_list_insert_before(&n1, &n2);
    tt_list_insert_before(&n1, &n5);
    // n2, n5, n1
    tt_list_insert_after(&n1, &n3);
    tt_list_insert_after(&n1, &n4);
    // n2, n5, n1, n4, n3
    TT_TEST_CHECK_EQUAL(tt_list_count(&l1), 5, "");
    TT_TEST_CHECK_EQUAL(tt_list_empty(&l1), TT_FALSE, "");
    TT_TEST_CHECK_EQUAL(tt_list_head(&l1), &n2, "");
    TT_TEST_CHECK_EQUAL(tt_list_tail(&l1), &n3, "");

    // remove n5: n2,n1,n4,n3
    TT_TEST_CHECK_EQUAL(tt_list_remove(&n5), &n1, "");
    TT_TEST_CHECK_EQUAL(tt_list_count(&l1), 4, "");
    TT_TEST_CHECK_EQUAL(tt_list_empty(&l1), TT_FALSE, "");
    TT_TEST_CHECK_EQUAL(tt_list_head(&l1), &n2, "");
    TT_TEST_CHECK_EQUAL(tt_list_tail(&l1), &n3, "");

    TT_TEST_CHECK_EQUAL(tt_list_remove(&n5), NULL, "");

    // remove n2: n1,n4,n3
    TT_TEST_CHECK_EQUAL(tt_list_remove(&n2), &n1, "");
    TT_TEST_CHECK_EQUAL(tt_list_count(&l1), 3, "");
    TT_TEST_CHECK_EQUAL(tt_list_empty(&l1), TT_FALSE, "");
    TT_TEST_CHECK_EQUAL(tt_list_head(&l1), &n1, "");
    TT_TEST_CHECK_EQUAL(tt_list_tail(&l1), &n3, "");

    // remove n4: n1,n3
    TT_TEST_CHECK_EQUAL(tt_list_remove(&n4), &n3, "");
    TT_TEST_CHECK_EQUAL(tt_list_count(&l1), 2, "");
    TT_TEST_CHECK_EQUAL(tt_list_empty(&l1), TT_FALSE, "");
    TT_TEST_CHECK_EQUAL(tt_list_head(&l1), &n1, "");
    TT_TEST_CHECK_EQUAL(tt_list_tail(&l1), &n3, "");

    // remove n3: n1
    TT_TEST_CHECK_EQUAL(tt_list_remove(&n3), NULL, "");
    TT_TEST_CHECK_EQUAL(tt_list_count(&l1), 1, "");
    TT_TEST_CHECK_EQUAL(tt_list_empty(&l1), TT_FALSE, "");
    TT_TEST_CHECK_EQUAL(tt_list_head(&l1), &n1, "");
    TT_TEST_CHECK_EQUAL(tt_list_tail(&l1), &n1, "");

    tt_list_clear(&l1);
    TT_TEST_CHECK_EQUAL(tt_list_remove(&n1), NULL, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_dl_list)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_dlist_t l1;
    tt_dnode_t n1, n2, n3, n4, n5;

    TT_TEST_CASE_ENTER()
    // test start

    tt_dlist_init(&l1);
    tt_dlist_clear(&l1);
    TT_TEST_CHECK_EQUAL(tt_dlist_count(&l1), 0, "");
    TT_TEST_CHECK_EQUAL(tt_dlist_empty(&l1), TT_TRUE, "");
    TT_TEST_CHECK_EQUAL(tt_dlist_head(&l1), NULL, "");
    TT_TEST_CHECK_EQUAL(tt_dlist_tail(&l1), NULL, "");
    TT_TEST_CHECK_EQUAL(tt_dlist_pop_head(&l1), NULL, "");
    TT_TEST_CHECK_EQUAL(tt_dlist_pop_tail(&l1), NULL, "");

    tt_dnode_init(&n1);
    tt_dnode_init(&n2);
    tt_dnode_init(&n3);
    tt_dnode_init(&n4);
    tt_dnode_init(&n5);

    tt_dlist_push_head(&l1, &n1);
    TT_TEST_CHECK_EQUAL(tt_dlist_count(&l1), 1, "");
    TT_TEST_CHECK_EQUAL(tt_dlist_empty(&l1), TT_FALSE, "");
    TT_TEST_CHECK_EQUAL(tt_dlist_head(&l1), &n1, "");
    TT_TEST_CHECK_EQUAL(tt_dlist_tail(&l1), &n1, "");
    TT_TEST_CHECK_EQUAL(tt_dlist_pop_tail(&l1), &n1, "");
    // now empty:
    TT_TEST_CHECK_EQUAL(tt_dlist_count(&l1), 0, "");
    TT_TEST_CHECK_EQUAL(tt_dlist_empty(&l1), TT_TRUE, "");
    TT_TEST_CHECK_EQUAL(tt_dlist_head(&l1), NULL, "");
    TT_TEST_CHECK_EQUAL(tt_dlist_tail(&l1), NULL, "");
    TT_TEST_CHECK_EQUAL(tt_dlist_pop_head(&l1), NULL, "");
    TT_TEST_CHECK_EQUAL(tt_dlist_pop_tail(&l1), NULL, "");

    tt_dlist_push_tail(&l1, &n1);
    TT_TEST_CHECK_EQUAL(tt_dlist_count(&l1), 1, "");
    TT_TEST_CHECK_EQUAL(tt_dlist_empty(&l1), TT_FALSE, "");
    TT_TEST_CHECK_EQUAL(tt_dlist_head(&l1), &n1, "");
    TT_TEST_CHECK_EQUAL(tt_dlist_tail(&l1), &n1, "");
    TT_TEST_CHECK_EQUAL(tt_dlist_pop_head(&l1), &n1, "");
    // now empty:
    TT_TEST_CHECK_EQUAL(tt_dlist_count(&l1), 0, "");
    TT_TEST_CHECK_EQUAL(tt_dlist_empty(&l1), TT_TRUE, "");
    TT_TEST_CHECK_EQUAL(tt_dlist_head(&l1), NULL, "");
    TT_TEST_CHECK_EQUAL(tt_dlist_tail(&l1), NULL, "");
    TT_TEST_CHECK_EQUAL(tt_dlist_pop_head(&l1), NULL, "");
    TT_TEST_CHECK_EQUAL(tt_dlist_pop_tail(&l1), NULL, "");

    // multiple nodes:
    tt_dlist_push_head(&l1, &n1);
    tt_dlist_insert_before(&n1, &n2);
    tt_dlist_insert_before(&n1, &n5);
    // n2, n5, n1
    tt_dlist_insert_after(&n1, &n3);
    tt_dlist_insert_after(&n1, &n4);
    // n2, n5, n1, n4, n3
    TT_TEST_CHECK_EQUAL(tt_dlist_count(&l1), 5, "");
    TT_TEST_CHECK_EQUAL(tt_dlist_empty(&l1), TT_FALSE, "");
    TT_TEST_CHECK_EQUAL(tt_dlist_head(&l1), &n2, "");
    TT_TEST_CHECK_EQUAL(tt_dlist_tail(&l1), &n3, "");

    // remove n5: n2,n1,n4,n3
    tt_dlist_remove(&n5);
    TT_TEST_CHECK_EQUAL(tt_dlist_count(&l1), 4, "");
    TT_TEST_CHECK_EQUAL(tt_dlist_empty(&l1), TT_FALSE, "");
    TT_TEST_CHECK_EQUAL(tt_dlist_head(&l1), &n2, "");
    TT_TEST_CHECK_EQUAL(tt_dlist_tail(&l1), &n3, "");

    tt_dlist_remove(&n5);

    // remove n2: n1,n4,n3
    tt_dlist_remove(&n2);
    TT_TEST_CHECK_EQUAL(tt_dlist_count(&l1), 3, "");
    TT_TEST_CHECK_EQUAL(tt_dlist_empty(&l1), TT_FALSE, "");
    TT_TEST_CHECK_EQUAL(tt_dlist_head(&l1), &n1, "");
    TT_TEST_CHECK_EQUAL(tt_dlist_tail(&l1), &n3, "");

    // remove n4: n1,n3
    tt_dlist_remove(&n4);
    TT_TEST_CHECK_EQUAL(tt_dlist_count(&l1), 2, "");
    TT_TEST_CHECK_EQUAL(tt_dlist_empty(&l1), TT_FALSE, "");
    TT_TEST_CHECK_EQUAL(tt_dlist_head(&l1), &n1, "");
    TT_TEST_CHECK_EQUAL(tt_dlist_tail(&l1), &n3, "");

    // remove n3: n1
    tt_dlist_remove(&n3);
    TT_TEST_CHECK_EQUAL(tt_dlist_count(&l1), 1, "");
    TT_TEST_CHECK_EQUAL(tt_dlist_empty(&l1), TT_FALSE, "");
    TT_TEST_CHECK_EQUAL(tt_dlist_head(&l1), &n1, "");
    TT_TEST_CHECK_EQUAL(tt_dlist_tail(&l1), &n1, "");

    tt_dlist_clear(&l1);
    tt_dlist_remove(&n1);

    // test end
    TT_TEST_CASE_LEAVE()
}
