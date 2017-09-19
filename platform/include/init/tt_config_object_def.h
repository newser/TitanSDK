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
@file tt_config_object_def.h
@brief config object definition

this file includes config object definition
*/

#ifndef __TT_CONFIG_OBJECT_DEF__
#define __TT_CONFIG_OBJECT_DEF__

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

struct tt_cfgobj_s;
struct tt_buf_s;

typedef void (*tt_cfgobj_on_destroy_t)(IN struct tt_cfgobj_s *co);

typedef tt_result_t (*tt_cfgobj_read_t)(IN struct tt_cfgobj_s *co,
                                        IN const tt_char_t *line_sep,
                                        OUT struct tt_buf_s *output);

typedef tt_result_t (*tt_cfgobj_write_t)(IN struct tt_cfgobj_s *co,
                                         IN tt_u8_t *val,
                                         IN tt_u32_t val_len);

typedef struct tt_cfgobj_itf_s
{
    tt_cfgobj_on_destroy_t on_destroy;
    tt_cfgobj_read_t read;
    tt_cfgobj_write_t write;
} tt_cfgobj_itf_t;

typedef enum {
    TT_CFGOBJ_U32,
    TT_CFGOBJ_S32,
    TT_CFGOBJ_DIR,
    TT_CFGOBJ_STRING,
    TT_CFGOBJ_BOOL,
    TT_CFGOBJ_FLOAT,
    TT_CFGOBJ_EXE,

    TT_CFGOBJ_TYPE_NUM
} tt_cfgobj_type_t;
#define TT_CFGOBJ_TYPE_VALID(t) ((t) < TT_CFGOBJ_TYPE_NUM)

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif /* __TT_CONFIG_OBJECT_DEF__ */
