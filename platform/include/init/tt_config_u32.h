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
@file tt_config_u32.h
@brief config option of u32 type

this file defines config option of u32 type
*/

#ifndef __TT_CONFIG_U32__
#define __TT_CONFIG_U32__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <init/tt_config_object.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_cfgu32_s;

typedef tt_result_t (*tt_cfgu32_on_set_t)(IN struct tt_cfgobj_s *cnode,
                                          IN tt_u32_t new_val);

typedef struct tt_cfgu32_cb_s
{
    tt_cfgu32_on_set_t on_set;
} tt_cfgu32_cb_t;

typedef struct tt_cfgu32_s
{
    tt_cfgu32_cb_t cb;
} tt_cfgu32_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_cfgobj_t *tt_cfgu32_create(IN const tt_char_t *name,
                                        IN tt_u32_t *p_u32,
                                        IN OPT tt_cfgobj_attr_t *attr,
                                        IN OPT tt_cfgu32_cb_t *cb);

#endif /* __TT_CONFIG_U32__ */
