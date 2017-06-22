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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_xattr_rel)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_xattr_bool)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_xattr_int)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(xml_xattr_case)

TT_TEST_CASE("tt_unit_test_xattr_rel",
             "xml: attribute relation",
             tt_unit_test_xattr_rel,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_xattr_bool",
                 "xml: attribute value",
                 tt_unit_test_xattr_bool,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_xattr_int",
                 "xml: attribute value int",
                 tt_unit_test_xattr_int,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(xml_xattr_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(XML_UT_XATTR, 0, xml_xattr_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(tt_unit_test_xattr_rel)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    static const tt_char_t __ut_xattr[] =
        "<node b1=\"1\" "
        "u32_1=\"4294967295\" "
        "s32_1=\"-2147483648\" "
        "s64_1=\"-9223372036854775808\" "
        "u64_1=\"18446744073709551615\" "
        "float_1=\"12.34\" "
        "double_1=\"-2.789\" "

        "str_2=\"\" "
        "str_1=\"the last attr is a string\" "
        "/>";

TT_TEST_ROUTINE_DEFINE(tt_unit_test_xattr_rel)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_xdoc_t xd;
    tt_result_t ret;
    tt_xnode_t xn;
    tt_xattr_t xa, xa2;
    tt_u8_t buf[500];

    TT_TEST_CASE_ENTER()
    // test start

    tt_xdoc_create(&xd);

    ret = tt_xdoc_parse(&xd, (tt_u8_t *)__ut_xattr, sizeof(__ut_xattr), NULL);
    TT_UT_SUCCESS(ret, "");

    xn = tt_xdoc_root(&xd);
    TT_UT_NOT_NULL(xn, "");
    xn = tt_xnode_child_byname(xn, "node");
    TT_UT_NOT_NULL(xn, "");

    xa = tt_xnode_first_attr(xn);
    TT_UT_NOT_NULL(xa, "");
    TT_UT_EQUAL(tt_strcmp(tt_xattr_get_value(xa, "xx"), "1"), 0, "");

    xa2 = tt_xattr_prev(xa);
    TT_UT_NULL(xa2, "");

    xa2 = tt_xattr_next(xa);
    TT_UT_NOT_NULL(xa2, "");
    TT_UT_EQUAL(tt_strcmp(tt_xattr_get_value(xa2, "xx"), "4294967295"), 0, "");

    xa = tt_xnode_last_attr(xn);
    TT_UT_NOT_NULL(xa, "");
    TT_UT_EQUAL(tt_strcmp(tt_xattr_get_value(xa, "xx"),
                          "the last attr is a string"),
                0,
                "");

    xa2 = tt_xattr_prev(xa);
    TT_UT_NOT_NULL(xa2, "");
    TT_UT_EQUAL(tt_strcmp(tt_xattr_get_value(xa2, "xx"), ""), 0, "");

    xa2 = tt_xattr_next(xa);
    TT_UT_NULL(xa2, "");

    // append
    xa = tt_xnode_append_attr(xn, "attr_app");
    ret = tt_xattr_set_value(xa, "app_val");
    TT_UT_SUCCESS(ret, "");

    xa = tt_xnode_insert_attr_after(xn, xa, "i_after");
    ret = tt_xattr_set_value(xa, "after_val");
    TT_UT_SUCCESS(ret, "");

    xa = tt_xnode_prepend_attr(xn, "attr_prep");
    ret = tt_xattr_set_value(xa, "prep_val");
    TT_UT_SUCCESS(ret, "");

    xa = tt_xnode_insert_attr_before(xn, xa, "i_before");
    ret = tt_xattr_set_value(xa, "before_val");
    TT_UT_SUCCESS(ret, "");

    tt_memset(buf, 0, sizeof(buf));
    ret = tt_xdoc_render(&xd, buf, sizeof(buf), NULL);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_strcmp((tt_char_t *)buf,
                          "<?xml version=\"1.0\"?>\n<node "
                          "i_before=\"before_val\" attr_prep=\"prep_val\" "
                          "b1=\"1\" u32_1=\"4294967295\" s32_1=\"-2147483648\" "
                          "s64_1=\"-9223372036854775808\" "
                          "u64_1=\"18446744073709551615\" float_1=\"12.34\" "
                          "double_1=\"-2.789\" str_2=\"\" str_1=\"the last "
                          "attr is a string\" attr_app=\"app_val\" "
                          "i_after=\"after_val\" />\n"),
                0,
                "");

    // remove
    ret = tt_xnode_remove_attr(xn, xa);
    TT_UT_SUCCESS(ret, "");
    xa = NULL;
    ret = tt_xnode_remove_attr(xn, xa);
    TT_UT_FAIL(ret, "");

    ret = tt_xnode_remove_attr_byname(xn, "attr_prep");
    TT_UT_SUCCESS(ret, "");
    ret = tt_xnode_remove_attr_byname(xn, "???");
    TT_UT_FAIL(ret, "");

    tt_memset(buf, 0, sizeof(buf));
    ret = tt_xdoc_render(&xd, buf, sizeof(buf), NULL);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_strcmp((tt_char_t *)buf,
                          "<?xml version=\"1.0\"?>\n<node b1=\"1\" "
                          "u32_1=\"4294967295\" s32_1=\"-2147483648\" "
                          "s64_1=\"-9223372036854775808\" "
                          "u64_1=\"18446744073709551615\" float_1=\"12.34\" "
                          "double_1=\"-2.789\" str_2=\"\" str_1=\"the last "
                          "attr is a string\" attr_app=\"app_val\" "
                          "i_after=\"after_val\" />\n"),
                0,
                "");

    tt_xdoc_destroy(&xd);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_xattr_bool)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_xdoc_t xd;
    tt_result_t ret;
    tt_xnode_t xn;
    tt_xattr_t xa;

    TT_TEST_CASE_ENTER()
    // test start

    tt_xdoc_create(&xd);

    ret = tt_xdoc_parse(&xd, (tt_u8_t *)__ut_xattr, sizeof(__ut_xattr), NULL);
    TT_UT_SUCCESS(ret, "");

    xn = tt_xdoc_root(&xd);
    TT_UT_NOT_NULL(xn, "");
    xn = tt_xnode_child_byname(xn, "node");
    TT_UT_NOT_NULL(xn, "");

    // bool
    xa = tt_xnode_selectxptr_byname(xn, "b1");
    TT_UT_NOT_NULL(xa, "");
    TT_UT_EQUAL(tt_xattr_get_bool(xa, TT_FALSE), TT_TRUE, "");
    TT_UT_EQUAL(tt_xattr_get_u32(xa, 0), 1, "");
    TT_UT_EQUAL(tt_xattr_get_s32(xa, 0), 1, "");
    TT_UT_EQUAL(tt_xattr_get_u64(xa, 0), 1, "");
    TT_UT_EQUAL(tt_xattr_get_s64(xa, 0), 1, "");
    TT_UT_EQUAL(tt_xattr_get_float(xa, 0), 1, "");
    TT_UT_EQUAL(tt_xattr_get_double(xa, 0), 1, "");

    ret = tt_xattr_set_name(xa, "new b1");
    TT_UT_SUCCESS(ret, "");

    ret = tt_xattr_set_bool(xa, TT_FALSE);
    TT_UT_SUCCESS(ret, "");

    xa = tt_xnode_selectxptr_byname(xn, "new b1");
    TT_UT_NOT_NULL(xa, "");
    TT_UT_EQUAL(tt_xattr_get_bool(xa, TT_FALSE), TT_FALSE, "");
    TT_UT_EQUAL(tt_xattr_get_u32(xa, 0), 0, "");
    TT_UT_EQUAL(tt_xattr_get_s32(xa, 0), 0, "");
    TT_UT_EQUAL(tt_xattr_get_u64(xa, 0), 0, "");
    TT_UT_EQUAL(tt_xattr_get_s64(xa, 0), 0, "");
    TT_UT_EQUAL(tt_xattr_get_float(xa, 0), 0, "");
    TT_UT_EQUAL(tt_xattr_get_double(xa, 0), 0, "");

    tt_xdoc_destroy(&xd);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_xattr_int)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_xdoc_t xd;
    tt_result_t ret;
    tt_xnode_t xn;
    tt_xattr_t xa;

    TT_TEST_CASE_ENTER()
    // test start

    tt_xdoc_create(&xd);

    ret = tt_xdoc_parse(&xd, (tt_u8_t *)__ut_xattr, sizeof(__ut_xattr), NULL);
    TT_UT_SUCCESS(ret, "");

    xn = tt_xdoc_root(&xd);
    TT_UT_NOT_NULL(xn, "");
    xn = tt_xnode_child_byname(xn, "node");
    TT_UT_NOT_NULL(xn, "");

    // u32
    xa = tt_xnode_selectxptr_byname(xn, "u32_1");
    TT_UT_NOT_NULL(xa, "");
    TT_UT_EQUAL(tt_xattr_get_bool(xa, TT_TRUE), TT_FALSE, "");
    TT_UT_EQUAL(tt_xattr_get_u32(xa, 0), 4294967295, "");
    TT_UT_EQUAL(tt_xattr_get_s32(xa, 0), 2147483647, "");
    TT_UT_EQUAL(tt_xattr_get_u64(xa, 0), 4294967295, "");
    TT_UT_EQUAL(tt_xattr_get_s64(xa, 0), 4294967295, "");
    TT_UT_EQUAL(tt_xattr_get_float(xa, 0), 4294967295, "");
    TT_UT_EQUAL(tt_xattr_get_double(xa, 0), 4294967295, "");

    ret = tt_xattr_set_u32(xa, 1);
    TT_UT_SUCCESS(ret, "");
    xa = tt_xnode_selectxptr_byname(xn, "u32_1");
    TT_UT_NOT_NULL(xa, "");
    TT_UT_EQUAL(tt_xattr_get_u32(xa, 0), 1, "");

    // s32
    xa = tt_xnode_selectxptr_byname(xn, "s32_1");
    TT_UT_NOT_NULL(xa, "");
    TT_UT_EQUAL(tt_xattr_get_bool(xa, TT_TRUE), TT_FALSE, "");
    TT_UT_EQUAL(tt_xattr_get_u32(xa, 0), 0, "");
    TT_UT_EQUAL(tt_xattr_get_s32(xa, 0), -2147483648, "");
    TT_UT_EQUAL(tt_xattr_get_u64(xa, 0), 0, "");
    TT_UT_EQUAL(tt_xattr_get_s64(xa, 0), -2147483648, "");
    TT_UT_EQUAL(tt_xattr_get_float(xa, 0), -2147483648, "");
    TT_UT_EQUAL(tt_xattr_get_double(xa, 0), -2147483648, "");

    ret = tt_xattr_set_s32(xa, 2147483647);
    TT_UT_SUCCESS(ret, "");
    xa = tt_xnode_selectxptr_byname(xn, "s32_1");
    TT_UT_NOT_NULL(xa, "");
    TT_UT_EQUAL(tt_xattr_get_u32(xa, 0), 2147483647, "");

    // s64
    xa = tt_xnode_selectxptr_byname(xn, "s64_1");
    TT_UT_NOT_NULL(xa, "");
    TT_UT_EQUAL(tt_xattr_get_bool(xa, TT_TRUE), TT_FALSE, "");
    TT_UT_EQUAL(tt_xattr_get_u32(xa, 0), 0, "");
    TT_UT_EQUAL(tt_xattr_get_s32(xa, 0), -2147483648, ""); // truncated
    TT_UT_EQUAL(tt_xattr_get_u64(xa, 0), 0, "");
    TT_UT_EQUAL(tt_xattr_get_s64(xa, 0), -9223372036854775808ULL, "");
    // TT_UT_EQUAL(tt_xattr_get_float(xa, 0), -9.22337203E+18, "");
    // TT_UT_EQUAL(tt_xattr_get_double(xa, 0), -9.2233720368547758E+18, "");

    ret = tt_xattr_set_s64(xa, 9223372036854775807);
    TT_UT_SUCCESS(ret, "");
    xa = tt_xnode_selectxptr_byname(xn, "s64_1");
    TT_UT_NOT_NULL(xa, "");
    TT_UT_EQUAL(tt_xattr_get_u64(xa, 0), 9223372036854775807, "");

    // u64
    xa = tt_xnode_selectxptr_byname(xn, "u64_1");
    TT_UT_NOT_NULL(xa, "");
    TT_UT_EQUAL(tt_xattr_get_bool(xa, TT_FALSE), TT_TRUE, ""); // begins with 1
    TT_UT_EQUAL(tt_xattr_get_u32(xa, 0), 4294967295, "");
    TT_UT_EQUAL(tt_xattr_get_s32(xa, 0), 2147483647, ""); // truncated
    TT_UT_EQUAL(tt_xattr_get_u64(xa, 0), 18446744073709551615ULL, "");
    TT_UT_EQUAL(tt_xattr_get_s64(xa, 0), 9223372036854775807ULL, "");

    ret = tt_xattr_set_s64(xa, 1);
    TT_UT_SUCCESS(ret, "");
    xa = tt_xnode_selectxptr_byname(xn, "u64_1");
    TT_UT_NOT_NULL(xa, "");
    TT_UT_EQUAL(tt_xattr_get_u64(xa, 0), 1, "");

    // float
    xa = tt_xnode_selectxptr_byname(xn, "float_1");
    TT_UT_NOT_NULL(xa, "");
    TT_UT_EQUAL(tt_xattr_get_bool(xa, TT_FALSE), TT_TRUE, ""); // begins with 1
    TT_UT_EQUAL(tt_xattr_get_u32(xa, 0), 12, "");
    TT_UT_EQUAL(tt_xattr_get_s32(xa, 0), 12, ""); // truncated
    TT_UT_EQUAL(tt_xattr_get_u64(xa, 0), 12, "");
    TT_UT_EQUAL(tt_xattr_get_s64(xa, 0), 12, "");

    ret = tt_xattr_set_float(xa, -12.34);
    TT_UT_SUCCESS(ret, "");
    xa = tt_xnode_selectxptr_byname(xn, "float_1");
    TT_UT_NOT_NULL(xa, "");
    TT_UT_EXP(fabsf(tt_xattr_get_float(xa, 0) - (-12.34f)) < 0.001, "");
    TT_UT_EQUAL(tt_xattr_get_u32(xa, 111), 0, ""); // ?

    // double
    xa = tt_xnode_selectxptr_byname(xn, "double_1");
    TT_UT_NOT_NULL(xa, "");
    TT_UT_EQUAL(tt_xattr_get_bool(xa, TT_TRUE),
                TT_FALSE,
                ""); // not begins with 1
    TT_UT_EQUAL(tt_xattr_get_u32(xa, 0), 0, "");
    TT_UT_EQUAL(tt_xattr_get_s32(xa, 0), -2, ""); // truncated
    TT_UT_EQUAL(tt_xattr_get_u64(xa, 0), 0, "");
    TT_UT_EQUAL(tt_xattr_get_s64(xa, 0), -2, "");

    ret = tt_xattr_set_double(xa, -12.345);
    TT_UT_SUCCESS(ret, "");
    xa = tt_xnode_selectxptr_byname(xn, "double_1");
    TT_UT_NOT_NULL(xa, "");
    TT_UT_EXP(fabs(tt_xattr_get_double(xa, 0) - (-12.345)) < 0.0001, "");

    tt_xdoc_destroy(&xd);

    // test end
    TT_TEST_CASE_LEAVE()
}
