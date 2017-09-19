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

#include <cli/shell/tt_console_shell.h>

#include <init/tt_init_config.h>
#include <io/tt_console.h>

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

static tt_result_t __csh_send(IN struct tt_cli_s *cli,
                              IN void *param,
                              IN tt_u8_t *ev,
                              IN tt_u32_t ev_num);

static tt_cli_itf_t __csh_itf = {
    NULL,

    __csh_send,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __csh_ev_handler(IN void *param,
                                    IN tt_cons_ev_t ev,
                                    IN tt_cons_ev_data_t *ev_data);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_console_sh_create(IN tt_shell_t *sh,
                                 IN tt_cli_mode_t mode,
                                 IN OPT tt_sh_attr_t *attr)
{
    return tt_sh_create(sh, tt_g_config_root, mode, &__csh_itf, attr);
}

tt_result_t tt_console_sh_run(IN tt_shell_t *sh, IN tt_bool_t local)
{
    TT_ASSERT(sh != NULL);
    TT_ASSERT(sh->cli.itf.send == __csh_send);

    tt_sh_start(sh);
    tt_console_run(__csh_ev_handler, sh, local);

    return TT_SUCCESS;
}

tt_result_t __csh_send(IN struct tt_cli_s *cli,
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

tt_result_t __csh_ev_handler(IN void *param,
                             IN tt_cons_ev_t ev,
                             IN tt_cons_ev_data_t *ev_data)
{
    tt_cons_ev_key_t *ev_key;
    tt_u32_t i;
    tt_shell_t *sh = (tt_shell_t *)param;
    tt_cli_t *cli = &sh->cli;

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
