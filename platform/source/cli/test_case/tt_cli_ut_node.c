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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_cli_cfgnode)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_cli_cfgnode_u32)

// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(cli_node_case)

TT_TEST_CASE("tt_unit_test_clinode",
             "cli test node",
             tt_unit_test_cli_cfgnode,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_cli_cfgnode_u32",
                 "cli test node, u32",
                 tt_unit_test_cli_cfgnode_u32,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(cli_node_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(CLI_UT_CFGNODE, 0, cli_node_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(tt_unit_test_cli_cfgnode_u32)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(tt_unit_test_cli_cfgnode)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_cfgnode_t *cnode;
    tt_cfgnode_attr_t attr;
    tt_blob_t name = {(tt_u8_t *)&attr, 2};
    tt_blob_t val = {(tt_u8_t *)&attr, 3};
    tt_buf_t out;

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&out, NULL);

    tt_cfgnode_attr_default(&attr);
    attr.display_name = "display_name";
    attr.brief = "info";
    attr.detail = "usage";

    cnode = tt_cfgnode_create(1,
                              TT_CFGNODE_TYPE_STRING,
                              "name",
                              NULL,
                              (void *)1,
                              &attr);
    TT_UT_NOT_EQUAL(cnode, NULL, "");

    TT_UT_EQUAL(cnode->type, TT_CFGNODE_TYPE_STRING, "");
    TT_UT_EQUAL(cnode->opaque, (void *)1, "");
    TT_UT_EQUAL(cnode->itf, NULL, "");
    TT_UT_EQUAL(tt_strcmp(cnode->name, "name"), 0, "");
    TT_UT_EQUAL(tt_strcmp(cnode->display_name, "display_name"), 0, "");
    TT_UT_EQUAL(tt_strcmp(cnode->brief, "info"), 0, "");
    TT_UT_EQUAL(tt_strcmp(cnode->detail, "usage"), 0, "");
    TT_UT_EQUAL(cnode->removing, TT_FALSE, "");
    TT_UT_EQUAL(cnode->modified, TT_FALSE, "");

#if 0
    // may fail, but should not crash
    tt_cfgnode_add(cnode, &name, &val);
    tt_cfgnode_rm(cnode, &name);
    tt_cfgnode_ls(cnode, &out);
    tt_cfgnode_get(cnode, &out);
    tt_cfgnode_set(cnode, &val);
    tt_cfgnode_check(cnode, &val);
#endif

    tt_cfgnode_destroy(cnode, TT_TRUE);
    tt_buf_destroy(&out);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_cli_cfgnode_u32)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_cfgnode_t *cnode;
    tt_u32_t val = 100;
    tt_result_t ret;
    tt_u8_t c[] = "u32";
    tt_blob_t n = {c, sizeof(c)};
    tt_blob_t v = {c, sizeof(c)};
    tt_buf_t out;

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&out, NULL);

    cnode = tt_cfgu32_create("", NULL, &val, NULL, NULL, NULL);
    TT_UT_NOT_EQUAL(cnode, NULL, "");

    ret = tt_cfgnode_add(cnode, &n, &v);
    TT_UT_EQUAL(ret, TT_BAD_PARAM, "");
    ret = tt_cfgnode_rm(cnode, &n);
    TT_UT_EQUAL(ret, TT_BAD_PARAM, "");

    tt_buf_clear(&out);
    ret = tt_cfgnode_ls(cnode, NULL, &out);

    tt_cfgnode_destroy(cnode, TT_TRUE);
    tt_buf_destroy(&out);

    // test end
    TT_TEST_CASE_LEAVE()
}
