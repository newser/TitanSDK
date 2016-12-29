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

#include <memory/tt_memory_pool.h>

#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <memory/tt_page.h>
#include <misc/tt_util.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#if 1
#define TT_ASSERT_MP TT_ASSERT
#else
#define TT_ASSERT_MP(...)
#endif

// ========================================
// mem pool
// ========================================

#define __MP_CAN_EXPAND(pool)                                                  \
    (((pool)->attr.max_area_num == TT_MEMPOOL_AREA_NOLIMIT) ||                 \
     (tt_list_count(&(pool)->area_list) < (pool)->attr.max_area_num))

// ========================================
// mem area
// ========================================

#define __AREA_FIRST_BLK(area) ((__mp_block_t *)((area)->begin))
// note "end block" is a fake block following the area
#define __AREA_END_BLK(area) ((__mp_block_t *)((area)->end))

// ========================================
// mem block
// ========================================

#define __BLK_STATUS_ALLOCATED (1 << 0)
#define __BLK_STATUS_MASK (~(__BLK_STATUS_ALLOCATED))

#define __BLK_MARK_ALLOCATED(block) ((block)->size |= __BLK_STATUS_ALLOCATED)
#define __BLK_MARK_FREE(block) ((block)->size &= ~(__BLK_STATUS_ALLOCATED))

#define __BLK_IS_ALLOCATED(block) ((block)->size & __BLK_STATUS_ALLOCATED)
#define __BLK_IS_FREE(block) (!(__BLK_IS_ALLOCATED((block))))

#define __BLK_SIZE_MASK (~3)
#define __BLK_PREV_SIZE(block) ((block)->prev_size &= __BLK_SIZE_MASK)
#define __BLK_SIZE(block) ((block)->size & __BLK_SIZE_MASK)
#define __BLK_SIZE_SET(dst_size, src_size)                                     \
    do {                                                                       \
        (dst_size) &= ~__BLK_SIZE_MASK;                                        \
        (dst_size) |= ((src_size)&__BLK_SIZE_MASK);                            \
    } while (0)
#define __BLK_SIZE_ADD(dst_size, add_size)                                     \
    do {                                                                       \
        (dst_size) += ((add_size)&__BLK_SIZE_MASK);                            \
    } while (0)

#define __BLK_PREV(block)                                                      \
    TT_PTR_DEC(__mp_block_t, (block), __BLK_PREV_SIZE((block)))
#define __BLK_NEXT(block) TT_PTR_INC(__mp_block_t, block, __BLK_SIZE((block)))

#define __BLK_SPLIT(block, alloc_size)                                         \
    TT_PTR_INC(__mp_block_t, (block), (alloc_size))

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

struct __mp_block_s;

typedef struct
{
    tt_lnode_t node;
    void *begin;
    void *end; // addr of the first byte exceeding area

    tt_mempool_t *parent_pool;
    tt_s32_t ref_counter;
    struct __mp_block_s *last_block;
    tt_u32_t total_free_size;
} __mp_area_t;

typedef struct __mp_block_s
{
    // total size of previous block including overheads
    //  - if allocated, it includes size of overheads
    //  - if free, total size of available segment
    tt_u32_t prev_size;

    // total size of this block including overheads
    //  - if allocated, it includes size of overheads
    //  - if free, total size of available segment
    tt_u32_t size;
    // we have guranteed that the tt_g_cpu_align_order >= 2,
    // so the least significant 2 bits of the prev_size/size can be reused

    __mp_area_t *area;

#ifdef TT_MEMORY_TAG_ENABLE
    const tt_char_t *func;
    tt_u32_t line;
#endif
} __mp_block_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_u32_t tt_s_areadesc_size;

tt_u32_t tt_s_blockdesc_size;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __mp_component_init(IN tt_component_t *comp,
                                       IN tt_profile_t *profile);

static __mp_block_t *__mp_alloc(IN tt_mempool_t *mp, IN tt_u32_t alloc_size);

static __mp_block_t *__mp_alloc_in_area(IN __mp_area_t *area, IN tt_u32_t size);

static __mp_area_t *__mp_expand(IN tt_mempool_t *mp);

static void __block_alloc_adjust(IN __mp_area_t *area,
                                 IN __mp_block_t *block,
                                 IN tt_u32_t size);

static void __mp_free_in_area(IN __mp_area_t *area, IN __mp_block_t *block);

static void __mp_area_stat_show(IN __mp_area_t *area);

static tt_result_t __mp_expensive_check(IN tt_mempool_t *mp);

static tt_result_t __area_expensive_check(IN __mp_area_t *area);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_mempool_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __mp_component_init,
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

tt_result_t tt_mempool_create(IN tt_mempool_t *mp,
                              IN tt_u32_t max_block_size,
                              IN tt_mempool_attr_t *attr)
{
    tt_u32_t area_size;

    TT_ASSERT(mp != NULL);

    tt_memset(mp, 0, sizeof(tt_mempool_t));

    if (attr != NULL) {
        tt_memcpy(&mp->attr, attr, sizeof(tt_mempool_attr_t));
    } else {
        tt_mempool_attr_default(&mp->attr);
    }

    area_size = tt_s_areadesc_size + max_block_size;
    TT_U32_ALIGN_INC_PAGE(area_size);
    mp->area_size = area_size;

    tt_list_init(&mp->area_list);

    mp->last_area = NULL;

    if (mp->attr.sync && !TT_OK(tt_spinlock_create(&mp->lock, NULL))) {
        TT_ERROR("fail to create mp page list lock");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_mempool_destroy(IN tt_mempool_t *mp, IN tt_bool_t force)
{
    tt_lnode_t *node;

    TT_ASSERT(mp != NULL);

    // check whether all blocks has been returned
    if (!force) {
        node = tt_list_head(&mp->area_list);
        while (node != NULL) {
            __mp_area_t *area = TT_CONTAINER(node, __mp_area_t, node);
            node = node->next;

            if (area->ref_counter != 0) {
                TT_ERROR("area[%p] is still being referenced[%d]",
                         area,
                         area->ref_counter);
                return;
            }
        }
    }

    // free all pages
    node = tt_list_head(&mp->area_list);
    while (node != NULL) {
        __mp_area_t *area = TT_CONTAINER(node, __mp_area_t, node);
        node = node->next;

        tt_page_free(area, mp->area_size);
    }
}

void tt_mempool_attr_default(OUT tt_mempool_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->max_area_num = TT_MEMPOOL_AREA_NOLIMIT;

    attr->hwcache_align = TT_FALSE;

    attr->sync = TT_FALSE;
}

void *tt_mempool_alloc_tag(IN tt_mempool_t *mp,
                           IN tt_u32_t size
#ifdef TT_MEMORY_TAG_ENABLE
                           ,
                           IN const tt_char_t *func,
                           IN tt_u32_t line
#endif
                           )
{
    __mp_block_t *block;

    TT_ASSERT(mp != NULL);

    if (size == 0) {
        return NULL;
    }

    // calc alloc size
    size += tt_s_blockdesc_size;
    TT_U32_ALIGN_INC_CPU(size);
    if (mp->attr.hwcache_align) {
        TT_U32_ALIGN_INC_CACHE(size);
    }

    if (size > (mp->area_size - tt_s_areadesc_size)) {
        TT_ERROR("required size[%d] is larger than area size[%d]",
                 size,
                 mp->area_size - tt_s_areadesc_size);
        return NULL;
    }

    // alloc
    if (mp->attr.sync) {
        tt_spinlock_acquire(&mp->lock);
    }
    block = __mp_alloc(mp, size);
    // TT_ASSERT(TT_OK(__mp_expensive_check(mp)));
    if (mp->attr.sync) {
        tt_spinlock_release(&mp->lock);
    }
    if (block == NULL) {
        TT_ERROR("fail to allocate memory from pool");
        return NULL;
    }

#ifdef TT_MEMORY_TAG_ENABLE
    block->func = func;
    block->line = line;
#endif

    return TT_PTR_INC(void, block, tt_s_blockdesc_size);
}

void tt_mempool_free(IN void *ptr)
{
    __mp_block_t *block;
    __mp_area_t *area;
    tt_mempool_t *mp;

    if (ptr == NULL) {
        TT_WARN("freeing null pointer");
        return;
    }

    block = TT_PTR_DEC(__mp_block_t, ptr, tt_s_blockdesc_size);
    TT_ASSERT(__BLK_IS_ALLOCATED(block));

    area = block->area;

    mp = area->parent_pool;
    TT_ASSERT(mp != NULL);

    // free
    if (mp->attr.sync) {
        tt_spinlock_acquire(&mp->lock);
    }
    __mp_free_in_area(area, block);
    // TT_ASSERT(TT_OK(__mp_expensive_check(mp)));
    if (mp->attr.sync) {
        tt_spinlock_release(&mp->lock);
    }
}

void tt_mempool_show(IN tt_mempool_t *mp)
{
    tt_lnode_t *node;

    if (mp->attr.sync) {
        tt_spinlock_acquire(&mp->lock);
    }

    TT_INFO("-------------------------------------");
    TT_INFO("[Memory Pool] statistic");

    TT_INFO("- pool:");
    TT_INFO("    addr: %p", mp);
    TT_INFO("    area number: %d", tt_list_count(&mp->area_list));
    TT_INFO("    area size: %d", mp->area_size);

    node = tt_list_head(&mp->area_list);
    while (node != NULL) {
        __mp_area_t *area = TT_CONTAINER(node, __mp_area_t, node);
        node = node->next;

        __mp_area_stat_show(area);
    }

    TT_INFO("-------------------------------------");

    if (mp->attr.sync) {
        tt_spinlock_release(&mp->lock);
    }
}

tt_result_t __mp_component_init(IN tt_component_t *comp,
                                IN tt_profile_t *profile)
{
    // area descriptor size
    tt_s_areadesc_size = sizeof(__mp_area_t);
    TT_U32_ALIGN_INC_CPU(tt_s_areadesc_size);
    TT_U32_ALIGN_INC_CACHE(tt_s_areadesc_size);

    // block descriptor size
    tt_s_blockdesc_size = sizeof(__mp_block_t);
    TT_U32_ALIGN_INC_CPU(tt_s_blockdesc_size);

    return TT_SUCCESS;
}

__mp_block_t *__mp_alloc(IN tt_mempool_t *mp, IN tt_u32_t size)
{
    __mp_area_t *area;
    __mp_block_t *block;
    tt_lnode_t *node;

    // alloc from last area to tail
    node = mp->last_area;
    while (node != NULL) {
        area = TT_CONTAINER(node, __mp_area_t, node);
        node = node->next;

        block = __mp_alloc_in_area(area, size);
        if (block != NULL) {
            mp->last_area = node;
            return block;
        }
    }
    // alloc from head area to last area
    node = tt_list_head(&mp->area_list);
    while (node != mp->last_area) {
        area = TT_CONTAINER(node, __mp_area_t, node);
        node = node->next;

        block = __mp_alloc_in_area(area, size);
        if (block != NULL) {
            mp->last_area = node;
            return block;
        }
    }

    // expand
    if (!__MP_CAN_EXPAND(mp)) {
        TT_WARN("pool size has reached limitation");
        return NULL;
    }
    area = __mp_expand(mp);
    if (area == NULL) {
        TT_ERROR("pool expanding fails");
        return NULL;
    }
    TT_ASSERT_MP(area->node.lst == &mp->area_list);
    mp->last_area = node;

    return __mp_alloc_in_area(area, size);
}

__mp_block_t *__mp_alloc_in_area(IN __mp_area_t *area, IN tt_u32_t size)
{
    __mp_block_t *first_blk = __AREA_FIRST_BLK(area);
    __mp_block_t *end_blk = __AREA_END_BLK(area);
    __mp_block_t *cur_blk;

    // size has included overheads
    if (size > area->total_free_size) {
        return NULL;
    }

    cur_blk = area->last_block;
    do {
        TT_ASSERT_MP(cur_blk >= first_blk);
        TT_ASSERT_MP(cur_blk < end_blk);

        if (__BLK_IS_FREE(cur_blk) && (size <= __BLK_SIZE(cur_blk))) {
            __block_alloc_adjust(area, cur_blk, size);
            return cur_blk;
        }

        // next block
        cur_blk = __BLK_NEXT(cur_blk);
        TT_ASSERT_MP(cur_blk <= end_blk);
        if (cur_blk == end_blk) {
            cur_blk = first_blk;
        }

        // each time doing below compare, no block has been adjusted, so
        // we can use area->last_block as the ending mark
    } while (cur_blk != area->last_block);
    return NULL;
}

__mp_area_t *__mp_expand(IN tt_mempool_t *mp)
{
    __mp_area_t *new_area;
    __mp_block_t *first_blk;

    new_area = tt_page_alloc(mp->area_size);
    if (new_area == NULL) {
        TT_ERROR("fail to alloc new area");
        return NULL;
    }

    // init first block
    first_blk = TT_PTR_INC(__mp_block_t, new_area, tt_s_areadesc_size);
    first_blk->prev_size = 0;
    first_blk->size = mp->area_size - tt_s_areadesc_size;
    first_blk->area = new_area;

    // init area
    tt_lnode_init(&new_area->node);
    new_area->begin = first_blk;
    new_area->end = TT_PTR_INC(void, new_area, mp->area_size);

    new_area->parent_pool = mp;
    new_area->ref_counter = 0;
    new_area->last_block = first_blk;
    new_area->total_free_size = first_blk->size;

    // add area to pool
    tt_list_push_tail(&mp->area_list, &new_area->node);

    return new_area;
}

void __block_alloc_adjust(IN __mp_area_t *area,
                          IN __mp_block_t *block,
                          IN tt_u32_t size)
{
    tt_u32_t left_size = block->size - size;
    __mp_block_t *next_blk = __BLK_NEXT(block);
    __mp_block_t *end_blk = __AREA_END_BLK(area);

    TT_ASSERT_MP(next_blk <= end_blk);
    TT_ASSERT_MP((next_blk == end_blk) || __BLK_IS_ALLOCATED(next_blk));

    // update block and neighbours
    if (left_size > tt_s_blockdesc_size) {
        __mp_block_t *new_blk;

        // allocated block
        block->size = size;

        // new block
        new_blk = __BLK_SPLIT(block, size);
        new_blk->prev_size = size;
        new_blk->size = left_size;
        new_blk->area = area;

        if (next_blk != end_blk) {
            next_blk->prev_size = new_blk->size;
        }

        area->last_block = new_blk;
    } else {
        area->last_block = block;
    }
    __BLK_MARK_ALLOCATED(block);

    // update area
    ++area->ref_counter;
    TT_ASSERT_MP(area->ref_counter >= 0);
    area->total_free_size -= __BLK_SIZE(block);
}

void __mp_free_in_area(IN __mp_area_t *area, IN __mp_block_t *block)
{
    __mp_block_t *first_blk = __AREA_FIRST_BLK(area);
    __mp_block_t *end_blk = __AREA_END_BLK(area);

    __mp_block_t *next_blk;
    tt_u32_t freed_size = __BLK_SIZE(block);

    TT_ASSERT_MP(block >= first_blk);
    TT_ASSERT_MP(block < end_blk);

    __BLK_MARK_FREE(block);
    area->last_block = block;

    // adjust next blocks
    next_blk = __BLK_NEXT(block);
    TT_ASSERT_MP(next_blk <= end_blk);
    if ((next_blk != end_blk) && (__BLK_IS_FREE(next_blk))) {
        __mp_block_t *next_next_block;

        // next block is free, need merge
        block->size += next_blk->size;

        // update next next block
        next_next_block = __BLK_NEXT(next_blk);
        TT_ASSERT_MP(next_next_block <= end_blk);
        if (next_next_block != end_blk) {
            TT_ASSERT_MP(__BLK_IS_ALLOCATED(next_next_block));
            next_next_block->prev_size = block->size;
        }

        // no need to change area->last_block
    }

    // adjust previous blocks
    if (block != first_blk) {
        __mp_block_t *prev_block = __BLK_PREV(block);
        TT_ASSERT_MP(prev_block >= first_blk);

        if (__BLK_IS_FREE(prev_block)) {
            // previous block is free, need merge
            prev_block->size += block->size;

            // update neighbor's neighbor
            next_blk = __BLK_NEXT(prev_block);
            TT_ASSERT_MP(next_blk <= end_blk);
            if (next_blk != end_blk) {
                TT_ASSERT_MP(__BLK_IS_ALLOCATED(next_blk));
                next_blk->prev_size = prev_block->size;
            }

            // MUST change area->last_block!!! as "block" disappeared
            area->last_block = prev_block;
        }
    }

    // update area
    --area->ref_counter;
    TT_ASSERT_MP(area->ref_counter >= 0);
    area->total_free_size += freed_size;
}

void __mp_area_stat_show(IN __mp_area_t *area)
{
    __mp_block_t *first_blk = __AREA_FIRST_BLK(area);
    __mp_block_t *end_blk = __AREA_END_BLK(area);
    __mp_block_t *cur_blk = first_blk;

    TT_INFO("- area:");
    while (cur_blk < end_blk) {
        __mp_block_t *next_blk = __BLK_NEXT(cur_blk);

        if (__BLK_IS_ALLOCATED(cur_blk)) {
#ifdef TT_MEMORY_TAG_ENABLE
            TT_INFO("    [A] %p - %p(%d bytes), by [%s:%d]",
                    TT_PTR_INC(void, cur_blk, tt_s_blockdesc_size),
                    TT_PTR_DEC(void, next_blk, 1),
                    __BLK_SIZE(cur_blk) - tt_s_blockdesc_size,
                    cur_blk->func,
                    cur_blk->line);
#else
            TT_INFO("    [A] %p - %p(%d bytes)",
                    TT_PTR_INC(void, cur_blk, tt_s_blockdesc_size),
                    TT_PTR_DEC(void, next_blk, 1),
                    __BLK_SIZE(cur_blk) - tt_s_blockdesc_size);
#endif
        } else {
            TT_INFO("    [F] %p - %p(%d bytes)",
                    TT_PTR_INC(void, cur_blk, tt_s_blockdesc_size),
                    TT_PTR_DEC(void, next_blk, 1),
                    __BLK_SIZE(cur_blk) - tt_s_blockdesc_size);
        }

        cur_blk = next_blk;
    }
    TT_ASSERT_MP(cur_blk == end_blk);
}

tt_result_t __mp_expensive_check(IN tt_mempool_t *mp)
{
    tt_lnode_t *node;

    node = tt_list_head(&mp->area_list);
    while (node != NULL) {
        __mp_area_t *area = TT_CONTAINER(node, __mp_area_t, node);
        node = node->next;

        if (!TT_OK(__area_expensive_check(area))) {
            return TT_FAIL;
        }
    }

    return TT_SUCCESS;
}

tt_result_t __area_expensive_check(IN __mp_area_t *area)
{
    __mp_block_t *first_blk = __AREA_FIRST_BLK(area);
    __mp_block_t *end_blk = __AREA_END_BLK(area);
    __mp_block_t *cur_blk = first_blk;

    while (cur_blk != end_blk) {
        __mp_block_t *next_blk = __BLK_NEXT(cur_blk);
        if (next_blk > end_blk) {
            TT_ERROR("block[%p] size error", cur_blk);
            goto check_fail;
        }

        if (__BLK_IS_FREE(cur_blk)) {
            if ((next_blk != end_blk) && __BLK_IS_FREE(next_blk)) {
                TT_ERROR("block[%p]'s next should be allocated", cur_blk);
                goto check_fail;
            }
        }

        cur_blk = next_blk;
    }

    return TT_SUCCESS;

check_fail:
    __mp_area_stat_show(area);
    return TT_FAIL;
}
