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

#include <math.h>

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
TT_TEST_ROUTINE_DECLARE(case_xpath_node)
TT_TEST_ROUTINE_DECLARE(case_xpath_attr)
TT_TEST_ROUTINE_DECLARE(case_xpath_eval)
TT_TEST_ROUTINE_DECLARE(case_xpath_xpvar)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(xml_xpath_case)

TT_TEST_CASE("case_xpath_node",
             "xml: xpath query node",
             case_xpath_node,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("case_xpath_attr",
                 "xml: xpath query attr",
                 case_xpath_attr,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_xpath_eval",
                 "xml: xpath evaluate",
                 case_xpath_eval,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_xpath_xpvar",
                 "xml: xpath evaluate with variables",
                 case_xpath_xpvar,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(xml_xpath_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(XML_UT_XPATH, 0, xml_xpath_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(case_xpath_eval)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    static const tt_char_t __ut_xpath[] =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "\n"
        "<bookstore>\n"
        "\n"
        "<book category=\"cooking\">\n"
        "  <title lang=\"en\">Everyday Italian</title>\n"
        "  <author>Giada De Laurentiis</author>\n"
        "  <year>2005</year>\n"
        "  <price>30.00</price>\n"
        "</book>\n"
        "\n"
        "<book category=\"children\">\n"
        "  <title lang=\"en\">Harry Potter</title>\n"
        "  <author>J K. Rowling</author>\n"
        "  <year>2005</year>\n"
        "  <price>29.99</price>\n"
        "</book>\n"
        "\n"
        "<book category=\"web\">\n"
        "  <title lang=\"en\">XQuery Kick Start</title>\n"
        "  <author>James McGovern</author>\n"
        "  <author>Per Bothner</author>\n"
        "  <author>Kurt Cagle</author>\n"
        "  <author>James Linn</author>\n"
        "  <author>Vaidyanathan Nagarajan</author>\n"
        "  <year>2003</year>\n"
        "  <price>49.99</price>\n"
        "</book>\n"
        "\n"
        "<book category=\"web\">\n"
        "  <title lang=\"en\">Learning XML</title>\n"
        "  <author>Erik T. Ray</author>\n"
        "  <year>2003</year>\n"
        "  <price>39.95</price>\n"
        "</book>\n"
        "\n"
        "</bookstore>\n";

TT_TEST_ROUTINE_DEFINE(case_xpath_node)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_xdoc_t xd;
    tt_result_t ret;
    tt_xnode_t root, xn;
    tt_xnode_t xa;
    tt_xpath_t xp;
    tt_xpnodes_t xpns;
    tt_xpnodes_iter_t i;
    tt_u32_t n;

    TT_TEST_CASE_ENTER()
    // test start

    tt_xdoc_create(&xd);

    ret = tt_xdoc_parse(&xd, (tt_u8_t *)__ut_xpath, sizeof(__ut_xpath), NULL);
    TT_UT_SUCCESS(ret, "");

    root = tt_xdoc_root(&xd);
    TT_UT_NOT_NULL(root, "");

    // invalid xpath
    ret = tt_xpath_create(&xp, "//nodes[#true()]", NULL);
    TT_UT_FAIL(ret, "");

    ret = tt_xpath_create(&xp, "/bookstore/book/title", NULL);
    TT_UT_SUCCESS(ret, "");

    tt_xpnodes_init(&xpns);
    tt_xnode_selectxp_all(root, &xp, &xpns);
    TT_UT_EQUAL(tt_xpnodes_count(&xpns), 4, "");

    n = 0;
    tt_xpnodes_sort(&xpns, TT_FALSE);
    tt_xpnodes_iter(&xpns, &i);
    while (TT_OK(tt_xpnodes_iter_next(&i, &xn, &xa))) {
        TT_UT_NOT_NULL(xn, "");
        TT_UT_NULL(xa, "");

        if (n == 0) {
            TT_UT_EQUAL(tt_strcmp(tt_xnode_get_value(xn, ""),
                                  "Everyday Italian"),
                        0,
                        "");
        } else if (n == 3) {
            TT_UT_EQUAL(tt_strcmp(tt_xnode_get_value(xn, ""), "Learning XML"),
                        0,
                        "");
        }
        ++n;
    }
    TT_UT_EQUAL(n, 4, "");

    // reverse
    n = 0;
    tt_xpnodes_sort(&xpns, TT_TRUE);
    tt_xpnodes_iter(&xpns, &i);
    while (TT_OK(tt_xpnodes_iter_next(&i, &xn, &xa))) {
        TT_UT_NOT_NULL(xn, "");
        TT_UT_NULL(xa, "");

        if (n == 3) {
            TT_UT_EQUAL(tt_strcmp(tt_xnode_get_value(xn, ""),
                                  "Everyday Italian"),
                        0,
                        "");
        } else if (n == 0) {
            TT_UT_EQUAL(tt_strcmp(tt_xnode_get_value(xn, ""), "Learning XML"),
                        0,
                        "");
        }
        ++n;
    }
    TT_UT_EQUAL(n, 4, "");

    // single
    tt_xnode_selectxp(root, &xp, &xn, &xa);
    TT_UT_NOT_NULL(xn, "");
    TT_UT_NULL(xa, "");

    tt_xpnodes_destroy(&xpns);
    tt_xpath_destroy(&xp);

    // none match
    ret = tt_xpath_create(&xp, "/bookstore/book123/title", NULL);
    TT_UT_SUCCESS(ret, "");

    tt_xpnodes_init(&xpns);
    tt_xnode_selectxp_all(root, &xp, &xpns);
    TT_UT_EQUAL(tt_xpnodes_count(&xpns), 0, "");

    // single
    tt_xnode_selectxp(root, &xp, &xn, &xa);
    TT_UT_NULL(xn, "");
    TT_UT_NULL(xa, "");

    // select path directly
    tt_xpnodes_destroy(&xpns);
    tt_xpnodes_init(&xpns);
    tt_xnode_select_all(root, "/bookstore/book/title", NULL, &xpns);
    TT_UT_EQUAL(tt_xpnodes_count(&xpns), 4, "");

    n = 0;
    tt_xpnodes_sort(&xpns, TT_FALSE);
    tt_xpnodes_iter(&xpns, &i);
    while (TT_OK(tt_xpnodes_iter_next(&i, &xn, &xa))) {
        TT_UT_NOT_NULL(xn, "");
        TT_UT_NULL(xa, "");

        if (n == 0) {
            TT_UT_EQUAL(tt_strcmp(tt_xnode_get_value(xn, ""),
                                  "Everyday Italian"),
                        0,
                        "");
        } else if (n == 3) {
            TT_UT_EQUAL(tt_strcmp(tt_xnode_get_value(xn, ""), "Learning XML"),
                        0,
                        "");
        }
        ++n;
    }
    TT_UT_EQUAL(n, 4, "");

    tt_xpnodes_destroy(&xpns);
    tt_xpath_destroy(&xp);

    tt_xdoc_destroy(&xd);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_xpath_attr)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_xdoc_t xd;
    tt_result_t ret;
    tt_xnode_t xn;
    tt_xnode_t xa;
    tt_xpath_t xp;
    tt_xpnodes_t xpns;
    tt_xpnodes_iter_t i;
    tt_u32_t n;

    TT_TEST_CASE_ENTER()
    // test start

    tt_xdoc_create(&xd);

    ret = tt_xdoc_parse(&xd, (tt_u8_t *)__ut_xpath, sizeof(__ut_xpath), NULL);
    TT_UT_SUCCESS(ret, "");

    ret = tt_xpath_create(&xp, "/bookstore/book/@category", NULL);
    TT_UT_SUCCESS(ret, "");

    tt_xpnodes_init(&xpns);
    tt_xdoc_selectxp_all(&xd, &xp, &xpns);
    TT_UT_EQUAL(tt_xpnodes_count(&xpns), 4, "");

    n = 0;
    tt_xpnodes_sort(&xpns, TT_FALSE);
    tt_xpnodes_iter(&xpns, &i);
    while (TT_OK(tt_xpnodes_iter_next(&i, &xn, &xa))) {
        TT_UT_NULL(xn, "");
        TT_UT_NOT_NULL(xa, "");

        if (n == 0) {
            TT_UT_EQUAL(tt_strcmp(tt_xattr_get_value(xa, ""), "cooking"),
                        0,
                        "");
        } else if (n == 3) {
            TT_UT_EQUAL(tt_strcmp(tt_xattr_get_value(xa, ""), "web"), 0, "");
        }
        ++n;
    }
    TT_UT_EQUAL(n, 4, "");

    // reverse
    n = 0;
    tt_xpnodes_sort(&xpns, TT_TRUE);
    tt_xpnodes_iter(&xpns, &i);
    while (TT_OK(tt_xpnodes_iter_next(&i, &xn, &xa))) {
        TT_UT_NULL(xn, "");
        TT_UT_NOT_NULL(xa, "");

        if (n == 3) {
            TT_UT_EQUAL(tt_strcmp(tt_xattr_get_value(xa, ""), "cooking"),
                        0,
                        "");
        } else if (n == 0) {
            TT_UT_EQUAL(tt_strcmp(tt_xattr_get_value(xa, ""), "web"), 0, "");
        }
        ++n;
    }
    TT_UT_EQUAL(n, 4, "");

    // single
    tt_xdoc_selectxp(&xd, &xp, &xn, &xa);
    TT_UT_NULL(xn, "");
    TT_UT_NOT_NULL(xa, "");

    tt_xpnodes_destroy(&xpns);
    tt_xpath_destroy(&xp);

    // none match
    ret = tt_xpath_create(&xp, "/bookstore/book/@not_exist", NULL);
    TT_UT_SUCCESS(ret, "");

    tt_xpnodes_init(&xpns);
    tt_xdoc_selectxp_all(&xd, &xp, &xpns);
    TT_UT_EQUAL(tt_xpnodes_count(&xpns), 0, "");

    // single
    tt_xdoc_selectxp(&xd, &xp, &xn, &xa);
    TT_UT_NULL(xn, "");
    TT_UT_NULL(xa, "");

    // select path directly
    tt_xpnodes_destroy(&xpns);
    tt_xpnodes_init(&xpns);
    tt_xdoc_select_all(&xd, "/bookstore/book/@category", NULL, &xpns);
    TT_UT_EQUAL(tt_xpnodes_count(&xpns), 4, "");

    n = 0;
    tt_xpnodes_sort(&xpns, TT_FALSE);
    tt_xpnodes_iter(&xpns, &i);
    while (TT_OK(tt_xpnodes_iter_next(&i, &xn, &xa))) {
        TT_UT_NULL(xn, "");
        TT_UT_NOT_NULL(xa, "");

        if (n == 0) {
            TT_UT_EQUAL(tt_strcmp(tt_xattr_get_value(xa, ""), "cooking"),
                        0,
                        "");
        } else if (n == 3) {
            TT_UT_EQUAL(tt_strcmp(tt_xattr_get_value(xa, ""), "web"), 0, "");
        }
        ++n;
    }
    TT_UT_EQUAL(n, 4, "");

    tt_xpnodes_destroy(&xpns);
    tt_xpath_destroy(&xp);

    tt_xdoc_destroy(&xd);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_xpath_eval)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_xdoc_t xd;
    tt_result_t ret;
    tt_xnode_t xn;
    tt_xnode_t xa;
    tt_xpath_t xp;
    tt_char_t buf[100];

    TT_TEST_CASE_ENTER()
    // test start

    tt_xdoc_create(&xd);

    ret = tt_xdoc_parse(&xd, (tt_u8_t *)__ut_xpath, sizeof(__ut_xpath), NULL);
    TT_UT_SUCCESS(ret, "");

    tt_xdoc_select(&xd, "/bookstore/book", NULL, &xn, &xa);
    TT_UT_NOT_NULL(xn, "");
    TT_UT_NULL(xa, "");

    ret = tt_xpath_create(&xp, "string-length(@category)", NULL);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_xnode_eval_bool(xn, &xp), TT_TRUE, "");
    TT_UT_EQUAL(tt_xnode_eval_number(xn, &xp), sizeof("cooking") - 1, "");
    tt_memset(buf, 0, sizeof(buf));
#if !TT_ENV_OS_IS_ANDROID
    tt_xnode_eval_cstr(xn, &xp, buf, sizeof(buf));
    TT_UT_EQUAL(tt_strcmp(buf, "7"), 0, "");
#endif

    ret = tt_xpath_create(&xp, "string-length(@category)", NULL);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_xnode_eval_bool(NULL, &xp), TT_FALSE, "");
    TT_UT_EQUAL(tt_xnode_eval_number(NULL, &xp), 0, "");
    tt_memset(buf, 0, sizeof(buf));
    tt_xnode_eval_cstr(NULL, &xp, buf, sizeof(buf));
    TT_UT_EQUAL(tt_strcmp(buf, "0"), 0, "");

    tt_xpath_destroy(&xp);

    // xdoc eval
    ret = tt_xpath_create(&xp, "sum(//book/price)", NULL);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_xdoc_eval_bool(&xd, &xp), TT_TRUE, "");
    TT_UT_EQUAL(tt_xdoc_eval_number(&xd, &xp), 149.93, "");
    tt_memset(buf, 0, sizeof(buf));
    tt_xdoc_eval_cstr(&xd, &xp, buf, sizeof(buf));
    TT_UT_EQUAL(tt_strcmp(buf, "149.93"), 0, "");
    tt_xpath_destroy(&xp);

    ret = tt_xpath_create(&xp, "sum(//book/price123)", NULL);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_xdoc_eval_bool(&xd, &xp), TT_FALSE, "");
    TT_UT_EQUAL(tt_xdoc_eval_number(&xd, &xp), 0, "");
    tt_memset(buf, 0, sizeof(buf));
    tt_xdoc_eval_cstr(&xd, &xp, buf, sizeof(buf));
    TT_UT_EQUAL(tt_strcmp(buf, "0"), 0, "");
    tt_xpath_destroy(&xp);

    tt_xdoc_destroy(&xd);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_xpath_xpvar)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_xdoc_t xd;
    tt_result_t ret;
    tt_xnode_t xn;
    tt_xnode_t xa;
    tt_xpath_t xp;
    tt_char_t buf[100];
    tt_xpvars_t xpvs;

    TT_TEST_CASE_ENTER()
    // test start

    tt_xdoc_create(&xd);
    tt_xpvars_init(&xpvs);

    ret = tt_xdoc_parse(&xd, (tt_u8_t *)__ut_xpath, sizeof(__ut_xpath), NULL);
    TT_UT_SUCCESS(ret, "");

    // xpvar: cstr
    ret = tt_xpvars_set_cstr(&xpvs, "t", "Learning XML");
    TT_UT_SUCCESS(ret, "");
    ret = tt_xpvars_set_bool(&xpvs, "t", TT_FALSE);
    TT_UT_FAIL(ret, "");
    ret = tt_xpvars_set_number(&xpvs, "t", 123);
    TT_UT_FAIL(ret, "");

    tt_xdoc_select(&xd,
                   "/bookstore/book[title=string($t)]/author",
                   &xpvs,
                   &xn,
                   &xa);
    TT_UT_NOT_NULL(xn, "");
    TT_UT_NULL(xa, "");
    TT_UT_EQUAL(tt_strcmp(tt_xnode_get_value(xn, ""), "Erik T. Ray"), 0, "");

    ret = tt_xpvars_set_cstr(&xpvs, "t", "Harry Potter");
    TT_UT_SUCCESS(ret, "");
    tt_xnode_select(tt_xdoc_root(&xd),
                    "/bookstore/book[title=string($t)]/author",
                    &xpvs,
                    &xn,
                    &xa);
    TT_UT_NOT_NULL(xn, "");
    TT_UT_NULL(xa, "");
    TT_UT_EQUAL(tt_strcmp(tt_xnode_get_value(xn, ""), "J K. Rowling"), 0, "");

    // xpvar: number
    ret = tt_xpvars_set_number(&xpvs, "price", 29.99);
    TT_UT_SUCCESS(ret, "");
    ret = tt_xpvars_set_cstr(&xpvs, "price", "Learning XML");
    TT_UT_FAIL(ret, "");
    ret = tt_xpvars_set_bool(&xpvs, "price", TT_FALSE);
    TT_UT_FAIL(ret, "");

    tt_xdoc_select(&xd,
                   "/bookstore/book[price=number($price)]/author",
                   &xpvs,
                   &xn,
                   &xa);
    TT_UT_NOT_NULL(xn, "");
    TT_UT_NULL(xa, "");
    TT_UT_EQUAL(tt_strcmp(tt_xnode_get_value(xn, ""), "J K. Rowling"), 0, "");

    ret = tt_xpath_create(&xp,
                          "/bookstore/book[price=number($price)]/author",
                          &xpvs);
    TT_UT_SUCCESS(ret, "");
    TT_UT_NOT_NULL(xn, "");
    TT_UT_NULL(xa, "");
    TT_UT_EQUAL(tt_strcmp(tt_xnode_get_value(xn, ""), "J K. Rowling"), 0, "");

    ret = tt_xpvars_set_number(&xpvs, "price", 1.1);
    TT_UT_SUCCESS(ret, "");

    tt_xdoc_select(&xd,
                   "/bookstore/book[price=number($price)]/author",
                   &xpvs,
                   &xn,
                   &xa);
    TT_UT_NULL(xn, "");
    TT_UT_NULL(xa, "");

    tt_xnode_selectxp(tt_xdoc_root(&xd), &xp, &xn, &xa);
    TT_UT_NULL(xn, "");
    TT_UT_NULL(xa, "");

    ret = tt_xpath_create(&xp, "string-length($t)", &xpvs);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_xdoc_eval_bool(&xd, &xp), TT_TRUE, "");
    tt_memset(buf, 0, sizeof(buf));
    tt_xdoc_eval_cstr(&xd, &xp, buf, sizeof(buf));
    TT_UT_EQUAL(tt_strcmp(buf, "12"), 0, "");
    TT_UT_EQUAL(tt_xnode_eval_number(xn, &xp), sizeof("J K. Rowling") - 1, "");
    tt_xpath_destroy(&xp);

    // test end
    TT_TEST_CASE_LEAVE()
}
