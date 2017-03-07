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

#include <memory/tt_page.h>

#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <os/tt_atomic.h>

#include <tt_cstd_api.h>
#include <tt_page_native.h>

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

static tt_atomic_s64_t tt_stat_page_alloc;

static tt_atomic_s64_t tt_stat_page_peak;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __page_component_init(IN tt_component_t *comp,
                                         IN tt_profile_t *profile);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_page_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __page_component_init,
    };

    // init component
    tt_component_init(&comp, TT_COMPONENT_PAGE, "Page", NULL, &itf);

    // register component
    tt_component_register(&comp);
}

void *tt_page_alloc(IN tt_u32_t size)
{
    void *p;

    TT_U32_ALIGN_INC_PAGE(size);
#ifdef TT_PAGE_BY_MALLOC
    p = tt_c_malloc(size);
#else
    p = tt_page_alloc_ntv(size);
#endif
    if (p != NULL) {
        tt_s64_t val;

        tt_atomic_s64_add(&tt_stat_page_alloc, size);

        // updating peak value is not atomic, it's not accurate
        // but only for reference
        val = tt_atomic_s64_get(&tt_stat_page_alloc);
        if (val > tt_atomic_s64_get(&tt_stat_page_peak)) {
            tt_atomic_s64_set(&tt_stat_page_peak, val);
        }
    }
    return p;
}

void tt_page_free(IN void *addr, IN tt_u32_t size)
{
    if (addr != NULL) {
#ifdef TT_PAGE_BY_MALLOC
        tt_c_free(addr);
#else
        TT_U32_ALIGN_INC_PAGE(size);
        tt_page_free_ntv(addr, size);
#endif
        tt_atomic_s64_sub(&tt_stat_page_alloc, size);
    }
}

void *tt_page_alloc_align(IN tt_u32_t size_order, OUT tt_uintptr_t *handle)
{
    void *p;

    size_order = TT_MAX(size_order, tt_g_page_size_order);
#ifdef TT_PAGE_BY_MALLOC
    p = tt_c_malloc((tt_u32_t)(1 << (size_order + 1)));
    if (p != NULL) {
        *handle = (tt_uintptr_t)p;
        TT_PTR_ALIGN_INC(p, size_order);
    }
#else
    p = tt_page_alloc_align_ntv(size_order, handle);
#endif
    if (p != NULL) {
        tt_s64_t val;

        val = (tt_s64_t)1 << size_order;
        tt_atomic_s64_add(&tt_stat_page_alloc, val);

        // updating peak value is not atomic, it's not accurate
        // but only for reference
        val = tt_atomic_s64_get(&tt_stat_page_alloc);
        if (val > tt_atomic_s64_get(&tt_stat_page_peak)) {
            tt_atomic_s64_set(&tt_stat_page_peak, val);
        }
    }
    return p;
}

void tt_page_free_align(IN void *addr,
                        IN tt_u32_t size_order,
                        IN tt_uintptr_t handle)
{
#ifdef TT_PAGE_BY_MALLOC
    tt_c_free((void *)handle);
#else
    size_order = TT_MAX(size_order, tt_g_page_size_order);
    tt_page_free_align_ntv(addr, size_order, handle);
#endif
    tt_atomic_s64_sub(&tt_stat_page_alloc, 1 << size_order);
}

tt_result_t __page_component_init(IN tt_component_t *comp,
                                  IN tt_profile_t *profile)
{
    tt_atomic_s64_set(&tt_stat_page_alloc, 0);

    tt_atomic_s64_set(&tt_stat_page_peak, 0);

    return TT_SUCCESS;
}
