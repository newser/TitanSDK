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

#include <log/tt_log.h>
#include <log/tt_log_io_std.h>
#include <os/tt_spinlock.h>
#include <tt_platform.h>

// portlayer header files
#include <tt_cstd_api.h>

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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_log_io_std)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_log_mgr)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(log_case)

TT_TEST_CASE("tt_unit_test_log_io_std",
             "testing log io std",
             tt_unit_test_log_io_std,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_log_mgr",
                 "testing log manager",
                 tt_unit_test_log_mgr,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(log_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(TEST_UNIT_LOG, 0, log_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(name)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    tt_result_t __test_lio(tt_logio_t *lio)
{
    tt_logfmt_t lfmt;
    tt_result_t ret;
    tt_u32_t i, n;

    // 1. test format

    // empty format
    ret = tt_logfmt_create(&lfmt, TT_LOG_LEVEL_DETAIL, "", 0, NULL);
    if (!TT_OK(ret))
        return TT_FAIL;

    ret = tt_logfmt_add_io(&lfmt, lio);
    if (!TT_OK(ret))
        return TT_FAIL;

    tt_logfmt_input_flf(&lfmt, NULL, 0, "no output");
    tt_logfmt_output(&lfmt);

    tt_logfmt_destroy(&lfmt);

    // single field
    ret = tt_logfmt_create(&lfmt,
                           TT_LOG_LEVEL_INFO,
                           "${seq_no:%6d} ${level:%.2s} ${function:}\n",
                           0,
                           NULL);
    if (!TT_OK(ret))
        return TT_FAIL;

    ret = tt_logfmt_add_io(&lfmt, lio);
    if (!TT_OK(ret))
        return TT_FAIL;

    tt_logfmt_input_flf(&lfmt, NULL, 0, "expected: [6d] IN");
    tt_logfmt_output(&lfmt);

    tt_logfmt_destroy(&lfmt);

    // single field: truncate time
    ret = tt_logfmt_create(&lfmt,
                           TT_LOG_LEVEL_INFO,
                           "${content} ${time}\n",
                           12,
                           NULL);
    if (!TT_OK(ret))
        return TT_FAIL;

    ret = tt_logfmt_add_io(&lfmt, lio);
    if (!TT_OK(ret))
        return TT_FAIL;

    tt_logfmt_input_flf(&lfmt, NULL, 0, "123456789");
    tt_logfmt_output(&lfmt);

    tt_logfmt_destroy(&lfmt);

    // single field: wront format
    ret = tt_logfmt_create(&lfmt,
                           TT_LOG_LEVEL_INFO,
                           "${content} {${level:%d}\n",
                           0,
                           NULL);
    if (!TT_OK(ret))
        return TT_FAIL;

    ret = tt_logfmt_add_io(&lfmt, lio);
    if (!TT_OK(ret))
        return TT_FAIL;

    tt_logfmt_input_flf(&lfmt, NULL, 0, "level %%d");
    tt_logfmt_output(&lfmt);

    tt_logfmt_destroy(&lfmt);

    // invalid field
    ret = tt_logfmt_create(&lfmt,
                           TT_LOG_LEVEL_WARN,
                           "-- ${level} ${ function}\n",
                           0,
                           NULL);
    if (TT_OK(ret))
        return TT_FAIL; // invalid "${ "

    ret = tt_logfmt_create(&lfmt,
                           TT_LOG_LEVEL_WARN,
                           "-- ${level} ${}\n",
                           0,
                           NULL);
    if (TT_OK(ret))
        return TT_FAIL; // invalid "${}"

    ret = tt_logfmt_create(&lfmt,
                           TT_LOG_LEVEL_WARN,
                           "-- ${level} ${function\n",
                           0,
                           NULL);
    if (TT_OK(ret))
        return TT_FAIL; // invalid "${function"

    // invalid format
    ret = tt_logfmt_create(&lfmt,
                           TT_LOG_LEVEL_INFO,
                           "${level } ${function}\n",
                           0,
                           NULL);
    if (TT_OK(ret))
        return TT_FAIL;

    // multiple fields
    ret = tt_logfmt_create(&lfmt,
                           TT_LOG_LEVEL_ERROR,
                           "-- ${logger:%.3s} ${function}9999 $${content}"
                           "time(T): ${time:%Y}\n",
                           0,
                           "ABCDEF");
    if (!TT_OK(ret))
        return TT_FAIL;

    ret = tt_logfmt_add_io(&lfmt, lio);
    if (!TT_OK(ret))
        return TT_FAIL;

    tt_logfmt_input_flf(&lfmt,
                        NULL,
                        0,
                        "expected: %s",
                        "-- ERRORfunc $content\n");
    tt_logfmt_output(&lfmt);

    tt_logfmt_destroy(&lfmt);

    // multiple fields
    ret = tt_logfmt_create(&lfmt,
                           TT_LOG_LEVEL_FATAL,
                           "$${level} $${function:{${}{}} $${content} 1234",
                           0,
                           NULL);
    if (!TT_OK(ret))
        return TT_FAIL;

    ret = tt_logfmt_add_io(&lfmt, lio);
    if (!TT_OK(ret))
        return TT_FAIL;

    tt_logfmt_input_flf(&lfmt, NULL, 0, "expected: %s", "$FATAL $func${}} \n");
    tt_logfmt_output(&lfmt);

    tt_logfmt_destroy(&lfmt);

    // print all
    ret = tt_logfmt_create(&lfmt,
                           TT_LOG_LEVEL_FATAL,
                           "${seq_no:%5d} ${level:%.5s} "
                           "${time:%Y-%m-%d:%H-%M-%S} ${logger:%.6s} "
                           "${content} <${function} - ${line}>\n",
                           0,
                           "test-logger");
    if (!TT_OK(ret))
        return TT_FAIL;

    ret = tt_logfmt_add_io(&lfmt, lio);
    if (!TT_OK(ret))
        return TT_FAIL;

    i = 0;
    while (i++ < 10) {
        tt_logfmt_input_flf(&lfmt, __FUNCTION__, __LINE__, "all");
        tt_logfmt_output(&lfmt);
    }

    tt_logfmt_destroy(&lfmt);

    // print content more than buffer size
    ret = tt_logfmt_create(&lfmt,
                           TT_LOG_LEVEL_FATAL,
                           "${seq_no} ${content}\n",
                           100,
                           NULL);
    if (!TT_OK(ret))
        return TT_FAIL;

    ret = tt_logfmt_add_io(&lfmt, lio);
    if (!TT_OK(ret))
        return TT_FAIL;

    i = 0;
    while (i++ < 500) {
        tt_logfmt_input_flf(&lfmt, __FUNCTION__, __LINE__, "all");
    }
    tt_logfmt_output(&lfmt);

    tt_logfmt_destroy(&lfmt);

    // print content more than buffer size, expandable log fmt
    ret = tt_logfmt_create(&lfmt,
                           TT_LOG_LEVEL_FATAL,
                           "${seq_no} ${content}\n",
                           0,
                           NULL);
    if (!TT_OK(ret))
        return TT_FAIL;

    ret = tt_logfmt_add_io(&lfmt, lio);
    if (!TT_OK(ret))
        return TT_FAIL;

    n = ((TT_LOG_BUF_INIT_SIZE / 3) + 1) << 2;
    i = 0;
    while (i++ < n) {
        tt_logfmt_input_flf(&lfmt, __FUNCTION__, __LINE__, "all");
    }
    tt_logfmt_output(&lfmt);

    tt_logfmt_destroy(&lfmt);

    return TT_SUCCESS;
}

tt_result_t __test_vsnprintf(const char *fmt, ...)
{
    tt_char_t buf[10];
    tt_u32_t n;
    va_list args;

    va_start(args, fmt);
    n = tt_vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    if (n != 9)
        return TT_FAIL;

    n = tt_snprintf(buf, sizeof(buf), "%s", "123456789abcdef");
    if (n != 9)
        return TT_FAIL;

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_log_io_std)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    tt_logio_t *lio_std;

    TT_TEST_CASE_ENTER()
    // test start

    ret = __test_vsnprintf("%s", "123456789abcdef");
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "vsnprintf");

    lio_std = tt_logio_std_create(NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = __test_lio(lio_std);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_logio_enable_lock(lio_std);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = __test_lio(lio_std);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    tt_logio_destroy(lio_std);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_log_mgr)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

    TT_TEST_CASE_ENTER()
    // test start

    TT_DETAIL("%s", "this is DETAIL");
    TT_INFO("%s", "this is INFO");
    TT_WARN("%s", "this is WARN");
    TT_ERROR("%s", "this is ERROR");
    TT_FATAL("%s", "this is FATAL");

    do {
        tt_logmgr_enable(&tt_g_logmgr, TT_LOG_LEVEL_INFO);
        TT_DETAIL("%s", "disable DETAIL");

        tt_logmgr_enable(&tt_g_logmgr, TT_LOG_LEVEL_WARN);
        TT_INFO("%s", "disable INFO");

        tt_logmgr_enable(&tt_g_logmgr, TT_LOG_LEVEL_ERROR);
        TT_WARN("%s", "disable WARN");

        tt_logmgr_enable(&tt_g_logmgr, TT_LOG_LEVEL_FATAL);
        TT_ERROR("%s", "disable ERROR");

        tt_logmgr_enable(&tt_g_logmgr, TT_LOG_LEVEL_NUM);
        TT_FATAL("%s", "disable FATAL");

        tt_logmgr_enable(&tt_g_logmgr, TT_LOG_LEVEL_DETAIL);
        TT_DETAIL("%s", "enable DETAIL");
        TT_INFO("%s", "enable INFO");
        TT_WARN("%s", "enable WARN");
        TT_ERROR("%s", "enable ERROR");
        TT_FATAL("%s", "enable FATAL");
    } while (0);

    // enable all log by the end
    tt_logmgr_enable(&tt_g_logmgr, TT_LOG_LEVEL_DETAIL);

    // test end
    TT_TEST_CASE_LEAVE()
}
