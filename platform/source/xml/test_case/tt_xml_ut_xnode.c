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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_xnode_bool)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_xnode_int)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(xml_xnode_case)

TT_TEST_CASE("tt_unit_test_xnode_bool",
             "xml: node value bool",
             tt_unit_test_xnode_bool,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_xnode_int",
                 "xml: node value int",
                 tt_unit_test_xnode_int,
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
    TT_TEST_ROUTINE_DEFINE(tt_unit_test_xattr_rel)
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
        "<u32_1> 4294967295</u32_1>"
        "<s32_1>-2147483648  </s32_1>"
        "<s64_1> -9223372036854775808  </s64_1>"
        "<u64_1>18446744073709551615</u64_1>"
        "<float_1>12.34</float_1>"
        "<double_1>-2.789   </double_1>"

        "<str_2></str_2>"
        "<str_1>the last attr is a string</str_1>"
        "</node>";

TT_TEST_ROUTINE_DEFINE(tt_unit_test_xnode_bool)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_xdoc_t xd;
    tt_result_t ret;
    tt_xnode_t xn;
    tt_xnode_t xa;

    TT_TEST_CASE_ENTER()
    // test start

    tt_xdoc_init(&xd);

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

TT_TEST_ROUTINE_DEFINE(tt_unit_test_xnode_int)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_xdoc_t xd;
    tt_result_t ret;
    tt_xnode_t xn;
    tt_xnode_t xa;

    TT_TEST_CASE_ENTER()
    // test start

    tt_xdoc_init(&xd);

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
    TT_UT_EQUAL(tt_xnode_get_u32(xa, 0), 4294967295, "");
    TT_UT_EQUAL(tt_xnode_get_s32(xa, 0), 2147483647, "");
    TT_UT_EQUAL(tt_xnode_get_u64(xa, 0), 4294967295, "");
    TT_UT_EQUAL(tt_xnode_get_s64(xa, 0), 4294967295, "");
    TT_UT_EQUAL(tt_xnode_get_float(xa, 0), 4294967295, "");
    TT_UT_EQUAL(tt_xnode_get_double(xa, 0), 4294967295, "");

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
    TT_UT_EQUAL(tt_xnode_get_s32(xa, 0), -2147483648, "");
    TT_UT_EQUAL(tt_xnode_get_u64(xa, 0), 0, "");
    TT_UT_EQUAL(tt_xnode_get_s64(xa, 0), -2147483648, "");
    TT_UT_EQUAL(tt_xnode_get_float(xa, 0), -2147483648, "");
    TT_UT_EQUAL(tt_xnode_get_double(xa, 0), -2147483648, "");

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
    TT_UT_EQUAL(tt_xnode_get_s32(xa, 0), -2147483648, ""); // truncated
    TT_UT_EQUAL(tt_xnode_get_u64(xa, 0), 0, "");
    TT_UT_EQUAL(tt_xnode_get_s64(xa, 0), -9223372036854775808ULL, "");
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
    TT_UT_EQUAL(tt_xnode_get_u32(xa, 0), 4294967295, "");
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

    ret = tt_xnode_set_float(xa, -12.34);
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
