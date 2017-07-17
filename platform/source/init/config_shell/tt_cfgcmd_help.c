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

#include <init/config_shell/tt_cfgcmd_help.h>

#include <algorithm/tt_buffer_format.h>
#include <init/config_shell/tt_config_shell.h>
#include <init/tt_config_path.h>

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

static const tt_char_t __help_info[] = "show config node or command detail";

static const tt_char_t __help_usage[] = "";

static tt_u32_t __help_run(IN tt_cfgsh_t *sh,
                           IN tt_u32_t argc,
                           IN tt_char_t *arv[],
                           OUT tt_buf_t *output);

tt_cfgcmd_t tt_g_cfgcmd_help = {
    TT_CFGCMD_NAME_HELP, __help_info, __help_usage, __help_run,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_u32_t __help_list(IN tt_cfgsh_t *sh, OUT tt_buf_t *output);

static tt_u32_t __help_single(IN tt_cfgsh_t *sh,
                              IN tt_char_t *name,
                              OUT tt_buf_t *output);

static tt_u32_t __help_multiple(IN tt_cfgsh_t *sh,
                                IN tt_char_t *name[],
                                IN tt_u32_t path_num,
                                OUT tt_buf_t *output);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_u32_t __help_run(IN tt_cfgsh_t *sh,
                    IN tt_u32_t argc,
                    IN tt_char_t *argv[],
                    OUT tt_buf_t *output)
{
    if (argc == 0) {
        return __help_list(sh, output);
    } else if (argc == 1) {
        return __help_single(sh, argv[0], output);
    } else {
        return __help_multiple(sh, argv, argc, output);
    }
}

tt_u32_t __help_list(IN tt_cfgsh_t *sh, OUT tt_buf_t *output)
{
    tt_u32_t i, max_len = 11;
    tt_bool_t has_enter = TT_FALSE;

    for (i = 0; i < TT_CFGCMD_NUM; ++i) {
        tt_cfgcmd_t *cmd;
        tt_u32_t name_len;

        cmd = tt_g_cfgcmd[i];
        if (cmd == NULL) {
            continue;
        }

        name_len = (tt_u32_t)tt_strlen(cmd->name) + 4;

        max_len = TT_MAX(max_len, name_len);
    }

    tt_buf_put_cstr(output, "COMMAND");
    tt_buf_put_rep(output, ' ', max_len - 7);
    tt_buf_put_cstr(output, "DESCRIPTION");
    tt_buf_put_u8(output, TT_CLI_EV_ENTER);

    for (i = 0; i < TT_CFGCMD_NUM; ++i) {
        tt_cfgcmd_t *cmd;
        tt_u32_t name_len;

        cmd = tt_g_cfgcmd[i];
        if (cmd == NULL) {
            continue;
        }

        name_len = (tt_u32_t)tt_strlen(cmd->name);

        tt_buf_put_cstr(output, cmd->name);
        tt_buf_put_rep(output, ' ', max_len - name_len);
        tt_buf_put_cstr(output, TT_COND(cmd->info != NULL, cmd->info, ""));
        tt_buf_put_u8(output, TT_CLI_EV_ENTER);

        has_enter = TT_CLIOC_OUT;
    }
    if (has_enter) {
        tt_buf_dec_wp(output, 1);
    }

    return TT_CLIOC_OUT;
}

tt_u32_t __help_single(IN tt_cfgsh_t *sh,
                       IN tt_char_t *name,
                       OUT tt_buf_t *output)
{
    tt_cfgcmd_t *cmd;
    tt_cfgobj_t *cnode;
    tt_blob_t name_blob;

    cmd = tt_cfgcmd_find(name);
    if (cmd != NULL) {
        tt_buf_put_cstr(output, TT_COND(cmd->usage != NULL, cmd->usage, ""));
        return TT_CLIOC_OUT;
    }

    name_blob.addr = (tt_u8_t *)name;
    name_blob.len = (tt_u32_t)tt_strlen(name);
    cnode = tt_cfgpath_p2n(sh->root, sh->current, &name_blob);
    if (cnode != NULL) {
        tt_buf_put_cstr(output, tt_cfgobj_detail(cnode));
        return TT_CLIOC_OUT;
    }

    tt_buf_putf(output, "can not find: %s", name);
    return TT_CLIOC_OUT;
}

tt_u32_t __help_multiple(IN tt_cfgsh_t *sh,
                         IN tt_char_t *name[],
                         IN tt_u32_t path_num,
                         OUT tt_buf_t *output)
{
    return __help_single(sh, name[0], output);
}
