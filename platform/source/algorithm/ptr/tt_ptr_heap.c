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

#include <algorithm/ptr/tt_ptr_heap.h>

#include <algorithm/tt_algorithm_def.h>
#include <misc/tt_assert.h>
#include <misc/tt_util.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#ifdef TT_ARRAY_HEAP_SANITY_CHECK
#define __PH_ASSERTION
#define __PH_DO_SANITY_CHECK // expensive check
#endif

#ifdef __PH_ASSERTION
#define TT_ASSERT_PH TT_ASSERT
#else
#define TT_ASSERT_PH(...)
#endif

#ifdef __PH_DO_SANITY_CHECK
#define __PH_SANITY_CHECK(h, i)                                                \
    TT_ASSERT_ALWAYS(TT_OK(__ph_sanity_check((h), (i))))
#else
#define __PH_SANITY_CHECK(h, i)
#endif

#define __PH_PARENT(i) (((i)-1) >> 1)
#define __PH_LEFT(i) (((i) << 1) + 1)
#define __PH_RIGHT(i) (((i) << 1) + 2)

#define __SWAP_PH_NODE(ph, a, b)                                               \
    do {                                                                       \
        if ((a) != (b)) {                                                      \
            tt_phnode_t __tmp;                                                 \
                                                                               \
            __tmp.p = (ph)->node[(a)].p;                                       \
            __tmp.idx = (ph)->node[(a)].idx;                                   \
                                                                               \
            (ph)->node[(a)].p = (ph)->node[(b)].p;                             \
            (ph)->node[(a)].idx = (ph)->node[(b)].idx;                         \
            if ((ph)->node[(a)].idx != NULL) {                                 \
                *((ph)->node[(a)].idx) = a;                                    \
            }                                                                  \
                                                                               \
            (ph)->node[(b)].p = __tmp.p;                                       \
            (ph)->node[(b)].idx = __tmp.idx;                                   \
            if ((ph)->node[(b)].idx != NULL) {                                 \
                *((ph)->node[(b)].idx) = b;                                    \
            }                                                                  \
        }                                                                      \
    } while (0)

#define __COPY_PH_NODE(ph, dst, src)                                           \
    do {                                                                       \
        if ((dst) != (src)) {                                                  \
            (ph)->node[(dst)].p = (ph)->node[(src)].p;                         \
            (ph)->node[(dst)].idx = (ph)->node[(src)].idx;                     \
            if ((ph)->node[(dst)].idx != NULL) {                               \
                *((ph)->node[(dst)].idx) = dst;                                \
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

static void __fix_downward(IN tt_ptrheap_t *ph, IN tt_u32_t idx);

static void __fix_upward(IN tt_ptrheap_t *ph, IN tt_u32_t idx);

static void __build_heap(IN tt_ptrheap_t *ph);

static tt_result_t __ph_sanity_check(IN tt_ptrheap_t *ph, IN tt_u32_t idx);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_ptrheap_init(IN tt_ptrheap_t *ph,
                     IN OPT tt_cmp_t cmp,
                     IN OPT tt_ptrheap_attr_t *attr)
{
    tt_ptrheap_attr_t __attr;

    TT_ASSERT(ph != NULL);

    if (attr == NULL) {
        tt_ptrheap_attr_default(&__attr);
        attr = &__attr;
    }
    TT_ASSERT_ALWAYS(
        !TT_U32_MUL_WOULD_OVFL(sizeof(tt_phnode_t), attr->min_extent_num));
    TT_ASSERT_ALWAYS(
        !TT_U32_MUL_WOULD_OVFL(sizeof(tt_phnode_t), attr->max_extent_num));
    TT_ASSERT_ALWAYS(
        !TT_U32_MUL_WOULD_OVFL(sizeof(tt_phnode_t), attr->max_limit_num));

    ph->p = NULL;
    ph->cmp = TT_COND(cmp != NULL, cmp, tt_cmp_ptr);
    tt_memspg_init(&ph->mspg,
                   attr->min_extent_num * sizeof(tt_phnode_t),
                   attr->max_extent_num * sizeof(tt_phnode_t),
                   attr->max_limit_num * sizeof(tt_phnode_t));
    ph->size = 0;
    ph->capacity = 0;
    ph->count = 0;
}

void tt_ptrheap_destroy(IN tt_ptrheap_t *ph)
{
    TT_ASSERT(ph != NULL);

    tt_ptrheap_clear(ph);

    if (ph->node != NULL) {
        tt_memspg_compress(&ph->mspg, &ph->p, &ph->size, 0);
    }
}

void tt_ptrheap_attr_default(IN tt_ptrheap_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->min_extent_num = 16;
    attr->max_extent_num = 128;
    attr->max_limit_num = 0;
}

tt_result_t __ptrheap_reserve(IN tt_ptrheap_t *ph, IN tt_u32_t count)
{
    TT_DO(tt_memspg_extend(&ph->mspg,
                           &ph->p,
                           &ph->size,
                           (ph->capacity + count) * sizeof(tt_phnode_t)));
    ph->capacity = ph->size / sizeof(tt_phnode_t);

    return TT_SUCCESS;
}

void tt_ptrheap_fix(IN tt_ptrheap_t *ph, IN tt_u32_t idx)
{
    TT_ASSERT_ALWAYS(idx < ph->count);

    __fix_downward(ph, idx);
    __fix_upward(ph, idx);

    __PH_SANITY_CHECK(ph, 0);
}

void tt_ptrheap_clear(IN tt_ptrheap_t *ph)
{
    tt_u32_t i;
    for (i = 0; i < ph->count; ++i) {
        if (ph->node[i].idx != NULL) {
            *(ph->node[i].idx) = TT_POS_NULL;
        }
    }
    ph->count = 0;
}

tt_result_t tt_ptrheap_add(IN tt_ptrheap_t *ph,
                           IN tt_ptr_t p,
                           IN OUT tt_u32_t *idx)
{
    if (p == NULL) {
        return TT_FAIL;
    }

    TT_DO(tt_ptrheap_reserve(ph, 1));

    // put the new node at the end
    ph->node[ph->count].p = p;
    ph->node[ph->count].idx = idx;
    if (idx != NULL) {
        *idx = ph->count;
    }
    ++ph->count;

    __fix_upward(ph, ph->count - 1);

    __PH_SANITY_CHECK(ph, 0);
    return TT_SUCCESS;
}

void tt_ptrheap_remove(IN tt_ptrheap_t *ph, IN tt_u32_t idx)
{
    TT_ASSERT_ALWAYS(idx < ph->count);

    if (ph->node[idx].idx != NULL) {
        *(ph->node[idx].idx) = TT_POS_NULL;
    }

    --ph->count;
    if (idx != ph->count) {
        __COPY_PH_NODE(ph, idx, ph->count);

        __fix_downward(ph, idx);

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
        // which violates ph property: 80 > 50

        __fix_upward(ph, idx);
    }

    __PH_SANITY_CHECK(ph, 0);
}

tt_bool_t tt_ptrheap_contain(IN tt_ptrheap_t *ph, IN tt_ptr_t p)
{
    tt_u32_t i;
    for (i = 0; i < ph->count; ++i) {
        if (ph->cmp(ph->node[i].p, p) == 0) {
            return TT_TRUE;
        }
    }
    return TT_FALSE;
}

tt_ptr_t tt_ptrheap_pop(IN tt_ptrheap_t *ph)
{
    tt_ptr_t head;

    if (ph->count == 0) {
        return NULL;
    }
    head = ph->node[0].p;
    tt_ptrheap_remove(ph, 0);

    return head;
}

void __fix_downward(IN tt_ptrheap_t *ph, IN tt_u32_t idx)
{
    tt_u32_t l, r, max;

make_sub:

    l = __PH_LEFT(idx);
    r = __PH_RIGHT(idx);
    max = idx;

    // find max node
    if ((l < ph->count) && (ph->cmp(ph->node[l].p, ph->node[max].p) > 0)) {
        max = l;
    }
    if ((r < ph->count) && (ph->cmp(ph->node[r].p, ph->node[max].p) > 0)) {
        max = r;
    }

    if (max != idx) {
        __SWAP_PH_NODE(ph, max, idx);

        idx = max;
        goto make_sub;
    }
}

void __fix_upward(IN tt_ptrheap_t *ph, IN tt_u32_t idx)
{
    while (idx > 0) {
        tt_u32_t parent = __PH_PARENT(idx);

        if (ph->cmp(ph->node[parent].p, ph->node[idx].p) >= 0) {
            break;
        }

        __SWAP_PH_NODE(ph, parent, idx);
        idx = parent;
    }
}

void __build_heap(IN tt_ptrheap_t *ph)
{
    tt_u32_t i;

    if (ph->count == 0) {
        return;
    }
    TT_ASSERT_PH(ph->node != NULL);
    TT_ASSERT_PH(ph->capacity > 0);

    // make ph from the parent of last node
    for (i = __PH_PARENT(ph->count - 1); i != ~0; --i) {
        __fix_downward(ph, i);
    }
}

#if 1
#define __ASSERT_PSC TT_ASSERT
#else
#define __ASSERT_PSC(...)
#endif
tt_result_t __ph_sanity_check(IN tt_ptrheap_t *ph, IN tt_u32_t idx)
{
    tt_u32_t l = __PH_LEFT(idx);
    tt_u32_t r = __PH_RIGHT(idx);

    if (l < ph->count) {
        // left child should be less than parent
        if (ph->cmp(ph->node[l].p, ph->node[idx].p) > 0) {
            __ASSERT_PSC(0);
            return TT_FAIL;
        }

        if (!TT_OK(__ph_sanity_check(ph, l))) {
            __ASSERT_PSC(0);
            return TT_FAIL;
        }
    }

    if (r < ph->count) {
        // right child should be less than parent
        if (ph->cmp(ph->node[r].p, ph->node[idx].p) > 0) {
            __ASSERT_PSC(0);
            return TT_FAIL;
        }

        if (!TT_OK(__ph_sanity_check(ph, r))) {
            __ASSERT_PSC(0);
            return TT_FAIL;
        }
    }

    return TT_SUCCESS;
}
