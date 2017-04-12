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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_xp_basic)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_xp_content)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_xp_tag)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_xp_attr)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_xp_pi)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_xp_comment)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_xp_cdata)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_xp_decode)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_xp_doctype)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(xp_case)

TT_TEST_CASE("tt_unit_test_xp_basic",
             "xml parse: basic test",
             tt_unit_test_xp_basic,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_xp_content",
                 "xml parse: content",
                 tt_unit_test_xp_content,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_xp_tag",
                 "xml parse: tag",
                 tt_unit_test_xp_tag,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_xp_attr",
                 "xml parse: attribute",
                 tt_unit_test_xp_attr,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_xp_pi",
                 "xml parse: processing instruction",
                 tt_unit_test_xp_pi,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_xp_comment",
                 "xml parse: comment",
                 tt_unit_test_xp_comment,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_xp_cdata",
                 "xml parse: cdata",
                 tt_unit_test_xp_cdata,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_xp_decode",
                 "xml parse: char decoding",
                 tt_unit_test_xp_decode,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_xp_doctype",
                 "xml parse: doctype",
                 tt_unit_test_xp_doctype,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(xp_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(XML_UT_PSST_PARSE, 0, xp_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(tt_unit_test_xp_doctype)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    tt_result_t __just_print(IN struct tt_xmlparser_s *parser,
                             IN tt_u8_t *data,
                             IN tt_u32_t data_len)
{
    tt_char_t buf[100] = {0};

    memcpy(buf, data, TT_MIN(data_len, sizeof(buf) - 1));
    TT_INFO("%s", buf);

    return TT_SUCCESS;
}

tt_result_t __just_print_st(IN struct tt_xmlparser_s *parser)
{
    return TT_SUCCESS;
}

tt_result_t __just_print_nv(IN struct tt_xmlparser_s *parser,
                            IN tt_u8_t *name,
                            IN tt_u32_t name_len,
                            IN tt_u8_t *value,
                            IN tt_u32_t value_len)
{
    tt_char_t buf[100] = {0};
    tt_char_t nbuf[100] = {0};
    tt_char_t vbuf[100] = {0};

    tt_memcpy(nbuf, name, TT_MIN(name_len, sizeof(nbuf) - 1));
    tt_memcpy(vbuf, value, TT_MIN(value_len, sizeof(vbuf) - 1));
    tt_snprintf(buf, sizeof(buf) - 1, "%s=%s", nbuf, vbuf);
    TT_INFO("%s", buf);

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_xp_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    char b1[] = "<node1 attr1=\"v1\">text1</node1>";
    tt_xmlparser_t xp;
    tt_xmlparser_cb_t cb = {0};
    tt_result_t ret;

    TT_TEST_CASE_ENTER()
    // test start

    cb.on_text = __just_print;
    cb.on_start_tag = __just_print;
    cb.on_start_tag_end = __just_print_st;
    cb.on_end_tag = __just_print;
    cb.on_attr = __just_print_nv;

    ret = tt_xmlparser_create(&xp, NULL, &cb, NULL);
    TT_UT_SUCCESS(ret, "");

    // can update 0 data
    ret = tt_xmlparser_update(&xp, (tt_u8_t *)b1, 0);
    TT_UT_SUCCESS(ret, "");

    ret = tt_xmlparser_update(&xp, (tt_u8_t *)b1, sizeof(b1));
    TT_UT_SUCCESS(ret, "");

    tt_xmlparser_destroy(&xp);

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_u32_t __seq;
static tt_u32_t __err_line;

tt_result_t __xp2_on_content(IN struct tt_xmlparser_s *parser,
                             IN tt_u8_t *value,
                             IN tt_u32_t value_len)
{
    if (__seq == 0) {
        if ((value_len != sizeof("    test content\n\r\n ") - 1) ||
            tt_strncmp((tt_char_t *)value,
                       "    test content\n\r\n ",
                       value_len) != 0) {
            __err_line = __LINE__;
            return TT_FAIL;
        }
    } else if (__seq == 1) {
        if ((value_len != sizeof(" t2t2t2t2\r    t3    \n") - 1) ||
            tt_strncmp((tt_char_t *)value,
                       " t2t2t2t2\r    t3    \n",
                       value_len) != 0) {
            __err_line = __LINE__;
            return TT_FAIL;
        }
    } else if (__seq == 2) {
        if ((value_len != sizeof(" ending \r\n\r\n 123 ")) ||
            tt_strncmp((tt_char_t *)value,
                       " ending \r\n\r\n 123 ",
                       value_len) != 0) {
            __err_line = __LINE__;
            return TT_FAIL;
        }
    } else {
        __err_line = __LINE__;
        return TT_FAIL;
    }

    ++__seq;
    return TT_SUCCESS;
}

tt_result_t __xp2_no_content(IN struct tt_xmlparser_s *parser,
                             IN tt_u8_t *value,
                             IN tt_u32_t value_len)
{
    __err_line = __LINE__;
    return TT_FAIL;
}

tt_result_t __xp2_on_content_123(IN struct tt_xmlparser_s *parser,
                                 IN tt_u8_t *value,
                                 IN tt_u32_t value_len)
{
    if ((value_len != sizeof("123") - 1) ||
        tt_strncmp((char *)value, "123", value_len) != 0) {
        __err_line = __LINE__;
        return TT_FAIL;
    }

    return TT_FAIL;
}

tt_result_t __xp2_on_content_trim(IN struct tt_xmlparser_s *parser,
                                  IN tt_u8_t *value,
                                  IN tt_u32_t value_len)
{
    if (__seq == 0) {
        if ((value_len != sizeof("test content") - 1) ||
            tt_strncmp((tt_char_t *)value, "test content", value_len) != 0) {
            __err_line = __LINE__;
            return TT_FAIL;
        }
    } else if (__seq == 1) {
        if ((value_len != sizeof("ending \r\n\r\n 123") - 1) ||
            tt_strncmp((tt_char_t *)value, "ending \r\n\r\n 123", value_len) !=
                0) {
            __err_line = __LINE__;
            return TT_FAIL;
        }
    } else {
        __err_line = __LINE__;
        return TT_FAIL;
    }

    ++__seq;
    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_xp_content)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    char b1[] =
        "    test content\n"
        "\r\n"
        " <node> t2t2t2t2\r"
        "    t3    \n</node> ending "
        "\r\n\r\n 123 ";
    char b2[] =
        "    test content\n"
        "\r\n"
        " <node> \r\n\t\n  </node> ending "
        "\r\n\r\n 123 \r\n";

    tt_xmlparser_t xp;
    tt_xmlparser_cb_t cb = {0};
    tt_result_t ret;
    tt_u32_t n, len;
    tt_xmlparser_attr_t attr;

    TT_TEST_CASE_ENTER()
    // test start

    cb.on_text = __xp2_on_content;

    tt_xmlparser_attr_default(&attr);
    attr.trim_text = TT_FALSE;

    ret = tt_xmlparser_create(&xp, NULL, &cb, &attr);
    TT_UT_SUCCESS(ret, "");

    //////////////////////
    // 1. content
    __seq = 0;
    __err_line = 0;

    for (n = 0; n < sizeof((b1));) {
        len = sizeof(b1) - n;
        if (len > 10) {
            len = tt_rand_u32() % 10; // 0-9
        }
        ret = tt_xmlparser_update(&xp, (tt_u8_t *)&b1[n], len);
        TT_UT_SUCCESS(ret, "");
        n += len;
    }
    TT_ASSERT(n == sizeof(b1));

    ret = tt_xmlparser_final(&xp, NULL);
    TT_UT_SUCCESS(ret, "");

    TT_UT_EQUAL(__seq, 3, "");
    TT_UT_EQUAL(__err_line, 0, "");

    //////////////////////
    // 2. empty content
    __seq = 0;
    __err_line = 0;

    tt_xmlparser_reset(&xp, 0);

    // hack it, there should be no content callback
    xp.cb.on_text = __xp2_no_content;

    ret = tt_xmlparser_update(&xp, (tt_u8_t *)"", 0);
    TT_UT_SUCCESS(ret, "");

    ret = tt_xmlparser_final(&xp, NULL);
    TT_UT_SUCCESS(ret, "");

    TT_UT_EQUAL(__err_line, 0, "");

    //////////////////////
    // 3. short content
    __seq = 0;
    __err_line = 0;

    tt_xmlparser_reset(&xp, 0);

    // hack it
    xp.cb.on_text = __xp2_on_content_123;

    ret = tt_xmlparser_update(&xp, (tt_u8_t *)"123", 3);
    TT_UT_SUCCESS(ret, "");

    ret = tt_xmlparser_final(&xp, NULL);
    TT_UT_SUCCESS(ret, "");

    TT_UT_EQUAL(__err_line, 0, "");

    tt_xmlparser_destroy(&xp);

    //////////////////////
    // trim text
    tt_xmlparser_attr_default(&attr);
    attr.trim_text = TT_TRUE;

    cb.on_text = __xp2_on_content_trim;

    ret = tt_xmlparser_create(&xp, NULL, &cb, &attr);
    TT_UT_SUCCESS(ret, "");

    __seq = 0;
    __err_line = 0;

    for (n = 0; n < sizeof((b2)) - 1;) {
        len = sizeof(b2) - 1 - n;
        if (len > 10) {
            len = tt_rand_u32() % 10; // 0-9
        }
        ret = tt_xmlparser_update(&xp, (tt_u8_t *)&b2[n], len);
        TT_UT_SUCCESS(ret, "");
        n += len;
    }
    TT_ASSERT(n == sizeof(b2) - 1);

    ret = tt_xmlparser_final(&xp, NULL);
    TT_UT_SUCCESS(ret, "");

    tt_xmlparser_destroy(&xp);

    // test end
    TT_TEST_CASE_LEAVE()
}

tt_result_t __xp3_on_stag1(IN struct tt_xmlparser_s *parser,
                           IN tt_u8_t *value,
                           IN tt_u32_t value_len)
{
    if (__seq == 0) {
        if ((value_len != sizeof("tag") - 1) ||
            tt_strncmp((tt_char_t *)value, "tag", value_len) != 0) {
            __err_line = __LINE__;
            return TT_FAIL;
        }
    } else {
        __err_line = __LINE__;
        return TT_FAIL;
    }

    ++__seq;
    return TT_SUCCESS;
}

static tt_s32_t __stag_num;

tt_result_t __xp3_on_etag1(IN struct tt_xmlparser_s *parser,
                           IN tt_u8_t *value,
                           IN tt_u32_t value_len)
{
    --__stag_num;

    if (__seq == 1) {
        if ((value_len != sizeof("tag") - 1) ||
            tt_strncmp((tt_char_t *)value, "tag", value_len) != 0) {
            __err_line = __LINE__;
            return TT_FAIL;
        }
    } else {
        __err_line = __LINE__;
        return TT_FAIL;
    }

    ++__seq;
    return TT_SUCCESS;
}

tt_result_t __xp3_on_stag1_end(IN struct tt_xmlparser_s *parser)
{
    __err_line = 0;
    return TT_SUCCESS;
}

tt_result_t __xp3_on_stag1_comp(IN struct tt_xmlparser_s *parser)
{
    ++__stag_num;

    __err_line = 0;
    return TT_SUCCESS;
}

tt_result_t __xp3_on_stag2(IN struct tt_xmlparser_s *parser,
                           IN tt_u8_t *value,
                           IN tt_u32_t value_len)
{
    if (__seq == 0) {
        if ((value_len != sizeof("tag1") - 1) ||
            tt_strncmp((tt_char_t *)value, "tag1", value_len) != 0) {
            __err_line = __LINE__;
            return TT_FAIL;
        }
    } else if (__seq == 1) {
        if ((value_len != sizeof("tag2") - 1) ||
            tt_strncmp((tt_char_t *)value, "tag2", value_len) != 0) {
            __err_line = __LINE__;
            return TT_FAIL;
        }
    } else if (__seq == 2) {
        if ((value_len != sizeof("tag3") - 1) ||
            tt_strncmp((tt_char_t *)value, "tag3", value_len) != 0) {
            __err_line = __LINE__;
            return TT_FAIL;
        }
    } else if (__seq == 4) {
        if ((value_len != sizeof("tag4") - 1) ||
            tt_strncmp((tt_char_t *)value, "tag4", value_len) != 0) {
            __err_line = __LINE__;
            return TT_FAIL;
        }
    } else {
        __err_line = __LINE__;
        return TT_FAIL;
    }

    ++__seq;
    return TT_SUCCESS;
}

tt_result_t __xp3_on_etag2(IN struct tt_xmlparser_s *parser,
                           IN tt_u8_t *value,
                           IN tt_u32_t value_len)
{
    if (__seq == 6) {
        if ((value_len != sizeof("tag2") - 1) ||
            tt_strncmp((tt_char_t *)value, "tag2", value_len) != 0) {
            __err_line = __LINE__;
            return TT_FAIL;
        }
    } else if (__seq == 7) {
        if ((value_len != sizeof("tag1") - 1) ||
            tt_strncmp((tt_char_t *)value, "tag1", value_len) != 0) {
            __err_line = __LINE__;
            return TT_FAIL;
        }
    } else {
        __err_line = __LINE__;
        return TT_FAIL;
    }

    ++__seq;
    return TT_SUCCESS;
}

tt_result_t __xp3_on_stag2_end(IN struct tt_xmlparser_s *parsern)
{
    if (__seq == 3) {
    } else if (__seq == 5) {
    } else {
        __err_line = __LINE__;
        return TT_FAIL;
    }

    ++__seq;
    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_xp_tag)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    char b1[] = "<tag></tag>";
    char b2[] = "  <tag     ></tag  >";
    char b3[] = "< tag    ></tag  >";
    char b4[] = " <tag  />";
    char b5[] = " <tag  / >";
    char b6[] =
        " <tag1  > \r\n"
        "<tag2><tag3/> <tag4/> </tag2> </tag1  >";
    char b7[] = "<tag    ></tag xx >";
    char b8[] = "<tag    ></ tag>";
    char b9[] = "<tag    ></>";
    char b10[] = "<></>";

    tt_xmlparser_t xp;
    tt_xmlparser_cb_t cb = {0};
    tt_result_t ret;
    tt_u8_t *p, *p_end;
    tt_u32_t n, len;

    TT_TEST_CASE_ENTER()
    // test start

    cb.on_start_tag_complete = __xp3_on_stag1_comp;
    cb.on_start_tag = __xp3_on_stag1;
    cb.on_start_tag_end = __xp3_on_stag1_end;
    cb.on_end_tag = __xp3_on_etag1;

    ret = tt_xmlparser_create(&xp, NULL, &cb, NULL);
    TT_UT_SUCCESS(ret, "");

    //////////////////////
    // 1. simple tag
    __seq = 0;
    __err_line = 0;
    __stag_num = 0;

    for (n = 0; n < sizeof((b1));) {
        len = sizeof(b1) - n;
        if (len > 5) {
            len = tt_rand_u32() % 5; // 0-4
        }
        ret = tt_xmlparser_update(&xp, (tt_u8_t *)&b1[n], len);
        TT_UT_SUCCESS(ret, "");
        n += len;
    }
    TT_ASSERT(n == sizeof(b1));

    ret = tt_xmlparser_final(&xp, NULL);
    TT_UT_SUCCESS(ret, "");

    TT_UT_EQUAL(__seq, 2, "");
    TT_UT_EQUAL(__err_line, 0, "");
    TT_UT_EQUAL(__stag_num, 0, "");

    //////////////////////
    // 2. simple tag has spaces
    __seq = 0;
    __err_line = 0;
    __stag_num = 0;

    tt_xmlparser_reset(&xp, 0);

    for (n = 0; n < sizeof((b2));) {
        len = sizeof(b2) - n;
        if (len > 5) {
            len = tt_rand_u32() % 5; // 0-4
        }
        ret = tt_xmlparser_update(&xp, (tt_u8_t *)&b2[n], len);
        TT_UT_SUCCESS(ret, "");
        n += len;
    }
    TT_ASSERT(n == sizeof(b2));

    ret = tt_xmlparser_final(&xp, NULL);
    TT_UT_SUCCESS(ret, "");

    TT_UT_EQUAL(__seq, 2, "");
    TT_UT_EQUAL(__err_line, 0, "");
    TT_UT_EQUAL(__stag_num, 0, "");

    //////////////////////
    // 3. EMPTY tag is invalid

    tt_xmlparser_reset(&xp, 0);

    ret = tt_xmlparser_update(&xp, (tt_u8_t *)b3, sizeof(b3));
    TT_UT_NOT_EQUAL(ret, TT_SUCCESS, "");

    //////////////////////
    // 4. start tag ends
    __seq = 0;
    __err_line = __LINE__;
    __stag_num = 0;

    tt_xmlparser_reset(&xp, 0);

    for (n = 0; n < sizeof((b4));) {
        len = sizeof(b4) - n;
        if (len > 5) {
            len = tt_rand_u32() % 5; // 0-4
        }
        ret = tt_xmlparser_update(&xp, (tt_u8_t *)&b4[n], len);
        TT_UT_SUCCESS(ret, "");
        n += len;
    }
    TT_ASSERT(n == sizeof(b4));

    ret = tt_xmlparser_final(&xp, NULL);
    TT_UT_SUCCESS(ret, "");

    TT_UT_EQUAL(__err_line, 0, "");
    TT_UT_EQUAL(__stag_num, 0, "");

    //////////////////////
    // 5. start tag ends with "< / >
    __stag_num = 0;

    tt_xmlparser_reset(&xp, 0);

    ret = tt_xmlparser_update(&xp, (tt_u8_t *)b5, sizeof(b5));
    TT_UT_NOT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(__stag_num, 0, "");

    //////////////////////
    // 7/8/9. invalid end tag

    __seq = 0;
    __err_line = 0;
    tt_xmlparser_reset(&xp, 0);
    ret = tt_xmlparser_update(&xp, (tt_u8_t *)b7, sizeof(b7));
    TT_UT_NOT_EQUAL(ret, TT_SUCCESS, "");

    __seq = 0;
    __err_line = 0;
    tt_xmlparser_reset(&xp, 0);
    ret = tt_xmlparser_update(&xp, (tt_u8_t *)b8, sizeof(b8));
    TT_UT_NOT_EQUAL(ret, TT_SUCCESS, "");

    __seq = 0;
    __err_line = 0;
    tt_xmlparser_reset(&xp, 0);
    ret = tt_xmlparser_update(&xp, (tt_u8_t *)b9, sizeof(b9));
    TT_UT_NOT_EQUAL(ret, TT_SUCCESS, "");

    //////////////////////
    // 6. embedded tags
    __seq = 0;
    __err_line = 0;

    tt_xmlparser_reset(&xp, 0);
    xp.cb.on_start_tag = __xp3_on_stag2;
    xp.cb.on_start_tag_end = __xp3_on_stag2_end;
    xp.cb.on_end_tag = __xp3_on_etag2;

    ret = tt_xmlparser_update(&xp, (tt_u8_t *)b6, sizeof(b6));
    TT_UT_SUCCESS(ret, "");

    ret = tt_xmlparser_final(&xp, NULL);
    TT_UT_SUCCESS(ret, "");

    TT_UT_EQUAL(__err_line, 0, "");

    // parse char by char
    p = (tt_u8_t *)b6;
    p_end = p + sizeof(b6);
    __seq = 0;
    __err_line = 0;
    tt_xmlparser_reset(&xp, 0);
    while (p < p_end) {
        ret = tt_xmlparser_update(&xp, p, 1);
        TT_UT_SUCCESS(ret, "");
        ++p;
    }
    ret = tt_xmlparser_final(&xp, NULL);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(__err_line, 0, "");

    // 10. no name
    ret = tt_xmlparser_update(&xp, (tt_u8_t *)b10, sizeof(b10));
    TT_UT_NOT_EQUAL(ret, TT_SUCCESS, "");

    tt_xmlparser_destroy(&xp);

    // test end
    TT_TEST_CASE_LEAVE()
}

tt_result_t __xp4_on_attr_1(IN struct tt_xmlparser_s *parser,
                            IN tt_u8_t *name,
                            IN tt_u32_t name_len,
                            IN tt_u8_t *value,
                            IN tt_u32_t value_len)
{
    if (__seq == 0) {
        if ((name_len != sizeof("attr") - 1) ||
            tt_strncmp((tt_char_t *)name, "attr", name_len) != 0) {
            __err_line = __LINE__;
            return TT_FAIL;
        }
        if ((value_len != sizeof("value") - 1) ||
            tt_strncmp((tt_char_t *)value, "value", value_len) != 0) {
            __err_line = __LINE__;
            return TT_FAIL;
        }
    } else if (__seq == 1) {
        if ((name_len != sizeof("attr2") - 1) ||
            tt_strncmp((tt_char_t *)name, "attr2", name_len) != 0) {
            __err_line = __LINE__;
            return TT_FAIL;
        }
        if ((value_len != sizeof("value2") - 1) ||
            tt_strncmp((tt_char_t *)value, "value2", value_len) != 0) {
            __err_line = __LINE__;
            return TT_FAIL;
        }
    } else {
        __err_line = __LINE__;
        return TT_FAIL;
    }

    ++__seq;
    return TT_SUCCESS;
}

tt_result_t __xp4_on_attr_2(IN struct tt_xmlparser_s *parser,
                            IN tt_u8_t *name,
                            IN tt_u32_t name_len,
                            IN tt_u8_t *value,
                            IN tt_u32_t value_len)
{
    if (__seq == 0) {
        if ((name_len != sizeof("attr") - 1) ||
            tt_strncmp((tt_char_t *)name, "attr", name_len) != 0) {
            __err_line = __LINE__;
            return TT_FAIL;
        }
        if ((value_len != sizeof("value") - 1) ||
            tt_strncmp((tt_char_t *)value, "value", value_len) != 0) {
            __err_line = __LINE__;
            return TT_FAIL;
        }
    } else if (__seq == 1) {
        if ((name_len != sizeof("attr2") - 1) ||
            tt_strncmp((tt_char_t *)name, "attr2", name_len) != 0) {
            __err_line = __LINE__;
            return TT_FAIL;
        }
        if ((value_len != sizeof("") - 1) ||
            tt_strncmp((tt_char_t *)value, "", value_len) != 0) {
            __err_line = __LINE__;
            return TT_FAIL;
        }
    } else {
        __err_line = __LINE__;
        return TT_FAIL;
    }

    ++__seq;
    return TT_SUCCESS;
}

tt_result_t __xp4_on_attr_3(IN struct tt_xmlparser_s *parser,
                            IN tt_u8_t *name,
                            IN tt_u32_t name_len,
                            IN tt_u8_t *value,
                            IN tt_u32_t value_len)
{
    if (__seq == 0) {
        if ((name_len != sizeof("'attr'") - 1) ||
            tt_strncmp((tt_char_t *)name, "'attr'", name_len) != 0) {
            __err_line = __LINE__;
            return TT_FAIL;
        }
        if ((value_len != sizeof("\"value\"") - 1) ||
            tt_strncmp((tt_char_t *)value, "\"value\"", value_len) != 0) {
            __err_line = __LINE__;
            return TT_FAIL;
        }
    } else if (__seq == 1) {
        if ((name_len != sizeof("\"attr2\"") - 1) ||
            tt_strncmp((tt_char_t *)name, "\"attr2\"", name_len) != 0) {
            __err_line = __LINE__;
            return TT_FAIL;
        }
        if ((value_len != sizeof("'value2'") - 1) ||
            tt_strncmp((tt_char_t *)value, "'value2'", value_len) != 0) {
            __err_line = __LINE__;
            return TT_FAIL;
        }
    } else {
        __err_line = __LINE__;
        return TT_FAIL;
    }

    ++__seq;
    return TT_SUCCESS;
}

static tt_result_t __xp4_on_stag_com(IN struct tt_xmlparser_s *xp)
{
    __stag_num++;

    return TT_SUCCESS;
}

static tt_result_t __xp4_on_etag(IN struct tt_xmlparser_s *xp,
                                 IN tt_u8_t *value,
                                 IN tt_u32_t value_len)
{
    __stag_num--;

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_xp_attr)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    char b1[] =
        "<tag attr\r\n =\r\n\t \"value\"   attr2\r\n\t =\r\n\t \'value2\'   "
        "></tag>";
    char b2[] = "<tag  attr=\t\'value\'   attr2=value2></tag>";
    char b3[] = "<tag   attr=\"value\"   attr2=\"value2></tag>";
    char b4[] = "<tag    attr=\"value\"   attr2=value2\'></tag>";
    char b5[] = "<tag    attr=\'value\'   attr2=\"\"/>";
    char b6[] =
        "<tag 'attr'\r\n\t =\r\n\t '\"value\"'   \"attr2\"\r\n\t =\r\n\t "
        "\"'value2'\"   ></tag>";
    char b7_1[] = "<tag    a   b></tag>";
    char b7_2[] = "<tag    a></tag>";
    char b7_3[] = "<tag    a/>";
    char b7_4[] = "<tag =a/>";
    char b7_5[] = "<tag = />";
    char b7_6[] = "<tag attr=value/>";

    tt_xmlparser_t xp;
    tt_xmlparser_cb_t cb = {0};
    tt_result_t ret;
    tt_u8_t *p, *p_end;
    tt_u32_t n, len;

    TT_TEST_CASE_ENTER()
    // test start

    cb.on_attr = __xp4_on_attr_1;
    cb.on_start_tag_complete = __xp4_on_stag_com;
    cb.on_end_tag = __xp4_on_etag;

    ret = tt_xmlparser_create(&xp, NULL, &cb, NULL);
    TT_UT_SUCCESS(ret, "");

    //////////////////////
    // 1. simple tag
    __seq = 0;
    __err_line = 0;
    __stag_num = 0;

    tt_xmlparser_reset(&xp, 0);

    for (n = 0; n < sizeof((b1));) {
        len = sizeof(b1) - n;
        if (len > 5) {
            len = tt_rand_u32() % 5; // 0-4
        }
        ret = tt_xmlparser_update(&xp, (tt_u8_t *)&b1[n], len);
        TT_UT_SUCCESS(ret, "");
        n += len;
    }
    TT_ASSERT(n == sizeof(b1));

    ret = tt_xmlparser_final(&xp, NULL);
    TT_UT_SUCCESS(ret, "");

    TT_UT_EQUAL(__seq, 2, "");
    TT_UT_EQUAL(__err_line, 0, "");
    TT_UT_EQUAL(__stag_num, 0, "");

    //////////////////////
    // 2/3/4. value is not in quotes
    __seq = 0;
    __err_line = 0;
    tt_xmlparser_reset(&xp, 0);
    ret = tt_xmlparser_update(&xp, (tt_u8_t *)b2, sizeof(b2));
    TT_UT_NOT_EQUAL(ret, TT_SUCCESS, "");

    // can not find terminating node, so update is ok but final failed
    __seq = 0;
    __err_line = 0;
    tt_xmlparser_reset(&xp, 0);
    ret = tt_xmlparser_update(&xp, (tt_u8_t *)b3, sizeof(b3));
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_xmlparser_final(&xp, NULL);
    TT_UT_NOT_EQUAL(ret, TT_SUCCESS, "");

    __seq = 0;
    __err_line = 0;
    tt_xmlparser_reset(&xp, 0);
    ret = tt_xmlparser_update(&xp, (tt_u8_t *)b4, sizeof(b4));
    TT_UT_NOT_EQUAL(ret, TT_SUCCESS, "");

    //////////////////////
    // 5. EMPTY attribute value
    __seq = 0;
    __err_line = 0;
    __stag_num = 0;

    tt_xmlparser_reset(&xp, 0);

    xp.cb.on_attr = __xp4_on_attr_2;

    for (n = 0; n < sizeof((b5));) {
        len = sizeof(b5) - n;
        if (len > 5) {
            len = tt_rand_u32() % 5; // 0-4
        }
        ret = tt_xmlparser_update(&xp, (tt_u8_t *)&b5[n], len);
        TT_UT_SUCCESS(ret, "");
        n += len;
    }
    TT_ASSERT(n == sizeof(b5));

    ret = tt_xmlparser_final(&xp, NULL);
    TT_UT_SUCCESS(ret, "");

    TT_UT_EQUAL(__seq, 2, "");
    TT_UT_EQUAL(__err_line, 0, "");
    TT_UT_EQUAL(__stag_num, 0, "");

    //////////////////////
    // 6. attribute value
    __seq = 0;
    __err_line = 0;
    __stag_num = 0;

    tt_xmlparser_reset(&xp, 0);

    xp.cb.on_attr = __xp4_on_attr_3;

    for (n = 0; n < sizeof((b6));) {
        len = sizeof(b6) - n;
        if (len > 5) {
            len = tt_rand_u32() % 5; // 0-4
        }
        ret = tt_xmlparser_update(&xp, (tt_u8_t *)&b6[n], len);
        TT_UT_SUCCESS(ret, "");
        n += len;
    }
    TT_ASSERT(n == sizeof(b6));

    ret = tt_xmlparser_final(&xp, NULL);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(__stag_num, 0, "");

    // parse char by char
    p = (tt_u8_t *)b6;
    p_end = p + sizeof(b6);
    __seq = 0;
    __err_line = 0;
    __stag_num = 0;

    tt_xmlparser_reset(&xp, 0);
    while (p < p_end) {
        ret = tt_xmlparser_update(&xp, p, 1);
        TT_UT_SUCCESS(ret, "");
        ++p;
    }
    ret = tt_xmlparser_final(&xp, NULL);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(__err_line, 0, "");
    TT_UT_EQUAL(__stag_num, 0, "");

    //////////////////////
    // 7. invalid attribute
    __seq = 0;
    __err_line = 0;
    tt_xmlparser_reset(&xp, 0);
    ret = tt_xmlparser_update(&xp, (tt_u8_t *)b7_1, sizeof(b7_1));
    TT_UT_NOT_EQUAL(ret, TT_SUCCESS, "");

    __seq = 0;
    __err_line = 0;
    tt_xmlparser_reset(&xp, 0);
    ret = tt_xmlparser_update(&xp, (tt_u8_t *)b7_2, sizeof(b7_2));
    TT_UT_NOT_EQUAL(ret, TT_SUCCESS, "");

    __seq = 0;
    __err_line = 0;
    tt_xmlparser_reset(&xp, 0);
    ret = tt_xmlparser_update(&xp, (tt_u8_t *)b7_3, sizeof(b7_3));
    TT_UT_NOT_EQUAL(ret, TT_SUCCESS, "");

    __seq = 0;
    __err_line = 0;
    tt_xmlparser_reset(&xp, 0);
    ret = tt_xmlparser_update(&xp, (tt_u8_t *)b7_4, sizeof(b7_4));
    TT_UT_NOT_EQUAL(ret, TT_SUCCESS, "");

    __seq = 0;
    __err_line = 0;
    tt_xmlparser_reset(&xp, 0);
    ret = tt_xmlparser_update(&xp, (tt_u8_t *)b7_5, sizeof(b7_5));
    TT_UT_NOT_EQUAL(ret, TT_SUCCESS, "");

    __seq = 0;
    __err_line = 0;
    tt_xmlparser_reset(&xp, 0);
    ret = tt_xmlparser_update(&xp, (tt_u8_t *)b7_6, sizeof(b7_6));
    TT_UT_NOT_EQUAL(ret, TT_SUCCESS, "");

    tt_xmlparser_destroy(&xp);

    // test end
    TT_TEST_CASE_LEAVE()
}

tt_result_t __xp5_on_pi_1(IN struct tt_xmlparser_s *parser,
                          IN tt_u8_t *name,
                          IN tt_u32_t name_len,
                          IN tt_u8_t *value,
                          IN tt_u32_t value_len)
{
    if ((name_len != sizeof("pi1") - 1) ||
        tt_strncmp((tt_char_t *)name, "pi1", name_len) != 0) {
        __err_line = __LINE__;
        return TT_FAIL;
    }
    if ((value_len != 0) || (value != NULL)) {
        __err_line = __LINE__;
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t __xp5_on_pi_2(IN struct tt_xmlparser_s *parser,
                          IN tt_u8_t *name,
                          IN tt_u32_t name_len,
                          IN tt_u8_t *value,
                          IN tt_u32_t value_len)
{
    if ((name_len != sizeof("pi1") - 1) ||
        tt_strncmp((tt_char_t *)name, "pi1", name_len) != 0) {
        __err_line = __LINE__;
        return TT_FAIL;
    }
    if ((value_len != sizeof("abcdefg  12345") - 1) ||
        tt_strncmp((tt_char_t *)value, "abcdefg  12345", value_len) != 0) {
        __err_line = __LINE__;
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_xp_pi)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    char b1[] = "<tag><?pi1?></tag>";
    char b2[] = " <?pi1 ?> ";
    char b3_1[] = " <?pi1 ? > ";
    char b3_2[] = " <? pi1 ?> ";
    char b4[] = "<tag><?pi1   abcdefg  12345\r   ?></tag>";

    tt_xmlparser_t xp;
    tt_xmlparser_cb_t cb = {0};
    tt_result_t ret;
    tt_u8_t *p, *p_end;
    tt_u32_t n, len;

    TT_TEST_CASE_ENTER()
    // test start

    cb.on_pi = __xp5_on_pi_1;

    ret = tt_xmlparser_create(&xp, NULL, &cb, NULL);
    TT_UT_SUCCESS(ret, "");

    //////////////////////
    // 1. simple tag
    __err_line = 0;
    tt_xmlparser_reset(&xp, 0);

    for (n = 0; n < sizeof((b1));) {
        len = sizeof(b1) - n;
        if (len > 3) {
            len = tt_rand_u32() % 3; // 0-2
        }
        ret = tt_xmlparser_update(&xp, (tt_u8_t *)&b1[n], len);
        TT_UT_SUCCESS(ret, "");
        n += len;
    }
    TT_ASSERT(n == sizeof(b1));

    ret = tt_xmlparser_final(&xp, NULL);
    TT_UT_SUCCESS(ret, "");

    TT_UT_EQUAL(__err_line, 0, "");

    //////////////////////
    // 2. empty pi value
    __err_line = 0;
    tt_xmlparser_reset(&xp, 0);

    ret = tt_xmlparser_update(&xp, (tt_u8_t *)b2, sizeof(b2));
    TT_UT_SUCCESS(ret, "");

    ret = tt_xmlparser_final(&xp, NULL);
    TT_UT_SUCCESS(ret, "");

    TT_UT_EQUAL(__err_line, 0, "");

    //////////////////////
    // 3. invalid pi value
    __err_line = 0;
    tt_xmlparser_reset(&xp, 0);
    ret = tt_xmlparser_update(&xp, (tt_u8_t *)b3_1, sizeof(b3_1));
    TT_UT_NOT_EQUAL(ret, TT_SUCCESS, "");

    __err_line = 0;
    tt_xmlparser_reset(&xp, 0);
    ret = tt_xmlparser_update(&xp, (tt_u8_t *)b3_2, sizeof(b3_2));
    TT_UT_NOT_EQUAL(ret, TT_SUCCESS, "");

    //////////////////////
    // 4. pi value
    __err_line = 0;
    tt_xmlparser_reset(&xp, 0);

    xp.cb.on_pi = __xp5_on_pi_2;
    ret = tt_xmlparser_update(&xp, (tt_u8_t *)b4, sizeof(b4));
    TT_UT_SUCCESS(ret, "");

    ret = tt_xmlparser_final(&xp, NULL);
    TT_UT_SUCCESS(ret, "");

    TT_UT_EQUAL(__err_line, 0, "");

    // parse char by char
    p = (tt_u8_t *)b4;
    p_end = p + sizeof(b4);
    __err_line = 0;
    tt_xmlparser_reset(&xp, 0);
    while (p < p_end) {
        ret = tt_xmlparser_update(&xp, p, 1);
        TT_UT_SUCCESS(ret, "");
        ++p;
    }
    ret = tt_xmlparser_final(&xp, NULL);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(__err_line, 0, "");


    tt_xmlparser_destroy(&xp);

    // test end
    TT_TEST_CASE_LEAVE()
}

tt_result_t __xp6_on_cm_1(IN struct tt_xmlparser_s *parser,
                          IN tt_u8_t *value,
                          IN tt_u32_t value_len)
{
    if ((value_len != sizeof("comment1->-- >->-") - 1) ||
        (tt_strncmp((tt_char_t *)value, "comment1->-- >->-", value_len) != 0)) {
        __err_line = __LINE__;
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t __xp6_on_cm_2(IN struct tt_xmlparser_s *parser,
                          IN tt_u8_t *value,
                          IN tt_u32_t value_len)
{
    if ((value_len != sizeof("comment1->-- >->-\r\n  -- \tcomment2") - 1) ||
        (tt_strncmp((tt_char_t *)value,
                    "comment1->-- >->-\r\n  -- \tcomment2",
                    value_len) != 0)) {
        __err_line = __LINE__;
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t __xp6_on_cm_2_notrim(IN struct tt_xmlparser_s *parser,
                                 IN tt_u8_t *value,
                                 IN tt_u32_t value_len)
{
    if ((value_len !=
         sizeof("  comment1->-- >->-\r\n  -- \tcomment2\r\n  \t ") - 1) ||
        (tt_strncmp((tt_char_t *)value,
                    "  comment1->-- >->-\r\n  -- \tcomment2\r\n  \t ",
                    value_len) != 0)) {
        __err_line = __LINE__;
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t __xp6_on_cm_empty(IN struct tt_xmlparser_s *parser,
                              IN tt_u8_t *value,
                              IN tt_u32_t value_len)
{
    __err_line = __LINE__;
    return TT_FAIL;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_xp_comment)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    char b1[] = "<tag><!--comment1->-- >->---></tag>";
    char b1_1[] = "<tag><!- -comment1--></tag>";
    char b2[] = "<tag><!--  comment1->-- >->-\r\n  \t --></tag>";
    char b3[] =
        "<tag><!--  comment1->-- >->-\r\n  -- \tcomment2\r\n  \t --></tag>";
    char b4[] = "<!---->";

    tt_xmlparser_t xp;
    tt_xmlparser_cb_t cb = {0};
    tt_result_t ret;
    tt_u8_t *p, *p_end;
    tt_u32_t n, len;
    tt_xmlparser_attr_t attr;

    TT_TEST_CASE_ENTER()
    // test start

    cb.on_comment = __xp6_on_cm_1;

    tt_xmlparser_attr_default(&attr);
    attr.trim_comment = TT_TRUE;

    ret = tt_xmlparser_create(&xp, NULL, &cb, &attr);
    TT_UT_SUCCESS(ret, "");

    //////////////////////
    // 1. simple
    __err_line = 0;
    tt_xmlparser_reset(&xp, 0);

    for (n = 0; n < sizeof((b1));) {
        len = sizeof(b1) - n;
        if (len > 3) {
            len = tt_rand_u32() % 3; // 0-2
        }
        ret = tt_xmlparser_update(&xp, (tt_u8_t *)&b1[n], len);
        TT_UT_SUCCESS(ret, "");
        n += len;
    }
    TT_ASSERT(n == sizeof(b1));

    ret = tt_xmlparser_final(&xp, NULL);
    TT_UT_SUCCESS(ret, "");

    TT_UT_EQUAL(__err_line, 0, "");

    // invalid comment
    __err_line = 0;
    tt_xmlparser_reset(&xp, 0);
    ret = tt_xmlparser_update(&xp, (tt_u8_t *)b1_1, sizeof(b1_1));
    TT_UT_NOT_EQUAL(ret, TT_SUCCESS, "");

    //////////////////////
    // 2. remove beginning and ending white spaces
    __err_line = 0;
    tt_xmlparser_reset(&xp, 0);

    for (n = 0; n < sizeof((b2));) {
        len = sizeof(b2) - n;
        if (len > 3) {
            len = tt_rand_u32() % 5; // 0-2
        }
        ret = tt_xmlparser_update(&xp, (tt_u8_t *)&b2[n], len);
        TT_UT_SUCCESS(ret, "");
        n += len;
    }
    TT_ASSERT(n == sizeof(b2));

    ret = tt_xmlparser_final(&xp, NULL);
    TT_UT_SUCCESS(ret, "");

    TT_UT_EQUAL(__err_line, 0, "");

    //////////////////////
    // 3. ..
    __err_line = 0;
    tt_xmlparser_reset(&xp, 0);

    xp.cb.on_comment = __xp6_on_cm_2;

    for (n = 0; n < sizeof((b3));) {
        len = sizeof(b3) - n;
        if (len > 3) {
            len = tt_rand_u32() % 5; // 0-2
        }
        ret = tt_xmlparser_update(&xp, (tt_u8_t *)&b3[n], len);
        TT_UT_SUCCESS(ret, "");
        n += len;
    }
    TT_ASSERT(n == sizeof(b3));

    ret = tt_xmlparser_final(&xp, NULL);
    TT_UT_SUCCESS(ret, "");

    TT_UT_EQUAL(__err_line, 0, "");

    // parse char by char
    p = (tt_u8_t *)b3;
    p_end = p + sizeof(b3);
    __err_line = 0;
    tt_xmlparser_reset(&xp, 0);
    while (p < p_end) {
        ret = tt_xmlparser_update(&xp, p, 1);
        TT_UT_SUCCESS(ret, "");
        ++p;
    }
    ret = tt_xmlparser_final(&xp, NULL);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(__err_line, 0, "");

    //////////////////////
    // 4. empty comment
    __err_line = 0;
    tt_xmlparser_reset(&xp, 0);

    xp.cb.on_comment = __xp6_on_cm_empty;

    ret = tt_xmlparser_update(&xp, (tt_u8_t *)b4, sizeof(b4));
    TT_UT_SUCCESS(ret, "");

    ret = tt_xmlparser_final(&xp, NULL);
    TT_UT_SUCCESS(ret, "");

    TT_UT_EQUAL(__err_line, 0, "");


    tt_xmlparser_destroy(&xp);


    /////////////////////////////
    tt_xmlparser_attr_default(&attr);
    attr.trim_comment = TT_FALSE;

    ret = tt_xmlparser_create(&xp, NULL, &cb, &attr);
    TT_UT_SUCCESS(ret, "");

    // trim comment
    __err_line = 0;
    tt_xmlparser_reset(&xp, 0);

    cb.on_comment = __xp6_on_cm_1;

    xp.cb.on_comment = __xp6_on_cm_2_notrim;

    ret = tt_xmlparser_update(&xp, (tt_u8_t *)b3, sizeof(b3));
    TT_UT_SUCCESS(ret, "");

    ret = tt_xmlparser_final(&xp, NULL);
    TT_UT_SUCCESS(ret, "");

    TT_UT_EQUAL(__err_line, 0, "");

    tt_xmlparser_destroy(&xp);

    // test end
    TT_TEST_CASE_LEAVE()
}

tt_result_t __xp7_on_cdata_1(IN struct tt_xmlparser_s *parser,
                             IN tt_u8_t *value,
                             IN tt_u32_t value_len)
{
    if ((value_len != sizeof(" ><![CDATA[test cdata]>]\r\n ") - 1) ||
        (tt_strncmp((tt_char_t *)value,
                    " ><![CDATA[test cdata]>]\r\n ",
                    value_len) != 0)) {
        __err_line = __LINE__;
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t __xp7_on_cdata_1_trim(IN struct tt_xmlparser_s *parser,
                                  IN tt_u8_t *value,
                                  IN tt_u32_t value_len)
{
    if ((value_len != sizeof("><![CDATA[test cdata]>]") - 1) ||
        (tt_strncmp((tt_char_t *)value, "><![CDATA[test cdata]>]", value_len) !=
         0)) {
        __err_line = __LINE__;
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t __xp7_on_cdata_none(IN struct tt_xmlparser_s *parser,
                                IN tt_u8_t *value,
                                IN tt_u32_t value_len)
{
    __err_line = __LINE__;
    return TT_FAIL;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_xp_cdata)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    char b1[] = "<tag><![CDATA[ ><![CDATA[test cdata]>]\r\n ]]></tag>";
    char b1_1[] = "<tag><![CDATA<![CDATA[test cdata]>]]]></tag>";
    char b1_2[] = "<tag><! [CDATA[<![CDATA[test cdata]>]]]></tag>";
    char b2[] = "<tag><![CDATA[]]></tag>";

    tt_xmlparser_t xp;
    tt_xmlparser_cb_t cb = {0};
    tt_result_t ret;
    tt_u8_t *p, *p_end;
    tt_u32_t n, len;
    tt_xmlparser_attr_t attr;

    TT_TEST_CASE_ENTER()
    // test start

    cb.on_cdata = __xp7_on_cdata_1;

    tt_xmlparser_attr_default(&attr);
    attr.trim_cdata = TT_FALSE;

    ret = tt_xmlparser_create(&xp, NULL, &cb, &attr);
    TT_UT_SUCCESS(ret, "");

    //////////////////////
    // 1. simple
    __err_line = 0;
    tt_xmlparser_reset(&xp, 0);

    for (n = 0; n < sizeof((b1));) {
        len = sizeof(b1) - n;
        if (len > 3) {
            len = tt_rand_u32() % 5; // 0-2
        }
        ret = tt_xmlparser_update(&xp, (tt_u8_t *)&b1[n], len);
        TT_UT_SUCCESS(ret, "");
        n += len;
    }
    TT_ASSERT(n == sizeof(b1));

    ret = tt_xmlparser_final(&xp, NULL);
    TT_UT_SUCCESS(ret, "");

    TT_UT_EQUAL(__err_line, 0, "");

    // parse char by char
    p = (tt_u8_t *)b1;
    p_end = p + sizeof(b1);
    __err_line = 0;
    tt_xmlparser_reset(&xp, 0);
    while (p < p_end) {
        ret = tt_xmlparser_update(&xp, p, 1);
        TT_UT_SUCCESS(ret, "");
        ++p;
    }
    ret = tt_xmlparser_final(&xp, NULL);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(__err_line, 0, "");

    // invalid comment
    __err_line = 0;
    tt_xmlparser_reset(&xp, 0);
    ret = tt_xmlparser_update(&xp, (tt_u8_t *)b1_1, sizeof(b1_1));
    TT_UT_NOT_EQUAL(ret, TT_SUCCESS, "");

    __err_line = 0;
    tt_xmlparser_reset(&xp, 0);
    ret = tt_xmlparser_update(&xp, (tt_u8_t *)b1_2, sizeof(b1_2));
    TT_UT_NOT_EQUAL(ret, TT_SUCCESS, "");

    //////////////////////
    // 2. empty cdata
    __err_line = 0;
    tt_xmlparser_reset(&xp, 0);

    ret = tt_xmlparser_update(&xp, (tt_u8_t *)b2, sizeof(b2));
    TT_UT_SUCCESS(ret, "");

    ret = tt_xmlparser_final(&xp, NULL);
    TT_UT_SUCCESS(ret, "");

    TT_UT_EQUAL(__err_line, 0, "");

    tt_xmlparser_destroy(&xp);

    //////////////////////
    // trim cdata
    __err_line = 0;

    cb.on_cdata = __xp7_on_cdata_1_trim;

    tt_xmlparser_attr_default(&attr);
    attr.trim_cdata = TT_TRUE;

    ret = tt_xmlparser_create(&xp, NULL, &cb, &attr);
    TT_UT_SUCCESS(ret, "");

    for (n = 0; n < sizeof((b1));) {
        len = sizeof(b1) - n;
        if (len > 3) {
            len = tt_rand_u32() % 5; // 0-2
        }
        ret = tt_xmlparser_update(&xp, (tt_u8_t *)&b1[n], len);
        TT_UT_SUCCESS(ret, "");
        n += len;
    }
    TT_ASSERT(n == sizeof(b1));

    ret = tt_xmlparser_final(&xp, NULL);
    TT_UT_SUCCESS(ret, "");

    TT_UT_EQUAL(__err_line, 0, "");

    /////// none after trim
    __err_line = 0;
    tt_xmlparser_reset(&xp, 0);

    xp.cb.on_cdata = __xp7_on_cdata_none;

    ret = tt_xmlparser_update(&xp, (tt_u8_t *)b2, sizeof(b2));
    TT_UT_SUCCESS(ret, "");

    ret = tt_xmlparser_final(&xp, NULL);
    TT_UT_SUCCESS(ret, "");

    TT_UT_EQUAL(__err_line, 0, "");


    tt_xmlparser_destroy(&xp);


    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_xp_decode)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_xmlmem_t xm;
    tt_result_t ret;
    tt_char_t *dec;
    tt_s32_t cmp_ret;

    TT_TEST_CASE_ENTER()
    // test start

    ret = tt_xmlmem_create(&xm, NULL);
    TT_UT_SUCCESS(ret, "");

    // empty
    dec = tt_xml_chdec_len(&xm, "", 0);
    TT_UT_NOT_EQUAL(dec, NULL, "");
    cmp_ret = tt_strcmp(dec, "");
    TT_UT_EQUAL(cmp_ret, 0, "");
    tt_xm_free(dec);

    // empty, long len
    dec = tt_xml_chdec_len(&xm, "", 10);
    TT_UT_NOT_EQUAL(dec, NULL, "");
    cmp_ret = tt_strcmp(dec, "");
    TT_UT_EQUAL(cmp_ret, 0, "");
    tt_xm_free(dec);

    // basic
    dec = tt_xml_chdec_len(
        &xm, "&quot;&apos;&lt;&gt;&amp;&#0038;&#xA2;&#x20AC;&#x10348;", 10000);
    TT_UT_NOT_EQUAL(dec, NULL, "");
    cmp_ret = tt_strcmp(dec, "\"'<>&&\xc2\xa2\xe2\x82\xac\xf0\x90\x8d\x88");
    TT_UT_EQUAL(cmp_ret, 0, "");
    tt_xm_free(dec);

    // mixed
    dec = tt_xml_chdec_len(&xm,
                           "1111&quot;22222&apos;33&#0038;444&#x10348;",
                           10000);
    TT_UT_NOT_EQUAL(dec, NULL, "");
    cmp_ret = tt_strcmp(dec, "1111\"22222'33&444\xf0\x90\x8d\x88");
    TT_UT_EQUAL(cmp_ret, 0, "");
    tt_xm_free(dec);

    // mixed
    dec = tt_xml_chdec_len(&xm,
                           "&quot;22222&apos;33&#0038;444&#x10348;5555",
                           10000);
    TT_UT_NOT_EQUAL(dec, NULL, "");
    cmp_ret = tt_strcmp(dec,
                        "\"22222'33&444\xf0\x90\x8d\x88"
                        "5555");
    TT_UT_EQUAL(cmp_ret, 0, "");
    tt_xm_free(dec);

    // see &, but not valid
    dec = tt_xml_chdec(&xm, "&");
    TT_UT_NOT_EQUAL(dec, NULL, "");
    cmp_ret = tt_strcmp(dec, "&");
    TT_UT_EQUAL(cmp_ret, 0, "");
    tt_xm_free(dec);

    // see &, but not valid
    dec = tt_xml_chdec(&xm, "&&&");
    TT_UT_NOT_EQUAL(dec, NULL, "");
    cmp_ret = tt_strcmp(dec, "&&&");
    TT_UT_EQUAL(cmp_ret, 0, "");
    tt_xm_free(dec);

    // not terminated
    dec = tt_xml_chdec(&xm, "12345&quot");
    TT_UT_NOT_EQUAL(dec, NULL, "");
    cmp_ret = tt_strcmp(dec, "12345&quot");
    TT_UT_EQUAL(cmp_ret, 0, "");
    tt_xm_free(dec);

    // invalid escape
    dec = tt_xml_chdec(&xm, "&;ending");
    TT_UT_NOT_EQUAL(dec, NULL, "");
    cmp_ret = tt_strcmp(dec, "&;ending");
    TT_UT_EQUAL(cmp_ret, 0, "");
    tt_xm_free(dec);

    // invalid escape
    dec = tt_xml_chdec(&xm, "begin&unknown;ending");
    TT_UT_NOT_EQUAL(dec, NULL, "");
    cmp_ret = tt_strcmp(dec, "begin&unknown;ending");
    TT_UT_EQUAL(cmp_ret, 0, "");
    tt_xm_free(dec);

    // hex: 1
    dec = tt_xml_chdec(&xm, "&#x1;ending");
    TT_UT_NOT_EQUAL(dec, NULL, "");
    cmp_ret = tt_strcmp(dec,
                        "\x1"
                        "ending");
    TT_UT_EQUAL(cmp_ret, 0, "");
    tt_xm_free(dec);

    // hex: max
    dec = tt_xml_chdec(&xm, "&#x7FFFFFFF;ending");
    TT_UT_NOT_EQUAL(dec, NULL, "");
    cmp_ret = tt_strcmp(dec,
                        "\xfd\xbf\xbf\xbf\xbf\xbf"
                        "ending");
    TT_UT_EQUAL(cmp_ret, 0, "");
    tt_xm_free(dec);

    // hex: max, not verify encoded value
    dec = tt_xml_chdec(&xm, "&#xFFFFFFFF;ending");
    TT_UT_NOT_EQUAL(dec, NULL, "");
    tt_xm_free(dec);

    // hex: invalid
    dec = tt_xml_chdec(&xm, "&#x1FFFFFFFF;ending");
    TT_UT_NOT_EQUAL(dec, NULL, "");
    cmp_ret = tt_strcmp(dec, "&#x1FFFFFFFF;ending");
    TT_UT_EQUAL(cmp_ret, 0, "");
    tt_xm_free(dec);

    // decimal: 1
    dec = tt_xml_chdec(&xm, "&#1;ending");
    TT_UT_NOT_EQUAL(dec, NULL, "");
    cmp_ret = tt_strcmp(dec,
                        "\x1"
                        "ending");
    TT_UT_EQUAL(cmp_ret, 0, "");
    tt_xm_free(dec);

    // decimal: max
    dec = tt_xml_chdec(&xm, "begin&#2147483647;");
    TT_UT_NOT_EQUAL(dec, NULL, "");
    cmp_ret = tt_strcmp(dec,
                        "begin"
                        "\xfd\xbf\xbf\xbf\xbf\xbf");
    TT_UT_EQUAL(cmp_ret, 0, "");
    tt_xm_free(dec);

    // decimal: max, no verify
    dec = tt_xml_chdec(&xm, "begin&#4294967295;");
    TT_UT_NOT_EQUAL(dec, NULL, "");
    tt_xm_free(dec);

    // decimal: invalid
    dec = tt_xml_chdec(&xm, "begin&#4294967296;");
    TT_UT_NOT_EQUAL(dec, NULL, "");
    cmp_ret = tt_strcmp(dec, "begin&#4294967296;");
    TT_UT_EQUAL(cmp_ret, 0, "");
    tt_xm_free(dec);

    tt_xmlmem_destroy(&xm);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_xp_doctype)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    char b1[] =
        "<tag><!DOCTYPE content><![CDATA[ ><![CDATA[test cdata]>]\r\n "
        "]]></tag>";
    char b1_1[] = "<!DOCTYPE><![CDATA[ ><![CDATA[test cdata]>]\r\n ]]>";
    char b1_2[] = "<!DOCTYP  ><![CDATA[ ><![CDATA[test cdata]>]\r\n ]]>";
    char b1_3[] = "<![CDATA[ ><![CDATA[test cdata]>]\r\n ]]><!DOCTYPF>";
    char b2[] = "<tag><![CDATA[]]><!DOCTYPE </tag";

    tt_xmlparser_t xp;
    tt_xmlparser_cb_t cb = {0};
    tt_result_t ret;
    tt_u8_t *p, *p_end;
    tt_u32_t n, len;
    tt_xmlparser_attr_t attr;

    TT_TEST_CASE_ENTER()
    // test start

    cb.on_cdata = __xp7_on_cdata_1;

    tt_xmlparser_attr_default(&attr);
    attr.trim_cdata = TT_FALSE;

    ret = tt_xmlparser_create(&xp, NULL, &cb, &attr);
    TT_UT_SUCCESS(ret, "");

    //////////////////////
    // 1. simple
    __err_line = 0;
    tt_xmlparser_reset(&xp, 0);

    for (n = 0; n < sizeof((b1));) {
        len = sizeof(b1) - n;
        if (len > 3) {
            len = tt_rand_u32() % 5; // 0-2
        }
        ret = tt_xmlparser_update(&xp, (tt_u8_t *)&b1[n], len);
        TT_UT_SUCCESS(ret, "");
        n += len;
    }
    TT_ASSERT(n == sizeof(b1));

    ret = tt_xmlparser_final(&xp, NULL);
    TT_UT_SUCCESS(ret, "");

    TT_UT_EQUAL(__err_line, 0, "");

    // parse char by char
    p = (tt_u8_t *)b1;
    p_end = p + sizeof(b1);
    __err_line = 0;
    tt_xmlparser_reset(&xp, 0);
    while (p < p_end) {
        ret = tt_xmlparser_update(&xp, p, 1);
        TT_UT_SUCCESS(ret, "");
        ++p;
    }
    ret = tt_xmlparser_final(&xp, NULL);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(__err_line, 0, "");

    ///////////////// empty doctype
    __err_line = 0;
    tt_xmlparser_reset(&xp, 0);

    for (n = 0; n < sizeof((b1_1));) {
        len = sizeof(b1_1) - n;
        if (len > 3) {
            len = tt_rand_u32() % 5; // 0-2
        }
        ret = tt_xmlparser_update(&xp, (tt_u8_t *)&b1_1[n], len);
        TT_UT_SUCCESS(ret, "");
        n += len;
    }
    TT_ASSERT(n == sizeof(b1_1));

    ret = tt_xmlparser_final(&xp, NULL);
    TT_UT_SUCCESS(ret, "");

    TT_UT_EQUAL(__err_line, 0, "");

    /////////// invalid doctype
    __err_line = 0;
    tt_xmlparser_reset(&xp, 0);
    ret = tt_xmlparser_update(&xp, (tt_u8_t *)b1_2, sizeof(b1_2));
    TT_UT_NOT_EQUAL(ret, TT_SUCCESS, "");

    /////////// invalid doctype
    __err_line = 0;
    tt_xmlparser_reset(&xp, 0);
    ret = tt_xmlparser_update(&xp, (tt_u8_t *)b1_3, sizeof(b1_3));
    TT_UT_NOT_EQUAL(ret, TT_SUCCESS, "");

    //////////////////////
    // 2. invalid, but parsable, not terminated
    __err_line = 0;
    tt_xmlparser_reset(&xp, 0);

    ret = tt_xmlparser_update(&xp, (tt_u8_t *)b2, sizeof(b2));
    TT_UT_SUCCESS(ret, "");

    ret = tt_xmlparser_final(&xp, NULL);
    TT_UT_NOT_EQUAL(ret, TT_SUCCESS, "");

    tt_xmlparser_destroy(&xp);

    // test end
    TT_TEST_CASE_LEAVE()
}
