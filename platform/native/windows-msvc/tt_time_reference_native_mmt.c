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

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_time_reference_native_mmt.h>

#include <misc/tt_util.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

// 5 millisecond.
// default resolution on some platform is not as expected
#define __TT_TIME_RESOLUTION 5

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

tt_time_ref_ctx_ntv_mmt_t tt_g_time_ref_ctx_mmt;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_time_ref_component_init_ntv_mmt()
{
    tt_result_t result = TT_FAIL;

    if (!InitializeCriticalSectionAndSpinCount(&tt_g_time_ref_ctx_mmt.cs,
                                               4000)) {
        TT_ERROR("fail to create critical section");
        return TT_FAIL;
    }
    tt_g_time_ref_ctx_mmt.high_32bit = 0;
    tt_g_time_ref_ctx_mmt.low_32bit = 0;

    // set expected resolution
    // does setting resolution harm performance??
    do {
        TIMECAPS tc;
        if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) == TIMERR_NOERROR) {
            UINT resolution = 0;

            resolution = TT_MAX(tc.wPeriodMin, __TT_TIME_RESOLUTION);
            resolution = TT_MIN(resolution, tc.wPeriodMax);
            timeBeginPeriod(resolution);
            TT_INFO("time resolution is set to %dms", resolution);
        } else {
            TT_WARN("time resolution is not as expected");
        }
    } while (0);

    return TT_SUCCESS;
}
