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
@file tt_platform_info.h
@brief ts platform information

this file defines platform information.
*/

#ifndef __TT_PLATFORM_INFO__
#define __TT_PLATFORM_INFO__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_NUMA_NODE_ID_UNSPECIFIED (~0)

// align with cpu
#define TT_U32_ALIGN_INC_CPU(n) TT_U32_ALIGN_INC((n), tt_g_cpu_align_order)
#define TT_PTR_ALIGN_INC_CPU(p) TT_PTR_ALIGN_INC((p), tt_g_cpu_align_order)

// align with cache line
#define TT_U32_ALIGN_INC_CACHE(n)                                              \
    TT_U32_ALIGN_INC((n), tt_g_cache_line_size_order)
#define TT_PTR_ALIGN_INC_CACHE(p)                                              \
    TT_PTR_ALIGN_INC((p), tt_g_cache_line_size_order)

// align with page size
#define TT_U32_ALIGN_INC_PAGE(n) TT_U32_ALIGN_INC((n), tt_g_page_size_order)
#define TT_PTR_ALIGN_INC_PAGE(p) TT_PTR_ALIGN_INC((p), tt_g_page_size_order)

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_profile_s;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

/**
@var tt_g_page_size
each system page is of tt_g_page_size bytes size

@note
- if not initialized, the value is 0 and should be considered as an invalid
  value
- it equals (1 << tt_g_page_size_order)
*/
tt_export tt_u32_t tt_g_page_size;
tt_export tt_u32_t tt_g_page_size_order;

tt_export tt_u32_t tt_g_cpu_num;

tt_export tt_u32_t tt_g_cache_line_size;
tt_export tt_u32_t tt_g_cache_line_size_order;

tt_export tt_u32_t tt_g_numa_node_id;
tt_export tt_u32_t tt_g_numa_node_id_thread;
tt_export tt_u32_t tt_g_numa_node_id_memory;

tt_export tt_u32_t tt_g_cpu_align_order;
tt_export tt_u32_t tt_g_platform_64bit;
tt_export tt_bool_t tt_g_big_endian;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_result_t tt_platform_info_load(IN struct tt_profile_s *profile);

tt_export tt_bool_t tt_platform_numa_enabled();

#endif /* __TT_PLATFORM_INFO__ */
