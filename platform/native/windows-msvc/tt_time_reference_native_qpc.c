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

#include <tt_time_reference_native_qpc.h>

#include <tt_sys_error.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

tt_time_ref_ctx_ntv_qpc_t tt_g_time_ref_ctx_qpc;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_time_ref_component_init_ntv_qpc()
{
    if (sizeof(LARGE_INTEGER) != sizeof(tt_s64_t)) {
        TT_ERROR(
            "size of LARGE_INTEGER[%d] does not equal size of tt_s64_t[%d]",
            sizeof(LARGE_INTEGER),
            sizeof(tt_s64_t));
        return TT_FAIL;
    }

    if (!QueryPerformanceFrequency(&tt_g_time_ref_ctx_qpc.freq)) {
        TT_ERROR_NTV("fail to get frequency");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}
