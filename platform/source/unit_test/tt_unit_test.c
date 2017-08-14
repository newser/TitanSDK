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

#include <misc/tt_assert.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define TT_TEST_UNIT_DECLARE(name)                                             \
    extern tt_test_unit_t TT_MAKE_TEST_UNIT_NAME(name);

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

TT_TEST_UNIT_DECLARE(TEST_UNIT_LOG)
TT_TEST_UNIT_DECLARE(TEST_UNIT_ATOMIC)
TT_TEST_UNIT_DECLARE(TEST_UNIT_SPIN_LOCK)
TT_TEST_UNIT_DECLARE(TEST_UNIT_THREAD)
TT_TEST_UNIT_DECLARE(TEST_UNIT_MUTEX)
TT_TEST_UNIT_DECLARE(TEST_UNIT_SEM)
TT_TEST_UNIT_DECLARE(TEST_UNIT_RWLOCK)
TT_TEST_UNIT_DECLARE(TEST_UNIT_SLAB)
TT_TEST_UNIT_DECLARE(TEST_UNIT_MISC)
TT_TEST_UNIT_DECLARE(TEST_UNIT_MEMPOOL)
TT_TEST_UNIT_DECLARE(TEST_UNIT_SLAB)
TT_TEST_UNIT_DECLARE(TEST_UNIT_TIME_REF)
TT_TEST_UNIT_DECLARE(TEST_UNIT_FS)
TT_TEST_UNIT_DECLARE(TEST_UNIT_TIMER)
TT_TEST_UNIT_DECLARE(TEST_UNIT_SOCKET)
TT_TEST_UNIT_DECLARE(TEST_UNIT_IPC)
TT_TEST_UNIT_DECLARE(TEST_UNIT_CHARSET)
TT_TEST_UNIT_DECLARE(TEST_UNIT_CFGNODE)
TT_TEST_UNIT_DECLARE(TEST_UNIT_CFGPATH)
TT_TEST_UNIT_DECLARE(TEST_UNIT_CFGSHELL)
TT_TEST_UNIT_DECLARE(TEST_UNIT_LOG_PATTERN)
TT_TEST_UNIT_DECLARE(TEST_UNIT_FIBER)
TT_TEST_UNIT_DECLARE(TEST_UNIT_DATE)

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

tt_test_unit_t *tt_g_test_unit_list[TEST_UNIT_NUM] = {
    &TT_MAKE_TEST_UNIT_NAME(TEST_UNIT_LOG),
#if 0
    &TT_MAKE_TEST_UNIT_NAME(TEST_UNIT_LOG_PATTERN),
#endif

#if 0
    &TT_MAKE_TEST_UNIT_NAME(TEST_UNIT_MUTEX),
    &TT_MAKE_TEST_UNIT_NAME(TEST_UNIT_ATOMIC),
    &TT_MAKE_TEST_UNIT_NAME(TEST_UNIT_SPIN_LOCK),
    &TT_MAKE_TEST_UNIT_NAME(TEST_UNIT_RWLOCK),
    &TT_MAKE_TEST_UNIT_NAME(TEST_UNIT_THREAD),
    &TT_MAKE_TEST_UNIT_NAME(TEST_UNIT_FIBER),
    &TT_MAKE_TEST_UNIT_NAME(TEST_UNIT_SEM),
#endif

#if 0
    &TT_MAKE_TEST_UNIT_NAME(TEST_UNIT_SLAB),
    &TT_MAKE_TEST_UNIT_NAME(TEST_UNIT_MEMPOOL),
#endif

#if 0
    &TT_MAKE_TEST_UNIT_NAME(TEST_UNIT_TIME_REF),
    &TT_MAKE_TEST_UNIT_NAME(TEST_UNIT_DATE),
    &TT_MAKE_TEST_UNIT_NAME(TEST_UNIT_TIMER),
#endif

#if 0
    &TT_MAKE_TEST_UNIT_NAME(TEST_UNIT_MISC),
    &TT_MAKE_TEST_UNIT_NAME(TEST_UNIT_CHARSET),
    &TT_MAKE_TEST_UNIT_NAME(TEST_UNIT_CFGNODE),
    &TT_MAKE_TEST_UNIT_NAME(TEST_UNIT_CFGPATH),
    &TT_MAKE_TEST_UNIT_NAME(TEST_UNIT_CFGSHELL),
#endif

#if 0
&TT_MAKE_TEST_UNIT_NAME(TEST_UNIT_FS),
    &TT_MAKE_TEST_UNIT_NAME(TEST_UNIT_IPC),
    &TT_MAKE_TEST_UNIT_NAME(TEST_UNIT_SOCKET),
#endif
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

// case to item in framework
static tt_result_t tt_test_case_to_item(IN tt_test_class_t *test_class,
                                        IN tt_test_case_t *unit);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_test_unit_init(IN tt_ptr_t reserved)
{
    tt_test_unit_id_t unit_id = TEST_UNIT_BEGIN;
    while (unit_id < TEST_UNIT_NUM) {
        tt_result_t result = TT_FAIL;

        if (tt_g_test_unit_list[unit_id] != NULL) {
            result = tt_test_unit_to_class(tt_g_test_unit_list[unit_id]);
            if (!TT_OK(result)) {
                return TT_FAIL;
            }
        }

        // next
        ++unit_id;
    }

#if 0
    do {
        tt_result_t tt_alg_ut_init(IN tt_ptr_t reserved);
        tt_alg_ut_init(0);
    } while (0);
#endif

#if 0
    do {
        tt_result_t tt_dns_ut_init(IN tt_ptr_t reserved);
        tt_dns_ut_init(0);
    } while (0);
#endif

#if 0
    do {
        tt_result_t tt_ssl_ut_init(IN tt_ptr_t reserved);
        tt_ssl_ut_init(0);
    } while (0);
#endif

#if 0
    do {
        tt_result_t tt_crypto_ut_init(IN tt_ptr_t reserved);
        tt_crypto_ut_init(0);
    } while (0);
#endif

#if 0
    do {
        tt_result_t tt_ssh_ut_init(IN tt_ptr_t reserved);
        tt_ssh_ut_init(0);
    } while (0);
#endif

#if 0
    do {
        tt_result_t tt_xml_ut_init(IN tt_ptr_t reserved);
        tt_xml_ut_init(0);
    } while (0);
#endif

#if 0
    do {
        tt_result_t tt_cli_ut_init(IN tt_ptr_t reserved);
        tt_cli_ut_init(0);
    } while (0);
#endif

    return TT_SUCCESS;
}

tt_result_t tt_test_unit_run(IN const tt_char_t *unit_name)
{
    return tt_test_run(unit_name);
}

tt_result_t tt_test_unit_list(IN const tt_char_t *unit_name)
{
    tt_u32_t succ_num = 0;
    tt_u32_t fail_num = 0;
    tt_result_t ret = TT_SUCCESS;

    ret = tt_test_list(unit_name, &succ_num, &fail_num);

    TT_TEST_INFO("\r\ntest case result: [%d / %d] OK\r\n",
                 succ_num,
                 succ_num + fail_num);
#if TT_ENV_OS_IS_IOS
    {
        extern void __ios_display(const char *str);

        tt_char_t buf[200] = {0};
        snprintf(buf,
                 sizeof(buf) - 1,
                 "\r\ntest case result: [%d / %d] OK\r\n",
                 succ_num,
                 succ_num + fail_num);
        __ios_display(buf);
    }
#elif TT_ENV_OS_IS_ANDROID
    __android_display("\r\ntest case result: [%d / %d] OK\r\n",
                      succ_num,
                      succ_num + fail_num);
#endif

    return ret;
}

tt_result_t tt_test_unit_to_class(IN tt_test_unit_t *unit)
{
    tt_test_class_t *test_class = NULL;
    tt_result_t result = TT_FAIL;
    tt_u32_t index = 0;

    TT_ASSERT(unit != NULL);

    // create a test class in framework
    result = tt_test_class_create(unit->name, unit->attribute, &test_class);
    if (!TT_OK(result)) {
        TT_ERROR("fail to create test class: %s", unit->name);
        return TT_FAIL;
    }

    // add test items
    for (index = 0; index < unit->case_num; ++index) {
        tt_test_case_t *test_case = &(unit->case_list[index]);

        result = tt_test_case_to_item(test_class, test_case);
        if (!TT_OK(result)) {
            return TT_FAIL;
        }
    }

    return TT_SUCCESS;
}

tt_result_t tt_test_case_to_item(IN tt_test_class_t *test_class,
                                 IN tt_test_case_t *unit)
{
    tt_result_t result = TT_FAIL;
    tt_test_entry_t entry = {unit->routine,
                             unit->param,
                             unit->routine_enter,
                             unit->enter_param,
                             unit->routine_leave,
                             unit->leave_param};

    result = tt_test_item_add(test_class, &entry, unit->name, unit->comment);
    if (!TT_OK(result)) {
        TT_ERROR("fail to create test item: %s", unit->name);
        return TT_FAIL;
    }

    return TT_SUCCESS;
}
