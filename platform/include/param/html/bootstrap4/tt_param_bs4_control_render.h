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
@file tt_param_bs4control_render.h
@brief parameter bootstrap4 control render

this file includes parameter render definition
*/

#ifndef __TT_PARAM_BS4_CONTROL_RENDER__
#define __TT_PARAM_BS4_CONTROL_RENDER__

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

struct tt_param_bs4content_s;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_result_t
tt_param_bs4ctrl_render(IN struct tt_param_bs4content_s *ct,
                        IN struct tt_param_s *param,
                        IN tt_param_bs4level_t lv,
                        OUT struct tt_buf_s *buf);

tt_export tt_result_t
tt_param_bs4ctrl_render_pair(IN struct tt_param_bs4content_s *ct,
                             IN struct tt_param_s *p1,
                             IN struct tt_param_s *p2,
                             IN tt_param_bs4level_t lv,
                             OUT struct tt_buf_s *buf);

#endif /* __TT_PARAM_BS4_CONTROL_RENDER__ */