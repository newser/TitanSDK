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

#include <memory/tt_slab.h>

#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <log/tt_log.h>
#include <memory/tt_page.h>
#include <misc/tt_assert.h>
#include <misc/tt_util.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

// object format:
// | tag | data |

#define __SLAB_OBJTAG(obj) TT_PTR_DEC(__slab_objtag_t, obj, tt_s_objtag_size)

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef struct
{
    tt_lnode_t node;
    tt_slab_t *parent_slab;
    tt_s32_t ref_counter;
} __slab_area_t;

typedef struct
{
    __slab_area_t *area;

#ifdef TT_MEMORY_TAG_ENABLE
    const tt_char_t *func;
    tt_u32_t line;
#endif
} __slab_objtag_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_u32_t tt_s_areadesc_size;

static tt_u32_t tt_s_objtag_size;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __slab_component_init(IN tt_component_t *comp,
                                         IN tt_profile_t *profile);

static tt_result_t __slab_attr_check(IN tt_slab_attr_t *attr);

static tt_result_t __slab_expand(IN tt_slab_t *slab);

static void __slab_populate(IN tt_slab_t *slab, IN __slab_area_t *area);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_slab_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __slab_component_init,
    };

    // init component
    tt_component_init(&comp, TT_COMPONENT_SLAB, "Memory Slab", NULL, &itf);

    // register component
    tt_component_register(&comp);
}

tt_result_t tt_slab_create(IN tt_slab_t *slab,
                           IN tt_u32_t obj_size,
                           IN OPT tt_slab_attr_t *attr)
{
    tt_u32_t size;

    TT_ASSERT(slab != NULL);
    TT_ASSERT(obj_size > 0);

    tt_memset(slab, 0, sizeof(tt_slab_t));

    // attr
    if (attr != NULL) {
        tt_memcpy(&slab->attr, attr, sizeof(tt_slab_attr_t));
    } else {
        tt_slab_attr_default(&slab->attr);
    }
    if (!TT_OK(__slab_attr_check(&slab->attr))) {
        TT_ERROR("invalid slab attributes");
        return TT_FAIL;
    }

    // each obj size is at least a tt_lnode_t
    size = TT_MAX(obj_size, sizeof(tt_lnode_t));
    size += tt_s_objtag_size;
    TT_U32_ALIGN_INC_CPU(size);
    if (slab->attr.hwcache_align) {
        TT_U32_ALIGN_INC_CACHE(size);
    }
    slab->obj_size = size;

    tt_list_init(&slab->free_obj_list);

    // here obj_size should have been aligned
    if (TT_U32_MUL_WOULD_OVFL(slab->obj_size, slab->attr.objnum_per_expand)) {
        TT_ERROR("too large obj size or num");
        return TT_FAIL;
    }
    size = slab->obj_size * slab->attr.objnum_per_expand;
    if (TT_U32_ADD_WOULD_OVFL(size, tt_s_areadesc_size)) {
        TT_ERROR("too large obj size or num");
        return TT_FAIL;
    }
    size += tt_s_areadesc_size;
    TT_U32_ALIGN_INC_PAGE(size);
    slab->area_size = size;

    tt_list_init(&slab->area_list);

    if (!TT_OK(tt_spinlock_create(&slab->lock, NULL))) {
        TT_ERROR("fail to create slab lock");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_slab_destroy(IN tt_slab_t *slab, IN tt_bool_t brute)
{
    tt_lnode_t *node;

    TT_ASSERT(slab != NULL);

    // check whether all objects has been released
    if (!brute) {
        node = tt_list_head(&slab->area_list);
        while (node != NULL) {
            __slab_area_t *area = TT_CONTAINER(node, __slab_area_t, node);
            node = node->next;

            if (area->ref_counter != 0) {
                TT_FATAL("pages[%p] is still being referenced[%d]",
                         area,
                         area->ref_counter);
                return;
            }
        }
    }

    // free all pages
    node = tt_list_head(&slab->area_list);
    while (node != NULL) {
        __slab_area_t *area = TT_CONTAINER(node, __slab_area_t, node);
        node = node->next;

        tt_page_free(area, slab->area_size);
    }

    tt_spinlock_destroy(&slab->lock);
}

void tt_slab_attr_default(OUT tt_slab_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    // by default, buffer 16 object
    attr->objnum_per_expand = 16;

    // no need do alignment
    attr->hwcache_align = TT_FALSE;
}

void *tt_slab_alloc_tag(IN tt_slab_t *slab
#ifdef TT_MEMORY_TAG_ENABLE
                        ,
                        IN const tt_char_t *func,
                        IN tt_u32_t line
#endif
                        )
{
    tt_u8_t *obj;

    TT_ASSERT(slab != NULL);

    tt_spinlock_acquire(&slab->lock);

    // allocate
    obj = (tt_u8_t *)tt_list_pophead(&slab->free_obj_list);
    if (obj == NULL) {
        __slab_expand(slab);
        obj = (tt_u8_t *)tt_list_pophead(&slab->free_obj_list);
    }

    if (obj != NULL) {
        __slab_objtag_t *objtag = __SLAB_OBJTAG(obj);
        __slab_area_t *area = objtag->area;
        TT_ASSERT(slab == area->parent_slab);

#ifdef TT_MEMORY_TAG_ENABLE
        objtag->func = func;
        objtag->line = line;
#endif

        ++area->ref_counter;
        TT_ASSERT(area->ref_counter > 0);
    } else {
        TT_ERROR("fail to allocate object from slab");
    }

    tt_spinlock_release(&slab->lock);
    return obj;
}

void tt_slab_free(IN void *obj)
{
    __slab_objtag_t *objtag;
    __slab_area_t *area;
    tt_slab_t *slab;

    if (obj == NULL) {
        TT_WARN("releasing NULL pointer to slab cache");
        return;
    }

    objtag = __SLAB_OBJTAG(obj);
    area = objtag->area;
    slab = area->parent_slab;

    tt_spinlock_acquire(&slab->lock);

#ifdef TT_MEMORY_TAG_ENABLE
    objtag->func = NULL;
#endif

    // release page reference
    --area->ref_counter;
    TT_ASSERT(area->ref_counter >= 0);

    // free to slab
    tt_lnode_init((tt_lnode_t *)obj);
    tt_list_addhead(&slab->free_obj_list, (tt_lnode_t *)obj);

    tt_spinlock_release(&slab->lock);
}

tt_result_t __slab_component_init(IN tt_component_t *comp,
                                  IN tt_profile_t *profile)
{
    // area descriptor size
    tt_s_areadesc_size = sizeof(__slab_area_t);
    TT_U32_ALIGN_INC_CPU(tt_s_areadesc_size);
    TT_U32_ALIGN_INC_CACHE(tt_s_areadesc_size);

    // obj tag size
    tt_s_objtag_size = sizeof(__slab_objtag_t);
    TT_U32_ALIGN_INC_CPU(tt_s_objtag_size);

    return TT_SUCCESS;
}

tt_result_t __slab_attr_check(IN tt_slab_attr_t *attr)
{
    if (attr->objnum_per_expand <= 0) {
        TT_ERROR("invalid objnum_per_expand: %d", attr->objnum_per_expand);
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t __slab_expand(IN tt_slab_t *slab)
{
    __slab_area_t *area;
    tt_u8_t *pos, *end;
    tt_u32_t obj_size;

    area = (__slab_area_t *)tt_page_alloc(slab->area_size);
    if (area == NULL) {
        TT_ERROR("fail to alloc slab area");
        return TT_FAIL;
    }
    tt_memset(area, 0, sizeof(__slab_area_t));

    // init slab area
    tt_lnode_init(&area->node);
    area->parent_slab = slab;
    area->ref_counter = 0;

    tt_list_addtail(&slab->area_list, &area->node);

    // pass area size, do not use sizeof(__slab_area_t)
    pos = TT_PTR_INC(tt_u8_t, area, tt_s_areadesc_size);
    end = TT_PTR_INC(tt_u8_t, area, slab->area_size);
    obj_size = slab->obj_size;

    // split left spaces into objects
    while (pos + obj_size <= end) {
        __slab_objtag_t *objtag;
        tt_lnode_t *node;

        objtag = (__slab_objtag_t *)pos;
        objtag->area = area;
#ifdef TT_MEMORY_TAG_ENABLE
        objtag->func = NULL;
#endif

        node = TT_PTR_INC(tt_lnode_t, objtag, sizeof(__slab_objtag_t));
        tt_lnode_init(node);
        tt_list_addtail(&slab->free_obj_list, node);

        pos += obj_size;
    }

    return TT_SUCCESS;
}
