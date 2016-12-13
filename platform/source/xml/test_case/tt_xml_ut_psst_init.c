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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_psst_init_utf8)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_psst_init_utf16le)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_psst_init_utf16be)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_psst_init_utf32le)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_psst_init_utf32be)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(xml_psst_case)

TT_TEST_CASE("tt_unit_test_psst_init_utf8",
             "xml parse state: init, utf8 encoding",
             tt_unit_test_psst_init_utf8,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_psst_init_utf16le",
                 "xml parse state: init, utf16le encoding",
                 tt_unit_test_psst_init_utf16le,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_psst_init_utf16be",
                 "xml parse state: init, utf16be encoding",
                 tt_unit_test_psst_init_utf16be,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_psst_init_utf32le",
                 "xml parse state: init, utf32le encoding",
                 tt_unit_test_psst_init_utf32le,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_psst_init_utf32be",
                 "xml parse state: init, utf32be encoding",
                 tt_unit_test_psst_init_utf32be,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(xml_psst_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(XML_UT_PSST_INIT, 0, xml_psst_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(tt_unit_test_psst_init_utf8)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(tt_unit_test_psst_init_utf8)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_xmlparser_t xp;
    tt_result_t ret;
    tt_xmlmem_t xm;

    TT_TEST_CASE_ENTER()
    // test start

    ret = tt_xmlmem_create(&xm, NULL);
    TT_TEST_CHECK_SUCCESS(ret, "");

    // utf8
    {
        tt_u8_t b1[] = {0xef, 0xbb, 0xbf, 0x9, 0x9, 0x9};

        ret = tt_xmlparser_create(&xp, &xm, NULL, NULL);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        ret = tt_xmlparser_update(&xp, b1, 1);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_xmlparser_update(&xp, b1 + 1, 1);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_xmlparser_update(&xp, b1 + 2, 1);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_xmlparser_update(&xp, b1 + 3, 3);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        TT_TEST_CHECK_EQUAL(xp.from_encoding, TT_CHARSET_UTF8, "");

        tt_xmlparser_destroy(&xp);
    }

    // invalid mark, then all data must be in buf of csconv
    {
        tt_u8_t b1[] = {0x1, 0x2, 0x3, 0x4};
        tt_u8_t outb[sizeof(b1)] = {0};
        tt_u32_t outn = sizeof(outb);

        ret = tt_xmlparser_create(&xp, &xm, NULL, NULL);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        ret = tt_xmlparser_update(&xp, b1, 2);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_xmlparser_update(&xp, b1 + 2, 2);

        TT_TEST_CHECK_EQUAL(xp.from_encoding, TT_CHARSET_UTF8, "");

        tt_chsetconv_output(&xp.csconv, outb, &outn);
        TT_TEST_CHECK_EQUAL(tt_memcmp(outb, b1, outn), 0, "");

        tt_xmlparser_destroy(&xp);
    }

    tt_xmlmem_destroy(&xm);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_psst_init_utf16le)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_xmlparser_t xp;
    tt_result_t ret;
    tt_xmlmem_t xm;

    TT_TEST_CASE_ENTER()
    // test start

    ret = tt_xmlmem_create(&xm, NULL);
    TT_TEST_CHECK_SUCCESS(ret, "");

    // utf16le
    {
        tt_u8_t b1[] = {0xff, 0xfe, 0x9, 0x0, 0x9, 0x0, 0x9, 0x0};

        ret = tt_xmlparser_create(&xp, &xm, NULL, NULL);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        ret = tt_xmlparser_update(&xp, b1, 1);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_xmlparser_update(&xp, b1 + 1, 2);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_xmlparser_update(&xp, b1 + 3, 2);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        TT_TEST_CHECK_EQUAL(xp.from_encoding, TT_CHARSET_UTF16LE, "");

        tt_xmlparser_destroy(&xp);
    }

    tt_xmlmem_destroy(&xm);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_psst_init_utf16be)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_xmlparser_t xp;
    tt_result_t ret;
    tt_xmlmem_t xm;

    TT_TEST_CASE_ENTER()
    // test start

    ret = tt_xmlmem_create(&xm, NULL);
    TT_TEST_CHECK_SUCCESS(ret, "");

    // utf16be
    {
        tt_u8_t b1[] = {0xfe, 0xff, 0x0, 0x9, 0x0, 0x9, 0x0, 0x9};

        ret = tt_xmlparser_create(&xp, &xm, NULL, NULL);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        ret = tt_xmlparser_update(&xp, b1, 1);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_xmlparser_update(&xp, b1 + 1, 4);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        TT_TEST_CHECK_EQUAL(xp.from_encoding, TT_CHARSET_UTF16BE, "");

        tt_xmlparser_destroy(&xp);
    }

    tt_xmlmem_destroy(&xm);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_psst_init_utf32le)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_xmlparser_t xp;
    tt_result_t ret;
    tt_xmlmem_t xm;

    TT_TEST_CASE_ENTER()
    // test start

    ret = tt_xmlmem_create(&xm, NULL);
    TT_TEST_CHECK_SUCCESS(ret, "");

    // utf32le
    {
        tt_u8_t b1[] = {0xff, 0xfe, 0, 0, 0x9, 0, 0, 0};

        ret = tt_xmlparser_create(&xp, &xm, NULL, NULL);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        ret = tt_xmlparser_update(&xp, b1, 3);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_xmlparser_update(&xp, b1 + 3, 1);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_xmlparser_update(&xp, b1 + 4, 4);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        TT_TEST_CHECK_EQUAL(xp.from_encoding, TT_CHARSET_UTF32LE, "");

        tt_xmlparser_destroy(&xp);
    }

    tt_xmlmem_destroy(&xm);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_psst_init_utf32be)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_xmlparser_t xp;
    tt_result_t ret;
    tt_xmlmem_t xm;

    TT_TEST_CASE_ENTER()
    // test start

    ret = tt_xmlmem_create(&xm, NULL);
    TT_TEST_CHECK_SUCCESS(ret, "");

    // utf32be
    {
        tt_u8_t b1[] = {0, 0, 0xfe, 0xff, 0, 0, 0, 0x9};

        ret = tt_xmlparser_create(&xp, &xm, NULL, NULL);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        ret = tt_xmlparser_update(&xp, b1, 1);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_xmlparser_update(&xp, b1 + 1, 2);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        ret = tt_xmlparser_update(&xp, b1 + 3, 5);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        TT_TEST_CHECK_EQUAL(xp.from_encoding, TT_CHARSET_UTF32BE, "");

        tt_xmlparser_destroy(&xp);
    }

    tt_xmlmem_destroy(&xm);

    // test end
    TT_TEST_CASE_LEAVE()
}
