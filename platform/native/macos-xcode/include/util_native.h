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
@file util_native.h
@brief all basic type definitions

this file define all basic types

*/

#ifndef __TT_UTIL_NATIVE_CPP__
#define __TT_UTIL_NATIVE_CPP__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

namespace tt {

namespace native {

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

template<typename T>
T bswap(T val)
{
    static_assert((sizeof(T) == 1) || (sizeof(T) == 2) || (sizeof(T) == 4) ||
                  (sizeof(T) == 8));
    if constexpr (sizeof(T) == 1) {
        return val;
    } else if constexpr (sizeof(T) == 2) {
        return __builtin_bswap16(val);
    } else if constexpr (sizeof(T) == 4) {
        return __builtin_bswap32(val);
    } else {
        return __builtin_bswap64(val);
    }
}

}

}

#endif /* __TT_UTIL_NATIVE_CPP__ */
