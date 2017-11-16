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

#include <cli/shell/tt_shell.h>

#include <algorithm/tt_buffer_format.h>
#include <cli/shell/tt_shell_command.h>
#include <init/tt_config_exe.h>
#include <init/tt_config_path.h>
#include <memory/tt_memory_alloc.h>

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

const tt_char_t tt_g_sh_line_sep[2] = {TT_CLI_EV_ENTER, 0};

const tt_char_t tt_g_sh_colume_sep[5] = {' ', ' ', ' ', ' ', 0};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_u32_t __sh_on_cmd(IN tt_cli_t *cli,
                            IN void *param,
                            IN const tt_char_t *cmd,
                            IN tt_buf_t *output);

static tt_u32_t __sh_on_complete(IN tt_cli_t *cli,
                                 IN void *param,
                                 IN tt_u8_t *cur,
                                 IN tt_u32_t cur_len,
                                 IN tt_bool_t wait4cmd,
                                 IN struct tt_buf_s *output);

static tt_bool_t __sh_on_quit(IN tt_cli_t *cli, IN tt_buf_t *output);

tt_result_t __parse_arg(IN tt_shell_t *sh, IN tt_char_t *line);

static tt_result_t __put_arg(IN tt_shell_t *sh, IN tt_char_t *arg);

static tt_result_t __expand_arg(IN tt_shell_t *sh);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_sh_create(IN tt_shell_t *sh,
                         IN tt_cfgobj_t *root,
                         IN tt_cli_mode_t mode,
                         IN tt_cli_itf_t *itf,
                         IN OPT tt_sh_attr_t *attr)
{
    tt_sh_attr_t __attr;
    tt_cli_cb_t cb;

    TT_ASSERT(sh != NULL);

    if (attr == NULL) {
        tt_sh_attr_default(&__attr);
        attr = &__attr;
    }

    sh->root = root;
    sh->current = root;

    tt_memset(sh->i_arg, 0, sizeof(sh->i_arg));
    sh->arg = sh->i_arg;
    sh->exit_msg = attr->exit_msg;
    sh->arg_size = TT_SH_ARG_NUM;
    sh->arg_num = 0;

    cb.param = NULL;
    cb.on_cmd = __sh_on_cmd;
    cb.on_complete = __sh_on_complete;
    cb.on_quit = __sh_on_quit;
    if (!TT_OK(tt_cli_create(&sh->cli, mode, &cb, itf, &attr->cli_attr))) {
        return TT_FAIL;
    }

    // set subtitle to current root name
    tt_cli_update_prefix(&sh->cli, NULL, root->name, 0);

    return TT_SUCCESS;
}

void tt_sh_destroy(IN tt_shell_t *sh)
{
    TT_ASSERT(sh != NULL);

    tt_cli_destroy(&sh->cli);

    if (sh->arg != sh->i_arg) {
        tt_free(sh->arg);
    }
}

void tt_sh_attr_default(IN tt_sh_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->exit_msg = "exiting...";

    tt_cli_attr_default(&attr->cli_attr);
}

tt_u32_t __sh_on_cmd(IN tt_cli_t *cli,
                     IN void *param,
                     IN const tt_char_t *line,
                     IN tt_buf_t *output)
{
    tt_shell_t *sh = TT_CONTAINER(cli, tt_shell_t, cli);
    const tt_char_t *name;
    tt_shcmd_t *cmd;
    tt_cfgobj_t *co;

    if (line[0] == 0) {
        return TT_CLIOC_NOOUT;
    }

    sh->arg_num = 0;
    if (!TT_OK(__parse_arg(sh, (tt_char_t *)line)) || (sh->arg_num == 0)) {
        tt_buf_putf(output, "bad input: ", line);
        return TT_CLIOC_OUT;
    }
    name = sh->arg[0];

    // run as a command
    cmd = tt_shcmd_find(name);
    if (cmd != NULL) {
        TT_ASSERT(cmd->run != NULL);
        TT_ASSERT(sh->arg_num > 0);
        // exclude the command name
        return cmd->run(sh, sh->arg_num - 1, &sh->arg[1], output);
    }

    // run as a executable object
    co = tt_cfgpath_p2n(sh->root, sh->current, name, (tt_u32_t)tt_strlen(name));
    if (co == NULL) {
        tt_buf_putf(output, "not found: %s", name);
        return TT_CLIOC_OUT;
    } else if (co->type != TT_CFGOBJ_EXE) {
        tt_buf_putf(output, "not executable: %s", name);
        return TT_CLIOC_OUT;
    } else {
        tt_u32_t rp, wp;
        tt_result_t result;
        tt_u32_t status = TT_CLIOC_NOOUT;

        tt_buf_backup_rwp(output, &rp, &wp);
        result = tt_cfgexe_run(TT_CFGOBJ_CAST(co, tt_cfgexe_t),
                               sh->arg_num - 1,
                               &sh->arg[1],
                               tt_g_sh_line_sep,
                               output,
                               &status);
        if (TT_OK(result)) {
            return status;
        } else {
            tt_buf_restore_rwp(output, &rp, &wp);
            tt_buf_putf(output, "%s: failed", name);
            return TT_CLIOC_OUT;
        }
    }
}

tt_u32_t __sh_on_complete(IN tt_cli_t *cli,
                          IN void *param,
                          IN tt_u8_t *cur,
                          IN tt_u32_t cur_len,
                          IN tt_bool_t wait4cmd,
                          IN struct tt_buf_s *output)
{
    tt_shell_t *sh = TT_CONTAINER(cli, tt_shell_t, cli);
    tt_u32_t status;

    if (wait4cmd) {
        if (!TT_OK(tt_cli_complete(cur,
                                   cur_len,
                                   tt_g_shcmd_name,
                                   TT_SHCMD_NUM,
                                   &status,
                                   output))) {
            status = TT_CLICP_NONE;
        }
    } else {
        if (TT_OK(tt_cfgpath_complete(sh->root,
                                      sh->current,
                                      (tt_char_t *)cur,
                                      cur_len,
                                      &status,
                                      output))) {
            switch (status) {
                case TT_CFGPCP_PARTIAL:
                    status = TT_CLICP_PARTIAL;
                    break;
                case TT_CFGPCP_FULL:
                    status = TT_CLICP_FULL;
                    break;
                case TT_CFGPCP_FULL_MORE:
                    status = TT_CLICP_FULL_MORE;
                    break;

                case TT_CFGPCP_NONE:
                default:
                    status = TT_CLICP_NONE;
                    break;
            }
        } else {
            status = TT_CLICP_NONE;
        }
    }

    return status;
}

tt_bool_t __sh_on_quit(IN tt_cli_t *cli, IN tt_buf_t *output)
{
    tt_shell_t *sh = TT_CONTAINER(cli, tt_shell_t, cli);

    if ((sh->exit_msg != NULL) && (sh->exit_msg[0] != 0)) {
        tt_buf_put_cstr(output, sh->exit_msg);
        return TT_TRUE;
    } else {
        return TT_FALSE;
    }
}

tt_result_t __parse_arg(IN tt_shell_t *sh, IN tt_char_t *line)
{
    tt_char_t *p = line, *arg = NULL, end_c;
    tt_bool_t d_quots = TT_FALSE, s_quots = TT_FALSE;

again:
    // arg start
    while ((*p != 0) && (*p == ' ')) {
        ++p;
    }
    if (*p == 0) {
        goto done;
    }
    if (*p == '"') {
        d_quots = TT_TRUE;
        arg = ++p;
    } else if (*p == '\'') {
        s_quots = TT_TRUE;
        arg = ++p;
    } else {
        arg = p;
    }

    // arg end
    if (d_quots) {
        end_c = '"';
        d_quots = TT_FALSE;
    } else if (s_quots) {
        end_c = '\'';
        s_quots = TT_FALSE;
    } else {
        end_c = ' ';
    }
    while ((*p != 0) && (*p != end_c)) {
        ++p;
    }
    if (arg < p) {
        TT_DO(__put_arg(sh, arg));
    }
    if (*p != 0) {
        *p++ = 0;
        goto again;
    }
done:

    return TT_SUCCESS;
}

tt_result_t __put_arg(IN tt_shell_t *sh, IN tt_char_t *arg)
{
    TT_ASSERT(sh->arg_num <= sh->arg_size);
    if ((sh->arg_num == sh->arg_size) && !TT_OK(__expand_arg(sh))) {
        return TT_FAIL;
    }
    TT_ASSERT(sh->arg_num < sh->arg_size);

    sh->arg[sh->arg_num++] = arg;
    return TT_SUCCESS;
}

tt_result_t __expand_arg(IN tt_shell_t *sh)
{
    tt_u32_t num, size;
    tt_char_t **new_arg;

    num = sh->arg_size + TT_SH_ARG_NUM;
    size = num * sizeof(tt_char_t *);
    new_arg = (tt_char_t **)tt_zalloc(size);
    if (new_arg == NULL) {
        return TT_FAIL;
    }
    tt_memcpy(new_arg, sh->arg, sh->arg_size * sizeof(tt_char_t *));

    if (sh->arg != sh->i_arg) {
        tt_free(sh->arg);
    }
    sh->arg = new_arg;
    sh->arg_size = num;

    return TT_SUCCESS;
}
