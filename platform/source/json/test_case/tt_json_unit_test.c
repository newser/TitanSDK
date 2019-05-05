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

#define TT_JSON_UT_DECLARE(name)                                               \
    extern tt_test_unit_t TT_MAKE_TEST_UNIT_NAME(name);

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef enum
{
    JSON_UT_BEGIN = 0,

    JSON_UT_JDOC = JSON_UT_BEGIN,

    JSON_UT_NUM // number of test units
} tt_json_ut_id_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

TT_JSON_UT_DECLARE(JSON_UT_JDOC)

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

tt_test_unit_t *tt_g_json_ut_list[JSON_UT_NUM] = {
    &TT_MAKE_TEST_UNIT_NAME(JSON_UT_JDOC),
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_json_ut_init(IN tt_ptr_t reserved)
{
    tt_json_ut_id_t unit_id = JSON_UT_BEGIN;
    while (unit_id < JSON_UT_NUM) {
        tt_result_t result = TT_FAIL;

        if (tt_g_json_ut_list[unit_id] != NULL) {
            result = tt_test_unit_to_class(tt_g_json_ut_list[unit_id]);
            if (!TT_OK(result)) { return TT_FAIL; }
        }

        // next
        ++unit_id;
    }

    return TT_SUCCESS;
}
