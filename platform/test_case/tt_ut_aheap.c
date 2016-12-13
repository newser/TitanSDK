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

#include "tt_unit_test_case_config.h"
#include <unit_test/tt_unit_test.h>

#include <algorithm/tt_array_heap.h>
#include <algorithm/tt_red_black_tree.h>
#include <timer/tt_time_reference.h>

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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_arheap_basic)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_arheap_correct)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_arheap_perf)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(aheap_case)

TT_TEST_CASE("tt_unit_test_arheap_basic",
             "testing array heap basic api",
             tt_unit_test_arheap_basic,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_arheap_correct",
                 "testing aheap correctness",
                 tt_unit_test_arheap_correct,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_arheap_perf",
                 "testing aheap performance",
                 tt_unit_test_arheap_perf,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(aheap_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(TEST_UNIT_ARRAY_HEAP, 0, aheap_case)

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

    static tt_s32_t ah_comparer(IN void *l, IN void *r)
{
    if ((tt_u32_t)(tt_uintptr_t)l < (tt_u32_t)(tt_uintptr_t)r)
        return -1;
    else if ((tt_u32_t)(tt_uintptr_t)l == (tt_u32_t)(tt_uintptr_t)r)
        return 0;
    else
        return 1;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_arheap_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_arheap_t ah;
    tt_arheap_attr_t attr;
    tt_result_t ret;
    tt_s32_t h;

    TT_TEST_CASE_ENTER()
    // test start

    tt_arheap_attr_default(&attr);

    ret = tt_arheap_create(&ah, ah_comparer, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(tt_arheap_pophead(&ah), NULL, "");
    TT_TEST_CHECK_EQUAL(tt_arheap_head(&ah), NULL, "");

    ret = tt_arheap_destroy(&ah);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    attr.initial_node_num = 1;
    ret = tt_arheap_create(&ah, ah_comparer, &attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_arheap_add(&ah, (tt_ptr_t)1, &h);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(h, 0, "");
    TT_TEST_CHECK_EQUAL(tt_arheap_head(&ah), (tt_ptr_t)1, "");
    TT_TEST_CHECK_EQUAL(tt_arheap_pophead(&ah), (tt_ptr_t)1, "");
    TT_TEST_CHECK_EQUAL(h, TT_ARHEAP_NODE_NULL, "");
    TT_TEST_CHECK_EQUAL(tt_arheap_pophead(&ah), NULL, "");
    TT_TEST_CHECK_EQUAL(h, TT_ARHEAP_NODE_NULL, "");

    ret = tt_arheap_add(&ah, (tt_ptr_t)2, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_arheap_add(&ah, (tt_ptr_t)1, &h);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_arheap_add(&ah, (tt_ptr_t)1, &h);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_arheap_add(&ah, (tt_ptr_t)3, &h);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_arheap_add(&ah, (tt_ptr_t)~0, &h);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_arheap_add(&ah, (tt_ptr_t)~0, &h);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    TT_TEST_CHECK_EQUAL(tt_arheap_head(&ah), (tt_ptr_t)~0, "");
    TT_TEST_CHECK_EQUAL(tt_arheap_pophead(&ah), (tt_ptr_t)~0, "");
    TT_TEST_CHECK_EQUAL(tt_arheap_pophead(&ah), (tt_ptr_t)~0, "");
    TT_TEST_CHECK_EQUAL(tt_arheap_pophead(&ah), (tt_ptr_t)3, "");
    TT_TEST_CHECK_EQUAL(tt_arheap_pophead(&ah), (tt_ptr_t)2, "");

    ret = tt_arheap_destroy(&ah);
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");


    TT_TEST_CHECK_EQUAL(tt_arheap_pophead(&ah), (tt_ptr_t)1, "");
    TT_TEST_CHECK_EQUAL(tt_arheap_head(&ah), (tt_ptr_t)1, "");
    TT_TEST_CHECK_EQUAL(tt_arheap_pophead(&ah), (tt_ptr_t)1, "");
    TT_TEST_CHECK_EQUAL(tt_arheap_pophead(&ah), NULL, "");
    TT_TEST_CHECK_EQUAL(tt_arheap_pophead(&ah), NULL, "");

    ret = tt_arheap_destroy(&ah);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

#define __t2_num 10000
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

tt_s32_t rb_key_comparer(IN void *n, IN const tt_u8_t *key, IN tt_u32_t key_len)
{
    struct trb_t *ll = (struct trb_t *)n;
    if (ll->v < *(int *)key)
        return -1;
    else if (ll->v == *(int *)key)
        return 0;
    else
        return 1;
}

static tt_s32_t _dh[__t2_num];

TT_TEST_ROUTINE_DEFINE(tt_unit_test_arheap_correct)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_arheap_t ah;
    tt_arheap_attr_t attr;
    tt_result_t ret;

    tt_rbtree_t rbt;
    int i;

    tt_s64_t begin, end;
    tt_s32_t t1, t2;
    unsigned int seed = (tt_u32_t)time(NULL);

    TT_TEST_CASE_ENTER()
    // test start

    // create aheap
    tt_arheap_attr_default(&attr);

    ret = tt_arheap_create(&ah, ah_comparer, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // create a rbtree
    tt_rbtree_init(&rbt, rb_comparer, rb_key_comparer);

    seed = 1413209774;
    srand(seed);
    for (i = 0; i < sizeof(trb) / sizeof(struct trb_t); ++i) {
        trb[i].v = rand();
        tt_rbnode_init(&trb[i].n);
    }

    begin = tt_time_ref();
    for (i = 0; i < sizeof(trb) / sizeof(struct trb_t); ++i) {
        tt_rbtree_add(&rbt, &trb[i].n);
    }
    end = tt_time_ref();
    t1 = (tt_s32_t)tt_time_ref2ms(end - begin);

    begin = tt_time_ref();
    for (i = 0; i < sizeof(trb) / sizeof(struct trb_t); ++i) {
        tt_arheap_add(&ah, (tt_ptr_t)(tt_uintptr_t)trb[i].v, NULL);
    }
    end = tt_time_ref();
    t2 = (tt_s32_t)tt_time_ref2ms(end - begin);

    TT_RECORD_INFO("insert, rbtree [%d]ms, heap [%d]ms", t1, t2);

    // cmp
    for (i = 0; i < sizeof(trb) / sizeof(struct trb_t); ++i) {
        tt_rbnode_t *pn = tt_rbtree_max(rbt.root);
        int pv = (int)(tt_uintptr_t)tt_arheap_head(&ah);
        TT_TEST_CHECK_EQUAL(((struct trb_t *)pn)->v, pv, "");

        tt_rbtree_remove(pn);
        tt_arheap_pophead(&ah);
    }

    TT_TEST_CHECK_EQUAL(tt_arheap_head(&ah), NULL, "");
    TT_TEST_CHECK_EQUAL(tt_arheap_pophead(&ah), NULL, "");
    TT_TEST_CHECK_EQUAL(ah.tail_idx, 0, "");

    // test remove
    for (i = 0; i < sizeof(trb) / sizeof(struct trb_t); ++i) {
        tt_arheap_add(&ah, (tt_ptr_t)(tt_uintptr_t)trb[i].v, &_dh[i]);
    }
    for (i = 0; i < sizeof(trb) / sizeof(struct trb_t); ++i) {
        trb[i].v = rand();
        tt_arheap_fix(&ah, _dh[i]);
    }
    for (i = 0; i < sizeof(trb) / sizeof(struct trb_t); ++i) {
        tt_arheap_remove(&ah, _dh[i]);
        TT_TEST_CHECK_EQUAL(_dh[i], TT_ARHEAP_NODE_NULL, "");
    }

    ret = tt_arheap_destroy(&ah);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

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

TT_TEST_ROUTINE_DEFINE(tt_unit_test_arheap_perf)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_arheap_t ah;
    tt_arheap_attr_t attr;
    tt_result_t ret;

    tt_rbtree_t rbt;
    int i;

    tt_s64_t begin, end;
    tt_s32_t t1, t2;

    TT_TEST_CASE_ENTER()
    // test start

    srand((tt_u32_t)time(NULL));

    // create aheap
    tt_arheap_attr_default(&attr);
    ret = tt_arheap_create(&ah, ah_comparer, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // create a rbtree
    tt_rbtree_init(&rbt, rb_comparer, rb_key_comparer);
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
            tt_rbnode_t *n1 = tt_rbtree_max(rbt.root);
            if (n1 != NULL)
                foo1(((struct trb_t *)n1)->v);
        } else if (_act[i] == 1) {
            trb2[i].v = _act_val[i];
            tt_rbtree_add(&rbt, &trb2[i].n);
        } else {
            tt_rbnode_t *n1 = tt_rbtree_max(rbt.root);
            if (n1 != NULL)
                tt_rbtree_remove(n1);
        }
    }
    end = tt_time_ref();
    t1 = (tt_s32_t)tt_time_ref2ms(end - begin);

    begin = tt_time_ref();
    for (i = 0; i < sizeof(trb) / sizeof(struct trb_t); ++i) {
        if (_act[i] == 0) {
            int n1 = (int)(tt_uintptr_t)tt_arheap_head(&ah);
            if (n1 != 0)
                foo1(n1);
        } else if (_act[i] == 1) {
            tt_arheap_add(&ah, (tt_ptr_t)(tt_uintptr_t)_act_val[i], NULL);
        } else {
            tt_arheap_pophead(&ah);
        }
    }
    end = tt_time_ref();
    t2 = (tt_s32_t)tt_time_ref2ms(end - begin);

    TT_RECORD_INFO("aheap: %dms, rbtree: %dms", t2, t1);

    while (!tt_arheap_empty(&ah)) {
        tt_arheap_pophead(&ah);
    }
    ret = tt_arheap_destroy(&ah);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // test end
    TT_TEST_CASE_LEAVE()
}
