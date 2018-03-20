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

#include <memory/tt_memory_pool.h>

#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <memory/tt_memory_alloc.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#if 1
#define TT_ASSERT_MP TT_ASSERT
#else
#define TT_ASSERT_MP(...)
#endif

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef struct
{
    tt_snode_t node;
    tt_u8_t *addr;
    tt_u32_t len;
} __mp_frame_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __mp_component_init(IN tt_component_t *comp,
                                       IN tt_profile_t *profile);

static void __mp_component_exit(IN tt_component_t *comp);

static tt_u8_t *__mp_alloc(IN tt_mempool_t *mp, IN tt_u32_t size);

static __mp_frame_t *__mp_expand(IN tt_mempool_t *mp);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_mempool_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __mp_component_init, __mp_component_exit,
    };

    // init component
    tt_component_init(&comp,
                      TT_COMPONENT_MEMORY_POOL,
                      "Memory Pool",
                      NULL,
                      &itf);

    // register component
    tt_component_register(&comp);
}

void tt_mempool_init(IN tt_mempool_t *mp,
                     IN tt_u32_t max_block_size,
                     IN OPT tt_mempool_attr_t *attr)
{
    tt_mempool_attr_t __attr;
    tt_u32_t size;

    TT_ASSERT(mp != NULL);
    TT_ASSERT(max_block_size != 0);

    if (attr == NULL) {
        tt_mempool_attr_default(&__attr);
        attr = &__attr;
    }

    tt_slist_init(&mp->frame_list);
    mp->frame_num = 0;

    size = max_block_size;
    TT_U32_ALIGN_INC_CPU(size);
    mp->frame_size = size;

    size = attr->max_pool_size;
    TT_U32_ALIGN_INC_CPU(size);
    mp->max_frame_num = ((size + mp->frame_size - 1) / mp->frame_size);
}

void tt_mempool_destroy(IN tt_mempool_t *mp)
{
    tt_snode_t *sn;

    TT_ASSERT(mp != NULL);

    sn = tt_slist_head(&mp->frame_list);
    while (sn != NULL) {
        __mp_frame_t *frame = TT_CONTAINER(sn, __mp_frame_t, node);
        sn = sn->next;

        tt_free(frame);
    }
}

void tt_mempool_attr_default(OUT tt_mempool_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->max_pool_size = 0;
}

void *tt_mp_alloc(IN tt_mempool_t *mp, IN tt_u32_t size)
{
    TT_ASSERT(mp != NULL);

    if (size == 0) {
        return NULL;
    }

    TT_U32_ALIGN_INC_CPU(size);
    if (size > mp->frame_size) {
        TT_ERROR("[%d] exceed max block size[%d]", size, mp->frame_size);
        return NULL;
    }

    return __mp_alloc(mp, size);
}

tt_result_t __mp_component_init(IN tt_component_t *comp,
                                IN tt_profile_t *profile)
{
    return TT_SUCCESS;
}

void __mp_component_exit(IN tt_component_t *comp)
{
}

tt_u8_t *__mp_alloc(IN tt_mempool_t *mp, IN tt_u32_t size)
{
    tt_snode_t *sn;
    __mp_frame_t *frame;
    tt_u8_t *p;

    TT_ASSERT_MP(size <= mp->frame_size);
    sn = tt_slist_head(&mp->frame_list);
    if (sn != NULL) {
        frame = TT_CONTAINER(sn, __mp_frame_t, node);
        if ((frame->len < size) && ((frame = __mp_expand(mp)) == NULL)) {
            return NULL;
        }
    } else if ((frame = __mp_expand(mp)) == NULL) {
        return NULL;
    }

    TT_ASSERT_MP((frame != NULL) && (frame->len >= size));
    p = frame->addr;
    frame->addr += size;
    frame->len -= size;
    return p;
}

__mp_frame_t *__mp_expand(IN tt_mempool_t *mp)
{
    __mp_frame_t *frame;

    if ((mp->max_frame_num != 0) && (mp->frame_num >= mp->max_frame_num)) {
        TT_ERROR("exceeding pool size limit");
        return NULL;
    }

    frame = tt_malloc(sizeof(__mp_frame_t) + mp->frame_size);
    if (frame == NULL) {
        TT_ERROR("fail to alloc new frame");
        return NULL;
    }

    tt_snode_init(&frame->node);
    frame->addr = TT_PTR_INC(tt_u8_t, frame, sizeof(__mp_frame_t));
    frame->len = mp->frame_size;

    tt_slist_push_head(&mp->frame_list, &frame->node);
    ++mp->frame_num;

    return frame;
}
