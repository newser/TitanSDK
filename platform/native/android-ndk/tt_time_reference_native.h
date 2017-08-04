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

#include <time.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

/**
@def tt_time_ref2ms_ntv(ref, ctx)
convert time reference to millisecond
*/
#define tt_time_ref2ms_ntv(ref) (ref)

/**
@def tt_time_ms2ref_NTV(ms, ctx)
convert millisecond to time reference
*/
#define tt_time_ms2ref_ntv(ms) (ms)

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

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

tt_inline tt_s64_t tt_time_ref_ntv()
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return (tt_s64_t)((now.tv_sec * 1000) + (now.tv_nsec / 1000000));
}

#endif /* __TT_TIME_REFERENCE_NATIVE__ */
