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

#include <init/config_shell/tt_config_shell.h>

#include <algorithm/tt_buffer_format.h>
#include <init/config_shell/tt_config_command.h>
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

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_u32_t __cfgsh_on_cmd(IN struct tt_cli_s *cli,
                               IN void *param,
                               IN const tt_char_t *cmd,
                               IN tt_buf_t *output);

static tt_u32_t __cfgsh_on_complete(IN struct tt_cli_s *cli,
                                    IN void *param,
                                    IN tt_blob_t *cursor_data,
                                    IN tt_bool_t wait4cmd,
                                    IN struct tt_buf_s *output);

static tt_bool_t __cfgsh_on_quit(IN struct tt_cli_s *cli, IN tt_buf_t *output);

tt_result_t __parse_arg(IN tt_cfgsh_t *sh, IN tt_char_t *line);
static tt_result_t __put_arg(IN tt_cfgsh_t *sh, IN tt_char_t *arg);
static tt_result_t __expand_arg(IN tt_cfgsh_t *sh);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_cfgsh_create(IN tt_cfgsh_t *sh,
                            IN tt_cli_mode_t mode,
                            IN tt_cli_itf_t *itf,
                            IN tt_cfgnode_t *root,
                            IN OPT tt_cfgsh_attr_t *attr)
{
    tt_cfgsh_attr_t __attr;
    tt_cli_cb_t cb;

    TT_ASSERT(sh != NULL);

    if (attr == NULL) {
        tt_cfgsh_attr_default(&__attr);
        attr = &__attr;
    }

    sh->root = root;
    sh->current = root;

    tt_memset(sh->i_arg, 0, sizeof(sh->i_arg));
    sh->arg = sh->i_arg;
    sh->arg_num = TT_CFGSH_ARG_NUM;
    sh->arg_idx = 0;

    cb.param = NULL;
    cb.on_cmd = __cfgsh_on_cmd;
    cb.on_complete = __cfgsh_on_complete;
    cb.on_quit = __cfgsh_on_quit;

    if (!TT_OK(tt_cli_create(&sh->cli, mode, &cb, itf, &attr->cli_attr))) {
        return TT_FAIL;
    }
    sh->exit_msg = attr->exit_msg;

    tt_cli_refresh_prefix(&sh->cli, NULL, root->name, 0);

    return TT_SUCCESS;
}

void tt_cfgsh_destroy(IN tt_cfgsh_t *sh)
{
    TT_ASSERT(sh != NULL);

    if (sh->arg != sh->i_arg) {
        tt_free(sh->arg);
    }

    tt_cli_destroy(&sh->cli);
}

void tt_cfgsh_attr_default(IN tt_cfgsh_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    tt_cli_attr_default(&attr->cli_attr);

    attr->exit_msg = "exiting";
}

tt_u32_t __cfgsh_on_cmd(IN struct tt_cli_s *cli,
                        IN void *param,
                        IN const tt_char_t *line,
                        IN tt_buf_t *output)
{
    tt_cfgsh_t *sh = TT_CONTAINER(cli, tt_cfgsh_t, cli);
    tt_cfgcmd_t *cmd;

    if (line[0] == 0) {
        return TT_CLIOC_NOOUT;
    }

    sh->arg_idx = 0;
    if (!TT_OK(__parse_arg(sh, (tt_char_t *)line)) || (sh->arg_idx == 0)) {
        tt_buf_putf(output, "bad input: ", line);
        return TT_CLIOC_OUT;
    }

    cmd = tt_cfgcmd_find(sh->arg[0]);
    if (cmd == NULL) {
        tt_buf_putf(output, "command not found: %s", sh->arg[0]);
        return TT_CLIOC_OUT;
    }

    TT_ASSERT(cmd->run != NULL);
    TT_ASSERT(sh->arg_idx > 0);
    return cmd->run(sh, sh->arg_idx - 1, &sh->arg[1], output);
}

tt_u32_t __cfgsh_on_complete(IN struct tt_cli_s *cli,
                             IN void *param,
                             IN tt_blob_t *cursor_data,
                             IN tt_bool_t wait4cmd,
                             IN struct tt_buf_s *output)
{
    tt_cfgsh_t *sh = TT_CONTAINER(cli, tt_cfgsh_t, cli);
    tt_u32_t status;

    if (wait4cmd) {
        if (!TT_OK(tt_cli_complete(cursor_data,
                                   tt_g_cfgcmd_name,
                                   TT_CFGCMD_NUM,
                                   &status,
                                   output))) {
            status = TT_CLICP_NONE;
        }
    } else {
        if (TT_OK(tt_cfgpath_complete(sh->root,
                                      sh->current,
                                      cursor_data,
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

tt_bool_t __cfgsh_on_quit(IN struct tt_cli_s *cli, IN tt_buf_t *output)
{
    tt_cfgsh_t *sh = TT_CONTAINER(cli, tt_cfgsh_t, cli);

    if ((sh->exit_msg != NULL) && (sh->exit_msg[0] != 0)) {
        tt_buf_put_cstr(output, sh->exit_msg);
        return TT_TRUE;
    } else {
        return TT_FALSE;
    }
}

tt_result_t __parse_arg(IN tt_cfgsh_t *sh, IN tt_char_t *line)
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
    TT_DO(__put_arg(sh, arg));
    if (*p != 0) {
        *p++ = 0;
        goto again;
    }
done:

    return TT_SUCCESS;
}

tt_result_t __put_arg(IN tt_cfgsh_t *sh, IN tt_char_t *arg)
{
    TT_ASSERT(sh->arg_idx <= sh->arg_num);
    if ((sh->arg_idx == sh->arg_num) && !TT_OK(__expand_arg(sh))) {
        return TT_FAIL;
    }
    TT_ASSERT(sh->arg_idx < sh->arg_num);

    sh->arg[sh->arg_idx++] = arg;
    return TT_SUCCESS;
}

tt_result_t __expand_arg(IN tt_cfgsh_t *sh)
{
    tt_u32_t num, size;
    tt_char_t **new_arg;

    num = sh->arg_num + TT_CFGSH_ARG_NUM;
    size = num * sizeof(tt_char_t *);
    new_arg = (tt_char_t **)tt_malloc(size);
    if (new_arg == NULL) {
        return TT_FAIL;
    }
    tt_memset(new_arg, 0, size);
    tt_memcpy(new_arg, sh->arg, sh->arg_num * sizeof(tt_char_t *));

    if (sh->arg != sh->i_arg) {
        tt_free(sh->arg);
    }
    sh->arg = new_arg;
    sh->arg_num = num;

    return TT_SUCCESS;
}
