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
@file tt_platform_info_native.h
@brief getting platform info

this file provide interfaces for getting platform info
*/

#ifndef __TT_PLATFORM_INFO_NATIVE__
#define __TT_PLATFORM_INFO_NATIVE__

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

struct tt_profile_s;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_platform_page_size_load(OUT tt_u32_t *page_size);

extern tt_result_t tt_platform_cpu_num_load(OUT tt_u32_t *cpu_num);

extern tt_result_t tt_platform_cache_line_size_load(OUT tt_u32_t *size);

/**
 @note
 - this function should do numa initialization
 */
extern tt_result_t tt_platform_numa_node_id_load(
    IN struct tt_profile_s *profile,
    OUT tt_u32_t *numa_id,
    OUT tt_u32_t *numa_id_thread,
    OUT tt_u32_t *numa_id_memory);

tt_inline tt_bool_t tt_platform_numa_enabled_ntv()
{
#ifdef TT_PLATFORM_NUMA_ENABLE
    return TT_TRUE;
#else
    return TT_FALSE;
#endif
}

#endif /* __TT_PLATFORM_INFO_NATIVE__ */
