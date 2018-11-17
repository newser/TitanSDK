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

#include <network/http/tt_http_header.h>

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
TT_TEST_ROUTINE_DECLARE(case_http_hdr_basic)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(http_hdr_case)

TT_TEST_CASE("case_http_hdr_basic",
             "http header basic",
             case_http_hdr_basic,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE_LIST_DEFINE_END(http_hdr_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(HTTP_UT_HDR, 0, http_hdr_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(case_http_hdr_basic)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(case_http_hdr_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_http_hval_t *hv;
    tt_u8_t *p;
    tt_http_hdr_t h;
    tt_char_t buf[128];
    tt_u32_t n, n2;

    TT_TEST_CASE_ENTER()
    // test start

    // hval

    hv = tt_http_hval_create(0);
    TT_UT_NOT_NULL(hv, "");
    tt_http_hval_destroy(hv);

    hv = tt_http_hval_create(1);
    TT_UT_NOT_NULL(hv, "");
    p = TT_HTTP_HVAL_CAST(hv, tt_u8_t);
    tt_memset(p, 0, 1);
    tt_http_hval_destroy(hv);

    hv = tt_http_hval_create(100);
    TT_UT_NOT_NULL(hv, "");
    p = TT_HTTP_HVAL_CAST(hv, tt_u8_t);
    tt_memset(p, 0, 100);
    tt_http_hval_destroy(hv);

    // header

    tt_http_hdr_init(&h,
                     TT_HTTP_HDR_HOST,
                     &tt_g_http_hdr_line_itf,
                     &tt_g_http_hval_blob_itf);
    tt_http_hdr_clear(&h);
    tt_http_hdr_destroy(&h);

    {
        tt_http_hdr_init(&h,
                         TT_HTTP_HDR_HOST,
                         &tt_g_http_hdr_line_itf,
                         &tt_g_http_hval_blob_itf);

        TT_UT_SUCCESS(tt_http_hdr_parse(&h, "a,b,c,d,", sizeof("a,b,c,d,") - 1),
                      "");
        TT_UT_EQUAL(tt_dlist_count(&h.val), 1, "");
        n = tt_http_hdr_render_len(&h);
        TT_UT_EQUAL(n, sizeof("Host: a,b,c,d,\r\n") - 1, "");
        n2 = tt_http_hdr_render(&h, buf);
        buf[n2] = 0;
        TT_UT_EQUAL(n, n2, "");
        TT_UT_STREQ(buf, "Host: a,b,c,d,\r\n", "");

        TT_UT_SUCCESS(tt_http_hdr_parse(&h, ",xx,yy", sizeof(",xx,yy") - 1),
                      "");
        TT_UT_EQUAL(tt_dlist_count(&h.val), 2, "");
        n = tt_http_hdr_render_len(&h);
        TT_UT_EQUAL(n, sizeof("Host: a,b,c,d,\r\nHost: ,xx,yy\r\n") - 1, "");
        n2 = tt_http_hdr_render(&h, buf);
        buf[n2] = 0;
        TT_UT_EQUAL(n, n2, "");
        TT_UT_STREQ(buf, "Host: a,b,c,d,\r\nHost: ,xx,yy\r\n", "");

        // empty
        TT_UT_SUCCESS(tt_http_hdr_parse(&h, ",xx,yy", 0), "");
        TT_UT_EQUAL(tt_dlist_count(&h.val), 2, "");
        TT_UT_SUCCESS(tt_http_hdr_parse(&h, "", 1), "");
        TT_UT_EQUAL(tt_dlist_count(&h.val), 2, "");
        n = tt_http_hdr_render_len(&h);
        TT_UT_EQUAL(n, sizeof("Host: a,b,c,d,\r\nHost: ,xx,yy\r\n") - 1, "");
        n2 = tt_http_hdr_render(&h, buf);
        buf[n2] = 0;
        TT_UT_EQUAL(n, n2, "");
        TT_UT_STREQ(buf, "Host: a,b,c,d,\r\nHost: ,xx,yy\r\n", "");

        tt_http_hdr_destroy(&h);
    }

    {
        tt_http_hdr_init(&h,
                         TT_HTTP_HDR_HOST,
                         &tt_g_http_hdr_cs_itf,
                         &tt_g_http_hval_blob_itf);

        TT_UT_SUCCESS(tt_http_hdr_parse(&h, ",,,,", sizeof(",,,,") - 1), "");
        TT_UT_EQUAL(tt_dlist_count(&h.val), 0, "");
        n2 = tt_http_hdr_render(&h, buf);
        buf[n2] = 0;
        TT_UT_EQUAL(n2, sizeof("Host: \r\n") - 1, "");
        TT_UT_STREQ(buf, "Host: \r\n", "");

        TT_UT_SUCCESS(tt_http_hdr_parse(&h, "a,b,c,d,", sizeof("a,b,c,d,") - 1),
                      "");
        TT_UT_EQUAL(tt_dlist_count(&h.val), 4, "");
        n = tt_http_hdr_render_len(&h);
        TT_UT_EQUAL(n, sizeof("Host: a, b, c, d\r\n") - 1, "");
        n2 = tt_http_hdr_render(&h, buf);
        buf[n2] = 0;
        TT_UT_EQUAL(n, n2, "");
        TT_UT_STREQ(buf, "Host: a, b, c, d\r\n", "");

        // append
        TT_UT_SUCCESS(tt_http_hdr_parse(&h,
                                        ",,xx,,yy,,",
                                        sizeof(",,xx,,yy,,") - 1),
                      "");
        TT_UT_EQUAL(tt_dlist_count(&h.val), 6, "");
        n = tt_http_hdr_render_len(&h);
        TT_UT_EQUAL(n, sizeof("Host: a, b, c, d, xx, yy\r\n") - 1, "");
        n2 = tt_http_hdr_render(&h, buf);
        buf[n2] = 0;
        TT_UT_EQUAL(n, n2, "");
        TT_UT_STREQ(buf, "Host: a, b, c, d, xx, yy\r\n", "");

        // empty
        TT_UT_SUCCESS(tt_http_hdr_parse(&h, ",,,,", sizeof(",,,,") - 1), "");
        TT_UT_EQUAL(tt_dlist_count(&h.val), 6, "");
        n2 = tt_http_hdr_render(&h, buf);
        buf[n2] = 0;
        TT_UT_STREQ(buf, "Host: a, b, c, d, xx, yy\r\n", "");

        tt_http_hdr_destroy(&h);
    }

    // test end
    TT_TEST_CASE_LEAVE()
}
