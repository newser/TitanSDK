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

#if (TT_ENV_OS_FEATURE & TT_ENV_OS_FEATURE_IOS_SIMULATOR)
#define __X509_CA "/tmp/tt_ca.crt"
#define __CA_key "/tmp/tt_ca.key"

#define __X509_CA2 "/tmp/tt_ca_int.crt"
#define __CA_key2 "/tmp/tt_ca_int.key"

#define __X509_LEAF "/tmp/tt_ca_leaf.crt"
#define __leaf_key "/tmp/tt_ca_leaf.key"

#define __X509_CRL1 "/tmp/tt_ca.crl"
#else
extern tt_string_t __ca_path;
#define __X509_CA tt_string_cstr(&__ca_path)

extern tt_string_t __ca_key_path;
#define __CA_key tt_string_cstr(&__ca_key_path)

extern tt_string_t __ca2_path;
#define __X509_CA2 tt_string_cstr(&__ca2_path)

extern tt_string_t __ca_key2_path;
#define __CA_key2 tt_string_cstr(&__ca_key2_path)

extern tt_string_t __leaf_path;
#define __X509_LEAF tt_string_cstr(&__leaf_path)

extern tt_string_t __leaf_key_path;
#define __leaf_key tt_string_cstr(&__leaf_key_path)

extern tt_string_t __crl_path;
#define __X509_CRL1 tt_string_cstr(&__crl_path)
#endif

#elif TT_ENV_OS_IS_ANDROID

#define PATH "/data/data/com.titansdk.titansdkunittest/"

#define __X509_CA PATH "tt_ca.crt"
#define __CA_key PATH "tt_ca.key"

#define __X509_CA2 PATH "tt_ca_int.crt"
#define __CA_key2 PATH "tt_ca_int.key"

#define __X509_LEAF PATH "tt_ca_leaf.crt"
#define __leaf_key PATH "tt_ca_leaf.key"

#define __X509_CRL1 PATH "tt_ca.crl"

#else

#define __X509_CA "tt_ca.crt"
#define __CA_key "tt_ca.key"

#define __X509_CA2 "tt_ca_int.crt"
#define __CA_key2 "tt_ca_int.key"

#define __X509_LEAF "tt_ca_leaf.crt"
#define __leaf_key "tt_ca_leaf.key"

#define __X509_CRL1 "tt_ca.crl"

#endif

#if 0
#define __SSL_DETAIL TT_INFO
#else
#define __SSL_DETAIL(...)
#endif

//#define __TCP_DETAIL

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
TT_TEST_ROUTINE_DECLARE(case_ssl_basic)
TT_TEST_ROUTINE_DECLARE(case_ssl_ver)
TT_TEST_ROUTINE_DECLARE(case_ssl_auth)
TT_TEST_ROUTINE_DECLARE(case_ssl_alpn)
TT_TEST_ROUTINE_DECLARE(case_ssl_resume)
TT_TEST_ROUTINE_DECLARE(case_ssl_ticket)
TT_TEST_ROUTINE_DECLARE(case_ssl_multhread)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(ssl_io_case)

#if 1
TT_TEST_CASE("case_ssl_basic",
             "ssl: basic io",
             case_ssl_basic,
             NULL,
             __x509_prepare,
             NULL,
             NULL,
             NULL)
,
#endif

#if 1
    TT_TEST_CASE("case_ssl_ver",
                 "ssl: version",
                 case_ssl_ver,
                 NULL,
                 __x509_prepare,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_ssl_auth",
                 "ssl: authentication",
                 case_ssl_auth,
                 NULL,
                 __x509_prepare,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_ssl_alpn",
                 "ssl: alpn",
                 case_ssl_alpn,
                 NULL,
                 __x509_prepare,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_ssl_resume",
                 "ssl: session resume",
                 case_ssl_resume,
                 NULL,
                 __x509_prepare,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_ssl_ticket",
                 "ssl: session ticket",
                 case_ssl_ticket,
                 NULL,
                 __x509_prepare,
                 NULL,
                 NULL,
                 NULL),

#endif

    TT_TEST_CASE("case_ssl_multhread",
                 "ssl: multi thread",
                 case_ssl_multhread,
                 NULL,
                 __x509_prepare,
                 NULL,
                 NULL,
                 NULL),


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
    TT_TEST_ROUTINE_DEFINE(case_ssl_ver)
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

static tt_u32_t __ut_ev_snd, __ut_ev_rcv;
static tt_s64_t __ut_ssl_max_diff;

static const tt_char_t *__ut_alpn;
static const tt_char_t *__ut_server_name;

static tt_ssl_t *__ut_ssl_connect(const tt_char_t *addr, tt_u16_t port)
{
    tt_skt_t *s;
    tt_ssl_t *ssl;

    s = tt_skt_create(TT_NET_AF_INET, TT_NET_PROTO_TCP, NULL);
    if (s == NULL) {
        __ssl_err_line = __LINE__;
        return NULL;
    }

    if (!TT_OK(tt_skt_connect_p(s, TT_NET_AF_INET, addr, port))) {
        __ssl_err_line = __LINE__;
        tt_skt_destroy(s);
        return NULL;
    }

    ssl = tt_ssl_create(s, &sc_cli_1);
    if (ssl == NULL) {
        __ssl_err_line = __LINE__;
        tt_skt_destroy(s);
        return NULL;
    }

    return ssl;
}

static tt_ssl_t *__ut_ssl_accept(const tt_char_t *addr, tt_u16_t port)
{
    tt_skt_t *s, *new_s;
    tt_ssl_t *ssl;
    tt_fiber_ev_t *p_fev;
    tt_tmr_t *p_tmr;

    s = tt_skt_create(TT_NET_AF_INET, TT_NET_PROTO_TCP, NULL);
    if (s == NULL) {
        __ssl_err_line = __LINE__;
        return NULL;
    }

    tt_skt_set_reuseaddr(s, TT_TRUE);

    if (!TT_OK(tt_skt_bind_p(s, TT_NET_AF_INET, "127.0.0.1", 60606))) {
        __ssl_err_line = __LINE__;
        tt_skt_destroy(s);
        return NULL;
    }

    if (!TT_OK(tt_skt_listen(s))) {
        __ssl_err_line = __LINE__;
        tt_skt_destroy(s);
        return NULL;
    }

    new_s = tt_skt_accept(s, NULL, NULL, &p_fev, &p_tmr);
    if (new_s == NULL) {
        __ssl_err_line = __LINE__;
        tt_skt_destroy(s);
        return NULL;
    }

    ssl = tt_ssl_create(new_s, &sc_svr_1);
    if (ssl == NULL) {
        __ssl_err_line = __LINE__;
        tt_skt_destroy(s);
        tt_skt_destroy(new_s);
        return NULL;
    }

    tt_skt_destroy(s);
    return ssl;
}

#ifdef __UT_LITE__
#define SB_NUM 100
#else
#define SB_NUM (1 << 10)
#endif

static tt_result_t __f_svr(IN void *param)
{
    tt_skt_t *s, *new_s;
    tt_u8_t buf[1 << 14] = "6789";
    tt_u32_t n;
    tt_result_t ret;
    tt_fiber_ev_t *fev;
    tt_tmr_t *tmr, *e_tmr;
    tt_ssl_t *ssl;
    tt_fiber_ev_t *p_fev;
    tt_tmr_t *p_tmr;

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

    new_s = tt_skt_accept(s, NULL, NULL, &p_fev, &p_tmr);
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

    while ((ret = tt_ssl_recv(ssl, buf, sizeof(buf), &n, &fev, &e_tmr)) !=
           TT_E_END) {
        tt_u32_t total = n;
#ifdef __TCP_DETAIL
        if (n < sizeof(buf)) {
            TT_INFO("server recv %d", n);
        }
#endif
        __svr_recvd += n;
        if (fev != NULL) {
            if ((fev->src == NULL) && (fev->ev != 0x87654321)) {
                __ssl_err_line = __LINE__;
            }
            if ((fev->src != NULL) && (fev->ev != 0x12345678)) {
                __ssl_err_line = __LINE__;
            }
            ++__ut_ev_rcv;
            tt_fiber_finish(fev);
        }

        if (e_tmr != NULL) {
            tt_s64_t now = tt_time_ref();
            now -= (tt_s64_t)(tt_uintptr_t)e_tmr->param;
            now = labs((long)now);
            now = tt_time_ref2ms(now);
            if (now > __ut_ssl_max_diff) {
                __ut_ssl_max_diff = now;
            }

            now = tt_rand_u32() % 3;
            if (now == 0) {
                tt_tmr_destroy(e_tmr);
            } else if (now == 1) {
                tt_tmr_stop(e_tmr);
            } else {
                tt_tmr_set_delay(e_tmr, tt_rand_u32() % 5 + 5);
                tt_tmr_set_param(e_tmr, (void *)(tt_uintptr_t)tt_time_ref());
                tt_tmr_start(e_tmr);
            }
        }

        if (tt_rand_u32() % 100 == 0) {
            tmr = tt_tmr_create(tt_rand_u32() % 5 + 5,
                                0,
                                (void *)(tt_uintptr_t)tt_time_ref());
            if (tmr == NULL) {
                __ssl_err_line = __LINE__;
                return TT_FAIL;
            }
            tt_tmr_start(tmr);
        }

        /*
         it has to recv all data, otherwise, data are accumulated in recv
         buffer of new_s. if new_s recv buffer is full, client will be
         blocked in tt_skt_send()
         */
        while (total < sizeof(buf)) {
            if (!TT_OK(tt_ssl_recv(ssl, buf, sizeof(buf), &n, &fev, &e_tmr))) {
                __ssl_err_line = __LINE__;
                return TT_FAIL;
            }
#ifdef __TCP_DETAIL
            if (n < sizeof(buf)) {
                TT_INFO("server recv %d", n);
            }
#endif
            __svr_recvd += n;

            if (fev != NULL) {
                if ((fev->src == NULL) && (fev->ev != 0x87654321)) {
                    __ssl_err_line = __LINE__;
                }
                if ((fev->src != NULL) && (fev->ev != 0x12345678)) {
                    __ssl_err_line = __LINE__;
                }
                ++__ut_ev_rcv;
                tt_fiber_finish(fev);
            }

            if (e_tmr != NULL) {
                tt_s64_t now = tt_time_ref();
                now -= (tt_s64_t)(tt_uintptr_t)e_tmr->param;
                now = labs((long)now);
                now = tt_time_ref2ms(now);
                if (now > __ut_ssl_max_diff) {
                    __ut_ssl_max_diff = now;
                }

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

            if (tt_rand_u32() % 100 == 0) {
                tmr = tt_tmr_create(tt_rand_u32() % 5 + 5,
                                    0,
                                    (void *)(tt_uintptr_t)tt_time_ref());
                if (tmr == NULL) {
                    __ssl_err_line = __LINE__;
                    return TT_FAIL;
                }
                tt_tmr_start(tmr);
            }

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

    // may be some fev and tmr left, receive them
    while (TT_OK(ret = tt_ssl_recv(ssl, buf, sizeof(buf), &n, &fev, &tmr))) {
    }
    if (ret != TT_E_END) {
        __ssl_err_line = __LINE__;
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
    tt_tmr_t *tmr, *e_tmr;
    tt_ssl_t *ssl;
    tt_fiber_t *svr = tt_fiber_find("svr");

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

    if (!TT_OK(tt_ssl_set_hostname(ssl, "child-1"))) {
        __ssl_err_line = __LINE__;
        return TT_FAIL;
    }

    if (!TT_OK(tt_ssl_handshake(ssl, &fev, &tmr))) {
        __ssl_err_line = __LINE__;
        return TT_FAIL;
    }

    loop = 0;
    while (loop++ < SB_NUM) {
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

        if (tt_rand_u32() % 5 == 0) {
            tt_u32_t r = tt_rand_u32() % 2;
            if (r == 0) {
                tt_fiber_ev_t e;
                tt_fiber_ev_init(&e, 0x12345678);
                __SSL_DETAIL("=> cli send ev wait");
                tt_fiber_send_ev(svr, &e, TT_TRUE);
                __SSL_DETAIL("<= cli send ev wait");
            } else {
                tt_fiber_ev_t *e = tt_fiber_ev_create(0x87654321, 0);
                __SSL_DETAIL("=> cli send ev");
                tt_fiber_send_ev(svr, e, TT_FALSE);
                __SSL_DETAIL("<= cli send ev");
            }
            ++__ut_ev_snd;
        }

        total = 0;
        while (total < sizeof(buf)) {
            if (!TT_OK(tt_ssl_recv(ssl, buf, sizeof(buf), &n, &fev, &e_tmr))) {
                __ssl_err_line = __LINE__;
                return TT_FAIL;
            }
#ifdef __TCP_DETAIL
            if (n < sizeof(buf)) {
                TT_INFO("client recv %d", n);
            }
#endif
            __cli_recvd += n;

            if (e_tmr != NULL) {
                tt_s64_t now = tt_time_ref();
                now -= (tt_s64_t)(tt_uintptr_t)e_tmr->param;
                now = labs((long)now);
                now = tt_time_ref2ms(now);
                if (now > __ut_ssl_max_diff) {
                    __ut_ssl_max_diff = now;
                }

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

            if (tt_rand_u32() % 100 == 0) {
                tmr = tt_tmr_create(tt_rand_u32() % 5 + 5,
                                    0,
                                    (void *)(tt_uintptr_t)tt_time_ref());
                if (tmr == NULL) {
                    __ssl_err_line = __LINE__;
                    return TT_FAIL;
                }
                tt_tmr_start(tmr);
            }

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

    while (tt_ssl_recv(ssl, buf, sizeof(buf), &n, &fev, &tmr) != TT_E_END) {
    }
#ifdef __TCP_DETAIL
    TT_INFO("client recv end");
#endif

    // tt_skt_destroy(s);
    tt_ssl_destroy(ssl);

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(case_ssl_basic)
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

    tt_task_add_fiber(&t, "svr", __f_svr, NULL, NULL);
    tt_task_add_fiber(&t, "cli", __f_cli, NULL, NULL);

    __ssl_err_line = 0;
    tt_atomic_s64_set(&__io_num, 0);

    ret = tt_ssl_config_create(&sc_cli_1,
                               TT_SSL_CLIENT,
                               TT_SSL_TRANSPORT_STREAM,
                               TT_SSL_PRESET_DEFAULT);
    TT_UT_SUCCESS(ret, "");
    tt_ssl_config_auth(&sc_cli_1, TT_SSL_AUTH_REQUIRED);

    ret = tt_ssl_config_create(&sc_svr_1,
                               TT_SSL_SERVER,
                               TT_SSL_TRANSPORT_STREAM,
                               TT_SSL_PRESET_DEFAULT);
    TT_UT_SUCCESS(ret, "");
    tt_ssl_config_extended_master_secret(&sc_svr_1, TT_FALSE);

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
    TT_RECORD_INFO(
        "ssl: %f MB/s, cli sent/recv: %d/%d, svr sent/recv: %d/%d, time diff: "
        "%d",
        ((float)tt_atomic_s64_get(&__io_num) / (1 << 20)) * 1000 / dur,
        __cli_sent,
        __cli_recvd,
        __svr_sent,
        __svr_recvd,
        (tt_s32_t)__ut_ssl_max_diff);

    tt_x509cert_destroy(&ca);
    tt_x509crl_destroy(&crl);
    tt_x509cert_destroy(&cert);
    tt_pk_destroy(&pk);
    tt_ssl_config_destroy(&sc_cli_1);
    tt_ssl_config_destroy(&sc_svr_1);

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_result_t __f_svr_fail(IN void *param)
{
    tt_ssl_t *ssl = __ut_ssl_accept("127.0.0.1", 60606);
    tt_fiber_ev_t *p_ev;
    tt_tmr_t *e_tmr;

    if (ssl == NULL) {
        __ssl_err_line = __LINE__;
        return TT_FAIL;
    }

    if (TT_OK(tt_ssl_handshake(ssl, &p_ev, &e_tmr))) {
        __ssl_err_line = __LINE__;
        return TT_FAIL;
    }

    tt_ssl_destroy(ssl);
    return TT_SUCCESS;
}

static tt_result_t __f_svr_ok(IN void *param)
{
    tt_ssl_t *ssl = __ut_ssl_accept("127.0.0.1", 60606);
    tt_fiber_ev_t *p_ev;
    tt_tmr_t *e_tmr;

    if (ssl == NULL) {
        __ssl_err_line = __LINE__;
        return TT_FAIL;
    }

    if (!TT_OK(tt_ssl_handshake(ssl, &p_ev, &e_tmr))) {
        __ssl_err_line = __LINE__;
        return TT_FAIL;
    }

    if (__ut_alpn != NULL) {
        const tt_char_t *a = tt_ssl_get_alpn(ssl);
        if (tt_strcmp(a, __ut_alpn) != 0) {
            __ssl_err_line = __LINE__;
            return TT_FAIL;
        }
    }

    tt_ssl_destroy(ssl);
    return TT_SUCCESS;
}

static tt_result_t __f_cli_fail(IN void *param)
{
    tt_ssl_t *ssl = __ut_ssl_connect("127.0.0.1", 60606);
    tt_fiber_ev_t *p_ev;
    tt_tmr_t *e_tmr;

    if (ssl == NULL) {
        __ssl_err_line = __LINE__;
        return TT_FAIL;
    }

    if (TT_OK(tt_ssl_handshake(ssl, &p_ev, &e_tmr))) {
        __ssl_err_line = __LINE__;
        return TT_FAIL;
    }

    tt_ssl_destroy(ssl);
    return TT_SUCCESS;
}

static tt_result_t __f_cli_ok(IN void *param)
{
    tt_ssl_t *ssl = __ut_ssl_connect("127.0.0.1", 60606);
    tt_fiber_ev_t *p_ev;
    tt_tmr_t *e_tmr;

    if (ssl == NULL) {
        __ssl_err_line = __LINE__;
        return TT_FAIL;
    }

    if (__ut_server_name != NULL) {
        if (!TT_OK(tt_ssl_set_hostname(ssl, __ut_server_name))) {
            __ssl_err_line = __LINE__;
            return TT_FAIL;
        }
    }

    if (!TT_OK(tt_ssl_handshake(ssl, &p_ev, &e_tmr))) {
        __ssl_err_line = __LINE__;
        return TT_FAIL;
    }

    tt_ssl_destroy(ssl);
    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(case_ssl_ver)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    tt_task_t t;
    tt_x509cert_t ca, cert;
    tt_x509crl_t crl;
    tt_pk_t pk;

    TT_TEST_CASE_ENTER()
    // test start

    ret = tt_task_create(&t, NULL);
    TT_UT_SUCCESS(ret, "");

    __ssl_err_line = 0;
    tt_atomic_s64_set(&__io_num, 0);

    ret = tt_ssl_config_create(&sc_cli_1,
                               TT_SSL_CLIENT,
                               TT_SSL_TRANSPORT_STREAM,
                               TT_SSL_PRESET_DEFAULT);
    TT_UT_SUCCESS(ret, "");
    tt_ssl_config_version(&sc_cli_1, TT_TLS_V1_1, TT_TLS_V1_1);

    ret = tt_ssl_config_create(&sc_svr_1,
                               TT_SSL_SERVER,
                               TT_SSL_TRANSPORT_STREAM,
                               TT_SSL_PRESET_DEFAULT);
    TT_UT_SUCCESS(ret, "");
    tt_ssl_config_version(&sc_svr_1, TT_TLS_V1_2, TT_TLS_V1_2);

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

    tt_task_add_fiber(&t, "svr", __f_svr_fail, NULL, NULL);
    tt_task_add_fiber(&t, "cli", __f_cli_fail, NULL, NULL);
    ret = tt_task_run(&t);
    TT_UT_SUCCESS(ret, "");
    tt_task_wait(&t);
    TT_UT_EQUAL(__ssl_err_line, 0, "");

    // same version
    tt_ssl_config_version(&sc_cli_1, TT_TLS_V1_2, TT_TLS_V1_2);
    tt_ssl_config_version(&sc_svr_1, TT_TLS_V1_0, TT_TLS_V1_2);

    ret = tt_task_create(&t, NULL);
    TT_UT_SUCCESS(ret, "");
    tt_task_add_fiber(&t, "svr", __f_svr_ok, NULL, NULL);
    tt_task_add_fiber(&t, "cli", __f_cli_ok, NULL, NULL);
    ret = tt_task_run(&t);
    TT_UT_SUCCESS(ret, "");
    tt_task_wait(&t);
    TT_UT_EQUAL(__ssl_err_line, 0, "");

    tt_x509cert_destroy(&ca);
    tt_x509crl_destroy(&crl);
    tt_x509cert_destroy(&cert);
    tt_pk_destroy(&pk);
    tt_ssl_config_destroy(&sc_cli_1);
    tt_ssl_config_destroy(&sc_svr_1);

    // test end
    TT_TEST_CASE_LEAVE()
}

tt_result_t __ut_on_sni(IN struct tt_ssl_s *ssl,
                        IN const tt_u8_t *sni,
                        IN tt_u32_t len,
                        IN void *param)
{
    if (len != sizeof("child-1") - 1 || tt_memcmp(sni, "child-1", len) != 0) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(case_ssl_auth)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    tt_task_t t;
    tt_x509cert_t ca, ca2, cert, cert2;
    tt_x509crl_t crl;
    tt_pk_t pk, pk2;

    TT_TEST_CASE_ENTER()
    // test start

    ret = tt_task_create(&t, NULL);
    TT_UT_SUCCESS(ret, "");

    __ssl_err_line = 0;
    tt_atomic_s64_set(&__io_num, 0);
    __ut_server_name = "child-1";

    ret = tt_ssl_config_create(&sc_cli_1,
                               TT_SSL_CLIENT,
                               TT_SSL_TRANSPORT_STREAM,
                               TT_SSL_PRESET_DEFAULT);
    TT_UT_SUCCESS(ret, "");
    tt_ssl_config_auth(&sc_cli_1, TT_SSL_AUTH_NONE);

    ret = tt_ssl_config_create(&sc_svr_1,
                               TT_SSL_SERVER,
                               TT_SSL_TRANSPORT_STREAM,
                               TT_SSL_PRESET_DEFAULT);
    TT_UT_SUCCESS(ret, "");
    tt_ssl_config_auth(&sc_svr_1, TT_SSL_AUTH_REQUIRED);

    // server cert
    tt_x509cert_init(&cert);
    ret = tt_x509cert_add_file(&cert, __X509_CA2);
    TT_UT_SUCCESS(ret, "");
    tt_pk_init(&pk);
    ret = tt_pk_load_private_file(&pk, __CA_key2, (tt_u8_t *)"456", 3);
    TT_UT_SUCCESS(ret, "");
    tt_ssl_config_cert(&sc_svr_1, &cert, &pk);

    // server ca
    tt_x509cert_init(&ca2);
    ret = tt_x509cert_add_file(&ca2, __X509_CA);
    TT_UT_SUCCESS(ret, "");
    ret = tt_x509cert_add_file(&ca2, __X509_CA2);
    TT_UT_SUCCESS(ret, "");
    tt_ssl_config_ca(&sc_svr_1, &ca2, NULL);

    // client cert
    tt_x509cert_init(&cert2);
    ret = tt_x509cert_add_file(&cert2, __X509_LEAF);
    TT_UT_SUCCESS(ret, "");
    tt_pk_init(&pk2);
    ret = tt_pk_load_private_file(&pk2, __leaf_key, (tt_u8_t *)"789", 3);
    tt_ssl_config_cert(&sc_cli_1, &cert2, &pk2);

    // client ca
    tt_x509cert_init(&ca);
    ret = tt_x509cert_add_file(&ca, __X509_CA);
    TT_UT_SUCCESS(ret, "");
    tt_x509crl_init(&crl);
    // ret = tt_x509crl_add_file(&crl, __X509_CRL1);
    TT_UT_SUCCESS(ret, "");
    tt_ssl_config_ca(&sc_cli_1, &ca, &crl);

    ret = tt_task_create(&t, NULL);
    TT_UT_SUCCESS(ret, "");
    tt_task_add_fiber(&t, "svr", __f_svr_ok, NULL, NULL);
    tt_task_add_fiber(&t, "cli", __f_cli_ok, NULL, NULL);
    ret = tt_task_run(&t);
    TT_UT_SUCCESS(ret, "");
    tt_task_wait(&t);
    TT_UT_EQUAL(__ssl_err_line, 0, "");

    // sni
    tt_ssl_config_auth(&sc_cli_1, TT_SSL_AUTH_REQUIRED);
    tt_ssl_config_sni(&sc_svr_1, __ut_on_sni, NULL);

    ret = tt_task_create(&t, NULL);
    TT_UT_SUCCESS(ret, "");
    tt_task_add_fiber(&t, "svr", __f_svr_ok, NULL, NULL);
    tt_task_add_fiber(&t, "cli", __f_cli_ok, NULL, NULL);
    ret = tt_task_run(&t);
    TT_UT_SUCCESS(ret, "");
    tt_task_wait(&t);
    TT_UT_EQUAL(__ssl_err_line, 0, "");

    tt_x509cert_destroy(&ca);
    tt_x509cert_destroy(&ca2);
    tt_x509crl_destroy(&crl);
    tt_x509cert_destroy(&cert);
    tt_x509cert_destroy(&cert2);
    tt_pk_destroy(&pk);
    tt_pk_destroy(&pk2);
    tt_ssl_config_destroy(&sc_cli_1);
    tt_ssl_config_destroy(&sc_svr_1);

    __ut_server_name = NULL;

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_ssl_alpn)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    tt_task_t t;
    tt_x509cert_t ca, cert;
    tt_x509crl_t crl;
    tt_pk_t pk;
    const tt_char_t *cli_alpn[] = {"h1", "h2", "h3", NULL};
    const tt_char_t *svr_alpn1[] = {"h4", "h2", "h1", NULL};
    const tt_char_t *svr_alpn2[] = {"x", NULL};

    TT_TEST_CASE_ENTER()
    // test start

    ret = tt_task_create(&t, NULL);
    TT_UT_SUCCESS(ret, "");

    __ssl_err_line = 0;
    tt_atomic_s64_set(&__io_num, 0);
    __ut_alpn = "h2";

    ret = tt_ssl_config_create(&sc_cli_1,
                               TT_SSL_CLIENT,
                               TT_SSL_TRANSPORT_STREAM,
                               TT_SSL_PRESET_DEFAULT);
    TT_UT_SUCCESS(ret, "");
    tt_ssl_config_alpn(&sc_cli_1, cli_alpn);

    ret = tt_ssl_config_create(&sc_svr_1,
                               TT_SSL_SERVER,
                               TT_SSL_TRANSPORT_STREAM,
                               TT_SSL_PRESET_DEFAULT);
    TT_UT_SUCCESS(ret, "");
    tt_ssl_config_alpn(&sc_svr_1, svr_alpn2);

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

    tt_task_add_fiber(&t, "svr", __f_svr_fail, NULL, NULL);
    tt_task_add_fiber(&t, "cli", __f_cli_fail, NULL, NULL);
    ret = tt_task_run(&t);
    TT_UT_SUCCESS(ret, "");
    tt_task_wait(&t);
    TT_UT_EQUAL(__ssl_err_line, 0, "");

    // same version
    tt_ssl_config_alpn(&sc_svr_1, svr_alpn1);

    ret = tt_task_create(&t, NULL);
    TT_UT_SUCCESS(ret, "");
    tt_task_add_fiber(&t, "svr", __f_svr_ok, NULL, NULL);
    tt_task_add_fiber(&t, "cli", __f_cli_ok, NULL, NULL);
    ret = tt_task_run(&t);
    TT_UT_SUCCESS(ret, "");
    tt_task_wait(&t);
    TT_UT_EQUAL(__ssl_err_line, 0, "");

    tt_x509cert_destroy(&ca);
    tt_x509crl_destroy(&crl);
    tt_x509cert_destroy(&cert);
    tt_pk_destroy(&pk);
    tt_ssl_config_destroy(&sc_cli_1);
    tt_ssl_config_destroy(&sc_svr_1);

    __ut_alpn = NULL;

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_result_t __f_svr_acc(IN void *param)
{
    tt_skt_t *new_s = (tt_skt_t *)param;
    tt_u8_t buf[1 << 10] = "6789";
    tt_ssl_t *ssl;
    tt_result_t ret;
    tt_fiber_ev_t *fev;
    tt_tmr_t *tmr, *e_tmr;
    tt_u32_t n;

    ssl = tt_ssl_create(new_s, &sc_svr_1);
    if (ssl == NULL) {
        __ssl_err_line = __LINE__;
        return TT_FAIL;
    }

    if (!TT_OK(tt_ssl_handshake(ssl, &fev, &tmr))) {
        __ssl_err_line = __LINE__;
        return TT_FAIL;
    }

    while ((ret = tt_ssl_recv(ssl, buf, sizeof(buf), &n, &fev, &e_tmr)) !=
           TT_E_END) {
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
            if (!TT_OK(tt_ssl_recv(ssl, buf, sizeof(buf), &n, &fev, &e_tmr))) {
                __ssl_err_line = __LINE__;
                return TT_FAIL;
            }
#ifdef __TCP_DETAIL
            if (n < sizeof(buf)) {
                TT_INFO("server recv %d", n);
            }
#endif

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

    // may be some fev and tmr left, receive them
    while (TT_OK(ret = tt_ssl_recv(ssl, buf, sizeof(buf), &n, &fev, &tmr))) {
    }
    if (ret != TT_E_END) {
        __ssl_err_line = __LINE__;
    }

#ifdef __TCP_DETAIL
    TT_INFO("server recv end");
#endif

    // tt_skt_destroy(new_s);
    tt_ssl_destroy(ssl);
    return TT_SUCCESS;
}

static tt_result_t __f_svr_mul(IN void *param)
{
    tt_skt_t *s, *new_s;
    tt_u32_t n = (tt_u32_t)(tt_uintptr_t)param;
    tt_fiber_t *f;
    tt_fiber_ev_t *p_fev;
    tt_tmr_t *p_tmr;

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

    while (n-- != 0) {
        new_s = tt_skt_accept(s, NULL, NULL, &p_fev, &p_tmr);
        if (new_s == NULL) {
            __ssl_err_line = __LINE__;
            return TT_FAIL;
        }

        f = tt_fiber_create(NULL, __f_svr_acc, new_s, NULL);
        if (f == NULL) {
            __ssl_err_line = __LINE__;
            return TT_FAIL;
        }
        tt_fiber_resume(f, TT_FALSE);
    }

    tt_skt_destroy(s);
    return TT_SUCCESS;
}

#ifdef __UT_LITE__
#define CM_NUM 10
#else
#define CM_NUM 100
#endif

static tt_result_t __f_cli_mul(IN void *param)
{
    tt_u8_t buf[1 << 10] = "123";
    tt_u32_t n, loop, num = (tt_u32_t)(tt_uintptr_t)param;
    tt_fiber_ev_t *fev;
    tt_tmr_t *tmr, *e_tmr;
    tt_ssl_t *ssl;
    tt_fiber_t *svr = tt_fiber_find("svr");

    while (num-- != 0) {
        ssl = __ut_ssl_connect("127.0.0.1", 60606);
        if (ssl == NULL) {
            __ssl_err_line = __LINE__;
            return TT_FAIL;
        }

        if (!TT_OK(tt_ssl_handshake(ssl, &fev, &tmr))) {
            __ssl_err_line = __LINE__;
            return TT_FAIL;
        }

        loop = 0;
        while (loop++ < CM_NUM) {
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
                if (!TT_OK(
                        tt_ssl_recv(ssl, buf, sizeof(buf), &n, &fev, &e_tmr))) {
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

        while (tt_ssl_recv(ssl, buf, sizeof(buf), &n, &fev, &tmr) != TT_E_END) {
        }
#ifdef __TCP_DETAIL
        TT_INFO("client recv end");
#endif

        // tt_skt_destroy(s);
        tt_ssl_destroy(ssl);
    }

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(case_ssl_resume)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    tt_task_t t;
    tt_x509cert_t ca, cert;
    tt_x509crl_t crl;
    tt_pk_t pk;
    tt_u32_t i;

    TT_TEST_CASE_ENTER()
    // test start

    ret = tt_task_create(&t, NULL);
    TT_UT_SUCCESS(ret, "");

    __ssl_err_line = 0;

    ret = tt_ssl_config_create(&sc_cli_1,
                               TT_SSL_CLIENT,
                               TT_SSL_TRANSPORT_STREAM,
                               TT_SSL_PRESET_DEFAULT);
    TT_UT_SUCCESS(ret, "");
    tt_ssl_config_cache(&sc_cli_1, TT_FALSE, NULL);

    ret = tt_ssl_config_create(&sc_svr_1,
                               TT_SSL_SERVER,
                               TT_SSL_TRANSPORT_STREAM,
                               TT_SSL_PRESET_DEFAULT);
    TT_UT_SUCCESS(ret, "");
    tt_ssl_config_encrypt_then_mac(&sc_svr_1, TT_FALSE);

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

    tt_task_add_fiber(&t, "svr", __f_svr_mul, (void *)15, NULL);
    for (i = 0; i < 5; ++i) {
        tt_task_add_fiber(&t, NULL, __f_cli_mul, (void *)3, NULL);
    }
    ret = tt_task_run(&t);
    TT_UT_SUCCESS(ret, "");
    tt_task_wait(&t);
    TT_UT_EQUAL(__ssl_err_line, 0, "");

    // enable server session resume
    tt_ssl_config_cache(&sc_svr_1, TT_FALSE, NULL);

    ret = tt_task_create(&t, NULL);
    TT_UT_SUCCESS(ret, "");
    tt_task_add_fiber(&t, "svr", __f_svr_mul, (void *)15, NULL);
    for (i = 0; i < 5; ++i) {
        tt_task_add_fiber(&t, NULL, __f_cli_mul, (void *)3, NULL);
    }
    ret = tt_task_run(&t);
    TT_UT_SUCCESS(ret, "");
    tt_task_wait(&t);
    TT_UT_EQUAL(__ssl_err_line, 0, "");

    tt_x509cert_destroy(&ca);
    tt_x509crl_destroy(&crl);
    tt_x509cert_destroy(&cert);
    tt_pk_destroy(&pk);
    tt_ssl_config_destroy(&sc_cli_1);
    tt_ssl_config_destroy(&sc_svr_1);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_ssl_ticket)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    tt_task_t t;
    tt_x509cert_t ca, cert;
    tt_x509crl_t crl;
    tt_pk_t pk;
    tt_u32_t i;

    TT_TEST_CASE_ENTER()
    // test start

    ret = tt_task_create(&t, NULL);
    TT_UT_SUCCESS(ret, "");

    __ssl_err_line = 0;
    tt_atomic_s64_set(&__io_num, 0);

    ret = tt_ssl_config_create(&sc_cli_1,
                               TT_SSL_CLIENT,
                               TT_SSL_TRANSPORT_STREAM,
                               TT_SSL_PRESET_DEFAULT);
    TT_UT_SUCCESS(ret, "");
    tt_ssl_config_cache(&sc_cli_1, TT_TRUE, NULL);

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

    tt_task_add_fiber(&t, "svr", __f_svr_mul, (void *)15, NULL);
    for (i = 0; i < 5; ++i) {
        tt_task_add_fiber(&t, NULL, __f_cli_mul, (void *)3, NULL);
    }
    ret = tt_task_run(&t);
    TT_UT_SUCCESS(ret, "");
    tt_task_wait(&t);
    TT_UT_EQUAL(__ssl_err_line, 0, "");

    // enable client session ticket
    tt_ssl_config_cache(&sc_svr_1, TT_TRUE, NULL);

    ret = tt_task_create(&t, NULL);
    TT_UT_SUCCESS(ret, "");
    tt_task_add_fiber(&t, "svr", __f_svr_mul, (void *)15, NULL);
    for (i = 0; i < 5; ++i) {
        tt_task_add_fiber(&t, NULL, __f_cli_mul, (void *)3, NULL);
    }
    ret = tt_task_run(&t);
    TT_UT_SUCCESS(ret, "");
    tt_task_wait(&t);
    TT_UT_EQUAL(__ssl_err_line, 0, "");

    tt_x509cert_destroy(&ca);
    tt_x509crl_destroy(&crl);
    tt_x509cert_destroy(&cert);
    tt_pk_destroy(&pk);
    tt_ssl_config_destroy(&sc_cli_1);
    tt_ssl_config_destroy(&sc_svr_1);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_ssl_multhread)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    tt_task_t t[10];
    tt_x509cert_t ca, cert;
    tt_x509crl_t crl;
    tt_pk_t pk;
    tt_u32_t i, k;

    TT_TEST_CASE_ENTER()
    // test start

    __ssl_err_line = 0;

    ret = tt_ssl_config_create(&sc_cli_1,
                               TT_SSL_CLIENT,
                               TT_SSL_TRANSPORT_STREAM,
                               TT_SSL_PRESET_DEFAULT);
    TT_UT_SUCCESS(ret, "");
    tt_ssl_config_cache(&sc_cli_1, TT_TRUE, NULL);

    ret = tt_ssl_config_create(&sc_svr_1,
                               TT_SSL_SERVER,
                               TT_SSL_TRANSPORT_STREAM,
                               TT_SSL_PRESET_DEFAULT);
    TT_UT_SUCCESS(ret, "");
    tt_ssl_config_cache(&sc_svr_1, TT_TRUE, NULL);

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

    for (k = 0; k < sizeof(t) / sizeof(t[0]); ++k) {
        ret = tt_task_create(&t[k], NULL);
        TT_UT_SUCCESS(ret, "");

        tt_task_add_fiber(&t[k], "svr", __f_svr_mul, (void *)15, NULL);
        for (i = 0; i < 5; ++i) {
            tt_task_add_fiber(&t[k], NULL, __f_cli_mul, (void *)3, NULL);
        }
        ret = tt_task_run(&t[k]);
        TT_UT_SUCCESS(ret, "");
        tt_task_wait(&t[k]);
    }
    TT_UT_EQUAL(__ssl_err_line, 0, "");

    tt_x509cert_destroy(&ca);
    tt_x509crl_destroy(&crl);
    tt_x509cert_destroy(&cert);
    tt_pk_destroy(&pk);
    tt_ssl_config_destroy(&sc_cli_1);
    tt_ssl_config_destroy(&sc_svr_1);

    // test end
    TT_TEST_CASE_LEAVE()
}
