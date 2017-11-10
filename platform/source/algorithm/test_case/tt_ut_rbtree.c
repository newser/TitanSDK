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

#include <unit_test/tt_unit_test.h>

#include <tt_platform.h>

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
TT_TEST_ROUTINE_DECLARE(case_rbtree)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(rbtree_case)

TT_TEST_CASE(
    "case_rbtree", "testing rbtree", case_rbtree, NULL, NULL, NULL, NULL, NULL)
,

    TT_TEST_CASE_LIST_DEFINE_END(rbtree_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(ALG_UT_RBTREE, 0, rbtree_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    extern tt_result_t __rbt_expensive_check(IN tt_rbtree_t *tree,
                                             IN tt_rbnode_t *node,
                                             OUT tt_u32_t *bh);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

/*
TT_TEST_ROUTINE_DEFINE(case_rbtree)
{
    //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

    TT_TEST_CASE_ENTER()
    // test start

    // test end
    TT_TEST_CASE_LEAVE()
}
*/

typedef struct
{
    tt_rbnode_t node;
    tt_u32_t val;
} __rb_item_t;

static tt_s32_t __rb_cmpkey(IN void *p, IN tt_u8_t *key, IN tt_u32_t key_len)
{
    __rb_item_t *i = (__rb_item_t *)p;
    tt_u32_t v = *(tt_u32_t *)key;
    if (i->val < v) {
        return -1;
    } else if (i->val == v) {
        return 0;
    } else {
        return 1;
    }
}

TT_TEST_ROUTINE_DEFINE(case_rbtree)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_u32_t array_num = 0;
    tt_u32_t *array = NULL;
    __rb_item_t *rb_array = NULL;
    tt_rbtree_t tree;
    tt_rbtree_attr_t attr;

    tt_u32_t black_num = 0;
    tt_result_t ret;

    TT_TEST_CASE_ENTER()
    // test start

    tt_rbtree_attr_default(&attr);
    attr.cmpkey = __rb_cmpkey;

    do {
        tt_u32_t i = 0;

#ifdef __UT_LITE__
        array_num = tt_rand_u32() % 10000 + 20;
#else
        array_num = tt_rand_u32() % 100 + 20;
#endif
        // array_num = 20;

        array = (tt_u32_t *)tt_malloc(sizeof(tt_u32_t) * array_num);
        for (i = 0; i < array_num; ++i) {
            array[i] = tt_rand_u32();
        }
        array[0] = 18265;
        array[1] = 13217;
        array[2] = 17844;
        array[3] = 15016;
        array[4] = 2674;
        array[5] = 322;
        array[6] = 99999;
        array[7] = 20321;
        array[8] = 2133;
        array[9] = 25999;
        array[10] = 555660;
        array[11] = 23142654;
        array[12] = 45646;
        array[13] = 78913;
        array[14] = 4563103;
        array[15] = 456479;
        array[16] = 6;
        array[17] = 123455;
        array[18] = 5679846;
        array[19] = 12397946;

        rb_array = (__rb_item_t *)tt_malloc(sizeof(__rb_item_t) * array_num);
        for (i = 0; i < array_num; ++i) {
            tt_rbnode_init(&rb_array[i].node);
            rb_array[i].val = array[i];
        }

        tt_qsort(array, array_num, sizeof(tt_u32_t), tt_cmp_u32);

        tt_rbtree_init(&tree, &attr);
        TT_UT_EQUAL(tt_rbtree_root(&tree), NULL, "");
        TT_UT_EQUAL(tt_rbtree_count(&tree), 0, "");
        TT_UT_EQUAL(tt_rbtree_empty(&tree), TT_TRUE, "");
        TT_UT_EQUAL(tt_rbtree_min(&tree), NULL, "");
        TT_UT_EQUAL(tt_rbtree_max(&tree), NULL, "");
        TT_UT_EQUAL(tt_rbtree_find(&tree,
                                   (tt_u8_t *)&array[tt_rand_u32() % array_num],
                                   sizeof(tt_u32_t)),
                    NULL,
                    "");
        TT_UT_EQUAL(tt_rbtree_find_gteq(&tree,
                                        (tt_u8_t *)&array[tt_rand_u32() %
                                                          array_num],
                                        sizeof(tt_u32_t)),
                    NULL,
                    "");
        TT_UT_EQUAL(tt_rbtree_find_lteq(&tree,
                                        (tt_u8_t *)&array[tt_rand_u32() %
                                                          array_num],
                                        sizeof(tt_u32_t)),
                    NULL,
                    "");
        tt_rbtree_remove(&tree, &rb_array[tt_rand_u32() % array_num].node);

        // add 1 node
        tt_rbtree_add(&tree,
                      (tt_u8_t *)&rb_array[0].val,
                      sizeof(tt_u32_t),
                      &rb_array[0].node);
        TT_UT_EQUAL(tt_rbtree_root(&tree), &rb_array[0].node, "");
        TT_UT_EQUAL(tt_rbtree_count(&tree), 1, "");
        TT_UT_EQUAL(tt_rbtree_empty(&tree), TT_FALSE, "");
        TT_UT_EQUAL(tt_rbtree_min(&tree), &rb_array[0].node, "");
        TT_UT_EQUAL(tt_rbtree_max(&tree), &rb_array[0].node, "");
        TT_UT_EQUAL(tt_rbtree_prev(&tree, &rb_array[0].node), NULL, "");
        TT_UT_EQUAL(tt_rbtree_next(&tree, &rb_array[0].node), NULL, "");
        TT_UT_EQUAL(tt_rbtree_find(&tree,
                                   (tt_u8_t *)&rb_array[0].val,
                                   sizeof(tt_u32_t)),
                    &rb_array[0].node,
                    "");
        TT_UT_EQUAL(tt_rbtree_find(&tree,
                                   (tt_u8_t *)&black_num,
                                   sizeof(tt_u32_t)),
                    NULL,
                    "");

        // each time remove min
        for (i = 1; i < array_num; ++i) {
            tt_rbtree_add(&tree,
                          (tt_u8_t *)&rb_array[i].val,
                          sizeof(tt_u32_t),
                          &rb_array[i].node);

            if (i % 23 == 0) {
                tt_rbnode_t *n;
                __rb_item_t *it;

                black_num = array[i] + 1;
                n = tt_rbtree_find_lteq(&tree,
                                        (tt_u8_t *)&black_num,
                                        sizeof(tt_u32_t));
                TT_UT_NOT_EQUAL(n, NULL, "");
                it = TT_CONTAINER(n, __rb_item_t, node);
                TT_UT_EXP(it->val <= black_num, "");

                black_num = array[i] - 1;
                n = tt_rbtree_find_gteq(&tree,
                                        (tt_u8_t *)&black_num,
                                        sizeof(tt_u32_t));
                TT_UT_NOT_EQUAL(n, NULL, "");
                it = TT_CONTAINER(n, __rb_item_t, node);
                TT_UT_EXP(it->val >= black_num, "");
            }
        }

        black_num = 0;
        ret = __rbt_expensive_check(&tree, tt_rbtree_root(&tree), &black_num);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");

        // each time remove min
        for (i = 0; i < array_num; ++i) {
            tt_rbnode_t *node_max = NULL;
            __rb_item_t *item_max = NULL;
            tt_rbnode_t *node_min = NULL;
            __rb_item_t *item_min = NULL;

            node_min = tt_rbtree_min(&tree);
            item_min = TT_CONTAINER(node_min, __rb_item_t, node);
            TT_UT_EQUAL(array[i], item_min->val, "");

            node_max = tt_rbtree_max(&tree);
            item_max = TT_CONTAINER(node_max, __rb_item_t, node);
            TT_UT_EQUAL(array[array_num - 1], item_max->val, "");

            if (i != array_num - 1) {
                tt_rbnode_t *n = tt_rbtree_next(&tree, node_min);
                __rb_item_t *it = TT_CONTAINER(n, __rb_item_t, node);
                TT_UT_NOT_EQUAL(n, NULL, "");
                TT_UT_EQUAL(it->val, array[i + 1], "");
            }

            tt_rbtree_remove(&tree, node_min);
            TT_UT_EQUAL(tt_rbtree_count(&tree), array_num - i - 1, "");

            if ((i % 39 == 0) && (i != array_num - 1)) {
                ret = __rbt_expensive_check(&tree,
                                            tt_rbtree_root(&tree),
                                            &black_num);
                TT_UT_EQUAL(ret, TT_SUCCESS, "");
            }
        }
        TT_UT_EQUAL(tt_rbtree_count(&tree), 0, "");

        // each time remove max
        for (i = 0; i < array_num; ++i) {
            tt_rbtree_add(&tree,
                          (tt_u8_t *)&rb_array[i].val,
                          sizeof(tt_u32_t),
                          &rb_array[i].node);
        }
        tt_qsort(array, array_num, sizeof(tt_u32_t), tt_cmp_u32);
        for (i = array_num - 1; i != ~0; --i) {
            tt_rbnode_t *node_max = NULL;
            __rb_item_t *item_max = NULL;
            tt_rbnode_t *node_min = NULL;
            __rb_item_t *item_min = NULL;
            tt_rbnode_t *n;
            __rb_item_t *it;

            node_min = tt_rbtree_min(&tree);
            item_min = TT_CONTAINER(node_min, __rb_item_t, node);
            TT_UT_EQUAL(array[0], item_min->val, "");

            node_max = tt_rbtree_max(&tree);
            item_max = TT_CONTAINER(node_max, __rb_item_t, node);
            TT_UT_EQUAL(array[i], item_max->val, "");

            if (i != 0) {
                tt_rbnode_t *n = tt_rbtree_prev(&tree, node_max);
                __rb_item_t *it = TT_CONTAINER(n, __rb_item_t, node);
                TT_UT_NOT_EQUAL(n, NULL, "");
                TT_UT_EQUAL(it->val, array[i - 1], "");
            }

            n = tt_rbtree_find(&tree, (tt_u8_t *)&array[i], sizeof(tt_u32_t));
            it = TT_CONTAINER(n, __rb_item_t, node);
            TT_UT_EQUAL(it->val, array[i], "");

            tt_rbtree_remove(&tree, node_max);
            TT_UT_EQUAL(tt_rbtree_count(&tree), i, "");

            if ((i % 41 == 0) && (i != 0)) {
                ret = __rbt_expensive_check(&tree,
                                            tt_rbtree_root(&tree),
                                            &black_num);
                TT_UT_EQUAL(ret, TT_SUCCESS, "");
            }
        }
        TT_UT_EQUAL(tt_rbtree_count(&tree), 0, "");

        // delete from middle node to end
        for (i = 0; i < array_num; ++i) {
            tt_rbtree_add(&tree,
                          (tt_u8_t *)&rb_array[i].val,
                          sizeof(tt_u32_t),
                          &rb_array[i].node);
        }
        tt_qsort(array, array_num, sizeof(tt_u32_t), tt_cmp_u32);
        i = 1;
        while (i < array_num - 1) {
            tt_rbnode_t *node_max = NULL;
            __rb_item_t *item_max = NULL;
            tt_rbnode_t *node_min = NULL;
            __rb_item_t *item_min = NULL;

            node_min = tt_rbtree_min(&tree);
            item_min = TT_CONTAINER(node_min, __rb_item_t, node);
            TT_UT_EQUAL(array[0], item_min->val, "");

            node_max = tt_rbtree_max(&tree);
            item_max = TT_CONTAINER(node_max, __rb_item_t, node);
            TT_UT_EQUAL(array[array_num - 1], item_max->val, "");

            i += tt_rand_u32() % 100;
        }

        tt_free(array);
        tt_free(rb_array);
    } while (0);

    // test end
    TT_TEST_CASE_LEAVE()
}
