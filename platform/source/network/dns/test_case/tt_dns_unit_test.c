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

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define TT_DNS_UT_DECLARE(name)                                                \
    extern tt_test_unit_t TT_MAKE_TEST_UNIT_NAME(name);

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef enum {
    DNS_UT_BEGIN = 0,

    DNS_UT_QUERY = DNS_UT_BEGIN,
    DNS_UT_RR,
    DNS_UT_CACHE,

    DNS_UT_NUM // number of test units
} tt_dns_ut_id_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

TT_DNS_UT_DECLARE(DNS_UT_QUERY)
TT_DNS_UT_DECLARE(DNS_UT_RR)
TT_DNS_UT_DECLARE(DNS_UT_CACHE)

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

tt_test_unit_t *tt_g_dns_ut_list[DNS_UT_NUM] = {
#if 1
    &TT_MAKE_TEST_UNIT_NAME(DNS_UT_QUERY),
    &TT_MAKE_TEST_UNIT_NAME(DNS_UT_RR),
#endif
    &TT_MAKE_TEST_UNIT_NAME(DNS_UT_CACHE),
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_dns_ut_init(IN tt_ptr_t reserved)
{
    tt_dns_ut_id_t unit_id = DNS_UT_BEGIN;
    while (unit_id < DNS_UT_NUM) {
        tt_result_t result = TT_FAIL;

        if (tt_g_dns_ut_list[unit_id] != NULL) {
            result = tt_test_unit_to_class(tt_g_dns_ut_list[unit_id]);
            if (!TT_OK(result)) {
                return TT_FAIL;
            }
        }

        // next
        ++unit_id;
    }

    return TT_SUCCESS;
}
