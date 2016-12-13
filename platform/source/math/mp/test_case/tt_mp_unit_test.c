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

#define TT_BN_UT_DECLARE(name)                                                 \
    extern tt_test_unit_t TT_MAKE_TEST_UNIT_NAME(name);

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef enum {
    MP_UT_BEGIN = 0,

    MPN_BASIC = MP_UT_BEGIN,
    MPN_MUL,
    MPN_DIV,
    MPN_MOD,
    MPN_GCD,
    MPN_EXP,

    MP_UT_NUM // number of test units
} tt_mp_ut_id_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

TT_BN_UT_DECLARE(MPN_BASIC)
TT_BN_UT_DECLARE(MPN_MUL)
TT_BN_UT_DECLARE(MPN_DIV)
TT_BN_UT_DECLARE(MPN_MOD)
TT_BN_UT_DECLARE(MPN_GCD)
TT_BN_UT_DECLARE(MPN_EXP)

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

tt_test_unit_t *tt_g_mp_ut_list[MP_UT_NUM] = {
    &TT_MAKE_TEST_UNIT_NAME(MPN_BASIC),
    &TT_MAKE_TEST_UNIT_NAME(MPN_MUL),
    &TT_MAKE_TEST_UNIT_NAME(MPN_DIV),
    &TT_MAKE_TEST_UNIT_NAME(MPN_MOD),
    &TT_MAKE_TEST_UNIT_NAME(MPN_GCD),
    &TT_MAKE_TEST_UNIT_NAME(MPN_EXP),
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_mp_ut_init(IN tt_ptr_t reserved)
{
    tt_mp_ut_id_t unit_id = MP_UT_BEGIN;
    while (unit_id < MP_UT_NUM) {
        tt_result_t result = TT_FAIL;

        if (tt_g_mp_ut_list[unit_id] != NULL) {
            result = tt_test_unit_to_class(tt_g_mp_ut_list[unit_id]);
            if (!TT_OK(result)) {
                return TT_FAIL;
            }
        }

        // next
        ++unit_id;
    }

    return TT_SUCCESS;
}
