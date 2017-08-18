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

#define __INUSE_LIMIT 3600000 // see tt_dns_cache.c

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

extern tt_dns_entry_t *__de_get(IN tt_dns_cache_t *dc,
                                IN const tt_char_t *name,
                                IN tt_bool_t create);

extern void __dc_check_inuse(IN tt_dns_cache_t *dc, IN tt_s64_t now);

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

// === routine declarations ================
TT_TEST_ROUTINE_DECLARE(case_dc_basic)
TT_TEST_ROUTINE_DECLARE(case_dc_get_a)
TT_TEST_ROUTINE_DECLARE(case_dc_get_aaaa)
TT_TEST_ROUTINE_DECLARE(case_dc_exception)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(dns_cache_case)

TT_TEST_CASE("case_dc_basic",
             "dns cache basic",
             case_dc_basic,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

#if 1
    TT_TEST_CASE("case_dc_get_a",
                 "dns cache get a record",
                 case_dc_get_a,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_dc_get_aaaa",
                 "dns cache get aaaa record",
                 case_dc_get_aaaa,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_dc_exception",
                 "dns cache get exceptional case",
                 case_dc_exception,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),
#endif

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
    TT_TEST_ROUTINE_DEFINE(case_dc_basic)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(case_dc_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_dns_cache_t *dc;
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
    TT_UT_EXP(abs((int)ms - __INUSE_LIMIT) < 100, "");

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

    now = tt_time_ref();
    TT_UT_EQUAL(tt_dns_entry_inuse(de2, now, tt_time_ms2ref(10000)),
                TT_TRUE,
                "");
    TT_UT_EQUAL(tt_dns_rr_inuse(&de2->rr[TT_DNS_A_IN]), TT_FALSE, "");
    TT_UT_EQUAL(tt_dns_rr_inuse(&de2->rr[TT_DNS_AAAA_IN]), TT_FALSE, "");
    {
        // hacking
        tt_dns_rr_t *drr = &de2->rr[TT_DNS_AAAA_IN];
        tt_dnode_t node;

        TT_UT_NULL(drr->querying_fb, "");
        drr->querying_fb = (tt_fiber_t *)drr;
        TT_UT_EQUAL(tt_dns_rr_inuse(drr), TT_TRUE, "");
        TT_UT_EQUAL(tt_dns_entry_inuse(de2, now, tt_time_ms2ref(10000)),
                    TT_TRUE,
                    "");
        TT_UT_EQUAL(tt_dns_entry_inuse(de2, now, 1), TT_TRUE, "");
        drr->querying_fb = NULL;

        tt_dnode_init(&node);
        tt_dlist_push_head(&drr->waiting, &node);
        TT_UT_EQUAL(tt_dns_rr_inuse(drr), TT_TRUE, "");
        tt_dlist_pop_head(&drr->waiting);

        TT_UT_EQUAL(tt_dns_rr_inuse(drr), TT_FALSE, "");
        now = tt_time_ref();
        TT_UT_EQUAL(tt_dns_entry_inuse(de2, now, tt_time_ms2ref(10000)),
                    TT_TRUE,
                    "");
        {
            tt_s64_t bak = de2->timestamp;
            de2->timestamp = now - 100;
            TT_UT_EQUAL(tt_dns_entry_inuse(de2, now, 1), TT_FALSE, "");
            de2->timestamp = bak;
        }
    }

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
    TT_UT_EXP(abs((int)ms - 200) < 100, "");

    tt_sleep(100);
    ms = tt_dns_cache_run(dc);
    TT_UT_EXP(abs((int)ms - 100) < 100, "");

    tt_sleep(150);
    ms = tt_dns_cache_run(dc);
    // de1 expired, de2 has 250ms left
    TT_INFO("ms: %d", ms);
    TT_UT_EXP(abs((int)ms - 250) < 100, "");
    now = tt_time_ref();

    ttl_ms = 123;
    b_ret = tt_dns_entry_run(de, now, &ttl_ms);
    TT_UT_EQUAL(b_ret, TT_FALSE, "");
    TT_UT_EQUAL(ttl_ms, TT_TIME_INFINITE, "");

    ttl_ms = 123;
    b_ret = tt_dns_entry_run(de2, now, &ttl_ms);
    TT_UT_EQUAL(b_ret, TT_FALSE, "");
    TT_UT_EXP(abs((int)ttl_ms - 250) < 100, "");

    tt_sleep(400);
    // both de and de2 expired now
    ms = tt_dns_cache_run(dc);
    TT_UT_EXP(ms > 10000, "");

    TT_UT_EQUAL(tt_hmap_count(&dc->map), 2, "");
    TT_UT_EQUAL(tt_ptrheap_count(&dc->heap), 2, "");
    {
        // hacking
        dc->next_check = 0;
        ms = tt_dns_cache_run(dc);
        TT_UT_EXP(ms > 10000, "");
        TT_UT_EQUAL(tt_hmap_count(&dc->map), 2, "");
        TT_UT_EQUAL(tt_ptrheap_count(&dc->heap), 2, "");
    }
    {
        // force the two entry expire
        __dc_check_inuse(dc,
                         tt_time_ref() + tt_time_ms2ref(__INUSE_LIMIT + 10000));
        TT_UT_EQUAL(tt_hmap_count(&dc->map), 0, "");
        TT_UT_EQUAL(tt_ptrheap_count(&dc->heap), 0, "");
    }

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

TT_TEST_ROUTINE_DEFINE(case_dc_get_a)
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

TT_TEST_ROUTINE_DEFINE(case_dc_get_aaaa)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
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

static tt_result_t __udp_answer_any(IN tt_skt_t *s,
                                    IN tt_u8_t *buf,
                                    IN tt_u32_t len,
                                    IN tt_sktaddr_t *addr)
{
    const tt_u8_t ans[] =
        {0xc0, 0x0c, 0, 1, 0, 1, 1, 2, 3, 4, 0, 4, 7, 8, 9, 10};

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

    DUT_INFO("udp answer");
    return tt_skt_sendto(s, buf, len + sizeof(ans), NULL, addr);
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
            ret = __udp_answer_any(s, buf, recvd, &addr);
        } else if (i == 1) {
            DUT_INFO("udp answer");
            __udp_tc = TT_FALSE;
            ret = __udp_answer_any(s, buf, recvd, &addr);
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

extern tt_result_t __tcp_svr1(IN void *param);

static tt_result_t __tcp_answer_rand(IN tt_skt_t *s,
                                     IN tt_u8_t *buf,
                                     IN tt_u32_t len,
                                     IN tt_sktaddr_t *addr)
{
    const tt_u8_t ans[] =
        {0xc0, 0x0c, 0, 1, 0, 1, 1, 2, 3, 4, 0, 4, 7, 8, 9, 10};
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

#define __DNAME_NUM 100
#define __DNAME_LEN 100
static tt_char_t __ut_dname[__DNAME_NUM][__DNAME_LEN];
static tt_u32_t __d_fb_num;

#define __D_FB_NUM 10

static tt_result_t __dc_get3(IN void *param)
{
    tt_u32_t idx = (tt_u32_t)(tt_uintptr_t)param;
    tt_dns_rrlist_t *rrl;
    tt_u32_t num = tt_rand_u32() % 10 + 1, i = 0;

    while (i++ < num) {
        const tt_char_t *name = __ut_dname[tt_rand_u32() % __DNAME_NUM];
        if (tt_rand_u32() % 2) {
            DUT_INFO("fb[%d/%d] a query [%s] start", i, num, name);
            rrl = tt_dns_get_a(name);
            DUT_INFO("fb[%d/%d] a query [%s] end", i, num, name);
        } else {
            DUT_INFO("fb[%d/%d] aaaa query [%s] start", i, num, name);
            rrl = tt_dns_get_aaaa(name);
            DUT_INFO("fb[%d/%d] aaaa query [%s] end", i, num, name);
        }
        (void)rrl;
    }

    if (--__d_fb_num == 0) {
        tt_task_exit(NULL);
    }
    DUT_INFO("fb[%d/%d] end", idx, __D_FB_NUM);
    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(case_dc_exception)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_task_t t;
    tt_result_t ret;
    tt_task_attr_t attr;
    const tt_char_t *svr[] = {
        "127.0.0.1:53210", "[::1]:53211",
    };
    __svr_param_t sp[4];
    tt_u32_t i;

    TT_TEST_CASE_ENTER()
    // test start

    for (i = 0; i < __DNAME_NUM; ++i) {
        tt_u32_t n = tt_rand_u32() % (__DNAME_LEN - 1) + 1;
        tt_u32_t j;
        for (j = 0; j < n; ++j) {
            __ut_dname[i][j] = tt_rand_u32() % 20 + 'a';
        }
    }

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

    __d_fb_num = __D_FB_NUM;
    for (i = 0; i < __D_FB_NUM; ++i) {
        tt_task_add_fiber(&t, NULL, __dc_get3, (void *)(tt_uintptr_t)i, NULL);
    }

    ret = tt_task_run(&t);
    TT_UT_SUCCESS(ret, "");
    tt_task_wait(&t);

    // TT_UT_EQUAL(__dns_errline, 0, "");

    // test end
    TT_TEST_CASE_LEAVE()
}
