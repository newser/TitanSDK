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

#include <network/http/rule/tt_http_rule_startwith.h>
#include <network/http/tt_http_host.h>
#include <network/http/tt_http_host_set.h>
#include <network/http/tt_http_rule.h>
#include <network/http/tt_http_server.h>
#include <network/http/tt_http_uri.h>

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
TT_TEST_ROUTINE_DECLARE(case_http_rule_startwith)
TT_TEST_ROUTINE_DECLARE(case_http_host)
TT_TEST_ROUTINE_DECLARE(case_http_hostset)

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

    TT_TEST_CASE("case_http_rule_startwith",
                 "http uri rule: prefix",
                 case_http_rule_startwith,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_http_host",
                 "http host",
                 case_http_host,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_http_hostset",
                 "http host map",
                 case_http_hostset,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

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
    TT_TEST_ROUTINE_DEFINE(case_http_hostset)
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
    tt_u32_t idx = *TT_HTTP_RULE_CAST(r, tt_u32_t);

    __hr1_destroyed[idx] = TT_TRUE;
}

static tt_bool_t __hr1_match_called[3];
static tt_bool_t __hr1_match_ret[3];
static tt_bool_t __hr1_match(IN struct tt_http_rule_s *r,
                             IN struct tt_http_uri_s *uri,
                             IN OUT struct tt_string_s *path,
                             IN tt_http_rule_ctx_t *ctx)
{
    tt_u32_t idx = *TT_HTTP_RULE_CAST(r, tt_u32_t);

    __hr1_match_called[idx] = TT_TRUE;
    return __hr1_match_ret[idx];
}

static tt_bool_t __hr1_pre_called[3];
static tt_bool_t __hr1_pre_ret[3];
static tt_http_rule_result_t __rule_pre(IN struct tt_http_rule_s *r,
                                        IN struct tt_http_uri_s *uri,
                                        IN OUT struct tt_string_s *path,
                                        IN tt_http_rule_ctx_t *ctx)
{
    tt_u32_t idx = *TT_HTTP_RULE_CAST(r, tt_u32_t);

    __hr1_pre_called[idx] = TT_TRUE;
    return __hr1_pre_ret[idx];
}

static tt_bool_t __hr1_post_called[3];
static tt_bool_t __hr1_post_ret[3];
static tt_http_rule_result_t __rule_post(IN struct tt_http_rule_s *r,
                                         IN struct tt_http_uri_s *uri,
                                         IN OUT struct tt_string_s *path,
                                         IN tt_http_rule_ctx_t *ctx)
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
    tt_http_rule_ctx_t ctx;

    TT_TEST_CASE_ENTER()
    // test start

    tt_http_rule_ctx_init(&ctx);

    r = tt_http_rule_create(0, &i1, TT_HTTP_RULE_ERROR);
    TT_UT_NOT_NULL(r, "");
    rret = tt_http_rule_apply(r, NULL, NULL, &ctx);
    TT_UT_EQUAL(rret, TT_HTTP_RULE_ERROR, "");
    tt_http_rule_release(r);

    r = tt_http_rule_create(0, &i1, TT_HTTP_RULE_NEXT);
    TT_UT_NOT_NULL(r, "");

    {
        child = tt_http_rule_create(sizeof(tt_u32_t), &i2, TT_HTTP_RULE_NEXT);
        *TT_HTTP_RULE_CAST(child, tt_u32_t) = 0;
        tt_http_rule_add(r, child);
        tt_http_rule_release(child);

        child = tt_http_rule_create(sizeof(tt_u32_t), &i2, TT_HTTP_RULE_NEXT);
        *TT_HTTP_RULE_CAST(child, tt_u32_t) = 1;
        tt_http_rule_add(r, child);
        tt_http_rule_release(child);

        child = tt_http_rule_create(sizeof(tt_u32_t), &i2, TT_HTTP_RULE_NEXT);
        *TT_HTTP_RULE_CAST(child, tt_u32_t) = 2;
        tt_http_rule_add(r, child);
        tt_http_rule_release(child);
    }

    __hr1_clear();
    // go through all
    for (i = 0; i < 3; ++i) {
        __hr1_match_ret[i] = TT_TRUE;
    }
    rret = tt_http_rule_apply(r, NULL, NULL, &ctx);
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
    rret = tt_http_rule_apply(r, NULL, NULL, &ctx);
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
    rret = tt_http_rule_apply(r, NULL, NULL, &ctx);
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
    rret = tt_http_rule_apply(r, NULL, NULL, &ctx);
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
    // continue
    for (i = 0; i < 3; ++i) {
        __hr1_match_ret[i] = TT_TRUE;
    }
    __hr1_post_ret[1] = TT_HTTP_RULE_CONTINUE;
    rret = tt_http_rule_apply(r, NULL, NULL, &ctx);
    TT_UT_EQUAL(rret, TT_HTTP_RULE_CONTINUE, "");
    for (i = 0; i < 2; ++i) {
        TT_UT_TRUE(__hr1_match_called[i], "");
        TT_UT_TRUE(__hr1_pre_called[i], "");
        TT_UT_TRUE(__hr1_post_called[i], "");
    }
    TT_UT_FALSE(__hr1_match_called[2], "");
    TT_UT_FALSE(__hr1_pre_called[2], "");
    TT_UT_FALSE(__hr1_post_called[2], "");

    tt_http_rule_release(r);
    for (i = 0; i < 3; ++i) {
        TT_UT_TRUE(__hr1_destroyed[i], "");
    }

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_http_rule_startwith)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_http_rule_t *r, *r2;
    tt_http_uri_t uri;
    tt_http_rule_result_t rret;
    tt_http_rule_ctx_t ctx;
    tt_string_t s;

    TT_TEST_CASE_ENTER()
    // test start

    tt_http_uri_init(&uri);
    tt_string_init(&s, NULL);

    r = tt_http_rule_startwith_create("/", NULL, TT_HTTP_RULE_BREAK);
    TT_UT_EQUAL(r->default_result, TT_HTTP_RULE_BREAK, "");
    TT_UT_NOT_NULL(r, "");

    // not match
    tt_http_rule_ctx_init(&ctx);
    rret = tt_http_rule_apply(r, &uri, &s, &ctx);
    TT_UT_EQUAL(rret, TT_HTTP_RULE_NEXT, "");
    TT_UT_FALSE(ctx.path_modified, "");

    // whole match
    tt_string_set(&s, "/");
    tt_http_rule_ctx_init(&ctx);
    rret = tt_http_rule_apply(r, &uri, &s, &ctx);
    TT_UT_EQUAL(rret, TT_HTTP_RULE_BREAK, "");
    TT_UT_FALSE(ctx.path_modified, "");

    // partial match
    tt_string_set(&s, "//abc");
    tt_http_rule_ctx_init(&ctx);
    rret = tt_http_rule_apply(r, &uri, &s, &ctx);
    TT_UT_EQUAL(rret, TT_HTTP_RULE_BREAK, "");
    TT_UT_FALSE(ctx.path_modified, "");

    // not match
    tt_string_set(&s, "a/");
    tt_http_rule_ctx_init(&ctx);
    rret = tt_http_rule_apply(r, &uri, &s, &ctx);
    TT_UT_EQUAL(rret, TT_HTTP_RULE_NEXT, "");
    TT_UT_FALSE(ctx.path_modified, "");

    {
        r2 = tt_http_rule_startwith_create("abc", NULL, TT_HTTP_RULE_BREAK);
        TT_UT_NOT_NULL(r, "");
        tt_http_rule_add(r, r2);
        tt_http_rule_release(r2);

        r2 = tt_http_rule_startwith_create("xyz", "x", TT_HTTP_RULE_BREAK);
        TT_UT_NOT_NULL(r, "");
        tt_http_rule_add(r, r2);
        tt_http_rule_release(r2);

        r2 = tt_http_rule_startwith_create("123", "12345", TT_HTTP_RULE_BREAK);
        TT_UT_NOT_NULL(r, "");
        tt_http_rule_add(r, r2);
        tt_http_rule_release(r2);

        // not match
        tt_string_set(&s, "a/");
        tt_http_rule_ctx_init(&ctx);
        rret = tt_http_rule_apply(r, &uri, &s, &ctx);
        TT_UT_EQUAL(rret, TT_HTTP_RULE_NEXT, "");
        TT_UT_FALSE(ctx.path_modified, "");

        // match first
        tt_string_set(&s, "/ab");
        tt_http_rule_ctx_init(&ctx);
        rret = tt_http_rule_apply(r, &uri, &s, &ctx);
        TT_UT_EQUAL(rret, TT_HTTP_RULE_BREAK, "");
        TT_UT_EQUAL(ctx.path_pos, 1, "");
        TT_UT_FALSE(ctx.path_modified, "");

        // match second
        tt_string_set(&s, "/abc");
        tt_http_rule_ctx_init(&ctx);
        rret = tt_http_rule_apply(r, &uri, &s, &ctx);
        TT_UT_EQUAL(rret, TT_HTTP_RULE_BREAK, "");
        TT_UT_EQUAL(ctx.path_pos, 4, "");
        TT_UT_FALSE(ctx.path_modified, "");

        // more than matching
        tt_string_set(&s, "/abc/");
        tt_http_rule_ctx_init(&ctx);
        rret = tt_http_rule_apply(r, &uri, &s, &ctx);
        TT_UT_EQUAL(rret, TT_HTTP_RULE_BREAK, "");
        TT_UT_EQUAL(ctx.path_pos, 4, "");
    }

    // replace
    {
        tt_string_set(&s, "/xyz/");
        tt_http_rule_ctx_init(&ctx);
        rret = tt_http_rule_apply(r, &uri, &s, &ctx);
        TT_UT_EQUAL(rret, TT_HTTP_RULE_BREAK, "");
        TT_UT_STREQ(tt_string_cstr(&s), "/x/", "");
        TT_UT_EQUAL(ctx.path_pos, 2, "");
        TT_UT_TRUE(ctx.path_modified, "");

        tt_string_set(&s, "/123999/");
        tt_http_rule_ctx_init(&ctx);
        rret = tt_http_rule_apply(r, &uri, &s, &ctx);
        TT_UT_EQUAL(rret, TT_HTTP_RULE_BREAK, "");
        TT_UT_STREQ(tt_string_cstr(&s), "/12345999/", "");
        TT_UT_EQUAL(ctx.path_pos, 6, "");
        TT_UT_TRUE(ctx.path_modified, "");
    }

    tt_http_rule_release(r);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_http_host)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_http_host_t *hh;
    tt_http_rule_t *child;
    tt_http_rule_itf_t i2 = {__hr1_destroy,
                             __hr1_match,
                             __rule_pre,
                             __rule_post};
    tt_http_rule_result_t rret;
    tt_u32_t pos, i;
    tt_http_uri_t uri;

    TT_TEST_CASE_ENTER()
    // test start

    tt_http_uri_init(&uri);

    hh = tt_http_host_create("123", tt_http_host_match_cmp);
    TT_UT_NOT_NULL(hh, "");
    tt_http_host_destroy(hh);

    hh = tt_http_host_create("123", NULL);
    TT_UT_NOT_NULL(hh, "");

    {
        child = tt_http_rule_create(sizeof(tt_u32_t), &i2, TT_HTTP_RULE_NEXT);
        *TT_HTTP_RULE_CAST(child, tt_u32_t) = 0;
        tt_http_host_add_rule(hh, child);
        tt_http_rule_release(child);

        child = tt_http_rule_create(sizeof(tt_u32_t), &i2, TT_HTTP_RULE_NEXT);
        *TT_HTTP_RULE_CAST(child, tt_u32_t) = 1;
        tt_http_host_add_rule(hh, child);
        tt_http_rule_release(child);

        child = tt_http_rule_create(sizeof(tt_u32_t), &i2, TT_HTTP_RULE_NEXT);
        *TT_HTTP_RULE_CAST(child, tt_u32_t) = 2;
        tt_http_host_add_rule(hh, child);
        tt_http_rule_release(child);
    }

    __hr1_clear();
    // go through all
    for (i = 0; i < 3; ++i) {
        __hr1_match_ret[i] = TT_TRUE;
    }
    rret = tt_http_host_apply(hh, &uri);
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
    rret = tt_http_host_apply(hh, &uri);
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
    rret = tt_http_host_apply(hh, &uri);
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
    rret = tt_http_host_apply(hh, &uri);
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
    // continue
    for (i = 0; i < 3; ++i) {
        __hr1_match_ret[i] = TT_TRUE;
    }
    __hr1_post_ret[1] = TT_HTTP_RULE_CONTINUE;
    rret = tt_http_host_apply(hh, &uri);
    TT_UT_EQUAL(rret, TT_HTTP_RULE_ERROR, "");

    tt_http_host_destroy(hh);
    for (i = 0; i < 3; ++i) {
        TT_UT_TRUE(__hr1_destroyed[i], "");
    }

    // match any
    {
        hh = tt_http_host_create("123", NULL);
        TT_UT_NOT_NULL(hh, "");

        TT_UT_TRUE(tt_http_host_match(hh, NULL, 0), "");

        tt_http_host_destroy(hh);
    }

    tt_http_uri_destroy(&uri);

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
    tt_http_hostset_t *hs;
    tt_http_host_t *ho;

    tt_sktaddr_init(&addr, TT_NET_AF_INET);
    tt_sktaddr_set_ip_p(&addr, "127.0.0.1");
    tt_sktaddr_set_port(&addr, 12399);

    // set host
    {
        tt_http_rule_t *r;

        ho = tt_http_host_create("does not matter", NULL);
        __ck_err(ho != NULL);

        r = tt_http_rule_startwith_create("/",
                                          "/index.html",
                                          TT_HTTP_RULE_BREAK);
        __ck_err(r != NULL);
        tt_http_host_add_rule(ho, r);
    }

    {
        hs = tt_current_http_hostset(TT_TRUE);
        __ck_err(hs != NULL);

        tt_http_hostset_add(hs, ho);
    }

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

    return TT_SUCCESS;

    tt_task_create(&t, NULL);
    tt_task_add_fiber(&t, NULL, __http_svr_fb, NULL, NULL);
    tt_task_run(&t);

    tt_task_wait(&t);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_http_hostset)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_http_hostset_t m, *pm;
    tt_http_host_t *h1, *h2, *h[3];

    TT_TEST_CASE_ENTER()
    // test start

    tt_http_hostset_init(&m);
    tt_http_hostset_destroy(&m);

    tt_http_hostset_init(&m);

    // find empty map
    {
        TT_UT_NULL(tt_http_hostset_match(&m, ""), "");
        TT_UT_NULL(tt_http_hostset_match(&m, "1"), "");
    }

    // add default
    h1 = tt_http_host_create("test.com", NULL);
    TT_UT_NOT_NULL(h1, "");
    tt_http_hostset_set_default(&m, h1);
    TT_UT_EQUAL(m.default_host, h1, "");

    h2 = tt_http_host_create("test.com:80", NULL);
    TT_UT_NOT_NULL(h2, "");
    tt_http_hostset_set_default(&m, h2);
    TT_UT_EQUAL(m.default_host, h2, "");

    // find empty map but with default
    {
        TT_UT_EQUAL(tt_http_hostset_match(&m, ""), h2, "");
        TT_UT_EQUAL(tt_http_hostset_match(&m, "1"), h2, "");
        TT_UT_EQUAL(tt_http_hostset_match(&m, "test.com:80"), h2, "");
    }

    // add some host
    h[0] = tt_http_host_create("0", tt_http_host_match_cmp);
    TT_UT_NOT_NULL(h[0], "");
    tt_http_hostset_add(&m, h[0]);

    h[1] = tt_http_host_create("1", tt_http_host_match_cmp);
    TT_UT_NOT_NULL(h[1], "");
    tt_http_hostset_add(&m, h[1]);

    h[2] = tt_http_host_create("2", tt_http_host_match_cmp);
    TT_UT_NOT_NULL(h[2], "");
    tt_http_hostset_add(&m, h[2]);

    {
        TT_UT_EQUAL(tt_http_hostset_match(&m, ""), h2, "");
        TT_UT_EQUAL(tt_http_hostset_match(&m, "11"), h2, "");
        TT_UT_EQUAL(tt_http_hostset_match(&m, "test.com:80"), h2, "");

        TT_UT_EQUAL(tt_http_hostset_match(&m, "0"), h[0], "");
        TT_UT_EQUAL(tt_http_hostset_match(&m, "1"), h[1], "");
        TT_UT_EQUAL(tt_http_hostset_match(&m, "2"), h[2], "");
    }

    tt_http_hostset_destroy(&m);

    // test end
    TT_TEST_CASE_LEAVE()
}
