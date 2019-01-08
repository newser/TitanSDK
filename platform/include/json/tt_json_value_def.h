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
@file tt_json_value_def.h
@brief json val def

this file specifies json val
*/

#ifndef __TT_JSON_VALUE_DEF__
#define __TT_JSON_VALUE_DEF__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

// refer GenericValue::Data
#define TT_JVAL_SIZE 24

#define DEF_JVAL_TYPE(t)                                                       \
    typedef struct                                                             \
    {                                                                          \
        tt_u8_t reserved[TT_JVAL_SIZE];                                        \
        tt_ptr_t extra;                                                        \
    } t

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

DEF_JVAL_TYPE(tt_jval_t);

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif /* __TT_JSON_VALUE_DEF__ */
