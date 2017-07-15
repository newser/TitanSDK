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

#include <cli/tt_cli_demo.h>

#include <algorithm/tt_buffer_format.h>
#include <io/tt_console.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef struct
{
    tt_cli_t cli;
} __cli_demo_t;

typedef struct
{
    const tt_char_t **cmd;
    tt_u32_t cmd_num;

    const tt_char_t **arg;
    tt_u32_t arg_num;
} __cli_demo_app_t;

typedef struct
{
    tt_u32_t a;
} __cli_demo_io_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static __cli_demo_t tt_s_cli_demo;
static __cli_demo_app_t tt_s_cli_demo_app;
static __cli_demo_io_t tt_s_cli_demo_io;

static const tt_char_t *__cli_demo_cmd[] =
    {"help", "ls", "lsof", "cd", "cdets", "unit-test"};

static const tt_char_t *__cli_demo_arg[] = {"Applications",
                                            "Users",
                                            "etc",
                                            "opt",
                                            "option",
                                            "var",
                                            "Library",
                                            "Volume-A",
                                            "Volume-B"};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_u32_t __cli_app_on_cmd(IN struct tt_cli_s *cli,
                                 IN void *param,
                                 IN const tt_char_t *cmd,
                                 IN tt_buf_t *output);
static tt_u32_t __cli_app_on_complete(IN struct tt_cli_s *cli,
                                      IN void *param,
                                      IN tt_blob_t *cursor_data,
                                      IN tt_bool_t wait4cmd,
                                      IN tt_buf_t *output);
static tt_bool_t __cli_app_on_quit(IN struct tt_cli_s *cli,
                                   IN struct tt_buf_s *output);

static tt_result_t __cli_io_send(IN struct tt_cli_s *cli,
                                 IN void *param,
                                 IN tt_u8_t *ev,
                                 IN tt_u32_t ev_num);

static tt_result_t __console_ev_handler(IN void *param,
                                        IN tt_cons_ev_t ev,
                                        IN tt_cons_ev_data_t *ev_data);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_cli_demo_run()
{
    tt_result_t result;
    tt_cli_cb_t cb;
    tt_cli_itf_t itf;

    tt_s_cli_demo_app.cmd = __cli_demo_cmd;
    tt_s_cli_demo_app.cmd_num =
        sizeof(__cli_demo_cmd) / sizeof(__cli_demo_cmd[0]);
    tt_s_cli_demo_app.arg = __cli_demo_arg;
    tt_s_cli_demo_app.arg_num =
        sizeof(__cli_demo_arg) / sizeof(__cli_demo_arg[0]);

    cb.param = &tt_s_cli_demo_app;
    cb.on_cmd = __cli_app_on_cmd;
    cb.on_complete = __cli_app_on_complete;
    cb.on_quit = __cli_app_on_quit;

    itf.param = &tt_s_cli_demo_io;
    itf.send = __cli_io_send;

    result =
        tt_cli_create(&tt_s_cli_demo.cli, TT_CLI_MODE_DEFAUTL, &cb, &itf, NULL);
    if (!TT_OK(result)) {
        return TT_FAIL;
    }

    result = tt_cli_start(&tt_s_cli_demo.cli);
    if (!TT_OK(result)) {
        tt_cli_destroy(&tt_s_cli_demo.cli);
        return TT_FAIL;
    }

    tt_console_run(__console_ev_handler, &tt_s_cli_demo.cli, TT_TRUE);

    tt_cli_destroy(&tt_s_cli_demo.cli);
    return TT_SUCCESS;
}

tt_u32_t __cli_app_on_cmd(IN struct tt_cli_s *cli,
                          IN void *param,
                          IN const tt_char_t *cmd,
                          IN tt_buf_t *output)
{
    if (cmd != NULL) {
        tt_buf_put(output, (tt_u8_t *)"input: ", sizeof("input: ") - 1);
        tt_buf_put(output, (tt_u8_t *)cmd, (tt_u32_t)tt_strlen(cmd));
    } else {
        tt_buf_put(output, (tt_u8_t *)"exiting..", sizeof("exiting..") - 1);
    }

    return TT_CLIOC_OUT;
}

tt_u32_t __cli_app_on_complete(IN struct tt_cli_s *cli,
                               IN void *param,
                               IN tt_blob_t *cursor_data,
                               IN tt_bool_t wait4cmd,
                               IN tt_buf_t *output)
{
    __cli_demo_app_t *app = (__cli_demo_app_t *)param;
    const tt_char_t **option;
    tt_u32_t option_num;
    tt_u32_t status;

    if (wait4cmd) {
        option = app->cmd;
        option_num = app->cmd_num;
    } else {
        option = app->arg;
        option_num = app->arg_num;
    }

    tt_cli_complete(cursor_data, option, option_num, &status, output);

    return status;
}

tt_bool_t __cli_app_on_quit(IN struct tt_cli_s *cli, IN struct tt_buf_s *output)
{
    tt_buf_put_cstr(output, "goodbye");
    return TT_TRUE;
}

tt_result_t __cli_io_send(IN struct tt_cli_s *cli,
                          IN void *param,
                          IN tt_u8_t *ev,
                          IN tt_u32_t ev_num)
{
    tt_u32_t i;
    tt_cons_ev_data_t ev_data;

    for (i = 0; i < ev_num; ++i) {
        tt_u8_t e = ev[i];
        if (TT_CLI_EV_VALID(e)) {
            switch (e) {
#define __EVMAP(from, to)                                                      \
    case from:                                                                 \
        ev[i] = to;                                                            \
        break
                __EVMAP(TT_CLI_EV_UP, TT_CONS_EXTKEY_UP);
                __EVMAP(TT_CLI_EV_DOWN, TT_CONS_EXTKEY_DOWN);
                __EVMAP(TT_CLI_EV_RIGHT, TT_CONS_EXTKEY_RIGHT);
                __EVMAP(TT_CLI_EV_LEFT, TT_CONS_EXTKEY_LEFT);
                __EVMAP(TT_CLI_EV_INTR, TT_CONS_EXTKEY_CTRLC);
                __EVMAP(TT_CLI_EV_QUIT, TT_CONS_EXTKEY_CTRLD);
                __EVMAP(TT_CLI_EV_DELETE, TT_CONS_EXTKEY_DELETE);
                __EVMAP(TT_CLI_EV_ENTER, TT_CONS_EXTKEY_CRLF);
                __EVMAP(TT_CLI_EV_TAB, TT_CONS_EXTKEY_TAB);
#undef __EVMAP
                default:
                    break;
            }
        }
    }

    ev_data.key.key = ev;
    ev_data.key.key_num = ev_num;
    return tt_console_send(TT_CONS_EV_KEY, &ev_data);
}

tt_result_t __console_ev_handler(IN void *param,
                                 IN tt_cons_ev_t ev,
                                 IN tt_cons_ev_data_t *ev_data)
{
    tt_u32_t i;
    tt_cons_ev_key_t *ev_key;
    tt_cli_t *cli = (tt_cli_t *)param;

    if (ev != TT_CONS_EV_KEY) {
        return TT_SUCCESS;
    }

    ev_key = &ev_data->key;
    for (i = 0; i < ev_key->key_num; ++i) {
        tt_u8_t k = ev_key->key[i];
        if (TT_CONS_KEY_IS_EXTENDED(k)) {
            switch (k) {
#define __EVMAP(from, to)                                                      \
    case from:                                                                 \
        ev_key->key[i] = to;                                                   \
        break
                __EVMAP(TT_CONS_EXTKEY_UP, TT_CLI_EV_UP);
                __EVMAP(TT_CONS_EXTKEY_DOWN, TT_CLI_EV_DOWN);
                __EVMAP(TT_CONS_EXTKEY_RIGHT, TT_CLI_EV_RIGHT);
                __EVMAP(TT_CONS_EXTKEY_LEFT, TT_CLI_EV_LEFT);
                __EVMAP(TT_CONS_EXTKEY_CTRLC, TT_CLI_EV_INTR);
                __EVMAP(TT_CONS_EXTKEY_CTRLD, TT_CLI_EV_QUIT);
                __EVMAP(TT_CONS_EXTKEY_DELETE, TT_CLI_EV_DELETE);
                __EVMAP(TT_CONS_EXTKEY_CRLF, TT_CLI_EV_ENTER);
                __EVMAP(TT_CONS_EXTKEY_TAB, TT_CLI_EV_TAB);
#undef __EVMAP
                default:
                    break;
            }
        }
    }

    if (tt_cli_input(cli, ev_key->key, ev_key->key_num) != TT_END) {
        return TT_SUCCESS;
    } else {
        // any failure would result in console exit
        return TT_END;
    }
}
