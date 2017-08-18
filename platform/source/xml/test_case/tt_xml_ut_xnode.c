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
TT_TEST_ROUTINE_DECLARE(case_xnode_bool)
TT_TEST_ROUTINE_DECLARE(case_xnode_int)
TT_TEST_ROUTINE_DECLARE(case_xnode_rel)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(xml_xnode_case)

TT_TEST_CASE("case_xnode_bool",
             "xml: node value bool",
             case_xnode_bool,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("case_xnode_int",
                 "xml: node value int",
                 case_xnode_int,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_xnode_rel",
                 "xml: node relation",
                 case_xnode_rel,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(xml_xnode_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(XML_UT_XNODE, 0, xml_xnode_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(case_xnode_rel)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    static const tt_char_t __ut_xnode[] =
        "<node> <b1>1</b1> "
        "<u32_1> 0xFFFFFFFF</u32_1>"
        "<s32_1>-2147483648  </s32_1>"
        "<s64_1> -9223372036854775808  </s64_1>"
        "<u64_1>18446744073709551615</u64_1>"
        "<float_1>12.34</float_1>"
        "<double_1>-2.789   </double_1>"

        "<str_2></str_2>"
        "<str_1>the last attr is a string</str_1>"
        "</node>";

TT_TEST_ROUTINE_DEFINE(case_xnode_bool)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_xdoc_t xd;
    tt_result_t ret;
    tt_xnode_t xn;
    tt_xnode_t xa;

    TT_TEST_CASE_ENTER()
    // test start

    tt_xdoc_create(&xd);

    ret = tt_xdoc_parse(&xd, (tt_u8_t *)__ut_xnode, sizeof(__ut_xnode), NULL);
    TT_UT_SUCCESS(ret, "");

    xn = tt_xdoc_root(&xd);
    TT_UT_NOT_NULL(xn, "");
    xn = tt_xnode_child_byname(xn, "node");
    TT_UT_NOT_NULL(xn, "");

    // bool
    xa = tt_xnode_child_byname(xn, "b1");
    TT_UT_NOT_NULL(xa, "");
    TT_UT_EQUAL(tt_xnode_get_bool(xa, TT_FALSE), TT_TRUE, "");
    TT_UT_EQUAL(tt_xnode_get_u32(xa, 0), 1, "");
    TT_UT_EQUAL(tt_xnode_get_s32(xa, 0), 1, "");
    TT_UT_EQUAL(tt_xnode_get_u64(xa, 0), 1, "");
    TT_UT_EQUAL(tt_xnode_get_s64(xa, 0), 1, "");
    TT_UT_EQUAL(tt_xnode_get_float(xa, 0), 1, "");
    TT_UT_EQUAL(tt_xnode_get_double(xa, 0), 1, "");

    ret = tt_xnode_set_name(xa, "new b1");
    TT_UT_SUCCESS(ret, "");

    ret = tt_xnode_set_bool(xa, TT_FALSE);
    TT_UT_SUCCESS(ret, "");

    xa = tt_xnode_child_byname(xn, "new b1");
    TT_UT_NOT_NULL(xa, "");
    TT_UT_EQUAL(tt_xnode_get_bool(xa, TT_FALSE), TT_FALSE, "");
    TT_UT_EQUAL(tt_xnode_get_u32(xa, 0), 0, "");
    TT_UT_EQUAL(tt_xnode_get_s32(xa, 0), 0, "");
    TT_UT_EQUAL(tt_xnode_get_u64(xa, 0), 0, "");
    TT_UT_EQUAL(tt_xnode_get_s64(xa, 0), 0, "");
    TT_UT_EQUAL(tt_xnode_get_float(xa, 0), 0, "");
    TT_UT_EQUAL(tt_xnode_get_double(xa, 0), 0, "");

    tt_xdoc_destroy(&xd);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_xnode_int)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_xdoc_t xd;
    tt_result_t ret;
    tt_xnode_t xn;
    tt_xnode_t xa;

    TT_TEST_CASE_ENTER()
    // test start

    tt_xdoc_create(&xd);

    ret = tt_xdoc_parse(&xd, (tt_u8_t *)__ut_xnode, sizeof(__ut_xnode), NULL);
    TT_UT_SUCCESS(ret, "");

    xn = tt_xdoc_root(&xd);
    TT_UT_NOT_NULL(xn, "");
    xn = tt_xnode_child_byname(xn, "node");
    TT_UT_NOT_NULL(xn, "");

    // u32
    xa = tt_xnode_child_byname(xn, "u32_1");
    TT_UT_NOT_NULL(xa, "");
    TT_UT_EQUAL(tt_xnode_get_bool(xa, TT_TRUE), TT_FALSE, "");
    TT_UT_EQUAL(tt_xnode_get_u32(xa, 0), 0xFFFFFFFF, "");
    TT_UT_EQUAL(tt_xnode_get_s32(xa, 0), 2147483647, "");
    TT_UT_EQUAL(tt_xnode_get_u64(xa, 0), 0xFFFFFFFF, "");
    TT_UT_EQUAL(tt_xnode_get_s64(xa, 0), 0xFFFFFFFF, "");
    TT_UT_EQUAL(tt_xnode_get_float(xa, 0), 0xFFFFFFFF, "");
    TT_UT_EQUAL(tt_xnode_get_double(xa, 0), 0xFFFFFFFF, "");

    ret = tt_xnode_set_u32(xa, 1);
    TT_UT_SUCCESS(ret, "");
    xa = tt_xnode_child_byname(xn, "u32_1");
    TT_UT_NOT_NULL(xa, "");
    TT_UT_EQUAL(tt_xnode_get_u32(xa, 0), 1, "");

    // s32
    xa = tt_xnode_child_byname(xn, "s32_1");
    TT_UT_NOT_NULL(xa, "");
    TT_UT_EQUAL(tt_xnode_get_bool(xa, TT_TRUE), TT_FALSE, "");
    TT_UT_EQUAL(tt_xnode_get_u32(xa, 0), 0, "");
    TT_UT_EQUAL(tt_xnode_get_s32(xa, 0), -2147483648LL, "");
    TT_UT_EQUAL(tt_xnode_get_u64(xa, 0), 0, "");
    TT_UT_EQUAL(tt_xnode_get_s64(xa, 0), -2147483648LL, "");
    TT_UT_EQUAL(tt_xnode_get_float(xa, 0), -2147483648LL, "");
    TT_UT_EQUAL(tt_xnode_get_double(xa, 0), -2147483648LL, "");

    ret = tt_xnode_set_s32(xa, 2147483647);
    TT_UT_SUCCESS(ret, "");
    xa = tt_xnode_child_byname(xn, "s32_1");
    TT_UT_NOT_NULL(xa, "");
    TT_UT_EQUAL(tt_xnode_get_u32(xa, 0), 2147483647, "");

    // s64
    xa = tt_xnode_child_byname(xn, "s64_1");
    TT_UT_NOT_NULL(xa, "");
    TT_UT_EQUAL(tt_xnode_get_bool(xa, TT_TRUE), TT_FALSE, "");
    TT_UT_EQUAL(tt_xnode_get_u32(xa, 0), 0, "");
    TT_UT_EQUAL(tt_xnode_get_s32(xa, 0), -2147483648LL, ""); // truncated
    TT_UT_EQUAL(tt_xnode_get_u64(xa, 0), 0, "");
    // TT_UT_EQUAL(tt_xnode_get_s64(xa, 0), -9223372036854775808LL, "");
    TT_UT_EQUAL(tt_xnode_get_s64(xa, 0), (tt_s64_t)0x8000000000000000, "");
    // TT_UT_EQUAL(tt_xnode_get_float(xa, 0), -9.22337203E+18, "");
    // TT_UT_EQUAL(tt_xnode_get_double(xa, 0), -9.2233720368547758E+18, "");

    ret = tt_xnode_set_s64(xa, 9223372036854775807);
    TT_UT_SUCCESS(ret, "");
    xa = tt_xnode_child_byname(xn, "s64_1");
    TT_UT_NOT_NULL(xa, "");
    TT_UT_EQUAL(tt_xnode_get_u64(xa, 0), 9223372036854775807, "");

    // u64
    xa = tt_xnode_child_byname(xn, "u64_1");
    TT_UT_NOT_NULL(xa, "");
    TT_UT_EQUAL(tt_xnode_get_bool(xa, TT_FALSE), TT_TRUE, ""); // begins with 1
    TT_UT_EQUAL(tt_xnode_get_u32(xa, 0), 0xFFFFFFFF, "");
    TT_UT_EQUAL(tt_xnode_get_s32(xa, 0), 2147483647, ""); // truncated
    TT_UT_EQUAL(tt_xnode_get_u64(xa, 0), 18446744073709551615ULL, "");
    TT_UT_EQUAL(tt_xnode_get_s64(xa, 0), 9223372036854775807ULL, "");

    ret = tt_xnode_set_s64(xa, 1);
    TT_UT_SUCCESS(ret, "");
    xa = tt_xnode_child_byname(xn, "u64_1");
    TT_UT_NOT_NULL(xa, "");
    TT_UT_EQUAL(tt_xnode_get_u64(xa, 0), 1, "");

    // float
    xa = tt_xnode_child_byname(xn, "float_1");
    TT_UT_NOT_NULL(xa, "");
    TT_UT_EQUAL(tt_xnode_get_bool(xa, TT_FALSE), TT_TRUE, ""); // begins with 1
    TT_UT_EQUAL(tt_xnode_get_u32(xa, 0), 12, "");
    TT_UT_EQUAL(tt_xnode_get_s32(xa, 0), 12, ""); // truncated
    TT_UT_EQUAL(tt_xnode_get_u64(xa, 0), 12, "");
    TT_UT_EQUAL(tt_xnode_get_s64(xa, 0), 12, "");

    ret = tt_xnode_set_float(xa, -12.34f);
    TT_UT_SUCCESS(ret, "");
    xa = tt_xnode_child_byname(xn, "float_1");
    TT_UT_NOT_NULL(xa, "");
    TT_UT_EXP(fabsf(tt_xnode_get_float(xa, 0) - (-12.34f)) < 0.001, "");
    TT_UT_EQUAL(tt_xnode_get_u32(xa, 111), 0, ""); // ?

    // double
    xa = tt_xnode_child_byname(xn, "double_1");
    TT_UT_NOT_NULL(xa, "");
    TT_UT_EQUAL(tt_xnode_get_bool(xa, TT_TRUE),
                TT_FALSE,
                ""); // not begins with 1
    TT_UT_EQUAL(tt_xnode_get_u32(xa, 0), 0, "");
    TT_UT_EQUAL(tt_xnode_get_s32(xa, 0), -2, ""); // truncated
    TT_UT_EQUAL(tt_xnode_get_u64(xa, 0), 0, "");
    TT_UT_EQUAL(tt_xnode_get_s64(xa, 0), -2, "");

    ret = tt_xnode_set_double(xa, -12.345);
    TT_UT_SUCCESS(ret, "");
    xa = tt_xnode_child_byname(xn, "double_1");
    TT_UT_NOT_NULL(xa, "");
    TT_UT_EXP(fabs(tt_xnode_get_double(xa, 0) - (-12.345)) < 0.0001, "");

    tt_xdoc_destroy(&xd);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_xnode_rel)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_xdoc_t xd;
    tt_result_t ret;
    tt_xnode_t root, xn, xn2;
    tt_u8_t buf[512];

    TT_TEST_CASE_ENTER()
    // test start

    ret = tt_xdoc_create(&xd);
    TT_UT_SUCCESS(ret, "");

    root = tt_xdoc_root(&xd);
    TT_UT_NOT_NULL(root, "");

    // add: doc
    xn = tt_xnode_append_child(root, TT_XNODE_DOCUMENT);
    TT_UT_NULL(xn, "");

    // add: null
    xn = tt_xnode_append_child(root, TT_XNODE_NULL);
    TT_UT_NULL(xn, "");

    // add: element
    xn = tt_xnode_append_child(root, TT_XNODE_ELEMENT);
    TT_UT_NOT_NULL(xn, "");
    ret = tt_xnode_set_name(xn, "node-1");
    TT_UT_SUCCESS(ret, "");
    ret = tt_xnode_set_value(xn, "value-1");
    TT_UT_FAIL(ret, "");

    // add: text
    xn = tt_xnode_prepend_child(xn, TT_XNODE_TEXT);
    TT_UT_NOT_NULL(xn, "");
    ret = tt_xnode_set_name(xn, "node-1");
    TT_UT_FAIL(ret, "");
    ret = tt_xnode_set_value(xn, "value-1");
    TT_UT_SUCCESS(ret, "");

    // add: cdata
    xn = tt_xnode_prepend_child(root, TT_XNODE_CDATA);
    TT_UT_NOT_NULL(xn, "");
    ret = tt_xnode_set_name(xn, "node-2");
    TT_UT_FAIL(ret, "");
    ret = tt_xnode_set_value(xn, "cdata-2");
    TT_UT_SUCCESS(ret, "");

    // insert after
    xn2 = tt_xnode_insert_child_after(root, xn, TT_XNODE_COMMENT);
    TT_UT_NOT_NULL(xn2, "");
    ret = tt_xnode_set_name(xn2, "node-3");
    TT_UT_FAIL(ret, "");
    ret = tt_xnode_set_value(xn2, "comment-3");
    TT_UT_SUCCESS(ret, "");

    // insert before
    xn2 = tt_xnode_insert_child_before(root, xn, TT_XNODE_PI);
    TT_UT_NOT_NULL(xn2, "");
    ret = tt_xnode_set_name(xn2, "node-4");
    TT_UT_SUCCESS(ret, "");
    ret = tt_xnode_set_value(xn2, "pi-4");
    TT_UT_SUCCESS(ret, "");

    // add: declaration
    xn = tt_xnode_append_child(root, TT_XNODE_DECLARATION);
    TT_UT_NOT_NULL(xn, "");
    ret = tt_xnode_set_name(xn, "node-5");
    TT_UT_SUCCESS(ret, "");
    ret = tt_xnode_set_value(xn, "declare-1");
    TT_UT_FAIL(ret, "");

    // add: doctype
    xn = tt_xnode_append_child(root, TT_XNODE_DOCTYPE);
    TT_UT_NOT_NULL(xn, "");
    ret = tt_xnode_set_name(xn, "node-6");
    TT_UT_FAIL(ret, "");
    ret = tt_xnode_set_value(xn, "doctype-6");
    TT_UT_SUCCESS(ret, "");

    tt_memset(buf, 0, sizeof(buf));
    ret = tt_xdoc_render(&xd, buf, sizeof(buf), NULL);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_strcmp((tt_char_t *)buf,
                          "<?xml version=\"1.0\"?>\n<?node-4 "
                          "pi-4?><![CDATA[cdata-2]]><!--comment-3-->\n<node-1>"
                          "value-1</node-1>\n<?node-5?>\n<!DOCTYPE "
                          "doctype-6>\n"),
                0,
                "");

    // remove child
    ret = tt_xnode_remove_child(root, xn);
    TT_UT_SUCCESS(ret, "");
    ret = tt_xnode_remove_child(root, NULL);
    TT_UT_FAIL(ret, "");

    ret = tt_xnode_remove_child_byname(root, "node-4");
    TT_UT_SUCCESS(ret, "");
    ret = tt_xnode_remove_child_byname(root, "node-99");
    TT_UT_FAIL(ret, "");

    tt_memset(buf, 0, sizeof(buf));
    ret = tt_xdoc_render(&xd, buf, sizeof(buf), NULL);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_strcmp((tt_char_t *)buf,
                          "<?xml "
                          "version=\"1.0\"?>\n<![CDATA[cdata-2]]><!--comment-3-"
                          "->\n<node-1>value-1</node-1>\n<?node-5?>\n"),
                0,
                "");

    tt_xdoc_destroy(&xd);

    // test end
    TT_TEST_CASE_LEAVE()
}
