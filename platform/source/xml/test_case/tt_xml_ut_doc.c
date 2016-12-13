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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_doc_basic)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(xdoc_case)

TT_TEST_CASE("tt_unit_test_doc_basic",
             "xml node parse: basic doc",
             tt_unit_test_doc_basic,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE_LIST_DEFINE_END(xdoc_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(XML_UT_PSST_DOC, 0, xdoc_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
     TT_TEST_ROUTINE_DEFINE(tt_unit_test_doc_basic)
     {
     //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
     tt_xmldoc_t xdoc;
     tt_result_t ret;

     TT_TEST_CASE_ENTER()
     // test start

     ret = tt_xmldoc_create(&xdoc, NULL);
     TT_TEST_CHECK_SUCCESS(ret, "");

     tt_xmldoc_destroy(&xdoc);

     // test end
     TT_TEST_CASE_LEAVE()
     }
     */

    // clang-format off
static tt_u8_t __tdb_xml1[] =
"<?xml version=\"1.0\"?>\n"
"<?xml-stylesheet href=\"catalog.xsl\" type=\"text/xsl\"?>\n"
"<!DOCTYPE catalog SYSTEM \"catalog.dtd\">\n"
"<catalog>\n"
"   <product description=\"Cardigan Sweater\" product_image=\"cardigan.jpg\">\n"
"      <catalog_item gender=\"Men's\">\n"
"         <item_number>QWZ5671</item_number>\n"
"         <price>39.95</price>\n"
"         <size description=\"Medium\">\n"
"            <color_swatch image=\"red_cardigan.jpg\">Red</color_swatch>\n"
"            <color_swatch image=\"burgundy_cardigan.jpg\">Burgundy</color_swatch>\n"
"         </size>\n"
"         <size description=\"Large\">\n"
"            <color_swatch image=\"red_cardigan.jpg\">Red</color_swatch>\n"
"            <color_swatch image=\"burgundy_cardigan.jpg\">Burgundy</color_swatch>\n"
"         </size>\n"
"      </catalog_item>\n"
"      <catalog_item gender=\"Women's\">\n"
"         <item_number>RRX9856</item_number>\n"
"         <price>42.50</price>\n"
"         <size description=\"Small\">\n"
"            <color_swatch image=\"red_cardigan.jpg\">Red</color_swatch>\n"
"            <color_swatch image=\"navy_cardigan.jpg\">Navy</color_swatch>\n"
"            <color_swatch image=\"burgundy_cardigan.jpg\">Burgundy</color_swatch>\n"
"         </size>\n"
"         <size description=\"Medium\">\n"
"            <color_swatch image=\"red_cardigan.jpg\">Red</color_swatch>\n"
"            <color_swatch image=\"navy_cardigan.jpg\">Navy</color_swatch>\n"
"            <color_swatch image=\"burgundy_cardigan.jpg\">Burgundy</color_swatch>\n"
"            <color_swatch image=\"black_cardigan.jpg\">Black</color_swatch>\n"
"         </size>\n"
"         <size description=\"Large\">\n"
"            <color_swatch image=\"navy_cardigan.jpg\">Navy</color_swatch>\n"
"            <color_swatch image=\"black_cardigan.jpg\">Black</color_swatch>\n"
"         </size>\n"
"         <size description=\"Extra Large\">\n"
"            <color_swatch image=\"burgundy_cardigan.jpg\">Burgundy</color_swatch>\n"
"            <color_swatch image=\"black_cardigan.jpg\">Black</color_swatch>\n"
"         </size>\n"
"      </catalog_item>\n"
"   </product>\n"
"</catalog>\n";
// clang-format on

TT_TEST_ROUTINE_DEFINE(tt_unit_test_doc_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_xmldoc_t xdoc;
    tt_result_t ret;

    TT_TEST_CASE_ENTER()
    // test start

    ret = tt_xmldoc_create(&xdoc, NULL);
    TT_TEST_CHECK_SUCCESS(ret, "");

    ret = tt_xmldoc_update(&xdoc, __tdb_xml1, sizeof(__tdb_xml1));
    TT_TEST_CHECK_SUCCESS(ret, "");

    ret = tt_xmldoc_final(&xdoc, NULL);
    TT_TEST_CHECK_SUCCESS(ret, "");

    tt_xmldoc_destroy(&xdoc);

    // test end
    TT_TEST_CASE_LEAVE()
}
