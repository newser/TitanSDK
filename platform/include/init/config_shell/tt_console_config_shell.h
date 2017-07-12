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
@file tt_console_config_shell.h
@brief console config shell

this file defines console config shell
*/

#ifndef __TT_CONSOLE_CONFIG_SHELL__
#define __TT_CONSOLE_CONFIG_SHELL__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <init/config_shell/tt_config_shell.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_console_cfgsh_create(IN tt_cfgsh_t *sh,
                                           IN tt_cli_mode_t mode,
                                           IN OPT tt_cfgsh_attr_t *attr);

extern tt_result_t tt_console_cfgsh_run(IN tt_cfgsh_t *sh, IN tt_bool_t local);

#endif /* __TT_CONSOLE_CONFIG_SHELL__ */
