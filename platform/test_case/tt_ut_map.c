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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_hash)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_map)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_map_uniq)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(map_case)

TT_TEST_CASE("tt_unit_test_hash",
             "testing hash algorithms",
             tt_unit_test_hash,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_map",
                 "testing map basic api",
                 tt_unit_test_map,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_map_uniq",
                 "testing unique map api",
                 tt_unit_test_map_uniq,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(map_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(TEST_UNIT_MAP, 0, map_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(tt_unit_test_hash)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

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

void __h1_node2key(IN tt_mnode_t *node,
                   OUT const tt_u8_t **key,
                   OUT tt_u32_t *key_len)
{
    // dummy function
}

#define __TN_NUM 100000
#define __HC_NUM 1000

TT_TEST_ROUTINE_DEFINE(tt_unit_test_hash)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    tt_u32_t i;
    tt_hashctx_t hctx;
    tt_u32_t *num;
    tt_u32_t *hc;
    tt_s64_t start, end;
    tt_u32_t t, min, max;

    TT_TEST_CASE_ENTER()
    // test start

    tt_hashctx_init(&hctx);
    for (i = 0; i < sizeof(__h1_murmur3) / sizeof(__h1_murmur3[0]); ++i) {
        tt_hashcode_t hc;

        hctx.seed = __h1_murmur3[i].seed;
        hc = tt_hash_murmur3((tt_u8_t *)__h1_murmur3[i].key,
                             __h1_murmur3[i].key_len,
                             &hctx);
        TT_TEST_CHECK_EQUAL(hc, __h1_murmur3[i].hash, "");
    }

    // performace: murmur3
    num = tt_malloc(sizeof(tt_u32_t) * __TN_NUM);
    hc = tt_malloc(sizeof(tt_u32_t) * __HC_NUM);

    for (i = 0; i < __TN_NUM; ++i) {
        num[i] = tt_rand_u32();
    }
    for (i = 0; i < __HC_NUM; ++i) {
        hc[i] = 0;
    }

    tt_hashctx_init(&hctx);
    start = tt_time_ref();
    for (i = 0; i < __TN_NUM; ++i) {
        tt_hashcode_t c = tt_hash_murmur3((tt_u8_t *)&num[i], 4, &hctx);
        hc[c % __HC_NUM] += 1;
    }
    end = tt_time_ref();
    t = (tt_u32_t)tt_time_ref2ms(end - start);

    min = ~0;
    max = 0;
    for (i = 0; i < __HC_NUM; ++i) {
        if (hc[i] != 0) {
            min = TT_MIN(min, hc[i]);
            max = TT_MAX(max, hc[i]);
        }
    }
    TT_INFO("hash murmur3, time: %d, min: %d, max: %d", t, min, max);

    for (i = 0; i < __HC_NUM; ++i) {
        hc[i] = 0;
    }

    // performace: fnv1a
    tt_hashctx_init(&hctx);
    start = tt_time_ref();
    for (i = 0; i < __TN_NUM; ++i) {
        tt_hashcode_t c = tt_hash_fnv1a((tt_u8_t *)&num[i], 4, &hctx);
        hc[c % __HC_NUM] += 1;
    }
    end = tt_time_ref();
    t = (tt_u32_t)tt_time_ref2ms(end - start);

    min = ~0;
    max = 0;
    for (i = 0; i < __HC_NUM; ++i) {
        if (hc[i] != 0) {
            min = TT_MIN(min, hc[i]);
            max = TT_MAX(max, hc[i]);
        }
    }
    TT_INFO("hash fnv1a, time: %d, min: %d, max: %d", t, min, max);

    tt_free(num);
    tt_free(hc);

    // test end
    TT_TEST_CASE_LEAVE()
}

#define __h2_num 10000

struct __h2_case_t
{
    tt_mnode_t hnode;

    tt_u8_t *key;
    tt_u32_t key_len;
} __h2_cases[__h2_num];

void __h2_node2key(IN tt_mnode_t *node,
                   OUT const tt_u8_t **key,
                   OUT tt_u32_t *key_len)
{
    struct __h2_case_t *h = TT_CONTAINER(node, struct __h2_case_t, hnode);

    *key = h->key;
    *key_len = h->key_len;
}

void __hm_count(IN struct tt_map_s *hmap, IN tt_mnode_t *hnode, IN void *param)
{
    tt_u32_t *pi = (tt_u32_t *)param;
    *pi += 1;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_map)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_map_t *hmap;
    tt_result_t ret;
    tt_u32_t i, j;
    tt_map_hl_attr_t attr;

    TT_TEST_CASE_ENTER()
    // test start

    tt_map_hashlist_attr_default(&attr);
    // by default, it allows duplicated keys

    // 10000 => 97 slots
    hmap = tt_map_hashlist_create(__h2_node2key, 97, &attr);
    TT_TEST_CHECK_NOT_EQUAL(hmap, NULL, "");
    TT_TEST_CHECK_EQUAL(tt_map_count(hmap), 0, "");
    TT_TEST_CHECK_EQUAL(tt_map_empty(hmap), TT_TRUE, "");

    for (i = 0; i < __h2_num; ++i) {
        if ((i % 101) == 1) {
            tt_u32_t len = __h2_cases[i - 1].key_len;

            __h2_cases[i].key = (tt_u8_t *)tt_malloc(len);
            for (j = 0; j < len; ++j) {
                __h2_cases[i].key[j] = __h2_cases[i - 1].key[j];
            }
            __h2_cases[i].key_len = len;
        } else {
            tt_u32_t len = rand() % 100 + 1;

            __h2_cases[i].key = (tt_u8_t *)tt_malloc(len);
            for (j = 0; j < len; ++j) {
                __h2_cases[i].key[j] = (tt_u8_t)rand();
            }
            __h2_cases[i].key_len = len;
        }


        tt_mnode_init(&__h2_cases[i].hnode);

        // insert
        tt_map_add(hmap, &__h2_cases[i].hnode);
        if (i % 100 == 0) {
            tt_mnode_t *node = NULL;

            TT_TEST_CHECK_EQUAL(tt_map_count(hmap), i + 1, "");
            TT_TEST_CHECK_EQUAL(tt_map_empty(hmap), TT_FALSE, "");
            TT_TEST_CHECK_EQUAL(tt_map_contain_key(hmap,
                                                   __h2_cases[i].key,
                                                   __h2_cases[i].key_len),
                                TT_TRUE,
                                "");

            node = tt_map_find(hmap,
                               __h2_cases[i].key,
                               __h2_cases[i].key_len,
                               node);
            while ((node != NULL) && (node != &__h2_cases[i].hnode)) {
                node = tt_map_find(hmap,
                                   __h2_cases[i].key,
                                   __h2_cases[i].key_len,
                                   node);
            }
            TT_TEST_CHECK_NOT_EQUAL(node, NULL, "");
        }
    }

    // for each
    i = 0;
    tt_map_foreach(hmap, __hm_count, &i);
    TT_TEST_CHECK_EQUAL(i, __h2_num, "");
    TT_TEST_CHECK_EQUAL(tt_map_count(hmap), __h2_num, "");

    for (i = 0; i < __h2_num; ++i) {
        tt_mnode_t *hnode =
            tt_map_find(hmap, __h2_cases[i].key, __h2_cases[i].key_len, NULL);
        while ((hnode != &__h2_cases[i].hnode) && (hnode != NULL)) {
            hnode = tt_map_find(hmap,
                                __h2_cases[i].key,
                                __h2_cases[i].key_len,
                                hnode);
        }

        // must be able to find a node
        TT_TEST_CHECK_NOT_EQUAL(hnode, NULL, "");
    }

    // remove
    for (i = 0; (i < __h2_num) && (i % 10 == 0); ++i) {
        tt_map_remove(hmap, &__h2_cases[i].hnode);
    }
    for (i = 0; (i < __h2_num) && (i % 9 == 0); ++i) {
        tt_map_remove_equal(hmap, __h2_cases[i].key, __h2_cases[i].key_len);
    }

    tt_map_clear(hmap);
    TT_TEST_CHECK_EQUAL(tt_map_count(hmap), 0, "");
    TT_TEST_CHECK_EQUAL(tt_map_empty(hmap), TT_TRUE, "");

    for (i = 0; i < __h2_num; ++i) {
        tt_free(__h2_cases[i].key);
    }

    tt_map_destroy(hmap);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_map_uniq)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_map_t *hmap;
    tt_result_t ret;
    tt_u32_t i, j, base;
    tt_map_hl_attr_t attr;

    TT_TEST_CASE_ENTER()
    // test start

    tt_map_hashlist_attr_default(&attr);
    attr.uniq_key = TT_TRUE;

    // 10000 => 97 slots
    hmap = tt_map_hashlist_create(__h2_node2key, 97, &attr);
    TT_TEST_CHECK_NOT_EQUAL(hmap, NULL, "");
    TT_TEST_CHECK_EQUAL(tt_map_count(hmap), 0, "");
    TT_TEST_CHECK_EQUAL(tt_map_empty(hmap), TT_TRUE, "");

    base = tt_rand_u32() % 100 + 1;
    for (i = 0; i < __h2_num; ++i) {
        tt_u32_t len = base + i;
        // each key has different length

        __h2_cases[i].key = (tt_u8_t *)tt_malloc(len);
        for (j = 0; j < len; ++j) {
            __h2_cases[i].key[j] = (tt_u8_t)rand();
        }

        __h2_cases[i].key_len = len;

        tt_mnode_init(&__h2_cases[i].hnode);

        // insert
        tt_map_add(hmap, &__h2_cases[i].hnode);
        if (i % 100 == 0) {
            tt_mnode_t *node = NULL;

            TT_TEST_CHECK_EQUAL(tt_map_count(hmap), i + 1, "");
            TT_TEST_CHECK_EQUAL(tt_map_empty(hmap), TT_FALSE, "");
            TT_TEST_CHECK_EQUAL(tt_map_contain_key(hmap,
                                                   __h2_cases[i].key,
                                                   __h2_cases[i].key_len),
                                TT_TRUE,
                                "");

            node = tt_map_find(hmap,
                               __h2_cases[i].key,
                               __h2_cases[i].key_len,
                               node);
            while ((node != NULL) && (node != &__h2_cases[i].hnode)) {
                node = tt_map_find(hmap,
                                   __h2_cases[i].key,
                                   __h2_cases[i].key_len,
                                   node);
            }
            TT_TEST_CHECK_NOT_EQUAL(node, NULL, "");

            // adding a duplicate key would fail
            ret = tt_map_add(hmap, &__h2_cases[i].hnode);
            TT_TEST_CHECK_FAIL(ret, "");
        }
    }

    // for each
    i = 0;
    tt_map_foreach(hmap, __hm_count, &i);
    TT_TEST_CHECK_EQUAL(i, __h2_num, "");
    TT_TEST_CHECK_EQUAL(tt_map_count(hmap), __h2_num, "");

    for (i = 0; i < __h2_num; ++i) {
        tt_mnode_t *hnode =
            tt_map_find(hmap, __h2_cases[i].key, __h2_cases[i].key_len, NULL);
        while ((hnode != &__h2_cases[i].hnode) && (hnode != NULL)) {
            hnode = tt_map_find(hmap,
                                __h2_cases[i].key,
                                __h2_cases[i].key_len,
                                hnode);
        }

        // must be able to find a node
        TT_TEST_CHECK_NOT_EQUAL(hnode, NULL, "");
    }

    // remove
    for (i = 0; (i < __h2_num) && (i % 10 == 0); ++i) {
        tt_map_remove(hmap, &__h2_cases[i].hnode);
    }
    for (i = 0; (i < __h2_num) && (i % 9 == 0); ++i) {
        tt_map_remove_equal(hmap, __h2_cases[i].key, __h2_cases[i].key_len);
    }

    tt_map_clear(hmap);
    TT_TEST_CHECK_EQUAL(tt_map_count(hmap), 0, "");
    TT_TEST_CHECK_EQUAL(tt_map_empty(hmap), TT_TRUE, "");

    for (i = 0; i < __h2_num; ++i) {
        tt_free(__h2_cases[i].key);
    }

    tt_map_destroy(hmap);

    // test end
    TT_TEST_CASE_LEAVE()
}
