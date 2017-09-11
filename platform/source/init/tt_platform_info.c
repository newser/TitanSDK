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

#include <init/tt_platform_info.h>

#include <init/tt_profile.h>
#include <log/tt_log.h>
#include <misc/tt_util.h>

#include <tt_cstd_api.h>
#include <tt_platform_info_native.h>

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

tt_u32_t tt_g_page_size = 0;
tt_u32_t tt_g_page_size_order = 0;

tt_u32_t tt_g_cpu_num;

tt_u32_t tt_g_cache_line_size;
tt_u32_t tt_g_cache_line_size_order;

tt_u32_t tt_g_numa_node_id = TT_NUMA_NODE_ID_UNSPECIFIED;
tt_u32_t tt_g_numa_node_id_thread = TT_NUMA_NODE_ID_UNSPECIFIED;
tt_u32_t tt_g_numa_node_id_memory = TT_NUMA_NODE_ID_UNSPECIFIED;

tt_u32_t tt_g_cpu_align_order;
tt_u32_t tt_g_platform_64bit;
tt_bool_t tt_g_big_endian;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __basic_type_check();

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_platform_info_load(IN tt_profile_t *profile)
{
    tt_result_t result = TT_FAIL;

    // first load numa info
    result = tt_platform_numa_node_id_load(profile,
                                           &tt_g_numa_node_id,
                                           &tt_g_numa_node_id_thread,
                                           &tt_g_numa_node_id_memory);
    if (!TT_OK(result)) {
        TT_ERROR("fail to load numa node id\n");
        return TT_FAIL;
    }

    // page size
    result = tt_platform_page_size_load(&tt_g_page_size);
    if (!TT_OK(result)) {
        TT_ERROR("fail to load page size\n");
        return TT_FAIL;
    }

    result = tt_order(tt_g_page_size, &tt_g_page_size_order);
    if (!TT_OK(result) || (tt_g_page_size != (1 << tt_g_page_size_order))) {
        TT_ERROR("fail to load page size order [%x - %d]\n",
                 tt_g_page_size,
                 tt_g_page_size_order);
        return TT_FAIL;
    }

    // cpu num
    result = tt_platform_cpu_num_load(&tt_g_cpu_num);
    if (!TT_OK(result)) {
        TT_ERROR("fail to load cpu num\n");
        return TT_FAIL;
    }

    // cache line size
    result = tt_platform_cache_line_size_load(&tt_g_cache_line_size);
    if (!TT_OK(result)) {
        TT_ERROR("fail to load cache line size\n");
        return TT_FAIL;
    }

    result = tt_order(tt_g_cache_line_size, &tt_g_cache_line_size_order);
    if (!TT_OK(result) ||
        (tt_g_cache_line_size != (1 << tt_g_cache_line_size_order))) {
        TT_ERROR("fail to load cache line size order [%x - %d]\n",
                 tt_g_cache_line_size,
                 tt_g_cache_line_size_order);
        return TT_FAIL;
    }

    // other
    if (sizeof(void *) == 4) {
        tt_g_cpu_align_order = 2;
        tt_g_platform_64bit = 0;
    } else if (sizeof(void *) == 8) {
        tt_g_cpu_align_order = 3;
        tt_g_platform_64bit = 1;
    } else {
        TT_ERROR("unknown platform, sizeof(void*): %d\n",
                 (tt_u32_t)sizeof(void *));
        return TT_FAIL;
    }

    do {
        tt_u16_t v16 = 1;
        tt_u8_t *v8 = (tt_u8_t *)&v16;
        tt_g_big_endian = TT_COND(*v8 == 0, TT_TRUE, TT_FALSE);
    } while (0);

    return __basic_type_check();
}

tt_bool_t tt_platform_numa_enabled()
{
    return tt_platform_numa_enabled_ntv();
}

tt_result_t __basic_type_check()
{
#define __CHECK_TYPE_LEN(t, l)                                                 \
    do {                                                                       \
        if (sizeof(t) != l) {                                                  \
            TT_ERROR("sizeof(%s) != %d\n", #t, l);                             \
            return TT_FAIL;                                                    \
        }                                                                      \
    } while (0)

    __CHECK_TYPE_LEN(tt_u8_t, 1);
    __CHECK_TYPE_LEN(tt_s8_t, 1);
    __CHECK_TYPE_LEN(tt_u16_t, 2);
    __CHECK_TYPE_LEN(tt_s16_t, 2);
    __CHECK_TYPE_LEN(tt_u32_t, 4);
    __CHECK_TYPE_LEN(tt_s32_t, 4);
    __CHECK_TYPE_LEN(tt_u64_t, 8);
    __CHECK_TYPE_LEN(tt_s64_t, 8);
    __CHECK_TYPE_LEN(tt_float_t, 4);
    __CHECK_TYPE_LEN(tt_double_t, 8);

#if TT_ENV_IS_64BIT
    __CHECK_TYPE_LEN(tt_ptr_t, 8);
    __CHECK_TYPE_LEN(tt_uintptr_t, 8);
    __CHECK_TYPE_LEN(tt_intptr_t, 8);
    __CHECK_TYPE_LEN(tt_ptrdiff_t, 8);
#else
    __CHECK_TYPE_LEN(tt_ptr_t, 4);
    __CHECK_TYPE_LEN(tt_uintptr_t, 4);
    __CHECK_TYPE_LEN(tt_intptr_t, 4);
    __CHECK_TYPE_LEN(tt_ptrdiff_t, 4);
#endif

    return TT_SUCCESS;
}
