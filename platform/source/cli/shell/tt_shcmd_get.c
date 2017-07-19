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

#include <cli/shell/tt_shcmd_get.h>

#include <algorithm/tt_buffer_format.h>
#include <cli/shell/tt_shell.h>
#include <init/tt_config_directory.h>
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

static const tt_char_t __get_info[] = "get entry value";

static const tt_char_t __get_usage[] = "testing get";

static tt_u32_t __get_run(IN tt_shell_t *sh,
                          IN tt_u32_t argc,
                          IN tt_char_t *arv[],
                          OUT tt_buf_t *output);

tt_shcmd_t tt_g_shcmd_get = {
    TT_SHCMD_NAME_GET, __get_info, __get_usage, __get_run,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_u32_t __get_single(IN tt_shell_t *sh,
                             IN tt_char_t *path,
                             OUT tt_buf_t *output);

static tt_u32_t __get_multiple(IN tt_shell_t *sh,
                               IN tt_char_t *path[],
                               IN tt_u32_t path_num,
                               OUT tt_buf_t *output);

static tt_u32_t __get_val(IN tt_cfgobj_t *co, OUT tt_buf_t *output);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_u32_t __get_run(IN tt_shell_t *sh,
                   IN tt_u32_t argc,
                   IN tt_char_t *argv[],
                   OUT tt_buf_t *output)
{
    if (argc == 0) {
        tt_buf_putf(output, "usage: get [name]");
        return TT_CLIOC_OUT;
    } else if (argc == 1) {
        return __get_single(sh, argv[0], output);
    } else {
        return __get_multiple(sh, argv, argc, output);
    }
}

tt_u32_t __get_single(IN tt_shell_t *sh,
                      IN tt_char_t *path,
                      OUT tt_buf_t *output)
{
    tt_cfgobj_t *co;

    co = tt_cfgpath_p2n(sh->root, sh->current, path, (tt_u32_t)tt_strlen(path));
    if (co == NULL) {
        tt_buf_putf(output, "not found: %s", path);
        return TT_CLIOC_OUT;
    }

    if (co->type == TT_CFGOBJ_DIR) {
        tt_buf_putf(output, "get: %s: is a direcoty", path);
        return TT_CLIOC_OUT;
    }

    return __get_val(co, output);
}

tt_u32_t __get_multiple(IN tt_shell_t *sh,
                        IN tt_char_t *path[],
                        IN tt_u32_t path_num,
                        OUT tt_buf_t *output)
{
    tt_u32_t i;

    for (i = 0; i < path_num; ++i) {
        tt_buf_put_cstr(output, path[i]);
        tt_buf_put_u8(output, ':');
        TT_SH_NEWLINE(output);

        __get_single(sh, path[i], output);
        if (i != (path_num - 1)) {
            TT_SH_NEWLINE(output);
            TT_SH_NEWLINE(output);
        }
    }

    return TT_CLIOC_OUT;
}

tt_u32_t __get_val(IN tt_cfgobj_t *co, OUT tt_buf_t *output)
{
    tt_u32_t rp, wp;
    tt_result_t result;

    tt_buf_put_cstr(output, co->name);
    tt_buf_put_cstr(output, ": ");

    tt_buf_backup_rwp(output, &rp, &wp);
    result = tt_cfgobj_read(co, tt_g_sh_line_sep, output);
    if (!TT_OK(result)) {
        tt_buf_restore_rwp(output, &rp, &wp);
        if (result == TT_NOT_SUPPORT) {
            tt_buf_putf(output, "not supported operation");
        } else {
            tt_buf_putf(output, "internal error");
        }
    }

    return TT_CLIOC_OUT;
}
