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

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

// === routine declarations ================
TT_TEST_ROUTINE_DECLARE(case_cli)
TT_TEST_ROUTINE_DECLARE(case_cli_stress)
TT_TEST_ROUTINE_DECLARE(case_cli_ac)
TT_TEST_ROUTINE_DECLARE(case_cli_readline)

// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(cli_basic_case)

TT_TEST_CASE("case_cli", "cli test", case_cli, NULL, NULL, NULL, NULL, NULL)
,

    TT_TEST_CASE("case_cli_stress",
                 "cli stress test ",
                 case_cli_stress,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_cli_ac",
                 "cli auto complete",
                 case_cli_ac,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_cli_readline",
                 "cli read line",
                 case_cli_readline,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(cli_basic_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(CLI_UT_BASIC, 0, cli_basic_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(case_cli_readline)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    typedef struct
{
    tt_u8_t in[100];
    tt_u32_t in_num;
    tt_u8_t out[100];
    tt_u32_t out_num;
    const tt_char_t *cmd;
} __cli_case_t;

static tt_u32_t __ut_cli_idx;
static tt_result_t __ut_cli_ret;
static tt_u32_t __ut_cli_err;
static tt_char_t __ut_cli_cmd[100];

tt_u32_t __ut_cli_on_cmd(IN struct tt_cli_s *cli,
                         IN void *param,
                         IN const tt_char_t *cmd,
                         IN tt_buf_t *output)
{
    tt_u32_t n;

    if (param != &__ut_cli_idx) {
        __ut_cli_ret = TT_FAIL;
        __ut_cli_err = __LINE__;
        return TT_CLIOC_NOOUT;
    }

    if (cmd != NULL) {
        n = (tt_u32_t)tt_strlen(cmd);
        // stress test may exceed
        if (n >= 99) {
            n = 99;
        }
        tt_buf_put(output, (tt_u8_t *)cmd, n);
        tt_memcpy(__ut_cli_cmd, cmd, n);
        __ut_cli_cmd[n] = 0;

        if (tt_strcmp(cmd, "exit") == 0) {
            return TT_CLIOC_END;
        } else {
            return TT_CLIOC_OUT;
        }
    }
    return TT_CLIOC_NOOUT;
}

static tt_buf_t __ut_cli_obuf;

tt_result_t __ut_cli_send(IN struct tt_cli_s *cli,
                          IN void *param,
                          IN tt_u8_t *ev,
                          IN tt_u32_t ev_num)
{
    if (param != &__ut_cli_obuf) {
        __ut_cli_ret = TT_FAIL;
        __ut_cli_err = __LINE__;
        return TT_SUCCESS;
    }

    if (__ut_cli_idx == ~0) {
        const tt_char_t *c = "title:sub_title$ ";
        if (ev_num != tt_strlen(c) + 1) {
            __ut_cli_ret = TT_FAIL;
            __ut_cli_err = __LINE__;
            return TT_SUCCESS;
        }
        if (ev[0] != TT_CLI_EV_ENTER) {
            __ut_cli_ret = TT_FAIL;
            __ut_cli_err = __LINE__;
            return TT_SUCCESS;
        }
        if (tt_strncmp(c, (tt_char_t *)ev + 1, ev_num - 1) != 0) {
            __ut_cli_ret = TT_FAIL;
            __ut_cli_err = __LINE__;
            return TT_SUCCESS;
        }

        return TT_SUCCESS;
    }

    tt_buf_put(&__ut_cli_obuf, ev, ev_num);

    return TT_SUCCESS;
}

static __cli_case_t cli_case[] = {
    {
        // 0
        {'a'},
        1,
        {'a'},
        1,
        NULL,
    },
    {
        // 1
        {' ', '~'},
        2,
        {' ', '~'},
        2,
        NULL,
    },
    {
        // 2
        {
            TT_CLI_EV_RIGHT, TT_CLI_EV_LEFT,
        },
        2,
        {TT_CLI_EV_LEFT},
        1,
        NULL,
    },
    {
        // 3
        {TT_CLI_EV_LEFT, TT_CLI_EV_LEFT, TT_CLI_EV_LEFT},
        3,
        {TT_CLI_EV_LEFT, TT_CLI_EV_LEFT},
        2,
        NULL,
    },
    {
        // 4
        // may change when up and down could be processed
        {
            TT_CLI_EV_UP,
        },
        1,
        {0},
        0,
        NULL,
    },
    {
        // 5
        {
            TT_CLI_EV_DOWN,
        },
        1,
        {0},
        0,
        NULL,
    },
    {
        // 6
        {
            TT_CLI_EV_ENTER,
        },
        1,
        {TT_CLI_EV_ENTER,
         'a',
         ' ',
         '~',
         TT_CLI_EV_ENTER,
         't',
         'i',
         't',
         'l',
         'e',
         ':',
         's',
         'u',
         'b',
         '_',
         't',
         'i',
         't',
         'l',
         'e',
         '$',
         ' '},
        22,
        "a ~",
    },
    {
        // 7
        {'a', 'A', 'z', 'Z', '0', '9'},
        6,
        {'a', 'A', 'z', 'Z', '0', '9'},
        6,
        NULL,
    },
    {
        // 8
        {TT_CLI_EV_DELETE},
        1,
        {TT_CLI_EV_DELETE},
        1,
        NULL,
    },
    {
        // 9: aAzZ0[] => aAz0
        {TT_CLI_EV_LEFT, TT_CLI_EV_DELETE},
        2,
        {TT_CLI_EV_LEFT,
         TT_CLI_EV_DELETE,
         '0',
         ' ',
         TT_CLI_EV_LEFT,
         TT_CLI_EV_LEFT},
        6,
        NULL,
    },
    {
        // 10
        {TT_CLI_EV_INTR},
        1,
        {'^', 'C', TT_CLI_EV_ENTER,
         't', 'i', 't',
         'l', 'e', ':',
         's', 'u', 'b',
         '_', 't', 'i',
         't', 'l', 'e',
         '$', ' '},
        20,
        (void *)1,
    },
    {
        // 11
        {TT_CLI_EV_ENTER},
        1,
        {TT_CLI_EV_ENTER,
         't',
         'i',
         't',
         'l',
         'e',
         ':',
         's',
         'u',
         'b',
         '_',
         't',
         'i',
         't',
         'l',
         'e',
         '$',
         ' '},
        18,
        (void *)1,
    },
#if 1
    {
        // 12
        {'e', 'x', 'i', 't', TT_CLI_EV_ENTER},
        5,
        {'e', 'x', 'i', 't', TT_CLI_EV_ENTER, 'e', 'x', 'i', 't'},
        9,
        NULL,
    },
#else
    {
        // 12
        {'a', 'A', 'z', 'Z', '0', '9'},
        6,
        {'a', 'A', 'z', 'Z', '0', '9'},
        6,
        NULL,
    },
    {
        // 13
        {TT_CLI_EV_QUIT},
        1,
        {'^', 'D', TT_CLI_EV_ENTER},
        3,
        (void *)1,
    },
#endif
};

TT_TEST_ROUTINE_DEFINE(case_cli)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_cli_t cli;
    tt_cli_attr_t attr;
    tt_result_t ret;
    tt_u32_t i;

    tt_cli_cb_t cb = {&__ut_cli_idx, __ut_cli_on_cmd};
    tt_cli_itf_t itf = {&__ut_cli_obuf, __ut_cli_send};

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&__ut_cli_obuf, NULL);

    tt_cli_attr_default(&attr);
    attr.title = "title";
    attr.sub_title = "sub_title";
    attr.seperator = '$';

    ret = tt_cli_create(&cli, TT_CLI_MODE_DEFAUTL, &cb, &itf, &attr);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_cli_set_cb(&cli, &cb);
    tt_cli_set_itf(&cli, &itf);

    __ut_cli_idx = ~0;
    __ut_cli_ret = TT_SUCCESS;
    __ut_cli_err = 0;

    ret = tt_cli_start(&cli);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(__ut_cli_ret, TT_SUCCESS, "");

    for (i = 0; i < sizeof(cli_case) / sizeof(cli_case[0]); ++i) {
        tt_buf_t cmp_buf;
        tt_s32_t cmp_ret;

        tt_buf_clear(&__ut_cli_obuf);
        __ut_cli_cmd[0] = 0;

        __ut_cli_idx = i;
        ret = tt_cli_input(&cli, cli_case[i].in, cli_case[i].in_num);
        if (i == sizeof(cli_case) / sizeof(cli_case[0]) - 1) {
            TT_UT_EQUAL(ret, TT_E_END, "");
        } else {
            TT_UT_EQUAL(ret, TT_SUCCESS, "");
        }

        if (cli_case[i].out_num == 0) {
            TT_UT_EQUAL(TT_BUF_RLEN(&__ut_cli_obuf), 0, "");
        } else {
            ret = tt_buf_create_nocopy(&cmp_buf,
                                       cli_case[i].out,
                                       cli_case[i].out_num,
                                       NULL);
            TT_UT_EQUAL(ret, TT_SUCCESS, "");
            cmp_ret = tt_buf_cmp(&cmp_buf, &__ut_cli_obuf);
            TT_UT_EQUAL(cmp_ret, 0, "");
            tt_buf_destroy(&cmp_buf);
        }

        if (cli_case[i].cmd == (void *)1) {
            // make sure no cb
            TT_UT_EQUAL(__ut_cli_cmd[0], 0, "");
        } else if (cli_case[i].cmd != NULL) {
            cmp_ret = tt_strcmp(__ut_cli_cmd, cli_case[i].cmd);
            TT_UT_EQUAL(cmp_ret, 0, "");
        }
    }

    tt_cli_destroy(&cli);

    // test end
    TT_TEST_CASE_LEAVE()
}

#define __cli_st_num 100

TT_TEST_ROUTINE_DEFINE(case_cli_stress)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_cli_t cli;
    tt_result_t ret;
    tt_u32_t i, n, k;
    tt_u8_t evbuf[100];
    tt_cli_cb_t cb = {&__ut_cli_idx, __ut_cli_on_cmd};
    tt_cli_itf_t itf = {&__ut_cli_obuf, __ut_cli_send};

    TT_TEST_CASE_ENTER()
    // test start

    ret = tt_cli_create(&cli, TT_CLI_MODE_DEFAUTL, &cb, &itf, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    for (k = 0; k < __cli_st_num; ++k) {
        i = tt_rand_u32() % 10;
        n = tt_rand_u32() % sizeof(evbuf);
        if (i < 4) {
            // input chars
            for (i = 0; i < n; ++i) {
                evbuf[i] = (tt_u8_t)tt_rand_u32() & 0x7f;
            }
        } else if (i < 9) {
            // input ev
            for (i = 0; i < n; ++i) {
                if (tt_rand_u32() & 1) {
                    evbuf[i] = (tt_u8_t)tt_rand_u32() & 0x7f;
                } else {
                    evbuf[i] = TT_CLI_EV_START +
                               (tt_u8_t)tt_rand_u32() % TT_CLI_EV_NUM;
                }
            }
        } else {
            for (i = 0; i < n; ++i) {
                evbuf[i] = (tt_u8_t)tt_rand_u32();
            }
        }

        tt_cli_input(&cli, evbuf, n);
        // tt_string_print(&cl.line, 0);
    }

    tt_cli_destroy(&cli);

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_u32_t __ut_cli_on_ac(IN struct tt_cli_s *cli,
                               IN void *param,
                               IN tt_u8_t *cur,
                               IN tt_u32_t cur_len,
                               IN tt_bool_t wait4cmd,
                               IN tt_buf_t *output)
{
    // available cmd: 1 12 123 1234
    // available data: a ab abcdf abcde
    // input: "   1235   abcd1   abce"

    const tt_char_t *cmd[] = {
        "1", "12", "1234",
    };
    const tt_char_t *data[] = {
        "a", "ab", "abcdf", "abcde",
    };
    tt_u32_t status;
    tt_result_t result;

    result = tt_cli_complete(cur,
                             cur_len,
                             TT_COND(wait4cmd, cmd, data),
                             TT_COND(wait4cmd,
                                     sizeof(cmd) / sizeof(cmd[0]),
                                     sizeof(data) / sizeof(data[0])),
                             &status,
                             output);
    if (TT_OK(result)) {
        return status;
    } else {
        __ut_cli_ret = TT_FAIL;
        __ut_cli_err = __LINE__;
        return TT_CLICP_NONE;
    }
}

static tt_string_t __ac_string;
static tt_string_t __ac_string1;
static tt_string_t __ac_string2;
static tt_u32_t __ac_cursor;
#define __pref_len (sizeof("title:sub_title$ ") - 1)
static tt_u32_t __ac_idx;

tt_result_t __ut_cli_send_ac(IN struct tt_cli_s *cli,
                             IN void *param,
                             IN tt_u8_t *ev,
                             IN tt_u32_t ev_num)
{
    // simulate a terminal
    tt_u32_t i;
    for (i = 0; i < ev_num; ++i) {
        tt_u8_t e = ev[i];
        if (e <= 0x7f) {
            TT_ASSERT(__ac_cursor <= tt_string_len(&__ac_string));
            if (__ac_cursor < tt_string_len(&__ac_string)) {
                tt_string_setfrom_c(&__ac_string, __ac_cursor, e);
            } else {
                tt_string_append_c(&__ac_string, e);
            }
            ++__ac_cursor;
        } else if (e == TT_CLI_EV_LEFT) {
            TT_ASSERT(__ac_cursor > __pref_len);
            --__ac_cursor;
        } else if (e == TT_CLI_EV_RIGHT) {
            TT_ASSERT((__ac_cursor - __pref_len) <=
                      tt_string_len(&__ac_string));
            ++__ac_cursor;
        } else if (e == TT_CLI_EV_ENTER) {
            tt_string_copy(&__ac_string2, &__ac_string1);
            tt_string_copy(&__ac_string1, &__ac_string);

            tt_string_clear(&__ac_string);
            __ac_cursor = 0;
            ++__ac_idx;
        } else if (e == TT_CLI_EV_DELETE) {
            TT_ASSERT(__ac_cursor > 0);
            tt_string_remove_range(&__ac_string, __ac_cursor, 1);
            --__ac_cursor;
        }
    }
    return TT_SUCCESS;
}

#if 1
#define __SHOW_AC_STR()                                                        \
    do {                                                                       \
        tt_string_print(&__ac_string2, 0);                                     \
        tt_string_print(&__ac_string1, 0);                                     \
        tt_string_print(&__ac_string, 0);                                      \
        TT_INFO("------");                                                     \
    } while (0)
#else
#define __SHOW_AC_STR()
#endif

TT_TEST_ROUTINE_DEFINE(case_cli_ac)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_cli_t cli;
    tt_cli_attr_t attr;
    tt_result_t ret;
    tt_s32_t cmp_ret;

    tt_cli_cb_t cb = {&__ut_cli_idx, __ut_cli_on_cmd, __ut_cli_on_ac};
    tt_cli_itf_t itf = {&__ut_cli_obuf, __ut_cli_send_ac};

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&__ut_cli_obuf, NULL);
    tt_string_init(&__ac_string, NULL);
    tt_string_init(&__ac_string1, NULL);
    tt_string_init(&__ac_string2, NULL);

    tt_cli_attr_default(&attr);
    attr.title = "title";
    attr.sub_title = "sub_title";
    attr.seperator = '$';

    ret = tt_cli_create(&cli, TT_CLI_MODE_DEFAUTL, &cb, &itf, &attr);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    __ut_cli_idx = ~0;
    __ut_cli_ret = TT_SUCCESS;
    __ut_cli_err = 0;

    ret = tt_cli_start(&cli);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(__ut_cli_ret, TT_SUCCESS, "");
    __SHOW_AC_STR();
    cmp_ret = tt_string_cmp(&__ac_string, "title:sub_title$ ");
    TT_UT_EQUAL(cmp_ret, 0, "");

    {
        const tt_char_t *a = "   1235   abcd1   abce";
        tt_cli_input(&cli, (tt_u8_t *)a, (tt_u32_t)tt_strlen(a));
        __SHOW_AC_STR();
        TT_UT_EQUAL(__ut_cli_ret, TT_SUCCESS, "");

        cmp_ret = tt_string_cmp(&__ac_string,
                                "title:sub_title$    1235   abcd1   abce");
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    {
        // "   1235   abcd1   abce[]"
        tt_cli_input_ev(&cli, TT_CLI_EV_TAB);
        __SHOW_AC_STR();
        TT_UT_EQUAL(__ut_cli_ret, TT_SUCCESS, "");

        // none match
        cmp_ret = tt_string_cmp(&__ac_string,
                                "title:sub_title$    1235   abcd1   abce");
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    {
        tt_cli_input_ev(&cli, TT_CLI_EV_LEFT);
        // "   1235   abcd1   abc[e]"
        tt_cli_input_ev(&cli, TT_CLI_EV_TAB);
        __SHOW_AC_STR();
        TT_UT_EQUAL(__ut_cli_ret, TT_SUCCESS, "");

        // abc has two candi: abcde, abcdf, so complete 1 char 'd'
        cmp_ret = tt_string_cmp(&__ac_string,
                                "title:sub_title$    1235   abcd1   abcde");
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    {
        tt_cli_input_ev(&cli, TT_CLI_EV_LEFT);
        // "   1235   abcd1   abc[d]e"
        tt_cli_input_ev(&cli, TT_CLI_EV_TAB);
        __SHOW_AC_STR();
        TT_UT_EQUAL(__ut_cli_ret, TT_SUCCESS, "");

        // abc has two candi: abcde, abcdf, so complete 1 char 'd'
        cmp_ret = tt_string_cmp(&__ac_string,
                                "title:sub_title$    1235   abcd1   abcdde");
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    {
        tt_cli_input_ev(&cli, TT_CLI_EV_LEFT);
        tt_cli_input_ev(&cli, TT_CLI_EV_LEFT);
        // "   1235   abcd1   ab[c]dde"
        tt_cli_input_ev(&cli, TT_CLI_EV_TAB);
        __SHOW_AC_STR();
        TT_UT_EQUAL(__ut_cli_ret, TT_SUCCESS, "");

        cmp_ret = tt_string_cmp(&__ac_string2,
                                "title:sub_title$    1235   abcd1   abcdde");
        TT_UT_EQUAL(cmp_ret, 0, "");
        cmp_ret = tt_string_cmp(&__ac_string1, "ab abcdf abcde");
        TT_UT_EQUAL(cmp_ret, 0, "");
        cmp_ret = tt_string_cmp(&__ac_string,
                                "title:sub_title$    1235   abcd1   abcdde");
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    {
        tt_cli_input_ev(&cli, TT_CLI_EV_LEFT);
        // "   1235   abcd1   a[b]cdde"
        tt_cli_input_ev(&cli, TT_CLI_EV_TAB);
        __SHOW_AC_STR();
        TT_UT_EQUAL(__ut_cli_ret, TT_SUCCESS, "");

        cmp_ret = tt_string_cmp(&__ac_string2,
                                "title:sub_title$    1235   abcd1   abcdde");
        TT_UT_EQUAL(cmp_ret, 0, "");
        cmp_ret = tt_string_cmp(&__ac_string1, "a ab abcdf abcde");
        TT_UT_EQUAL(cmp_ret, 0, "");
        cmp_ret = tt_string_cmp(&__ac_string,
                                "title:sub_title$    1235   abcd1   abcdde");
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    {
        tt_cli_input_ev(&cli, TT_CLI_EV_LEFT);
        // "   1235   abcd1   [a]bcdde"
        tt_cli_input_ev(&cli, TT_CLI_EV_TAB);
        __SHOW_AC_STR();
        TT_UT_EQUAL(__ut_cli_ret, TT_SUCCESS, "");

        cmp_ret = tt_string_cmp(&__ac_string2,
                                "title:sub_title$    1235   abcd1   abcdde");
        TT_UT_EQUAL(cmp_ret, 0, "");
        cmp_ret = tt_string_cmp(&__ac_string1, "a ab abcdf abcde");
        TT_UT_EQUAL(cmp_ret, 0, "");
        cmp_ret = tt_string_cmp(&__ac_string,
                                "title:sub_title$    1235   abcd1   abcdde");
    }

    /////////// 3 spaces
    {
        tt_cli_input_ev(&cli, TT_CLI_EV_LEFT);
        // "   1235   abcd1  [ ]abcdde"
        tt_cli_input_ev(&cli, TT_CLI_EV_TAB);
        __SHOW_AC_STR();
        TT_UT_EQUAL(__ut_cli_ret, TT_SUCCESS, "");

        cmp_ret = tt_string_cmp(&__ac_string2,
                                "title:sub_title$    1235   abcd1   abcdde");
        TT_UT_EQUAL(cmp_ret, 0, "");
        cmp_ret = tt_string_cmp(&__ac_string1, "a ab abcdf abcde");
        TT_UT_EQUAL(cmp_ret, 0, "");
        cmp_ret = tt_string_cmp(&__ac_string,
                                "title:sub_title$    1235   abcd1   abcdde");
    }
    {
        tt_cli_input_ev(&cli, TT_CLI_EV_LEFT);
        // "   1235   abcd1 [ ] abcdde"
        tt_cli_input_ev(&cli, TT_CLI_EV_TAB);
        __SHOW_AC_STR();
        TT_UT_EQUAL(__ut_cli_ret, TT_SUCCESS, "");

        cmp_ret = tt_string_cmp(&__ac_string2,
                                "title:sub_title$    1235   abcd1   abcdde");
        TT_UT_EQUAL(cmp_ret, 0, "");
        cmp_ret = tt_string_cmp(&__ac_string1, "a ab abcdf abcde");
        TT_UT_EQUAL(cmp_ret, 0, "");
        cmp_ret = tt_string_cmp(&__ac_string,
                                "title:sub_title$    1235   abcd1   abcdde");
    }
    {
        tt_u32_t org_idx = __ac_idx;

        tt_cli_input_ev(&cli, TT_CLI_EV_LEFT);
        // "   1235   abcd1[ ]  abce"
        tt_cli_input_ev(&cli, TT_CLI_EV_TAB);
        __SHOW_AC_STR();
        TT_UT_EQUAL(__ut_cli_ret, TT_SUCCESS, "");

        cmp_ret = tt_string_cmp(&__ac_string,
                                "title:sub_title$    1235   abcd1   abcdde");
        TT_UT_EQUAL(cmp_ret, 0, "");
        TT_UT_EQUAL(org_idx, __ac_idx, ""); // no new line
    }

    {
        tt_u32_t org_idx = __ac_idx;

        tt_cli_input_ev(&cli, TT_CLI_EV_LEFT);
        // "   1235   abcd[1]   abce"
        tt_cli_input_ev(&cli, TT_CLI_EV_TAB);
        __SHOW_AC_STR();
        TT_UT_EQUAL(__ut_cli_ret, TT_SUCCESS, "");

        cmp_ret = tt_string_cmp(&__ac_string2,
                                "title:sub_title$    1235   abcd1   abcdde");
        TT_UT_EQUAL(cmp_ret, 0, "");
        cmp_ret = tt_string_cmp(&__ac_string1, "abcdf abcde");
        TT_UT_EQUAL(cmp_ret, 0, "");
        cmp_ret = tt_string_cmp(&__ac_string,
                                "title:sub_title$    1235   abcd1   abcdde");
        TT_UT_EQUAL(org_idx + 2, __ac_idx, ""); // 2 new line
    }

    {
        tt_cli_input_ev(&cli, TT_CLI_EV_LEFT);
        // "   1235   abc[d]1    abcdde"
        tt_cli_input_ev(&cli, TT_CLI_EV_LEFT);
        // "   1235   ab[c]d1    abcdde"
        tt_cli_input_ev(&cli, TT_CLI_EV_LEFT);
        tt_cli_input_ev(&cli, TT_CLI_EV_LEFT);
        // "   1235   [a]bcd1    abcdde"
        tt_cli_input_ev(&cli, TT_CLI_EV_TAB);
        __SHOW_AC_STR();
        TT_UT_EQUAL(__ut_cli_ret, TT_SUCCESS, "");

        // show all data
        cmp_ret = tt_string_cmp(&__ac_string2,
                                "title:sub_title$    1235   abcd1   abcdde");
        TT_UT_EQUAL(cmp_ret, 0, "");
        cmp_ret = tt_string_cmp(&__ac_string1, "a ab abcdf abcde");
        TT_UT_EQUAL(cmp_ret, 0, "");
        cmp_ret = tt_string_cmp(&__ac_string,
                                "title:sub_title$    1235   abcd1   abcdde");
    }

    // 3 spaces
    {
        tt_cli_input_ev(&cli, TT_CLI_EV_LEFT);
        tt_cli_input_ev(&cli, TT_CLI_EV_LEFT);
        // "   1235 [ ] abcd1    abcdde"

        tt_cli_input_ev(&cli, TT_CLI_EV_TAB);
        __SHOW_AC_STR();
        TT_UT_EQUAL(__ut_cli_ret, TT_SUCCESS, "");

        // show all data
        cmp_ret = tt_string_cmp(&__ac_string2,
                                "title:sub_title$    1235   abcd1   abcdde");
        TT_UT_EQUAL(cmp_ret, 0, "");
        cmp_ret = tt_string_cmp(&__ac_string1, "a ab abcdf abcde");
        TT_UT_EQUAL(cmp_ret, 0, "");
        cmp_ret = tt_string_cmp(&__ac_string,
                                "title:sub_title$    1235   abcd1   abcdde");
    }

    // command
    {
        tt_u32_t org_idx = __ac_idx;

        tt_cli_input_ev(&cli, TT_CLI_EV_LEFT);
        // "   1235[ ]  abcd1    abcdde"

        tt_cli_input_ev(&cli, TT_CLI_EV_TAB);
        __SHOW_AC_STR();
        TT_UT_EQUAL(__ut_cli_ret, TT_SUCCESS, "");

        // none match
        cmp_ret = tt_string_cmp(&__ac_string,
                                "title:sub_title$    1235   abcd1   abcdde");
        TT_UT_EQUAL(cmp_ret, 0, "");
        TT_UT_EQUAL(org_idx, __ac_idx, ""); // 2 new line
    }

    {
        tt_u32_t org_idx = __ac_idx;

        tt_cli_input_ev(&cli, TT_CLI_EV_LEFT);
        // "   123[5]  abcd1    abcdde"

        tt_cli_input_ev(&cli, TT_CLI_EV_TAB);
        __SHOW_AC_STR();
        TT_UT_EQUAL(__ut_cli_ret, TT_SUCCESS, "");

        // none match
        cmp_ret = tt_string_cmp(&__ac_string,
                                "title:sub_title$    1234 5   abcd1   abcdde");
        TT_UT_EQUAL(cmp_ret, 0, "");
        TT_UT_EQUAL(org_idx, __ac_idx, ""); // 2 new line
        // "   1234 [5]  abcd1    abcdde"
    }

    {
        tt_u32_t org_idx = __ac_idx;

        tt_cli_input_ev(&cli, TT_CLI_EV_LEFT);
        tt_cli_input_ev(&cli, TT_CLI_EV_LEFT);
        tt_cli_input_ev(&cli, TT_CLI_EV_LEFT);
        // "   12[3]4 5  abcd1    abcdde"

        tt_cli_input_ev(&cli, TT_CLI_EV_TAB);
        __SHOW_AC_STR();
        TT_UT_EQUAL(__ut_cli_ret, TT_SUCCESS, "");

        // none match
        cmp_ret = tt_string_cmp(&__ac_string2,
                                "title:sub_title$    1234 5   abcd1   abcdde");
        TT_UT_EQUAL(cmp_ret, 0, "");
        cmp_ret = tt_string_cmp(&__ac_string1, "12 1234");
        TT_UT_EQUAL(cmp_ret, 0, "");
        cmp_ret = tt_string_cmp(&__ac_string,
                                "title:sub_title$    1234 5   abcd1   abcdde");
        TT_UT_EQUAL(cmp_ret, 0, "");
        TT_UT_EQUAL(org_idx + 2, __ac_idx, ""); // 2 new line
        // "   1234 [5]  abcd1    abcdde"
    }

    {
        tt_cli_input_ev(&cli, TT_CLI_EV_LEFT);
        // "   1[2]34 5  abcd1    abcdde"

        tt_cli_input_ev(&cli, TT_CLI_EV_TAB);
        __SHOW_AC_STR();
        TT_UT_EQUAL(__ut_cli_ret, TT_SUCCESS, "");

        // none match
        cmp_ret = tt_string_cmp(&__ac_string2,
                                "title:sub_title$    1234 5   abcd1   abcdde");
        TT_UT_EQUAL(cmp_ret, 0, "");
        cmp_ret = tt_string_cmp(&__ac_string1, "1 12 1234");
        TT_UT_EQUAL(cmp_ret, 0, "");
        cmp_ret = tt_string_cmp(&__ac_string,
                                "title:sub_title$    1234 5   abcd1   abcdde");
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    {
        tt_cli_input_ev(&cli, TT_CLI_EV_LEFT);
        // "   [1]234 5  abcd1    abcdde"

        tt_cli_input_ev(&cli, TT_CLI_EV_TAB);
        __SHOW_AC_STR();
        TT_UT_EQUAL(__ut_cli_ret, TT_SUCCESS, "");

        // none match
        cmp_ret = tt_string_cmp(&__ac_string2,
                                "title:sub_title$    1234 5   abcd1   abcdde");
        TT_UT_EQUAL(cmp_ret, 0, "");
        cmp_ret = tt_string_cmp(&__ac_string1, "1 12 1234");
        TT_UT_EQUAL(cmp_ret, 0, "");
        cmp_ret = tt_string_cmp(&__ac_string,
                                "title:sub_title$    1234 5   abcd1   abcdde");
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    {
        tt_cli_input_ev(&cli, TT_CLI_EV_LEFT);
        // "  [ ]1234 5  abcd1    abcdde"

        tt_cli_input_ev(&cli, TT_CLI_EV_TAB);
        __SHOW_AC_STR();
        TT_UT_EQUAL(__ut_cli_ret, TT_SUCCESS, "");

        // none match
        cmp_ret = tt_string_cmp(&__ac_string2,
                                "title:sub_title$    1234 5   abcd1   abcdde");
        TT_UT_EQUAL(cmp_ret, 0, "");
        cmp_ret = tt_string_cmp(&__ac_string1, "1 12 1234");
        TT_UT_EQUAL(cmp_ret, 0, "");
        cmp_ret = tt_string_cmp(&__ac_string,
                                "title:sub_title$    1234 5   abcd1   abcdde");
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    {
        tt_cli_input_ev(&cli, TT_CLI_EV_LEFT);
        // " [ ] 1234 5  abcd1    abcdde"

        tt_cli_input_ev(&cli, TT_CLI_EV_TAB);
        __SHOW_AC_STR();
        TT_UT_EQUAL(__ut_cli_ret, TT_SUCCESS, "");

        // none match
        cmp_ret = tt_string_cmp(&__ac_string2,
                                "title:sub_title$    1234 5   abcd1   abcdde");
        TT_UT_EQUAL(cmp_ret, 0, "");
        cmp_ret = tt_string_cmp(&__ac_string1, "1 12 1234");
        TT_UT_EQUAL(cmp_ret, 0, "");
        cmp_ret = tt_string_cmp(&__ac_string,
                                "title:sub_title$    1234 5   abcd1   abcdde");
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    {
        tt_cli_input_ev(&cli, TT_CLI_EV_LEFT);
        // "[ ]  1234 5  abcd1    abcdde"

        tt_cli_input_ev(&cli, TT_CLI_EV_TAB);
        __SHOW_AC_STR();
        TT_UT_EQUAL(__ut_cli_ret, TT_SUCCESS, "");

        // none match
        cmp_ret = tt_string_cmp(&__ac_string2,
                                "title:sub_title$    1234 5   abcd1   abcdde");
        TT_UT_EQUAL(cmp_ret, 0, "");
        cmp_ret = tt_string_cmp(&__ac_string1, "1 12 1234");
        TT_UT_EQUAL(cmp_ret, 0, "");
        cmp_ret = tt_string_cmp(&__ac_string,
                                "title:sub_title$    1234 5   abcd1   abcdde");
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    tt_cli_destroy(&cli);
    tt_string_destroy(&__ac_string);
    tt_string_destroy(&__ac_string1);
    tt_string_destroy(&__ac_string2);

    // test end
    TT_TEST_CASE_LEAVE()
}

static const tt_char_t *__ut_read_content;
static tt_u32_t __ut_read_more;
static tt_bool_t __ut_read_cb;

tt_u32_t __ut_cli_on_read(IN struct tt_cli_s *cli,
                          IN const tt_char_t *content,
                          IN tt_buf_t *output)
{
    __ut_read_cb = TT_TRUE;

    if (tt_strcmp(content, __ut_read_content) != 0) {
        __ut_cli_err = __LINE__;
        return TT_CLIOR_DONE;
    }

    tt_buf_put_cstr(output, content);
    return __ut_read_more;
}

TT_TEST_ROUTINE_DEFINE(case_cli_readline)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_cli_t cli;
    tt_result_t ret;
    tt_s32_t cmp_ret;

    tt_cli_itf_t itf = {&__ut_cli_obuf, __ut_cli_send};

    TT_TEST_CASE_ENTER()
    // test start

    __ut_cli_err = 0;
    __ut_read_more = TT_CLIOR_MORE;
    __ut_cli_idx = 0;

    tt_buf_init(&__ut_cli_obuf, NULL);

    ret = tt_cli_create(&cli, TT_CLI_MODE_DEFAUTL, NULL, &itf, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_cli_start(&cli);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_cli_read_line(&cli, __ut_cli_on_read);

    // empty
    {
        tt_u8_t ev[1] = {TT_CLI_EV_ENTER};

        tt_buf_clear(&__ut_cli_obuf);

        __ut_read_content = "";
        __ut_read_cb = TT_FALSE;
        ret = tt_cli_input(&cli, ev, 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(__ut_cli_err, 0, "");
        TT_UT_EQUAL(__ut_read_cb, TT_TRUE, "");

        cmp_ret = tt_buf_cmp_cstr(&__ut_cli_obuf, "\x87");
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // read a line
    {
        tt_u8_t ev[] = {'1', '2', '3', TT_CLI_EV_ENTER};

        tt_buf_clear(&__ut_cli_obuf);

        __ut_read_content = "123";
        __ut_read_cb = TT_FALSE;
        ret = tt_cli_input(&cli, ev, sizeof(ev));
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(__ut_cli_err, 0, "");
        TT_UT_EQUAL(__ut_read_cb, TT_TRUE, "");

        cmp_ret = tt_buf_cmp_cstr(&__ut_cli_obuf, "123\207123");
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // read an editted line
    {
        tt_u8_t ev[] = {'1',
                        '2',
                        '3',
                        TT_CLI_EV_LEFT,
                        TT_CLI_EV_DELETE,
                        TT_CLI_EV_DELETE,
                        '5',
                        '4',
                        TT_CLI_EV_RIGHT,
                        TT_CLI_EV_ENTER,
                        0};

        tt_buf_clear(&__ut_cli_obuf);

        __ut_read_content = "543";
        __ut_read_cb = TT_FALSE;
        ret = tt_cli_input(&cli, ev, sizeof(ev) - 1);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(__ut_cli_err, 0, "");
        TT_UT_EQUAL(__ut_read_cb, TT_TRUE, "");

        // cmp_ret = tt_buf_cmp_cstr(&__ut_cli_obuf, (tt_char_t*)ev);
        // TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // read multiple lines
    {
        tt_u8_t ev[] = {'1', '2', '3', TT_CLI_EV_ENTER, '5', '4', '6'};
        tt_u8_t ev2[] = {TT_CLI_EV_ENTER, '7', '8', '9'};

        tt_buf_clear(&__ut_cli_obuf);

        __ut_read_content = "123";
        __ut_read_cb = TT_FALSE;
        ret = tt_cli_input(&cli, ev, sizeof(ev));
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(__ut_cli_err, 0, "");
        TT_UT_EQUAL(__ut_read_cb, TT_TRUE, "");

        cmp_ret = tt_buf_cmp_cstr(&__ut_cli_obuf, "123\207123546");
        TT_UT_EQUAL(cmp_ret, 0, "");

        __ut_read_content = "546";
        __ut_read_cb = TT_FALSE;
        __ut_read_more = TT_CLIOR_DONE;

        tt_buf_clear(&__ut_cli_obuf);
        ret = tt_cli_input(&cli, ev2, sizeof(ev2));
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(__ut_cli_err, 0, "");
        TT_UT_EQUAL(__ut_read_cb, TT_TRUE, "");

        cmp_ret = tt_buf_cmp_cstr(&__ut_cli_obuf, "\207546titansdk$ ");
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // read and exit
    tt_cli_read_line(&cli, __ut_cli_on_read);
    {
        tt_u8_t ev[] = {'e', 'x', 'i', 't', TT_CLI_EV_ENTER};

        tt_buf_clear(&__ut_cli_obuf);

        __ut_read_content = "exit";
        __ut_read_cb = TT_FALSE;
        __ut_read_more = TT_CLIOR_END;

        ret = tt_cli_input(&cli, ev, sizeof(ev));
        TT_UT_EQUAL(ret, TT_E_END, "");
        TT_UT_EQUAL(__ut_cli_err, 0, "");
        TT_UT_EQUAL(__ut_read_cb, TT_TRUE, "");

        cmp_ret = tt_buf_cmp_cstr(&__ut_cli_obuf, "exit\207exit");
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    tt_cli_destroy(&cli);

    tt_buf_destroy(&__ut_cli_obuf);

    // test end
    TT_TEST_CASE_LEAVE()
}
