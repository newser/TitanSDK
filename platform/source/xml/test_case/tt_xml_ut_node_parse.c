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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_xnp_basic)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_xnp_ns)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_xnp_ns_etag)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_xnp_ns_invalid)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(xnp_case)

TT_TEST_CASE("tt_unit_test_xnp_basic",
             "xml node parse: basic test",
             tt_unit_test_xnp_basic,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_xnp_ns",
                 "xml node parse: namespace",
                 tt_unit_test_xnp_ns,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_xnp_ns_etag",
                 "xml node parse: invalid end tag",
                 tt_unit_test_xnp_ns_etag,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_xnp_ns_invalid",
                 "xml node parse: invalid ns usage",
                 tt_unit_test_xnp_ns_invalid,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(xnp_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(XML_UT_PSST_NODE_PARSE, 0, xnp_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(tt_unit_test_xnp_basic)
     {
     //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
     tt_xmlnp_t xnp;
     tt_result_t ret;
     tt_xmlnp_cb_t cb;

     TT_TEST_CASE_ENTER()
     // test start

     ret = tt_xmlnp_create(&xnp, &cb, (void*)0x123, NULL);
     TT_TEST_CHECK_SUCCESS(ret, "");

     tt_xmlnp_destroy(&xnp)

     // test end
     TT_TEST_CASE_LEAVE()
     }
    */

    static tt_char_t __test_xml[] =
        "\
<?xml vers&amp;ion=\"1.0\" encoding=\"UTF-8\"  ?>\n\
<!DOCTYPE sqlMap PUBLIC \"-//iBATIS.com//DTD SQL Map 2.0//EN\" \"http://www.ibatis.com/dtd/sql-map-2.dtd\">\n\
<sql&amp;Map namespace=\"EmpComplex  Result\">\n\
\n\
    text &amp; - &quot; - &apos;\n\
    <typeAlias ali&apos;as&apos;=\"empVO\" type  = \"egov&apos;framework&apos;.rte.psl.dataaccess.vo.EmpVO\" />\n\
    text2 &lt; /&gt;\n\
\n\
    <![CDATA[ \n\
this is cdata \n\
can not escape &quot; &quot; \n\
    ]]> \n\
text3 &lt; /&gt;\n\
\n\
    <!-- \n\
this is comment \n\
can not escape &quot; &quot; \n\
    --> \n\
\n\
</sql&amp;Map>";

static tt_result_t __ut_xnp_ret;
static tt_u32_t __ut_xnp_err_line;
static tt_u32_t __ut_xnp_seq;

#define __1XON_FAIL                                                            \
    __ut_xnp_ret = TT_FAIL;                                                    \
    __ut_xnp_err_line = __LINE__;                                              \
    return;

void __1_xnp_on_node_start(IN void *param, IN tt_xnode_t *xn)
{
    if (param != (void *)0x123) {
        __1XON_FAIL;
    }

    if (__ut_xnp_seq == 1) {
        // 1. node is a element
        if (xn->type != TT_XNODE_TYPE_ELEMENT) {
            __1XON_FAIL;
        }
        if (tt_strcmp(xn->name, "sql&Map") != 0) {
            __1XON_FAIL;
        }
        if (xn->value != NULL) {
            __1XON_FAIL;
        }

        // check attr
        {
            tt_xnode_t *a = tt_xnode_first_attr(xn);

            if (a == NULL) {
                __1XON_FAIL;
            }
            if (tt_strcmp(a->name, "namespace") != 0) {
                __1XON_FAIL;
            }
            if (tt_strcmp(a->value, "EmpComplex  Result") != 0) {
                __1XON_FAIL;
            }

            if (tt_xnode_next(a) != NULL) {
                __1XON_FAIL;
            }
            if (tt_xnode_last_attr(xn) != a) {
                __1XON_FAIL;
            }
        }
    } else {
        __1XON_FAIL;
    }

    __ut_xnp_seq++;
}

void __1_xnp_on_node_end(IN void *param, IN tt_xnode_t *xn)
{
    if (param != (void *)0x123) {
        __1XON_FAIL;
    }

    if (__ut_xnp_seq == 8) {
        // 9. node is a element end
        if (xn->type != TT_XNODE_TYPE_ELEMENT) {
            __1XON_FAIL;
        }
        if (tt_strcmp(xn->name, "sql&Map") != 0) {
            __1XON_FAIL;
        }
        if (xn->value != NULL) {
            __1XON_FAIL;
        }

        // check attr
        {
            tt_xnode_t *a = tt_xnode_first_attr(xn);

            if (a == NULL) {
                __1XON_FAIL;
            }
            if (tt_strcmp(a->name, "namespace") != 0) {
                __1XON_FAIL;
            }
            if (tt_strcmp(a->value, "EmpComplex  Result") != 0) {
                __1XON_FAIL;
            }

            if (tt_xnode_next(a) != NULL) {
                __1XON_FAIL;
            }
            if (tt_xnode_last_attr(xn) != a) {
                __1XON_FAIL;
            }
        }
    } else {
        __1XON_FAIL;
    }

    __ut_xnp_seq++;

    tt_xnode_destroy(xn);
}

void __1_xnp_on_node(IN void *param, IN tt_xnode_t *xn)
{
    if (param != (void *)0x123) {
        __1XON_FAIL;
    }

    if (__ut_xnp_seq == 0) {
        // 0. first node is a pi
        if (xn->type != TT_XNODE_TYPE_PI) {
            __1XON_FAIL;
        }
        if (tt_strcmp(xn->name, "xml") != 0) {
            __1XON_FAIL;
        }
        if (tt_strcmp(xn->value, "vers&amp;ion=\"1.0\" encoding=\"UTF-8\"") !=
            0) {
            __1XON_FAIL;
        }
    } else if (__ut_xnp_seq == 2) {
        // 2. node is a text
        if (xn->type != TT_XNODE_TYPE_TEXT) {
            __1XON_FAIL;
        }
        if (tt_strcmp(xn->value, "text & - \" - '") != 0) {
            __1XON_FAIL;
        }
        if (xn->name != NULL) {
            __1XON_FAIL;
        }
    } else if (__ut_xnp_seq == 3) {
        // 3. node is a elmt
        if (xn->type != TT_XNODE_TYPE_ELEMENT) {
            __1XON_FAIL;
        }
        if (tt_strcmp(xn->name, "typeAlias") != 0) {
            __1XON_FAIL;
        }
        if (xn->value != NULL) {
            __1XON_FAIL;
        }

        // check attr
        {
            tt_xnode_t *a = tt_xnode_first_attr(xn);

            if (a == NULL) {
                __1XON_FAIL;
            }
            if (tt_strcmp(a->name, "ali'as'") != 0) {
                __1XON_FAIL;
            }
            if (tt_strcmp(a->value, "empVO") != 0) {
                __1XON_FAIL;
            }

            a = tt_xnode_next(a);
            if (a == NULL) {
                __1XON_FAIL;
            }
            if (tt_strcmp(a->name, "type") != 0) {
                __1XON_FAIL;
            }
            if (tt_strcmp(a->value,
                          "egov'framework'.rte.psl.dataaccess.vo.EmpVO") != 0) {
                __1XON_FAIL;
            }

            a = tt_xnode_next(a);
            if (a != NULL) {
                __1XON_FAIL;
            }
        }
    } else if (__ut_xnp_seq == 4) {
        // 5. node is a text
        if (xn->type != TT_XNODE_TYPE_TEXT) {
            __1XON_FAIL;
        }
        if (tt_strcmp(xn->value, "text2 < />") != 0) {
            __1XON_FAIL;
        }
        if (xn->name != NULL) {
            __1XON_FAIL;
        }
    } else if (__ut_xnp_seq == 5) {
        // 6. node is a cdata
        if (xn->type != TT_XNODE_TYPE_CDATA) {
            __1XON_FAIL;
        }
        if (tt_strcmp(xn->value,
                      "this is cdata \ncan not escape &quot; &quot;") != 0) {
            __1XON_FAIL;
        }
        if (xn->name != NULL) {
            __1XON_FAIL;
        }
    } else if (__ut_xnp_seq == 6) {
        // 7. node is a text
        if (xn->type != TT_XNODE_TYPE_TEXT) {
            __1XON_FAIL;
        }
        if (tt_strcmp(xn->value, "text3 < />") != 0) {
            __1XON_FAIL;
        }
        if (xn->name != NULL) {
            __1XON_FAIL;
        }
    } else if (__ut_xnp_seq == 7) {
        // 8. node is a comment
        if (xn->type != TT_XNODE_TYPE_COMMENT) {
            __1XON_FAIL;
        }
        if (tt_strcmp(xn->value,
                      "this is comment \ncan not escape &quot; &quot;") != 0) {
            __1XON_FAIL;
        }
        if (xn->name != NULL) {
            __1XON_FAIL;
        }
    } else {
        __1XON_FAIL;
    }

    __ut_xnp_seq++;

    tt_xnode_destroy(xn);
}

void __1_xnp_on_error(IN void *param, IN tt_u32_t reserved)
{
    __ut_xnp_ret = TT_FAIL;
    __ut_xnp_err_line = __LINE__;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_xnp_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_xmlnp_t xnp;
    tt_result_t ret;
    tt_xmlnp_cb_t cb = {0};
    tt_u32_t i;
    tt_xmlmem_t xm;

    TT_TEST_CASE_ENTER()
    // test start

    ret = tt_xmlmem_create(&xm, NULL);
    TT_TEST_CHECK_SUCCESS(ret, "");

    cb.on_node_start = __1_xnp_on_node_start;
    cb.on_node_end = __1_xnp_on_node_end;
    cb.on_node = __1_xnp_on_node;
    cb.on_error = __1_xnp_on_error;

    ret = tt_xmlnp_create(&xnp, &xm, NULL, &cb, (void *)0x123, NULL);
    TT_TEST_CHECK_SUCCESS(ret, "");

    __ut_xnp_ret = TT_SUCCESS;
    __ut_xnp_err_line = 0;
    __ut_xnp_seq = 0;

    for (i = 0; i < sizeof((__test_xml));) {
        tt_u32_t len = sizeof(__test_xml) - i;
        if (len > 10) {
            len = tt_rand_u32() % 10; // 0-9
        }
        ret = tt_xmlnp_update(&xnp, (tt_u8_t *)&__test_xml[i], len);
        TT_TEST_CHECK_SUCCESS(ret, "");
        i += len;
    }
    TT_ASSERT(i == sizeof(__test_xml));

    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_SUCCESS(__ut_xnp_ret, "");

    ret = tt_xmlnp_final(&xnp, NULL);
    TT_TEST_CHECK_SUCCESS(ret, "");

    ///// random test
    for (i = 0; i < sizeof((__test_xml));) {
        tt_u32_t len = sizeof(__test_xml) - i, j;
        tt_u8_t buf[10];
        if (len > 10) {
            len = tt_rand_u32() % 10; // 0-9
        }

        __ut_xnp_seq = 0;
        tt_memcpy(buf, &__test_xml[i], len);
        for (j = 0; j < len; ++j) {
            __test_xml[i + j] = (tt_u8_t)tt_rand_u32();
        }
        tt_xmlnp_update(&xnp, (tt_u8_t *)__test_xml, sizeof(__test_xml));
        tt_memcpy(&__test_xml[i], buf, len);

        i += len;
    }
    ///// random test end

    tt_xmlnp_destroy(&xnp);
    tt_xmlmem_destroy(&xm);

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_char_t __test_xml_ns[] =
    "<tag1>"
    "  <ns1:tag2 xmlns:ns1 = \"ns1 uri\" xmlns = \"defns1\">"
    "    <tag3 xmlns:ns2 = \"ns2 &apos;\" xmlns = \"defns2\">"
    "      <tag4/>"
    "      <ns1:tag4-1/>"
    "      <ns2:tag4-2/>"
    "    </tag3>"
    "    <tag5/>"
    "  </ns1:tag2>"
    "  <ns1:tag6/>"
    "</tag1>"
    "";

#define __2XON_FAIL                                                            \
    __ut_xnp_ret = TT_FAIL;                                                    \
    __ut_xnp_err_line = __LINE__;                                              \
    return;

void __2_xnp_on_node_start(IN void *param, IN tt_xnode_t *xn)
{
    if (param != (void *)0x123) {
        __2XON_FAIL;
    }

    if (__ut_xnp_seq == 0) {
        if (xn->ns != NULL) {
            __2XON_FAIL
        }
    } else if (__ut_xnp_seq == 1) {
        if (xn->ns == NULL) {
            __2XON_FAIL
        }
        if (tt_strcmp(xn->name, "tag2") != 0) {
            __2XON_FAIL
        }
        if (tt_strcmp(xn->ns->prefix, "ns1") != 0) {
            __2XON_FAIL
        }
        if (tt_strcmp(xn->ns->uri, "ns1 uri") != 0) {
            __2XON_FAIL
        }
    } else if (__ut_xnp_seq == 2) {
        if (xn->ns == NULL) {
            __2XON_FAIL
        }
        if (tt_strcmp(xn->name, "tag3") != 0) {
            __2XON_FAIL
        }
        if (xn->ns->prefix != NULL) {
            __2XON_FAIL
        }
        if (tt_strcmp(xn->ns->uri, "defns2") != 0) {
            __2XON_FAIL
        }
    } else {
        __2XON_FAIL
    }

    __ut_xnp_seq++;
}

void __2_xnp_on_node_end(IN void *param, IN tt_xnode_t *xn)
{
    if (param != (void *)0x123) {
        __2XON_FAIL;
    }

    if (__ut_xnp_seq == 6) {
        // tag3 end
        if (xn->ns == NULL) {
            __2XON_FAIL
        }
        if (tt_strcmp(xn->name, "tag3") != 0) {
            __2XON_FAIL
        }
        if (xn->ns->prefix != NULL) {
            __2XON_FAIL
        }
        if (tt_strcmp(xn->ns->uri, "defns2") != 0) {
            __2XON_FAIL
        }
    } else if (__ut_xnp_seq == 8) {
        // tag2 end
        if (xn->ns == NULL) {
            __2XON_FAIL
        }
        if (tt_strcmp(xn->name, "tag2") != 0) {
            __2XON_FAIL
        }
        if (tt_strcmp(xn->ns->prefix, "ns1") != 0) {
            __2XON_FAIL
        }
        if (tt_strcmp(xn->ns->uri, "ns1 uri") != 0) {
            __2XON_FAIL
        }
    } else if (__ut_xnp_seq == 10) {
        // tag1 end
        if (xn->ns != NULL) {
            __2XON_FAIL
        }
    } else {
        __2XON_FAIL
    }

    __ut_xnp_seq++;

    tt_xnode_destroy(xn);
}

void __2_xnp_on_node(IN void *param, IN tt_xnode_t *xn)
{
    if (param != (void *)0x123) {
        __2XON_FAIL;
    }

    if (__ut_xnp_seq == 3) {
        if (xn->ns == NULL) {
            __2XON_FAIL
        }
        if (tt_strcmp(xn->name, "tag4") != 0) {
            __2XON_FAIL
        }
        if (xn->ns->prefix != NULL) {
            __2XON_FAIL
        }
        if (tt_strcmp(xn->ns->uri, "defns2") != 0) {
            __2XON_FAIL
        }
    } else if (__ut_xnp_seq == 4) {
        if (xn->ns == NULL) {
            __2XON_FAIL
        }
        if (tt_strcmp(xn->name, "tag4-1") != 0) {
            __2XON_FAIL
        }
        if (tt_strcmp(xn->ns->prefix, "ns1") != 0) {
            __2XON_FAIL
        }
        if (tt_strcmp(xn->ns->uri, "ns1 uri") != 0) {
            __2XON_FAIL
        }
    } else if (__ut_xnp_seq == 5) {
        if (xn->ns == NULL) {
            __2XON_FAIL
        }
        if (tt_strcmp(xn->name, "tag4-2") != 0) {
            __2XON_FAIL
        }
        if (tt_strcmp(xn->ns->prefix, "ns2") != 0) {
            __2XON_FAIL
        }
        if (tt_strcmp(xn->ns->uri, "ns2 '") != 0) {
            __2XON_FAIL
        }
    } else if (__ut_xnp_seq == 7) {
        if (xn->ns == NULL) {
            __2XON_FAIL
        }
        if (tt_strcmp(xn->name, "tag5") != 0) {
            __2XON_FAIL
        }
        if (xn->ns->prefix != NULL) {
            __2XON_FAIL
        }
        if (tt_strcmp(xn->ns->uri, "defns1") != 0) {
            __2XON_FAIL
        }
    } else if (__ut_xnp_seq == 9) {
        if (xn->ns != NULL) {
            __2XON_FAIL
        }
        if (tt_strcmp(xn->name, "ns1:tag6") != 0) {
            __2XON_FAIL
        }
    }

    __ut_xnp_seq++;

    tt_xnode_destroy(xn);
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_xnp_ns)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_xmlnp_t xnp;
    tt_result_t ret;
    tt_xmlnp_cb_t cb = {0};
    tt_u32_t i;
    tt_xmlmem_t xm;
    tt_xmlns_mgr_t xnmgr;

    TT_TEST_CASE_ENTER()
    // test start

    ret = tt_xmlmem_create(&xm, NULL);
    TT_TEST_CHECK_SUCCESS(ret, "");

    tt_xmlns_mgr_init(&xnmgr, &xm);

    cb.on_node_start = __2_xnp_on_node_start;
    cb.on_node_end = __2_xnp_on_node_end;
    cb.on_node = __2_xnp_on_node;
    cb.on_error = __1_xnp_on_error;

    ret = tt_xmlnp_create(&xnp, &xm, &xnmgr, &cb, (void *)0x123, NULL);
    TT_TEST_CHECK_SUCCESS(ret, "");

    __ut_xnp_ret = TT_SUCCESS;
    __ut_xnp_err_line = 0;
    __ut_xnp_seq = 0;

#if 1
    for (i = 0; i < sizeof((__test_xml_ns));) {
        tt_u32_t len = sizeof(__test_xml_ns) - i;
        if (len > 10) {
            len = tt_rand_u32() % 10; // 0-9
        }
        ret = tt_xmlnp_update(&xnp, (tt_u8_t *)&__test_xml_ns[i], len);
        TT_TEST_CHECK_SUCCESS(ret, "");
        i += len;
    }
    TT_ASSERT(i == sizeof(__test_xml_ns));
#else
    ret =
        tt_xmlnp_update(&xnp, (tt_u8_t *)__test_xml_ns, sizeof(__test_xml_ns));
#endif

    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_SUCCESS(__ut_xnp_ret, "");

    ret = tt_xmlnp_final(&xnp, NULL);
    TT_TEST_CHECK_SUCCESS(ret, "");

    ///// random test
    for (i = 0; i < sizeof((__test_xml_ns));) {
        tt_u32_t len = sizeof(__test_xml_ns) - i, j;
        tt_u8_t buf[5];
        if (len > 5) {
            len = tt_rand_u32() % 5; // 0-4
        }

        __ut_xnp_seq = 0;
        tt_memcpy(buf, &__test_xml_ns[i], len);
        for (j = 0; j < len; ++j) {
            __test_xml_ns[i + j] = (tt_u8_t)tt_rand_u32();
        }
        tt_xmlnp_update(&xnp, (tt_u8_t *)__test_xml_ns, sizeof(__test_xml_ns));
        tt_memcpy(&__test_xml_ns[i], buf, len);

        i += len;
    }
    ///// random test end

    tt_xmlnp_destroy(&xnp);
    tt_xmlns_mgr_destroy(&xnmgr);
    tt_xmlmem_destroy(&xm);

    // test end
    TT_TEST_CASE_LEAVE()
}

static void __3_xnp_on_node(IN void *param, IN tt_xnode_t *xn)
{
    tt_xnode_destroy(xn);
}

static void __3_xnp_on_error(IN void *param, IN tt_u32_t reserved)
{
    // expect on_error called
    __ut_xnp_ret = TT_SUCCESS;
    __ut_xnp_err_line = __LINE__;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_xnp_ns_etag)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_xmlnp_t xnp;
    tt_result_t ret;
    tt_xmlnp_cb_t cb = {0};
    tt_xmlmem_t xm;
    tt_xmlns_mgr_t xnmgr;

    TT_TEST_CASE_ENTER()
    // test start

    ret = tt_xmlmem_create(&xm, NULL);
    TT_TEST_CHECK_SUCCESS(ret, "");

    tt_xmlns_mgr_init(&xnmgr, &xm);

    cb.on_node = __3_xnp_on_node;
    cb.on_error = __3_xnp_on_error;

    ret = tt_xmlnp_create(&xnp, &xm, &xnmgr, &cb, (void *)0x123, NULL);
    TT_TEST_CHECK_SUCCESS(ret, "");

    {
        static tt_u8_t tx1[] =
            "<tag1 xmlns=\"def ns1\" xmlns:ns2=\"ns2\">\n"
            "    <tag2/>"
            "</ns2:tag1>";

        __ut_xnp_ret = TT_FAIL;
        __ut_xnp_err_line = 0;

        ret = tt_xmlnp_update(&xnp, tx1, sizeof(tx1));
        TT_TEST_CHECK_FAIL(ret, "");
        TT_TEST_CHECK_SUCCESS(__ut_xnp_ret, "");

        tt_xmlnp_reset(&xnp, 0);
        tt_xmlns_mgr_reset(&xnmgr);
    }

    {
        static tt_u8_t tx1[] =
            "<ns2:tag1 xmlns=\"def ns1\" xmlns:ns2=\"ns2\">\n"
            "    <tag2/>"
            "</tag1>";

        __ut_xnp_ret = TT_FAIL;
        __ut_xnp_err_line = 0;

        ret = tt_xmlnp_update(&xnp, tx1, sizeof(tx1));
        TT_TEST_CHECK_FAIL(ret, "");
        TT_TEST_CHECK_SUCCESS(__ut_xnp_ret, "");

        tt_xmlnp_reset(&xnp, 0);
        tt_xmlns_mgr_reset(&xnmgr);
    }

    {
        static tt_u8_t tx1[] =
            "<ns1:tag1 xmlns:ns1=\"def ns1\" xmlns:ns2=\"ns2\">\n"
            "    <tag2/>"
            "</ns2:tag1>";

        __ut_xnp_ret = TT_FAIL;
        __ut_xnp_err_line = 0;

        ret = tt_xmlnp_update(&xnp, tx1, sizeof(tx1));
        TT_TEST_CHECK_FAIL(ret, "");
        TT_TEST_CHECK_SUCCESS(__ut_xnp_ret, "");

        tt_xmlnp_reset(&xnp, 0);
        tt_xmlns_mgr_reset(&xnmgr);
    }

    tt_xmlnp_destroy(&xnp);
    tt_xmlns_mgr_destroy(&xnmgr);
    tt_xmlmem_destroy(&xm);

    // test end
    TT_TEST_CASE_LEAVE()
}

static void __4_xnp_on_node_1(IN void *param, IN tt_xnode_t *xn)
{
    tt_xnode_t *attr;

#define __4XON_FAIL                                                            \
    __ut_xnp_ret = TT_FAIL;                                                    \
    __ut_xnp_err_line = __LINE__;                                              \
    return;

    if (__ut_xnp_seq == 0) {
        if (tt_strcmp(xn->name, "ns3:tag1") != 0) {
            __4XON_FAIL;
        }

        attr = tt_xnode_first_attr(xn);
        if (attr == NULL) {
            __4XON_FAIL;
        }
        if (tt_strcmp(attr->name, "xmlnss:ns3") != 0) {
            __4XON_FAIL;
        }
        if (tt_strcmp(attr->value, "ns3") != 0) {
            __4XON_FAIL;
        }

        if ((attr = tt_xnode_next(attr)) != NULL) {
            __4XON_FAIL;
        }
    } else if (__ut_xnp_seq == 1) {
        tt_xnode_destroy(xn);
    } else if (__ut_xnp_seq == 2) {
        if (tt_strcmp(xn->name, "ns3:tag1") != 0) {
            __4XON_FAIL;
        }

        attr = tt_xnode_first_attr(xn);
        if (tt_strcmp(attr->name, "xmlnss:ns3") != 0) {
            __4XON_FAIL;
        }
        if (tt_strcmp(attr->value, "ns3") != 0) {
            __4XON_FAIL;
        }

        if ((attr = tt_xnode_next(xn)) != NULL) {
            __4XON_FAIL;
        }

        tt_xnode_destroy(xn);
    } else {
        __4XON_FAIL;
    }

    ++__ut_xnp_seq;
}

static void __4_xnp_on_node_2(IN void *param, IN tt_xnode_t *xn)
{
    if (__ut_xnp_seq == 0 || __ut_xnp_seq == 3) {
        if (tt_strcmp(xn->name, "tag1") != 0) {
            __4XON_FAIL;
        }

        if (xn->ns == NULL) {
            __4XON_FAIL;
        }

        if (strcmp(xn->ns->prefix, "ns2") != 0) {
            __4XON_FAIL;
        }
        if (strcmp(xn->ns->uri, "ns2") != 0) {
            __4XON_FAIL;
        }

        if (__ut_xnp_seq == 3) {
            tt_xnode_destroy(xn);
        }
    } else if (__ut_xnp_seq == 1) {
        if (tt_strcmp(xn->name, "tag2") != 0) {
            __4XON_FAIL;
        }

        if (xn->ns == NULL) {
            __4XON_FAIL;
        }

        if (strcmp(xn->ns->prefix, "ns2") != 0) {
            __4XON_FAIL;
        }
        if (strcmp(xn->ns->uri, "sub ns2") != 0) {
            __4XON_FAIL;
        }
        tt_xnode_destroy(xn);
    } else if (__ut_xnp_seq == 2) {
        if (tt_strcmp(xn->name, "tag3") != 0) {
            __4XON_FAIL;
        }

        if (xn->ns == NULL) {
            __4XON_FAIL;
        }

        if (strcmp(xn->ns->prefix, "ns2") != 0) {
            __4XON_FAIL;
        }
        if (strcmp(xn->ns->uri, "sub sub ns2") != 0) {
            __4XON_FAIL;
        }
        tt_xnode_destroy(xn);
    } else {
        __4XON_FAIL;
    }

    ++__ut_xnp_seq;
}

static void __4_xnp_on_node_3(IN void *param, IN tt_xnode_t *xn)
{
    if (__ut_xnp_seq == 0) {
        if (tt_strcmp(xn->name, ":tag1") != 0) {
            __4XON_FAIL;
        }
    } else if (__ut_xnp_seq == 1) {
        if (tt_strcmp(xn->name, "tag2:") != 0) {
            __4XON_FAIL;
        }
    } else if (__ut_xnp_seq == 2) {
        if (tt_strcmp(xn->name, "tag2:") != 0) {
            __4XON_FAIL;
        }
        tt_xnode_destroy(xn);
    } else if (__ut_xnp_seq == 3) {
        if (tt_strcmp(xn->name, ":tag1") != 0) {
            __4XON_FAIL;
        }
        tt_xnode_destroy(xn);
    } else {
        __4XON_FAIL;
    }

    ++__ut_xnp_seq;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_xnp_ns_invalid)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_xmlnp_t xnp;
    tt_result_t ret;
    tt_xmlnp_cb_t cb = {0};
    tt_xmlmem_t xm;
    tt_xmlns_mgr_t xnmgr;

    TT_TEST_CASE_ENTER()
    // test start

    ret = tt_xmlmem_create(&xm, NULL);
    TT_TEST_CHECK_SUCCESS(ret, "");

    tt_xmlns_mgr_init(&xnmgr, &xm);

    cb.on_node_start = __4_xnp_on_node_1;
    cb.on_node_end = __4_xnp_on_node_1;
    cb.on_node = __4_xnp_on_node_1;
    cb.on_error = __1_xnp_on_error;

    ret = tt_xmlnp_create(&xnp, &xm, &xnmgr, &cb, (void *)0x123, NULL);
    TT_TEST_CHECK_SUCCESS(ret, "");

    __ut_xnp_ret = TT_SUCCESS;
    __ut_xnp_err_line = 0;

    // unexisted ns
    {
        static tt_u8_t tx1[] =
            "<ns3:tag1 xmlns=\"def ns1\" xmlns:ns2=\"ns2\" "
            "xmlnss:ns3=\"ns3\">\n"
            "    <tag2/>"
            "</ns3:tag1>";

        __ut_xnp_seq = 0;

        ret = tt_xmlnp_update(&xnp, tx1, sizeof(tx1));
        TT_TEST_CHECK_SUCCESS(ret, "");
        TT_TEST_CHECK_SUCCESS(__ut_xnp_ret, "");

        tt_xmlnp_reset(&xnp, 0);
        tt_xmlns_mgr_reset(&xnmgr);
    }

    // ns overwrite
    {
        static tt_u8_t tx1[] =
            "<ns2:tag1 xmlns=\"def ns1\" xmlns:ns2=\"ns2\">\n"
            "    <ns2:tag2 xmlns:ns2=\"sub ns2\"/>"
            "    <ns2:tag3 xmlns:ns2=\"sub sub ns2\"/>"
            "</ns2:tag1>";

        __ut_xnp_seq = 0;

        xnp.cb.on_node_start = __4_xnp_on_node_2;
        xnp.cb.on_node_end = __4_xnp_on_node_2;
        xnp.cb.on_node = __4_xnp_on_node_2;

        ret = tt_xmlnp_update(&xnp, tx1, sizeof(tx1));
        TT_TEST_CHECK_SUCCESS(ret, "");
        TT_TEST_CHECK_SUCCESS(__ut_xnp_ret, "");

        tt_xmlnp_reset(&xnp, 0);
        tt_xmlns_mgr_reset(&xnmgr);
    }

    // no prefix or name
    {
        static tt_u8_t tx1[] =
            "<:tag1 xmlns=\"def ns1\" xmlns:ns2=\"ns2\">\n"
            "    <tag2: xmlns:tag2=\"  tag2 \"></tag2:>"
            "</:tag1>";

        __ut_xnp_seq = 0;

        xnp.cb.on_node_start = __4_xnp_on_node_3;
        xnp.cb.on_node_end = __4_xnp_on_node_3;
        xnp.cb.on_node = __4_xnp_on_node_3;

        ret = tt_xmlnp_update(&xnp, tx1, sizeof(tx1));
        TT_TEST_CHECK_SUCCESS(ret, "");
        TT_TEST_CHECK_SUCCESS(__ut_xnp_ret, "");

        tt_xmlnp_reset(&xnp, 0);
        tt_xmlns_mgr_reset(&xnmgr);
    }

    // an incomplete xml
    {
        static tt_u8_t tx1[] =
            "<:tag1 xmlns=\"def ns1\" xmlns:ns2=\"ns2\">\n"
            "    <tag2: xmlns:tag2=\"  tag2 \"></tag2:>";

        __ut_xnp_seq = 0;

        xnp.cb.on_node_start = __4_xnp_on_node_3;
        xnp.cb.on_node_end = __4_xnp_on_node_3;
        xnp.cb.on_node = __4_xnp_on_node_3;

        ret = tt_xmlnp_update(&xnp, tx1, sizeof(tx1));
        TT_TEST_CHECK_SUCCESS(ret, "");
        TT_TEST_CHECK_SUCCESS(__ut_xnp_ret, "");

        ret = tt_xmlnp_final(&xnp, NULL);
        TT_TEST_CHECK_FAIL(ret, "");

        tt_xmlnp_reset(&xnp, 0);
        tt_xmlns_mgr_reset(&xnmgr);
    }

    tt_xmlnp_destroy(&xnp);
    tt_xmlns_mgr_destroy(&xnmgr);
    tt_xmlmem_destroy(&xm);

    // test end
    TT_TEST_CASE_LEAVE()
}
