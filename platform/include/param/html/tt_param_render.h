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
@file tt_param_render.h
@brief parameter render

this file includes parameter render definition
*/

#ifndef __TT_PARAM_RENDER__
#define __TT_PARAM_RENDER__

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

typedef tt_result_t (*tt_param_render_run_t)(IN struct tt_param_s *root,
                                             IN void *ctx,
                                             OUT struct tt_buf_s *buf);

typedef struct
{
    tt_param_render_run_t header;
    tt_param_render_run_t main;
    tt_param_render_run_t footer;
} tt_param_render_itf_t;

typedef struct
{
    tt_param_render_itf_t *itf;
} tt_param_render_t;

typedef struct
{
    tt_u32_t reserved;
} tt_param_render_attr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export void tt_param_render_init(IN tt_param_render_t *r,
                                    IN tt_param_render_itf_t *itf,
                                    OPT tt_param_render_attr_t *attr);

tt_export void tt_param_render_destroy(IN tt_param_render_t *r);

tt_export void tt_param_render_attr_default(IN tt_param_render_attr_t *attr);

#endif /* __TT_PARAM_RENDER__ */
