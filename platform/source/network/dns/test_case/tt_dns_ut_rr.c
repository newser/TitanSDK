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

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

extern int __a_parse(IN const unsigned char *abuf,
                     IN int alen,
                     OUT tt_s64_t *ttl,
                     OUT tt_dns_rrlist_t *rrl);

extern int __aaaa_parse(IN const unsigned char *abuf,
                        IN int alen,
                        OUT tt_s64_t *ttl,
                        OUT tt_dns_rrlist_t *rrl);


////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

// === routine declarations ================
TT_TEST_ROUTINE_DECLARE(case_dns_rr_notify)
TT_TEST_ROUTINE_DECLARE(case_dns_rr_a)
TT_TEST_ROUTINE_DECLARE(case_dns_rr_aaaa)
TT_TEST_ROUTINE_DECLARE(case_dns_parse_a)
TT_TEST_ROUTINE_DECLARE(case_dns_parse_aaaa)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(dns_rr_case)

TT_TEST_CASE(
    "case_dns_rr_a", "dns rr a", case_dns_rr_a, NULL, NULL, NULL, NULL, NULL)
,

    TT_TEST_CASE("case_dns_rr_aaaa",
                 "dns rr aaaa",
                 case_dns_rr_aaaa,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_dns_parse_a",
                 "dns parse a",
                 case_dns_parse_a,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_dns_parse_aaaa",
                 "dns parse aaaa",
                 case_dns_parse_aaaa,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(dns_rr_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(DNS_UT_RR, 0, dns_rr_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(case_dns_parse_a)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    static tt_u32_t __err_line;
static tt_u32_t __rr_recv;
static tt_u32_t __rr_send;

#if 0
static tt_result_t __simu_wait(IN void *param)
{
    tt_dns_rr_t *drr = (tt_dns_rr_t *)param;
    tt_u32_t i = 0;
    tt_fiber_ev_t *fev;

    __rr_recv = 0;
    while (i++ < 10) {
        if (tt_dlist_empty(&drr->rrlist.rr)) {
            tt_dns_rrlist_t *rrl = tt_dns_rr_wait(drr);
            __rr_recv += tt_dlist_count(&rrl->rr);
        }
        tt_dns_rrlist_clear(&drr->rrlist);
    }

    fev = tt_fiber_ev_create(0, 0);
    tt_fiber_send_ev(tt_fiber_find("f2"), fev, TT_FALSE);

    return TT_SUCCESS;
}

static tt_result_t __simu_notify(IN void *param)
{
    tt_dns_rr_t *drr = (tt_dns_rr_t *)param;
    tt_fiber_t *fb = tt_current_fiber();
    tt_tmr_t *t = tt_tmr_create(tt_rand_u32() % 2000, 0, NULL);

    __rr_send = 0;

    tt_tmr_start(t);
    while (1) {
        tt_fiber_ev_t *fev;
        tt_tmr_t *r_tmr;
        tt_bool_t r = tt_fiber_recv(fb, TT_TRUE, &fev, &r_tmr);
        tt_dns_a_t *a;
        tt_u32_t i;

        if (!r) {
            __err_line = __LINE__;
            tt_task_exit(NULL);
            return TT_FAIL;
        }

        if (fev != NULL) {
            tt_fiber_finish(fev);
            return TT_SUCCESS;
        }

        i = tt_rand_u32() % 5 + 1;
        __rr_send += i;
        while (i-- != 0) {
            a = tt_malloc(sizeof(tt_dns_a_t));
            tt_dnode_init(&a->node);
            tt_dlist_push_tail(&drr->rrlist.rr, &a->node);
        }
        tt_dns_rr_noitfy(drr);

        tt_tmr_set_delay(t, tt_rand_u32() % 2000);
        tt_tmr_start(t);
    }

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(case_dns_rr_notify)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_dns_rr_t drr;
    tt_result_t ret;
    tt_task_t t;

    TT_TEST_CASE_ENTER()
    // test start

    tt_dns_rr_init(&drr, "123", TT_DNS_A_IN);
    tt_dns_rr_clear(&drr);
    tt_dns_rr_check(&drr, tt_time_ref());

    ret = tt_task_create(&t, NULL);
    TT_UT_SUCCESS(ret, "");

    tt_task_add_fiber(&t, "f1", __simu_wait, &drr, NULL);
    tt_task_add_fiber(&t, "f2", __simu_notify, &drr, NULL);

    ret = tt_task_run(&t);
    TT_UT_SUCCESS(ret, "");

    tt_task_wait(&t);
    TT_UT_EQUAL(__err_line, 0, "");
    TT_UT_EQUAL(__rr_send, __rr_recv + tt_dlist_count(&drr.rrlist.rr), "");

    tt_dns_rr_clear(&drr);
    tt_dns_rr_check(&drr, tt_time_ref());

    // test end
    TT_TEST_CASE_LEAVE()
}
#endif

TT_TEST_ROUTINE_DEFINE(case_dns_rr_a)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_dns_rrlist_t rrl, rrl2;
    tt_dns_a_t *a;
    tt_sktaddr_ip32_t ip32;
    tt_result_t ret;

    TT_TEST_CASE_ENTER()
    // test start

    tt_dns_rrlist_init(&rrl, TT_DNS_A_IN);
    tt_dns_rrlist_clear(&rrl);
    tt_dns_rrlist_init(&rrl2, TT_DNS_A_IN);

    a = tt_dns_a_head(&rrl);
    TT_UT_NULL(a, "");

    tt_dns_rrlist_copy(&rrl2, &rrl);
    TT_UT_EQUAL(tt_dns_rrlist_count(&rrl), 0, "");
    TT_UT_EQUAL(tt_dns_rrlist_empty(&rrl), TT_TRUE, "");
    tt_dns_rrlist_clear(&rrl);

    // add 2 a to rrlist
    a = tt_malloc(sizeof(tt_dns_a_t));
    TT_UT_NOT_NULL(a, "");
    tt_dnode_init(&a->node);
    ((tt_u8_t *)&a->ip)[0] = 12;
    tt_dlist_push_tail(&rrl.rr, &a->node);

    a = tt_malloc(sizeof(tt_dns_a_t));
    TT_UT_NOT_NULL(a, "");
    tt_dnode_init(&a->node);
    ((tt_u8_t *)&a->ip)[0] = 13;
    tt_dlist_push_tail(&rrl.rr, &a->node);

    TT_UT_EQUAL(tt_dns_rrlist_count(&rrl), 2, "");
    TT_UT_EQUAL(tt_dns_rrlist_empty(&rrl), TT_FALSE, "");

    // copy rrl to rrl2
    tt_dns_rrlist_copy(&rrl2, &rrl);
    TT_UT_EQUAL(tt_dns_rrlist_count(&rrl2), 2, "");

    a = tt_dns_a_head(&rrl2);
    TT_UT_NOT_NULL(a, "");
    TT_UT_EQUAL(((tt_u8_t *)&a->ip)[0], 12, "");

    a = tt_dns_a_next(a);
    TT_UT_NOT_NULL(a, "");
    TT_UT_EQUAL(((tt_u8_t *)&a->ip)[0], 13, "");

    a = tt_dns_a_next(a);
    TT_UT_NULL(a, "");

    tt_dns_rrlist_move(&rrl, &rrl2);
    TT_UT_EQUAL(tt_dns_rrlist_count(&rrl), 4, "");
    TT_UT_EQUAL(tt_dns_rrlist_count(&rrl2), 0, "");

    tt_dns_rrlist_clear(&rrl);
    tt_dns_rrlist_clear(&rrl2);

    ip32.__u32 = 0x1234;
    ret = tt_dns_rrlist_add_a(&rrl, &ip32);
    TT_UT_SUCCESS(ret, "");

    a = tt_dns_a_head(&rrl);
    TT_UT_EQUAL(a->ip.a32.__u32, 0x1234, "");
    tt_dns_rrlist_clear(&rrl);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_dns_rr_aaaa)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_dns_rrlist_t rrl, rrl2;
    tt_dns_aaaa_t *a;
    tt_result_t ret;
    tt_sktaddr_ip128_t ip;

    TT_TEST_CASE_ENTER()
    // test start

    tt_dns_rrlist_init(&rrl, TT_DNS_AAAA_IN);
    tt_dns_rrlist_clear(&rrl);
    tt_dns_rrlist_init(&rrl2, TT_DNS_AAAA_IN);

    a = tt_dns_aaaa_head(&rrl);
    TT_UT_NULL(a, "");

    tt_dns_rrlist_copy(&rrl2, &rrl);
    TT_UT_EQUAL(tt_dns_rrlist_count(&rrl), 0, "");
    tt_dns_rrlist_clear(&rrl);

    // add 2 a to rrlist
    a = tt_malloc(sizeof(tt_dns_aaaa_t));
    TT_UT_NOT_NULL(a, "");
    tt_dnode_init(&a->node);
    ((tt_u8_t *)&a->ip)[0] = 12;
    tt_dlist_push_tail(&rrl.rr, &a->node);

    a = tt_malloc(sizeof(tt_dns_aaaa_t));
    TT_UT_NOT_NULL(a, "");
    tt_dnode_init(&a->node);
    ((tt_u8_t *)&a->ip)[0] = 13;
    tt_dlist_push_tail(&rrl.rr, &a->node);

    TT_UT_EQUAL(tt_dns_rrlist_count(&rrl), 2, "");

    // copy rrl to rrl2
    tt_dns_rrlist_copy(&rrl2, &rrl);
    TT_UT_EQUAL(tt_dns_rrlist_count(&rrl2), 2, "");

    a = tt_dns_aaaa_head(&rrl2);
    TT_UT_NOT_NULL(a, "");
    TT_UT_EQUAL(((tt_u8_t *)&a->ip)[0], 12, "");

    a = tt_dns_aaaa_next(a);
    TT_UT_NOT_NULL(a, "");
    TT_UT_EQUAL(((tt_u8_t *)&a->ip)[0], 13, "");

    a = tt_dns_aaaa_next(a);
    TT_UT_NULL(a, "");

    tt_dns_rrlist_clear(&rrl);
    tt_dns_rrlist_clear(&rrl2);

    ip.__u32[0] = 0x1234;
    ip.__u32[3] = 0x5678;
    ret = tt_dns_rrlist_add_aaaa(&rrl, &ip);
    TT_UT_SUCCESS(ret, "");

    a = tt_dns_aaaa_head(&rrl);
    TT_UT_EQUAL(a->ip.a128.__u32[0], 0x1234, "");
    TT_UT_EQUAL(a->ip.a128.__u32[3], 0x5678, "");
    tt_dns_rrlist_clear(&rrl);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_dns_parse_a)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_u8_t resp[] = {
        0x75, 0xc0, 0x81, 0x80, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
        0x03, 0x77, 0x77, 0x77, 0x06, 0x6e, 0x65, 0x74, 0x62, 0x73, 0x64, 0x03,
        0x6f, 0x72, 0x67, 0x00, 0x00, 0x01, 0x00, 0x01, 0xc0, 0x0c, 0x00, 0x01,
        0x00, 0x01, 0x00, 0x01, 0x40, 0xef, 0x00, 0x04, 0xcc, 0x98, 0xbe, 0x0c,
    };
    tt_u8_t resp_more_q[] = {
        0x75, 0xc0, 0x81, 0x80, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
        0x03, 0x77, 0x77, 0x77, 0x06, 0x6e, 0x65, 0x74, 0x62, 0x73, 0x64, 0x03,
        0x6f, 0x72, 0x67, 0x00, 0x00, 0x01, 0x00, 0x01, 0xc0, 0x0c, 0x00, 0x01,
        0x00, 0x01, 0x00, 0x01, 0x40, 0xef, 0x00, 0x04, 0xcc, 0x98, 0xbe, 0x0c,
    };
    tt_u8_t resp_inval_name[] = {
        0x75, 0xc0, 0x81, 0x80, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
        0x7f, 0x77, 0x77, 0x77, 0x06, 0x6e, 0x65, 0x74, 0x62, 0x73, 0x64, 0x03,
        0x6f, 0x72, 0x67, 0x00, 0x00, 0x01, 0x00, 0x01, 0xc0, 0x0c, 0x00, 0x01,
        0x00, 0x01, 0x00, 0x01, 0x40, 0xef, 0x00, 0x04, 0xcc, 0x98, 0xbe, 0x0c,
    };
    tt_u8_t resp_many_ans[] = {
        0x75, 0xc0, 0x81, 0x80, 0x00, 0x01, 0x7f, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x03, 0x77, 0x77, 0x77, 0x06, 0x6e, 0x65, 0x74, 0x62, 0x73, 0x64, 0x03,
        0x6f, 0x72, 0x67, 0x00, 0x00, 0x01, 0x00, 0x01, 0xc0, 0x0c, 0x00, 0x01,
        0x00, 0x01, 0x00, 0x01, 0x40, 0xef, 0x00, 0x04, 0xcc, 0x98, 0xbe, 0x0c,
    };
    tt_u8_t resp_inval_rrname[] = {
        0x75, 0xc0, 0x81, 0x80, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
        0x03, 0x77, 0x77, 0x77, 0x06, 0x6e, 0x65, 0x74, 0x62, 0x73, 0x64, 0x03,
        0x6f, 0x72, 0x67, 0x00, 0x00, 0x01, 0x00, 0x01, 0x12, 0x34, 0x00, 0x01,
        0x00, 0x01, 0x00, 0x01, 0x40, 0xef, 0x00, 0x04, 0xcc, 0x98, 0xbe, 0x0c,
    };
    tt_u8_t resp_long_rrlen[] = {
        0x75, 0xc0, 0x81, 0x80, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
        0x03, 0x77, 0x77, 0x77, 0x06, 0x6e, 0x65, 0x74, 0x62, 0x73, 0x64, 0x03,
        0x6f, 0x72, 0x67, 0x00, 0x00, 0x01, 0x00, 0x01, 0xc0, 0x0c, 0x00, 0x01,
        0x00, 0x01, 0x00, 0x01, 0x40, 0xef, 0x00, 0x05, 0xcc, 0x98, 0xbe, 0x0c,
    };
    tt_u8_t resp_no_answer[] = {
        0x75, 0xc0, 0x81, 0x80, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
        0x03, 0x77, 0x77, 0x77, 0x06, 0x6e, 0x65, 0x74, 0x62, 0x73, 0x64, 0x03,
        0x6f, 0x72, 0x67, 0x00, 0x00, 0x01, 0x00, 0x01, 0xc0, 0x0c, 0x00, 0x01,
        0x00, 0x02, 0x00, 0x01, 0x40, 0xef, 0x00, 0x04, 0xcc, 0x98, 0xbe, 0x0c,
    };
    tt_u8_t resp_2_answer[] = {
        0x75, 0xc0, 0x81, 0x80, 0x00, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00,
        0x00, 0x03, 0x77, 0x77, 0x77, 0x06, 0x6e, 0x65, 0x74, 0x62, 0x73,
        0x64, 0x03, 0x6f, 0x72, 0x67, 0x00, 0x00, 0x01, 0x00, 0x01, 0xc0,
        0x0c, 0x00, 0x01, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0x00, 0x04,
        0xcc, 0x98, 0xbe, 0x0c, 0xc0, 0x0c, 0x00, 0x01, 0x00, 0x01, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x04, 0x01, 0x02, 0x03, 0x04,
    };
    tt_u8_t resp_inval_2th[] = {
        0x75, 0xc0, 0x81, 0x80, 0x00, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00,
        0x00, 0x03, 0x77, 0x77, 0x77, 0x06, 0x6e, 0x65, 0x74, 0x62, 0x73,
        0x64, 0x03, 0x6f, 0x72, 0x67, 0x00, 0x00, 0x01, 0x00, 0x01, 0xc0,
        0x0c, 0x00, 0x01, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0x00, 0x04,
        0xcc, 0x98, 0xbe, 0x0c, 0xff, 0xff, 0x00, 0x01, 0x00, 0x01, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x04, 0x01, 0x02, 0x03, 0x04,
    };
    tt_u8_t resp_rand[sizeof(resp_2_answer)];
    tt_dns_rrlist_t rrl;
    tt_s64_t ttl = 1;
    int status;
    tt_dns_a_t *a;
    tt_u32_t i;

    TT_TEST_CASE_ENTER()
    // test start

    tt_dns_rrlist_init(&rrl, TT_DNS_A_IN);

    // incomplete
    status = __a_parse(resp, 2, &ttl, &rrl);
    TT_UT_NOT_EQUAL(status, 0, "");

    // more than 1 question
    status = __a_parse(resp_more_q, sizeof(resp_more_q), &ttl, &rrl);
    TT_UT_NOT_EQUAL(status, 0, "");

    // invalid name
    status = __a_parse(resp_inval_name, sizeof(resp_inval_name), &ttl, &rrl);
    TT_UT_NOT_EQUAL(status, 0, "");

    // too many answer
    status = __a_parse(resp_many_ans, sizeof(resp_many_ans), &ttl, &rrl);
    TT_UT_NOT_EQUAL(status, 0, "");

    // invalid rr name
    status =
        __a_parse(resp_inval_rrname, sizeof(resp_inval_rrname), &ttl, &rrl);
    TT_UT_NOT_EQUAL(status, 0, "");

    // long rr length
    status = __a_parse(resp_long_rrlen, sizeof(resp_long_rrlen), &ttl, &rrl);
    TT_UT_NOT_EQUAL(status, 0, "");

    // no answer
    status = __a_parse(resp_no_answer, sizeof(resp_no_answer), &ttl, &rrl);
    TT_UT_NOT_EQUAL(status, 0, "");

    // valid 1 rr
    status = __a_parse(resp, sizeof(resp), &ttl, &rrl);
    TT_UT_EQUAL(status, 0, "");
    TT_UT_EQUAL(tt_dns_rrlist_count(&rrl), 1, "");
    TT_UT_EQUAL(ttl, 82159, "");

    a = tt_dns_a_head(&rrl);
    TT_UT_EQUAL(a->ip.a32.__u32, 0x0cbe98cc, "");
    tt_dns_rrlist_clear(&rrl);

    // valid 2 rr
    status = __a_parse(resp_2_answer, sizeof(resp_2_answer), &ttl, &rrl);
    TT_UT_EQUAL(status, 0, "");
    TT_UT_EQUAL(tt_dns_rrlist_count(&rrl), 2, "");
    TT_UT_EQUAL(ttl, 0, "");

    a = tt_dns_a_head(&rrl);
    TT_UT_EQUAL(a->ip.a32.__u32, 0x0cbe98cc, "");

    a = tt_dns_a_next(a);
    TT_UT_EQUAL(a->ip.a32.__u32, 0x04030201, "");
    tt_dns_rrlist_clear(&rrl);

    // no answer
    status = __a_parse(resp_inval_2th, sizeof(resp_inval_2th), &ttl, &rrl);
    TT_UT_NOT_EQUAL(status, 0, "");

    i = 0;
    while (i++ < 10) {
        tt_u32_t k = tt_rand_u32() % sizeof(resp_2_answer);
        tt_u32_t len = tt_rand_u32() % (sizeof(resp_2_answer) - k);
        tt_memcpy(resp_rand, resp_2_answer, sizeof(resp_2_answer));
        while (len-- != 0) {
            resp_rand[k++] = (tt_u8_t)tt_rand_u32();
        }
        status = __a_parse(resp_rand, sizeof(resp_rand), &ttl, &rrl);
        if (status == 0) {
            tt_dns_rrlist_clear(&rrl);
        }
    }

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_dns_parse_aaaa)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_u8_t resp[] = {
        0xf0, 0xd4, 0x81, 0x80, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
        0x03, 0x77, 0x77, 0x77, 0x06, 0x6e, 0x65, 0x74, 0x62, 0x73, 0x64, 0x03,
        0x6f, 0x72, 0x67, 0x00, 0x00, 0x1c, 0x00, 0x01, 0xc0, 0x0c, 0x00, 0x1c,
        0x00, 0x01, 0x00, 0x01, 0x51, 0x80, 0x00, 0x10, 0x20, 0x01, 0x04, 0xf8,
        0x00, 0x04, 0x00, 0x07, 0x02, 0xe0, 0x81, 0xff, 0xfe, 0x52, 0x9a, 0x6b,
    };
    tt_u8_t resp_more_q[] = {
        0xf0, 0xd4, 0x81, 0x80, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
        0x03, 0x77, 0x77, 0x77, 0x06, 0x6e, 0x65, 0x74, 0x62, 0x73, 0x64, 0x03,
        0x6f, 0x72, 0x67, 0x00, 0x00, 0x1c, 0x00, 0x01, 0xc0, 0x0c, 0x00, 0x1c,
        0x00, 0x01, 0x00, 0x01, 0x51, 0x80, 0x00, 0x10, 0x20, 0x01, 0x04, 0xf8,
        0x00, 0x04, 0x00, 0x07, 0x02, 0xe0, 0x81, 0xff, 0xfe, 0x52, 0x9a, 0x6b,
    };
    tt_u8_t resp_inval_name[] = {
        0xf0, 0xd4, 0x81, 0x80, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
        0x04, 0x77, 0x77, 0x77, 0x06, 0x6e, 0x65, 0x74, 0x62, 0x73, 0x64, 0x03,
        0x6f, 0x72, 0x67, 0x00, 0x00, 0x1c, 0x00, 0x01, 0xc0, 0x0c, 0x00, 0x1c,
        0x00, 0x01, 0x00, 0x01, 0x51, 0x80, 0x00, 0x10, 0x20, 0x01, 0x04, 0xf8,
        0x00, 0x04, 0x00, 0x07, 0x02, 0xe0, 0x81, 0xff, 0xfe, 0x52, 0x9a, 0x6b,
    };
    tt_u8_t resp_many_ans[] = {
        0xf0, 0xd4, 0x81, 0x80, 0x00, 0x01, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00,
        0x03, 0x77, 0x77, 0x77, 0x06, 0x6e, 0x65, 0x74, 0x62, 0x73, 0x64, 0x03,
        0x6f, 0x72, 0x67, 0x00, 0x00, 0x1c, 0x00, 0x01, 0xc0, 0x0c, 0x00, 0x1c,
        0x00, 0x01, 0x00, 0x01, 0x51, 0x80, 0x00, 0x10, 0x20, 0x01, 0x04, 0xf8,
        0x00, 0x04, 0x00, 0x07, 0x02, 0xe0, 0x81, 0xff, 0xfe, 0x52, 0x9a, 0x6b,
    };
    tt_u8_t resp_inval_rrname[] = {
        0xf0, 0xd4, 0x81, 0x80, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
        0x03, 0x77, 0x77, 0x77, 0x06, 0x6e, 0x65, 0x74, 0x62, 0x73, 0x64, 0x03,
        0x6f, 0x72, 0x67, 0x00, 0x00, 0x1c, 0x00, 0x01, 0xc1, 0xcc, 0x00, 0x1c,
        0x00, 0x01, 0x00, 0x01, 0x51, 0x80, 0x00, 0x10, 0x20, 0x01, 0x04, 0xf8,
        0x00, 0x04, 0x00, 0x07, 0x02, 0xe0, 0x81, 0xff, 0xfe, 0x52, 0x9a, 0x6b,
    };
    tt_u8_t resp_long_rrlen[] = {
        0xf0, 0xd4, 0x81, 0x80, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
        0x03, 0x77, 0x77, 0x77, 0x06, 0x6e, 0x65, 0x74, 0x62, 0x73, 0x64, 0x03,
        0x6f, 0x72, 0x67, 0x00, 0x00, 0x1c, 0x00, 0x01, 0xc0, 0x0c, 0x00, 0x1c,
        0x00, 0x01, 0x00, 0x01, 0x51, 0x80, 0x80, 0x10, 0x20, 0x01, 0x04, 0xf8,
        0x00, 0x04, 0x00, 0x07, 0x02, 0xe0, 0x81, 0xff, 0xfe, 0x52, 0x9a, 0x6b,
    };
    tt_u8_t resp_no_answer[] = {
        0xf0, 0xd4, 0x81, 0x80, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
        0x03, 0x77, 0x77, 0x77, 0x06, 0x6e, 0x65, 0x74, 0x62, 0x73, 0x64, 0x03,
        0x6f, 0x72, 0x67, 0x00, 0x00, 0x1c, 0x00, 0x01, 0x12, 0x0c, 0x00, 0x1c,
        0x00, 0x01, 0x00, 0x01, 0x51, 0x80, 0x00, 0x10, 0x20, 0x01, 0x04, 0xf8,
        0x00, 0x04, 0x00, 0x07, 0x02, 0xe0, 0x81, 0xff, 0xfe, 0x52, 0x9a, 0x6b,
    };
    tt_u8_t resp_2_answer[] = {
        0xf0, 0xd4, 0x81, 0x80, 0x00, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00,
        0x00, 0x03, 0x77, 0x77, 0x77, 0x06, 0x6e, 0x65, 0x74, 0x62, 0x73,
        0x64, 0x03, 0x6f, 0x72, 0x67, 0x00, 0x00, 0x1c, 0x00, 0x01, 0xc0,
        0x0c, 0x00, 0x1c, 0x00, 0x01, 0x12, 0x34, 0x56, 0x78, 0x00, 0x10,
        0x20, 0x01, 0x04, 0xf8, 0x00, 0x04, 0x00, 0x07, 0x02, 0xe0, 0x81,
        0xff, 0xfe, 0x52, 0x9a, 0x6b, 0xc0, 0x0c, 0x00, 0x1c, 0x00, 0x01,
        0x87, 0x65, 0x43, 0x21, 0x00, 0x10, 0x12, 0x34, 0x56, 0xf8, 0x00,
        0x04, 0x00, 0x07, 0x02, 0xe0, 0x81, 0xff, 0xfe, 0x52, 0x9a, 0x6b,
    };
    tt_u8_t resp_inval_2th[] = {
        0xf0, 0xd4, 0x81, 0x80, 0x00, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00,
        0x00, 0x03, 0x77, 0x77, 0x77, 0x06, 0x6e, 0x65, 0x74, 0x62, 0x73,
        0x64, 0x03, 0x6f, 0x72, 0x67, 0x00, 0x00, 0x1c, 0x00, 0x01, 0xc0,
        0x0c, 0x00, 0x1c, 0x00, 0x01, 0x12, 0x34, 0x56, 0x78, 0x00, 0x10,
        0x20, 0x01, 0x04, 0xf8, 0x00, 0x04, 0x00, 0x07, 0x02, 0xe0, 0x81,
        0xff, 0xfe, 0x52, 0x9a, 0x6b, 0xc0, 0x0c, 0x00, 0x1c, 0x00, 0x01,
        0x87, 0x65, 0x43, 0x21, 0x80, 0x10, 0x12, 0x34, 0x56, 0xf8, 0x00,
        0x04, 0x00, 0x07, 0x02, 0xe0, 0x81, 0xff, 0xfe, 0x52, 0x9a, 0x6b,
    };
    tt_u8_t resp_rand[sizeof(resp_2_answer)];
    tt_dns_rrlist_t rrl;
    tt_s64_t ttl = 1;
    int status;
    tt_dns_aaaa_t *a;
    tt_u32_t i;

    TT_TEST_CASE_ENTER()
    // test start

    tt_dns_rrlist_init(&rrl, TT_DNS_AAAA_IN);

    // incomplete
    status = __aaaa_parse(resp, 2, &ttl, &rrl);
    TT_UT_NOT_EQUAL(status, 0, "");

    // more than 1 question
    status = __aaaa_parse(resp_more_q, sizeof(resp_more_q), &ttl, &rrl);
    TT_UT_NOT_EQUAL(status, 0, "");

    // invalid name
    status = __aaaa_parse(resp_inval_name, sizeof(resp_inval_name), &ttl, &rrl);
    TT_UT_NOT_EQUAL(status, 0, "");

    // too many answer
    status = __aaaa_parse(resp_many_ans, sizeof(resp_many_ans), &ttl, &rrl);
    TT_UT_NOT_EQUAL(status, 0, "");

    // invalid rr name
    status =
        __aaaa_parse(resp_inval_rrname, sizeof(resp_inval_rrname), &ttl, &rrl);
    TT_UT_NOT_EQUAL(status, 0, "");

    // long rr length
    status = __aaaa_parse(resp_long_rrlen, sizeof(resp_long_rrlen), &ttl, &rrl);
    TT_UT_NOT_EQUAL(status, 0, "");

    // no answer
    status = __aaaa_parse(resp_no_answer, sizeof(resp_no_answer), &ttl, &rrl);
    TT_UT_NOT_EQUAL(status, 0, "");

    // valid 1 rr
    status = __aaaa_parse(resp, sizeof(resp), &ttl, &rrl);
    TT_UT_EQUAL(status, 0, "");
    TT_UT_EQUAL(tt_dns_rrlist_count(&rrl), 1, "");
    TT_UT_EQUAL(ttl, 86400, "");

    a = tt_dns_aaaa_head(&rrl);
    TT_UT_EQUAL(a->ip.a128.__u8[0], 0x20, "");
    TT_UT_EQUAL(a->ip.a128.__u8[15], 0x6b, "");
    tt_dns_rrlist_clear(&rrl);

    // valid 2 rr
    status = __aaaa_parse(resp_2_answer, sizeof(resp_2_answer), &ttl, &rrl);
    TT_UT_EQUAL(status, 0, "");
    TT_UT_EQUAL(tt_dns_rrlist_count(&rrl), 2, "");
    TT_UT_EQUAL(ttl, 0, "");

    a = tt_dns_aaaa_head(&rrl);
    TT_UT_EQUAL(a->ip.a128.__u8[0], 0x20, "");
    TT_UT_EQUAL(a->ip.a128.__u8[15], 0x6b, "");

    a = tt_dns_aaaa_next(a);
    TT_UT_EQUAL(a->ip.a128.__u8[0], 0x12, "");
    TT_UT_EQUAL(a->ip.a128.__u8[15], 0x6b, "");
    tt_dns_rrlist_clear(&rrl);

    // no answer
    status = __aaaa_parse(resp_inval_2th, sizeof(resp_inval_2th), &ttl, &rrl);
    TT_UT_NOT_EQUAL(status, 0, "");

    i = 0;
    while (i++ < 10) {
        tt_u32_t k = tt_rand_u32() % sizeof(resp_2_answer);
        tt_u32_t len = tt_rand_u32() % (sizeof(resp_2_answer) - k);
        tt_memcpy(resp_rand, resp_2_answer, sizeof(resp_2_answer));
        while (len-- != 0) {
            resp_rand[k++] = (tt_u8_t)tt_rand_u32();
        }
        status = __aaaa_parse(resp_rand, sizeof(resp_rand), &ttl, &rrl);
        if (status == 0) {
            tt_dns_rrlist_clear(&rrl);
        }
    }

    // test end
    TT_TEST_CASE_LEAVE()
}
