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

#include <cli/shell/tt_shell_command.h>

#include <cli/shell/tt_shcmd_cd.h>
#include <cli/shell/tt_shcmd_help.h>
#include <cli/shell/tt_shcmd_ls.h>
#include <cli/shell/tt_shcmd_pwd.h>
#include <cli/shell/tt_shcmd_quit.h>
#include <cli/shell/tt_shcmd_set.h>
#include <cli/shell/tt_shcmd_show.h>
#include <misc/tt_assert.h>

#include <tt_cstd_api.h>

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

tt_shcmd_t *tt_g_shcmd[TT_SHCMD_NUM] = {
    &tt_g_shcmd_ls, // TT_SHCMD_LS
    &tt_g_shcmd_cd, // TT_SHCMD_CD
    &tt_g_shcmd_help, // TT_SHCMD_HELP
    &tt_g_shcmd_pwd, // TT_SHCMD_PWD
    &tt_g_shcmd_quit, // TT_SHCMD_QUIT
    &tt_g_shcmd_show, // TT_SHCMD_SHOW
    &tt_g_shcmd_set, // TT_SHCMD_SET
};

const tt_char_t *tt_g_shcmd_name[TT_SHCMD_NUM] = {
    TT_SHCMD_NAME_LS, // TT_SHCMD_LS
    TT_SHCMD_NAME_CD, // TT_SHCMD_CD
    TT_SHCMD_NAME_HELP, // TT_SHCMD_HELP
    TT_SHCMD_NAME_PWD, // TT_SHCMD_PWD
    TT_SHCMD_NAME_QUIT, // TT_SHCMD_QUIT
    TT_SHCMD_NAME_SHOW, // TT_SHCMD_SHOW
    TT_SHCMD_NAME_SET, // TT_SHCMD_SET
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_shcmd_t *tt_shcmd_find(IN const tt_char_t *name)
{
    tt_u32_t i;
    for (i = 0; i < TT_SHCMD_NUM; ++i) {
        if (tt_strcmp(tt_g_shcmd[i]->name, name) == 0) { return tt_g_shcmd[i]; }
    }
    return NULL;
}
