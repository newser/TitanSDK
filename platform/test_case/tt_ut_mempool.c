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

#include "tt_unit_test_case_config.h"
#include <unit_test/tt_unit_test.h>

#include <algorithm/tt_stack.h>
#include <log/tt_log.h>
#include <memory/tt_memory_alloc.h>
#include <memory/tt_memory_pool.h>
#include <memory/tt_memory_spring.h>
#include <memory/tt_page.h>
#include <os/tt_mutex.h>
#include <os/tt_thread.h>
#include <time/tt_time_reference.h>

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
TT_TEST_ROUTINE_DECLARE(case_mempool_basic)
TT_TEST_ROUTINE_DECLARE(case_mempool_alloc_free)
TT_TEST_ROUTINE_DECLARE(case_mempool_sanity)

TT_TEST_ROUTINE_DECLARE(case_ptr_stack)
TT_TEST_ROUTINE_DECLARE(case_mem_spg)
TT_TEST_ROUTINE_DECLARE(case_mem_page)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(mempool_case)

TT_TEST_CASE("case_mempool_basic",
             "testing mempool basic operation",
             case_mempool_basic,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("case_mempool_alloc_free",
                 "testing mempool alloc free APIs",
                 case_mempool_alloc_free,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_mempool_sanity",
                 "testing mempool sanity test",
                 case_mempool_sanity,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_mem_spg",
                 "testing memory spring",
                 case_mem_spg,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_mem_page",
                 "testing memory page",
                 case_mem_page,
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
    TT_TEST_ROUTINE_DEFINE(case_mem_page)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(case_mempool_basic)
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
    pool_attr.max_pool_size = 0;

    // create a pool
    tt_mempool_init(&pool, 8000, &pool_attr);

    p1 = tt_mp_alloc(&pool, 0);
    TT_UT_EQUAL(p1, NULL, "");
    p1 = tt_mp_alloc(&pool, 100);
    TT_UT_NOT_EQUAL(p1, NULL, "");
    p2 = tt_mp_alloc(&pool, 8000);
    TT_UT_NOT_EQUAL(p2, NULL, "");
    p3 = tt_mp_alloc(&pool, 8001);
    TT_UT_EQUAL(p3, NULL, "");

    // destroy
    tt_mempool_destroy(&pool);

    //// test mempool create/destroy 2

    // all default
    tt_mempool_init(&pool, 2000, NULL);

    p1 = tt_mp_alloc(&pool, 100);
    TT_UT_NOT_EQUAL(p1, NULL, "");
    p2 = tt_mp_alloc(&pool, 2000);
    TT_UT_NOT_EQUAL(p2, NULL, "");
    p3 = tt_mp_alloc(&pool, 500);
    TT_UT_NOT_EQUAL(p3, NULL, "");

    tt_mempool_destroy(&pool);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_mempool_alloc_free)
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
    pool_attr.max_pool_size = 18000;
    tt_mempool_init(&pool, 9000, &pool_attr);
    TT_UT_EQUAL(pool.max_frame_num, 2, "");

    //// start alloc free test

    // exceeding limitation
    p1 = tt_mp_alloc(&pool, 8999);
    TT_UT_NOT_EQUAL(p1, NULL, "");
    p2 = tt_mp_alloc(&pool, 9001);
    TT_UT_EQUAL(p2, NULL, "");

    // about 128b
    p1 = tt_mp_alloc(&pool, 100);
    TT_UT_NOT_EQUAL(p1, NULL, "");
    // about 256b
    p2 = tt_mp_alloc(&pool, 200);
    TT_UT_NOT_EQUAL(p2, NULL, "");
    // all left
    p3 = tt_mp_alloc(&pool, 7500);
    TT_UT_NOT_EQUAL(p3, NULL, "");

    // allocate in 256b
    // about 128b
    p2 = tt_mp_alloc(&pool, 100);
    TT_UT_NOT_EQUAL(p2, NULL, "");
    p4 = tt_mp_alloc(&pool, 100);
    TT_UT_NOT_EQUAL(p4, NULL, "");
    // now should about 1000bytes left
    p4 = tt_mp_alloc(&pool, 900);
    TT_UT_NOT_EQUAL(p4, NULL, "");
    p4 = tt_mp_alloc(&pool, 101);
    TT_UT_EQUAL(p4, NULL, "");

    tt_mempool_destroy(&pool);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_mempool_sanity)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_mempool_t pool;
    tt_mempool_attr_t pool_attr;
    void *p;

    tt_result_t result = TT_FAIL;

    TT_TEST_CASE_ENTER()
    // test start

    //// test mempool create/destroy 1

    tt_mempool_attr_default(&pool_attr);
    pool_attr.max_pool_size = tt_rand_u32() % (1 << 17);

    // create a pool
    tt_mempool_init(&pool, 8000, &pool_attr);

    do {
        tt_u32_t size = tt_rand_u32() % (1 << 8) + 1;
        p = tt_mp_alloc(&pool, size);
        if (p != NULL) {
            tt_memset(p, 0xcf, size);
        }
    } while (p != NULL);

    // destroy
    tt_mempool_destroy(&pool);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_mem_spg)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_memspg_t mspg;
    tt_result_t ret;
    tt_u32_t size, i;
    tt_u8_t *p, val[4] = {0, 1, 2, 3};

    TT_TEST_CASE_ENTER()
    // test start

    tt_memspg_init(&mspg, 5, 12, 1 << 30);

    size = tt_memspg_next_size(&mspg, 0);
    TT_UT_EQUAL(size, 5, "");

    size = tt_memspg_next_size(&mspg, 5);
    TT_UT_EQUAL(size, 10, "");

    size = tt_memspg_next_size(&mspg, 6);
    TT_UT_EQUAL(size, 12, "");

    size = tt_memspg_next_size(&mspg, 11);
    TT_UT_EQUAL(size, 22, "");

    size = tt_memspg_next_size(&mspg, 12);
    TT_UT_EQUAL(size, 24, "");

    size = tt_memspg_next_size(&mspg, 13);
    TT_UT_EQUAL(size, 25, "");

    size = tt_memspg_next_size(&mspg, (1 << 30));
    TT_UT_EQUAL(size, 0, "");

    size = tt_memspg_next_size(&mspg, ~0);
    TT_UT_EQUAL(size, 0, "");

    size = tt_memspg_next_size(&mspg, ~0 - 1);
    TT_UT_EQUAL(size, 0, "");

    // no max limit
    tt_memspg_init(&mspg, 5, 12, 0);

    size = tt_memspg_next_size(&mspg, ~0 - 12);
    TT_UT_EQUAL(size, ~0, "");

    size = tt_memspg_next_size(&mspg, ~0 - 11);
    TT_UT_EQUAL(size, 0, "");

    // expand
    p = NULL;
    size = ~0 - 1;
    ret = tt_memspg_extend(&mspg, &p, &size, ~0);
    TT_UT_FAIL(ret, "");

    size = 0;
    ret = tt_memspg_extend(&mspg, &p, &size, 1);
    TT_UT_SUCCESS(ret, "");
    TT_UT_NOT_EQUAL(p, NULL, "");
    TT_UT_EQUAL(size, 5, "");

    ret = tt_memspg_extend(&mspg, &p, &size, 6);
    TT_UT_SUCCESS(ret, "");
    TT_UT_NOT_EQUAL(p, NULL, "");
    TT_UT_EQUAL(size, 10, "");

    ret = tt_memspg_extend(&mspg, &p, &size, 11);
    TT_UT_SUCCESS(ret, "");
    TT_UT_NOT_EQUAL(p, NULL, "");
    TT_UT_EQUAL(size, 20, "");

    ret = tt_memspg_extend(&mspg, &p, &size, 21);
    TT_UT_SUCCESS(ret, "");
    TT_UT_NOT_EQUAL(p, NULL, "");
    TT_UT_EQUAL(size, 32, "");

    ret = tt_memspg_extend(&mspg, &p, &size, 55);
    TT_UT_SUCCESS(ret, "");
    TT_UT_NOT_EQUAL(p, NULL, "");
    TT_UT_EQUAL(size, 56, "");

    ret = tt_memspg_extend(&mspg, &p, &size, 80);
    TT_UT_SUCCESS(ret, "");
    TT_UT_NOT_EQUAL(p, NULL, "");
    TT_UT_EQUAL(size, 80, "");

    for (i = 0; i < 80; ++i) {
        p[i] = i;
    }
    ret = tt_memspg_compress_range(&mspg, &p, &size, 3, 78);
    TT_UT_SUCCESS(ret, "");
    TT_UT_NOT_EQUAL(p, NULL, "");
    TT_UT_EQUAL(size, 75, "");
    TT_UT_EQUAL(p[0], 3, "");
    TT_UT_EQUAL(p[1], 4, "");
    TT_UT_EQUAL(p[20], 23, "");
    TT_UT_EQUAL(p[50], 53, "");
    TT_UT_EQUAL(p[74], 77, "");

    ret = tt_memspg_compress(&mspg, &p, &size, size - 20);
    TT_UT_SUCCESS(ret, "");
    TT_UT_NOT_EQUAL(p, NULL, "");
    TT_UT_EQUAL(size, 55, "");

    ret = tt_memspg_compress(&mspg, &p, &size, size - 40);
    TT_UT_SUCCESS(ret, "");
    TT_UT_NOT_EQUAL(p, NULL, "");
    TT_UT_EQUAL(size, 15, "");

    ret = tt_memspg_compress(&mspg, &p, &size, size - 15);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(p, NULL, "");
    TT_UT_EQUAL(size, 0, "");

    // set to zero
    p = val;
    size = sizeof(val);
    ret = tt_memspg_extend_ex(&mspg,
                              &p,
                              &size,
                              20,
                              TT_MSPGEXT_NOFREE | TT_MSPGEXT_ZERO);
    TT_UT_SUCCESS(ret, "");
    for (i = 0; i < sizeof(val); ++i) {
        TT_UT_EQUAL(p[i], val[i], "");
    }
    for (; i < size; ++i) {
        TT_UT_EQUAL(p[i], 0, "");
    }

    ret = tt_memspg_extend_ex(&mspg, &p, &size, 30, TT_MSPGEXT_ZERO);
    TT_UT_SUCCESS(ret, "");
    for (i = 0; i < sizeof(val); ++i) {
        TT_UT_EQUAL(p[i], val[i], "");
    }
    for (; i < size; ++i) {
        TT_UT_EQUAL(p[i], 0, "");
    }

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_mem_page)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_u8_t *p, *cmp_p;
    tt_uintptr_t h;

    TT_TEST_CASE_ENTER()
    // test start

    p = tt_page_alloc(0);
    TT_UT_NOT_EQUAL(p, NULL, "");
    tt_page_free(p, 0);

    p = tt_page_alloc(1);
    TT_UT_NOT_EQUAL(p, NULL, "");
    tt_page_free(p, 1);

    p = tt_page_alloc(12345);
    TT_UT_NOT_EQUAL(p, NULL, "");
    tt_page_free(p, 12345);

    // page align
    p = tt_page_alloc_align(0, &h);
    TT_UT_NOT_EQUAL(p, NULL, "");
    cmp_p = p;
    TT_PTR_ALIGN_INC(cmp_p, tt_g_page_size_order);
    TT_UT_EQUAL(p, cmp_p, "");
    tt_page_free_align(p, 0, h);

    p = tt_page_alloc_align(1, &h);
    TT_UT_NOT_EQUAL(p, NULL, "");
    cmp_p = p;
    TT_PTR_ALIGN_INC(cmp_p, tt_g_page_size_order);
    TT_UT_EQUAL(p, cmp_p, "");
    tt_page_free_align(p, 1, h);

    p = tt_page_alloc_align(tt_g_page_size_order + 3, &h);
    TT_UT_NOT_EQUAL(p, NULL, "");
    cmp_p = p;
    TT_PTR_ALIGN_INC(cmp_p, tt_g_page_size_order + 3);
    TT_UT_EQUAL(p, cmp_p, "");
    tt_page_free_align(p, 0, h);

    // malloc align
    p = tt_malloc_align(1, 0);
    TT_UT_NOT_EQUAL(p, NULL, "");
    tt_free_align(p);

    p = tt_malloc_align(1, 4);
    TT_UT_NOT_EQUAL(p, NULL, "");
    cmp_p = p;
    TT_PTR_ALIGN_INC(cmp_p, 4);
    TT_UT_EQUAL(p, cmp_p, "");
    tt_free_align(p);

    p = tt_malloc_align(100, 8);
    TT_UT_NOT_EQUAL(p, NULL, "");
    cmp_p = p;
    TT_PTR_ALIGN_INC(cmp_p, 8);
    TT_UT_EQUAL(p, cmp_p, "");
    TT_PTR_ALIGNED(p, 8);
    tt_free_align(p);

    p = tt_xmalloc_align(200, 12);
    TT_UT_NOT_EQUAL(p, NULL, "");
    cmp_p = p;
    TT_PTR_ALIGN_INC(cmp_p, 12);
    TT_UT_EQUAL(p, cmp_p, "");
    TT_PTR_ALIGNED(p, 12);
    tt_free_align(p);

    // test end
    TT_TEST_CASE_LEAVE()
}
