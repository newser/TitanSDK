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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_map_basic)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_map_share_key)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_ptrmap)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(hmap_case)

TT_TEST_CASE("tt_unit_test_hash",
             "testing hash algorithms",
             tt_unit_test_hash,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_map_basic",
                 "testing map basic",
                 tt_unit_test_map_basic,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_map_share_key",
                 "testing map, sharing key",
                 tt_unit_test_map_share_key,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_ptrmap",
                 "testing ptrmap",
                 tt_unit_test_ptrmap,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(hmap_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(ALG_UT_HASHMAP, 0, hmap_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(tt_unit_test_map_share_key)
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

void __h1_node2key(IN tt_hnode_t *node,
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
        TT_UT_EQUAL(hc, __h1_murmur3[i].hash, "");
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

tt_bool_t __hm_count(IN tt_u8_t *key,
                     IN tt_u32_t key_len,
                     IN tt_hnode_t *mnode,
                     IN void *param)
{
    tt_u32_t *pi = (tt_u32_t *)param;
    *pi += 1;

    return TT_TRUE;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_map_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_hashmap_t hmap;
    tt_result_t ret;
    tt_u32_t i, j, base;
    tt_hmap_attr_t attr;

    TT_TEST_CASE_ENTER()
    // test start

    tt_hmap_attr_default(&attr);

    // 10000 => 97 slots
    ret = tt_hmap_create(&hmap, 97, &attr);
    TT_UT_SUCCESS(ret, "");
    tt_hmap_clear((&hmap));
    TT_UT_EQUAL(tt_hmap_count(&hmap), 0, "");
    TT_UT_EQUAL(tt_hmap_empty(&hmap), TT_TRUE, "");
    TT_UT_EQUAL(tt_hmap_find(&hmap, (tt_u8_t *)&i, 1), NULL, "");

    base = tt_rand_u32() % 100 + 1;
    for (i = 0; i < __h2_num; ++i) {
        tt_u32_t len = base + i;
        // each key has different length

        __h2_cases[i].key = (tt_u8_t *)tt_malloc(len);
        for (j = 0; j < len; ++j) {
            __h2_cases[i].key[j] = (tt_u8_t)rand();
        }

        __h2_cases[i].key_len = len;

        tt_hnode_init(&__h2_cases[i].hnode);
        if (i % 91 == 0) {
            TT_UT_EQUAL(tt_hmap_contain(&hmap, &__h2_cases[i].hnode),
                        TT_FALSE,
                        "");
        }

        // insert
        ret = tt_hmap_add(&hmap,
                          __h2_cases[i].key,
                          __h2_cases[i].key_len,
                          &__h2_cases[i].hnode);
        TT_UT_SUCCESS(ret, "");
        if (i % 100 == 0) {
            tt_hnode_t *node = NULL;
            struct __h2_case_t *hc;

            TT_UT_EQUAL(tt_hmap_count(&hmap), i + 1, "");
            TT_UT_EQUAL(tt_hmap_empty(&hmap), TT_FALSE, "");
            TT_UT_EQUAL(tt_hmap_contain_key(&hmap,
                                            __h2_cases[i].key,
                                            __h2_cases[i].key_len),
                        TT_TRUE,
                        "");
            TT_UT_EQUAL(tt_hmap_contain(&hmap, &__h2_cases[i].hnode),
                        TT_TRUE,
                        "");

            node =
                tt_hmap_find(&hmap, __h2_cases[i].key, __h2_cases[i].key_len);
            TT_UT_NOT_EQUAL(node, NULL, "");
            hc = TT_CONTAINER(node, struct __h2_case_t, hnode);
            TT_UT_EQUAL(hc - &__h2_cases[0], i, "");

            // adding a duplicate key would fail
            ret = tt_hmap_add(&hmap,
                              __h2_cases[i].key,
                              __h2_cases[i].key_len,
                              &__h2_cases[i].hnode);
            TT_UT_FAIL(ret, "");
        }
    }

    // for each
    i = 0;
    tt_hmap_foreach(&hmap, __hm_count, &i);
    TT_UT_EQUAL(i, __h2_num, "");
    TT_UT_EQUAL(tt_hmap_count(&hmap), __h2_num, "");

    for (i = 0; i < __h2_num; ++i) {
        tt_hnode_t *hnode =
            tt_hmap_find(&hmap, __h2_cases[i].key, __h2_cases[i].key_len);

        // must be able to find a node
        TT_UT_NOT_EQUAL(hnode, NULL, "");
    }

    // remove
    for (i = 0; (i < __h2_num) && (i % 10 == 0); ++i) {
        tt_hmap_remove(&hmap, &__h2_cases[i].hnode);
        TT_UT_EQUAL(tt_hmap_contain(&hmap, &__h2_cases[i].hnode), TT_FALSE, "");
    }
    for (i = 0; (i < __h2_num) && (i % 9 == 0); ++i) {
        tt_hmap_remove_key(&hmap, __h2_cases[i].key, __h2_cases[i].key_len);
        TT_UT_EQUAL(tt_hmap_contain_key(&hmap,
                                        __h2_cases[i].key,
                                        __h2_cases[i].key_len),
                    TT_FALSE,
                    "");
    }

    tt_hmap_clear(&hmap);
    TT_UT_EQUAL(tt_hmap_count(&hmap), 0, "");
    TT_UT_EQUAL(tt_hmap_empty(&hmap), TT_TRUE, "");

    for (i = 0; i < __h2_num; ++i) {
        tt_free(__h2_cases[i].key);
    }

    tt_hmap_destroy(&hmap);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_map_share_key)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_hashmap_t hmap;
    tt_result_t ret;
    tt_u32_t i, j;
    tt_hmap_attr_t attr;
#define __knum 1000
    tt_u32_t key[__knum] = {0};

    TT_TEST_CASE_ENTER()
    // test start

    tt_hmap_attr_default(&attr);

    // generate different keys
    for (i = 0; i < __knum; ++i) {
        tt_u32_t k;

    ag:
        k = tt_rand_u32();
        for (j = 0; j < i; ++j) {
            if (key[j] == k) {
                goto ag;
            }
        }
        key[i] = k;
    }

    // 10000 => 97 slots
    ret = tt_hmap_create(&hmap, 197, &attr);
    TT_UT_SUCCESS(ret, "");
    tt_hmap_clear((&hmap));
    TT_UT_EQUAL(tt_hmap_count(&hmap), 0, "");
    TT_UT_EQUAL(tt_hmap_empty(&hmap), TT_TRUE, "");
    TT_UT_EQUAL(tt_hmap_find(&hmap, (tt_u8_t *)&i, 1), NULL, "");

    TT_ASSERT(__knum <= __h2_num);
    for (i = 0; i < __knum; ++i) {
        tt_hnode_init(&__h2_cases[i].hnode);
        if (i % 91 == 0) {
            TT_UT_EQUAL(tt_hmap_contain(&hmap, &__h2_cases[i].hnode),
                        TT_FALSE,
                        "");
        }

        // insert
        ret = tt_hmap_add(&hmap,
                          (tt_u8_t *)&key[i],
                          sizeof(tt_u32_t),
                          &__h2_cases[i].hnode);
        TT_UT_SUCCESS(ret, "");
        if (i % 100 == 0) {
            tt_hnode_t *node = NULL;
            struct __h2_case_t *hc;

            TT_UT_EQUAL(tt_hmap_count(&hmap), i + 1, "");
            TT_UT_EQUAL(tt_hmap_empty(&hmap), TT_FALSE, "");
            TT_UT_EQUAL(tt_hmap_contain_key(&hmap,
                                            (tt_u8_t *)&key[i],
                                            sizeof(tt_u32_t)),
                        TT_TRUE,
                        "");
            TT_UT_EQUAL(tt_hmap_contain(&hmap, &__h2_cases[i].hnode),
                        TT_TRUE,
                        "");

            node = tt_hmap_find(&hmap, (tt_u8_t *)&key[i], sizeof(tt_u32_t));
            TT_UT_NOT_EQUAL(node, NULL, "");
            hc = TT_CONTAINER(node, struct __h2_case_t, hnode);
            TT_UT_EQUAL(hc - &__h2_cases[0], i, "");

            // adding a duplicate key would fail
            ret = tt_hmap_add(&hmap,
                              (tt_u8_t *)&key[i],
                              sizeof(tt_u32_t),
                              &__h2_cases[i].hnode);
            TT_UT_FAIL(ret, "");
        }
    }

    // for each
    i = 0;
    tt_hmap_foreach(&hmap, __hm_count, &i);
    TT_UT_EQUAL(i, __knum, "");
    TT_UT_EQUAL(tt_hmap_count(&hmap), __knum, "");

    for (i = 0; i < __knum; ++i) {
        tt_hnode_t *hnode =
            tt_hmap_find(&hmap, (tt_u8_t *)&key[i], sizeof(tt_u32_t));

        // must be able to find a node
        TT_UT_NOT_EQUAL(hnode, NULL, "");
    }

    // remove
    for (i = 0; (i < __knum) && (i % 10 == 0); ++i) {
        tt_hmap_remove(&hmap, &__h2_cases[i].hnode);
        TT_UT_EQUAL(tt_hmap_contain(&hmap, &__h2_cases[i].hnode), TT_FALSE, "");
    }
    for (i = 0; (i < __knum) && (i % 9 == 0); ++i) {
        tt_hmap_remove_key(&hmap, (tt_u8_t *)&key[i], sizeof(tt_u32_t));
        TT_UT_EQUAL(tt_hmap_contain_key(&hmap,
                                        (tt_u8_t *)&key[i],
                                        sizeof(tt_u32_t)),
                    TT_FALSE,
                    "");
    }

    tt_hmap_clear(&hmap);
    TT_UT_EQUAL(tt_hmap_count(&hmap), 0, "");
    TT_UT_EQUAL(tt_hmap_empty(&hmap), TT_TRUE, "");

    tt_hmap_destroy(&hmap);

    // test end
    TT_TEST_CASE_LEAVE()
}

tt_bool_t __pm_count(IN tt_u8_t *key,
                     IN tt_u32_t key_len,
                     IN tt_ptr_t p,
                     IN void *param)
{
    tt_u32_t *pi = (tt_u32_t *)param;
    *pi += 1;

    return TT_TRUE;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_ptrmap)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_ptrhmap_t hmap, h2;
    tt_result_t ret;
    tt_u32_t i, j, base;
    tt_ptrhmap_attr_t attr;

    TT_TEST_CASE_ENTER()
    // test start

    tt_ptrhmap_attr_default(&attr);

    // 10000 => 97 slots
    ret = tt_ptrhmap_create(&hmap, 97, &attr);
    TT_UT_SUCCESS(ret, "");
    tt_ptrhmap_clear((&hmap));
    TT_UT_EQUAL(tt_ptrhmap_count(&hmap), 0, "");
    TT_UT_EQUAL(tt_ptrhmap_empty(&hmap), TT_TRUE, "");
    TT_UT_EQUAL(tt_ptrhmap_find(&hmap, (tt_u8_t *)&i, 1), NULL, "");

    base = tt_rand_u32() % 100 + 1;
    for (i = 0; i < __h2_num; ++i) {
        tt_u32_t len = base + i;
        // each key has different length

        __h2_cases[i].key = (tt_u8_t *)tt_malloc(len);
        for (j = 0; j < len; ++j) {
            __h2_cases[i].key[j] = (tt_u8_t)rand();
        }

        __h2_cases[i].key_len = len;

        if (i % 91 == 0) {
            TT_UT_EQUAL(tt_ptrhmap_contain_ptr(&hmap, &__h2_cases[i]),
                        TT_FALSE,
                        "");
            TT_UT_EQUAL(tt_ptrhmap_contain_key(&hmap,
                                               __h2_cases[i].key,
                                               __h2_cases[i].key_len),
                        TT_FALSE,
                        "");
        }

        // insert
        ret = tt_ptrhmap_add(&hmap,
                             __h2_cases[i].key,
                             __h2_cases[i].key_len,
                             &__h2_cases[i]);
        TT_UT_SUCCESS(ret, "");
        if (i % 100 == 0) {
            struct __h2_case_t *p = NULL;

            TT_UT_EQUAL(tt_ptrhmap_count(&hmap), i + 1, "");
            TT_UT_EQUAL(tt_ptrhmap_empty(&hmap), TT_FALSE, "");
            TT_UT_EQUAL(tt_ptrhmap_contain_key(&hmap,
                                               __h2_cases[i].key,
                                               __h2_cases[i].key_len),
                        TT_TRUE,
                        "");
            TT_UT_EQUAL(tt_ptrhmap_contain_ptr(&hmap, &__h2_cases[i]),
                        TT_TRUE,
                        "");

            p = (struct __h2_case_t *)tt_ptrhmap_find(&hmap,
                                                      __h2_cases[i].key,
                                                      __h2_cases[i].key_len);
            TT_UT_NOT_EQUAL(p, NULL, "");
            TT_UT_EQUAL(p - &__h2_cases[0], i, "");

            // adding a duplicate key would fail
            ret = tt_ptrhmap_add(&hmap,
                                 __h2_cases[i].key,
                                 __h2_cases[i].key_len,
                                 &__h2_cases[i]);
            TT_UT_FAIL(ret, "");
        }
    }

    // for each
    i = 0;
    tt_ptrhmap_foreach(&hmap, __pm_count, &i);
    TT_UT_EQUAL(i, __h2_num, "");
    TT_UT_EQUAL(tt_ptrhmap_count(&hmap), __h2_num, "");

    for (i = 0; i < __h2_num; ++i) {
        tt_hnode_t *hnode =
            tt_ptrhmap_find(&hmap, __h2_cases[i].key, __h2_cases[i].key_len);

        // must be able to find a node
        TT_UT_NOT_EQUAL(hnode, NULL, "");
    }

    // remove
    for (i = 0; (i < __h2_num) && (i % 10 == 0); ++i) {
        tt_bool_t b = tt_ptrhmap_remove_key(&hmap,
                                            __h2_cases[i].key,
                                            __h2_cases[i].key_len);
        TT_UT_EQUAL(b, TT_TRUE, "");
        TT_UT_EQUAL(tt_ptrhmap_contain_ptr(&hmap, &__h2_cases[i]),
                    TT_FALSE,
                    "");
    }
    for (i = 1; (i < __h2_num) && (i % 9 == 0); ++i) {
        tt_bool_t b = tt_ptrhmap_remove_ptr(&hmap, &__h2_cases[i]);
        TT_UT_EQUAL(b, TT_TRUE, "");
        TT_UT_EQUAL(tt_ptrhmap_contain_key(&hmap,
                                           __h2_cases[i].key,
                                           __h2_cases[i].key_len),
                    TT_FALSE,
                    "");
    }
    for (i = 1; (i < __h2_num) && (i % 9 == 0); ++i) {
        tt_bool_t b = tt_ptrhmap_remove_pair(&hmap,
                                             __h2_cases[i].key,
                                             __h2_cases[i].key_len,
                                             &__h2_cases[i]);
        TT_UT_EQUAL(b, TT_TRUE, "");
        TT_UT_EQUAL(tt_ptrhmap_contain_key(&hmap,
                                           __h2_cases[i].key,
                                           __h2_cases[i].key_len),
                    TT_FALSE,
                    "");
    }

    // replace
    for (i = 1; (i < __h2_num) && (i % 13 == 0); ++i) {
        // &__h2_cases[i] => &ret
        tt_bool_t b = tt_ptrhmap_replace(&hmap,
                                         __h2_cases[i].key,
                                         __h2_cases[i].key_len,
                                         &ret);
        TT_UT_EQUAL(b, TT_TRUE, "");
        TT_UT_EQUAL(tt_ptrhmap_contain_ptr(&hmap, &ret), TT_FALSE, "");
        TT_UT_EQUAL(tt_ptrhmap_find(&hmap,
                                    __h2_cases[i].key,
                                    __h2_cases[i].key_len),
                    &ret,
                    "");
        TT_UT_EQUAL(tt_ptrhmap_contain_ptr(&hmap, &__h2_cases[i]),
                    TT_FALSE,
                    "");

        // &ret => &__h2_cases[i]
        b = tt_ptrhmap_replace_equal(&hmap,
                                     __h2_cases[i].key,
                                     __h2_cases[i].key_len,
                                     &ret,
                                     &__h2_cases[i]);
        TT_UT_EQUAL(b, TT_TRUE, "");
        TT_UT_EQUAL(tt_ptrhmap_find(&hmap,
                                    __h2_cases[i].key,
                                    __h2_cases[i].key_len),
                    &__h2_cases[i],
                    "");
    }

    ret = tt_ptrhmap_create(&h2, 197, NULL);
    TT_UT_SUCCESS(ret, "");

    tt_ptrhmap_clear(&hmap);
    TT_UT_EQUAL(tt_ptrhmap_count(&hmap), 0, "");
    TT_UT_EQUAL(tt_ptrhmap_empty(&hmap), TT_TRUE, "");
    TT_UT_EQUAL(tt_slist_count(&hmap.cache), hmap.cache_count, "");

    for (i = 0; i < __h2_num; ++i) {
        tt_free(__h2_cases[i].key);
    }

    tt_ptrhmap_destroy(&hmap);
    tt_ptrhmap_destroy(&h2);

    // test end
    TT_TEST_CASE_LEAVE()
}
