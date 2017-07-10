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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_dns_cache_basic)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(dns_cache_case)

TT_TEST_CASE("tt_unit_test_dns_cache_basic",
             "dns cache basic",
             tt_unit_test_dns_cache_basic,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

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
    TT_TEST_ROUTINE_DEFINE(tt_unit_test_dns_cache_basic)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    static tt_u32_t __dns_errline;
static tt_bool_t __udp_tc;

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

static tt_result_t __dc_get1(IN void *param)
{
    tt_dns_rrlist_t *rrl;

    rrl = tt_dns_get_a("163.com");
    if (tt_dns_rrlist_count(rrl) != 1) {
        __dns_errline = __LINE__;
        tt_task_exit(NULL);
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_dns_cache_basic)
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

    sp[0].af = TT_NET_AF_INET;
    sp[0].name = "127.0.0.1";
    sp[0].port = 43210;
    sp[0].on_recv = __udp_answer;
    sp[0].ignore_num = 0;
    tt_task_add_fiber(&t, "udp1", __udp_svr1, &sp[0], NULL);

    tt_task_add_fiber(&t, "dc", __dc_get1, dc, NULL);

    ret = tt_task_run(&t);
    TT_UT_SUCCESS(ret, "");
    tt_task_wait(&t);

    // test end
    TT_TEST_CASE_LEAVE()
}
