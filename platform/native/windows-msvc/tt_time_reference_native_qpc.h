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
@file tt_time_reference_native_qpc.h
@brief system time reference APIs

this file specifies system interfaces for time reference
*/

#ifndef __TT_TIME_REFERENCE_NATIVE_QPC__
#define __TT_TIME_REFERENCE_NATIVE_QPC__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

/**
@struct tt_time_ref_ctx_ntv_qpc_t
reference time context
*/
typedef struct
{
    LARGE_INTEGER freq;
} tt_time_ref_ctx_ntv_qpc_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

extern tt_time_ref_ctx_ntv_qpc_t tt_g_time_ref_ctx_qpc;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/**
@fn tt_result_t tt_time_ref_component_init_ntv_qpc()
initialize ts time reference system

@return
- TT_SUCCESS if initialization succeeds
- TT_FAIL otherwise
*/
extern tt_result_t tt_time_ref_component_init_ntv_qpc();

tt_inline tt_s64_t tt_time_ref_ntv_qpc()
{
    LARGE_INTEGER now;

    // reason to use QueryPerformanceCounter():
    //  - GetTickCount64() is of less accuracy(10ms - 15ms)
    //  - timeGetTime() has wrap around issue (overflow each 49 days)
    QueryPerformanceCounter(&now);
    return (tt_s64_t)now.QuadPart;
}

tt_inline tt_s64_t tt_time_ref2ms_ntv_qpc(IN tt_s64_t ref)
{
    return (ref * 1000) / tt_g_time_ref_ctx_qpc.freq.QuadPart;
}

tt_inline tt_s64_t tt_time_ms2ref_ntv_qpc(IN tt_s64_t ms)
{
    return (ms * tt_g_time_ref_ctx_qpc.freq.QuadPart) / 1000;
}

#endif /* __TT_TIME_REFERENCE_NATIVE_QPC__ */
