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

#include <network/http/tt_http_parser.h>
#include <network/http/tt_http_render.h>
#include <network/http/tt_http_service_manager.h>

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
TT_TEST_ROUTINE_DECLARE(case_http_render_req)
TT_TEST_ROUTINE_DECLARE(case_http_render_resp)
TT_TEST_ROUTINE_DECLARE(case_http_svcmgr)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(http_render_case)

TT_TEST_CASE("case_http_render_req",
             "http render req",
             case_http_render_req,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("case_http_render_resp",
                 "http render resp",
                 case_http_render_resp,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_http_svcmgr",
                 "http service manager",
                 case_http_svcmgr,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(http_render_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(HTTP_UT_RENDER, 0, http_render_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(case_http_svcmgr)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(case_http_render_req)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_http_req_render_t r;
    tt_result_t ret;
    tt_http_hdr_t *h;
    tt_uri_t *u;

    TT_TEST_CASE_ENTER()
    // test start

    tt_http_req_render_init(&r, NULL);

    TT_UT_EQUAL(tt_http_req_render_get_uri(&r), &r.uri, "");

    TT_UT_EQUAL(tt_http_req_render_get_method(&r), TT_HTTP_METHOD_NUM, "");
    tt_http_req_render_set_method(&r, TT_HTTP_MTD_GET);
    TT_UT_EQUAL(tt_http_req_render_get_method(&r), TT_HTTP_MTD_GET, "");

    TT_UT_EQUAL(tt_http_req_render_get_version(&r), TT_HTTP_VER_NUM, "");
    tt_http_req_render_set_version(&r, TT_HTTP_V1_1);
    TT_UT_EQUAL(tt_http_req_render_get_version(&r), TT_HTTP_V1_1, "");

    u = tt_http_req_render_get_uri(&r);
    TT_UT_SUCCESS(tt_uri_set_path(u, "/a/b/c"), "");

    // no header
    {
        const tt_char_t *msg = "GET /a/b/c HTTP/1.1\r\n\r\n";
        tt_char_t *data;
        tt_u32_t len;

        TT_UT_SUCCESS(tt_http_req_render(&r, &data, &len), "");
        TT_UT_EQUAL(len, tt_strlen(msg), "");
        TT_UT_EXP(tt_strncmp(data, msg, len) == 0, "");
    }

    tt_http_req_render_clear(&r);
    TT_UT_EQUAL(tt_http_req_render_get_method(&r), TT_HTTP_METHOD_NUM, "");
    TT_UT_EQUAL(tt_http_req_render_get_version(&r), TT_HTTP_VER_NUM, "");
    TT_UT_EQUAL(tt_uri_render(u, NULL)[0], 0, "");
    tt_http_req_render_set_method(&r, TT_HTTP_MTD_POST);
    tt_http_req_render_set_version(&r, TT_HTTP_V1_0);
    TT_UT_SUCCESS(tt_uri_set_path(u, "aaa.com"), "");

    // add a header
    TT_UT_SUCCESS(tt_http_req_render_add_line(&r, TT_HTTP_HDR_HOST, "sample"),
                  "");
    {
        const tt_char_t *msg =
            "POST aaa.com HTTP/1.0\r\n"
            "Host: sample\r\n"
            "\r\n";
        tt_char_t *data;
        tt_u32_t len;

        TT_UT_SUCCESS(tt_http_req_render(&r, &data, &len), "");
        TT_UT_EQUAL(len, tt_strlen(msg), "");
        TT_UT_EXP(tt_strncmp(data, msg, len) == 0, "");
    }

    // 2 headers
    {
        tt_blobex_t b[3];
        const tt_char_t *msg =
            "POST aaa.com HTTP/1.0\r\n"
            "Host: sample\r\n"
            "Date: 1, 22, 333\r\n"
            "\r\n";
        tt_char_t *data;
        tt_u32_t len;

        tt_blobex_set(&b[0], (tt_u8_t *)"1", 1, TT_FALSE);
        tt_blobex_set(&b[1], (tt_u8_t *)"22", 2, TT_FALSE);
        tt_blobex_set(&b[2], (tt_u8_t *)"333", 3, TT_FALSE);
        TT_UT_SUCCESS(tt_http_req_render_add_cs(&r, TT_HTTP_HDR_DATE, b, 3),
                      "");

        TT_UT_SUCCESS(tt_http_req_render(&r, &data, &len), "");
        TT_UT_EQUAL(len, tt_strlen(msg), "");
        TT_UT_EXP(tt_strncmp(data, msg, len) == 0, "");

        {
            const tt_char_t *msg =
                "POST aaa.com HTTP/1.0\r\n"
                "Connection: close\r\n"
                "Host: sample\r\n"
                "Date: 1, 22, 333\r\n"
                "\r\n";

            tt_http_req_render_set_conn(&r, TT_HTTP_CONN_CLOSE);
            TT_UT_SUCCESS(tt_http_req_render(&r, &data, &len), "");
            TT_UT_EQUAL(len, tt_strlen(msg), "");
            TT_UT_EXP(tt_strncmp(data, msg, len) == 0, "");
        }
        {
            const tt_char_t *msg =
                "POST aaa.com HTTP/1.0\r\n"
                "Connection: keep-alive\r\n"
                "Host: sample\r\n"
                "Date: 1, 22, 333\r\n"
                "\r\n";

            tt_http_req_render_set_conn(&r, TT_HTTP_CONN_KEEP_ALIVE);
            TT_UT_SUCCESS(tt_http_req_render(&r, &data, &len), "");
            TT_UT_EQUAL(len, tt_strlen(msg), "");
            TT_UT_EXP(tt_strncmp(data, msg, len) == 0, "");
        }
        {
            tt_http_req_render_set_conn(&r, TT_HTTP_CONN_NONE);
            TT_UT_SUCCESS(tt_http_req_render(&r, &data, &len), "");
            TT_UT_EQUAL(len, tt_strlen(msg), "");
            TT_UT_EXP(tt_strncmp(data, msg, len) == 0, "");
        }
    }

    tt_http_req_render_destroy(&r);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_http_render_resp)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_http_resp_render_t r;
    tt_result_t ret;
    tt_http_hdr_t *h;

    TT_TEST_CASE_ENTER()
    // test start

    tt_http_resp_render_init(&r, NULL);

    TT_UT_EQUAL(tt_http_resp_render_get_status(&r), TT_HTTP_STATUS_INVALID, "");
    tt_http_resp_render_set_status(&r, TT_HTTP_STATUS_OK);
    TT_UT_EQUAL(tt_http_resp_render_get_status(&r), TT_HTTP_STATUS_OK, "");

    TT_UT_EQUAL(tt_http_resp_render_get_version(&r), TT_HTTP_VER_NUM, "");
    tt_http_resp_render_set_version(&r, TT_HTTP_V1_1);
    TT_UT_EQUAL(tt_http_resp_render_get_version(&r), TT_HTTP_V1_1, "");

    // no header
    {
        const tt_char_t *msg = "HTTP/1.1 200 OK\r\n\r\n";
        tt_char_t *data;
        tt_u32_t len;

        TT_UT_SUCCESS(tt_http_resp_render(&r, &data, &len), "");
        TT_UT_EQUAL(len, tt_strlen(msg), "");
        TT_UT_EXP(tt_strncmp(data, msg, len) == 0, "");
    }

    tt_http_resp_render_clear(&r);
    TT_UT_EQUAL(tt_http_resp_render_get_status(&r), TT_HTTP_STATUS_INVALID, "");
    TT_UT_EQUAL(tt_http_resp_render_get_version(&r), TT_HTTP_VER_NUM, "");
    tt_http_resp_render_set_status(&r, TT_HTTP_STATUS_OK);
    tt_http_resp_render_set_version(&r, TT_HTTP_V1_0);

    // add a header
    TT_UT_SUCCESS(tt_http_resp_render_add_line(&r, TT_HTTP_HDR_HOST, "sample"),
                  "");
    {
        const tt_char_t *msg =
            "HTTP/1.0 200 OK\r\n"
            "Host: sample\r\n"
            "\r\n";
        tt_char_t *data;
        tt_u32_t len;

        TT_UT_SUCCESS(tt_http_resp_render(&r, &data, &len), "");
        TT_UT_EQUAL(len, tt_strlen(msg), "");
        TT_UT_EXP(tt_strncmp(data, msg, len) == 0, "");
    }

    // 2 headers
    {
        tt_blobex_t b[3];
        const tt_char_t *msg =
            "HTTP/1.0 200 OK\r\n"
            "Host: sample\r\n"
            "Date: 1, 22, 333\r\n"
            "\r\n";
        tt_char_t *data;
        tt_u32_t len;

        tt_blobex_set(&b[0], (tt_u8_t *)"1", 1, TT_FALSE);
        tt_blobex_set(&b[1], (tt_u8_t *)"22", 2, TT_FALSE);
        tt_blobex_set(&b[2], (tt_u8_t *)"333", 3, TT_FALSE);
        TT_UT_SUCCESS(tt_http_resp_render_add_cs(&r, TT_HTTP_HDR_DATE, b, 3),
                      "");

        TT_UT_SUCCESS(tt_http_resp_render(&r, &data, &len), "");
        TT_UT_EQUAL(len, tt_strlen(msg), "");
        TT_UT_EXP(tt_strncmp(data, msg, len) == 0, "");

        {
            const tt_char_t *msg =
                "HTTP/1.0 200 OK\r\n"
                "Connection: close\r\n"
                "Host: sample\r\n"
                "Date: 1, 22, 333\r\n"
                "\r\n";

            tt_http_resp_render_set_conn(&r, TT_HTTP_CONN_CLOSE);
            TT_UT_SUCCESS(tt_http_resp_render(&r, &data, &len), "");
            TT_UT_EQUAL(len, tt_strlen(msg), "");
            TT_UT_EXP(tt_strncmp(data, msg, len) == 0, "");
        }
        {
            const tt_char_t *msg =
                "HTTP/1.0 200 OK\r\n"
                "Connection: keep-alive\r\n"
                "Host: sample\r\n"
                "Date: 1, 22, 333\r\n"
                "\r\n";

            tt_http_resp_render_set_conn(&r, TT_HTTP_CONN_KEEP_ALIVE);
            TT_UT_SUCCESS(tt_http_resp_render(&r, &data, &len), "");
            TT_UT_EQUAL(len, tt_strlen(msg), "");
            TT_UT_EXP(tt_strncmp(data, msg, len) == 0, "");
        }
        {
            tt_http_resp_render_set_conn(&r, TT_HTTP_CONN_NONE);
            TT_UT_SUCCESS(tt_http_resp_render(&r, &data, &len), "");
            TT_UT_EQUAL(len, tt_strlen(msg), "");
            TT_UT_EXP(tt_strncmp(data, msg, len) == 0, "");
        }
    }

    tt_http_resp_render_destroy(&r);

    // test end
    TT_TEST_CASE_LEAVE()
}

#define __IS_NUM 3
#define __OS_NUM 3

typedef struct __is_idx_s
{
    tt_u32_t idx;
} __is_idx_t;

static tt_bool_t __is_destroyed[__IS_NUM];
static void __inserv_destroy(IN struct tt_http_inserv_s *s)
{
    __is_idx_t *i = TT_HTTP_INSERV_CAST(s, __is_idx_t);
    __is_destroyed[i->idx] = TT_TRUE;
}

static tt_bool_t __is_clear[__IS_NUM];
static void __inserv_clear(IN struct tt_http_inserv_s *s)
{
    __is_idx_t *i = TT_HTTP_INSERV_CAST(s, __is_idx_t);
    __is_clear[i->idx] = TT_TRUE;
}

static tt_http_inserv_action_t __is_on_uri_act[__IS_NUM];
static tt_bool_t __is_on_uri_called[__IS_NUM];
static tt_http_inserv_action_t __is_on_uri(
    IN struct tt_http_inserv_s *s,
    IN struct tt_http_parser_s *req,
    OUT struct tt_http_resp_render_s *resp)
{
    __is_idx_t *i = TT_HTTP_INSERV_CAST(s, __is_idx_t);
    __is_on_uri_called[i->idx] = TT_TRUE;
    return __is_on_uri_act[i->idx];
}

static tt_http_inserv_action_t __is_on_header_act[__IS_NUM];
static tt_bool_t __is_on_header_called[__IS_NUM];
static tt_http_inserv_action_t __is_on_header(
    IN struct tt_http_inserv_s *s,
    IN struct tt_http_parser_s *req,
    OUT struct tt_http_resp_render_s *resp)
{
    __is_idx_t *i = TT_HTTP_INSERV_CAST(s, __is_idx_t);
    __is_on_header_called[i->idx] = TT_TRUE;
    return __is_on_header_act[i->idx];
}

static tt_http_inserv_action_t __is_on_body_act[__IS_NUM];
static tt_bool_t __is_on_body_called[__IS_NUM];
static tt_http_inserv_action_t __is_on_body(
    IN struct tt_http_inserv_s *s,
    IN struct tt_http_parser_s *req,
    OUT struct tt_http_resp_render_s *resp)
{
    __is_idx_t *i = TT_HTTP_INSERV_CAST(s, __is_idx_t);
    __is_on_body_called[i->idx] = TT_TRUE;
    return __is_on_body_act[i->idx];
}

static tt_http_inserv_action_t __is_on_trailing_act[__IS_NUM];
static tt_bool_t __is_on_trailing_called[__IS_NUM];
static tt_http_inserv_action_t __is_on_trailing(
    IN struct tt_http_inserv_s *s,
    IN struct tt_http_parser_s *req,
    OUT struct tt_http_resp_render_s *resp)
{
    __is_idx_t *i = TT_HTTP_INSERV_CAST(s, __is_idx_t);
    __is_on_trailing_called[i->idx] = TT_TRUE;
    return __is_on_trailing_act[i->idx];
}

static tt_http_inserv_action_t __is_on_complete_act[__IS_NUM];
static tt_bool_t __is_on_complete_called[__IS_NUM];
static tt_http_inserv_action_t __is_on_complete(
    IN struct tt_http_inserv_s *s,
    IN struct tt_http_parser_s *req,
    OUT struct tt_http_resp_render_s *resp)
{
    __is_idx_t *i = TT_HTTP_INSERV_CAST(s, __is_idx_t);
    __is_on_complete_called[i->idx] = TT_TRUE;
    return __is_on_complete_act[i->idx];
}

static tt_bool_t __os_destroyed[__OS_NUM];
static void __outserv_destroy(IN struct tt_http_outserv_s *s)
{
    __is_idx_t *i = TT_HTTP_INSERV_CAST(s, __is_idx_t);
    __os_destroyed[i->idx] = TT_TRUE;
}

static tt_bool_t __os_clear[__OS_NUM];
static void __outserv_clear(IN struct tt_http_outserv_s *s)
{
    __is_idx_t *i = TT_HTTP_INSERV_CAST(s, __is_idx_t);
    __os_clear[i->idx] = TT_TRUE;
}

static tt_bool_t __os_on_resp[__OS_NUM];
static void __outserv_on_resp(IN struct tt_http_outserv_s *s,
                              IN struct tt_http_parser_s *req,
                              IN OUT struct tt_http_resp_render_s *resp)
{
    __is_idx_t *i = TT_HTTP_INSERV_CAST(s, __is_idx_t);
    __os_on_resp[i->idx] = TT_TRUE;
}

TT_TEST_ROUTINE_DEFINE(case_http_svcmgr)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_http_svcmgr_t sm;
    tt_http_inserv_t *is[3];
    tt_http_inserv_itf_t is_itf = {
        __inserv_destroy, __inserv_clear,
    };
    tt_http_inserv_cb_t is_cb = {__is_on_uri,
                                 __is_on_header,
                                 __is_on_body,
                                 __is_on_trailing,
                                 __is_on_complete};
    tt_http_outserv_itf_t __os_itf = {
        __outserv_destroy, __outserv_clear,
    };
    tt_http_outserv_cb_t __os_cb = {
        __outserv_on_resp,
    };

    tt_http_parser_t req;
    tt_http_resp_render_t resp;

    TT_TEST_CASE_ENTER()
    // test start

    (void)req;
    tt_http_resp_render_init(&resp, NULL);

    tt_http_svcmgr_init(&sm);
    tt_http_svcmgr_destroy(&sm);

    {
        tt_u32_t i = 0;

        tt_http_svcmgr_init(&sm);

        for (i = 0; i < __IS_NUM; ++i) {
            is[i] = tt_http_inserv_create(sizeof(__is_idx_t), &is_itf, &is_cb);
            __is_idx_t *ii = TT_HTTP_INSERV_CAST(is[i], __is_idx_t);
            ii->idx = i;

            tt_http_svcmgr_add_inserv(&sm, is[i]);
        }

        tt_memset(__is_clear, 0, sizeof(__is_clear));
        tt_http_svcmgr_clear(&sm);
        for (i = 0; i < __IS_NUM; ++i) {
            TT_UT_TRUE(__is_clear[i], "");
        }

        // on uri
        {
            tt_memset(__is_on_uri_called, 0, sizeof(__is_on_uri_called));
            for (i = 0; i < __IS_NUM; ++i) {
                __is_on_uri_act[i] = TT_HTTP_INSERV_ACT_IGNORE;
            }
            TT_UT_EQUAL(tt_http_svcmgr_on_uri(&sm, NULL, NULL),
                        TT_HTTP_INSERV_ACT_IGNORE,
                        "");
            for (i = 0; i < __IS_NUM; ++i) {
                TT_UT_TRUE(__is_on_uri_called[i], "");
            }

            tt_memset(__is_on_uri_called, 0, sizeof(__is_on_uri_called));
            __is_on_uri_act[1] = TT_HTTP_INSERV_ACT_DISCARD;
            TT_UT_EQUAL(tt_http_svcmgr_on_uri(&sm, NULL, NULL),
                        TT_HTTP_INSERV_ACT_DISCARD,
                        "");
            __is_on_uri_act[1] = TT_HTTP_INSERV_ACT_IGNORE;
            TT_UT_TRUE(__is_on_uri_called[0], "");
            TT_UT_TRUE(__is_on_uri_called[1], "");
            for (i = 2; i < __IS_NUM; ++i) {
                TT_UT_FALSE(__is_on_uri_called[i], "");
            }
            TT_UT_EQUAL(tt_http_svcmgr_on_header(&sm, NULL, &resp),
                        TT_HTTP_INSERV_ACT_DISCARD,
                        "");
            TT_UT_EQUAL(tt_http_svcmgr_on_body(&sm, NULL, &resp),
                        TT_HTTP_INSERV_ACT_DISCARD,
                        "");
            TT_UT_EQUAL(tt_http_svcmgr_on_trailing(&sm, NULL, &resp),
                        TT_HTTP_INSERV_ACT_DISCARD,
                        "");
            TT_UT_EQUAL(tt_http_svcmgr_on_complete(&sm, NULL, &resp),
                        TT_HTTP_INSERV_ACT_DISCARD,
                        "");

            tt_http_svcmgr_clear(&sm);

            tt_memset(__is_on_uri_called, 0, sizeof(__is_on_uri_called));
            __is_on_uri_act[0] = TT_HTTP_INSERV_ACT_OWNER;
            __is_on_uri_act[2] = TT_HTTP_INSERV_ACT_OWNER;
            TT_UT_EQUAL(tt_http_svcmgr_on_uri(&sm, NULL, &resp),
                        TT_HTTP_INSERV_ACT_OWNER,
                        "");
            __is_on_uri_act[0] = TT_HTTP_INSERV_ACT_IGNORE;
            __is_on_uri_act[2] = TT_HTTP_INSERV_ACT_IGNORE;
            for (i = 0; i < __IS_NUM; ++i) {
                TT_UT_TRUE(__is_on_uri_called[i], "");
            }
            TT_UT_EQUAL(sm.owner, is[0], "");
        }

        // on header
        {
            tt_http_svcmgr_clear(&sm);

            tt_memset(__is_on_header_called, 0, sizeof(__is_on_header_called));
            for (i = 0; i < __IS_NUM; ++i) {
                __is_on_header_act[i] = TT_HTTP_INSERV_ACT_IGNORE;
            }
            TT_UT_EQUAL(tt_http_svcmgr_on_header(&sm, NULL, NULL),
                        TT_HTTP_INSERV_ACT_IGNORE,
                        "");
            for (i = 0; i < __IS_NUM; ++i) {
                TT_UT_TRUE(__is_on_header_called[i], "");
            }

            // no owner
            tt_memset(__is_on_body_called, 0, sizeof(__is_on_body_called));
            TT_UT_EQUAL(tt_http_svcmgr_on_body(&sm, NULL, &resp),
                        TT_HTTP_INSERV_ACT_DISCARD,
                        "");
            TT_UT_EQUAL(sm.discarding, TT_TRUE, "");
            for (i = 0; i < __IS_NUM; ++i) {
                TT_UT_FALSE(__is_on_body_called[i], "");
            }

            tt_http_svcmgr_clear(&sm);

            tt_memset(__is_on_header_called, 0, sizeof(__is_on_header_called));
            __is_on_header_act[0] = TT_HTTP_INSERV_ACT_CLOSE;
            TT_UT_EQUAL(tt_http_svcmgr_on_header(&sm, NULL, NULL),
                        TT_HTTP_INSERV_ACT_CLOSE,
                        "");
            __is_on_header_act[0] = TT_HTTP_INSERV_ACT_IGNORE;
            TT_UT_TRUE(__is_on_header_called[0], "");
            for (i = 1; i < __IS_NUM; ++i) {
                TT_UT_FALSE(__is_on_header_called[i], "");
            }

            tt_memset(__is_on_header_called, 0, sizeof(__is_on_header_called));
            __is_on_header_act[1] = TT_HTTP_INSERV_ACT_OWNER;
            __is_on_header_act[2] = TT_HTTP_INSERV_ACT_OWNER;
            TT_UT_EQUAL(tt_http_svcmgr_on_header(&sm, NULL, NULL),
                        TT_HTTP_INSERV_ACT_OWNER,
                        "");
            __is_on_header_act[1] = TT_HTTP_INSERV_ACT_IGNORE;
            __is_on_header_act[2] = TT_HTTP_INSERV_ACT_IGNORE;
            TT_UT_EQUAL(sm.owner, is[1], "");
            for (i = 0; i < __IS_NUM; ++i) {
                TT_UT_TRUE(__is_on_header_called[i], "");
            }

            // a owner
            tt_memset(__is_on_body_called, 0, sizeof(__is_on_body_called));
            __is_on_body_act[1] = TT_HTTP_INSERV_ACT_OWNER;
            TT_UT_EQUAL(tt_http_svcmgr_on_body(&sm, NULL, &resp),
                        TT_HTTP_INSERV_ACT_OWNER,
                        "");
            __is_on_body_act[1] = TT_HTTP_INSERV_ACT_IGNORE;
            for (i = 0; i < __IS_NUM; ++i) {
                if (i == 1) {
                    TT_UT_TRUE(__is_on_body_called[i], "");
                } else {
                    TT_UT_FALSE(__is_on_body_called[i], "");
                }
            }

            // on trailing
            tt_memset(__is_on_trailing_called,
                      0,
                      sizeof(__is_on_trailing_called));
            __is_on_trailing_act[1] = TT_HTTP_INSERV_ACT_IGNORE;
            TT_UT_EQUAL(tt_http_svcmgr_on_trailing(&sm, NULL, &resp),
                        TT_HTTP_INSERV_ACT_IGNORE,
                        "");
            for (i = 0; i < __IS_NUM; ++i) {
                if (i == 1) {
                    TT_UT_TRUE(__is_on_trailing_called[i], "");
                } else {
                    TT_UT_FALSE(__is_on_trailing_called[i], "");
                }
            }

            // on complete
            tt_memset(__is_on_complete_called,
                      0,
                      sizeof(__is_on_complete_called));
            __is_on_complete_act[1] = TT_HTTP_INSERV_ACT_OWNER;
            TT_UT_EQUAL(tt_http_svcmgr_on_complete(&sm, NULL, &resp),
                        TT_HTTP_INSERV_ACT_OWNER,
                        "");
            for (i = 0; i < __IS_NUM; ++i) {
                if (i == 1) {
                    TT_UT_TRUE(__is_on_complete_called[i], "");
                } else {
                    TT_UT_FALSE(__is_on_complete_called[i], "");
                }
            }
        }

        // on body: a service return discard
        {
            tt_http_svcmgr_clear(&sm);

            // select owner
            for (i = 0; i < __IS_NUM; ++i) {
                __is_on_header_act[i] = TT_HTTP_INSERV_ACT_IGNORE;
            }
            __is_on_header_act[__IS_NUM - 1] = TT_HTTP_INSERV_ACT_OWNER;
            TT_UT_EQUAL(tt_http_svcmgr_on_header(&sm, NULL, NULL),
                        TT_HTTP_INSERV_ACT_OWNER,
                        "");

            // call once
            tt_memset(__is_on_body_called, 0, sizeof(__is_on_body_called));
            __is_on_body_act[__IS_NUM - 1] = TT_HTTP_INSERV_ACT_IGNORE;
            TT_UT_EQUAL(tt_http_svcmgr_on_body(&sm, NULL, &resp),
                        TT_HTTP_INSERV_ACT_IGNORE,
                        "");
            for (i = 0; i < __IS_NUM; ++i) {
                if (i == __IS_NUM - 1) {
                    TT_UT_TRUE(__is_on_body_called[i], "");
                } else {
                    TT_UT_FALSE(__is_on_body_called[i], "");
                }
            }

            // call once
            tt_memset(__is_on_body_called, 0, sizeof(__is_on_body_called));
            __is_on_body_act[__IS_NUM - 1] = TT_HTTP_INSERV_ACT_DISCARD;
            TT_UT_EQUAL(tt_http_svcmgr_on_body(&sm, NULL, &resp),
                        TT_HTTP_INSERV_ACT_DISCARD,
                        "");
            for (i = 0; i < __IS_NUM; ++i) {
                if (i == __IS_NUM - 1) {
                    TT_UT_TRUE(__is_on_body_called[i], "");
                } else {
                    TT_UT_FALSE(__is_on_body_called[i], "");
                }
            }
            TT_UT_TRUE(sm.discarding, "");
        }

        // on trailing: a service return discard
        {
            tt_http_svcmgr_clear(&sm);

            // select owner
            for (i = 0; i < __IS_NUM; ++i) {
                __is_on_header_act[i] = TT_HTTP_INSERV_ACT_IGNORE;
            }
            __is_on_header_act[__IS_NUM - 1] = TT_HTTP_INSERV_ACT_OWNER;
            TT_UT_EQUAL(tt_http_svcmgr_on_header(&sm, NULL, NULL),
                        TT_HTTP_INSERV_ACT_OWNER,
                        "");

            // call once
            tt_memset(__is_on_body_called, 0, sizeof(__is_on_body_called));
            __is_on_body_act[__IS_NUM - 1] = TT_HTTP_INSERV_ACT_IGNORE;
            TT_UT_EQUAL(tt_http_svcmgr_on_body(&sm, NULL, &resp),
                        TT_HTTP_INSERV_ACT_IGNORE,
                        "");
            for (i = 0; i < __IS_NUM; ++i) {
                if (i == __IS_NUM - 1) {
                    TT_UT_TRUE(__is_on_body_called[i], "");
                } else {
                    TT_UT_FALSE(__is_on_body_called[i], "");
                }
            }

            // call on_trailing
            tt_memset(__is_on_trailing_called,
                      0,
                      sizeof(__is_on_trailing_called));
            __is_on_trailing_act[__IS_NUM - 1] = TT_HTTP_INSERV_ACT_DISCARD;
            TT_UT_EQUAL(tt_http_svcmgr_on_trailing(&sm, NULL, &resp),
                        TT_HTTP_INSERV_ACT_DISCARD,
                        "");
            for (i = 0; i < __IS_NUM; ++i) {
                if (i == __IS_NUM - 1) {
                    TT_UT_TRUE(__is_on_trailing_called[i], "");
                } else {
                    TT_UT_FALSE(__is_on_trailing_called[i], "");
                }
            }
            TT_UT_TRUE(sm.discarding, "");
        }

        // if no owner
        {
            tt_http_svcmgr_clear(&sm);
            tt_http_resp_render_clear(&resp);
            TT_UT_EQUAL(tt_http_svcmgr_on_body(&sm, NULL, &resp),
                        TT_HTTP_INSERV_ACT_DISCARD,
                        "");
            TT_UT_TRUE(sm.discarding, "");
            TT_UT_TRUE(TT_HTTP_STATUS_VALID(
                           tt_http_resp_render_get_status(&resp)),
                       "");

            tt_http_svcmgr_clear(&sm);
            tt_http_resp_render_clear(&resp);
            TT_UT_EQUAL(tt_http_svcmgr_on_trailing(&sm, NULL, &resp),
                        TT_HTTP_INSERV_ACT_DISCARD,
                        "");
            TT_UT_TRUE(sm.discarding, "");
            TT_UT_TRUE(TT_HTTP_STATUS_VALID(
                           tt_http_resp_render_get_status(&resp)),
                       "");

            tt_http_svcmgr_clear(&sm);
            tt_http_resp_render_clear(&resp);
            TT_UT_EQUAL(tt_http_svcmgr_on_complete(&sm, NULL, &resp),
                        TT_HTTP_INSERV_ACT_DISCARD,
                        "");
            TT_UT_TRUE(sm.discarding, "");
            TT_UT_TRUE(TT_HTTP_STATUS_VALID(
                           tt_http_resp_render_get_status(&resp)),
                       "");
        }

        // test on resp
        tt_http_svcmgr_on_resp(&sm, NULL, NULL);
        {
            tt_http_outserv_t *os[__OS_NUM];

            for (i = 0; i < __OS_NUM; ++i) {
                os[i] = tt_http_outserv_create(sizeof(__is_idx_t),
                                               &__os_itf,
                                               &__os_cb);
                TT_HTTP_OUTSERV_CAST(os[i], __is_idx_t)->idx = i;
                tt_http_svcmgr_add_outserv(&sm, os[i]);
            }

            tt_http_svcmgr_on_resp(&sm, NULL, NULL);

            tt_memset(__os_clear, 0, sizeof(__os_clear));
            tt_http_svcmgr_clear(&sm);
            for (i = 0; i < __OS_NUM; ++i) {
                TT_UT_TRUE(__os_clear[i], "");
            }
        }

        tt_memset(__is_destroyed, 0, sizeof(__is_destroyed));
        tt_memset(__os_destroyed, 0, sizeof(__os_destroyed));
        tt_http_svcmgr_destroy(&sm);
        for (i = 0; i < __IS_NUM; ++i) {
            TT_UT_TRUE(__is_destroyed[i], "");
            TT_UT_TRUE(__os_destroyed[i], "");
        }
    }


    // test end
    TT_TEST_CASE_LEAVE()
}
