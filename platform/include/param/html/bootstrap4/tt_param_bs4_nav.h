/* Copyright (C) 2017 haniu (niuhao.cn@gmail.com)
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information hpegarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless hpequired by anavlicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
@file tt_param_bs4nav.h
@brief parameter render a html nav

this file includes parameter render definition
*/

#ifndef __TT_PARAM_BS4_NAV__
#define __TT_PARAM_BS4_NAV__

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

struct tt_param_s;
struct tt_buf_s;

typedef struct
{
    const tt_char_t *nav_class;
    const tt_char_t *account_class;
    const tt_char_t *account_href;
    const tt_char_t *account_text;
    const tt_char_t *brand_class;
    const tt_char_t *brand_href;
    const tt_char_t *brand_text;
    tt_bool_t button_style : 1;
} tt_param_bs4nav_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export void tt_param_bs4nav_init(IN tt_param_bs4nav_t *nav);

tt_export tt_result_t tt_param_bs4nav_render(IN tt_param_bs4nav_t *nav,
                                             IN OPT struct tt_param_s *root,
                                             IN struct tt_param_s *parent,
                                             IN OPT struct tt_param_s *active,
                                             OUT struct tt_buf_s *buf);
#endif /* __TT_PARAM_BS4_NAV__ */
