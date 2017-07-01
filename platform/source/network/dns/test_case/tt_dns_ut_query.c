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

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

// === routine declarations ================
TT_TEST_ROUTINE_DECLARE(tt_unit_test_dns_query_basic)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_dns_query_u2t)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(dns_query_case)

#if 0
TT_TEST_CASE("tt_unit_test_dns_query_basic",
             "dns query",
             tt_unit_test_dns_query_basic,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,
#endif

TT_TEST_CASE("tt_unit_test_dns_query_u2t",
             "dns query fail over",
             tt_unit_test_dns_query_u2t,
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
    const tt_char_t *name;
    tt_u16_t port;
    on_recv_t on_recv;
    tt_u32_t ignore_num;
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

    s = tt_udp_server_p(TT_NET_AF_INET, NULL, sp->name, sp->port);
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

        if (++i < sp->ignore_num) {
            continue;
        }

        ret = sp->on_recv(s, buf, recvd, &addr);
        if (ret != TT_PROCEEDING) {
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

static tt_result_t __tcp_svr1(IN void *param)
{
    tt_skt_t *s, *new_s;
    tt_result_t ret;
    tt_u8_t buf[2048];
    struct tt_fiber_ev_s *fev;
    struct tt_tmr_s *tmr;
    tt_u32_t recvd;
    __svr_param_t *sp = (__svr_param_t *)param;
    tt_u32_t i;

    s = tt_tcp_server_p(TT_NET_AF_INET, NULL, sp->name, sp->port);
    if (s == NULL) {
        __dns_errline = __LINE__;
        return TT_FAIL;
    }

    new_s = tt_skt_accept(s, NULL, NULL);
    DUT_INFO("accept new tcp skt");
    tt_skt_destroy(s);
    if (s == NULL) {
        __dns_errline = __LINE__;
        return TT_FAIL;
    }

    i = 0;
    while (1) {
        ret = tt_skt_recv(new_s, buf, sizeof(buf), &recvd, &fev, &tmr);
        if (!TT_OK(ret)) {
            __dns_errline = __LINE__;
            return TT_FAIL;
        }

        if (++i < sp->ignore_num) {
            continue;
        }

        ret = sp->on_recv(new_s, buf, recvd, NULL);
        if (ret != TT_PROCEEDING) {
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

    if (len >= 2) {
        tt_u16_t n = (buf[0] << 8) + buf[1];
        n += sizeof(ans);
        buf[0] = (n >> 8);
        buf[1] = n & 0xFF;
    }
    
    if (len >= 5) {
        buf[4] |= 0x80; // response
    }
    if (len >= 10) {
        buf[9] |= 1; // 1 answer
    }
    tt_memcpy(&buf[len], ans, sizeof(ans));

    return tt_skt_send(s, buf, len + sizeof(ans), NULL);
}

static tt_result_t __dns_query_1(IN void *param)
{
    tt_sktaddr_ip_t ip;
    tt_result_t ret;

    ret = tt_dns_query4(tt_current_dns(), "163.com", &ip);
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
    __svr_param_t sp[2] = {0};

    TT_TEST_CASE_ENTER()
    // test start

    __dns_errline = 0;
    __udp_tc = TT_FALSE;

    tt_task_attr_default(&attr);
    attr.enable_dns = TT_TRUE;
    attr.dns_attr.server = svr;
    attr.dns_attr.server_num = sizeof(svr) / sizeof(svr[0]);

    ret = tt_task_create(&t, &attr);
    TT_UT_SUCCESS(ret, "");

    sp[0].name = "127.0.0.1";
    sp[0].port = 43210;
    sp[0].on_recv = NULL;
    sp[0].ignore_num = ~0;
    tt_task_add_fiber(&t, NULL, __udp_svr1, &sp[0], NULL);
    sp[1].name = "127.0.0.1";
    sp[1].port = 43211;
    sp[1].on_recv = __udp_answer;
    sp[1].ignore_num = 2;
    tt_task_add_fiber(&t, NULL, __udp_svr1, &sp[1], NULL);
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

    // from tcp
    ret = tt_dns_query4(tt_current_dns(), "163.com", &ip);
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

    // from udp
    ret = tt_dns_query4(tt_current_dns(), "163.com", &ip);
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

    tt_task_exit(NULL);
    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_dns_query_u2t)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_task_t t;
    tt_result_t ret;
    tt_task_attr_t attr;
    const tt_char_t *svr[] = {
        "127.0.0.1:43210", "127.0.0.1:43211",
    };
    __svr_param_t sp[3] = {0};

    TT_TEST_CASE_ENTER()
    // test start

    __dns_errline = 0;
    __udp_tc = TT_TRUE;

    tt_task_attr_default(&attr);
    attr.enable_dns = TT_TRUE;
    attr.dns_attr.server = svr;
    attr.dns_attr.server_num = sizeof(svr) / sizeof(svr[0]);

    ret = tt_task_create(&t, &attr);
    TT_UT_SUCCESS(ret, "");

    sp[0].name = "127.0.0.1";
    sp[0].port = 43210;
    sp[0].on_recv = NULL;
    sp[0].ignore_num = ~0;
    tt_task_add_fiber(&t, NULL, __udp_svr1, &sp[0], NULL);
    sp[1].name = "127.0.0.1";
    sp[1].port = 43211;
    sp[1].on_recv = __udp_answer;
    sp[1].ignore_num = 2;
    tt_task_add_fiber(&t, NULL, __udp_svr1, &sp[1], NULL);
    sp[2].name = "127.0.0.1";
    sp[2].port = 43211;
    sp[2].on_recv = __tcp_answer;
    sp[2].ignore_num = 2;
    tt_task_add_fiber(&t, NULL, __tcp_svr1, &sp[2], NULL);
    tt_task_add_fiber(&t, NULL, __dns_query_2, NULL, NULL);

    ret = tt_task_run(&t);
    TT_UT_SUCCESS(ret, "");
    tt_task_wait(&t);

    TT_UT_EQUAL(__dns_errline, 0, "");

    // test end
    TT_TEST_CASE_LEAVE()
}
