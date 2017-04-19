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
@file tt_process.h
@brief process

this file defines process APIs
*/

#ifndef __TT_PROCESS__
#define __TT_PROCESS__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_process_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_PROCESS_EXIT_SUCCESS TT_PROCESS_EXIT_SUCCESS_NTV
#define TT_PROCESS_EXIT_FAILURE TT_PROCESS_EXIT_FAILURE_NTV
#define TT_PROCESS_EXIT_UNKNOWN 0xFF

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct tt_process_attr_s
{
    tt_u32_t reserved;
} tt_process_attr_t;

typedef struct
{
    tt_process_ntv_t sys_proc;
} tt_process_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern void tt_process_component_register();

// - argv[0] should be the file name
//   the final argv must be NULL
// - the created process must be released use wait() whenever
//   this function returned success
extern tt_result_t tt_process_create(IN tt_process_t *proc,
                                     IN const tt_char_t *path,
                                     IN OPT tt_char_t *const arg[],
                                     IN OPT tt_process_attr_t *attr);

// - exit_code is set only when this function return success
// - exit_code is set to TT_PROCESS_EXIT_UNKNOWN, when created process is
//   terminated abnormally so that tt_process_exit() is not called
extern tt_result_t tt_process_wait(IN tt_process_t *proc,
                                   IN tt_bool_t block,
                                   IN OPT tt_u8_t *exit_code);

extern void tt_process_attr_default(IN tt_process_attr_t *attr);

extern void tt_process_exit(IN tt_u8_t exit_code);

extern tt_char_t *tt_process_path(IN OPT tt_process_t *proc);

#endif /* __TT_PROCESS__ */