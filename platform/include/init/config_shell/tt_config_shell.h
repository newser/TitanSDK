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
@file tt_config_shell.h
@brief sh for configuration

this file defines config sh interface
*/

#ifndef __TT_CONFIG_SHELL__
#define __TT_CONFIG_SHELL__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <cli/tt_cli.h>
#include <init/tt_config_object.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_CFGSH_ARG_NUM 10

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct
{
    tt_cli_attr_t cli_attr;

    const tt_char_t *exit_msg;
} tt_cfgsh_attr_t;

typedef struct tt_cfgsh_s
{
    tt_cfgobj_t *root;
    tt_cfgobj_t *current;

    tt_char_t *i_arg[TT_CFGSH_ARG_NUM];
    tt_char_t **arg;
    tt_u32_t arg_num;
    tt_u32_t arg_idx;

    tt_cli_t cli;
    const tt_char_t *exit_msg;
} tt_cfgsh_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_cfgsh_create(IN tt_cfgsh_t *sh,
                                   IN tt_cli_mode_t mode,
                                   IN tt_cli_itf_t *itf,
                                   IN tt_cfgobj_t *root,
                                   IN OPT tt_cfgsh_attr_t *attr);

extern void tt_cfgsh_destroy(IN tt_cfgsh_t *sh);

extern void tt_cfgsh_attr_default(IN tt_cfgsh_attr_t *attr);

tt_inline tt_result_t tt_cfgsh_start(IN tt_cfgsh_t *sh)
{
    return tt_cli_start(&sh->cli);
}

tt_inline tt_result_t tt_cfgsh_input(IN tt_cfgsh_t *sh,
                                     IN tt_u8_t *ev,
                                     IN tt_u32_t ev_num)
{
    return tt_cli_input(&sh->cli, ev, ev_num);
}

tt_inline void tt_cfgsh_read_line(IN tt_cfgsh_t *sh,
                                  IN tt_cli_on_read_t on_read)
{
    tt_cli_read_line(&sh->cli, on_read);
}

#endif /* __TT_CONFIG_SHELL__ */
