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

#define TT_SSH_UT_DECLARE(name)                                                \
    extern tt_test_unit_t TT_MAKE_TEST_UNIT_NAME(name);

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef enum {
    SSH_UT_BEGIN = 0,

    SSH_BASIC = SSH_UT_BEGIN,
    SSH_MSG,
    SSH_MSG_VERXCHG,
    SSH_CTX,
    SSH_KDF,
    SSH_ENC,
    SSH_MAC,
    SSH_AUTH_MSG,
    SSH_CONN_MSG,

    SSH_UT_NUM // number of test units
} tt_ssh_ut_id_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

TT_SSH_UT_DECLARE(SSH_BASIC)
TT_SSH_UT_DECLARE(SSH_MSG)
TT_SSH_UT_DECLARE(SSH_MSG_VERXCHG)
TT_SSH_UT_DECLARE(SSH_CTX)
TT_SSH_UT_DECLARE(SSH_KDF)
TT_SSH_UT_DECLARE(SSH_ENC)
TT_SSH_UT_DECLARE(SSH_MAC)
TT_SSH_UT_DECLARE(SSH_AUTH_MSG)
TT_SSH_UT_DECLARE(SSH_CONN_MSG)

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

tt_test_unit_t *tt_g_ssh_ut_list[SSH_UT_NUM] = {
#if 0
    &TT_MAKE_TEST_UNIT_NAME(SSH_MSG),
    &TT_MAKE_TEST_UNIT_NAME(SSH_MSG_VERXCHG),
    &TT_MAKE_TEST_UNIT_NAME(SSH_CTX),
    &TT_MAKE_TEST_UNIT_NAME(SSH_KDF),
    &TT_MAKE_TEST_UNIT_NAME(SSH_MAC),
    &TT_MAKE_TEST_UNIT_NAME(SSH_AUTH_MSG),
    &TT_MAKE_TEST_UNIT_NAME(SSH_CONN_MSG),
#else
    &TT_MAKE_TEST_UNIT_NAME(SSH_BASIC),
#endif
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_ssh_ut_init(IN tt_ptr_t reserved)
{
    tt_ssh_ut_id_t unit_id = SSH_UT_BEGIN;
    while (unit_id < SSH_UT_NUM) {
        tt_result_t result = TT_FAIL;

        if (tt_g_ssh_ut_list[unit_id] != NULL) {
            result = tt_test_unit_to_class(tt_g_ssh_ut_list[unit_id]);
            if (!TT_OK(result)) {
                return TT_FAIL;
            }
        }

        // next
        ++unit_id;
    }

    return TT_SUCCESS;
}
