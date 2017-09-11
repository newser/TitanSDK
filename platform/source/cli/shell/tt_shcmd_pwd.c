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

#include <cli/shell/tt_shcmd_pwd.h>

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

static const tt_char_t __pwd_info[] = "show current shell path";

static const tt_char_t __pwd_usage[] = "testing pwd";

static tt_u32_t __pwd_run(IN tt_shell_t *sh,
                          IN tt_u32_t argc,
                          IN tt_char_t *arv[],
                          OUT tt_buf_t *output);

tt_shcmd_t tt_g_shcmd_pwd = {
    TT_SHCMD_NAME_PWD, __pwd_info, __pwd_usage, __pwd_run,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_u32_t __pwd_run(IN tt_shell_t *sh,
                   IN tt_u32_t argc,
                   IN tt_char_t *argv[],
                   OUT tt_buf_t *output)
{
    tt_u32_t rp, wp;

    if (sh->current == NULL) {
        tt_buf_putf(output, "internal error");
        return TT_CLIOC_OUT;
    }

    tt_buf_backup_rwp(output, &rp, &wp);
    tt_buf_put_u8(output, '/');
    if (!TT_OK(tt_cfgpath_n2p(sh->root, sh->current, output))) {
        tt_buf_restore_rwp(output, &rp, &wp);

        tt_buf_putf(output, "internal error");
        return TT_CLIOC_OUT;
    }

    return TT_CLIOC_OUT;
}
