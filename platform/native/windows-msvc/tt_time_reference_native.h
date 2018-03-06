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
@file tt_time_reference_native.h
@brief system time reference APIs

this file specifies system interfaces for time reference
*/

#ifndef __TT_TIME_REFERENCE_NATIVE__
#define __TT_TIME_REFERENCE_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

//#define __TIME_WRAP_AROUND_PROTECT

/**
@def tt_time_ref2ms_ntv(ref, ctx)
convert time reference to millisecond
*/
#define tt_time_ref2ms_ntv(ref) (ref)
/**
@def tt_time_ms2ref_ntv(ms, ctx)
convert millisecond to time reference
*/
#define tt_time_ms2ref_ntv(ms) (ms)

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

/**
@struct tt_time_counter_t
reference time context
*/
typedef struct
{
    __declspec(align(4)) LONG lock;
    union
    {
        tt_u64_t v64;
        tt_u32_t v32[2];
#if TT_ENV_IS_BIG_ENDIAN == 1
#define __HIGH 0
#define __LOW 1
#else
#define __HIGH 1
#define __LOW 0
#endif
    };
} tt_time_counter_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

extern tt_time_counter_t tt_g_time_counter;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/**
@fn tt_result_t tt_time_ref_component_init_ntv()
initialize ts time reference system

@return
- TT_SUCCESS if initialization succeeds
- TT_FAIL otherwise
*/
extern tt_result_t tt_time_ref_component_init_ntv();

tt_inline void tt_time_ref_component_exit_ntv()
{
}

tt_inline tt_s64_t tt_time_ref_ntv()
{
#ifdef __TIME_WRAP_AROUND_PROTECT
    DWORD now = timeGetTime();
    // - GetTickCount() is of less accuracy(10ms - 15ms)
    // - QueryPerformanceCounter() performances varies on diff platforms
    // - timeGetTime() is monotonical

    while (InterlockedCompareExchange(&tt_g_time_counter.lock, 1, 0) != 0)
        ;

    if (now < tt_g_time_counter.v32[__LOW]) {
        tt_g_time_counter.v32[__HIGH] += 1;
    }
    tt_g_time_counter.v32[__LOW] = now;

    InterlockedCompareExchange(&tt_g_time_counter.lock, 0, 1);

    return (tt_s64_t)tt_g_time_counter.v64;
#else
    return (tt_s64_t)timeGetTime();
#endif
}

#endif /* __TT_TIME_REFERENCE_NATIVE__ */
