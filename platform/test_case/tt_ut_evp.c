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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_evp_timer_basic)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_evp_timer_st)

TT_TEST_ROUTINE_DECLARE(tt_unit_test_evp_threadev_perf)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(evp_case)

#if 1
TT_TEST_CASE("tt_unit_test_evp_timer_basic_evp",
             "testing evcenter time basic API 1 evp",
             tt_unit_test_evp_timer_basic,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_evp_timer_st",
                 "testing evpoller time stress test",
                 tt_unit_test_evp_timer_st,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),
#endif

#if 1
    TT_TEST_CASE("tt_unit_test_evp_threadev_perf",
                 "testing evpoller thread ev perf compare",
                 tt_unit_test_evp_threadev_perf,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),
#endif

    TT_TEST_CASE_LIST_DEFINE_END(evp_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(TEST_UNIT_EVPOLLER, 0, evp_case)

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

    static tt_u32_t __evp_t1_error_line;
static tt_result_t __evp_t1_ret;
static tt_u32_t __evp_t1_id;
static tt_tmr_t *tmr_short, *tmr_mid, *tmr_long;
static tt_u32_t __stopped;

static void _timer_check(IN struct tt_tmr_s *timer,
                         IN void *param,
                         IN tt_u32_t reason)
{
    tt_s64_t now = tt_time_ref();
    tt_s64_t diff = tt_time_ref2ms(abs((int)(now - (tt_s64_t)timer->opaque)));
    tt_s64_t exp;

    TT_ASSERT(reason == TT_TMR_CB_EXPIRED);

    // check accuracy
    if (diff > 10) {
        __evp_t1_error_line = __LINE__;
        __evp_t1_ret = TT_FAIL;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    if ((tt_u32_t)(tt_uintptr_t)timer->cb_param == 1) {
        // short timer
        __evp_t1_id = 1;
        TT_ASSERT(timer == tmr_short);

        tt_tmr_destroy(timer);
    } else if ((tt_u32_t)(tt_uintptr_t)timer->cb_param == 2) {
        // mid timer
        __evp_t1_id = 2;
        TT_ASSERT(timer == tmr_mid);

        // stop long timer
        tt_tmr_stop(tmr_long);

        // let middle timer expire before long timer
        exp = tt_time_ref();
        exp += tt_time_ms2ref(100);
        do {
            tt_tmr_set_delay(tmr_mid, 100);
            tt_tmr_set_cb(tmr_mid, _timer_check);
            tt_tmr_set_cbparam(tmr_mid, (void *)4);
            tt_tmr_set_opaque(tmr_mid, (void *)exp);
            tt_tmr_start(tmr_mid);
        } while (0);
        tt_tmr_start(tmr_mid);

        // let middle timer expire before long timer
        exp = tt_time_ref();
        exp += tt_time_ms2ref(200);
        do {
            tt_tmr_set_delay(tmr_long, 200);
            tt_tmr_set_cb(tmr_long, _timer_check);
            tt_tmr_set_cbparam(tmr_long, (void *)3);
            tt_tmr_set_opaque(tmr_long, (void *)exp);
            tt_tmr_start(tmr_long);
        } while (0);
        tt_tmr_start(tmr_long);
    } else if ((tt_u32_t)(tt_uintptr_t)timer->cb_param == 3) {
        // long timer
        __evp_t1_id = 3;
        TT_ASSERT(timer == tmr_long);

        tt_tmr_destroy(timer);

        // all done
        tt_evc_exit(TT_LOCAL_EVC);
    } else if ((tt_u32_t)(tt_uintptr_t)timer->cb_param == 4) {
        // re-expired mid timer
        __evp_t1_id = 4;
        TT_ASSERT(timer == tmr_mid);

        tt_tmr_destroy(timer);

        // destroy long tmr while it's still managed by evp
        tt_tmr_destroy(tmr_long);

        exp = tt_time_ref();
        exp += tt_time_ms2ref(100);
        tmr_long =
            tt_tmr_create(NULL, 100, (void *)exp, _timer_check, (void *)3, 0);
        tmr_long->opaque = (void *)exp;
        tt_tmr_start(tmr_long);
    }
}

tt_result_t __evp_timer_basic(IN struct tt_evcenter_s *evc,
                              IN void *on_init_param)
{
#define __evp1_exit_if(e)                                                      \
    do {                                                                       \
        if ((e)) {                                                             \
            __evp_t1_error_line = __LINE__;                                    \
            __evp_t1_ret = TT_FAIL;                                            \
            return TT_FAIL;                                                    \
        }                                                                      \
    } while (0)

    tt_s64_t exp;
    tt_result_t ret;

    // start timer
    exp = tt_time_ref();
    exp += tt_time_ms2ref(100);
    tmr_short =
        tt_tmr_create(NULL, 100, (void *)exp, _timer_check, (void *)1, 0);
    __evp1_exit_if(tmr_short == NULL);

    ret = tt_tmr_start(tmr_short);
    __evp1_exit_if(!TT_OK(ret));

    // start mid timer
    exp = tt_time_ref();
    exp += tt_time_ms2ref(200);
    tmr_mid = tt_tmr_create(NULL, 200, (void *)exp, _timer_check, (void *)2, 0);
    __evp1_exit_if(tmr_mid == NULL);
    ret = tt_tmr_start(tmr_mid);
    __evp1_exit_if(!TT_OK(ret));

    // start long timer
    exp = tt_time_ref();
    exp += tt_time_ms2ref(300);
    tmr_long =
        tt_tmr_create(NULL, 300, (void *)exp, _timer_check, (void *)3, 0);
    __evp1_exit_if(tmr_mid == NULL);
    ret = tt_tmr_start(tmr_long);
    __evp1_exit_if(!TT_OK(ret));

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_evp_timer_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_evcenter_t evc;
    tt_evc_attr_t evc_attr;
    tt_result_t ret;

    TT_TEST_CASE_ENTER()
    // test start

    tt_evc_attr_default(&evc_attr);
    evc_attr.on_init = __evp_timer_basic;
    evc_attr.evc_name = "ut_b1";

    // create
    ret = tt_evc_create(&evc, TT_FALSE, &evc_attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // destroy
    __evp_t1_ret = TT_SUCCESS;
    ret = tt_evc_wait(&evc);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(__evp_t1_ret, TT_SUCCESS, "");

    // local run
    ret = tt_evc_create(&evc, TT_TRUE, &evc_attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_evc_wait(&evc);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

#define __T3_TIMER_NUM (10000)
#define __T3_TIME (5000)

static tt_result_t __t3_ret = TT_SUCCESS;
static tt_result_t __t3_error_line;
static tt_tmr_t *__t3_timer[__T3_TIMER_NUM];
static tt_s64_t max_delay;
static tt_u32_t created_timer_num;

static void __t3_timer_check(IN struct tt_tmr_s *timer,
                             IN void *param,
                             IN tt_u32_t reason)
{
    tt_s64_t now = tt_time_ref();
    tt_s64_t diff = tt_time_ref2ms(abs((int)(now - (tt_s64_t)timer->opaque)));
    tt_u32_t op;
    tt_u32_t op_idx = rand() % __T3_TIMER_NUM;
    tt_tmr_t *op_tmr = __t3_timer[op_idx];

    tt_evpoller_t *evp = tt_evp_current();
    tt_evcenter_t *evc = TT_CONTAINER(timer->mgr, tt_evcenter_t, tmr_mgr);
    // so that the evp 2/4 would only operate tmr at 2, 6, 10, ...

    TT_ASSERT(reason == TT_TMR_CB_EXPIRED);

    // can not check accuracy for this case, we only check stability
    /*if (diff > 10)
    {
        __t3_error_line = __LINE__;
        __t3_ret = TT_FAIL;
        return;
    }*/
    if (diff > max_delay) {
        max_delay = diff;
    }
    // - actually the delay here is not accurate, as it spent two much on
    //   creating and starting time without running evp, just for ref
    // - __AH_CONSISTENCY_CHECK may also impact accuracy

    op = rand() % 5;
    switch (op) {
        case 0: {
            // start it
            tt_s64_t abs_exp = tt_time_ref();
            abs_exp += tt_time_ms2ref(op_tmr->delay_ms);

            op_tmr->opaque = (void *)abs_exp;
            tt_tmr_start(op_tmr);
        } break;
        case 1: {
            // stop it
            tt_tmr_stop(op_tmr);
        } break;
        case 2: {
            // modify it and activate
            tt_s64_t exp = (rand() % __T3_TIME + 1000);
            tt_s64_t abs_exp = tt_time_ref();

            abs_exp += tt_time_ms2ref(exp);
            do {
                tt_tmr_set_delay(op_tmr, exp);
                tt_tmr_set_cb(op_tmr, __t3_timer_check);
                tt_tmr_set_cbparam(op_tmr, op_tmr->cb_param);
                tt_tmr_set_opaque(op_tmr, (void *)abs_exp);
                tt_tmr_start(op_tmr);
            } while (0);
        } break;
        case 3: {
            // modify it and deactivate
            tt_s64_t exp = (rand() % __T3_TIME + 1000);
            tt_s64_t abs_exp = tt_time_ref();

            abs_exp += tt_time_ms2ref(exp);
            do {
                tt_tmr_set_delay(op_tmr, exp);
                tt_tmr_set_cb(op_tmr, __t3_timer_check);
                tt_tmr_set_cbparam(op_tmr, op_tmr->cb_param);
                tt_tmr_set_opaque(op_tmr, (void *)abs_exp);
                tt_tmr_start(op_tmr);
            } while (0);
        } break;
        case 4: {
            // destroy it and restart
            tt_s64_t exp = (rand() % __T3_TIME + 1000);
            tt_s64_t abs_exp = tt_time_ref();

            tt_tmr_destroy(__t3_timer[op_idx]);
            __t3_timer[op_idx] = NULL;

            abs_exp += tt_time_ms2ref(exp);
            __t3_timer[op_idx] = tt_tmr_create(NULL,
                                               exp,
                                               (void *)abs_exp,

                                               __t3_timer_check,
                                               (void *)(tt_ptrdiff_t)op_idx,
                                               0);
            ++created_timer_num;
            tt_tmr_start(__t3_timer[op_idx]);
        } break;
        default:
            break;
    }
}

static void __t3_timer_final(IN struct tt_tmr_s *timer,
                             IN void *param,
                             IN tt_u32_t reason)
{
    tt_tmr_mgr_t *tm = timer->mgr;
    tt_tmr_t *head = tt_tmr_mgr_head(tm);
    tt_u32_t i, n = 0;
    tt_s64_t now = tt_time_ref();

    if ((head != NULL) && (head->absolute_expire_time < now)) {
        tt_s64_t diff = now - head->absolute_expire_time;

        diff = tt_time_ref2ms(diff);
        if (diff > 10) {
            TT_ERROR("some timer should have expired: %ld -> %ld, [%d]ms",
                     head->absolute_expire_time,
                     now,
                     diff);
            __t3_ret = TT_FAIL;
        }
    }

    for (i = 0; i < __T3_TIMER_NUM; ++i) {
        if (__t3_timer[i] != NULL) {
            tt_tmr_destroy(__t3_timer[i]);
            __t3_timer[i] = NULL;
            ++n;
        }
    }

    tt_evc_exit((tt_evcenter_t *)param);
}

unsigned int __evp_st_seed;

tt_result_t __evp_timer_basic_st(IN struct tt_evcenter_s *evc,
                                 IN void *on_init_param)
{
#define __evp3_exit_if(e)                                                      \
    do {                                                                       \
        if ((e)) {                                                             \
            __evp_t1_error_line = __LINE__;                                    \
            __evp_t1_ret = TT_FAIL;                                            \
            return TT_FAIL;                                                    \
        }                                                                      \
    } while (0)

    tt_result_t ret;
    tt_tmr_t *final_tmr;

    tt_u32_t i = 0;

    //__evp_st_seed = (unsigned int)time(NULL);
    __evp_st_seed = 1416299756;
    srand(__evp_st_seed);

    for (i = 0; i < __T3_TIMER_NUM; ++i) {
        tt_s64_t exp = (rand() % __T3_TIME) + 1000;
        // need time to create these timers, so + 1000 to enable accuracy of
        // max_delay
        tt_s64_t abs_exp = tt_time_ref();
        abs_exp += tt_time_ms2ref(exp);

        __t3_timer[i] = tt_tmr_create(NULL,
                                      exp,
                                      (void *)abs_exp,

                                      __t3_timer_check,
                                      (void *)(tt_uintptr_t)i,
                                      0);
        ++created_timer_num;
        __evp3_exit_if(__t3_timer[i] == NULL);

        ret = tt_tmr_start(__t3_timer[i]);
        __evp3_exit_if(!TT_OK(ret));
    }
    // would start so many timer without running evp effect timer accuracy?

    final_tmr = tt_tmr_create(NULL, __T3_TIME, 0, __t3_timer_final, evc, 0);
    __evp3_exit_if(final_tmr == NULL);
    ret = tt_tmr_start(final_tmr);
    __evp3_exit_if(!TT_OK(ret));

    return TT_SUCCESS;
}

int __show_tm;

tt_result_t __evp_timer_basic_on_uni(IN struct tt_evcenter_s *evc,
                                     IN void *on_init_param)
{
    tt_s64_t ne;

    // not recomend to call tt_tmr_mgr_run out of poller, but now all
    // pollers should have exited
    ne = tt_tmr_mgr_run(&evc->tmr_mgr);
    if (ne != TT_TIME_INFINITE) {
        TT_ERROR(
            "should be no timer left, but %d, total created: %d, wait: %ld",
            evc->tmr_mgr.tmr_heap.tail_idx,
            created_timer_num,
            ne);
        __t3_ret = TT_FAIL;
    }

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_evp_timer_st)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_evcenter_t evc;
    tt_evc_attr_t evc_attr;
    tt_result_t ret;

    tt_s64_t begin, end, diff;

    TT_TEST_CASE_ENTER()
    // test start

    tt_evc_attr_default(&evc_attr);
    evc_attr.on_init = __evp_timer_basic_st;
    evc_attr.on_exit = __evp_timer_basic_on_uni;

    begin = tt_time_ref();

    // create
    ret = tt_evc_create(&evc, TT_FALSE, &evc_attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    __evp_t1_ret = TT_SUCCESS;
    ret = tt_evc_wait(&evc);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(__evp_t1_ret, TT_SUCCESS, "");

    end = tt_time_ref();
    diff = end - begin;
    TT_RECORD_INFO("evp basic st: %dms, max_delay: %dms",
                   tt_time_ref2ms(diff),
                   max_delay);

    // destroy
    // ret = tt_evc_destroy(&evc);
    // TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    TT_TEST_CHECK_EQUAL(__t3_ret, TT_SUCCESS, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

#define __EVP_PERF_MAGIC 1239910
#define __EVP_PERF_ID 123
#define __EVP_PERF_NUM 100000

typedef struct
{
    tt_u32_t magic;
    tt_evcenter_t *sender;
} __evp_perf_ev;

static tt_evcenter_t adv_evc_snd, adv_evc_rcv;
static tt_u32_t snd_num, rcv_num;

static tt_result_t __evp_perf_on_thread_ev(IN struct tt_evpoller_s *evpoller,
                                           IN tt_ev_t *ev)
{
    __evp_perf_ev *epev = TT_EV_DATA(ev, __evp_perf_ev);
    tt_result_t ret;
    tt_evcenter_t *sender;

    TT_ASSERT(epev->magic == __EVP_PERF_MAGIC);
    TT_ASSERT(epev->sender != NULL);

    if ((evpoller->evc == &adv_evc_rcv) && (++rcv_num >= __EVP_PERF_NUM)) {
        // receiver come here
        tt_evc_exit(evpoller->evc);
        return TT_SUCCESS;
    }

    sender = epev->sender;
    epev->sender = evpoller->evc;
    ret = tt_evc_sendto_thread(sender, ev);
    TT_ASSERT(TT_OK(ret));

    if ((evpoller->evc == &adv_evc_snd) && (++snd_num >= __EVP_PERF_NUM)) {
        // sender come here
        tt_evc_exit(evpoller->evc);
        return TT_SUCCESS;
    }

    return TT_SUCCESS;
}

static tt_result_t __evp_perf_on_init(IN struct tt_evcenter_s *evc,
                                      IN void *on_init_param)
{
    tt_ev_t *ev =
        tt_thread_ev_create(__EVP_PERF_ID, sizeof(__evp_perf_ev), NULL);
    __evp_perf_ev *epev = TT_EV_DATA(ev, __evp_perf_ev);
    tt_result_t ret;
    tt_evcenter_t *sender = NULL, *receiver = NULL;

    TT_ASSERT(epev != NULL);

    sender = &adv_evc_snd;
    receiver = &adv_evc_rcv;

    epev->magic = __EVP_PERF_MAGIC;
    epev->sender = sender;

    if ((tt_uintptr_t)on_init_param % 100 == 1) {
        // this is sender on_init
        ++snd_num;
        ret = tt_evc_sendto_thread(receiver, ev);
        TT_ASSERT(TT_OK(ret));
    }

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_evp_threadev_perf)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_s64_t start, end, t2;
    tt_result_t ret;

    tt_evc_attr_t evc_attr;

    TT_TEST_CASE_ENTER()
    // test start

    tt_evc_attr_default(&evc_attr);

    // basic evp
    evc_attr.on_init = __evp_perf_on_init;
    evc_attr.thread_ev_attr.on_thread_ev = __evp_perf_on_thread_ev;

    // run
    snd_num = rcv_num = 0;
    start = tt_time_ref();

    evc_attr.on_init_param = (void *)100;
    ret = tt_evc_create(&adv_evc_rcv, TT_FALSE, &evc_attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    evc_attr.on_init_param = (void *)101;
    ret = tt_evc_create(&adv_evc_snd, TT_FALSE, &evc_attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_evc_wait(&adv_evc_rcv);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_evc_wait(&adv_evc_snd);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    end = tt_time_ref();
    t2 = end - start;

    TT_RECORD_INFO("events: %d, adv: %dms",
                   __EVP_PERF_NUM,
                   (tt_u32_t)tt_time_ref2ms(t2));

    // test end
    TT_TEST_CASE_LEAVE()
}
