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

#include <mach/mach_time.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

extern mach_timebase_info_data_t tt_g_timebase;

extern tt_s64_t tt_g_timebase_denom;

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
    // force converting to tt_s64_t
    // absolute time won't be so large that it overflowed
    return (tt_s64_t)(mach_absolute_time());
}

tt_inline tt_s64_t tt_time_ref2ms_ntv(IN tt_s64_t ref)
{
    // ns to ms
    return (ref * tt_g_timebase.numer) / tt_g_timebase_denom;
}

tt_inline tt_s64_t tt_time_ms2ref_ntv(IN tt_s64_t ms)
{
    // ms to ns, would overflow?
    return (ms * tt_g_timebase_denom) / tt_g_timebase.numer;
}

#endif /* __TT_TIME_REFERENCE_NATIVE__ */
