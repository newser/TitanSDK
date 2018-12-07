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

#include <stdlib.h>

#include <network/http/tt_http_rule.h>
#include <network/http/tt_http_server.h>

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
TT_TEST_ROUTINE_DECLARE(case_http_rule_basic)

TT_TEST_ROUTINE_DECLARE(case_http_server_basic)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(http_svr_case)

TT_TEST_CASE("case_http_rule_basic",
             "http uri rule basic",
             case_http_rule_basic,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("case_http_server_basic",
                 "http server basic",
                 case_http_server_basic,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(http_svr_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(HTTP_UT_SVR, 0, http_svr_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(case_http_rule_basic)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    static tt_bool_t __hr1_destroyed[3];
static void __hr1_destroy(IN struct tt_http_rule_s *r)
{
    tt_u32_t idx = TT_HTTP_RULE_CAST(r, tt_u32_t);

    __hr1_destroyed[idx] = TT_TRUE;
}

static tt_bool_t __hr1_match_called[3];
static tt_bool_t __hr1_match_ret[3];
static tt_bool_t __hr1_match(IN struct tt_http_rule_s *r,
                             IN struct tt_string_s *uri,
                             IN tt_u32_t pos)
{
    tt_u32_t idx = *TT_HTTP_RULE_CAST(r, tt_u32_t);

    TT_INFO("hr[%d]", idx);
    __hr1_match_called[idx] = TT_TRUE;
    return __hr1_match_ret[idx];
}

static tt_bool_t __hr1_pre_called[3];
static tt_bool_t __hr1_pre_ret[3];
static tt_http_rule_result_t __rule_pre(IN struct tt_http_rule_s *r,
                                        IN OUT struct tt_string_s *uri,
                                        IN OUT tt_u32_t *pos)
{
    tt_u32_t idx = *TT_HTTP_RULE_CAST(r, tt_u32_t);

    __hr1_pre_called[idx] = TT_TRUE;
    return __hr1_pre_ret[idx];
}

static tt_bool_t __hr1_post_called[3];
static tt_bool_t __hr1_post_ret[3];
static tt_http_rule_result_t __rule_post(IN struct tt_http_rule_s *r,
                                         IN OUT struct tt_string_s *uri,
                                         IN OUT tt_u32_t *pos)
{
    tt_u32_t idx = *TT_HTTP_RULE_CAST(r, tt_u32_t);

    __hr1_post_called[idx] = TT_TRUE;
    return __hr1_post_ret[idx];
}

void __hr1_clear()
{
    tt_u32_t i;
    for (i = 0; i < 3; ++i) {
        __hr1_destroyed[i] = TT_FALSE;

        __hr1_match_called[i] = TT_FALSE;
        __hr1_match_ret[i] = TT_TRUE;

        __hr1_pre_called[i] = TT_FALSE;
        __hr1_pre_ret[i] = TT_HTTP_RULE_NEXT;

        __hr1_post_called[i] = TT_FALSE;
        __hr1_post_ret[i] = TT_HTTP_RULE_NEXT;
    }
}

TT_TEST_ROUTINE_DEFINE(case_http_rule_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_http_rule_t *r, *child;
    tt_http_rule_itf_t i1 = {0};
    tt_http_rule_itf_t i2 = {__hr1_destroy,
                             __hr1_match,
                             __rule_pre,
                             __rule_post};
    tt_http_rule_result_t rret;
    tt_u32_t pos, i;

    TT_TEST_CASE_ENTER()
    // test start

    r = tt_http_rule_create(0, &i1);
    TT_UT_NOT_NULL(r, "");
    tt_http_rule_release(r);

    r = tt_http_rule_create(0, &i1);
    TT_UT_NOT_NULL(r, "");

    {
        child = tt_http_rule_create(sizeof(tt_u32_t), &i2);
        *TT_HTTP_RULE_CAST(child, tt_u32_t) = 0;
        tt_http_rule_add(r, child);
        tt_http_rule_release(child);

        child = tt_http_rule_create(sizeof(tt_u32_t), &i2);
        *TT_HTTP_RULE_CAST(child, tt_u32_t) = 1;
        tt_http_rule_add(r, child);
        tt_http_rule_release(child);

        child = tt_http_rule_create(sizeof(tt_u32_t), &i2);
        *TT_HTTP_RULE_CAST(child, tt_u32_t) = 2;
        tt_http_rule_add(r, child);
        tt_http_rule_release(child);
    }

    __hr1_clear();
    // go through all
    for (i = 0; i < 3; ++i) {
        __hr1_match_ret[i] = TT_TRUE;
    }
    rret = tt_http_rule_process(r, NULL, &pos);
    TT_UT_EQUAL(rret, TT_HTTP_RULE_NEXT, "");
    for (i = 0; i < 3; ++i) {
        TT_UT_TRUE(__hr1_match_called[i], "");
        TT_UT_TRUE(__hr1_pre_called[i], "");
        TT_UT_TRUE(__hr1_post_called[i], "");
    }

    __hr1_clear();
    // the first/third match
    for (i = 0; i < 3; ++i) {
        __hr1_match_ret[i] = TT_TRUE;
    }
    __hr1_match_ret[1] = TT_FALSE;
    rret = tt_http_rule_process(r, NULL, &pos);
    TT_UT_EQUAL(rret, TT_HTTP_RULE_NEXT, "");
    for (i = 0; i < 1; ++i) {
        TT_UT_TRUE(__hr1_match_called[i], "");
        if (i == 0 || i == 3) {
            TT_UT_TRUE(__hr1_pre_called[i], "");
            TT_UT_TRUE(__hr1_post_called[i], "");
        } else {
            TT_UT_FALSE(__hr1_pre_called[i], "");
            TT_UT_FALSE(__hr1_post_called[i], "");
        }
    }

    __hr1_clear();
    // the second pre error
    for (i = 0; i < 3; ++i) {
        __hr1_match_ret[i] = TT_TRUE;
    }
    __hr1_pre_ret[1] = TT_HTTP_RULE_ERROR;
    rret = tt_http_rule_process(r, NULL, &pos);
    TT_UT_EQUAL(rret, TT_HTTP_RULE_ERROR, "");
    TT_UT_TRUE(__hr1_match_called[0], "");
    TT_UT_TRUE(__hr1_pre_called[0], "");
    TT_UT_TRUE(__hr1_post_called[0], "");
    TT_UT_TRUE(__hr1_match_called[1], "");
    TT_UT_TRUE(__hr1_pre_called[1], "");
    TT_UT_FALSE(__hr1_post_called[1], "");
    TT_UT_FALSE(__hr1_match_called[2], "");
    TT_UT_FALSE(__hr1_pre_called[2], "");
    TT_UT_FALSE(__hr1_post_called[2], "");

    __hr1_clear();
    // the second post error
    for (i = 0; i < 3; ++i) {
        __hr1_match_ret[i] = TT_TRUE;
    }
    __hr1_post_ret[1] = TT_HTTP_RULE_ERROR;
    rret = tt_http_rule_process(r, NULL, &pos);
    TT_UT_EQUAL(rret, TT_HTTP_RULE_ERROR, "");
    TT_UT_TRUE(__hr1_match_called[0], "");
    TT_UT_TRUE(__hr1_pre_called[0], "");
    TT_UT_TRUE(__hr1_post_called[0], "");
    TT_UT_TRUE(__hr1_match_called[1], "");
    TT_UT_TRUE(__hr1_pre_called[1], "");
    TT_UT_TRUE(__hr1_post_called[1], "");
    TT_UT_FALSE(__hr1_match_called[2], "");
    TT_UT_FALSE(__hr1_pre_called[2], "");
    TT_UT_FALSE(__hr1_post_called[2], "");

    __hr1_clear();
    // the infin loop
    for (i = 0; i < 3; ++i) {
        __hr1_match_ret[i] = TT_TRUE;
    }
    __hr1_post_ret[2] = TT_HTTP_RULE_CONTINUE;
    rret = tt_http_rule_process(r, NULL, &pos);
    TT_UT_EQUAL(rret, TT_HTTP_RULE_ERROR, "");

    tt_http_rule_release(r);
    for (i = 0; i < 3; ++i) {
        TT_UT_TRUE(__hr1_destroyed[i], "");
    }

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_u32_t __hs_err_line;

#define __ck_err(e)                                                            \
    do {                                                                       \
        if (!(e)) {                                                            \
            __hs_err_line = __LINE__;                                          \
            tt_task_exit(NULL);                                                \
        }                                                                      \
    } while (0)

static tt_result_t __http_svr_fb(IN void *param)
{
    tt_sktaddr_t addr;
    tt_skt_t *s;
    tt_http_server_t svr;
    tt_result_t ret;

    tt_sktaddr_init(&addr, TT_NET_AF_INET);
    tt_sktaddr_set_ip_p(&addr, "127.0.0.1");
    tt_sktaddr_set_port(&addr, 12399);

    s = tt_tcp_server(TT_NET_AF_INET, NULL, &addr);
    __ck_err(s != NULL);

    ret = tt_http_server_create_skt(&svr, s, NULL);
    __ck_err(TT_OK(ret));

    tt_http_server_run_fiber(&svr);

    tt_http_server_destroy(&svr);

    tt_task_exit(NULL);
    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(case_http_server_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_task_t t;

    TT_TEST_CASE_ENTER()
    // test start

    tt_task_create(&t, NULL);
    tt_task_add_fiber(&t, NULL, __http_svr_fb, NULL, NULL);
    tt_task_run(&t);

    tt_task_wait(&t);

    // test end
    TT_TEST_CASE_LEAVE()
}
