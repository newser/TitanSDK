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
@file tt_config_exe.h
@brief config option of executable type

this file defines config option of executable type
*/

#ifndef __TT_CONFIG_EXE__
#define __TT_CONFIG_EXE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <init/tt_config_object.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef tt_result_t (*tt_cfgexe_run_t)(IN struct tt_cfgobj_s *co,
                                       IN tt_u32_t argc,
                                       IN tt_char_t *argv[],
                                       IN const tt_char_t *line_sep,
                                       OUT struct tt_buf_s *output,
                                       OUT tt_u32_t *status);

typedef struct
{
    tt_cfgexe_run_t run;
} tt_cfgexe_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_cfgobj_t *tt_cfgexe_create(IN const tt_char_t *name,
                                        IN OPT tt_cfgobj_attr_t *attr,
                                        IN tt_cfgexe_run_t run);

tt_inline tt_result_t tt_cfgexe_run(IN tt_cfgexe_t *ce,
                                    IN tt_u32_t argc,
                                    IN tt_char_t *argv[],
                                    IN const tt_char_t *line_sep,
                                    OUT struct tt_buf_s *output,
                                    OUT tt_u32_t *status)
{
    return ce->run(TT_CFGOBJ_OF(ce), argc, argv, line_sep, output, status);
}

#endif /* __TT_CONFIG_EXE__ */
