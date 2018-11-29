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

#include <network/http/tt_http_render.h>

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
    TT_TEST_ROUTINE_DEFINE(case_http_render_resp)
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
    }

    tt_http_resp_render_destroy(&r);

    // test end
    TT_TEST_CASE_LEAVE()
}
