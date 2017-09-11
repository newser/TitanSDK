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

#include <tt_platform_info_native.h>

#include <init/tt_platform_info.h>
#include <init/tt_profile.h>
#include <log/tt_log.h>

#include <sys/sysctl.h>

#include <stdio.h>

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

tt_result_t tt_platform_page_size_load(OUT tt_u32_t *page_size)
{
    int name[2];
    int __page_size = 0;
    size_t len = sizeof(__page_size);

    // read page size
    name[0] = CTL_HW;
    name[1] = HW_PAGESIZE;
    if (sysctl(name, 2, &__page_size, &len, NULL, 0) != 0) {
        TT_ERROR("fail to get os page size");
        return TT_FAIL;
    }
    if (__page_size <= 0) {
        TT_ERROR("unknown os page size");
        return TT_FAIL;
    }

    *page_size = __page_size;
    return TT_SUCCESS;
}

tt_result_t tt_platform_cpu_num_load(OUT tt_u32_t *cpu_num)
{
    int name[2];
    int __cpu_num = 0;
    size_t len = sizeof(__cpu_num);

    name[0] = CTL_HW;
    name[1] = HW_NCPU;
    if (sysctl(name, 2, &__cpu_num, &len, NULL, 0) != 0) {
        TT_ERROR("fail to get cpu number");
        return TT_FAIL;
    }
    if (__cpu_num <= 0) {
        TT_ERROR("unknown cpu number");
        return TT_FAIL;
    }

    *cpu_num = __cpu_num;
    return TT_SUCCESS;
}

tt_result_t tt_platform_cache_line_size_load(OUT tt_u32_t *size)
{
    int name[2];
    int __cache_line_size = 0;
    size_t len = sizeof(__cache_line_size);

    name[0] = CTL_HW;
    name[1] = HW_CACHELINE;
    if (sysctl(name, 2, &__cache_line_size, &len, NULL, 0) != 0) {
        TT_ERROR("fail to get cpu cache line size");
        return TT_FAIL;
    }
    if (__cache_line_size <= 0) {
        TT_ERROR("unknown cpu cache line size");
        return TT_FAIL;
    }

    *size = __cache_line_size;
    return TT_SUCCESS;
}

tt_result_t tt_platform_numa_node_id_load(IN tt_profile_t *profile,
                                          OUT tt_u32_t *numa_id,
                                          OUT tt_u32_t *numa_id_thread,
                                          OUT tt_u32_t *numa_id_memory)
{
    // macos does not support numa
    *numa_id = TT_NUMA_NODE_ID_UNSPECIFIED;
    *numa_id_thread = TT_NUMA_NODE_ID_UNSPECIFIED;
    *numa_id_memory = TT_NUMA_NODE_ID_UNSPECIFIED;
    return TT_SUCCESS;
}
