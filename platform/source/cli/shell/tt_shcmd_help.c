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

#include <cli/shell/tt_shcmd_help.h>

#include <algorithm/tt_buffer_format.h>
#include <cli/shell/tt_shell.h>
#include <param/tt_param_path.h>

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

static const tt_char_t __help_info[] = "show command or entry usage";

static const tt_char_t __help_usage[] = "";

static tt_u32_t __help_run(IN tt_shell_t *sh,
                           IN tt_u32_t argc,
                           IN tt_char_t *arv[],
                           OUT tt_buf_t *output);

tt_shcmd_t tt_g_shcmd_help = {
    TT_SHCMD_NAME_HELP, __help_info, __help_usage, __help_run,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_u32_t __help_all(IN tt_shell_t *sh, OUT tt_buf_t *output);

static tt_u32_t __help_single(IN tt_shell_t *sh,
                              IN tt_char_t *name,
                              OUT tt_buf_t *output);

static tt_u32_t __help_multiple(IN tt_shell_t *sh,
                                IN tt_char_t *name[],
                                IN tt_u32_t path_num,
                                OUT tt_buf_t *output);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_u32_t __help_run(IN tt_shell_t *sh,
                    IN tt_u32_t argc,
                    IN tt_char_t *argv[],
                    OUT tt_buf_t *output)
{
    if (argc == 0) {
        return __help_all(sh, output);
    } else if (argc == 1) {
        return __help_single(sh, argv[0], output);
    } else {
        return __help_multiple(sh, argv, argc, output);
    }
}

tt_u32_t __help_all(IN tt_shell_t *sh, OUT tt_buf_t *output)
{
    tt_u32_t i, max_len = 0;

    for (i = 0; i < TT_SHCMD_NUM; ++i) {
        tt_shcmd_t *cmd;
        tt_u32_t len;

        cmd = tt_g_shcmd[i];
        len = (tt_u32_t)tt_strlen(cmd->name) + 4;
        max_len = TT_MAX(max_len, len);
    }

    for (i = 0; i < TT_SHCMD_NUM; ++i) {
        tt_shcmd_t *cmd;
        tt_u32_t len;

        cmd = tt_g_shcmd[i];
        len = (tt_u32_t)tt_strlen(cmd->name);

        tt_buf_put_cstr(output, cmd->name);
        tt_buf_put_rep(output, ' ', max_len - len);
        tt_buf_put_cstr(output, TT_COND(cmd->info != NULL, cmd->info, ""));
        if (i != (TT_SHCMD_NUM - 1)) {
            TT_SH_NEWLINE(output);
        }
    }

    return TT_CLIOC_OUT;
}

tt_u32_t __help_single(IN tt_shell_t *sh,
                       IN tt_char_t *name,
                       OUT tt_buf_t *output)
{
    tt_shcmd_t *cmd;
    tt_param_t *co;

    cmd = tt_shcmd_find(name);
    if (cmd != NULL) {
        tt_buf_put_cstr(output, TT_COND(cmd->usage != NULL, cmd->usage, ""));
        return TT_CLIOC_OUT;
    }

    co = tt_param_path_p2n(sh->root,
                           sh->current,
                           name,
                           (tt_u32_t)tt_strlen(name));
    if (co != NULL) {
        tt_buf_put_cstr(output, tt_param_detail(co));
        return TT_CLIOC_OUT;
    }

    tt_buf_putf(output, "not found: %s", name);
    return TT_CLIOC_OUT;
}

tt_u32_t __help_multiple(IN tt_shell_t *sh,
                         IN tt_char_t *name[],
                         IN tt_u32_t path_num,
                         OUT tt_buf_t *output)
{
    return __help_single(sh, name[0], output);
}
