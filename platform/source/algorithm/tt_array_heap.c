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

#include <algorithm/tt_array_heap.h>

#include <memory/tt_memory_alloc.h>
#include <misc/tt_assert.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#ifdef TT_ARRAY_HEAP_SANITY_CHECK
#define __AH_ASSERTION
#define __AH_DO_SANITY_CHECK // expensive check
#endif

#ifdef __AH_ASSERTION
#define TT_ASSERT_AH TT_ASSERT
#else
#define TT_ASSERT_AH(...)
#endif

#ifdef __AH_DO_SANITY_CHECK
#define __AH_SANITY_CHECK(h, i)                                                \
    TT_ASSERT_ALWAYS(TT_OK(__ah_sanity_check((h), (i))))
#else
#define __AH_SANITY_CHECK(h, i)
#endif

#define __AH_PARENT(i) (((i)-1) >> 1)
#define __AH_LEFT(i) (((i) << 1) + 1)
#define __AH_RIGHT(i) (((i) << 1) + 2)

#define __AH_INIT_SIZE(ah)                                                     \
    ((tt_u32_t)(sizeof(tt_ahnode_t) * (ah)->attr.initial_node_num))
#define __AH_SIZE(ah) ((tt_u32_t)(sizeof(tt_ahnode_t) * (ah)->node_num))

#define __SWAP_AH_NODE(aheap, a_idx, b_idx)                                    \
    do {                                                                       \
        if ((a_idx) != (b_idx)) {                                              \
            tt_ahnode_t __tmp;                                                 \
                                                                               \
            __tmp.data = (aheap)->node[(a_idx)].data;                          \
            __tmp.node_idx = (aheap)->node[(a_idx)].node_idx;                  \
                                                                               \
            (aheap)->node[(a_idx)].data = (aheap)->node[(b_idx)].data;         \
            (aheap)->node[(a_idx)].node_idx = (aheap)->node[(b_idx)].node_idx; \
            if ((aheap)->node[(a_idx)].node_idx != NULL) {                     \
                *((aheap)->node[(a_idx)].node_idx) = (tt_s32_t)a_idx;          \
            }                                                                  \
                                                                               \
            (aheap)->node[(b_idx)].data = __tmp.data;                          \
            (aheap)->node[(b_idx)].node_idx = __tmp.node_idx;                  \
            if ((aheap)->node[(b_idx)].node_idx != NULL) {                     \
                *((aheap)->node[(b_idx)].node_idx) = (tt_s32_t)b_idx;          \
            }                                                                  \
        }                                                                      \
    } while (0)

#define __COPY_AH_NODE(aheap, dst_idx, src_idx)                                \
    do {                                                                       \
        if ((dst_idx) != (src_idx)) {                                          \
            (aheap)->node[(dst_idx)].data = (aheap)->node[(src_idx)].data;     \
            (aheap)->node[(dst_idx)].node_idx =                                \
                (aheap)->node[(src_idx)].node_idx;                             \
            if ((aheap)->node[(dst_idx)].node_idx != NULL) {                   \
                *((aheap)->node[(dst_idx)].node_idx) = (tt_s32_t)dst_idx;      \
            }                                                                  \
        }                                                                      \
    } while (0)

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

static void __make_heap(IN tt_arheap_t *heap, IN tt_u32_t node_idx);
static void __build_heap(IN tt_arheap_t *heap);

static tt_result_t __ah_sanity_check(IN tt_arheap_t *heap,
                                     IN tt_u32_t node_idx);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_arheap_create(IN tt_arheap_t *heap,
                             IN tt_cmp_t comparer,
                             IN tt_arheap_attr_t *attr)
{
    TT_ASSERT(heap != NULL);
    TT_ASSERT(comparer != NULL);

    tt_memset(heap, 0, sizeof(tt_arheap_t));

    // allocate node array
    heap->node = NULL;
    heap->node_num = 0;
    heap->tail_idx = 0;

    // comparer
    heap->comparer = comparer;

    // save attributes
    if (attr != NULL) {
        tt_memcpy(&heap->attr, attr, sizeof(tt_arheap_attr_t));
    } else {
        tt_arheap_attr_default(&heap->attr);
    }
    TT_ASSERT(heap->attr.initial_node_num > 0);

    return TT_SUCCESS;
}

tt_result_t tt_arheap_destroy(IN tt_arheap_t *heap)
{
    TT_ASSERT(heap != NULL);

    if (heap->node != NULL) {
        if (!tt_arheap_empty(heap)) {
            TT_ERROR("heap not empty, tail [%d]", heap->tail_idx);
            return TT_FAIL;
        }

        tt_mem_free(heap->node);
    }

    return TT_SUCCESS;
}

void tt_arheap_attr_default(IN tt_arheap_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    // 16 as initial estimation
    attr->initial_node_num = 16;
}

tt_result_t tt_arheap_add(IN tt_arheap_t *heap,
                          IN tt_ptr_t data,
                          OUT tt_s32_t *node_idx)
{
    tt_u32_t i;

    TT_ASSERT(heap != NULL);

    // check if allocated
    if (heap->node == NULL) {
        tt_u32_t allocated = tt_mem_size(__AH_INIT_SIZE(heap));
        void *node_array = tt_mem_alloc(allocated);
        if (node_array == NULL) {
            TT_ERROR("fail to allocate array heap");
            return TT_FAIL;
        }
        TT_ASSERT_AH(__AH_INIT_SIZE(heap) > 0);
        tt_memset(node_array, 0, __AH_INIT_SIZE(heap));

        heap->node = node_array;
        // heap->node_num = heap->attr.initial_node_num;
        heap->node_num = allocated / sizeof(tt_ahnode_t);
        heap->tail_idx = 0;
    }
    TT_ASSERT_AH(heap->node_num != 0);

    // check if should expand
    if (heap->tail_idx == heap->node_num) {
        tt_u32_t allocated = tt_mem_size(__AH_SIZE(heap) * 2);
        void *node_array = tt_mem_alloc(allocated);
        if (node_array == NULL) {
            TT_ERROR("fail to expand array heap");
            return TT_FAIL;
        }
        tt_memset(node_array, 0, __AH_SIZE(heap) * 2);
        tt_memcpy(node_array, heap->node, __AH_SIZE(heap));

        tt_mem_free(heap->node);
        heap->node = node_array;

        // heap->node_num = heap->node_num * 2;
        heap->node_num = allocated / sizeof(tt_ahnode_t);
        // keep heap->tail_idx
    }
    TT_ASSERT_AH(heap->tail_idx < heap->node_num);

    // now insert

    heap->node[heap->tail_idx].data = data;
    heap->node[heap->tail_idx].node_idx = node_idx;
    i = heap->tail_idx;
    ++heap->tail_idx;

    while (i > 0) {
        tt_u32_t parent = __AH_PARENT(i);

        if (heap->comparer(heap->node[parent].data, heap->node[i].data) < 0) {
            // parent < node
            __SWAP_AH_NODE(heap, parent, i);

            i = parent;
        } else {
            // all fixed
            break;
        }
    }

    // the new node is at index i
    if (heap->node[i].node_idx != NULL) {
        *(heap->node[i].node_idx) = (tt_s32_t)i;
    }

    __AH_SANITY_CHECK(heap, 0);
    return TT_SUCCESS;
}

void tt_arheap_remove(IN tt_arheap_t *heap, IN tt_s32_t node_idx)
{
    tt_u32_t idx = (tt_u32_t)node_idx;

    TT_ASSERT(heap != NULL);
    TT_ASSERT(idx < heap->tail_idx);
    // now heap->tail_idx must > 0

    if (heap->node[node_idx].node_idx != NULL) {
        *(heap->node[node_idx].node_idx) = TT_ARHEAP_NODE_NULL;
    }

    --heap->tail_idx;
    if (idx != heap->tail_idx) {
        __COPY_AH_NODE(heap, idx, heap->tail_idx);

        // fix downward
        __make_heap(heap, idx);

        //            100         |
        //           /   \        |
        //          50   90       |
        //         / \   / \      |
        //        10 20 70 80     |
        // when removing the node "20" without upward fixing
        //            100         |
        //           /   \        |
        //          50   90       |
        //         / \   /        |
        //        10 80 70        |
        // which violates heap property: 80 > 50

        // fix upward
        while (idx > 0) {
            tt_u32_t parent = __AH_PARENT(idx);

            if (heap->comparer(heap->node[parent].data, heap->node[idx].data) <
                0) {
                // parent < node
                __SWAP_AH_NODE(heap, parent, idx);

                idx = parent;
            } else {
                // all fixed
                break;
            }
        }
    }

    __AH_SANITY_CHECK(heap, 0);
}

void tt_arheap_fix(IN tt_arheap_t *heap, IN tt_s32_t node_idx)
{
    tt_u32_t idx = (tt_u32_t)node_idx;

    TT_ASSERT(heap != NULL);
    TT_ASSERT(idx < heap->tail_idx);
    // now heap->tail_idx must > 0

    // fix downward
    __make_heap(heap, idx);
    // __make_heap would do adjustment if the node is decreased

    // fix upward
    while (idx > 0) {
        tt_u32_t parent = __AH_PARENT(idx);

        if (heap->comparer(heap->node[parent].data, heap->node[idx].data) < 0) {
            // parent < node
            __SWAP_AH_NODE(heap, parent, idx);

            idx = parent;
        } else {
            // all fixed
            break;
        }
    }

    __AH_SANITY_CHECK(heap, 0);
}

tt_ptr_t tt_arheap_pophead(IN tt_arheap_t *heap)
{
    tt_ptr_t head_data;

    TT_ASSERT(heap != NULL);

    if (heap->tail_idx == 0) {
        return NULL;
    }
    TT_ASSERT_AH(heap->node != NULL);
    TT_ASSERT_AH(heap->node_num > 0);

    head_data = heap->node[0].data;

    if (heap->node[0].node_idx != NULL) {
        *(heap->node[0].node_idx) = TT_ARHEAP_NODE_NULL;
    }

    --heap->tail_idx;
    __COPY_AH_NODE(heap, 0, heap->tail_idx);
    __make_heap(heap, 0);

    __AH_SANITY_CHECK(heap, 0);
    return head_data;
}

void __make_heap(IN tt_arheap_t *heap, IN tt_u32_t node_idx)
{
    tt_u32_t l_idx;
    tt_u32_t r_idx;
    tt_u32_t largest_idx;

make_sub:

    l_idx = __AH_LEFT(node_idx);
    r_idx = __AH_RIGHT(node_idx);
    largest_idx = node_idx;

    // vs left
    if ((l_idx < heap->tail_idx) &&
        (heap->comparer(heap->node[l_idx].data, heap->node[largest_idx].data) >
         0)) {
        // left > largest_idx
        largest_idx = l_idx;
    }

    // vs right
    if ((r_idx < heap->tail_idx) &&
        (heap->comparer(heap->node[r_idx].data, heap->node[largest_idx].data) >
         0)) {
        // right > largest_idx
        largest_idx = r_idx;
    }

    if (largest_idx != node_idx) {
        __SWAP_AH_NODE(heap, largest_idx, node_idx);

        // next loop
        node_idx = largest_idx;
        goto make_sub;
    }
}

void __build_heap(IN tt_arheap_t *heap)
{
    tt_s32_t i;

    if (heap->tail_idx == 0) {
        return;
    }
    TT_ASSERT_AH(heap->node != NULL);
    TT_ASSERT_AH(heap->node_num > 0);

    // make heap from the parent of last node
    for (i = (tt_s32_t)__AH_PARENT(heap->tail_idx - 1); i >= 0; --i) {
        __make_heap(heap, i);
    }
}

#if 1
#define __ASSERT_ACC TT_ASSERT
#else
#define __ASSERT_ACC(...)
#endif
tt_result_t __ah_sanity_check(IN tt_arheap_t *heap, IN tt_u32_t node_idx)
{
    tt_u32_t l_idx = __AH_LEFT(node_idx);
    tt_u32_t r_idx = __AH_RIGHT(node_idx);

    if (l_idx < heap->tail_idx) {
        // left child should be less than parent
        if (heap->comparer(heap->node[l_idx].data, heap->node[node_idx].data) >
            0) {
            __ASSERT_ACC(0);
            return TT_FAIL;
        }

        // left sub tree consistency
        if (!TT_OK(__ah_sanity_check(heap, l_idx))) {
            __ASSERT_ACC(0);
            return TT_FAIL;
        }
    } else {
        // no left child
        return TT_SUCCESS;
    }

    if (r_idx < heap->tail_idx) {
        // right child should be less than parent
        if (heap->comparer(heap->node[r_idx].data, heap->node[node_idx].data) >
            0) {
            __ASSERT_ACC(0);
            return TT_FAIL;
        }

        // right sub tree consistency
        if (!TT_OK(__ah_sanity_check(heap, r_idx))) {
            __ASSERT_ACC(0);
            return TT_FAIL;
        }
    } else {
        // no right child
        return TT_SUCCESS;
    }

    return TT_SUCCESS;
}
