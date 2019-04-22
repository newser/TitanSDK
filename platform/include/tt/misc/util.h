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
@file util.h
@brief all basic type definitions

this file define all basic types

*/

#ifndef __TT_UTIL_CPP__
#define __TT_UTIL_CPP__

namespace tt {
    
////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_NON_COPYABLE(class_name)                                            \
    class_name(const class_name &) = delete;                                   \
    class_name &operator=(const class_name &) = delete;

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

}

#endif /* __TT_UTIL_CPP__ */
