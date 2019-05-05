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

#include <tt_platform_info_native.h>

#include <init/tt_platform_info.h>
#include <init/tt_profile.h>
#include <log/tt_log.h>

#include <tt_cstd_api.h>

#include <stdio.h>
#include <unistd.h>

#ifdef TT_PLATFORM_ENABLE_NUMA
#include <numa.h>
#endif

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

static int __cpu_cache_line_size(int cpu_id, const char *cache_type);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_platform_page_size_load(OUT tt_u32_t *page_size)
{
    long __page_size = sysconf(_SC_PAGESIZE);
    if (__page_size <= 0) {
        TT_ERROR("unknown os page size\n");
        return TT_FAIL;
    }

    *page_size = (tt_u32_t)__page_size;
    return TT_SUCCESS;
}

tt_result_t tt_platform_cpu_num_load(OUT tt_u32_t *cpu_num)
{
    long __cpu_num = 0;

#ifdef TT_PLATFORM_ENABLE_NUMA
    do {
        tt_u32_t i, n;

        if (tt_g_numa_node_id == TT_NUMA_NODE_ID_UNSPECIFIED) { break; }

        if (numa_all_cpus_ptr == NULL) {
            TT_ERROR("null numa_all_cpus_ptr\n");
            return TT_FAIL;
        }

        n = numa_num_possible_cpus();
        for (i = 0; i < n; ++i) {
            if (numa_bitmask_isbitset(numa_all_cpus_ptr, i)) { ++__cpu_num; }
        }
        if (__cpu_num <= 0) {
            TT_ERROR("unknown cpu number\n");
            return TT_FAIL;
        }

        *cpu_num = (tt_u32_t)__cpu_num;
        return TT_SUCCESS;
    } while (0);
#endif

    __cpu_num = sysconf(_SC_NPROCESSORS_ONLN);
    if (__cpu_num <= 0) {
        TT_ERROR("unknown cpu number\n");
        return TT_FAIL;
    }

    *cpu_num = (tt_u32_t)__cpu_num;
    return TT_SUCCESS;
}

tt_result_t tt_platform_cache_line_size_load(OUT tt_u32_t *size)
{
    int __size = __cpu_cache_line_size(0, "Data");
    if (__size <= 0) {
        TT_ERROR("unknown cpu number\n");
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
#ifdef TT_PLATFORM_ENABLE_NUMA
    do {
        tt_u32_t node_id = 0;

        // todo: read node_id from profile

        if (node_id == TT_NUMA_NODE_ID_UNSPECIFIED) { break; }

        // if any error occurred during checking numa, we set numa node
        // id to unspecified rather than returning error

        if (numa_available() < 0) {
            TT_ERROR("numa not available\n");
            break;
        }
        if (numa_all_nodes_ptr == NULL) {
            TT_ERROR("null numa_all_nodes_ptr\n");
            break;
        }
        if (numa_bitmask_isbitset(numa_all_nodes_ptr, node_id)) {
            TT_ERROR("node[%d] is not in numa_all_nodes_ptr\n", node_id);
            break;
        }
        if (numa_node_size64(node_id, NULL) <= 0) {
            TT_ERROR("node[%d] has no memory\n", node_id);
            break;
        }

        *numa_id = node_id;
        *numa_id_thread = node_id;
        *numa_id_memory = node_id;
        return TT_SUCCESS;
    } while (0);
#endif

    *numa_id = TT_NUMA_NODE_ID_UNSPECIFIED;
    *numa_id_thread = TT_NUMA_NODE_ID_UNSPECIFIED;
    *numa_id_memory = TT_NUMA_NODE_ID_UNSPECIFIED;
    return TT_SUCCESS;
}

int __cpu_cache_line_size(int cpu_id, const char *cache_type)
{
    char path[200] = {0};
    FILE *fp = NULL;
    char *ret = NULL;

    int cache_index = 0;
    long int cache_line_size = 0;

// x86 has 2 cache types: 1 for data, 1 for instruction
#define __CACHE_TYPE_NUM 2
    while (cache_index < __CACHE_TYPE_NUM) {
        sprintf(path, "/sys/devices/system/cpu/cpu%d/cache/index%d/type",
                cpu_id, cache_index);
        fp = fopen(path, "r");
        if (fp == NULL) {
            TT_ERROR("fail to open file[%s]\n", path);
            return -1;
        }

        tt_memset(path, 0, sizeof(path));
        ret = fgets(path, sizeof(path), fp);
        fclose(fp);
        if (ret == NULL) {
            TT_ERROR("empty cache type file\n");
            return -1;
        }

        if (strstr(path, cache_type) != NULL) { break; }

        ++cache_index;
    }
    if (cache_index >= __CACHE_TYPE_NUM) {
        TT_ERROR("fail to match cache type[%s]\n", cache_type);
        return -1;
    }

    sprintf(path,
            "/sys/devices/system/cpu/cpu%d/cache/index%d/coherency_line_size",
            cpu_id, cache_index);
    fp = fopen(path, "r");
    if (fp == NULL) {
        TT_ERROR("fail to open file[%s]\n", path);
        return -1;
    }

    tt_memset(path, 0, sizeof(path));
    ret = fgets(path, sizeof(path), fp);
    fclose(fp);
    if (ret == NULL) {
        TT_ERROR("empty cache size file\n");
        return -1;
    }

    cache_line_size = strtol(path, NULL, 10);
    if (cache_line_size == 0) {
        TT_ERROR("fail to parse cache size[%s]\n", path);
        return -1;
    }

    return (int)cache_line_size;
}
