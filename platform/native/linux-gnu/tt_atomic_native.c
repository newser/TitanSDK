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

#include <tt_atomic_native.h>

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

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_atomic_component_init_ntv()
{
    if (sizeof(tt_atomic_s32_ntv_t) != 4) {
        TT_ERROR("sizeof(tt_atomic_s32_ntv_t) is %d, expecting 4bytes",
                 sizeof(tt_atomic_s32_ntv_t));
        return TT_FAIL;
    }

    if (sizeof(tt_atomic_s64_ntv_t) != 8) {
        TT_ERROR("sizeof(tt_atomic_s64_ntv_t) is %d, expecting 8bytes",
                 sizeof(tt_atomic_s64_ntv_t));
        return TT_FAIL;
    }

    return TT_SUCCESS;
}
