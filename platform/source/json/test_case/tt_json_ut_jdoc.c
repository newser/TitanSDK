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

#include <tt_platform.h>

#include <json/tt_json_document.h>
#include <json/tt_json_value.h>

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

#if TT_ENV_OS_IS_IOS

#if (TT_ENV_OS_FEATURE & TT_ENV_OS_FEATURE_IOS_SIMULATOR)
#define __UT_JF_PATH "/tmp/tt_test_json"
#else
static tt_string_t xf_path;
#define __UT_JF_PATH tt_string_cstr(&xf_path)
#endif

#elif TT_ENV_OS_IS_ANDROID
#define __UT_JF_PATH "/data/data/com.titansdk.titansdkunittest/tt_test_json"
#else
#define __UT_JF_PATH "tt_test_json"
#endif

static void __json_enter(void *enter_param)
{
#if TT_ENV_OS_IS_IOS && !(TT_ENV_OS_FEATURE & TT_ENV_OS_FEATURE_IOS_SIMULATOR)
    static tt_bool_t done = TT_FALSE;
    tt_char_t *s;

    if (done) { return; }

    tt_string_init(&xf_path, NULL);

    s = getenv("HOME");
    if (s != NULL) {
        tt_string_append(&xf_path, s);
        tt_string_append(&xf_path, "/Library/Caches/tt_test_json");

        done = TT_TRUE;
    }
#endif
}

// === routine declarations ================
TT_TEST_ROUTINE_DECLARE(case_jdoc_basic)
TT_TEST_ROUTINE_DECLARE(case_jval_basic)
TT_TEST_ROUTINE_DECLARE(case_jval_array)
TT_TEST_ROUTINE_DECLARE(case_jval_object)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(json_jdoc_case)

TT_TEST_CASE("case_jdoc_basic", "json: document basic", case_jdoc_basic, NULL,
             __json_enter, NULL, NULL, NULL)
,

    TT_TEST_CASE("case_jval_basic", "json: value basic", case_jval_basic, NULL,
                 NULL, NULL, NULL, NULL),

    TT_TEST_CASE("case_jval_array", "json: array basic", case_jval_array, NULL,
                 NULL, NULL, NULL, NULL),

    TT_TEST_CASE("case_jval_object", "json: object basic", case_jval_object,
                 NULL, NULL, NULL, NULL, NULL),

    TT_TEST_CASE_LIST_DEFINE_END(json_jdoc_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(JSON_UT_JDOC, 0, json_jdoc_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(case_jval_object)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(case_jdoc_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_jdoc_t jd;
    tt_char_t text[] = "{\"hello\": \"world\"}";
    tt_jdoc_parse_attr_t pa;

    TT_TEST_CASE_ENTER()
    // test start

    TT_UT_SUCCESS(tt_jdoc_create(&jd), "");
    tt_jdoc_destroy(&jd);

    tt_jdoc_parse_attr_default(&pa);

    // utf8
    {
        TT_UT_SUCCESS(tt_jdoc_create(&jd), "");
        TT_UT_SUCCESS(tt_jdoc_parse(&jd, text, sizeof(text) - 1, NULL), "");
        tt_jdoc_destroy(&jd);
    }

    {
        tt_char_t test[] = "{123";
        TT_UT_SUCCESS(tt_jdoc_create(&jd), "");
        TT_UT_FAIL(tt_jdoc_parse(&jd, test, sizeof(test), NULL), "");
        tt_jdoc_destroy(&jd);
    }

    {
        tt_char_t buf[100];
        tt_u32_t n = sizeof(buf);
        tt_chsetconv_t c;

        TT_UT_SUCCESS(tt_chsetconv_create(&c, TT_CHARSET_UTF8,
                                          TT_CHARSET_UTF16LE, NULL),
                      "");
        TT_UT_SUCCESS(tt_chsetconv_input(&c, text, sizeof(text)), "");
        tt_chsetconv_output(&c, buf, &n);

        TT_UT_SUCCESS(tt_jdoc_create(&jd), "");
        pa.encoding = TT_JDOC_UTF16_LE;
        TT_UT_SUCCESS(tt_jdoc_parse(&jd, buf, n, &pa), "");
        tt_jdoc_destroy(&jd);

        tt_chsetconv_destroy(&c);
    }

    {
        tt_char_t buf[100];
        tt_u32_t n = sizeof(buf);
        tt_chsetconv_t c;

        TT_UT_SUCCESS(tt_chsetconv_create(&c, TT_CHARSET_UTF8,
                                          TT_CHARSET_UTF16BE, NULL),
                      "");
        TT_UT_SUCCESS(tt_chsetconv_input(&c, text, sizeof(text)), "");
        tt_chsetconv_output(&c, buf, &n);
        TT_INFO("n: %d", n);

        TT_UT_SUCCESS(tt_jdoc_create(&jd), "");
        pa.encoding = TT_JDOC_UTF16_BE;
        TT_UT_SUCCESS(tt_jdoc_parse(&jd, buf, n, &pa), "");
        tt_jdoc_destroy(&jd);

        tt_chsetconv_destroy(&c);
    }

    {
        tt_char_t buf[100];
        tt_u32_t n = sizeof(buf);
        tt_chsetconv_t c;

        TT_UT_SUCCESS(tt_chsetconv_create(&c, TT_CHARSET_UTF8,
                                          TT_CHARSET_UTF32LE, NULL),
                      "");
        TT_UT_SUCCESS(tt_chsetconv_input(&c, text, sizeof(text)), "");
        tt_chsetconv_output(&c, buf, &n);

        TT_UT_SUCCESS(tt_jdoc_create(&jd), "");
        pa.encoding = TT_JDOC_UTF32_LE;
        TT_UT_SUCCESS(tt_jdoc_parse(&jd, buf, n, &pa), "");
        tt_jdoc_destroy(&jd);

        tt_chsetconv_destroy(&c);
    }

    {
        tt_char_t buf[100];
        tt_u32_t n = sizeof(buf);
        tt_chsetconv_t c;

        TT_UT_SUCCESS(tt_chsetconv_create(&c, TT_CHARSET_UTF8,
                                          TT_CHARSET_UTF32BE, NULL),
                      "");
        TT_UT_SUCCESS(tt_chsetconv_input(&c, text, sizeof(text)), "");
        tt_chsetconv_output(&c, buf, &n);

        TT_UT_SUCCESS(tt_jdoc_create(&jd), "");
        pa.encoding = TT_JDOC_UTF32_BE;
        TT_UT_SUCCESS(tt_jdoc_parse(&jd, buf, n, &pa), "");
        tt_jdoc_destroy(&jd);

        tt_chsetconv_destroy(&c);
    }

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_jval_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_jval_t jv, jv2;
    tt_jdoc_t jd;

    TT_TEST_CASE_ENTER()
    // test start

    tt_jval_init(&jv);
    tt_jval_destroy(&jv);

    tt_jdoc_create(&jd);

    // null
    {
        tt_jval_init(&jv);
        TT_UT_TRUE(tt_jval_is_null(&jv), "");
        TT_UT_EQUAL(tt_jval_get_type(&jv), TT_JVAL_NULL, "");
        TT_UT_FALSE(tt_jval_is_bool(&jv), "");
        TT_UT_FALSE(tt_jval_is_number(&jv), "");
        TT_UT_FALSE(tt_jval_is_u32(&jv), "");
        TT_UT_FALSE(tt_jval_is_s32(&jv), "");
        TT_UT_FALSE(tt_jval_is_u64(&jv), "");
        TT_UT_FALSE(tt_jval_is_s64(&jv), "");
        TT_UT_FALSE(tt_jval_is_double(&jv), "");
        TT_UT_FALSE(tt_jval_is_str(&jv), "");
        TT_UT_FALSE(tt_jval_is_array(&jv), "");

        tt_jval_set_bool(&jv, TT_FALSE);
        TT_UT_FALSE(tt_jval_is_null(&jv), "");
        tt_jval_set_null(&jv);
        TT_UT_TRUE(tt_jval_is_null(&jv), "");
        tt_jval_destroy(&jv);
    }

    // bool
    {
        tt_jval_init_bool(&jv, TT_TRUE);
        TT_UT_EQUAL(tt_jval_get_type(&jv), TT_JVAL_BOOL, "");
        TT_UT_TRUE(tt_jval_is_bool(&jv), "");
        TT_UT_EQUAL(tt_jval_get_bool(&jv), TT_TRUE, "");

        tt_jval_set_bool(&jv, TT_FALSE);
        TT_UT_EQUAL(tt_jval_get_type(&jv), TT_JVAL_BOOL, "");
        TT_UT_TRUE(tt_jval_is_bool(&jv), "");
        TT_UT_EQUAL(tt_jval_get_bool(&jv), TT_FALSE, "");

        tt_jval_set_bool(&jv, TT_TRUE);
        TT_UT_TRUE(tt_jval_is_bool(&jv), "");
        TT_UT_EQUAL(tt_jval_get_bool(&jv), TT_TRUE, "");

        tt_jval_destroy(&jv);
    }

    // 32bit
    {
        tt_jval_init_u32(&jv, 0xffffffff);
        TT_UT_EQUAL(tt_jval_get_type(&jv), TT_JVAL_NUMBER, "");
        TT_UT_TRUE(tt_jval_is_u32(&jv), "");
        TT_UT_EQUAL(tt_jval_get_u32(&jv), 0xffffffff, "");

        tt_jval_set_bool(&jv, TT_FALSE);
        TT_UT_TRUE(tt_jval_is_bool(&jv), "");
        TT_UT_EQUAL(tt_jval_get_bool(&jv), TT_FALSE, "");

        tt_jval_set_u32(&jv, 0xffffffff);
        TT_UT_TRUE(tt_jval_is_u32(&jv), "");
        TT_UT_EQUAL(tt_jval_get_u32(&jv), 0xffffffff, "");

        tt_jval_destroy(&jv);
    }

    // s32bit
    {
        tt_jval_init_s32(&jv, 0xffffffff);
        TT_UT_EQUAL(tt_jval_get_type(&jv), TT_JVAL_NUMBER, "");
        TT_UT_TRUE(tt_jval_is_s32(&jv), "");
        TT_UT_EQUAL(tt_jval_get_s32(&jv), 0xffffffff, "");

        tt_jval_set_bool(&jv, TT_FALSE);
        TT_UT_TRUE(tt_jval_is_bool(&jv), "");
        TT_UT_EQUAL(tt_jval_get_bool(&jv), TT_FALSE, "");

        tt_jval_set_s32(&jv, 0xffffffff);
        TT_UT_TRUE(tt_jval_is_s32(&jv), "");
        TT_UT_EQUAL(tt_jval_get_s32(&jv), -1, "");

        tt_jval_destroy(&jv);
    }

    // u64
    {
        tt_jval_init_u64(&jv, ~0);
        TT_UT_EQUAL(tt_jval_get_type(&jv), TT_JVAL_NUMBER, "");
        TT_UT_TRUE(tt_jval_is_u64(&jv), "");
        TT_UT_EQUAL(tt_jval_get_u64(&jv), 0xffffffffffffffff, "");

        tt_jval_set_bool(&jv, TT_FALSE);
        TT_UT_TRUE(tt_jval_is_bool(&jv), "");
        TT_UT_EQUAL(tt_jval_get_bool(&jv), TT_FALSE, "");

        tt_jval_set_u64(&jv, 0xffffffff);
        TT_UT_TRUE(tt_jval_is_u64(&jv), "");
        TT_UT_EQUAL(tt_jval_get_u64(&jv), 0xffffffff, "");

        tt_jval_destroy(&jv);
    }

    // s64
    {
        tt_jval_init_s64(&jv, ~0);
        TT_UT_EQUAL(tt_jval_get_type(&jv), TT_JVAL_NUMBER, "");
        TT_UT_TRUE(tt_jval_is_s64(&jv), "");
        TT_UT_EQUAL(tt_jval_get_s64(&jv), -1, "");

        tt_jval_set_bool(&jv, TT_FALSE);
        TT_UT_TRUE(tt_jval_is_bool(&jv), "");
        TT_UT_EQUAL(tt_jval_get_bool(&jv), TT_FALSE, "");

        tt_jval_set_s64(&jv, 0xffffffff);
        TT_UT_TRUE(tt_jval_is_s64(&jv), "");
        TT_UT_EQUAL(tt_jval_get_s64(&jv), 0xffffffff, "");

        tt_jval_destroy(&jv);
    }

    // double
    {
        tt_jval_init_double(&jv, 1.1);
        TT_UT_EQUAL(tt_jval_get_type(&jv), TT_JVAL_NUMBER, "");
        TT_UT_TRUE(tt_jval_is_double(&jv), "");
        TT_UT_EQUAL(tt_jval_get_double(&jv), 1.1, "");

        tt_jval_set_bool(&jv, TT_FALSE);
        TT_UT_TRUE(tt_jval_is_bool(&jv), "");
        TT_UT_EQUAL(tt_jval_get_bool(&jv), TT_FALSE, "");

        tt_jval_set_double(&jv, 2.2);
        TT_UT_TRUE(tt_jval_is_double(&jv), "");
        TT_UT_EQUAL(tt_jval_get_double(&jv), 2.2, "");

        {
            tt_jval_t v2;
            tt_jval_init_str(&v2, "1111", &jd);
            tt_jval_swap(&jv, &v2);

            TT_UT_TRUE(tt_jval_is_str(&jv), "");
            TT_UT_STREQ(tt_jval_get_str(&jv), "1111", "");
            TT_UT_TRUE(tt_jval_is_double(&v2), "");
            TT_UT_EQUAL(tt_jval_get_double(&v2), 2.2, "");
        }

        tt_jval_destroy(&jv);
    }

    // string
    {
        tt_jval_init_str(&jv, "123", &jd);
        TT_UT_EQUAL(tt_jval_get_type(&jv), TT_JVAL_STRING, "");
        TT_UT_TRUE(tt_jval_is_str(&jv), "");
        TT_UT_STREQ(tt_jval_get_str(&jv), "123", "");

        tt_jval_set_bool(&jv, TT_FALSE);
        TT_UT_TRUE(tt_jval_is_bool(&jv), "");
        TT_UT_EQUAL(tt_jval_get_bool(&jv), TT_FALSE, "");

        tt_jval_set_strn(&jv, "123", 3);
        TT_UT_TRUE(tt_jval_is_str(&jv), "");
        TT_UT_STREQ(tt_jval_get_str(&jv), "123", "");

        tt_jval_copy_strn(&jv, "123", 3, &jd);
        TT_UT_TRUE(tt_jval_is_str(&jv), "");
        TT_UT_STREQ(tt_jval_get_str(&jv), "123", "");

        tt_jval_init_strn(&jv2, "456", 3, &jd);
        TT_UT_TRUE(tt_jval_is_str(&jv2), "");
        TT_UT_STREQ(tt_jval_get_str(&jv2), "456", "");

        TT_UT_FALSE(tt_jval_cmp(&jv, &jv2), "");
        TT_UT_TRUE(tt_jval_cmp(&jv, &jv), "");

        tt_jval_set_strn(&jv2, "123", 3);
        TT_UT_TRUE(tt_jval_cmp(&jv, &jv2), "");

        tt_jval_copy_strn(&jv2, "123", 3, &jd);
        TT_UT_TRUE(tt_jval_cmp(&jv, &jv2), "");

        tt_jval_destroy(&jv);
        tt_jval_destroy(&jv2);
    }

    {
        tt_char_t t[10] = "abc";

        tt_jval_create_str(&jv, t, &jd);
        TT_UT_EQUAL(tt_jval_get_type(&jv), TT_JVAL_STRING, "");
        TT_UT_TRUE(tt_jval_is_str(&jv), "");
        t[0] = 'A';
        TT_UT_STREQ(tt_jval_get_str(&jv), "abc", "");
    }

    tt_jdoc_destroy(&jd);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_jval_array)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_jval_t jv;
    tt_jdoc_t jd;

    TT_TEST_CASE_ENTER()
    // test start

    TT_UT_SUCCESS(tt_jdoc_create(&jd), "");

    tt_jval_init_array(&jv, &jd);
    TT_UT_EQUAL(tt_jval_get_type(&jv), TT_JVAL_ARRAY, "");
    tt_jval_is_array(&jv);
    TT_UT_TRUE(tt_jarray_empty(&jv), "");
    TT_UT_EQUAL(tt_jarray_count(&jv), 0, "");
    tt_jarray_reserve(&jv, 10, &jd);
    TT_UT_TRUE(tt_jarray_empty(&jv), "");
    TT_UT_EQUAL(tt_jarray_count(&jv), 0, "");
    TT_UT_EQUAL(tt_jarray_capacity(&jv), 10, "");
    tt_jval_destroy(&jv);

    {
        tt_jval_t v1, v2, v3, *pv;
        tt_jarray_iter_t iter;

        tt_jval_init_bool(&jv, TT_TRUE);
        tt_jval_set_array(&jv);
        TT_UT_TRUE(tt_jval_is_array(&jv), "");

        tt_jval_init_bool(&v1, TT_TRUE);
        tt_jval_init_s32(&v2, -2);
        tt_jval_init_strn(&v3, "12345", 5, &jd);

        tt_jarray_iter(&jv, &iter);
        pv = tt_jarray_iter_next(&iter);
        TT_UT_NULL(pv, "");

        tt_jarray_push(&jv, &v1, &jd);
        tt_jarray_push(&jv, &v2, &jd);
        tt_jarray_push(&jv, &v3, &jd);
        TT_UT_FALSE(tt_jarray_empty(&jv), "");
        TT_UT_EQUAL(tt_jarray_count(&jv), 3, "");

        tt_jarray_reserve(&jv, 10, &jd);
        TT_UT_EXP(tt_jarray_capacity(&jv) >= 13, "");

        pv = tt_jarray_get(&jv, 0);
        TT_UT_EQUAL(tt_jval_get_bool(pv), TT_TRUE, "");
        pv = tt_jarray_get(&jv, 1);
        TT_UT_EQUAL(tt_jval_get_s32(pv), -2, "");
        pv = tt_jarray_get(&jv, 2);
        TT_UT_STREQ(tt_jval_get_str(pv), "12345", "");

        tt_jarray_iter(&jv, &iter);
        pv = tt_jarray_iter_next(&iter);
        TT_UT_EQUAL(tt_jval_get_bool(pv), TT_TRUE, "");
        pv = tt_jarray_iter_next(&iter);
        TT_UT_EQUAL(tt_jval_get_s32(pv), -2, "");
        pv = tt_jarray_iter_next(&iter);
        TT_UT_STREQ(tt_jval_get_str(pv), "12345", "");
        pv = tt_jarray_iter_next(&iter);
        TT_UT_NULL(pv, "");

        tt_jarray_pop(&jv);
        TT_UT_EQUAL(tt_jarray_count(&jv), 2, "");
    }

    {
        tt_jval_t *pv;

        tt_jarray_clear(&jv);
        TT_UT_EQUAL(tt_jarray_count(&jv), 0, "");

        tt_jarray_push_null(&jv, &jd);
        tt_jarray_push_bool(&jv, TT_TRUE, &jd);
        tt_jarray_push_u32(&jv, 0xffffffff, &jd);
        tt_jarray_push_s32(&jv, -3, &jd);
        tt_jarray_push_u64(&jv, ~0, &jd);
        tt_jarray_push_s64(&jv, -4, &jd);
        tt_jarray_push_double(&jv, 3.14, &jd);
        tt_jarray_push_str(&jv, "123", TT_TRUE, &jd);
        tt_jarray_push_str(&jv, "456", TT_FALSE, &jd);
        TT_UT_EQUAL(tt_jarray_count(&jv), 9, "");

        pv = tt_jarray_get(&jv, 0);
        TT_UT_TRUE(tt_jval_is_null(pv), "");
        pv = tt_jarray_get(&jv, 1);
        TT_UT_TRUE(tt_jval_get_bool(pv), "");
        pv = tt_jarray_get(&jv, 2);
        TT_UT_EQUAL(tt_jval_get_u32(pv), 0xffffffff, "");
        pv = tt_jarray_get(&jv, 3);
        TT_UT_EQUAL(tt_jval_get_s32(pv), -3, "");
        pv = tt_jarray_get(&jv, 4);
        TT_UT_EQUAL(tt_jval_get_u64(pv), ~0, "");
        pv = tt_jarray_get(&jv, 5);
        TT_UT_EQUAL(tt_jval_get_s64(pv), -4, "");
        pv = tt_jarray_get(&jv, 6);
        TT_UT_EQUAL(tt_jval_get_double(pv), 3.14, "");
        pv = tt_jarray_get(&jv, 7);
        TT_UT_STREQ(tt_jval_get_str(pv), "123", "");
        pv = tt_jarray_get(&jv, 8);
        TT_UT_STREQ(tt_jval_get_str(pv), "456", "");
    }

    tt_jval_init_array(&jv, &jd);
    tt_jval_is_array(&jv);
    tt_jval_destroy(&jv);

    tt_jdoc_destroy(&jd);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_jval_object)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_jval_t jv, *pv;
    tt_jdoc_t jd;
    tt_jobj_iter_t i;

    TT_TEST_CASE_ENTER()
    // test start

    TT_UT_SUCCESS(tt_jdoc_create(&jd), "");

    tt_jval_init_obj(&jv, &jd);
    TT_UT_EQUAL(tt_jval_get_type(&jv), TT_JVAL_OBJECT, "");
    TT_UT_TRUE(tt_jval_is_obj(&jv), "");
    TT_UT_NULL(tt_jobj_find(&jv, "1"), "");
    TT_UT_FALSE(tt_jobj_contain(&jv, "member"), "");
    TT_UT_TRUE(tt_jobj_empty(&jv), "");
    TT_UT_EQUAL(tt_jobj_member_count(&jv), 0, "");
    tt_jobj_clear(&jv);
    TT_UT_TRUE(tt_jval_is_obj(&jv), "");
    {
        tt_jobj_iter(&jv, &i);
        TT_UT_NULL(tt_jobj_iter_next(&i), "");
    }

    tt_jval_destroy(&jv);

    {
        tt_jval_t name, val;
        tt_jval_init_str(&name, "name", &jd);
        tt_jval_init_str(&val, "val", &jd);
        tt_jobj_add_nv(&jv, &name, &val, &jd);
        TT_UT_FALSE(tt_jobj_empty(&jv), "");
        TT_UT_EQUAL(tt_jobj_member_count(&jv), 1, "");

        tt_jobj_reserve(&jv, 3, &jd);
        TT_UT_EXP(tt_jobj_capacity(&jv) >= 4, "");

        pv = tt_jobj_find(&jv, "nam");
        TT_UT_NULL(pv, "");
        pv = tt_jobj_find(&jv, "name");
        TT_UT_NOT_NULL(pv, "");
        TT_UT_STREQ(tt_jval_get_str(pv), "val", "");

        TT_UT_TRUE(tt_jobj_contain(&jv, "name"), "");

        tt_jval_set_s32(pv, 666);
        pv = tt_jobj_find(&jv, "name");
        TT_UT_NOT_NULL(pv, "");
        TT_UT_EQUAL(tt_jval_get_s32(pv), 666, "");

        tt_jobj_remove(&jv, "nam");
        TT_UT_TRUE(tt_jobj_contain(&jv, "name"), "");
        tt_jobj_remove(&jv, "name");
        TT_UT_FALSE(tt_jobj_contain(&jv, "name"), "");
    }

    {
        tt_jobj_clear(&jv);
        TT_UT_TRUE(tt_jobj_empty(&jv), "");

        pv = tt_jobj_find(&jv, "nam");
        TT_UT_NULL(pv, "");

        tt_jobj_add_null(&jv, "null", sizeof("null") - 1, TT_TRUE, &jd);
        tt_jobj_add_bool(&jv, "bool", sizeof("bool") - 1, TT_FALSE, TT_FALSE,
                         &jd);
        tt_jobj_add_u32(&jv, "u32", sizeof("u32") - 1, TT_TRUE, 0xfffffffe,
                        &jd);
        tt_jobj_add_s32(&jv, "s32", sizeof("s32") - 1, TT_FALSE, -1, &jd);
        tt_jobj_add_u64(&jv, "u64", sizeof("u64") - 1, TT_TRUE,
                        0xfffffffefffffffe, &jd);
        tt_jobj_add_s64(&jv, "s64", sizeof("s64") - 1, TT_FALSE,
                        0xfffffffeffffffff, &jd);
        tt_jobj_add_double(&jv, "double", sizeof("double") - 1, TT_TRUE, 1.23,
                           &jd);
        tt_jobj_add_strn(&jv, "str", sizeof("str") - 1, TT_FALSE, "str val",
                         sizeof("str val") - 1, TT_TRUE, &jd);
        TT_UT_EQUAL(tt_jobj_member_count(&jv), 8, "");

        pv = tt_jobj_find(&jv, "null");
        TT_UT_NOT_NULL(pv, "");
        TT_UT_EQUAL(tt_jval_is_null(pv), TT_TRUE, "");

        pv = tt_jobj_find(&jv, "bool");
        TT_UT_NOT_NULL(pv, "");
        TT_UT_EQUAL(tt_jval_get_bool(pv), TT_FALSE, "");

        pv = tt_jobj_find(&jv, "u32");
        TT_UT_NOT_NULL(pv, "");
        TT_UT_EQUAL(tt_jval_get_u32(pv), 0xfffffffe, "");

        pv = tt_jobj_find(&jv, "s32");
        TT_UT_NOT_NULL(pv, "");
        TT_UT_EQUAL(tt_jval_get_s32(pv), -1, "");

        pv = tt_jobj_find(&jv, "u64");
        TT_UT_NOT_NULL(pv, "");
        TT_UT_EQUAL(tt_jval_get_u64(pv), 0xfffffffefffffffe, "");

        pv = tt_jobj_find(&jv, "s64");
        TT_UT_NOT_NULL(pv, "");
        TT_UT_EQUAL(tt_jval_get_s64(pv), 0xfffffffeffffffff, "");

        pv = tt_jobj_find(&jv, "double");
        TT_UT_NOT_NULL(pv, "");
        TT_UT_EQUAL(tt_jval_get_double(pv), 1.23, "");

        pv = tt_jobj_find(&jv, "str");
        TT_UT_NOT_NULL(pv, "");
        TT_UT_STREQ(tt_jval_get_str(pv), "str val", "");

        {
            tt_u32_t n = 0;

            tt_jobj_iter(&jv, &i);
            while ((pv = tt_jobj_iter_next(&i)) != NULL) {
                if (tt_jval_is_null(pv)) {
                } else if (tt_jval_is_bool(pv)) {
                    TT_UT_EQUAL(tt_jval_get_bool(pv), TT_FALSE, "");
                } else if (tt_jval_is_u32(pv)) {
                    TT_UT_EQUAL(tt_jval_get_u32(pv), 0xfffffffe, "");
                } else if (tt_jval_is_s32(pv)) {
                    TT_UT_EQUAL(tt_jval_get_s32(pv), -1, "");
                } else if (tt_jval_is_u64(pv)) {
                    TT_UT_EQUAL(tt_jval_get_u64(pv), 0xfffffffefffffffe, "");
                } else if (tt_jval_is_s64(pv)) {
                    TT_UT_EQUAL(tt_jval_get_s64(pv), 0xfffffffeffffffff, "");
                } else if (tt_jval_is_str(pv)) {
                    TT_UT_STREQ(tt_jval_get_str(pv), "str val", "");
                } else if (tt_jval_is_double(pv)) {
                    TT_UT_EQUAL(tt_jval_get_double(pv), 1.23, "");
                }
                ++n;
            }
            TT_UT_EQUAL(n, 8, "");
        }

        {
            tt_jdoc_render_attr_t ra;
            tt_buf_t b;
            tt_jval_t *pv;

            tt_jval_swap(tt_jdoc_get_root(&jd), &jv);

            tt_jdoc_render_attr_default(&ra);

            tt_buf_init(&b, NULL);

            tt_buf_clear(&b);
            ra.encoding = TT_JDOC_UTF8;
            TT_UT_SUCCESS(tt_jdoc_render(&jd, &b, &ra), "");
            // tt_buf_print_cstr(&b, 0);
            {
                tt_jdoc_t pjd;
                tt_jdoc_parse_attr_t dpa;

                tt_jdoc_create(&pjd);
                tt_jdoc_parse_attr_default(&dpa);
                dpa.encoding = TT_JDOC_UTF8;
                TT_UT_SUCCESS(tt_jdoc_parse(&pjd, TT_BUF_RPOS(&b),
                                            TT_BUF_RLEN(&b), &dpa),
                              "");

                pv = tt_jobj_find(tt_jdoc_get_root(&pjd), "str");
                TT_UT_NOT_NULL(pv, "");
                TT_UT_STREQ(tt_jval_get_str(pv), "str val", "");

                tt_jdoc_destroy(&pjd);
            }

            tt_buf_clear(&b);
            ra.encoding = TT_JDOC_UTF16_LE;
            TT_UT_SUCCESS(tt_jdoc_render(&jd, &b, &ra), "");
            {
                tt_jdoc_t pjd;
                tt_jdoc_parse_attr_t dpa;

                tt_jdoc_create(&pjd);
                tt_jdoc_parse_attr_default(&dpa);
                dpa.encoding = TT_JDOC_UTF16_LE;
                TT_UT_SUCCESS(tt_jdoc_parse(&pjd, TT_BUF_RPOS(&b),
                                            TT_BUF_RLEN(&b), &dpa),
                              "");

                pv = tt_jobj_find(tt_jdoc_get_root(&pjd), "str");
                TT_UT_NOT_NULL(pv, "");
                TT_UT_STREQ(tt_jval_get_str(pv), "str val", "");

                tt_jdoc_destroy(&pjd);
            }

            tt_buf_clear(&b);
            ra.encoding = TT_JDOC_UTF16_BE;
            TT_UT_SUCCESS(tt_jdoc_render(&jd, &b, &ra), "");
            {
                tt_jdoc_t pjd;
                tt_jdoc_parse_attr_t dpa;

                tt_jdoc_create(&pjd);
                tt_jdoc_parse_attr_default(&dpa);
                dpa.encoding = TT_JDOC_UTF16_BE;
                TT_UT_SUCCESS(tt_jdoc_parse(&pjd, TT_BUF_RPOS(&b),
                                            TT_BUF_RLEN(&b), &dpa),
                              "");

                pv = tt_jobj_find(tt_jdoc_get_root(&pjd), "str");
                TT_UT_NOT_NULL(pv, "");
                TT_UT_STREQ(tt_jval_get_str(pv), "str val", "");

                tt_jdoc_destroy(&pjd);
            }

            tt_buf_clear(&b);
            ra.encoding = TT_JDOC_UTF32_BE;
            TT_UT_SUCCESS(tt_jdoc_render(&jd, &b, &ra), "");
            {
                tt_jdoc_t pjd;
                tt_jdoc_parse_attr_t dpa;

                tt_jdoc_create(&pjd);
                tt_jdoc_parse_attr_default(&dpa);
                dpa.encoding = TT_JDOC_UTF32_BE;
                TT_UT_SUCCESS(tt_jdoc_parse(&pjd, TT_BUF_RPOS(&b),
                                            TT_BUF_RLEN(&b), &dpa),
                              "");

                pv = tt_jobj_find(tt_jdoc_get_root(&pjd), "str");
                TT_UT_NOT_NULL(pv, "");
                TT_UT_STREQ(tt_jval_get_str(pv), "str val", "");

                tt_jdoc_destroy(&pjd);
            }

            tt_buf_clear(&b);
            ra.encoding = TT_JDOC_UTF32_LE;
            TT_UT_SUCCESS(tt_jdoc_render_file(&jd, __UT_JF_PATH, &ra), "");
            {
                tt_jdoc_t pjd;
                tt_jdoc_parse_attr_t dpa;

                tt_jdoc_create(&pjd);
                tt_jdoc_parse_attr_default(&dpa);
                // invalid encodingr
                dpa.encoding = TT_JDOC_UTF32_BE;
                TT_UT_FAIL(tt_jdoc_parse_file(&pjd, __UT_JF_PATH, &dpa), "");
                tt_jdoc_destroy(&pjd);
            }
            {
                tt_jdoc_t pjd;
                tt_jdoc_parse_attr_t dpa;

                tt_jdoc_create(&pjd);
                tt_jdoc_parse_attr_default(&dpa);
                dpa.encoding = TT_JDOC_UTF32_LE;
                TT_UT_SUCCESS(tt_jdoc_parse_file(&pjd, __UT_JF_PATH, &dpa), "");

                pv = tt_jobj_find(tt_jdoc_get_root(&pjd), "str");
                TT_UT_NOT_NULL(pv, "");
                TT_UT_STREQ(tt_jval_get_str(pv), "str val", "");

                tt_jdoc_destroy(&pjd);
            }

            tt_buf_destroy(&b);
        }
    }

    tt_jdoc_destroy(&jd);

    // test end
    TT_TEST_CASE_LEAVE()
}
