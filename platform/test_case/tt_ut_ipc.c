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

#if TT_ENV_OS_IS_IOS
#define __IPC_PATH "../tmp/ipc_basic"
#else
#define __IPC_PATH "./ipc_basic1"
#endif

#if TT_ENV_OS_IS_WINDOWS
#define __app_file path
#elif TT_ENV_OS_IS_IOS
extern const char *get_app_path();
//#define __app_file get_app_path()
#define __app_file path
#define __app_file_sc get_app_path()
#else
//#define __app_file "./app_unit_test"
#define __app_file path
#define __app_file_sc "./测试"
#endif

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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_ipc_basic)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_ipc_client)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_ipc_stress)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_ipc_fiber_ev)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(ipc_case)

TT_TEST_CASE("tt_unit_test_ipc_basic",
             "testing ipc basic test",
             tt_unit_test_ipc_basic,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_ipc_client",
                 "testing ipc client",
                 tt_unit_test_ipc_client,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_ipc_stress",
                 "testing ipc stress test",
                 tt_unit_test_ipc_stress,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_ipc_fiber_ev",
                 "testing ipc fiber event",
                 tt_unit_test_ipc_fiber_ev,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(ipc_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(TEST_UNIT_IPC, 0, ipc_case)


//////////////////////////////////////////////////////////
// interface declaratioN
//////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////
// interface implementation
//////////////////////////////////////////////////////////

/*
TT_TEST_ROUTINE_DEFINE(tt_unit_test_ipc_client)
{
    //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

    TT_TEST_CASE_ENTER()
    // test start

    // test end
    TT_TEST_CASE_LEAVE()
}
*/

#define __CONN_NUM 10
#define __ROUND_NUM 100

#if 1
#define TT_INFO_IPC TT_INFO
#else
#define TT_INFO_IPC(...)
#endif

        static tt_u32_t __err_line;

tt_result_t __ipc_cli_1(IN void *param)
{
    tt_ipc_t *ipc;
    tt_u8_t sbuf[11] = "1234567890", rbuf[11] = {0};
    tt_u32_t n, len, cn, rn;

    cn = 0;
    while (cn++ < __CONN_NUM) {
        TT_INFO_IPC("cli con %d", cn);

        ipc = tt_ipc_create(NULL, NULL);
        if (ipc == NULL) {
            __err_line = __LINE__;
            TT_INFO_IPC("err: %d", __err_line);
            return TT_FAIL;
        }

        tt_sleep(50);
        if (!TT_OK(tt_ipc_connect_retry(ipc, __IPC_PATH, 100, 10))) {
            __err_line = __LINE__;
            TT_INFO_IPC("err: %d", __err_line);
            return TT_FAIL;
        }

        rn = 0;
        while (rn++ < __ROUND_NUM) {
            if (!TT_OK(tt_ipc_send(ipc, sbuf, sizeof(sbuf), &n))) {
                TT_INFO_IPC("err %d", __LINE__);
                __err_line = __LINE__;
                return TT_FAIL;
            }
            TT_INFO_IPC("[%d] cli sent %d", rn, n);
            if (sizeof(sbuf) != n) {
                __err_line = __LINE__;
                return TT_FAIL;
            }

            len = 0;
            while (len < sizeof(rbuf)) {
                if (!TT_OK(tt_ipc_recv(ipc,
                                       rbuf + len,
                                       sizeof(rbuf) - len,
                                       &n,
                                       NULL))) {
                    TT_INFO_IPC("err %d", __LINE__);
                    __err_line = __LINE__;
                    return TT_FAIL;
                }
                len += n;
            }
            TT_ASSERT(len == sizeof(rbuf));
            TT_INFO_IPC("[%d] cli recv %d, %s", rn, n, rbuf);
            if (tt_strcmp((tt_char_t *)rbuf, "0987654321") != 0) {
                __err_line = __LINE__;
                return TT_FAIL;
            }
        }

        tt_ipc_destroy(ipc);
    }

    return TT_SUCCESS;
}

tt_result_t __ipc_svr_1(IN void *param)
{
    tt_ipc_t *ipc, *new_ipc;
    tt_u8_t sbuf[11] = "0987654321", rbuf[11];
    tt_u32_t n, len, cn, rn;
    tt_result_t ret;

    ipc = tt_ipc_create(__IPC_PATH, NULL);
    if (ipc == NULL) {
        __err_line = __LINE__;
        return TT_FAIL;
    }

    cn = 0;
    while (cn++ < __CONN_NUM) {
        TT_INFO_IPC("svr acc %d", cn);

        new_ipc = tt_ipc_accept(ipc, NULL);
        if (new_ipc == NULL) {
            __err_line = __LINE__;
            return TT_FAIL;
        }

        rn = 0;
        len = 0;
        while (TT_OK((ret = tt_ipc_recv(new_ipc,
                                        rbuf + len,
                                        sizeof(rbuf) - len,
                                        &n,
                                        NULL)))) {
            len += n;
            if (len < sizeof(rbuf)) {
                continue;
            }
            TT_ASSERT(len == sizeof(rbuf));
            len = 0;
            TT_INFO_IPC("[%d] svr recv %d, %s", rn, n, rbuf);
            if (tt_strcmp((tt_char_t *)rbuf, "1234567890") != 0) {
                __err_line = __LINE__;
                return TT_FAIL;
            }

            if (!TT_OK(tt_ipc_send(new_ipc, sbuf, sizeof(sbuf), &n))) {
                __err_line = __LINE__;
                return TT_FAIL;
            }
            if (sizeof(sbuf) != n) {
                __err_line = __LINE__;
                return TT_FAIL;
            }
            TT_INFO_IPC("[%d] svr sent %d", rn, n);
        }
        if (ret != TT_END) {
            __err_line = __LINE__;
            return TT_FAIL;
        }

        TT_INFO_IPC("svr acc done %d, ret: %x", cn, ret);
        tt_ipc_destroy(new_ipc);
    }

    tt_ipc_destroy(ipc);

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_ipc_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_task_t t;
    tt_result_t ret;
    tt_process_t proc;
    tt_char_t *argv[20] = {0};
    tt_char_t *path;

    TT_TEST_CASE_ENTER()

#if TT_ENV_OS_IS_WINDOWS
    tt_fcreate(__IPC_PATH, NULL);
#endif

    ret = tt_task_create(&t, NULL);
    TT_UT_SUCCESS(ret, "");

    tt_task_add_fiber(&t, "svr", __ipc_svr_1, NULL, NULL);
    // tt   _task_add_fiber(&t, "cli", __ipc_cli_1, NULL, NULL);

    ret = tt_task_run(&t);
    TT_UT_SUCCESS(ret, "");

    path = tt_process_path(NULL);
    TT_UT_NOT_EQUAL(path, NULL, "");
    argv[0] = "app_unit_test";
    argv[1] = "ipc-1";
    argv[2] = __IPC_PATH;

    ret = tt_process_create(&proc, __app_file, argv, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_free(path);

    tt_task_wait(&t);

    ret = tt_process_wait(&proc, TT_TRUE, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    TT_UT_EQUAL(__err_line, 0, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_result_t __ipc_svr_acc_2(IN void *param)
{
    tt_ipc_t *new_ipc = (tt_ipc_t *)param;
    tt_u8_t sbuf[11] = "0987654321", rbuf[11];
    tt_u32_t n, len = 0, rn;
    tt_result_t ret;

    rn = 0;
    while (TT_OK(
        (ret =
             tt_ipc_recv(new_ipc, rbuf + len, sizeof(rbuf) - len, &n, NULL)))) {
        len += n;
        if (len < sizeof(rbuf)) {
            continue;
        }
        TT_ASSERT(len == sizeof(rbuf));
        len = 0;
        if (tt_strcmp((tt_char_t *)rbuf, "1234567890") != 0) {
            __err_line = __LINE__;
            return TT_FAIL;
        }
        TT_INFO_IPC("[%d] acc recv %d", rn, n);

        if (!TT_OK(tt_ipc_send(new_ipc, sbuf, sizeof(sbuf), &n))) {
            __err_line = __LINE__;
            return TT_FAIL;
        }
        if (sizeof(sbuf) != n) {
            __err_line = __LINE__;
            return TT_FAIL;
        }
        TT_INFO_IPC("[%d] acc sent %d", rn, n);

        if (tt_rand_u32() % __CONN_NUM == 0) {
            TT_INFO("server close ipc");
            tt_ipc_destroy(new_ipc);
            return TT_SUCCESS;
        }
    }
    if (ret != TT_END) {
        __err_line = __LINE__;
        return TT_FAIL;
    }

    tt_ipc_destroy(new_ipc);

    return TT_SUCCESS;
}

static tt_process_t proc[__CONN_NUM];

static tt_result_t __ipc_svr_2(IN void *param)
{
    tt_ipc_t *ipc = (tt_ipc_t *)param, *new_ipc;
    tt_fiber_t *fb;
    tt_u32_t cn;
    tt_char_t *argv[20] = {0};
    tt_char_t *path;
    tt_result_t ret;

    path = tt_process_path(NULL);
    TT_ASSERT(path != NULL);
    argv[0] = "app_unit_test";
    argv[1] = "ipc-2";
    argv[2] = __IPC_PATH;

    cn = 0;
    while (cn < __CONN_NUM) {
        TT_INFO_IPC("svr acc %d", cn);

        ret = tt_process_create(&proc[cn], __app_file, argv, NULL);
        if (!TT_OK(ret)) {
            __err_line = __LINE__;
            return TT_FAIL;
        }

        new_ipc = tt_ipc_accept(ipc, NULL);
        if (new_ipc == NULL) {
            __err_line = __LINE__;
            return TT_FAIL;
        }
        TT_INFO("new ipc");

        fb = tt_fiber_create(NULL, __ipc_svr_acc_2, new_ipc, NULL);
        if (fb == NULL) {
            __err_line = __LINE__;
            return TT_FAIL;
        }
        tt_fiber_resume(fb, TT_FALSE);

        ++cn;
    }

    tt_free(path);

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_ipc_stress)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_task_t t;
    tt_result_t ret;
    tt_ipc_t *ipc;
    tt_u32_t cn;

    TT_TEST_CASE_ENTER()

#if TT_ENV_OS_IS_WINDOWS
    tt_fcreate(__IPC_PATH, NULL);
#endif

    ipc = tt_ipc_create(__IPC_PATH, NULL);
    TT_UT_NOT_NULL(ipc, "");

    ret = tt_task_create(&t, NULL);
    TT_UT_SUCCESS(ret, "");

    tt_task_add_fiber(&t, "svr", __ipc_svr_2, ipc, NULL);

    ret = tt_task_run(&t);
    TT_UT_SUCCESS(ret, "");
    tt_task_wait(&t);

    if (__err_line == 0) {
        cn = 0;
        while (cn < __CONN_NUM) {
            if (!TT_OK(tt_process_wait(&proc[cn], TT_TRUE, NULL))) {
                __err_line = __LINE__;
            }

            ++cn;
        }
    }

    tt_ipc_destroy(ipc);

    TT_UT_EQUAL(__err_line, 0, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

tt_result_t __ipc_cli_oneshot(IN void *param)
{
    tt_ipc_t *ipc;
    tt_u8_t sbuf[11] = "1234567890", rbuf[11];
    tt_u32_t n, len, rn;

    ipc = tt_ipc_create(NULL, NULL);
    if (ipc == NULL) {
        __err_line = __LINE__;
        return TT_FAIL;
    }

    tt_sleep(50);
    if (!TT_OK(tt_ipc_connect_retry(ipc, __IPC_PATH, 100, 10))) {
        __err_line = __LINE__;
        return TT_FAIL;
    }

    rn = 0;
    while (rn++ < __ROUND_NUM) {
        if (!TT_OK(tt_ipc_send(ipc, sbuf, sizeof(sbuf), &n))) {
            __err_line = __LINE__;
            return TT_FAIL;
        }
        if (sizeof(sbuf) != n) {
            __err_line = __LINE__;
            return TT_FAIL;
        }
        TT_INFO_IPC("[%d] cli sent %d", rn, n);

        len = 0;
        while (len < sizeof(rbuf)) {
            if (!TT_OK(tt_ipc_recv(ipc, rbuf, sizeof(rbuf), &n, NULL))) {
                __err_line = __LINE__;
                return TT_FAIL;
            }
            len += n;
        }
        TT_ASSERT(len == sizeof(rbuf));
        if (tt_strcmp((tt_char_t *)rbuf, "0987654321") != 0) {
            __err_line = __LINE__;
            return TT_FAIL;
        }
        TT_INFO_IPC("[%d] cli recv %d", rn, n);
    }

    tt_ipc_destroy(ipc);

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_ipc_client)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_task_t t;
    tt_result_t ret;
    tt_process_t proc;
    tt_char_t *argv[20] = {0};
    tt_char_t *path;

    TT_TEST_CASE_ENTER()

#if TT_ENV_OS_IS_WINDOWS
    tt_fcreate(__IPC_PATH, NULL);
#endif

    ret = tt_task_create(&t, NULL);
    TT_UT_SUCCESS(ret, "");

    tt_task_add_fiber(&t, "svr", __ipc_cli_1, NULL, NULL);

    ret = tt_task_run(&t);
    TT_UT_SUCCESS(ret, "");

    path = tt_process_path(NULL);
    TT_UT_NOT_EQUAL(path, NULL, "");
    argv[0] = "app_unit_test";
    argv[1] = "ipc-svr";
    argv[2] = __IPC_PATH;

    ret = tt_process_create(&proc, __app_file, argv, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_free(path);

    tt_task_wait(&t);

    ret = tt_process_wait(&proc, TT_TRUE, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    TT_UT_EQUAL(__err_line, 0, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

#define __fiber_num 10
#define __ev_per_fiber 100

static tt_u32_t __ipc_fev_num;

tt_result_t __ipc_svr_fev(IN void *param)
{
    tt_ipc_t *ipc, *new_ipc;
    tt_u8_t sbuf[11] = "0987654321", rbuf[11];
    tt_u32_t n, len, cn, rn;
    tt_result_t ret;
    tt_fiber_ev_t *fev;
    tt_fiber_t *cfb = tt_current_fiber();

    ipc = tt_ipc_create(__IPC_PATH, NULL);
    if (ipc == NULL) {
        __err_line = __LINE__;
        return TT_FAIL;
    }

    cn = 0;
    while (cn++ < __CONN_NUM) {
        TT_INFO_IPC("svr acc %d", cn);

        new_ipc = tt_ipc_accept(ipc, NULL);
        if (new_ipc == NULL) {
            __err_line = __LINE__;
            return TT_FAIL;
        }

        rn = 0;
        len = 0;
        while (TT_OK((ret = tt_ipc_recv(new_ipc,
                                        rbuf + len,
                                        sizeof(rbuf) - len,
                                        &n,
                                        &fev)))) {
            if (fev != NULL) {
                if (fev->src != NULL) {
                    if (fev->ev != 0xaabbccdd) {
                        __err_line = __LINE__;
                    }
                    fev->ev = 0xddccbbaa;
                } else {
                    if (fev->ev != 0x11223344) {
                        __err_line = __LINE__;
                    }
                }
                ++__ipc_fev_num;
                tt_fiber_finish(fev);
            }

            len += n;
            if (len < sizeof(rbuf)) {
                continue;
            }
            TT_ASSERT(len == sizeof(rbuf));
            len = 0;
            TT_INFO_IPC("[%d] svr recv %d, %s", rn, n, rbuf);
            if (tt_strcmp((tt_char_t *)rbuf, "1234567890") != 0) {
                __err_line = __LINE__;
                return TT_FAIL;
            }

            if (!TT_OK(tt_ipc_send(new_ipc, sbuf, sizeof(sbuf), &n))) {
                __err_line = __LINE__;
                return TT_FAIL;
            }
            if (sizeof(sbuf) != n) {
                __err_line = __LINE__;
                return TT_FAIL;
            }
            TT_INFO_IPC("[%d] svr sent %d", rn, n);
        }
        if (ret != TT_END) {
            __err_line = __LINE__;
            return TT_FAIL;
        }

        TT_INFO_IPC("svr acc done %d, ret: %x", cn, ret);
        tt_ipc_destroy(new_ipc);
    }

    tt_ipc_destroy(ipc);

    while ((__ipc_fev_num < __fiber_num * __ev_per_fiber) &&
           ((fev = tt_fiber_recv(cfb, TT_TRUE)) != NULL)) {
        if (fev->src != NULL) {
            if (fev->ev != 0xaabbccdd) {
                __err_line = __LINE__;
            }
            fev->ev = 0xddccbbaa;
        } else {
            if (fev->ev != 0x11223344) {
                __err_line = __LINE__;
            }
        }
        ++__ipc_fev_num;
        tt_fiber_finish(fev);
    }

    return TT_SUCCESS;
}

tt_result_t __ipc_cli_fev(IN void *param)
{
    tt_fiber_t *t = tt_fiber_find("svr");
    tt_u32_t i;

    i = 0;
    while (i++ < __ev_per_fiber) {
        if (tt_rand_u32() % 2) {
            tt_fiber_ev_t fev;
            tt_fiber_ev_init(&fev, 0xaabbccdd);
            tt_fiber_send(t, &fev, TT_TRUE);
            if (fev.ev != 0xddccbbaa) {
                __err_line = __LINE__;
            }
        } else {
            tt_fiber_ev_t *fev = tt_fiber_ev_create(0x11223344, 0);
            tt_fiber_send(t, fev, TT_FALSE);
        }
    }

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_ipc_fiber_ev)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_task_t t;
    tt_result_t ret;
    tt_process_t proc;
    tt_char_t *argv[20] = {0};
    tt_char_t *path;
    tt_u32_t n;

    TT_TEST_CASE_ENTER()

#if TT_ENV_OS_IS_WINDOWS
    tt_fcreate(__IPC_PATH, NULL);
#endif

    ret = tt_task_create(&t, NULL);
    TT_UT_SUCCESS(ret, "");

    tt_task_add_fiber(&t, "svr", __ipc_svr_fev, NULL, NULL);
    n = 0;
    while (n++ < __fiber_num) {
        tt_task_add_fiber(&t, NULL, __ipc_cli_fev, NULL, NULL);
    }

    ret = tt_task_run(&t);
    TT_UT_SUCCESS(ret, "");

    path = tt_process_path(NULL);
    TT_UT_NOT_EQUAL(path, NULL, "");
    argv[0] = "app_unit_test";
    argv[1] = "ipc-1";
    argv[2] = __IPC_PATH;

    ret = tt_process_create(&proc, __app_file, argv, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_free(path);

    tt_task_wait(&t);

    ret = tt_process_wait(&proc, TT_TRUE, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    TT_UT_EQUAL(__err_line, 0, "");
    TT_UT_EQUAL(__ipc_fev_num, __fiber_num * __ev_per_fiber, "");

    // test end
    TT_TEST_CASE_LEAVE()
}
