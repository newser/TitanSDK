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
@file tt_param_bs4_control.h
@brief parameter bootstrap4 control

this file includes parameter bootstrap4 control
*/

#ifndef __TT_PARAM_BS4_CONTROL__
#define __TT_PARAM_BS4_CONTROL__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef enum {
    TT_PARAM_BS4_INPUT,
    TT_PARAM_BS4_SELECT,
    TT_PARAM_BS4_TEXTAREA,

    TT_PARAM_BS4_TYPE_NUM
} tt_param_bs4_type_t;
#define TT_PARAM_BS4_TYPE_VALID(c) ((c) < TT_PARAM_BS4_TYPE_NUM)

typedef struct
{
    const tt_char_t *pattern;
    tt_char_t min[13];
    tt_char_t max[13];
    tt_char_t minlen[7];
    tt_char_t maxlen[7];
    tt_char_t step[8];
} tt_param_bs4_input_t;

typedef struct
{
    const tt_char_t *selected;
    const tt_char_t **option;
    tt_u8_t option_num;
    tt_bool_t multiple : 1;
} tt_param_bs4_select_t;

typedef struct
{
    tt_char_t rows[4];
} tt_param_bs4_textarea_t;

typedef struct
{
    tt_param_bs4_type_t type;
    union
    {
        tt_param_bs4_input_t input;
        tt_param_bs4_select_t select;
        tt_param_bs4_textarea_t textarea;
    };
    tt_bool_t whole_line : 1;
} tt_param_bs4_ctrl_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export void tt_param_bs4_ctrl_init(IN tt_param_bs4_ctrl_t *ctrl);

tt_export void tt_param_bs4_ctrl_set_type(IN tt_param_bs4_ctrl_t *ctrl,
                                          IN tt_param_bs4_type_t type);

// ========================================
// param bs4 control: input
// ========================================

tt_export void tt_param_bs4_input_init(IN tt_param_bs4_input_t *i);

tt_export void tt_param_bs4_input_set_pattern(IN tt_param_bs4_input_t *i,
                                              IN const tt_char_t *pattern);

tt_export void tt_param_bs4_input_clear_pattern(IN tt_param_bs4_input_t *i);

tt_export void tt_param_bs4_input_set_min(IN tt_param_bs4_input_t *i,
                                          IN tt_s32_t len);

tt_export void tt_param_bs4_input_clear_min(IN tt_param_bs4_input_t *i);

tt_export void tt_param_bs4_input_set_max(IN tt_param_bs4_input_t *i,
                                          IN tt_s32_t len);

tt_export void tt_param_bs4_input_clear_max(IN tt_param_bs4_input_t *i);

tt_export void tt_param_bs4_input_set_minlen(IN tt_param_bs4_input_t *i,
                                             IN tt_u32_t len);

tt_export void tt_param_bs4_input_clear_minlen(IN tt_param_bs4_input_t *i);

tt_export void tt_param_bs4_input_set_maxlen(IN tt_param_bs4_input_t *i,
                                             IN tt_u32_t len);

tt_export void tt_param_bs4_input_clear_maxlen(IN tt_param_bs4_input_t *i);

tt_export void tt_param_bs4_input_set_step(IN tt_param_bs4_input_t *i,
                                           IN tt_u8_t step);

tt_export void tt_param_bs4_input_clear_step(IN tt_param_bs4_input_t *i);

tt_export void tt_param_bs4_input_clear(IN tt_param_bs4_input_t *i);

// ========================================
// param bs4 control: select
// ========================================

tt_export void tt_param_bs4_select_init(IN tt_param_bs4_select_t *s);

tt_export void tt_param_bs4_select_set_selected(IN tt_param_bs4_select_t *s,
                                                IN const tt_char_t *selected);

tt_export void tt_param_bs4_select_set_option(IN tt_param_bs4_select_t *s,
                                              IN const tt_char_t **option,
                                              IN tt_u8_t num);

tt_export void tt_param_bs4_select_set_multiple(IN tt_param_bs4_select_t *s,
                                                IN tt_bool_t multiple);

tt_export void tt_param_bs4_select_clear(IN tt_param_bs4_select_t *s);

// ========================================
// param bs4 control: textarea
// ========================================

tt_export void tt_param_bs4_textarea_set_rows(IN tt_param_bs4_textarea_t *t,
                                              IN tt_u8_t rows);

#endif /* __TT_PARAM_BS4_CONTROL__ */
