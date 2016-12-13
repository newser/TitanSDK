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

#include <log/tt_log.h>
#include <memory/tt_slab.h>
#include <misc/tt_util.h>
#include <os/tt_thread.h>

// portlayer header files
#include <tt_cstd_api.h>

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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_slab_cd)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_slab_allocfree)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_slab_mt)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(slab_case)

TT_TEST_CASE("tt_unit_test_slab_cd",
             "testing mempnc slab create and destroy",
             tt_unit_test_slab_cd,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_slab_allocfree",
                 "testing mempnc slab alloc and free",
                 tt_unit_test_slab_allocfree,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_slab_mt",
                 "testing mempnc slab api in multi thread",
                 tt_unit_test_slab_mt,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(slab_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(TEST_UNIT_SLAB, 0, slab_case)

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

    TT_TEST_ROUTINE_DEFINE(tt_unit_test_slab_cd)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_slab_t slab;
    tt_result_t result = TT_FAIL;
    tt_slab_attr_t slab_attr;

    tt_u32_t tmp;


    TT_TEST_CASE_ENTER()
    // test start

    // create a slab
    result = tt_slab_create(&slab, 5, NULL);
    TT_TEST_CHECK_EQUAL(result, TT_SUCCESS, "");
    // destroy a slab
    tt_slab_destroy(&slab, TT_FALSE);

    // valid attributes
    tt_slab_attr_default(&slab_attr);
    result = tt_slab_create(&slab, 5, &slab_attr);
    TT_TEST_CHECK_EQUAL(result, TT_SUCCESS, "");
    tt_slab_destroy(&slab, TT_FALSE);
    // invalid attributes
    slab_attr.objnum_per_expand = 0;
    result = tt_slab_create(&slab, 5, &slab_attr);
    TT_TEST_CHECK_EQUAL(result, TT_FAIL, "");

    // too large size can not be satisfied
    tt_slab_attr_default(&slab_attr);
    slab_attr.objnum_per_expand = 0x4000000;
    result = tt_slab_create(&slab, 8000000, &slab_attr);
    TT_TEST_CHECK_EQUAL(result, TT_FAIL, "");
    slab_attr.objnum_per_expand = 0x40;
    result = tt_slab_create(&slab, 0x80000000, &slab_attr);
    TT_TEST_CHECK_EQUAL(result, TT_FAIL, "");

    // check cache alignment
    tt_slab_attr_default(&slab_attr);
    slab_attr.hwcache_align = TT_TRUE;
    result = tt_slab_create(&slab, 5, &slab_attr);
    TT_TEST_CHECK_EQUAL(result, TT_SUCCESS, "");
    // is it already aligned?
    tmp = slab.obj_size;
    TT_U32_ALIGN_INC_CACHE(tmp);
    TT_TEST_CHECK_EQUAL(tmp, slab.obj_size, "");
    // done
    tt_slab_destroy(&slab, TT_FALSE);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_slab_allocfree)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

    // tt_numa_node_t *numa_node = tt_g_numa_node[0];
    tt_slab_t slab;
    tt_slab_attr_t slab_attr;
    tt_result_t result = TT_FAIL;

    void *obj[1000] = {0};
    tt_u32_t i = 0;
    tt_u32_t objsize = 0;

    TT_TEST_CASE_ENTER()
    // test start

    // create a slab
    objsize = 120;
    tt_slab_attr_default(&slab_attr);
    slab_attr.objnum_per_expand = 1; // then 1 page per expand
    result = tt_slab_create(&slab, objsize, &slab_attr);
    TT_TEST_CHECK_EQUAL(result, TT_SUCCESS, "");

    // trigger some expanding
    for (i = 0; i < sizeof(obj) / sizeof(void *); ++i) {
        obj[i] = tt_slab_alloc(&slab);
        tt_memset(obj[i], 0, objsize);
    }
    // free all
    for (i = 0; i < sizeof(obj) / sizeof(void *); ++i) {
        tt_slab_free(obj[i]);
    }

    // destroy slab
    tt_slab_destroy(&slab, TT_FALSE);

    // with hwcache aligned

    // create a slab
    objsize = 128;
    tt_slab_attr_default(&slab_attr);
    slab_attr.hwcache_align = TT_TRUE;
    result = tt_slab_create(&slab, objsize, &slab_attr);
    TT_TEST_CHECK_EQUAL(result, TT_SUCCESS, "");

    // trigger some expanding
    for (i = 0; i < sizeof(obj) / sizeof(void *); ++i) {
        obj[i] = tt_slab_alloc(&slab);
        tt_memset(obj[i], 0, objsize);
    }
    // free all
    for (i = 0; i < sizeof(obj) / sizeof(void *); ++i) {
        tt_slab_free(obj[i]);
    }

    // destroy slab
    tt_slab_destroy(&slab, TT_FALSE);

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_thread_t *test_threads[10];
static tt_slab_t __slab;
static tt_u32_t __objsize = 0;

static tt_result_t test_routine_1(IN tt_thread_t *thread, IN void *param)
{
    tt_ptrdiff_t idx = (tt_ptrdiff_t)param;
    int i = 0;

    void *obj[1000] = {0};

    // TT_ASSERT(thread == test_threads[idx]);

    for (i = 0; i < sizeof(obj) / sizeof(void *); ++i) {
        obj[i] = tt_slab_alloc(&__slab);

        if (obj[i] != NULL) {
            tt_memset(obj, 0, __objsize);
        }
    }

    for (i = 0; i < sizeof(obj) / sizeof(void *); ++i) {
        if (obj[i] != NULL) {
            tt_slab_free(obj[i]);
        }
    }

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_slab_mt)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

    tt_slab_attr_t slab_attr;
    tt_result_t result = TT_FAIL;

    tt_ptrdiff_t i = 0;

    // tt_numa_node_t *numa_node = tt_g_numa_node[0];

    TT_TEST_CASE_ENTER()
    // test start

    // create a slab
    __objsize = 120;
    tt_slab_attr_default(&slab_attr);
    result = tt_slab_create(&__slab, __objsize, &slab_attr);
    TT_TEST_CHECK_EQUAL(result, TT_SUCCESS, "");

    for (i = 0; i < sizeof(test_threads) / sizeof(tt_thread_t *); ++i) {
        test_threads[i] =
            tt_thread_create(NULL, test_routine_1, (void *)i, NULL);
    }

    for (i = 0; i < sizeof(test_threads) / sizeof(tt_thread_t *); ++i) {
        tt_thread_wait(test_threads[i]);
    }

    // test end
    TT_TEST_CASE_LEAVE()
}
