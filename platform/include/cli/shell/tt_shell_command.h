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

/**
@file tt_shell_command.h
@brief shell command

this file defines shell command
*/

#ifndef __TT_SHELL_COMMAND__
#define __TT_SHELL_COMMAND__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_SHCMD_NAME_LS "ls"
#define TT_SHCMD_NAME_CD "cd"
#define TT_SHCMD_NAME_HELP "help"
#define TT_SHCMD_NAME_PWD "pwd"
#define TT_SHCMD_NAME_QUIT "quit"
#define TT_SHCMD_NAME_GET "get"
#define TT_SHCMD_NAME_SET "set"

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_sh_s;
struct tt_buf_s;

// see return value of tt_cli_on_cmd_t
typedef tt_u32_t (*tt_shcmd_run_t)(IN struct tt_sh_s *sh,
                                   IN tt_u32_t argc,
                                   IN tt_char_t *arv[],
                                   OUT struct tt_buf_s *output);

typedef enum {
    TT_SHCMD_LS,
    TT_SHCMD_CD,
    TT_SHCMD_HELP,
    TT_SHCMD_PWD,
    TT_SHCMD_QUIT,
    TT_SHCMD_GET,
    TT_SHCMD_SET,

    TT_SHCMD_NUM,
} tt_shcmd_id_t;
#define TT_SHCMD_VALID(c) ((c) < TT_SHCMD_NUM)

typedef struct
{
    const tt_char_t *name;
    const tt_char_t *info;
    const tt_char_t *usage;

    tt_shcmd_run_t run;
} tt_shcmd_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

tt_export tt_shcmd_t *tt_g_shcmd[TT_SHCMD_NUM];

tt_export const tt_char_t *tt_g_shcmd_name[TT_SHCMD_NUM];

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_shcmd_t *tt_shcmd_find(IN const tt_char_t *name);

#endif /* __TT_SHELL_COMMAND__ */
