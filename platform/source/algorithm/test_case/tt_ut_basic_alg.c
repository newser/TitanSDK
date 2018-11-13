/*
 * import header files
 */
#ifdef __cplusplus
extern "C" {
#endif

#include <unit_test/tt_unit_test.h>

#include <algorithm/tt_binary_search.h>
#include <algorithm/tt_blobex.h>
#include <algorithm/tt_hashmap.h>
#include <algorithm/tt_list.h>
#include <algorithm/tt_red_black_tree.h>
#include <algorithm/tt_rng_xorshift.h>
#include <algorithm/tt_sort.h>
#include <algorithm/tt_stack.h>
#include <log/tt_log.h>
#include <memory/tt_memory_alloc.h>
#include <os/tt_spinlock.h>
#include <os/tt_thread.h>
#include <time/tt_time_reference.h>

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
extern void *__short_bsearch_gteq(IN void *key,
                                  IN void *elem_start,
                                  IN tt_u32_t elem_num,
                                  IN tt_u32_t elem_size,
                                  IN tt_cmp_t comparer);
extern void *__short_bsearch_lteq(IN void *key,
                                  IN void *elem_start,
                                  IN tt_u32_t elem_num,
                                  IN tt_u32_t elem_size,
                                  IN tt_cmp_t comparer);

extern tt_result_t __rbt_expensive_check(IN tt_rbnode_t *node,
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
TT_TEST_ROUTINE_DECLARE(case_basic_alg_qsort)
TT_TEST_ROUTINE_DECLARE(case_basic_alg_qsort_random)
TT_TEST_ROUTINE_DECLARE(case_basic_alg_bsearch)
TT_TEST_ROUTINE_DECLARE(case_basic_alg_min_larger)
TT_TEST_ROUTINE_DECLARE(case_basic_alg_max_less)
TT_TEST_ROUTINE_DECLARE(case_blobex)

TT_TEST_ROUTINE_DECLARE(case_alg_rng)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(basic_alg_case)

TT_TEST_CASE("case_basic_alg_qsort",
             "testing tt_qsort()",
             case_basic_alg_qsort,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("case_basic_alg_qsort_random",
                 "testing tt_qsort()",
                 case_basic_alg_qsort_random,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_basic_alg_bsearch",
                 "testing tt_bsearch()",
                 case_basic_alg_bsearch,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_basic_alg_min_larger",
                 "testing tt_bsearch_gteq()",
                 case_basic_alg_min_larger,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_basic_alg_max_less",
                 "testing tt_bsearch_lteq()",
                 case_basic_alg_max_less,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_alg_rng",
                 "testing random num generator",
                 case_alg_rng,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_blobex",
                 "testing blobex",
                 case_blobex,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(basic_alg_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(ALG_UT_BASIC_ALG, 0, basic_alg_case)

    /*
     * interface implementation
     */


    /*
    TT_TEST_ROUTINE_DEFINE(case_alg_rng)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(case_basic_alg_qsort)
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
        TT_UT_EQUAL(test_array1[index], test_array2[index], "");
    }

    // test end
    TT_TEST_CASE_LEAVE()
}

static int c_cmp(const void *l, const void *r)
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

TT_TEST_ROUTINE_DEFINE(case_basic_alg_qsort_random)
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
        // tt_c_qsort(test_array2, array_len, sizeof(tt_u32_t), c_cmp);
        tt_qsort(test_array2, array_len, sizeof(tt_u32_t), tt_cmp_u32);

        // should got same result
        for (index = 0; index < array_len; ++index) {
            TT_UT_EQUAL(test_array1[index], test_array2[index], "");
        }
        for (index = 0; index < array_len - 1; ++index) {
            TT_UT_EXP(test_array1[index] <= test_array1[index + 1], "");
        }

        ++test;
    }

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_basic_alg_bsearch)
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
    TT_UT_EQUAL(*result, key, "");
    result = (tt_u8_t *)__short_bsearch(&key,
                                        test_array1,
                                        32,
                                        sizeof(tt_u8_t),
                                        test_u8_comparer);
    TT_UT_EQUAL(*result, key, "");

    // no matching key
    key = 99;
    result = (tt_u8_t *)
        tt_bsearch(&key, test_array1, 32, sizeof(tt_u8_t), test_u8_comparer);
    TT_UT_EQUAL(result, NULL, "");
    result = (tt_u8_t *)__short_bsearch(&key,
                                        test_array1,
                                        32,
                                        sizeof(tt_u8_t),
                                        test_u8_comparer);
    TT_UT_EQUAL(result, NULL, "");

    // no matching key
    key = 250;
    result = (tt_u8_t *)
        tt_bsearch(&key, test_array1, 32, sizeof(tt_u8_t), test_u8_comparer);
    TT_UT_EQUAL(result, NULL, "");
    result = (tt_u8_t *)__short_bsearch(&key,
                                        test_array1,
                                        32,
                                        sizeof(tt_u8_t),
                                        test_u8_comparer);
    TT_UT_EQUAL(result, NULL, "");

    // no matching key
    key = 121;
    result = (tt_u8_t *)
        tt_bsearch(&key, test_array1, 32, sizeof(tt_u8_t), test_u8_comparer);
    TT_UT_EQUAL(result, NULL, "");
    result = (tt_u8_t *)__short_bsearch(&key,
                                        test_array1,
                                        32,
                                        sizeof(tt_u8_t),
                                        test_u8_comparer);
    TT_UT_EQUAL(result, NULL, "");

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
        TT_UT_EQUAL(*result, key, "");

        result = (tt_u32_t *)__short_bsearch((void *)&key,
                                             test_array2,
                                             array_len,
                                             sizeof(tt_u32_t),
                                             test_comparer);
        TT_UT_EQUAL(*result, key, "");

        ++test;
    }

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_basic_alg_min_larger)
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
    result = (tt_u8_t *)tt_bsearch_gteq(&key,
                                        test_array1,
                                        32,
                                        sizeof(tt_u8_t),
                                        test_u8_comparer);
    TT_UT_EQUAL(*result, 122, "");
    result = (tt_u8_t *)__short_bsearch_gteq(&key,
                                             test_array1,
                                             32,
                                             sizeof(tt_u8_t),
                                             test_u8_comparer);
    TT_UT_EQUAL(*result, 122, "");

    // general search
    key = 122;
    result = (tt_u8_t *)tt_bsearch_gteq(&key,
                                        test_array1,
                                        32,
                                        sizeof(tt_u8_t),
                                        test_u8_comparer);
    TT_UT_EQUAL(*result, 122, "");
    result = (tt_u8_t *)__short_bsearch_gteq(&key,
                                             test_array1,
                                             32,
                                             sizeof(tt_u8_t),
                                             test_u8_comparer);
    TT_UT_EQUAL(*result, 122, "");

    // no matching key
    key = 99;
    result = (tt_u8_t *)tt_bsearch_gteq(&key,
                                        test_array1,
                                        32,
                                        sizeof(tt_u8_t),
                                        test_u8_comparer);
    TT_UT_EQUAL(*result, 100, "");
    result = (tt_u8_t *)__short_bsearch_gteq(&key,
                                             test_array1,
                                             32,
                                             sizeof(tt_u8_t),
                                             test_u8_comparer);
    TT_UT_EQUAL(*result, 100, "");

    // no matching key
    key = 250;
    result = (tt_u8_t *)tt_bsearch_gteq(&key,
                                        test_array1,
                                        32,
                                        sizeof(tt_u8_t),
                                        test_u8_comparer);
    TT_UT_EQUAL(result, NULL, "");
    result = (tt_u8_t *)__short_bsearch_gteq(&key,
                                             test_array1,
                                             32,
                                             sizeof(tt_u8_t),
                                             test_u8_comparer);
    TT_UT_EQUAL(result, NULL, "");

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
        result = (tt_u32_t *)tt_bsearch_gteq((void *)&key,
                                             test_array2,
                                             array_len,
                                             sizeof(tt_u32_t),
                                             test_comparer);
        result2 = (tt_u32_t *)__short_bsearch_gteq((void *)&key,
                                                   test_array2,
                                                   array_len,
                                                   sizeof(tt_u32_t),
                                                   test_comparer);
        TT_UT_EQUAL(*result, *result2, "");

        ++test;
    }

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_basic_alg_max_less)
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
    result = (tt_u8_t *)tt_bsearch_lteq(&key,
                                        test_array1,
                                        32,
                                        sizeof(tt_u8_t),
                                        test_u8_comparer);
    TT_UT_EQUAL(*result, 120, "");
    result = (tt_u8_t *)__short_bsearch_lteq(&key,
                                             test_array1,
                                             32,
                                             sizeof(tt_u8_t),
                                             test_u8_comparer);
    TT_UT_EQUAL(*result, 120, "");

    // general search
    key = 122;
    result = (tt_u8_t *)tt_bsearch_lteq(&key,
                                        test_array1,
                                        32,
                                        sizeof(tt_u8_t),
                                        test_u8_comparer);
    TT_UT_EQUAL(*result, 122, "");
    result = (tt_u8_t *)__short_bsearch_lteq(&key,
                                             test_array1,
                                             32,
                                             sizeof(tt_u8_t),
                                             test_u8_comparer);
    TT_UT_EQUAL(*result, 122, "");

    // no matching key
    key = 99;
    result = (tt_u8_t *)tt_bsearch_lteq(&key,
                                        test_array1,
                                        32,
                                        sizeof(tt_u8_t),
                                        test_u8_comparer);
    TT_UT_EQUAL(result, NULL, "");
    result = (tt_u8_t *)__short_bsearch_lteq(&key,
                                             test_array1,
                                             32,
                                             sizeof(tt_u8_t),
                                             test_u8_comparer);
    TT_UT_EQUAL(result, NULL, "");

    // no matching key
    key = 250;
    result = (tt_u8_t *)tt_bsearch_lteq(&key,
                                        test_array1,
                                        32,
                                        sizeof(tt_u8_t),
                                        test_u8_comparer);
    TT_UT_EQUAL(*result, 162, "");
    result = (tt_u8_t *)__short_bsearch_lteq(&key,
                                             test_array1,
                                             32,
                                             sizeof(tt_u8_t),
                                             test_u8_comparer);
    TT_UT_EQUAL(*result, 162, "");

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
        result = (tt_u32_t *)tt_bsearch_lteq((void *)&key,
                                             test_array2,
                                             array_len,
                                             sizeof(tt_u32_t),
                                             test_comparer);
        result2 = (tt_u32_t *)__short_bsearch_lteq((void *)&key,
                                                   test_array2,
                                                   array_len,
                                                   sizeof(tt_u32_t),
                                                   test_comparer);
        TT_UT_EQUAL(*result, *result2, "");

        ++test;
    }

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

static tt_result_t test_routine_rb(IN void *param)
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
            tt_rbtree_add(&__rbtree,
                          (tt_u8_t *)&prbi[i].val,
                          sizeof(&prbi[i].val),
                          &prbi[i].node);
            if (mt)
                tt_spinlock_release(&__lock);
        } else {
            for (; j < i; ++j) {
                if (prbi[j].val != 0) {
                    if (mt)
                        tt_spinlock_acquire(&__lock);
                    tt_rbtree_remove(&__rbtree, &prbi[j].node);
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
            tt_rbtree_remove(&__rbtree, &prbi[i].node);
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

TT_TEST_ROUTINE_DEFINE(case_alg_rbtree_mt)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

    // tt_result_t result = TT_FAIL;
    tt_ptrdiff_t i;
    tt_s64_t start_time, end_time;
    tt_u32_t t1, t2, t3;

    TT_TEST_CASE_ENTER()
    // test start

    tt_rbtree_init(&__rbtree, NULL);
    tt_spinlock_create(&__lock, NULL);

    srand((int)time(NULL));

    start_time = tt_time_ref();
    for (i = 0; i < sizeof(test_threads) / sizeof(tt_thread_t *); ++i) {
        test_threads[i] = tt_thread_create(test_routine_rb, (void *)i, NULL);
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

#define __RAND_SIZE 100000
TT_TEST_ROUTINE_DEFINE(case_alg_rng)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    static tt_u32_t num[__RAND_SIZE];
    tt_s64_t start, end, t;
    tt_u32_t i, min_n, max_n;
    tt_rng_t *rng;

    TT_TEST_CASE_ENTER()
    // test start

    // xorshift
    rng = tt_rng_xorshift_create();
    for (i = 0; i < __RAND_SIZE; ++i) {
        num[i] = 0;
    }
    min_n = ~0;
    max_n = 0;

    start = tt_time_ref();
    for (i = 0; i < __RAND_SIZE; ++i) {
        num[tt_rand_u64() % __RAND_SIZE] += 1;
    }
    end = tt_time_ref();
    t = tt_time_ref2ms(end - start);

    for (i = 0; i < __RAND_SIZE; ++i) {
        if (num[i] < min_n)
            min_n = num[i];
        if (num[i] >= max_n)
            max_n = num[i];
    }
    TT_RECORD_INFO("xorshift, time: %dms, min: %d, max: %d", t, min_n, max_n);

    tt_rng_destroy(rng);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_blobex)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_blobex_t b, b2, *pb;
    tt_result_t r;
    tt_u8_t buf[3] = {'1', '2', '3'};
    tt_u8_t buf2[2] = {'4', '5'};
    tt_u8_t *p;

    TT_TEST_CASE_ENTER()
    // test start

    // create null: reserved space
    TT_UT_SUCCESS(tt_blobex_create(&b, NULL, 1), "");
    TT_UT_EQUAL(tt_blobex_len(&b), 1, "");
    TT_UT_NOT_EQUAL(tt_blobex_addr(&b), NULL, "");
    TT_UT_EQUAL(TT_BOOL(__BLOBEX_IS_OWNER(&b)), TT_TRUE, "");
    tt_blobex_destroy(&b);

    // create with data
    TT_UT_SUCCESS(tt_blobex_create(&b, buf, 3), "");
    TT_UT_EQUAL(tt_blobex_len(&b), 3, "");
    p = (tt_u8_t *)tt_blobex_addr(&b);
    TT_UT_EQUAL(p[0], '1', "");
    TT_UT_EQUAL(p[1], '2', "");
    TT_UT_EQUAL(p[2], '3', "");
    TT_UT_EQUAL(TT_BOOL(__BLOBEX_IS_OWNER(&b)), TT_TRUE, "");
    tt_blobex_destroy(&b);

    // init null
    tt_blobex_init(&b, NULL, 3);
    TT_UT_EQUAL(tt_blobex_addr(&b), NULL, "");
    TT_UT_EQUAL(tt_blobex_len(&b), 3, "");
    tt_blobex_destroy(&b);

    // init with data
    tt_blobex_init(&b, buf, 3);
    TT_UT_EQUAL(tt_blobex_addr(&b), buf, "");
    p = (tt_u8_t *)tt_blobex_addr(&b);
    TT_UT_EQUAL(p[0], '1', "");
    TT_UT_EQUAL(p[1], '2', "");
    TT_UT_EQUAL(p[2], '3', "");
    TT_UT_EQUAL(tt_blobex_len(&b), 3, "");
    TT_UT_EQUAL(TT_BOOL(__BLOBEX_IS_OWNER(&b)), TT_FALSE, "");
    tt_blobex_destroy(&b);

    // owner to ref
    TT_UT_SUCCESS(tt_blobex_create(&b, buf, 3), "");
    TT_UT_SUCCESS(tt_blobex_set(&b, buf2, 2, TT_FALSE), "");
    TT_UT_EQUAL(tt_blobex_addr(&b), buf2, "");
    TT_UT_EQUAL(tt_blobex_len(&b), 2, "");
    TT_UT_EQUAL(TT_BOOL(__BLOBEX_IS_OWNER(&b)), TT_FALSE, "");

    // ref to owner
    TT_UT_SUCCESS(tt_blobex_set(&b, buf, 3, TT_TRUE), "");
    p = (tt_u8_t *)tt_blobex_addr(&b);
    TT_UT_EQUAL(p[0], '1', "");
    TT_UT_EQUAL(p[1], '2', "");
    TT_UT_EQUAL(p[2], '3', "");
    TT_UT_EQUAL(tt_blobex_len(&b), 3, "");
    TT_UT_EQUAL(TT_BOOL(__BLOBEX_IS_OWNER(&b)), TT_TRUE, "");
    tt_blobex_destroy(&b);

    // cmp
    tt_blobex_create(&b, buf, 2);
    tt_blobex_init(&b2, buf2, 2);
    TT_UT_EQUAL(tt_blobex_cmp(&b, &b), 0, "");
    TT_UT_EXP(tt_blobex_cmp(&b, &b2) < 0, "");
    TT_UT_EXP(tt_blobex_memcmp(&b, tt_blobex_addr(&b2), tt_blobex_len(&b2)) < 0,
              "");
    TT_UT_EQUAL(tt_blobex_cmp(&b2, &b2), 0, "");
    TT_UT_EXP(tt_blobex_cmp(&b2, &b) > 0, "");
    TT_UT_EXP(tt_blobex_memcmp(&b2, tt_blobex_addr(&b), tt_blobex_len(&b)) > 0,
              "");
    TT_UT_EXP(tt_blobex_memcmp(&b2, tt_blobex_addr(&b2), tt_blobex_len(&b2)) ==
                  0,
              "");

    TT_UT_EQUAL(tt_blobex_strcmp(&b, "12"), 0, "");
    TT_UT_EXP(tt_blobex_strcmp(&b, "1") > 0, "");
    TT_UT_EXP(tt_blobex_strcmp(&b, "123") < 0, "");

    TT_UT_EQUAL(tt_blobex_strcmp(&b2, "45"), 0, "");
    TT_UT_EXP(tt_blobex_strcmp(&b2, "4") > 0, "");
    TT_UT_EXP(tt_blobex_strcmp(&b2, "456") < 0, "");

    {
        char buf[] = "123";
        tt_u8_t *addr;
        tt_blobex_t a;
        tt_blobex_init(&a, (tt_u8_t *)buf, sizeof(buf) - 1);
        TT_UT_SUCCESS(tt_blobex_own(&a), "");
        TT_UT_NOT_EQUAL(__BLOBEX_ADDR(&a), (tt_u8_t *)buf, "");
        TT_UT_EXP(tt_blobex_strcmp(&a, buf) == 0, "");

        addr = __BLOBEX_ADDR(&a);
        TT_UT_SUCCESS(tt_blobex_own(&a), "");
        TT_UT_EQUAL(__BLOBEX_ADDR(&a), addr, "");

        tt_blobex_destroy(&a);
    }

    // per thread
    pb = tt_thread_alloc_blobex();
    TT_UT_NOT_NULL(pb, "");
    tt_thread_free_blobex(pb);

    pb = tt_thread_create_blobex(buf, 3);
    TT_UT_NOT_NULL(pb, "");
    TT_UT_EQUAL(tt_blobex_strcmp(pb, "123"), 0, "");
    tt_thread_free_blobex(pb);

    pb = tt_thread_init_blobex(buf2, 2);
    TT_UT_NOT_NULL(pb, "");
    TT_UT_EQUAL(tt_blobex_strcmp(pb, "45"), 0, "");
    tt_thread_free_blobex(pb);

    // test end
    TT_TEST_CASE_LEAVE()
}
