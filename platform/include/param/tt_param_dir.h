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
@file tt_param_dir.h
@brief config option of directory type

this file defines config option of directory type
*/

#ifndef __TT_PARAM_DIR__
#define __TT_PARAM_DIR__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <param/tt_param.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct tt_param_dir_s
{
    tt_list_t child;
    tt_u32_t child_name_len;
} tt_param_dir_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_param_t *tt_param_dir_create(IN const tt_char_t *name,
                                          IN OPT tt_param_attr_t *attr);

tt_export tt_result_t tt_param_dir_add(IN tt_param_dir_t *pd,
                                       IN tt_param_t *child);

tt_export void tt_param_dir_remove(IN tt_param_dir_t *pd, IN tt_param_t *child);

tt_export tt_param_t *tt_param_dir_find(IN tt_param_dir_t *pd,
                                        IN const tt_char_t *name,
                                        IN tt_u32_t name_len);

#endif /* __TT_PARAM_DIR__ */
