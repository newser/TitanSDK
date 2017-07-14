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

#include <init/config_shell/tt_cfgcmd_pwd.h>

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

static const tt_char_t __cd_info[] = "change group";

static const tt_char_t __cd_usage[] = "testing change group";

static tt_u32_t __cd_run(IN tt_cfgsh_t *sh,
                         IN tt_u32_t argc,
                         IN tt_char_t *arv[],
                         OUT tt_buf_t *output);

tt_cfgcmd_t tt_g_cfgcmd_cd = {
    TT_CFGCMD_NAME_CD, __cd_info, __cd_usage, __cd_run,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_u32_t __cd_run(IN tt_cfgsh_t *sh,
                  IN tt_u32_t argc,
                  IN tt_char_t *argv[],
                  OUT tt_buf_t *output)
{
    tt_blob_t path;
    tt_cfgnode_t *cnode;

    if (argc == 0) {
        return TT_CLIOC_NOOUT;
    }

    path.addr = (tt_u8_t *)argv[0];
    path.len = (tt_u32_t)tt_strlen(argv[0]);
    cnode = tt_cfgpath_p2n(sh->root, sh->current, &path);
    if (cnode == NULL) {
        tt_buf_putf(output, "can not find: %s", path.addr);
        return TT_CLIOC_OUT;
    }

    if (cnode->type != TT_CFGNODE_TYPE_GROUP) {
        tt_buf_putf(output, "not a group: %s", path.addr);
        return TT_CLIOC_OUT;
    }

    sh->current = cnode;
    tt_cli_update_prefix(&sh->cli, NULL, cnode->name, 0);

#if 0
    tt_buf_put_cstr(output, "cd: /");
    tt_cfgpath_n2p(sh->root, sh->current, output);
    return TT_CLIOC_OUT;
#else
    return TT_CLIOC_NOOUT;
#endif
}
