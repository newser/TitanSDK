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
@brief string parameter

this file defines string parameter
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

typedef struct tt_param_str_cb_s
{
    tt_bool_t (*pre_set)(IN struct tt_param_s *p,
                         IN tt_char_t *new_val,
                         IN tt_u32_t len);
    void (*post_set)(IN struct tt_param_s *p,
                     IN tt_char_t *new_val,
                     IN tt_u32_t len);
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
                                          IN tt_string_t *p_val,
                                          IN OPT tt_param_attr_t *attr,
                                          IN OPT tt_param_str_cb_t *cb);

tt_export const tt_char_t *tt_param_get_str(IN tt_param_t *p,
                                            OUT tt_string_t *val);

tt_export tt_result_t tt_param_set_str_n(IN tt_param_t *p,
                                         IN const tt_char_t *val,
                                         IN tt_u32_t len);

tt_inline tt_result_t tt_param_set_str(IN tt_param_t *p,
                                       IN const tt_char_t *val)
{
    return tt_param_set_str_n(p, val, (tt_u32_t)tt_strlen(val));
}

#endif /* __TT_PARAM_STRING__ */
