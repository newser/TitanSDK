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
@file tt_basic_type.h
@brief all basic type definitions

this file define all basic types

<hr>

<b>BASIC PRINCIPLE</b><br>
- each type is wrapped, so that they are under control
- type differences between 32bit and 64bit platform should be consider,
  while it only solve compile issues, dynamical checking platform bits
  is cared elsewhere

<hr>

<b>NAMING CONVENTION</b><br>
- function name is like "tt_<obj name>_<action>_<param form>", "obj name" can
  be 1-2 words, "action" can be 1-2 words, "param form" is only 1 word
- function to initialize some module should end with "_system_init"
- structure which only integrate some function pointers is always named
  as "tt_xxx_itf_t" (some other software may use "xxx_op_t")
- the structure which can be created and destroyed has functions named as
  "create" and "destroy", for example, a mutex can be created and must be
  destroyed when it's not needed to release resource. those which no need
  to release resource would have only one function named "init", such as
  a list node
- the parameters of a function generally includes: this pointer, mandatory
  parameters, and finally a attribute structure includes all optional
  parameters
- common memory alloc function return void*, which also indicate allocation
  result. common free function return void, but give a error trace when
  necessary
*/

#ifndef __TT_BASIC_TYPE__
#define __TT_BASIC_TYPE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <config/tt_platform_config.h>
#include <misc/tt_error_def.h>

#include <tt_intrinsic.h>
#include <tt_system_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

/**
@def TT_TRUE
value "true" of bool type
*/
#define TT_TRUE (1)
/**
@def TT_FALSE
value "false" of bool type
*/
#define TT_FALSE (0)

/**
@def TT_TIME_INFINITE
indicate infinite time length
*/
//#define TT_TIME_INFINITE (~0)
#define TT_TIME_INFINITE (0x7FFFFFFFFFFFFFFF)

// ========================================
// helper macro definition
// ========================================

/**
@def IN
indicate such param is an input param
*/
#define IN
/**
@def OUT
indicate such param is an output param
*/
#define OUT
/**
@def OPT
indicate such param is an optional param
*/
#define OPT

/**
@def TO
indicate ownership of such param is passed in, the receiver should release
it. it implies IN
*/
#define TO
/**
@def FROM
indicate ownership of such param is passed out, the receiver should release
it. it implies OUT
*/
#define FROM

/**
@def __TT_MANDATORY__
this macro is to be put into a struct indicating members below must be
filled by application
*/
#define __TT_MANDATORY__
/**
@def TT_OPTIONAL
this macro is to be put into a struct indicating members below has default
value but can also be modified by application
*/
#define TT_OPTIONAL

#define __TT_PUBLIC__

/**
@def __TT_PRIVATE__
this macro is to be put into a struct indicating members below should never
be modified by application
*/
#define __TT_PRIVATE__

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

/**
@typedef tt_bool_t
bool type, can only be TT_TRUE or TT_FALSE
*/
typedef tt_u8_t tt_bool_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif /* __TT_BASIC_TYPE__ */
