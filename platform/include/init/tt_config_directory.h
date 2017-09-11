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
@file tt_config_directory.h
@brief config option of directory type

this file defines config option of directory type
*/

#ifndef __TT_CONFIG_DIRECTORY__
#define __TT_CONFIG_DIRECTORY__

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

typedef struct tt_cfgdir_s
{
    tt_list_t child;
    tt_u32_t child_name_len;
} tt_cfgdir_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_cfgobj_t *tt_cfgdir_create(IN const tt_char_t *name,
                                        IN OPT tt_cfgobj_attr_t *attr);

tt_export tt_result_t tt_cfgdir_add(IN tt_cfgdir_t *cd, IN tt_cfgobj_t *child);

tt_export void tt_cfgdir_remove(IN tt_cfgdir_t *cd, IN tt_cfgobj_t *child);

tt_export tt_cfgobj_t *tt_cfgdir_find(IN tt_cfgdir_t *cd,
                                      IN const tt_char_t *name,
                                      IN tt_u32_t name_len);

tt_export tt_result_t tt_cfgdir_ls(IN tt_cfgdir_t *c,
                                   IN const tt_char_t *col_sep,
                                   IN const tt_char_t *line_sep,
                                   OUT struct tt_buf_s *output);

#endif /* __TT_CONFIG_DIRECTORY__ */
