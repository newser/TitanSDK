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
TT_TEST_ROUTINE_DECLARE(case_http_uri_basic)
TT_TEST_ROUTINE_DECLARE(case_http_uri_parse)
TT_TEST_ROUTINE_DECLARE(case_http_uri_parse_query)
TT_TEST_ROUTINE_DECLARE(case_http_uri_encode)
TT_TEST_ROUTINE_DECLARE(case_http_uri_encode_query)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(http_uri_case)

TT_TEST_CASE("case_http_uri_basic",
             "http uri basic",
             case_http_uri_basic,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("case_http_uri_parse",
                 "http uri parsing",
                 case_http_uri_parse,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_http_uri_parse_query",
                 "http uri parsing query",
                 case_http_uri_parse_query,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_http_uri_encode",
                 "http uri encoding",
                 case_http_uri_encode,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_http_uri_encode_query",
                 "http uri encoding query",
                 case_http_uri_encode_query,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(http_uri_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(HTTP_UT_URI, 0, http_uri_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(case_http_uri_parse_query)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(case_http_uri_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_http_uri_t u;

    TT_TEST_CASE_ENTER()
    // test start

    tt_http_uri_init(&u);
    TT_UT_EQUAL(tt_http_uri_get_scheme(&u), TT_HTTP_SCHEME_UNDEFINED, "");
    {
        tt_http_uri_set_scheme(&u, TT_HTTP_SCHEME_HTTP);
        TT_UT_EQUAL(tt_http_uri_get_scheme(&u), TT_HTTP_SCHEME_HTTP, "");
        tt_http_uri_set_scheme(&u, TT_HTTP_SCHEME_HTTPS);
        TT_UT_EQUAL(tt_http_uri_get_scheme(&u), TT_HTTP_SCHEME_HTTPS, "");
        TT_UT_STREQ(tt_http_uri_render(&u), "https:", "");
    }
    TT_UT_STREQ(tt_http_uri_get_userinfo(&u), "", "");
    TT_UT_STREQ(tt_http_uri_get_host(&u), "", "");
    TT_UT_EQUAL(tt_http_uri_get_port(&u), 0, "");
    TT_UT_STREQ(tt_http_uri_get_authority(&u), "", "");
    TT_UT_STREQ(tt_http_uri_get_fragment(&u), "", "");

    // clear after init
    tt_http_uri_clear(&u);
    TT_UT_EQUAL(tt_http_uri_get_scheme(&u), TT_HTTP_SCHEME_UNDEFINED, "");
    TT_UT_STREQ(tt_http_uri_get_userinfo(&u), "", "");
    TT_UT_STREQ(tt_http_uri_get_host(&u), "", "");
    TT_UT_EQUAL(tt_http_uri_get_port(&u), 0, "");
    TT_UT_STREQ(tt_http_uri_get_authority(&u), "", "");
    TT_UT_STREQ(tt_http_uri_get_fragment(&u), "", "");

    // set xxx
    tt_http_uri_set_scheme(&u, TT_HTTP_SCHEME_HTTPS);
    TT_UT_EQUAL(tt_http_uri_get_scheme(&u), TT_HTTP_SCHEME_HTTPS, "");

    TT_UT_SUCCESS(tt_http_uri_set_userinfo(&u, "u"), "");
    TT_UT_STREQ(tt_http_uri_get_userinfo(&u), "u", "");

    TT_UT_SUCCESS(tt_http_uri_set_host(&u, "h"), "");
    TT_UT_STREQ(tt_http_uri_get_host(&u), "h", "");

    tt_http_uri_set_port(&u, 123);
    TT_UT_EQUAL(tt_http_uri_get_port(&u), 123, "");

    TT_UT_STREQ(tt_http_uri_get_authority(&u), "u@h:123", "");

    TT_UT_SUCCESS(tt_http_uri_set_fragment(&u, "fff"), "");
    TT_UT_STREQ(tt_http_uri_get_fragment(&u), "fff", "");

    // clear
    tt_http_uri_clear(&u);

    tt_http_uri_destroy(&u);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_http_uri_parse)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_http_uri_t u;
    tt_fpath_t *fp;

    TT_TEST_CASE_ENTER()
    // test start

    tt_http_uri_init(&u);

    TT_UT_SUCCESS(tt_http_uri_create_cstr(&u, ""), "");
    TT_UT_EQUAL(tt_http_uri_get_scheme(&u), TT_HTTP_SCHEME_UNDEFINED, "");
    TT_UT_STREQ(tt_http_uri_get_userinfo(&u), "", "");
    TT_UT_STREQ(tt_http_uri_get_host(&u), "", "");
    TT_UT_EQUAL(tt_http_uri_get_port(&u), 0, "");
    TT_UT_STREQ(tt_http_uri_get_authority(&u), "", "");
    TT_UT_STREQ(tt_http_uri_get_fragment(&u), "", "");
    {
        tt_http_uri_param_iter_t pi;
        tt_http_uri_pparam_iter(&u, &pi);
        TT_UT_EQUAL(tt_http_uri_param_next(&pi), NULL, "");
    }
    {
        TT_UT_NULL(tt_http_uri_find_pparam(&u, ""), "");
        TT_UT_NULL(tt_http_uri_find_pparam(&u, "1"), "");

        TT_UT_SUCCESS(tt_http_uri_add_pparam(&u, "q1"), "");
        TT_UT_NOT_NULL(tt_http_uri_find_pparam(&u, "q1"), "");

        TT_UT_SUCCESS(tt_http_uri_add_pparam_nv(&u, "q2", "v2"), "");
        TT_UT_NOT_NULL(tt_http_uri_find_pparam(&u, "q2"), "");
    }
    {
        TT_UT_FALSE(tt_http_uri_remove_pparam(&u, ""), "");
        TT_UT_FALSE(tt_http_uri_remove_pparam(&u, "1"), "");
        TT_UT_FALSE(tt_http_uri_remove_pparam_nv(&u, "1", "2"), "");
    }

    TT_UT_SUCCESS(tt_http_uri_parse(&u, ""), "");
    TT_UT_EQUAL(tt_http_uri_get_scheme(&u), TT_HTTP_SCHEME_UNDEFINED, "");
    TT_UT_STREQ(tt_http_uri_get_userinfo(&u), "", "");
    TT_UT_STREQ(tt_http_uri_get_host(&u), "", "");
    TT_UT_EQUAL(tt_http_uri_get_port(&u), 0, "");
    TT_UT_STREQ(tt_http_uri_get_authority(&u), "", "");
    TT_UT_STREQ(tt_http_uri_get_fragment(&u), "", "");

    // scheme
    TT_UT_FAIL(tt_http_uri_parse(&u, "123:"), "");
    TT_UT_FAIL(tt_http_uri_parse(&u, "htt:"), "");
    TT_UT_FAIL(tt_http_uri_parse(&u, "httpx:"), "");

    TT_UT_SUCCESS(tt_http_uri_parse(&u, "http:"), "");
    TT_UT_EQUAL(tt_http_uri_get_scheme(&u), TT_HTTP_SCHEME_HTTP, "");

    TT_UT_SUCCESS(tt_http_uri_parse(&u, "https:"), "");
    TT_UT_EQUAL(tt_http_uri_get_scheme(&u), TT_HTTP_SCHEME_HTTPS, "");

    // path
    TT_UT_SUCCESS(tt_http_uri_parse(&u, "http://u:p@host/"), "");
    TT_UT_STREQ(tt_http_uri_get_userinfo(&u), "u:p", "");
    TT_UT_STREQ(tt_http_uri_get_host(&u), "host", "");
    fp = tt_http_uri_get_path(&u);
    TT_UT_STREQ(tt_fpath_cstr(fp), "/", "");
    {
        tt_http_uri_param_iter_t pi;
        tt_http_uri_pparam_iter(&u, &pi);
        TT_UT_EQUAL(tt_http_uri_param_next(&pi), NULL, "");
    }
    {
        TT_UT_NULL(tt_http_uri_find_pparam(&u, ""), "");
        TT_UT_NULL(tt_http_uri_find_pparam(&u, "1"), "");
    }
    {
        TT_UT_FALSE(tt_http_uri_remove_pparam(&u, ""), "");
        TT_UT_FALSE(tt_http_uri_remove_pparam(&u, "1"), "");
        TT_UT_FALSE(tt_http_uri_remove_pparam_nv(&u, "1", "2"), "");
    }

    TT_UT_SUCCESS(tt_http_uri_parse(&u, "http://u:p@host/;"), "");
    TT_UT_STREQ(tt_fpath_cstr(fp), "/", "");
    {
        tt_http_uri_param_iter_t pi;
        tt_http_uri_pparam_iter(&u, &pi);
        TT_UT_EQUAL(tt_http_uri_param_next(&pi), NULL, "");
    }
    {
        TT_UT_FALSE(tt_http_uri_remove_pparam(&u, ""), "");
        TT_UT_FALSE(tt_http_uri_remove_pparam(&u, "1"), "");
        TT_UT_FALSE(tt_http_uri_remove_pparam_nv(&u, "1", "2"), "");
    }

    TT_UT_SUCCESS(tt_http_uri_parse(&u, "http://u:p@host;/"), "");
    TT_UT_STREQ(tt_fpath_cstr(fp), "/", "");
    {
        tt_http_uri_param_iter_t pi;
        tt_http_uri_pparam_iter(&u, &pi);
        TT_UT_EQUAL(tt_http_uri_param_next(&pi), NULL, "");
    }

    TT_UT_SUCCESS(tt_http_uri_parse(&u, "http://u:p@host;/a/b/c"), "");
    TT_UT_STREQ(tt_fpath_cstr(fp), "/a/b/c", "");
    {
        tt_http_uri_param_iter_t pi;
        tt_http_uri_pparam_iter(&u, &pi);
        TT_UT_EQUAL(tt_http_uri_param_next(&pi), NULL, "");
    }

    TT_UT_SUCCESS(tt_http_uri_parse(&u, "http://u:p@host;/a/b/c;k1=v1&k2=v2"),
                  "");
    TT_UT_STREQ(tt_fpath_cstr(fp), "/a/b/c", "");
    {
        tt_http_uri_param_iter_t pi;
        tt_kv_t *kvb;
        tt_http_uri_pparam_iter(&u, &pi);

        kvb = tt_http_uri_param_next(&pi);
        TT_UT_EQUAL(tt_blobex_strcmp(&kvb->key, "k1"), 0, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&kvb->val, "v1"), 0, "");

        kvb = tt_http_uri_param_next(&pi);
        TT_UT_EQUAL(tt_blobex_strcmp(&kvb->key, "k2"), 0, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&kvb->val, "v2"), 0, "");

        TT_UT_EQUAL(tt_http_uri_param_next(&pi), NULL, "");
    }
    {
        tt_kv_t *kv;

        TT_UT_NULL(tt_http_uri_find_pparam(&u, ""), "");
        TT_UT_NULL(tt_http_uri_find_pparam(&u, "k3"), "");

        kv = tt_http_uri_find_pparam(&u, "k1");
        TT_UT_NOT_NULL(kv, "");
        TT_UT_EXP(tt_blobex_strcmp(&kv->val, "v1") == 0, "");

        kv = tt_http_uri_find_pparam(&u, "k2");
        TT_UT_NOT_NULL(kv, "");
        TT_UT_EXP(tt_blobex_strcmp(&kv->val, "v2") == 0, "");

        TT_UT_SUCCESS(tt_http_uri_add_pparam(&u, "q1"), "");
        TT_UT_NOT_NULL((kv = tt_http_uri_find_pparam(&u, "q1")), "");
        TT_UT_EXP(tt_blobex_len(&kv->val) == 0, "");

        TT_UT_SUCCESS(tt_http_uri_add_pparam_nv(&u, "q2", "v2"), "");
        TT_UT_NOT_NULL((kv = tt_http_uri_find_pparam(&u, "q2")), "");
        TT_UT_EXP(tt_blobex_strcmp(&kv->val, "v2") == 0, "");
    }
    {
        TT_UT_FALSE(tt_http_uri_remove_pparam(&u, ""), "");
        TT_UT_FALSE(tt_http_uri_remove_pparam(&u, "1"), "");
        TT_UT_FALSE(tt_http_uri_remove_pparam_nv(&u, "1", "2"), "");

        TT_UT_TRUE(tt_http_uri_remove_pparam(&u, "k1"), "");
        TT_UT_NULL(tt_http_uri_find_pparam(&u, "k1"), "");
        TT_UT_NOT_NULL(tt_http_uri_find_pparam(&u, "k2"), "");

        TT_UT_TRUE(tt_http_uri_remove_pparam(&u, "k2"), "");
        TT_UT_NULL(tt_http_uri_find_pparam(&u, "k1"), "");
        TT_UT_NULL(tt_http_uri_find_pparam(&u, "k2"), "");
    }

    TT_UT_SUCCESS(tt_http_uri_parse(&u, "/a/b/c;k1"), "");
    TT_UT_EQUAL(tt_http_uri_get_scheme(&u), TT_HTTP_SCHEME_UNDEFINED, "");
    TT_UT_STREQ(tt_fpath_cstr(fp), "/a/b/c", "");
    {
        tt_http_uri_param_iter_t pi;
        tt_kv_t *kvb;
        tt_http_uri_pparam_iter(&u, &pi);

        kvb = tt_http_uri_param_next(&pi);
        TT_UT_EQUAL(tt_blobex_strcmp(&kvb->key, "k1"), 0, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&kvb->val, ""), 0, "");

        TT_UT_EQUAL(tt_http_uri_param_next(&pi), NULL, "");
    }
    {
        tt_kv_t *kv;

        TT_UT_NULL(tt_http_uri_find_pparam(&u, ""), "");
        TT_UT_NULL(tt_http_uri_find_pparam(&u, "k3"), "");

        kv = tt_http_uri_find_pparam(&u, "k1");
        TT_UT_NOT_NULL(kv, "");
        TT_UT_EXP(tt_blobex_len(&kv->val) == 0, "");
    }

    TT_UT_SUCCESS(tt_http_uri_parse(&u, "a/b/c/;"), "");
    TT_UT_EQUAL(tt_http_uri_get_scheme(&u), TT_HTTP_SCHEME_UNDEFINED, "");
    TT_UT_STREQ(tt_fpath_cstr(fp), "a/b/c/", "");
    {
        tt_http_uri_param_iter_t pi;
        tt_http_uri_pparam_iter(&u, &pi);
        TT_UT_EQUAL(tt_http_uri_param_next(&pi), NULL, "");
    }

    // empty path
    TT_UT_SUCCESS(tt_http_uri_parse(&u, ";"), "");
    {
        tt_http_uri_param_iter_t pi;
        tt_http_uri_pparam_iter(&u, &pi);
        TT_UT_EQUAL(tt_http_uri_param_next(&pi), NULL, "");
    }

    // 1 eq param
    TT_UT_SUCCESS(tt_http_uri_parse(&u, ";k1=v1&"), "");
    {
        tt_http_uri_param_iter_t pi;
        tt_kv_t *kvb;
        tt_http_uri_pparam_iter(&u, &pi);

        kvb = tt_http_uri_param_next(&pi);
        TT_UT_EQUAL(tt_blobex_strcmp(&kvb->key, "k1"), 0, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&kvb->val, "v1"), 0, "");

        TT_UT_EQUAL(tt_http_uri_param_next(&pi), NULL, "");
    }
    {
        tt_kv_t *kv;

        TT_UT_NULL(tt_http_uri_find_pparam(&u, ""), "");
        TT_UT_NULL(tt_http_uri_find_pparam(&u, "k3"), "");

        kv = tt_http_uri_find_pparam(&u, "k1");
        TT_UT_NOT_NULL(kv, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&kv->val, "v1"), 0, "");
    }

    tt_http_uri_destroy(&u);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_http_uri_parse_query)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_http_uri_t u;
    tt_kv_t *kv;

    TT_TEST_CASE_ENTER()
    // test start

    tt_http_uri_init(&u);

    TT_UT_SUCCESS(tt_http_uri_parse(&u, "?q1=v1&q2&"), "");
    kv = tt_http_uri_find_qparam(&u, "q1");
    TT_UT_NOT_NULL(kv, "");
    TT_UT_EXP(tt_blobex_strcmp(&kv->val, "v1") == 0, "");
    kv = tt_http_uri_find_qparam(&u, "q2");
    TT_UT_NOT_NULL(kv, "");
    TT_UT_EXP(tt_blobex_strcmp(&kv->val, "") == 0, "");

    TT_UT_FALSE(tt_http_uri_remove_qparam_nv(&u, "q2", "v2"), "");
    kv = tt_http_uri_find_qparam(&u, "q2");
    TT_UT_NOT_NULL(kv, "");
    TT_UT_EXP(tt_blobex_strcmp(&kv->val, "") == 0, "");

    TT_UT_TRUE(tt_http_uri_remove_qparam_nv(&u, "q1", "v1"), "");
    kv = tt_http_uri_find_qparam(&u, "q1");
    TT_UT_NULL(kv, "");

    TT_UT_TRUE(tt_http_uri_remove_qparam(&u, "q2"), "");
    kv = tt_http_uri_find_qparam(&u, "q2");
    TT_UT_NULL(kv, "");

    tt_http_uri_destroy(&u);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_http_uri_encode)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_http_uri_t u;
    const tt_char_t *s;
    tt_kv_t *kv;
    tt_fpath_t *fp;

    TT_TEST_CASE_ENTER()
    // test start

    tt_http_uri_init(&u);

    // path
    s = "/a/b/c";
    TT_UT_SUCCESS(tt_http_uri_parse_n(&u, (tt_char_t *)s, tt_strlen(s)), "");
    TT_UT_STREQ(tt_http_uri_render(&u), "/a/b/c", "");

    s = "a/b/c/";
    TT_UT_SUCCESS(tt_http_uri_parse_n(&u, (tt_char_t *)s, tt_strlen(s)), "");
    TT_UT_STREQ(tt_http_uri_render(&u), "a/b/c/", "");

    // path with 0 params
    s = "/a/b/c;?";
    TT_UT_SUCCESS(tt_http_uri_parse_n(&u, (tt_char_t *)s, tt_strlen(s)), "");
    TT_UT_STREQ(tt_http_uri_render(&u), "/a/b/c;", "");

    // path with 1 params
    s = "a/b/c/;a?";
    TT_UT_SUCCESS(tt_http_uri_parse_n(&u, (tt_char_t *)s, tt_strlen(s)), "");
    TT_UT_STREQ(tt_http_uri_render(&u), "a/b/c/;a", "");

    s = "a/b/c/;a=1?";
    TT_UT_SUCCESS(tt_http_uri_parse_n(&u, (tt_char_t *)s, tt_strlen(s)), "");
    TT_UT_STREQ(tt_http_uri_render(&u), "a/b/c/;a=1", "");

    // path with 2 params
    s = "/a/b/c;a=1&b=2?";
    TT_UT_SUCCESS(tt_http_uri_parse_n(&u, (tt_char_t *)s, tt_strlen(s)), "");
    TT_UT_STREQ(tt_http_uri_render(&u), "/a/b/c;a=1&b=2", "");
    kv = tt_http_uri_find_pparam(&u, "b");
    TT_UT_NOT_NULL(kv, "");
    TT_UT_EQUAL(tt_blobex_strcmp(&kv->val, "2"), 0, "");

    s = "/a/b/c/;a=1&b=2&";
    TT_UT_SUCCESS(tt_http_uri_parse_n(&u, (tt_char_t *)s, tt_strlen(s)), "");
    TT_UT_STREQ(tt_http_uri_render(&u), "/a/b/c/;a=1&b=2&", "");
    kv = tt_http_uri_find_pparam(&u, "b");
    TT_UT_NOT_NULL(kv, "");
    TT_UT_EQUAL(tt_blobex_strcmp(&kv->val, "2"), 0, "");
    {
        TT_UT_SUCCESS(tt_http_uri_add_pparam(&u, "c"), "");
        TT_UT_STREQ(tt_http_uri_render(&u), "/a/b/c/;a=1&b=2&c", "");
    }

    // change path
    fp = tt_http_uri_get_path(&u);
    tt_fpath_to_parent(fp);
    tt_fpath_set_filename(fp, "f=g .exe");
    tt_http_uri_update_path(&u);
    TT_UT_STREQ(tt_http_uri_render(&u), "/a/b/f%3dg%20.exe;a=1&b=2&c", "");

    // clear path
    tt_fpath_clear(fp);
    tt_http_uri_update_path(&u);
    TT_UT_STREQ(tt_http_uri_render(&u), ";a=1&b=2&c", "");

    // change param
    TT_UT_SUCCESS(tt_http_uri_add_pparam(&u, "/;=&"), "");
    TT_UT_STREQ(tt_http_uri_render(&u), ";a=1&b=2&c&%2f%3b%3d%26", "");

    // remove param
    TT_UT_FALSE(tt_http_uri_remove_pparam(&u, "d"), "");
    TT_UT_TRUE(tt_http_uri_remove_pparam(&u, "b"), "");
    TT_UT_STREQ(tt_http_uri_render(&u), ";a=1&c&%2f%3b%3d%26", "");
    TT_UT_TRUE(tt_http_uri_remove_pparam(&u, "/;=&"), "");
    TT_UT_STREQ(tt_http_uri_render(&u), ";a=1&c", "");
    TT_UT_FALSE(tt_http_uri_remove_pparam_nv(&u, "a", ""), "");
    TT_UT_TRUE(tt_http_uri_remove_pparam_nv(&u, "a", "1"), "");
    TT_UT_TRUE(tt_http_uri_remove_pparam(&u, "c"), "");
    TT_UT_STREQ(tt_http_uri_render(&u), "", "");

    tt_fpath_parse(fp, "a/b.c");
    tt_http_uri_update_path(&u);
    TT_UT_STREQ(tt_http_uri_render(&u), "a/b.c", "");
    tt_http_uri_add_pparam(&u, "111");
    TT_UT_STREQ(tt_http_uri_render(&u), "a/b.c;111", "");
    tt_http_uri_remove_pparam(&u, "111");
    TT_UT_STREQ(tt_http_uri_render(&u), "a/b.c", "");

    tt_http_uri_destroy(&u);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_http_uri_encode_query)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_http_uri_t u;
    const tt_char_t *s;
    tt_kv_t *kv;
    tt_fpath_t *fp;

    TT_TEST_CASE_ENTER()
    // test start

    tt_http_uri_init(&u);

    TT_UT_SUCCESS(tt_http_uri_add_qparam(&u, "1 2"), "");
    TT_UT_STREQ(tt_http_uri_render(&u), "?1%202", "");

    TT_UT_SUCCESS(tt_http_uri_add_qparam_nv(&u, "?", "?"), "");
    TT_UT_STREQ(tt_http_uri_render(&u), "?1%202&%3f=%3f", "");

    TT_UT_NOT_NULL(tt_http_uri_find_qparam(&u, "1 2"), "");
    TT_UT_NOT_NULL(tt_http_uri_find_qparam(&u, "?"), "");
    TT_UT_NULL(tt_http_uri_find_qparam(&u, "1%202"), "");

    TT_UT_FALSE(tt_http_uri_remove_qparam(&u, "xx"), "");
    TT_UT_TRUE(tt_http_uri_remove_qparam(&u, "?"), "");
    TT_UT_STREQ(tt_http_uri_render(&u), "?1%202", "");

    TT_UT_TRUE(tt_http_uri_remove_qparam(&u, "1 2"), "");
    TT_UT_STREQ(tt_http_uri_render(&u), "", "");

    tt_http_uri_clear(&u);

    // WITH A PATH
    fp = tt_http_uri_get_path(&u);
    tt_fpath_parse(fp, "a/b");
    tt_http_uri_add_pparam(&u, "ppm");
    TT_UT_STREQ(tt_http_uri_render(&u), "a/b;ppm", "");

    TT_UT_SUCCESS(tt_http_uri_add_qparam(&u, "1 2"), "");
    TT_UT_STREQ(tt_http_uri_render(&u), "a/b;ppm?1%202", "");

    TT_UT_SUCCESS(tt_http_uri_add_qparam_nv(&u, "?", "?"), "");
    TT_UT_STREQ(tt_http_uri_render(&u), "a/b;ppm?1%202&%3f=%3f", "");

    TT_UT_NOT_NULL(tt_http_uri_find_qparam(&u, "1 2"), "");
    TT_UT_NOT_NULL(tt_http_uri_find_qparam(&u, "?"), "");
    TT_UT_NULL(tt_http_uri_find_qparam(&u, "a/b;ppm1%202"), "");

    TT_UT_FALSE(tt_http_uri_remove_qparam(&u, "xx"), "");
    TT_UT_TRUE(tt_http_uri_remove_qparam(&u, "?"), "");
    TT_UT_STREQ(tt_http_uri_render(&u), "a/b;ppm?1%202", "");

    TT_UT_TRUE(tt_http_uri_remove_qparam(&u, "1 2"), "");
    TT_UT_STREQ(tt_http_uri_render(&u), "a/b;ppm", "");

    tt_http_uri_destroy(&u);

    // test end
    TT_TEST_CASE_LEAVE()
}
