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

#include <stdio.h>
#include <unistd.h>

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

static int __cache_size_from_sysfs(int cpu_id, const char *cache_type);

static long __cache_size_from_cpuinfo(int cpu_id);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_platform_page_size_load(OUT tt_u32_t *page_size)
{
    long __page_size = sysconf(_SC_PAGESIZE);
    if (__page_size <= 0) {
        TT_ERROR("unknown os page size");
        return TT_FAIL;
    }

    *page_size = (tt_u32_t)__page_size;
    return TT_SUCCESS;
}

tt_result_t tt_platform_cpu_num_load(OUT tt_u32_t *cpu_num)
{
    long __cpu_num = 0;

    __cpu_num = sysconf(_SC_NPROCESSORS_ONLN);
    if (__cpu_num <= 0) {
        TT_ERROR("unknown cpu number");
        return TT_FAIL;
    }

    *cpu_num = (tt_u32_t)__cpu_num;
    return TT_SUCCESS;
}

tt_result_t tt_platform_cache_line_size_load(OUT tt_u32_t *size)
{
    int __size;

    __size = __cache_size_from_sysfs(0, "Data");
    if (__size > 0) {
        *size = (tt_u32_t)__size;
        return TT_SUCCESS;
    }

    __size = (int)__cache_size_from_cpuinfo(0);
    if (__size > 0) {
        *size = (tt_u32_t)__size;
        return TT_SUCCESS;
    }

    TT_ERROR("unknown cache size");
#if 0
    return TT_FAIL;
#else
    // seems no way to get cpu cache line size when doing ci test
    // with android arm emulator
    *size = 64;
    return TT_SUCCESS;
#endif
}

tt_result_t tt_platform_numa_node_id_load(IN tt_profile_t *profile,
                                          OUT tt_u32_t *numa_id,
                                          OUT tt_u32_t *numa_id_thread,
                                          OUT tt_u32_t *numa_id_memory)
{
    *numa_id = TT_NUMA_NODE_ID_UNSPECIFIED;
    *numa_id_thread = TT_NUMA_NODE_ID_UNSPECIFIED;
    *numa_id_memory = TT_NUMA_NODE_ID_UNSPECIFIED;
    return TT_SUCCESS;
}

int __cache_size_from_sysfs(int cpu_id, const char *cache_type)
{
    char path[200] = {0};
    FILE *fp = NULL;
    char *ret = NULL;

    int cache_index = 0;
    long int cache_line_size = 0;

// x86 has 2 cache types: 1 for data, 1 for instruction
#define __CACHE_TYPE_NUM 2
    while (cache_index < __CACHE_TYPE_NUM) {
        sprintf(path,
                "/sys/devices/system/cpu/cpu%d/cache/index%d/type",
                cpu_id,
                cache_index);
        fp = fopen(path, "r");
        if (fp == NULL) {
            TT_ERROR("fail to open file[%s]", path);
            return -1;
        }

        tt_memset(path, 0, sizeof(path));
        ret = fgets(path, sizeof(path), fp);
        fclose(fp);
        if (ret == NULL) {
            TT_ERROR("empty cache type file");
            return -1;
        }

        if (strstr(path, cache_type) != NULL) {
            break;
        }

        ++cache_index;
    }
    if (cache_index >= __CACHE_TYPE_NUM) {
        TT_ERROR("fail to match cache type[%s]", cache_type);
        return -1;
    }

    sprintf(path,
            "/sys/devices/system/cpu/cpu%d/cache/index%d/coherency_line_size",
            cpu_id,
            cache_index);
    fp = fopen(path, "r");
    if (fp == NULL) {
        TT_ERROR("fail to open file[%s]", path);
        return -1;
    }

    tt_memset(path, 0, sizeof(path));
    ret = fgets(path, sizeof(path), fp);
    fclose(fp);
    if (ret == NULL) {
        TT_ERROR("empty cache size file");
        return -1;
    }

    cache_line_size = strtol(path, NULL, 10);
    if (cache_line_size == 0) {
        TT_ERROR("fail to parse cache size[%s]", path);
        return -1;
    }

    return (int)cache_line_size;
}

long __cache_size_from_cpuinfo(int cpu_id)
{
    FILE *fp;
    char buf[200];
    char *p = buf;
    size_t cap = sizeof(buf);

    fp = fopen("/proc/cpuinfo", "r");
    if (fp == NULL) {
        TT_ERROR("fail to open /proc/cpuinfo");
        return -1;
    }

    while (getline(&p, &cap, fp) > 0) {
        char *pos;
        long size;

        if ((pos = strstr(p, "cache_alignment")) == NULL) {
            goto next;
        }
        pos += sizeof("cache_alignment") - 1;

        if ((pos = strchr(p, ':')) == NULL) {
            goto next;
        }
        pos += 1;

        size = strtol(pos, NULL, 10);
        if ((size != 0) && (size != LONG_MIN) && (size != LONG_MAX)) {
            if (p != buf) {
                free(p);
            }
            return size;
        }

    next:
        if (p != buf) {
            free(p);
        }
        p = buf;
        cap = sizeof(buf);
    }

    fclose(fp);
    return -1;
}
