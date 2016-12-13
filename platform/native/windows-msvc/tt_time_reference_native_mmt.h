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
@file tt_time_reference_native_mmt.h
@brief system time reference APIs

this file specifies system interfaces for time reference
*/

#ifndef __TT_TIME_REFERENCE_NATIVE_MMT__
#define __TT_TIME_REFERENCE_NATIVE_MMT__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

/**
@def tt_time_ref2ms_ntv_mmt(ref, ctx)
convert time reference to millisecond
*/
#define tt_time_ref2ms_ntv_mmt(ref) (ref)
/**
@def tt_time_ms2ref_ntv_mmt(ms, ctx)
convert millisecond to time reference
*/
#define tt_time_ms2ref_ntv_mmt(ms) (ms)

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

/**
@struct tt_time_ref_ctx_ntv_mmt_t
reference time context
*/
typedef struct
{
    CRITICAL_SECTION cs;

    tt_u32_t high_32bit;
    tt_u32_t low_32bit;
} tt_time_ref_ctx_ntv_mmt_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

extern tt_time_ref_ctx_ntv_mmt_t tt_g_time_ref_ctx_mmt;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/**
@fn tt_result_t tt_time_ref_component_init_ntv_mmt()
initialize ts time reference system

@return
- TT_SUCCESS if initialization succeeds
- TT_FAIL otherwise
*/
extern tt_result_t tt_time_ref_component_init_ntv_mmt();

tt_inline tt_s64_t tt_time_ref_ntv_mmt()
{
    DWORD now = timeGetTime();
    // DWORD now = GetTickCount();

    EnterCriticalSection(&tt_g_time_ref_ctx_mmt.cs);

    // reason to use timeGetTime:
    //  - GetTickCount() is of less accuracy(10ms - 15ms)
    //  - QueryPerformanceCounter() performs poor on win xp

    // timeGetTime() is monotonical
    if (now < tt_g_time_ref_ctx_mmt.low_32bit) {
        tt_g_time_ref_ctx_mmt.high_32bit += 1;
    }
    tt_g_time_ref_ctx_mmt.low_32bit = now;

    LeaveCriticalSection(&tt_g_time_ref_ctx_mmt.cs);

    return (tt_s64_t)((((tt_u64_t)tt_g_time_ref_ctx_mmt.high_32bit) << 32) +
                      ((tt_u64_t)tt_g_time_ref_ctx_mmt.low_32bit));
}

#endif /* __TT_TIME_REFERENCE_NATIVE_MMT__ */
