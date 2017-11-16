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

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_time_reference_native.h>

#include <misc/tt_util.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

// 5 millisecond.
// default resolution on some platform is not as expected
#define __TIME_RESOLUTION 5

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

tt_time_counter_t tt_g_time_counter;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_time_ref_component_init_ntv()
{
    TIMECAPS tc;
    UINT resolution;

    tt_g_time_counter.lock = 0;
    tt_g_time_counter.v32[__LOW] = timeGetTime();
    tt_g_time_counter.v32[__HIGH] = 0;

    // set time resolution
    if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR) {
        TT_WARN("fail to get time resolution");
        goto out;
    }

    resolution = TT_MAX(tc.wPeriodMin, __TIME_RESOLUTION);
    resolution = TT_MIN(resolution, tc.wPeriodMax);
    if (timeBeginPeriod(resolution) != TIMERR_NOERROR) {
        TT_WARN("time resolution may not be as expected");
        goto out;
    }

out:
    return TT_SUCCESS;
}
