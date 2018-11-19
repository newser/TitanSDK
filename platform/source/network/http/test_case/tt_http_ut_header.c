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
TT_TEST_ROUTINE_DECLARE(case_http_body)
TT_TEST_ROUTINE_DECLARE(case_http_body2)
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
    TT_TEST_ROUTINE_DEFINE(case_http_body)
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
            "Connection: keep-alive\r\n"
            "\r\n";

        tt_u32_t slen = sizeof(p) - 1, i;
        tt_dnode_t *rhn;
        tt_http_rawhdr_t *rh;
        tt_http_rawval_t *rv;

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

        __check("Host", "0.0.0.0=5000");
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
    }

    // with body request
    {
        tt_char_t p2[] =
            "PATCH /file.txt HTTP/1.1\r\n"
            "Host: www.example.com\r\n"
            "Content-Type: application/example\r\n"
            "If-Match: \"e0023aa4e\"\r\n"
            "Content-Length: 10\r\n"
            "\r\n";

        tt_char_t body[] = "1234567890abcd";

        tt_u32_t slen = sizeof(p2) - 1, i;
        tt_http_rawhdr_t *rh;
        tt_http_rawval_t *rv;

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

        __check("Host", "www.example.com");
        __check("Content-Type", "application/example");
        __check("If-Match", "\"e0023aa4e\"");
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
            "Content-Type: text/plain\r\n"
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

        TT_UT_EQUAL(tt_http_parser_get_method(&hp), TT_HTTP_METHOD_NUM, "");
        TT_UT_EQUAL(tt_http_parser_get_status(&hp), TT_HTTP_STATUS_OK, "");
        TT_UT_EQUAL(tt_http_parser_get_version(&hp), TT_HTTP_V1_1, "");

        __check("Content-Type", "text/plain");
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
                    TT_UT_EQUAL(tt_blobex_strcmp(&hp.uri, "/favicon.ico"),
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
                    TT_UT_EQUAL(tt_blobex_strcmp(&hp.uri, "/file.txt"), 0, "");
                }
            }

            if (!prev_hdr && hp.complete_header) {
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
