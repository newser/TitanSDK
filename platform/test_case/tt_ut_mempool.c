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

#include <algorithm/tt_stack.h>
#include <log/tt_log.h>
#include <memory/tt_memory_pool.h>
#include <memory/tt_memory_spring.h>
#include <os/tt_mutex.h>
#include <os/tt_thread.h>
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

typedef struct _test_tt_mempool_blockdesc_t
{
    tt_u32_t prev_size;
    tt_u32_t size;
    void *area;

#ifdef TT_MEMORY_TAG_ENABLE
    const tt_char_t *function;
    tt_u32_t line;
#endif
} test_tt_mempool_blockdesc_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

extern tt_u32_t tt_s_blockdesc_size_aligned;

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

// === routine declarations ================
TT_TEST_ROUTINE_DECLARE(tt_unit_test_mempool_basic)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_mempool_alloc_free)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_mempool_mt_aligned)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_mempool_mt_nonaligned)

TT_TEST_ROUTINE_DECLARE(tt_unit_test_ptr_stack)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_mem_spg)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(mempool_case)

#if 1
TT_TEST_CASE("tt_unit_test_mempool_basic",
             "testing mempool basic operation",
             tt_unit_test_mempool_basic,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_mempool_alloc_free",
                 "testing mempool alloc free APIs",
                 tt_unit_test_mempool_alloc_free,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_mempool_mt_aligned",
                 "testing mempool in multi-thread env",
                 tt_unit_test_mempool_mt_aligned,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_mempool_mt_aligned",
                 "testing mempool in multi-thread no aligned",
                 tt_unit_test_mempool_mt_nonaligned,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_ptr_stack",
                 "testing ptr pool",
                 tt_unit_test_ptr_stack,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),
#endif

    TT_TEST_CASE("tt_unit_test_mem_spg",
                 "testing memory spring",
                 tt_unit_test_mem_spg,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(mempool_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(TEST_UNIT_MEMPOOL, 0, mempool_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(tt_unit_test_mem_spg)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(tt_unit_test_mempool_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_mempool_t pool;
    tt_mempool_attr_t pool_attr;
    void *p1, *p2, *p3;

    tt_result_t result = TT_FAIL;

    TT_TEST_CASE_ENTER()
    // test start

    //// test mempool create/destroy 1

    tt_mempool_attr_default(&pool_attr);
    pool_attr.hwcache_align = TT_TRUE;
    pool_attr.sync = TT_TRUE;

    // create a pool
    result = tt_mempool_create(&pool, 8000, &pool_attr);
    TT_TEST_CHECK_EQUAL(result, TT_SUCCESS, "");

    p1 = tt_mempool_alloc(&pool, 100);
    TT_TEST_CHECK_NOT_EQUAL(p1, NULL, "");
    p2 = tt_mempool_alloc(&pool, 8000);
    TT_TEST_CHECK_NOT_EQUAL(p2, NULL, "");
    p3 = tt_mempool_alloc(&pool, 10000);
    TT_TEST_CHECK_EQUAL(p3, NULL, "");

    tt_mempool_free(p1);
    tt_mempool_free(p2);
    tt_mempool_free(p3);

    // destroy
    tt_mempool_destroy(&pool, TT_FALSE);

    //// test mempool create/destroy 2

    // all default
    result = tt_mempool_create(&pool, 2000, NULL);
    TT_TEST_CHECK_EQUAL(result, TT_SUCCESS, "");

    p1 = tt_mempool_alloc(&pool, 100);
    TT_TEST_CHECK_NOT_EQUAL(p1, NULL, "");
    p2 = tt_mempool_alloc(&pool, 2000);
    TT_TEST_CHECK_NOT_EQUAL(p2, NULL, "");
    p3 = tt_mempool_alloc(&pool, 500);
    TT_TEST_CHECK_NOT_EQUAL(p3, NULL, "");

    tt_mempool_free(p2);
    tt_mempool_free(p3);
    // destroy fail
    tt_mempool_destroy(&pool, TT_FALSE);

    tt_mempool_free(p1);
    // destroy ok
    tt_mempool_destroy(&pool, TT_FALSE);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_mempool_alloc_free)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_mempool_t pool;
    tt_mempool_attr_t pool_attr;
    void *p1, *p2, *p3, *p4;

    tt_result_t result = TT_FAIL;
    // tt_numa_node_t *numa_node = tt_g_numa_node[0];
    // tt_memory_env_t *env = &numa_node->memory_env;

    TT_TEST_CASE_ENTER()
    // test start


    // create a pool
    tt_mempool_attr_default(&pool_attr);
    pool_attr.sync = TT_TRUE;
    pool_attr.max_area_num = 1; // only one area
    result = tt_mempool_create(&pool, 9000, &pool_attr);
    TT_TEST_CHECK_EQUAL(result, TT_SUCCESS, "");

    //// start alloc free test

    // exceeding limitation
    p1 = tt_mempool_alloc(&pool, 8999);
    TT_TEST_CHECK_NOT_EQUAL(p1, NULL, "");
    p2 = tt_mempool_alloc(&pool, 9001);
    TT_TEST_CHECK_EQUAL(p2, NULL, "");
    tt_mempool_free(p1);

    // about 128b
    p1 = tt_mempool_alloc(&pool, 100);
    TT_TEST_CHECK_NOT_EQUAL(p1, NULL, "");
    // about 256b
    p2 = tt_mempool_alloc(&pool, 200);
    TT_TEST_CHECK_NOT_EQUAL(p2, NULL, "");
    // all left
    p3 = tt_mempool_alloc(&pool, 7500);
    TT_TEST_CHECK_NOT_EQUAL(p3, NULL, "");
    tt_mempool_show(&pool);
    // free 256b
    tt_mempool_free(p2);
    tt_mempool_show(&pool);
    // allocate in 256b
    // about 128b
    p2 = tt_mempool_alloc(&pool, 100);
    TT_TEST_CHECK_NOT_EQUAL(p2, NULL, "");
    tt_mempool_show(&pool);
    tt_mempool_free(p1);
    // about 128b
    p4 = tt_mempool_alloc(&pool, 100);
    TT_TEST_CHECK_NOT_EQUAL(p4, NULL, "");
    tt_mempool_show(&pool);

    // free
    // now: p4 p2 free p3
    tt_mempool_free(p4);
    tt_mempool_free(p3);
    tt_mempool_free(p2);
    tt_mempool_show(&pool);
    tt_mempool_destroy(&pool, TT_FALSE);

    // test end
    TT_TEST_CASE_LEAVE()
}

#define __SLOT_NUM 10000

static tt_thread_t *test_threads[4];
static tt_mempool_t __mpool;
static void *__aptr[sizeof(test_threads) / sizeof(tt_thread_t *)][__SLOT_NUM];
tt_u32_t __seed;

extern tt_u32_t tt_s_blockdesc_size;

static tt_result_t test_routine_1(IN tt_thread_t *thread, IN void *param)
{
    tt_ptrdiff_t idx = (tt_ptrdiff_t)param;
    int i = 0;
    int j = 0;
    // tt_result_t ret = TT_FAIL;

    void **aptr = __aptr[idx];

    // TT_ASSERT(thread == test_threads[idx]);

    srand(__seed);

    for (i = 0; i < __SLOT_NUM; ++i) {
        int op = rand() % 3;
        // 1/3 free, 2/3 alloc

        if (op) {
            tt_u32_t asize = 0;

            while (asize == 0) {
                asize = rand() & (1 << (tt_g_page_size_order - 3));
            }
            // 1/8 page size per allocation

            aptr[i] = tt_mempool_alloc(&__mpool, asize);
            if (aptr[i]) {
                test_tt_mempool_blockdesc_t *block =
                    TT_PTR_DEC(test_tt_mempool_blockdesc_t,
                               aptr[i],
                               tt_s_blockdesc_size);
                TT_ASSERT(block->size & 1);
                TT_ASSERT(((block->size & ~1) - tt_s_blockdesc_size) >= asize);

                tt_memset(aptr[i], 0xff, asize);
            }
        } else {
            for (; j < i; ++j) {
                if (aptr[j]) {
                    tt_mempool_free(aptr[j]);
                    aptr[j] = NULL;
                    break;
                }
            }
        }
    }

    // free all
    for (i = 0; i < __SLOT_NUM; ++i) {
        if (aptr[i]) {
            tt_mempool_free(aptr[i]);
            aptr[i] = NULL;
        }
    }

    return TT_SUCCESS;
}

static tt_result_t test_routine_sysmalloc(IN tt_thread_t *thread,
                                          IN void *param)
{
    tt_ptrdiff_t idx = (tt_ptrdiff_t)param;
    int i = 0;
    int j = 0;
    // tt_result_t ret = TT_FAIL;

    void **aptr = __aptr[idx];

    // TT_ASSERT(thread == test_threads[idx]);

    srand(__seed);

    for (i = 0; i < __SLOT_NUM; ++i) {
        int op = rand() % 3;
        // 1/3 free, 2/3 alloc

        if (op) {
            tt_u32_t asize = 0;

            while (asize == 0) {
                asize = rand() >> (tt_g_page_size_order - 3);
            }
            // 1/8 page size per allocation

            aptr[i] = malloc(asize);
            // TT_ASSERT(aptr[i] != NULL);
        } else {
            for (; j < i; ++j) {
                if (aptr[j]) {
                    free(aptr[j]);
                    aptr[j] = NULL;
                    break;
                }
            }
        }
    }

    // free all
    for (i = 0; i < __SLOT_NUM; ++i) {
        if (aptr[i]) {
            free(aptr[i]);
            aptr[i] = NULL;
        }
    }

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_mempool_mt_aligned)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

    tt_result_t result = TT_FAIL;
    tt_ptrdiff_t i;
    tt_s64_t start_time, end_time;
    tt_u32_t pool_time, smalloc_time;

    tt_mempool_attr_t pool_attr;
    // tt_memory_env_t *env = &numa_node->memory_env;

    TT_TEST_CASE_ENTER()
    // test start


    //// test mempool create/destroy 1

    tt_mempool_attr_default(&pool_attr);
    pool_attr.hwcache_align = TT_TRUE;
    pool_attr.sync = TT_TRUE;

    // create a pool
    result = tt_mempool_create(&__mpool, 800000, &pool_attr);
    TT_TEST_CHECK_EQUAL(result, TT_SUCCESS, "");

    srand((int)time(NULL));
    __seed = rand();

    tt_memset(__aptr, 0, sizeof(__aptr));
    start_time = tt_time_ref();
    for (i = 0; i < sizeof(test_threads) / sizeof(tt_thread_t *); ++i) {
        test_threads[i] =
            tt_thread_create(NULL, test_routine_1, (void *)i, NULL);
    }
    for (i = 0; i < sizeof(test_threads) / sizeof(tt_thread_t *); ++i) {
        tt_thread_wait(test_threads[i]);
    }
    end_time = tt_time_ref();
    pool_time = (tt_u32_t)tt_time_ref2ms(end_time - start_time);

    tt_memset(__aptr, 0, sizeof(__aptr));
    start_time = tt_time_ref();
    for (i = 0; i < sizeof(test_threads) / sizeof(tt_thread_t *); ++i) {
        test_threads[i] =
            tt_thread_create(NULL, test_routine_sysmalloc, (void *)i, NULL);
    }
    for (i = 0; i < sizeof(test_threads) / sizeof(tt_thread_t *); ++i) {
        tt_thread_wait(test_threads[i]);
    }
    end_time = tt_time_ref();
    smalloc_time = (tt_u32_t)tt_time_ref2ms(end_time - start_time);

    TT_RECORD_INFO("time consumed: %d ms, sys malloc: %d ms",
                   pool_time,
                   smalloc_time);

    tt_mempool_destroy(&__mpool, TT_FALSE);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_mempool_mt_nonaligned)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

    tt_result_t result = TT_FAIL;
    tt_ptrdiff_t i;
    tt_s64_t start_time, end_time;

    tt_mempool_attr_t pool_attr;

    TT_TEST_CASE_ENTER()
    // test start


    //// test mempool create/destroy 1

    tt_mempool_attr_default(&pool_attr);
    // pool_attr.flag |= TT_MEMPOOL_HWCACHELINE_ALIGN;
    pool_attr.sync = TT_TRUE;

    // create a pool
    result = tt_mempool_create(&__mpool, 800000, &pool_attr);
    TT_TEST_CHECK_EQUAL(result, TT_SUCCESS, "");

    srand((int)time(NULL));

    tt_memset(__aptr, 0, sizeof(__aptr));
    start_time = tt_time_ref();
    for (i = 0; i < sizeof(test_threads) / sizeof(tt_thread_t *); ++i) {
        test_threads[i] =
            tt_thread_create(NULL, test_routine_1, (void *)i, NULL);
    }
    for (i = 0; i < sizeof(test_threads) / sizeof(tt_thread_t *); ++i) {
        tt_thread_wait(test_threads[i]);
    }
    end_time = tt_time_ref();
    TT_RECORD_INFO("time consumed: %d ms",
                   (tt_u32_t)tt_time_ref2ms(end_time - start_time));

    tt_mempool_destroy(&__mpool, TT_FALSE);


    // test end
    TT_TEST_CASE_LEAVE()
}

#if 1
#define __PP_NUM 100000
#define __PP_LEFT_NUM 10000
static int __pp_ar[__PP_NUM];

static int __pp_des;
static int __pp_cnt;
void __pp_destructor(IN void *obj)
{
    if (__pp_des)
        ++__pp_cnt;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_ptr_stack)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

    tt_result_t result = TT_FAIL;
    tt_ptrstack_t pp;
    int i;
    tt_stack_attr_t sattr;

    TT_TEST_CASE_ENTER()
    // test start

    tt_stack_attr_default(&sattr);
    sattr.obj_destroy = __pp_destructor;

    result = tt_ptrstack_create(&pp, 0, &sattr);
    TT_TEST_CHECK_EQUAL(result, TT_SUCCESS, "");

    TT_TEST_CHECK_EQUAL(tt_ptrstack_pop(&pp), NULL, "");

    for (i = 0; i < __PP_NUM; ++i) {
        __pp_ar[i] = (int)rand();
        if (__pp_ar[i] == 0)
            __pp_ar[i] = 1;
    }

    for (i = 0; i < __PP_NUM; ++i)
        tt_ptrstack_push(&pp, (tt_ptr_t)(tt_ptrdiff_t)__pp_ar[i]);

    // pop half
    for (i = __PP_NUM - 1; i > __PP_NUM / 2; --i) {
        int n = (int)(tt_ptrdiff_t)tt_ptrstack_pop(&pp);
        TT_TEST_CHECK_EQUAL(n, __pp_ar[i], "");
    }

    ++i;
    for (; i < __PP_NUM; ++i)
        tt_ptrstack_push(&pp, (tt_ptr_t)(tt_ptrdiff_t)__pp_ar[i]);

    // pop whole
    for (i = __PP_NUM - 1; i >= __PP_LEFT_NUM; --i) {
        int n = (int)(tt_ptrdiff_t)tt_ptrstack_pop(&pp);
        TT_TEST_CHECK_EQUAL(n, __pp_ar[i], "");
    }

    __pp_des = 1;
    result = tt_ptrstack_destroy(&pp);
    TT_TEST_CHECK_EQUAL(result, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(__pp_cnt, __PP_LEFT_NUM, "");

    // test end
    TT_TEST_CASE_LEAVE()
}
#endif

extern tt_u32_t __memspg_next_size(IN tt_memspg_t *mspg, IN tt_u32_t size);

TT_TEST_ROUTINE_DEFINE(tt_unit_test_mem_spg)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_memspg_t mspg;
    tt_result_t ret;
    tt_u32_t size;
    tt_u8_t *p;

    TT_TEST_CASE_ENTER()
    // test start

    tt_memspg_init(&mspg, 5, 12, 1 << 30);

    size = __memspg_next_size(&mspg, 0);
    TT_TEST_CHECK_EQUAL(size, 5, "");

    size = __memspg_next_size(&mspg, 5);
    TT_TEST_CHECK_EQUAL(size, 10, "");

    size = __memspg_next_size(&mspg, 6);
    TT_TEST_CHECK_EQUAL(size, 12, "");

    size = __memspg_next_size(&mspg, 11);
    TT_TEST_CHECK_EQUAL(size, 22, "");

    size = __memspg_next_size(&mspg, 12);
    TT_TEST_CHECK_EQUAL(size, 24, "");

    size = __memspg_next_size(&mspg, 13);
    TT_TEST_CHECK_EQUAL(size, 25, "");

    size = __memspg_next_size(&mspg, (1 << 30));
    TT_TEST_CHECK_EQUAL(size, 0, "");

    size = __memspg_next_size(&mspg, ~0);
    TT_TEST_CHECK_EQUAL(size, 0, "");

    size = __memspg_next_size(&mspg, ~0 - 1);
    TT_TEST_CHECK_EQUAL(size, 0, "");

    // no max limit
    tt_memspg_init(&mspg, 5, 12, 0);

    size = __memspg_next_size(&mspg, ~0 - 12);
    TT_TEST_CHECK_EQUAL(size, ~0, "");

    size = __memspg_next_size(&mspg, ~0 - 11);
    TT_TEST_CHECK_EQUAL(size, 0, "");

    // expand
    p = NULL;
    size = ~0 - 1;
    ret = tt_memspg_extend(&mspg, &p, &size, ~0);
    TT_TEST_CHECK_FAIL(ret, "");

    size = 0;
    ret = tt_memspg_extend(&mspg, &p, &size, 1);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_NOT_EQUAL(p, NULL, "");
    TT_TEST_CHECK_EQUAL(size, 5, "");

    ret = tt_memspg_extend(&mspg, &p, &size, 6);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_NOT_EQUAL(p, NULL, "");
    TT_TEST_CHECK_EQUAL(size, 10, "");

    ret = tt_memspg_extend(&mspg, &p, &size, 11);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_NOT_EQUAL(p, NULL, "");
    TT_TEST_CHECK_EQUAL(size, 20, "");

    ret = tt_memspg_extend(&mspg, &p, &size, 21);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_NOT_EQUAL(p, NULL, "");
    TT_TEST_CHECK_EQUAL(size, 32, "");

    ret = tt_memspg_extend(&mspg, &p, &size, 55);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_NOT_EQUAL(p, NULL, "");
    TT_TEST_CHECK_EQUAL(size, 56, "");

    ret = tt_memspg_extend(&mspg, &p, &size, 80);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_NOT_EQUAL(p, NULL, "");
    TT_TEST_CHECK_EQUAL(size, 80, "");

    ret = tt_memspg_compress(&mspg, &p, &size, size - 5);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_NOT_EQUAL(p, NULL, "");
    TT_TEST_CHECK_EQUAL(size, 75, "");

    ret = tt_memspg_compress(&mspg, &p, &size, size - 20);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_NOT_EQUAL(p, NULL, "");
    TT_TEST_CHECK_EQUAL(size, 55, "");

    ret = tt_memspg_compress(&mspg, &p, &size, size - 40);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_NOT_EQUAL(p, NULL, "");
    TT_TEST_CHECK_EQUAL(size, 15, "");

    ret = tt_memspg_compress(&mspg, &p, &size, size - 15);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_EQUAL(p, NULL, "");
    TT_TEST_CHECK_EQUAL(size, 0, "");

    // test end
    TT_TEST_CASE_LEAVE()
}
