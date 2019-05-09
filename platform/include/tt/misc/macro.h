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
@file macro.h
@brief all basic type definitions

this file define all basic types

*/

#ifndef __TT_MISC_MACRO_CPP__
#define __TT_MISC_MACRO_CPP__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <stdexcept>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_ENABLE_IF(e) , typename std::enable_if<e, int>::type = 0

#define TT_NO_COPY_CTOR(class_name) class_name(const class_name &) = delete;

#define TT_NO_OP_EQ(class_name)                                                \
    class_name &operator=(const class_name &) = delete;

#define TT_NON_COPYABLE(class_name)                                            \
    TT_NO_COPY_CTOR(class_name)                                                \
    TT_NO_OP_EQ(class_name)

// ========================================
// throw
// ========================================

#define TT_EXCEPTION_IF_0(etype, e)                                            \
    do {                                                                       \
        if (e) { throw etype(); }                                              \
    } while (0)

#define TT_EXCEPTION_IF_1(etype, e, info)                                      \
    do {                                                                       \
        if (e) { throw etype(info); }                                          \
    } while (0)

#define TT_INVALID_ARG_IF(e, info)                                             \
    TT_EXCEPTION_IF_1(std::invalid_argument, e, info)

#define TT_OVERFLOW_IF(e, info) TT_EXCEPTION_IF_1(std::overflow_error, e, info)

#define TT_BAD_CALL_IF(e) TT_EXCEPTION_IF_0(std::bad_function_call, e)

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

namespace tt {

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

}

#endif /* __TT_MISC_MACRO_CPP__ */
