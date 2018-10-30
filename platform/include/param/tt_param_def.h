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
@file tt_param_def.h
@brief config object definition

this file includes config object definition
*/

#ifndef __TT_PARAM_DEF__
#define __TT_PARAM_DEF__

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

typedef void (*tt_param_on_destroy_t)(IN struct tt_param_s *p);

typedef tt_result_t (*tt_param_read_t)(IN struct tt_param_s *p,
                                       OUT struct tt_buf_s *output);

typedef tt_result_t (*tt_param_write_t)(IN struct tt_param_s *p,
                                        IN tt_u8_t *val,
                                        IN tt_u32_t val_len);

typedef struct tt_param_itf_s
{
    tt_param_on_destroy_t on_destroy;
    tt_param_read_t read;
    tt_param_write_t write;
} tt_param_itf_t;

typedef enum {
    TT_PARAM_U32,
    TT_PARAM_S32,
    TT_PARAM_DIR,
    TT_PARAM_STRING,
    TT_PARAM_BOOL,
    TT_PARAM_FLOAT,
    TT_PARAM_EXE,

    TT_PARAM_TYPE_NUM
} tt_param_type_t;
#define TT_PARAM_TYPE_VALID(t) ((t) < TT_PARAM_TYPE_NUM)

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif /* __TT_PARAM_DEF__ */
