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
@file tt_param_bs4_content.h
@brief parameter bootstrap4 content

this file includes parameter render definition
*/

#ifndef __TT_PARAM_BS4_CONTENT__
#define __TT_PARAM_BS4_CONTENT__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <param/html/bootstrap4/tt_param_bs4_auth.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_param_s;
struct tt_buf_s;

typedef struct tt_param_bs4_content_s
{
    const tt_char_t *group_class;
    const tt_char_t *group0_class;
    const tt_char_t *group1_class;
    const tt_char_t *title_class;
    const tt_char_t *name_class;
    const tt_char_t *val_class;
    const tt_char_t *btn_class;
} tt_param_bs4_content_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export void tt_param_bs4_content_init(IN tt_param_bs4_content_t *ct);

tt_export tt_result_t tt_param_bs4_content_render(IN tt_param_bs4_content_t *ct,
                                                  IN struct tt_param_s *param,
                                                  IN tt_param_bs4_level_t lv,
                                                  OUT struct tt_buf_s *buf);

#endif /* __TT_PARAM_BS4_CONTENT__ */
