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
@file tt_process.h
@brief process native

this file defines process native APIs
*/

#ifndef __TT_PROCESS_NATIVE__
#define __TT_PROCESS_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_PROCESS_EXIT_SUCCESS_NTV EXIT_SUCCESS
#define TT_PROCESS_EXIT_FAILURE_NTV EXIT_FAILURE

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_process_attr_s;

typedef struct
{
    PROCESS_INFORMATION proc_info;
} tt_process_ntv_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

// the final argv must be NULL
extern tt_result_t tt_process_create_ntv(IN tt_process_ntv_t *sys_proc,
                                         IN const tt_char_t *path,
                                         IN OPT tt_char_t *const argv[],
                                         IN OPT struct tt_process_attr_s *attr);

extern tt_result_t tt_process_wait_ntv(IN tt_process_ntv_t *sys_proc,
                                       IN tt_bool_t block,
                                       IN OPT tt_u8_t *exit_code);

extern void tt_process_exit_ntv(IN tt_u8_t exit_code);

extern tt_char_t *tt_process_path_ntv(IN OPT tt_process_ntv_t *sys_proc);

extern tt_char_t *tt_current_path_ntv(IN tt_bool_t end_slash);

extern const tt_char_t *tt_process_name_ntv();

#endif /* __TT_PROCESS_NATIVE__ */
