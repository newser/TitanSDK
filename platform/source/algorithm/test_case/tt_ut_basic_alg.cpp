/*
 * import header files
 */

#include <tt/algorithm/blob.h>
#include <tt/memory/memory_spring.h>

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
extern void __short_sort(IN void *elem_start, IN tt_u32_t elem_num,
                         IN tt_u32_t elem_size, IN tt_cmp_t comparer);

extern void *__short_bsearch(IN void *key, IN void *elem_start,
                             IN tt_u32_t elem_num, IN tt_u32_t elem_size,
                             IN tt_cmp_t comparer);
extern void *__short_bsearch_gteq(IN void *key, IN void *elem_start,
                                  IN tt_u32_t elem_num, IN tt_u32_t elem_size,
                                  IN tt_cmp_t comparer);
extern void *__short_bsearch_lteq(IN void *key, IN void *elem_start,
                                  IN tt_u32_t elem_num, IN tt_u32_t elem_size,
                                  IN tt_cmp_t comparer);

extern tt_result_t __rbt_expensive_check(IN tt_rbnode_t *node,
                                         IN OUT tt_u32_t *black_node_num);

static tt_s32_t test_rb_key_comparer(IN void *l, IN const tt_u8_t *key,
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
TT_TEST_ROUTINE_DECLARE(case_blobex_cpp)
TT_TEST_ROUTINE_DECLARE(case_memspg_cpp)

TT_TEST_ROUTINE_DECLARE(case_alg_rng)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(basic_alg_case)

TT_TEST_CASE("case_basic_alg_qsort", "testing tt_qsort()", case_basic_alg_qsort,
             NULL, NULL, NULL, NULL, NULL)
,

    TT_TEST_CASE("case_basic_alg_qsort_random", "testing tt_qsort()",
                 case_basic_alg_qsort_random, NULL, NULL, NULL, NULL, NULL),

    /*
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
     */

    TT_TEST_CASE("case_alg_rng", "testing random num generator", case_alg_rng,
                 NULL, NULL, NULL, NULL, NULL),

    TT_TEST_CASE("case_blobex", "testing blobex", case_blobex, NULL, NULL, NULL,
                 NULL, NULL),

    TT_TEST_CASE("case_blobex_cpp", "testing blobex", case_blobex_cpp, NULL,
                 NULL, NULL, NULL, NULL),

    TT_TEST_CASE("case_memspg_cpp", "testing memspg", case_memspg_cpp, NULL,
                 NULL, NULL, NULL, NULL),

    TT_TEST_CASE_LIST_DEFINE_END(basic_alg_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(ALG_UT_BASIC_ALG, 0, basic_alg_case)

    /*
     * interface implementation
     */

    /*
    TT_TEST_ROUTINE_DEFINE(case_memspg_cpp)
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

#if 0
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
#endif

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

tt_s32_t test_rb_key_comparer(IN void *l, IN const tt_u8_t *key,
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

            if (mt) tt_spinlock_acquire(&__lock);
            tt_rbtree_add(&__rbtree, (tt_u8_t *)&prbi[i].val,
                          sizeof(&prbi[i].val), &prbi[i].node);
            if (mt) tt_spinlock_release(&__lock);
        } else {
            for (; j < i; ++j) {
                if (prbi[j].val != 0) {
                    if (mt) tt_spinlock_acquire(&__lock);
                    tt_rbtree_remove(&__rbtree, &prbi[j].node);
                    if (mt) tt_spinlock_release(&__lock);
                    prbi[j].val = 0;
                    break;
                }
            }
        }
    }

    // free all
    for (i = 0; i < __RBNODE_NUM; ++i) {
        if (prbi[i].val != 0) {
            if (mt) tt_spinlock_acquire(&__lock);
            tt_rbtree_remove(&__rbtree, &prbi[i].node);
            if (mt) tt_spinlock_release(&__lock);
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

            if (mt) tt_spinlock_acquire(&__lock);

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

            if (mt) tt_spinlock_release(&__lock);
        } else {
            for (; j < i; ++j) {
                if (prbi[j].val != 0) {
                    if (mt) tt_spinlock_acquire(&__lock);
                    rb_erase(&prbi[j].node, &__rbtree2);
                    if (mt) tt_spinlock_release(&__lock);
                    prbi[j].val = 0;
                    break;
                }
            }
        }
    }

    // free all
    for (i = 0; i < __RBNODE_NUM; ++i) {
        if (prbi[i].val != 0) {
            if (mt) tt_spinlock_acquire(&__lock);
            rb_erase(&prbi[i].node, &__rbtree2);
            if (mt) tt_spinlock_release(&__lock);
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

            if (mt) tt_spinlock_acquire(&__lock);

            __rbtree3.insert(make_pair(prbi[i].val, &prbi[i]));

            if (mt) tt_spinlock_release(&__lock);
        } else {
            for (; j < i; ++j) {
                if (prbi[j].val != 0) {
                    if (mt) tt_spinlock_acquire(&__lock);

                    __rbtree3.erase(__rbtree3.find(prbi[j].val));

                    if (mt) tt_spinlock_release(&__lock);
                    prbi[j].val = 0;
                    break;
                }
            }
        }
    }

    // free all
    for (i = 0; i < __RBNODE_NUM; ++i) {
        if (prbi[i].val != 0) {
            if (mt) tt_spinlock_acquire(&__lock);
            __rbtree3.erase(__rbtree3.find(prbi[j].val));
            if (mt) tt_spinlock_release(&__lock);
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
        tt_thread_create(&test_threads[i], NULL, test_routine_rb2, (void *)i,
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
        tt_thread_create(&test_threads[i], NULL, test_routine_rb3, (void *)i,
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
    for (i = 0; i < __RAND_SIZE; ++i) { num[i] = 0; }
    min_n = ~0;
    max_n = 0;

    start = tt_time_ref();
    for (i = 0; i < __RAND_SIZE; ++i) { num[tt_rand_u64() % __RAND_SIZE] += 1; }
    end = tt_time_ref();
    t = tt_time_ref2ms(end - start);

    for (i = 0; i < __RAND_SIZE; ++i) {
        if (num[i] < min_n) min_n = num[i];
        if (num[i] >= max_n) max_n = num[i];
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

    tt_blobex_destroy(&b);
    tt_blobex_destroy(&b2);

    {
        tt_blobex_init(&b, NULL, 0);

        TT_UT_SUCCESS(tt_blobex_memcat(&b, NULL, 0), "");
        TT_UT_EQUAL(tt_blobex_addr(&b), NULL, "");
        TT_UT_EQUAL(tt_blobex_len(&b), 0, "");
        TT_UT_SUCCESS(tt_blobex_memcat(&b, (tt_u8_t *)"1", 1), "");
        TT_UT_NOT_NULL(tt_blobex_addr(&b), "");
        TT_UT_EQUAL(tt_blobex_len(&b), 1, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&b, "1"), 0, "");
        TT_UT_SUCCESS(tt_blobex_memcat(&b, (tt_u8_t *)"23", 2), "");
        TT_UT_NOT_NULL(tt_blobex_addr(&b), "");
        TT_UT_EQUAL(tt_blobex_len(&b), 3, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&b, "123"), 0, "");

        TT_UT_SUCCESS(tt_blobex_set(&b, (tt_u8_t *)"xyz", 3, TT_FALSE), "");

        TT_UT_SUCCESS(tt_blobex_memcat(&b, NULL, 0), "");
        TT_UT_EQUAL(tt_blobex_len(&b), 3, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&b, "xyz"), 0, "");
        TT_UT_SUCCESS(tt_blobex_memcat(&b, (tt_u8_t *)"1", 1), "");
        TT_UT_NOT_NULL(tt_blobex_addr(&b), "");
        TT_UT_EQUAL(tt_blobex_len(&b), 4, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&b, "xyz1"), 0, "");
        TT_UT_SUCCESS(tt_blobex_memcat(&b, (tt_u8_t *)"23", 2), "");
        TT_UT_NOT_NULL(tt_blobex_addr(&b), "");
        TT_UT_EQUAL(tt_blobex_len(&b), 6, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&b, "xyz123"), 0, "");

        tt_blobex_destroy(&b);
    }

    {
        char buf[] = "123";
        tt_u8_t *addr;
        tt_blobex_t a, b;
        tt_blobex_init(&a, (tt_u8_t *)buf, sizeof(buf) - 1);

        tt_blobex_init(&b, NULL, 0);
        TT_UT_SUCCESS(tt_blobex_smart_copy(&b, &a), "");
        TT_UT_EQUAL(tt_blobex_addr(&b), buf, "");

        TT_UT_SUCCESS(tt_blobex_own(&a), "");
        TT_UT_NOT_EQUAL(__BLOBEX_ADDR(&a), (tt_u8_t *)buf, "");
        TT_UT_EXP(tt_blobex_strcmp(&a, buf) == 0, "");

        addr = __BLOBEX_ADDR(&a);
        TT_UT_SUCCESS(tt_blobex_own(&a), "");
        TT_UT_EQUAL(__BLOBEX_ADDR(&a), addr, "");

        TT_UT_SUCCESS(tt_blobex_smart_copy(&b, &a), "");
        TT_UT_NOT_EQUAL(tt_blobex_addr(&b), buf, "");
        TT_UT_NOT_EQUAL(tt_blobex_addr(&b), tt_blobex_addr(&a), "");

        tt_blobex_destroy(&a);
        tt_blobex_destroy(&b);
    }

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_blobex_cpp)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt::blob b, b2, *pb;
    tt_u8_t buf[3] = {'1', '2', '3'};
    tt_u8_t buf2[2] = {'4', '5'};
    tt_u8_t *p;

    TT_TEST_CASE_ENTER()
    // test start

    // create null: reserved space
    {
        tt::blob b(nullptr, 1, true);
        TT_UT_EQUAL(b.size(), 1, "");
        TT_UT_NOT_EQUAL(b.addr(), NULL, "");
        TT_UT_EQUAL(TT_BOOL(b.is_owner()), true, "");
        uint8_t *p = (uint8_t *)b.addr();
        p[0] = 1;
    }

    // create with data
    {
        tt::blob b(buf, 3, true);
        TT_UT_EQUAL(b.size(), 3, "");
        p = (tt_u8_t *)b.addr();
        TT_UT_EQUAL(p[0], '1', "");
        TT_UT_EQUAL(p[1], '2', "");
        TT_UT_EQUAL(p[2], '3', "");
        TT_UT_EQUAL(TT_BOOL(b.is_owner()), true, "");
        TT_UT_NOT_EQUAL(b.addr(), buf, "");
    }

    // init null
    {
        tt::blob b(NULL, 3, false);
        TT_UT_EQUAL(b.addr(), NULL, "");
        TT_UT_EQUAL(b.size(), 3, "");
        TT_UT_EQUAL(b.is_owner(), false, "");
    }

    // init with data
    {
        tt::blob b(buf, 3, false);
        TT_UT_EQUAL(b.addr(), buf, "");
        p = (tt_u8_t *)b.addr();
        TT_UT_EQUAL(p[0], '1', "");
        TT_UT_EQUAL(p[1], '2', "");
        TT_UT_EQUAL(p[2], '3', "");
        TT_UT_EQUAL(b.size(), 3, "");
        TT_UT_EQUAL(TT_BOOL(b.is_owner()), false, "");
    }

    // owner to ref
    {
        tt::blob b(buf, 3, true);
        b.set(buf2, 2, false);
        TT_UT_EQUAL(b.addr(), buf2, "");
        TT_UT_EQUAL(b.size(), 2, "");
        TT_UT_EQUAL(TT_BOOL(b2.is_owner()), false, "");

        // ref to owner
        b.set(buf, 3, true);
        p = (tt_u8_t *)b.addr();
        TT_UT_NOT_EQUAL(b.addr(), buf, "");
        TT_UT_EQUAL(p[0], '1', "");
        TT_UT_EQUAL(p[1], '2', "");
        TT_UT_EQUAL(p[2], '3', "");
        TT_UT_EQUAL(b.size(), 3, "");
        TT_UT_EQUAL(TT_BOOL(b.is_owner()), true, "");
    }

    // cmp
    {
        tt::blob b(buf, 2, true);
        tt::blob b2(buf2, 2, false);
        TT_UT_EQUAL(b.cmp(b), 0, "");
        TT_UT_EXP(b.cmp(b2) < 0, "");
        TT_UT_EXP(b.cmp(b2.addr(), b2.size()) < 0, "");
        TT_UT_EQUAL(b2.cmp(b2), 0, "");
        TT_UT_EXP(b2.cmp(b) > 0, "");
        TT_UT_EXP(b2.cmp(b.addr(), b.size()) > 0, "");
        TT_UT_EXP(b2.cmp(b2.addr(), b2.size()) == 0, "");

        TT_UT_EQUAL(b.cmp("12"), 0, "");
        TT_UT_EXP(b.cmp("1") > 0, "");
        TT_UT_EXP(b.cmp("123") < 0, "");

        TT_UT_EQUAL(b2.cmp("45"), 0, "");
        TT_UT_EXP(b2.cmp("4") > 0, "");
        TT_UT_EXP(b2.cmp("456") < 0, "");

        TT_UT_TRUE(b == b, "");
        TT_UT_TRUE(b != b2, "");
        TT_UT_TRUE(b < b2, "");
        TT_UT_TRUE(b <= b2, "");
        TT_UT_TRUE(b2 > b, "");
        TT_UT_TRUE(b2 >= b, "");

        TT_UT_TRUE(b == "12", "");
        TT_UT_TRUE(b != "45", "");
        TT_UT_TRUE(b < "45", "");
        TT_UT_TRUE(b <= "45", "");
        TT_UT_TRUE(b > "11", "");
        TT_UT_TRUE(b >= "1", "");
    }

    {
        tt::blob b(NULL, 0, false);

        b.cat(NULL, 0);
        TT_UT_EQUAL(b.addr(), NULL, "");
        TT_UT_EQUAL(b.size(), 0, "");
        b.cat((tt_u8_t *)"1", 1);
        TT_UT_NOT_NULL(b.addr(), "");
        TT_UT_EQUAL(b.size(), 1, "");
        TT_UT_EQUAL(b.cmp("1"), 0, "");
        b.cat((tt_u8_t *)"23", 2);
        TT_UT_NOT_NULL(b.addr(), "");
        TT_UT_EQUAL(b.size(), 3, "");
        TT_UT_EQUAL(b.cmp("123"), 0, "");

        b.set((tt_u8_t *)"xyz", 3, false);

        b.cat(NULL, 0);
        TT_UT_EQUAL(b.size(), 3, "");
        TT_UT_EQUAL(b.cmp("xyz"), 0, "");
        b.cat((tt_u8_t *)"1", 1);
        TT_UT_NOT_NULL(b.addr(), "");
        TT_UT_EQUAL(b.size(), 4, "");
        TT_UT_EQUAL(b.cmp("xyz1"), 0, "");
        b.cat((tt_u8_t *)"23", 2);
        TT_UT_NOT_NULL(b.addr(), "");
        TT_UT_EQUAL(b.size(), 6, "");
        TT_UT_EQUAL(b.cmp("xyz123"), 0, "");

        b.clear();
        b += "1";
        b += "22";
        b += "333";

        tt::blob b2((void *)"4444", 4, false);
        b += b2;
        TT_UT_TRUE(b == "1223334444", "");
    }

    {
        tt::blob b;
        b.set("123", false);

        tt::blob b2("4567", true);
        b.copy(b2);
        TT_UT_TRUE(b == "4567", "");
        TT_UT_TRUE(b.is_owner(), "");
        TT_UT_TRUE(b2 == "4567", "");

        tt::blob b3("abc", false);
        b.move(b3);
        TT_UT_TRUE(b == "abc", "");
        TT_UT_FALSE(b.is_owner(), "");
        TT_UT_TRUE(b3.empty(), "");

        b.move(tt::blob{"xy", true});
        TT_UT_TRUE(b == "xy", "");
        TT_UT_TRUE(b.is_owner(), "");

        tt::blob b4(tt::blob{"xy", true});
        TT_UT_TRUE(b4 == "xy", "");
        TT_UT_TRUE(b4.is_owner(), "");
    }

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_memspg_cpp)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

    TT_TEST_CASE_ENTER()
    // test start

    {
        using test_memspg = tt::memspg<6, 12>;
        TT_UT_EQUAL(test_memspg::align_size(0), 1 << 6, "");
        TT_UT_EQUAL(test_memspg::align_size((1 << 6) - 1), 1 << 6, "");
        TT_UT_EQUAL(test_memspg::align_size(1 << 6), 1 << 6, "");

        TT_UT_EQUAL(test_memspg::align_size((1 << 6) + 1), 1 << 7, "");
        TT_UT_EQUAL(test_memspg::align_size((1 << 7) + 1), 1 << 8, "");
        TT_UT_EQUAL(test_memspg::align_size((1 << 12) - 1), 1 << 12, "");
        TT_UT_EQUAL(test_memspg::align_size((1 << 12)), 1 << 12, "");

        TT_UT_EQUAL(test_memspg::align_size((1 << 12)), 1 << 12, "");
        TT_UT_EQUAL(test_memspg::align_size((1 << 12) + 1), 2 << 12, "");
        TT_UT_EQUAL(test_memspg::align_size((1 << 13) + (1 << 12) + 1),
                    (1 << 13) + (2 << 12), "");
    }

    {
        tt::memspg<> m;
    }

    {
        tt::memspg<6, 9> m; // high: 512
        uint8_t *p = (uint8_t *)m.addr();
        TT_UT_NOT_EQUAL(p, nullptr, "");
        TT_UT_EQUAL(m.size(), 1 << 6, "");
        p[0] = 0;
        p[63] = 0;

        uint8_t *p2 = (uint8_t *)m.resize(1);
        TT_UT_EQUAL(p, p2, "");
        TT_UT_EQUAL(m.size(), 1 << 6, "");
        p2[0] = 0;
        p2[63] = 0;

        p2 = (uint8_t *)m.resize(63);
        TT_UT_EQUAL(p, p2, "");
        TT_UT_EQUAL(m.size(), 1 << 6, "");
        p2[0] = 0;
        p2[63] = 0;

        p2 = (uint8_t *)m.resize(64);
        TT_UT_EQUAL(p, p2, "");
        TT_UT_EQUAL(m.size(), 1 << 6, "");
        p2[0] = 0;
        p2[63] = 0;

        p2 = (uint8_t *)m.resize(65);
        TT_UT_NOT_EQUAL(p, p2, "");
        TT_UT_EQUAL(m.size(), 128, "");
        p2[0] = 0;
        p2[127] = 0;

        uint8_t *p3 = (uint8_t *)m.resize(127);
        TT_UT_EQUAL(p3, p2, "");
        p3 = (uint8_t *)m.resize(128);
        TT_UT_EQUAL(p3, p2, "");

        p3 = (uint8_t *)m.resize(129);
        TT_UT_NOT_EQUAL(p3, p2, "");
        TT_UT_EQUAL(m.size(), 256, "");

        p2 = (uint8_t *)m.resize(256);
        TT_UT_EQUAL(p3, p2, "");
        TT_UT_EQUAL(m.size(), 256, "");

        p3 = (uint8_t *)m.resize(257);
        TT_UT_NOT_EQUAL(p3, p2, "");
        TT_UT_EQUAL(m.size(), 512, "");

        p2 = (uint8_t *)m.resize(513);
        TT_UT_NOT_EQUAL(p3, p2, "");
        TT_UT_EQUAL(m.size(), 1024, "");

        p3 = (uint8_t *)m.resize(1024);
        TT_UT_EQUAL(p3, p2, "");
        TT_UT_EQUAL(m.size(), 1024, "");

        p2 = (uint8_t *)m.resize(1025);
        TT_UT_NOT_EQUAL(p3, p2, "");
        TT_UT_EQUAL(m.size(), 1536, "");

        // less
        void *old_p = p2;
        p2 = (uint8_t *)m.resize(1535);
        TT_UT_EQUAL(p2, old_p, "");
        TT_UT_EQUAL(m.size(), 1536, "");

        old_p = p2;
        p2 = (uint8_t *)m.resize(1500);
        TT_UT_EQUAL(p2, old_p, "");
        TT_UT_EQUAL(m.size(), 1536, "");

        old_p = p2;
        p2 = (uint8_t *)m.resize(1025);
        TT_UT_EQUAL(p2, old_p, "");
        TT_UT_EQUAL(m.size(), 1536, "");

        old_p = p2;
        p2 = (uint8_t *)m.resize(1024);
        TT_UT_NOT_EQUAL(p2, old_p, "");
        TT_UT_EQUAL(m.size(), 1024, "");

        old_p = p2;
        p2 = (uint8_t *)m.resize(512);
        TT_UT_NOT_EQUAL(p2, old_p, "");
        TT_UT_EQUAL(m.size(), 512, "");

        old_p = p2;
        p2 = (uint8_t *)m.resize(511);
        TT_UT_EQUAL(p2, old_p, "");
        TT_UT_EQUAL(m.size(), 512, "");

        old_p = p2;
        p2 = (uint8_t *)m.resize(64);
        TT_UT_NOT_EQUAL(p2, old_p, "");
        TT_UT_EQUAL(m.size(), 64, "");
    }

    {
        tt::memspg<6, 8> m; // 64, 256
        uint8_t *p = (uint8_t *)m.resize(128);
        for (int i = 0; i < 128; ++i) { p[i] = i; }

        // head
        p = (uint8_t *)m.resize(250, 0, 120);
        TT_UT_EQUAL(p[0], 0, "");
        TT_UT_EQUAL(p[119], 119, "");

        // mid
        p = (uint8_t *)m.resize(500, 10, 110);
        TT_UT_EQUAL(p[10], 10, "");
        TT_UT_EQUAL(p[119], 119, "");

        // mid
        p = (uint8_t *)m.resize(1000, 20, 80);
        TT_UT_EQUAL(p[20], 20, "");
        TT_UT_EQUAL(p[99], 99, "");

        // less
        p = (uint8_t *)m.resize(600, 20, 70);
        TT_UT_EQUAL(p[20], 20, "");
        TT_UT_EQUAL(p[89], 89, "");

        p = (uint8_t *)m.resize(100, 20, 60);
        TT_UT_EQUAL(p[20], 20, "");
        TT_UT_EQUAL(p[79], 79, "");

        p = (uint8_t *)m.resize(64, 20, 60);
        TT_UT_EQUAL(p[20], 20, "");
        TT_UT_EQUAL(p[63], 63, "");

        // again
        p = (uint8_t *)m.resize(128);
        for (int i = 0; i < 128; ++i) { p[i] = i; }
        p = (uint8_t *)m.resize(64, 80, 20);

        // again
        p = (uint8_t *)m.resize(128);
        for (int i = 0; i < 128; ++i) { p[i] = i; }
        p = (uint8_t *)m.resize(64, 50, 30, 10);
        TT_UT_EQUAL(p[10], 50, "");
        TT_UT_EQUAL(p[23], 63, "");

        p = (uint8_t *)m.resize(250);
        for (int i = 0; i < 250; ++i) { p[i] = i; }
        p = (uint8_t *)m.resize(128, 200, 50, 100);
        TT_UT_EQUAL(p[100], 200, "");
        TT_UT_EQUAL(p[127], 227, "");

        p = (uint8_t *)m.resize(250);
        for (int i = 0; i < 250; ++i) { p[i] = i; }
        p = (uint8_t *)m.resize(128, 100, 50, 50);
        TT_UT_EQUAL(p[50], 100, "");
        TT_UT_EQUAL(p[99], 149, "");
    }

    // test end
    TT_TEST_CASE_LEAVE()
}
