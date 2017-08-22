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

#include <unit_test/tt_unit_test.h>

#include <algorithm/ptr/tt_ptr_heap.h>
#include <algorithm/tt_algorithm_def.h>
#include <algorithm/tt_red_black_tree.h>
#include <time/tt_time_reference.h>

// portlayer header files
#include <tt_cstd_api.h>

#include <time.h>
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

// === routine declarations ================
TT_TEST_ROUTINE_DECLARE(case_ptrheap_basic)
TT_TEST_ROUTINE_DECLARE(case_ptrheap_correct)
TT_TEST_ROUTINE_DECLARE(case_ptrheap_perf)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(heap_case)

TT_TEST_CASE("case_ptrheap_basic",
             "testing ptr heap basic api",
             case_ptrheap_basic,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("case_ptrheap_correct",
                 "testing ptr heap correctness",
                 case_ptrheap_correct,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_ptrheap_perf",
                 "testing ptr heap performance",
                 case_ptrheap_perf,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(heap_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(ALG_UT_HEAP, 0, heap_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(name)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(case_ptrheap_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_ptrheap_t ph;
    tt_ptrheap_attr_t attr;
    tt_result_t ret;
    tt_u32_t h, h0;

    TT_TEST_CASE_ENTER()
    // test start

    tt_ptrheap_attr_default(&attr);

    tt_ptrheap_init(&ph, NULL, NULL);
    TT_UT_EQUAL(tt_ptrheap_pop(&ph), NULL, "");
    TT_UT_EQUAL(tt_ptrheap_head(&ph), NULL, "");
    TT_UT_EQUAL(tt_ptrheap_count(&ph), 0, "");
    TT_UT_EQUAL(tt_ptrheap_empty(&ph), TT_TRUE, "");
    tt_ptrheap_clear(&ph);
    TT_UT_EQUAL(tt_ptrheap_contain(&ph, (tt_ptr_t)0), TT_FALSE, "");
    tt_ptrheap_destroy(&ph);

    tt_ptrheap_init(&ph, NULL, &attr);

    ret = tt_ptrheap_add(&ph, (tt_ptr_t)1, &h);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(h, 0, "");
    TT_UT_EQUAL(tt_ptrheap_head(&ph), (tt_ptr_t)1, "");
    TT_UT_EQUAL(tt_ptrheap_count(&ph), 1, "");
    TT_UT_EQUAL(tt_ptrheap_empty(&ph), TT_FALSE, "");
    TT_UT_EQUAL(tt_ptrheap_contain(&ph, (tt_ptr_t)1), TT_TRUE, "");
    TT_UT_EQUAL(tt_ptrheap_pop(&ph), (tt_ptr_t)1, "");
    TT_UT_EQUAL(h, TT_POS_NULL, "");
    TT_UT_EQUAL(tt_ptrheap_pop(&ph), NULL, "");
    TT_UT_EQUAL(h, TT_POS_NULL, "");

    ret = tt_ptrheap_add(&ph, NULL, NULL);
    TT_UT_FAIL(ret, "");

    ret = tt_ptrheap_add(&ph, (tt_ptr_t)3, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_ptrheap_add(&ph, (tt_ptr_t)2, &h);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_ptrheap_add(&ph, (tt_ptr_t)1, &h0);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_ptrheap_add(&ph, (tt_ptr_t)3, &h);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_ptrheap_add(&ph, (tt_ptr_t)~0, &h);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_ptrheap_add(&ph, (tt_ptr_t)~0, &h);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    TT_UT_EQUAL(tt_ptrheap_count(&ph), 6, "");
    TT_UT_EQUAL(tt_ptrheap_empty(&ph), TT_FALSE, "");
    TT_UT_EQUAL(tt_ptrheap_contain(&ph, (tt_ptr_t)~0), TT_TRUE, "");

    TT_UT_EQUAL(tt_ptrheap_head(&ph), (tt_ptr_t)~0, "");
    TT_UT_EQUAL(tt_ptrheap_pop(&ph), (tt_ptr_t)~0, "");
    TT_UT_EQUAL(tt_ptrheap_pop(&ph), (tt_ptr_t)~0, "");
    TT_UT_EQUAL(tt_ptrheap_pop(&ph), (tt_ptr_t)3, "");

    tt_ptrheap_remove(&ph, h0);
    TT_UT_EQUAL(tt_ptrheap_count(&ph), 2, "");
    TT_UT_EQUAL(tt_ptrheap_head(&ph), (tt_ptr_t)3, "");

    tt_ptrheap_destroy(&ph);

    // test end
    TT_TEST_CASE_LEAVE()
}

#define __t2_num 100000
struct trb_t
{
    tt_rbnode_t n;
    int v;
} trb[__t2_num];

static tt_s32_t rb_comparer(IN void *l, IN void *r)
{
    struct trb_t *ll = (struct trb_t *)l;
    struct trb_t *rr = (struct trb_t *)r;
    if (ll->v < rr->v)
        return -1;
    else if (ll->v == rr->v)
        return 0;
    else
        return 1;
}

static tt_s32_t rb_key_comparer(IN void *n,
                                IN tt_u8_t *key,
                                IN tt_u32_t key_len)
{
    struct trb_t *ll = (struct trb_t *)n;
    if (ll->v < *(int *)key)
        return -1;
    else if (ll->v == *(int *)key)
        return 0;
    else
        return 1;
}

static tt_u32_t _dh[__t2_num];

TT_TEST_ROUTINE_DEFINE(case_ptrheap_correct)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_ptrheap_t ph;
    tt_ptrheap_attr_t attr;
    tt_rbtree_attr_t rbattr;

    tt_rbtree_t rbt;
    int i;

    tt_s64_t begin, end;
    tt_s32_t t1, t2;
    unsigned int seed = (tt_u32_t)time(NULL);

    TT_TEST_CASE_ENTER()
    // test start

    // create ptr heap
    tt_ptrheap_attr_default(&attr);

    tt_ptrheap_init(&ph, NULL, NULL);

    // create a rbtree
    tt_rbtree_attr_default(&rbattr);
    rbattr.cmpkey = rb_key_comparer;
    tt_rbtree_init(&rbt, &rbattr);

    seed = 1413209774;
    srand(seed);
    for (i = 0; i < sizeof(trb) / sizeof(struct trb_t); ++i) {
        trb[i].v = rand();
        tt_rbnode_init(&trb[i].n);
    }

    begin = tt_time_ref();
    for (i = 0; i < sizeof(trb) / sizeof(struct trb_t); ++i) {
        tt_rbtree_add(&rbt, (tt_u8_t *)&trb[i].v, sizeof(trb[i].v), &trb[i].n);
    }
    end = tt_time_ref();
    t1 = (tt_s32_t)tt_time_ref2ms(end - begin);

    begin = tt_time_ref();
    for (i = 0; i < sizeof(trb) / sizeof(struct trb_t); ++i) {
        tt_ptrheap_add(&ph, (tt_ptr_t)(tt_uintptr_t)trb[i].v, NULL);
    }
    end = tt_time_ref();
    t2 = (tt_s32_t)tt_time_ref2ms(end - begin);

    TT_RECORD_INFO("insert, rbtree [%d]ms, heap [%d]ms", t1, t2);

    // cmp
    for (i = 0; i < sizeof(trb) / sizeof(struct trb_t); ++i) {
        tt_rbnode_t *pn = tt_rbtree_max(&rbt);
        int pv = (int)(tt_uintptr_t)tt_ptrheap_head(&ph);
        TT_UT_EQUAL(((struct trb_t *)pn)->v, pv, "");

        tt_rbtree_remove(&rbt, pn);
        tt_ptrheap_pop(&ph);
    }

    TT_UT_EQUAL(tt_ptrheap_head(&ph), NULL, "");
    TT_UT_EQUAL(tt_ptrheap_pop(&ph), NULL, "");
    TT_UT_EQUAL(ph.count, 0, "");

    // test remove
    for (i = 0; i < sizeof(trb) / sizeof(struct trb_t); ++i) {
        tt_ptrheap_add(&ph, (tt_ptr_t)(tt_uintptr_t)trb[i].v, &_dh[i]);
    }
    for (i = 0; i < sizeof(trb) / sizeof(struct trb_t); ++i) {
        trb[i].v = rand();
        tt_ptrheap_fix(&ph, _dh[i]);
    }
    for (i = 0; i < sizeof(trb) / sizeof(struct trb_t); ++i) {
        tt_ptrheap_remove(&ph, _dh[i]);
        if (_dh[i] != TT_POS_NULL) {
            TT_INFO("_dh[%d]: %u", i, _dh[i]);
        }
        TT_UT_EQUAL(_dh[i], TT_POS_NULL, "");
    }

    tt_ptrheap_destroy(&ph);

    // test end
    TT_TEST_CASE_LEAVE()
}

//////////////////////////////////////////////////////////////
#define aperf_num 1000000
static int _act[aperf_num];
static int _act_val[aperf_num];
struct trb_t trb2[aperf_num];

void foo1(int v)
{
    int i = v;
    ++i;
}

TT_TEST_ROUTINE_DEFINE(case_ptrheap_perf)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_ptrheap_t ph;
    tt_ptrheap_attr_t attr;

    tt_rbtree_t rbt;
    int i;

    tt_s64_t begin, end;
    tt_s32_t t1, t2;

    TT_TEST_CASE_ENTER()
    // test start

    srand((tt_u32_t)time(NULL));

    // create ptr heap
    tt_ptrheap_attr_default(&attr);
    tt_ptrheap_init(&ph, NULL, NULL);

    // create a rbtree
    tt_rbtree_init(&rbt, NULL);
    for (i = 0; i < sizeof(trb) / sizeof(struct trb_t); ++i) {
        tt_rbnode_init(&trb[i].n);
    }

    // generate action
    for (i = 0; i < aperf_num; ++i) {
        // 0-check 1-insert 2-remove
        _act[i] = rand() % 10;
        if (_act[i] < 6)
            _act[i] = 0; // 60% check
        else if (_act[i] >= 8)
            _act[i] = 1; // 20% insert
        else
            _act[i] = 2; // 20% remove

        _act_val[i] = rand();
    }

    begin = tt_time_ref();
    for (i = 0; i < aperf_num; ++i) {
        if (_act[i] == 0) {
            tt_rbnode_t *n1 = tt_rbtree_max(&rbt);
            if (n1 != NULL)
                foo1(((struct trb_t *)n1)->v);
        } else if (_act[i] == 1) {
            trb2[i].v = _act_val[i];
            tt_rbtree_add(&rbt,
                          (tt_u8_t *)&trb2[i].v,
                          sizeof(trb2[i].v),
                          &trb2[i].n);
        } else {
            tt_rbnode_t *n1 = tt_rbtree_max(&rbt);
            if (n1 != NULL)
                tt_rbtree_remove(&rbt, n1);
        }
    }
    end = tt_time_ref();
    t1 = (tt_s32_t)tt_time_ref2ms(end - begin);

    begin = tt_time_ref();
    for (i = 0; i < sizeof(trb) / sizeof(struct trb_t); ++i) {
        if (_act[i] == 0) {
            int n1 = (int)(tt_uintptr_t)tt_ptrheap_head(&ph);
            if (n1 != 0)
                foo1(n1);
        } else if (_act[i] == 1) {
            tt_ptrheap_add(&ph, (tt_ptr_t)(tt_uintptr_t)_act_val[i], NULL);
        } else {
            tt_ptrheap_pop(&ph);
        }
    }
    end = tt_time_ref();
    t2 = (tt_s32_t)tt_time_ref2ms(end - begin);

    TT_RECORD_INFO("ptr heap: %dms, rbtree: %dms", t2, t1);

    while (!tt_ptrheap_empty(&ph)) {
        tt_ptrheap_pop(&ph);
    }
    tt_ptrheap_destroy(&ph);

    // test end
    TT_TEST_CASE_LEAVE()
}
