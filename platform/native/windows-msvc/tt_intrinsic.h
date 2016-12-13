/* Licensed to the Apache Software Foundation (ASF) under one or more
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
@file tt_intrinsic.h
@brief specific compiler symbols

this file defines compiler symbols
*/

#ifndef __TT_INTRINSIC__
#define __TT_INTRINSIC__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_LIKELY
#define TT_UNLIKELY

// static library

/**
@def TT_INLINE
a function with such prefix would be expanded as inline

@note
- this macro is compiler specific, different compilers would have different
  definitions
*/
#define tt_inline static __inline

// dynamic library
#ifdef TT_PLATFORM_DYNAMIC_LIB_BUILDING
#define TT_EXPORT __declspec(dllexport) extern
#else
#define TT_EXPORT __declspec(dllimport) extern
#endif

#define TT_RESTRICT restrict

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif /* __TT_INTRINSIC__ */
