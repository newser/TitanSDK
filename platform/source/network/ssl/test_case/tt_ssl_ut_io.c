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

// private keys
#define ____IMPORT_KEY_PKCS8
#include "tt_ssl_ut_privkey_pkcs8.h"

// x509
#define ____IMPORT_CERT_X509
#include "tt_ssl_ut_cert_x509.h"
// PKCS7
#define ____IMPORT_CERT_PKCS7
#include "tt_ssl_ut_cert_pkcs7.h"
// PKCS12
#define ____IMPORT_CERT_PKCS12
#include "tt_ssl_ut_cert_pkcs12.h"

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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_ssl_client)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_ssl_basic)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_ssl_destroy_immd)

TT_TEST_ROUTINE_DECLARE(tt_unit_test_ssl_svr_auth)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_ssl_svr_auth_fail)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_ssl_svr_auth_fail_cn)

TT_TEST_ROUTINE_DECLARE(tt_unit_test_ssl_cli_auth)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_ssl_cli_auth_fail)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_ssl_cli_auth_fail_cn)

TT_TEST_ROUTINE_DECLARE(tt_unit_test_ssl_stress_1_evc)

// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(ssl_case)

#if 0
    TT_TEST_CASE("tt_unit_test_ssl_client", 
                 "testing ssl client api", 
                 tt_unit_test_ssl_client, NULL, 
                 NULL, NULL,
                 NULL, NULL),
#endif

#if 1
TT_TEST_CASE("tt_unit_test_ssl_basic",
             "testing ssl basic: client&server",
             tt_unit_test_ssl_basic,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
, TT_TEST_CASE("tt_unit_test_ssl_destroy_immd",
               "testing ssl destroy immediately",
               tt_unit_test_ssl_destroy_immd,
               NULL,
               NULL,
               NULL,
               NULL,
               NULL),
#endif

#if 0
    TT_TEST_CASE("tt_unit_test_ssl_svr_auth",
                 "testing ssl server auth",
                 tt_unit_test_ssl_svr_auth,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_ssl_svr_auth_fail",
                 "testing ssl server auth: invalid svr cert",
                 tt_unit_test_ssl_svr_auth_fail,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_ut_ssl_svr_auth_fail_cn",
                 "testing ssl server auth: invalid common name",
                 tt_unit_test_ssl_svr_auth_fail_cn,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),
#endif

#if 0
    TT_TEST_CASE("tt_unit_test_ssl_cli_auth",
                 "testing ssl client auth",
                 tt_unit_test_ssl_cli_auth,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_ssl_cli_auth_fail",
                 "testing ssl client auth: invalid svr cert",
                 tt_unit_test_ssl_cli_auth_fail,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_ut_ssl_cli_auth_fail_cn",
                 "testing ssl client auth: invalid common name",
                 tt_unit_test_ssl_cli_auth_fail_cn,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),
#endif

#if 0
/*
 stress test on all platforms:
 - [mac os x] by xcode instrument tool: no cf object leak, no socket fd
   leak, but memory(RSS) still increase slowly, i doubt it's due to secure
   transport sesson cache, but the memory does not fall back even after
   serveral hours. it consumes 500 bytes more for each ssl sessions
 - [linux] by "ps -o rss", the meory does not change, but valgrind reports
   lots of memory leak issues
 - [windows] by windbg, we see the heap size does change when __ST1_SSL_NUM
   increases. but whether there is resource leak when calling tt_sslctx_t
   APIs is not verified
 */
TT_TEST_CASE("tt_unit_test_ssl_stress_1_evc",
             "testing ssl stress test",
             tt_unit_test_ssl_stress_1_evc,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,
#endif

    TT_TEST_CASE_LIST_DEFINE_END(ssl_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(SSL_UT_IO, 0, ssl_case)

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

    static tt_ssl_t __c1_ssl;
static tt_skt_t __c1_skt;
static tt_result_t __c1_ret;
static tt_sslctx_t __c1_ctx;
static tt_u32_t __c1_err_line;

static tt_char_t __c1_msg[] =
    "GET / HTTP/1.1\r\n"
    "Host: www.163.com\r\n"
    "Connection: keep-alive\r\n"
    "Accept: "
    "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/"
    "*;q=0.8\r\n"
    "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_9_5) "
    "AppleWebKit/537.36 (KHTML, like Gecko) Chrome/40.0.2214.111 "
    "Safari/537.36\r\n"
    "Accept-Encoding: gzip, deflate, sdch\r\n"
    "Accept-Language: en-US,en;q=0.8,zh-CN;q=0.6,zh;q=0.4\r\n\r\n";
static tt_u8_t __c1_recv_buf[100];

void __c1_skt_on_destroy(IN tt_skt_t *skt, IN void *cb_param)
{
    // would be overwritten
    TT_ASSERT(0);
}

void __c1_ssl_on_destroy(IN struct tt_ssl_s *ssl, IN void *cb_param)
{
    __c1_ret = TT_SUCCESS;
    tt_evc_exit(TT_LOCAL_EVC);
}

void __c1_ssl_on_send(IN struct tt_ssl_s *ssl,
                      IN tt_blob_t *buf_array,
                      IN tt_u32_t buf_num,
                      IN tt_ssl_aioctx_t *aioctx,
                      IN tt_u32_t send_len)
{
    if (!TT_OK(aioctx->result)) {
        __c1_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    if (send_len != sizeof(__c1_msg) - 1) {
        __c1_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }
}

void __c1_ssl_on_recv(IN struct tt_ssl_s *ssl,
                      IN tt_blob_t *buf_array,
                      IN tt_u32_t buf_num,
                      IN tt_ssl_aioctx_t *aioctx,
                      IN tt_u32_t recv_len)
{
    tt_blob_t buf;
    tt_result_t ret;

    if (!TT_OK(aioctx->result)) {
        if (aioctx->result != TT_END) {
            __c1_err_line = __LINE__;
            tt_evc_exit(TT_LOCAL_EVC);
            return;
        }

        // wait for destroy
        return;
    }

    TT_INFO("ssl recv: \n%s", buf_array[0].addr);
    tt_memset(buf_array[0].addr, 0, buf_array[0].len);

    buf.addr = __c1_recv_buf;
    buf.len = sizeof(__c1_recv_buf) - 1;
    ret = tt_ssl_recv_async(ssl, &buf, 1, __c1_ssl_on_recv, NULL);
    if (ret != TT_SUCCESS) {
        __c1_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }
}

void __c1_ssl_on_connect(IN struct tt_ssl_s *ssl,
                         IN tt_sktaddr_t *remote_addr,
                         IN tt_ssl_aioctx_t *aioctx)
{
    tt_blob_t buf;
    tt_result_t ret;

    if (!TT_OK(aioctx->result)) {
        __c1_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    buf.addr = (tt_u8_t *)__c1_msg;
    buf.len = sizeof(__c1_msg) - 1;
    ret = tt_ssl_send_async(ssl, &buf, 1, __c1_ssl_on_send, NULL);
    if (ret != TT_SUCCESS) {
        __c1_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    ret = tt_async_ssl_shutdown(ssl, TT_SSL_SHUTDOWN_WR);
    if (ret != TT_SUCCESS) {
        __c1_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    buf.addr = __c1_recv_buf;
    buf.len = sizeof(__c1_recv_buf) - 1;
    ret = tt_ssl_recv_async(ssl, &buf, 1, __c1_ssl_on_recv, NULL);
    if (ret != TT_SUCCESS) {
        __c1_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }
}

tt_result_t __c1_on_init(IN struct tt_evcenter_s *evc, IN void *on_init_param)
{
    tt_skt_exit_t skt_de = {__c1_skt_on_destroy};
    tt_result_t ret;
    tt_ssl_exit_t ssl_de = {__c1_ssl_on_destroy};
    tt_sktaddr_t remote_addr;

    ret = tt_async_skt_create(&__c1_skt,
                              TT_NET_AF_INET,
                              TT_NET_PROTO_TCP,
                              TT_SKT_ROLE_TCP_CONNECT,
                              NULL,
                              &skt_de);
    if (!TT_OK(ret))
        return TT_FAIL;

    ret = tt_sslctx_create(&__c1_ctx, TT_SSL_ROLE_CLIENT, NULL);
    if (!TT_OK(ret))
        return TT_FAIL;

    ret = tt_sslctx_commit(&__c1_ctx, NULL, 0);
    if (!TT_OK(ret))
        return TT_FAIL;

    ret = tt_async_ssl_create(&__c1_ssl, &__c1_skt, &__c1_ctx, NULL, &ssl_de);
    if (!TT_OK(ret))
        return TT_FAIL;

#define __c1_svr_addr "123.58.180.8"
#define __c1_svr_port 443

    tt_sktaddr_init(&remote_addr, TT_NET_AF_INET);
    tt_sktaddr_set_ip_p(&remote_addr, __c1_svr_addr);
    tt_sktaddr_set_port(&remote_addr, __c1_svr_port);

    ret = tt_ssl_connect_async(&__c1_ssl,
                               &remote_addr,
                               __c1_ssl_on_connect,
                               NULL);
    if (ret != TT_SUCCESS)
        return TT_FAIL;

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_ssl_client)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_evcenter_t evc;
    tt_evc_attr_t attr;
    tt_result_t ret;

    TT_TEST_CASE_ENTER()
    // test start

    __c1_ret = TT_FAIL;

    tt_evc_attr_default(&attr);
    attr.on_init = __c1_on_init;

    ret = tt_evc_create(&evc, TT_FALSE, &attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_evc_wait(&evc);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    TT_TEST_CHECK_EQUAL(__c1_ret, TT_SUCCESS, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

typedef struct
{
    const tt_char_t *svr_addr;
    tt_u32_t svr_port;

    // server
    tt_ssl_cert_format_t svr_cert_fmt;
    tt_blob_t *svr_cert;
    tt_ssl_cert_attr_t *svr_cert_attr;
    tt_ssl_privkey_format_t svr_privkey_fmt;
    tt_blob_t *svr_privkey;
    tt_ssl_privkey_attr_t *svr_privkey_attr;

    tt_ssl_cert_format_t svr_ca_fmt;
    tt_blob_t *svr_ca;
    tt_ssl_cert_attr_t *svr_ca_attr;

    tt_ssl_verify_t *svr_verify;

    // client
    tt_ssl_cert_format_t cli_cert_fmt;
    tt_blob_t *cli_cert;
    tt_ssl_cert_attr_t *cli_cert_attr;
    tt_ssl_privkey_format_t cli_privkey_fmt;
    tt_blob_t *cli_privkey;
    tt_ssl_privkey_attr_t *cli_privkey_attr;

    tt_ssl_cert_format_t cli_ca_fmt;
    tt_blob_t *cli_ca;
    tt_ssl_cert_attr_t *cli_ca_attr;

    tt_ssl_verify_t *cli_verify;

    // callback
    tt_ssl_on_accept_t on_accept;
    tt_ssl_on_connect_t on_connect;
} __ssl_ut_param_t;

static tt_skt_t __sut_con_skt;
static tt_skt_t __sut_acc_skt;
static tt_skt_t __sut_lis_skt;
static tt_sktaddr_t svr_addr;

static tt_sslctx_t __sut_cli_sc;
static tt_sslctx_t __sut_svr_sc;

static tt_ssl_t __sut_con_ssl;
static tt_ssl_t __sut_acc_ssl;
static tt_ssl_t __sut_lis_ssl;

static tt_u32_t __sut_err_line;
static tt_u32_t __sut_ret;

static tt_u32_t __sut_flag;
#define __sut_con_destroyed (1 << 0)
#define __sut_acc_destroyed (1 << 1)
#define __sut_lis_destroyed (1 << 2)

#define __sut_con_created (1 << 3)
#define __sut_acc_created (1 << 4)
#define __sut_lis_created (1 << 5)

#define __sut_con_ssl_created (1 << 6)
#define __sut_acc_ssl_created (1 << 7)
#define __sut_lis_ssl_created (1 << 8)

#define __sut_data_num 10
#define __sut_data_len 100
static tt_char_t __acc_send[__sut_data_num][__sut_data_len + 1];
static tt_char_t __con_recv[__sut_data_len * 2 + 1];
static tt_u32_t __con_recv_num;

static tt_char_t __con_send[__sut_data_num][__sut_data_len + 1];
static tt_char_t __acc_recv[__sut_data_len * 2 + 1];
static tt_u32_t __acc_recv_num;

static __ssl_ut_param_t __sut_param;

#define __SUT_SHOW_IO_DATA

void __sut_skt_on_destroy(IN tt_skt_t *skt, IN void *cb_param)
{
    // would be overwritten
    TT_ASSERT(0);
}

void __sut_con_on_destroy(IN struct tt_ssl_s *ssl, IN void *cb_param)
{
    __sut_flag |= __sut_con_destroyed;
    if (__sut_flag & __sut_acc_destroyed) {
        __sut_ret = TT_SUCCESS;
        tt_evc_exit(TT_LOCAL_EVC);
    }
}

void __sut_acc_on_destroy(IN struct tt_ssl_s *ssl, IN void *cb_param)
{
    __sut_flag |= __sut_acc_destroyed;
    if (__sut_flag & __sut_con_destroyed) {
        __sut_ret = TT_SUCCESS;
        tt_evc_exit(TT_LOCAL_EVC);
    }
}

void __sut_lis_on_destroy(IN struct tt_ssl_s *ssl, IN void *cb_param)
{
    __sut_flag |= __sut_lis_destroyed;
}

void __sut_ssl_on_recv(IN struct tt_ssl_s *ssl,
                       IN tt_blob_t *buf_array,
                       IN tt_u32_t buf_num,
                       IN tt_ssl_aioctx_t *aioctx,
                       IN tt_u32_t recv_len)
{
    tt_result_t ret;
    tt_u8_t *data = buf_array[0].addr;
    tt_u32_t data_len = recv_len;
    tt_bool_t acc = TT_FALSE;

    if (!TT_OK(aioctx->result)) {
        if (aioctx->result == TT_END) {
            tt_async_ssl_shutdown(ssl, TT_SSL_SHUTDOWN_RDWR);
            return;
        }

        __sut_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    buf_array[0].addr[recv_len] = 0;

    if (ssl == &__sut_acc_ssl) {
        acc = TT_TRUE;
        (void)acc;

        __acc_recv_num += recv_len;
#ifdef __SUT_SHOW_IO_DATA
        TT_INFO("acc ssl recv[%d][->%d]: %s",
                recv_len,
                __acc_recv_num,
                (tt_char_t *)buf_array[0].addr);
#endif
        TT_ASSERT(__acc_recv_num <= __sut_data_num * __sut_data_len);
        if (__acc_recv_num == __sut_data_num * __sut_data_len &&
            (__con_recv_num == __sut_data_num * __sut_data_len)) {
#if 1
            /*if (tt_async_ssl_shutdown(&__sut_con_ssl, TT_SSL_SHUTDOWN_WR) !=
            TT_SUCCESS) {
                __sut_err_line = __LINE__;
                tt_evc_exit(TT_LOCAL_EVC);
                return;
            }*/
            if (tt_async_ssl_shutdown(&__sut_acc_ssl, TT_SSL_SHUTDOWN_WR) !=
                TT_SUCCESS) {
                __sut_err_line = __LINE__;
                tt_evc_exit(TT_LOCAL_EVC);
                return;
            }
#endif
        }
    } else {
        TT_ASSERT(ssl == &__sut_con_ssl);

        __con_recv_num += recv_len;
#ifdef __SUT_SHOW_IO_DATA
        TT_INFO("con ssl recv[%d][->%d]: %s",
                recv_len,
                __con_recv_num,
                (tt_char_t *)buf_array[0].addr);
#endif
        TT_ASSERT(__con_recv_num <= __sut_data_num * __sut_data_len);
        if (__con_recv_num == __sut_data_num * __sut_data_len &&
            __acc_recv_num == __sut_data_num * __sut_data_len) {
#if 1
            if (tt_async_ssl_shutdown(&__sut_con_ssl, TT_SSL_SHUTDOWN_WR) !=
                TT_SUCCESS) {
                __sut_err_line = __LINE__;
                tt_evc_exit(TT_LOCAL_EVC);
                return;
            }
/*if (tt_async_ssl_shutdown(&__sut_acc_ssl, TT_SSL_SHUTDOWN_WR) != TT_SUCCESS) {
    __sut_err_line = __LINE__;
    tt_evc_exit(TT_LOCAL_EVC);
    return;
}*/
#endif
        }
    }

    // otherwise, keep receiving
    data_len = tt_rand_u32() % (__sut_data_len) + 1;
    buf_array[0].len = data_len;
#ifdef __SUT_SHOW_IO_DATA
    if (acc)
        TT_INFO("acc ssl to recv[%d]", data_len);
    else
        TT_INFO("con ssl to recv[%d]", data_len);
#endif
    ret = tt_ssl_recv_async(ssl, buf_array, buf_num, __sut_ssl_on_recv, NULL);
    if (ret != TT_SUCCESS) {
        __sut_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }
}

void __sut_ssl_on_send(IN struct tt_ssl_s *ssl,
                       IN tt_blob_t *buf_array,
                       IN tt_u32_t buf_num,
                       IN tt_ssl_aioctx_t *aioctx,
                       IN tt_u32_t send_len)
{
    if (!TT_OK(aioctx->result)) {
        __sut_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }
}

void __sut_on_accept(IN struct tt_ssl_s *listening_ssl,
                     IN struct tt_ssl_s *new_ssl,
                     IN tt_ssl_aioctx_t *aioctx)
{
    tt_blob_t buf;
    tt_result_t ret;
    tt_u32_t i;

    if (!TT_OK(aioctx->result)) {
        __sut_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    __sut_flag |= __sut_acc_created;
    __sut_flag |= __sut_acc_ssl_created;

    for (i = 0; i < __sut_data_num; ++i) {
        tt_u8_t *data = (tt_u8_t *)__acc_send[i];
        tt_u32_t data_len = __sut_data_len;

        tt_memset(data, 'a' + i, data_len);
        buf.addr = data;
        buf.len = data_len;

        ret = tt_ssl_send_async(new_ssl, &buf, 1, __sut_ssl_on_send, NULL);

        if (ret != TT_SUCCESS) {
            __sut_err_line = __LINE__;
            tt_evc_exit(TT_LOCAL_EVC);
            return;
        }
    }
#if 0 
    // on some platform, it auto respond a close notify immediately after receiving
    // a close notify rather than sending all data out first
    if (tt_async_ssl_shutdown(new_ssl, TT_SSL_SHUTDOWN_WR) != TT_SUCCESS) {
        __sut_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }
#endif

    buf.addr = (tt_u8_t *)__acc_recv;
    buf.len = (tt_u32_t)__sut_data_len * 2;
    ret = tt_ssl_recv_async(new_ssl, &buf, 1, __sut_ssl_on_recv, NULL);
    if (ret != TT_SUCCESS) {
        __sut_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }
}

void __sut_ssl_on_connect(IN struct tt_ssl_s *ssl,
                          IN tt_sktaddr_t *remote_addr,
                          IN tt_ssl_aioctx_t *aioctx)
{
    tt_blob_t buf;
    tt_result_t ret;
    tt_u32_t i;

    if (!TT_OK(aioctx->result)) {
        __sut_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    for (i = 0; i < __sut_data_num; ++i) {
        tt_u8_t *data = (tt_u8_t *)__con_send[i];
        tt_u32_t data_len = __sut_data_len;

        tt_memset(data, 'A' + i, data_len);
        buf.addr = data;
        buf.len = data_len;

        ret = tt_ssl_send_async(ssl, &buf, 1, __sut_ssl_on_send, NULL);

        if (ret != TT_SUCCESS) {
            __sut_err_line = __LINE__;
            tt_evc_exit(TT_LOCAL_EVC);
            return;
        }
    }
#if 0
    if (tt_async_ssl_shutdown(ssl, TT_SSL_SHUTDOWN_WR) != TT_SUCCESS) {
        __sut_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }
#endif

    buf.addr = (tt_u8_t *)__con_recv;
    buf.len = (tt_u32_t)__sut_data_len * 2;
    ret = tt_ssl_recv_async(ssl, &buf, 1, __sut_ssl_on_recv, NULL);
    if (ret != TT_SUCCESS) {
        __sut_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }
}

tt_result_t __ssl_ut_on_init(IN struct tt_evcenter_s *evc,
                             IN void *on_init_param)
{
    tt_skt_exit_t skt_de = {__sut_skt_on_destroy};
    tt_result_t ret;
    tt_ssl_exit_t ssl_de = {0};
    tt_sktaddr_t svr_addr;
    __ssl_ut_param_t *utp = (__ssl_ut_param_t *)&__sut_param;
    tt_skt_attr_t skt_attr;

    // client: skt
    ret = tt_async_skt_create(&__sut_con_skt,
                              TT_NET_AF_INET,
                              TT_NET_PROTO_TCP,
                              TT_SKT_ROLE_TCP_CONNECT,
                              NULL,
                              &skt_de);
    if (!TT_OK(ret))
        return TT_FAIL;
    __sut_flag |= __sut_con_created;

    // client: ssl context
    ret = tt_sslctx_create(&__sut_cli_sc, TT_SSL_ROLE_CLIENT, NULL);
    if (!TT_OK(ret))
        return TT_FAIL;

    if (utp->cli_cert != NULL) {
        ret = tt_sslctx_add_cert(&__sut_cli_sc,
                                 utp->cli_cert_fmt,
                                 utp->cli_cert,
                                 utp->cli_cert_attr,
                                 utp->cli_privkey_fmt,
                                 utp->cli_privkey,
                                 utp->cli_privkey_attr);
        if (!TT_OK(ret))
            return TT_FAIL;
    }

    if (utp->cli_ca != NULL) {
        ret = tt_sslctx_add_ca(&__sut_cli_sc,
                               utp->cli_ca_fmt,
                               utp->cli_ca,
                               utp->cli_ca_attr);
        if (!TT_OK(ret))
            return TT_FAIL;
    }

    if (utp->cli_verify != NULL) {
        tt_sslctx_set_verify(&__sut_cli_sc, utp->cli_verify);
    }

    ret = tt_sslctx_commit(&__sut_cli_sc, NULL, 0);
    if (!TT_OK(ret))
        return TT_FAIL;

    // client: ssl
    ssl_de.on_destroy = __sut_con_on_destroy;
    ret = tt_async_ssl_create(&__sut_con_ssl,
                              &__sut_con_skt,
                              &__sut_cli_sc,
                              NULL,
                              &ssl_de);
    if (!TT_OK(ret))
        return TT_FAIL;
    __sut_flag |= __sut_con_ssl_created;

    // addr
    tt_sktaddr_init(&svr_addr, TT_NET_AF_INET);
    tt_sktaddr_set_ip_p(&svr_addr, (tt_char_t *)utp->svr_addr);
    tt_sktaddr_set_port(&svr_addr, utp->svr_port);

    // listen: skt
    tt_skt_attr_default(&skt_attr);

    tt_skt_attr_set_reuseaddr(&skt_attr, TT_TRUE);

    ret = tt_tcp_server_async(&__sut_lis_skt,
                              TT_NET_AF_INET,
                              &skt_attr,
                              &svr_addr,
                              TT_SKT_BACKLOG_DEFAULT,
                              &skt_de);
    if (!TT_OK(ret))
        return TT_FAIL;
    __sut_flag |= __sut_lis_created;

    // server: ssl context
    ret = tt_sslctx_create(&__sut_svr_sc, TT_SSL_ROLE_SERVER, NULL);
    if (!TT_OK(ret))
        return TT_FAIL;

    ret = tt_sslctx_add_cert(&__sut_svr_sc,
                             __sut_param.svr_cert_fmt,
                             __sut_param.svr_cert,
                             __sut_param.svr_cert_attr,
                             __sut_param.svr_privkey_fmt,
                             __sut_param.svr_privkey,
                             __sut_param.svr_privkey_attr);
    if (!TT_OK(ret))
        return TT_FAIL;

    if (utp->svr_ca != NULL) {
        ret = tt_sslctx_add_ca(&__sut_svr_sc,
                               utp->svr_ca_fmt,
                               utp->svr_ca,
                               utp->svr_ca_attr);
        if (!TT_OK(ret))
            return TT_FAIL;
    }

    if (utp->svr_verify != NULL) {
        tt_sslctx_set_verify(&__sut_svr_sc, utp->svr_verify);
    }

    ret = tt_sslctx_commit(&__sut_svr_sc, NULL, 0);
    if (!TT_OK(ret))
        return TT_FAIL;

    // server: ssl
    ssl_de.on_destroy = __sut_lis_on_destroy;
    ret = tt_async_ssl_create(&__sut_lis_ssl,
                              &__sut_lis_skt,
                              &__sut_svr_sc,
                              NULL,
                              &ssl_de);
    if (!TT_OK(ret))
        return TT_FAIL;
    __sut_flag |= __sut_lis_ssl_created;

    // server: accept
    ssl_de.on_destroy = __sut_acc_on_destroy;
    ret = tt_ssl_accept_async(&__sut_lis_ssl,
                              &__sut_acc_ssl,
                              &__sut_acc_skt,
                              NULL,
                              NULL,
                              &ssl_de,
                              __sut_param.on_accept != NULL
                                  ? __sut_param.on_accept
                                  : __sut_on_accept,
                              NULL);
    if (!TT_OK(ret))
        return TT_FAIL;

    // wait for server then connect??
    tt_sleep(500);

    // start connect
    ret = tt_ssl_connect_async(&__sut_con_ssl,
                               &svr_addr,
                               __sut_param.on_connect != NULL
                                   ? __sut_param.on_connect
                                   : __sut_ssl_on_connect,
                               NULL);
    if (!TT_OK(ret))
        return TT_FAIL;

    return TT_SUCCESS;
}

tt_result_t __ssl_ut()
{
    tt_evcenter_t evc;
    tt_evc_attr_t attr;
    tt_result_t ret;

    __sut_err_line = 0;
    __sut_ret = TT_FAIL;
    __sut_flag = 0;
    __acc_recv_num = 0;
    __con_recv_num = 0;

    tt_evc_attr_default(&attr);
    attr.on_init = __ssl_ut_on_init;

    ret = tt_evc_create(&evc, TT_FALSE, &attr);
    if (!TT_OK(ret))
        return ret;

    ret = tt_evc_wait(&evc);
    if (!TT_OK(ret))
        return ret;

    if (!(__sut_flag & __sut_con_destroyed)) {
        if (__sut_flag & __sut_con_ssl_created) {
            tt_async_ssl_destroy(&__sut_con_ssl, TT_TRUE);
        }
    }

    if (!(__sut_flag & __sut_acc_destroyed)) {
        if (__sut_flag & __sut_acc_ssl_created) {
            tt_async_ssl_destroy(&__sut_acc_ssl, TT_TRUE);
        }
    }

    if (__sut_flag & __sut_lis_ssl_created) {
        tt_async_ssl_destroy(&__sut_lis_ssl, TT_TRUE);
    }

    tt_sslctx_destroy(&__sut_cli_sc);
    tt_sslctx_destroy(&__sut_svr_sc);

    return TT_SUCCESS;
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

#define __SUT_LOCAL_ADDR "127.0.0.1";
//#define __SUT_LOCAL_ADDR "192.168.10.110");

TT_TEST_ROUTINE_DEFINE(tt_unit_test_ssl_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_blob_t svr_cert;
    tt_ssl_cert_attr_t cert_attr;
    tt_result_t ret;
    tt_bool_t enabled;

    TT_TEST_CASE_ENTER()
    // test start

    enabled = tt_ssl_enabled();
#ifdef TT_PLATFORM_SSL_ENABLE
    TT_TEST_CHECK_EQUAL(enabled, TT_TRUE, "");
#else
    // actually it never reach here
    TT_TEST_CHECK_EQUAL(enabled, TT_FALSE, "");
#endif

    tt_memset(&__sut_param, 0, sizeof(__sut_param));

    __sut_param.svr_addr = __SUT_LOCAL_ADDR;
    __sut_param.svr_port = 20020;

    __sut_param.svr_cert_fmt = TT_SSL_CERT_FMT_PKCS12;

    svr_cert.addr = __s_cert_111_11_1_p12_enc;
    svr_cert.len = sizeof(__s_cert_111_11_1_p12_enc);
    __sut_param.svr_cert = &svr_cert;

    cert_attr.encrypted = TT_TRUE;
    cert_attr.pem_armor = TT_FALSE;
    cert_attr.password = "abc123";
    __sut_param.svr_cert_attr = &cert_attr;

    __sut_param.svr_privkey_fmt = TT_SSL_PRIVKEY_FMT_NONE;

    ret = __ssl_ut();
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // this case may fail, when close_notify(sent by ssl_shutdown)
    // comes before processing send_aio
    TT_TEST_CHECK_EQUAL(__sut_ret, TT_SUCCESS, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_u32_t __immd_cli_cnt;
static tt_u32_t __immd_svr_cnt;

void __immd_on_connect(IN struct tt_ssl_s *ssl,
                       IN tt_sktaddr_t *remote_addr,
                       IN tt_ssl_aioctx_t *aioctx)
{
    if (aioctx->result != TT_CANCELLED) {
        __sut_err_line = __LINE__;
        return;
    }

    --__immd_cli_cnt;
}

void __immd_on_accept(IN struct tt_ssl_s *listening_ssl,
                      IN struct tt_ssl_s *new_ssl,
                      IN tt_ssl_aioctx_t *aioctx)
{
    if (aioctx->result != TT_CANCELLED) {
        __sut_err_line = __LINE__;
        return;
    }

    --__immd_svr_cnt;
}

void __immd_on_destroy(IN struct tt_ssl_s *ssl, IN void *cb_param)
{
    *(tt_u32_t *)cb_param -= 1;
}

tt_result_t __ssl_immd_on_init(IN struct tt_evcenter_s *evc,
                               IN void *on_init_param)
{
    tt_skt_exit_t skt_de = {__sut_skt_on_destroy};
    tt_result_t ret;
    tt_ssl_exit_t ssl_de = {0};
    tt_sktaddr_t svr_addr;
    __ssl_ut_param_t *utp = (__ssl_ut_param_t *)&__sut_param;
    tt_skt_attr_t skt_attr;
    tt_blob_t buf = {(tt_u8_t *)&ret, (tt_u32_t)sizeof(ret)};

    // 2 accept, 1 connect
    __immd_cli_cnt = 2;
    __immd_svr_cnt = 3;

    // client: skt
    ret = tt_async_skt_create(&__sut_con_skt,
                              TT_NET_AF_INET,
                              TT_NET_PROTO_TCP,
                              TT_SKT_ROLE_TCP_CONNECT,
                              NULL,
                              &skt_de);
    if (!TT_OK(ret))
        return TT_FAIL;
    __sut_flag |= __sut_con_created;

    // client: ssl context
    ret = tt_sslctx_create(&__sut_cli_sc, TT_SSL_ROLE_CLIENT, NULL);
    if (!TT_OK(ret))
        return TT_FAIL;

    if (utp->cli_cert != NULL) {
        ret = tt_sslctx_add_cert(&__sut_cli_sc,
                                 utp->cli_cert_fmt,
                                 utp->cli_cert,
                                 utp->cli_cert_attr,
                                 utp->cli_privkey_fmt,
                                 utp->cli_privkey,
                                 utp->cli_privkey_attr);
        if (!TT_OK(ret))
            return TT_FAIL;
    }

    if (utp->cli_ca != NULL) {
        ret = tt_sslctx_add_ca(&__sut_cli_sc,
                               utp->cli_ca_fmt,
                               utp->cli_ca,
                               utp->cli_ca_attr);
        if (!TT_OK(ret))
            return TT_FAIL;
    }

    if (utp->cli_verify != NULL) {
        tt_sslctx_set_verify(&__sut_cli_sc, utp->cli_verify);
    }

    ret = tt_sslctx_commit(&__sut_cli_sc, NULL, 0);
    if (!TT_OK(ret))
        return TT_FAIL;

    // client: ssl
    ssl_de.on_destroy = __immd_on_destroy;
    ssl_de.cb_param = &__immd_cli_cnt;
    ret = tt_async_ssl_create(&__sut_con_ssl,
                              &__sut_con_skt,
                              &__sut_cli_sc,
                              NULL,
                              &ssl_de);
    if (!TT_OK(ret))
        return TT_FAIL;
    __sut_flag |= __sut_con_ssl_created;

    // addr
    tt_sktaddr_init(&svr_addr, TT_NET_AF_INET);
    tt_sktaddr_set_ip_p(&svr_addr, (tt_char_t *)utp->svr_addr);
    tt_sktaddr_set_port(&svr_addr, utp->svr_port);

    // listen: skt
    tt_skt_attr_default(&skt_attr);

    tt_skt_attr_set_reuseaddr(&skt_attr, TT_TRUE);

    ret = tt_tcp_server_async(&__sut_lis_skt,
                              TT_NET_AF_INET,
                              &skt_attr,
                              &svr_addr,
                              TT_SKT_BACKLOG_DEFAULT,
                              &skt_de);
    if (!TT_OK(ret))
        return TT_FAIL;
    __sut_flag |= __sut_lis_created;

    // server: ssl context
    ret = tt_sslctx_create(&__sut_svr_sc, TT_SSL_ROLE_SERVER, NULL);
    if (!TT_OK(ret))
        return TT_FAIL;

    ret = tt_sslctx_add_cert(&__sut_svr_sc,
                             __sut_param.svr_cert_fmt,
                             __sut_param.svr_cert,
                             __sut_param.svr_cert_attr,
                             __sut_param.svr_privkey_fmt,
                             __sut_param.svr_privkey,
                             __sut_param.svr_privkey_attr);
    if (!TT_OK(ret))
        return TT_FAIL;

    if (utp->svr_ca != NULL) {
        ret = tt_sslctx_add_ca(&__sut_svr_sc,
                               utp->svr_ca_fmt,
                               utp->svr_ca,
                               utp->svr_ca_attr);
        if (!TT_OK(ret))
            return TT_FAIL;
    }

    if (utp->svr_verify != NULL) {
        tt_sslctx_set_verify(&__sut_svr_sc, utp->svr_verify);
    }

    ret = tt_sslctx_commit(&__sut_svr_sc, NULL, 0);
    if (!TT_OK(ret))
        return TT_FAIL;

    // server: ssl
    ssl_de.on_destroy = __immd_on_destroy;
    ssl_de.cb_param = &__immd_svr_cnt;
    ret = tt_async_ssl_create(&__sut_lis_ssl,
                              &__sut_lis_skt,
                              &__sut_svr_sc,
                              NULL,
                              &ssl_de);
    if (!TT_OK(ret))
        return TT_FAIL;
    __sut_flag |= __sut_lis_ssl_created;

    //////////////////////////////////////////////////////////
    // test server
    //////////////////////////////////////////////////////////

    // server: 2 accept
    ssl_de.on_destroy = __sut_acc_on_destroy;
    ret = tt_ssl_accept_async(&__sut_lis_ssl,
                              &__sut_acc_ssl,
                              &__sut_acc_skt,
                              NULL,
                              NULL,
                              &ssl_de,
                              __immd_on_accept,
                              NULL);
    if (!TT_OK(ret))
        return TT_FAIL;
    ret = tt_ssl_accept_async(&__sut_lis_ssl,
                              &__sut_acc_ssl,
                              &__sut_acc_skt,
                              NULL,
                              NULL,
                              &ssl_de,
                              __immd_on_accept,
                              NULL);
    if (!TT_OK(ret))
        return TT_FAIL;

    // wait for server then connect??
    tt_sleep(500);

    // server can not send/recv/connect
    ret = tt_ssl_send_async(&__sut_lis_ssl, &buf, 1, __sut_ssl_on_send, NULL);
    if (ret == TT_SUCCESS) {
        __sut_err_line = __LINE__;
        return TT_FAIL;
    }
    ret = tt_ssl_recv_async(&__sut_lis_ssl, &buf, 1, __sut_ssl_on_send, NULL);
    if (ret == TT_SUCCESS) {
        __sut_err_line = __LINE__;
        return TT_FAIL;
    }
    ret = tt_ssl_connect_async(&__sut_lis_ssl,
                               &svr_addr,
                               __sut_param.on_connect != NULL
                                   ? __sut_param.on_connect
                                   : __sut_ssl_on_connect,
                               NULL);
    if (TT_OK(ret)) {
        __sut_err_line = __LINE__;
        return TT_FAIL;
    }

    //////////////////////////////////////////////////////////
    // test server
    //////////////////////////////////////////////////////////

    // start connect
    ret = tt_ssl_connect_async(&__sut_con_ssl,
                               &svr_addr,
                               __immd_on_connect,
                               NULL);
    if (!TT_OK(ret))
        return TT_FAIL;

    // server can not send/recv before connected
    ret = tt_ssl_send_async(&__sut_con_ssl, &buf, 1, __sut_ssl_on_send, NULL);
    if (ret == TT_SUCCESS) {
        __sut_err_line = __LINE__;
        return TT_FAIL;
    }
    ret = tt_ssl_recv_async(&__sut_con_ssl, &buf, 1, __sut_ssl_on_send, NULL);
    if (ret == TT_SUCCESS) {
        __sut_err_line = __LINE__;
        return TT_FAIL;
    }
    ret = tt_ssl_accept_async(&__sut_con_ssl,
                              &__sut_acc_ssl,
                              &__sut_acc_skt,
                              NULL,
                              NULL,
                              &ssl_de,
                              __immd_on_accept,
                              NULL);
    if (TT_OK(ret)) {
        __sut_err_line = __LINE__;
        return TT_FAIL;
    }

    //////////////////////////////////////////////////////////
    // destroy immediately
    //////////////////////////////////////////////////////////

    tt_async_ssl_destroy(&__sut_lis_ssl, TT_TRUE);
    tt_async_ssl_destroy(&__sut_con_ssl, TT_TRUE);
    __sut_ret = TT_SUCCESS;

    // return fail, so evc would exit
    return TT_FAIL;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_ssl_destroy_immd)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_blob_t svr_cert;
    tt_ssl_cert_attr_t cert_attr;
    tt_result_t ret;
    tt_bool_t enabled;
    tt_evc_attr_t attr;
    tt_evcenter_t evc;

    TT_TEST_CASE_ENTER()
    // test start

    __sut_ret = TT_FAIL;
    __sut_err_line = 0;

    enabled = tt_ssl_enabled();
#ifdef TT_PLATFORM_SSL_ENABLE
    TT_TEST_CHECK_EQUAL(enabled, TT_TRUE, "");
#else
    // actually it never reach here
    TT_TEST_CHECK_EQUAL(enabled, TT_FALSE, "");
#endif

    tt_memset(&__sut_param, 0, sizeof(__sut_param));

    __sut_param.svr_addr = __SUT_LOCAL_ADDR;
    __sut_param.svr_port = 20020;

    __sut_param.svr_cert_fmt = TT_SSL_CERT_FMT_PKCS12;

    svr_cert.addr = __s_cert_111_11_1_p12_enc;
    svr_cert.len = sizeof(__s_cert_111_11_1_p12_enc);
    __sut_param.svr_cert = &svr_cert;

    cert_attr.encrypted = TT_TRUE;
    cert_attr.pem_armor = TT_FALSE;
    cert_attr.password = "abc123";
    __sut_param.svr_cert_attr = &cert_attr;

    __sut_param.svr_privkey_fmt = TT_SSL_PRIVKEY_FMT_NONE;

    tt_evc_attr_default(&attr);
    attr.on_init = __ssl_immd_on_init;

    ret = tt_evc_create(&evc, TT_FALSE, &attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_evc_wait(&evc);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    TT_TEST_CHECK_EQUAL(__sut_ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(__sut_err_line, 0, "");

    TT_TEST_CHECK_EQUAL(__immd_cli_cnt, 0, "");
    TT_TEST_CHECK_EQUAL(__immd_svr_cnt, 0, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

TT_TEST_ROUTINE_DEFINE(tt_unit_test_ssl_svr_auth)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_blob_t svr_cert;
    tt_ssl_cert_attr_t cert_attr;

    tt_blob_t cli_ca;
    tt_ssl_cert_attr_t cli_ca_attr;
    tt_ssl_verify_t cli_verify;

    tt_result_t ret;

    TT_TEST_CASE_ENTER()
    // test start

    tt_memset(&__sut_param, 0, sizeof(__sut_param));

    __sut_param.svr_addr = "127.0.0.1";
    __sut_param.svr_port = 20020;

    // server's cert

    __sut_param.svr_cert_fmt = TT_SSL_CERT_FMT_PKCS12;

    tt_ssl_verify_init(&cli_verify);
    // cli_verify.enable = TT_TRUE;
    cli_verify.verify_peer = TT_TRUE;
    cli_verify.allow_expired_cert = TT_TRUE;
    __sut_param.cli_verify = &cli_verify;

#if TT_ENV_OS_IS_WINDOWS
    svr_cert.addr = __s_cert_c_p12_enc;
    svr_cert.len = sizeof(__s_cert_c_p12_enc);

    cli_verify.common_name = "C-int";
#else
    svr_cert.addr = __s_cert_e_c_p12_enc;
    svr_cert.len = sizeof(__s_cert_e_c_p12_enc);

    cli_verify.common_name = "E-leaf";
#endif

    __sut_param.svr_cert = &svr_cert;

    cert_attr.encrypted = TT_TRUE;
    cert_attr.pem_armor = TT_FALSE;
    cert_attr.password = "abc123";
    __sut_param.svr_cert_attr = &cert_attr;

    __sut_param.svr_privkey_fmt = TT_SSL_PRIVKEY_FMT_NONE;

    // client's ca

    __sut_param.cli_ca_fmt = TT_SSL_CERT_FMT_X509;

    cli_ca.addr = __s_cert_11_1_x509_pem;
    cli_ca.len = sizeof(__s_cert_11_1_x509_pem);
    __sut_param.cli_ca = &cli_ca;

    cli_ca_attr.encrypted = TT_FALSE;
    cli_ca_attr.pem_armor = TT_TRUE;
    cli_ca_attr.password = NULL;
    __sut_param.cli_ca_attr = &cli_ca_attr;

    ret = __ssl_ut();
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    TT_TEST_CHECK_EQUAL(__sut_ret, TT_SUCCESS, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

static tt_u32_t __saf_fail_counter;
void __saf_on_accept(IN struct tt_ssl_s *listening_ssl,
                     IN struct tt_ssl_s *new_ssl,
                     IN tt_ssl_aioctx_t *aioctx)
{
    // expect fail
    if (!TT_OK(aioctx->result)) {
        ++__saf_fail_counter;

        __sut_flag |= __sut_acc_destroyed;
        if (__sut_flag & __sut_con_destroyed) {
            __sut_ret = TT_SUCCESS;
            tt_evc_exit(TT_LOCAL_EVC);
        }

        return;
    }

    tt_evc_exit(TT_LOCAL_EVC);
}

void __saf_on_connect(IN struct tt_ssl_s *ssl,
                      IN tt_sktaddr_t *remote_addr,
                      IN tt_ssl_aioctx_t *aioctx)
{
    // expect fail
    if (!TT_OK(aioctx->result)) {
        ++__saf_fail_counter;

        tt_async_ssl_destroy(ssl, TT_FALSE);
        return;
    }

    tt_evc_exit(TT_LOCAL_EVC);
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_ssl_svr_auth_fail)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_blob_t svr_cert;
    tt_ssl_cert_attr_t cert_attr;

    tt_blob_t cli_ca;
    tt_ssl_cert_attr_t cli_ca_attr;
    tt_ssl_verify_t cli_verify;

    tt_result_t ret;

    TT_TEST_CASE_ENTER()
    // test start

    __saf_fail_counter = 0;

    tt_memset(&__sut_param, 0, sizeof(__sut_param));

    __sut_param.svr_addr = "127.0.0.1";
    __sut_param.svr_port = 20020;

    // server's cert, not issued by client's ca

    __sut_param.svr_cert_fmt = TT_SSL_CERT_FMT_PKCS12;

    svr_cert.addr = __s_cert_11_p12_enc;
    svr_cert.len = sizeof(__s_cert_11_p12_enc);
    __sut_param.svr_cert = &svr_cert;

    cert_attr.encrypted = TT_TRUE;
    cert_attr.pem_armor = TT_FALSE;
    cert_attr.password = "abc123";
    __sut_param.svr_cert_attr = &cert_attr;

    __sut_param.svr_privkey_fmt = TT_SSL_PRIVKEY_FMT_NONE;

    // client's ca

    __sut_param.cli_cert_fmt = TT_SSL_CERT_FMT_X509;

    cli_ca.addr = __s_cert_b_1_x509_pem;
    cli_ca.len = sizeof(__s_cert_b_1_x509_pem);
    __sut_param.cli_ca = &cli_ca;

    cli_ca_attr.encrypted = TT_FALSE;
    cli_ca_attr.pem_armor = TT_TRUE;
    cli_ca_attr.password = NULL;
    __sut_param.cli_ca_attr = &cli_ca_attr;

    tt_ssl_verify_init(&cli_verify);
    // cli_verify.enable = TT_TRUE;
    cli_verify.verify_peer = TT_TRUE;
    cli_verify.allow_expired_cert = TT_TRUE;
    __sut_param.cli_verify = &cli_verify;

    // callbacks
    __sut_param.on_connect = __saf_on_connect;
    //__sut_param.on_accept = __saf_on_accept;
    // only client known handshake fail(caused by schannel)

    ret = __ssl_ut();
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // TT_TEST_CHECK_EQUAL(__sut_ret,TT_SUCCESS,"");
    TT_TEST_CHECK_EQUAL(__saf_fail_counter, 1, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

TT_TEST_ROUTINE_DEFINE(tt_unit_test_ssl_svr_auth_fail_cn)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_blob_t svr_cert;
    tt_ssl_cert_attr_t cert_attr;

    tt_blob_t cli_ca;
    tt_ssl_cert_attr_t cli_ca_attr;
    tt_ssl_verify_t cli_verify;

    tt_result_t ret;

    TT_TEST_CASE_ENTER()
    // test start

    __saf_fail_counter = 0;

    tt_memset(&__sut_param, 0, sizeof(__sut_param));

    __sut_param.svr_addr = "127.0.0.1";
    __sut_param.svr_port = 20020;

    // server's cert

    __sut_param.svr_cert_fmt = TT_SSL_CERT_FMT_PKCS12;

    tt_ssl_verify_init(&cli_verify);
    // cli_verify.enable = TT_TRUE;
    cli_verify.verify_peer = TT_TRUE;
    cli_verify.allow_expired_cert = TT_TRUE;
    __sut_param.cli_verify = &cli_verify;

#if TT_ENV_OS_IS_WINDOWS
    svr_cert.addr = __s_cert_c_p12_enc;
    svr_cert.len = sizeof(__s_cert_c_p12_enc);

    cli_verify.common_name = "not C-int";
#else
    svr_cert.addr = __s_cert_e_c_p12_enc;
    svr_cert.len = sizeof(__s_cert_e_c_p12_enc);

    cli_verify.common_name = "not E-leaf";
#endif

    __sut_param.svr_cert = &svr_cert;

    cert_attr.encrypted = TT_TRUE;
    cert_attr.pem_armor = TT_FALSE;
    cert_attr.password = "abc123";
    __sut_param.svr_cert_attr = &cert_attr;

    __sut_param.svr_privkey_fmt = TT_SSL_PRIVKEY_FMT_NONE;

    // client's ca

    __sut_param.cli_ca_fmt = TT_SSL_CERT_FMT_X509;

    cli_ca.addr = __s_cert_11_1_x509_pem;
    cli_ca.len = sizeof(__s_cert_11_1_x509_pem);
    __sut_param.cli_ca = &cli_ca;

    cli_ca_attr.encrypted = TT_FALSE;
    cli_ca_attr.pem_armor = TT_TRUE;
    cli_ca_attr.password = NULL;
    __sut_param.cli_ca_attr = &cli_ca_attr;

    // callbacks
    __sut_param.on_connect = __saf_on_connect;
    //__sut_param.on_accept = __saf_on_accept;
    // only client known handshake fail(caused by schannel)

    ret = __ssl_ut();
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // TT_TEST_CHECK_EQUAL(__sut_ret,TT_SUCCESS,"");
    TT_TEST_CHECK_EQUAL(__saf_fail_counter, 1, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

TT_TEST_ROUTINE_DEFINE(tt_unit_test_ssl_cli_auth)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_blob_t svr_cert;
    tt_ssl_cert_attr_t cert_attr;

    tt_blob_t svr_ca;
    tt_ssl_cert_attr_t svr_ca_attr;
    tt_ssl_verify_t svr_verify;

    tt_result_t ret;

    TT_TEST_CASE_ENTER()
    // test start

    tt_memset(&__sut_param, 0, sizeof(__sut_param));

    __sut_param.svr_addr = "127.0.0.1";
    __sut_param.svr_port = 20020;

    // server's cert

    __sut_param.svr_cert_fmt = TT_SSL_CERT_FMT_PKCS12;

    tt_ssl_verify_init(&svr_verify);
    // svr_verify.enable = TT_TRUE;
    svr_verify.verify_peer = TT_TRUE;
    svr_verify.allow_expired_cert = TT_TRUE;
    __sut_param.svr_verify = &svr_verify;

#if TT_ENV_OS_IS_WINDOWS
    svr_cert.addr = __s_cert_c_p12_enc;
    svr_cert.len = sizeof(__s_cert_c_p12_enc);

    svr_verify.common_name = "C-int";
#else
    svr_cert.addr = __s_cert_e_c_p12_enc;
    svr_cert.len = sizeof(__s_cert_e_c_p12_enc);

    svr_verify.common_name = "E-leaf";
#endif
    __sut_param.svr_cert = &svr_cert;

    cert_attr.encrypted = TT_TRUE;
    cert_attr.pem_armor = TT_FALSE;
    cert_attr.password = "abc123";
    __sut_param.svr_cert_attr = &cert_attr;

    __sut_param.svr_privkey_fmt = TT_SSL_PRIVKEY_FMT_NONE;

    // server's ca
    __sut_param.svr_ca_fmt = TT_SSL_CERT_FMT_X509;

    svr_ca.addr = __s_cert_11_1_x509_pem;
    svr_ca.len = sizeof(__s_cert_11_1_x509_pem);
    __sut_param.svr_ca = &svr_ca;

    svr_ca_attr.encrypted = TT_FALSE;
    svr_ca_attr.pem_armor = TT_TRUE;
    svr_ca_attr.password = NULL;
    __sut_param.svr_ca_attr = &svr_ca_attr;

    // client's cert, same cert as svr...
    __sut_param.cli_cert_fmt = TT_SSL_CERT_FMT_PKCS12;
    __sut_param.cli_cert = &svr_cert;
    __sut_param.cli_cert_attr = &cert_attr;
    __sut_param.cli_privkey_fmt = TT_SSL_PRIVKEY_FMT_NONE;

    ret = __ssl_ut();
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    TT_TEST_CHECK_EQUAL(__sut_ret, TT_SUCCESS, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

static tt_u32_t __caf_fail_counter;
void __caf_on_accept(IN struct tt_ssl_s *listening_ssl,
                     IN struct tt_ssl_s *new_ssl,
                     IN tt_ssl_aioctx_t *aioctx)
{
    // expect fail
    if (!TT_OK(aioctx->result)) {
        ++__caf_fail_counter;

        __sut_flag |= __sut_acc_destroyed;
        if (__sut_flag & __sut_con_destroyed) {
            __sut_ret = TT_SUCCESS;
            tt_evc_exit(TT_LOCAL_EVC);
        }

        return;
    }

    tt_evc_exit(TT_LOCAL_EVC);
}

void __caf_on_connect(IN struct tt_ssl_s *ssl,
                      IN tt_sktaddr_t *remote_addr,
                      IN tt_ssl_aioctx_t *aioctx)
{
    // expect fail
    if (!TT_OK(aioctx->result)) {
        ++__caf_fail_counter;

        // tt_async_ssl_destroy(ssl, TT_FALSE);
        tt_async_ssl_shutdown(ssl, TT_SSL_SHUTDOWN_RDWR);
        return;
    }

    tt_evc_exit(TT_LOCAL_EVC);
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_ssl_cli_auth_fail)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_blob_t svr_cert;
    tt_ssl_cert_attr_t cert_attr;

    tt_blob_t svr_ca;
    tt_ssl_cert_attr_t svr_ca_attr;
    tt_ssl_verify_t svr_verify;

    tt_blob_t cli_cert;
    tt_ssl_cert_attr_t cli_cert_attr;

    tt_result_t ret;

    TT_TEST_CASE_ENTER()
    // test start

    __caf_fail_counter = 0;

    tt_memset(&__sut_param, 0, sizeof(__sut_param));

    __sut_param.svr_addr = "127.0.0.1";
    __sut_param.svr_port = 20020;

    // server's cert

    __sut_param.svr_cert_fmt = TT_SSL_CERT_FMT_PKCS12;

    svr_cert.addr = __s_cert_11_p12_enc;
    svr_cert.len = sizeof(__s_cert_11_p12_enc);
    __sut_param.svr_cert = &svr_cert;

    cert_attr.encrypted = TT_TRUE;
    cert_attr.pem_armor = TT_FALSE;
    cert_attr.password = "abc123";
    __sut_param.svr_cert_attr = &cert_attr;

    __sut_param.svr_privkey_fmt = TT_SSL_PRIVKEY_FMT_NONE;

    tt_ssl_verify_init(&svr_verify);
    // svr_verify.enable = TT_TRUE;
    svr_verify.verify_peer = TT_TRUE;
    svr_verify.allow_expired_cert = TT_TRUE;
    __sut_param.svr_verify = &svr_verify;

    // server's ca
    __sut_param.svr_ca_fmt = TT_SSL_CERT_FMT_X509;

    svr_ca.addr = __s_cert_1_x509_pem;
    svr_ca.len = sizeof(__s_cert_1_x509_pem);
    __sut_param.svr_ca = &svr_ca;

    svr_ca_attr.encrypted = TT_FALSE;
    svr_ca_attr.pem_armor = TT_TRUE;
    svr_ca_attr.password = NULL;
    __sut_param.svr_ca_attr = &svr_ca_attr;

    // client's cert, not issued by svr ca

    __sut_param.cli_cert_fmt = TT_SSL_CERT_FMT_PKCS12;

    cli_cert.addr = __s_cert_4_p12_enc;
    cli_cert.len = sizeof(__s_cert_4_p12_enc);
    __sut_param.cli_cert = &cli_cert;

    cli_cert_attr.encrypted = TT_TRUE;
    cli_cert_attr.pem_armor = TT_FALSE;
    cli_cert_attr.password = "abc123";
    __sut_param.cli_cert_attr = &cli_cert_attr;

    __sut_param.cli_privkey_fmt = TT_SSL_PRIVKEY_FMT_NONE;

    // callbacks
    //__sut_param.on_connect = __caf_on_connect;
    __sut_param.on_accept = __caf_on_accept;
    // only server know auth failed

    ret = __ssl_ut();
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    TT_TEST_CHECK_EQUAL(__caf_fail_counter, 1, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

TT_TEST_ROUTINE_DEFINE(tt_unit_test_ssl_cli_auth_fail_cn)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_blob_t svr_cert;
    tt_ssl_cert_attr_t cert_attr;

    tt_blob_t svr_ca;
    tt_ssl_cert_attr_t svr_ca_attr;
    tt_ssl_verify_t svr_verify;

    tt_result_t ret;

    TT_TEST_CASE_ENTER()
    // test start

    __caf_fail_counter = 0;

    tt_memset(&__sut_param, 0, sizeof(__sut_param));

    __sut_param.svr_addr = "127.0.0.1";
    __sut_param.svr_port = 20020;

    // server's cert

    __sut_param.svr_cert_fmt = TT_SSL_CERT_FMT_PKCS12;

    tt_ssl_verify_init(&svr_verify);
    // svr_verify.enable = TT_TRUE;
    svr_verify.verify_peer = TT_TRUE;
    svr_verify.allow_expired_cert = TT_TRUE;
    __sut_param.svr_verify = &svr_verify;

#if TT_ENV_OS_IS_WINDOWS
    svr_cert.addr = __s_cert_c_p12_enc;
    svr_cert.len = sizeof(__s_cert_c_p12_enc);

    svr_verify.common_name = "C-int xxx";
#else
    svr_cert.addr = __s_cert_e_c_p12_enc;
    svr_cert.len = sizeof(__s_cert_e_c_p12_enc);

    svr_verify.common_name = "E-leaf xxx";
#endif
    __sut_param.svr_cert = &svr_cert;

    cert_attr.encrypted = TT_TRUE;
    cert_attr.pem_armor = TT_FALSE;
    cert_attr.password = "abc123";
    __sut_param.svr_cert_attr = &cert_attr;

    __sut_param.svr_privkey_fmt = TT_SSL_PRIVKEY_FMT_NONE;

    // server's ca
    __sut_param.svr_ca_fmt = TT_SSL_CERT_FMT_X509;

    svr_ca.addr = __s_cert_11_1_x509_pem;
    svr_ca.len = sizeof(__s_cert_11_1_x509_pem);
    __sut_param.svr_ca = &svr_ca;

    svr_ca_attr.encrypted = TT_FALSE;
    svr_ca_attr.pem_armor = TT_TRUE;
    svr_ca_attr.password = NULL;
    __sut_param.svr_ca_attr = &svr_ca_attr;

    // client's cert, same cert as svr...
    __sut_param.cli_cert_fmt = TT_SSL_CERT_FMT_PKCS12;
    __sut_param.cli_cert = &svr_cert;
    __sut_param.cli_cert_attr = &cert_attr;
    __sut_param.cli_privkey_fmt = TT_SSL_PRIVKEY_FMT_NONE;

    // callbacks
    //__sut_param.on_connect = __caf_on_connect;
    __sut_param.on_accept = __caf_on_accept;
    // only server know auth failed

    ret = __ssl_ut();
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    TT_TEST_CHECK_EQUAL(__caf_fail_counter, 1, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

//#define __SSL_STRESS_DETAIL

static tt_sslctx_t __st1_svr_ctx;
static tt_sslctx_t __st1_cli_ctx;

static tt_skt_t __st1_lis_skt;
static tt_ssl_t __st1_lis_ssl;
static tt_sktaddr_t __st1_lis_addr;

#define __ST1_CONCURRENT_NUM 100
#define __ST1_SSL_NUM 1000

#define __ST_EVC_NUM 1
#define __ST_BUF_SIZE 500
static tt_u8_t __st_buf[__ST_EVC_NUM][__ST_BUF_SIZE];

#define __ST_SES_RESUME TT_TRUE

static tt_u32_t __st_err_line;
static tt_bool_t __st_end[__ST_EVC_NUM];

static tt_u32_t __st_ssl_num[__ST_EVC_NUM][2];
// [][0] records acc ssl num
// [][1] records con ssl num

static tt_u32_t __st_aio_con_num;
static tt_u32_t __st_aio_acc_num;
static tt_u32_t __st_aio_on_con_num;
static tt_u32_t __st_aio_on_acc_num;

static void __st1_ssl_on_accept(IN struct tt_ssl_s *listening_ssl,
                                IN struct tt_ssl_s *new_ssl,
                                IN tt_ssl_aioctx_t *aioctx);
static void __st1_ssl_on_connect(IN struct tt_ssl_s *ssl,
                                 IN tt_sktaddr_t *remote_addr,
                                 IN tt_ssl_aioctx_t *aioctx);

static void __st1_lis_on_destroy(IN struct tt_ssl_s *ssl, IN void *cb_param)
{
#ifdef __SSL_STRESS_DETAIL
    TT_INFO("listen ssl[%p] on destroy", ssl);
#endif

    tt_evc_exit(TT_LOCAL_EVC);
}

static void __st1_acc_on_destroy(IN struct tt_ssl_s *ssl, IN void *cb_param)
{
    tt_u32_t evc_idx = (tt_u32_t)(tt_uintptr_t)cb_param;

#ifdef __SSL_STRESS_DETAIL
    TT_INFO("accept ssl[%p] on destroy", ssl);
#endif

    ++__st_ssl_num[evc_idx][0];
    // TT_ASSERT(__st_ssl_num[evc_idx][0] <= __ST1_SSL_NUM);
    if (__st_ssl_num[evc_idx][0] == __ST1_SSL_NUM) {
        if (__st_ssl_num[evc_idx][1] == __ST1_SSL_NUM) {
            tt_async_ssl_destroy(&__st1_lis_ssl, TT_FALSE);
        }
        return;
    } else if (__st_ssl_num[evc_idx][0] >
               (__ST1_SSL_NUM - __ST1_CONCURRENT_NUM)) {
        // the ssl first destroyed in final pair
        return;
    }

    do {
        tt_ssl_t *new_ssl = tt_malloc(sizeof(tt_ssl_t));
        tt_skt_t *new_skt = tt_malloc(sizeof(tt_skt_t));
        tt_skt_attr_t skt_attr;
        tt_ssl_attr_t ssl_attr;
        tt_ssl_exit_t acc_ssl_de = {__st1_acc_on_destroy,
                                    (void *)(tt_uintptr_t)evc_idx};

        tt_skt_attr_default(&skt_attr);
        skt_attr.from_alloc = TT_TRUE;

        tt_ssl_attr_default(&ssl_attr);
        ssl_attr.session_resume = __ST_SES_RESUME;
        ssl_attr.from_alloc = TT_TRUE;

        if (tt_ssl_accept_async(&__st1_lis_ssl,
                                new_ssl,
                                new_skt,
                                &skt_attr,
                                &ssl_attr,
                                &acc_ssl_de,
                                __st1_ssl_on_accept,
                                NULL) != TT_SUCCESS) {
            __st_err_line = __LINE__;
            tt_evc_exit(TT_LOCAL_EVC);
            return;
        }
        ++__st_aio_acc_num;
    } while (0);
}

static void __st1_con_on_destroy(IN struct tt_ssl_s *ssl, IN void *cb_param)
{
    tt_u32_t evc_idx = (tt_u32_t)(tt_uintptr_t)cb_param;

#ifdef __SSL_STRESS_DETAIL
    TT_INFO("connect ssl[%p] on destroy", ssl);
#endif

    ++__st_ssl_num[evc_idx][1];
    // TT_ASSERT(__st_ssl_num[evc_idx][1] <= __ST1_SSL_NUM);
    if (__st_ssl_num[evc_idx][1] == __ST1_SSL_NUM) {
        if (__st_ssl_num[evc_idx][0] == __ST1_SSL_NUM) {
            tt_async_ssl_destroy(&__st1_lis_ssl, TT_FALSE);
        }
        return;
    } else if (__st_ssl_num[evc_idx][1] >
               (__ST1_SSL_NUM - __ST1_CONCURRENT_NUM)) {
        // the ssl first destroyed in final pair
        return;
    }

    do {
        tt_ssl_t *new_ssl = tt_malloc(sizeof(tt_ssl_t));
        tt_skt_t *new_skt = tt_malloc(sizeof(tt_skt_t));
        tt_skt_attr_t skt_attr;
        tt_ssl_attr_t ssl_attr;
        tt_skt_exit_t skt_de = {__sut_skt_on_destroy,
                                (void *)(tt_uintptr_t)evc_idx};
        tt_ssl_exit_t con_ssl_de = {__st1_con_on_destroy,
                                    (void *)(tt_uintptr_t)evc_idx};

        tt_skt_attr_default(&skt_attr);
        skt_attr.from_alloc = TT_TRUE;

        tt_ssl_attr_default(&ssl_attr);
        ssl_attr.from_alloc = TT_TRUE;
        ssl_attr.session_resume = __ST_SES_RESUME;

        tt_async_skt_create(new_skt,
                            TT_NET_AF_INET,
                            TT_NET_PROTO_TCP,
                            TT_SKT_ROLE_TCP_CONNECT,
                            &skt_attr,
                            &skt_de);
        tt_async_ssl_create(new_ssl,
                            new_skt,
                            &__st1_cli_ctx,
                            &ssl_attr,
                            &con_ssl_de);

        if (tt_ssl_connect_async(new_ssl,
                                 &__st1_lis_addr,
                                 __st1_ssl_on_connect,
                                 NULL) != TT_SUCCESS) {
            __st_err_line = __LINE__;
            tt_evc_exit(TT_LOCAL_EVC);
        }
        ++__st_aio_con_num;
    } while (0);
}

static void __st1_ssl_on_recv(IN struct tt_ssl_s *ssl,
                              IN tt_blob_t *buf_array,
                              IN tt_u32_t buf_num,
                              IN tt_ssl_aioctx_t *aioctx,
                              IN tt_u32_t recv_len)
{
    tt_blob_t buf;

#ifdef __SSL_STRESS_DETAIL
    TT_INFO("ssl[%p] recv %d", ssl, recv_len);
#endif

    if (!TT_OK(aioctx->result)) {
        if (aioctx->result == TT_END) {
            tt_async_ssl_shutdown(ssl, TT_SSL_SHUTDOWN_RDWR);
            return;
        } else {
            __st_err_line = __LINE__;
            tt_evc_exit(TT_LOCAL_EVC);
            return;
        }
    }

    // received data, shutdown wr to close ssl
    tt_async_ssl_shutdown(ssl, TT_SSL_SHUTDOWN_WR);

    buf.addr = __st_buf[(tt_u32_t)(tt_uintptr_t)aioctx->cb_param];
    buf.len = tt_rand_u32() % __ST_BUF_SIZE + 1;
    if (tt_ssl_recv_async(ssl, &buf, 1, __st1_ssl_on_recv, NULL) !=
        TT_SUCCESS) {
        __st_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }
}

static void __st1_ssl_on_send(IN struct tt_ssl_s *ssl,
                              IN tt_blob_t *buf_array,
                              IN tt_u32_t buf_num,
                              IN tt_ssl_aioctx_t *aioctx,
                              IN tt_u32_t send_len)
{
#ifdef __SSL_STRESS_DETAIL
    TT_INFO("ssl[%p] send %d", ssl, send_len);
#endif

    if (!TT_OK(aioctx->result) && (aioctx->result != TT_END)) {
        __st_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    return;
}

static void __st1_ssl_on_accept(IN struct tt_ssl_s *listening_ssl,
                                IN struct tt_ssl_s *new_ssl,
                                IN tt_ssl_aioctx_t *aioctx)
{
    tt_blob_t buf;

#ifdef __SSL_STRESS_DETAIL
    TT_INFO("ssl[%p] on accept", listening_ssl);
#endif

    ++__st_aio_on_acc_num;

    if (!TT_OK(aioctx->result)) {
        if (aioctx->result == TT_END) {
            // it's possible accept aio return io_end, i.e. the acc_on_destroy
            // is called and the acc skt is 9th destroyed, it would post an
            // accept aio and then the 10th con skt is destroyed, which destroy
            // listen socket and the latest posted accept aio returns io_end
            return;
        }

        {
            tt_u16_t port;
            tt_sktaddr_t addr;

            if (TT_OK(tt_skt_local_addr(new_ssl->skt, &addr))) {
                port = tt_sktaddr_get_port(&addr);
                TT_INFO("fail port: %d", port);
            }
        }
        __st_err_line = __LINE__;
        // tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

#if 0    
    // test without ssl io
    tt_async_ssl_shutdown(new_ssl, TT_SSL_SHUTDOWN_RDWR);
    return;
#endif

    buf.addr = __st_buf[(tt_u32_t)(tt_uintptr_t)aioctx->cb_param];
    buf.len = tt_rand_u32() % __ST_BUF_SIZE + 1;
    if (tt_ssl_recv_async(new_ssl, &buf, 1, __st1_ssl_on_recv, NULL) !=
        TT_SUCCESS) {
        __st_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    // send some data
    buf.addr = __st_buf[(tt_u32_t)(tt_uintptr_t)aioctx->cb_param];
    buf.len = tt_rand_u32() % __ST_BUF_SIZE + 1;
    if (tt_ssl_send_async(new_ssl, &buf, 1, __st1_ssl_on_send, NULL) !=
        TT_SUCCESS) {
        __st_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

#if 0 // shutdown when received some data
    // shutdown
    if (tt_async_ssl_shutdown(new_ssl, TT_SSL_SHUTDOWN_WR) != TT_SUCCESS) {
        __st_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }
#endif
}

static void __st1_ssl_on_connect(IN struct tt_ssl_s *ssl,
                                 IN tt_sktaddr_t *remote_addr,
                                 IN tt_ssl_aioctx_t *aioctx)
{
    tt_blob_t buf;

#ifdef __SSL_STRESS_DETAIL
    TT_INFO("ssl[%p] on connect", ssl);
#endif

    ++__st_aio_on_con_num;

    if (!TT_OK(aioctx->result)) {
        tt_u16_t port;
        tt_sktaddr_t addr;

        if (TT_OK(tt_skt_local_addr(ssl->skt, &addr))) {
            port = tt_sktaddr_get_port(&addr);
            TT_INFO("fail port: %d", port);
        }

        __st_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

#if 0
    // test without ssl io
    tt_async_ssl_shutdown(ssl, TT_SSL_SHUTDOWN_RDWR);
    return;
#endif

    buf.addr = __st_buf[(tt_u32_t)(tt_uintptr_t)aioctx->cb_param];
    buf.len = tt_rand_u32() % __ST_BUF_SIZE + 1;
    if (tt_ssl_recv_async(ssl, &buf, 1, __st1_ssl_on_recv, NULL) !=
        TT_SUCCESS) {
        __st_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    // send some data
    buf.addr = __st_buf[(tt_u32_t)(tt_uintptr_t)aioctx->cb_param];
    buf.len = tt_rand_u32() % __ST_BUF_SIZE + 1;
    if (tt_ssl_send_async(ssl, &buf, 1, __st1_ssl_on_send, NULL) !=
        TT_SUCCESS) {
        __st_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

#if 0 // shutdown when received some data
    // shutdown
    if (tt_async_ssl_shutdown(ssl, TT_SSL_SHUTDOWN_WR) != TT_SUCCESS) {
        __st_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }
#endif
}

static tt_result_t __st1_evc_on_init(IN struct tt_evcenter_s *evc,
                                     IN void *on_init_param)
{
    tt_result_t ret;
    tt_u32_t i = (tt_u32_t)(tt_uintptr_t)on_init_param;
    tt_skt_exit_t skt_de = {__sut_skt_on_destroy, (void *)(tt_uintptr_t)i};
    tt_ssl_exit_t lis_ssl_de = {__st1_lis_on_destroy, (void *)(tt_uintptr_t)i};
    tt_ssl_exit_t acc_ssl_de = {__st1_acc_on_destroy, (void *)(tt_uintptr_t)i};
    tt_ssl_exit_t con_ssl_de = {__st1_con_on_destroy, (void *)(tt_uintptr_t)i};
    tt_skt_attr_t lis_skt_attr;

    __st_end[i] = TT_FALSE;
    __st_ssl_num[i][0] = 0;
    __st_ssl_num[i][1] = 0;

    tt_sktaddr_init(&__st1_lis_addr, TT_NET_AF_INET);
    tt_sktaddr_set_ip_p(&__st1_lis_addr, "127.0.0.1");
    tt_sktaddr_set_port(&__st1_lis_addr, 33333);

    tt_skt_attr_default(&lis_skt_attr);
    tt_skt_attr_set_reuseaddr(&lis_skt_attr, TT_TRUE);
    tt_skt_attr_set_reuseport(&lis_skt_attr, TT_TRUE);

    ret = tt_tcp_server_async(&__st1_lis_skt,
                              TT_NET_AF_INET,
                              &lis_skt_attr,
                              &__st1_lis_addr,
                              TT_SKT_BACKLOG_DEFAULT,
                              &skt_de);
    if (!TT_OK(ret)) {
        return ret;
    }

    ret = tt_async_ssl_create(&__st1_lis_ssl,
                              &__st1_lis_skt,
                              &__st1_svr_ctx,
                              NULL,
                              &lis_ssl_de);
    if (!TT_OK(ret)) {
        return ret;
    }

#if 1 // disable code below to act as a tls client
    for (i = 0; i < __ST1_CONCURRENT_NUM; ++i) {
        tt_ssl_t *new_ssl = tt_malloc(sizeof(tt_ssl_t));
        tt_skt_t *new_skt = tt_malloc(sizeof(tt_skt_t));
        tt_skt_attr_t skt_attr;
        tt_ssl_attr_t ssl_attr;

        tt_skt_attr_default(&skt_attr);
        skt_attr.from_alloc = TT_TRUE;

        tt_ssl_attr_default(&ssl_attr);
        ssl_attr.from_alloc = TT_TRUE;
        ssl_attr.session_resume = __ST_SES_RESUME;

        ret = tt_ssl_accept_async(&__st1_lis_ssl,
                                  new_ssl,
                                  new_skt,
                                  &skt_attr,
                                  &ssl_attr,
                                  &acc_ssl_de,
                                  __st1_ssl_on_accept,
                                  NULL);
        if (ret != TT_SUCCESS) {
            return TT_FAIL;
        }
        ++__st_aio_acc_num;
    }
#endif

#if 1 // disable code below to act as a tls server
    for (i = 0; i < __ST1_CONCURRENT_NUM; ++i) {
        tt_ssl_t *new_ssl = tt_malloc(sizeof(tt_ssl_t));
        tt_skt_t *new_skt = tt_malloc(sizeof(tt_skt_t));
        tt_skt_attr_t skt_attr;
        tt_ssl_attr_t ssl_attr;

        tt_skt_attr_default(&skt_attr);
        skt_attr.from_alloc = TT_TRUE;

        tt_ssl_attr_default(&ssl_attr);
        ssl_attr.from_alloc = TT_TRUE;
        ssl_attr.session_resume = __ST_SES_RESUME;

        tt_async_skt_create(new_skt,
                            TT_NET_AF_INET,
                            TT_NET_PROTO_TCP,
                            TT_SKT_ROLE_TCP_CONNECT,
                            &skt_attr,
                            &skt_de);
        tt_async_ssl_create(new_ssl,
                            new_skt,
                            &__st1_cli_ctx,
                            &ssl_attr,
                            &con_ssl_de);

        ret = tt_ssl_connect_async(new_ssl,
                                   &__st1_lis_addr,
                                   __st1_ssl_on_connect,
                                   NULL);
        if (ret != TT_SUCCESS) {
            return TT_FAIL;
        }
        ++__st_aio_con_num;
    }
#endif

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_ssl_stress_1_evc)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    tt_blob_t cert;
    tt_ssl_cert_attr_t cert_attr;

    tt_evcenter_t evc;
    tt_evc_attr_t evc_attr;

    TT_TEST_CASE_ENTER()
    // test start

    __st_err_line = 0;

    // server ssl ctx
    ret = tt_sslctx_create(&__st1_svr_ctx, TT_SSL_ROLE_SERVER, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    cert.addr = __s_cert_e_c_p12_enc;
    cert.len = sizeof(__s_cert_e_c_p12_enc);

    cert_attr.encrypted = TT_TRUE;
    cert_attr.pem_armor = TT_FALSE;
    cert_attr.password = "abc123";

    ret = tt_sslctx_add_cert(&__st1_svr_ctx,
                             TT_SSL_CERT_FMT_PKCS12,
                             &cert,
                             &cert_attr,
                             TT_SSL_PRIVKEY_FMT_NONE,
                             NULL,
                             NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_sslctx_commit(&__st1_svr_ctx, NULL, 0);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // client ssl ctx
    ret = tt_sslctx_create(&__st1_cli_ctx, TT_SSL_ROLE_CLIENT, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_sslctx_commit(&__st1_cli_ctx, NULL, 0);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // evc
    tt_evc_attr_default(&evc_attr);
    evc_attr.on_init = __st1_evc_on_init;
    evc_attr.on_init_param = 0;

    ret = tt_evc_create(&evc, TT_FALSE, &evc_attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_evc_wait(&evc);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    __st1_svr_ctx.ssl_cache.attr.peer_expire_ms = 1000; // hack
    tt_sslcache_scan(&__st1_svr_ctx.ssl_cache);

    TT_INFO("__st_err_line: %d", __st_err_line);
    TT_TEST_CHECK_EQUAL(__st_err_line, 0, "");

    // test end
    TT_TEST_CASE_LEAVE()
}
