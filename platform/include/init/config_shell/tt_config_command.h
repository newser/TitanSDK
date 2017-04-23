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

/**
@file tt_config_cmd.h
@brief config shell command

this file defines config shell command
*/

#ifndef __TT_CONFIG_CMD__
#define __TT_CONFIG_CMD__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_CFGCMD_NAME_LS "ls"
#define TT_CFGCMD_NAME_CD "cd"
#define TT_CFGCMD_NAME_HELP "help"
#define TT_CFGCMD_NAME_PWD "pwd"
#define TT_CFGCMD_NAME_QUIT "quit"
#define TT_CFGCMD_NAME_GET "get"
#define TT_CFGCMD_NAME_SET "set"
#define TT_CFGCMD_NAME_STATUS "status"
#define TT_CFGCMD_NAME_COMMIT "commit"
#define TT_CFGCMD_NAME_RESTORE "restore"

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_cfgsh_s;
struct tt_buf_s;

// see return value of tt_cli_on_cmd_t
typedef tt_u32_t (*tt_cfgcmd_run_t)(IN struct tt_cfgsh_s *sh,
                                    IN tt_u32_t argc,
                                    IN tt_char_t *arv[],
                                    OUT struct tt_buf_s *output);

typedef enum {
    TT_CFGCMD_LS,
    TT_CFGCMD_CD,
    TT_CFGCMD_HELP,
    TT_CFGCMD_PWD,
    TT_CFGCMD_QUIT,
    TT_CFGCMD_GET,
    TT_CFGCMD_SET,
    TT_CFGCMD_STATUS,
    TT_CFGCMD_COMMIT,
    TT_CFGCMD_RESTORE,

    TT_CFGCMD_NUM,
} tt_cfgcmd_id_t;
#define TT_CFGCMD_VALID(c) ((c) < TT_CFGCMD_NUM)

typedef struct
{
    const tt_char_t *name;
    const tt_char_t *info;
    const tt_char_t *usage;

    tt_cfgcmd_run_t run;
} tt_cfgcmd_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

extern tt_cfgcmd_t *tt_g_cfgcmd[TT_CFGCMD_NUM];

extern const tt_char_t *tt_g_cfgcmd_name[TT_CFGCMD_NUM];

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_cfgcmd_t *tt_cfgcmd_find(IN const tt_char_t *name);

#endif /* __TT_CONFIG_CMD__ */
