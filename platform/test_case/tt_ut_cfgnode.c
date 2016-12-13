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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_cfgnode)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_cfgnode_u32)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_cfgnode_s32)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_cfgnode_str)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_cfgnode_grp)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_cfgnode_grp_ar)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_cfgnode_bool)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(cli_node_case)

TT_TEST_CASE("tt_unit_test_clinode",
             "config node",
             tt_unit_test_cfgnode,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_cfgnode_u32",
                 "config node, u32",
                 tt_unit_test_cfgnode_u32,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_cfgnode_s32",
                 "config node, s32",
                 tt_unit_test_cfgnode_s32,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_cfgnode_str",
                 "config node, string",
                 tt_unit_test_cfgnode_str,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_cfgnode_grp",
                 "config node, group",
                 tt_unit_test_cfgnode_grp,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_cfgnode_grp_ar",
                 "config node, group add/rm",
                 tt_unit_test_cfgnode_grp_ar,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_cfgnode_bool",
                 "config node, bool",
                 tt_unit_test_cfgnode_bool,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(cli_node_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(TEST_UNIT_CFGNODE, 0, cli_node_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(tt_unit_test_cfgpath_basic)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(tt_unit_test_cfgnode)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_cfgnode_t *cnode;
    tt_cfgnode_attr_t attr;
    tt_blob_t name = {(tt_u8_t *)&attr, 2};
    tt_blob_t val = {(tt_u8_t *)&attr, 3};
    tt_buf_t out;
    tt_cfgnode_itf_t itf = {0};

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
                              &itf,
                              (void *)1,
                              &attr);
    TT_TEST_CHECK_NOT_EQUAL(cnode, NULL, "");

    TT_TEST_CHECK_EQUAL(cnode->type, TT_CFGNODE_TYPE_STRING, "");
    TT_TEST_CHECK_EQUAL(cnode->opaque, (void *)1, "");
    TT_TEST_CHECK_EQUAL(cnode->itf, &itf, "");
    TT_TEST_CHECK_EQUAL(tt_strcmp(cnode->name, "name"), 0, "");
    TT_TEST_CHECK_EQUAL(tt_strcmp(cnode->display_name, "display_name"), 0, "");
    TT_TEST_CHECK_EQUAL(tt_strcmp(cnode->brief, "info"), 0, "");
    TT_TEST_CHECK_EQUAL(tt_strcmp(cnode->detail, "usage"), 0, "");
    TT_TEST_CHECK_EQUAL(cnode->removing, TT_FALSE, "");
    TT_TEST_CHECK_EQUAL(cnode->modified, TT_FALSE, "");

    // may fail, but should not crash
    tt_cfgnode_add(cnode, &name, &val);
    tt_cfgnode_rm(cnode, &name);
    tt_cfgnode_ls(cnode, NULL, &out);
    tt_cfgnode_get(cnode, &out);
    tt_cfgnode_set(cnode, &val);
    tt_cfgnode_check(cnode, &val);
    tt_cfgnode_commit(cnode);

    tt_cfgnode_destroy(cnode, TT_TRUE);
    tt_buf_destroy(&out);

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_u32_t __ut_err_line;
static tt_bool_t __ut_on_destroy_ok;
static tt_bool_t __ut_cb_called;

static tt_u32_t __ut_u32_set;
static tt_string_t __ut_str_set;
static tt_s32_t __ut_s32_set;
static tt_bool_t __ut_bool_set;

static void __val_on_destroy(IN struct tt_cfgnode_s *cnode,
                             IN tt_bool_t committed)
{
    if (__ut_on_destroy_ok) {
        __ut_err_line = __LINE__;
        return;
    }

    if (!committed) {
        __ut_err_line = __LINE__;
        return;
    }

    __ut_on_destroy_ok = TT_TRUE;
}

static tt_bool_t __u32_on_set(IN struct tt_cfgnode_s *cnode,
                              IN tt_u32_t new_val)
{
    __ut_cb_called = TT_TRUE;
    __ut_u32_set = new_val;
    return TT_TRUE;
}

static tt_bool_t __s32_on_set(IN struct tt_cfgnode_s *cnode,
                              IN tt_s32_t new_val)
{
    __ut_cb_called = TT_TRUE;
    __ut_s32_set = new_val;
    return TT_TRUE;
}

static tt_bool_t __str_on_set(IN struct tt_cfgnode_s *cnode,
                              IN tt_string_t *new_val)
{
    __ut_cb_called = TT_TRUE;
    tt_string_copy(&__ut_str_set, new_val);
    return TT_TRUE;
}

static tt_bool_t __bool_on_set(IN struct tt_cfgnode_s *cnode,
                               IN tt_bool_t new_val)
{
    __ut_cb_called = TT_TRUE;
    __ut_bool_set = new_val;
    return TT_TRUE;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_cfgnode_u32)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_cfgnode_t *cnode;
    tt_u32_t val = 0;
    tt_result_t ret;
    tt_u8_t c[] = "u32";
    tt_buf_t out;
    tt_s32_t cmp_ret;
    const tt_char_t *max_u32 = "4294967295";
    const tt_char_t *invalid_u32 = "4294967296";
    const tt_char_t *invalid_u32_2 = "-94967295";
    const tt_char_t *u32_0 = "0";
    tt_blob_t n = {c, sizeof(c)}, v = {c, sizeof(c)};
    tt_cfgu32_attr_t attr;
    tt_cfgu32_cb_t cb = {__val_on_destroy, __u32_on_set};

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&out, NULL);

    cnode = tt_cfgu32_create("", NULL, NULL, &val, &cb, NULL);
    TT_TEST_CHECK_NOT_EQUAL(cnode, NULL, "");

    // add
    ret = tt_cfgnode_add(cnode, &n, &v);
    TT_TEST_CHECK_EQUAL(ret, TT_BAD_PARAM, "");

    // rm
    ret = tt_cfgnode_rm(cnode, &n);
    TT_TEST_CHECK_EQUAL(ret, TT_BAD_PARAM, "");

    // ls
    {
        const tt_char_t *this_out =
            "PERM    TYPE    NAME    DESCRIPTION\n"
            "--g-    u32             ";

        tt_buf_clear(&out);
        ret = tt_cfgnode_ls(cnode, NULL, &out);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&out, this_out);
        TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    }

    // get
    tt_buf_clear(&out);
    val = 0;
    ret = tt_cfgnode_get(cnode, &out);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&out, "0");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    // set
    ret = tt_cfgnode_set(cnode, &v);
    TT_TEST_CHECK_EQUAL(ret, TT_BAD_PARAM, "");

    // check
    v.addr = (tt_u8_t *)max_u32;
    v.len = (tt_u32_t)tt_strlen(max_u32);
    ret = tt_cfgnode_check(cnode, &v);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    v.addr = (tt_u8_t *)invalid_u32;
    v.len = (tt_u32_t)tt_strlen(invalid_u32);
    ret = tt_cfgnode_check(cnode, &v);
    TT_TEST_CHECK_EQUAL(ret, TT_BAD_PARAM, "");

    // commit
    ret = tt_cfgnode_commit(cnode);
    TT_TEST_CHECK_EQUAL(ret, TT_BAD_PARAM, "");

    __ut_err_line = 0;
    __ut_on_destroy_ok = TT_FALSE;
    tt_cfgnode_destroy(cnode, TT_FALSE);
    TT_TEST_CHECK_EQUAL(__ut_err_line, 0, "");
    TT_TEST_CHECK_EQUAL(__ut_on_destroy_ok, TT_FALSE, "");

    // node 2
    tt_cfgu32_attr_default(&attr);
    attr.cnode_attr.brief = "test node 2";
    attr.mode = TT_CFGVAL_MODE_GS;

    cnode = tt_cfgu32_create("node2", NULL, NULL, &val, &cb, &attr);
    TT_TEST_CHECK_NOT_EQUAL(cnode, NULL, "");

    // add
    ret = tt_cfgnode_add(cnode, &n, &v);
    TT_TEST_CHECK_EQUAL(ret, TT_BAD_PARAM, "");

    // rm
    ret = tt_cfgnode_rm(cnode, &n);
    TT_TEST_CHECK_EQUAL(ret, TT_BAD_PARAM, "");

    // ls
    {
        const tt_char_t *this_out =
            "PERM    TYPE    NAME     DESCRIPTION\n"
            "--gs    u32     node2    test node 2";

        tt_buf_clear(&out);
        ret = tt_cfgnode_ls(cnode, NULL, &out);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&out, this_out);
        TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    }

    tt_buf_clear(&out);
    ret = tt_cfgnode_describe(cnode, 0, &out);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&out, "--gs    u32     node2    test node 2");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    tt_buf_clear(&out);
    ret = tt_cfgnode_describe(cnode, 3, &out);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&out, "--gs    u32     ???    test node 2");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    tt_buf_clear(&out);
    ret = tt_cfgnode_describe(cnode, 10, &out);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret =
        tt_buf_cmp_cstr(&out, "--gs    u32     node2         test node 2");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    // get
    tt_buf_clear(&out);
    val = ~0;
    ret = tt_cfgnode_get(cnode, &out);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&out, "4294967295");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    // set
    tt_buf_clear(&out);
    v.addr = (tt_u8_t *)invalid_u32;
    v.len = (tt_u32_t)tt_strlen(invalid_u32);
    ret = tt_cfgnode_set(cnode, &v);
    TT_TEST_CHECK_EQUAL(ret, TT_BAD_PARAM, "");

    v.addr = (tt_u8_t *)invalid_u32_2;
    v.len = (tt_u32_t)tt_strlen(invalid_u32_2);
    ret = tt_cfgnode_set(cnode, &v);
    TT_TEST_CHECK_EQUAL(ret, TT_BAD_PARAM, "");

    v.addr = (tt_u8_t *)u32_0;
    v.len = (tt_u32_t)tt_strlen(u32_0);
    ret = tt_cfgnode_set(cnode, &v);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    tt_buf_clear(&out);
    ret = tt_cfgnode_get(cnode, &out);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&out, "4294967295 --> 0");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    v.addr = (tt_u8_t *)max_u32;
    v.len = (tt_u32_t)tt_strlen(max_u32);
    ret = tt_cfgnode_set(cnode, &v);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    tt_buf_clear(&out);
    ret = tt_cfgnode_get(cnode, &out);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&out, "4294967295");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    // commit
    __ut_cb_called = TT_FALSE;
    ret = tt_cfgnode_commit(cnode);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(__ut_cb_called, TT_FALSE, "");

    v.addr = (tt_u8_t *)u32_0;
    v.len = (tt_u32_t)tt_strlen(u32_0);
    ret = tt_cfgnode_set(cnode, &v);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    __ut_u32_set = 123;
    ret = tt_cfgnode_commit(cnode);
    TT_TEST_CHECK_EQUAL(ret, TT_END, "");
    TT_TEST_CHECK_EQUAL(__ut_cb_called, TT_TRUE, "");
    TT_TEST_CHECK_EQUAL(__ut_u32_set, 0, "");

    __ut_err_line = 0;
    __ut_on_destroy_ok = TT_FALSE;
    tt_cfgnode_destroy(cnode, TT_TRUE);
    TT_TEST_CHECK_EQUAL(__ut_err_line, 0, "");
    TT_TEST_CHECK_EQUAL(__ut_on_destroy_ok, TT_TRUE, "");

    tt_buf_destroy(&out);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_cfgnode_s32)
{
    // tt_s32_t param = TT_TEST_ROUTINE_PARAM(tt_s32_t);
    tt_cfgnode_t *cnode;
    tt_s32_t val = 0;
    tt_result_t ret;
    tt_u8_t c[] = "s32";
    tt_buf_t out;
    tt_s32_t cmp_ret;
    const tt_char_t *max_s32 = "2147483647";
    const tt_char_t *min_s32 = "-2147483648";
    const tt_char_t *invalid_s32 = "2147483648";
    const tt_char_t *invalid_s32_2 = "-2147483649";
    const tt_char_t *s32_n1 = "  -1";
    tt_blob_t n = {c, sizeof(c)}, v = {c, sizeof(c)};
    tt_cfgs32_attr_t attr;
    tt_cfgs32_cb_t cb = {__val_on_destroy, __s32_on_set};

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&out, NULL);

    cnode = tt_cfgs32_create("", NULL, NULL, &val, &cb, NULL);
    TT_TEST_CHECK_NOT_EQUAL(cnode, NULL, "");

    // add
    ret = tt_cfgnode_add(cnode, &n, &v);
    TT_TEST_CHECK_EQUAL(ret, TT_BAD_PARAM, "");

    // rm
    ret = tt_cfgnode_rm(cnode, &n);
    TT_TEST_CHECK_EQUAL(ret, TT_BAD_PARAM, "");

    // ls
    {
        const tt_char_t *this_out =
            "PERM    TYPE    NAME    DESCRIPTION\n"
            "--g-    s32             ";

        tt_buf_clear(&out);
        ret = tt_cfgnode_ls(cnode, NULL, &out);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&out, this_out);
        TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    }

    // get
    tt_buf_clear(&out);
    ret = tt_cfgnode_get(cnode, &out);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&out, "0");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    // set
    ret = tt_cfgnode_set(cnode, &v);
    TT_TEST_CHECK_EQUAL(ret, TT_BAD_PARAM, "");

    // check
    v.addr = (tt_u8_t *)max_s32;
    v.len = (tt_u32_t)tt_strlen(max_s32);
    ret = tt_cfgnode_check(cnode, &v);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    v.addr = (tt_u8_t *)invalid_s32;
    v.len = (tt_u32_t)tt_strlen(invalid_s32);
    ret = tt_cfgnode_check(cnode, &v);
    TT_TEST_CHECK_EQUAL(ret, TT_BAD_PARAM, "");

    ret = tt_cfgnode_commit(cnode);
    TT_TEST_CHECK_EQUAL(ret, TT_BAD_PARAM, "");

    tt_cfgnode_destroy(cnode, TT_TRUE);

    // node 2
    tt_cfgs32_attr_default(&attr);
    attr.cnode_attr.brief = "test node 2";
    attr.mode = TT_CFGVAL_MODE_GS;

    cnode = tt_cfgs32_create("node1234567890", NULL, NULL, &val, &cb, &attr);
    TT_TEST_CHECK_NOT_EQUAL(cnode, NULL, "");

    // add
    ret = tt_cfgnode_add(cnode, &n, &v);
    TT_TEST_CHECK_EQUAL(ret, TT_BAD_PARAM, "");

    // rm
    ret = tt_cfgnode_rm(cnode, &n);
    TT_TEST_CHECK_EQUAL(ret, TT_BAD_PARAM, "");

    // ls
    {
        const tt_char_t *this_out =
            "PERM    TYPE    NAME              DESCRIPTION\n"
            "--gs    s32     node1234567890    test node 2";

        tt_buf_clear(&out);
        ret = tt_cfgnode_ls(cnode, NULL, &out);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&out, this_out);
        TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    }


    {
        const tt_char_t *this_out =
            "--gs    s32     node1234567890    test node 2";

        tt_buf_clear(&out);
        ret = tt_cfgnode_describe(cnode, 0, &out);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&out, this_out);
        TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    }

    {
        const tt_char_t *this_out = "--gs    s32     ???????    test node 2";

        tt_buf_clear(&out);
        ret = tt_cfgnode_describe(cnode, 7, &out);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&out, this_out);
        TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    }

    {
        const tt_char_t *this_out = "--gs    s32     ????????    test node 2";

        tt_buf_clear(&out);
        ret = tt_cfgnode_describe(cnode, 8, &out);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&out, this_out);
        TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    }

    {
        const tt_char_t *this_out =
            "--gs    s32     node1234567890      test node 2";

        tt_buf_clear(&out);
        ret = tt_cfgnode_describe(cnode, 16, &out);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&out, this_out);
        TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    }

    // get
    val = ~0;
    tt_buf_clear(&out);
    ret = tt_cfgnode_get(cnode, &out);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&out, "-1");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    // set
    v.addr = (tt_u8_t *)max_s32;
    v.len = (tt_u32_t)tt_strlen(max_s32);
    ret = tt_cfgnode_set(cnode, &v);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    tt_buf_clear(&out);
    ret = tt_cfgnode_get(cnode, &out);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&out, "-1 --> 2147483647");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    tt_buf_clear(&out);
    v.addr = (tt_u8_t *)invalid_s32;
    v.len = (tt_u32_t)tt_strlen(invalid_s32);
    ret = tt_cfgnode_set(cnode, &v);
    TT_TEST_CHECK_EQUAL(ret, TT_BAD_PARAM, "");

    v.addr = (tt_u8_t *)invalid_s32_2;
    v.len = (tt_u32_t)tt_strlen(invalid_s32_2);
    ret = tt_cfgnode_set(cnode, &v);
    TT_TEST_CHECK_EQUAL(ret, TT_BAD_PARAM, "");

    // to -2147483648
    v.addr = (tt_u8_t *)min_s32;
    v.len = (tt_u32_t)tt_strlen(min_s32);
    ret = tt_cfgnode_set(cnode, &v);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    tt_buf_clear(&out);
    ret = tt_cfgnode_get(cnode, &out);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&out, "-1 --> -2147483648");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    // to -1
    v.addr = (tt_u8_t *)s32_n1;
    v.len = (tt_u32_t)tt_strlen(s32_n1);
    ret = tt_cfgnode_set(cnode, &v);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    tt_buf_clear(&out);
    ret = tt_cfgnode_get(cnode, &out);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&out, "-1");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    // commit
    __ut_cb_called = TT_FALSE;
    ret = tt_cfgnode_commit(cnode);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(__ut_cb_called, TT_FALSE, "");

    v.addr = (tt_u8_t *)min_s32;
    v.len = (tt_u32_t)tt_strlen(min_s32);
    ret = tt_cfgnode_set(cnode, &v);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    __ut_cb_called = TT_FALSE;
    ret = tt_cfgnode_commit(cnode);
    TT_TEST_CHECK_EQUAL(ret, TT_END, "");
    TT_TEST_CHECK_EQUAL(__ut_cb_called, TT_TRUE, "");
    TT_TEST_CHECK_EQUAL(__ut_s32_set, (tt_s32_t)-2147483648L, "");

    tt_cfgnode_destroy(cnode, TT_TRUE);
    tt_buf_destroy(&out);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_cfgnode_str)
{
    // tt_s32_t param = TT_TEST_ROUTINE_PARAM(tt_s32_t);
    tt_cfgnode_t *cnode;
    tt_result_t ret;
    tt_u8_t c[] = "s32";
    tt_buf_t out;
    tt_string_t val;
    tt_s32_t cmp_ret;
    const tt_char_t *max_s32 = "2147483647";
    const tt_char_t *min_s32 = "-2147483648";
    tt_char_t invalid_str[] = {
        'a', 'b', 0x1,
    };
    const tt_char_t *invalid_s32_2 = "-2147483649";
    const tt_char_t *test_str = "test string";
    tt_blob_t n = {c, sizeof(c)}, v = {c, sizeof(c)};
    tt_cfgstr_attr_t attr;
    tt_cfgstr_cb_t cb = {NULL, __str_on_set};

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&out, NULL);
    tt_string_init(&val, NULL);
    tt_string_init(&__ut_str_set, NULL);

    cnode = tt_cfgstr_create("", NULL, NULL, &val, &cb, NULL);
    TT_TEST_CHECK_NOT_EQUAL(cnode, NULL, "");

    // add
    ret = tt_cfgnode_add(cnode, &n, &v);
    TT_TEST_CHECK_EQUAL(ret, TT_BAD_PARAM, "");

    // rm
    ret = tt_cfgnode_rm(cnode, &n);
    TT_TEST_CHECK_EQUAL(ret, TT_BAD_PARAM, "");

    // ls
    {
        const tt_char_t *this_out = "--g-    str         ";

        tt_buf_clear(&out);
        ret = tt_cfgnode_describe(cnode, 0, &out);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&out, this_out);
        TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    }

    // get
    tt_buf_clear(&out);
    ret = tt_cfgnode_get(cnode, &out);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&out, "\"\"");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    // set
    ret = tt_cfgnode_set(cnode, &v);
    TT_TEST_CHECK_EQUAL(ret, TT_BAD_PARAM, "");

    // check
    v.addr = (tt_u8_t *)max_s32;
    v.len = (tt_u32_t)tt_strlen(max_s32);
    ret = tt_cfgnode_check(cnode, &v);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    v.addr = (tt_u8_t *)invalid_str;
    v.len = (tt_u32_t)tt_strlen(invalid_str);
    ret = tt_cfgnode_check(cnode, &v);
    TT_TEST_CHECK_EQUAL(ret, TT_BAD_PARAM, "");

    // rm
    ret = tt_cfgnode_commit(cnode);
    TT_TEST_CHECK_EQUAL(ret, TT_BAD_PARAM, "");

    tt_cfgnode_destroy(cnode, TT_TRUE);

    // node 2
    tt_cfgstr_attr_default(&attr);
    attr.cnode_attr.brief = "test node 2";
    attr.mode = TT_CFGVAL_MODE_GS;

    cnode = tt_cfgstr_create("node2", NULL, NULL, &val, &cb, &attr);
    TT_TEST_CHECK_NOT_EQUAL(cnode, NULL, "");

    // add
    ret = tt_cfgnode_add(cnode, &n, &v);
    TT_TEST_CHECK_EQUAL(ret, TT_BAD_PARAM, "");

    // rm
    ret = tt_cfgnode_rm(cnode, &n);
    TT_TEST_CHECK_EQUAL(ret, TT_BAD_PARAM, "");

    // ls
    {
        const tt_char_t *this_out = "--gs    str     node2    test node 2";

        tt_buf_clear(&out);
        ret = tt_cfgnode_describe(cnode, 0, &out);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&out, this_out);
        TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    }

    // get
    tt_string_clear(&val);
    tt_string_append(&val, test_str);
    tt_buf_clear(&out);
    ret = tt_cfgnode_get(cnode, &out);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&out, "\"test string\"");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    // set
    v.addr = (tt_u8_t *)max_s32;
    v.len = (tt_u32_t)tt_strlen(max_s32);
    ret = tt_cfgnode_set(cnode, &v);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    tt_buf_clear(&out);
    ret = tt_cfgnode_get(cnode, &out);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&out, "\"test string\" --> \"2147483647\"");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    v.addr = (tt_u8_t *)invalid_s32_2;
    v.len = (tt_u32_t)tt_strlen(invalid_s32_2);
    ret = tt_cfgnode_set(cnode, &v);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    tt_buf_clear(&out);
    ret = tt_cfgnode_get(cnode, &out);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&out, "\"test string\" --> \"-2147483649\"");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    v.addr = (tt_u8_t *)test_str;
    v.len = (tt_u32_t)tt_strlen(test_str);
    ret = tt_cfgnode_set(cnode, &v);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    tt_buf_clear(&out);
    ret = tt_cfgnode_get(cnode, &out);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&out, "\"test string\"");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    // commit
    __ut_cb_called = TT_FALSE;
    ret = tt_cfgnode_commit(cnode);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(__ut_cb_called, TT_FALSE, "");

    v.addr = (tt_u8_t *)invalid_s32_2;
    v.len = (tt_u32_t)tt_strlen(invalid_s32_2);
    ret = tt_cfgnode_set(cnode, &v);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    __ut_cb_called = TT_FALSE;
    ret = tt_cfgnode_commit(cnode);
    TT_TEST_CHECK_EQUAL(ret, TT_END, "");
    TT_TEST_CHECK_EQUAL(__ut_cb_called, TT_TRUE, "");
    TT_TEST_CHECK_EQUAL(tt_string_cmp(&__ut_str_set, "-2147483649"), 0, "");

    tt_cfgnode_destroy(cnode, TT_TRUE);
    tt_buf_destroy(&out);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_cfgnode_grp)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_cfgnode_t *cgrp, *subg, *subg1, *subg2, *subg3, *subg_c, *c1, *c2, *c3,
        *c4, *tmp;
    tt_result_t ret;
    tt_u32_t u32_val = ~0;
    tt_s32_t s32_val = ~0, cmp_ret;
    tt_string_t str_val;
    tt_buf_t output;
    tt_blob_t name = {(tt_u8_t *)&ret, sizeof(ret)};
    tt_blob_t val = {(tt_u8_t *)&ret, sizeof(ret)};

    TT_TEST_CASE_ENTER()
    // test start

    tt_string_init(&str_val, NULL);
    tt_string_append(&str_val, "test string");

    tt_buf_init(&output, NULL);

    cgrp = tt_cfggrp_create("", NULL, NULL, NULL, NULL);
    TT_TEST_CHECK_NOT_EQUAL(cgrp, NULL, "");
    subg = tt_cfggrp_create("sub-group", NULL, NULL, NULL, NULL);
    TT_TEST_CHECK_NOT_EQUAL(subg, NULL, "");
    subg1 = tt_cfggrp_create("sub-group1", NULL, NULL, NULL, NULL);
    TT_TEST_CHECK_NOT_EQUAL(subg, NULL, "");
    subg2 = tt_cfggrp_create("sub-group22", NULL, NULL, NULL, NULL);
    TT_TEST_CHECK_NOT_EQUAL(subg, NULL, "");
    subg3 = tt_cfggrp_create("sub-group21", NULL, NULL, NULL, NULL);
    TT_TEST_CHECK_NOT_EQUAL(subg, NULL, "");

    subg_c = tt_cfgu32_create("subg-c", NULL, NULL, &u32_val, NULL, NULL);
    TT_TEST_CHECK_NOT_EQUAL(subg_c, NULL, "");
    c1 = tt_cfgu32_create("c1", NULL, NULL, &u32_val, NULL, NULL);
    TT_TEST_CHECK_NOT_EQUAL(subg_c, NULL, "");
    c2 = tt_cfgs32_create("c22", NULL, NULL, &s32_val, NULL, NULL);
    TT_TEST_CHECK_NOT_EQUAL(subg_c, NULL, "");
    c3 = tt_cfgstr_create("c333", NULL, NULL, &str_val, NULL, NULL);
    TT_TEST_CHECK_NOT_EQUAL(subg_c, NULL, "");
    c4 = tt_cfgstr_create("c322", NULL, NULL, &str_val, NULL, NULL);
    TT_TEST_CHECK_NOT_EQUAL(subg_c, NULL, "");

    // invalid name
    tmp = tt_cfgu32_create("1_2_3", NULL, NULL, &u32_val, NULL, NULL);
    TT_TEST_CHECK_EQUAL(tmp, NULL, "");

    tt_cfggrp_add(cgrp, subg);
    tt_cfggrp_add(cgrp, c1);
    tt_cfggrp_add(cgrp, subg1);
    tt_cfggrp_add(cgrp, c2);
    tt_cfggrp_add(cgrp, subg2);
    tt_cfggrp_add(cgrp, c3);
    tt_cfggrp_add(cgrp, subg3);
    tt_cfggrp_add(cgrp, c4);
    tt_cfggrp_add(subg, subg_c);

    //////////////////////////////////////////////////////

    ret = tt_cfgnode_add(cgrp, &name, &val);
    TT_TEST_CHECK_EQUAL(ret, TT_BAD_PARAM, "");

    ret = tt_cfgnode_rm(cgrp, &name);
    TT_TEST_CHECK_EQUAL(ret, TT_BAD_PARAM, "");

    {
        const tt_char_t *this_out = "----    grp     /    ";

        tt_buf_clear(&output);
        ret = tt_cfgnode_describe(cgrp, 0, &output);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&output, this_out);
        TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    }

    {
        const tt_char_t *this_out = "----    grp     /             ";

        tt_buf_clear(&output);
        ret = tt_cfgnode_describe(cgrp, 10, &output);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&output, this_out);
        TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    }

    {
        const tt_char_t *this_out =
            "PERM    TYPE    NAME            DESCRIPTION\n"
            "----    grp     sub-group/      \n"
            "----    grp     sub-group1/     \n"
            "----    grp     sub-group21/    \n"
            "----    grp     sub-group22/    \n"
            "--g-    u32     c1              \n"
            "--g-    s32     c22             \n"
            "--g-    str     c322            \n"
            "--g-    str     c333            ";

        tt_buf_clear(&output);
        ret = tt_cfgnode_ls(cgrp, NULL, &output);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&output, this_out);
        TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    }

    ret = tt_cfgnode_get(cgrp, &output);
    TT_TEST_CHECK_EQUAL(ret, TT_BAD_PARAM, "");

    ret = tt_cfgnode_set(cgrp, &val);
    TT_TEST_CHECK_EQUAL(ret, TT_BAD_PARAM, "");

    ret = tt_cfgnode_check(cgrp, &val);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    tt_cfgnode_destroy(cgrp, TT_TRUE);

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_u32_t __ut_u32;
static tt_s32_t __ut_s32;
static tt_string_t __ut_str;

static tt_cfgnode_t *__ut_create_child(IN struct tt_cfggrp_s *cgrp,
                                       IN tt_blob_t *name,
                                       IN tt_blob_t *val)
{
    tt_cfgnode_t *cnode = NULL;

    if (tt_strncmp("n_u32", (tt_char_t *)name->addr, name->len) == 0) {
        tt_cfgu32_attr_t attr;
        tt_cfgu32_attr_default(&attr);
        attr.cnode_attr.brief = "testing u32 child node";
        attr.mode = TT_CFGVAL_MODE_GS;

        cnode = tt_cfgu32_create("n-u32-111223",
                                 NULL,
                                 NULL,
                                 &__ut_u32,
                                 NULL,
                                 &attr);
    } else if (tt_strncmp("n_s32", (tt_char_t *)name->addr, name->len) == 0) {
        tt_cfgs32_attr_t attr;
        tt_cfgs32_attr_default(&attr);
        attr.cnode_attr.brief = "testing s32 child";
        attr.mode = TT_CFGVAL_MODE_GS;

        cnode =
            tt_cfgs32_create("n-s32-xx", NULL, NULL, &__ut_s32, NULL, &attr);
    } else if (tt_strncmp("n_str", (tt_char_t *)name->addr, name->len) == 0) {
        tt_cfgstr_attr_t attr;
        tt_cfgstr_attr_default(&attr);
        attr.cnode_attr.brief = "s32 string";
        attr.mode = TT_CFGVAL_MODE_GS;

        cnode =
            tt_cfgstr_create("n-string", NULL, NULL, &__ut_str, NULL, &attr);
    }

    tt_cfgnode_set(cnode, val);
    return cnode;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_cfgnode_grp_ar)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_cfgnode_t *cgrp;
    tt_result_t ret;
    tt_buf_t output;
    tt_blob_t name = {(tt_u8_t *)&ret, sizeof(ret)};
    tt_blob_t val = {(tt_u8_t *)&ret, sizeof(ret)};
    tt_s32_t cmp_ret;
    tt_cfggrp_attr_t attr;
    tt_cfggrp_cb_t cb = {__ut_create_child};

    const tt_char_t *ls_out_empty = "";

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&output, NULL);
    tt_string_init(&__ut_str, NULL);

    tt_cfggrp_attr_default(&attr);
    attr.mode = TT_CFGGRP_MODE_ARL;
    attr.cnode_attr.brief = "an addable and removable group";

    cgrp = tt_cfggrp_create("", NULL, NULL, &cb, &attr);
    TT_TEST_CHECK_NOT_EQUAL(cgrp, NULL, "");

    //////////////////////////////////////////////////////

    ret = tt_cfgnode_get(cgrp, &output);
    TT_TEST_CHECK_EQUAL(ret, TT_BAD_PARAM, "");

    ret = tt_cfgnode_set(cgrp, &val);
    TT_TEST_CHECK_EQUAL(ret, TT_BAD_PARAM, "");

    ret = tt_cfgnode_check(cgrp, &val);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    {
        const tt_char_t *outstr = "PERM    TYPE    NAME    DESCRIPTION\n";
        tt_buf_clear(&output);
        ret = tt_cfgnode_ls(cgrp, NULL, &output);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&output, outstr);
        TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    }

    // add 1 node
    name.addr = (tt_u8_t *)"n_u32";
    name.len = sizeof("n_u32") - 1;
    val.addr = (tt_u8_t *)"123";
    val.len = sizeof("123") - 1;
    ret = tt_cfgnode_add(cgrp, &name, &val);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    {
        const tt_char_t *outstr = "PERM    TYPE    NAME    DESCRIPTION\n";
        tt_buf_clear(&output);
        ret = tt_cfgnode_ls(cgrp, NULL, &output);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&output, outstr);
        TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    }

    ret = tt_cfgnode_commit(cgrp);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    {
        const tt_char_t *outstr =
            "PERM    TYPE    NAME            DESCRIPTION\n"
            "--gs    u32     n-u32-111223    testing u32 child node";

        tt_buf_clear(&output);
        ret = tt_cfgnode_ls(cgrp, NULL, &output);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&output, outstr);
        TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    }

    // add 2 node
    name.addr = (tt_u8_t *)"n_s32";
    name.len = sizeof("n_s32") - 1;
    val.addr = (tt_u8_t *)"-123";
    val.len = sizeof("-123") - 1;
    ret = tt_cfgnode_add(cgrp, &name, &val);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_cfgnode_commit(cgrp);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    {
        const tt_char_t outstr[] =
            "PERM    TYPE    NAME            DESCRIPTION\x01\x02"
            "--gs    s32     n-s32-xx        testing s32 child\x01\x02"
            "--gs    u32     n-u32-111223    testing u32 child node";

        tt_buf_clear(&output);
        ret = tt_cfgnode_ls(cgrp, "\001\002", &output);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&output, outstr);
        TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    }

    // rm 1 node
    name.addr = (tt_u8_t *)"n_u32";
    name.len = sizeof("n_u32") - 1;
    ret = tt_cfgnode_rm(cgrp, &name);
    TT_TEST_CHECK_EQUAL(ret, TT_BAD_PARAM, "");

    // can not add beofore commiting
    name.addr = (tt_u8_t *)"n_u32";
    name.len = sizeof("n_u32") - 1;
    val.addr = (tt_u8_t *)"123";
    val.len = sizeof("123") - 1;
    ret = tt_cfgnode_add(cgrp, &name, &val);
    TT_TEST_CHECK_EQUAL(ret, TT_BAD_PARAM, "");

    name.addr = (tt_u8_t *)"n-s32-xx";
    name.len = sizeof("n-s32-xx") - 1;
    ret = tt_cfgnode_rm(cgrp, &name);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    {
        const tt_char_t outstr[] =
            "PERM    TYPE    NAME            DESCRIPTION\x01\x02"
            "--gs    s32     n-s32-xx        testing s32 child\x01\x02"
            "--gs    u32     n-u32-111223    testing u32 child node";

        tt_buf_clear(&output);
        ret = tt_cfgnode_ls(cgrp, "\001\002", &output);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&output, outstr);
        TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    }
    ret = tt_cfgnode_commit(cgrp);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    {
        const tt_char_t outstr[] =
            "PERM    TYPE    NAME            DESCRIPTION\x01\x02"
            "--gs    u32     n-u32-111223    testing u32 child node";

        tt_buf_clear(&output);
        ret = tt_cfgnode_ls(cgrp, "\001\002", &output);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&output, outstr);
        TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    }

    // add 2 node
    name.addr = (tt_u8_t *)"n_s32";
    name.len = sizeof("n_s32") - 1;
    val.addr = (tt_u8_t *)"-999";
    val.len = sizeof("-999") - 1;
    ret = tt_cfgnode_add(cgrp, &name, &val);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    name.addr = (tt_u8_t *)"n_str";
    name.len = sizeof("n_str") - 1;
    val.addr = (tt_u8_t *)"this is a string";
    val.len = sizeof("this is a string") - 1;
    ret = tt_cfgnode_add(cgrp, &name, &val);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // rm uncommitted node
    name.addr = (tt_u8_t *)"n-s32-xx";
    name.len = sizeof("n-s32-xx") - 1;
    ret = tt_cfgnode_rm(cgrp, &name);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_cfgnode_commit(cgrp);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    {
        const tt_char_t outstr[] =
            "PERM    TYPE    NAME            DESCRIPTION\r\n"
            "--gs    str     n-string        s32 string\r\n"
            "--gs    u32     n-u32-111223    testing u32 child node";

        tt_buf_clear(&output);
        ret = tt_cfgnode_ls(cgrp, "\r\n", &output);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&output, outstr);
        TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    }

    tt_cfgnode_destroy(cgrp, TT_TRUE);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_cfgnode_bool)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_cfgnode_t *cnode;
    tt_bool_t val = TT_FALSE;
    tt_result_t ret;
    tt_u8_t c[] = "u32";
    tt_buf_t out;
    tt_s32_t cmp_ret;
    tt_blob_t n = {c, sizeof(c)}, v = {c, sizeof(c)};
    tt_cfgbool_attr_t attr;
    tt_cfgbool_cb_t cb = {__val_on_destroy, __bool_on_set};

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&out, NULL);

    cnode = tt_cfgbool_create("", NULL, NULL, &val, &cb, NULL);
    TT_TEST_CHECK_NOT_EQUAL(cnode, NULL, "");

    // add
    ret = tt_cfgnode_add(cnode, &n, &v);
    TT_TEST_CHECK_EQUAL(ret, TT_BAD_PARAM, "");

    // rm
    ret = tt_cfgnode_rm(cnode, &n);
    TT_TEST_CHECK_EQUAL(ret, TT_BAD_PARAM, "");

    // ls
    {
        const tt_char_t *this_out =
            "PERM    TYPE    NAME    DESCRIPTION\n"
            "--g-    bool            ";

        tt_buf_clear(&out);
        ret = tt_cfgnode_ls(cnode, NULL, &out);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&out, this_out);
        TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    }

    // get
    tt_buf_clear(&out);
    val = TT_TRUE;
    ret = tt_cfgnode_get(cnode, &out);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&out, "true");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    tt_buf_clear(&out);
    val = TT_FALSE;
    ret = tt_cfgnode_get(cnode, &out);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&out, "false");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    // set
    ret = tt_cfgnode_set(cnode, &v);
    TT_TEST_CHECK_EQUAL(ret, TT_BAD_PARAM, "");

    // check
    v.addr = (tt_u8_t *)"true";
    v.len = (tt_u32_t)sizeof("true") - 1;
    ret = tt_cfgnode_check(cnode, &v);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    v.addr = (tt_u8_t *)"false";
    v.len = (tt_u32_t)sizeof("false") - 1;
    ret = tt_cfgnode_check(cnode, &v);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    v.addr = (tt_u8_t *)"truee";
    v.len = (tt_u32_t)sizeof("truee") - 1;
    ret = tt_cfgnode_check(cnode, &v);
    TT_TEST_CHECK_EQUAL(ret, TT_BAD_PARAM, "");

    v.addr = (tt_u8_t *)"";
    v.len = (tt_u32_t)sizeof("");
    ret = tt_cfgnode_check(cnode, &v);
    TT_TEST_CHECK_EQUAL(ret, TT_BAD_PARAM, "");

    // commit
    ret = tt_cfgnode_commit(cnode);
    TT_TEST_CHECK_EQUAL(ret, TT_BAD_PARAM, "");

    __ut_err_line = 0;
    __ut_on_destroy_ok = TT_FALSE;
    tt_cfgnode_destroy(cnode, TT_FALSE);
    TT_TEST_CHECK_EQUAL(__ut_err_line, 0, "");
    TT_TEST_CHECK_EQUAL(__ut_on_destroy_ok, TT_FALSE, "");

    // node 2
    tt_cfgbool_attr_default(&attr);
    attr.cnode_attr.brief = "test bool node";
    attr.mode = TT_CFGVAL_MODE_GS;

    cnode = tt_cfgbool_create("node-bool", NULL, NULL, &val, &cb, &attr);
    TT_TEST_CHECK_NOT_EQUAL(cnode, NULL, "");

    // add
    ret = tt_cfgnode_add(cnode, &n, &v);
    TT_TEST_CHECK_EQUAL(ret, TT_BAD_PARAM, "");

    // rm
    ret = tt_cfgnode_rm(cnode, &n);
    TT_TEST_CHECK_EQUAL(ret, TT_BAD_PARAM, "");

    // ls
    {
        const tt_char_t *this_out =
            "PERM    TYPE    NAME         DESCRIPTION\n"
            "--gs    bool    node-bool    test bool node";

        tt_buf_clear(&out);
        ret = tt_cfgnode_ls(cnode, NULL, &out);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&out, this_out);
        TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");
    }

    tt_buf_clear(&out);
    ret = tt_cfgnode_describe(cnode, 0, &out);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret =
        tt_buf_cmp_cstr(&out, "--gs    bool    node-bool    test bool node");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    tt_buf_clear(&out);
    ret = tt_cfgnode_describe(cnode, 3, &out);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&out, "--gs    bool    ???    test bool node");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    tt_buf_clear(&out);
    ret = tt_cfgnode_describe(cnode, 10, &out);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret =
        tt_buf_cmp_cstr(&out, "--gs    bool    node-bool     test bool node");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    // get
    tt_buf_clear(&out);
    val = ~0;
    ret = tt_cfgnode_get(cnode, &out);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&out, "true");
    TT_TEST_CHECK_EQUAL(cmp_ret, 0, "");

    // set
    val = TT_FALSE;
    tt_buf_clear(&out);
    v.addr = (tt_u8_t *)"true";
    v.len = (tt_u32_t)sizeof("true") - 1;
    ret = tt_cfgnode_set(cnode, &v);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    tt_buf_clear(&out);
    v.addr = (tt_u8_t *)"false";
    v.len = (tt_u32_t)sizeof("false") - 1;
    ret = tt_cfgnode_set(cnode, &v);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // commit
    __ut_cb_called = TT_FALSE;
    ret = tt_cfgnode_commit(cnode);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(__ut_cb_called, TT_FALSE, "");
    TT_TEST_CHECK_EQUAL(val, TT_FALSE, "");

    val = TT_FALSE;
    tt_buf_clear(&out);
    v.addr = (tt_u8_t *)"true";
    v.len = (tt_u32_t)sizeof("true") - 1;
    ret = tt_cfgnode_set(cnode, &v);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_cfgnode_commit(cnode);
    TT_TEST_CHECK_EQUAL(ret, TT_END, "");
    TT_TEST_CHECK_EQUAL(__ut_cb_called, TT_TRUE, "");
    TT_TEST_CHECK_EQUAL(__ut_bool_set, TT_TRUE, "");

    __ut_err_line = 0;
    __ut_on_destroy_ok = TT_FALSE;
    tt_cfgnode_destroy(cnode, TT_TRUE);
    TT_TEST_CHECK_EQUAL(__ut_err_line, 0, "");
    TT_TEST_CHECK_EQUAL(__ut_on_destroy_ok, TT_TRUE, "");

    tt_buf_destroy(&out);

    // test end
    TT_TEST_CASE_LEAVE()
}
