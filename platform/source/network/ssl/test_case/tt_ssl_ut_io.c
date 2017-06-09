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

#define __X509_CA "tt_ca.crt"
#define __CA_key "tt_ca.key"

#define __X509_CA2 "tt_ca_int.crt"
#define __CA_key2 "tt_ca_int.key"

#define __X509_LEAF "tt_ca_leaf.crt"
#define __leaf_key "tt_ca_leaf.key"

#define __X509_CRL1 "tt_ca.crl"

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

extern void __x509_prepare(void *);
extern tt_bool_t has_x509;

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

// === routine declarations ================
TT_TEST_ROUTINE_DECLARE(tt_unit_test_ssl_basic)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(ssl_io_case)

TT_TEST_CASE("tt_unit_test_ssl_basic",
             "ssl: basic io",
             tt_unit_test_ssl_basic,
             NULL,
             __x509_prepare,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE_LIST_DEFINE_END(ssl_io_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(SSL_UT_IO, 0, ssl_io_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(tt_unit_test_ssl_basic)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    static tt_u32_t __ssl_err_line;
static tt_atomic_s64_t __io_num;
static tt_u32_t __svr_sent, __svr_recvd, __cli_recvd, __cli_sent;
static tt_ssl_config_t sc_cli_1, sc_svr_1;

static tt_result_t __f_svr(IN void *param)
{
    tt_skt_t *s, *new_s;
    tt_u8_t buf[1 << 14] = "6789";
    tt_u32_t n;
    tt_result_t ret;
    tt_fiber_ev_t *fev;
    tt_tmr_t *tmr;
    tt_ssl_t *ssl;

    s = tt_skt_create(TT_NET_AF_INET, TT_NET_PROTO_TCP, NULL);
    if (s == NULL) {
        __ssl_err_line = __LINE__;
        return TT_FAIL;
    }

    tt_skt_set_reuseaddr(s, TT_TRUE);

    if (!TT_OK(tt_skt_bind_p(s, TT_NET_AF_INET, "127.0.0.1", 60606))) {
        __ssl_err_line = __LINE__;
        return TT_FAIL;
    }

    if (!TT_OK(tt_skt_listen(s))) {
        __ssl_err_line = __LINE__;
        return TT_FAIL;
    }

    new_s = tt_skt_accept(s, NULL, NULL);
    if (new_s == NULL) {
        __ssl_err_line = __LINE__;
        return TT_FAIL;
    }

    ssl = tt_ssl_create(new_s, &sc_svr_1);
    if (ssl == NULL) {
        __ssl_err_line = __LINE__;
        return TT_FAIL;
    }

    if (!TT_OK(tt_ssl_handshake(ssl, &fev, &tmr))) {
        __ssl_err_line = __LINE__;
        return TT_FAIL;
    }

    while ((ret = tt_ssl_recv(ssl, buf, sizeof(buf), &n, &fev, &tmr)) !=
           TT_END) {
        tt_u32_t total = n;
#ifdef __TCP_DETAIL
        if (n < sizeof(buf)) {
            TT_INFO("server recv %d", n);
        }
#endif
        /*
         it has to recv all data, otherwise, data are accumulated in recv
         buffer of new_s. if new_s recv buffer is full, client will be
         blocked in tt_skt_send()
         */
        while (total < sizeof(buf)) {
            if (!TT_OK(tt_ssl_recv(ssl, buf, sizeof(buf), &n, &fev, &tmr))) {
                __ssl_err_line = __LINE__;
                return TT_FAIL;
            }
#ifdef __TCP_DETAIL
            if (n < sizeof(buf)) {
                TT_INFO("server recv %d", n);
            }
#endif
            __svr_recvd += n;

            total += n;
        }
        TT_ASSERT(total == sizeof(buf));
        tt_atomic_s64_add(&__io_num, total);

        if (!TT_OK(tt_ssl_send(ssl, buf, sizeof(buf), &n))) {
            __ssl_err_line = __LINE__;
            return TT_FAIL;
        }
#ifdef __TCP_DETAIL
        if (n < sizeof(buf)) {
            TT_INFO("server send %d", n);
        }
#endif
        __svr_sent += n;
        if (n != sizeof(buf)) {
            __ssl_err_line = __LINE__;
            return TT_FAIL;
        }
    }

    if (!TT_OK(tt_ssl_shutdown(ssl, TT_SSL_SHUT_WR))) {
        __ssl_err_line = __LINE__;
        return TT_FAIL;
    }
#ifdef __TCP_DETAIL
    TT_INFO("server shutdown");
#endif

    if (tt_ssl_recv(ssl, buf, sizeof(buf), &n, &fev, &tmr) != TT_END) {
        __ssl_err_line = __LINE__;
        return TT_FAIL;
    }
#ifdef __TCP_DETAIL
    TT_INFO("server recv end");
#endif

    // tt_skt_destroy(new_s);
    tt_ssl_destroy(ssl);
    tt_skt_destroy(s);

    return TT_SUCCESS;
}

static tt_result_t __f_cli(IN void *param)
{
    tt_skt_t *s;
    tt_u8_t buf[1 << 14] = "123";
    tt_u32_t n, loop;
    tt_fiber_ev_t *fev;
    tt_tmr_t *tmr;
    tt_ssl_t *ssl;

    s = tt_skt_create(TT_NET_AF_INET, TT_NET_PROTO_TCP, NULL);
    if (s == NULL) {
        __ssl_err_line = __LINE__;
        return TT_FAIL;
    }

    if (!TT_OK(tt_skt_connect_p(s, TT_NET_AF_INET, "127.0.0.1", 60606))) {
        __ssl_err_line = __LINE__;
        return TT_FAIL;
    }

    ssl = tt_ssl_create(s, &sc_cli_1);
    if (ssl == NULL) {
        __ssl_err_line = __LINE__;
        return TT_FAIL;
    }

    if (!TT_OK(tt_ssl_handshake(ssl, &fev, &tmr))) {
        __ssl_err_line = __LINE__;
        return TT_FAIL;
    }

    loop = 0;
    while (loop++ < (1 << 13)) {
        tt_u32_t total = 0;

        if (!TT_OK(tt_ssl_send(ssl, buf, sizeof(buf), &n))) {
            __ssl_err_line = __LINE__;
            return TT_FAIL;
        }
#ifdef __TCP_DETAIL
        if (n < sizeof(buf)) {
            TT_INFO("client sent %d", n);
        }
#endif
        __cli_sent += n;
        if (n != sizeof(buf)) {
            __ssl_err_line = __LINE__;
            return TT_FAIL;
        }

        total = 0;
        while (total < sizeof(buf)) {
            if (!TT_OK(tt_ssl_recv(ssl, buf, sizeof(buf), &n, &fev, &tmr))) {
                __ssl_err_line = __LINE__;
                return TT_FAIL;
            }
#ifdef __TCP_DETAIL
            if (n < sizeof(buf)) {
                TT_INFO("client recv %d", n);
            }
#endif
            __cli_recvd += n;

            total += n;
        }
        TT_ASSERT(total == sizeof(buf));
    }

    if (!TT_OK(tt_ssl_shutdown(ssl, TT_SSL_SHUT_WR))) {
        __ssl_err_line = __LINE__;
        return TT_FAIL;
    }
#ifdef __TCP_DETAIL
    TT_INFO("client shutdown");
#endif

    while (tt_ssl_recv(ssl, buf, sizeof(buf), &n, &fev, &tmr) != TT_END) {
    }
#ifdef __TCP_DETAIL
    TT_INFO("client recv end");
#endif

    // tt_skt_destroy(s);
    tt_ssl_destroy(ssl);

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_ssl_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    tt_task_t t;
    tt_s64_t start, end, dur;
    tt_x509cert_t ca, cert;
    tt_x509crl_t crl;
    tt_pk_t pk;

    TT_TEST_CASE_ENTER()
    // test start

    ret = tt_task_create(&t, NULL);
    TT_UT_SUCCESS(ret, "");

    tt_task_add_fiber(&t, NULL, __f_svr, NULL, NULL);
    tt_task_add_fiber(&t, NULL, __f_cli, NULL, NULL);

    __ssl_err_line = 0;
    tt_atomic_s64_set(&__io_num, 0);

    ret = tt_ssl_config_create(&sc_cli_1,
                               TT_SSL_CLIENT,
                               TT_SSL_TRANSPORT_STREAM,
                               TT_SSL_PRESET_DEFAULT);
    TT_UT_SUCCESS(ret, "");

    ret = tt_ssl_config_create(&sc_svr_1,
                               TT_SSL_SERVER,
                               TT_SSL_TRANSPORT_STREAM,
                               TT_SSL_PRESET_DEFAULT);
    TT_UT_SUCCESS(ret, "");

    tt_x509cert_init(&cert);
    ret = tt_x509cert_add_file(&cert, __X509_CA2);
    TT_UT_SUCCESS(ret, "");
    tt_pk_init(&pk);
    ret = tt_pk_load_private_file(&pk, __CA_key2, (tt_u8_t *)"456", 3);
    tt_ssl_config_cert(&sc_svr_1, &cert, &pk);

    tt_x509cert_init(&ca);
    ret = tt_x509cert_add_file(&ca, __X509_CA);
    TT_UT_SUCCESS(ret, "");
    tt_x509crl_init(&crl);
    // ret = tt_x509crl_add_file(&crl, __X509_CRL1);
    TT_UT_SUCCESS(ret, "");
    tt_ssl_config_ca(&sc_cli_1, &ca, &crl);

    start = tt_time_ref();

    ret = tt_task_run(&t);
    TT_UT_SUCCESS(ret, "");

    tt_task_wait(&t);
    TT_UT_EQUAL(__ssl_err_line, 0, "");

    end = tt_time_ref();
    dur = tt_time_ref2ms(end - start);
    TT_RECORD_INFO("speed: %f MB/s",
                   (float)(tt_atomic_s64_get(&__io_num) >> 20) * 1000 / dur);

    tt_x509cert_destroy(&ca);
    tt_x509crl_destroy(&crl);
    tt_x509cert_destroy(&cert);
    tt_pk_destroy(&pk);

    // test end
    TT_TEST_CASE_LEAVE()
}
