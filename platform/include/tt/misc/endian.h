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
@file endian.h
@brief all basic type definitions

this file define all basic types

*/

#ifndef __TT_ENDIAN_CPP__
#define __TT_ENDIAN_CPP__

#include <tt/misc/util.h>

#include <endian_native.h>

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

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

#if defined(TT_BIG_ENDIAN)

template<typename T TT_ENBALE_IF(std::is_integral<T>::value)>
T n2h(T val)
{
    return val;
}

template<typename T TT_ENBALE_IF(std::is_integral<T>::value)>
T h2n(T val)
{
    return val;
}

#elif defined(TT_LITTLE_ENDIAN)

#define __CAN_N2H(t)                                                           \
    std::is_integral<T>::value &&                                              \
        (sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8)

template<typename T TT_ENABLE_IF(__CAN_N2H(T))>
T n2h(T val)
{
    return native::bswap(val);
}

template<typename T TT_ENABLE_IF(__CAN_N2H(T))>
T h2n(T val)
{
    return native::bswap(val);
}

#else

#error unknown endian!

#endif

}

#endif /* __TT_ENDIAN_CPP__ */
