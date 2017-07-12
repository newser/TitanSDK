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
#include <memory/tt_memory_alloc.h>
#include <memory/tt_slab.h>
#include <misc/tt_util.h>
#include <os/tt_thread.h>
#include <time/tt_time_reference.h>

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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_slab_vs_malloc)
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

#if 0
TT_TEST_CASE("tt_unit_test_slab_vs_malloc",
             "testing mempnc slab vs malloc",
             tt_unit_test_slab_vs_malloc,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL),
#endif

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
    TT_TEST_ROUTINE_DEFINE(tt_unit_test_slab_vs_malloc)
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
    void *p;

    tt_u32_t tmp;


    TT_TEST_CASE_ENTER()
    // test start

    // create a slab
    result = tt_slab_create(&slab, 5, NULL);
    TT_UT_EQUAL(result, TT_SUCCESS, "");
    // destroy a slab
    tt_slab_destroy(&slab);

    // valid attributes
    tt_slab_attr_default(&slab_attr);
    result = tt_slab_create(&slab, 5, &slab_attr);
    TT_UT_EQUAL(result, TT_SUCCESS, "");
    tt_slab_destroy(&slab);

    slab_attr.bulk_num = 0;
    result = tt_slab_create(&slab, 5, &slab_attr);
    p = tt_slab_alloc(&slab);
    TT_UT_NOT_EQUAL(p, NULL, "");
    tt_slab_free(p);
    TT_UT_EQUAL(result, TT_SUCCESS, "");
    tt_slab_destroy(&slab);

    // too large size can not be satisfied
    tt_slab_attr_default(&slab_attr);
    slab_attr.bulk_num = 0x4000000;
    result = tt_slab_create(&slab, 8000000, &slab_attr);
    TT_UT_EQUAL(result, TT_FAIL, "");
    slab_attr.bulk_num = 0x40;
    result = tt_slab_create(&slab, 0x80000000, &slab_attr);
    TT_UT_EQUAL(result, TT_FAIL, "");

    // check cache alignment
    tt_slab_attr_default(&slab_attr);
    slab_attr.cache_align = TT_TRUE;
    result = tt_slab_create(&slab, 5, &slab_attr);
    TT_UT_EQUAL(result, TT_SUCCESS, "");
    // is it already aligned?
    tmp = slab.obj_size;
    TT_U32_ALIGN_INC_CACHE(tmp);
    TT_UT_EQUAL(tmp, slab.obj_size, "");
    // done
    tt_slab_destroy(&slab);

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
    slab_attr.bulk_num = 1; // auto set
    result = tt_slab_create(&slab, objsize, &slab_attr);
    TT_UT_EQUAL(result, TT_SUCCESS, "");

    // trigger some expanding
    for (i = 0; i < sizeof(obj) / sizeof(obj[0]); ++i) {
        obj[i] = tt_slab_alloc(&slab);
        tt_memset(obj[i], 0xcc, objsize);
    }
    // free all
    for (i = 0; i < sizeof(obj) / sizeof(obj[0]); ++i) {
        tt_slab_free(obj[i]);
    }

    // destroy slab
    tt_slab_destroy(&slab);

    // with cache aligned

    // create a slab
    objsize = 128;
    tt_slab_attr_default(&slab_attr);
    slab_attr.cache_align = TT_TRUE;
    result = tt_slab_create(&slab, objsize, &slab_attr);
    TT_UT_EQUAL(result, TT_SUCCESS, "");

    // trigger some expanding
    for (i = 0; i < sizeof(obj) / sizeof(obj[0]); ++i) {
        obj[i] = tt_slab_alloc(&slab);
        tt_memset(obj[i], 0xdd, objsize);
    }
    // free all
    for (i = 0; i < sizeof(obj) / sizeof(obj[0]); ++i) {
        tt_slab_free(obj[i]);
    }

    // destroy slab
    tt_slab_destroy(&slab);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_slab_vs_malloc)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

    // tt_numa_node_t *numa_node = tt_g_numa_node[0];
    tt_slab_t slab;
    tt_slab_attr_t slab_attr;
    tt_result_t result = TT_FAIL;
    tt_s64_t t1, t2, start, end;

    void *obj[1000000] = {0};
    tt_u32_t i = 0;
    tt_u32_t objsize = 0;

    TT_TEST_CASE_ENTER()
    // test start

    // create a slab
    objsize = 30;
    tt_slab_attr_default(&slab_attr);
    slab_attr.bulk_num = 1; // auto set
    result = tt_slab_create(&slab, objsize, &slab_attr);
    TT_UT_EQUAL(result, TT_SUCCESS, "");

    start = tt_time_ref();
    for (i = 0; i < sizeof(obj) / sizeof(obj[0]); ++i) {
        obj[i] = tt_slab_alloc(&slab);
        // tt_memset(obj[i], 0xcc, objsize);
    }
    for (i = 0; i < sizeof(obj) / sizeof(obj[0]); ++i) {
        tt_slab_free(obj[i]);
    }
    end = tt_time_ref();
    t1 = tt_time_ref2ms(end - start);

    // destroy slab
    tt_slab_destroy(&slab);

    start = tt_time_ref();
    for (i = 0; i < sizeof(obj) / sizeof(obj[0]); ++i) {
        obj[i] = tt_malloc(objsize);
        // tt_memset(obj[i], 0xcc, objsize);
    }
    for (i = 0; i < sizeof(obj) / sizeof(obj[0]); ++i) {
        tt_free(obj[i]);
    }
    end = tt_time_ref();
    t2 = tt_time_ref2ms(end - start);

    TT_RECORD_INFO("slab: %dms, malloc: %dms", t1, t2);

    // test end
    TT_TEST_CASE_LEAVE()
}
