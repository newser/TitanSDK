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
@file tt_param_bs4page.h
@brief parameter bootstrap4 page

this file includes parameter render definition
*/

#ifndef __TT_PARAM_BS4_PAGE__
#define __TT_PARAM_BS4_PAGE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <param/html/bootstrap4/tt_param_bs4_auth.h>
#include <param/html/bootstrap4/tt_param_bs4_content.h>
#include <param/html/bootstrap4/tt_param_bs4_nav.h>
#include <param/html/bootstrap4/tt_param_bs4_sidebar.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct
{
    tt_param_bs4nav_t nav;
    tt_param_bs4sidebar_t sidebar;
    tt_param_bs4content_t content;
    const tt_char_t *lang;
    const tt_char_t *css;
    const tt_char_t *js;
    const tt_char_t *js_extra;
    const tt_char_t *head_extra;
    const tt_char_t *footer_class;
    const tt_char_t *footer_text;
} tt_param_bs4page_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export void tt_param_bs4page_init(IN tt_param_bs4page_t *pg);

tt_export tt_result_t tt_param_bs4page_render(IN tt_param_bs4page_t *pg,
                                              IN OPT struct tt_param_s *root,
                                              IN struct tt_param_s *param,
                                              IN tt_param_bs4level_t lv,
                                              OUT struct tt_buf_s *buf);

#endif /* __TT_PARAM_BS4_PAGE__ */
