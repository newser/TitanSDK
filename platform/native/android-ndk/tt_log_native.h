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
@file tt_log_native.h
@brief log native

this file defines log native APIs
*/

#ifndef __TT_LOG_NATIVE__
#define __TT_LOG_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

#include <android/log.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_PRINTF(...)                                                         \
    do {                                                                       \
        __android_log_print(ANDROID_LOG_INFO, "platform", __VA_ARGS__);        \
    } while (0)

// __android_log_print() would automatically add a LF
#define TT_PRINTF_LF(...)

#define TT_VPRINTF(fmt, ap)                                                    \
    do {                                                                       \
        __android_log_vprint(ANDROID_LOG_INFO, "platform", fmt, ap);           \
    } while (0)

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_profile_s;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_log_component_init_ntv(IN struct tt_profile_s *profile);

extern void tt_log_component_exit_ntv();

#endif /* __TT_LOG_NATIVE__ */
