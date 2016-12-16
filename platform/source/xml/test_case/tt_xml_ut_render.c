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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_xml_ch_enc)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_xmlrd_tag)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_xmlrd_other)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(xrdr_case)

TT_TEST_CASE("tt_unit_test_xml_ch_enc",
             "xml char encode",
             tt_unit_test_xml_ch_enc,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_xmlrd_tag",
                 "xml render: tag",
                 tt_unit_test_xmlrd_tag,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_xmlrd_other",
                 "xml render: pi/comment/cdata",
                 tt_unit_test_xmlrd_other,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(xrdr_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(XML_UT_RENDER, 0, xrdr_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
     TT_TEST_ROUTINE_DEFINE(tt_unit_test_xmlrd_other)
     {
     //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
     tt_xmlrender_t xr;
     tt_result_t ret;

     TT_TEST_CASE_ENTER()
     // test start

     ret = tt_xmlrender_create(&xr, NULL);
     TT_TEST_CHECK_SUCCESS(ret, "");

     tt_xmlrender_destroy(&xr);

     // test end
     TT_TEST_CASE_LEAVE()
     }
     */

    TT_TEST_ROUTINE_DEFINE(tt_unit_test_xml_ch_enc)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    tt_buf_t buf;
    tt_s32_t cmp_ret;

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&buf, NULL);

    tt_buf_clear(&buf);
    ret = tt_xml_chenc("\"'<>&", &buf);
    TT_TEST_CHECK_SUCCESS(ret, "");
    cmp_ret = tt_buf_cmp_cstr(&buf, "&quot;&apos;&lt;&gt;&amp;");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    tt_buf_clear(&buf);
    ret = tt_xml_chenc("", &buf);
    TT_TEST_CHECK_SUCCESS(ret, "");
    cmp_ret = tt_buf_cmp_cstr(&buf, "");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    tt_buf_clear(&buf);
    ret = tt_xml_chenc("1\"22'333<4444>&55555", &buf);
    TT_TEST_CHECK_SUCCESS(ret, "");
    cmp_ret = tt_buf_cmp_cstr(&buf, "1&quot;22&apos;333&lt;4444&gt;&amp;55555");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    tt_buf_destroy(&buf);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_xmlrd_tag)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_xmlrender_t xr;
    tt_result_t ret;
    tt_u32_t cmp_ret;

    TT_TEST_CASE_ENTER()
    // test start

    ret = tt_xmlrender_create(&xr, NULL);
    TT_TEST_CHECK_SUCCESS(ret, "");

    // a xml tag with attr
    {
        tt_xmlrender_reset(&xr, 0);

        ret = tt_xmlrender_text(&xr, "text1\"'<>&");
        TT_TEST_CHECK_SUCCESS(ret, "");

        ret = tt_xmlrender_stag(&xr, "");
        TT_TEST_CHECK_FAIL(ret, "");

        ret = tt_xmlrender_stag(&xr, "ele1");
        TT_TEST_CHECK_SUCCESS(ret, "");

        ret = tt_xmlrender_attr(&xr, "attr1", "val1");
        TT_TEST_CHECK_SUCCESS(ret, "");

        ret = tt_xmlrender_attr(&xr, "", "val2");
        TT_TEST_CHECK_FAIL(ret, "");

        ret = tt_xmlrender_attr(&xr, "attr3", "");
        TT_TEST_CHECK_SUCCESS(ret, "");

        ret = tt_xmlrender_stag_end(&xr);
        TT_TEST_CHECK_SUCCESS(ret, "");

        cmp_ret = tt_buf_cmp_cstr(&xr.buf,
                                  "text1&quot;&apos;&lt;&gt;&amp;<ele1 "
                                  "attr1=\"val1\" attr3=\"\"/>\n");
        TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    }

    // indent elements
    {
        const tt_char_t *__xr =
            "<el&amp;e1>\n"
            "  <sub_ele&amp; "
            "&quot;attr1&apos;=\"&lt;val1&gt;\">content&quot;&apos;&lt;&gt;&"
            "apos;&quot;</sub_ele&amp;>\n"
            "</el&amp;e1>\n";

        tt_xmlrender_reset(&xr, 0);

        ret = tt_xmlrender_stag(&xr, "el&e1");
        TT_TEST_CHECK_SUCCESS(ret, "");

        ret = tt_xmlrender_stag_complete(&xr, TT_TRUE);
        TT_TEST_CHECK_SUCCESS(ret, "");

        ret = tt_xmlrender_stag(&xr, "sub_ele&");
        TT_TEST_CHECK_SUCCESS(ret, "");

        ret = tt_xmlrender_attr(&xr, "\"attr1'", "<val1>");
        TT_TEST_CHECK_SUCCESS(ret, "");

        ret = tt_xmlrender_stag_complete(&xr, TT_FALSE);
        TT_TEST_CHECK_SUCCESS(ret, "");

        ret = tt_xmlrender_text(&xr, "content\"'<>'\"");
        TT_TEST_CHECK_SUCCESS(ret, "");

        ret = tt_xmlrender_etag(&xr, "sub_ele&");
        TT_TEST_CHECK_SUCCESS(ret, "");

        ret = tt_xmlrender_etag(&xr, "el&e1");
        TT_TEST_CHECK_SUCCESS(ret, "");

        cmp_ret = tt_buf_cmp_cstr(&xr.buf, __xr);
        TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    }

    tt_xmlrender_destroy(&xr);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_xmlrd_other)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_xmlrender_t xr;
    tt_result_t ret;
    tt_s32_t cmp_ret;

    TT_TEST_CASE_ENTER()
    // test start

    ret = tt_xmlrender_create(&xr, NULL);
    TT_TEST_CHECK_SUCCESS(ret, "");

    // pi
    {
        tt_xmlrender_reset(&xr, 0);

        ret = tt_xmlrender_pi(&xr, "", "");
        TT_TEST_CHECK_FAIL(ret, "");

        ret = tt_xmlrender_pi(&xr, "xml", "");
        TT_TEST_CHECK_SUCCESS(ret, "");
        cmp_ret = tt_buf_cmp_cstr(&xr.buf, "<?xml?>\n");
        TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

        ret = tt_xmlrender_pi(&xr, "pi2", "pi2 content \"'<>&");
        TT_TEST_CHECK_SUCCESS(ret, "");
        cmp_ret =
            tt_buf_cmp_cstr(&xr.buf, "<?xml?>\n<?pi2 pi2 content \"'<>&?>\n");
        TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    }

    // comment
    {
        tt_xmlrender_reset(&xr, 0);

        ret = tt_xmlrender_comment(&xr, "");
        TT_TEST_CHECK_SUCCESS(ret, "");
        cmp_ret = tt_buf_cmp_cstr(&xr.buf, "<!--  -->\n");
        TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

        ret = tt_xmlrender_comment(&xr, "\"'<>&content ");
        TT_TEST_CHECK_SUCCESS(ret, "");
        cmp_ret =
            tt_buf_cmp_cstr(&xr.buf, "<!--  -->\n<!-- \"'<>&content  -->\n");
        TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    }

    // cdata
    {
        tt_xmlrender_reset(&xr, 0);

        ret = tt_xmlrender_cdata(&xr, "");
        TT_TEST_CHECK_SUCCESS(ret, "");
        cmp_ret = tt_buf_cmp_cstr(&xr.buf, "<![CDATA[]]>\n");
        TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

        ret = tt_xmlrender_cdata(&xr, "\"'<>&content ");
        TT_TEST_CHECK_SUCCESS(ret, "");
        cmp_ret = tt_buf_cmp_cstr(&xr.buf,
                                  "<![CDATA[]]>\n<![CDATA[\"'<>&content ]]>\n");
        TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    }

    tt_xmlrender_destroy(&xr);

    // test end
    TT_TEST_CASE_LEAVE()
}
