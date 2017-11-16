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

#include <cli/shell/tt_shcmd_set.h>

#include <algorithm/tt_buffer_format.h>
#include <cli/shell/tt_shell.h>
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

static const tt_char_t __set_info[] = "set value";

static const tt_char_t __set_usage[] = "testing set";

static tt_u32_t __set_run(IN tt_shell_t *sh,
                          IN tt_u32_t argc,
                          IN tt_char_t *arv[],
                          OUT tt_buf_t *output);

tt_shcmd_t tt_g_shcmd_set = {
    TT_SHCMD_NAME_SET, __set_info, __set_usage, __set_run,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_u32_t __set_run(IN tt_shell_t *sh,
                   IN tt_u32_t argc,
                   IN tt_char_t *argv[],
                   OUT tt_buf_t *output)
{
    const tt_char_t *path, *val;
    tt_cfgobj_t *co;
    tt_u32_t rp, wp;
    tt_result_t result;

    if (argc < 2) {
        tt_buf_putf(output, "usage: set [name] [value]");
        return TT_CLIOC_OUT;
    }

    path = argv[0];
    co = tt_cfgpath_p2n(sh->root, sh->current, path, (tt_u32_t)tt_strlen(path));
    if (co == NULL) {
        tt_buf_putf(output, "not found: %s", argv[0]);
        return TT_CLIOC_OUT;
    }

    if (co->type == TT_CFGOBJ_DIR) {
        tt_buf_putf(output, "set: %s: is a direcoty", path);
        return TT_CLIOC_OUT;
    }

    val = argv[1];
    tt_buf_backup_rwp(output, &rp, &wp);
    result = tt_cfgobj_write(co, (tt_u8_t *)val, (tt_u32_t)tt_strlen(val));
    if (TT_OK(result)) {
        return TT_CLIOC_NOOUT;
    } else {
        tt_buf_restore_rwp(output, &rp, &wp);
        if (result == TT_E_UNSUPPORT) {
            tt_buf_putf(output, "not supported operation");
        } else {
            tt_buf_putf(output, "internal error");
        }
        return TT_CLIOC_OUT;
    }
}
