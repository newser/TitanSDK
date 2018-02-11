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

#include <memory/tt_slab.h>

#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <memory/tt_memory_alloc.h>
#include <misc/tt_assert.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

// object format:
// | tag | data |

#define __OBJTAG(obj) TT_PTR_DEC(__slab_objtag_t, obj, tt_s_objtag_size)

#define __MIN_BULK_NUM 16

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef struct
{
    tt_dnode_t node;
    tt_slab_t *slab;
    tt_s32_t ref;
} __slab_frame_t;

typedef struct
{
    __slab_frame_t *frame;

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

static tt_u32_t tt_s_fdesc_size;
static tt_u32_t tt_s_fdesc_size_aligned;

static tt_u32_t tt_s_objtag_size;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __slab_component_init(IN tt_component_t *comp,
                                         IN tt_profile_t *profile);

static void __slab_component_exit(IN tt_component_t *comp);

static tt_result_t __slab_attr_check(IN tt_slab_attr_t *attr);

static tt_result_t __slab_alloc_frame(IN tt_slab_t *slab);

static void __slab_free_frame(IN tt_slab_t *slab, IN __slab_frame_t *frame);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_slab_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __slab_component_init, __slab_component_exit,
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
    tt_slab_attr_t __attr;
    tt_u32_t size, fdesc_size;

    TT_ASSERT(slab != NULL);
    TT_ASSERT(obj_size != 0);

    if (attr == NULL) {
        tt_slab_attr_default(&__attr);
        attr = &__attr;
    }
    if (attr->bulk_num < __MIN_BULK_NUM) {
        attr->bulk_num = __MIN_BULK_NUM;
    }

    tt_dlist_init(&slab->obj_list);
    tt_dlist_init(&slab->frame_list);

    size = obj_size;
    TT_U32_ALIGN_INC_CPU(size);
    // each obj size is at least a tt_dnode_t
    if (size < sizeof(tt_dnode_t)) {
        size = sizeof(tt_dnode_t);
    }
    size += tt_s_objtag_size;
    if (attr->cache_align) {
        TT_U32_ALIGN_INC_CACHE(size);
    }
    slab->obj_size = size;

    slab->obj_num = 0;

    if (TT_U32_MUL_WOULD_OVFL(slab->obj_size, attr->bulk_num)) {
        TT_ERROR("too large obj size or num");
        return TT_FAIL;
    }
    size = slab->obj_size * attr->bulk_num;
    fdesc_size =
        TT_COND(attr->cache_align, tt_s_fdesc_size_aligned, tt_s_fdesc_size);
    if (TT_U32_ADD_WOULD_OVFL(size, fdesc_size)) {
        TT_ERROR("too large obj size or num");
        return TT_FAIL;
    }
    size += fdesc_size;
    if (size >= (1u << 31)) {
        TT_ERROR("too large size");
        return TT_FAIL;
    }
    slab->frame_size = size;

    slab->cache_align = attr->cache_align;

    return TT_SUCCESS;
}

void tt_slab_destroy(IN tt_slab_t *slab)
{
    tt_dnode_t *node;

    TT_ASSERT(slab != NULL);

    node = tt_dlist_head(&slab->frame_list);
    while (node != NULL) {
        __slab_frame_t *frame = TT_CONTAINER(node, __slab_frame_t, node);
        node = node->next;

        // either give a fatal log or an assertion
        if (frame->ref != 0) {
            TT_FATAL("frame[%p] is still being referenced[%d]",
                     frame,
                     frame->ref);
            return;
        }
    }

    node = tt_dlist_head(&slab->frame_list);
    while (node != NULL) {
        __slab_frame_t *frame = TT_CONTAINER(node, __slab_frame_t, node);
        node = node->next;

        tt_free(frame);
    }
}

void tt_slab_attr_default(OUT tt_slab_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    // by default, buffer __MIN_BULK_NUM object
    attr->bulk_num = __MIN_BULK_NUM;

    // no need do alignment
    attr->cache_align = TT_FALSE;
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

    obj = (tt_u8_t *)tt_dlist_pop_head(&slab->obj_list);
    if (obj == NULL) {
        __slab_alloc_frame(slab);
        obj = (tt_u8_t *)tt_dlist_pop_head(&slab->obj_list);
    }

    if (obj != NULL) {
        __slab_objtag_t *otag = __OBJTAG(obj);
        __slab_frame_t *frame = otag->frame;
        TT_ASSERT(slab == frame->slab);

#ifdef TT_MEMORY_TAG_ENABLE
        otag->func = func;
        otag->line = line;
#endif

        ++frame->ref;
    }
    return obj;
}

void tt_slab_free(IN void *obj)
{
    __slab_frame_t *frame;
    tt_slab_t *slab;

    if (obj == NULL) {
        return;
    }

    frame = __OBJTAG(obj)->frame;
    --frame->ref;
    TT_ASSERT(frame->ref >= 0);

    slab = frame->slab;
    tt_dnode_init((tt_dnode_t *)obj);
    tt_dlist_push_head(&slab->obj_list, (tt_dnode_t *)obj);
    ++slab->obj_num;

    // free the frame when:
    //  - all objects belonging to the frame are returned
    //  - the available size in slab is as twice larger than frame size
    if ((frame->ref == 0) &&
        ((slab->obj_num * slab->obj_size) >= (slab->frame_size << 1))) {
        __slab_free_frame(slab, frame);
    }
}

tt_result_t __slab_component_init(IN tt_component_t *comp,
                                  IN tt_profile_t *profile)
{
    // frame descriptor size
    tt_s_fdesc_size = sizeof(__slab_frame_t);

    // frame descriptor size, cache aligned
    tt_s_fdesc_size_aligned = tt_s_fdesc_size;
    TT_U32_ALIGN_INC_CACHE(tt_s_fdesc_size_aligned);

    // obj tag size
    tt_s_objtag_size = sizeof(__slab_objtag_t);

    return TT_SUCCESS;
}

void __slab_component_exit(IN tt_component_t *comp)
{
}

tt_result_t __slab_alloc_frame(IN tt_slab_t *slab)
{
    __slab_frame_t *frame;
    tt_u8_t *pos, *end;
    tt_u32_t obj_size;

    frame = (__slab_frame_t *)tt_malloc(slab->frame_size);
    if (frame == NULL) {
        TT_ERROR("fail to alloc slab frame");
        return TT_FAIL;
    }

    tt_dnode_init(&frame->node);
    frame->slab = slab;
    frame->ref = 0;

    tt_dlist_push_tail(&slab->frame_list, &frame->node);

    pos = TT_PTR_INC(tt_u8_t,
                     frame,
                     TT_COND(slab->cache_align,
                             tt_s_fdesc_size_aligned,
                             tt_s_fdesc_size));
    end = TT_PTR_INC(tt_u8_t, frame, slab->frame_size);
    obj_size = slab->obj_size;
    while ((pos + obj_size) <= end) {
        __slab_objtag_t *otag;
        tt_dnode_t *node;

        otag = (__slab_objtag_t *)pos;
        otag->frame = frame;
#ifdef TT_MEMORY_TAG_ENABLE
        otag->func = NULL;
#endif

        node = TT_PTR_INC(tt_dnode_t, otag, sizeof(__slab_objtag_t));
        tt_dnode_init(node);
        tt_dlist_push_tail(&slab->obj_list, node);

        pos += obj_size;
    }

    return TT_SUCCESS;
}

void __slab_free_frame(IN tt_slab_t *slab, IN __slab_frame_t *frame)
{
    tt_u8_t *pos, *end;
    tt_u32_t obj_size;

    pos = TT_PTR_INC(tt_u8_t,
                     frame,
                     TT_COND(slab->cache_align,
                             tt_s_fdesc_size_aligned,
                             tt_s_fdesc_size));
    end = TT_PTR_INC(tt_u8_t, frame, slab->frame_size);
    obj_size = slab->obj_size;
    while ((pos + obj_size) <= end) {
        __slab_objtag_t *otag = (__slab_objtag_t *)pos;

        TT_ASSERT(otag->frame == frame);
        tt_dlist_remove(&slab->obj_list,
                        TT_PTR_INC(tt_dnode_t, otag, sizeof(__slab_objtag_t)));

        pos += obj_size;
    }

    tt_dlist_remove(&slab->frame_list, &frame->node);
    tt_free(frame);
}
