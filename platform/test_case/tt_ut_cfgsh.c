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

extern tt_result_t __parse_arg(IN tt_shell_t *sh, IN tt_char_t *line);

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static void __cfgsh_ut_enter(void *enter_param);

static void __cfgsh_ut_exit(void *exit_param);

// === routine declarations ================
TT_TEST_ROUTINE_DECLARE(case_cfgsh_ls)
TT_TEST_ROUTINE_DECLARE(case_cfgsh_help)
TT_TEST_ROUTINE_DECLARE(case_cfgsh_pwd)
TT_TEST_ROUTINE_DECLARE(case_cfgsh_cd)
TT_TEST_ROUTINE_DECLARE(case_cfgsh_get)
TT_TEST_ROUTINE_DECLARE(case_cfgsh_set)
TT_TEST_ROUTINE_DECLARE(case_cfgsh_quit)

TT_TEST_ROUTINE_DECLARE(case_cfgsh_parse_arg)
TT_TEST_ROUTINE_DECLARE(case_cfgsh_exec)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(cfgsh_case)

TT_TEST_CASE("case_cfgsh_parse_arg", "testing shell: parse arguments",
             case_cfgsh_parse_arg, NULL, __cfgsh_ut_enter, NULL,
             __cfgsh_ut_exit, NULL)
,

    TT_TEST_CASE("case_cfgsh_ls", "testing shell: ls", case_cfgsh_ls, NULL,
                 __cfgsh_ut_enter, NULL, __cfgsh_ut_exit, NULL),

    TT_TEST_CASE("case_cfgsh_help", "testing shell: help", case_cfgsh_help,
                 NULL, __cfgsh_ut_enter, NULL, __cfgsh_ut_exit, NULL),

    TT_TEST_CASE("case_cfgsh_pwd", "testing shell: pwd", case_cfgsh_pwd, NULL,
                 __cfgsh_ut_enter, NULL, __cfgsh_ut_exit, NULL),

    TT_TEST_CASE("case_cfgsh_cd", "testing shell: cd", case_cfgsh_cd, NULL,
                 __cfgsh_ut_enter, NULL, __cfgsh_ut_exit, NULL),

    TT_TEST_CASE("case_cfgsh_get", "testing shell: get", case_cfgsh_get, NULL,
                 __cfgsh_ut_enter, NULL, __cfgsh_ut_exit, NULL),

    TT_TEST_CASE("case_cfgsh_set", "testing shell: set", case_cfgsh_set, NULL,
                 __cfgsh_ut_enter, NULL, __cfgsh_ut_exit, NULL),

    TT_TEST_CASE("case_cfgsh_quit", "testing shell: quit", case_cfgsh_quit,
                 NULL, __cfgsh_ut_enter, NULL, __cfgsh_ut_exit, NULL),

    TT_TEST_CASE("case_cfgsh_exec", "testing shell: execute", case_cfgsh_exec,
                 NULL, __cfgsh_ut_enter, NULL, __cfgsh_ut_exit, NULL),

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
    TT_TEST_ROUTINE_DEFINE(case_cfgsh_quit)
    {
     // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
     tt_shell_t sh;
     tt_result_t ret;
     tt_sh_attr_t attr;
     tt_cli_itf_t itf;
     tt_u32_t cmp_ret;

     TT_TEST_CASE_ENTER()
     // test start

     itf.param = NULL;
     itf.send = __ut_cfgsh_send;

     tt_sh_attr_default(&attr);
     attr.cli_attr.title = "shell";
     attr.cli_attr.seperator = '$';

     ret = tt_sh_create(&sh, root, TT_CLI_MODE_DEFAUTL, &itf, &attr);
     TT_UT_EQUAL(ret, TT_SUCCESS, "");

     tt_buf_clear(&__ut_buf_out);
     ret = tt_sh_start(&sh);
     TT_UT_EQUAL(ret, TT_SUCCESS, "");
     cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, "\nshell$ ");
     TT_UT_EQUAL(cmp_ret, 0, "");

     tt_sh_destroy(&sh);

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    static tt_bool_t __cfgsh_ut_init = TT_TRUE;

static tt_param_t *root, *g1, *g11, *g12, *g121, *c1211, *exec;
static tt_param_t *u1, *s1;
static tt_u32_t __u32_val;
static tt_s32_t __s32_val;
static tt_buf_t __ut_buf_out;

static tt_u32_t exec_mode;
static tt_result_t __exec_run(IN struct tt_param_s *co, IN tt_u32_t argc,
                              IN tt_char_t *argv[], OUT struct tt_buf_s *output,
                              OUT tt_u32_t *status)
{
    if (exec_mode == 0) {
        *status = TT_CLIOC_NOOUT;
        return TT_SUCCESS;
    } else if (exec_mode == 1) {
        return TT_E_BADARG;
    }

    if (argc > 0) {
        tt_buf_putf(output, "arg[0]: %s, final: %s", argv[0], argv[argc - 1]);
    } else {
        tt_buf_putf(output, "no arg");
    }
    *status = TT_CLIOC_OUT;
    return TT_SUCCESS;
}

void __cfgsh_ut_enter(void *enter_param)
{
    tt_param_attr_t g_attr;
    tt_param_attr_t u32_attr;
    tt_param_attr_t s32_attr;
    tt_param_attr_t exec_attr;

    if (!__cfgsh_ut_init) { return; }
    __cfgsh_ut_init = TT_FALSE;

    /*
     root
     |- g1
        |- g11
        |- g12
           |- g121
              |- c1211
              |- exec
     |- s1
     |- u1
     */

    tt_param_attr_default(&g_attr);
    tt_param_attr_default(&u32_attr);
    tt_param_attr_default(&s32_attr);
    tt_param_attr_default(&exec_attr);

    root = tt_param_dir_create("", &g_attr);

    g_attr.brief = "group 1, testing";
    g_attr.detail = "group 1, usage";
    g1 = tt_param_dir_create("g1", &g_attr);
    tt_param_dir_add(TT_PARAM_CAST(root, tt_param_dir_t), g1);

    u32_attr.brief = "u32 val under g1";
    u32_attr.can_write = TT_FALSE;
    u1 = tt_param_u32_create("u1", &__u32_val, &u32_attr, NULL);
    tt_param_dir_add(TT_PARAM_CAST(root, tt_param_dir_t), u1);

    s32_attr.brief = "s32 val under g1";
    s1 = tt_param_s32_create("s1", &__s32_val, &s32_attr, NULL);
    tt_param_dir_add(TT_PARAM_CAST(root, tt_param_dir_t), s1);

    g11 = tt_param_dir_create("g11", &g_attr);
    g12 = tt_param_dir_create("g12", &g_attr);
    g121 = tt_param_dir_create("g121", &g_attr);

    u32_attr.brief = "leaf child";
    u32_attr.detail = "c1211's usage";
    u32_attr.can_write = TT_TRUE;
    c1211 = tt_param_u32_create("c1211", &__u32_val, &u32_attr, NULL);
    tt_param_dir_add(TT_PARAM_CAST(g121, tt_param_dir_t), c1211);

    exec = tt_param_exe_create("exec", NULL, __exec_run);
    tt_param_dir_add(TT_PARAM_CAST(g121, tt_param_dir_t), exec);

    tt_param_dir_add(TT_PARAM_CAST(g1, tt_param_dir_t), g11);
    tt_param_dir_add(TT_PARAM_CAST(g1, tt_param_dir_t), g12);
    tt_param_dir_add(TT_PARAM_CAST(g12, tt_param_dir_t), g121);

    tt_buf_init(&__ut_buf_out, NULL);
}

void __cfgsh_ut_exit(void *exit_param)
{
    __cfgsh_ut_init = TT_TRUE;

    tt_param_destroy(root);
    root = NULL;

    tt_buf_destroy(&__ut_buf_out);
}

static tt_result_t __ut_cfgsh_send(IN struct tt_cli_s *cli, IN void *param,
                                   IN tt_u8_t *ev, IN tt_u32_t ev_num)
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

TT_TEST_ROUTINE_DEFINE(case_cfgsh_parse_arg)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_shell_t sh;
    tt_result_t ret;
    tt_sh_attr_t attr;
    tt_cli_itf_t itf;
    tt_u32_t cmp_ret;

    TT_TEST_CASE_ENTER()
    // test start

    itf.param = NULL;
    itf.send = __ut_cfgsh_send;

    tt_sh_attr_default(&attr);
    attr.cli_attr.title = "shell";
    attr.cli_attr.seperator = '$';

    ret = tt_sh_create(&sh, root, TT_CLI_MODE_DEFAUTL, &itf, &attr);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_sh_destroy(&sh);
    return TT_SUCCESS;

    ret = tt_sh_create(&sh, root, TT_CLI_MODE_DEFAUTL, &itf, &attr);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_buf_clear(&__ut_buf_out);
    ret = tt_sh_start(&sh);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, "\nshell$ ");
    TT_UT_EQUAL(cmp_ret, 0, "");

    // normal
    {
        tt_char_t cmd[] = "  cmd 1 22 333";

        sh.arg_num = 0;
        ret = __parse_arg(&sh, cmd);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(sh.arg_num, 4, "");
        TT_UT_EQUAL(tt_strcmp(sh.arg[0], "cmd"), 0, "");
        TT_UT_EQUAL(tt_strcmp(sh.arg[1], "1"), 0, "");
        TT_UT_EQUAL(tt_strcmp(sh.arg[2], "22"), 0, "");
        TT_UT_EQUAL(tt_strcmp(sh.arg[3], "333"), 0, "");
    }

    // empty
    {
        tt_char_t cmd[] = "";

        sh.arg_num = 0;
        ret = __parse_arg(&sh, cmd);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(sh.arg_num, 0, "");
    }

    // only spaces
    {
        tt_char_t cmd[] = "       ";

        sh.arg_num = 0;
        ret = __parse_arg(&sh, cmd);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(sh.arg_num, 0, "");
    }

    // with double quotes
    {
        tt_char_t cmd[] = " cmd \" arg1 'not param' arg1.1 \" arg2     ";

        sh.arg_num = 0;
        ret = __parse_arg(&sh, cmd);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(sh.arg_num, 3, "");
        TT_UT_EQUAL(tt_strcmp(sh.arg[0], "cmd"), 0, "");
        TT_UT_EQUAL(tt_strcmp(sh.arg[1], " arg1 'not param' arg1.1 "), 0, "");
        TT_UT_EQUAL(tt_strcmp(sh.arg[2], "arg2"), 0, "");
    }

    // half double quotes
    {
        tt_char_t cmd[] = "\" arg1 'not param' arg1.1   ";

        sh.arg_num = 0;
        ret = __parse_arg(&sh, cmd);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(sh.arg_num, 1, "");
        TT_UT_EQUAL(tt_strcmp(sh.arg[0], " arg1 'not param' arg1.1   "), 0, "");
    }

    // with single quotes
    {
        tt_char_t cmd[] = " cmd ' \"this is not q\" ' arg2     ";

        sh.arg_num = 0;
        ret = __parse_arg(&sh, cmd);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(sh.arg_num, 3, "");
        TT_UT_EQUAL(tt_strcmp(sh.arg[0], "cmd"), 0, "");
        TT_UT_EQUAL(tt_strcmp(sh.arg[1], " \"this is not q\" "), 0, "");
        TT_UT_EQUAL(tt_strcmp(sh.arg[2], "arg2"), 0, "");
    }

    // half single quotes
    {
        tt_char_t cmd[] = "' arg1 \"not param \" arg1.1   ";

        sh.arg_num = 0;
        ret = __parse_arg(&sh, cmd);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(sh.arg_num, 1, "");
        TT_UT_EQUAL(tt_strcmp(sh.arg[0], " arg1 \"not param \" arg1.1   "), 0,
                    "");
    }

    // end with single quotes
    {
        tt_char_t cmd[] = " cmd p1 p2 \'";

        sh.arg_num = 0;
        ret = __parse_arg(&sh, cmd);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(sh.arg_num, 3, "");
        TT_UT_EQUAL(tt_strcmp(sh.arg[0], "cmd"), 0, "");
        TT_UT_EQUAL(tt_strcmp(sh.arg[1], "p1"), 0, "");
        TT_UT_EQUAL(tt_strcmp(sh.arg[2], "p2"), 0, "");
    }

    // expand args
    {
        tt_char_t cmd[] = " cmd 0 1 2 3 4 5 6 7 8 9 10 11 12 13 \"";

        sh.arg_num = 0;
        ret = __parse_arg(&sh, cmd);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(sh.arg_num, 15, "");
        TT_UT_EQUAL(tt_strcmp(sh.arg[1], "0"), 0, "");
        TT_UT_EQUAL(tt_strcmp(sh.arg[11], "10"), 0, "");
        TT_UT_EQUAL(tt_strcmp(sh.arg[14], "13"), 0, "");
    }

    tt_sh_destroy(&sh);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_cfgsh_ls)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_shell_t sh;
    tt_result_t ret;
    tt_sh_attr_t attr;
    tt_cli_itf_t itf;
    tt_u32_t cmp_ret;

    TT_TEST_CASE_ENTER()
    // test start

    itf.param = NULL;
    itf.send = __ut_cfgsh_send;

    tt_sh_attr_default(&attr);
    attr.cli_attr.title = "shell";
    attr.cli_attr.seperator = '$';

    ret = tt_sh_create(&sh, root, TT_CLI_MODE_DEFAUTL, &itf, &attr);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_buf_clear(&__ut_buf_out);
    ret = tt_sh_start(&sh);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, "\nshell$ ");
    TT_UT_EQUAL(cmp_ret, 0, "");

    // ls current
    {
        const tt_char_t *this_out =
            "ls\n"
            "--    dir       g1/    group 1, testing\n"
            "r-    u32       u1     u32 val under g1\n"
            "rw    s32       s1     s32 val under g1\n"
            "shell$ ";

        tt_buf_clear(&__ut_buf_out);
        ret = tt_sh_input(&sh, (tt_u8_t *)"ls\x87", 3);
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
        ret = tt_sh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // ls single
    {
        tt_char_t this_in[] = "ls s1\x87";
        const tt_char_t *this_out =
            "ls s1\n"
            "rw    s32       s1    s32 val under g1\n"
            "shell$ ";

        tt_buf_clear(&__ut_buf_out);
        ret = tt_sh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // ls single, empty group
    {
        tt_char_t this_in[] = "ls /g1/g11\x87";
        const tt_char_t *this_out =
            "ls /g1/g11\n"
            "\n"
            "shell$ ";

        tt_buf_clear(&__ut_buf_out);
        ret = tt_sh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
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
            "rw    s32       s1    s32 val under g1\n"
            "\n"
            "/g1/:\n"
            "--    dir       g11/    group 1, testing\n"
            "--    dir       g12/    group 1, testing\n"
            "\n"
            "/u1:\n"
            "r-    u32       u1    u32 val under g1\n"
            "\n"
            "g1/g12/g121:\n"
            "rw    u32       c1211    leaf child\n"
            "--    exe       exec     \n"
            "\n"
            "g1/g11/:\n"
            "\n"
            "shell$ ";

        tt_buf_clear(&__ut_buf_out);
        ret = tt_sh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
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
            "rw    s32       s1    s32 val under g1\n"
            "\n"
            "/root/g1/:\n"
            "can not find: /root/g1/\n"
            "\n"
            "/root/g1/u1:\n"
            "can not find: /root/g1/u1\n"
            "shell$ ";

        tt_buf_clear(&__ut_buf_out);
        ret = tt_sh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    tt_sh_destroy(&sh);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_cfgsh_help)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_shell_t sh;
    tt_result_t ret;
    tt_sh_attr_t attr;
    tt_cli_itf_t itf;
    tt_u32_t cmp_ret;

    TT_TEST_CASE_ENTER()
    // test start

    itf.param = NULL;
    itf.send = __ut_cfgsh_send;

    tt_sh_attr_default(&attr);
    attr.cli_attr.title = "shell";
    attr.cli_attr.seperator = '$';

    ret = tt_sh_create(&sh, root, TT_CLI_MODE_DEFAUTL, &itf, &attr);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_buf_clear(&__ut_buf_out);
    ret = tt_sh_start(&sh);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, "\nshell$ ");
    TT_UT_EQUAL(cmp_ret, 0, "");

    // list all commands
    {
        tt_char_t this_in[] = "  help  \x87";
        const tt_char_t *this_out = "";

        tt_buf_clear(&__ut_buf_out);
        ret = tt_sh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        // TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // unknown cmd
    {
        tt_char_t this_in[] = "  help unknown-cmd  \x87";
        const tt_char_t *this_out =
            "  help unknown-cmd  \n"
            "not found: unknown-cmd\n"
            "shell$ ";

        tt_buf_clear(&__ut_buf_out);
        ret = tt_sh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
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
        ret = tt_sh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
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
        ret = tt_sh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
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
        ret = tt_sh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // show mul
    {
        tt_char_t this_in[] = "  help g1 ls\x87";
        const tt_char_t *this_out =
            "  help g1 ls\n"
            "group 1, usage\n"
            "shell$ ";

        tt_buf_clear(&__ut_buf_out);
        ret = tt_sh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    tt_sh_destroy(&sh);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_cfgsh_pwd)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_shell_t sh;
    tt_result_t ret;
    tt_sh_attr_t attr;
    tt_cli_itf_t itf;
    tt_u32_t cmp_ret;

    TT_TEST_CASE_ENTER()
    // test start

    itf.param = NULL;
    itf.send = __ut_cfgsh_send;

    tt_sh_attr_default(&attr);
    attr.cli_attr.title = "shell";
    attr.cli_attr.seperator = '$';

    ret = tt_sh_create(&sh, root, TT_CLI_MODE_DEFAUTL, &itf, &attr);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_buf_clear(&__ut_buf_out);
    ret = tt_sh_start(&sh);
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
        ret = tt_sh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
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
        ret = tt_sh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
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
        ret = tt_sh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    tt_sh_destroy(&sh);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_cfgsh_cd)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_shell_t sh;
    tt_result_t ret;
    tt_sh_attr_t attr;
    tt_cli_itf_t itf;
    tt_u32_t cmp_ret;

    TT_TEST_CASE_ENTER()
    // test start

    itf.param = NULL;
    itf.send = __ut_cfgsh_send;

    tt_sh_attr_default(&attr);
    attr.cli_attr.title = "shell";
    attr.cli_attr.seperator = '$';

    ret = tt_sh_create(&sh, root, TT_CLI_MODE_DEFAUTL, &itf, &attr);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_buf_clear(&__ut_buf_out);
    ret = tt_sh_start(&sh);
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
        ret = tt_sh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
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
        ret = tt_sh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
        TT_UT_EQUAL(sh.current, g1, "");
    }

    // ls
    {
        tt_char_t this_in[] = "ls\x87";
        const tt_char_t *this_out =
            "ls\n"
            "--    dir       g11/    group 1, testing\n"
            "--    dir       g12/    group 1, testing\n"
            "shell:g1$ ";

        tt_buf_clear(&__ut_buf_out);
        ret = tt_sh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
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
        ret = tt_sh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
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
        ret = tt_sh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // cd invalid path
    {
        tt_char_t this_in[] = "  cd ../g1/g1111 \x87";
        const tt_char_t this_out[] =
            "  cd ../g1/g1111 \n"
            "not found: ../g1/g1111\n"
            "shell:g11$ ";

        tt_buf_clear(&__ut_buf_out);
        ret = tt_sh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
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
            "not a directory: /g1/g12/g121/c1211\n"
            "shell:g11$ ";

        tt_buf_clear(&__ut_buf_out);
        ret = tt_sh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // exit
    {
        tt_char_t this_in[] = "\x85";
        const tt_char_t *this_out = "^D\nexiting...\n";

        tt_buf_clear(&__ut_buf_out);
        ret = tt_sh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_E_END, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    tt_sh_destroy(&sh);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_cfgsh_get)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_shell_t sh;
    tt_result_t ret;
    tt_sh_attr_t attr;
    tt_cli_itf_t itf;
    tt_u32_t cmp_ret;

    TT_TEST_CASE_ENTER()
    // test start

    itf.param = NULL;
    itf.send = __ut_cfgsh_send;

    tt_sh_attr_default(&attr);
    attr.cli_attr.title = "shell";
    attr.cli_attr.seperator = '$';

    ret = tt_sh_create(&sh, root, TT_CLI_MODE_DEFAUTL, &itf, &attr);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_buf_clear(&__ut_buf_out);
    ret = tt_sh_start(&sh);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, "\nshell$ ");
    TT_UT_EQUAL(cmp_ret, 0, "");

    // get single
    {
        tt_char_t this_in[] = "  show /g1/g12/g121/c1211\x87";
        const tt_char_t *this_out =
            "  show /g1/g12/g121/c1211\n"
            "c1211: 12300\n"
            "shell$ ";

        __u32_val = 12300;
        tt_buf_clear(&__ut_buf_out);
        ret = tt_sh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // get single, invalid path
    {
        tt_char_t this_in[] = "  show /g1/g12/g121/c12111\x87";
        const tt_char_t *this_out =
            "  show /g1/g12/g121/c12111\n"
            "not found: /g1/g12/g121/c12111\n"
            "shell$ ";

        __u32_val = 12300;
        tt_buf_clear(&__ut_buf_out);
        ret = tt_sh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // get a group
    {
        tt_char_t this_in[] = "  show g1/g12\x87";
        const tt_char_t *this_out =
            "  show g1/g12\n"
            "show: g1/g12: is a direcoty\n"
            "shell$ ";

        __u32_val = 12300;
        tt_buf_clear(&__ut_buf_out);
        ret = tt_sh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // get multiple
    {
        tt_char_t this_in[] = "  show s1 /u1\x87";
        const tt_char_t *this_out =
            "  show s1 /u1\n"
            "s1:\n"
            "s1: 0\n"
            "\n"
            "/u1:\n"
            "u1: 12300\n"
            "shell$ ";

        __u32_val = 12300;
        tt_buf_clear(&__ut_buf_out);
        ret = tt_sh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // get multiple, with some invalid path, some group
    {
        tt_char_t this_in[] = "  show s1 s1234567890 g1 /u1\x87";
        const tt_char_t *this_out =
            "  show s1 s1234567890 g1 /u1\n"
            "s1:\n"
            "s1: 0\n"
            "\n"
            "s1234567890:\n"
            "not found: s1234567890\n"
            "\n"
            "g1:\n"
            "show: g1: is a direcoty\n"
            "\n"
            "/u1:\n"
            "u1: 12300\n"
            "shell$ ";

        __u32_val = 12300;
        tt_buf_clear(&__ut_buf_out);
        ret = tt_sh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // get current
    {
        tt_char_t this_in[] = "  show \x87";
        const tt_char_t *this_out =
            "  show \n"
            "usage: show [name]\n"
            "shell$ ";

        __u32_val = 12300;
        tt_buf_clear(&__ut_buf_out);
        ret = tt_sh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    tt_sh_destroy(&sh);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_cfgsh_set)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_shell_t sh;
    tt_result_t ret;
    tt_sh_attr_t attr;
    tt_cli_itf_t itf;
    tt_u32_t cmp_ret;

    TT_TEST_CASE_ENTER()
    // test start

    itf.param = NULL;
    itf.send = __ut_cfgsh_send;

    tt_sh_attr_default(&attr);
    attr.cli_attr.title = "shell";
    attr.cli_attr.seperator = '$';

    ret = tt_sh_create(&sh, root, TT_CLI_MODE_DEFAUTL, &itf, &attr);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_buf_clear(&__ut_buf_out);
    ret = tt_sh_start(&sh);
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
        ret = tt_sh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // set a value, invalid command
    {
        tt_char_t this_in[] = "  set s1\x87";
        const tt_char_t *this_out =
            "  set s1\n"
            "usage: set [name] [value]\n"
            "shell$ ";

        __u32_val = 12300;
        tt_buf_clear(&__ut_buf_out);
        ret = tt_sh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
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
        ret = tt_sh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // set a invalid value
    {
        tt_char_t this_in[] = "  set s123 999\x87";
        const tt_char_t *this_out =
            "  set s123 999\n"
            "not found: s123\n"
            "shell$ ";

        __u32_val = 12300;
        tt_buf_clear(&__ut_buf_out);
        ret = tt_sh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // set a group
    {
        tt_char_t this_in[] = "  set g1 999\x87";
        const tt_char_t *this_out =
            "  set g1 999\n"
            "set: g1: is a direcoty\n"
            "shell$ ";

        __u32_val = 12300;
        tt_buf_clear(&__ut_buf_out);
        ret = tt_sh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    tt_sh_destroy(&sh);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_cfgsh_quit)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_shell_t sh;
    tt_result_t ret;
    tt_sh_attr_t attr;
    tt_cli_itf_t itf;
    tt_u32_t cmp_ret;

    TT_TEST_CASE_ENTER()
    // test start

    itf.param = NULL;
    itf.send = __ut_cfgsh_send;

    tt_sh_attr_default(&attr);
    attr.cli_attr.title = "shell";
    attr.cli_attr.seperator = '$';
    attr.exit_msg = "goodbye";

    ret = tt_sh_create(&sh, root, TT_CLI_MODE_DEFAUTL, &itf, &attr);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_buf_clear(&__ut_buf_out);
    ret = tt_sh_start(&sh);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, "\nshell$ ");
    TT_UT_EQUAL(cmp_ret, 0, "");

    {
        tt_char_t this_in[] = "  qui\x87";
        const tt_char_t *this_out =
            "  qui\n"
            "not found: qui\n"
            "shell$ ";

        tt_buf_clear(&__ut_buf_out);
        ret = tt_sh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    {
        tt_char_t this_in[] = "  quit \x87";
        const tt_char_t *this_out =
            "  quit \n"
            "goodbye\n";

        tt_buf_clear(&__ut_buf_out);
        ret = tt_sh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_E_END, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    tt_sh_destroy(&sh);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_cfgsh_exec)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_shell_t sh;
    tt_result_t ret;
    tt_sh_attr_t attr;
    tt_cli_itf_t itf;
    tt_u32_t cmp_ret;

    TT_TEST_CASE_ENTER()
    // test start

    itf.param = NULL;
    itf.send = __ut_cfgsh_send;

    tt_sh_attr_default(&attr);
    attr.cli_attr.title = "shell";
    attr.cli_attr.seperator = '$';
    attr.exit_msg = "goodbye";

    ret = tt_sh_create(&sh, root, TT_CLI_MODE_DEFAUTL, &itf, &attr);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_buf_clear(&__ut_buf_out);
    ret = tt_sh_start(&sh);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, "\nshell$ ");
    TT_UT_EQUAL(cmp_ret, 0, "");

    // not found
    {
        tt_char_t this_in[] = "  /g1/g12/g121/exec1 \x87";
        const tt_char_t *this_out =
            "  /g1/g12/g121/exec1 \n"
            "not found: /g1/g12/g121/exec1\n"
            "shell$ ";

        tt_buf_clear(&__ut_buf_out);
        ret = tt_sh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // not executalbe
    {
        tt_char_t this_in[] = "  g1/g12/g121/c1211 \x87";
        const tt_char_t *this_out =
            "  g1/g12/g121/c1211 \n"
            "not executable: g1/g12/g121/c1211\n"
            "shell$ ";

        tt_buf_clear(&__ut_buf_out);
        ret = tt_sh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // exec, no out
    {
        tt_char_t this_in[] = "  g1/g12/g121/exec \x87";
        const tt_char_t *this_out =
            "  g1/g12/g121/exec \n"
            "shell$ ";

        exec_mode = 0;
        tt_buf_clear(&__ut_buf_out);
        ret = tt_sh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // exec, failed
    {
        tt_char_t this_in[] = "  g1/g12/g121/exec \x87";
        const tt_char_t *this_out =
            "  g1/g12/g121/exec \n"
            "g1/g12/g121/exec: failed\n"
            "shell$ ";

        exec_mode = 1;
        tt_buf_clear(&__ut_buf_out);
        ret = tt_sh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // exec, no arg
    {
        tt_char_t this_in[] = "  g1/g12/g121/exec \x87";
        const tt_char_t *this_out =
            "  g1/g12/g121/exec \n"
            "no arg\n"
            "shell$ ";

        exec_mode = 2;
        tt_buf_clear(&__ut_buf_out);
        ret = tt_sh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // exec, 1 arg
    {
        tt_char_t this_in[] = "  g1/g12/g121/exec --p1=2 \x87";
        const tt_char_t *this_out =
            "  g1/g12/g121/exec --p1=2 \n"
            "arg[0]: --p1=2, final: --p1=2\n"
            "shell$ ";

        exec_mode = 2;
        tt_buf_clear(&__ut_buf_out);
        ret = tt_sh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // exec, mul arg
    {
        tt_char_t this_in[] =
            "  g1/g12/g121/exec 0 1 2 3 4 5 6 7 8 9 0 a b c d e f \x87";
        const tt_char_t *this_out =
            "  g1/g12/g121/exec 0 1 2 3 4 5 6 7 8 9 0 a b c d e f \n"
            "arg[0]: 0, final: f\n"
            "shell$ ";

        exec_mode = 2;
        tt_buf_clear(&__ut_buf_out);
        ret = tt_sh_input(&sh, (tt_u8_t *)this_in, sizeof(this_in) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        cmp_ret = tt_buf_cmp_cstr(&__ut_buf_out, this_out);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    tt_sh_destroy(&sh);

    // test end
    TT_TEST_CASE_LEAVE()
}
