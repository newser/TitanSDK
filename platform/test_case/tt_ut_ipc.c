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
//#define __app_file "./unit_test"
#define __app_file path
#define __app_file_sc "./测试"
#endif

#define __CONN_NUM 10
#define __ROUND_NUM 100

#if 0
#define TT_INFO_IPC TT_INFO
#else
#define TT_INFO_IPC(...)
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
TT_TEST_ROUTINE_DECLARE(case_ipc_basic)
TT_TEST_ROUTINE_DECLARE(case_ipc_client)
TT_TEST_ROUTINE_DECLARE(case_ipc_stress)
TT_TEST_ROUTINE_DECLARE(case_ipc_fiber_ev)
TT_TEST_ROUTINE_DECLARE(case_ipc_ev)
TT_TEST_ROUTINE_DECLARE(case_ipc_skt)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(ipc_case)

TT_TEST_CASE("case_ipc_basic",
             "testing ipc basic test",
             case_ipc_basic,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("case_ipc_client",
                 "testing ipc client",
                 case_ipc_client,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_ipc_stress",
                 "testing ipc stress test",
                 case_ipc_stress,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_ipc_fiber_ev",
                 "testing ipc fiber event",
                 case_ipc_fiber_ev,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_ipc_ev",
                 "testing ipc event",
                 case_ipc_ev,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_ipc_skt",
                 "testing ipc sendskt",
                 case_ipc_skt,
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
    TT_TEST_ROUTINE_DEFINE(case_ipc_client)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    static tt_u32_t __ipc_err_line;
static tt_s64_t __ipc_max_diff;

tt_export tt_result_t __ipc_cli_1(IN void *param)
{
    tt_ipc_t *ipc;
    tt_u8_t sbuf[11] = "1234567890", rbuf[11] = {0};
    tt_u32_t n, len, cn, rn;
    tt_fiber_ev_t *fev;
    tt_tmr_t *tmr;
    tt_char_t addr[200];
    tt_result_t ret;
    tt_skt_t *skt;

    cn = 0;
    while (cn++ < __CONN_NUM) {
        TT_INFO_IPC("cli con %d", cn);

        ipc = tt_ipc_create(NULL, NULL);
        if (ipc == NULL) {
            __ipc_err_line = __LINE__;
            TT_INFO_IPC("err: %d", __ipc_err_line);
            return TT_FAIL;
        }

        // not connected yet, local: empty
        ret = tt_ipc_local_addr(ipc, NULL, sizeof(addr), &n);
        if (!TT_OK(ret)) {
            __ipc_err_line = __LINE__;
            TT_INFO_IPC("err: %d", __ipc_err_line);
            return TT_FAIL;
        } /* // linux abstract socket...
        else if (n != 0) {
            __ipc_err_line = __LINE__;
            TT_INFO_IPC("err: %d, n: %d, addr[0]: %x", __ipc_err_line, n,
        addr[0]);
            return TT_FAIL;
        }*/
        // not connected yet, remote: none
        if (TT_OK(tt_ipc_remote_addr(ipc, NULL, sizeof(addr), &n))) {
            __ipc_err_line = __LINE__;
            TT_INFO_IPC("err: %d", __ipc_err_line);
            return TT_FAIL;
        }

#if TT_ENV_OS_IS_WINDOWS
        tt_sleep(50);
#endif
        if (!TT_OK(tt_ipc_connect_retry(ipc, __IPC_PATH, 100, 10))) {
            __ipc_err_line = __LINE__;
            TT_INFO_IPC("err: %d", __ipc_err_line);
            return TT_FAIL;
        }

        // connected, local: empty
        if (!TT_OK(tt_ipc_local_addr(ipc, NULL, sizeof(addr), &n)) /*||
            (n != 0)*/) {
            __ipc_err_line = __LINE__;
            TT_INFO_IPC("err: %d", __ipc_err_line);
            return TT_FAIL;
        }
        if (!TT_OK(tt_ipc_remote_addr(ipc, NULL, sizeof(addr), &n)) /*||
            (n != tt_strlen(__IPC_PATH))*/) {
            __ipc_err_line = __LINE__;
            TT_INFO_IPC("err: %d", __ipc_err_line);
            return TT_FAIL;
        }
        if (tt_ipc_remote_addr(ipc, addr, 1, &n) != TT_E_NOSPC) {
            __ipc_err_line = __LINE__;
            TT_INFO_IPC("err: %d", __ipc_err_line);
            return TT_FAIL;
        }
        if (!TT_OK(tt_ipc_remote_addr(ipc, addr, sizeof(addr), NULL)) /*||
            (tt_strcmp(addr, __IPC_PATH) != 0)*/) {
            __ipc_err_line = __LINE__;
            TT_INFO_IPC("err: %d", __ipc_err_line);
            return TT_FAIL;
        }

        rn = 0;
        while (rn++ < __ROUND_NUM) {
            if (!TT_OK(tt_ipc_send(ipc, sbuf, sizeof(sbuf), &n))) {
                TT_INFO_IPC("err %d", __LINE__);
                __ipc_err_line = __LINE__;
                return TT_FAIL;
            }
            TT_INFO_IPC("[%d] cli sent %d", rn, n);
            if (sizeof(sbuf) != n) {
                __ipc_err_line = __LINE__;
                return TT_FAIL;
            }

            len = 0;
            while (len < sizeof(rbuf)) {
                if (!TT_OK(tt_ipc_recv(ipc,
                                       rbuf + len,
                                       sizeof(rbuf) - len,
                                       &n,
                                       &fev,
                                       &tmr,
                                       &skt))) {
                    TT_INFO_IPC("err %d", __LINE__);
                    __ipc_err_line = __LINE__;
                    return TT_FAIL;
                }
                len += n;
            }
            TT_ASSERT(len == sizeof(rbuf));
            TT_INFO_IPC("[%d] cli recv %d, %s", rn, n, rbuf);
            if (tt_strcmp((tt_char_t *)rbuf, "0987654321") != 0) {
                __ipc_err_line = __LINE__;
                return TT_FAIL;
            }
        }

        tt_ipc_destroy(ipc);
    }

    return TT_SUCCESS;
}

tt_export tt_result_t __ipc_svr_1(IN void *param)
{
    tt_ipc_t *ipc, *new_ipc;
    tt_u8_t sbuf[11] = "0987654321", rbuf[11];
    tt_u32_t n, len, cn, rn;
    tt_result_t ret;
    tt_fiber_ev_t *fev;
    tt_tmr_t *tmr;
    tt_char_t addr[200];
    tt_skt_t *skt;

    (void)rn;

    ipc = tt_ipc_create(__IPC_PATH, NULL);
    if (ipc == NULL) {
        __ipc_err_line = __LINE__;
        return TT_FAIL;
    }

    // binded, locad: path
    if (!TT_OK(tt_ipc_local_addr(ipc, NULL, sizeof(addr), &n)) ||
        (n != tt_strlen(__IPC_PATH) + 1)) {
        __ipc_err_line = __LINE__;
        return TT_FAIL;
    }
    if (tt_ipc_local_addr(ipc, addr, 1, &n) != TT_E_NOSPC) {
        __ipc_err_line = __LINE__;
        return TT_FAIL;
    }
    if (!TT_OK(tt_ipc_local_addr(ipc, addr, sizeof(addr), NULL)) ||
        (tt_strcmp(addr, __IPC_PATH) != 0)) {
        __ipc_err_line = __LINE__;
        return TT_FAIL;
    }

    // binded, remote: none
    if (TT_OK(tt_ipc_remote_addr(ipc, addr, sizeof(addr), &n))) {
        // not connected yet
        __ipc_err_line = __LINE__;
        return TT_FAIL;
    }

    cn = 0;
    while (cn++ < __CONN_NUM) {
        TT_INFO_IPC("svr acc %d", cn);

        while ((new_ipc = tt_ipc_accept(ipc, NULL, &fev, &tmr)) == NULL) {
            if (fev == NULL && tmr == NULL) {
                __ipc_err_line = __LINE__;
                return TT_FAIL;
            }

            if (fev != NULL) {
                tt_fiber_finish(fev);
            }
        }
        if (fev != NULL) {
            tt_fiber_finish(fev);
        }

        // accepted, local: path
        if (!TT_OK(tt_ipc_local_addr(new_ipc, NULL, sizeof(addr), &n)) /*||
            (n != tt_strlen(__IPC_PATH))*/) {
            __ipc_err_line = __LINE__;
            return TT_FAIL;
        }
        if (tt_ipc_local_addr(new_ipc, addr, 1, &n) != TT_E_NOSPC) {
            __ipc_err_line = __LINE__;
            return TT_FAIL;
        }
        if (!TT_OK(tt_ipc_local_addr(new_ipc, addr, sizeof(addr), NULL)) /*||
            (tt_strcmp(addr, __IPC_PATH) != 0)*/) {
            __ipc_err_line = __LINE__;
            return TT_FAIL;
        }

        // accepted, remote: empty
        if (!TT_OK(tt_ipc_remote_addr(new_ipc, addr, sizeof(addr), &n))) {
            __ipc_err_line = __LINE__;
            return TT_FAIL;
        }

        rn = 0;
        len = 0;
        while (TT_OK((ret = tt_ipc_recv(new_ipc,
                                        rbuf + len,
                                        sizeof(rbuf) - len,
                                        &n,
                                        &fev,
                                        &tmr,
                                        &skt)))) {
            len += n;
            if (len < sizeof(rbuf)) {
                continue;
            }
            TT_ASSERT(len == sizeof(rbuf));
            len = 0;
            TT_INFO_IPC("[%d] svr recv %d, %s", rn, n, rbuf);
            if (tt_strcmp((tt_char_t *)rbuf, "1234567890") != 0) {
                __ipc_err_line = __LINE__;
                return TT_FAIL;
            }

            if (!TT_OK(tt_ipc_send(new_ipc, sbuf, sizeof(sbuf), &n))) {
                __ipc_err_line = __LINE__;
                return TT_FAIL;
            }
            if (sizeof(sbuf) != n) {
                __ipc_err_line = __LINE__;
                return TT_FAIL;
            }
            TT_INFO_IPC("[%d] svr sent %d", rn, n);
        }
        if (ret != TT_E_END) {
            __ipc_err_line = __LINE__;
            return TT_FAIL;
        }

        TT_INFO_IPC("svr acc done %d, ret: %x", cn, ret);
        tt_ipc_destroy(new_ipc);
    }

    tt_ipc_destroy(ipc);

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(case_ipc_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_task_t t;
    tt_result_t ret;
    tt_process_t proc;
    tt_char_t *argv[20] = {0};
    tt_char_t *path;

    TT_TEST_CASE_ENTER()

#if TT_ENV_OS_IS_IOS || TT_ENV_OS_IS_ANDROID
    return TT_SUCCESS;
#endif

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
    argv[0] = "unit_test";
    argv[1] = "ipc-1";
    argv[2] = __IPC_PATH;

    ret = tt_process_create(&proc, __app_file, argv, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_free(path);

    tt_task_wait(&t);

    ret = tt_process_wait(&proc, TT_TRUE, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    TT_UT_EQUAL(__ipc_err_line, 0, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_result_t __ipc_svr_acc_2(IN void *param)
{
    tt_ipc_t *new_ipc = (tt_ipc_t *)param;
    tt_u8_t sbuf[11] = "0987654321", rbuf[11];
    tt_u32_t n, len = 0, rn;
    tt_result_t ret;
    tt_fiber_ev_t *fev;
    tt_tmr_t *tmr;
    tt_skt_t *skt;

    rn = 0;
    (void)rn;
    while (TT_OK((ret = tt_ipc_recv(new_ipc,
                                    rbuf + len,
                                    sizeof(rbuf) - len,
                                    &n,
                                    &fev,
                                    &tmr,
                                    &skt)))) {
        len += n;
        if (len < sizeof(rbuf)) {
            continue;
        }
        TT_ASSERT(len == sizeof(rbuf));
        len = 0;
        if (tt_strcmp((tt_char_t *)rbuf, "1234567890") != 0) {
            __ipc_err_line = __LINE__;
            return TT_FAIL;
        }
        TT_INFO_IPC("[%d] acc recv %d", rn, n);

        if (!TT_OK(tt_ipc_send(new_ipc, sbuf, sizeof(sbuf), &n))) {
            __ipc_err_line = __LINE__;
            return TT_FAIL;
        }
        if (sizeof(sbuf) != n) {
            __ipc_err_line = __LINE__;
            return TT_FAIL;
        }
        TT_INFO_IPC("[%d] acc sent %d", rn, n);

        if (tt_rand_u32() % __CONN_NUM == 0) {
            TT_INFO("server close ipc");
            tt_ipc_destroy(new_ipc);
            return TT_SUCCESS;
        }
    }
    if (ret != TT_E_END) {
        __ipc_err_line = __LINE__;
        return TT_FAIL;
    }

    tt_ipc_destroy(new_ipc);

    return TT_SUCCESS;
}

static tt_process_t proc[__CONN_NUM];
static tt_ipc_t *__stress_ipc;

static tt_result_t __ipc_svr_2(IN void *param)
{
    tt_ipc_t *ipc, *new_ipc;
    tt_fiber_t *fb;
    tt_u32_t cn;
    tt_char_t *argv[20] = {0};
    tt_char_t *path;
    tt_result_t ret;

    __stress_ipc = tt_ipc_create(__IPC_PATH, NULL);
    if (__stress_ipc == NULL) {
        __ipc_err_line = __LINE__;
        return TT_FAIL;
    }
    ipc = __stress_ipc;

    path = tt_process_path(NULL);
    TT_ASSERT(path != NULL);
    argv[0] = "unit_test";
    argv[1] = "ipc-2";
    argv[2] = __IPC_PATH;

    cn = 0;
    while (cn < __CONN_NUM) {
        tt_fiber_ev_t *fev;
        tt_tmr_t *tmr;

        TT_INFO_IPC("svr acc %d", cn);

        ret = tt_process_create(&proc[cn], __app_file, argv, NULL);
        if (!TT_OK(ret)) {
            __ipc_err_line = __LINE__;
            return TT_FAIL;
        }

        while ((new_ipc = tt_ipc_accept(ipc, NULL, &fev, &tmr)) == NULL) {
            if (fev == NULL && tmr == NULL) {
                __ipc_err_line = __LINE__;
                return TT_FAIL;
            }

            if (fev != NULL) {
                tt_fiber_finish(fev);
            }
        }
        TT_INFO("new ipc");
        if (fev != NULL) {
            tt_fiber_finish(fev);
        }

        fb = tt_fiber_create(NULL, __ipc_svr_acc_2, new_ipc, NULL);
        if (fb == NULL) {
            __ipc_err_line = __LINE__;
            return TT_FAIL;
        }
        tt_fiber_resume(fb, TT_FALSE);

        ++cn;
    }

    tt_free(path);

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(case_ipc_stress)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_task_t t;
    tt_result_t ret;
    tt_u32_t cn;

    TT_TEST_CASE_ENTER()

#if TT_ENV_OS_IS_IOS || TT_ENV_OS_IS_ANDROID
    return TT_SUCCESS;
#endif

#if TT_ENV_OS_IS_WINDOWS
    tt_fcreate(__IPC_PATH, NULL);
#endif

    ret = tt_task_create(&t, NULL);
    TT_UT_SUCCESS(ret, "");

    tt_task_add_fiber(&t, "svr", __ipc_svr_2, NULL, NULL);

    ret = tt_task_run(&t);
    TT_UT_SUCCESS(ret, "");
    tt_task_wait(&t);

    if (__ipc_err_line == 0) {
        cn = 0;
        while (cn < __CONN_NUM) {
            if (!TT_OK(tt_process_wait(&proc[cn], TT_TRUE, NULL))) {
                __ipc_err_line = __LINE__;
            }

            ++cn;
        }
    }

    if (__stress_ipc != NULL) {
        tt_ipc_destroy(__stress_ipc);
    }

    TT_UT_EQUAL(__ipc_err_line, 0, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

tt_export tt_result_t __ipc_cli_oneshot(IN void *param)
{
    tt_ipc_t *ipc;
    tt_u8_t sbuf[11] = "1234567890", rbuf[11];
    tt_u32_t n, len, rn;
    tt_fiber_ev_t *fev;
    tt_tmr_t *tmr;
    tt_skt_t *skt;

    ipc = tt_ipc_create(NULL, NULL);
    if (ipc == NULL) {
        __ipc_err_line = __LINE__;
        return TT_FAIL;
    }

#if TT_ENV_OS_IS_WINDOWS
    tt_sleep(50);
#endif
    if (!TT_OK(tt_ipc_connect_retry(ipc, __IPC_PATH, 100, 10))) {
        __ipc_err_line = __LINE__;
        return TT_FAIL;
    }

    rn = 0;
    while (rn++ < __ROUND_NUM) {
        if (!TT_OK(tt_ipc_send(ipc, sbuf, sizeof(sbuf), &n))) {
            __ipc_err_line = __LINE__;
            return TT_FAIL;
        }
        if (sizeof(sbuf) != n) {
            __ipc_err_line = __LINE__;
            return TT_FAIL;
        }
        TT_INFO_IPC("[%d] cli sent %d", rn, n);

        len = 0;
        while (len < sizeof(rbuf)) {
            if (!TT_OK(tt_ipc_recv(ipc,
                                   rbuf,
                                   sizeof(rbuf),
                                   &n,
                                   &fev,
                                   &tmr,
                                   &skt))) {
                __ipc_err_line = __LINE__;
                return TT_FAIL;
            }
            len += n;
        }
        TT_ASSERT(len == sizeof(rbuf));
        if (tt_strcmp((tt_char_t *)rbuf, "0987654321") != 0) {
            __ipc_err_line = __LINE__;
            return TT_FAIL;
        }
        TT_INFO_IPC("[%d] cli recv %d", rn, n);
    }

    tt_ipc_destroy(ipc);

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(case_ipc_client)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_task_t t;
    tt_result_t ret;
    tt_process_t proc;
    tt_char_t *argv[20] = {0};
    tt_char_t *path;

    TT_TEST_CASE_ENTER()

#if TT_ENV_OS_IS_IOS || TT_ENV_OS_IS_ANDROID
    return TT_SUCCESS;
#endif

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
    argv[0] = "unit_test";
    argv[1] = "ipc-svr";
    argv[2] = __IPC_PATH;

    ret = tt_process_create(&proc, __app_file, argv, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_free(path);

    tt_task_wait(&t);

    ret = tt_process_wait(&proc, TT_TRUE, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    TT_UT_EQUAL(__ipc_err_line, 0, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

#define __fiber_num 10
#define __ev_per_fiber 100

static tt_u32_t __ipc_fev_num;

tt_result_t __ipc_svr_fev(IN void *param)
{
    tt_ipc_t *ipc, *new_ipc;
    tt_u8_t sbuf[11] = "0987654321", rbuf[11] = {0};
    tt_u32_t n, len, cn, rn;
    tt_result_t ret;
    tt_fiber_ev_t *fev;
    tt_fiber_t *cfb = tt_current_fiber();
    tt_tmr_t *tmr, *e_tmr;
    tt_skt_t *skt;

    (void)rn;

    ipc = tt_ipc_create(__IPC_PATH, NULL);
    if (ipc == NULL) {
        __ipc_err_line = __LINE__;
        return TT_FAIL;
    }

    cn = 0;
    while (cn++ < __CONN_NUM) {
        TT_INFO_IPC("svr acc %d", cn);

        do {
            new_ipc = tt_ipc_accept(ipc, NULL, &fev, &tmr);

            if (new_ipc == NULL && fev == NULL && tmr == NULL) {
                __ipc_err_line = __LINE__;
                return TT_FAIL;
            }

            if (fev != NULL) {
                TT_INFO_IPC("svr acc event");
                if (fev->src != NULL) {
                    if (fev->ev != 0xaabbccdd) {
                        __ipc_err_line = __LINE__;
                    }
                    fev->ev = 0xddccbbaa;
                } else {
                    if (fev->ev != 0x11223344) {
                        __ipc_err_line = __LINE__;
                    }
                }
                ++__ipc_fev_num;
                tt_fiber_finish(fev);
            }
        } while (new_ipc == NULL);

        tmr = tt_tmr_create(tt_rand_u32() % 5 + 5,
                            cn,
                            (void *)(tt_uintptr_t)tt_time_ref());
        if (tmr == NULL) {
            __ipc_err_line = __LINE__;
            return TT_FAIL;
        }
        tt_tmr_start(tmr);

        rn = 0;
        len = 0;
        while (TT_OK((ret = tt_ipc_recv(new_ipc,
                                        rbuf + len,
                                        sizeof(rbuf) - len,
                                        &n,
                                        &fev,
                                        &e_tmr,
                                        &skt)))) {
            if (fev != NULL) {
                if (fev->src != NULL) {
                    if (fev->ev != 0xaabbccdd) {
                        __ipc_err_line = __LINE__;
                    }
                    fev->ev = 0xddccbbaa;
                } else {
                    if (fev->ev != 0x11223344) {
                        __ipc_err_line = __LINE__;
                    }
                }
                ++__ipc_fev_num;
                tt_fiber_finish(fev);
            }

            if (e_tmr != NULL) {
                tt_s64_t now = tt_time_ref();
                now -= (tt_s64_t)(tt_uintptr_t)e_tmr->param;
                now = labs((long)now);
                now = tt_time_ref2ms(now);
                if (now > __ipc_max_diff) {
                    __ipc_max_diff = now;
                }
            }

            len += n;
            if (len < sizeof(rbuf)) {
                continue;
            }
            TT_ASSERT(len == sizeof(rbuf));
            len = 0;
            TT_INFO_IPC("[%d] svr recv %d, %s", rn, n, rbuf);
            if (tt_strcmp((tt_char_t *)rbuf, "1234567890") != 0) {
                __ipc_err_line = __LINE__;
                return TT_FAIL;
            }
            tt_memset(rbuf, 0, sizeof(rbuf));

            if (!TT_OK(tt_ipc_send(new_ipc, sbuf, sizeof(sbuf), &n))) {
                __ipc_err_line = __LINE__;
                return TT_FAIL;
            }
            if (sizeof(sbuf) != n) {
                __ipc_err_line = __LINE__;
                return TT_FAIL;
            }
            TT_INFO_IPC("[%d] svr sent %d", rn, n);
        }
        if (ret != TT_E_END) {
            __ipc_err_line = __LINE__;
            return TT_FAIL;
        }

        TT_INFO_IPC("svr acc done %d, ret: %x", cn, ret);
        tt_ipc_destroy(new_ipc);
    }

    tt_ipc_destroy(ipc);

    while ((__ipc_fev_num < __fiber_num * __ev_per_fiber) &&
           ((fev = tt_fiber_recv_ev(cfb, TT_TRUE)) != NULL)) {
        if (fev->src != NULL) {
            if (fev->ev != 0xaabbccdd) {
                __ipc_err_line = __LINE__;
            }
            fev->ev = 0xddccbbaa;
        } else {
            if (fev->ev != 0x11223344) {
                __ipc_err_line = __LINE__;
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

    if (t == NULL) {
        __ipc_err_line = __LINE__;
        return TT_FAIL;
    }

    i = 0;
    while (i++ < __ev_per_fiber) {
        if (tt_rand_u32() % 2) {
            tt_fiber_ev_t fev;
            tt_fiber_ev_init(&fev, 0xaabbccdd);
            tt_fiber_send_ev(t, &fev, TT_TRUE);
            if (fev.ev != 0xddccbbaa) {
                __ipc_err_line = __LINE__;
            }
        } else {
            tt_fiber_ev_t *fev = tt_fiber_ev_create(0x11223344, 0);
            tt_fiber_send_ev(t, fev, TT_FALSE);
        }
    }

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(case_ipc_fiber_ev)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_task_t t;
    tt_result_t ret;
    tt_process_t proc;
    tt_char_t *argv[20] = {0};
    tt_char_t *path;
    tt_u32_t n;

    TT_TEST_CASE_ENTER()

#if TT_ENV_OS_IS_IOS || TT_ENV_OS_IS_ANDROID
    return TT_SUCCESS;
#endif

#if TT_ENV_OS_IS_WINDOWS
    tt_fcreate(__IPC_PATH, NULL);
#endif

    __ipc_max_diff = 0;

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
    argv[0] = "unit_test";
    argv[1] = "ipc-1";
    argv[2] = __IPC_PATH;

    ret = tt_process_create(&proc, __app_file, argv, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_free(path);

    tt_task_wait(&t);

    ret = tt_process_wait(&proc, TT_TRUE, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    TT_UT_EQUAL(__ipc_err_line, 0, "");
    TT_UT_EQUAL(__ipc_fev_num, __fiber_num * __ev_per_fiber, "");
    // TT_UT_EXP(__ipc_max_diff < 10, "");
    TT_RECORD_INFO("max diff: %d", __ipc_max_diff);

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_u32_t ___ipc_cli_sent, __ipc_cli_recvd;
static tt_u32_t __ipc_svr_sent, __ipc_svr_recvd;

tt_export tt_result_t __ipc_cli_pev(IN void *param)
{
    tt_ipc_t *ipc;
    tt_u8_t sbuf[11] = "1234567890", rbuf[11] = {0};
    tt_u32_t n, cn, rn;
    tt_ipc_ev_t *pev;
    tt_result_t ret;
    tt_fiber_ev_t *fev;
    tt_tmr_t *tmr;
    tt_skt_t *skt;

    cn = 0;
    while (cn++ < __CONN_NUM) {
        ipc = tt_ipc_create(NULL, NULL);
        if (ipc == NULL) {
            __ipc_err_line = __LINE__;
            TT_INFO_IPC("err: %d", __ipc_err_line);
            return TT_FAIL;
        }

#if TT_ENV_OS_IS_WINDOWS
        tt_sleep(50);
#endif
        if (!TT_OK(tt_ipc_connect_retry(ipc, __IPC_PATH, 100, 10))) {
            __ipc_err_line = __LINE__;
            TT_INFO_IPC("err: %d", __ipc_err_line);
            return TT_FAIL;
        }
        TT_INFO("[%d] ipc connected", cn);

        rn = 0;
        while (rn++ < __ROUND_NUM) {
            n = tt_rand_u32() & 0xFF;
            pev = tt_ipc_ev_create(n, n);
            if (pev != NULL) {
                tt_u8_t *pev_data = TT_IPC_EV_CAST(pev, tt_u8_t);
                tt_u32_t i;

                TT_ASSERT(n == pev->ev);
                TT_ASSERT(n == pev->size);
                for (i = 0; i < pev->size; ++i) {
                    pev_data[i] = n;
                }
            } else {
                __ipc_err_line = __LINE__;
            }

            if (!TT_OK(tt_ipc_send_ev(ipc, pev))) {
                __ipc_err_line = __LINE__;
                return TT_FAIL;
            }
            ___ipc_cli_sent += sizeof(tt_ipc_ev_t) + n;
            TT_INFO_IPC("[%d:%d] cli sent ev %d, total: %d",
                        cn,
                        rn,
                        n,
                        ___ipc_cli_sent);

            if (TT_OK((ret = tt_ipc_recv_ev(ipc, &pev, &fev, &tmr, &skt)))) {
                if (pev != NULL) {
                    tt_u8_t *pev_data = TT_IPC_EV_CAST(pev, tt_u8_t);
                    tt_u32_t i;

                    __ipc_cli_recvd += sizeof(tt_ipc_ev_t) + pev->ev;
                    TT_INFO_IPC("[%d:%d] cli recv ev %d, total",
                                cn,
                                rn,
                                pev->ev,
                                __ipc_cli_recvd);
                    for (i = 0; i < pev->size; ++i) {
                        if (pev_data[i] != pev->size) {
                            __ipc_err_line = __LINE__;
                        }
                    }
                }
            } else {
                TT_INFO_IPC("[%d:%d] cli recv error", cn, rn);
                __ipc_err_line = __LINE__;
            }
        }
        TT_INFO_IPC("[%d] cli end", cn);

        tt_ipc_destroy(ipc);
    }

    return TT_SUCCESS;
}

tt_result_t __ipc_svr_pev_fev(IN void *param)
{
    tt_ipc_t *ipc, *new_ipc;
    tt_u32_t n, cn, rn;
    tt_result_t ret;
    tt_fiber_ev_t *fev;
    tt_fiber_t *cfb = tt_current_fiber();
    tt_ipc_ev_t *pev;
    tt_tmr_t *tmr, *e_tmr;
    tt_skt_t *skt;

    (void)rn;

    ipc = tt_ipc_create(__IPC_PATH, NULL);
    if (ipc == NULL) {
        __ipc_err_line = __LINE__;
        return TT_FAIL;
    }

    cn = 0;
    while (cn++ < __CONN_NUM) {
        tt_fiber_ev_t *fev;

        do {
            new_ipc = tt_ipc_accept(ipc, NULL, &fev, &tmr);

            if (new_ipc == NULL && fev == NULL && tmr == NULL) {
                __ipc_err_line = __LINE__;
                return TT_FAIL;
            }

            if (fev != NULL) {
                TT_INFO_IPC("svr acc event");
                if (fev->src != NULL) {
                    if (fev->ev != 0xaabbccdd) {
                        __ipc_err_line = __LINE__;
                    }
                    fev->ev = 0xddccbbaa;
                } else {
                    if (fev->ev != 0x11223344) {
                        __ipc_err_line = __LINE__;
                    }
                }
                ++__ipc_fev_num;
                tt_fiber_finish(fev);
            }
        } while (new_ipc == NULL);
        TT_INFO_IPC("ipc accepted %d", cn);

        rn = 0;
        while (
            TT_OK((ret = tt_ipc_recv_ev(new_ipc, &pev, &fev, &e_tmr, &skt)))) {
            if (fev != NULL) {
                TT_INFO_IPC("[%d:%d] svr recv fiber ev %x", cn, rn, fev->ev);
                if (fev->src != NULL) {
                    if (fev->ev != 0xaabbccdd) {
                        __ipc_err_line = __LINE__;
                    }
                    fev->ev = 0xddccbbaa;
                } else {
                    if (fev->ev != 0x11223344) {
                        __ipc_err_line = __LINE__;
                    }
                }
                ++__ipc_fev_num;
                tt_fiber_finish(fev);
            }

            if (pev != NULL) {
                tt_u8_t *pev_data = TT_IPC_EV_CAST(pev, tt_u8_t);
                tt_u32_t i;

                __ipc_svr_recvd += sizeof(tt_ipc_ev_t) + pev->size;
                TT_INFO_IPC("[%d:%d] svr recv ipc ev %x, total: %d",
                            cn,
                            rn,
                            pev->ev,
                            __ipc_svr_recvd);
                TT_ASSERT(pev->ev == pev->size);
                for (i = 0; i < pev->size; ++i) {
                    pev_data[i] = pev->ev;
                }
            } else {
                continue;
            }

            if (e_tmr != NULL) {
                tt_s64_t now = tt_time_ref();
                now -= (tt_s64_t)(tt_uintptr_t)e_tmr->param;
                now = labs((long)now);
                now = tt_time_ref2ms(now);
                if (now > __ipc_max_diff) {
                    __ipc_max_diff = now;
                }

                if (e_tmr->ev == ~0) {
                    tt_tmr_set_param(e_tmr,
                                     (void *)(tt_uintptr_t)tt_time_ref());
                    tt_tmr_start(e_tmr);
                } else {
                    now = tt_rand_u32() % 3;
                    if (now == 0) {
                        tt_tmr_destroy(e_tmr);
                    } else if (now == 1) {
                        tt_tmr_stop(e_tmr);
                    } else {
                        tt_tmr_set_delay(e_tmr, tt_rand_u32() % 5 + 5);
                        tt_tmr_set_param(e_tmr,
                                         (void *)(tt_uintptr_t)tt_time_ref());
                        tt_tmr_start(e_tmr);
                    }
                }
            }

            if (tt_rand_u32() % 100 == 0) {
                tmr = tt_tmr_create(tt_rand_u32() % 5 + 5,
                                    0,
                                    (void *)(tt_uintptr_t)tt_time_ref());
                if (tmr == NULL) {
                    __ipc_err_line = __LINE__;
                    return TT_FAIL;
                }
                tt_tmr_start(tmr);
            }

            n = tt_rand_u32() & 0xFF;
            pev = tt_ipc_ev_create(n, n);
            ret = tt_ipc_send_ev(new_ipc, pev);
            if (!TT_OK(ret)) {
                break; // client may already disconnect
                if (ret == TT_E_END) {
                    break;
                } else {
                    __ipc_err_line = __LINE__;
                    return TT_FAIL;
                }
            }
            __ipc_svr_sent += sizeof(tt_ipc_ev_t) + pev->size;
            TT_INFO_IPC("[%d:%d] svr sent ev %d, total: %d",
                        cn,
                        rn,
                        n,
                        __ipc_svr_sent);
        }
// client may already disconnect
#if 0
        if (ret != TT_E_END) {
            __ipc_err_line = __LINE__;
            return TT_FAIL;
        }
#endif

        TT_INFO_IPC("svr acc done %d, ret: %x", cn, ret);
        tt_ipc_destroy(new_ipc);
    }

    tt_ipc_destroy(ipc);

    while ((__ipc_fev_num < __fiber_num * __ev_per_fiber) &&
           ((fev = tt_fiber_recv_ev(cfb, TT_TRUE)) != NULL)) {
        if (fev->src != NULL) {
            if (fev->ev != 0xaabbccdd) {
                __ipc_err_line = __LINE__;
            }
            fev->ev = 0xddccbbaa;
        } else {
            if (fev->ev != 0x11223344) {
                __ipc_err_line = __LINE__;
            }
        }
        ++__ipc_fev_num;
        tt_fiber_finish(fev);
    }

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(case_ipc_ev)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_task_t t;
    tt_result_t ret;
    tt_process_t proc;
    tt_char_t *argv[20] = {0};
    tt_char_t *path;
    tt_u32_t n;

    TT_TEST_CASE_ENTER()

#if TT_ENV_OS_IS_IOS || TT_ENV_OS_IS_ANDROID
    return TT_SUCCESS;
#endif

#if TT_ENV_OS_IS_WINDOWS
    tt_fcreate(__IPC_PATH, NULL);
#endif

    __ipc_fev_num = 0;
    __ipc_max_diff = 0;

    ret = tt_task_create(&t, NULL);
    TT_UT_SUCCESS(ret, "");

    tt_task_add_fiber(&t, "svr", __ipc_svr_pev_fev, NULL, NULL);
    n = 0;
    while (n++ < __fiber_num) {
        tt_task_add_fiber(&t, NULL, __ipc_cli_fev, NULL, NULL);
    }

    ret = tt_task_run(&t);
    TT_UT_SUCCESS(ret, "");

    path = tt_process_path(NULL);
    TT_UT_NOT_EQUAL(path, NULL, "");
    argv[0] = "unit_test";
    argv[1] = "ipc-pev";
    argv[2] = __IPC_PATH;

    ret = tt_process_create(&proc, __app_file, argv, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_free(path);

    tt_task_wait(&t);

    ret = tt_process_wait(&proc, TT_TRUE, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    TT_UT_EQUAL(__ipc_err_line, 0, "");
    TT_UT_EQUAL(__ipc_fev_num, __fiber_num * __ev_per_fiber, "");
    TT_RECORD_INFO("max diff: %d", __ipc_max_diff);

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_result_t __ipc_tcp_svr1(IN void *param)
{
    tt_skt_t *s, *new_s;
    tt_fiber_ev_t *p_fev;
    tt_tmr_t *p_tmr;
    tt_ipc_t *ipc = NULL;

    s = tt_tcp_server_p(TT_NET_AF_INET, NULL, "127.0.0.1", 63639);
    if (s == NULL) {
        __ipc_err_line = __LINE__;
        goto out;
    }

    new_s = tt_skt_accept(s, NULL, NULL, &p_fev, &p_tmr);
    if (new_s == NULL) {
        __ipc_err_line = __LINE__;
        goto out;
    }

    ipc = tt_ipc_create(NULL, NULL);
    if (ipc == NULL) {
        __ipc_err_line = __LINE__;
        goto out;
    }

#if TT_ENV_OS_IS_WINDOWS
    tt_sleep(50);
#endif
    if (!TT_OK(tt_ipc_connect_retry(ipc, __IPC_PATH, 100, 10))) {
        __ipc_err_line = __LINE__;
        goto out;
    }

    if (!TT_OK(tt_ipc_send_skt(ipc, new_s))) {
        __ipc_err_line = __LINE__;
        goto out;
    }

out:
    if (s != NULL) {
        tt_skt_destroy(s);
    }

    if (ipc != NULL) {
        tt_ipc_destroy(ipc);
    }

    return TT_SUCCESS;
}

static tt_result_t __ipc_tcp_cli1(IN void *param)
{
    tt_skt_t *s;
    tt_u8_t buf[20];
    tt_u32_t n, len;
    tt_result_t ret;
    tt_fiber_ev_t *fev;
    tt_tmr_t *tmr;

    s = tt_skt_create(TT_NET_AF_INET, TT_NET_PROTO_TCP, NULL);
    if (s == NULL) {
        __ipc_err_line = __LINE__;
        goto fail;
    }

    if (!TT_OK(tt_skt_connect_p(s, TT_NET_AF_INET, "127.0.0.1", 63639))) {
        __ipc_err_line = __LINE__;
        goto fail;
    }

    if (!TT_OK(tt_skt_send(s, (tt_u8_t *)"0123456789", 10, NULL))) {
        __ipc_err_line = __LINE__;
        goto fail;
    }
    if (!TT_OK(tt_skt_shutdown(s, TT_SKT_SHUT_WR))) {
        __ipc_err_line = __LINE__;
        goto fail;
    }

    len = 0;
    while (
        (len < sizeof(buf)) &&
        TT_OK(
            ret =
                tt_skt_recv(s, buf + len, sizeof(buf) - len, &n, &fev, &tmr))) {
        len += n;
    }
    if (ret != TT_E_END) {
        __ipc_err_line = __LINE__;
        goto fail;
    }

    if (len != 10 || tt_memcmp(buf, "0123456789", 10) != 0) {
        __ipc_err_line = __LINE__;
        goto fail;
    }

    tt_skt_destroy(s);
    return TT_SUCCESS;

fail:
    tt_task_exit(NULL);
    return TT_FAIL;
}

tt_export tt_result_t __ipc_skt(IN void *param)
{
    tt_ipc_t *ipc, *new_ipc;
    tt_u8_t buf[20];
    tt_u32_t n, len;
    tt_result_t ret;
    tt_fiber_ev_t *fev;
    tt_tmr_t *tmr;
    tt_skt_t *tcp = NULL;
    tt_ipc_ev_t *pev;

    ipc = tt_ipc_create(__IPC_PATH, NULL);
    if (ipc == NULL) {
        __ipc_err_line = __LINE__;
        return TT_FAIL;
    }

    new_ipc = tt_ipc_accept(ipc, NULL, &fev, &tmr);
    if (new_ipc == NULL) {
        __ipc_err_line = __LINE__;
        return TT_FAIL;
    }
    TT_INFO("accept an ipc");

    ret = tt_ipc_recv_ev(new_ipc, &pev, &fev, &tmr, &tcp);
    TT_INFO("tt_ipc_recvskt returned: %d, %p", ret, pev);
    // tt_sleep(5000);
    if (!TT_OK(ret)) {
        __ipc_err_line = __LINE__;
        return TT_FAIL;
    }
    if (pev != NULL) {
        // no event in this case
        __ipc_err_line = __LINE__;
        return TT_FAIL;
    }
    if (tcp == NULL) {
        __ipc_err_line = __LINE__;
        return TT_FAIL;
    }
    TT_INFO("received skt");

    len = 0;
    while ((len < sizeof(buf)) && TT_OK(ret = tt_skt_recv(tcp,
                                                          buf + len,
                                                          sizeof(buf) - len,
                                                          &n,
                                                          &fev,
                                                          &tmr))) {
        len += n;
    }
    if (ret != TT_E_END) {
        __ipc_err_line = __LINE__;
        return TT_FAIL;
    }

    if (!TT_OK(tt_skt_send(tcp, buf, len, NULL))) {
        __ipc_err_line = __LINE__;
        return TT_FAIL;
    }
    if (!TT_OK(tt_skt_shutdown(tcp, TT_SKT_SHUT_WR))) {
        __ipc_err_line = __LINE__;
        return TT_FAIL;
    }
    tt_skt_destroy(tcp);

    tt_ipc_destroy(new_ipc);
    tt_ipc_destroy(ipc);
    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(case_ipc_skt)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_task_t t;
    tt_result_t ret;
    tt_process_t proc;
    tt_char_t *argv[20] = {0};
    tt_char_t *path;

    TT_TEST_CASE_ENTER()

#if TT_ENV_OS_IS_IOS || TT_ENV_OS_IS_ANDROID
    return TT_SUCCESS;
#endif

#if TT_ENV_OS_IS_WINDOWS
    tt_fcreate(__IPC_PATH, NULL);
#endif

    __ipc_err_line = 0;

    ret = tt_task_create(&t, NULL);
    TT_UT_SUCCESS(ret, "");

    tt_task_add_fiber(&t, "svr", __ipc_tcp_svr1, NULL, NULL);
    tt_task_add_fiber(&t, "cli", __ipc_tcp_cli1, NULL, NULL);

    ret = tt_task_run(&t);
    TT_UT_SUCCESS(ret, "");

    path = tt_process_path(NULL);
    TT_UT_NOT_EQUAL(path, NULL, "");
    argv[0] = "unit_test";
    argv[1] = "ipc-skt";
    argv[2] = __IPC_PATH;

    ret = tt_process_create(&proc, __app_file, argv, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_free(path);

    tt_task_wait(&t);

    ret = tt_process_wait(&proc, TT_TRUE, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    TT_UT_EQUAL(__ipc_err_line, 0, "");

    // test end
    TT_TEST_CASE_LEAVE()
}
