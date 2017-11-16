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
@file tt_reference_counter.h
@brief reference counter

this file defines reference counter utilities
*/

#ifndef __TT_REFERENCE_COUNTER__
#define __TT_REFERENCE_COUNTER__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <config/tt_log_config.h>
#include <log/tt_log.h>
#include <os/tt_atomic.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

// object with reference counter:
//  - create --> kill
//  - create --> run --> destroy (--> __destroy)

// create:
//  - only create, do not start members that would reference the created
//    object, a created object can be killed
//  - steps:
//     - create m1, m2, ..., add ref, return succes
//     - if failed: kill ..., m2, m2, return fail

// kill:
//  - called when members that would reference the object are not started yet.
//    the killing process is synchronous
//  - steps:
//     - kill m1, m2, ...

// run:
//  - start running members that would reference the object, a running oject
//    can only be destroyed, result of killing a running object is unspecified
//  - generally result of running members are ignored, assuming all calls to
//    start running succeed. the "start running" operation are generally to
//    start
//    some timers or start socket connecting which rarely fail(here it's
//    referring
//    the result of "start connecting" operation not the result of "connecting"
//    operation) in practice.
//  - steps:
//     - run m1, m2, ...

// destroy
//  - destroying a running object, the object would be destroyed when all
//  members
//    are destroyed, so the process is asynchronous
//  - steps:
//     - destroy m1, m2, ...
//     - dec ref

#ifdef TT_SHOW_REF_COUNTER

#define TT_REF_ADD(__var_type, __var_ptr, __ref_name)                          \
    do {                                                                       \
        tt_s32_t __nr = tt_atomic_s32_inc(&(__var_ptr)->__ref_name);           \
        TT_INFO("%s[%p] ref add: [%x]", #__var_type, __var_ptr, __nr);         \
    } while (0)
#define TT_REF_RELEASE(__var_type, __var_ptr, __ref_name, __destroy_api)       \
    do {                                                                       \
        tt_s32_t __nr = tt_atomic_s32_dec(&(__var_ptr)->__ref_name);           \
        TT_INFO("%s[%p] ref release[%x]", #__var_type, __var_ptr, __nr);       \
        if (__nr == 0) {                                                       \
            __destroy_api(__var_ptr);                                          \
        } else if (__nr < 0) {                                                 \
            TT_ERROR("redundant ref release");                                 \
        }                                                                      \
    } while (0)

#else

#define TT_REF_ADD(__var_type, __var_ptr, __ref_name)                          \
    do {                                                                       \
        tt_atomic_s32_inc(&(__var_ptr)->__ref_name);                           \
    } while (0)
#define TT_REF_RELEASE(__var_type, __var_ptr, __ref_name, __destroy_api)       \
    do {                                                                       \
        if (tt_atomic_s32_dec(&(__var_ptr)->__ref_name) == 0) {                \
            __destroy_api(__var_ptr);                                          \
        }                                                                      \
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

#endif /* __TT_REFERENCE_COUNTER__ */
