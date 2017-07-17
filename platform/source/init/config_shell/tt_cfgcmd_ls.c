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

#include <init/config_shell/tt_cfgcmd_ls.h>

#include <algorithm/tt_buffer_format.h>
#include <cli/tt_cli_def.h>
#include <init/config_shell/tt_config_shell.h>
#include <init/tt_config_object.h>
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

static const tt_char_t __ls_info[] = "list group contents or value information";

static const tt_char_t __ls_usage[] = "testing";

static tt_u32_t __ls_run(IN tt_cfgsh_t *sh,
                         IN tt_u32_t argc,
                         IN tt_char_t *arv[],
                         OUT tt_buf_t *output);

tt_cfgcmd_t tt_g_cfgcmd_ls = {
    TT_CFGCMD_NAME_LS, __ls_info, __ls_usage, __ls_run,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_u32_t __ls_current(IN tt_cfgsh_t *sh, OUT tt_buf_t *output);

static tt_u32_t __ls_single(IN tt_cfgsh_t *sh,
                            IN tt_char_t *path,
                            OUT tt_buf_t *output);

static tt_u32_t __ls_multiple(IN tt_cfgsh_t *sh,
                              IN tt_char_t *path[],
                              IN tt_u32_t path_num,
                              OUT tt_buf_t *output);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_u32_t __ls_run(IN tt_cfgsh_t *sh,
                  IN tt_u32_t argc,
                  IN tt_char_t *argv[],
                  OUT tt_buf_t *output)
{
    if (argc == 0) {
        return __ls_current(sh, output);
    } else if (argc == 1) {
        return __ls_single(sh, argv[0], output);
    } else {
        return __ls_multiple(sh, argv, argc, output);
    }
}

tt_u32_t __ls_current(IN tt_cfgsh_t *sh, OUT tt_buf_t *output)
{
    tt_cfgobj_t *cnode;
    tt_u32_t rp, wp;
    tt_char_t sep[2] = {TT_CLI_EV_ENTER, 0};
    tt_result_t result;

    cnode = sh->current;
    if (cnode == NULL) {
        tt_buf_putf(output, "internal error");
        return TT_CLIOC_OUT;
    }

    tt_buf_backup_rwp(output, &rp, &wp);
    result = tt_cfgobj_read(cnode, sep, output);
    if (!TT_OK(result)) {
        tt_buf_restore_rwp(output, &rp, &wp);
        if (result == TT_BAD_PARAM) {
            tt_buf_putf(output, "not supported operation");
        } else {
            tt_buf_putf(output, "internal error");
        }
    }

    return TT_CLIOC_OUT;
}

tt_u32_t __ls_single(IN tt_cfgsh_t *sh,
                     IN tt_char_t *path,
                     OUT tt_buf_t *output)
{
    tt_cfgobj_t *cnode;
    tt_blob_t path_blob;
    tt_u32_t rp, wp;
    tt_char_t sep[2] = {TT_CLI_EV_ENTER, 0};
    tt_result_t result;

    path_blob.addr = (tt_u8_t *)path;
    path_blob.len = (tt_u32_t)tt_strlen(path);
    cnode = tt_cfgpath_p2n(sh->root, sh->current, &path_blob);
    if (cnode == NULL) {
        tt_buf_putf(output, "can not find: %s", path);
        return TT_CLIOC_OUT;
    }

    tt_buf_backup_rwp(output, &rp, &wp);
    result = tt_cfgobj_read(cnode, sep, output);
    if (!TT_OK(result)) {
        tt_buf_restore_rwp(output, &rp, &wp);
        if (result == TT_BAD_PARAM) {
            tt_buf_putf(output, "not supported operation");
        } else {
            tt_buf_putf(output, "internal error");
        }
    }

    return TT_CLIOC_OUT;
}

tt_u32_t __ls_multiple(IN tt_cfgsh_t *sh,
                       IN tt_char_t *path[],
                       IN tt_u32_t path_num,
                       OUT tt_buf_t *output)
{
    tt_u32_t i;
    tt_bool_t has_enter = TT_FALSE;

    for (i = 0; i < path_num; ++i) {
        tt_buf_put_cstr(output, path[i]);
        tt_buf_put_u8(output, ':');
        tt_buf_put_u8(output, TT_CLI_EV_ENTER);

        __ls_single(sh, path[i], output);
        tt_buf_put_rep(output, TT_CLI_EV_ENTER, 2);

        has_enter = TT_TRUE;
    }
    if (has_enter) {
        tt_buf_dec_wp(output, 1);
    }

    return TT_CLIOC_OUT;
}
