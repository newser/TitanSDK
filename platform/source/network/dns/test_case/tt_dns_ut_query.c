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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_dns_query)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(dns_query_case)

TT_TEST_CASE("tt_unit_test_dns_query",
             "dns query",
             tt_unit_test_dns_query,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE_LIST_DEFINE_END(dns_query_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(DNS_UT_QUERY, 0, dns_query_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(tt_unit_test_dns_query)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

tt_result_t __dns_query_1(IN void *param)
{
    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_dns_query)
{
    //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_task_t t;
    tt_result_t ret;
    tt_task_attr_t attr;
    
    TT_TEST_CASE_ENTER()
    // test start

    tt_task_attr_default(&attr);
    attr.enable_dns = TT_TRUE;

    ret = tt_task_create(&t, &attr);
    TT_UT_SUCCESS(ret, "");

    tt_task_add_fiber(&t, NULL, __dns_query_1, NULL, NULL);

    ret = tt_task_run(&t);
    TT_UT_SUCCESS(ret, "");
    tt_task_wait(&t);
    
    // test end
    TT_TEST_CASE_LEAVE()
}

