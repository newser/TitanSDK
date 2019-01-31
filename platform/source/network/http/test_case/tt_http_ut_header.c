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

#include <math.h>
#include <stdlib.h>

#include <network/http/header/tt_http_hdr_accept_encoding.h>
#include <network/http/header/tt_http_hdr_auth.h>
#include <network/http/header/tt_http_hdr_content_encoding.h>
#include <network/http/header/tt_http_hdr_etag.h>
#include <network/http/header/tt_http_hdr_transfer_encoding.h>
#include <network/http/tt_http_content_type_map.h>
#include <network/http/tt_http_header.h>
#include <network/http/tt_http_parser.h>
#include <network/http/tt_http_raw_header.h>

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
TT_TEST_ROUTINE_DECLARE(case_http_test_only)
TT_TEST_ROUTINE_DECLARE(case_http_hdr_parse1)
TT_TEST_ROUTINE_DECLARE(case_http_hdr_parse2)
TT_TEST_ROUTINE_DECLARE(case_http_body)
TT_TEST_ROUTINE_DECLARE(case_http_body2)
TT_TEST_ROUTINE_DECLARE(case_http_contype_map)
TT_TEST_ROUTINE_DECLARE(case_http_hdr_txenc)
TT_TEST_ROUTINE_DECLARE(case_http_hdr_contenc)
TT_TEST_ROUTINE_DECLARE(case_http_hdr_accenc)
TT_TEST_ROUTINE_DECLARE(case_http_hdr_etag)
TT_TEST_ROUTINE_DECLARE(case_http_hdr_ifmatch)
TT_TEST_ROUTINE_DECLARE(case_http_hdr_ifnmatch)
TT_TEST_ROUTINE_DECLARE(case_http_hdr_auth)
TT_TEST_ROUTINE_DECLARE(case_http_misc)
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

    TT_TEST_CASE("case_http_test_only",
                 "http test only",
                 case_http_test_only,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_http_hdr_parse1",
                 "http parse header",
                 case_http_hdr_parse1,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_http_hdr_parse2",
                 "http parse header 2",
                 case_http_hdr_parse2,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_http_body",
                 "http parse body",
                 case_http_body,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_http_body2",
                 "http parse body",
                 case_http_body2,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_http_contype_map",
                 "http content map ops",
                 case_http_contype_map,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_http_hdr_txenc",
                 "http hdr: transfer-encoding",
                 case_http_hdr_txenc,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_http_hdr_contenc",
                 "http hdr: content-encoding",
                 case_http_hdr_contenc,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_http_hdr_accenc",
                 "http hdr: accept-encoding",
                 case_http_hdr_accenc,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_http_hdr_etag",
                 "http hdr: etag",
                 case_http_hdr_etag,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_http_hdr_ifmatch",
                 "http hdr: ifmatch",
                 case_http_hdr_ifmatch,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_http_hdr_ifnmatch",
                 "http hdr: ifnmatch",
                 case_http_hdr_ifnmatch,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_http_hdr_auth",
                 "http hdr: authorization",
                 case_http_hdr_auth,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_http_misc",
                 "http test misc things",
                 case_http_misc,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

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
    TT_TEST_ROUTINE_DEFINE(case_http_hdr_etag)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    static tt_http_hdr_itf_t __hdr_cs_itf;

static tt_result_t __hdr_cs_add_val(IN struct tt_http_hdr_s *h,
                                    IN const tt_char_t *val,
                                    IN tt_u32_t len)
{
    *TT_PTR_INC(tt_u32_t, h, sizeof(struct tt_http_hdr_s)) += len;
    return TT_SUCCESS;
}

static tt_http_hdr_itf_t __hdr_cs_i = {
    NULL, __hdr_cs_add_val, NULL, NULL,
};

TT_TEST_ROUTINE_DEFINE(case_http_hdr_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_http_hval_t *hv;
    tt_u8_t *p;
    tt_http_hdr_t *h;
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

    h = tt_http_hdr_create(0,
                           TT_HTTP_HDR_HOST,
                           &tt_g_http_hdr_line_itf,
                           &tt_g_http_hval_blob_itf);
    tt_http_hdr_clear(h);
    tt_http_hdr_destroy(h);

    {
        h = tt_http_hdr_create(0,
                               TT_HTTP_HDR_HOST,
                               &tt_g_http_hdr_line_itf,
                               &tt_g_http_hval_blob_itf);

        TT_UT_SUCCESS(tt_http_hdr_parse_n(h,
                                          "a,b,c,d,",
                                          sizeof("a,b,c,d,") - 1),
                      "");
        TT_UT_EQUAL(tt_dlist_count(&h->val), 1, "");
        n = tt_http_hdr_render_len(h);
        TT_UT_EQUAL(n, sizeof("Host: a,b,c,d,\r\n") - 1, "");
        n2 = tt_http_hdr_render(h, buf);
        buf[n2] = 0;
        TT_UT_EQUAL(n, n2, "");
        TT_UT_STREQ(buf, "Host: a,b,c,d,\r\n", "");

        TT_UT_SUCCESS(tt_http_hdr_parse_n(h, ",xx,yy", sizeof(",xx,yy") - 1),
                      "");
        TT_UT_EQUAL(tt_dlist_count(&h->val), 2, "");
        n = tt_http_hdr_render_len(h);
        TT_UT_EQUAL(n, sizeof("Host: a,b,c,d,\r\nHost: ,xx,yy\r\n") - 1, "");
        n2 = tt_http_hdr_render(h, buf);
        buf[n2] = 0;
        TT_UT_EQUAL(n, n2, "");
        TT_UT_STREQ(buf, "Host: a,b,c,d,\r\nHost: ,xx,yy\r\n", "");

        // empty
        TT_UT_SUCCESS(tt_http_hdr_parse_n(h, ",xx,yy", 0), "");
        TT_UT_EQUAL(tt_dlist_count(&h->val), 2, "");
        TT_UT_SUCCESS(tt_http_hdr_parse_n(h, "", 1), "");
        TT_UT_EQUAL(tt_dlist_count(&h->val), 2, "");
        n = tt_http_hdr_render_len(h);
        TT_UT_EQUAL(n, sizeof("Host: a,b,c,d,\r\nHost: ,xx,yy\r\n") - 1, "");
        n2 = tt_http_hdr_render(h, buf);
        buf[n2] = 0;
        TT_UT_EQUAL(n, n2, "");
        TT_UT_STREQ(buf, "Host: a,b,c,d,\r\nHost: ,xx,yy\r\n", "");

        tt_http_hdr_destroy(h);
    }

    {
        h = tt_http_hdr_create(0,
                               TT_HTTP_HDR_HOST,
                               &tt_g_http_hdr_cs_itf,
                               &tt_g_http_hval_blob_itf);

        TT_UT_SUCCESS(tt_http_hdr_parse_n(h, ",,,,", sizeof(",,,,") - 1), "");
        TT_UT_EQUAL(tt_dlist_count(&h->val), 0, "");
        n2 = tt_http_hdr_render(h, buf);
        buf[n2] = 0;
        TT_UT_EQUAL(n2, sizeof("Host: \r\n") - 1, "");
        TT_UT_STREQ(buf, "Host: \r\n", "");

        TT_UT_SUCCESS(tt_http_hdr_parse_n(h,
                                          "a,b,c,d,",
                                          sizeof("a,b,c,d,") - 1),
                      "");
        TT_UT_EQUAL(tt_dlist_count(&h->val), 4, "");
        n = tt_http_hdr_render_len(h);
        TT_UT_EQUAL(n, sizeof("Host: a, b, c, d\r\n") - 1, "");
        n2 = tt_http_hdr_render(h, buf);
        buf[n2] = 0;
        TT_UT_EQUAL(n, n2, "");
        TT_UT_STREQ(buf, "Host: a, b, c, d\r\n", "");

        // append
        TT_UT_SUCCESS(tt_http_hdr_parse_n(h,
                                          ",,xx,,yy,,",
                                          sizeof(",,xx,,yy,,") - 1),
                      "");
        TT_UT_EQUAL(tt_dlist_count(&h->val), 6, "");
        n = tt_http_hdr_render_len(h);
        TT_UT_EQUAL(n, sizeof("Host: a, b, c, d, xx, yy\r\n") - 1, "");
        n2 = tt_http_hdr_render(h, buf);
        buf[n2] = 0;
        TT_UT_EQUAL(n, n2, "");
        TT_UT_STREQ(buf, "Host: a, b, c, d, xx, yy\r\n", "");

        // empty
        TT_UT_SUCCESS(tt_http_hdr_parse_n(h, ",,,,", sizeof(",,,,") - 1), "");
        TT_UT_EQUAL(tt_dlist_count(&h->val), 6, "");
        n2 = tt_http_hdr_render(h, buf);
        buf[n2] = 0;
        TT_UT_STREQ(buf, "Host: a, b, c, d, xx, yy\r\n", "");

        tt_http_hdr_destroy(h);
    }

    {
        tt_http_hdr_t *p;

        p = tt_http_hdr_create_line(0, TT_HTTP_HDR_HOST, NULL);
        TT_UT_NOT_NULL(p, "");
        tt_http_hdr_destroy(p);

        p = tt_http_hdr_create_line(0, TT_HTTP_HDR_HOST, NULL);
        TT_UT_NOT_NULL(p, "");
        TT_UT_SUCCESS(tt_http_hdr_parse(p, "1,2, 3--4,"), "");

        n = tt_http_hdr_render_len(p); //"Host: 1,2, 3--4,\r\n"
        TT_UT_EQUAL(n, sizeof("Host: 1,2, 3--4,\r\n") - 1, "");

        n2 = tt_http_hdr_render(p, buf);
        buf[n2] = 0;
        TT_UT_EQUAL(n, n2, "");
        TT_UT_STREQ(buf, "Host: 1,2, 3--4,\r\n", "");

        tt_http_hdr_destroy(p);
    }

    {
        tt_http_hdr_t *p;

        p = tt_http_hdr_create_cs(sizeof(tt_u32_t),
                                  TT_HTTP_HDR_HOST,
                                  &__hdr_cs_i);
        TT_UT_NOT_NULL(p, "");
        tt_http_hdr_destroy(p);

        p = tt_http_hdr_create_cs(sizeof(tt_u32_t),
                                  TT_HTTP_HDR_HOST,
                                  &__hdr_cs_i);
        TT_UT_NOT_NULL(p, "");
        *TT_PTR_INC(tt_u32_t, p, sizeof(tt_http_hdr_t)) = 0;

        TT_UT_SUCCESS(tt_http_hdr_parse(p, " 1 "), "");
        TT_UT_SUCCESS(tt_http_hdr_parse(p, "22 "), "");
        TT_UT_SUCCESS(tt_http_hdr_parse(p, " 333"), "");
        TT_UT_EQUAL(*TT_PTR_INC(tt_u32_t, p, sizeof(tt_http_hdr_t)),
                    1 + 2 + 3,
                    "");

        tt_http_hdr_destroy(p);
    }

    {
        tt_http_hdr_t *p;

        p = tt_http_hdr_create_csq(sizeof(tt_u32_t),
                                   TT_HTTP_HDR_HOST,
                                   &__hdr_cs_i);
        TT_UT_NOT_NULL(p, "");
        *TT_PTR_INC(tt_u32_t, p, sizeof(tt_http_hdr_t)) = 0;
        TT_UT_SUCCESS(tt_http_hdr_parse(p, " "), "");
        TT_UT_EQUAL(*TT_PTR_INC(tt_u32_t, p, sizeof(tt_http_hdr_t)), 0, "");
        TT_UT_SUCCESS(tt_http_hdr_parse(p, "  \"  ,   \" "), "");
        // 8 chars: ["  ,   "]
        TT_UT_EQUAL(*TT_PTR_INC(tt_u32_t, p, sizeof(tt_http_hdr_t)), 8, "");
        TT_UT_SUCCESS(tt_http_hdr_parse(p, " \",\" ,, , \",,,   "), "");
        // 3 chars: [","]
        // 4 chars: [",,,], ending ws are trimned
        TT_UT_EQUAL(*TT_PTR_INC(tt_u32_t, p, sizeof(tt_http_hdr_t)),
                    8 + 3 + 4,
                    "");
        tt_http_hdr_destroy(p);

        p = tt_http_hdr_create_csq(sizeof(tt_u32_t),
                                   TT_HTTP_HDR_HOST,
                                   &__hdr_cs_i);
        TT_UT_NOT_NULL(p, "");
        *TT_PTR_INC(tt_u32_t, p, sizeof(tt_http_hdr_t)) = 0;

        TT_UT_SUCCESS(tt_http_hdr_parse(p, " 1 "), "");
        TT_UT_SUCCESS(tt_http_hdr_parse(p, "22 "), "");
        TT_UT_SUCCESS(tt_http_hdr_parse(p, " 333"), "");
        TT_UT_EQUAL(*TT_PTR_INC(tt_u32_t, p, sizeof(tt_http_hdr_t)),
                    1 + 2 + 3,
                    "");

        tt_http_hdr_destroy(p);
    }

    {
        tt_http_hdr_t *p;

        p = tt_http_hdr_create_line(sizeof(tt_u32_t),
                                    TT_HTTP_HDR_HOST,
                                    &__hdr_cs_i);
        TT_UT_NOT_NULL(p, "");
        tt_http_hdr_destroy(p);

        p = tt_http_hdr_create_line(sizeof(tt_u32_t),
                                    TT_HTTP_HDR_HOST,
                                    &__hdr_cs_i);
        TT_UT_NOT_NULL(p, "");
        *TT_PTR_INC(tt_u32_t, p, sizeof(tt_http_hdr_t)) = 0;

        TT_UT_SUCCESS(tt_http_hdr_parse(p, " 1,22, "), "");
        TT_UT_SUCCESS(tt_http_hdr_parse(p, "333, 4444, "), "");
        TT_UT_EQUAL(*TT_PTR_INC(tt_u32_t, p, sizeof(tt_http_hdr_t)), 15, "");

        tt_http_hdr_destroy(p);
    }

#if 0
    {
        tt_http_hdr_t *p;

        p = tt_http_hdr_create_cs(TT_HTTP_HDR_HOST, NULL, 0, NULL);
        TT_UT_NOT_NULL(p, "");
        *TT_PTR_INC(tt_u32_t, p, sizeof(tt_http_hdr_t)) = 0;

        TT_UT_SUCCESS(tt_http_hdr_parse(p, ",   1,22 ,333, 4444 ,"), "");
        TT_UT_EQUAL(*TT_PTR_INC(tt_u32_t, p, sizeof(tt_http_hdr_t)), 10, "");

        tt_http_hdr_destroy(p);
    }
#endif

    // test end
    TT_TEST_CASE_LEAVE()
}

static int __msg_beg(http_parser *hp)
{
    printf("%s\n", __FUNCTION__);
    return 0;
}

static int __uri(http_parser *hp, const char *at, size_t length)
{
    char buf[128] = {0};
    tt_strncpy(buf, at, TT_MIN(127, length));
    printf("%s: %s\n", __FUNCTION__, buf);
    return 0;
}

static int __status(http_parser *hp, const char *at, size_t length)
{
    char buf[128] = {0};
    tt_strncpy(buf, at, TT_MIN(127, length));
    printf("%s: %s\n", __FUNCTION__, buf);
    return 0;
}

static int __hdr_field(http_parser *hp, const char *at, size_t length)
{
    char buf[128] = {0};
    tt_strncpy(buf, at, TT_MIN(127, length));
    printf("%s: %s\n", __FUNCTION__, buf);
    return 0;
}

static int __hdr_val(http_parser *hp, const char *at, size_t length)
{
    char buf[128] = {0};
    tt_strncpy(buf, at, TT_MIN(127, length));
    printf("%s: %s\n", __FUNCTION__, buf);
    return 0;
}

static int __hdr_comp(http_parser *hp)
{
    printf("%s\n", __FUNCTION__);
    return 0;
}

static int __body(http_parser *hp, const char *at, size_t length)
{
    char buf[128] = {0};
    tt_strncpy(buf, at, TT_MIN(127, length));
    printf("%s: %s\n", __FUNCTION__, buf);
    return 0;
}

static int __msg_comp(http_parser *hp)
{
    printf("%s\n", __FUNCTION__);
    return 0;
}

static int __chunk_hdr(http_parser *hp)
{
    printf("%s\n", __FUNCTION__);
    return 0;
}

static int __chunk_comp(http_parser *hp)
{
    printf("%s\n", __FUNCTION__);
    return 0;
}

TT_TEST_ROUTINE_DEFINE(case_http_test_only)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    http_parser hp;
    http_parser_settings set = {
        __msg_beg,
        __uri,
        __status,
        __hdr_field,
        __hdr_val,
        __hdr_comp,
        __body,
        __msg_comp,
        __chunk_hdr,
        __chunk_comp,
    };
    const char *s;
    tt_u32_t slen;

    TT_TEST_CASE_ENTER()
    // test start

    s = "POST /chunked_w_nonsense_after_length HTTP/1.1\r\n" // 48
        //"Transfer-Encoding: chunked\r\n" // 28
        "\r\n" // 2
        "5; ilovew3;whattheluck=aretheseparametersfor\r\nhello\r\n" // 54
        "6; blahblah; blah\r\n world\r\n" // 28
        "0\r\n" // 3
        "Vary: *\r\n"
        "Content-Type: text/plain\r\n"
        "\r\n";

    //    s = "GET /get_funky_content_length_body_hello HTTP/1.0\r\n" // 51
    //    "conTENT-Length: 5\r\n" // 19
    //    "\r\n" // 2
    //    "HELLO"; // 5

    s = "GET / HTTP/1.1\r\n"
        "Line1:   abc\r\n"
        "\tdef\r\n"
        " ghi\r\n"
        "\t\tjkl\r\n"
        "  mno \r\n"
        "\t \tqrs\r\n"
        "Line2: \t line2\t\r\n"
        "Line3:\r\n"
        " line3\r\n"
        "Line4: \r\n"
        " \r\n"
        "Connection:\r\n"
        " close\r\n"
        "\r\n";

    //    s = "HTTP/1.1 200 OK\r\n"
    //    "Date: Tue, 04 Aug 2009 07:59:32 GMT\r\n"
    //    "Server: Apache\r\n"
    //    "X-Powered-By: Servlet/2.5 JSP/2.1\r\n"
    //    "Content-Type: text/xml; charset=utf-8\r\n"
    //    "Connection: close\r\n"
    //    "\r\n";

    slen = tt_strlen(s);

    //    http_parser_init(&hp, HTTP_REQUEST);
    http_parser_init(&hp, HTTP_BOTH);
    //    http_parser_execute(&hp, &set, s, tt_strlen(s));
    {
        tt_u32_t n1 = 30;
        size_t x;

        x = http_parser_execute(&hp, &set, s, 50);
        printf("final: %d\n", http_body_is_final(&hp));
        x = http_parser_execute(&hp, &set, s + x, slen - x);
        printf("final: %d\n", http_body_is_final(&hp));
    }

    printf("error: %s\n", http_errno_description(hp.http_errno));

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_http_hdr_parse1)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_http_parser_t hp;
    tt_slab_t rh, rv;
    tt_u8_t *wp;
    tt_u32_t wlen;

    TT_TEST_CASE_ENTER()
    // test start

    TT_UT_SUCCESS(tt_slab_create(&rh, sizeof(tt_http_rawhdr_t), NULL), "");
    TT_UT_SUCCESS(tt_slab_create(&rv, sizeof(tt_http_rawval_t), NULL), "");

    TT_UT_SUCCESS(tt_http_parser_create(&hp, &rh, &rv, NULL), "");

    {
        tt_char_t p[] =
            "GET /a/b/c/d HTTP/1.0\r\n" // 23
            "Line1:   abc\r\n" // 14 => 37
            "\tdef \r\n" // 7 => 44
            "Xine3:\r\n" // 8 => 52
            "A:xyz\r\n" // 7 => 59
            "\r\n" // 2 => 61
            ;

        tt_u32_t i;

        tt_http_parser_wpos(&hp, &wp, &wlen);
        TT_UT_EXP(wlen > sizeof(p), "");

        for (i = 0; i < sizeof(p) - 1; ++i) {
            *wp++ = p[i];
            tt_http_parser_inc_wp(&hp, 1);
            TT_UT_SUCCESS(tt_http_parser_run(&hp), "");

            if (i == 3) {
                TT_UT_EQUAL(tt_http_parser_get_method(&hp),
                            TT_HTTP_MTD_GET,
                            "");
            }

            if (i < 4) {
                tt_blobex_t *b = tt_http_parser_get_rawuri(&hp);
                TT_UT_EQUAL(tt_blobex_len(b), 0, "");
                TT_UT_FALSE(hp.complete_line1, "");
                TT_UT_FALSE(hp.complete_header, "");
                TT_UT_FALSE(hp.complete_message, "");
            } else if (i == 4) {
                tt_blobex_t *b = tt_http_parser_get_rawuri(&hp);
                TT_UT_EQUAL(tt_blobex_len(b), 1, "");
                TT_UT_EQUAL(tt_blobex_strcmp(b, "/"), 0, "");
            } else if (i == 5) {
                tt_blobex_t *b = tt_http_parser_get_rawuri(&hp);
                TT_UT_EQUAL(tt_blobex_len(b), 2, "");
                TT_UT_EQUAL(tt_blobex_strcmp(b, "/a"), 0, "");
            } else if (i == 12) {
                tt_blobex_t *b = tt_http_parser_get_rawuri(&hp);
                TT_UT_EQUAL(tt_blobex_len(b), 8, "");
                TT_UT_EQUAL(tt_blobex_strcmp(b, "/a/b/c/d"), 0, "");
            } else if (i == 13) {
                tt_blobex_t *b = tt_http_parser_get_rawuri(&hp);
                TT_UT_EQUAL(tt_blobex_len(b), 8, "");
                TT_UT_EQUAL(tt_blobex_strcmp(b, "/a/b/c/d"), 0, "");
            }

            if (i == 21) {
                TT_UT_EQUAL(tt_http_parser_get_version(&hp), TT_HTTP_V1_0, "");
                TT_UT_FALSE(hp.complete_line1, "");
            }

            if (i == 23) {
                TT_UT_TRUE(hp.complete_line1, "");
                TT_UT_FALSE(hp.complete_header, "");
                TT_UT_FALSE(hp.complete_message, "");

                // see "L"
                TT_UT_EQUAL(tt_dlist_count(&hp.rawhdr), 0, "");
                TT_UT_NOT_NULL(hp.rh, "");
                TT_UT_EQUAL(tt_blobex_strcmp(&hp.rh->name, "L"), 0, "");
            } else if ((i == 28) || (i == 29) || (i == 30)) {
                // see "Line1" or "Line1:" or "Line1: "
                TT_UT_EQUAL(tt_dlist_count(&hp.rawhdr), 0, "");
                TT_UT_NOT_NULL(hp.rh, "");
                TT_UT_EQUAL(tt_blobex_strcmp(&hp.rh->name, "Line1"), 0, "");
            }

            if (i == 32) {
                // see "Line1:   a"
                TT_UT_EQUAL(tt_dlist_count(&hp.rawhdr), 0, "");
                TT_UT_NOT_NULL(hp.rh, "");
                TT_UT_EQUAL(tt_blobex_strcmp(&hp.rh->name, "Line1"), 0, "");

                TT_UT_NOT_NULL(hp.rv, "");
                TT_UT_EQUAL(tt_blobex_strcmp(&hp.rv->val, "a"), 0, "");
            } else if (i == 33) {
                TT_UT_NOT_NULL(hp.rv, "");
                TT_UT_EQUAL(tt_blobex_strcmp(&hp.rv->val, "ab"), 0, "");
            } else if (i == 36) {
                TT_UT_NOT_NULL(hp.rv, "");
                TT_UT_EQUAL(tt_blobex_strcmp(&hp.rv->val, "abc"), 0, "");
            } else if (i == 37) {
                TT_UT_NOT_NULL(hp.rv, "");
                TT_UT_EQUAL(tt_blobex_strcmp(&hp.rv->val, "abc\t"), 0, "");
            } else if (i == 42) {
                TT_UT_NOT_NULL(hp.rv, "");
                TT_UT_EQUAL(tt_blobex_strcmp(&hp.rv->val, "abc\tdef "), 0, "");
            }

            if (i == 44) {
                TT_UT_EQUAL(tt_dlist_count(&hp.rawhdr), 1, "");

                TT_UT_NOT_NULL(hp.rh, "");
                TT_UT_NULL(hp.rv, "");
                TT_UT_EQUAL(tt_blobex_strcmp(&hp.rh->name, "X"), 0, "");
            } else if (i == 51) {
                TT_UT_NOT_NULL(hp.rh, "");
                TT_UT_NULL(hp.rv, "");
                TT_UT_EQUAL(tt_blobex_strcmp(&hp.rh->name, "Xine3"), 0, "");
            }

            if (i == 52) {
                TT_UT_EQUAL(tt_dlist_count(&hp.rawhdr), 2, "");

                TT_UT_NOT_NULL(hp.rh, "");
                TT_UT_NULL(hp.rv, "");
                TT_UT_EQUAL(tt_blobex_strcmp(&hp.rh->name, "A"), 0, "");
            } else if (i == 58) {
                TT_UT_NOT_NULL(hp.rv, "");
                TT_UT_EQUAL(tt_blobex_strcmp(&hp.rv->val, "xyz"), 0, "");
            }

            if (i == 60) {
                TT_UT_EQUAL(tt_dlist_count(&hp.rawhdr), 3, "");
                TT_UT_NULL(hp.rh, "");
                TT_UT_NULL(hp.rv, "");

                TT_UT_TRUE(hp.complete_line1, "");
                TT_UT_TRUE(hp.complete_header, "");
                TT_UT_TRUE(hp.complete_message, "");
            }
        }
    }

    tt_http_parser_destroy(&hp);

    tt_slab_destroy(&rh);
    tt_slab_destroy(&rv);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_http_body)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_http_parser_t hp;
    tt_slab_t rh, rv;
    tt_u8_t *wp;
    tt_u32_t wlen;

    TT_TEST_CASE_ENTER()
    // test start

    TT_UT_SUCCESS(tt_slab_create(&rh, sizeof(tt_http_rawhdr_t), NULL), "");
    TT_UT_SUCCESS(tt_slab_create(&rv, sizeof(tt_http_rawval_t), NULL), "");

    TT_UT_SUCCESS(tt_http_parser_create(&hp, &rh, &rv, NULL), "");

    // no body request
    {
        tt_char_t p[] =
            "GET /favicon.ico HTTP/1.1\r\n"
            "Host: 0.0.0.0=5000\r\n"
            "User-Agent: Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.9) "
            "Gecko/2008061015 Firefox/3.0\r\n"
            "Accept: "
            "text/html,application/xhtml+xml,application/xml;q=0.9,*/"
            "*;q=0.8\r\n"
            "Accept-Language: en-us,en;q=0.5\r\n"
            "Accept-Encoding: gzip,deflate\r\n"
            "Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r\n"
            "Keep-Alive: 300\r\n"
            "Etag: \"bfc13a64729c4290ef5b2c2730249c88ca92d82d\"\r\n"
            "Etag: *\r\n"
            "If-match: \"abc\"\r\n"
            "If-match: * \r\n"
            "If-none-match: *\r\n"
            "If-none-match: W/\"123\", 789 \r\n"
            "Authorization: Digest realm=\"Authorization\"\r\n"
            "WWW-Authenticate: Digest realm=\"WWW-Authenticate\"\r\n"
            "Proxy-Authorization: Digest realm=\"Proxy-Authorization\"\r\n"
            "Proxy-Authenticate: Digest realm=\"Proxy-Authenticate\"\r\n"
            "Connection: keep-alive\r\n"
            "Etag: W/\"67ab43\", \"54ed21, \"7892dd\"\r\n"
            "If-match: W/\"456\", 789\" \r\n"
            "\r\n";

        tt_u32_t slen = sizeof(p) - 1, i;
        tt_http_rawhdr_t *rh;
        tt_http_rawval_t *rv;
        tt_blobex_t *ho;
        tt_http_txenc_t e[10];
        tt_http_enc_t ce[10];

        tt_http_parser_wpos(&hp, &wp, &wlen);
        TT_UT_EXP(wlen > sizeof(p), "");

        TT_UT_FALSE(hp.complete_line1, "");
        TT_UT_FALSE(hp.complete_header, "");
        TT_UT_FALSE(hp.complete_message, "");

        for (i = 0; i < slen; ++i) {
            *wp++ = p[i];
            tt_http_parser_inc_wp(&hp, 1);
            TT_UT_SUCCESS(tt_http_parser_run(&hp), "");
        }

        TT_UT_EQUAL(tt_http_parser_get_contype(&hp), TT_HTTP_CONTYPE_NUM, "");
        TT_UT_TRUE(hp.updated_contype, "");
        TT_UT_FALSE(hp.miss_contype, "");

        TT_UT_EQUAL(tt_http_parser_get_content_len(&hp), -1, "");
        TT_UT_TRUE(hp.updated_content_len, "");
        TT_UT_FALSE(hp.miss_content_len, "");

#define __check(n, v)                                                          \
    do {                                                                       \
        rh = tt_http_rawhdr_find(&hp.rawhdr, n);                               \
        TT_UT_NOT_NULL(rh, "");                                                \
        rv = tt_http_rawval_find(&rh->val, v);                                 \
        TT_UT_NOT_NULL(rv, "");                                                \
    } while (0)

#define __check_trail(n, v)                                                    \
    do {                                                                       \
        rh = tt_http_rawhdr_find(&hp.trailing_rawhdr, n);                      \
        TT_UT_NOT_NULL(rh, "");                                                \
        rv = tt_http_rawval_find(&rh->val, v);                                 \
        TT_UT_NOT_NULL(rv, "");                                                \
    } while (0)

        __check("host", "0.0.0.0=5000");
        __check("User-Agent",
                "Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.9) "
                "Gecko/2008061015 Firefox/3.0");
        __check("Accept",
                "text/html,application/xhtml+xml,application/xml;q=0.9,*/"
                "*;q=0.8");
        __check("Accept-Language", "en-us,en;q=0.5");
        __check("Accept-Charset", "ISO-8859-1,utf-8;q=0.7,*;q=0.7");
        __check("Accept-Encoding", "gzip,deflate");
        __check("Keep-Alive", "300");
        __check("Connection", "keep-alive");

        TT_UT_TRUE(hp.complete_line1, "");
        TT_UT_TRUE(hp.complete_header, "");
        TT_UT_TRUE(hp.complete_message, "");

        ho = tt_http_parser_get_host(&hp);
        TT_UT_NOT_NULL(ho, "");
        TT_UT_EQUAL(tt_blobex_strcmp(ho, "0.0.0.0=5000"), 0, "");
        ho = tt_http_parser_get_host(&hp);
        TT_UT_NOT_NULL(ho, "");
        TT_UT_EQUAL(tt_blobex_strcmp(ho, "0.0.0.0=5000"), 0, "");

        TT_UT_EQUAL(tt_http_parser_get_txenc(&hp, e), 0, "");
        TT_UT_FALSE(hp.miss_txenc, "");

        TT_UT_EQUAL(tt_http_parser_get_contenc(&hp, ce), 0, "");
        TT_UT_FALSE(hp.miss_contenc, "");

        {
            tt_http_accenc_t *ha = tt_http_parser_get_accenc(&hp);
            TT_UT_TRUE(ha->has[TT_HTTP_ENC_GZIP], "");
            TT_UT_EXP(fabs(ha->weight[TT_HTTP_ENC_GZIP] - 1.0) < 0.0001, "");
            TT_UT_TRUE(ha->has[TT_HTTP_ENC_DEFLATE], "");
            TT_UT_EXP(fabs(ha->weight[TT_HTTP_ENC_DEFLATE] - 1.0) < 0.0001, "");
            TT_UT_FALSE(ha->has[TT_HTTP_ENC_COMPRESS], "");
            TT_UT_FALSE(ha->has[TT_HTTP_ENC_BR], "");
            TT_UT_FALSE(ha->has[TT_HTTP_ENC_IDENTITY], "");
        }

        {
            tt_http_hdr_t *h = tt_http_parser_get_etag(&hp);
            tt_http_etag_t *e;
            TT_UT_NOT_NULL(h, "");
            TT_UT_EQUAL(h->name, TT_HTTP_HDR_ETAG, "");
            TT_UT_TRUE(hp.updated_etag, "");

            e = tt_http_etag_head(h);
            TT_UT_NOT_NULL(e, "");
            TT_UT_EQUAL(
                tt_blobex_strcmp(&e->etag,
                                 "bfc13a64729c4290ef5b2c2730249c88ca92d82d"),
                0,
                "");
            TT_UT_FALSE(e->weak, "");
            TT_UT_FALSE(e->aster, "");

            e = tt_http_etag_next(e);
            TT_UT_NOT_NULL(e, "");
            TT_UT_TRUE(e->aster, "");

            e = tt_http_etag_next(e);
            TT_UT_NOT_NULL(e, "");

            e = tt_http_etag_next(e);
            TT_UT_NOT_NULL(e, "");
            TT_UT_EQUAL(tt_blobex_strcmp(&e->etag, "7892dd"), 0, "");

            e = tt_http_etag_next(e);
            TT_UT_NULL(e, "");
        }

        {
            tt_http_hdr_t *h = tt_http_parser_get_ifmatch(&hp);
            tt_http_etag_t *e;
            TT_UT_NOT_NULL(h, "");
            TT_UT_EQUAL(h->name, TT_HTTP_HDR_IF_MATCH, "");
            TT_UT_TRUE(hp.updated_ifmatch, "");

            e = tt_http_etag_head(h);
            TT_UT_NOT_NULL(e, "");
            TT_UT_EQUAL(tt_blobex_strcmp(&e->etag, "abc"), 0, "");
            TT_UT_FALSE(e->weak, "");
            TT_UT_FALSE(e->aster, "");

            e = tt_http_etag_next(e);
            TT_UT_NOT_NULL(e, "");
            TT_UT_TRUE(e->aster, "");

            e = tt_http_etag_next(e);
            TT_UT_NOT_NULL(e, "");
            TT_UT_EQUAL(tt_blobex_strcmp(&e->etag, "456"), 0, "");

            e = tt_http_etag_next(e);
            TT_UT_NULL(e, "");
        }

        {
            tt_http_hdr_t *h = tt_http_parser_get_ifnmatch(&hp);
            tt_http_etag_t *e;
            TT_UT_NOT_NULL(h, "");
            TT_UT_EQUAL(h->name, TT_HTTP_HDR_IF_N_MATCH, "");
            TT_UT_TRUE(hp.updated_ifnmatch, "");

            e = tt_http_etag_head(h);
            TT_UT_NOT_NULL(e, "");
            TT_UT_TRUE(e->aster, "");

            e = tt_http_etag_next(e);
            TT_UT_NOT_NULL(e, "");
            TT_UT_EQUAL(tt_blobex_strcmp(&e->etag, "123"), 0, "");
            TT_UT_TRUE(e->weak, "");
            TT_UT_FALSE(e->aster, "");

            e = tt_http_etag_next(e);
            TT_UT_NULL(e, "");
        }

        {
            tt_http_hdr_t *h;
            tt_http_auth_t *ha;

            h = tt_http_parser_get_auth(&hp);
            TT_UT_NOT_NULL(h, "");
            ha = tt_http_hdr_auth_get(h);
            TT_UT_NOT_NULL(ha, "");
            TT_UT_EQUAL(tt_blobex_strcmp(&ha->realm, "Authorization"), 0, "");

            h = tt_http_parser_get_www_auth(&hp);
            TT_UT_NOT_NULL(h, "");
            ha = tt_http_hdr_auth_get(h);
            TT_UT_NOT_NULL(ha, "");
            TT_UT_EQUAL(tt_blobex_strcmp(&ha->realm, "WWW-Authenticate"),
                        0,
                        "");

            h = tt_http_parser_get_proxy_authorization(&hp);
            TT_UT_NOT_NULL(h, "");
            ha = tt_http_hdr_auth_get(h);
            TT_UT_NOT_NULL(ha, "");
            TT_UT_EQUAL(tt_blobex_strcmp(&ha->realm, "Proxy-Authorization"),
                        0,
                        "");

            h = tt_http_parser_get_proxy_authenticate(&hp);
            TT_UT_NOT_NULL(h, "");
            ha = tt_http_hdr_auth_get(h);
            TT_UT_NOT_NULL(ha, "");
            TT_UT_EQUAL(tt_blobex_strcmp(&ha->realm, "Proxy-Authenticate"),
                        0,
                        "");
        }

        {
            tt_http_uri_t *u = tt_http_parser_get_uri(&hp);
            TT_UT_NOT_NULL(u, "");
            TT_UT_STREQ(tt_http_uri_render(u), "/favicon.ico", "");
            TT_UT_EQUAL(tt_http_uri_get_scheme(u),
                        TT_HTTP_SCHEME_UNDEFINED,
                        "");
        }
    }

    // with body request
    {
        tt_char_t p2[] =
            "PATCH /file.txt HTTP/1.1\r\n"
            "Host: www.example.com\r\n"
            "Content-Type: application/javascript\r\n"
            "Transfer-Encoding:1, chunked, 333, gzip\r\n"
            "Content-Encoding:1, chunked, 333, gzip\r\n"
            "Transfer-Encoding:gzip, \r\n"
            "Content-Encoding:, identity\r\n"
            "Content-Length: 10\r\n"
            "Transfer-Encoding:, deflate\r\n"
            "Content-Encoding:, deflate\r\n"
            "\r\n";

        tt_char_t body[] = "1234567890abcd";

        tt_u32_t slen = sizeof(p2) - 1, i;
        tt_http_rawhdr_t *rh;
        tt_http_rawval_t *rv;
        tt_http_txenc_t e[10];
        tt_http_enc_t ce[10];

        tt_http_parser_clear(&hp, TT_FALSE);

        TT_UT_FALSE(hp.complete_line1, "");
        TT_UT_FALSE(hp.complete_header, "");
        TT_UT_FALSE(hp.complete_message, "");

        for (i = 0; i < slen;) {
            tt_u32_t this_len;
            if (i + 30 < slen) {
                this_len = tt_rand_u32() % 30 + 1;
            } else {
                this_len = slen - i;
            }
            tt_http_parser_wpos(&hp, &wp, &wlen);
            tt_memcpy(wp, &p2[i], this_len);
            tt_http_parser_inc_wp(&hp, this_len);
            TT_UT_SUCCESS(tt_http_parser_run(&hp), "");
            i += this_len;
        }
        TT_ASSERT(i == slen);

        TT_UT_EQUAL(tt_http_parser_get_method(&hp), TT_HTTP_MTD_PATCH, "");
        TT_UT_EQUAL(tt_http_parser_get_version(&hp), TT_HTTP_V1_1, "");

        TT_UT_EQUAL(tt_http_parser_get_contype(&hp),
                    TT_HTTP_CONTYPE_APP_JS,
                    "");
        TT_UT_EQUAL(tt_http_parser_get_contype(&hp),
                    TT_HTTP_CONTYPE_APP_JS,
                    "");
        TT_UT_TRUE(hp.updated_contype, "");
        TT_UT_FALSE(hp.miss_contype, "");

        TT_UT_EQUAL(tt_http_parser_get_content_len(&hp), 10, "");
        TT_UT_TRUE(hp.updated_content_len, "");
        TT_UT_FALSE(hp.miss_content_len, "");

        TT_UT_EQUAL(tt_http_parser_get_txenc(&hp, e), 4, "");
        TT_UT_EQUAL(e[0], TT_HTTP_TXENC_CHUNKED, "");
        TT_UT_EQUAL(e[1], TT_HTTP_TXENC_GZIP, "");
        TT_UT_EQUAL(e[2], TT_HTTP_TXENC_GZIP, "");
        TT_UT_EQUAL(e[3], TT_HTTP_TXENC_DEFLATE, "");
        TT_UT_TRUE(hp.miss_txenc, "");

        TT_UT_EQUAL(tt_http_parser_get_contenc(&hp, ce), 3, "");
        TT_UT_EQUAL(ce[0], TT_HTTP_ENC_GZIP, "");
        TT_UT_EQUAL(ce[1], TT_HTTP_ENC_IDENTITY, "");
        TT_UT_EQUAL(ce[2], TT_HTTP_ENC_DEFLATE, "");
        TT_UT_TRUE(hp.miss_txenc, "");

        {
            tt_http_accenc_t *ha = tt_http_parser_get_accenc(&hp);
            TT_UT_FALSE(ha->has[TT_HTTP_ENC_GZIP], "");
            TT_UT_FALSE(ha->has[TT_HTTP_ENC_DEFLATE], "");
            TT_UT_FALSE(ha->has[TT_HTTP_ENC_COMPRESS], "");
            TT_UT_FALSE(ha->has[TT_HTTP_ENC_BR], "");
            TT_UT_FALSE(ha->has[TT_HTTP_ENC_IDENTITY], "");
        }

        {
            tt_http_hdr_t *h = tt_http_parser_get_etag(&hp);
            TT_UT_NULL(h, "");
            TT_UT_TRUE(hp.updated_etag, "");
        }

        {
            tt_http_hdr_t *h = tt_http_parser_get_ifmatch(&hp);
            TT_UT_NULL(h, "");
            TT_UT_TRUE(hp.updated_ifmatch, "");
        }

        {
            tt_http_hdr_t *h = tt_http_parser_get_ifnmatch(&hp);
            TT_UT_NULL(h, "");
            TT_UT_TRUE(hp.updated_ifnmatch, "");
        }

        __check("Host", "www.example.com");
        __check("Content-Type", "application/javascript");
        __check("Content-Length", "10");

        TT_UT_TRUE(hp.complete_line1, "");
        TT_UT_TRUE(hp.complete_header, "");
        TT_UT_FALSE(hp.complete_message, "");

        // body: "1"
        tt_http_parser_wpos(&hp, &wp, &wlen);
        tt_memcpy(wp, &body[0], 1);
        tt_http_parser_inc_wp(&hp, 1);
        TT_UT_SUCCESS(tt_http_parser_run(&hp), "");
        TT_UT_EQUAL(hp.body_counter, 1, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&hp.body, "1"), 0, "");
        TT_UT_FALSE(hp.complete_message, "");

        // body: "23"
        tt_http_parser_wpos(&hp, &wp, &wlen);
        tt_memcpy(wp, &body[1], 2);
        tt_http_parser_inc_wp(&hp, 2);
        TT_UT_SUCCESS(tt_http_parser_run(&hp), "");
        TT_UT_EQUAL(hp.body_counter, 2, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&hp.body, "23"), 0, "");
        TT_UT_FALSE(hp.complete_message, "");

        // body: "4567890"
        tt_http_parser_wpos(&hp, &wp, &wlen);
        tt_memcpy(wp, &body[3], 7);
        tt_http_parser_inc_wp(&hp, 7);
        TT_UT_SUCCESS(tt_http_parser_run(&hp), "");
        TT_UT_EQUAL(hp.body_counter, 3, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&hp.body, "4567890"), 0, "");
        TT_UT_TRUE(hp.complete_message, "");
        TT_UT_FALSE(hp.complete_trailing_header, "");

        // fail if more data
        tt_http_parser_wpos(&hp, &wp, &wlen);
        tt_memcpy(wp, &body[3], 7);
        tt_http_parser_inc_wp(&hp, 7);
        TT_UT_FAIL(tt_http_parser_run(&hp), "");

        tt_http_parser_clear(&hp, TT_TRUE);
    }

    // with chunked body
    {
        tt_char_t p2[] =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain1\r\n"
            "Transfer-Encoding: chunked\r\n"
            "\r\n";

        tt_char_t body[] =
            "25  \r\n"
            "This is the data in the first chunk\r\n"
            "\r\n"
            "1C\r\n"
            "and this is the second one\r\n"
            "\r\n"
            "1C\r\n"
            "and this is the third1 one\r\n"
            "\r\n"
            "0  \r\n"
            "\r\n";

        tt_u32_t slen = sizeof(p2) - 1, i;
        tt_http_rawhdr_t *rh;
        tt_http_rawval_t *rv;

        tt_http_parser_clear(&hp, TT_TRUE);

        TT_UT_FALSE(hp.complete_line1, "");
        TT_UT_FALSE(hp.complete_header, "");
        TT_UT_FALSE(hp.complete_message, "");

        for (i = 0; i < slen;) {
            tt_u32_t this_len;
            if (i + 10 < slen) {
                this_len = tt_rand_u32() % 10 + 1;
            } else {
                this_len = slen - i;
            }
            tt_http_parser_wpos(&hp, &wp, &wlen);
            tt_memcpy(wp, &p2[i], this_len);
            tt_http_parser_inc_wp(&hp, this_len);
            TT_UT_SUCCESS(tt_http_parser_run(&hp), "");
            i += this_len;
        }
        TT_ASSERT(i == slen);

        TT_UT_EQUAL(tt_http_parser_get_contype(&hp), TT_HTTP_CONTYPE_NUM, "");
        TT_UT_TRUE(hp.updated_contype, "");
        TT_UT_TRUE(hp.miss_contype, "");

        TT_UT_EQUAL(tt_http_parser_get_method(&hp), TT_HTTP_METHOD_NUM, "");
        TT_UT_EQUAL(tt_http_parser_get_status(&hp), TT_HTTP_STATUS_OK, "");
        TT_UT_EQUAL(tt_http_parser_get_version(&hp), TT_HTTP_V1_1, "");

        TT_UT_NULL(tt_http_parser_get_host(&hp), "");
        TT_UT_NULL(tt_http_parser_get_host(&hp), "");

        TT_UT_EQUAL(tt_http_parser_get_contype(&hp), TT_HTTP_CONTYPE_NUM, "");
        TT_UT_EQUAL(tt_http_parser_get_contype(&hp), TT_HTTP_CONTYPE_NUM, "");

        __check("Content-Type", "text/plain1");
        __check("Transfer-Encoding", "chunked");

        TT_UT_TRUE(hp.complete_line1, "");
        TT_UT_TRUE(hp.complete_header, "");
        TT_UT_FALSE(hp.complete_message, "");

        // body: "2"
        tt_http_parser_wpos(&hp, &wp, &wlen);
        tt_memcpy(wp, &body[0], 1);
        tt_http_parser_inc_wp(&hp, 1);
        TT_UT_SUCCESS(tt_http_parser_run(&hp), "");
        TT_UT_EQUAL(hp.body_counter, 0, "");
        // body: "5"
        tt_http_parser_wpos(&hp, &wp, &wlen);
        tt_memcpy(wp, &body[1], 1);
        tt_http_parser_inc_wp(&hp, 1);
        TT_UT_SUCCESS(tt_http_parser_run(&hp), "");
        TT_UT_EQUAL(hp.body_counter, 0, "");
        // body: "  \r"
        tt_http_parser_wpos(&hp, &wp, &wlen);
        tt_memcpy(wp, &body[2], 3);
        tt_http_parser_inc_wp(&hp, 3);
        TT_UT_SUCCESS(tt_http_parser_run(&hp), "");
        TT_UT_EQUAL(hp.body_counter, 0, "");
        // body: "\n"
        tt_http_parser_wpos(&hp, &wp, &wlen);
        tt_memcpy(wp, &body[5], 1);
        tt_http_parser_inc_wp(&hp, 1);
        TT_UT_SUCCESS(tt_http_parser_run(&hp), "");
        TT_UT_EQUAL(hp.body_counter, 0, "");

        // body: "This"
        tt_http_parser_wpos(&hp, &wp, &wlen);
        tt_memcpy(wp, &body[6], 4);
        tt_http_parser_inc_wp(&hp, 4);
        TT_UT_SUCCESS(tt_http_parser_run(&hp), "");
        TT_UT_EQUAL(hp.body_counter, 1, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&hp.body, "This"), 0, "");
        TT_UT_FALSE(hp.complete_message, "");

        // body: " is the data in the first chunk\r\n"
        tt_http_parser_wpos(&hp, &wp, &wlen);
        tt_memcpy(wp, &body[10], 33);
        tt_http_parser_inc_wp(&hp, 33);
        TT_UT_SUCCESS(tt_http_parser_run(&hp), "");
        TT_UT_EQUAL(hp.body_counter, 2, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&hp.body,
                                     " is the data in the first chunk\r\n"),
                    0,
                    "");
        TT_UT_FALSE(hp.complete_message, "");

        // body: "\r"
        tt_http_parser_wpos(&hp, &wp, &wlen);
        tt_memcpy(wp, &body[43], 1);
        tt_http_parser_inc_wp(&hp, 1);
        TT_UT_SUCCESS(tt_http_parser_run(&hp), "");
        TT_UT_EQUAL(hp.body_counter, 2, "");
        // body: "\n"
        tt_http_parser_wpos(&hp, &wp, &wlen);
        tt_memcpy(wp, &body[44], 1);
        tt_http_parser_inc_wp(&hp, 1);
        TT_UT_SUCCESS(tt_http_parser_run(&hp), "");
        TT_UT_EQUAL(hp.body_counter, 2, "");

        // body: "1C\r\nand this is the second one\r\n\r\n1C\r\nand this is the
        // third1 one\r\n\r\n0  \r"
        tt_http_parser_wpos(&hp, &wp, &wlen);
        tt_memcpy(wp, &body[45], 72);
        tt_http_parser_inc_wp(&hp, 72);
        TT_UT_SUCCESS(tt_http_parser_run(&hp), "");
        TT_UT_EQUAL(hp.body_counter, 3, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&hp.body,
                                     "and this is the second one\r\n"),
                    0,
                    "");
        TT_UT_FALSE(hp.complete_message, "");

        TT_UT_SUCCESS(tt_http_parser_run(&hp), "");
        TT_UT_EQUAL(hp.body_counter, 4, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&hp.body,
                                     "and this is the third1 one\r\n"),
                    0,
                    "");
        TT_UT_FALSE(hp.complete_message, "");

        TT_UT_EXP(TT_BUF_RLEN(&hp.buf) > 0, "");
        TT_UT_SUCCESS(tt_http_parser_run(&hp), "");
        TT_UT_EQUAL(hp.body_counter, 4, "");
        TT_UT_FALSE(hp.complete_message, "");

        // final
        tt_http_parser_wpos(&hp, &wp, &wlen);
        *wp++ = '\n';
        tt_http_parser_inc_wp(&hp, 1);
        TT_UT_SUCCESS(tt_http_parser_run(&hp), "");
        TT_UT_FALSE(hp.complete_message, "");
        TT_UT_FALSE(hp.complete_trailing_header, "");

        // trailing headers
        {
            tt_char_t th[] =
                "X-Vary: *\r\n"
                "X-Content-Type: text/plain\r\n"
                "X-xxx:\r\n"
                "\r\n";

            tt_http_parser_wpos(&hp, &wp, &wlen);
            tt_memcpy(wp, th, sizeof(th) - 1);
            tt_http_parser_inc_wp(&hp, sizeof(th) - 1);
            TT_UT_SUCCESS(tt_http_parser_run(&hp), "");

            __check_trail("X-Vary", "*");
            __check_trail("X-Content-Type", "text/plain");
            __check_trail("X-xxx", "");

            TT_UT_TRUE(hp.complete_message, "");
            TT_UT_TRUE(hp.complete_trailing_header, "");
        };

        tt_http_parser_clear(&hp, TT_TRUE);
    }

    tt_http_parser_destroy(&hp);

    tt_slab_destroy(&rh);
    tt_slab_destroy(&rv);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_http_body2)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_http_parser_t hp;
    tt_slab_t rh, rv;
    tt_u8_t *wp;
    tt_u32_t wlen;
    tt_u32_t msg_num = 0, slen, i, n;
    tt_u32_t check_len[6] = {0},
             true_len[6] = {
                 0, 0x25 + 0x1c + 0x1c, 10, 0, 0x25 + 0x1c + 0x1c, 10,
             };

    // nobody + chunked + len
    tt_char_t p[] =
        "GET /favicon.ico HTTP/1.1\r\n"
        "Host: 0.0.0.0=5000\r\n"
        "User-Agent: Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.9) "
        "Gecko/2008061015 Firefox/3.0\r\n"
        "Accept: "
        "text/html,application/xhtml+xml,application/xml;q=0.9,*/"
        "*;q=0.8\r\n"
        "Accept-Language: en-us,en;q=0.5\r\n"
        "Accept-Encoding: gzip,deflate\r\n"
        "Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r\n"
        "Keep-Alive: 300\r\n"
        "Connection: keep-alive\r\n"
        "\r\n"

        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Transfer-Encoding: chunked\r\n"
        "\r\n"
        "25  \r\n"
        "This is the data in the first chunk\r\n"
        "\r\n"
        "1C\r\n"
        "and this is the second one\r\n"
        "\r\n"
        "1C\r\n"
        "and this is the third1 one\r\n"
        "\r\n"
        "0  \r\n"
        "X-Vary: *\r\n"
        "X-Content-Type: text/plain\r\n"
        "X-xxx:\r\n"
        "\r\n"

        "PATCH /file.txt HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "Content-Type: application/example\r\n"
        "If-Match: \"e0023aa4e\"\r\n"
        "Content-Length: 10\r\n"
        "\r\n"
        "1234567890"

        "GET /favicon.ico HTTP/1.1\r\n"
        "Host: 0.0.0.0=5000\r\n"
        "User-Agent: Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.9) "
        "Gecko/2008061015 Firefox/3.0\r\n"
        "Accept: "
        "text/html,application/xhtml+xml,application/xml;q=0.9,*/"
        "*;q=0.8\r\n"
        "Accept-Language: en-us,en;q=0.5\r\n"
        "Accept-Encoding: gzip,deflate\r\n"
        "Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r\n"
        "Keep-Alive: 300\r\n"
        "Connection: keep-alive\r\n"
        "\r\n"

        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Transfer-Encoding: chunked\r\n"
        "\r\n"
        "25  \r\n"
        "This is the data in the first chunk\r\n"
        "\r\n"
        "1C\r\n"
        "and this is the second one\r\n"
        "\r\n"
        "1C\r\n"
        "and this is the third1 one\r\n"
        "\r\n"
        "0  \r\n"
        "X-Vary: *\r\n"
        "X-Content-Type: text/plain\r\n"
        "X-xxx:\r\n"
        "\r\n"

        "PATCH /file.txt HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "Content-Type: application/example\r\n"
        "If-Match: \"e0023aa4e\"\r\n"
        "Content-Length: 10\r\n"
        "\r\n"
        "1234567890";


    TT_TEST_CASE_ENTER()
    // test start

    TT_UT_SUCCESS(tt_slab_create(&rh, sizeof(tt_http_rawhdr_t), NULL), "");
    TT_UT_SUCCESS(tt_slab_create(&rv, sizeof(tt_http_rawval_t), NULL), "");

    TT_UT_SUCCESS(tt_http_parser_create(&hp, &rh, &rv, NULL), "");

    i = 0;
    slen = sizeof(p) - 1;
    while (i < slen) {
        tt_http_rawhdr_t *rh;
        tt_http_rawval_t *rv;

        if (slen - i < 50) {
            n = slen - i;
        } else {
            n = tt_rand_u32() % 50 + 1;
        }

        tt_http_parser_wpos(&hp, &wp, &wlen);
        tt_memcpy(wp, &p[i], n);
        tt_http_parser_inc_wp(&hp, n);
        i += n;

        while (tt_http_parser_rlen(&hp) > 0) {
            tt_bool_t prev_uri = hp.complete_line1;
            tt_bool_t prev_hdr = hp.complete_header;
            tt_u32_t body_num = hp.body_counter;
            tt_bool_t prev_th = hp.complete_trailing_header;
            tt_bool_t prev_msg = hp.complete_message;

            TT_UT_SUCCESS(tt_http_parser_run(&hp), "");

            if (!prev_uri && hp.complete_line1) {
                if ((msg_num == 0) || (msg_num == 3)) {
                    TT_UT_EQUAL(tt_http_parser_get_method(&hp),
                                TT_HTTP_MTD_GET,
                                "");
                    TT_UT_EQUAL(tt_blobex_strcmp(&hp.rawuri, "/favicon.ico"),
                                0,
                                "");
                } else if ((msg_num == 1) || (msg_num == 4)) {
                    TT_UT_EQUAL(tt_http_parser_get_status(&hp),
                                TT_HTTP_STATUS_OK,
                                "");
                } else if ((msg_num == 2) || (msg_num == 5)) {
                    TT_UT_EQUAL(tt_http_parser_get_method(&hp),
                                TT_HTTP_MTD_PATCH,
                                "");
                    TT_UT_EQUAL(tt_blobex_strcmp(&hp.rawuri, "/file.txt"),
                                0,
                                "");
                }
            }

            if (!prev_hdr && hp.complete_header) {
                TT_INFO("%d", hp.parser.content_length);
                if ((msg_num == 0) || (msg_num == 3)) {
                    __check("Host", "0.0.0.0=5000");
                    __check("User-Agent",
                            "Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.9) "
                            "Gecko/2008061015 Firefox/3.0");
                    __check("Accept",
                            "text/html,application/xhtml+xml,application/"
                            "xml;q=0.9,*/"
                            "*;q=0.8");
                    __check("Accept-Language", "en-us,en;q=0.5");
                    __check("Accept-Charset", "ISO-8859-1,utf-8;q=0.7,*;q=0.7");
                    __check("Accept-Encoding", "gzip,deflate");
                    __check("Keep-Alive", "300");
                    __check("Connection", "keep-alive");
                } else if ((msg_num == 1) || (msg_num == 4)) {
                    __check("Content-Type", "text/plain");
                    __check("Transfer-Encoding", "chunked");
                } else if ((msg_num == 2) || (msg_num == 5)) {
                    __check("Host", "www.example.com");
                    __check("Content-Type", "application/example");
                    __check("If-Match", "\"e0023aa4e\"");
                    __check("Content-Length", "10");
                }
            }

            if (body_num < hp.body_counter) {
                check_len[msg_num] += tt_blobex_len(&hp.body);
            }

            if (!prev_th && hp.complete_trailing_header) {
                if ((msg_num == 1) || (msg_num == 4)) {
                    __check_trail("X-Vary", "*");
                    __check_trail("X-Content-Type", "text/plain");
                    __check_trail("X-xxx", "");
                } else {
                    TT_UT_FALSE(TT_TRUE, "");
                }
            }

            if (!prev_msg && hp.complete_message) {
                TT_UT_EQUAL(check_len[msg_num], true_len[msg_num], "");

                ++msg_num;
                tt_http_parser_clear(&hp, TT_FALSE);
            }
        }
    }
    TT_ASSERT(i == slen);

    tt_http_parser_destroy(&hp);

    tt_slab_destroy(&rh);
    tt_slab_destroy(&rv);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_http_hdr_parse2)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_http_parser_t hp;
    tt_slab_t rh, rv;
    tt_u8_t *wp;
    tt_u32_t wlen;
    tt_u32_t slen, i, n;

    // nobody + chunked + len
    tt_char_t p[] =
        "GET /favicon.ico HTTP/1.1\r\n"
        "Host:   \r\n"
        "User-Agent: Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.9)\r\n"
        "User-Agent: 1u"
        "Gecko/2008061015 Firefox/3.0\r\n"
        "Accept: "
        "text/html,application/xhtml+xml,application/xml;q=0.9,*/"
        "*;q=0.8\r\n"
        "User-Agent: \r\n"
        "Accept-Language: en-us,en;q=0.5\r\n"
        "Accept-Encoding: gzip,deflate\r\n"
        "Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r\n"
        "Keep-Alive: 300\r\n"
        "Connection: keep-alive\r\n"
        "\r\n";

    TT_TEST_CASE_ENTER()
    // test start

    TT_UT_SUCCESS(tt_slab_create(&rh, sizeof(tt_http_rawhdr_t), NULL), "");
    TT_UT_SUCCESS(tt_slab_create(&rv, sizeof(tt_http_rawval_t), NULL), "");

    TT_UT_SUCCESS(tt_http_parser_create(&hp, &rh, &rv, NULL), "");

    slen = sizeof(p) - 1;
    tt_http_parser_wpos(&hp, &wp, &wlen);
    TT_UT_EXP(wlen > slen, "");
    tt_memcpy(wp, p, slen);
    tt_http_parser_inc_wp(&hp, slen);

    while (tt_http_parser_rlen(&hp) > 0) {
        TT_UT_SUCCESS(tt_http_parser_run(&hp), "");
        if (hp.complete_message) {
            break;
        }
    }

    {
        TT_UT_EQUAL(tt_http_rawhdr_count_name(&hp.rawhdr, "unexist"), 0, "");
        TT_UT_EQUAL(tt_http_rawhdr_count_name(&hp.rawhdr, "connection"), 1, "");
        TT_UT_EQUAL(tt_http_rawhdr_count_name(&hp.rawhdr, "user-AgenT"), 3, "");
    }

    {
        TT_UT_EQUAL(tt_http_parser_get_contype(&hp), TT_HTTP_CONTYPE_NUM, "");
        TT_UT_EQUAL(tt_http_parser_get_contype(&hp), TT_HTTP_CONTYPE_NUM, "");
    }

    {
        tt_blobex_t *ho = tt_http_parser_get_host(&hp);
        TT_UT_NOT_NULL(ho, "");
        TT_UT_EQUAL(tt_blobex_addr(ho), NULL, "");
        TT_UT_EQUAL(tt_blobex_len(ho), 0, "");
    }

    tt_http_parser_destroy(&hp);

    tt_slab_destroy(&rh);
    tt_slab_destroy(&rv);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_http_contype_map)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_http_contype_map_t m;
    tt_http_contype_entry_t *e;

    TT_TEST_CASE_ENTER()
    // test start

    TT_UT_SUCCESS(tt_http_contype_map_create(&m, NULL), "");
    tt_http_contype_map_clear_static(&m);
    tt_http_contype_map_clear_dynamic(&m);
    tt_http_contype_map_clear(&m);
    tt_http_contype_map_destroy(&m);

    TT_UT_SUCCESS(tt_http_contype_map_create(&m, NULL), "");

    // null ext
    {
        TT_UT_SUCCESS(tt_http_contype_map_add(&m,
                                              TT_HTTP_CONTYPE_TXT_HTML,
                                              "app/html",
                                              NULL),
                      "");

        e = tt_http_contype_map_find_type(&m, TT_HTTP_CONTYPE_TXT_HTML);
        TT_UT_NOT_NULL(e, "");
        TT_UT_STREQ(e->name, "app/html", "");
        TT_UT_NULL(e->ext, "");

        e = tt_http_contype_map_find_type(&m, TT_HTTP_CONTYPE_TXT_PLAIN);
        TT_UT_NULL(e, "");

        // find the default
        tt_http_contype_map_set_default(&m, TT_HTTP_CONTYPE_TXT_HTML);
        e = tt_http_contype_map_find_type(&m, TT_HTTP_CONTYPE_TXT_PLAIN);
        TT_UT_NOT_NULL(e, "");
        TT_UT_STREQ(e->name, "app/html", "");
        TT_UT_NULL(e->ext, "");

        e = tt_http_contype_map_find_name(&m, "app/html");
        TT_UT_NOT_NULL(e, "");
        TT_UT_STREQ(e->name, "app/html", "");
        TT_UT_NULL(e->ext, "");

        e = tt_http_contype_map_find_ext(&m, "htm");
        // come from default
        TT_UT_NOT_NULL(e, "");
        TT_UT_EQUAL(e->type, TT_HTTP_CONTYPE_TXT_HTML, "");

        // clear dynamic
        tt_http_contype_map_clear_dynamic(&m);
        TT_UT_NULL(tt_http_contype_map_find_type(&m, TT_HTTP_CONTYPE_TXT_PLAIN),
                   "");
        TT_UT_NULL(tt_http_contype_map_find_type(&m, TT_HTTP_CONTYPE_TXT_HTML),
                   "");

        tt_http_contype_map_clear_static(&m);
        tt_http_contype_map_clear_dynamic(&m);
        tt_http_contype_map_clear(&m);
    }

    // some ext
    {
        TT_UT_SUCCESS(tt_http_contype_map_add(&m,
                                              TT_HTTP_CONTYPE_TXT_HTML,
                                              "html",
                                              ";"),
                      "");
        TT_UT_SUCCESS(tt_http_contype_map_add(&m,
                                              TT_HTTP_CONTYPE_TXT_PLAIN,
                                              "plain",
                                              ";a;b;c;"),
                      "");
        TT_UT_SUCCESS(tt_http_contype_map_add(&m,
                                              TT_HTTP_CONTYPE_APP_JS,
                                              "js",
                                              "1;2;3"),
                      "");
        TT_UT_SUCCESS(tt_http_contype_map_add(&m,
                                              TT_HTTP_CONTYPE_TXT_CSS,
                                              "css",
                                              "xxx"),
                      "");

        e = tt_http_contype_map_find_name(&m, "js");
        TT_UT_NOT_NULL(e, "");
        TT_UT_STREQ(e->name, "js", "");
        TT_UT_STREQ(e->ext, "1;2;3", "");

        e = tt_http_contype_map_find_ext(&m, "a");
        TT_UT_NOT_NULL(e, "");
        TT_UT_STREQ(e->name, "plain", "");
        e = tt_http_contype_map_find_ext(&m, "c");
        TT_UT_NOT_NULL(e, "");
        TT_UT_STREQ(e->name, "plain", "");

        e = tt_http_contype_map_find_ext(&m, "1");
        TT_UT_NOT_NULL(e, "");
        TT_UT_STREQ(e->name, "js", "");
        e = tt_http_contype_map_find_ext(&m, "3");
        TT_UT_NOT_NULL(e, "");
        TT_UT_STREQ(e->name, "js", "");

        e = tt_http_contype_map_find_ext(&m, "xxx");
        TT_UT_NOT_NULL(e, "");
        TT_UT_STREQ(e->name, "css", "");
        e = tt_http_contype_map_find_ext(&m, "xx");
        TT_UT_NULL(e, "");

        // remove some
        tt_http_contype_map_remove(&m, TT_HTTP_CONTYPE_TXT_CSS);
        e = tt_http_contype_map_find_type(&m, TT_HTTP_CONTYPE_TXT_CSS);
        TT_UT_NULL(e, "");
        e = tt_http_contype_map_find_ext(&m, "xxx");
        TT_UT_NULL(e, "");

        tt_http_contype_map_remove(&m, TT_HTTP_CONTYPE_APP_JS);
        e = tt_http_contype_map_find_type(&m, TT_HTTP_CONTYPE_APP_JS);
        TT_UT_NULL(e, "");
        e = tt_http_contype_map_find_ext(&m, "1");
        TT_UT_NULL(e, "");
        e = tt_http_contype_map_find_ext(&m, "3");
        TT_UT_NULL(e, "");

        tt_http_contype_map_remove(&m, TT_HTTP_CONTYPE_TXT_PLAIN);
        e = tt_http_contype_map_find_type(&m, TT_HTTP_CONTYPE_TXT_PLAIN);
        TT_UT_NULL(e, "");
        e = tt_http_contype_map_find_ext(&m, "a");
        TT_UT_NULL(e, "");
        e = tt_http_contype_map_find_ext(&m, "c");
        TT_UT_NULL(e, "");

        tt_http_contype_map_clear_dynamic(&m);
        e = tt_http_contype_map_find_type(&m, TT_HTTP_CONTYPE_TXT_HTML);
        TT_UT_NULL(e, "");
        e = tt_http_contype_map_find_name(&m, "html");
        TT_UT_NULL(e, "");
    }

    // static
    {
        tt_http_contype_map_set_static(&m, tt_g_http_contype_static);

        e = tt_http_contype_map_find_type(&m, TT_HTTP_CONTYPE_TXT_PLAIN);
        TT_UT_NOT_NULL(e, "");
        TT_UT_STREQ(e->name, "text/plain", "");
        TT_UT_STREQ(e->ext, "txt", "");

        e = tt_http_contype_map_find_name(&m, "text/css");
        TT_UT_NOT_NULL(e, "");
        TT_UT_STREQ(e->name, "text/css", "");
        TT_UT_STREQ(e->ext, "css", "");

        e = tt_http_contype_map_find_ext(&m, "html");
        TT_UT_NOT_NULL(e, "");
        TT_UT_STREQ(e->name, "text/html", "");
        TT_UT_STREQ(e->ext, "htm;html;htx;xhtml", "");
    }

    // dynamic then static
    {
        tt_http_contype_map_clear(&m);

        TT_UT_SUCCESS(tt_http_contype_map_add(&m,
                                              TT_HTTP_CONTYPE_TXT_HTML,
                                              "text/html",
                                              "htm;html;htx;xhtml"),
                      "");

        tt_http_contype_map_set_static(&m, tt_g_http_contype_static);

        e = tt_http_contype_map_find_type(&m, TT_HTTP_CONTYPE_TXT_HTML);
        TT_UT_EQUAL(e, &m.dynamic_entry[TT_HTTP_CONTYPE_TXT_HTML], "");
        e = tt_http_contype_map_find_name(&m, "text/html");
        TT_UT_EQUAL(e, &m.dynamic_entry[TT_HTTP_CONTYPE_TXT_HTML], "");
        e = tt_http_contype_map_find_ext(&m, "xhtml");
        TT_UT_EQUAL(e, &m.dynamic_entry[TT_HTTP_CONTYPE_TXT_HTML], "");
    }

    // static THEN dynamic
    {
        tt_http_contype_map_clear(&m);

        tt_http_contype_map_set_static(&m, tt_g_http_contype_static);

        TT_UT_SUCCESS(tt_http_contype_map_add(&m,
                                              TT_HTTP_CONTYPE_TXT_HTML,
                                              "text/html",
                                              "htm;html;htx;xhtml"),
                      "");

        e = tt_http_contype_map_find_type(&m, TT_HTTP_CONTYPE_TXT_HTML);
        TT_UT_EQUAL(e, &m.dynamic_entry[TT_HTTP_CONTYPE_TXT_HTML], "");
        e = tt_http_contype_map_find_name(&m, "text/html");
        TT_UT_EQUAL(e, &m.dynamic_entry[TT_HTTP_CONTYPE_TXT_HTML], "");
        e = tt_http_contype_map_find_ext(&m, "xhtml");
        TT_UT_EQUAL(e, &m.dynamic_entry[TT_HTTP_CONTYPE_TXT_HTML], "");
    }

    tt_http_contype_map_destroy(&m);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_http_hdr_txenc)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_http_hdr_t *h;
    tt_u8_t e[10];

    TT_TEST_CASE_ENTER()
    // test start

    h = tt_http_hdr_txenc_create();
    TT_UT_NOT_NULL(h, "");
    TT_UT_EQUAL(tt_http_hdr_render_len(h), 0, "");
    TT_UT_EQUAL(tt_http_hdr_render(h, NULL), 0, "");
    tt_http_hdr_destroy(h);

    h = tt_http_hdr_txenc_create();
    TT_UT_NOT_NULL(h, "");
    TT_UT_FALSE(h->missed_field, "");

    TT_UT_SUCCESS(tt_http_hdr_parse(h, ""), "");
    TT_UT_EQUAL(tt_http_hdr_txenc_get(h, e), 0, "");
    TT_UT_EQUAL(tt_http_hdr_render_len(h), 0, "");
    TT_UT_EQUAL(tt_http_hdr_render(h, NULL), 0, "");

    tt_http_hdr_parse(h, "AA,BB");
    TT_UT_EQUAL(tt_http_hdr_txenc_get(h, e), 0, "");
    TT_UT_TRUE(h->missed_field, "");

    TT_UT_SUCCESS(tt_http_hdr_parse(h, "ChunkeD"), "");
    TT_UT_EQUAL(tt_http_hdr_txenc_get(h, e), 1, "");
    TT_UT_EQUAL(e[0], TT_HTTP_TXENC_CHUNKED, "");
    TT_UT_EQUAL(tt_http_hdr_render_len(h), 0, "");
    TT_UT_EQUAL(tt_http_hdr_render(h, NULL), 0, "");

    TT_UT_SUCCESS(tt_http_hdr_parse(h, " , GZIP, DEFLATE ,ChunkeD,"), "");
    TT_UT_EQUAL(tt_http_hdr_txenc_get(h, e), 4, "");
    TT_UT_EQUAL(e[0], TT_HTTP_TXENC_CHUNKED, "");
    TT_UT_EQUAL(e[1], TT_HTTP_TXENC_GZIP, "");
    TT_UT_EQUAL(e[2], TT_HTTP_TXENC_DEFLATE, "");
    TT_UT_EQUAL(e[3], TT_HTTP_TXENC_CHUNKED, "");

    // can have max 5
    tt_http_hdr_parse(h, " , compress, compress ,compress,");
    TT_UT_EQUAL(tt_http_hdr_txenc_get(h, e), 5, "");
    TT_UT_EQUAL(e[0], TT_HTTP_TXENC_CHUNKED, "");
    TT_UT_EQUAL(e[1], TT_HTTP_TXENC_GZIP, "");
    TT_UT_EQUAL(e[2], TT_HTTP_TXENC_DEFLATE, "");
    TT_UT_EQUAL(e[3], TT_HTTP_TXENC_CHUNKED, "");
    TT_UT_EQUAL(e[4], TT_HTTP_TXENC_COMPRESS, "");
    TT_UT_EQUAL(tt_http_hdr_render_len(h), 0, "");
    TT_UT_EQUAL(tt_http_hdr_render(h, NULL), 0, "");

    tt_http_hdr_destroy(h);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_http_hdr_contenc)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_http_hdr_t *h;
    tt_result_t ret;
    tt_u8_t e[10];
    tt_http_enc_t e2[10];
    tt_u32_t len;
    tt_char_t buf[100];

    TT_TEST_CASE_ENTER()
    // test start

    h = tt_http_hdr_contenc_create();
    TT_UT_NOT_NULL(h, "");
    TT_UT_EQUAL(tt_http_hdr_render_len(h), 0, "");
    TT_UT_EQUAL(tt_http_hdr_render(h, NULL), 0, "");
    tt_http_hdr_destroy(h);

    h = tt_http_hdr_contenc_create();

    // 1 token
    ret = tt_http_hdr_parse(h, ",gZip , ");
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_http_hdr_contenc_get(h, e), 1, "");
    TT_UT_EQUAL(e[0], TT_HTTP_ENC_GZIP, "");
    TT_UT_FALSE(h->missed_field, "");

    len = sizeof("Content-Encoding: gzip\r\n") - 1;
    TT_UT_EQUAL(tt_http_hdr_render_len(h), len, "");
    tt_memset(buf, 0, sizeof(buf));
    TT_UT_EQUAL(tt_http_hdr_render(h, buf), len, "");
    TT_UT_STREQ(buf, "Content-Encoding: gzip\r\n", "");

    // more tokens
    ret = tt_http_hdr_parse(h, ",aaa, Deflate, bbb,Gzip , ");
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_http_hdr_contenc_get(h, e), 3, "");
    TT_UT_EQUAL(e[0], TT_HTTP_ENC_GZIP, "");
    TT_UT_EQUAL(e[1], TT_HTTP_ENC_DEFLATE, "");
    TT_UT_EQUAL(e[2], TT_HTTP_ENC_GZIP, "");
    TT_UT_TRUE(h->missed_field, "");

    len = sizeof("Content-Encoding: gzip, deflate, gzip\r\n") - 1;
    TT_UT_EQUAL(tt_http_hdr_render_len(h), len, "");
    tt_memset(buf, 0, sizeof(buf));
    TT_UT_EQUAL(tt_http_hdr_render(h, buf), len, "");
    TT_UT_STREQ(buf, "Content-Encoding: gzip, deflate, gzip\r\n", "");

    // overflow
    h->missed_field = TT_TRUE;
    ret = tt_http_hdr_parse(h, "br, identity, identity,identity , ");
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_http_hdr_contenc_get(h, e), TT_HTTP_ENC_NUM, "");
    TT_UT_EQUAL(e[0], TT_HTTP_ENC_GZIP, "");
    TT_UT_EQUAL(e[1], TT_HTTP_ENC_DEFLATE, "");
    TT_UT_EQUAL(e[2], TT_HTTP_ENC_GZIP, "");
    TT_UT_EQUAL(e[3], TT_HTTP_ENC_BR, "");
    TT_UT_EQUAL(e[4], TT_HTTP_ENC_IDENTITY, "");
    TT_UT_TRUE(h->missed_field, "");

    // clear
    tt_http_hdr_contenc_set(h, NULL, 0);
    TT_UT_EQUAL(tt_http_hdr_render_len(h), 0, "");
    TT_UT_EQUAL(tt_http_hdr_render(h, NULL), 0, "");

    // set
    e2[0] = TT_HTTP_ENC_GZIP;
    e2[1] = TT_HTTP_ENC_BR;
    e2[2] = TT_HTTP_ENC_DEFLATE;
    e2[3] = TT_HTTP_ENC_IDENTITY;
    e2[4] = TT_HTTP_ENC_COMPRESS;
    tt_http_hdr_contenc_set(h, e2, 5);

    len =
        sizeof("Content-Encoding: gzip, br, deflate, identity, compress\r\n") -
        1;
    TT_UT_EQUAL(tt_http_hdr_render_len(h), len, "");
    tt_memset(buf, 0, sizeof(buf));
    TT_UT_EQUAL(tt_http_hdr_render(h, buf), len, "");
    TT_UT_STREQ(buf,
                "Content-Encoding: gzip, br, deflate, identity, compress\r\n",
                "");

    tt_http_hdr_destroy(h);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_http_misc)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_float_t q;

    TT_TEST_CASE_ENTER()
// test start

#define ut_parse_q(ss, pp, nn, qq)                                             \
    {                                                                          \
        tt_char_t buf[] = ss;                                                  \
        tt_char_t *p = buf;                                                    \
        tt_u32_t n = sizeof(buf) - 1;                                          \
        q = 100;                                                               \
        tt_http_parse_weight(&p, &n, &q);                                      \
        TT_UT_EQUAL(p, buf + pp, "");                                          \
        TT_UT_EQUAL(n, nn, "");                                                \
        TT_UT_EXP(fabs(q - qq) < 0.001, "");                                   \
    }

    ut_parse_q("", 0, 0, 1.0f);

    ut_parse_q(" ", 1, 0, 1.0f);

    ut_parse_q(" 1", 1, 1, 1.0f);

    ut_parse_q(" 1.2.3 ", 1, 5, 1.0f);

    ut_parse_q(" 1.2.3 ; ", 1, 5, 1.0f);

    ut_parse_q(" 1.2.3 ; q =", 1, 5, 1.0f);

    // invalid val
    ut_parse_q(" 1.2.3 ; q= ", 1, 5, 0.0f);
    ut_parse_q(" 1.2.3 ; q=", 1, 5, 0.0f);

    ut_parse_q(" 1.2.3 ; q= 1", 1, 5, 0.0f);

    ut_parse_q(" 1.2.3 ; q=1", 1, 5, 1.0f);
    ut_parse_q(" 1.2.3 ; q=a.2", 1, 5, 0.0f);
    ut_parse_q(" 1.2.3 ; q=-1", 1, 5, 0.0f);
    ut_parse_q(" 1.2.3 ; q=1 ", 1, 5, 1.0f);

    ut_parse_q(" 1.2.3 ; q=1.", 1, 5, 1.0f);
    ut_parse_q(" 1.2.3 ; q=1x", 1, 5, 1.0f);
    ut_parse_q(" 1.2.3 ; q=1. ", 1, 5, 1.0f);

    ut_parse_q(" 1.2.3 ; q=0.2", 1, 5, 0.2f);
    ut_parse_q(" 1.2.3 ; q=1.x", 1, 5, 1.0f);
    ut_parse_q(" 1.2.3 ; q=0.3 ", 1, 5, 0.3f);

    ut_parse_q(" 1.2.3 ; q=0.22", 1, 5, 0.22f);
    ut_parse_q(" 1.2.3 ; q=0.2x", 1, 5, 0.2f);
    ut_parse_q(" 1.2.3 ; q=0.33 ", 1, 5, 0.33f);

    ut_parse_q(" 1.2.3 ; q=0.222", 1, 5, 0.222f);
    ut_parse_q(" 1.2.3 ; q=0.22x", 1, 5, 0.22f);
    ut_parse_q(" 1.2.3 ; q=0.333 ", 1, 5, 0.333f);

    ut_parse_q(" 1.2.3 ; q=0.2222", 1, 5, 0.222f);
    ut_parse_q(" 1.2.3 ; q=0.222x", 1, 5, 0.222f);
    ut_parse_q(" 1.2.3 ; q=0.3333 ", 1, 5, 0.333f);

    ut_parse_q(" 1.2.3 ; q=1.3333 ", 1, 5, 1.0f);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_http_hdr_accenc)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_http_hdr_t *h;
    tt_char_t buf[128];
    tt_http_accenc_t *ha;

    TT_TEST_CASE_ENTER()
    // test start

    h = tt_http_hdr_accenc_create();
    TT_UT_NOT_NULL(h, "");
    TT_UT_EXP(tt_http_hdr_render_len(h) >= sizeof("Accept-Encoding: \r\n") - 1,
              "");
    tt_memset(buf, 0, sizeof(buf));
    TT_UT_EQUAL(tt_http_hdr_render(h, buf),
                sizeof("Accept-Encoding: \r\n") - 1,
                "");
    TT_UT_STREQ(buf, "Accept-Encoding: \r\n", "");

    ha = tt_http_hdr_accenc_get(h);
    TT_UT_NOT_NULL(ha, "");

    // 1 token
    ha->has[TT_HTTP_ENC_BR] = TT_TRUE;
    TT_UT_EXP(tt_http_hdr_render_len(h) >=
                  sizeof("Accept-Encoding: br\r\n") - 1,
              "");
    tt_memset(buf, 0, sizeof(buf));
    TT_UT_EQUAL(tt_http_hdr_render(h, buf),
                sizeof("Accept-Encoding: br\r\n") - 1,
                "");
    TT_UT_STREQ(buf, "Accept-Encoding: br\r\n", "");

    // 1 token with q
    ha->has[TT_HTTP_ENC_BR] = TT_TRUE;
    ha->weight[TT_HTTP_ENC_BR] = 0.2;
    TT_UT_EXP(tt_http_hdr_render_len(h) >=
                  sizeof("Accept-Encoding: br;q=0.2\r\n") - 1,
              "");
    tt_memset(buf, 0, sizeof(buf));
    TT_UT_EQUAL(tt_http_hdr_render(h, buf),
                sizeof("Accept-Encoding: br;q=0.2\r\n") - 1,
                "");
    TT_UT_STREQ(buf, "Accept-Encoding: br;q=0.2\r\n", "");

    // 2 token
    ha->has[TT_HTTP_ENC_GZIP] = TT_TRUE;
    // ha->weight[TT_HTTP_ENC_GZIP] = 0.3;
    ha->weight[TT_HTTP_ENC_DEFLATE] = 0.4;
    TT_UT_EXP(tt_http_hdr_render_len(h) >=
                  sizeof("Accept-Encoding: br;q=0.2, gzip\r\n") - 1,
              "");
    tt_memset(buf, 0, sizeof(buf));
    TT_UT_EQUAL(tt_http_hdr_render(h, buf),
                sizeof("Accept-Encoding: br;q=0.2, gzip\r\n") - 1,
                "");
    TT_UT_STREQ(buf, "Accept-Encoding: br;q=0.2, gzip\r\n", "");

    // 3 token
    ha->has[TT_HTTP_ENC_DEFLATE] = TT_TRUE;
    TT_UT_EXP(tt_http_hdr_render_len(h) >=
                  sizeof("Accept-Encoding: deflate;q=0.4, br;q=0.2, gzip\r\n") -
                      1,
              "");
    tt_memset(buf, 0, sizeof(buf));
    TT_UT_EQUAL(tt_http_hdr_render(h, buf),
                sizeof("Accept-Encoding: deflate;q=0.4, br;q=0.2, gzip\r\n") -
                    1,
                "");
    TT_UT_STREQ(buf, "Accept-Encoding: deflate;q=0.4, br;q=0.2, gzip\r\n", "");

    // with aster
    ha->has_aster = TT_TRUE;
    TT_UT_EXP(tt_http_hdr_render_len(h) >=
                  sizeof(
                      "Accept-Encoding: deflate;q=0.4, br;q=0.2, gzip, *\r\n") -
                      1,
              "");
    tt_memset(buf, 0, sizeof(buf));
    TT_UT_EQUAL(tt_http_hdr_render(h, buf),
                sizeof(
                    "Accept-Encoding: deflate;q=0.4, br;q=0.2, gzip, *\r\n") -
                    1,
                "");
    TT_UT_STREQ(buf,
                "Accept-Encoding: deflate;q=0.4, br;q=0.2, gzip, *\r\n",
                "");

    // with aster weight
    ha->has_aster = TT_TRUE;
    ha->aster_weight = 0;
    TT_UT_EXP(tt_http_hdr_render_len(h) >=
                  sizeof("Accept-Encoding: deflate;q=0.4, br;q=0.2, gzip, "
                         "*;q=0\r\n") -
                      1,
              "");
    tt_memset(buf, 0, sizeof(buf));
    TT_UT_EQUAL(
        tt_http_hdr_render(h, buf),
        sizeof("Accept-Encoding: deflate;q=0.4, br;q=0.2, gzip, *;q=0\r\n") - 1,
        "");
    TT_UT_STREQ(buf,
                "Accept-Encoding: deflate;q=0.4, br;q=0.2, gzip, *;q=0\r\n",
                "");

    tt_http_hdr_destroy(h);

    h = tt_http_hdr_accenc_create();
    TT_UT_NOT_NULL(h, "");

    // a single aster
    ha = tt_http_hdr_accenc_get(h);

    tt_http_accenc_set_aster(ha, TT_TRUE, 1.2);

    TT_UT_EXP(tt_http_hdr_render_len(h) >=
                  sizeof("Accept-Encoding: *;q=1\r\n") - 1,
              "");
    tt_memset(buf, 0, sizeof(buf));
    TT_UT_EQUAL(tt_http_hdr_render(h, buf),
                sizeof("Accept-Encoding: *;q=1\r\n") - 1,
                "");
    TT_UT_STREQ(buf, "Accept-Encoding: *;q=1\r\n", "");

    // remove aster weight
    tt_http_accenc_set_aster(ha, TT_TRUE, -1.2);
    tt_memset(buf, 0, sizeof(buf));
    TT_UT_EQUAL(tt_http_hdr_render(h, buf),
                sizeof("Accept-Encoding: *\r\n") - 1,
                "");
    TT_UT_STREQ(buf, "Accept-Encoding: *\r\n", "");

    {
        tt_http_accenc_t a;
        tt_http_accenc_init(&a);
        tt_http_accenc_set(&a, TT_HTTP_ENC_COMPRESS, TT_TRUE, 0.1);
        tt_http_accenc_set_aster(&a, TT_TRUE, 0.2);
        tt_http_hdr_accenc_set(h, &a);

        tt_memset(buf, 0, sizeof(buf));
        TT_UT_EQUAL(tt_http_hdr_render(h, buf),
                    sizeof("Accept-Encoding: compress;q=0.1, *;q=0.2\r\n") - 1,
                    "");
        TT_UT_STREQ(buf, "Accept-Encoding: compress;q=0.1, *;q=0.2\r\n", "");
    }

    tt_http_hdr_destroy(h);

    /////////////////////////////////////////////////////////////////

    h = tt_http_hdr_accenc_create();
    TT_UT_NOT_NULL(h, "");

    TT_UT_SUCCESS(tt_http_hdr_parse(h, ""), "");
    ha = tt_http_hdr_accenc_get(h);
    //...
    TT_UT_FALSE(ha->has_aster, "");
    TT_UT_FALSE(ha->has[TT_HTTP_ENC_GZIP], "");
    TT_UT_FALSE(ha->has[TT_HTTP_ENC_COMPRESS], "");

    TT_UT_SUCCESS(tt_http_hdr_parse(h, "* ; q=-0.99"), "");
    ha = tt_http_hdr_accenc_get(h);
    TT_UT_TRUE(ha->has_aster, "");
    TT_UT_EXP(fabs(ha->aster_weight - 0) < 0.0001, "");

    TT_UT_SUCCESS(
        tt_http_hdr_parse(h, " GZIP ; q=1.2, compress;q=0,*;q = 0.5 ,BR; xx;"),
        "");
    ha = tt_http_hdr_accenc_get(h);
    TT_UT_TRUE(ha->has_aster, "");
    // aster weight "q =" is invalid, and considered missed and use 1.0 as
    // default
    TT_UT_EXP(fabs(ha->aster_weight - 1.0) < 0.0001, "");
    TT_UT_TRUE(ha->has[TT_HTTP_ENC_GZIP], "");
    TT_UT_EXP(fabs(ha->weight[TT_HTTP_ENC_GZIP] - 1.0) < 0.0001, "");
    TT_UT_TRUE(ha->has[TT_HTTP_ENC_COMPRESS], "");
    TT_UT_EXP(fabs(ha->weight[TT_HTTP_ENC_COMPRESS] - 0) < 0.0001, "");
    TT_UT_TRUE(ha->has[TT_HTTP_ENC_BR], "");
    TT_UT_EXP(fabs(ha->weight[TT_HTTP_ENC_BR] - 1.0) < 0.0001, "");

    // no q
    TT_UT_SUCCESS(tt_http_hdr_parse(h, "  xx; ,yy, deflate"), "");
    TT_UT_TRUE(ha->has[TT_HTTP_ENC_DEFLATE], "");
    TT_UT_EXP(fabs(ha->weight[TT_HTTP_ENC_DEFLATE] - 1.0) < 0.0001, "");

    // overwrite
    TT_UT_SUCCESS(tt_http_hdr_parse(h, "deflate;q=0.2a"), "");
    TT_UT_TRUE(ha->has[TT_HTTP_ENC_DEFLATE], "");
    TT_UT_EXP(fabs(ha->weight[TT_HTTP_ENC_DEFLATE] - 0.2) < 0.0001, "");

    tt_http_hdr_destroy(h);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_http_hdr_etag)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_http_hdr_t *h;
    tt_http_etag_t *e;
    tt_char_t tmp[128] = {0};
    tt_u32_t len;

    TT_TEST_CASE_ENTER()
    // test start

    h = tt_http_hdr_etag_create();
    TT_UT_NOT_NULL(h, "");
    TT_UT_NULL(tt_http_etag_head(h), "");
    TT_UT_EQUAL(tt_http_hdr_render_len(h), sizeof("ETag: \r\n") - 1, "");
    tt_memset(tmp, 0, sizeof(tmp));
    TT_UT_EQUAL(tt_http_hdr_render(h, tmp), sizeof("ETag: \r\n") - 1, "");
    TT_UT_STREQ(tmp, "ETag: \r\n", "");
    tt_http_hdr_destroy(h);

    h = tt_http_hdr_etag_create();
    TT_UT_NOT_NULL(h, "");

    // empty
    tt_http_hdr_parse(h, "");
    TT_UT_NULL(tt_http_etag_head(h), "");

    // 1 char: invalid as must wrapped by double quotes
    tt_http_hdr_parse(h, "a");
    TT_UT_NULL(tt_http_etag_head(h), "");
    // invalid as must wrapped by double quotes
    tt_http_hdr_parse(h, "a , b, c,");
    TT_UT_NULL(tt_http_etag_head(h), "");

    // empty tag
    tt_http_hdr_parse(h, "\"\"");
    TT_UT_NULL(tt_http_etag_head(h), "");

    {
        // 1 tag
        tt_http_hdr_parse(h, "\" a  \"");

        e = tt_http_etag_head(h);
        TT_UT_NOT_NULL(e, "");
        TT_UT_FALSE(e->weak, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&e->etag, " a  "), 0, "");
    }

    {
        // try more
        tt_http_hdr_parse(h, " W/\"abc\", \"\" , \"aa, bb\", W/, \"end\"");

        e = tt_http_etag_head(h);
        TT_UT_NOT_NULL(e, "");
        e = tt_http_etag_next(e); // 2nd
        TT_UT_NOT_NULL(e, "");
        TT_UT_TRUE(e->weak, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&e->etag, "abc"), 0, "");
        e = tt_http_etag_next(e); // 3rd
        TT_UT_NOT_NULL(e, "");
        TT_UT_FALSE(e->weak, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&e->etag, "end"), 0, "");
        e = tt_http_etag_next(e); // 3rd
        TT_UT_NULL(e, "");
    }

    tt_http_hdr_destroy(h);

    ///////////// render

    h = tt_http_hdr_etag_create();
    TT_UT_NOT_NULL(h, "");

    TT_UT_SUCCESS(tt_http_hdr_etag_add(h, "1", TT_FALSE), "");
    TT_UT_EQUAL(tt_http_hdr_render_len(h), sizeof("ETag: \"1\"\r\n") - 1, "");
    tt_memset(tmp, 0, sizeof(tmp));
    TT_UT_EQUAL(tt_http_hdr_render(h, tmp), sizeof("ETag: \"1\"\r\n") - 1, "");
    TT_UT_STREQ(tmp, "ETag: \"1\"\r\n", "");

    TT_UT_SUCCESS(tt_http_hdr_etag_add(h, "22", TT_TRUE), "");
    TT_UT_EQUAL(tt_http_hdr_render_len(h),
                sizeof("ETag: \"1\", W/\"22\"\r\n") - 1,
                "");
    tt_memset(tmp, 0, sizeof(tmp));
    TT_UT_EQUAL(tt_http_hdr_render(h, tmp),
                sizeof("ETag: \"1\", W/\"22\"\r\n") - 1,
                "");
    TT_UT_STREQ(tmp, "ETag: \"1\", W/\"22\"\r\n", "");

    tt_http_hdr_destroy(h);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_http_hdr_ifmatch)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_http_hdr_t *h;
    tt_http_etag_t *e;
    tt_char_t tmp[128] = {0};
    tt_u32_t len;

    TT_TEST_CASE_ENTER()
    // test start

    h = tt_http_hdr_ifmatch_create();
    TT_UT_NOT_NULL(h, "");
    TT_UT_NULL(tt_http_etag_head(h), "");
    TT_UT_EQUAL(tt_http_hdr_render_len(h), sizeof("If-Match: \r\n") - 1, "");
    tt_memset(tmp, 0, sizeof(tmp));
    TT_UT_EQUAL(tt_http_hdr_render(h, tmp), sizeof("If-Match: \r\n") - 1, "");
    TT_UT_STREQ(tmp, "If-Match: \r\n", "");
    tt_http_hdr_destroy(h);

    h = tt_http_hdr_ifmatch_create();
    TT_UT_NOT_NULL(h, "");

    // empty
    tt_http_hdr_parse(h, "");
    TT_UT_NULL(tt_http_etag_head(h), "");

    // 1 char: invalid as must wrapped by double quotes
    tt_http_hdr_parse(h, "a");
    TT_UT_NULL(tt_http_etag_head(h), "");
    // invalid as must wrapped by double quotes
    tt_http_hdr_parse(h, "a , b, c,");
    TT_UT_NULL(tt_http_etag_head(h), "");

    // empty tag
    tt_http_hdr_parse(h, "\"\"");
    TT_UT_NULL(tt_http_etag_head(h), "");

    {
        // 1 tag
        tt_http_hdr_parse(h, "\" a  \"");

        e = tt_http_etag_head(h);
        TT_UT_NOT_NULL(e, "");
        TT_UT_FALSE(e->weak, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&e->etag, " a  "), 0, "");
    }

    {
        // try more
        tt_http_hdr_parse(h, " W/\"abc\", \"\" , \"aa, bb\", W/, \"end\",  * ");

        e = tt_http_etag_head(h);
        TT_UT_NOT_NULL(e, "");
        e = tt_http_etag_next(e); // 2nd
        TT_UT_NOT_NULL(e, "");
        TT_UT_TRUE(e->weak, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&e->etag, "abc"), 0, "");
        e = tt_http_etag_next(e); // 3rd
        TT_UT_NOT_NULL(e, "");
        TT_UT_FALSE(e->weak, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&e->etag, "end"), 0, "");

        e = tt_http_etag_next(e); // 3rd
        TT_UT_NOT_NULL(e, "");
        TT_UT_FALSE(e->weak, "");
        TT_UT_TRUE(e->aster, "");

        e = tt_http_etag_next(e); // 4th
        TT_UT_NULL(e, "");
    }

    tt_http_hdr_destroy(h);

    ///////////// render

    h = tt_http_hdr_ifmatch_create();
    TT_UT_NOT_NULL(h, "");

    TT_UT_SUCCESS(tt_http_hdr_ifmatch_add(h, "1", TT_FALSE), "");
    TT_UT_EQUAL(tt_http_hdr_render_len(h),
                sizeof("If-Match: \"1\"\r\n") - 1,
                "");
    tt_memset(tmp, 0, sizeof(tmp));
    TT_UT_EQUAL(tt_http_hdr_render(h, tmp),
                sizeof("If-Match: \"1\"\r\n") - 1,
                "");
    TT_UT_STREQ(tmp, "If-Match: \"1\"\r\n", "");

    TT_UT_SUCCESS(tt_http_hdr_ifmatch_add(h, "22", TT_TRUE), "");
    TT_UT_EQUAL(tt_http_hdr_render_len(h),
                sizeof("If-Match: \"1\", W/\"22\"\r\n") - 1,
                "");
    tt_memset(tmp, 0, sizeof(tmp));
    TT_UT_EQUAL(tt_http_hdr_render(h, tmp),
                sizeof("If-Match: \"1\", W/\"22\"\r\n") - 1,
                "");
    TT_UT_STREQ(tmp, "If-Match: \"1\", W/\"22\"\r\n", "");

    TT_UT_SUCCESS(tt_http_hdr_ifmatch_add_aster(h), "");
    TT_UT_EQUAL(tt_http_hdr_render_len(h),
                sizeof("If-Match: \"1\", W/\"22\", *\r\n") - 1,
                "");
    tt_memset(tmp, 0, sizeof(tmp));
    TT_UT_EQUAL(tt_http_hdr_render(h, tmp),
                sizeof("If-Match: \"1\", W/\"22\", *\r\n") - 1,
                "");
    TT_UT_STREQ(tmp, "If-Match: \"1\", W/\"22\", *\r\n", "");

    tt_http_hdr_destroy(h);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_http_hdr_ifnmatch)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_http_hdr_t *h;
    tt_http_etag_t *e;
    tt_char_t tmp[128] = {0};
    tt_u32_t len;

    TT_TEST_CASE_ENTER()
    // test start

    h = tt_http_hdr_ifnmatch_create();
    TT_UT_NOT_NULL(h, "");
    TT_UT_NULL(tt_http_etag_head(h), "");
    TT_UT_EQUAL(tt_http_hdr_render_len(h),
                sizeof("If-None-Match: \r\n") - 1,
                "");
    tt_memset(tmp, 0, sizeof(tmp));
    TT_UT_EQUAL(tt_http_hdr_render(h, tmp),
                sizeof("If-None-Match: \r\n") - 1,
                "");
    TT_UT_STREQ(tmp, "If-None-Match: \r\n", "");
    tt_http_hdr_destroy(h);

    h = tt_http_hdr_ifnmatch_create();
    TT_UT_NOT_NULL(h, "");

    // empty
    tt_http_hdr_parse(h, "");
    TT_UT_NULL(tt_http_etag_head(h), "");

    // 1 char: invalid as must wrapped by double quotes
    tt_http_hdr_parse(h, "a");
    TT_UT_NULL(tt_http_etag_head(h), "");
    // invalid as must wrapped by double quotes
    tt_http_hdr_parse(h, "a , b, c,");
    TT_UT_NULL(tt_http_etag_head(h), "");

    // empty tag
    tt_http_hdr_parse(h, "\"\"");
    TT_UT_NULL(tt_http_etag_head(h), "");

    {
        // 1 tag
        tt_http_hdr_parse(h, "\" a  \"");

        e = tt_http_etag_head(h);
        TT_UT_NOT_NULL(e, "");
        TT_UT_FALSE(e->weak, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&e->etag, " a  "), 0, "");
    }

    {
        // try more
        tt_http_hdr_parse(h, " W/\"abc\", \"\" , \"aa, bb\", W/, \"end\",  * ");

        e = tt_http_etag_head(h);
        TT_UT_NOT_NULL(e, "");
        e = tt_http_etag_next(e); // 2nd
        TT_UT_NOT_NULL(e, "");
        TT_UT_TRUE(e->weak, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&e->etag, "abc"), 0, "");
        e = tt_http_etag_next(e); // 3rd
        TT_UT_NOT_NULL(e, "");
        TT_UT_FALSE(e->weak, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&e->etag, "end"), 0, "");

        e = tt_http_etag_next(e); // 3rd
        TT_UT_NOT_NULL(e, "");
        TT_UT_FALSE(e->weak, "");
        TT_UT_TRUE(e->aster, "");

        e = tt_http_etag_next(e); // 4th
        TT_UT_NULL(e, "");
    }

    tt_http_hdr_destroy(h);

    ///////////// render

    h = tt_http_hdr_ifnmatch_create();
    TT_UT_NOT_NULL(h, "");

    TT_UT_SUCCESS(tt_http_hdr_ifnmatch_add(h, "1", TT_FALSE), "");
    TT_UT_EQUAL(tt_http_hdr_render_len(h),
                sizeof("If-None-Match: \"1\"\r\n") - 1,
                "");
    tt_memset(tmp, 0, sizeof(tmp));
    TT_UT_EQUAL(tt_http_hdr_render(h, tmp),
                sizeof("If-None-Match: \"1\"\r\n") - 1,
                "");
    TT_UT_STREQ(tmp, "If-None-Match: \"1\"\r\n", "");

    TT_UT_SUCCESS(tt_http_hdr_ifnmatch_add(h, "22", TT_TRUE), "");
    TT_UT_EQUAL(tt_http_hdr_render_len(h),
                sizeof("If-None-Match: \"1\", W/\"22\"\r\n") - 1,
                "");
    tt_memset(tmp, 0, sizeof(tmp));
    TT_UT_EQUAL(tt_http_hdr_render(h, tmp),
                sizeof("If-None-Match: \"1\", W/\"22\"\r\n") - 1,
                "");
    TT_UT_STREQ(tmp, "If-None-Match: \"1\", W/\"22\"\r\n", "");

    TT_UT_SUCCESS(tt_http_hdr_ifnmatch_add_aster(h), "");
    TT_UT_EQUAL(tt_http_hdr_render_len(h),
                sizeof("If-None-Match: \"1\", W/\"22\", *\r\n") - 1,
                "");
    tt_memset(tmp, 0, sizeof(tmp));
    TT_UT_EQUAL(tt_http_hdr_render(h, tmp),
                sizeof("If-None-Match: \"1\", W/\"22\", *\r\n") - 1,
                "");
    TT_UT_STREQ(tmp, "If-None-Match: \"1\", W/\"22\", *\r\n", "");

    tt_http_hdr_destroy(h);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_http_hdr_auth)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_http_hdr_t *h;
    tt_http_auth_t *ha;
    tt_char_t tmp[128] = {0};
    tt_u32_t len;

    TT_TEST_CASE_ENTER()
    // test start

    h = tt_http_hdr_auth_create();
    TT_UT_NOT_NULL(h, "");

    ha = tt_http_hdr_auth_get(h);
    TT_UT_NOT_NULL(ha, "");
    TT_UT_NULL(tt_blobex_addr(&ha->cnonce), "");
    TT_UT_NULL(tt_blobex_addr(&ha->nc), "");
    TT_UT_NULL(tt_blobex_addr(&ha->nonce), "");
    TT_UT_NULL(tt_blobex_addr(&ha->realm), "");
    TT_UT_NULL(tt_blobex_addr(&ha->opaque), "");
    TT_UT_NULL(tt_blobex_addr(&ha->response), "");
    TT_UT_NULL(tt_blobex_addr(&ha->username), "");
    TT_UT_NULL(tt_blobex_addr(&ha->uri), "");
    TT_UT_NULL(tt_blobex_addr(&ha->domain), "");
    TT_UT_EQUAL(ha->alg, TT_HTTP_AUTH_ALG_NUM, "");
    TT_UT_EQUAL(ha->qop_mask, 0, "");
    TT_UT_EQUAL(ha->stale, TT_HTTP_STALE_NUM, "");
    TT_UT_EQUAL(ha->scheme, TT_HTTP_AUTH_SCHEME_NUM, "");

    tt_http_hdr_destroy(h);

    h = tt_http_hdr_auth_create();
    TT_UT_NOT_NULL(h, "");

    // empty
    tt_http_hdr_parse(h, "");
    ha = tt_http_hdr_auth_get(h);
    TT_UT_NOT_NULL(ha, "");
    TT_UT_NULL(tt_blobex_addr(&ha->cnonce), "");
    TT_UT_NULL(tt_blobex_addr(&ha->nc), "");
    TT_UT_NULL(tt_blobex_addr(&ha->nonce), "");
    TT_UT_NULL(tt_blobex_addr(&ha->realm), "");
    TT_UT_NULL(tt_blobex_addr(&ha->opaque), "");
    TT_UT_NULL(tt_blobex_addr(&ha->response), "");
    TT_UT_NULL(tt_blobex_addr(&ha->username), "");
    TT_UT_NULL(tt_blobex_addr(&ha->uri), "");
    TT_UT_NULL(tt_blobex_addr(&ha->domain), "");
    TT_UT_EQUAL(ha->alg, TT_HTTP_AUTH_ALG_NUM, "");
    TT_UT_EQUAL(ha->qop_mask, 0, "");
    TT_UT_EQUAL(ha->stale, TT_HTTP_STALE_NUM, "");
    TT_UT_EQUAL(ha->scheme, TT_HTTP_AUTH_SCHEME_NUM, "");
    tt_http_hdr_destroy(h);

    // 1 char
    h = tt_http_hdr_auth_create();
    tt_http_hdr_parse(h, "a");
    ha = tt_http_hdr_auth_get(h);
    TT_UT_EQUAL(ha->scheme, TT_HTTP_AUTH_SCHEME_NUM, "");
    tt_http_hdr_destroy(h);

    // incomplete scheme
    h = tt_http_hdr_auth_create();
    tt_http_hdr_parse(h, " diges");
    ha = tt_http_hdr_auth_get(h);
    TT_UT_EQUAL(ha->scheme, TT_HTTP_AUTH_SCHEME_NUM, "");
    tt_http_hdr_destroy(h);

    h = tt_http_hdr_auth_create();
    tt_http_hdr_parse(h, " basic ");
    ha = tt_http_hdr_auth_get(h);
    TT_UT_EQUAL(ha->scheme, TT_HTTP_AUTH_BASIC, "");
    tt_http_hdr_destroy(h);

    // scheme with 1 param
    h = tt_http_hdr_auth_create();
    tt_http_hdr_parse(h, " digest realm=\"abc\" ,");
    ha = tt_http_hdr_auth_get(h);
    TT_UT_EQUAL(ha->scheme, TT_HTTP_AUTH_DIGEST, "");
    TT_UT_EQUAL(tt_blobex_strcmp(&ha->realm, "abc"), 0, "");
    tt_http_hdr_destroy(h);

    // scheme with more param
    {
        const tt_char_t *s =
            "  Digest  "
            " Domain=\"dmdm\",  "
            "username=\"us\",  "
            "  realm=\"  api@example.org  \","
            "uri=\"/doe.json\", "
            "algorithm=md5-sess,  "
            " nonce=\"ncnc \","
            "   nc=00000001,"
            "cnonce=\"cncn\","
            "qop=\" , , ,,, auth,  auth-int \","
            "response=\" resp \","
            "opaque=\"opqrst  \","
            "stale=false,"
            "userhash=true";
        const tt_char_t *s2 =
            "Authorization: Digest "
            "realm=\"  api@example.org  \", "
            "nonce=\"ncnc \", "
            "opaque=\"opqrst  \", "
            "response=\" resp \", "
            "username=\"us\", "
            "uri=\"/doe.json\", "
            "cnonce=\"cncn\", "
            "nc=00000001, "
            "qop=auth, "
            "algorithm=MD5-sess\r\n";
        tt_char_t buf[256] = {0};
        tt_u32_t n;

        h = tt_http_hdr_auth_create();
        tt_http_hdr_parse(h, s);
        ha = tt_http_hdr_auth_get(h);
        TT_UT_EQUAL(ha->scheme, TT_HTTP_AUTH_DIGEST, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&ha->realm, "  api@example.org  "), 0, "");
        TT_UT_EQUAL(tt_blobex_len(&ha->domain), 0, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&ha->nonce, "ncnc "), 0, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&ha->opaque, "opqrst  "), 0, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&ha->response, " resp "), 0, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&ha->username, "us"), 0, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&ha->uri, "/doe.json"), 0, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&ha->cnonce, "cncn"), 0, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&ha->nc, "00000001"), 0, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&ha->raw_qop, " , , ,,, auth,  auth-int "),
                    0,
                    "");
        TT_UT_EQUAL(ha->scheme, TT_HTTP_AUTH_DIGEST, "");
        TT_UT_EQUAL(ha->stale, TT_HTTP_STALE_NUM, "");
        TT_UT_EQUAL(ha->alg, TT_HTTP_AUTH_MD5_SESS, "");
        TT_UT_EQUAL(ha->qop_mask, TT_HTTP_QOP_AUTH | TT_HTTP_QOP_AUTH_INT, "");

        len = tt_http_hdr_render_len(h);
        TT_UT_EQUAL(len, tt_strlen(s2), "");

        n = tt_http_hdr_render(h, buf);
        TT_UT_EQUAL(n, len, "");
        TT_UT_STREQ(buf, s2, "");

        tt_http_hdr_destroy(h);
    }

    // www auth
    {
        const tt_char_t *s =
            "  Basic  "
            " Domain=\"dmdm\",  "
            "username=\"us\",  "
            "  realm=\"  api@example.org  \","
            "uri=\"/doe.json\", "
            "algorithm=md5,  "
            " nonce=\"ncnc \","
            "   nc=00000001,"
            "cnonce=\"cncn\","
            "qop=\" , , ,,, auth,  auth-int \","
            "response=\" resp \","
            "opaque=\"opqrst  \","
            "stale=true,"
            "userhash=true";
        const tt_char_t *s2 =
            "WWW-Authenticate: Basic "
            "realm=\"  api@example.org  \", "
            "domain=\"dmdm\", "
            "nonce=\"ncnc \", "
            "opaque=\"opqrst  \", "
            "qop=\"auth,auth-int\", "
            "stale=true, "
            "algorithm=MD5\r\n";
        tt_char_t buf[256] = {0};
        tt_u32_t n;

        h = tt_http_hdr_www_auth_create();
        tt_http_hdr_parse(h, s);
        ha = tt_http_hdr_auth_get(h);
        TT_UT_EQUAL(ha->scheme, TT_HTTP_AUTH_BASIC, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&ha->realm, "  api@example.org  "), 0, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&ha->domain, "dmdm"), 0, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&ha->nonce, "ncnc "), 0, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&ha->opaque, "opqrst  "), 0, "");
        TT_UT_EQUAL(tt_blobex_len(&ha->response), 0, "");
        TT_UT_EQUAL(tt_blobex_len(&ha->username), 0, "");
        TT_UT_EQUAL(tt_blobex_len(&ha->uri), 0, "");
        TT_UT_EQUAL(tt_blobex_len(&ha->cnonce), 0, "");
        TT_UT_EQUAL(tt_blobex_len(&ha->nc), 0, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&ha->raw_qop, " , , ,,, auth,  auth-int "),
                    0,
                    "");
        TT_UT_EQUAL(ha->scheme, TT_HTTP_AUTH_BASIC, "");
        TT_UT_EQUAL(ha->stale, TT_HTTP_STALE_TRUE, "");
        TT_UT_EQUAL(ha->alg, TT_HTTP_AUTH_MD5, "");
        TT_UT_EQUAL(ha->qop_mask, TT_HTTP_QOP_AUTH | TT_HTTP_QOP_AUTH_INT, "");

        len = tt_http_hdr_render_len(h);
        TT_UT_EQUAL(len, tt_strlen(s2), "");

        n = tt_http_hdr_render(h, buf);
        TT_UT_EQUAL(n, len, "");
        TT_UT_STREQ(buf, s2, "");

        tt_http_hdr_destroy(h);
    }

    // proxy auth
    {
        const tt_char_t *s =
            "  Digest  "
            " Domain=\"dmdm\",  "
            "username=\"us\",  "
            "  realm=\"  api@example.org  \","
            "uri=\"/doe.json\", "
            "algorithm=md5-sess,  "
            " nonce=\"ncnc \","
            "   nc=00000001,"
            "cnonce=\"cncn\","
            "qop=\" , , ,,, auth,  auth-int \","
            "response=\" resp \","
            "opaque=\"opqrst  \","
            "stale=false,"
            "userhash=true";
        const tt_char_t *s2 =
            "Proxy-Authorization: Digest "
            "realm=\"  api@example.org  \", "
            "nonce=\"ncnc \", "
            "opaque=\"opqrst  \", "
            "response=\" resp \", "
            "username=\"us\", "
            "uri=\"/doe.json\", "
            "cnonce=\"cncn\", "
            "nc=00000001, "
            "qop=auth, "
            "algorithm=MD5-sess\r\n";
        tt_char_t buf[256] = {0};
        tt_u32_t n;

        h = tt_http_hdr_proxy_authorization_create();
        tt_http_hdr_parse(h, s);
        ha = tt_http_hdr_auth_get(h);
        TT_UT_EQUAL(ha->scheme, TT_HTTP_AUTH_DIGEST, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&ha->realm, "  api@example.org  "), 0, "");
        TT_UT_EQUAL(tt_blobex_len(&ha->domain), 0, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&ha->nonce, "ncnc "), 0, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&ha->opaque, "opqrst  "), 0, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&ha->response, " resp "), 0, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&ha->username, "us"), 0, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&ha->uri, "/doe.json"), 0, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&ha->cnonce, "cncn"), 0, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&ha->nc, "00000001"), 0, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&ha->raw_qop, " , , ,,, auth,  auth-int "),
                    0,
                    "");
        TT_UT_EQUAL(ha->scheme, TT_HTTP_AUTH_DIGEST, "");
        TT_UT_EQUAL(ha->stale, TT_HTTP_STALE_NUM, "");
        TT_UT_EQUAL(ha->alg, TT_HTTP_AUTH_MD5_SESS, "");
        TT_UT_EQUAL(ha->qop_mask, TT_HTTP_QOP_AUTH | TT_HTTP_QOP_AUTH_INT, "");

        len = tt_http_hdr_render_len(h);
        TT_UT_EQUAL(len, tt_strlen(s2), "");

        n = tt_http_hdr_render(h, buf);
        TT_UT_EQUAL(n, len, "");
        TT_UT_STREQ(buf, s2, "");

        tt_http_hdr_destroy(h);
    }

    {
        const tt_char_t *s =
            "  Basic  "
            " Domain=\"dmdm\",  "
            "username=\"us\",  "
            "  realm=\"  api@example.org  \","
            "uri=\"/doe.json\", "
            "algorithm=md5,  "
            " nonce=\"ncnc \","
            "   nc=00000001,"
            "cnonce=\"cncn\","
            "qop=\" , , ,,, auth,  auth-int \","
            "response=\" resp \","
            "opaque=\"opqrst  \","
            "stale=true,"
            "userhash=true";
        const tt_char_t *s2 =
            "Proxy-Authenticate: Basic "
            "realm=\"  api@example.org  \", "
            "domain=\"dmdm\", "
            "nonce=\"ncnc \", "
            "opaque=\"opqrst  \", "
            "qop=\"auth,auth-int\", "
            "stale=true, "
            "algorithm=MD5\r\n";
        tt_char_t buf[256] = {0};
        tt_u32_t n;

        h = tt_http_hdr_proxy_authenticate_create();
        tt_http_hdr_parse(h, s);
        ha = tt_http_hdr_auth_get(h);
        TT_UT_EQUAL(ha->scheme, TT_HTTP_AUTH_BASIC, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&ha->realm, "  api@example.org  "), 0, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&ha->domain, "dmdm"), 0, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&ha->nonce, "ncnc "), 0, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&ha->opaque, "opqrst  "), 0, "");
        TT_UT_EQUAL(tt_blobex_len(&ha->response), 0, "");
        TT_UT_EQUAL(tt_blobex_len(&ha->username), 0, "");
        TT_UT_EQUAL(tt_blobex_len(&ha->uri), 0, "");
        TT_UT_EQUAL(tt_blobex_len(&ha->cnonce), 0, "");
        TT_UT_EQUAL(tt_blobex_len(&ha->nc), 0, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&ha->raw_qop, " , , ,,, auth,  auth-int "),
                    0,
                    "");
        TT_UT_EQUAL(ha->scheme, TT_HTTP_AUTH_BASIC, "");
        TT_UT_EQUAL(ha->stale, TT_HTTP_STALE_TRUE, "");
        TT_UT_EQUAL(ha->alg, TT_HTTP_AUTH_MD5, "");
        TT_UT_EQUAL(ha->qop_mask, TT_HTTP_QOP_AUTH | TT_HTTP_QOP_AUTH_INT, "");

        len = tt_http_hdr_render_len(h);
        TT_UT_EQUAL(len, tt_strlen(s2), "");

        n = tt_http_hdr_render(h, buf);
        TT_UT_EQUAL(n, len, "");
        TT_UT_STREQ(buf, s2, "");

        tt_http_hdr_destroy(h);
    }

    {
        const tt_char_t *s =
            "Digest username=\"Mufasa\","
            "realm=\"testrealm@host.com\","
            "uri=\"/dir/index.html\","
            "algorithm=MD5,"
            "nonce=\"dcd98b7102dd2f0e8b11d0f600bfb0c093\","
            "nc=00000001,"
            "cnonce=\"0a4f113b\","
            "qop=auth,"
            "response=\"6629fae49393a05397450978507c4ef1\","
            "opaque=\"5ccc069c403ebaf9f0171e9517f40e41\"";
        const tt_char_t *pwd = "Circle Of Life";
        const tt_char_t *mtd = "GET";
        tt_char_t resp[33] = {0};

        tt_http_auth_ctx_t c;

        h = tt_http_hdr_auth_create();
        tt_http_hdr_parse(h, s);
        ha = tt_http_hdr_auth_get(h);

        tt_http_auth_ctx_init(&c);
        tt_http_auth_ctx_new_nonce(&c);
        TT_INFO("nonce: %s", c.nonce);
        tt_http_auth_ctx_destroy(&c);

        TT_UT_SUCCESS(tt_http_auth_ctx_calc(&c,
                                            ha,
                                            (tt_char_t *)pwd,
                                            tt_strlen(pwd),
                                            TT_HTTP_QOP_AUTH,
                                            (tt_char_t *)mtd,
                                            tt_strlen(mtd),
                                            NULL,
                                            0,
                                            resp),
                      "");
        TT_UT_STREQ(resp, "6629fae49393a05397450978507c4ef1", "");

        // again
        TT_UT_SUCCESS(tt_http_auth_ctx_calc(&c,
                                            ha,
                                            (tt_char_t *)pwd,
                                            tt_strlen(pwd),
                                            TT_HTTP_QOP_AUTH,
                                            (tt_char_t *)mtd,
                                            tt_strlen(mtd),
                                            NULL,
                                            0,
                                            resp),
                      "");
        TT_UT_STREQ(resp, "6629fae49393a05397450978507c4ef1", "");

        tt_http_hdr_destroy(h);
    }

    // test end
    TT_TEST_CASE_LEAVE()
}
