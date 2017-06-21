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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_xpath_node)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_xpath_attr)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(xml_xpath_case)

TT_TEST_CASE("tt_unit_test_xpath_node",
             "xml: xpath query node",
             tt_unit_test_xpath_node,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_xpath_attr",
                 "xml: xpath query attr",
                 tt_unit_test_xpath_attr,
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
    TT_TEST_ROUTINE_DEFINE(tt_unit_test_xpath_node)
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

TT_TEST_ROUTINE_DEFINE(tt_unit_test_xpath_node)
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
    tt_xnode_select_all(root, &xp, &xpns);
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
    tt_xnode_select(root, &xp, &xn, &xa);
    TT_UT_NOT_NULL(xn, "");
    TT_UT_NULL(xa, "");

    tt_xpnodes_destroy(&xpns);
    tt_xpath_destroy(&xp);

    // none match
    ret = tt_xpath_create(&xp, "/bookstore/book123/title", NULL);
    TT_UT_SUCCESS(ret, "");

    tt_xpnodes_init(&xpns);
    tt_xnode_select_all(root, &xp, &xpns);
    TT_UT_EQUAL(tt_xpnodes_count(&xpns), 0, "");

    // single
    tt_xnode_select(root, &xp, &xn, &xa);
    TT_UT_NULL(xn, "");
    TT_UT_NULL(xa, "");

    tt_xpnodes_destroy(&xpns);
    tt_xpath_destroy(&xp);

    tt_xdoc_destroy(&xd);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_xpath_attr)
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

    ret = tt_xpath_create(&xp, "/bookstore/book/@category", NULL);
    TT_UT_SUCCESS(ret, "");

    tt_xpnodes_init(&xpns);
    tt_xnode_select_all(root, &xp, &xpns);
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
    tt_xnode_select(root, &xp, &xn, &xa);
    TT_UT_NULL(xn, "");
    TT_UT_NOT_NULL(xa, "");

    tt_xpnodes_destroy(&xpns);
    tt_xpath_destroy(&xp);

    // none match
    ret = tt_xpath_create(&xp, "/bookstore/book/@not_exist", NULL);
    TT_UT_SUCCESS(ret, "");

    tt_xpnodes_init(&xpns);
    tt_xnode_select_all(root, &xp, &xpns);
    TT_UT_EQUAL(tt_xpnodes_count(&xpns), 0, "");

    // single
    tt_xnode_select(root, &xp, &xn, &xa);
    TT_UT_NULL(xn, "");
    TT_UT_NULL(xa, "");

    tt_xpnodes_destroy(&xpns);
    tt_xpath_destroy(&xp);

    tt_xdoc_destroy(&xd);

    // test end
    TT_TEST_CASE_LEAVE()
}
