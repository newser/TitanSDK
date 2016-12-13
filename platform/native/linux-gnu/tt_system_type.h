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
@file tt_system_type.h
@brief system specific type

this file defines system specific type.

<hr>

<b>API BASIC PRINCIPLE</b><br>
- init function use "init_param", create function use "attr", other function
  use "option" as the name of parameter
- call back functions begin with "on_"
- the exclusive operation ends with create/destroy/acquire/release

<hr>

<b>OS WRAPPER API PRINCIPLE</b><br>
- wrapped API should have return value indicating whether system call
  is executed successfully and the return value should be checked by caller
- wrapper should be implemented as a function but not a macro
- wrapper should do param check, so portlayer api need not check param

<hr>

<b>OS PORT LAYER API PRINCIPLE</b><br>
- port layer API should have return value indicating whether system call
  is executed successfully and the return value should be checked by caller
- port layer should be implemented as a function but not a macro
- port layer API should not reference APIs in other port layer files, but
  if that is mandatory, just follow these principles, the only disadvantage
  is it may enlarge code size for the inline functions are compiled again
- port layer APIs are implemented in header file as inline function, the
  reason is that these port layer APIs should only be called by wrapper APIs
- all port layer API implementations can only be exposed when some protective
  macro is defined before including the header file. while typedef or macros
  are not protected by macro. for example, "TT_MUTEX_PORTLAYER_API_EXPOSE"
  should be defined before "#include <tt_mutex_native.h>" when the APIs
  defined in that header file are needed
*/

#if !defined(__linux__) && !defined(__linux)
#error "should only used by linux gcc"
#endif

#ifndef __TT_SYSTEM_TYPE__
#define __TT_SYSTEM_TYPE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <config/tt_environment_config.h>

#include <stddef.h>
#include <stdint.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

/**
@typedef tt_u8_t
1 byte type, unsigned
*/
typedef unsigned char tt_u8_t;
/**
@typedef tt_s8_t
1 byte type, signed
*/
typedef char tt_s8_t;
/**
@typedef tt_u16_t
2 byte type, unsigned
*/
typedef unsigned short tt_u16_t;
/**
@typedef tt_s16_t
2 byte type, signed
*/
typedef short tt_s16_t;
/**
@typedef tt_u32_t
4 byte type, unsigned
*/
typedef unsigned int tt_u32_t;
/**
@typedef tt_s32_t
4 byte type, signed
*/
typedef int tt_s32_t;

#if TT_ENV_IS_64BIT

/**
@typedef tt_u64_t
8 byte type, unsigned
*/
typedef unsigned long tt_u64_t;

/**
@typedef tt_s64_t
8 byte type, signed
*/
typedef long tt_s64_t;

#else

/**
@typedef tt_u64_t
8 byte type, unsigned
*/
typedef unsigned long long tt_u64_t;

/**
@typedef tt_s64_t
8 byte type, signed
*/
typedef long long tt_s64_t;

#endif

/**
@typedef tt_char_t
character type
*/
typedef char tt_char_t;

/**
@typedef tt_ptr_t
pointer type for address calculation
*/
typedef void *tt_ptr_t;

/**
@typedef tt_uintptr_t
pointer type for numeric calculation such as changing pointer value
*/
typedef uintptr_t tt_uintptr_t;

typedef intptr_t tt_intptr_t;

/**
@typedef tt_ptrdiff_t
size to store pointer distance
*/
typedef ptrdiff_t tt_ptrdiff_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif /* __TT_SYSTEM_TYPE__ */
