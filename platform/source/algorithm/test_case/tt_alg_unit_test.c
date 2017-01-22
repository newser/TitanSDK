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

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define TT_ALG_UT_DECLARE(name)                                                \
    extern tt_test_unit_t TT_MAKE_TEST_UNIT_NAME(name);

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef enum {
    ALG_UT_BEGIN = 0,

    ALG_UT_BASIC_ALG,
    ALG_UT_BUF,
    ALG_UT_VECTOR,
    ALG_UT_HASHMAP,
    ALG_UT_QUEUE,
    ALG_UT_RBTREE,
    ALG_UT_STRING,
    ALG_UT_LIST,
    ALG_UT_HEAP,
    ALG_UT_STACK,
    ALG_UT_RBUF,

    ALG_UT_NUM // number of test units
} tt_alg_ut_id_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

TT_ALG_UT_DECLARE(ALG_UT_BASIC_ALG)
TT_ALG_UT_DECLARE(ALG_UT_BUF)
TT_ALG_UT_DECLARE(ALG_UT_VECTOR)
TT_ALG_UT_DECLARE(ALG_UT_LIST)
TT_ALG_UT_DECLARE(ALG_UT_HASHMAP)
TT_ALG_UT_DECLARE(ALG_UT_QUEUE)
TT_ALG_UT_DECLARE(ALG_UT_RBTREE)
TT_ALG_UT_DECLARE(ALG_UT_STRING)
TT_ALG_UT_DECLARE(ALG_UT_HEAP)
TT_ALG_UT_DECLARE(ALG_UT_STACK)
TT_ALG_UT_DECLARE(ALG_UT_RBUF)

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

tt_test_unit_t *tt_g_alg_ut_list[ALG_UT_NUM] = {
#if 0
    &TT_MAKE_TEST_UNIT_NAME(ALG_UT_BASIC_ALG),
    &TT_MAKE_TEST_UNIT_NAME(ALG_UT_HEAP),
    &TT_MAKE_TEST_UNIT_NAME(ALG_UT_VECTOR),
    &TT_MAKE_TEST_UNIT_NAME(ALG_UT_LIST),
    &TT_MAKE_TEST_UNIT_NAME(ALG_UT_HASHMAP),
    &TT_MAKE_TEST_UNIT_NAME(ALG_UT_QUEUE),
    &TT_MAKE_TEST_UNIT_NAME(ALG_UT_RBTREE),
    &TT_MAKE_TEST_UNIT_NAME(ALG_UT_STRING),
    &TT_MAKE_TEST_UNIT_NAME(ALG_UT_STACK),
    &TT_MAKE_TEST_UNIT_NAME(ALG_UT_RBUF),
#endif
    &TT_MAKE_TEST_UNIT_NAME(ALG_UT_BUF),
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_alg_ut_init(IN tt_ptr_t reserved)
{
    tt_alg_ut_id_t unit_id = ALG_UT_BEGIN;
    while (unit_id < ALG_UT_NUM) {
        tt_result_t result = TT_FAIL;

        if (tt_g_alg_ut_list[unit_id] != NULL) {
            result = tt_test_unit_to_class(tt_g_alg_ut_list[unit_id]);
            if (!TT_OK(result)) {
                return TT_FAIL;
            }
        }

        // next
        ++unit_id;
    }

    return TT_SUCCESS;
}
