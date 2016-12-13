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

#include <init/config_shell/tt_config_command.h>

#include <init/config_shell/tt_cfgcmd_cd.h>
#include <init/config_shell/tt_cfgcmd_commit.h>
#include <init/config_shell/tt_cfgcmd_get.h>
#include <init/config_shell/tt_cfgcmd_help.h>
#include <init/config_shell/tt_cfgcmd_ls.h>
#include <init/config_shell/tt_cfgcmd_pwd.h>
#include <init/config_shell/tt_cfgcmd_quit.h>
#include <init/config_shell/tt_cfgcmd_restore.h>
#include <init/config_shell/tt_cfgcmd_set.h>
#include <init/config_shell/tt_cfgcmd_status.h>
#include <misc/tt_assert.h>

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

tt_cfgcmd_t *tt_g_cfgcmd[TT_CFGCMD_NUM] = {
    &tt_g_cfgcmd_ls, // TT_CFGCMD_LS
    &tt_g_cfgcmd_cd, // TT_CFGCMD_CD
    &tt_g_cfgcmd_help, // TT_CFGCMD_HELP
    &tt_g_cfgcmd_pwd, // TT_CFGCMD_PWD
    &tt_g_cfgcmd_quit, // TT_CFGCMD_QUIT
    &tt_g_cfgcmd_get, // TT_CFGCMD_GET
    &tt_g_cfgcmd_set, // TT_CFGCMD_SET
    &tt_g_cfgcmd_status, // TT_CFGCMD_STATUS,
    &tt_g_cfgcmd_commit, // TT_CFGCMD_COMMIT,
    &tt_g_cfgcmd_restore, // TT_CFGCMD_RESTORE,
};

const tt_char_t *tt_g_cfgcmd_name[TT_CFGCMD_NUM] = {
    TT_CFGCMD_NAME_LS, // TT_CFGCMD_LS
    TT_CFGCMD_NAME_CD, // TT_CFGCMD_CD
    TT_CFGCMD_NAME_HELP, // TT_CFGCMD_HELP
    TT_CFGCMD_NAME_PWD, // TT_CFGCMD_PWD
    TT_CFGCMD_NAME_QUIT, // TT_CFGCMD_QUIT
    TT_CFGCMD_NAME_GET, // TT_CFGCMD_GET
    TT_CFGCMD_NAME_SET, // TT_CFGCMD_SET
    TT_CFGCMD_NAME_STATUS, // TT_CFGCMD_STATUS,
    TT_CFGCMD_NAME_COMMIT, // TT_CFGCMD_COMMIT,
    TT_CFGCMD_NAME_RESTORE, // TT_CFGCMD_RESTORE,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_cfgcmd_t *tt_cfgcmd_find(IN const tt_char_t *name)
{
    tt_u32_t i;
    tt_cfgcmd_t *cmd;

    TT_ASSERT(name != NULL);

    for (i = 0; i < TT_CFGCMD_NUM; ++i) {
        cmd = tt_g_cfgcmd[i];

        if ((cmd != NULL) && (tt_strcmp(cmd->name, name) == 0)) {
            return cmd;
        }
    }
    return NULL;
}
