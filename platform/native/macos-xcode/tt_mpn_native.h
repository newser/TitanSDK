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
@file tt_mpn_native.h
@brief multi precision integer native

this file defines multi precision integer native utils
*/

#ifndef __TT_MPN_NATIVE__
#define __TT_MPN_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#if TT_ENV_IS_64BIT
#define __MPN_MUL(a, b, hret, lret)                                            \
    do {                                                                       \
        unsigned __int128 ret =                                                \
            ((unsigned __int128)(a)) * ((unsigned __int128)(b));               \
        (hret) = ret >> 64;                                                    \
        (lret) = ret & 0xFFFFFFFFFFFFFFFF;                                     \
    } while (0)
#else
#define __MPN_MUL(a, b, hret, lret)                                            \
    do {                                                                       \
        uint64_t ret = ((uint64_t)(a)) * ((uint64_t)(b));                      \
        (hret) = ret >> 32;                                                    \
        (lret) = ret & 0xFFFFFFFF;                                             \
    } while (0)
#endif

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif /* __TT_MPN_NATIVE__ */
