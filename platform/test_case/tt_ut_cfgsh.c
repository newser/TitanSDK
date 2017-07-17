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

#include "tt_unit_test_case_config.h"
#include <unit_test/tt_unit_test.h>

#include <log/tt_log.h>
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

extern tt_result_t __parse_arg(IN tt_cfgsh_t *sh, IN tt_char_t *line);

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static void __cfgsh_ut_enter(void *enter_param);

// === routine declarations ================
TT_TEST_ROUTINE_DECLARE(tt_unit_test_cfgsh_ls)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_cfgsh_help)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_cfgsh_pwd)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_cfgsh_cd)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_cfgsh_get)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_cfgsh_set)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_cfgsh_status)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_cfgsh_commit)

TT_TEST_ROUTINE_DECLARE(tt_unit_test_cfgsh_parse_arg)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(cfgsh_case)

TT_TEST_CASE("tt_unit_test_cfgsh_ls",
             "testing cfg shell: ls",
             tt_unit_test_cfgsh_ls,
             NULL,
             __cfgsh_ut_enter,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_cfgsh_help",
                 "testing cfg shell: help",
                 tt_unit_test_cfgsh_help,
                 NULL,
                 __cfgsh_ut_enter,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_cfgsh_pwd",
                 "testing cfg shell: pwd",
                 tt_unit_test_cfgsh_pwd,
                 NULL,
                 __cfgsh_ut_enter,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_cfgsh_cd",
                 "testing cfg shell: cd",
                 tt_unit_test_cfgsh_cd,
                 NULL,
                 __cfgsh_ut_enter,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_cfgsh_get",
                 "testing cfg shell: get",
                 tt_unit_test_cfgsh_get,
                 NULL,
                 __cfgsh_ut_enter,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_cfgsh_set",
                 "testing cfg shell: set",
                 tt_unit_test_cfgsh_set,
                 NULL,
                 __cfgsh_ut_enter,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_cfgsh_parse_arg",
                 "testing cfg shell: parse arguments",
                 tt_unit_test_cfgsh_parse_arg,
                 NULL,
                 __cfgsh_ut_enter,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_cfgsh_status",
                 "testing cfg shell: status",
                 tt_unit_test_cfgsh_status,
                 NULL,
                 __cfgsh_ut_enter,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_cfgsh_commit",
                 "testing cfg shell: commit",
                 tt_unit_test_cfgsh_commit,
                 NULL,
                 __cfgsh_ut_enter,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(cfgsh_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(TEST_UNIT_CFGSHELL, 0, cfgsh_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(tt_unit_test_cfgsh_commit)
    {
     // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
     tt_cfgsh_t sh;
     tt_result_t ret;
     tt_cfgsh_attr_t attr;
     tt_cli_itf_t itf;
     tt_u32_t cmp_ret;

     TT_TEST_CASE_ENTER()
     // test start

     itf.param = NULL;
     itf.send = __ut_cfgsh_send;

     tt_cfgsh_attr_default(&attr);
     attr.cli_attr.title = "shell";
     attr.cli_attr.seperator = '$';

     ret = tt_cfgsh_create(&sh, TT_CLI_MODE_DEFAUTL, &itf, root, &attr);
     TT_UT_EQUAL(ret, TT_SUCCESS, "");

     tt_buf_clear(&__ut_buf_out);
     ret = tt_cfgsh_start(&sh);
     TT_UT_EQUAL(ret, TT_SUCCESS, "");
     cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, "\nshell$ ");
     TT_UT_EQUAL(cmp_ret, 0, "");

     tt_cfgsh_destroy(&sh);

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    static tt_bool_t __cfgsh_ut_init = TT_TRUE;

static tt_cfgobj_t *root, *g1, *g11, *g12, *g121, *c1211;
static tt_cfgobj_t *u1, *s1;
static tt_u32_t __u32_val;
static tt_s32_t __s32_val;
static tt_buf_t __ut_buf_out;

void __cfgsh_ut_enter(void *enter_param)
{
    tt_cfgobj_attr_t g_attr;
    tt_cfgobj_attr_t u32_attr;
    tt_cfgobj_attr_t s32_attr;

    if (!__cfgsh_ut_init) {
        return;
    }
    __cfgsh_ut_init = TT_FALSE;

    /*
     root
     |- g1
        |- g11
        |- g12
           |- g121
              |- c1211
     |- s1
     |- u1
     */

    tt_cfgobj_attr_default(&g_attr);
    tt_cfgobj_attr_default(&u32_attr);
    tt_cfgobj_attr_default(&s32_attr);

    root = tt_cfgdir_create("", &g_attr);

    g_attr.brief = "group 1, testing";
    g_attr.detail = "group 1, usage";
    g1 = tt_cfgdir_create("g1", &g_attr);
    tt_cfgdir_add(TT_CFGOBJ_CAST(root, tt_cfgdir_t), g1);

    u32_attr.brief = "u32 val under g1";
    u1 = tt_cfgu32_create("u1", &__u32_val, &u32_attr, NULL);
    tt_cfgdir_add(TT_CFGOBJ_CAST(root, tt_cfgdir_t), u1);

    s32_attr.brief = "s32 val under g1";
    s1 = tt_cfgs32_create("s1", &__s32_val, &s32_attr, NULL);
    tt_cfgdir_add(TT_CFGOBJ_CAST(root, tt_cfgdir_t), s1);

    g11 = tt_cfgdir_create("g11", &g_attr);
    g12 = tt_cfgdir_create("g12", &g_attr);
    g121 = tt_cfgdir_create("g121", &g_attr);

    u32_attr.brief = "leaf child";
    u32_attr.detail = "c1211's usage";
    c1211 = tt_cfgu32_create("c1211", &__u32_val, &u32_attr, NULL);
    tt_cfgdir_add(TT_CFGOBJ_CAST(g121, tt_cfgdir_t), c1211);

    tt_cfgdir_add(TT_CFGOBJ_CAST(g1, tt_cfgdir_t), g11);
    tt_cfgdir_add(TT_CFGOBJ_CAST(g1, tt_cfgdir_t), g12);
    tt_cfgdir_add(TT_CFGOBJ_CAST(g12, tt_cfgdir_t), g121);

    tt_buf_init(&__ut_buf_out, NULL);
}

static tt_result_t __ut_cfgsh_send(IN struct tt_cli_s *cli,
                                   IN void *param,
                                   IN tt_u8_t *ev,
                                   IN tt_u32_t ev_num)
{
    tt_u32_t i;

    for (i = 0; i < ev_num; ++i) {
        if (ev[i] == TT_CLI_EV_ENTER) {
            tt_buf_put_u8(&__ut_buf_out, '\n');
        } else {
            tt_buf_put_u8(&__ut_buf_out, ev[i]);
        }
    }

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_cfgsh_ls)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_cfgsh_t sh;
    tt_result_t ret;
    tt_cfgsh_attr_t attr;
    tt_cli_itf_t itf;
    tt_u32_t cmp_ret;

    TT_TEST_CASE_ENTER()
    // test start

    itf.param = NULL;
    itf.send = __ut_cfgsh_send;

    tt_cfgsh_attr_default(&attr);
    attr.cli_attr.title = "shell";
    attr.cli_attr.seperator = '$';

    ret = tt_cfgsh_create(&sh, TT_CLI_MODE_DEFAUTL, &itf, root, &attr);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_buf_clear(&__ut_buf_out);
    ret = tt_cfgsh_start(&sh);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, "\nshell$ ");
    TT_UT_EQUAL(cmp_ret, 0, "");

    // ls current
    {
        const tt_char_t *this_out =
            "ls\n"
            "PERM    TYPE    NAME    DESCRIPTION\n"
            "----    grp     g1/     group 1, testing\n"
            "--gs    s32     s1      s32 val under g1\n"
            "--g-    u32     u1      u32 val under g1\n"
            "shell$ ";

        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)"ls\x87", 3);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // ls single, invalid
    {
        tt_char_t this_in[] = "ls s32\x87";
        const tt_char_t *this_out =
            "ls s32\n"
            "can not find: s32\n"
            "shell$ ";

        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // ls single
    {
        tt_char_t this_in[] = "ls s1\x87";
        const tt_char_t *this_out =
            "ls s1\n"
            "PERM    TYPE    NAME    DESCRIPTION\n"
            "--gs    s32     s1      s32 val under g1\n"
            "shell$ ";

        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // ls single, empty group
    {
        tt_char_t this_in[] = "ls /g1/g11\x87";
        const tt_char_t *this_out =
            "ls /g1/g11\n"
            "PERM    TYPE    NAME    DESCRIPTION\n"
            "\n"
            "shell$ ";

        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // ls multiple, include empty
    {
        tt_char_t this_in[] = "ls s1 /g1/ /u1 g1/g12/g121 g1/g11/  \x87";
        const tt_char_t *this_out =
            "ls s1 /g1/ /u1 g1/g12/g121 g1/g11/  \n"
            "s1:\n"
            "PERM    TYPE    NAME    DESCRIPTION\n"
            "--gs    s32     s1      s32 val under g1\n"
            "\n"
            "/g1/:\n"
            "PERM    TYPE    NAME    DESCRIPTION\n"
            "----    grp     g11/    group 1, testing\n"
            "----    grp     g12/    group 1, testing\n"
            "\n"
            "/u1:\n"
            "PERM    TYPE    NAME    DESCRIPTION\n"
            "--g-    u32     u1      u32 val under g1\n"
            "\n"
            "g1/g12/g121:\n"
            "PERM    TYPE    NAME     DESCRIPTION\n"
            "--gs    u32     c1211    leaf child\n"
            "\n"
            "g1/g11/:\n"
            "PERM    TYPE    NAME    DESCRIPTION\n"
            "\n"
            "\n"
            "shell$ ";

        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // ls multiple, include invalid
    {
        tt_char_t this_in[] = "ls s1 /root/g1/ /root/g1/u1 \x87";
        const tt_char_t *this_out =
            "ls s1 /root/g1/ /root/g1/u1 \n"
            "s1:\n"
            "PERM    TYPE    NAME    DESCRIPTION\n"
            "--gs    s32     s1      s32 val under g1\n"
            "\n"
            "/root/g1/:\n"
            "can not find: /root/g1/\n"
            "\n"
            "/root/g1/u1:\n"
            "can not find: /root/g1/u1\n"
            "\n"
            "shell$ ";

        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    tt_cfgsh_destroy(&sh);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_cfgsh_help)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_cfgsh_t sh;
    tt_result_t ret;
    tt_cfgsh_attr_t attr;
    tt_cli_itf_t itf;
    tt_u32_t cmp_ret;

    TT_TEST_CASE_ENTER()
    // test start

    itf.param = NULL;
    itf.send = __ut_cfgsh_send;

    tt_cfgsh_attr_default(&attr);
    attr.cli_attr.title = "shell";
    attr.cli_attr.seperator = '$';

    ret = tt_cfgsh_create(&sh, TT_CLI_MODE_DEFAUTL, &itf, root, &attr);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_buf_clear(&__ut_buf_out);
    ret = tt_cfgsh_start(&sh);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, "\nshell$ ");
    TT_UT_EQUAL(cmp_ret, 0, "");

    // list all commands
    {
        tt_char_t this_in[] = "  help  \x87";
        const tt_char_t *this_out = "";

        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        // TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // unknown cmd
    {
        tt_char_t this_in[] = "  help unknown-cmd  \x87";
        const tt_char_t *this_out =
            "  help unknown-cmd  \n"
            "can not find: unknown-cmd\n"
            "shell$ ";

        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // show an existed command
    {
        tt_char_t this_in[] = "  help ls  \x87";
        const tt_char_t *this_out =
            "  help ls  \n"
            "testing\n"
            "shell$ ";

        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // show an existed value
    {
        tt_char_t this_in[] = "  help /g1/g12/g121/c1211  \x87";
        const tt_char_t *this_out =
            "  help /g1/g12/g121/c1211  \n"
            "c1211's usage\n"
            "shell$ ";

        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // show an existed group
    {
        tt_char_t this_in[] = "  help g1\x87";
        const tt_char_t *this_out =
            "  help g1\n"
            "group 1, usage\n"
            "shell$ ";

        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    tt_cfgsh_destroy(&sh);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_cfgsh_pwd)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_cfgsh_t sh;
    tt_result_t ret;
    tt_cfgsh_attr_t attr;
    tt_cli_itf_t itf;
    tt_u32_t cmp_ret;

    TT_TEST_CASE_ENTER()
    // test start

    itf.param = NULL;
    itf.send = __ut_cfgsh_send;

    tt_cfgsh_attr_default(&attr);
    attr.cli_attr.title = "shell";
    attr.cli_attr.seperator = '$';

    ret = tt_cfgsh_create(&sh, TT_CLI_MODE_DEFAUTL, &itf, root, &attr);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_buf_clear(&__ut_buf_out);
    ret = tt_cfgsh_start(&sh);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, "\nshell$ ");
    TT_UT_EQUAL(cmp_ret, 0, "");

    // pwd root
    {
        tt_char_t this_in[] = "  pwd \x87";
        const tt_char_t *this_out =
            "  pwd \n"
            "/\n"
            "shell$ ";

        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // pwd leaf
    {
        tt_char_t this_in[] = "  pwd 123 \x87";
        const tt_char_t *this_out =
            "  pwd 123 \n"
            "/g1/g12/g121\n"
            "shell$ ";

        tt_buf_clear(&__ut_buf_out);
        sh.current = g121;
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // pwd middle
    {
        tt_char_t this_in[] = "  pwd 123  111 22 33\x87";
        const tt_char_t *this_out =
            "  pwd 123  111 22 33\n"
            "/g1/g12\n"
            "shell$ ";

        tt_buf_clear(&__ut_buf_out);
        sh.current = g12;
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    tt_cfgsh_destroy(&sh);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_cfgsh_cd)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_cfgsh_t sh;
    tt_result_t ret;
    tt_cfgsh_attr_t attr;
    tt_cli_itf_t itf;
    tt_u32_t cmp_ret;

    TT_TEST_CASE_ENTER()
    // test start

    itf.param = NULL;
    itf.send = __ut_cfgsh_send;

    tt_cfgsh_attr_default(&attr);
    attr.cli_attr.title = "shell";
    attr.cli_attr.seperator = '$';

    ret = tt_cfgsh_create(&sh, TT_CLI_MODE_DEFAUTL, &itf, root, &attr);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_buf_clear(&__ut_buf_out);
    ret = tt_cfgsh_start(&sh);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, "\nshell$ ");
    TT_UT_EQUAL(cmp_ret, 0, "");

    // cd, no param
    {
        tt_char_t this_in[] = "  cd\x87";
        const tt_char_t *this_out =
            "  cd\n"
            "shell$ ";

        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // cd middle
    {
        tt_char_t this_in[] = "  cd g1\x87";
        const tt_char_t *this_out =
            "  cd g1\n"
            "shell:g1$ ";

        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // ls
    {
        tt_char_t this_in[] = "ls\x87";
        const tt_char_t *this_out =
            "ls\n"
            "PERM    TYPE    NAME    DESCRIPTION\n"
            "----    grp     g11/    group 1, testing\n"
            "----    grp     g12/    group 1, testing\n"
            "shell:g1$ ";

        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // cd leaf
    {
        tt_char_t this_in[] = "  cd g12/g121/\x87";
        const tt_char_t *this_out =
            "  cd g12/g121/\n"
            "shell:g121$ ";

        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // cd absolute path
    {
        tt_char_t this_in[] = "  cd /g1/g11 \x87";
        const tt_char_t *this_out =
            "  cd /g1/g11 \n"
            "shell:g11$ ";

        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // cd invalid path
    {
        tt_char_t this_in[] = "  cd ../g1/g1111 \x87";
        const tt_char_t this_out[] =
            "  cd ../g1/g1111 \n"
            "can not find: ../g1/g1111\n"
            "shell:g11$ ";

        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        tt_buf_print_cstr(&__ut_buf_out, 0);
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // cd non-group
    {
        tt_char_t this_in[] = "  cd /g1/g12/g121/c1211\x87";
        const tt_char_t *this_out =
            "  cd /g1/g12/g121/c1211\n"
            "not a group: /g1/g12/g121/c1211\n"
            "shell:g11$ ";

        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // exit
    {
        tt_char_t this_in[] = "\x85";
        const tt_char_t *this_out = "^D\nexiting\n";

        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_END, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    tt_cfgsh_destroy(&sh);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_cfgsh_get)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_cfgsh_t sh;
    tt_result_t ret;
    tt_cfgsh_attr_t attr;
    tt_cli_itf_t itf;
    tt_u32_t cmp_ret;

    TT_TEST_CASE_ENTER()
    // test start

    itf.param = NULL;
    itf.send = __ut_cfgsh_send;

    tt_cfgsh_attr_default(&attr);
    attr.cli_attr.title = "shell";
    attr.cli_attr.seperator = '$';

    ret = tt_cfgsh_create(&sh, TT_CLI_MODE_DEFAUTL, &itf, root, &attr);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_buf_clear(&__ut_buf_out);
    ret = tt_cfgsh_start(&sh);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, "\nshell$ ");
    TT_UT_EQUAL(cmp_ret, 0, "");

    // get single
    {
        tt_char_t this_in[] = "  get /g1/g12/g121/c1211\x87";
        const tt_char_t *this_out =
            "  get /g1/g12/g121/c1211\n"
            "NAME     VALUE\n"
            "c1211    12300\n"
            "shell$ ";

        __u32_val = 12300;
        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // get single, invalid path
    {
        tt_char_t this_in[] = "  get /g1/g12/g121/c12111\x87";
        const tt_char_t *this_out =
            "  get /g1/g12/g121/c12111\n"
            "can not find: /g1/g12/g121/c12111\n"
            "shell$ ";

        __u32_val = 12300;
        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // get a group
    {
        tt_char_t this_in[] = "  get g1/g12\x87";
        const tt_char_t *this_out =
            "  get g1/g12\n"
            "NAME    VALUE\n"
            "g121    n/a(group)\n"
            "shell$ ";

        __u32_val = 12300;
        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // get multiple
    {
        tt_char_t this_in[] = "  get s1 /u1\x87";
        const tt_char_t *this_out =
            "  get s1 /u1\n"
            "s1:\n"
            "NAME    VALUE\n"
            "s1      0\n"
            "\n"
            "/u1:\n"
            "NAME    VALUE\n"
            "u1      12300\n"
            "\n"
            "shell$ ";

        __u32_val = 12300;
        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // get multiple, with some invalid path, some group
    {
        tt_char_t this_in[] = "  get s1 s1234567890 g1 /u1\x87";
        const tt_char_t *this_out =
            "  get s1 s1234567890 g1 /u1\n"
            "s1:\n"
            "NAME    VALUE\n"
            "s1      0\n"
            "\n"
            "s1234567890:\n"
            "can not find: s1234567890\n"
            "\n"
            "g1:\n"
            "NAME    VALUE\n"
            "g11     n/a(group)\n"
            "g12     n/a(group)\n"
            "\n"
            "/u1:\n"
            "NAME    VALUE\n"
            "u1      12300\n"
            "\n"
            "shell$ ";

        __u32_val = 12300;
        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // get current
    {
        tt_char_t this_in[] = "  get \x87";
        const tt_char_t *this_out =
            "  get \n"
            "NAME    VALUE\n"
            "g1      n/a(group)\n"
            "s1      0\n"
            "u1      12300\n"
            "shell$ ";

        __u32_val = 12300;
        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // get current, in a empty group
    {
        tt_char_t this_in[] = "  get \x87";
        const tt_char_t *this_out =
            "  get \n"
            "NAME    VALUE\n"
            "shell$ ";

        __u32_val = 12300;
        tt_buf_clear(&__ut_buf_out);
        sh.current = g11;
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    tt_cfgsh_destroy(&sh);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_cfgsh_set)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_cfgsh_t sh;
    tt_result_t ret;
    tt_cfgsh_attr_t attr;
    tt_cli_itf_t itf;
    tt_u32_t cmp_ret;

    TT_TEST_CASE_ENTER()
    // test start

    itf.param = NULL;
    itf.send = __ut_cfgsh_send;

    tt_cfgsh_attr_default(&attr);
    attr.cli_attr.title = "shell";
    attr.cli_attr.seperator = '$';

    ret = tt_cfgsh_create(&sh, TT_CLI_MODE_DEFAUTL, &itf, root, &attr);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_buf_clear(&__ut_buf_out);
    ret = tt_cfgsh_start(&sh);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, "\nshell$ ");
    TT_UT_EQUAL(cmp_ret, 0, "");

    // set a non writtable value
    {
        tt_char_t this_in[] = "  set /u1 999\x87";
        const tt_char_t *this_out =
            "  set /u1 999\n"
            "not supported operation\n"
            "shell$ ";

        __u32_val = 12300;
        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // set a value, invalid command
    {
        tt_char_t this_in[] = "  set s1\x87";
        const tt_char_t *this_out =
            "  set s1\n"
            "usage: set name value\n"
            "shell$ ";

        __u32_val = 12300;
        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // set a value
    {
        tt_char_t this_in[] = "  set s1 999\x87";
        const tt_char_t *this_out =
            "  set s1 999\n"
            "shell$ ";

        __u32_val = 12300;
        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // set a invalid value
    {
        tt_char_t this_in[] = "  set s123 999\x87";
        const tt_char_t *this_out =
            "  set s123 999\n"
            "can not find: s123\n"
            "shell$ ";

        __u32_val = 12300;
        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // set a group
    {
        tt_char_t this_in[] = "  set g1 999\x87";
        const tt_char_t *this_out =
            "  set g1 999\n"
            "not supported operation\n"
            "shell$ ";

        __u32_val = 12300;
        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    tt_cfgsh_destroy(&sh);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_cfgsh_parse_arg)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_cfgsh_t sh;
    tt_result_t ret;
    tt_cfgsh_attr_t attr;
    tt_cli_itf_t itf;
    tt_u32_t cmp_ret;

    TT_TEST_CASE_ENTER()
    // test start

    itf.param = NULL;
    itf.send = __ut_cfgsh_send;

    tt_cfgsh_attr_default(&attr);
    attr.cli_attr.title = "shell";
    attr.cli_attr.seperator = '$';

    ret = tt_cfgsh_create(&sh, TT_CLI_MODE_DEFAUTL, &itf, root, &attr);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_buf_clear(&__ut_buf_out);
    ret = tt_cfgsh_start(&sh);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, "\nshell$ ");
    TT_UT_EQUAL(cmp_ret, 0, "");

    // normal
    {
        tt_char_t cmd[] = "  cmd 1 22 333";

        sh.arg_idx = 0;
        ret = __parse_arg(&sh, cmd);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(sh.arg_idx, 4, "");
        TT_UT_EQUAL(tt_strcmp(sh.arg[0], "cmd"), 0, "");
        TT_UT_EQUAL(tt_strcmp(sh.arg[1], "1"), 0, "");
        TT_UT_EQUAL(tt_strcmp(sh.arg[2], "22"), 0, "");
        TT_UT_EQUAL(tt_strcmp(sh.arg[3], "333"), 0, "");
    }

    // empty
    {
        tt_char_t cmd[] = "";

        sh.arg_idx = 0;
        ret = __parse_arg(&sh, cmd);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(sh.arg_idx, 0, "");
    }

    // only spaces
    {
        tt_char_t cmd[] = "       ";

        sh.arg_idx = 0;
        ret = __parse_arg(&sh, cmd);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(sh.arg_idx, 0, "");
    }

    // with double quotes
    {
        tt_char_t cmd[] = " cmd \" arg1 'not param' arg1.1 \" arg2     ";

        sh.arg_idx = 0;
        ret = __parse_arg(&sh, cmd);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(sh.arg_idx, 3, "");
        TT_UT_EQUAL(tt_strcmp(sh.arg[0], "cmd"), 0, "");
        TT_UT_EQUAL(tt_strcmp(sh.arg[1], " arg1 'not param' arg1.1 "), 0, "");
        TT_UT_EQUAL(tt_strcmp(sh.arg[2], "arg2"), 0, "");
    }

    // half double quotes
    {
        tt_char_t cmd[] = "\" arg1 'not param' arg1.1   ";

        sh.arg_idx = 0;
        ret = __parse_arg(&sh, cmd);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(sh.arg_idx, 1, "");
        TT_UT_EQUAL(tt_strcmp(sh.arg[0], " arg1 'not param' arg1.1   "), 0, "");
    }

    // with single quotes
    {
        tt_char_t cmd[] = " cmd ' \"this is not q\" ' arg2     ";

        sh.arg_idx = 0;
        ret = __parse_arg(&sh, cmd);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(sh.arg_idx, 3, "");
        TT_UT_EQUAL(tt_strcmp(sh.arg[0], "cmd"), 0, "");
        TT_UT_EQUAL(tt_strcmp(sh.arg[1], " \"this is not q\" "), 0, "");
        TT_UT_EQUAL(tt_strcmp(sh.arg[2], "arg2"), 0, "");
    }

    // half single quotes
    {
        tt_char_t cmd[] = "' arg1 \"not param \" arg1.1   ";

        sh.arg_idx = 0;
        ret = __parse_arg(&sh, cmd);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(sh.arg_idx, 1, "");
        TT_UT_EQUAL(tt_strcmp(sh.arg[0], " arg1 \"not param \" arg1.1   "),
                    0,
                    "");
    }


    tt_cfgsh_destroy(&sh);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_cfgsh_status)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_cfgsh_t sh;
    tt_result_t ret;
    tt_cfgsh_attr_t attr;
    tt_cli_itf_t itf;
    tt_u32_t cmp_ret;
    tt_blob_t val;

    TT_TEST_CASE_ENTER()
    // test start

    itf.param = NULL;
    itf.send = __ut_cfgsh_send;

    tt_cfgsh_attr_default(&attr);
    attr.cli_attr.title = "shell";
    attr.cli_attr.seperator = '$';

    ret = tt_cfgsh_create(&sh, TT_CLI_MODE_DEFAUTL, &itf, root, &attr);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_buf_clear(&__ut_buf_out);
    ret = tt_cfgsh_start(&sh);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, "\nshell$ ");
    TT_UT_EQUAL(cmp_ret, 0, "");

    ///////////

    // tt_cfgobj_restore(root);

    // status current, no change
    {
        tt_char_t this_in[] = "  status\x87";
        const tt_char_t *this_out =
            "  status\n"
            "STATUS    NAME    DESCRIPTION\n"
            "\n"
            "shell$ ";

        __u32_val = 12300;
        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // status single, no change
    {
        tt_char_t this_in[] = "  status /g1/\x87";
        const tt_char_t *this_out =
            "  status /g1/\n"
            "STATUS    NAME    DESCRIPTION\n"
            "\n"
            "shell$ ";

        __u32_val = 12300;
        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // status multiple, no change, invalid path
    {
        tt_char_t this_in[] = "  status /g1/ / /g1/g12/g121 xx/\x87";
        const tt_char_t *this_out =
            "  status /g1/ / /g1/g12/g121 xx/\n"
            "/g1/:\n"
            "STATUS    NAME    DESCRIPTION\n"
            "\n"
            "\n"
            "/:\n"
            "STATUS    NAME    DESCRIPTION\n"
            "\n"
            "\n"
            "/g1/g12/g121:\n"
            "STATUS    NAME    DESCRIPTION\n"
            "\n"
            "\n"
            "xx/:\n"
            "can not find: xx/\n"
            "\n"
            "shell$ ";

        __u32_val = 12300;
        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // change some values
    val.addr = (tt_u8_t *)"-119922";
    val.len = (tt_u32_t)sizeof("-119922") - 1;
    ret = tt_cfgobj_write(s1, val.addr, val.len);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    val.addr = (tt_u8_t *)"229923";
    val.len = (tt_u32_t)sizeof("229923") - 1;
    ret = tt_cfgobj_write(c1211, val.addr, val.len);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    // status current, some are changed
    {
        tt_char_t this_in[] = "  status\x87";
        const tt_char_t *this_out =
            "  status\n"
            "STATUS    NAME                 DESCRIPTION\n"
            "M         g1/g12/g121/c1211    12300 --> 229923\n"
            "M         s1                   0 --> -119922\n"
            "shell$ ";

        __u32_val = 12300;
        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // status single
    {
        tt_char_t this_in[] = "  status g1\x87";
        const tt_char_t *this_out =
            "  status g1\n"
            "STATUS    NAME              DESCRIPTION\n"
            "M         g12/g121/c1211    12300 --> 229923\n"
            "shell$ ";

        __u32_val = 12300;
        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // status multiple
    {
        tt_char_t this_in[] = "  status g1 . /s1 u1  g1/g11/ xyz \x87";
        const tt_char_t *this_out =
            "  status g1 . /s1 u1  g1/g11/ xyz \n"
            "g1:\n"
            "STATUS    NAME              DESCRIPTION\n"
            "M         g12/g121/c1211    12300 --> 229923\n"
            "\n"
            ".:\n"
            "STATUS    NAME                 DESCRIPTION\n"
            "M         g1/g12/g121/c1211    12300 --> 229923\n"
            "M         s1                   0 --> -119922\n"
            "\n"
            "/s1:\n"
            "STATUS    NAME    DESCRIPTION\n"
            "M         s1      0 --> -119922\n"
            "\n"
            "u1:\n"
            "STATUS    NAME    DESCRIPTION\n"
            "\n"
            "\n"
            "g1/g11/:\n"
            "STATUS    NAME    DESCRIPTION\n"
            "\n"
            "\n"
            "xyz:\n"
            "can not find: xyz\n"
            "\n"
            "shell$ ";

        __u32_val = 12300;
        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    ////////////

    // restore invalid
    {
        tt_char_t this_in[] =
            "  restore g1/g12/g121/c1222 g1/g12/g121/c1233\x87status / \x87";
        const tt_char_t *this_out =
            "  restore g1/g12/g121/c1222 g1/g12/g121/c1233\n"
            "can not find: g1/g12/g121/c1222\n"
            "can not find: g1/g12/g121/c1233\n"
            "shell$ status / \n"
            "STATUS    NAME                 DESCRIPTION\n"
            "M         g1/g12/g121/c1211    12300 --> 229923\n"
            "M         s1                   0 --> -119922\n"
            "shell$ ";

        __u32_val = 12300;
        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // restore a value
    {
        tt_char_t this_in[] =
            "  restore g1/g12/g121/c1211 g1/g12/g121/c1222\x87status / \x87";
        const tt_char_t *this_out =
            "  restore g1/g12/g121/c1211 g1/g12/g121/c1222\n"
            "can not find: g1/g12/g121/c1222\n"
            "shell$ status / \n"
            "STATUS    NAME    DESCRIPTION\n"
            "M         s1      0 --> -119922\n"
            "shell$ ";

        __u32_val = 12300;
        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // restore a group
    {
        tt_char_t this_in[] = "  restore g1/g13 g1/g12 /\x87status / \x87";
        const tt_char_t *this_out =
            "  restore g1/g13 g1/g12 /\n"
            "can not find: g1/g13\n"
            "shell$ status / \n"
            "STATUS    NAME    DESCRIPTION\n"
            "\n"
            "shell$ ";

        __u32_val = 12300;
        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    ////////////

    tt_cfgsh_destroy(&sh);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_cfgsh_commit)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_cfgsh_t sh;
    tt_result_t ret;
    tt_cfgsh_attr_t attr;
    tt_cli_itf_t itf;
    tt_u32_t cmp_ret;

    TT_TEST_CASE_ENTER()
    // test start

    itf.param = NULL;
    itf.send = __ut_cfgsh_send;

    tt_cfgsh_attr_default(&attr);
    attr.cli_attr.title = "shell";
    attr.cli_attr.seperator = '$';

    ret = tt_cfgsh_create(&sh, TT_CLI_MODE_DEFAUTL, &itf, root, &attr);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_buf_clear(&__ut_buf_out);
    ret = tt_cfgsh_start(&sh);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, "\nshell$ ");
    TT_UT_EQUAL(cmp_ret, 0, "");

    ///////////////////////
    // invalid
    {
        tt_char_t this_in[] = "  commit /g1/\x87";
        const tt_char_t *this_out =
            "  commit /g1/\n"
            "usage: commit\n"
            "shell$ ";

        __u32_val = 12300;
        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // nothing is changed
    {
        tt_char_t this_in[] = "  commit\x87";
        const tt_char_t *this_out =
            "  commit\n"
            "shell$ ";

        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // changed, but not need reboot
    {
        tt_char_t this_in[] = "  commit\x87";
        const tt_char_t *this_out =
            "  commit\n"
            "committing configuration...done\n"
            "shell$ ";

        // hack
        // s1->modified = TT_TRUE;

        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");

        // s1->modified = TT_FALSE;
    }

    // changed, need reboot
    {
        tt_char_t this_in[] = "  commit\x87";
        const tt_char_t *this_out =
            "  commit\n"
            "committing current configuration MAY reboot\n"
            "are you sure to commit? [y/N] ";

        tt_char_t yes_in[] = "Y\x87";
        tt_char_t yes_out[] =
            "Y\n"
            "committing configuration...done\n"
            "exiting\n";

        tt_char_t no_in[] = "YY\x87";
        tt_char_t no_out[] = "YY\nshell$ ";

        // hack
        // c1211->modified = TT_TRUE;
        c1211->need_reboot = TT_TRUE;

        // commit
        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");

        // no
        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)no_in, sizeof(no_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, no_out);
        TT_UT_EQUAL(cmp_ret, 0, "");

        // commit
        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");

        // yes
        tt_buf_clear(&__ut_buf_out);
        ret = tt_cfgsh_input(&sh, (tt_u8_t *)yes_in, sizeof(yes_in) - 1);
        TT_UT_EQUAL(ret, TT_END, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, yes_out);
        TT_UT_EQUAL(cmp_ret, 0, "");

        c1211->need_reboot = TT_FALSE;
        // c1211->modified = TT_FALSE;
    }

    ///////////////////////

    tt_cfgsh_destroy(&sh);

    // test end
    TT_TEST_CASE_LEAVE()
}
