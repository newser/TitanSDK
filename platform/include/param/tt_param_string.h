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
@file tt_param_string.h
@brief config option of string type

this file defines config option of string type
*/

#ifndef __TT_PARAM_STRING__
#define __TT_PARAM_STRING__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_string.h>
#include <param/tt_param.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef tt_result_t (*tt_param_str_on_set_t)(IN struct tt_param_s *co,
                                             IN tt_string_t *new_val);

typedef struct tt_param_str_cb_s
{
    tt_param_str_on_set_t on_set;
} tt_param_str_cb_t;

typedef struct tt_param_str_s
{
    tt_param_str_cb_t cb;
    tt_string_t str;
} tt_param_str_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_param_t *tt_param_str_create(IN const tt_char_t *name,
                                          IN tt_string_t *p_str,
                                          IN OPT tt_param_attr_t *attr,
                                          IN OPT tt_param_str_cb_t *cb);

#endif /* __TT_PARAM_STRING__ */
