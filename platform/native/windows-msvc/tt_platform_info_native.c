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

#include <tt_cstd_api.h>

#include <iphlpapi.h>
#include <windows.h>

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

static tt_result_t __check_numa_node_id(tt_u32_t node_id);

static int __cpu_cache_line_size();

static tt_u32_t __mask_1num(IN ULONG_PTR mask);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_platform_page_size_load(OUT tt_u32_t *page_size)
{
    SYSTEM_INFO info;
    DWORD __page_size = 0;

    GetSystemInfo(&info);

    // read page size
    __page_size = info.dwPageSize;
    if (__page_size <= 0) {
        TT_ERROR("unknown os page size");
        return TT_FAIL;
    }

    *page_size = (tt_u32_t)__page_size;
    return TT_SUCCESS;
}

tt_result_t tt_platform_cpu_num_load(OUT tt_u32_t *cpu_num)
{
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION info = NULL;
    DWORD info_len = 0;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION cur_info = NULL;
    tt_u32_t cur_len = 0;
    tt_result_t result = TT_FAIL;
    tt_u32_t __cpu_num = 0;

    // get info
    if (!((GetLogicalProcessorInformation(info, &info_len) == FALSE) &&
          (GetLastError() == ERROR_INSUFFICIENT_BUFFER))) {
        return TT_FAIL;
    }
    info = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(info_len);
    if (info == NULL) {
        return TT_FAIL;
    }
    if (!GetLogicalProcessorInformation(info, &info_len)) {
        free(info);
        return TT_FAIL;
    }

    // enumerate info
    if (tt_g_numa_node_id != TT_NUMA_NODE_ID_UNSPECIFIED) {
        cur_info = info;
        while (cur_len < info_len) {
            // count all logical cpus with specified numa node id
            if ((cur_info->Relationship == RelationNumaNode) &&
                (cur_info->NumaNode.NodeNumber == tt_g_numa_node_id)) {
                __cpu_num += __mask_1num(cur_info->ProcessorMask);
            }

            ++cur_info;
            cur_len += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
        }
    } else {
        cur_info = info;
        while (cur_len < info_len) {
            // count all logical cpus of all cpu cores
            if (cur_info->Relationship == RelationProcessorCore) {
                __cpu_num += __mask_1num(cur_info->ProcessorMask);
            }

            ++cur_info;
            cur_len += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
        }
    }

    free(info);
    if (__cpu_num > 0) {
        *cpu_num = __cpu_num;
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_result_t tt_platform_cache_line_size_load(OUT tt_u32_t *size)
{
    int __size = __cpu_cache_line_size();
    if (__size <= 0) {
        TT_ERROR("unknown cpu number");
        return TT_FAIL;
    }

    *size = (tt_u32_t)__size;
    return TT_SUCCESS;
}

tt_result_t tt_platform_numa_node_id_load(IN tt_profile_t *profile,
                                          OUT tt_u32_t *numa_id,
                                          OUT tt_u32_t *numa_id_thread,
                                          OUT tt_u32_t *numa_id_memory)
{
    tt_u32_t node_id = 0;

#ifdef TT_PLATFORM_ENABLE_NUMA
    if ((node_id != TT_NUMA_NODE_ID_UNSPECIFIED) &&
        (TT_OK(__check_numa_node_id(node_id)))) {
        // todo: read node_id from profile

        *numa_id = node_id;
        *numa_id_thread = node_id;
        *numa_id_memory = node_id;
        return TT_SUCCESS;
    } else
#endif
    {
        *numa_id = TT_NUMA_NODE_ID_UNSPECIFIED;
        *numa_id_thread = TT_NUMA_NODE_ID_UNSPECIFIED;
        *numa_id_memory = TT_NUMA_NODE_ID_UNSPECIFIED;
        return TT_SUCCESS;
    }
}

tt_result_t __check_numa_node_id(tt_u32_t node_id)
{
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION info = NULL;
    DWORD info_len = 0;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION cur_info = NULL;
    tt_u32_t cur_len = 0;
    tt_result_t result = TT_FAIL;

    // get info
    if (!((GetLogicalProcessorInformation(info, &info_len) == FALSE) &&
          (GetLastError() == ERROR_INSUFFICIENT_BUFFER))) {
        return TT_FAIL;
    }
    info = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(info_len);
    if (info == NULL) {
        return TT_FAIL;
    }
    if (!GetLogicalProcessorInformation(info, &info_len)) {
        free(info);
        return TT_FAIL;
    }

    // enumerate info
    cur_info = info;
    while (cur_len < info_len) {
        if (cur_info->Relationship == RelationNumaNode) {
            if (cur_info->NumaNode.NodeNumber == node_id) {
                result = TT_SUCCESS;
                break;
            }
        }

        ++cur_info;
        cur_len += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
    }
    free(info);
    return result;
}

int __cpu_cache_line_size()
{
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION info = NULL;
    DWORD info_len = 0;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION cur_info = NULL;
    tt_u32_t cur_len = 0;
    int __cache_line_size = -1;

    // get info
    if (!((GetLogicalProcessorInformation(info, &info_len) == FALSE) &&
          (GetLastError() == ERROR_INSUFFICIENT_BUFFER))) {
        return -1;
    }
    info = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(info_len);
    if (info == NULL) {
        return -1;
    }
    if (!GetLogicalProcessorInformation(info, &info_len)) {
        free(info);
        return -1;
    }

    // enumerate info
    cur_info = info;
    while (cur_len < info_len) {
        if (cur_info->Relationship == RelationCache) {
            CACHE_DESCRIPTOR *cache = &cur_info->Cache;
            if ((cache->Level == 1) &&
                ((cache->Type == CacheData) || (cache->Type == CacheUnified))) {
                __cache_line_size = cur_info->Cache.LineSize;
                break;
                // would there be cpus with different cache line size?
            }
        }

        ++cur_info;
        cur_len += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
    }
    free(info);
    return __cache_line_size;
}

tt_u32_t __mask_1num(IN ULONG_PTR mask)
{
    tt_u32_t bit_pos = 0;
    tt_u32_t max_bit_pos = sizeof(ULONG_PTR) * 8;
    tt_u32_t ret = 0;

    for (bit_pos = 0; bit_pos < max_bit_pos; ++bit_pos) {
        if (mask & (1ULL << bit_pos)) {
            ++ret;
        }
    }

    return ret;
}
