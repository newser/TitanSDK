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
TT_TEST_ROUTINE_DECLARE(case_cli_line)
TT_TEST_ROUTINE_DECLARE(case_cli_line_stress)
TT_TEST_ROUTINE_DECLARE(case_cli_line_cursor)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(cli_line_case)

TT_TEST_CASE("case_cli_line",
             "cli line editor",
             case_cli_line,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("case_cli_line_stress",
                 "cli line editor stress test",
                 case_cli_line_stress,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_cli_line_cursor",
                 "cli line editor get curosr data",
                 case_cli_line_cursor,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(cli_line_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(CLI_UT_LINE, 0, cli_line_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(case_cli_line_cursor)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    typedef struct __cline_case
{
    tt_u8_t data[100];
    tt_u32_t num;
} __cline_case;

static __cline_case __cline_input[] = {
    {{'a'}, 1}, // single char
    {{0}, 1}, // invalid char
    {{3}, 1}, // invalid char
    {{'b', 'z', 'A', 'Z', '0', '9'}, 6}, // multiple char
    {{TT_CLI_EV_UP}, 1}, // up
    /*5*/ {{TT_CLI_EV_UP, TT_CLI_EV_DOWN}, 2}, // up and down
    {{TT_CLI_EV_RIGHT}, 1}, // right
    {{TT_CLI_EV_RIGHT, TT_CLI_EV_RIGHT}, 2}, // 2right
    {{TT_CLI_EV_LEFT}, 1}, // left
    {{TT_CLI_EV_LEFT, TT_CLI_EV_LEFT}, 2}, // 2left
    /*10*/ {{TT_CLI_EV_RIGHT}, 1}, // right
    {{TT_CLI_EV_LEFT,
      TT_CLI_EV_LEFT,
      TT_CLI_EV_LEFT,
      TT_CLI_EV_LEFT,
      TT_CLI_EV_LEFT,
      TT_CLI_EV_LEFT},
     6}, // 6left
    {{TT_CLI_EV_LEFT, TT_CLI_EV_LEFT}, 2}, // 2left

    {{TT_CLI_EV_DELETE}, 1}, // delete
    {{TT_CLI_EV_RIGHT, TT_CLI_EV_DELETE}, 2}, // right delete
    /*15*/ {{TT_CLI_EV_RIGHT,
             TT_CLI_EV_RIGHT,
             TT_CLI_EV_RIGHT,
             TT_CLI_EV_RIGHT,
             TT_CLI_EV_DELETE},
            5}, // 4right delete middle

    {{TT_CLI_EV_RIGHT,  TT_CLI_EV_RIGHT,  TT_CLI_EV_RIGHT,  TT_CLI_EV_RIGHT,
      TT_CLI_EV_RIGHT,  TT_CLI_EV_RIGHT,  TT_CLI_EV_RIGHT,  TT_CLI_EV_RIGHT,
      TT_CLI_EV_RIGHT,  TT_CLI_EV_DELETE, TT_CLI_EV_DELETE, TT_CLI_EV_DELETE,
      TT_CLI_EV_DELETE, TT_CLI_EV_DELETE, TT_CLI_EV_DELETE, TT_CLI_EV_DELETE,
      TT_CLI_EV_DELETE, TT_CLI_EV_RIGHT,  TT_CLI_EV_RIGHT,  TT_CLI_EV_LEFT,
      TT_CLI_EV_LEFT},
     21},
    // move to tail delete all

    {{TT_CLI_EV_INTR}, 1}, // intr
    {{'a'}, 1}, // single char
    {{TT_CLI_EV_INTR, TT_CLI_EV_INTR}, 2}, // 2intr
    {{TT_CLI_EV_QUIT}, 1}, // quit
    {{TT_CLI_EV_QUIT, TT_CLI_EV_QUIT}, 2}, // 2quit

    {{'b', TT_CLI_EV_LEFT, TT_CLI_EV_LEFT, TT_CLI_EV_LEFT},
     4}, // new char, 4 left
    {{'c'}, 1}, // new char
};
static __cline_case __cline_line[] = {
    {{'a'}, 1}, // after single char
    {{'a'}, 1}, // after invalid char
    {{'a'}, 1}, // after invalid char
    {{'a', 'b', 'z', 'A', 'Z', '0', '9'}, 7}, // multiple char
    {{'a', 'b', 'z', 'A', 'Z', '0', '9'}, 7}, // up, no effect
    {{'a', 'b', 'z', 'A', 'Z', '0', '9'}, 7}, // up and down, no effect
    {{'a', 'b', 'z', 'A', 'Z', '0', '9'}, 7}, // right
    {{'a', 'b', 'z', 'A', 'Z', '0', '9'}, 7}, // 2right
    {{'a', 'b', 'z', 'A', 'Z', '0', '9'}, 7}, // left
    {{'a', 'b', 'z', 'A', 'Z', '0', '9'}, 7}, // 2left
    {{'a', 'b', 'z', 'A', 'Z', '0', '9'}, 7}, // right
    {{'a', 'b', 'z', 'A', 'Z', '0', '9'}, 7}, // 6left
    {{'a', 'b', 'z', 'A', 'Z', '0', '9'}, 7}, // 2left

    {{'a', 'b', 'z', 'A', 'Z', '0', '9'}, 7}, // delete
    {{'b', 'z', 'A', 'Z', '0', '9'}, 6}, // right delete
    {{'b', 'z', 'A', '0', '9'}, 5}, // 4right delete middle
    {{0}, 0}, // move to tail delete all

    {{0}, 0}, // intr
    {{'a'}, 1}, // after single char
    {{'a'}, 1}, // 2intr
    {{'a'}, 1}, // quit
    {{'a'}, 1}, // 2quit

    {{'a', 'b'}, 2}, // new char, 3 left
    {{'c', 'a', 'b'}, 3}, // new char
};
static tt_u32_t __cline_cursor[] = {
    1, // after single char
    1, // after invalid char
    1, // after invalid char
    7, // after multiple char
    7, // up, no effect
    7, // up and down, no effect
    7, // right
    7, // 2right
    6, // left
    4, // 2left
    5, // right
    0, // 6left
    0, // 2left

    0, // delete
    0, // right delete
    3, // 4right delete middle
    0, // move to tail delete all

    0, // intr
    1, // after single char
    1, // 2intr
    1, // quit
    1, // 2quit

    0, // new char, 3 left
    1, // new char
};
static __cline_case __cline_output[] = {
    {{'a'}, 1}, // single char
    {{0}, 0}, // invalid char
    {{0}, 0}, // invalid char
    {{'b', 'z', 'A', 'Z', '0', '9'}, 6}, // multiple char
    {{0}, 0}, // up, no effect
    {{0}, 0}, // up and down, no effect
    {{0}, 0}, // right
    {{0}, 0}, // 2right
    {{TT_CLI_EV_LEFT}, 1}, // left
    {{TT_CLI_EV_LEFT, TT_CLI_EV_LEFT}, 2}, // 2left
    {{TT_CLI_EV_RIGHT}, 1}, // right
    {{TT_CLI_EV_LEFT,
      TT_CLI_EV_LEFT,
      TT_CLI_EV_LEFT,
      TT_CLI_EV_LEFT,
      TT_CLI_EV_LEFT},
     5}, // 6left
    {{0}, 0}, // left

    {{0}, 0}, // delete
    {{TT_CLI_EV_RIGHT,
      TT_CLI_EV_DELETE,
      'b',
      'z',
      'A',
      'Z',
      '0',
      '9',
      ' ',
      TT_CLI_EV_LEFT,
      TT_CLI_EV_LEFT,
      TT_CLI_EV_LEFT,
      TT_CLI_EV_LEFT,
      TT_CLI_EV_LEFT,
      TT_CLI_EV_LEFT,
      TT_CLI_EV_LEFT},
     16}, // right delete
    {{TT_CLI_EV_RIGHT,
      TT_CLI_EV_RIGHT,
      TT_CLI_EV_RIGHT,
      TT_CLI_EV_RIGHT,
      TT_CLI_EV_DELETE,
      '0',
      '9',
      ' ',
      TT_CLI_EV_LEFT,
      TT_CLI_EV_LEFT,
      TT_CLI_EV_LEFT},
     11}, // 4right delete middle

    {{
         TT_CLI_EV_RIGHT,
         TT_CLI_EV_RIGHT,
         TT_CLI_EV_DELETE,
         TT_CLI_EV_DELETE,
         TT_CLI_EV_DELETE,
         TT_CLI_EV_DELETE,
         TT_CLI_EV_DELETE,
     },
     7}, // move to tail delete all

    {{'^', 'C'}, 2}, // intr
    {{'a'}, 1}, // after single char
    {{'^', 'C', '^', 'C'}, 4}, // 2intr
    {{'^', 'D'}, 2}, // quit
    {{'^', 'D', '^', 'D'}, 4}, // 2quit

    {{'b', TT_CLI_EV_LEFT, TT_CLI_EV_LEFT}, 3}, // new char, 3 left
    {{'c', 'a', 'b', TT_CLI_EV_LEFT, TT_CLI_EV_LEFT}, 5}, // new char
};

TT_TEST_ROUTINE_DEFINE(case_cli_line)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_cline_t cl;
    tt_result_t ret;
    tt_u32_t i;
    tt_buf_t ob;

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&ob, NULL);

    ret = tt_cline_create(&cl, TT_CLI_MODE_DEFAUTL, &ob, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(cl.cursor, 0, "");

    for (i = 0; i < sizeof(__cline_input) / sizeof(__cline_input[0]); ++i) {
        const tt_char_t *cstr;
        tt_u32_t len;

        tt_buf_clear(&ob);
        ret = tt_cline_input(&cl, __cline_input[i].data, __cline_input[i].num);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");

        // line
        len = tt_string_len(&cl.line);
        TT_UT_EQUAL(len, __cline_line[i].num, "");

        cstr = tt_string_cstr(&cl.line);
        TT_UT_EQUAL(tt_strncmp(cstr,
                               (tt_char_t *)__cline_line[i].data,
                               __cline_line[i].num),
                    0,
                    "");


        // cursor
        TT_UT_EQUAL(cl.cursor, __cline_cursor[i], "");

        // output
        // tt_buf_clear(&ob);
        // ret = tt_cline_output(&cl, &ob, TT_TRUE);
        // TT_UT_EQUAL(ret, TT_SUCCESS, "");
        len = TT_BUF_RLEN(&ob);
        TT_UT_EQUAL(len, __cline_output[i].num, "");
        TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(&ob),
                              __cline_output[i].data,
                              __cline_output[i].num),
                    0,
                    "");
    }

    tt_cline_destroy(&cl);
    tt_buf_destroy(&ob);

    // test end
    TT_TEST_CASE_LEAVE()
}

#define __cli_st_num 100

TT_TEST_ROUTINE_DEFINE(case_cli_line_stress)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_cline_t cl;
    tt_result_t ret;
    tt_u32_t i, n, k;
    tt_buf_t ob;
    tt_u8_t evbuf[100];

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&ob, NULL);

    ret = tt_cline_create(&cl, TT_CLI_MODE_DEFAUTL, &ob, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(cl.cursor, 0, "");

    for (k = 0; k < __cli_st_num; ++k) {
        i = tt_rand_u32() % 10;
        n = tt_rand_u32() % sizeof(evbuf);
        if (i < 4) {
            // input chars
            for (i = 0; i < n; ++i) {
                evbuf[i] = (tt_u8_t)tt_rand_u32() & 0x7f;
            }
        } else if (i < 9) {
            // input chars and ev
            for (i = 0; i < n; ++i) {
                if (tt_rand_u32() & 3) {
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

        {
            tt_blob_t bl;
            tt_cline_cursor_data(&cl, &bl);
        }

        tt_buf_clear(&ob);
        tt_cline_input(&cl, evbuf, n);
        // tt_string_print(&cl.line, 0);
    }

    tt_cline_destroy(&cl);
    tt_buf_destroy(&ob);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_cli_line_cursor)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_cline_t cl;
    tt_result_t ret;
    tt_buf_t ob;
    tt_u8_t evbuf[100];
    tt_bool_t cmd;
    tt_blob_t cc;

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&ob, NULL);

    ret = tt_cline_create(&cl, TT_CLI_MODE_DEFAUTL, &ob, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    // emtpy cline
    cmd = tt_cline_cursor_data(&cl, &cc);
    TT_UT_EQUAL(cmd, TT_TRUE, "");
    TT_UT_EQUAL(cc.addr, NULL, "");
    TT_UT_EQUAL(cc.len, 0, "");

    evbuf[0] = TT_CLI_EV_RIGHT;
    ret = tt_cline_input(&cl, evbuf, 1);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    cmd = tt_cline_cursor_data(&cl, &cc);
    TT_UT_EQUAL(cmd, TT_TRUE, "");
    TT_UT_EQUAL(cc.addr, NULL, "");
    TT_UT_EQUAL(cc.len, 0, "");

    // full string
    evbuf[0] = ' ';
    evbuf[1] = ' ';
    evbuf[2] = ' ';
    evbuf[3] = '1';
    evbuf[4] = '2';
    evbuf[5] = '3';
    evbuf[6] = ' ';
    evbuf[7] = ' ';
    evbuf[8] = ' ';
    evbuf[9] = '4';
    evbuf[10] = '5';
    evbuf[11] = '6';
    ret = tt_cline_input(&cl, evbuf, 12);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    // not cmd
    cmd = tt_cline_cursor_data(&cl, &cc);
    TT_UT_EQUAL(cmd, TT_FALSE, "");
    TT_UT_EQUAL(cc.len, 3, "");
    TT_UT_EQUAL(tt_strncmp((tt_char_t *)cc.addr, "456", 3), 0, "");

    // left
    evbuf[0] = TT_CLI_EV_LEFT;
    ret = tt_cline_input(&cl, evbuf, 1);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    cmd = tt_cline_cursor_data(&cl, &cc);
    TT_UT_EQUAL(cmd, TT_FALSE, "");
    TT_UT_EQUAL(cc.len, 2, "");
    TT_UT_EQUAL(tt_strncmp((tt_char_t *)cc.addr, "45", 2), 0, "");

    // left
    evbuf[0] = TT_CLI_EV_LEFT;
    ret = tt_cline_input(&cl, evbuf, 1);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    cmd = tt_cline_cursor_data(&cl, &cc);
    TT_UT_EQUAL(cmd, TT_FALSE, "");
    TT_UT_EQUAL(cc.len, 1, "");
    TT_UT_EQUAL(tt_strncmp((tt_char_t *)cc.addr, "4", 1), 0, "");

    //////////// 3 spaces

    // left
    evbuf[0] = TT_CLI_EV_LEFT;
    ret = tt_cline_input(&cl, evbuf, 1);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    cmd = tt_cline_cursor_data(&cl, &cc);
    TT_UT_EQUAL(cmd, TT_FALSE, "");
    TT_UT_EQUAL(cc.addr, NULL, "");
    TT_UT_EQUAL(cc.len, 0, "");

    // left
    evbuf[0] = TT_CLI_EV_LEFT;
    ret = tt_cline_input(&cl, evbuf, 1);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    cmd = tt_cline_cursor_data(&cl, &cc);
    TT_UT_EQUAL(cmd, TT_FALSE, "");
    TT_UT_EQUAL(cc.addr, NULL, "");
    TT_UT_EQUAL(cc.len, 0, "");

    // left
    evbuf[0] = TT_CLI_EV_LEFT;
    ret = tt_cline_input(&cl, evbuf, 1);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    cmd = tt_cline_cursor_data(&cl, &cc);
    TT_UT_EQUAL(cmd, TT_FALSE, "");
    TT_UT_EQUAL(cc.addr, NULL, "");
    TT_UT_EQUAL(cc.len, 0, "");

    /////////////// cmd: "123"

    // left
    evbuf[0] = TT_CLI_EV_LEFT;
    ret = tt_cline_input(&cl, evbuf, 1);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    cmd = tt_cline_cursor_data(&cl, &cc);
    TT_UT_EQUAL(cmd, TT_TRUE, "");
    TT_UT_EQUAL(cc.len, 3, "");
    TT_UT_EQUAL(tt_strncmp((tt_char_t *)cc.addr, "123", 3), 0, "");

    // left
    evbuf[0] = TT_CLI_EV_LEFT;
    ret = tt_cline_input(&cl, evbuf, 1);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    cmd = tt_cline_cursor_data(&cl, &cc);
    TT_UT_EQUAL(cmd, TT_TRUE, "");
    TT_UT_EQUAL(cc.len, 2, "");
    TT_UT_EQUAL(tt_strncmp((tt_char_t *)cc.addr, "12", 2), 0, "");

    // left
    evbuf[0] = TT_CLI_EV_LEFT;
    ret = tt_cline_input(&cl, evbuf, 1);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    cmd = tt_cline_cursor_data(&cl, &cc);
    TT_UT_EQUAL(cmd, TT_TRUE, "");
    TT_UT_EQUAL(cc.len, 1, "");
    TT_UT_EQUAL(tt_strncmp((tt_char_t *)cc.addr, "1", 1), 0, "");

    //////////// 3 spaces

    // left
    evbuf[0] = TT_CLI_EV_LEFT;
    ret = tt_cline_input(&cl, evbuf, 1);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    cmd = tt_cline_cursor_data(&cl, &cc);
    TT_UT_EQUAL(cmd, TT_TRUE, "");
    TT_UT_EQUAL(cc.addr, NULL, "");
    TT_UT_EQUAL(cc.len, 0, "");

    // left
    evbuf[0] = TT_CLI_EV_LEFT;
    ret = tt_cline_input(&cl, evbuf, 1);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    cmd = tt_cline_cursor_data(&cl, &cc);
    TT_UT_EQUAL(cmd, TT_TRUE, "");
    TT_UT_EQUAL(cc.addr, NULL, "");
    TT_UT_EQUAL(cc.len, 0, "");

    // left
    evbuf[0] = TT_CLI_EV_LEFT;
    ret = tt_cline_input(&cl, evbuf, 1);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    cmd = tt_cline_cursor_data(&cl, &cc);
    TT_UT_EQUAL(cmd, TT_TRUE, "");
    TT_UT_EQUAL(cc.addr, NULL, "");
    TT_UT_EQUAL(cc.len, 0, "");

    tt_cline_destroy(&cl);
    tt_buf_destroy(&ob);

    // test end
    TT_TEST_CASE_LEAVE()
}
