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
@file tt_assert.h
@brief assert function

this file implement assert function.

<hr>

<b>ABSTRACE DESIGN</b><br>
assert API can be expanded as:
- empty code
- assertion

<hr>

<b>USAGE</b><br>
there are several types of assertions:
- TT_ASSERT() throw an assertion using system assert, but sometimes system
  assert is actually macro and can be empty then TT_ASSERT() would just
  give a error print
- TT_ASSERT_ALWAYS() would always throw exception

when to use TT_ASSERT()
- if you are sure the expression can be satisfied when system is working well
- you can put such macro to help trace error during debugging, and after that
  these macros can be expanded as empty code by configuration to save the
  checking time
- an example is null pointer, a pointer should not be null if all are correct.
  but during debugging, this error may occur and thus this macro is used.
  when all tests are done, expand this macro to be empty so that the pointer
  would not be chekced which is ever verified by ever tests
- an opposite example is using this macro to check return value of some API
  from external libraries of system api, this does not fit for that return
  value is out of control, ts itself can not gurantee that API always run well

when to use TT_ASSERT_ALWAYS()
- this macro use some assertion function which does not come from system
  implementation, so this would alway raise excetpion when checking does not
  pass
- when the checking can not pass, the system can not continue running
- examples can be: you find a wild pointer(assuming you can), stack overflows
  or checking result returned by non-ts applications
- if a condition is recoverable, for example, checking parameter passed by
  external application, "if{}" should be used instead

*/

#ifndef __TT_ASSERT__
#define __TT_ASSERT__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <config/tt_platform_config.h>
#include <log/tt_log.h>

#include <tt_assert_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

/**
@def TT_ASSERT_ALWAYS(e)
if e is not satisfied, always raise an exception
*/
#define TT_ASSERT_ALWAYS(e)                                                    \
    do {                                                                       \
        if (TT_UNLIKELY(!(e))) {                                               \
            TT_ERROR("false: (%s)", #e);                                       \
            tt_throw_exception_ntv(NULL);                                      \
        }                                                                      \
    } while (0)

#ifdef TT_ASSERT_ENABLE

/**
@def TT_ASSERT(e)
if e is not satisfied, throw an exception
*/
#define TT_ASSERT(e) TT_ASSERT_ALWAYS(e)

#else /* TT_ASSERT_ENABLE */

#define TT_ASSERT(e)

#endif /* TT_ASSERT_ENABLE */

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif /* __TT_ASSERT__ */
