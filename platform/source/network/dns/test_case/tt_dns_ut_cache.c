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

#include <stdlib.h>

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

extern tt_dns_entry_t *__de_get(IN tt_dns_cache_t *dc,
                                IN const tt_char_t *name,
                                IN tt_bool_t create);

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

// === routine declarations ================
TT_TEST_ROUTINE_DECLARE(tt_unit_test_dc_basic)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_dc_get_a)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_dc_get_aaaa)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(dns_cache_case)

TT_TEST_CASE("tt_unit_test_dc_basic",
             "dns cache basic",
             tt_unit_test_dc_basic,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_dc_get_a",
                 "dns cache get a record",
                 tt_unit_test_dc_get_a,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_dc_get_aaaa",
                 "dns cache get aaaa record",
                 tt_unit_test_dc_get_aaaa,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(dns_cache_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(DNS_UT_CACHE, 0, dns_cache_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(tt_unit_test_dc_basic)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(tt_unit_test_dc_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_dns_cache_t *dc;
    tt_result_t ret;
    tt_task_attr_t attr;
    const tt_char_t *svr[] = {
        "127.0.0.1:43210",
    };
    tt_dns_entry_t *de, *de2, *p;
    tt_s64_t now, ttl_ms, ms;
    tt_bool_t b_ret;

    TT_TEST_CASE_ENTER()
    // test start

    tt_task_attr_default(&attr);

    attr.dns_cache_attr.dns_attr.server = svr;
    attr.dns_cache_attr.dns_attr.server_num = sizeof(svr) / sizeof(svr[0]);
    attr.dns_cache_attr.dns_attr.timeout_ms = 1000;
    attr.dns_cache_attr.dns_attr.try_num = 3;

    dc = tt_dns_cache_create(&attr.dns_cache_attr);
    TT_UT_NOT_NULL(dc, "");

    // empty cache, wait infinite
    ms = tt_dns_cache_run(dc);
    TT_UT_EQUAL(ms, TT_TIME_INFINITE, "");

    de = __de_get(dc, "123", TT_FALSE);
    TT_UT_NULL(de, "");
    de = __de_get(dc, "123", TT_TRUE);
    TT_UT_NOT_NULL(de, "");
    de2 = __de_get(dc, "123", TT_FALSE);
    TT_UT_NOT_NULL(de, "");
    TT_UT_EQUAL(de, de2, "");
    de2 = __de_get(dc, "123", TT_TRUE);
    TT_UT_NOT_NULL(de, "");
    TT_UT_EQUAL(de, de2, "");
    TT_UT_EQUAL(tt_hmap_count(&dc->map), 1, "");
    TT_UT_EQUAL(tt_ptrheap_count(&dc->heap), 1, "");

    de2 = __de_get(dc, "1234", TT_FALSE);
    TT_UT_NULL(de2, "");
    de2 = __de_get(dc, "1234", TT_TRUE);
    TT_UT_NOT_NULL(de2, "");
    TT_UT_EQUAL(tt_hmap_count(&dc->map), 2, "");
    TT_UT_EQUAL(tt_ptrheap_count(&dc->heap), 2, "");

    now = tt_time_ref();

    // emtpy entry
    ttl_ms = 123;
    b_ret = tt_dns_entry_run(de, now, &ttl_ms);
    TT_UT_EQUAL(b_ret, TT_FALSE, "");
    TT_UT_EQUAL(ttl_ms, TT_TIME_INFINITE, "");

    tt_dns_entry_update_ttl(de, now + tt_time_ms2ref(1000));
    TT_UT_EQUAL(de->ttl, now + tt_time_ms2ref(1000), "");
    tt_dns_entry_update_ttl(de2, now + tt_time_ms2ref(500));
    TT_UT_EQUAL(de2->ttl, now + tt_time_ms2ref(500), "");
    p = tt_ptrheap_head(&dc->heap);
    TT_UT_EQUAL(p, de2, "");

    tt_dns_entry_update_ttl(de, now + tt_time_ms2ref(200));
    TT_UT_EQUAL(de->ttl, now + tt_time_ms2ref(200), "");
    p = tt_ptrheap_head(&dc->heap);
    TT_UT_EQUAL(p, de, "");

    ttl_ms = 123;
    b_ret = tt_dns_entry_run(de, now, &ttl_ms);
    TT_UT_EQUAL(b_ret, TT_FALSE, "");
    TT_UT_EQUAL(ttl_ms, 200, "");

    ttl_ms = 123;
    b_ret = tt_dns_entry_run(de2, now, &ttl_ms);
    TT_UT_EQUAL(b_ret, TT_FALSE, "");
    TT_UT_EQUAL(ttl_ms, 500, "");

    // cache would say wait 200ms
    ms = tt_dns_cache_run(dc);
    TT_UT_EXP(abs((int)ms - 200) < 20, "");

    tt_sleep(100);
    ms = tt_dns_cache_run(dc);
    TT_UT_EXP(abs((int)ms - 100) < 20, "");

    tt_sleep(100);
    ms = tt_dns_cache_run(dc);
    // de1 expired, de2 has 300ms left
    TT_UT_EXP(abs((int)ms - 300) < 20, "");
    now = tt_time_ref();

    ttl_ms = 123;
    b_ret = tt_dns_entry_run(de, now, &ttl_ms);
    TT_UT_EQUAL(b_ret, TT_FALSE, "");
    TT_UT_EQUAL(ttl_ms, TT_TIME_INFINITE, "");

    ttl_ms = 123;
    b_ret = tt_dns_entry_run(de2, now, &ttl_ms);
    TT_UT_EQUAL(b_ret, TT_FALSE, "");
    TT_UT_EXP(abs((int)ttl_ms - 300) < 20, "");

    tt_sleep(400);
    // both de and de2 expired now
    ms = tt_dns_cache_run(dc);
    TT_UT_EQUAL(ms, TT_TIME_INFINITE, "");

    tt_dns_cache_destroy(dc);

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_u32_t __dns_errline;
static tt_bool_t __udp_tc;
static tt_bool_t __ttl_0;

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

static tt_result_t __udp_answer(IN tt_skt_t *s,
                                IN tt_u8_t *buf,
                                IN tt_u32_t len,
                                IN tt_sktaddr_t *addr)
{
    const tt_u8_t ans[] = {3, '1', '6', '3', 3, 'c', 'o', 'm', 0, 0, 1, 0,
                           1, 1,   2,   3,   4, 0,   4,   7,   8, 9, 10};
    const tt_u8_t ans_ttl0[] = {3, '1', '6', '3', 3, 'c', 'o', 'm', 0, 0, 1, 0,
                                1, 0,   0,   0,   0, 0,   4,   7,   8, 9, 10};

    if (len >= 3) {
        buf[2] |= 0x80; // response
    }
    if (__udp_tc) {
        buf[2] |= 0x2; // truncated
    }
    if (len >= 8) {
        buf[7] |= 1; // 1 answer
    }
    if (__ttl_0) {
        tt_memcpy(&buf[len], ans_ttl0, sizeof(ans_ttl0));
    } else {
        tt_memcpy(&buf[len], ans, sizeof(ans));
    }

    DUT_INFO("udp answer");
    return tt_skt_sendto(s, buf, len + sizeof(ans), NULL, addr);
}

static tt_result_t __dc_get1(IN void *param)
{
    tt_dns_rrlist_t *rrl;
    tt_dns_a_t *a;

    __ttl_0 = TT_TRUE;
    rrl = tt_dns_get_a("163.com");
    if (tt_dns_rrlist_count(rrl) != 1) {
        __dns_errline = __LINE__;
        tt_task_exit(NULL);
        return TT_FAIL;
    }
    a = tt_dns_a_head(rrl);
    if (a->ip.a32.__u32 != 0xa090807) {
        __dns_errline = __LINE__;
        tt_task_exit(NULL);
        return TT_FAIL;
    }
    __ttl_0 = TT_FALSE;

    // again
    rrl = tt_dns_get_a("163.com");
    if (tt_dns_rrlist_count(rrl) != 1) {
        __dns_errline = __LINE__;
        tt_task_exit(NULL);
        return TT_FAIL;
    }
    a = tt_dns_a_head(rrl);
    if (a->ip.a32.__u32 != 0xa090807) {
        __dns_errline = __LINE__;
        tt_task_exit(NULL);
        return TT_FAIL;
    }

    // invalid query
    rrl = tt_dns_get_a("www.163.com");
    if (tt_dns_rrlist_count(rrl) != 0) {
        __dns_errline = __LINE__;
        tt_task_exit(NULL);
        return TT_FAIL;
    }
    // invalid query again
    rrl = tt_dns_get_a("www.163.com");
    if (tt_dns_rrlist_count(rrl) != 0) {
        __dns_errline = __LINE__;
        tt_task_exit(NULL);
        return TT_FAIL;
    }

    // terminate udp server
    {
        tt_fiber_ev_t *fev = tt_fiber_ev_create(0, 0);
        tt_fiber_send_ev(tt_fiber_find("udp1"), fev, TT_FALSE);
    }

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_dc_get_a)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_dns_cache_t *dc;
    tt_result_t ret;
    tt_task_attr_t attr;
    const tt_char_t *svr[] = {
        "127.0.0.1:43210",
    };
    tt_task_t t;
    __svr_param_t sp[1];

    TT_TEST_CASE_ENTER()
    // test start

    __dns_errline = 0;

    tt_task_attr_default(&attr);

    attr.enable_dns = TT_TRUE;
    attr.dns_cache_attr.dns_attr.server = svr;
    attr.dns_cache_attr.dns_attr.server_num = sizeof(svr) / sizeof(svr[0]);
    attr.dns_cache_attr.dns_attr.timeout_ms = 1000;
    attr.dns_cache_attr.dns_attr.try_num = 3;

    dc = tt_dns_cache_create(&attr.dns_cache_attr);
    TT_UT_NOT_NULL(dc, "");
    tt_dns_cache_destroy(dc);

    ret = tt_task_create(&t, &attr);
    TT_UT_SUCCESS(ret, "");

    tt_memset(&sp[0], 0, sizeof(__svr_param_t));
    sp[0].af = TT_NET_AF_INET;
    sp[0].name = "127.0.0.1";
    sp[0].port = 43210;
    sp[0].on_recv = __udp_answer;
    sp[0].ignore_num = 0;
    sp[0].recv_num = 0;
    tt_task_add_fiber(&t, "udp1", __udp_svr1, &sp[0], NULL);

    tt_task_add_fiber(&t, "dc", __dc_get1, dc, NULL);

    ret = tt_task_run(&t);
    TT_UT_SUCCESS(ret, "");
    tt_task_wait(&t);

    TT_UT_EQUAL(__dns_errline, 0, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_result_t __dc_get2(IN void *param)
{
    tt_dns_rrlist_t *rrl;
    tt_dns_aaaa_t *a;

    // first would succeed
    rrl = tt_dns_get_aaaa("163.com");
    if (tt_dns_rrlist_count(rrl) != 1) {
        __dns_errline = __LINE__;
        tt_task_exit(NULL);
        return TT_FAIL;
    }
    a = tt_dns_aaaa_head(rrl);
    if ((a->ip.a128.__u8[0] != 1) || (a->ip.a128.__u8[14] != 0xf)) {
        __dns_errline = __LINE__;
        tt_task_exit(NULL);
        return TT_FAIL;
    }

    // second from cache
    rrl = tt_dns_get_aaaa("163.com");
    if (tt_dns_rrlist_count(rrl) != 1) {
        __dns_errline = __LINE__;
        tt_task_exit(NULL);
        return TT_FAIL;
    }
    a = tt_dns_aaaa_head(rrl);
    if ((a->ip.a128.__u8[0] != 1) || (a->ip.a128.__u8[14] != 0xf)) {
        __dns_errline = __LINE__;
        tt_task_exit(NULL);
        return TT_FAIL;
    }

    // invalid query
    rrl = tt_dns_get_a("www.163.com");
    if (tt_dns_rrlist_count(rrl) != 0) {
        __dns_errline = __LINE__;
        tt_task_exit(NULL);
        return TT_FAIL;
    }
    // invalid query again
    rrl = tt_dns_get_a("www.163.com");
    if (tt_dns_rrlist_count(rrl) != 0) {
        __dns_errline = __LINE__;
        tt_task_exit(NULL);
        return TT_FAIL;
    }

    // terminate udp server
    {
        tt_fiber_ev_t *fev = tt_fiber_ev_create(0, 0);
        tt_fiber_send_ev(tt_fiber_find("udp1"), fev, TT_FALSE);
    }

    return TT_SUCCESS;
}

static tt_result_t __udp_answer6(IN tt_skt_t *s,
                                 IN tt_u8_t *buf,
                                 IN tt_u32_t len,
                                 IN tt_sktaddr_t *addr)
{
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

TT_TEST_ROUTINE_DEFINE(tt_unit_test_dc_get_aaaa)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_dns_cache_t *dc;
    tt_result_t ret;
    tt_task_attr_t attr;
    const tt_char_t *svr[] = {"[::1]:43210", "[::1]:43211"};
    tt_task_t t;
    __svr_param_t sp[2];

    TT_TEST_CASE_ENTER()
    // test start

    __dns_errline = 0;

    tt_task_attr_default(&attr);

    attr.enable_dns = TT_TRUE;
    attr.dns_cache_attr.dns_attr.server = svr;
    attr.dns_cache_attr.dns_attr.server_num = sizeof(svr) / sizeof(svr[0]);
    attr.dns_cache_attr.dns_attr.timeout_ms = 1000;
    attr.dns_cache_attr.dns_attr.try_num = 3;

    ret = tt_task_create(&t, &attr);
    TT_UT_SUCCESS(ret, "");

    tt_memset(&sp[0], 0, sizeof(__svr_param_t));
    sp[0].af = TT_NET_AF_INET6;
    sp[0].name = "::1";
    sp[0].port = 43210;
    sp[0].on_recv = __udp_answer6;
    sp[0].ignore_num = ~0; // all ignore
    sp[0].recv_num = 0;
    tt_task_add_fiber(&t, "udp1", __udp_svr1, &sp[0], NULL);

    tt_memset(&sp[1], 0, sizeof(__svr_param_t));
    sp[1].af = TT_NET_AF_INET6;
    sp[1].name = "::1";
    sp[1].port = 43211;
    sp[1].on_recv = __udp_answer6;
    sp[1].ignore_num = 0;
    sp[1].recv_num = 1;
    tt_task_add_fiber(&t, "udp2", __udp_svr1, &sp[1], NULL);

    tt_task_add_fiber(&t, "dc2", __dc_get2, NULL, NULL);

    ret = tt_task_run(&t);
    TT_UT_SUCCESS(ret, "");
    tt_task_wait(&t);

    TT_UT_EQUAL(__dns_errline, 0, "");

    // test end
    TT_TEST_CASE_LEAVE()
}
