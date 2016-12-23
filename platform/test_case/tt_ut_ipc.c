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

#if TT_ENV_OS_IS_WINDOWS
#define __app_file "Debug\\app_unit_test.exe"
#elif TT_ENV_OS_IS_IOS
extern const char *get_app_path();
#define __app_file get_app_path()
#else
#define __app_file "./app_unit_test"
#endif

#if TT_ENV_OS_IS_IOS
#define __IPC_PATH "../tmp/ipc_basic"
#else
#define __IPC_PATH "./ipc_basic"
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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_ipc_server)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_ipc_client)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_ipc_stress)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_ipc_stress_mul)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_ipc_destroy_immd)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(ipc_case)

#if 0
    TT_TEST_CASE("tt_unit_test_ipc_client",
                 "testing ipc basic client api",
                 tt_unit_test_ipc_client, NULL,
                 NULL, NULL,
                 NULL, NULL),
#endif

#if 0
    TT_TEST_CASE("tt_unit_test_ipc_server",
                 "testing ipc basic server api",
                 tt_unit_test_ipc_server, NULL, 
                 NULL, NULL,
                 NULL, NULL),
#endif

#if 1
TT_TEST_CASE("tt_unit_test_ipc_stress",
             "testing ipc stress test",
             tt_unit_test_ipc_stress,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,
#endif

#if 1
    TT_TEST_CASE("tt_unit_test_ipc_stress_mul",
                 "testing ipc stress test, multiple conn",
                 tt_unit_test_ipc_stress_mul,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),
#endif

    TT_TEST_CASE("tt_unit_test_ipc_destroy_immd",
                 "testing ipc destroy immediately",
                 tt_unit_test_ipc_destroy_immd,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(ipc_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(TEST_UNIT_IPC, 0, ipc_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    tt_u32_t __cli_mode;
static tt_u32_t __cli_closed;
#ifdef __SIMULATE_IPC_AIO_FAIL
#define __cli_num 10
#else
#define __cli_num 10
#endif

/*
TT_TEST_ROUTINE_DEFINE(tt_unit_test_ipc_destroy_immd)
{
    //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

    TT_TEST_CASE_ENTER()
    // test start

    // test end
    TT_TEST_CASE_LEAVE()
}
*/

static tt_ev_t *__uti_ev_gen()
{
    tt_ev_t *pev;
    tt_u8_t len = (tt_u8_t)tt_rand_u32() + 1;

    // use the data length as ev_id
    pev = tt_process_ev_create(len, len);
    if (pev == NULL) {
        return NULL;
    }
    tt_memset(TT_EV_DATA(pev, tt_ev_t), len, len);

    return pev;
}

static tt_bool_t __uti_ev_check(tt_ev_t *ev)
{
    tt_u8_t *ev_data = TT_EV_DATA(ev, tt_u8_t);
    tt_u32_t i;

    if (ev->ev_id != ev->data_size) {
        TT_ERROR("ev->ev_id[%d] != ev->data_size[%d]",
                 ev->ev_id,
                 ev->data_size);
        return TT_FALSE;
    }

    for (i = 0; i < ev->data_size; ++i) {
        if (ev_data[i] != (tt_u8_t)ev->data_size) {
            TT_ERROR("ev_data[%d] != ev->data_size[%d]",
                     ev_data[i],
                     ev->data_size);
            return TT_FALSE;
        }
    }

    return TT_TRUE;
}

#define __UTB_MSG_NUM 100

static tt_result_t __utb_ret = TT_FAIL;
static tt_result_t __utb_ret2 = TT_FAIL;
static tt_u32_t __utb_err_line = 0;
static tt_ipc_t __utb_acc_ipc;
static tt_ipc_t __utb_lis_ipc;

static tt_u32_t __utb_recv_num;

static void __utb_on_recv(IN struct tt_ipc_s *ipc,
                          IN tt_ipc_aioctx_t *aioctx,
                          IN TO tt_ev_t **ev,
                          IN tt_u32_t ev_num);

static void __utb_on_send(IN struct tt_ipc_s *ipc,
                          IN tt_ev_t *ev,
                          IN tt_ipc_aioctx_t *aioctx)
{
    if (aioctx->result != TT_SUCCESS) {
        if (aioctx->result == TT_END) {
            tt_async_ipc_destroy(ipc, TT_FALSE);
            return;
        }
        __utb_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    if (tt_ipc_recv_async(ipc, __utb_on_recv, NULL) != TT_SUCCESS) {
        __utb_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }
}

static void __utb_on_recv(IN struct tt_ipc_s *ipc,
                          IN tt_ipc_aioctx_t *aioctx,
                          IN TO tt_ev_t **ev,
                          IN tt_u32_t ev_num)
{
    tt_u32_t counter = (tt_u32_t)(tt_uintptr_t)aioctx->cb_param;
    tt_u32_t i;

    if (aioctx->result != TT_SUCCESS) {
        if (aioctx->result == TT_END) {
            tt_async_ipc_destroy(ipc, TT_FALSE);
            return;
        }
        __utb_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }
    if (ev_num == 0) {
        __utb_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    __utb_recv_num += ev_num;
    // TT_DEBUG("recv [%d] msg", __utb_recv_num);

    for (i = 0; i < ev_num; ++i) {
        tt_ev_t *new_ev;

        if (!__uti_ev_check(ev[i])) {
            __utb_err_line = __LINE__;
            tt_evc_exit(TT_LOCAL_EVC);
            return;
        }

        new_ev = tt_ev_copy(ev[i]);
        if (new_ev == NULL) {
            TT_ERROR("no mem for new ev");
            __utb_err_line = __LINE__;
            tt_evc_exit(TT_LOCAL_EVC);
            return;
        }

        if (tt_ipc_send_async(ipc,
                              new_ev,
                              __utb_on_send,
                              (void *)(tt_uintptr_t)(counter + i)) !=
            TT_SUCCESS) {
            __utb_err_line = __LINE__;
            tt_evc_exit(TT_LOCAL_EVC);
            return;
        }
    }
}

static void __utb_on_accept(IN struct tt_ipc_s *listening_ipc,
                            IN struct tt_ipc_s *new_ipc,
                            IN tt_ipc_aioctx_t *aioctx)
{
    if (aioctx->result != TT_SUCCESS) {
        __utb_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    if (tt_ipc_recv_async(new_ipc, __utb_on_recv, NULL) != TT_SUCCESS) {
        __utb_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }
}

static void __utb_on_destroy(IN struct tt_ipc_s *ipc, IN void *cb_param)
{
    if (ipc == &__utb_lis_ipc) {
        __utb_ret = TT_SUCCESS;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    if (__cli_mode == 0) {
        TT_ASSERT(ipc == &__utb_acc_ipc);
        tt_async_ipc_destroy(&__utb_lis_ipc, TT_FALSE);
    } else if (__cli_mode == 1) {
        ++__cli_closed;
        TT_DEBUG("closed[%d]", __cli_closed);
        if (__cli_closed == __cli_num) {
            tt_async_ipc_destroy(&__utb_lis_ipc, TT_FALSE);
        }
    }
}

static tt_result_t __utb_on_init(IN struct tt_evcenter_s *evc,
                                 IN void *on_init_param)
{
    tt_ipc_t *ipc = &__utb_lis_ipc;
    tt_result_t ret;
    tt_ipc_exit_t de = {__utb_on_destroy, NULL};

    tt_process_t child_proc;
    tt_char_t *argv[5] = {0};

    ret = tt_async_ipc_create(ipc, __IPC_PATH, NULL, &de);
    if (!TT_OK(ret)) {
        __utb_err_line = __LINE__;
        return TT_FAIL;
    }

    ret = tt_ipc_accept_async(ipc,
                              &__utb_acc_ipc,
                              NULL,
                              &de,
                              __utb_on_accept,
                              NULL);
    if (ret != TT_SUCCESS) {
        __utb_err_line = __LINE__;
        return TT_FAIL;
    }

#if 1
    argv[0] = "tsk_unit_test";
    argv[1] = "ipc";
    argv[2] = __IPC_PATH;
    ret = tt_process_create(&child_proc, __app_file, argv, NULL);
    if (ret != TT_SUCCESS) {
        __utb_err_line = __LINE__;
        return TT_FAIL;
    }
#endif

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_ipc_server)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    tt_evcenter_t evc;
    tt_evc_attr_t evc_attr;

    TT_TEST_CASE_ENTER()
    // test start

    __utb_ret = TT_FAIL;
    __utb_err_line = 0;

    tt_evc_attr_default(&evc_attr);
    evc_attr.on_init = __utb_on_init;

    ret = tt_evc_create(&evc, TT_FALSE, &evc_attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_evc_wait(&evc);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    TT_TEST_CHECK_EQUAL(__utb_ret, TT_SUCCESS, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_ipc_t __cli_ipc;
static tt_u32_t __cli_msg_num;

static tt_u32_t __cli_send_num;
static tt_u32_t __cli_recv_num;
static tt_u32_t __cli_to_send_num;
static tt_u32_t __cli_total_send_num;

// - 0: send some, recv some, exit
// - 1: send n1, recv n1, send n2, recv n2, ... exit

#ifdef __SIMULATE_IPC_AIO_FAIL
#define __cli_max_ipc_num 100
#else
#if TT_ENV_OS_IS_WINDOWS
#define __cli_max_ipc_num 1000
#else
#define __cli_max_ipc_num 10000
#endif
#endif

static void __cli_ipc_on_send(IN struct tt_ipc_s *ipc,
                              IN tt_ev_t *ev,
                              IN tt_ipc_aioctx_t *aioctx);

static void __cli_ipc_on_recv(IN struct tt_ipc_s *ipc,
                              IN tt_ipc_aioctx_t *aioctx,
                              IN TO tt_ev_t **ev,
                              IN tt_u32_t ev_num)
{
    tt_u32_t i;

    if (!TT_OK(aioctx->result)) {
        __utb_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    __cli_recv_num += ev_num;
    if (__cli_recv_num % 1000 == 0) {
        TT_DEBUG("recv [%d] msg", __cli_recv_num);
    }

    // we left the final ev, which would be destroyed by ipc aio
    for (i = 0; i < ev_num - 1; ++i) {
        tt_ev_t *c_ev = tt_ev_copy(ev[i]);
        if (c_ev != NULL) {
            if (!__uti_ev_check(c_ev)) {
                __utb_err_line = __LINE__;
                tt_evc_exit(TT_LOCAL_EVC);
                return;
            }

            tt_ev_destroy(c_ev);
        }
    }

    if (__cli_mode == 0) {
        // mode 0: send new event once recving is done
        if (!aioctx->cb_param) {
            tt_u32_t num = tt_rand_u32() % 10 + ev_num;

            while (num-- != 0) {
                tt_ev_t *ev = __uti_ev_gen();
                if (ev == NULL) {
                    __utb_err_line = __LINE__;
                    tt_evc_exit(TT_LOCAL_EVC);
                    return;
                }

                if (tt_ipc_send_async(ipc,
                                      ev,
                                      __cli_ipc_on_send,
                                      (void *)(tt_uintptr_t)1) != TT_SUCCESS) {
                    __utb_err_line = __LINE__;
                    tt_evc_exit(TT_LOCAL_EVC);
                    return;
                }
                // TT_DEBUG("2. sent ipc ev[%d] bytes", ev->data_size);
            }
        } else {
            // ipc messages recved in round 2 must be more than round 1
            if (__cli_recv_num >= __cli_send_num) {
                tt_async_ipc_destroy(ipc, TT_FALSE);
            }
        }
    } else if (__cli_mode == 1) {
        // mode 1: deliver new send when all are received
        if (__cli_recv_num == __cli_to_send_num) {
            tt_u32_t num = tt_rand_u32() % 10 + ev_num;

            __cli_total_send_num += __cli_to_send_num;
            if (__cli_total_send_num % 1000 == 0) {
                TT_DEBUG("this round: %d msg, total: %d msg",
                         __cli_to_send_num,
                         __cli_total_send_num);
            }
            if (__cli_total_send_num >= __cli_max_ipc_num) {
                TT_DEBUG("------------- cli ipc exit");
                tt_evc_exit(TT_LOCAL_EVC);
                return;
            }

            __cli_to_send_num = num;
            __cli_recv_num = 0;
            __cli_send_num = 0;

            while (num-- != 0) {
                tt_ev_t *ev = __uti_ev_gen();
                if (ev == NULL) {
                    __utb_err_line = __LINE__;
                    tt_evc_exit(TT_LOCAL_EVC);
                    return;
                }

                if (tt_ipc_send_async(ipc,
                                      ev,
                                      __cli_ipc_on_send,
                                      (void *)(tt_uintptr_t)1) != TT_SUCCESS) {
                    __utb_err_line = __LINE__;
                    tt_evc_exit(TT_LOCAL_EVC);
                    return;
                }
                // TT_DEBUG("2. sent ipc ev[%d] bytes", ev->data_size);
            }
        } else {
            // else keep receiving
            if (tt_ipc_recv_async(ipc, __cli_ipc_on_recv, aioctx->cb_param) !=
                TT_SUCCESS) {
                __utb_err_line = __LINE__;
                tt_evc_exit(TT_LOCAL_EVC);
                return;
            }
        }
    }
}

static void __cli_ipc_on_send(IN struct tt_ipc_s *ipc,
                              IN tt_ev_t *ev,
                              IN tt_ipc_aioctx_t *aioctx)
{
    if (!TT_OK(aioctx->result)) {
        __utb_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    ++__cli_send_num;
    // TT_DEBUG("send [%d] msg", __cli_send_num);

    if (__cli_mode == 0) {
        // mode 0: deliver a new recv once it reports sending done
        if (tt_ipc_recv_async(ipc, __cli_ipc_on_recv, aioctx->cb_param) !=
            TT_SUCCESS) {
            __utb_err_line = __LINE__;
            tt_evc_exit(TT_LOCAL_EVC);
            return;
        }
    } else if (__cli_mode == 1) {
        // mode 1: deliver a new recv once all are sent
        if (__cli_send_num == __cli_to_send_num) {
            if (tt_ipc_recv_async(ipc, __cli_ipc_on_recv, aioctx->cb_param) !=
                TT_SUCCESS) {
                __utb_err_line = __LINE__;
                tt_evc_exit(TT_LOCAL_EVC);
                return;
            }
        }
    }
}

static void __cli_ipc_on_connect(IN struct tt_ipc_s *ipc,
                                 IN const tt_char_t *remote_addr,
                                 IN tt_ipc_aioctx_t *aioctx)
{
    tt_ev_t *ev;
    tt_u32_t num = tt_rand_u32() % 10 + 1;

    if (!TT_OK(aioctx->result)) {
        __utb_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    __cli_to_send_num = num;
    __cli_recv_num = 0;
    __cli_send_num = 0;

    while (num-- != 0) {
        ev = __uti_ev_gen();
        if (ev == NULL) {
            __utb_err_line = __LINE__;
            tt_evc_exit(TT_LOCAL_EVC);
            return;
        }

        if (tt_ipc_send_async(ipc, ev, __cli_ipc_on_send, NULL) != TT_SUCCESS) {
            __utb_err_line = __LINE__;
            tt_evc_exit(TT_LOCAL_EVC);
            return;
        }
        TT_DEBUG("sent ipc ev[%d] bytes", ev->data_size);
    }
}

static void __utc_on_destroy(IN struct tt_ipc_s *ipc, IN void *cb_param)
{
    // generally it runs as a child thread, so exit it
    __utb_ret = TT_SUCCESS;
    tt_evc_exit(TT_LOCAL_EVC);
}

// static
tt_result_t __utc_on_init(IN struct tt_evcenter_s *evc, IN void *on_init_param)
{
    tt_ipc_t *ipc = &__cli_ipc;
    tt_result_t ret;
    tt_ipc_exit_t de = {__utc_on_destroy, NULL};

    ret = tt_async_ipc_create(ipc, NULL, NULL, &de);
    if (!TT_OK(ret)) {
        __utb_err_line = __LINE__;
        return TT_FAIL;
    }

    ret = tt_ipc_connect_async(ipc,
                               (const tt_char_t *)on_init_param,
                               __cli_ipc_on_connect,
                               NULL);
    if (ret != TT_SUCCESS) {
        __utb_err_line = __LINE__;
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_ipc_client)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    tt_evcenter_t evc;
    tt_evc_attr_t evc_attr;

    TT_TEST_CASE_ENTER()
    // test start

    __utb_ret = TT_FAIL;
    __utb_err_line = 0;

    tt_evc_attr_default(&evc_attr);
    evc_attr.on_init = __utc_on_init;
    evc_attr.on_init_param = __IPC_PATH;

    ret = tt_evc_create(&evc, TT_FALSE, &evc_attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_evc_wait(&evc);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    TT_TEST_CHECK_EQUAL(__utb_ret, TT_SUCCESS, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////

static tt_result_t __st1_on_init(IN struct tt_evcenter_s *evc,
                                 IN void *on_init_param)
{
    tt_ipc_t *ipc = &__utb_lis_ipc;
    tt_result_t ret;
    tt_ipc_exit_t de = {__utb_on_destroy, NULL};

    tt_process_t child_proc;
    tt_char_t *argv[5] = {0};

    ret = tt_async_ipc_create(ipc, __IPC_PATH, NULL, &de);
    if (!TT_OK(ret)) {
        __utb_err_line = __LINE__;
        return TT_FAIL;
    }

    ret = tt_ipc_accept_async(ipc,
                              &__utb_acc_ipc,
                              NULL,
                              &de,
                              __utb_on_accept,
                              NULL);
    if (ret != TT_SUCCESS) {
        __utb_err_line = __LINE__;
        return TT_FAIL;
    }

#if 1
    argv[0] = "tsk_unit_test";
    argv[1] = "ipc_stress";
    argv[2] = __IPC_PATH;
    ret = tt_process_create(&child_proc, __app_file, argv, NULL);
    if (ret != TT_SUCCESS) {
        __utb_err_line = __LINE__;
        return TT_FAIL;
    }
#endif

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_ipc_stress)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    tt_evcenter_t evc;
    tt_evc_attr_t evc_attr;

    TT_TEST_CASE_ENTER()
    // test start

    __utb_ret = TT_FAIL;
    __utb_err_line = 0;

    tt_evc_attr_default(&evc_attr);
    evc_attr.on_init = __st1_on_init;

    ret = tt_evc_create(&evc, TT_FALSE, &evc_attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_evc_wait(&evc);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    TT_TEST_CHECK_EQUAL(__utb_ret, TT_SUCCESS, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////

static tt_ipc_t __mul_acc[__cli_num];

static tt_result_t __stm_on_init(IN struct tt_evcenter_s *evc,
                                 IN void *on_init_param)
{
    tt_ipc_t *ipc = &__utb_lis_ipc;
    tt_result_t ret;
    tt_ipc_exit_t de = {__utb_on_destroy, NULL};
    tt_u32_t i;

    tt_process_t child_proc;
    tt_char_t *argv[5] = {0};

    ret = tt_async_ipc_create(ipc, __IPC_PATH, NULL, &de);
    if (!TT_OK(ret)) {
        __utb_err_line = __LINE__;
        return TT_FAIL;
    }

    for (i = 0; i < __cli_num; ++i) {
        ret = tt_ipc_accept_async(ipc,
                                  &__mul_acc[i],
                                  NULL,
                                  &de,
                                  __utb_on_accept,
                                  NULL);
        if (ret != TT_SUCCESS) {
            __utb_err_line = __LINE__;
            return TT_FAIL;
        }
    }

    for (i = 0; i < __cli_num; ++i) {
        argv[0] = "tsk_unit_test";
        argv[1] = "ipc_stress";
        argv[2] = __IPC_PATH;
        ret = tt_process_create(&child_proc, __app_file, argv, NULL);
        if (ret != TT_SUCCESS) {
            __utb_err_line = __LINE__;
            return TT_FAIL;
        }
    }

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_ipc_stress_mul)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    tt_evcenter_t evc;
    tt_evc_attr_t evc_attr;

    TT_TEST_CASE_ENTER()
    // test start

    __utb_ret = TT_FAIL;
    __utb_err_line = 0;

    __cli_mode = 1;

    tt_evc_attr_default(&evc_attr);
    evc_attr.on_init = __stm_on_init;

    ret = tt_evc_create(&evc, TT_FALSE, &evc_attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_evc_wait(&evc);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    TT_TEST_CHECK_EQUAL(__utb_ret, TT_SUCCESS, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_u32_t __di_cnt = 0;

static void __di_on_accept(IN struct tt_ipc_s *listening_ipc,
                           IN struct tt_ipc_s *new_ipc,
                           IN tt_ipc_aioctx_t *aioctx)
{
    if (TT_OK(aioctx->result)) {
        __utb_err_line = __LINE__;
        return;
    }

    if (aioctx->result != TT_CANCELLED) {
        __utb_err_line = __LINE__;
        return;
    }
}

static void __di_on_destroy(IN struct tt_ipc_s *ipc, IN void *cb_param)
{
    if (cb_param != (void *)(tt_uintptr_t)0x12345678) {
        __utb_err_line = __LINE__;
        return;
    }

    --__di_cnt;
    TT_INFO("%d ipc left", __di_cnt);
    if (__di_cnt == 0) {
        __utb_ret = TT_SUCCESS;
        tt_evc_exit(TT_LOCAL_EVC);
    }
}

static void __di_on_destroy2(IN struct tt_ipc_s *ipc, IN void *cb_param)
{
    if (cb_param != (void *)(tt_uintptr_t)0x87654321) {
        __utb_err_line = __LINE__;
        return;
    }

    --__di_cnt;
    TT_INFO("%d ipc left", __di_cnt);
    if (__di_cnt == 0) {
        __utb_ret = TT_SUCCESS;
        tt_evc_exit(TT_LOCAL_EVC);
    }
}

static void __di_on_destroy3(IN struct tt_ipc_s *ipc, IN void *cb_param)
{
    if (cb_param != (void *)(tt_uintptr_t)0x12345678) {
        __utb_err_line = __LINE__;
        return;
    }

    --__di_cnt;
    TT_INFO("%d ipc left", __di_cnt);
    if (__di_cnt == 0) {
        __utb_ret = TT_SUCCESS;
        tt_evc_exit(TT_LOCAL_EVC);
    }
}

static tt_result_t __di_on_init_svr(IN struct tt_evcenter_s *evc,
                                    IN void *on_init_param)
{
    tt_ipc_t *ipc = &__utb_lis_ipc;
    tt_result_t ret;
    tt_ipc_exit_t de = {__di_on_destroy3, (void *)(tt_uintptr_t)0x12345678};
    tt_u32_t i;

    ret = tt_async_ipc_create(ipc, __IPC_PATH, NULL, &de);
    if (!TT_OK(ret)) {
        __utb_err_line = __LINE__;
        return TT_FAIL;
    }

    for (i = 0; i < __cli_num; ++i) {
        ret = tt_ipc_accept_async(ipc,
                                  &__mul_acc[i],
                                  NULL,
                                  &de,
                                  __di_on_accept,
                                  NULL);
        if (ret != TT_SUCCESS) {
            __utb_err_line = __LINE__;
            return TT_FAIL;
        }
    }

    __di_cnt = 1;

#if TT_ENV_OS_IS_WINDOWS
    __di_cnt += __cli_num;
    for (i = 0; i < __cli_num; ++i) {
        tt_async_ipc_destroy(&__mul_acc[i], TT_TRUE);
    }
#endif

    tt_async_ipc_destroy(ipc, TT_TRUE);

    return TT_SUCCESS;
}

static void __di_on_connect(IN struct tt_ipc_s *ipc,
                            IN const tt_char_t *remote_addr,
                            IN tt_ipc_aioctx_t *aioctx)
{
    if (TT_OK(aioctx->result)) {
        __utb_err_line = __LINE__;
        return;
    }

    if (aioctx->result != TT_CANCELLED) {
        __utb_err_line = __LINE__;
        return;
    }
}

static tt_result_t __di_on_init_cli(IN struct tt_evcenter_s *evc,
                                    IN void *on_init_param)
{
    tt_ipc_t *ipc = &__cli_ipc;
    tt_result_t ret;
    tt_ipc_exit_t de = {__di_on_destroy, (void *)(tt_uintptr_t)0x12345678};
    tt_ipc_exit_t de2 = {__di_on_destroy2, (void *)(tt_uintptr_t)0x87654321};

    // create a server
    ret = tt_async_ipc_create(&__utb_lis_ipc, __IPC_PATH, NULL, &de2);
    if (!TT_OK(ret)) {
        __utb_err_line = __LINE__;
        return TT_FAIL;
    }

    ret = tt_ipc_accept_async(&__utb_lis_ipc,
                              &__utb_acc_ipc,
                              NULL,
                              &de,
                              __di_on_accept,
                              NULL);
    if (ret != TT_SUCCESS) {
        __utb_err_line = __LINE__;
        return TT_FAIL;
    }

    // client connect
    ret = tt_async_ipc_create(ipc, NULL, NULL, &de);
    if (!TT_OK(ret)) {
        __utb_err_line = __LINE__;
        return TT_FAIL;
    }

    // wrong address
    tt_ipc_connect_async(ipc,
                         (const tt_char_t *)"wrong name",
                         __di_on_connect,
                         NULL);

    tt_ipc_connect_async(ipc, __IPC_PATH, __di_on_connect, NULL);

    // would be reject
    ret = tt_ipc_connect_async(ipc, __IPC_PATH, __di_on_connect, NULL);
    if (ret == TT_SUCCESS) {
        __utb_err_line = __LINE__;
        return TT_FAIL;
    }

    __di_cnt = 2;

#if TT_ENV_OS_IS_WINDOWS
    __di_cnt += 1;
    tt_async_ipc_destroy(&__utb_acc_ipc, TT_TRUE);
#endif

    tt_async_ipc_destroy(&__utb_lis_ipc, TT_TRUE);
    tt_async_ipc_destroy(ipc, TT_TRUE);

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_ipc_destroy_immd)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    tt_evcenter_t evc;
    tt_evc_attr_t evc_attr;

    TT_TEST_CASE_ENTER()
    // test start

    // test server
    __utb_ret = TT_FAIL;
    __utb_err_line = 0;
    __di_cnt = 0;

    tt_evc_attr_default(&evc_attr);
    evc_attr.on_init = __di_on_init_svr;

    ret = tt_evc_create(&evc, TT_FALSE, &evc_attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_evc_wait(&evc);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    TT_TEST_CHECK_EQUAL(__utb_ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(__utb_err_line, 0, "");

    // test client
    __utb_ret = TT_FAIL;
    __utb_err_line = 0;
    __di_cnt = 0;

    tt_evc_attr_default(&evc_attr);
    evc_attr.on_init = __di_on_init_cli;

    ret = tt_evc_create(&evc, TT_FALSE, &evc_attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_evc_wait(&evc);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    TT_TEST_CHECK_EQUAL(__utb_ret, TT_SUCCESS, "");
    // TT_TEST_CHECK_EQUAL(__utb_ret2, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(__utb_err_line, 0, "");

    // test end
    TT_TEST_CASE_LEAVE()
}
