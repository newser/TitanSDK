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

#include <cli/shell/tt_shcmd_cd.h>

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

static const tt_char_t __cd_info[] = "change directory";

static const tt_char_t __cd_usage[] = "testing change directory";

static tt_u32_t __cd_run(IN tt_shell_t *sh,
                         IN tt_u32_t argc,
                         IN tt_char_t *arv[],
                         OUT tt_buf_t *output);

tt_shcmd_t tt_g_shcmd_cd = {
    TT_SHCMD_NAME_CD, __cd_info, __cd_usage, __cd_run,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_u32_t __cd_run(IN tt_shell_t *sh,
                  IN tt_u32_t argc,
                  IN tt_char_t *argv[],
                  OUT tt_buf_t *output)
{
    tt_param_t *p;
    const tt_char_t *path;

    if (argc == 0) {
        return TT_CLIOC_NOOUT;
    }

    path = argv[0];
    p = tt_param_path_p2n(sh->root,
                          sh->current,
                          path,
                          (tt_u32_t)tt_strlen(path));
    if (p == NULL) {
        tt_buf_putf(output, "not found: %s", path);
        return TT_CLIOC_OUT;
    }

    if (p->type != TT_PARAM_DIR) {
        tt_buf_putf(output, "not a directory: %s", path);
        return TT_CLIOC_OUT;
    }

    sh->current = p;
    tt_cli_update_prefix(&sh->cli, NULL, p->name, 0);

    return TT_CLIOC_NOOUT;
}
