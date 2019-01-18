/* Copyright (C) 2017 haniu (niuhao.cn@gmail.com)
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
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

tt_export tt_result_t __param_cli_line(IN tt_param_t *p,
                                       IN OPT const tt_char_t *col_sep,
                                       IN OPT tt_u32_t max_name_len,
                                       OUT tt_buf_t *output);

tt_export tt_result_t __param_cli_dir_ls(IN tt_param_dir_t *pd,
                                         IN const tt_char_t *col_sep,
                                         IN const tt_char_t *line_sep,
                                         OUT tt_buf_t *output);

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

// === routine declarations ================
TT_TEST_ROUTINE_DECLARE(case_cfgnode)
TT_TEST_ROUTINE_DECLARE(case_cfgnode_u32)
TT_TEST_ROUTINE_DECLARE(case_cfgnode_s32)
TT_TEST_ROUTINE_DECLARE(case_cfgnode_str)
TT_TEST_ROUTINE_DECLARE(case_cfgnode_dir)
TT_TEST_ROUTINE_DECLARE(case_cfgnode_grp_ar)
TT_TEST_ROUTINE_DECLARE(case_cfgnode_bool)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(cli_node_case)

TT_TEST_CASE(
    "case_clinode", "config node", case_cfgnode, NULL, NULL, NULL, NULL, NULL)
,

    TT_TEST_CASE("case_cfgnode_u32",
                 "config node, u32",
                 case_cfgnode_u32,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_cfgnode_s32",
                 "config node, s32",
                 case_cfgnode_s32,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_cfgnode_str",
                 "config node, string",
                 case_cfgnode_str,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_cfgnode_dir",
                 "config node, group",
                 case_cfgnode_dir,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_cfgnode_grp_ar",
                 "config node, group add/rm",
                 case_cfgnode_grp_ar,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_cfgnode_bool",
                 "config node, bool",
                 case_cfgnode_bool,
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
    TT_TEST_ROUTINE_DEFINE(case_cfgpath_basic)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(case_cfgnode)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_param_t *cnode;
    tt_param_attr_t attr;
    tt_blob_t name = {(tt_u8_t *)&attr, 2};
    tt_blob_t val = {(tt_u8_t *)&attr, 3};
    tt_buf_t out;
    tt_param_itf_t itf = {0};

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&out, NULL);

    tt_param_attr_default(&attr);
    attr.display = "display";
    attr.brief = "info";
    attr.detail = "usage";
    attr.need_reboot = TT_FALSE;
    attr.can_read = TT_TRUE;
    attr.can_write = TT_TRUE;

    cnode = tt_param_create(1, TT_PARAM_STRING, "name", &itf, (void *)1, &attr);
    TT_UT_NOT_EQUAL(cnode, NULL, "");

    TT_UT_EQUAL(cnode->type, TT_PARAM_STRING, "");
    TT_UT_EQUAL(cnode->opaque, (void *)1, "");
    TT_UT_EQUAL(cnode->itf, &itf, "");
    TT_UT_EQUAL(tt_strcmp(cnode->name, "name"), 0, "");
    TT_UT_EQUAL(tt_strcmp(cnode->display, "display"), 0, "");
    TT_UT_EQUAL(tt_strcmp(cnode->brief, "info"), 0, "");
    TT_UT_EQUAL(tt_strcmp(cnode->detail, "usage"), 0, "");
    TT_UT_EQUAL(cnode->need_reboot, TT_FALSE, "");
    TT_UT_EQUAL(cnode->can_read, TT_TRUE, "");
    TT_UT_EQUAL(cnode->can_write, TT_TRUE, "");

    // may fail, but should not crash
    tt_param_read(cnode, &out);
    tt_param_write(cnode, val.addr, val.len);

    tt_buf_clear(&out);
    TT_UT_SUCCESS(__param_cli_line(cnode, " ", 5, &out), "");
    TT_UT_EQUAL(tt_buf_cmp_cstr(&out, "-- string name  info"), 0, "");

    tt_buf_clear(&out);
    TT_UT_SUCCESS(__param_cli_line(cnode, NULL, 0, &out), "");
    TT_UT_EQUAL(tt_buf_cmp_cstr(&out, "--    string    name    info"), 0, "");

    tt_buf_clear(&out);
    TT_UT_SUCCESS(__param_cli_line(cnode, NULL, 2, &out), "");
    TT_UT_EQUAL(tt_buf_cmp_cstr(&out, "--    string    ??    info"), 0, "");

    tt_param_destroy(cnode);

    // node name
    attr.need_reboot = TT_TRUE;
    attr.can_read = TT_FALSE;
    attr.can_write = TT_FALSE;
    cnode = tt_param_create(1, TT_PARAM_DIR, "-name-", &itf, (void *)1, &attr);
    TT_UT_NOT_NULL(cnode, "");
    TT_UT_EQUAL(cnode->need_reboot, TT_TRUE, "");
    TT_UT_EQUAL(cnode->can_read, TT_FALSE, "");
    TT_UT_EQUAL(cnode->can_write, TT_FALSE, "");
    TT_UT_EQUAL(tt_param_read(cnode, &out), TT_E_UNSUPPORT, "");
    TT_UT_EQUAL(tt_param_read(cnode, &out), TT_E_UNSUPPORT, "");
    TT_UT_EQUAL(tt_param_read(cnode, &out), TT_E_UNSUPPORT, "");

    tt_buf_clear(&out);
    TT_UT_SUCCESS(__param_cli_line(cnode, " ", 8, &out), "");
    TT_UT_EQUAL(tt_buf_cmp_cstr(&out, "-- dir    -name-/  info"), 0, "");

    tt_buf_clear(&out);
    TT_UT_SUCCESS(__param_cli_line(cnode, NULL, 0, &out), "");
    TT_UT_EQUAL(tt_buf_cmp_cstr(&out, "--    dir       -name-/    info"),
                0,
                "");

    tt_buf_clear(&out);
    TT_UT_SUCCESS(__param_cli_line(cnode, NULL, 2, &out), "");
    TT_UT_EQUAL(tt_buf_cmp_cstr(&out, "--    dir       ??    info"), 0, "");

    tt_param_destroy(cnode);

    cnode = tt_param_create(1,
                            TT_PARAM_STRING,
                            "name?name",
                            &itf,
                            (void *)1,
                            &attr);
    TT_UT_NULL(cnode, "");

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

static void __val_on_destroy(IN struct tt_param_s *cnode,
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

static tt_result_t __u32_on_set(IN struct tt_param_s *cnode,
                                IN tt_u32_t new_val)
{
    *(tt_u32_t *)cnode->opaque = new_val;
    __ut_cb_called = TT_TRUE;
    __ut_u32_set = new_val;
    return TT_SUCCESS;
}

static tt_result_t __s32_on_set(IN struct tt_param_s *cnode,
                                IN tt_s32_t new_val)
{
    *(tt_s32_t *)cnode->opaque = new_val;
    __ut_cb_called = TT_TRUE;
    __ut_s32_set = new_val;
    return TT_SUCCESS;
}

static tt_result_t __str_on_set(IN struct tt_param_s *cnode,
                                IN tt_string_t *new_val)
{
    tt_string_t *s = (tt_string_t *)cnode->opaque;

    tt_string_clear(s);
    tt_string_copy(s, new_val);

    __ut_cb_called = TT_TRUE;
    tt_string_copy(&__ut_str_set, new_val);
    return TT_SUCCESS;
}

static tt_result_t __bool_on_set(IN struct tt_param_s *cnode,
                                 IN tt_bool_t new_val)
{
    *(tt_bool_t *)cnode->opaque = new_val;
    __ut_cb_called = TT_TRUE;
    __ut_bool_set = new_val;
    return TT_TRUE;
}

TT_TEST_ROUTINE_DEFINE(case_cfgnode_u32)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_param_t *cnode;
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
    tt_param_attr_t attr;
    tt_param_u32_cb_t cb = {__u32_on_set};

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&out, NULL);

    tt_param_attr_default(&attr);
    attr.can_read = TT_TRUE;
    attr.can_write = TT_FALSE;

    cnode = tt_param_u32_create("", &val, &attr, &cb);
    TT_UT_NOT_EQUAL(cnode, NULL, "");

    // ls
    {
        tt_buf_clear(&out);
        ret = tt_param_read(cnode, &out);
        TT_UT_SUCCESS(ret, "");
        cmp_ret = tt_buf_cmp_cstr(&out, "0");
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // get
    tt_buf_clear(&out);
    val = 0;
    ret = tt_param_read(cnode, &out);
    TT_UT_SUCCESS(ret, "");
    cmp_ret = tt_buf_cmp_cstr(&out, "0");
    TT_UT_EQUAL(cmp_ret, 0, "");

    // set, can not set
    ret = tt_param_write(cnode, v.addr, v.len);
    TT_UT_EQUAL(ret, TT_E_UNSUPPORT, "");

    tt_param_destroy(cnode);

    // node 2
    tt_param_attr_default(&attr);
    attr.brief = "test node 2";
    attr.can_read = TT_TRUE;
    attr.can_write = TT_TRUE;

    cnode = tt_param_u32_create("node2", &val, &attr, &cb);
    TT_UT_NOT_EQUAL(cnode, NULL, "");

    // ls
    {
        tt_buf_clear(&out);
        ret = tt_param_read(cnode, &out);
        TT_UT_SUCCESS(ret, "");
        cmp_ret = tt_buf_cmp_cstr(&out, "0");
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    tt_buf_clear(&out);
    ret = __param_cli_line(cnode, " ", 0, &out);
    TT_UT_SUCCESS(ret, "");
    cmp_ret = tt_buf_cmp_cstr(&out, "rw u32    node2 test node 2");
    TT_UT_EQUAL(cmp_ret, 0, "");

    tt_buf_clear(&out);
    ret = __param_cli_line(cnode, " ", 3, &out);
    TT_UT_SUCCESS(ret, "");
    cmp_ret = tt_buf_cmp_cstr(&out, "rw u32    ??? test node 2");
    TT_UT_EQUAL(cmp_ret, 0, "");

    tt_buf_clear(&out);
    ret = __param_cli_line(cnode, " ", 10, &out);
    TT_UT_SUCCESS(ret, "");
    cmp_ret = tt_buf_cmp_cstr(&out, "rw u32    node2      test node 2");
    TT_UT_EQUAL(cmp_ret, 0, "");

    // get
    tt_buf_clear(&out);
    val = ~0;
    ret = tt_param_read(cnode, &out);
    TT_UT_SUCCESS(ret, "");
    cmp_ret = tt_buf_cmp_cstr(&out, "4294967295");
    TT_UT_EQUAL(cmp_ret, 0, "");

    // set
    tt_buf_clear(&out);
    v.addr = (tt_u8_t *)invalid_u32;
    v.len = (tt_u32_t)tt_strlen(invalid_u32);
    ret = tt_param_write(cnode, v.addr, v.len);
    TT_UT_EQUAL(ret, TT_E_BADARG, "");

    v.addr = (tt_u8_t *)invalid_u32_2;
    v.len = (tt_u32_t)tt_strlen(invalid_u32_2);
    ret = tt_param_write(cnode, v.addr, v.len);
    TT_UT_EQUAL(ret, TT_E_BADARG, "");

    v.addr = (tt_u8_t *)u32_0;
    v.len = (tt_u32_t)tt_strlen(u32_0);
    ret = tt_param_write(cnode, v.addr, v.len);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(val, 0, "");
    tt_buf_clear(&out);
    ret = tt_param_read(cnode, &out);
    TT_UT_SUCCESS(ret, "");
    cmp_ret = tt_buf_cmp_cstr(&out, "0");
    TT_UT_EQUAL(cmp_ret, 0, "");

    v.addr = (tt_u8_t *)max_u32;
    v.len = (tt_u32_t)tt_strlen(max_u32);
    ret = tt_param_write(cnode, v.addr, v.len);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(val, 0xFFFFFFFF, "");
    tt_buf_clear(&out);
    ret = tt_param_read(cnode, &out);
    TT_UT_SUCCESS(ret, "");
    cmp_ret = tt_buf_cmp_cstr(&out, "4294967295");
    TT_UT_EQUAL(cmp_ret, 0, "");

    v.addr = (tt_u8_t *)u32_0;
    v.len = (tt_u32_t)tt_strlen(u32_0);
    ret = tt_param_write(cnode, v.addr, v.len);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(val, 0, "");

    tt_param_destroy(cnode);

    tt_buf_destroy(&out);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_cfgnode_s32)
{
    // tt_s32_t param = TT_TEST_ROUTINE_PARAM(tt_s32_t);
    tt_param_t *cnode;
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
    tt_param_attr_t attr;
    tt_param_s32_cb_t cb = {__s32_on_set};

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&out, NULL);

    tt_param_attr_default(&attr);
    attr.can_read = TT_TRUE;
    attr.can_write = TT_FALSE;

    cnode = tt_param_s32_create("", &val, &attr, &cb);
    TT_UT_NOT_EQUAL(cnode, NULL, "");

    // ls
    {
        tt_buf_clear(&out);
        ret = tt_param_read(cnode, &out);
        TT_UT_SUCCESS(ret, "");
        cmp_ret = tt_buf_cmp_cstr(&out, "0");
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // get
    tt_buf_clear(&out);
    ret = tt_param_read(cnode, &out);
    TT_UT_SUCCESS(ret, "");
    cmp_ret = tt_buf_cmp_cstr(&out, "0");
    TT_UT_EQUAL(cmp_ret, 0, "");

    // set
    ret = tt_param_write(cnode, v.addr, v.len);
    TT_UT_EQUAL(ret, TT_E_UNSUPPORT, "");

    tt_param_destroy(cnode);

    // node 2
    tt_param_attr_default(&attr);
    attr.brief = "test node 2";
    attr.can_read = TT_TRUE;
    attr.can_write = TT_TRUE;

    cnode = tt_param_s32_create("node1234567890", &val, &attr, &cb);
    TT_UT_NOT_EQUAL(cnode, NULL, "");

    // ls
    {
        tt_buf_clear(&out);
        ret = tt_param_read(cnode, &out);
        TT_UT_SUCCESS(ret, "");
        cmp_ret = tt_buf_cmp_cstr(&out, "0");
        TT_UT_EQUAL(cmp_ret, 0, "");
    }


    {
        tt_buf_clear(&out);
        ret = __param_cli_line(cnode, " ", 0, &out);
        TT_UT_SUCCESS(ret, "");
        cmp_ret = tt_buf_cmp_cstr(&out, "rw s32    node1234567890 test node 2");
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    {
        tt_buf_clear(&out);
        ret = __param_cli_line(cnode, " ", 7, &out);
        TT_UT_SUCCESS(ret, "");
        cmp_ret = tt_buf_cmp_cstr(&out, "rw s32    ??????? test node 2");
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    {
        tt_buf_clear(&out);
        ret = __param_cli_line(cnode, " ", 8, &out);
        TT_UT_SUCCESS(ret, "");
        cmp_ret = tt_buf_cmp_cstr(&out, "rw s32    ???????? test node 2");
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    {
        tt_buf_clear(&out);
        ret = __param_cli_line(cnode, " ", 16, &out);
        TT_UT_SUCCESS(ret, "");
        cmp_ret =
            tt_buf_cmp_cstr(&out, "rw s32    node1234567890   test node 2");
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // get
    val = ~0;
    tt_buf_clear(&out);
    ret = tt_param_read(cnode, &out);
    TT_UT_SUCCESS(ret, "");
    cmp_ret = tt_buf_cmp_cstr(&out, "-1");
    TT_UT_EQUAL(cmp_ret, 0, "");

    // set
    v.addr = (tt_u8_t *)max_s32;
    v.len = (tt_u32_t)tt_strlen(max_s32);
    ret = tt_param_write(cnode, v.addr, v.len);
    TT_UT_SUCCESS(ret, "");
    tt_buf_clear(&out);
    ret = tt_param_read(cnode, &out);
    TT_UT_SUCCESS(ret, "");
    cmp_ret = tt_buf_cmp_cstr(&out, "2147483647");
    TT_UT_EQUAL(cmp_ret, 0, "");
    TT_UT_EQUAL(val, 2147483647, "");

    tt_buf_clear(&out);
    v.addr = (tt_u8_t *)invalid_s32;
    v.len = (tt_u32_t)tt_strlen(invalid_s32);
    ret = tt_param_write(cnode, v.addr, v.len);
    TT_UT_EQUAL(ret, TT_E_BADARG, "");

    v.addr = (tt_u8_t *)invalid_s32_2;
    v.len = (tt_u32_t)tt_strlen(invalid_s32_2);
    ret = tt_param_write(cnode, v.addr, v.len);
    TT_UT_EQUAL(ret, TT_E_BADARG, "");

    // to -2147483648
    v.addr = (tt_u8_t *)min_s32;
    v.len = (tt_u32_t)tt_strlen(min_s32);
    ret = tt_param_write(cnode, v.addr, v.len);
    TT_UT_SUCCESS(ret, "");
    tt_buf_clear(&out);
    ret = tt_param_read(cnode, &out);
    TT_UT_SUCCESS(ret, "");
    cmp_ret = tt_buf_cmp_cstr(&out, "-2147483648");
    TT_UT_EQUAL(cmp_ret, 0, "");
    TT_UT_EQUAL(val, 0x80000000, "");

    // to -1
    v.addr = (tt_u8_t *)s32_n1;
    v.len = (tt_u32_t)tt_strlen(s32_n1);
    ret = tt_param_write(cnode, v.addr, v.len);
    TT_UT_SUCCESS(ret, "");
    tt_buf_clear(&out);
    ret = tt_param_read(cnode, &out);
    TT_UT_SUCCESS(ret, "");
    cmp_ret = tt_buf_cmp_cstr(&out, "-1");
    TT_UT_EQUAL(cmp_ret, 0, "");
    TT_UT_EQUAL(val, -1, "");

    v.addr = (tt_u8_t *)min_s32;
    v.len = (tt_u32_t)tt_strlen(min_s32);
    ret = tt_param_write(cnode, v.addr, v.len);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(val, 0x80000000, "");

    tt_param_destroy(cnode);
    tt_buf_destroy(&out);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_cfgnode_str)
{
    // tt_s32_t param = TT_TEST_ROUTINE_PARAM(tt_s32_t);
    tt_param_t *cnode;
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
    tt_param_attr_t attr;
    tt_param_str_cb_t cb = {__str_on_set};

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&out, NULL);
    tt_string_init(&val, NULL);
    tt_string_init(&__ut_str_set, NULL);

    tt_param_attr_default(&attr);
    attr.can_read = TT_TRUE;
    attr.can_write = TT_FALSE;

    cnode = tt_param_str_create("", &val, &attr, &cb);
    TT_UT_NOT_EQUAL(cnode, NULL, "");

    // ls
    {
        tt_buf_clear(&out);
        ret = __param_cli_line(cnode, " ", 0, &out);
        TT_UT_SUCCESS(ret, "");
        cmp_ret = tt_buf_cmp_cstr(&out, "r- string  ");
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // get
    tt_buf_clear(&out);
    ret = tt_param_read(cnode, &out);
    TT_UT_SUCCESS(ret, "");
    // cmp_ret = tt_buf_cmp_cstr(&out, "\"\"");
    cmp_ret = tt_buf_cmp_cstr(&out, "");
    TT_UT_EQUAL(cmp_ret, 0, "");

    // set
    ret = tt_param_write(cnode, v.addr, v.len);
    TT_UT_EQUAL(ret, TT_E_UNSUPPORT, "");

    tt_param_destroy(cnode);

    // node 2
    tt_param_attr_default(&attr);
    attr.brief = "test node 2";
    attr.can_read = TT_TRUE;
    attr.can_write = TT_TRUE;

    cnode = tt_param_str_create("node2", &val, &attr, &cb);
    TT_UT_NOT_EQUAL(cnode, NULL, "");

    // ls
    {
        tt_buf_clear(&out);
        ret = __param_cli_line(cnode, " ", 0, &out);
        TT_UT_SUCCESS(ret, "");
        cmp_ret = tt_buf_cmp_cstr(&out, "rw string node2 test node 2");
        TT_UT_EQUAL(cmp_ret, 0, "");
    }
    {
        tt_buf_clear(&out);
        ret = tt_param_cli_ls(cnode, "|||", " ", &out);
        TT_UT_SUCCESS(ret, "");
        cmp_ret = tt_buf_cmp_cstr(&out, "rw|||string|||node2|||test node 2");
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // get
    tt_string_clear(&val);
    tt_string_append(&val, test_str);
    tt_buf_clear(&out);
    ret = tt_param_read(cnode, &out);
    TT_UT_SUCCESS(ret, "");
    // cmp_ret = tt_buf_cmp_cstr(&out, "\"test string\"");
    cmp_ret = tt_buf_cmp_cstr(&out, "test string");
    TT_UT_EQUAL(cmp_ret, 0, "");

    // set
    v.addr = (tt_u8_t *)max_s32;
    v.len = (tt_u32_t)tt_strlen(max_s32);
    ret = tt_param_write(cnode, v.addr, v.len);
    TT_UT_SUCCESS(ret, "");
    tt_buf_clear(&out);
    ret = tt_param_read(cnode, &out);
    TT_UT_SUCCESS(ret, "");
    // cmp_ret = tt_buf_cmp_cstr(&out, "\"2147483647\"");
    cmp_ret = tt_buf_cmp_cstr(&out, "2147483647");
    TT_UT_EQUAL(cmp_ret, 0, "");
    TT_UT_EQUAL(tt_string_cmp(&val, "2147483647"), 0, "");

    v.addr = (tt_u8_t *)invalid_s32_2;
    v.len = (tt_u32_t)tt_strlen(invalid_s32_2);
    ret = tt_param_write(cnode, v.addr, v.len);
    TT_UT_SUCCESS(ret, "");
    tt_buf_clear(&out);
    ret = tt_param_read(cnode, &out);
    TT_UT_SUCCESS(ret, "");
    // cmp_ret = tt_buf_cmp_cstr(&out, "\"-2147483649\"");
    cmp_ret = tt_buf_cmp_cstr(&out, "-2147483649");
    TT_UT_EQUAL(cmp_ret, 0, "");
    TT_UT_EQUAL(tt_string_cmp(&val, invalid_s32_2), 0, "");

    v.addr = (tt_u8_t *)test_str;
    v.len = (tt_u32_t)tt_strlen(test_str);
    ret = tt_param_write(cnode, v.addr, v.len);
    TT_UT_SUCCESS(ret, "");
    tt_buf_clear(&out);
    ret = tt_param_read(cnode, &out);
    TT_UT_SUCCESS(ret, "");
    // cmp_ret = tt_buf_cmp_cstr(&out, "\"test string\"");
    cmp_ret = tt_buf_cmp_cstr(&out, "test string");
    TT_UT_EQUAL(cmp_ret, 0, "");
    TT_UT_EQUAL(tt_string_cmp(&val, "test string"), 0, "");

    v.addr = (tt_u8_t *)invalid_s32_2;
    v.len = (tt_u32_t)tt_strlen(invalid_s32_2);
    ret = tt_param_write(cnode, v.addr, v.len);
    TT_UT_SUCCESS(ret, "");
    tt_buf_clear(&out);
    ret = tt_param_read(cnode, &out);
    TT_UT_SUCCESS(ret, "");
    // cmp_ret = tt_buf_cmp_cstr(&out, "\"-2147483649\"");
    cmp_ret = tt_buf_cmp_cstr(&out, "-2147483649");
    TT_UT_EQUAL(cmp_ret, 0, "");
    TT_UT_EQUAL(tt_string_cmp(&val, invalid_s32_2), 0, "");

    tt_param_destroy(cnode);
    tt_buf_destroy(&out);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_cfgnode_dir)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_param_t *co, *subg, *subg1, *subg2, *subg3, *subg_c, *c1, *c2, *c3, *c4,
        *tmp;
    tt_param_dir_t *cgrp;
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

    co = tt_param_dir_create("", NULL);
    TT_UT_NOT_EQUAL(co, NULL, "");
    subg = tt_param_dir_create("sub-group", NULL);
    TT_UT_NOT_EQUAL(subg, NULL, "");
    subg1 = tt_param_dir_create("sub-group1", NULL);
    TT_UT_NOT_EQUAL(subg1, NULL, "");
    subg2 = tt_param_dir_create("sub-group22", NULL);
    TT_UT_NOT_EQUAL(subg2, NULL, "");
    subg3 = tt_param_dir_create("sub-group21", NULL);
    TT_UT_NOT_EQUAL(subg3, NULL, "");

    subg_c = tt_param_u32_create("subg-c", &u32_val, NULL, NULL);
    TT_UT_NOT_EQUAL(subg_c, NULL, "");
    c1 = tt_param_u32_create("c1", &u32_val, NULL, NULL);
    TT_UT_NOT_EQUAL(c1, NULL, "");
    c2 = tt_param_s32_create("c22", &s32_val, NULL, NULL);
    TT_UT_NOT_EQUAL(c2, NULL, "");
    c3 = tt_param_str_create("c333", &str_val, NULL, NULL);
    TT_UT_NOT_EQUAL(c3, NULL, "");
    c4 = tt_param_str_create("c322", &str_val, NULL, NULL);
    TT_UT_NOT_EQUAL(c4, NULL, "");

    // invalid name
    tmp = tt_param_u32_create("1_2_3", &u32_val, NULL, NULL);
    TT_UT_EQUAL(tmp, NULL, "");

    cgrp = TT_PARAM_CAST(co, tt_param_dir_t);
    tt_param_dir_add(cgrp, subg);
    tt_param_dir_add(cgrp, c1);
    tt_param_dir_add(cgrp, subg1);
    tt_param_dir_add(cgrp, c2);
    tt_param_dir_add(cgrp, subg2);
    tt_param_dir_add(cgrp, c3);
    tt_param_dir_add(cgrp, subg3);
    tt_param_dir_add(cgrp, c4);
    tt_param_dir_add(TT_PARAM_CAST(subg, tt_param_dir_t), subg_c);

    //////////////////////////////////////////////////////

    {
        tt_buf_clear(&output);
        ret = __param_cli_line(co, " ", 0, &output);
        TT_UT_SUCCESS(ret, "");
        cmp_ret = tt_buf_cmp_cstr(&output, "-- dir    / ");
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    {
        tt_buf_clear(&output);
        ret = __param_cli_line(co, NULL, 10, &output);
        TT_UT_SUCCESS(ret, "");
        cmp_ret = tt_buf_cmp_cstr(&output, "--    dir       /             ");
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    {
        const tt_char_t *this_out =
            "--    dir       sub-group/      \r\n"
            "rw    u32       c1              \r\n"
            "--    dir       sub-group1/     \r\n"
            "rw    s32       c22             \r\n"
            "--    dir       sub-group22/    \r\n"
            "rw    string    c333            \r\n"
            "--    dir       sub-group21/    \r\n"
            "rw    string    c322            ";

        tt_buf_clear(&output);
        ret = __param_cli_dir_ls(cgrp, NULL, "\r\n", &output);
        TT_UT_SUCCESS(ret, "");
        cmp_ret = tt_buf_cmp_cstr(&output, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    ret = tt_param_read(co, &output);
    TT_UT_EQUAL(ret, TT_E_UNSUPPORT, "");

    ret = tt_param_write(co, val.addr, val.len);
    TT_UT_EQUAL(ret, TT_E_UNSUPPORT, "");

    tt_param_destroy(co);

    tt_string_destroy(&str_val);
    tt_buf_destroy(&output);

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_u32_t __ut_u32;
static tt_s32_t __ut_s32;
static tt_string_t __ut_str;

static tt_param_t *__ut_create_child(IN struct tt_param_dir_s *cgrp,
                                     IN tt_blob_t *name,
                                     IN tt_blob_t *val)
{
    tt_param_t *cnode = NULL;

    if (tt_strncmp("n_u32", (tt_char_t *)name->addr, name->len) == 0) {
        tt_param_attr_t attr;
        tt_param_attr_default(&attr);
        attr.brief = "testing u32 child node";

        cnode = tt_param_u32_create("n-u32-111223", &__ut_u32, &attr, NULL);
    } else if (tt_strncmp("n_s32", (tt_char_t *)name->addr, name->len) == 0) {
        tt_param_attr_t attr;
        tt_param_attr_default(&attr);
        attr.brief = "testing s32 child";

        cnode = tt_param_s32_create("n-s32-xx", &__ut_s32, &attr, NULL);
    } else if (tt_strncmp("n_str", (tt_char_t *)name->addr, name->len) == 0) {
        tt_param_attr_t attr;
        tt_param_attr_default(&attr);
        attr.brief = "s32 string";

        cnode = tt_param_str_create("n-string", &__ut_str, &attr, NULL);
    }

    // tt_param_write(cnode, val);
    return cnode;
}

TT_TEST_ROUTINE_DEFINE(case_cfgnode_grp_ar)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_param_t *cgrp, *co;
    tt_result_t ret;
    tt_buf_t output;
    tt_blob_t name = {(tt_u8_t *)&ret, sizeof(ret)};
    tt_blob_t val = {(tt_u8_t *)&ret, sizeof(ret)};
    tt_s32_t cmp_ret;
    tt_param_attr_t attr;
    tt_u32_t u32_val = 0;
    tt_s32_t s32_val = 0;

    const tt_char_t *ls_out_empty = "";

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&output, NULL);
    tt_string_init(&__ut_str, NULL);

    tt_param_attr_default(&attr);
    attr.brief = "an addable and removable group";

    cgrp = tt_param_dir_create("", &attr);
    TT_UT_NOT_EQUAL(cgrp, NULL, "");
    TT_UT_NULL(tt_param_dir_head(TT_PARAM_CAST(cgrp, tt_param_dir_t)), "");

    //////////////////////////////////////////////////////

    ret = tt_param_read(cgrp, &output);
    TT_UT_EQUAL(ret, TT_E_UNSUPPORT, "");

    ret = tt_param_write(cgrp, val.addr, val.len);
    TT_UT_EQUAL(ret, TT_E_UNSUPPORT, "");

    {
        tt_buf_clear(&output);
        ret = tt_param_cli_ls(cgrp, NULL, NULL, &output);
        TT_UT_SUCCESS(ret, "");
        cmp_ret = tt_buf_cmp_cstr(&output, "");
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // add 1 node
    co = tt_param_u32_create("n-u32", &u32_val, NULL, NULL);
    TT_UT_NOT_NULL(co, "");
    ret = tt_param_dir_add(TT_PARAM_CAST(cgrp, tt_param_dir_t), co);
    TT_UT_SUCCESS(ret, "");
    {
        tt_param_t *c = tt_param_dir_head(TT_PARAM_CAST(cgrp, tt_param_dir_t));
        TT_UT_EQUAL(c, co, "");
        c = tt_param_dir_next(c);
        TT_UT_NULL(c, "");
    }

    {
        tt_buf_clear(&output);
        ret = tt_param_cli_ls(cgrp, NULL, NULL, &output);
        TT_UT_SUCCESS(ret, "");
        cmp_ret = tt_buf_cmp_cstr(&output, "rw    u32       n-u32    ");
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    {
        tt_buf_clear(&output);
        ret = tt_param_read(cgrp, &output);
        TT_UT_EQUAL(ret, TT_E_UNSUPPORT, "");
    }

    // add 2 node
    co = tt_param_s32_create("n-s32", &s32_val, NULL, NULL);
    TT_UT_NOT_NULL(co, "");
    ret = tt_param_dir_add(TT_PARAM_CAST(cgrp, tt_param_dir_t), co);
    TT_UT_SUCCESS(ret, "");
    {
        tt_param_t *c = tt_param_dir_head(TT_PARAM_CAST(cgrp, tt_param_dir_t));
        TT_UT_STREQ(c->name, "n-u32", "");
        c = tt_param_dir_next(c);
        TT_UT_NOT_NULL(c, "");
        TT_UT_STREQ(c->name, "n-s32", "");
        c = tt_param_dir_next(c);
        TT_UT_NULL(c, "");
    }

    {
        const tt_char_t outstr[] =
            "rw>>>u32   >>>n-u32>>>\r\n"
            "rw>>>s32   >>>n-s32>>>";

        tt_buf_clear(&output);
        ret = tt_param_cli_ls(cgrp, ">>>", "\r\n", &output);
        TT_UT_SUCCESS(ret, "");
        cmp_ret = tt_buf_cmp_cstr(&output, outstr);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // add a directory
    co = tt_param_dir_create("dir1", NULL);
    TT_UT_NOT_NULL(co, "");
    ret = tt_param_dir_add(TT_PARAM_CAST(cgrp, tt_param_dir_t), co);
    TT_UT_SUCCESS(ret, "");

    {
        const tt_char_t outstr[] =
            "rw    u32       n-u32    \r\n"
            "rw    s32       n-s32    \r\n"
            "--    dir       dir1/    ";

        tt_buf_clear(&output);
        ret = tt_param_cli_ls(cgrp, NULL, "\r\n", &output);
        TT_UT_SUCCESS(ret, "");
        cmp_ret = tt_buf_cmp_cstr(&output, outstr);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // duplicated a directory name
    co = tt_param_dir_create("dir1", NULL);
    TT_UT_NOT_NULL(co, "");
    ret = tt_param_dir_add(TT_PARAM_CAST(cgrp, tt_param_dir_t), co);
    TT_UT_FAIL(ret, "");
    tt_param_destroy(co);

    // insert dir before
    co = tt_param_dir_create("dir0", NULL);
    TT_UT_NOT_NULL(co, "");
    ret = tt_param_dir_add(TT_PARAM_CAST(cgrp, tt_param_dir_t), co);
    TT_UT_SUCCESS(ret, "");

    // insert after
    co = tt_param_dir_create("dir222", NULL);
    TT_UT_NOT_NULL(co, "");
    ret = tt_param_dir_add(TT_PARAM_CAST(cgrp, tt_param_dir_t), co);
    TT_UT_SUCCESS(ret, "");

    {
        const tt_char_t outstr[] =
            "rw    u32       n-u32      \r\n"
            "rw    s32       n-s32      \r\n"
            "--    dir       dir1/      \r\n"
            "--    dir       dir0/      \r\n"
            "--    dir       dir222/    ";

        tt_buf_clear(&output);
        ret = tt_param_cli_ls(cgrp, NULL, "\r\n", &output);
        TT_UT_SUCCESS(ret, "");
        cmp_ret = tt_buf_cmp_cstr(&output, outstr);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // duplicated a node name
    co = tt_param_dir_create("n-u32", NULL);
    TT_UT_NOT_NULL(co, "");
    ret = tt_param_dir_add(TT_PARAM_CAST(cgrp, tt_param_dir_t), co);
    TT_UT_FAIL(ret, "");
    tt_param_destroy(co);

    co = tt_param_s32_create("dir0", &s32_val, NULL, NULL);
    TT_UT_NOT_NULL(co, "");
    ret = tt_param_dir_add(TT_PARAM_CAST(cgrp, tt_param_dir_t), co);
    TT_UT_FAIL(ret, "");
    tt_param_destroy(co);

    // insert dir before
    co = tt_param_s32_create("n-s0", &s32_val, NULL, NULL);
    TT_UT_NOT_NULL(co, "");
    ret = tt_param_dir_add(TT_PARAM_CAST(cgrp, tt_param_dir_t), co);
    TT_UT_SUCCESS(ret, "");

    // insert after
    co = tt_param_s32_create("n-z666666", &s32_val, NULL, NULL);
    TT_UT_NOT_NULL(co, "");
    ret = tt_param_dir_add(TT_PARAM_CAST(cgrp, tt_param_dir_t), co);
    TT_UT_SUCCESS(ret, "");

    {
        TT_UT_EQUAL(co,
                    tt_param_dir_find_tid(TT_PARAM_CAST(cgrp, tt_param_dir_t),
                                          co->tid),
                    "");
        TT_UT_EQUAL(NULL,
                    tt_param_dir_find_tid(TT_PARAM_CAST(cgrp, tt_param_dir_t),
                                          ~0),
                    "");
    }

    {
        const tt_char_t outstr[] =
            "rw    u32       n-u32        \r\n"
            "rw    s32       n-s32        \r\n"
            "--    dir       dir1/        \r\n"
            "--    dir       dir0/        \r\n"
            "--    dir       dir222/      \r\n"
            "rw    s32       n-s0         \r\n"
            "rw    s32       n-z666666    ";

        tt_buf_clear(&output);
        ret = tt_param_cli_ls(cgrp, NULL, "\r\n", &output);
        TT_UT_SUCCESS(ret, "");
        cmp_ret = tt_buf_cmp_cstr(&output, outstr);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // find unexist
    co = tt_param_dir_find(TT_PARAM_CAST(cgrp, tt_param_dir_t), "xxx", 3);
    TT_UT_NULL(co, "");

    // partial
    co = tt_param_dir_find(TT_PARAM_CAST(cgrp, tt_param_dir_t), "dir", 3);
    TT_UT_NULL(co, "");

    // find and remove head
    co = tt_param_dir_find(TT_PARAM_CAST(cgrp, tt_param_dir_t), "dir0", 4);
    TT_UT_NOT_NULL(co, "");
    tt_param_dir_remove(TT_PARAM_CAST(cgrp, tt_param_dir_t), co);
    tt_param_destroy(co);

    {
        const tt_char_t outstr[] =
            "rw    u32       n-u32        \r\n"
            "rw    s32       n-s32        \r\n"
            "--    dir       dir1/        \r\n"
            "--    dir       dir222/      \r\n"
            "rw    s32       n-s0         \r\n"
            "rw    s32       n-z666666    ";

        tt_buf_clear(&output);
        ret = tt_param_cli_ls(cgrp, NULL, "\r\n", &output);
        TT_UT_SUCCESS(ret, "");
        cmp_ret = tt_buf_cmp_cstr(&output, outstr);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // find and remove tail
    co = tt_param_dir_find(TT_PARAM_CAST(cgrp, tt_param_dir_t), "n-z666666", 9);
    TT_UT_NOT_NULL(co, "");
    tt_param_dir_remove(TT_PARAM_CAST(cgrp, tt_param_dir_t), co);
    tt_param_destroy(co);

    {
        const tt_char_t outstr[] =
            "rw    u32       n-u32        \r\n"
            "rw    s32       n-s32        \r\n"
            "--    dir       dir1/        \r\n"
            "--    dir       dir222/      \r\n"
            "rw    s32       n-s0         ";

        tt_buf_clear(&output);
        ret = tt_param_cli_ls(cgrp, NULL, "\r\n", &output);
        TT_UT_SUCCESS(ret, "");
        cmp_ret = tt_buf_cmp_cstr(&output, outstr);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // find and remove middle
    co = tt_param_dir_find(TT_PARAM_CAST(cgrp, tt_param_dir_t), "n-s0", 4);
    TT_UT_NOT_NULL(co, "");
    tt_param_dir_remove(TT_PARAM_CAST(cgrp, tt_param_dir_t), co);
    tt_param_destroy(co);

    {
        const tt_char_t outstr[] =
            "rw    u32       n-u32        \r\n"
            "rw    s32       n-s32        \r\n"
            "--    dir       dir1/        \r\n"
            "--    dir       dir222/      ";

        tt_buf_clear(&output);
        ret = tt_param_cli_ls(cgrp, NULL, "\r\n", &output);
        TT_UT_SUCCESS(ret, "");
        cmp_ret = tt_buf_cmp_cstr(&output, outstr);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    tt_param_destroy(cgrp);

    tt_buf_destroy(&output);
    tt_string_destroy(&__ut_str);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_cfgnode_bool)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_param_t *cnode;
    tt_bool_t val = TT_FALSE;
    tt_result_t ret;
    tt_u8_t c[] = "u32";
    tt_buf_t out;
    tt_s32_t cmp_ret;
    tt_blob_t n = {c, sizeof(c)}, v = {c, sizeof(c)};
    tt_param_attr_t attr;
    tt_param_bool_cb_t cb = {__bool_on_set};

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&out, NULL);

    tt_param_attr_default(&attr);
    attr.can_read = TT_TRUE;
    attr.can_write = TT_FALSE;

    cnode = tt_param_bool_create("", &val, &attr, &cb);
    TT_UT_NOT_EQUAL(cnode, NULL, "");
    val = TT_TRUE;
    TT_UT_TRUE(tt_param_get_bool(cnode), "");
    val = TT_FALSE;
    TT_UT_FALSE(tt_param_get_bool(cnode), "");

    // ls
    {
        tt_buf_clear(&out);
        ret = tt_param_read(cnode, &out);
        TT_UT_SUCCESS(ret, "");
        cmp_ret = tt_buf_cmp_cstr(&out, "false");
        TT_UT_EQUAL(cmp_ret, 0, "");

        tt_buf_clear(&out);
        ret = tt_param_cli_ls(cnode, NULL, NULL, &out);
        TT_UT_SUCCESS(ret, "");
        cmp_ret = tt_buf_cmp_cstr(&out, "r-    bool          ");
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // get
    tt_buf_clear(&out);
    val = TT_TRUE;
    ret = tt_param_read(cnode, &out);
    TT_UT_SUCCESS(ret, "");
    cmp_ret = tt_buf_cmp_cstr(&out, "true");
    TT_UT_EQUAL(cmp_ret, 0, "");

    tt_buf_clear(&out);
    val = TT_FALSE;
    ret = tt_param_read(cnode, &out);
    TT_UT_SUCCESS(ret, "");
    cmp_ret = tt_buf_cmp_cstr(&out, "false");
    TT_UT_EQUAL(cmp_ret, 0, "");

    // set
    ret = tt_param_write(cnode, v.addr, v.len);
    TT_UT_EQUAL(ret, TT_E_UNSUPPORT, "");

    tt_param_destroy(cnode);

    // node 2
    tt_param_attr_default(&attr);
    attr.brief = "test bool node";
    attr.can_read = TT_TRUE;
    attr.can_write = TT_TRUE;

    cnode = tt_param_bool_create("node-bool", &val, &attr, &cb);
    TT_UT_NOT_EQUAL(cnode, NULL, "");

    // ls
    {
        val = TT_FALSE;
        tt_buf_clear(&out);
        ret = tt_param_read(cnode, &out);
        TT_UT_SUCCESS(ret, "");
        cmp_ret = tt_buf_cmp_cstr(&out, "false");
        TT_UT_EQUAL(cmp_ret, 0, "");

        tt_buf_clear(&out);
        ret = tt_param_cli_ls(cnode, NULL, NULL, &out);
        TT_UT_SUCCESS(ret, "");
        cmp_ret =
            tt_buf_cmp_cstr(&out,
                            "rw    bool      node-bool    test bool node");
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    tt_buf_clear(&out);
    ret = __param_cli_line(cnode, " ", 0, &out);
    TT_UT_SUCCESS(ret, "");
    cmp_ret = tt_buf_cmp_cstr(&out, "rw bool   node-bool test bool node");
    TT_UT_EQUAL(cmp_ret, 0, "");

    tt_buf_clear(&out);
    ret = __param_cli_line(cnode, " ", 3, &out);
    TT_UT_SUCCESS(ret, "");
    cmp_ret = tt_buf_cmp_cstr(&out, "rw bool   ??? test bool node");
    TT_UT_EQUAL(cmp_ret, 0, "");

    tt_buf_clear(&out);
    ret = __param_cli_line(cnode, " ", 10, &out);
    TT_UT_SUCCESS(ret, "");
    cmp_ret = tt_buf_cmp_cstr(&out, "rw bool   node-bool  test bool node");
    TT_UT_EQUAL(cmp_ret, 0, "");

    // get
    tt_buf_clear(&out);
    val = ~0;
    ret = tt_param_read(cnode, &out);
    TT_UT_SUCCESS(ret, "");
    cmp_ret = tt_buf_cmp_cstr(&out, "true");
    TT_UT_EQUAL(cmp_ret, 0, "");

    // set
    val = TT_FALSE;
    tt_buf_clear(&out);
    v.addr = (tt_u8_t *)"true";
    v.len = (tt_u32_t)sizeof("true") - 1;
    ret = tt_param_write(cnode, v.addr, v.len);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(val, TT_TRUE, "");

    tt_buf_clear(&out);
    v.addr = (tt_u8_t *)"false";
    v.len = (tt_u32_t)sizeof("false") - 1;
    ret = tt_param_write(cnode, v.addr, v.len);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(val, TT_FALSE, "");

    tt_buf_clear(&out);
    ret = tt_param_write(cnode, (tt_u8_t *)"TRUE", 4);
    TT_UT_FAIL(ret, "");

    tt_param_destroy(cnode);

    tt_buf_destroy(&out);

    // test end
    TT_TEST_CASE_LEAVE()
}
