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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_dns_rr_notify)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_dns_rr_a)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_dns_rr_aaaa)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(dns_rr_case)

TT_TEST_CASE("tt_unit_test_dns_rr_notify",
             "dns rr wait & notify",
             tt_unit_test_dns_rr_notify,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_dns_rr_a",
                 "dns rr a",
                 tt_unit_test_dns_rr_a,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_dns_rr_aaaa",
                 "dns rr aaaa",
                 tt_unit_test_dns_rr_aaaa,
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
    TT_TEST_ROUTINE_DEFINE(tt_unit_test_dns_rr_a)
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

TT_TEST_ROUTINE_DEFINE(tt_unit_test_dns_rr_notify)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_dns_rr_t drr;
    tt_result_t ret;
    tt_task_t t;

    TT_TEST_CASE_ENTER()
    // test start

    tt_dns_rr_init(&drr, TT_DNS_A_IN);
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

TT_TEST_ROUTINE_DEFINE(tt_unit_test_dns_rr_a)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_dns_rrlist_t rrl, rrl2;
    tt_dns_a_t *a;

    TT_TEST_CASE_ENTER()
    // test start

    tt_dns_rrlist_init(&rrl, TT_DNS_A_IN);
    tt_dns_rrlist_clear(&rrl);
    tt_dns_rrlist_init(&rrl2, TT_DNS_A_IN);

    a = tt_dns_a_head(&rrl);
    TT_UT_NULL(a, "");

    tt_dns_a_copy(&rrl2, &rrl);
    TT_UT_EQUAL(tt_dns_rrlist_count(&rrl), 0, "");
    tt_dns_rrlist_clear(&rrl);

    // add 2 a to rrlist
    a = tt_malloc(sizeof(tt_dns_a_t));
    TT_UT_NOT_NULL(a, "");
    tt_dnode_init(&a->node);
    ((tt_u8_t *)&a->addr)[0] = 12;
    tt_dlist_push_tail(&rrl.rr, &a->node);

    a = tt_malloc(sizeof(tt_dns_a_t));
    TT_UT_NOT_NULL(a, "");
    tt_dnode_init(&a->node);
    ((tt_u8_t *)&a->addr)[0] = 13;
    tt_dlist_push_tail(&rrl.rr, &a->node);

    TT_UT_EQUAL(tt_dns_rrlist_count(&rrl), 2, "");

    // copy rrl to rrl2
    tt_dns_a_copy(&rrl2, &rrl);
    TT_UT_EQUAL(tt_dns_rrlist_count(&rrl2), 2, "");

    a = tt_dns_a_head(&rrl2);
    TT_UT_NOT_NULL(a, "");
    TT_UT_EQUAL(((tt_u8_t *)&a->addr)[0], 12, "");

    a = tt_dns_a_next(a);
    TT_UT_NOT_NULL(a, "");
    TT_UT_EQUAL(((tt_u8_t *)&a->addr)[0], 13, "");

    a = tt_dns_a_next(a);
    TT_UT_NULL(a, "");

    tt_dns_rrlist_clear(&rrl);
    tt_dns_rrlist_clear(&rrl2);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_dns_rr_aaaa)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_dns_rrlist_t rrl, rrl2;
    tt_dns_aaaa_t *a;

    TT_TEST_CASE_ENTER()
    // test start

    tt_dns_rrlist_init(&rrl, TT_DNS_AAAA_IN);
    tt_dns_rrlist_clear(&rrl);
    tt_dns_rrlist_init(&rrl2, TT_DNS_AAAA_IN);

    a = tt_dns_aaaa_head(&rrl);
    TT_UT_NULL(a, "");

    tt_dns_aaaa_copy(&rrl2, &rrl);
    TT_UT_EQUAL(tt_dns_rrlist_count(&rrl), 0, "");
    tt_dns_rrlist_clear(&rrl);

    // add 2 a to rrlist
    a = tt_malloc(sizeof(tt_dns_aaaa_t));
    TT_UT_NOT_NULL(a, "");
    tt_dnode_init(&a->node);
    ((tt_u8_t *)&a->addr)[0] = 12;
    tt_dlist_push_tail(&rrl.rr, &a->node);

    a = tt_malloc(sizeof(tt_dns_aaaa_t));
    TT_UT_NOT_NULL(a, "");
    tt_dnode_init(&a->node);
    ((tt_u8_t *)&a->addr)[0] = 13;
    tt_dlist_push_tail(&rrl.rr, &a->node);

    TT_UT_EQUAL(tt_dns_rrlist_count(&rrl), 2, "");

    // copy rrl to rrl2
    tt_dns_aaaa_copy(&rrl2, &rrl);
    TT_UT_EQUAL(tt_dns_rrlist_count(&rrl2), 2, "");

    a = tt_dns_aaaa_head(&rrl2);
    TT_UT_NOT_NULL(a, "");
    TT_UT_EQUAL(((tt_u8_t *)&a->addr)[0], 12, "");

    a = tt_dns_aaaa_next(a);
    TT_UT_NOT_NULL(a, "");
    TT_UT_EQUAL(((tt_u8_t *)&a->addr)[0], 13, "");

    a = tt_dns_aaaa_next(a);
    TT_UT_NULL(a, "");

    tt_dns_rrlist_clear(&rrl);
    tt_dns_rrlist_clear(&rrl2);

    // test end
    TT_TEST_CASE_LEAVE()
}
