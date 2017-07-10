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

#define DUT_INFO TT_INFO

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

extern tt_result_t __ut_dns_query4(IN tt_dns_t d,
                                   IN const tt_char_t *name,
                                   OUT tt_sktaddr_ip_t *ip);

extern tt_result_t __ut_dns_query6(IN tt_dns_t d,
                                   IN const tt_char_t *name,
                                   OUT tt_sktaddr_ip_t *ip);

extern tt_dns_t __ut_current_dns_d();

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

// === routine declarations ================
TT_TEST_ROUTINE_DECLARE(tt_unit_test_dns_query_basic)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_dns_query_u2t)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_dns_query_timeout)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_dns_query_exception)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_dns_query4_first6)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_dns_query6_first4)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(dns_query_case)

#if 1
TT_TEST_CASE("tt_unit_test_dns_query_basic",
             "dns query",
             tt_unit_test_dns_query_basic,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_dns_query_u2t",
                 "dns query fail over",
                 tt_unit_test_dns_query_u2t,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_dns_query_timeout",
                 "dns query time out",
                 tt_unit_test_dns_query_timeout,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_dns_query_exception",
                 "dns query exceptional case",
                 tt_unit_test_dns_query_exception,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),
#endif

    TT_TEST_CASE("tt_unit_test_dns_query4_first6",
                 "dns query ip but ipv6 first",
                 tt_unit_test_dns_query4_first6,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_dns_query6_first4",
                 "dns query ipv6 but ip first",
                 tt_unit_test_dns_query6_first4,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(dns_query_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(DNS_UT_QUERY, 0, dns_query_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(tt_unit_test_dns_query_basic)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    static tt_u32_t __dns_errline;

typedef tt_result_t (*on_recv_t)(IN tt_skt_t *s,
                                 IN tt_u8_t *buf,
                                 IN tt_u32_t len,
                                 IN tt_sktaddr_t *addr);

typedef struct __svr_param_s
{
    tt_skt_t *s;
    tt_net_family_t af;
    const tt_char_t *name;
    tt_u16_t port;
    on_recv_t on_recv;
    tt_u32_t ignore_num;
    tt_u32_t recv_num;
    tt_u32_t acc_num;
} __svr_param_t;

static tt_result_t __udp_svr1(IN void *param)
{
    tt_skt_t *s;
    tt_result_t ret;
    tt_u8_t buf[2048];
    struct tt_fiber_ev_s *fev;
    struct tt_tmr_s *tmr;
    tt_u32_t recvd;
    __svr_param_t *sp = (__svr_param_t *)param;
    tt_sktaddr_t addr;
    tt_u32_t i;

    s = tt_udp_server_p(sp->af, NULL, sp->name, sp->port);
    if (s == NULL) {
        __dns_errline = __LINE__;
        return TT_FAIL;
    }

    i = 0;
    while (1) {
        ret = tt_skt_recvfrom(s, buf, sizeof(buf), &recvd, &addr, &fev, &tmr);
        if (!TT_OK(ret)) {
            __dns_errline = __LINE__;
            return TT_FAIL;
        }

        if (fev != NULL) {
            tt_fiber_finish(fev);
            break;
        }

        ++i;
        if (i <= sp->ignore_num) {
            continue;
        }

        ret = sp->on_recv(s, buf, recvd, &addr);
        if (!TT_OK(ret)) {
            break;
        }

        if ((sp->recv_num != 0) && (i >= sp->recv_num)) {
            break;
        }
    }
    if (!TT_OK(ret)) {
        __dns_errline = __LINE__;
        return TT_FAIL;
    }

    tt_skt_destroy(s);

    DUT_INFO("udp svr1 exit");
    return TT_SUCCESS;
}

static tt_result_t __tcp_acc1(IN void *param)
{
    __svr_param_t sp = *(__svr_param_t *)param;
    tt_skt_t *new_s = sp.s;
    tt_result_t ret;
    tt_u8_t buf[2048];
    struct tt_fiber_ev_s *fev;
    struct tt_tmr_s *tmr;
    tt_u32_t recvd;
    tt_u32_t i;

    tt_free(param);
    i = 0;
    while (1) {
        ret = tt_skt_recv(new_s, buf, sizeof(buf), &recvd, &fev, &tmr);
        if (!TT_OK(ret)) {
            __dns_errline = __LINE__;
            return TT_FAIL;
        }

        ++i;
        if (i <= sp.ignore_num) {
            continue;
        }

        ret = sp.on_recv(new_s, buf, recvd, NULL);
        if (!TT_OK(ret)) {
            break;
        }

        if ((sp.recv_num != 0) && (i >= sp.recv_num)) {
            break;
        }
    }
    if (!TT_OK(ret)) {
        __dns_errline = __LINE__;
        return TT_FAIL;
    }

    tt_skt_shutdown(new_s, TT_SKT_SHUT_WR);
    while (TT_OK(tt_skt_recv(new_s, buf, sizeof(buf), &recvd, &fev, &tmr))) {
    }
    tt_skt_destroy(new_s);

    return TT_SUCCESS;
}

static tt_result_t __tcp_svr1(IN void *param)
{
    tt_skt_t *s, *new_s;
    __svr_param_t *sp = (__svr_param_t *)param;
    tt_fiber_t *fb;
    tt_u32_t i;

    s = tt_tcp_server_p(sp->af, NULL, sp->name, sp->port);
    if (s == NULL) {
        __dns_errline = __LINE__;
        return TT_FAIL;
    }

    i = 0;
    while (1) {
        __svr_param_t *sp2;

        new_s = tt_skt_accept(s, NULL, NULL);
        DUT_INFO("accept new tcp skt");
        if (s == NULL) {
            __dns_errline = __LINE__;
            return TT_FAIL;
        }

        sp2 = tt_malloc(sizeof(__svr_param_t));
        *sp2 = *sp;
        sp2->s = new_s;

        fb = tt_fiber_create(NULL, __tcp_acc1, sp2, NULL);
        tt_fiber_resume(fb, TT_FALSE);

        ++i;
        if (i >= sp->acc_num) {
            break;
        }
    }

    tt_skt_destroy(s);
    return TT_SUCCESS;
}

static tt_bool_t __udp_tc;

static tt_result_t __udp_answer(IN tt_skt_t *s,
                                IN tt_u8_t *buf,
                                IN tt_u32_t len,
                                IN tt_sktaddr_t *addr)
{
    const tt_u8_t ans[] = {3, '1', '6', '3', 3, 'c', 'o', 'm', 0, 0, 1, 0,
                           1, 1,   2,   3,   4, 0,   4,   7,   8, 9, 10};

    if (len >= 3) {
        buf[2] |= 0x80; // response
    }
    if (__udp_tc) {
        buf[2] |= 0x2; // truncated
    }
    if (len >= 8) {
        buf[7] |= 1; // 1 answer
    }
    tt_memcpy(&buf[len], ans, sizeof(ans));

    return tt_skt_sendto(s, buf, len + sizeof(ans), NULL, addr);
}

static tt_result_t __tcp_answer(IN tt_skt_t *s,
                                IN tt_u8_t *buf,
                                IN tt_u32_t len,
                                IN tt_sktaddr_t *addr)
{
    const tt_u8_t ans[] = {3, '1', '6', '3', 3, 'c', 'o', 'm', 0, 0, 1, 0,
                           1, 1,   2,   3,   4, 0,   4,   7,   8, 9, 10};
    tt_u32_t msglen;

    if (len >= 2) {
        tt_u16_t n = (buf[0] << 8) + buf[1];
        msglen = 2 + n;
        n += sizeof(ans);
        buf[0] = (n >> 8);
        buf[1] = n & 0xFF;
    } else {
        msglen = 0;
    }

    if (len >= 5) {
        buf[4] |= 0x80; // response
    }
    if (len >= 10) {
        buf[9] |= 1; // 1 answer
    }
    tt_memcpy(&buf[msglen], ans, sizeof(ans));

    return tt_skt_send(s, buf, msglen + sizeof(ans), NULL);
}

static tt_result_t __dns_query_1(IN void *param)
{
    tt_sktaddr_ip_t ip;
    tt_result_t ret;
    tt_fiber_t *fb;

    ret = __ut_dns_query4(__ut_current_dns_d(), "163.com", &ip);
    if (!TT_OK(ret)) {
        __dns_errline = __LINE__;
        tt_task_exit(NULL);
        return TT_FAIL;
    }
    if (ip.a32.__u32 != 0x0a090807) {
        __dns_errline = __LINE__;
        tt_task_exit(NULL);
        return TT_FAIL;
    }

    fb = tt_fiber_find("udp1");
    if (fb != NULL) {
        tt_fiber_ev_t *fev = tt_fiber_ev_create(0, 0);
        tt_fiber_send_ev(fb, fev, TT_FALSE);
    }

    fb = tt_fiber_find("udp2");
    if (fb != NULL) {
        tt_fiber_ev_t *fev = tt_fiber_ev_create(0, 0);
        tt_fiber_send_ev(fb, fev, TT_FALSE);
    }

    tt_task_exit(NULL);
    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_dns_query_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_task_t t;
    tt_result_t ret;
    tt_task_attr_t attr;
    const tt_char_t *svr[] = {
        "127.0.0.1:43210", "127.0.0.1:43211",
    };
    __svr_param_t sp[2];

    TT_TEST_CASE_ENTER()
    // test start

    tt_memset(sp, 0, sizeof(sp));

    __dns_errline = 0;
    __udp_tc = TT_FALSE;

    tt_task_attr_default(&attr);
    attr.enable_dns = TT_TRUE;
    attr.dns_cache_attr.dns_attr.server = svr;
    attr.dns_cache_attr.dns_attr.server_num = sizeof(svr) / sizeof(svr[0]);
    attr.dns_cache_attr.dns_attr.timeout_ms = 1000;
    attr.dns_cache_attr.dns_attr.try_num = 5;
    attr.dns_cache_attr.dns_attr.send_buf_size = 777;
    attr.dns_cache_attr.dns_attr.recv_buf_size = 888;

    ret = tt_task_create(&t, &attr);
    TT_UT_SUCCESS(ret, "");

    sp[0].af = TT_NET_AF_INET;
    sp[0].name = "127.0.0.1";
    sp[0].port = 43210;
    sp[0].on_recv = NULL;
    sp[0].ignore_num = ~0;
    tt_task_add_fiber(&t, "udp1", __udp_svr1, &sp[0], NULL);
    sp[1].af = TT_NET_AF_INET;
    sp[1].name = "127.0.0.1";
    sp[1].port = 43211;
    sp[1].on_recv = __udp_answer;
    sp[1].ignore_num = 2;
    tt_task_add_fiber(&t, "udp2", __udp_svr1, &sp[1], NULL);
    tt_task_add_fiber(&t, NULL, __dns_query_1, NULL, NULL);

    ret = tt_task_run(&t);
    TT_UT_SUCCESS(ret, "");
    tt_task_wait(&t);

    TT_UT_EQUAL(__dns_errline, 0, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_result_t __dns_query_2(IN void *param)
{
    tt_sktaddr_ip_t ip;
    tt_result_t ret;
    tt_fiber_t *fb;

    // from tcp
    ret = __ut_dns_query4(__ut_current_dns_d(), "163.com", &ip);
    DUT_INFO("dns query 1 done");
    if (!TT_OK(ret)) {
        __dns_errline = __LINE__;
        tt_task_exit(NULL);
        return TT_FAIL;
    }
    if (ip.a32.__u32 != 0x0a090807) {
        __dns_errline = __LINE__;
        tt_task_exit(NULL);
        return TT_FAIL;
    }

    // from tcp again
    ret = __ut_dns_query4(__ut_current_dns_d(), "163.com", &ip);
    DUT_INFO("dns query 2 done");
    if (!TT_OK(ret)) {
        __dns_errline = __LINE__;
        tt_task_exit(NULL);
        return TT_FAIL;
    }
    if (ip.a32.__u32 != 0x0a090807) {
        __dns_errline = __LINE__;
        tt_task_exit(NULL);
        return TT_FAIL;
    }

    // from udp
    __udp_tc = TT_FALSE;
    ret = __ut_dns_query4(__ut_current_dns_d(), "163.com", &ip);
    DUT_INFO("dns query 2 done");
    if (!TT_OK(ret)) {
        __dns_errline = __LINE__;
        tt_task_exit(NULL);
        return TT_FAIL;
    }
    if (ip.a32.__u32 != 0x0a090807) {
        __dns_errline = __LINE__;
        tt_task_exit(NULL);
        return TT_FAIL;
    }

    fb = tt_fiber_find("udp1");
    if (fb != NULL) {
        tt_fiber_ev_t *fev = tt_fiber_ev_create(0, 0);
        tt_fiber_send_ev(fb, fev, TT_FALSE);
    }

    fb = tt_fiber_find("udp2");
    if (fb != NULL) {
        tt_fiber_ev_t *fev = tt_fiber_ev_create(0, 0);
        tt_fiber_send_ev(fb, fev, TT_FALSE);
    }

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_dns_query_u2t)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_task_t t;
    tt_result_t ret;
    tt_task_attr_t attr;
    const tt_char_t *svr[] = {
        "[::1]:43210", "[::1]:43211",
    };
    __svr_param_t sp[3];

    TT_TEST_CASE_ENTER()
    // test start

    tt_memset(sp, 0, sizeof(sp));

    __dns_errline = 0;
    __udp_tc = TT_TRUE;

    tt_task_attr_default(&attr);
    attr.enable_dns = TT_TRUE;
    attr.dns_cache_attr.dns_attr.server = svr;
    attr.dns_cache_attr.dns_attr.server_num = sizeof(svr) / sizeof(svr[0]);
    attr.dns_cache_attr.dns_attr.timeout_ms = 1000;
    attr.dns_cache_attr.dns_attr.try_num = 5;
    attr.dns_cache_attr.dns_attr.local_ip4 = "127.0.0.1";
    attr.dns_cache_attr.dns_attr.local_ip6 = "::1";

    ret = tt_task_create(&t, &attr);
    TT_UT_SUCCESS(ret, "");

    sp[0].af = TT_NET_AF_INET6;
    sp[0].name = "::1";
    sp[0].port = 43210;
    sp[0].on_recv = NULL;
    sp[0].ignore_num = ~0;
    sp[0].recv_num = ~0;
    tt_task_add_fiber(&t, "udp1", __udp_svr1, &sp[0], NULL);
    sp[1].af = TT_NET_AF_INET6;
    sp[1].name = "::1";
    sp[1].port = 43211;
    sp[1].on_recv = __udp_answer;
    sp[1].ignore_num = 1;
    sp[1].recv_num = ~0;
    tt_task_add_fiber(&t, "udp2", __udp_svr1, &sp[1], NULL);
    sp[2].af = TT_NET_AF_INET6;
    sp[2].name = "::1";
    sp[2].port = 43211;
    sp[2].on_recv = __tcp_answer;
    sp[2].recv_num = 1;
    sp[2].acc_num = 2;
    tt_task_add_fiber(&t, NULL, __tcp_svr1, &sp[2], NULL);
    tt_task_add_fiber(&t, NULL, __dns_query_2, NULL, NULL);

    ret = tt_task_run(&t);
    TT_UT_SUCCESS(ret, "");
    tt_task_wait(&t);

    TT_UT_EQUAL(__dns_errline, 0, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_result_t __dns_query_3(IN void *param)
{
    tt_sktaddr_ip_t ip;
    tt_result_t ret;
    tt_u32_t i, n = 3; // tt_rand_u32() % 10 + 5;
    tt_fiber_t *fb;

    for (i = 0; i < n; ++i) {
        ret = __ut_dns_query4(__ut_current_dns_d(), "163.com", &ip);
        DUT_INFO("dns query[%d/%d] done", i, n);
        if (ret != TT_TIME_OUT) {
            __dns_errline = __LINE__;
            tt_task_exit(NULL);
            return TT_FAIL;
        }
    }

    fb = tt_fiber_find("udp1");
    if (fb != NULL) {
        tt_fiber_ev_t *fev = tt_fiber_ev_create(0, 0);
        tt_fiber_send_ev(fb, fev, TT_FALSE);
    }

    fb = tt_fiber_find("udp2");
    if (fb != NULL) {
        tt_fiber_ev_t *fev = tt_fiber_ev_create(0, 0);
        tt_fiber_send_ev(fb, fev, TT_FALSE);
    }

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_dns_query_timeout)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_task_t t;
    tt_result_t ret;
    tt_task_attr_t attr;
    const tt_char_t *svr[] = {
        "127.0.0.1:43210", "127.0.0.1:43211",
    };
    __svr_param_t sp[3];

    TT_TEST_CASE_ENTER()
    // test start

    tt_memset(sp, 0, sizeof(sp));

    __dns_errline = 0;
    __udp_tc = TT_TRUE;

    tt_task_attr_default(&attr);
    attr.enable_dns = TT_TRUE;
    attr.dns_cache_attr.dns_attr.server = svr;
    attr.dns_cache_attr.dns_attr.server_num = sizeof(svr) / sizeof(svr[0]);
    attr.dns_cache_attr.dns_attr.timeout_ms = 1000;
    attr.dns_cache_attr.dns_attr.try_num = 3;

    ret = tt_task_create(&t, &attr);
    TT_UT_SUCCESS(ret, "");

    sp[0].af = TT_NET_AF_INET;
    sp[0].name = "127.0.0.1";
    sp[0].port = 43210;
    sp[0].on_recv = NULL;
    sp[0].ignore_num = ~0;
    sp[0].recv_num = ~0;
    tt_task_add_fiber(&t, "udp1", __udp_svr1, &sp[0], NULL);
    sp[1].af = TT_NET_AF_INET;
    sp[1].name = "127.0.0.1";
    sp[1].port = 43211;
    sp[1].on_recv = __udp_answer;
    sp[1].ignore_num = ~0;
    sp[1].recv_num = ~0;
    tt_task_add_fiber(&t, "udp2", __udp_svr1, &sp[1], NULL);
    tt_task_add_fiber(&t, NULL, __dns_query_3, NULL, NULL);

    ret = tt_task_run(&t);
    TT_UT_SUCCESS(ret, "");
    tt_task_wait(&t);

    TT_UT_EQUAL(__dns_errline, 0, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_result_t __udp_svr_rand(IN void *param)
{
    tt_skt_t *s;
    tt_result_t ret;
    tt_u8_t buf[2048];
    struct tt_fiber_ev_s *fev;
    struct tt_tmr_s *tmr;
    tt_u32_t recvd;
    __svr_param_t *sp = (__svr_param_t *)param;
    tt_sktaddr_t addr;
    tt_u32_t i;

    s = tt_udp_server_p(sp->af, NULL, sp->name, sp->port);
    if (s == NULL) {
        __dns_errline = __LINE__;
        return TT_FAIL;
    }

    i = 0;
    while (1) {
        ret = tt_skt_recvfrom(s, buf, sizeof(buf), &recvd, &addr, &fev, &tmr);
        if (!TT_OK(ret)) {
            __dns_errline = __LINE__;
            return TT_FAIL;
        }

        if (fev != NULL) {
            tt_fiber_finish(fev);
            break;
        }

        if (tt_rand_u32() % 3 == 0) {
            // ignored
            continue;
        }

        i = tt_rand_u32() % 4;
        if (i == 0) {
            DUT_INFO("udp truncated");
            __udp_tc = TT_TRUE;
            ret = __udp_answer(s, buf, recvd, &addr);
        } else if (i == 1) {
            DUT_INFO("udp answer");
            __udp_tc = TT_FALSE;
            ret = __udp_answer(s, buf, recvd, &addr);
        } else if (i == 2) {
            tt_u32_t n = tt_rand_u32() % 500 + 1, k;
            DUT_INFO("udp short rand");
            for (k = 0; k < n; ++k) {
                buf[k] = (tt_u8_t)tt_rand_u32();
            }
            ret = tt_skt_sendto(s, buf, n, NULL, &addr);
        } else {
            tt_u32_t n = tt_rand_u32() % 1500 + 1, k;
            DUT_INFO("udp long rand");
            for (k = 0; k < n; ++k) {
                buf[k] = (tt_u8_t)tt_rand_u32();
            }
            ret = tt_skt_sendto(s, buf, n, NULL, &addr);
        }
        if (!TT_OK(ret)) {
            break;
        }
    }
    if (!TT_OK(ret)) {
        __dns_errline = __LINE__;
        return TT_FAIL;
    }

    tt_skt_destroy(s);

    return TT_SUCCESS;
}

static tt_result_t __tcp_answer_rand(IN tt_skt_t *s,
                                     IN tt_u8_t *buf,
                                     IN tt_u32_t len,
                                     IN tt_sktaddr_t *addr)
{
    const tt_u8_t ans[] = {3, '1', '6', '3', 3, 'c', 'o', 'm', 0, 0, 1, 0,
                           1, 1,   2,   3,   4, 0,   4,   7,   8, 9, 10};
    tt_u32_t msglen, m;

    m = tt_rand_u32() % 4;
    if (m == 0) {
        tt_u32_t n = tt_rand_u32() % len + 1, k;
        DUT_INFO("tcp short rand");
        for (k = 0; k < n; ++k) {
            buf[k] = (tt_u8_t)tt_rand_u32();
        }
        return tt_skt_send(s, buf, n, NULL);
    } else if (m == 1) {
        tt_u32_t n = tt_rand_u32() % 1000 + 1, k;
        DUT_INFO("tcp long rand");
        for (k = 0; k < n; ++k) {
            buf[k] = (tt_u8_t)tt_rand_u32();
        }
        return tt_skt_send(s, buf, n, NULL);
    } else if (m == 2) {
        // force tcp disconnect
        DUT_INFO("tcp disconnect");
        return TT_FAIL;
    }

    if (len >= 2) {
        tt_u16_t n = (buf[0] << 8) + buf[1];
        msglen = 2 + n;
        n += sizeof(ans);
        buf[0] = (n >> 8);
        buf[1] = n & 0xFF;
    } else {
        msglen = 0;
    }

    if (len >= 5) {
        buf[4] |= 0x80; // response
    }
    if (len >= 10) {
        buf[9] |= 1; // 1 answer
    }
    tt_memcpy(&buf[msglen], ans, sizeof(ans));

    return tt_skt_send(s, buf, msglen + sizeof(ans), NULL);
}

static tt_result_t __dns_query_4(IN void *param)
{
    tt_sktaddr_ip_t ip;
    tt_result_t ret;
    tt_u32_t i, n = 20; // tt_rand_u32() % 10 + 5;
    tt_fiber_t *fb;

    for (i = 0; i < n; ++i) {
        ret = __ut_dns_query4(__ut_current_dns_d(), "163.com", &ip);
        DUT_INFO("dns query[%d/%d]: %s", i, n, TT_OK(ret) ? "ok" : "fail");
    }

    fb = tt_fiber_find("udp1");
    if (fb != NULL) {
        tt_fiber_ev_t *fev = tt_fiber_ev_create(0, 0);
        tt_fiber_send_ev(fb, fev, TT_FALSE);
    }

    fb = tt_fiber_find("udp2");
    if (fb != NULL) {
        tt_fiber_ev_t *fev = tt_fiber_ev_create(0, 0);
        tt_fiber_send_ev(fb, fev, TT_FALSE);
    }

    tt_task_exit(NULL);
    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_dns_query_exception)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_task_t t;
    tt_result_t ret;
    tt_task_attr_t attr;
    const tt_char_t *svr[] = {
        "127.0.0.1:53210", "[::1]:53211",
    };
    __svr_param_t sp[4];

    TT_TEST_CASE_ENTER()
    // test start

    tt_memset(sp, 0, sizeof(sp));

    __dns_errline = 0;

    tt_task_attr_default(&attr);
    attr.enable_dns = TT_TRUE;
    attr.dns_cache_attr.dns_attr.server = svr;
    attr.dns_cache_attr.dns_attr.server_num = sizeof(svr) / sizeof(svr[0]);
    attr.dns_cache_attr.dns_attr.timeout_ms = 1000;
    attr.dns_cache_attr.dns_attr.try_num = 5;

    ret = tt_task_create(&t, &attr);
    TT_UT_SUCCESS(ret, "");

    sp[0].af = TT_NET_AF_INET;
    sp[0].name = "127.0.0.1";
    sp[0].port = 53210;
    tt_task_add_fiber(&t, "udp1", __udp_svr_rand, &sp[0], NULL);
    sp[1].af = TT_NET_AF_INET;
    sp[1].name = "127.0.0.1";
    sp[1].port = 53210;
    sp[1].on_recv = __tcp_answer_rand;
    sp[1].recv_num = 1; // this tcp server only accept 1 query
    sp[1].acc_num = ~0;
    tt_task_add_fiber(&t, "tcp1", __tcp_svr1, &sp[1], NULL);
    sp[2].af = TT_NET_AF_INET6;
    sp[2].name = "::1";
    sp[2].port = 53211;
    tt_task_add_fiber(&t, "udp2", __udp_svr_rand, &sp[2], NULL);
    sp[3].af = TT_NET_AF_INET6;
    sp[3].name = "::1";
    sp[3].port = 53211;
    sp[3].on_recv = __tcp_answer_rand;
    sp[3].recv_num = 0; // always open
    sp[3].acc_num = ~0;
    tt_task_add_fiber(&t, "tcp2", __tcp_svr1, &sp[3], NULL);

    tt_task_add_fiber(&t, NULL, __dns_query_4, NULL, NULL);

    ret = tt_task_run(&t);
    TT_UT_SUCCESS(ret, "");
    tt_task_wait(&t);

    // TT_UT_EQUAL(__dns_errline, 0, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_result_t __dns_query_46(IN void *param)
{
    tt_sktaddr_ip_t ip;
    tt_result_t ret;
    tt_fiber_t *fb;

    ret = __ut_dns_query4(__ut_current_dns_d(), "163.com", &ip);
    if (TT_OK(ret)) {
        // no ip, only ipv6
        __dns_errline = __LINE__;
        tt_task_exit(NULL);
        return TT_FAIL;
    }

    ret = __ut_dns_query4(__ut_current_dns_d(), "163.com", &ip);
    if (!TT_OK(ret)) {
        __dns_errline = __LINE__;
        tt_task_exit(NULL);
        return TT_FAIL;
    }
    if (ip.a32.__u32 != 0x0a090807) {
        __dns_errline = __LINE__;
        tt_task_exit(NULL);
        return TT_FAIL;
    }

    fb = tt_fiber_find("udp1");
    if (fb != NULL) {
        tt_fiber_ev_t *fev = tt_fiber_ev_create(0, 0);
        tt_fiber_send_ev(fb, fev, TT_FALSE);
    }

    return TT_SUCCESS;
}

static tt_result_t __udp_answer46(IN tt_skt_t *s,
                                  IN tt_u8_t *buf,
                                  IN tt_u32_t len,
                                  IN tt_sktaddr_t *addr)
{
    static int n = 0;

    if (n++ == 0) {
        // ipv6
        const tt_u8_t ans[] = {3,   '1',  '6', '3', 3,   'c', 'o', 'm', 0,
                               0,   0x1c, 0,   1,   1,   2,   3,   4,   0,
                               4,   0x1,  0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
                               0x9, 0xa,  0xb, 0xc, 0xd, 0xe, 0xf, 0x0};

        if (len >= 3) {
            buf[2] |= 0x80; // response
        }
        if (len >= 8) {
            buf[7] |= 1; // 1 answer
        }
        tt_memcpy(&buf[len], ans, sizeof(ans));

        return tt_skt_sendto(s, buf, len + sizeof(ans), NULL, addr);
    } else {
        const tt_u8_t ans[] = {3, '1', '6', '3', 3, 'c', 'o', 'm', 0, 0, 1, 0,
                               1, 1,   2,   3,   4, 0,   4,   7,   8, 9, 10};
        // ipv4
        if (len >= 3) {
            buf[2] |= 0x80; // response
        }
        if (len >= 8) {
            buf[7] |= 1; // 1 answer
        }
        tt_memcpy(&buf[len], ans, sizeof(ans));

        return tt_skt_sendto(s, buf, len + sizeof(ans), NULL, addr);
    }
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_dns_query4_first6)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_task_t t;
    tt_result_t ret;
    tt_task_attr_t attr;
    const tt_char_t *svr[] = {
        "127.0.0.1:43210",
    };
    __svr_param_t sp;

    TT_TEST_CASE_ENTER()
    // test start

    tt_memset(&sp, 0, sizeof(sp));

    __dns_errline = 0;
    __udp_tc = TT_FALSE;

    tt_task_attr_default(&attr);
    attr.enable_dns = TT_TRUE;
    attr.dns_cache_attr.dns_attr.server = svr;
    attr.dns_cache_attr.dns_attr.server_num = sizeof(svr) / sizeof(svr[0]);
    attr.dns_cache_attr.dns_attr.timeout_ms = 1000;
    attr.dns_cache_attr.dns_attr.try_num = 5;

    ret = tt_task_create(&t, &attr);
    TT_UT_SUCCESS(ret, "");

    sp.af = TT_NET_AF_INET;
    sp.name = "127.0.0.1";
    sp.port = 43210;
    sp.on_recv = __udp_answer46;
    sp.ignore_num = 0;
    tt_task_add_fiber(&t, "udp1", __udp_svr1, &sp, NULL);
    tt_task_add_fiber(&t, NULL, __dns_query_46, NULL, NULL);

    ret = tt_task_run(&t);
    TT_UT_SUCCESS(ret, "");
    tt_task_wait(&t);

    TT_UT_EQUAL(__dns_errline, 0, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_result_t __dns_query_64(IN void *param)
{
    tt_sktaddr_ip_t ip;
    tt_result_t ret;
    tt_fiber_t *fb;

    ret = __ut_dns_query6(__ut_current_dns_d(), "163.com", &ip);
    if (TT_OK(ret)) {
        // no ipv6, only ipv4
        __dns_errline = __LINE__;
        tt_task_exit(NULL);
        return TT_FAIL;
    }

    ret = __ut_dns_query6(__ut_current_dns_d(), "163.com", &ip);
    if (!TT_OK(ret)) {
        __dns_errline = __LINE__;
        tt_task_exit(NULL);
        return TT_FAIL;
    }

    fb = tt_fiber_find("udp1");
    if (fb != NULL) {
        tt_fiber_ev_t *fev = tt_fiber_ev_create(0, 0);
        tt_fiber_send_ev(fb, fev, TT_FALSE);
    }

    return TT_SUCCESS;
}

static tt_result_t __udp_answer64(IN tt_skt_t *s,
                                  IN tt_u8_t *buf,
                                  IN tt_u32_t len,
                                  IN tt_sktaddr_t *addr)
{
    static int n = 0;

    if (n++ == 0) {
        const tt_u8_t ans[] = {3, '1', '6', '3', 3, 'c', 'o', 'm', 0, 0, 1, 0,
                               1, 1,   2,   3,   4, 0,   4,   7,   8, 9, 10};
        // ipv4
        if (len >= 3) {
            buf[2] |= 0x80; // response
        }
        if (len >= 8) {
            buf[7] |= 1; // 1 answer
        }
        tt_memcpy(&buf[len], ans, sizeof(ans));

        return tt_skt_sendto(s, buf, len + sizeof(ans), NULL, addr);
    } else {
        // ipv6
        const tt_u8_t ans[] = {3,   '1',  '6', '3', 3,   'c', 'o', 'm', 0,
                               0,   0x1c, 0,   1,   1,   2,   3,   4,   0,
                               16,  0x1,  0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
                               0x9, 0xa,  0xb, 0xc, 0xd, 0xe, 0xf, 0x0};

        if (len >= 3) {
            buf[2] |= 0x80; // response
        }
        if (len >= 8) {
            buf[7] |= 1; // 1 answer
        }
        tt_memcpy(&buf[len], ans, sizeof(ans));

        return tt_skt_sendto(s, buf, len + sizeof(ans), NULL, addr);
    }
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_dns_query6_first4)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_task_t t;
    tt_result_t ret;
    tt_task_attr_t attr;
    const tt_char_t *svr[] = {
        "127.0.0.1:43210",
    };
    __svr_param_t sp;

    TT_TEST_CASE_ENTER()
    // test start

    tt_memset(&sp, 0, sizeof(sp));

    __dns_errline = 0;

    tt_task_attr_default(&attr);
    attr.enable_dns = TT_TRUE;
    attr.dns_cache_attr.dns_attr.server = svr;
    attr.dns_cache_attr.dns_attr.server_num = sizeof(svr) / sizeof(svr[0]);
    attr.dns_cache_attr.dns_attr.timeout_ms = 1000;
    attr.dns_cache_attr.dns_attr.try_num = 5;

    ret = tt_task_create(&t, &attr);
    TT_UT_SUCCESS(ret, "");

    sp.af = TT_NET_AF_INET;
    sp.name = "127.0.0.1";
    sp.port = 43210;
    sp.on_recv = __udp_answer64;
    sp.ignore_num = 0;
    tt_task_add_fiber(&t, "udp1", __udp_svr1, &sp, NULL);
    tt_task_add_fiber(&t, NULL, __dns_query_64, NULL, NULL);

    ret = tt_task_run(&t);
    TT_UT_SUCCESS(ret, "");
    tt_task_wait(&t);

    TT_UT_EQUAL(__dns_errline, 0, "");

    // test end
    TT_TEST_CASE_LEAVE()
}
