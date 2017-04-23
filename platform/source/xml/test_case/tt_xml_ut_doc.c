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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_doc_render)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(xdoc_case)

TT_TEST_CASE("tt_unit_test_doc_basic",
             "xml doc parse: basic doc",
             tt_unit_test_doc_basic,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_doc_render",
                 "xml doc render: basic doc",
                 tt_unit_test_doc_render,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

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
     TT_TEST_ROUTINE_DEFINE(tt_unit_test_doc_render)
     {
     //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
     tt_xmldoc_t xdoc;
     tt_result_t ret;

     TT_TEST_CASE_ENTER()
     // test start

     ret = tt_xmldoc_create(&xdoc, NULL);
     TT_UT_SUCCESS(ret, "");

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
"      ------\n"
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
"      //////\n"
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

static tt_result_t __chkxdoc_ret;
static tt_u32_t __chkxdoc_err_line;
#define __chkxdoc(e)                                                           \
    do {                                                                       \
        if (!(e)) {                                                            \
            __chkxdoc_ret = TT_FAIL;                                           \
            __chkxdoc_err_line = __LINE__;                                     \
            return TT_FALSE;                                                   \
        }                                                                      \
    } while (0)

static tt_bool_t __check_doc(tt_xmldoc_t *xdoc)
{
    tt_xnode_t *root = tt_xmldoc_root(xdoc);
    tt_xnode_t *xn, *xn2;

    __chkxdoc(root->type == TT_XNODE_TYPE_DOC);

    // from first to last
    xn = tt_xnode_first_child(root);
    __chkxdoc(xn->type == TT_XNODE_TYPE_PI);
    __chkxdoc(tt_strcmp(xn->name, "xml") == 0);

    xn = tt_xnode_next(xn);
    __chkxdoc(xn->type == TT_XNODE_TYPE_PI);
    __chkxdoc(tt_strcmp(xn->name, "xml-stylesheet") == 0);

    xn = tt_xnode_next(xn);
    __chkxdoc(xn->type == TT_XNODE_TYPE_ELEMENT);
    __chkxdoc(tt_strcmp(xn->name, "catalog") == 0);

    xn = tt_xnode_next(xn);
    __chkxdoc(xn == NULL);

    // from last to first
    xn = tt_xnode_last_child(root);
    __chkxdoc(xn->type == TT_XNODE_TYPE_ELEMENT);
    __chkxdoc(tt_strcmp(xn->name, "catalog") == 0);

    xn = tt_xnode_prev(xn);
    __chkxdoc(xn->type == TT_XNODE_TYPE_PI);
    __chkxdoc(tt_strcmp(xn->name, "xml-stylesheet") == 0);

    xn = tt_xnode_prev(xn);
    __chkxdoc(xn->type == TT_XNODE_TYPE_PI);
    __chkxdoc(tt_strcmp(xn->name, "xml") == 0);

    xn = tt_xnode_prev(xn);
    __chkxdoc(xn == NULL);

    // check catolog node
    xn = tt_xnode_child_byname(root, "catalog");
    __chkxdoc(xn->type == TT_XNODE_TYPE_ELEMENT);
    __chkxdoc(tt_strcmp(xn->name, "catalog") == 0);

    xn2 = tt_xnode_next_byname(xn, "catalog");
    __chkxdoc(xn2 == NULL);

    // node: product, check attr
    xn = tt_xnode_child_byname(xn, "product");
    __chkxdoc(xn->type == TT_XNODE_TYPE_ELEMENT);
    __chkxdoc(tt_strcmp(xn->name, "product") == 0);

    xn2 = tt_xnode_first_attr(xn);
    __chkxdoc(xn2->type == TT_XNODE_TYPE_ATTR);
    __chkxdoc(tt_strcmp(xn2->name, "description") == 0);
    __chkxdoc(tt_strcmp(xn2->value, "Cardigan Sweater") == 0);

    xn2 = tt_xnode_next(xn2);
    __chkxdoc(xn2->type == TT_XNODE_TYPE_ATTR);
    __chkxdoc(tt_strcmp(xn2->name, "product_image") == 0);
    __chkxdoc(tt_strcmp(xn2->value, "cardigan.jpg") == 0);

    xn2 = tt_xnode_next(xn2);
    __chkxdoc(xn2 == NULL);

    xn2 = tt_xnode_attr_byname(xn, "product_image");
    __chkxdoc(xn2->type == TT_XNODE_TYPE_ATTR);
    __chkxdoc(tt_strcmp(xn2->name, "product_image") == 0);
    __chkxdoc(tt_strcmp(xn2->value, "cardigan.jpg") == 0);
    __chkxdoc(tt_xnode_get_text(xn2) == NULL);

    xn2 = tt_xnode_next_byname(xn2, "product_image");
    __chkxdoc(xn2 == NULL);

    // node: product, check text
    __chkxdoc(tt_strcmp(tt_xnode_get_text(xn), "------") == 0);

    xn2 = tt_xnode_last_child(xn);
    __chkxdoc(xn2->type == TT_XNODE_TYPE_ELEMENT);
    __chkxdoc(tt_strcmp(xn2->name, "catalog_item") == 0);
    __chkxdoc(tt_strcmp(tt_xnode_get_attrval(xn2, "gender"), "Women's") == 0);

    xn2 = tt_xnode_prev(xn2);
    __chkxdoc(xn2->type == TT_XNODE_TYPE_TEXT);
    __chkxdoc(tt_strcmp(tt_xnode_get_text(xn2), "//////") == 0);

    xn2 = tt_xnode_prev(xn2);
    __chkxdoc(xn2->type == TT_XNODE_TYPE_ELEMENT);
    __chkxdoc(tt_strcmp(xn2->name, "catalog_item") == 0);
    __chkxdoc(tt_strcmp(tt_xnode_get_attrval(xn2, "gender"), "Men's") == 0);

    xn = xn2;

    xn2 = tt_xnode_prev(xn2);
    __chkxdoc(xn2->type == TT_XNODE_TYPE_TEXT);
    __chkxdoc(tt_strcmp(tt_xnode_get_text(xn2), "------") == 0);

    xn2 = tt_xnode_prev(xn2);
    __chkxdoc(xn2 == NULL);

    // goto deepest
    xn = tt_xnode_child_byname(xn, "size");
    __chkxdoc(xn != NULL);
    __chkxdoc(tt_strcmp(tt_xnode_get_attrval(xn, "description"), "Medium") ==
              0);
    xn = tt_xnode_child_byname(xn, "color_swatch");
    __chkxdoc(xn != NULL);
    xn = tt_xnode_next(xn);
    __chkxdoc(xn != NULL);
    __chkxdoc(tt_strcmp(tt_xnode_get_attrval(xn, "image"),
                        "burgundy_cardigan.jpg") == 0);
    __chkxdoc(tt_strcmp(tt_xnode_get_text(xn), "Burgundy") == 0);

    return TT_TRUE;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_doc_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_xmldoc_t xdoc;
    tt_result_t ret;
    tt_u32_t n, len, i;

    TT_TEST_CASE_ENTER()
    // test start

    ret = tt_xmldoc_create(&xdoc, NULL);
    TT_UT_SUCCESS(ret, "");

    ret = tt_xmldoc_update(&xdoc, __tdb_xml1, sizeof(__tdb_xml1));
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(__check_doc(&xdoc), TT_TRUE, "");

    ret = tt_xmldoc_final(&xdoc, NULL);
    TT_UT_SUCCESS(ret, "");

    // update segments
    {
        tt_xmldoc_reset(&xdoc, 0);

        for (n = 0; n < sizeof((__tdb_xml1));) {
            len = sizeof(__tdb_xml1) - n;
            if (len > 5) {
                len = tt_rand_u32() % 5; // 0-4
            }
            ret = tt_xmldoc_update(&xdoc, (tt_u8_t *)&__tdb_xml1[n], len);
            TT_UT_SUCCESS(ret, "");
            n += len;
        }
        TT_ASSERT(n == sizeof(__tdb_xml1));

        ret = tt_xmldoc_final(&xdoc, NULL);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(__check_doc(&xdoc), TT_TRUE, "");
    }

    // random test
    {
        tt_xmldoc_reset(&xdoc, 0);

        for (i = 0; i < sizeof((__tdb_xml1));) {
            tt_u32_t len = sizeof(__tdb_xml1) - i, j;
            tt_u8_t buf[5];
            if (len > 5) {
                len = tt_rand_u32() % 5; // 0-9
            }

            tt_memcpy(buf, &__tdb_xml1[i], len);
            for (j = 0; j < len; ++j) {
                __tdb_xml1[i + j] = (tt_u8_t)tt_rand_u32();
            }
            tt_xmldoc_update(&xdoc, (tt_u8_t *)__tdb_xml1, sizeof(__tdb_xml1));
            tt_memcpy(&__tdb_xml1[i], buf, len);

            i += len;
        }
    }

    tt_xmldoc_destroy(&xdoc);

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_u32_t __utdr_err_line;

static tt_bool_t __utdr_1(tt_xmldoc_t *xdoc, tt_xmlnr_t *xnr)
{
    tt_u8_t utdr_in[] =
        "text1\n"
        "  <tag1/>\n";
    tt_u8_t utdr_out1[] =
        "<tag1 a2=\"v2\" a3=\"v3\" a4=\"v4\" a1=\"\" a5=\"v5\"/>\n";
    tt_u8_t utdr_out2[] =
        "<tag1 a2=\"v2\" a3=\"v3\" a4=\"v4\" a1=\"\" a5=\"v5\">text1</tag1>\n";
    tt_u8_t utdr_out3[] =
        "<tag1 a2=\"v2\" a3=\"v3\" a4=\"v4\" a1=\"\" a5=\"v5\"></tag1>\n";
    tt_u8_t utdr_out4[] = "<tag1 a3=\"v3\" a1=\"\"></tag1>\n";

    tt_u8_t *ut_out;
    tt_u32_t ut_out_len;

    tt_xnode_t *r2, *tmp;
    tt_result_t ret;

    tt_xmldoc_reset(xdoc, 0);
    tt_xmlnr_reset(xnr, 0);

    if (!TT_OK(tt_xmldoc_update(xdoc, utdr_in, sizeof(utdr_in)))) {
        __utdr_err_line = __LINE__;
        return TT_FALSE;
    }
    if (!TT_OK(tt_xmldoc_final(xdoc, NULL))) {
        __utdr_err_line = __LINE__;
        return TT_FALSE;
    }

    r2 = tt_xnode_first_child(xdoc->root);
    r2 = tt_xnode_next(r2);
    if (r2 == NULL || (r2->type != TT_XNODE_TYPE_ELEMENT)) {
        __utdr_err_line = __LINE__;
        return TT_FALSE;
    }

    // attributes
    if (tt_xnode_attr_num(r2) != 0) {
        __utdr_err_line = __LINE__;
        return TT_FALSE;
    }

    ret = tt_xnode_addtail_attr(r2, tt_xnode_attr_create(&xdoc->xm, "a1", ""));
    if (!TT_OK(ret)) {
        __utdr_err_line = __LINE__;
        return TT_FALSE;
    }

    ret =
        tt_xnode_addhead_attr(r2, tt_xnode_attr_create(&xdoc->xm, "a2", "v2"));
    if (!TT_OK(ret)) {
        __utdr_err_line = __LINE__;
        return TT_FALSE;
    }

    tmp = tt_xnode_attr_byname(r2, "a1");
    if (tmp == NULL) {
        __utdr_err_line = __LINE__;
        return TT_FALSE;
    }
    tt_xnode_insert_prev(tmp, tt_xnode_attr_create(&xdoc->xm, "a3", "v3"));

    tmp = tt_xnode_attr_byname(r2, "a3");
    if (tmp == NULL) {
        __utdr_err_line = __LINE__;
        return TT_FALSE;
    }
    tt_xnode_insert_next(tmp, tt_xnode_attr_create(&xdoc->xm, "a4", "v4"));
    // a2, a3, a4, a1

    if (!TT_OK(tt_xnode_set_attrval(r2, "a5", "v5"))) {
        __utdr_err_line = __LINE__;
        return TT_FALSE;
    }

    if (!TT_OK(tt_xmlnr_render(xnr, r2))) {
        __utdr_err_line = __LINE__;
        return TT_FALSE;
    }
    tt_xmlnr_data(xnr, &ut_out, &ut_out_len);
    if (ut_out_len != sizeof(utdr_out1) - 1 ||
        tt_memcmp(ut_out, utdr_out1, ut_out_len) != 0) {
        __utdr_err_line = __LINE__;
        return TT_FALSE;
    }

    // text
    if (tt_xnode_get_text(r2) != NULL) {
        __utdr_err_line = __LINE__;
        return TT_FALSE;
    }

    tt_xnode_set_text(r2, "text1");

    tt_xmlnr_reset(xnr, 0);
    if (!TT_OK(tt_xmlnr_render(xnr, r2))) {
        __utdr_err_line = __LINE__;
        return TT_FALSE;
    }
    tt_xmlnr_data(xnr, &ut_out, &ut_out_len);
    if (ut_out_len != sizeof(utdr_out2) - 1 ||
        tt_memcmp(ut_out, utdr_out2, ut_out_len) != 0) {
        __utdr_err_line = __LINE__;
        return TT_FALSE;
    }

    tt_xnode_set_text(r2, "");

    tt_xmlnr_reset(xnr, 0);
    if (!TT_OK(tt_xmlnr_render(xnr, r2))) {
        __utdr_err_line = __LINE__;
        return TT_FALSE;
    }
    tt_xmlnr_data(xnr, &ut_out, &ut_out_len);
    if (ut_out_len != sizeof(utdr_out3) - 1 ||
        tt_memcmp(ut_out, utdr_out3, ut_out_len) != 0) {
        __utdr_err_line = __LINE__;
        return TT_FALSE;
    }

    tt_xnode_remove_attr(r2, "a5");
    tt_xnode_remove_attr(r2, "a4");
    tt_xnode_remove_attr(r2, "a2");
    tt_xmlnr_reset(xnr, 0);
    if (!TT_OK(tt_xmlnr_render(xnr, r2))) {
        __utdr_err_line = __LINE__;
        return TT_FALSE;
    }
    tt_xmlnr_data(xnr, &ut_out, &ut_out_len);
    if (ut_out_len != sizeof(utdr_out4) - 1 ||
        tt_memcmp(ut_out, utdr_out4, ut_out_len) != 0) {
        __utdr_err_line = __LINE__;
        return TT_FALSE;
    }

    return TT_TRUE;
}

static tt_bool_t __utdr_2(tt_xmldoc_t *xdoc, tt_xmlnr_t *xnr)
{
    tt_u8_t utdr_in[] =
        "text1\n"
        "  <tag1/>\n";
    tt_u8_t utdr_out1[] =
        "<tag1>\n  <c2/>\n  <c1>\nc1 text1\n  </c1>\n  <c3/>\nroot "
        "text1\n</tag1>\n";
    tt_u8_t utdr_out2[] =
        "<tag1>\n  <c1>\nc1 text1\n  </c1>\nroot text1\n</tag1>\n";

    tt_u8_t *ut_out;
    tt_u32_t ut_out_len;

    tt_xnode_t *r2, *tmp;

    tt_xmldoc_reset(xdoc, 0);
    tt_xmlnr_reset(xnr, 0);

    if (!TT_OK(tt_xmldoc_update(xdoc, utdr_in, sizeof(utdr_in)))) {
        __utdr_err_line = __LINE__;
        return TT_FALSE;
    }
    if (!TT_OK(tt_xmldoc_final(xdoc, NULL))) {
        __utdr_err_line = __LINE__;
        return TT_FALSE;
    }

    r2 = tt_xnode_first_child(xdoc->root);
    r2 = tt_xnode_next(r2);
    if (r2 == NULL || (r2->type != TT_XNODE_TYPE_ELEMENT)) {
        __utdr_err_line = __LINE__;
        return TT_FALSE;
    }

    tmp = tt_xnode_elmt_create(&xdoc->xm, "c1");
    tt_xnode_set_text(tmp, "");
    tt_xnode_add_text(tmp, "c1 text1\n");
    tt_xnode_addtail_child(r2, tmp);

    tmp = tt_xnode_elmt_create(&xdoc->xm, "c2");
    tt_xnode_addhead_child(r2, tmp);

    tmp = tt_xnode_elmt_create(&xdoc->xm, "c3");
    tt_xnode_addtail_child(r2, tmp);

    tt_xnode_add_text(r2, "root text1\n");

    tt_xmlnr_reset(xnr, 0);
    if (!TT_OK(tt_xmlnr_render(xnr, r2))) {
        __utdr_err_line = __LINE__;
        return TT_FALSE;
    }
    tt_xmlnr_data(xnr, &ut_out, &ut_out_len);
    if (ut_out_len != sizeof(utdr_out1) - 1 ||
        tt_memcmp(ut_out, utdr_out1, ut_out_len) != 0) {
        __utdr_err_line = __LINE__;
        return TT_FALSE;
    }

    // remove
    tt_xnode_destroy(tt_xnode_remove_child(r2, "c3"));
    tt_xnode_destroy(tt_xnode_remove_child(r2, "c2"));
    tt_xmlnr_reset(xnr, 0);
    if (!TT_OK(tt_xmlnr_render(xnr, r2))) {
        __utdr_err_line = __LINE__;
        return TT_FALSE;
    }
    tt_xmlnr_data(xnr, &ut_out, &ut_out_len);
    if (ut_out_len != sizeof(utdr_out2) - 1 ||
        tt_memcmp(ut_out, utdr_out2, ut_out_len) != 0) {
        __utdr_err_line = __LINE__;
        return TT_FALSE;
    }

    return TT_TRUE;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_doc_render)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_xmldoc_t xdoc;
    tt_result_t ret;
    tt_xmlnr_t xnr;
    tt_bool_t b_ret;

    TT_TEST_CASE_ENTER()
    // test start

    ret = tt_xmldoc_create(&xdoc, NULL);
    TT_UT_SUCCESS(ret, "");

    ret = tt_xmlnr_create(&xnr, NULL);
    TT_UT_SUCCESS(ret, "");

    {
        tt_xnode_t *r2;

        tt_xmldoc_reset(&xdoc, 0);
        tt_xmlnr_reset(&xnr, 0);

        ret = tt_xmldoc_update(&xdoc, __tdb_xml1, sizeof(__tdb_xml1));
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(__check_doc(&xdoc), TT_TRUE, "");

        ret = tt_xmldoc_final(&xdoc, NULL);
        TT_UT_SUCCESS(ret, "");

        r2 = tt_xnode_clone(xdoc.root);
        TT_UT_NOT_EQUAL(r2, NULL, "");

        ret = tt_xmlnr_render(&xnr, xdoc.root);
        TT_UT_SUCCESS(ret, "");
    }

    b_ret = __utdr_1(&xdoc, &xnr);
    TT_UT_EQUAL(b_ret, TT_TRUE, "");

    b_ret = __utdr_2(&xdoc, &xnr);
    TT_UT_EQUAL(b_ret, TT_TRUE, "");

    tt_xmldoc_destroy(&xdoc);
    tt_xmlnr_destroy(&xnr);

    // test end
    TT_TEST_CASE_LEAVE()
}
