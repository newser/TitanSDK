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

#define TT_HTTP_UT_DECLARE(name)                                               \
    extern tt_test_unit_t TT_MAKE_TEST_UNIT_NAME(name);

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef enum {
    HTTP_UT_BEGIN = 0,

    HTTP_UT_URI = HTTP_UT_BEGIN,
    HTTP_UT_HDR,
    HTTP_UT_RENDER,
    HTTP_UT_SCONN,
    HTTP_UT_SVR,
    HTTP_UT_INSERV,

    HTTP_UT_NUM // number of test units
} tt_http_ut_id_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

TT_HTTP_UT_DECLARE(HTTP_UT_URI)
TT_HTTP_UT_DECLARE(HTTP_UT_HDR)
TT_HTTP_UT_DECLARE(HTTP_UT_RENDER)
TT_HTTP_UT_DECLARE(HTTP_UT_SCONN)
TT_HTTP_UT_DECLARE(HTTP_UT_SVR)
TT_HTTP_UT_DECLARE(HTTP_UT_INSERV)

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

tt_test_unit_t *tt_g_http_ut_list[HTTP_UT_NUM] = {
    &TT_MAKE_TEST_UNIT_NAME(HTTP_UT_URI),
    &TT_MAKE_TEST_UNIT_NAME(HTTP_UT_HDR),
    &TT_MAKE_TEST_UNIT_NAME(HTTP_UT_RENDER),
    &TT_MAKE_TEST_UNIT_NAME(HTTP_UT_SCONN),
    &TT_MAKE_TEST_UNIT_NAME(HTTP_UT_SVR),
    &TT_MAKE_TEST_UNIT_NAME(HTTP_UT_INSERV),
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_http_ut_init(IN tt_ptr_t reserved)
{
    tt_http_ut_id_t unit_id = HTTP_UT_BEGIN;
    while (unit_id < HTTP_UT_NUM) {
        tt_result_t result = TT_FAIL;

        if (tt_g_http_ut_list[unit_id] != NULL) {
            result = tt_test_unit_to_class(tt_g_http_ut_list[unit_id]);
            if (!TT_OK(result)) {
                return TT_FAIL;
            }
        }

        // next
        ++unit_id;
    }

    return TT_SUCCESS;
}