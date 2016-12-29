/*
 * import header files
 */
#ifdef __cplusplus
extern "C" {
#endif

#include "tt_unit_test_case_config.h"
#include <unit_test/tt_unit_test.h>

#include <algorithm/tt_binary_search.h>
#include <algorithm/tt_hash_map.h>
#include <algorithm/tt_list.h>
#include <algorithm/tt_quick_sort.h>
#include <algorithm/tt_red_black_tree.h>
#include <algorithm/tt_stack.h>
#include <log/tt_log.h>
#include <memory/tt_memory_alloc.h>
#include <os/tt_spinlock.h>
#include <os/tt_thread.h>
#include <timer/tt_time_reference.h>

// portlayer header files
#include <tt_cstd_api.h>

#include <time.h>

//#define TT_VS_RB2
#ifdef TT_VS_RB2
#include <algorithm/rbtree.h>
#endif

//#define TT_VS_RB3

/*
 * local definition
 */
// == macro

// == extern declaration
extern void __short_sort(IN void *elem_start,
                         IN tt_u32_t elem_num,
                         IN tt_u32_t elem_size,
                         IN tt_cmp_t comparer);

extern void *__short_bsearch(IN void *key,
                             IN void *elem_start,
                             IN tt_u32_t elem_num,
                             IN tt_u32_t elem_size,
                             IN tt_cmp_t comparer);
extern void *__short_bsearch_upper(IN void *key,
                                   IN void *elem_start,
                                   IN tt_u32_t elem_num,
                                   IN tt_u32_t elem_size,
                                   IN tt_cmp_t comparer);
extern void *__short_bsearch_lower(IN void *key,
                                   IN void *elem_start,
                                   IN tt_u32_t elem_num,
                                   IN tt_u32_t elem_size,
                                   IN tt_cmp_t comparer);

extern tt_result_t __rbtree_expensive_check(IN tt_rbnode_t *node,
                                            IN OUT tt_u32_t *black_node_num);

static tt_s32_t test_rb_key_comparer(IN void *l,
                                     IN const tt_u8_t *key,
                                     tt_u32_t key_len);

// == global variant

// == interface declaration
typedef struct rb_item_struct
{
    tt_u32_t val;
    tt_rbnode_t node;
} rb_item;

typedef struct lst_item_struct
{
    tt_u32_t val;
    tt_lnode_t node;
} lst_item;

tt_s32_t test_u8_comparer(IN void *l, IN void *r);
tt_s32_t test_comparer(IN void *l, IN void *r);
tt_s32_t test_rb_comparer(IN void *l, IN void *r);
tt_s32_t test_lst_comparer(IN void *l, IN void *r);

// === routine declarations ================
TT_TEST_ROUTINE_DECLARE(tt_unit_test_basic_alg_qsort)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_basic_alg_qsort_random)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_basic_alg_bsearch)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_basic_alg_min_larger)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_basic_alg_max_less)

TT_TEST_ROUTINE_DECLARE(tt_unit_test_alg_rbtree)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_alg_rbtree_random)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_alg_rbtree_single)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_alg_rbtree_mt)

TT_TEST_ROUTINE_DECLARE(tt_unit_test_alg_stack)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_alg_ptr_stack)

TT_TEST_ROUTINE_DECLARE(tt_unit_test_alg_hash_calc)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_alg_hash_collision)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(basic_alg_case)

TT_TEST_CASE("tt_unit_test_basic_alg_qsort",
             "testing tt_qsort()",
             tt_unit_test_basic_alg_qsort,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_basic_alg_qsort_random",
                 "testing tt_qsort()",
                 tt_unit_test_basic_alg_qsort_random,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_basic_alg_bsearch",
                 "testing tt_bsearch()",
                 tt_unit_test_basic_alg_bsearch,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_basic_alg_min_larger",
                 "testing tt_bsearch_upper()",
                 tt_unit_test_basic_alg_min_larger,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_basic_alg_max_less",
                 "testing tt_bsearch_lower()",
                 tt_unit_test_basic_alg_max_less,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_alg_rbtree",
                 "testing apis of red black tree",
                 tt_unit_test_alg_rbtree,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

#if 1
    TT_TEST_CASE("tt_unit_test_alg_rbtree_random",
                 "testing apis of red black tree randomly",
                 tt_unit_test_alg_rbtree_random,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_alg_rbtree_single",
                 "testing apis of red black by a single case",
                 tt_unit_test_alg_rbtree_single,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_alg_rbtree_mt",
                 "testing apis of red black in multi thread",
                 tt_unit_test_alg_rbtree_mt,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),
#endif

    TT_TEST_CASE("tt_unit_test_alg_stack",
                 "testing apis of stack operations",
                 tt_unit_test_alg_stack,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),
    TT_TEST_CASE("tt_unit_test_alg_ptr_stack",
                 "testing apis of pointer stack operations",
                 tt_unit_test_alg_ptr_stack,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_alg_hash_calc",
                 "testing apis of hash calculation",
                 tt_unit_test_alg_hash_calc,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),
    TT_TEST_CASE("tt_unit_test_alg_hash_collision",
                 "testing apis of hash collision",
                 tt_unit_test_alg_hash_collision,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(basic_alg_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(TEST_UNIT_BASIC_ALG, 0, basic_alg_case)

    /*
     * interface implementation
     */


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

    TT_TEST_ROUTINE_DEFINE(tt_unit_test_basic_alg_qsort)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_u8_t test_array1[32] = {0};
    tt_u8_t test_array2[32] = {0};
    tt_u32_t index = 0;

    TT_TEST_CASE_ENTER()
    // test start

    for (index = 0; index < 32; ++index) {
        test_array1[index] = test_array2[index] = index;
    }

    tt_qsort(test_array1, 32, sizeof(tt_u8_t), test_u8_comparer);

    for (index = 0; index < 32; ++index) {
        TT_TEST_CHECK_EQUAL(test_array1[index], test_array2[index], "");
    }

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_basic_alg_qsort_random)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_u32_t test = 0;

    TT_TEST_CASE_ENTER()
    // test start

    srand((tt_u32_t)time(NULL));

    while (test < 100) {
        tt_u32_t test_array1[1000] = {0};
        tt_u32_t test_array2[1000] = {0};
        tt_u32_t array_len = 0;
        tt_u32_t index = 0;

        // generate array randomly
        array_len = rand() % 1000 + 1;
        for (index = 0; index < array_len; ++index) {
            test_array1[index] = rand();
            test_array2[index] = test_array1[index];
        }

        // use qsort
        tt_qsort(test_array1, array_len, sizeof(tt_u32_t), test_comparer);
        __short_sort(test_array2, array_len, sizeof(tt_u32_t), test_comparer);

        // should got same result
        for (index = 0; index < array_len; ++index) {
            TT_TEST_CHECK_EQUAL(test_array1[index], test_array2[index], "");
        }

        ++test;
    }

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_basic_alg_bsearch)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

    tt_u8_t test_array1[32] = {0};
    tt_u32_t index = 0;
    tt_u8_t key;
    tt_u8_t *result = NULL;

    tt_u32_t test = 0;

    TT_TEST_CASE_ENTER()
    // test start

    for (index = 0; index < 32; ++index) {
        test_array1[index] = 100 + index * 2;
    }

    // general search
    key = 120;
    result = (tt_u8_t *)
        tt_bsearch(&key, test_array1, 32, sizeof(tt_u8_t), test_u8_comparer);
    TT_TEST_CHECK_EQUAL(*result, key, "");
    result = (tt_u8_t *)__short_bsearch(&key,
                                        test_array1,
                                        32,
                                        sizeof(tt_u8_t),
                                        test_u8_comparer);
    TT_TEST_CHECK_EQUAL(*result, key, "");

    // no matching key
    key = 99;
    result = (tt_u8_t *)
        tt_bsearch(&key, test_array1, 32, sizeof(tt_u8_t), test_u8_comparer);
    TT_TEST_CHECK_EQUAL(result, NULL, "");
    result = (tt_u8_t *)__short_bsearch(&key,
                                        test_array1,
                                        32,
                                        sizeof(tt_u8_t),
                                        test_u8_comparer);
    TT_TEST_CHECK_EQUAL(result, NULL, "");

    // no matching key
    key = 250;
    result = (tt_u8_t *)
        tt_bsearch(&key, test_array1, 32, sizeof(tt_u8_t), test_u8_comparer);
    TT_TEST_CHECK_EQUAL(result, NULL, "");
    result = (tt_u8_t *)__short_bsearch(&key,
                                        test_array1,
                                        32,
                                        sizeof(tt_u8_t),
                                        test_u8_comparer);
    TT_TEST_CHECK_EQUAL(result, NULL, "");

    // no matching key
    key = 121;
    result = (tt_u8_t *)
        tt_bsearch(&key, test_array1, 32, sizeof(tt_u8_t), test_u8_comparer);
    TT_TEST_CHECK_EQUAL(result, NULL, "");
    result = (tt_u8_t *)__short_bsearch(&key,
                                        test_array1,
                                        32,
                                        sizeof(tt_u8_t),
                                        test_u8_comparer);
    TT_TEST_CHECK_EQUAL(result, NULL, "");

    // random test
    srand((tt_u32_t)time(NULL));
    while (test < 100) {
        tt_u32_t test_array2[10000] = {0};
        tt_u32_t array_len = 0;
        tt_u32_t key = 0;
        tt_u32_t *result = NULL;

        // generate array randomly
        array_len = rand() % 10000 + 1;
        for (index = 0; index < array_len; ++index) {
            test_array2[index] = rand();
        }

        // use qsort
        tt_qsort(test_array2, array_len, sizeof(tt_u32_t), test_comparer);

        key = test_array2[rand() % array_len];
        result = (tt_u32_t *)tt_bsearch((void *)&key,
                                        test_array2,
                                        array_len,
                                        sizeof(tt_u32_t),
                                        test_comparer);
        TT_TEST_CHECK_EQUAL(*result, key, "");

        result = (tt_u32_t *)__short_bsearch((void *)&key,
                                             test_array2,
                                             array_len,
                                             sizeof(tt_u32_t),
                                             test_comparer);
        TT_TEST_CHECK_EQUAL(*result, key, "");

        ++test;
    }

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_basic_alg_min_larger)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

    tt_u8_t test_array1[32] = {0};
    tt_u32_t index = 0;
    tt_u8_t key;
    tt_u8_t *result = NULL;

    tt_u32_t test = 0;
    tt_u32_t seed = 0;

    TT_TEST_CASE_ENTER()
    // test start

    for (index = 0; index < 32; ++index) {
        test_array1[index] = 100 + index * 2;
    }

    // general search
    key = 121;
    result = (tt_u8_t *)tt_bsearch_upper(&key,
                                         test_array1,
                                         32,
                                         sizeof(tt_u8_t),
                                         test_u8_comparer);
    TT_TEST_CHECK_EQUAL(*result, 122, "");
    result = (tt_u8_t *)__short_bsearch_upper(&key,
                                              test_array1,
                                              32,
                                              sizeof(tt_u8_t),
                                              test_u8_comparer);
    TT_TEST_CHECK_EQUAL(*result, 122, "");

    // general search
    key = 122;
    result = (tt_u8_t *)tt_bsearch_upper(&key,
                                         test_array1,
                                         32,
                                         sizeof(tt_u8_t),
                                         test_u8_comparer);
    TT_TEST_CHECK_EQUAL(*result, 122, "");
    result = (tt_u8_t *)__short_bsearch_upper(&key,
                                              test_array1,
                                              32,
                                              sizeof(tt_u8_t),
                                              test_u8_comparer);
    TT_TEST_CHECK_EQUAL(*result, 122, "");

    // no matching key
    key = 99;
    result = (tt_u8_t *)tt_bsearch_upper(&key,
                                         test_array1,
                                         32,
                                         sizeof(tt_u8_t),
                                         test_u8_comparer);
    TT_TEST_CHECK_EQUAL(*result, 100, "");
    result = (tt_u8_t *)__short_bsearch_upper(&key,
                                              test_array1,
                                              32,
                                              sizeof(tt_u8_t),
                                              test_u8_comparer);
    TT_TEST_CHECK_EQUAL(*result, 100, "");

    // no matching key
    key = 250;
    result = (tt_u8_t *)tt_bsearch_upper(&key,
                                         test_array1,
                                         32,
                                         sizeof(tt_u8_t),
                                         test_u8_comparer);
    TT_TEST_CHECK_EQUAL(result, NULL, "");
    result = (tt_u8_t *)__short_bsearch_upper(&key,
                                              test_array1,
                                              32,
                                              sizeof(tt_u8_t),
                                              test_u8_comparer);
    TT_TEST_CHECK_EQUAL(result, NULL, "");

    // random test
    seed = (tt_u32_t)time(NULL);
    srand(seed);
    while (test < 100) {
        tt_u32_t test_array2[10000] = {0};
        tt_u32_t array_len = 0;
        tt_u32_t key = 0;
        tt_u32_t *result = NULL;
        tt_u32_t *result2 = NULL;

        // generate array randomly
        array_len = rand() % 10000 + 1;
        for (index = 0; index < array_len; ++index) {
            test_array2[index] = rand();
        }

        // use qsort
        tt_qsort(test_array2, array_len, sizeof(tt_u32_t), test_comparer);

        key = test_array2[rand() % array_len] - 1;
        result = (tt_u32_t *)tt_bsearch_upper((void *)&key,
                                              test_array2,
                                              array_len,
                                              sizeof(tt_u32_t),
                                              test_comparer);
        result2 = (tt_u32_t *)__short_bsearch_upper((void *)&key,
                                                    test_array2,
                                                    array_len,
                                                    sizeof(tt_u32_t),
                                                    test_comparer);
        TT_TEST_CHECK_EQUAL(*result, *result2, "");

        ++test;
    }

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_basic_alg_max_less)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

    tt_u8_t test_array1[32] = {0};
    tt_u32_t index = 0;
    tt_u8_t key;
    tt_u8_t *result = NULL;

    tt_u32_t test = 0;

    TT_TEST_CASE_ENTER()
    // test start

    for (index = 0; index < 32; ++index) {
        test_array1[index] = 100 + index * 2;
    }

    // general search
    key = 121;
    result = (tt_u8_t *)tt_bsearch_lower(&key,
                                         test_array1,
                                         32,
                                         sizeof(tt_u8_t),
                                         test_u8_comparer);
    TT_TEST_CHECK_EQUAL(*result, 120, "");
    result = (tt_u8_t *)__short_bsearch_lower(&key,
                                              test_array1,
                                              32,
                                              sizeof(tt_u8_t),
                                              test_u8_comparer);
    TT_TEST_CHECK_EQUAL(*result, 120, "");

    // general search
    key = 122;
    result = (tt_u8_t *)tt_bsearch_lower(&key,
                                         test_array1,
                                         32,
                                         sizeof(tt_u8_t),
                                         test_u8_comparer);
    TT_TEST_CHECK_EQUAL(*result, 122, "");
    result = (tt_u8_t *)__short_bsearch_lower(&key,
                                              test_array1,
                                              32,
                                              sizeof(tt_u8_t),
                                              test_u8_comparer);
    TT_TEST_CHECK_EQUAL(*result, 122, "");

    // no matching key
    key = 99;
    result = (tt_u8_t *)tt_bsearch_lower(&key,
                                         test_array1,
                                         32,
                                         sizeof(tt_u8_t),
                                         test_u8_comparer);
    TT_TEST_CHECK_EQUAL(result, NULL, "");
    result = (tt_u8_t *)__short_bsearch_lower(&key,
                                              test_array1,
                                              32,
                                              sizeof(tt_u8_t),
                                              test_u8_comparer);
    TT_TEST_CHECK_EQUAL(result, NULL, "");

    // no matching key
    key = 250;
    result = (tt_u8_t *)tt_bsearch_lower(&key,
                                         test_array1,
                                         32,
                                         sizeof(tt_u8_t),
                                         test_u8_comparer);
    TT_TEST_CHECK_EQUAL(*result, 162, "");
    result = (tt_u8_t *)__short_bsearch_lower(&key,
                                              test_array1,
                                              32,
                                              sizeof(tt_u8_t),
                                              test_u8_comparer);
    TT_TEST_CHECK_EQUAL(*result, 162, "");

    // random test
    srand((tt_u32_t)time(NULL));
    while (test < 100) {
        tt_u32_t test_array2[10000] = {0};
        tt_u32_t array_len = 0;
        tt_u32_t key = 0;
        tt_u32_t *result = NULL;
        tt_u32_t *result2 = NULL;

        // generate array randomly
        array_len = rand() % 10000 + 1;
        for (index = 0; index < array_len; ++index) {
            test_array2[index] = rand();
        }

        // use qsort
        tt_qsort(test_array2, array_len, sizeof(tt_u32_t), test_comparer);

        key = test_array2[rand() % array_len] + 1;
        result = (tt_u32_t *)tt_bsearch_lower((void *)&key,
                                              test_array2,
                                              array_len,
                                              sizeof(tt_u32_t),
                                              test_comparer);
        result2 = (tt_u32_t *)__short_bsearch_lower((void *)&key,
                                                    test_array2,
                                                    array_len,
                                                    sizeof(tt_u32_t),
                                                    test_comparer);
        TT_TEST_CHECK_EQUAL(*result, *result2, "");

        ++test;
    }

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_alg_rbtree)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_u32_t array_num = 0;
    tt_u32_t *array = NULL;
    rb_item *rb_array = NULL;
    tt_rbtree_t tree;

    tt_u32_t black_num = 0;
    tt_result_t ret;

    TT_TEST_CASE_ENTER()
    // test start

    srand((tt_u32_t)time(NULL));

    do {
        tt_u32_t i = 0;

        array_num = rand() % 100000;
        // array_num = 20;

        array = (tt_u32_t *)malloc(sizeof(tt_u32_t) * array_num);

        rb_array = (rb_item *)malloc(sizeof(rb_item) * array_num);
        tt_memset(rb_array, 0, sizeof(rb_item) * array_num);

        tt_rbtree_init(&tree, test_rb_comparer, test_rb_key_comparer);
        TT_TEST_CHECK_EQUAL(tt_rbtree_min(tree.root), NULL, "");
        TT_TEST_CHECK_EQUAL(tt_rbtree_max(tree.root), NULL, "");

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

        // each time remove min
        for (i = 0; i < array_num; ++i) {
            // array[i] = rand();

            rb_array[i].val = array[i];
            tt_rbnode_init(&rb_array[i].node);

            tt_rbtree_add(&tree, &rb_array[i].node);
        }

        black_num = 0;
        ret = __rbtree_expensive_check(tt_rbtree_root(&tree), &black_num);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        tt_qsort(array, array_num, sizeof(tt_u32_t), test_comparer);
        for (i = 0; i < array_num; ++i) {
            tt_rbnode_t *node_max = NULL;
            rb_item *item_max = NULL;
            tt_rbnode_t *node_min = NULL;
            rb_item *item_min = NULL;

            node_min = tt_rbtree_min(tree.root);
            item_min = TT_CONTAINER(node_min, rb_item, node);
            TT_TEST_CHECK_EQUAL(array[i], item_min->val, "");

            node_max = tt_rbtree_max(tree.root);
            item_max = TT_CONTAINER(node_max, rb_item, node);
            TT_TEST_CHECK_EQUAL(array[array_num - 1], item_max->val, "");

            tt_rbtree_remove(node_min);
        }

        // each time remove max
        for (i = 0; i < array_num; ++i) {
            // array[i] = rand();

            rb_array[i].val = array[i];
            tt_rbnode_init(&rb_array[i].node);

            tt_rbtree_add(&tree, &rb_array[i].node);
        }
        tt_qsort(array, array_num, sizeof(tt_u32_t), test_comparer);
        for (i = 0; i < array_num; ++i) {
            tt_rbnode_t *node_max = NULL;
            rb_item *item_max = NULL;
            tt_rbnode_t *node_min = NULL;
            rb_item *item_min = NULL;

            node_min = tt_rbtree_min(tree.root);
            item_min = TT_CONTAINER(node_min, rb_item, node);
            TT_TEST_CHECK_EQUAL(array[0], item_min->val, "");

            node_max = tt_rbtree_max(tree.root);
            item_max = TT_CONTAINER(node_max, rb_item, node);
            TT_TEST_CHECK_EQUAL(array[array_num - i - 1], item_max->val, "");

            tt_rbtree_remove(node_max);
        }

        // delete from middle node to end
        for (i = 0; i < array_num; ++i) {
            // array[i] = rand();

            rb_array[i].val = array[i];
            tt_rbnode_init(&rb_array[i].node);

            tt_rbtree_add(&tree, &rb_array[i].node);
        }
        tt_qsort(array, array_num, sizeof(tt_u32_t), test_comparer);
        for (i = 2; i < array_num - 1; ++i) {
            tt_rbnode_t *node_max = NULL;
            rb_item *item_max = NULL;
            tt_rbnode_t *node_min = NULL;
            rb_item *item_min = NULL;

            tt_rbnode_t *skey_node = NULL;
            tt_rbnode_t *node_del = NULL;
            rb_item item_del = {0};

            // delete a node
            item_del.val = array[i];
            node_del = tt_rbtree_find(tree.root, &item_del.node);
            skey_node = tt_rbtree_find_k(tree.root,
                                         (tt_u8_t *)&array[i],
                                         sizeof(tt_u32_t));
            TT_TEST_CHECK_EQUAL(node_del, skey_node, "");

            tt_rbtree_remove(node_del);
            node_del = tt_rbtree_find(tree.root, &item_del.node);
            // TT_TEST_CHECK_EQUAL(node_del, NULL, "");
            // not always null, maybe some nodes has same key value
            skey_node = tt_rbtree_find_k(tree.root,
                                         (tt_u8_t *)&array[i],
                                         sizeof(tt_u32_t));
            TT_TEST_CHECK_EQUAL(skey_node, node_del, "");

            node_min = tt_rbtree_min(tree.root);
            item_min = TT_CONTAINER(node_min, rb_item, node);
            TT_TEST_CHECK_EQUAL(array[0], item_min->val, "");

            node_max = tt_rbtree_max(tree.root);
            item_max = TT_CONTAINER(node_max, rb_item, node);
            TT_TEST_CHECK_EQUAL(array[array_num - 1], item_max->val, "");
        }
        // clear tree
        for (i = 0; i < array_num; ++i) {
            tt_rbnode_init(&rb_array[i].node);
        }
        tt_rbtree_init(&tree, test_rb_comparer, test_rb_key_comparer);

        // delete from middle node to begin
        for (i = 0; i < array_num; ++i) {
            // array[i] = rand();

            rb_array[i].val = array[i];
            tt_rbnode_init(&rb_array[i].node);

            tt_rbtree_add(&tree, &rb_array[i].node);
        }
        tt_qsort(array, array_num, sizeof(tt_u32_t), test_comparer);
        for (i = 7; i > 1; --i) {
            tt_rbnode_t *node_max = NULL;
            rb_item *item_max = NULL;
            tt_rbnode_t *node_min = NULL;
            rb_item *item_min = NULL;

            tt_rbnode_t *skey_node = NULL;
            tt_rbnode_t *node_del = NULL;
            rb_item item_del = {0};

            // delete a node
            item_del.val = array[i];
            node_del = tt_rbtree_find(tree.root, &item_del.node);
            skey_node = tt_rbtree_find_k(tree.root,
                                         (tt_u8_t *)&array[i],
                                         sizeof(tt_u32_t));
            TT_TEST_CHECK_EQUAL(node_del, skey_node, "");

            tt_rbtree_remove(node_del);

            node_del = tt_rbtree_find(tree.root, &item_del.node);
            // TT_TEST_CHECK_EQUAL(node_del, NULL, "");
            skey_node = tt_rbtree_find_k(tree.root,
                                         (tt_u8_t *)&array[i],
                                         sizeof(tt_u32_t));
            TT_TEST_CHECK_EQUAL(node_del, skey_node, "");

            node_min = tt_rbtree_min(tree.root);
            item_min = TT_CONTAINER(node_min, rb_item, node);
            TT_TEST_CHECK_EQUAL(array[0], item_min->val, "");

            node_max = tt_rbtree_max(tree.root);
            item_max = TT_CONTAINER(node_max, rb_item, node);
            TT_TEST_CHECK_EQUAL(array[array_num - 1], item_max->val, "");
        }
        // clear tree
        for (i = 0; i < array_num; ++i) {
            tt_rbnode_init(&rb_array[i].node);
        }
        tt_rbtree_init(&tree, test_rb_comparer, test_rb_key_comparer);


        free(array);
        free(rb_array);
    } while (0);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_alg_rbtree_random)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_u32_t array_num = 10000;
    lst_item *lst_array = NULL;
    rb_item *rb_array = NULL;
    tt_rbtree_t tree;

    lst_item *lst_array_bk = NULL;
    tt_u32_t *del_bk = NULL;
    tt_u32_t del_bk_num = 0;

    tt_u32_t test_num = 10;

    TT_TEST_CASE_ENTER()
// test start

#if (TT_UNIT_TEST_CASE_FAST & TT_UNIT_TEST_ALG_FAST)
    TT_TEST_CASE_LEAVE()
#endif

    srand((tt_u32_t)time(NULL));

    do {
        tt_u32_t i = 0;
        tt_list_t lst;

        tt_rbnode_t *rb_pos = NULL;
        tt_rbnode_t *skey_node = NULL;
        tt_lnode_t *lst_pos = NULL;

        tt_rbnode_t *search_rb = NULL;
        rb_item key_rb;
        tt_lnode_t *search_lst = NULL;
        lst_item key_lst;

        lst_array = (lst_item *)malloc(sizeof(lst_item) * array_num);
        tt_memset(lst_array, 0, sizeof(lst_item) * array_num);
        tt_list_init(&lst);

        lst_array_bk = (lst_item *)malloc(sizeof(lst_item) * array_num);
        tt_memset(lst_array_bk, 0, sizeof(lst_item) * array_num);
        del_bk = (tt_u32_t *)malloc(sizeof(tt_u32_t) * array_num);
        tt_memset(del_bk, 0xFF, sizeof(tt_u32_t) * array_num);
        del_bk_num = 0;

        rb_array = (rb_item *)malloc(sizeof(rb_item) * array_num);
        tt_memset(rb_array, 0, sizeof(rb_item) * array_num);
        tt_rbtree_init(&tree, test_rb_comparer, test_rb_key_comparer);

        for (i = 0; i < array_num; ++i) {
            lst_array[i].val = rand();

            rb_array[i].val = lst_array[i].val;
            tt_rbnode_init(&rb_array[i].node);
            tt_rbtree_add(&tree, &rb_array[i].node);
        }

        TT_TEST_CHECK_EQUAL(__rbtree_expensive_check(tt_rbtree_root(&tree),
                                                     NULL),
                            TT_SUCCESS,
                            "");

        tt_memcpy(lst_array_bk, lst_array, sizeof(lst_item) * array_num);

        tt_qsort(lst_array, array_num, sizeof(lst_item), test_lst_comparer);
        for (i = 0; i < array_num; ++i) {
            tt_lnode_init(&lst_array[i].node);
            tt_list_push_tail(&lst, &lst_array[i].node);
        }

        // min larger
        tt_rbnode_init(&key_rb.node);
        key_rb.val = rand();
        search_rb = tt_rbtree_find_upper(tree.root, &key_rb.node);

        skey_node = tt_rbtree_find_upper_k(tree.root,
                                           (tt_u8_t *)&key_rb.val,
                                           sizeof(tt_u32_t));
        TT_TEST_CHECK_EQUAL(search_rb, skey_node, "");

        tt_lnode_init(&key_lst.node);
        key_lst.val = key_rb.val;
        search_lst = (tt_lnode_t *)tt_bsearch_upper(&key_lst,
                                                    lst_array,
                                                    array_num,
                                                    sizeof(lst_item),
                                                    test_lst_comparer);
        if ((search_rb == NULL) || (search_lst == NULL)) {
            TT_TEST_CHECK_EQUAL((void *)search_rb, (void *)search_lst, "");
        } else {
            rb_item *search_rb_item = TT_CONTAINER(search_rb, rb_item, node);
            lst_item *search_lst_item = (lst_item *)search_lst;

            TT_TEST_CHECK_EQUAL(search_rb_item->val, search_lst_item->val, "");
            TT_TEST_CHECK_EXP(key_rb.val <= search_rb_item->val, "");
        }

        // max less
        tt_rbnode_init(&key_rb.node);
        key_rb.val = rand();
        search_rb = tt_rbtree_find_lower(tree.root, &key_rb.node);

        skey_node = tt_rbtree_find_lower_k(tree.root,
                                           (tt_u8_t *)&key_rb.val,
                                           sizeof(tt_u32_t));
        TT_TEST_CHECK_EQUAL(search_rb, skey_node, "");

        tt_lnode_init(&key_lst.node);
        key_lst.val = key_rb.val;
        search_lst = (tt_lnode_t *)tt_bsearch_lower(&key_lst,
                                                    lst_array,
                                                    array_num,
                                                    sizeof(lst_item),
                                                    test_lst_comparer);
        if ((search_rb == NULL) || (search_lst == NULL)) {
            TT_TEST_CHECK_EQUAL((void *)search_rb, (void *)search_lst, "");
        } else {
            rb_item *search_rb_item = TT_CONTAINER(search_rb, rb_item, node);
            lst_item *search_lst_item = (lst_item *)search_lst;

            TT_TEST_CHECK_EQUAL(search_rb_item->val, search_lst_item->val, "");
            TT_TEST_CHECK_EXP(key_rb.val >= search_rb_item->val, "");
        }

        // delete randomly
        for (i = 0; i < array_num / 2; ++i) {
            tt_u32_t j = rand() % array_num;
            lst_item *li = &lst_array[j];
            tt_rbnode_t *del_node = NULL;
            rb_item ri = {0};

            if (li->node.lst == &lst) {
                del_bk[del_bk_num++] = j;

                tt_list_remove(&li->node);

                ri.val = li->val;
                del_node = tt_rbtree_find(tree.root, &ri.node);

                skey_node = tt_rbtree_find_k(tree.root,
                                             (tt_u8_t *)&ri.val,
                                             sizeof(tt_u32_t));
                TT_TEST_CHECK_EQUAL(skey_node, del_node, "");

                tt_rbtree_remove(del_node);
            }
        }

        TT_TEST_CHECK_EQUAL(lst.count, tree.node_num, "");
        lst_pos = tt_list_head(&lst);
        rb_pos = tt_rbtree_min(tree.root);

        while (lst_pos != NULL) {
            lst_item *comp_lst = TT_CONTAINER(lst_pos, lst_item, node);
            rb_item *comp_rb = TT_CONTAINER(rb_pos, rb_item, node);

            TT_TEST_CHECK_EQUAL(comp_lst->val, comp_rb->val, "");

            lst_pos = lst_pos->next;
            rb_pos = tt_rbtree_next(rb_pos);
        }

        free(lst_array);
        free(rb_array);

        free(lst_array_bk);
        free(del_bk);
    } while (test_num--);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_alg_rbtree_single)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_u32_t array_num = 6;
    rb_item rb_array[6];
    tt_rbtree_t tree;

    tt_u32_t del[6] = {0};
    tt_u32_t del_num = 2;

    tt_rbnode_t *rb_pos = NULL;
    rb_item *ri = NULL;

    TT_TEST_CASE_ENTER()
    // test start

    memset(&rb_array, 0, sizeof(rb_array));
    do {
        tt_u32_t i = 0;

        rb_array[0].val = 4135;
        rb_array[1].val = 23632;
        rb_array[2].val = 150;
        rb_array[3].val = 12400;
        rb_array[4].val = 26123;
        rb_array[5].val = 25649;

        del[0] = 1;
        del[1] = 2;

        tt_rbtree_init(&tree, test_rb_comparer, test_rb_key_comparer);

        for (i = 0; i < array_num; ++i) {
            tt_rbnode_init(&rb_array[i].node);
            tt_rbtree_add(&tree, &rb_array[i].node);
        }

        // delete
        for (i = 0; i < del_num; ++i) {
            tt_rbnode_t *del_node = NULL;
            tt_rbnode_t *skey_node = NULL;
            rb_item ri = {0};

            ri.val = rb_array[del[i]].val;
            del_node = tt_rbtree_find(tree.root, &ri.node);
            skey_node = tt_rbtree_find_k(tree.root,
                                         (tt_u8_t *)&ri.val,
                                         sizeof(tt_u32_t));
            TT_TEST_CHECK_EQUAL(skey_node, del_node, "");

            tt_rbtree_remove(del_node);

            del_node = tt_rbtree_find(tree.root, &ri.node);
            skey_node = tt_rbtree_find_k(tree.root,
                                         (tt_u8_t *)&ri.val,
                                         sizeof(tt_u32_t));
            TT_TEST_CHECK_EQUAL(skey_node, del_node, "");
        }
    } while (0);

    rb_pos = tt_rbtree_min(tree.root);
    ri = TT_CONTAINER(rb_pos, rb_item, node);
    TT_TEST_CHECK_EQUAL(ri->val, 4135, "");

    rb_pos = tt_rbtree_next(rb_pos);
    ri = TT_CONTAINER(rb_pos, rb_item, node);
    TT_TEST_CHECK_EQUAL(ri->val, 12400, "");

    rb_pos = tt_rbtree_next(rb_pos);
    ri = TT_CONTAINER(rb_pos, rb_item, node);
    TT_TEST_CHECK_EQUAL(ri->val, 25649, "");

    rb_pos = tt_rbtree_next(rb_pos);
    ri = TT_CONTAINER(rb_pos, rb_item, node);
    TT_TEST_CHECK_EQUAL(ri->val, 26123, "");

    rb_pos = tt_rbtree_next(rb_pos);
    TT_TEST_CHECK_EQUAL(rb_pos, NULL, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

tt_s32_t test_u8_comparer(IN void *l, IN void *r)
{
    return (*((tt_u8_t *)l) - *((tt_u8_t *)r));
}

tt_s32_t test_comparer(IN void *l, IN void *r)
{
    tt_u32_t lv = *((tt_u32_t *)l);
    tt_u32_t rv = *((tt_u32_t *)r);

    if (lv < rv)
        return -1;
    else if (lv == rv)
        return 0;
    else
        return 1;
}

tt_s32_t test_rb_comparer(IN void *l, IN void *r)
{
    tt_u32_t lv = (TT_CONTAINER(l, rb_item, node))->val;
    tt_u32_t rv = (TT_CONTAINER(r, rb_item, node))->val;

    if (lv < rv)
        return -1;
    else if (lv == rv)
        return 0;
    else
        return 1;
}

tt_s32_t test_rb_key_comparer(IN void *l,
                              IN const tt_u8_t *key,
                              tt_u32_t key_len)
{
    tt_u32_t lv = (TT_CONTAINER(l, rb_item, node))->val;

    TT_ASSERT(key_len == sizeof(tt_u32_t));
    if (lv < *(tt_u32_t *)key)
        return -1;
    else if (lv == *(tt_u32_t *)key)
        return 0;
    else
        return 1;
}


tt_s32_t test_lst_comparer(IN void *l, IN void *r)
{
    return (((lst_item *)l)->val - ((lst_item *)r)->val);
}

#define __RBNODE_NUM 10000

static tt_thread_t *test_threads[4];
static tt_rbtree_t __rbtree;
static tt_spinlock_t __lock;
static rb_item __rbi[sizeof(test_threads) / sizeof(tt_thread_t *)]
                    [__RBNODE_NUM];

static tt_result_t test_routine_rb(IN tt_thread_t *thread, IN void *param)
{
    tt_ptrdiff_t idx = (tt_ptrdiff_t)param;
    int i = 0;
    int j = 0;
    // tt_result_t ret = TT_FAIL;

    rb_item *prbi = __rbi[idx];
    int mt = sizeof(test_threads) / sizeof(tt_thread_t *) - 1;

    srand((tt_u32_t)time(NULL));

    for (i = 0; i < __RBNODE_NUM; ++i) {
        int op = rand() % 3;
        // 1/3 remove, 2/3 insert

        if (op) {
            tt_rbnode_init(&prbi[i].node);
            while ((prbi[i].val = rand()) == 0)
                ;

            if (mt)
                tt_spinlock_acquire(&__lock);
            tt_rbtree_add(&__rbtree, &prbi[i].node);
            if (mt)
                tt_spinlock_release(&__lock);
        } else {
            for (; j < i; ++j) {
                if (prbi[j].val != 0) {
                    if (mt)
                        tt_spinlock_acquire(&__lock);
                    tt_rbtree_remove(&prbi[j].node);
                    if (mt)
                        tt_spinlock_release(&__lock);
                    prbi[j].val = 0;
                    break;
                }
            }
        }
    }

    // free all
    for (i = 0; i < __RBNODE_NUM; ++i) {
        if (prbi[i].val != 0) {
            if (mt)
                tt_spinlock_acquire(&__lock);
            tt_rbtree_remove(&prbi[i].node);
            if (mt)
                tt_spinlock_release(&__lock);
            prbi[i].val = 0;
            break;
        }
    }

    return TT_SUCCESS;
}

#ifdef TT_VS_RB2
typedef struct rb_item_struct2
{
    tt_u32_t val;
    struct rb_node node;
} rb_item2;

static struct rb_root __rbtree2;
static rb_item2 __rbi2[sizeof(test_threads) / sizeof(tt_thread_t *)]
                      [__RBNODE_NUM];

static tt_result_t test_routine_rb2(IN tt_thread_t *thread, IN void *param)
{
    tt_ptrdiff_t idx = (tt_ptrdiff_t)param;
    int i = 0;
    int j = 0;
    tt_result_t ret = TT_FAIL;

    rb_item2 *prbi = __rbi2[idx];
    rb_item2 *rbi;
    int mt = sizeof(test_threads) / sizeof(tt_thread_t *) - 1;

    struct rb_node **p, *parent = NULL;

    // TT_ASSERT(thread == &test_threads[idx]);

    for (i = 0; i < __RBNODE_NUM; ++i) {
        int op = rand() % 3;
        // 1/3 remove, 2/3 insert

        if (op) {
            while ((prbi[i].val = rand()) == 0)
                ;

            if (mt)
                tt_spinlock_acquire(&__lock);

            p = &__rbtree2.rb_node;
            while (*p) {
                parent = *p;
                rbi = TT_CONTAINER(parent, rb_item2, node);

                if (prbi[i].val < rbi->val)
                    p = &(*p)->rb_left;
                else
                    p = &(*p)->rb_right;
            }

            rb_link_node(&prbi[i].node, parent, p);

            if (mt)
                tt_spinlock_release(&__lock);
        } else {
            for (; j < i; ++j) {
                if (prbi[j].val != 0) {
                    if (mt)
                        tt_spinlock_acquire(&__lock);
                    rb_erase(&prbi[j].node, &__rbtree2);
                    if (mt)
                        tt_spinlock_release(&__lock);
                    prbi[j].val = 0;
                    break;
                }
            }
        }
    }

    // free all
    for (i = 0; i < __RBNODE_NUM; ++i) {
        if (prbi[i].val != 0) {
            if (mt)
                tt_spinlock_acquire(&__lock);
            rb_erase(&prbi[i].node, &__rbtree2);
            if (mt)
                tt_spinlock_release(&__lock);
            prbi[i].val = 0;
            break;
        }
    }

    return TT_SUCCESS;
}
#endif

#ifdef TT_VS_RB3
#ifdef __cplusplus
}

#include <map>
using namespace std;

#endif

typedef struct rb_item_struct3
{
    tt_u32_t val;
    tt_u32_t dummy;
} rb_item3;

static multimap<tt_u32_t, rb_item3 *> __rbtree3;
static rb_item3 __rbi3[sizeof(test_threads) / sizeof(tt_thread_t *)]
                      [__RBNODE_NUM];

static tt_result_t test_routine_rb3(IN tt_thread_t *thread, IN void *param)
{
    tt_ptrdiff_t idx = (tt_ptrdiff_t)param;
    int i = 0;
    int j = 0;
    tt_result_t ret = TT_FAIL;

    rb_item3 *prbi = __rbi3[idx];
    int mt = sizeof(test_threads) / sizeof(tt_thread_t *) - 1;


    // TT_ASSERT(thread == &test_threads[idx]);

    for (i = 0; i < __RBNODE_NUM; ++i) {
        int op = rand() % 3;
        // 1/3 remove, 2/3 insert

        if (op) {
            while ((prbi[i].val = rand()) == 0)
                ;

            if (mt)
                tt_spinlock_acquire(&__lock);

            __rbtree3.insert(make_pair(prbi[i].val, &prbi[i]));

            if (mt)
                tt_spinlock_release(&__lock);
        } else {
            for (; j < i; ++j) {
                if (prbi[j].val != 0) {
                    if (mt)
                        tt_spinlock_acquire(&__lock);

                    __rbtree3.erase(__rbtree3.find(prbi[j].val));

                    if (mt)
                        tt_spinlock_release(&__lock);
                    prbi[j].val = 0;
                    break;
                }
            }
        }
    }

    // free all
    for (i = 0; i < __RBNODE_NUM; ++i) {
        if (prbi[i].val != 0) {
            if (mt)
                tt_spinlock_acquire(&__lock);
            __rbtree3.erase(__rbtree3.find(prbi[j].val));
            if (mt)
                tt_spinlock_release(&__lock);
            prbi[i].val = 0;
            break;
        }
    }

    return TT_SUCCESS;
}
#ifdef __cplusplus
extern "C" {
#endif
#endif

TT_TEST_ROUTINE_DEFINE(tt_unit_test_alg_rbtree_mt)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

    // tt_result_t result = TT_FAIL;
    tt_ptrdiff_t i;
    tt_s64_t start_time, end_time;
    tt_u32_t t1, t2, t3;

    TT_TEST_CASE_ENTER()
    // test start

    tt_rbtree_init(&__rbtree, test_rb_comparer, test_rb_key_comparer);
    tt_spinlock_create(&__lock, NULL);

    srand((int)time(NULL));

    start_time = tt_time_ref();
    for (i = 0; i < sizeof(test_threads) / sizeof(tt_thread_t *); ++i) {
        test_threads[i] =
            tt_thread_create(NULL, test_routine_rb, (void *)i, NULL);
    }
    for (i = 0; i < sizeof(test_threads) / sizeof(tt_thread_t *); ++i) {
        tt_thread_wait(test_threads[i]);
    }
    end_time = tt_time_ref();
    t1 = (tt_u32_t)tt_time_ref2ms(end_time - start_time);

#ifdef TT_VS_RB2
    __rbtree2.rb_node = NULL;
    start_time = tt_time_ref();
    for (i = 0; i < sizeof(test_threads) / sizeof(tt_thread_t *); ++i) {
        tt_thread_create(&test_threads[i],
                         NULL,
                         test_routine_rb2,
                         (void *)i,
                         NULL);
    }
    for (i = 0; i < sizeof(test_threads) / sizeof(tt_thread_t *); ++i) {
        tt_thread_wait(&test_threads[i]);
    }
    end_time = tt_time_ref();
    t2 = (tt_u32_t)tt_time_ref2ms(end_time - start_time);
#else
    t2 = 0;
#endif

#ifdef TT_VS_RB3
    start_time = tt_time_ref();
    for (i = 0; i < sizeof(test_threads) / sizeof(tt_thread_t *); ++i) {
        tt_thread_create(&test_threads[i],
                         NULL,
                         test_routine_rb3,
                         (void *)i,
                         NULL);
    }
    for (i = 0; i < sizeof(test_threads) / sizeof(tt_thread_t *); ++i) {
        tt_thread_wait(&test_threads[i]);
    }
    end_time = tt_time_ref();
    t3 = (tt_u32_t)tt_time_ref2ms(end_time - start_time, NULL);
#else
    t3 = 0;
#endif

    TT_RECORD_INFO("rb mt time consumed: %d ms, %dms, %dms", t1, t2, t3);

    // test end
    TT_TEST_CASE_LEAVE()
}

#ifdef __cplusplus
}
#endif

#define __STK_NUM 100000
#define __STK_LEFT_NUM 10000
static int __stk_ar[__STK_NUM];

static int __stk_des;
static int __stk_cnt;
void __stk_destructor(IN void *obj)
{
    if (__stk_des)
        ++__stk_cnt;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_alg_stack)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

    tt_result_t result = TT_FAIL;
    tt_stack_t stk;
    int i, val, *pval;
    tt_stack_attr_t stk_attr;

    TT_TEST_CASE_ENTER()
    // test start

    tt_stack_attr_default(&stk_attr);
    stk_attr.obj_destroy = __stk_destructor;

    result = tt_stack_create(&stk, sizeof(int), 0, &stk_attr);
    TT_TEST_CHECK_EQUAL(result, TT_SUCCESS, "");

    pval = (int *)tt_stack_top(&stk);
    TT_TEST_CHECK_EQUAL(pval, NULL, "");
    result = tt_stack_pop(&stk, (tt_u8_t *)&val, sizeof(int));
    TT_TEST_CHECK_NOT_EQUAL(result, TT_SUCCESS, "");

    tt_stack_clear(&stk);
    pval = (int *)tt_stack_top(&stk);
    TT_TEST_CHECK_EQUAL(pval, NULL, "");
    result = tt_stack_pop(&stk, (tt_u8_t *)&val, sizeof(int));
    TT_TEST_CHECK_NOT_EQUAL(result, TT_SUCCESS, "");

    TT_TEST_CHECK_EQUAL(tt_stack_pop(&stk, (tt_u8_t *)&val, sizeof(int)),
                        TT_NO_RESOURCE,
                        "");

    for (i = 0; i < __STK_NUM; ++i) {
        __stk_ar[i] = (int)rand();
        if (__stk_ar[i] == 0)
            __stk_ar[i] = 1;
    }

    for (i = 0; i < __STK_NUM; ++i)
        tt_stack_push(&stk, (tt_u8_t *)&__stk_ar[i], sizeof(int));

    // pop half
    for (i = __STK_NUM - 1; i > __STK_NUM / 2; --i) {
        pval = (int *)tt_stack_top(&stk);
        TT_TEST_CHECK_NOT_EQUAL(pval, NULL, "");
        TT_TEST_CHECK_EQUAL(*pval, __stk_ar[i], "");

        result = tt_stack_pop(&stk, (tt_u8_t *)&val, sizeof(int));
        TT_TEST_CHECK_EQUAL(result, TT_SUCCESS, "");
        TT_TEST_CHECK_EQUAL(val, __stk_ar[i], "");
    }

    ++i;
    for (; i < __STK_NUM; ++i)
        tt_stack_push(&stk, (tt_u8_t *)&__stk_ar[i], sizeof(int));

    // pop whole
    for (i = __STK_NUM - 1; i >= __STK_LEFT_NUM; --i) {
        pval = (int *)tt_stack_top(&stk);
        TT_TEST_CHECK_NOT_EQUAL(pval, NULL, "");
        TT_TEST_CHECK_EQUAL(*pval, __stk_ar[i], "");

        result = tt_stack_pop(&stk, (tt_u8_t *)&val, sizeof(int));
        TT_TEST_CHECK_EQUAL(result, TT_SUCCESS, "");
        TT_TEST_CHECK_EQUAL(val, __stk_ar[i], "");
    }

    __stk_des = 1;
    __stk_cnt = 0;
    result = tt_stack_destroy(&stk);
    TT_TEST_CHECK_EQUAL(result, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(__stk_cnt, __STK_LEFT_NUM, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_alg_ptr_stack)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

    tt_result_t result = TT_FAIL;
    tt_ptrstack_t stk;
    int i, *p;
    tt_stack_attr_t sattr;

    TT_TEST_CASE_ENTER()
    // test start

    tt_stack_attr_default(&sattr);
    sattr.obj_destroy = __stk_destructor;

    result = tt_ptrstack_create(&stk, 0, &sattr);
    TT_TEST_CHECK_EQUAL(result, TT_SUCCESS, "");

    TT_TEST_CHECK_EQUAL(tt_ptrstack_pop(&stk), NULL, "");
    TT_TEST_CHECK_EQUAL(tt_ptrstack_top(&stk), NULL, "");

    for (i = 0; i < __STK_NUM; ++i) {
        __stk_ar[i] = (int)rand();
        if (__stk_ar[i] == 0)
            __stk_ar[i] = 1;
    }

    for (i = 0; i < __STK_NUM; ++i)
        tt_ptrstack_push(&stk, &__stk_ar[i]);

    // pop half
    for (i = __STK_NUM - 1; i > __STK_NUM / 2; --i) {
        int *p;

        p = (int *)tt_ptrstack_top(&stk);
        TT_TEST_CHECK_EQUAL(*p, __stk_ar[i], "");

        p = (int *)tt_ptrstack_pop(&stk);
        TT_TEST_CHECK_EQUAL(*p, __stk_ar[i], "");
    }

    ++i;
    for (; i < __STK_NUM; ++i)
        tt_ptrstack_push(&stk, &__stk_ar[i]);

    // pop whole
    for (i = __STK_NUM - 1; i >= __STK_LEFT_NUM; --i) {
        int *p;

        p = (int *)tt_ptrstack_top(&stk);
        TT_TEST_CHECK_EQUAL(*p, __stk_ar[i], "");

        p = (int *)tt_ptrstack_pop(&stk);
        TT_TEST_CHECK_EQUAL(*p, __stk_ar[i], "");
    }

    tt_stack_clear(&stk);
    p = (int *)tt_ptrstack_pop(&stk);
    TT_TEST_CHECK_EQUAL(p, NULL, "");
    p = tt_ptrstack_pop(&stk);
    TT_TEST_CHECK_EQUAL(p, NULL, "");

#if 0
    // stack is cleared, so can not test __stk_cnt
    __stk_des = 1;
    __stk_cnt = 0;
    result = tt_ptrstack_destroy(&stk);
    TT_TEST_CHECK_EQUAL(result, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(__stk_cnt, __STK_LEFT_NUM, "");
#else
    result = tt_ptrstack_destroy(&stk);
    TT_TEST_CHECK_EQUAL(result, TT_SUCCESS, "");
#endif

    // test end
    TT_TEST_CASE_LEAVE()
}

struct __h1_case
{
    const tt_char_t *key;
    tt_u32_t key_len;
    tt_u32_t seed;
    tt_hashcode_t hash;
};

struct __h1_case __h1_murmur3[] = {
    {"exfcm", 5, 0x2ba3a788, 0x39a853b6},
    {"zehwxw", 6, 0x5e26585b, 0x16fa4c03},
    {"ruoyadfvd", 9, 0x4d501d6a, 0xebb727cd},
    {"zr", 2, 0x339448a9, 0x3edb7ca5},
    {"qjf", 3, 0xe7546e8, 0xcf67c742},
};

void __h1_node2key(IN tt_hnode_t *node,
                   OUT const tt_u8_t **key,
                   OUT tt_u32_t *key_len)
{
    // dummy function
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_alg_hash_calc)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_hashmap_t hmap;
    tt_hashmap_attr_t attr;
    tt_result_t ret;
    tt_u32_t i;

    TT_TEST_CASE_ENTER()
    // test start

    tt_hashmap_attr_default(&attr);

    // test murmur3
    attr.hashalg = TT_HASHFUNC_MURMUR3;

    ret = tt_hashmap_create(&hmap, 1, __h1_node2key, &attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    for (i = 0; i < sizeof(__h1_murmur3) / sizeof(struct __h1_case); ++i) {
        tt_hashcode_t hash;

        // hack for test
        hmap.hashctx.seed = __h1_murmur3[i].seed;

        hash = tt_hashmap_hash(&hmap,
                               (tt_u8_t *)__h1_murmur3[i].key,
                               __h1_murmur3[i].key_len);
        TT_TEST_CHECK_EQUAL(__h1_murmur3[i].hash, hash, "");
    }

    // two same inputs generate same key
    do {
        char __ccc[] = "123456789abcdef";
        tt_hashcode_t v1 =
            tt_hashmap_hash(&hmap, (tt_u8_t *)__ccc, sizeof(__ccc));
        tt_hashcode_t v2 =
            tt_hashmap_hash(&hmap, (tt_u8_t *)__ccc, sizeof(__ccc));
        TT_TEST_CHECK_EQUAL(v1, v2, "");
    } while (0);

    ret = tt_hashmap_destroy(&hmap);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

#define __h2_num 10000

struct __h2_case_t
{
    tt_hnode_t hnode;

    tt_u8_t *key;
    tt_u32_t key_len;
} __h2_cases[__h2_num];

void __h2_node2key(IN tt_hnode_t *node,
                   OUT const tt_u8_t **key,
                   OUT tt_u32_t *key_len)
{
    struct __h2_case_t *h = TT_CONTAINER(node, struct __h2_case_t, hnode);

    *key = h->key;
    *key_len = h->key_len;
}

void __hm_count(IN struct tt_hashmap_s *hmap,
                IN tt_hnode_t *hnode,
                IN void *param)
{
    tt_u32_t *pi = (tt_u32_t *)param;
    *pi += 1;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_alg_hash_collision)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_hashmap_t hmap;
    tt_hashmap_attr_t attr;
    tt_result_t ret;
    tt_u32_t i, j;

    TT_TEST_CASE_ENTER()
    // test start

    srand((unsigned int)time(NULL));
    // srand(0);

    tt_hashmap_attr_default(&attr);

    attr.hashalg = TT_HASHFUNC_MURMUR3;
    // attr.hashalg = TT_hashfunc_FNV1A;

    // 10000 => 97 slots
    ret = tt_hashmap_create(&hmap, 97, __h2_node2key, &attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    for (i = 0; i < __h2_num; ++i) {
        tt_u32_t len = rand() % 100 + 1;

        __h2_cases[i].key = (tt_u8_t *)tt_malloc(len);
        for (j = 0; j < len; ++j) {
            __h2_cases[i].key[j] = (tt_u8_t)rand();
        }

        __h2_cases[i].key_len = len;

        tt_hnode_init(&__h2_cases[i].hnode);

        // insert
        tt_hashmap_add(&hmap, &__h2_cases[i].hnode);
    }

    // for each
    i = 0;
    tt_hashmap_foreach(&hmap, __hm_count, &i);
    TT_TEST_CHECK_EQUAL(i, __h2_num, "");

    for (i = 0; i < __h2_num; ++i) {
        tt_hnode_t *hnode = tt_hashmap_find(&hmap,
                                            __h2_cases[i].key,
                                            __h2_cases[i].key_len,
                                            NULL);
        while ((hnode != &__h2_cases[i].hnode) && (hnode != NULL)) {
            hnode = tt_hashmap_find(&hmap,
                                    __h2_cases[i].key,
                                    __h2_cases[i].key_len,
                                    hnode);
        }

        // must be able to find a node
        TT_TEST_CHECK_NOT_EQUAL(hnode, NULL, "");
    }

    // check max collision
    j = 0;
    for (i = 0; i < hmap.hslot.list_array.list_num; ++i) {
        if (hmap.hslot.list_array.list[i].count > j) {
            j = hmap.hslot.list_array.list[i].count;
        }
    }

    TT_RECORD_INFO("hslot num: %d, max collision: %f%%, %d/%d",
                   hmap.hslot.list_array.list_num,
                   (float)j / (float)__h2_num * 100,
                   j,
                   __h2_num);

    for (i = 0; i < __h2_num; ++i) {
        tt_hashmap_remove(&hmap, &__h2_cases[i].hnode);
    }

    // insert uniq
    {
        tt_hnode_t *hnode = NULL;

        i = tt_rand_u32() % __h2_num;

        tt_hashmap_add(&hmap, &__h2_cases[i].hnode);

        ret = tt_hashmap_adduniq(&hmap, &__h2_cases[i].hnode, &hnode);
        TT_TEST_CHECK_EQUAL(ret, TT_ALREADY_EXIST, "");
        TT_TEST_CHECK_EQUAL(hnode, &__h2_cases[i].hnode, "");

        tt_hashmap_remove(&hmap, hnode);
    }

    for (i = 0; i < __h2_num; ++i) {
        tt_free(__h2_cases[i].key);
    }

    ret = tt_hashmap_destroy(&hmap);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // test end
    TT_TEST_CASE_LEAVE()
}
