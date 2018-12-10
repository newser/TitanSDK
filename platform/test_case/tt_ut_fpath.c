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

#include <algorithm/tt_algorithm_def.h>
#include <io/tt_fpath.h>
#include <log/tt_log.h>
#include <misc/tt_uri.h>
#include <os/tt_atomic.h>
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
TT_TEST_ROUTINE_DECLARE(case_fpath_basic)
TT_TEST_ROUTINE_DECLARE(case_fpath_file)
TT_TEST_ROUTINE_DECLARE(case_fpath_prev)
TT_TEST_ROUTINE_DECLARE(case_fpath_parent)
TT_TEST_ROUTINE_DECLARE(case_fpath_root)
TT_TEST_ROUTINE_DECLARE(case_fpath_move)
TT_TEST_ROUTINE_DECLARE(case_fpath_name)
TT_TEST_ROUTINE_DECLARE(case_fpath_normalize)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(fpath_case)

TT_TEST_CASE("case_fpath_basic",
             "testing fpath basic",
             case_fpath_basic,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("case_fpath_file",
                 "testing fpath file",
                 case_fpath_file,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_fpath_parent",
                 "testing fpath goto parent",
                 case_fpath_parent,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_fpath_root",
                 "testing fpath goto root",
                 case_fpath_root,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_fpath_move",
                 "testing fpath move",
                 case_fpath_move,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_fpath_name",
                 "testing fpath get/set name",
                 case_fpath_name,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_fpath_normalize",
                 "testing fpath normalize",
                 case_fpath_normalize,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(fpath_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(TEST_UNIT_FPATH, 0, fpath_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(case_fpath_normalize)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(case_fpath_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_fpath_t fp, fp2;
    tt_result_t ret;

    TT_TEST_CASE_ENTER()
    // test start

    tt_fpath_init(&fp, TT_FPATH_SEP_UNIX);
    tt_fpath_init(&fp2, TT_FPATH_SEP_UNIX);
    TT_UT_EQUAL(tt_fpath_empty(&fp), TT_TRUE, "");
    TT_UT_EQUAL(tt_fpath_render(&fp)[0], 0, "");
    ret = tt_fpath_copy(&fp2, &fp);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_fpath_render(&fp2)[0], 0, "");
    tt_fpath_destroy(&fp);

    ret = tt_fpath_create_cstr(&fp, "", TT_FPATH_SEP_UNIX);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_fpath_empty(&fp), TT_TRUE, "");
    TT_UT_EQUAL(tt_fpath_render(&fp)[0], 0, "");
    TT_UT_EQUAL(tt_fpath_startwith(&fp, ""), TT_TRUE, "");
    TT_UT_EQUAL(tt_fpath_startwith(&fp, "a"), TT_FALSE, "");
    TT_UT_EQUAL(tt_fpath_endwith(&fp, ""), TT_TRUE, "");
    TT_UT_EQUAL(tt_fpath_endwith(&fp, "/"), TT_FALSE, "");
    ret = tt_fpath_copy(&fp2, &fp);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_fpath_render(&fp2)[0], 0, "");
    tt_fpath_destroy(&fp);

    ret = tt_fpath_create_cstr(&fp, "a/b/c/", TT_FPATH_SEP_UNIX);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_fpath_empty(&fp), TT_FALSE, "");
    TT_UT_EQUAL(tt_strcmp(tt_fpath_render(&fp), "a/b/c/"), 0, "");
    TT_UT_EQUAL(tt_fpath_cmp(&fp, "a/b/c/"), 0, "");
    TT_UT_EQUAL(tt_fpath_startwith(&fp, ""), TT_TRUE, "");
    TT_UT_EQUAL(tt_fpath_startwith(&fp, "a"), TT_TRUE, "");
    TT_UT_EQUAL(tt_fpath_startwith(&fp, "a/"), TT_TRUE, "");
    TT_UT_EQUAL(tt_fpath_startwith(&fp, "a/c"), TT_FALSE, "");
    TT_UT_EQUAL(tt_fpath_endwith(&fp, ""), TT_TRUE, "");
    TT_UT_EQUAL(tt_fpath_endwith(&fp, "/"), TT_TRUE, "");
    TT_UT_EQUAL(tt_fpath_endwith(&fp, "c/"), TT_TRUE, "");
    TT_UT_EQUAL(tt_fpath_endwith(&fp, "cc/"), TT_FALSE, "");
    ret = tt_fpath_copy(&fp2, &fp);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_fpath_cmp(&fp2, "a/b/c/"), 0, "");
    tt_fpath_destroy(&fp);

    // windows
    ret = tt_fpath_create_cstr(&fp, "c:\\a\\b\\c\\", TT_FPATH_SEP_WINDOWS);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_fpath_empty(&fp), TT_FALSE, "");
    TT_UT_EQUAL(tt_strcmp(tt_fpath_render(&fp), "c:\\a\\b\\c\\"), 0, "");
    TT_UT_EQUAL(tt_strcmp(tt_fpath_get_root(&fp), "c:\\"), 0, "");

    ret = tt_fpath_parse(&fp, "c:\\");
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_fpath_empty(&fp), TT_TRUE, "");
    TT_UT_EQUAL(tt_strcmp(tt_fpath_render(&fp), "c:\\"), 0, "");
    TT_UT_EQUAL(tt_strcmp(tt_fpath_get_root(&fp), "c:\\"), 0, "");

    tt_fpath_destroy(&fp);
    tt_fpath_destroy(&fp2);

    {
        tt_fpath_init(&fp, TT_FPATH_SEP_UNIX);

        TT_UT_SUCCESS(tt_fpath_parse(&fp, "c:/"), "");
        TT_UT_EQUAL(tt_fpath_count(&fp), 0, "");
        TT_UT_STREQ(tt_fpath_get_root(&fp), "c:/", "");

        TT_UT_SUCCESS(tt_fpath_parse(&fp, "c:"), "");
        TT_UT_EQUAL(tt_fpath_count(&fp), 0, "");
        TT_UT_STREQ(tt_fpath_get_root(&fp), "c:/", "");

        TT_UT_SUCCESS(tt_fpath_parse(&fp, "c:abc"), "");
        TT_UT_EQUAL(tt_fpath_count(&fp), 1, "");
        TT_UT_STREQ(tt_fpath_get_root(&fp), "c:/", "");
        TT_UT_STREQ(tt_fpath_get_name(&fp, 0), "abc", "");

        TT_UT_SUCCESS(tt_fpath_parse(&fp, "/a/b/c"), "");
        TT_UT_EQUAL(tt_fpath_count(&fp), 3, "");
        TT_UT_STREQ(tt_fpath_get_root(&fp), "/", "");
        TT_UT_STREQ(tt_fpath_get_name(&fp, 0), "a", "");
        TT_UT_STREQ(tt_fpath_get_name(&fp, 2), "c", "");

        // do not gurantee ok, but should not crash
        tt_fpath_parse(&fp, "c:\\abc");
        tt_fpath_destroy(&fp);
    }

    {
        TT_UT_SUCCESS(tt_fpath_create(&fp, "a/b/c.exe", 8, TT_FPATH_SEP_UNIX),
                      "");
        TT_UT_STREQ(tt_fpath_render(&fp), "a/b/c.ex", "");
        TT_UT_STREQ(tt_fpath_get_filename(&fp), "c.ex", "");

        TT_UT_SUCCESS(tt_fpath_parse_n(&fp, "a/b/c.exe", 8), "");
        TT_UT_STREQ(tt_fpath_render(&fp), "a/b/c.ex", "");
        TT_UT_STREQ(tt_fpath_get_filename(&fp), "c.ex", "");

        TT_UT_SUCCESS(tt_fpath_parse_n(&fp, "a/b/c.exe", 5), "");
        TT_UT_STREQ(tt_fpath_render(&fp), "a/b/c", "");
        TT_UT_STREQ(tt_fpath_get_filename(&fp), "c", "");
        TT_UT_STREQ(tt_fpath_get_basename(&fp), "c", "");
        TT_UT_STREQ(tt_fpath_get_extension(&fp), "", "");

        TT_UT_SUCCESS(tt_fpath_parse_n(&fp, "a/b/c.exe", 4), "");
        TT_UT_STREQ(tt_fpath_render(&fp), "a/b/", "");
        TT_UT_EQUAL(tt_fpath_count(&fp), 2, "");
        TT_UT_STREQ(tt_fpath_get_filename(&fp), "", "");

        TT_UT_SUCCESS(tt_fpath_parse_n(&fp, "a/b.xy/c.exe", 5), "");
        TT_UT_STREQ(tt_fpath_render(&fp), "a/b.x", "");
        TT_UT_STREQ(tt_fpath_get_filename(&fp), "b.x", "");
        TT_UT_STREQ(tt_fpath_get_basename(&fp), "b", "");
        TT_UT_STREQ(tt_fpath_get_extension(&fp), "x", "");

        TT_UT_SUCCESS(tt_fpath_parse_n(&fp, "/aa.zzz/b.xy/c.exe", 6), "");
        TT_UT_STREQ(tt_fpath_render(&fp), "/aa.zz", "");
        TT_UT_STREQ(tt_fpath_get_filename(&fp), "aa.zz", "");
        TT_UT_STREQ(tt_fpath_get_basename(&fp), "aa", "");
        TT_UT_STREQ(tt_fpath_get_extension(&fp), "zz", "");

        tt_fpath_destroy(&fp);
    }

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_fpath_file)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_fpath_t fp, fp2;
    tt_result_t ret;

    TT_TEST_CASE_ENTER()
    // test start

    tt_fpath_init(&fp, TT_FPATH_SEP_UNIX);
    tt_fpath_init(&fp2, TT_FPATH_SEP_UNIX);

    // empty
    TT_UT_EQUAL(tt_fpath_is_file(&fp), TT_FALSE, "");
    TT_UT_EQUAL(tt_fpath_is_dir(&fp), TT_TRUE, "");
    TT_UT_EQUAL(tt_strlen(tt_fpath_get_filename(&fp)), 0, "");

    TT_UT_EQUAL(tt_fpath_get_basename(&fp)[0], 0, "");
    TT_UT_EQUAL(tt_fpath_get_extension(&fp)[0], 0, "");

    ret = tt_fpath_set_basename(&fp, "base");
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_strcmp(tt_fpath_render(&fp), "base"), 0, "");
    ret = tt_fpath_set_extension(&fp, "ext");
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_strcmp(tt_fpath_render(&fp), "base.ext"), 0, "");

    // absolute file
    ret = tt_fpath_parse(&fp, "/a/b1/c22/cc.ext1.ext2");
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_fpath_is_file(&fp), TT_TRUE, "");
    TT_UT_EQUAL(tt_fpath_is_dir(&fp), TT_FALSE, "");
    TT_UT_EQUAL(tt_strcmp(tt_fpath_get_filename(&fp), "cc.ext1.ext2"), 0, "");

    TT_UT_EQUAL(tt_strcmp(tt_fpath_get_basename(&fp), "cc.ext1"), 0, "");
    TT_UT_EQUAL(tt_strcmp(tt_fpath_get_extension(&fp), "ext2"), 0, "");

    ret = tt_fpath_set_basename(&fp, "base");
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_strcmp(tt_fpath_render(&fp), "/a/b1/c22/base.ext2"), 0, "");
    ret = tt_fpath_set_extension(&fp, "3");
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_strcmp(tt_fpath_render(&fp), "/a/b1/c22/base.3"), 0, "");

    // relative file
    ret = tt_fpath_parse(&fp, "b1/c22/ccname");
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_fpath_is_file(&fp), TT_TRUE, "");
    TT_UT_EQUAL(tt_fpath_is_dir(&fp), TT_FALSE, "");
    TT_UT_EQUAL(tt_strcmp(tt_fpath_get_filename(&fp), "ccname"), 0, "");

    TT_UT_EQUAL(tt_strcmp(tt_fpath_get_basename(&fp), "ccname"), 0, "");
    TT_UT_EQUAL(tt_fpath_get_extension(&fp)[0], 0, "");

    ret = tt_fpath_set_basename(&fp, "base.a.b.c");
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_strcmp(tt_fpath_render(&fp), "b1/c22/base.a.b.c"), 0, "");
    ret = tt_fpath_set_extension(&fp, "ext3");
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_strcmp(tt_fpath_render(&fp), "b1/c22/base.a.b.c.ext3"),
                0,
                "");

    // single file
    ret = tt_fpath_parse(&fp, "a.b1.c22.d333");
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_fpath_is_file(&fp), TT_TRUE, "");
    TT_UT_EQUAL(tt_strcmp(tt_fpath_get_filename(&fp), "a.b1.c22.d333"), 0, "");

    TT_UT_EQUAL(tt_strcmp(tt_fpath_get_basename(&fp), "a.b1.c22"), 0, "");
    TT_UT_EQUAL(tt_strcmp(tt_fpath_get_extension(&fp), "d333"), 0, "");

    ret = tt_fpath_set_basename(&fp, "e.f1.g22");
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_strcmp(tt_fpath_render(&fp), "e.f1.g22.d333"), 0, "");
    ret = tt_fpath_set_extension(&fp, "h4444");
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_strcmp(tt_fpath_render(&fp), "e.f1.g22.h4444"), 0, "");

    // single file, no ext
    ret = tt_fpath_parse(&fp, "abcdef");
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_fpath_is_file(&fp), TT_TRUE, "");
    TT_UT_EQUAL(tt_strcmp(tt_fpath_get_filename(&fp), "abcdef"), 0, "");

    TT_UT_EQUAL(tt_strcmp(tt_fpath_get_basename(&fp), "abcdef"), 0, "");
    TT_UT_EQUAL(tt_fpath_get_extension(&fp)[0], 0, "");

    ret = tt_fpath_set_basename(&fp, "efg");
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_strcmp(tt_fpath_render(&fp), "efg"), 0, "");
    ret = tt_fpath_set_extension(&fp, "h4444");
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_strcmp(tt_fpath_render(&fp), "efg.h4444"), 0, "");

    {
        tt_fpath_parse(&fp, "/a/b/../c/.");
        TT_UT_EQUAL(tt_fpath_is_file(&fp), TT_FALSE, "");
        TT_UT_EQUAL(tt_fpath_is_dir(&fp), TT_TRUE, "");
        TT_UT_EQUAL(tt_strlen(tt_fpath_get_filename(&fp)), 0, "");

        TT_UT_EQUAL(tt_fpath_get_basename(&fp)[0], 0, "");
        TT_UT_EQUAL(tt_fpath_get_extension(&fp)[0], 0, "");

        ret = tt_fpath_set_extension(&fp, "h4444");
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_strcmp(tt_fpath_render(&fp), "/a/b/../c/./.h4444"),
                    0,
                    "");

        ret = tt_fpath_set_basename(&fp, "efg");
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_strcmp(tt_fpath_render(&fp), "/a/b/../c/./efg.h4444"),
                    0,
                    "");
    }

    {
        tt_fpath_parse(&fp, "..");
        TT_UT_EQUAL(tt_fpath_is_file(&fp), TT_FALSE, "");
        TT_UT_EQUAL(tt_fpath_is_dir(&fp), TT_TRUE, "");
        TT_UT_EQUAL(tt_strlen(tt_fpath_get_filename(&fp)), 0, "");

        TT_UT_EQUAL(tt_fpath_get_basename(&fp)[0], 0, "");
        TT_UT_EQUAL(tt_fpath_get_extension(&fp)[0], 0, "");

        ret = tt_fpath_set_basename(&fp, "efg");
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_strcmp(tt_fpath_render(&fp), "../efg"), 0, "");
        ret = tt_fpath_set_extension(&fp, "h4444");
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_strcmp(tt_fpath_render(&fp), "../efg.h4444"), 0, "");
    }

    {
        // relative dir
        ret = tt_fpath_parse(&fp, "b1/c22/ccname/");
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_fpath_is_file(&fp), TT_FALSE, "");
        TT_UT_EQUAL(tt_fpath_is_dir(&fp), TT_TRUE, "");

        TT_UT_EQUAL(tt_fpath_get_basename(&fp)[0], 0, "");
        TT_UT_EQUAL(tt_fpath_get_extension(&fp)[0], 0, "");

        ret = tt_fpath_set_basename(&fp, "base.a.b.c");
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_strcmp(tt_fpath_render(&fp), "b1/c22/ccname/base.a.b.c"),
                    0,
                    "");
        ret = tt_fpath_set_extension(&fp, "ext3");
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_strcmp(tt_fpath_render(&fp),
                              "b1/c22/ccname/base.a.b.c.ext3"),
                    0,
                    "");
    }

    {
        // absolute dir
        ret = tt_fpath_parse(&fp, "/a/b1/c22/../ccname/.");
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_fpath_is_file(&fp), TT_FALSE, "");
        TT_UT_EQUAL(tt_fpath_is_dir(&fp), TT_TRUE, "");

        TT_UT_EQUAL(tt_fpath_get_basename(&fp)[0], 0, "");
        TT_UT_EQUAL(tt_fpath_get_extension(&fp)[0], 0, "");

        ret = tt_fpath_set_basename(&fp, "base.a.b.c");
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_strcmp(tt_fpath_render(&fp),
                              "/a/b1/c22/../ccname/./base.a.b.c"),
                    0,
                    "");
        ret = tt_fpath_set_extension(&fp, "ext3");
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_strcmp(tt_fpath_render(&fp),
                              "/a/b1/c22/../ccname/./base.a.b.c.ext3"),
                    0,
                    "");
    }

    tt_fpath_destroy(&fp);
    tt_fpath_destroy(&fp2);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_fpath_move)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_fpath_t fp, fp2;
    tt_result_t ret;

    TT_TEST_CASE_ENTER()
    // test start

    tt_fpath_init(&fp, TT_FPATH_SEP_UNIX);
    tt_fpath_init(&fp2, TT_FPATH_SEP_UNIX);

    // empty
    {
        ret = tt_fpath_get_sibling(&fp, "sib1", &fp2);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_fpath_cmp(&fp2, "sib1"), 0, "");
        tt_fpath_clear(&fp2);
        ret = tt_fpath_get_child(&fp, "child1", &fp2);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_fpath_cmp(&fp2, "./child1"), 0, "");

        ret = tt_fpath_get_absolute(&fp, &fp2);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_fpath_is_absolute(&fp2), TT_TRUE, "");
        TT_UT_EQUAL(tt_fpath_is_relative(&fp2), TT_FALSE, "");
        TT_INFO("abs path: %s", tt_fpath_render(&fp2));

        ret = tt_fpath_to_sibling(&fp, "sib1");
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_fpath_cmp(&fp, "sib1"), 0, "");
        tt_fpath_clear(&fp);
        ret = tt_fpath_to_child(&fp, "child1");
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_fpath_cmp(&fp, "./child1"), 0, "");

        tt_fpath_clear(&fp);
        ret = tt_fpath_to_absolute(&fp);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_fpath_is_absolute(&fp), TT_TRUE, "");
        TT_UT_EQUAL(tt_fpath_is_relative(&fp), TT_FALSE, "");
        TT_INFO("abs path: %s", tt_fpath_render(&fp2));
    }

    // single file
    {
        tt_fpath_parse(&fp, "123");

        ret = tt_fpath_get_sibling(&fp, "sib1/sib2/sib3", &fp2);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_fpath_cmp(&fp2, "sib1/sib2/sib3"), 0, "");
        tt_fpath_clear(&fp2);
        ret = tt_fpath_get_child(&fp, "child1/child2/", &fp2);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_fpath_cmp(&fp2, "123/child1/child2/"), 0, "");

        ret = tt_fpath_get_absolute(&fp, &fp2);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_fpath_is_absolute(&fp2), TT_TRUE, "");
        TT_UT_EQUAL(tt_fpath_is_relative(&fp2), TT_FALSE, "");
        TT_INFO("abs path: %s", tt_fpath_render(&fp2));

        ret = tt_fpath_to_sibling(&fp, "sib1");
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_fpath_cmp(&fp, "sib1"), 0, "");
        tt_fpath_clear(&fp);
        tt_fpath_parse(&fp, "123");
        ret = tt_fpath_to_child(&fp, "child1");
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_fpath_cmp(&fp, "123/child1"), 0, "");

        tt_fpath_clear(&fp);
        ret = tt_fpath_to_absolute(&fp);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_fpath_is_absolute(&fp), TT_TRUE, "");
        TT_UT_EQUAL(tt_fpath_is_relative(&fp), TT_FALSE, "");
        TT_INFO("abs path: %s", tt_fpath_render(&fp2));
    }

    // absolute directory
    {
        tt_fpath_parse(&fp, "/a/b/c/d/e/");

        ret = tt_fpath_get_sibling(&fp, "sib1", &fp2);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_fpath_cmp(&fp2, "/a/b/c/d/sib1"), 0, "");
        tt_fpath_clear(&fp2);
        ret = tt_fpath_get_child(&fp, "child1/", &fp2);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_fpath_cmp(&fp2, "/a/b/c/d/e/child1/"), 0, "");

        ret = tt_fpath_get_absolute(&fp, &fp2);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_fpath_is_absolute(&fp2), TT_TRUE, "");
        TT_UT_EQUAL(tt_fpath_is_relative(&fp2), TT_FALSE, "");
        TT_INFO("abs path: %s", tt_fpath_render(&fp2));
        TT_UT_EQUAL(tt_fpath_cmp(&fp2, tt_fpath_render(&fp2)), 0, "");

        ret = tt_fpath_to_sibling(&fp, "sib1/");
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_fpath_cmp(&fp, "/a/b/c/d/sib1/"), 0, "");
        tt_fpath_parse(&fp, "/a/b/c/d/e/");
        ret = tt_fpath_to_child(&fp, "child1/child2/child3");
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_fpath_cmp(&fp, "/a/b/c/d/e/child1/child2/child3"),
                    0,
                    "");

        tt_fpath_clear(&fp);
        ret = tt_fpath_to_absolute(&fp);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_fpath_is_absolute(&fp), TT_TRUE, "");
        TT_UT_EQUAL(tt_fpath_is_relative(&fp), TT_FALSE, "");
        TT_INFO("abs path: %s", tt_fpath_render(&fp2));
    }

    // root directory
    {
        tt_fpath_parse(&fp, "/");

        ret = tt_fpath_get_sibling(&fp, "sib1", &fp2);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_fpath_cmp(&fp2, "/sib1"), 0, "");
        tt_fpath_clear(&fp2);
        ret = tt_fpath_get_child(&fp, "child1/", &fp2);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_fpath_cmp(&fp2, "/child1/"), 0, "");

        tt_fpath_parse(&fp, "/");
        ret = tt_fpath_to_sibling(&fp, "sib1/sib2");
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_fpath_cmp(&fp, "/sib1/sib2"), 0, "");
        tt_fpath_parse(&fp, "/");
        ret = tt_fpath_to_child(&fp, "child1/.");
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_fpath_cmp(&fp, "/child1/./"), 0, "");
    }

    tt_fpath_destroy(&fp);
    tt_fpath_destroy(&fp2);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_fpath_parent)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_fpath_t fp, fp2;
    tt_result_t ret;

    TT_TEST_CASE_ENTER()
    // test start

    tt_fpath_init(&fp, TT_FPATH_SEP_UNIX);
    tt_fpath_init(&fp2, TT_FPATH_SEP_UNIX);

    {
        ret = tt_fpath_get_parent(&fp, &fp2);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_fpath_cmp(&fp2, "../"), 0, "");

        tt_fpath_to_parent(&fp);
        TT_UT_EQUAL(tt_fpath_cmp(&fp, "../"), 0, "");
    }

    {
        tt_fpath_parse(&fp, "/");

        ret = tt_fpath_get_parent(&fp, &fp2);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_fpath_cmp(&fp2, "/"), 0, "");

        tt_fpath_to_parent(&fp);
        TT_UT_EQUAL(tt_fpath_cmp(&fp, "/"), 0, "");
    }

    {
        tt_fpath_parse(&fp, "ab");

        ret = tt_fpath_get_parent(&fp, &fp2);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_fpath_empty(&fp2), TT_TRUE, "");

        tt_fpath_to_parent(&fp);
        TT_UT_EQUAL(tt_fpath_empty(&fp2), TT_TRUE, "");
    }

    {
        tt_fpath_parse(&fp, "/ab");

        ret = tt_fpath_get_parent(&fp, &fp2);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_fpath_cmp(&fp2, "/"), 0, "");

        tt_fpath_to_parent(&fp);
        TT_UT_EQUAL(tt_fpath_cmp(&fp, "/"), 0, "");
    }

    {
        tt_fpath_parse(&fp, "/ab/./");

        ret = tt_fpath_get_parent(&fp, &fp2);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_fpath_cmp(&fp2, "/"), 0, "");

        tt_fpath_to_parent(&fp);
        TT_UT_EQUAL(tt_fpath_cmp(&fp, "/"), 0, "");
    }

    {
        tt_fpath_parse(&fp, "/ab/././.");

        ret = tt_fpath_get_parent(&fp, &fp2);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_fpath_cmp(&fp2, "/"), 0, "");

        tt_fpath_to_parent(&fp);
        TT_UT_EQUAL(tt_fpath_cmp(&fp, "/"), 0, "");
    }

    {
        tt_fpath_parse(&fp, "ab/cde/f/g/h");

        ret = tt_fpath_get_parent(&fp, &fp2);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_fpath_cmp(&fp2, "ab/cde/f/g/"), 0, "");

        tt_fpath_to_parent(&fp);
        TT_UT_EQUAL(tt_fpath_cmp(&fp, "ab/cde/f/g/"), 0, "");
    }

    {
        tt_fpath_parse(&fp, "/ab/cd/../h");

        ret = tt_fpath_get_parent(&fp, &fp2);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_fpath_cmp(&fp2, "/ab/cd/../"), 0, "");

        tt_fpath_to_parent(&fp);
        TT_UT_EQUAL(tt_fpath_cmp(&fp, "/ab/cd/../"), 0, "");
    }

    {
        tt_fpath_parse(&fp, "ab/../../..");

        ret = tt_fpath_get_parent(&fp, &fp2);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_fpath_cmp(&fp2, "../../../"), 0, "");

        tt_fpath_to_parent(&fp);
        TT_UT_EQUAL(tt_fpath_cmp(&fp, "../../../"), 0, "");
    }

    {
        tt_fpath_parse(&fp, "a/b/c/../");

        ret = tt_fpath_get_parent(&fp, &fp2);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_fpath_cmp(&fp2, "a/"), 0, "");

        tt_fpath_to_parent(&fp);
        TT_UT_EQUAL(tt_fpath_cmp(&fp, "a/"), 0, "");
    }

    tt_fpath_destroy(&fp);
    tt_fpath_destroy(&fp2);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_fpath_root)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_fpath_t fp, fp2;
    tt_result_t ret = TT_FAIL;

    TT_TEST_CASE_ENTER()
    // test start

    tt_fpath_init(&fp, TT_FPATH_SEP_UNIX);
    tt_fpath_init(&fp2, TT_FPATH_SEP_UNIX);

    {
        TT_UT_EQUAL(tt_fpath_get_root(&fp)[0], 0, "");

        tt_fpath_to_root(&fp);
        TT_UT_EQUAL(tt_fpath_empty(&fp), TT_TRUE, "");
    }

    {
        tt_fpath_parse(&fp, "/");

        TT_UT_EQUAL(tt_strcmp(tt_fpath_get_root(&fp), "/"), 0, "");

        tt_fpath_to_root(&fp);
        TT_UT_EQUAL(tt_fpath_cmp(&fp, "/"), 0, "");
    }

    {
        tt_fpath_parse(&fp, "ab");

        TT_UT_EQUAL(tt_fpath_get_root(&fp)[0], 0, "");

        tt_fpath_to_root(&fp);
        TT_UT_EQUAL(tt_fpath_empty(&fp), TT_TRUE, "");
    }

    {
        tt_fpath_parse(&fp, "/ab");

        TT_UT_EQUAL(tt_strcmp(tt_fpath_get_root(&fp), "/"), 0, "");

        tt_fpath_to_root(&fp);
        TT_UT_EQUAL(tt_fpath_cmp(&fp, "/"), 0, "");
    }

    {
        tt_fpath_parse(&fp, "/ab/./");

        TT_UT_EQUAL(tt_strcmp(tt_fpath_get_root(&fp), "/"), 0, "");

        tt_fpath_to_root(&fp);
        TT_UT_EQUAL(tt_fpath_cmp(&fp, "/"), 0, "");
    }

    {
        tt_fpath_parse(&fp, "/ab/././.");

        TT_UT_EQUAL(tt_strcmp(tt_fpath_get_root(&fp), "/"), 0, "");

        tt_fpath_to_root(&fp);
        TT_UT_EQUAL(tt_fpath_empty(&fp), TT_TRUE, "");
    }

    {
        tt_fpath_parse(&fp, "ab/cde/f/g/h");

        TT_UT_EQUAL(tt_fpath_get_root(&fp)[0], 0, "");

        tt_fpath_to_root(&fp);
        TT_UT_EQUAL(tt_fpath_empty(&fp), TT_TRUE, "");
    }

    {
        tt_fpath_parse(&fp, "/ab/cd/../h");

        TT_UT_EQUAL(tt_strcmp(tt_fpath_get_root(&fp), "/"), 0, "");

        tt_fpath_to_root(&fp);
        TT_UT_EQUAL(tt_fpath_cmp(&fp, "/"), 0, "");
    }

    tt_fpath_destroy(&fp);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_fpath_name)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_fpath_t fp, fp2;
    tt_fpath_iter_t iter;
    tt_result_t ret;

    TT_TEST_CASE_ENTER()
    // test start

    tt_fpath_init(&fp, TT_FPATH_SEP_UNIX);
    tt_fpath_init(&fp2, TT_FPATH_SEP_WINDOWS);

    // empty
    TT_UT_EQUAL(tt_fpath_count(&fp), 0, "");
    TT_UT_NULL(tt_fpath_get_name(&fp, 0), "");
    TT_UT_NULL(tt_fpath_get_name(&fp, 1), "");
    TT_UT_NULL(tt_fpath_get_name(&fp, ~0), "");
    TT_UT_FAIL(tt_fpath_set_name(&fp, 0, "123"), "");
    TT_UT_FAIL(tt_fpath_set_name(&fp, 1, "123"), "");
    TT_UT_FAIL(tt_fpath_set_name(&fp, ~0, "123"), "");
    TT_UT_EQUAL(tt_fpath_render(&fp)[0], 0, "");

    tt_fpath_iter(&fp, &iter);
    TT_UT_NULL(tt_fpath_iter_next(&iter), "");

    ret = tt_fpath_get_sub(&fp, 0, 0, &fp2);
    TT_UT_FAIL(ret, "");
    ret = tt_fpath_get_sub(&fp, 0, 1, &fp2);
    TT_UT_FAIL(ret, "");
    ret = tt_fpath_get_sub(&fp, 1, 0, &fp2);
    TT_UT_FAIL(ret, "");
    ret = tt_fpath_get_sub(&fp, 1, 1, &fp2);
    TT_UT_FAIL(ret, "");

    // root dir
    tt_fpath_parse(&fp, "c:/");
    TT_UT_EQUAL(tt_fpath_count(&fp), 0, "");
    TT_UT_NULL(tt_fpath_get_name(&fp, 0), "");
    TT_UT_NULL(tt_fpath_get_name(&fp, 1), "");
    TT_UT_NULL(tt_fpath_get_name(&fp, ~0), "");
    TT_UT_FAIL(tt_fpath_set_name(&fp, 0, "123"), "");
    TT_UT_FAIL(tt_fpath_set_name(&fp, 1, "123"), "");
    TT_UT_FAIL(tt_fpath_set_name(&fp, ~0, "123"), "");
    TT_UT_EQUAL(tt_strcmp(tt_fpath_render(&fp), "c:/"), 0, "");

    tt_fpath_iter(&fp, &iter);
    TT_UT_NULL(tt_fpath_iter_next(&iter), "");

    ret = tt_fpath_get_sub(&fp, 0, 0, &fp2);
    TT_UT_FAIL(ret, "");
    ret = tt_fpath_get_sub(&fp, 0, 1, &fp2);
    TT_UT_FAIL(ret, "");
    ret = tt_fpath_get_sub(&fp, 1, 0, &fp2);
    TT_UT_FAIL(ret, "");
    ret = tt_fpath_get_sub(&fp, 1, 1, &fp2);
    TT_UT_FAIL(ret, "");

    // single file
    tt_fpath_parse(&fp, "abc");
    TT_UT_EQUAL(tt_fpath_count(&fp), 1, "");
    TT_UT_EQUAL(tt_strcmp(tt_fpath_get_name(&fp, 0), "abc"), 0, "");
    TT_UT_NULL(tt_fpath_get_name(&fp, 1), "");
    TT_UT_NULL(tt_fpath_get_name(&fp, ~0), "");
    TT_UT_SUCCESS(tt_fpath_set_name(&fp, 0, "123"), "");
    TT_UT_FAIL(tt_fpath_set_name(&fp, 0, "123/"), "");
    TT_UT_FAIL(tt_fpath_set_name(&fp, 0, "123//"), "");
    TT_UT_FAIL(tt_fpath_set_name(&fp, 1, "123"), "");
    TT_UT_FAIL(tt_fpath_set_name(&fp, ~0, "123"), "");
    TT_UT_EQUAL(tt_strcmp(tt_fpath_render(&fp), "123"), 0, "");

    tt_fpath_iter(&fp, &iter);
    TT_UT_EQUAL(tt_strcmp(tt_fpath_iter_next(&iter), "123"), 0, "");
    TT_UT_NULL(tt_fpath_iter_next(&iter), "");

    ret = tt_fpath_get_sub(&fp, 0, 0, &fp2);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_fpath_render(&fp2)[0], 0, "");
    ret = tt_fpath_get_sub(&fp, 0, 1, &fp2);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_fpath_cmp(&fp2, "123"), 0, "");
    ret = tt_fpath_get_sub(&fp, 0, 2, &fp2);
    TT_UT_FAIL(ret, "");
    ret = tt_fpath_get_sub(&fp, 1, 1, &fp2);
    TT_UT_FAIL(ret, "");

    // single dir
    tt_fpath_parse(&fp, "/abc/");
    TT_UT_EQUAL(tt_fpath_count(&fp), 1, "");
    TT_UT_EQUAL(tt_strcmp(tt_fpath_get_name(&fp, 0), "abc"), 0, "");
    TT_UT_NULL(tt_fpath_get_name(&fp, 1), "");
    TT_UT_NULL(tt_fpath_get_name(&fp, ~0), "");
    TT_UT_SUCCESS(tt_fpath_set_name(&fp, 0, "123"), "");
    TT_UT_FAIL(tt_fpath_set_name(&fp, 0, "123/"), "");
    TT_UT_FAIL(tt_fpath_set_name(&fp, 0, "123//"), "");
    TT_UT_FAIL(tt_fpath_set_name(&fp, 1, "123"), "");
    TT_UT_FAIL(tt_fpath_set_name(&fp, ~0, "123"), "");
    TT_UT_EQUAL(tt_strcmp(tt_fpath_render(&fp), "/123/"), 0, "");

    tt_fpath_iter(&fp, &iter);
    TT_UT_EQUAL(tt_strcmp(tt_fpath_iter_next(&iter), "123"), 0, "");
    TT_UT_NULL(tt_fpath_iter_next(&iter), "");

    ret = tt_fpath_get_sub(&fp, 0, 0, &fp2);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_fpath_render(&fp2)[0], 0, "");
    ret = tt_fpath_get_sub(&fp, 0, 1, &fp2);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_fpath_cmp(&fp2, "123\\"), 0, "");
    ret = tt_fpath_get_sub(&fp, 0, 2, &fp2);
    TT_UT_FAIL(ret, "");
    ret = tt_fpath_get_sub(&fp, 1, 1, &fp2);
    TT_UT_FAIL(ret, "");

    // multiple level file
    tt_fpath_parse(&fp, "a/b/c/d");
    TT_UT_EQUAL(tt_fpath_count(&fp), 4, "");
    TT_UT_EQUAL(tt_strcmp(tt_fpath_get_name(&fp, 0), "a"), 0, "");
    TT_UT_EQUAL(tt_strcmp(tt_fpath_get_name(&fp, 1), "b"), 0, "");
    TT_UT_EQUAL(tt_strcmp(tt_fpath_get_name(&fp, 3), "d"), 0, "");
    TT_UT_NULL(tt_fpath_get_name(&fp, 4), "");
    TT_UT_NULL(tt_fpath_get_name(&fp, ~0), "");
    TT_UT_SUCCESS(tt_fpath_set_name(&fp, 0, "x1"), "");
    TT_UT_SUCCESS(tt_fpath_set_name(&fp, 1, "y2"), "");
    TT_UT_SUCCESS(tt_fpath_set_name(&fp, 2, "z3"), "");
    TT_UT_SUCCESS(tt_fpath_set_name(&fp, 3, "u4"), "");
    TT_UT_FAIL(tt_fpath_set_name(&fp, 4, "??"), "");
    TT_UT_FAIL(tt_fpath_set_name(&fp, ~0, "??"), "");
    TT_UT_EQUAL(tt_strcmp(tt_fpath_render(&fp), "x1/y2/z3/u4"), 0, "");

    tt_fpath_iter(&fp, &iter);
    TT_UT_EQUAL(tt_strcmp(tt_fpath_iter_next(&iter), "x1"), 0, "");
    TT_UT_EQUAL(tt_strcmp(tt_fpath_iter_next(&iter), "y2"), 0, "");
    TT_UT_EQUAL(tt_strcmp(tt_fpath_iter_next(&iter), "z3"), 0, "");
    TT_UT_EQUAL(tt_strcmp(tt_fpath_iter_next(&iter), "u4"), 0, "");
    TT_UT_NULL(tt_fpath_iter_next(&iter), "");

    ret = tt_fpath_get_sub(&fp, 0, 0, &fp2);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_fpath_render(&fp2)[0], 0, "");
    ret = tt_fpath_get_sub(&fp, 0, 1, &fp2);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_fpath_cmp(&fp2, "x1\\"), 0, "");
    ret = tt_fpath_get_sub(&fp, 1, 2, &fp2);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_fpath_cmp(&fp2, "y2\\z3\\"), 0, "");
    ret = tt_fpath_get_sub(&fp, 2, 2, &fp2);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_fpath_cmp(&fp2, "z3\\u4"), 0, "");
    ret = tt_fpath_get_sub(&fp, 3, 2, &fp2);
    TT_UT_FAIL(ret, "");

    // multiple level dir
    tt_fpath_parse(&fp, "a/b/c/d/");
    TT_UT_EQUAL(tt_fpath_count(&fp), 4, "");
    TT_UT_EQUAL(tt_strcmp(tt_fpath_get_name(&fp, 0), "a"), 0, "");
    TT_UT_EQUAL(tt_strcmp(tt_fpath_get_name(&fp, 1), "b"), 0, "");
    TT_UT_EQUAL(tt_strcmp(tt_fpath_get_name(&fp, 3), "d"), 0, "");
    TT_UT_NULL(tt_fpath_get_name(&fp, 4), "");
    TT_UT_NULL(tt_fpath_get_name(&fp, ~0), "");
    TT_UT_SUCCESS(tt_fpath_set_name(&fp, 0, "x1"), "");
    TT_UT_SUCCESS(tt_fpath_set_name(&fp, 1, "y2"), "");
    TT_UT_SUCCESS(tt_fpath_set_name(&fp, 2, "z3"), "");
    TT_UT_SUCCESS(tt_fpath_set_name(&fp, 3, "u4"), "");
    TT_UT_FAIL(tt_fpath_set_name(&fp, 4, "??"), "");
    TT_UT_FAIL(tt_fpath_set_name(&fp, ~0, "??"), "");
    TT_UT_EQUAL(tt_strcmp(tt_fpath_render(&fp), "x1/y2/z3/u4/"), 0, "");

    tt_fpath_iter(&fp, &iter);
    TT_UT_EQUAL(tt_strcmp(tt_fpath_iter_next(&iter), "x1"), 0, "");
    TT_UT_EQUAL(tt_strcmp(tt_fpath_iter_next(&iter), "y2"), 0, "");
    TT_UT_EQUAL(tt_strcmp(tt_fpath_iter_next(&iter), "z3"), 0, "");
    TT_UT_EQUAL(tt_strcmp(tt_fpath_iter_next(&iter), "u4"), 0, "");
    TT_UT_NULL(tt_fpath_iter_next(&iter), "");

    ret = tt_fpath_get_sub(&fp, 0, 0, &fp2);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_fpath_render(&fp2)[0], 0, "");
    ret = tt_fpath_get_sub(&fp, 0, 1, &fp2);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_fpath_cmp(&fp2, "x1\\"), 0, "");
    ret = tt_fpath_get_sub(&fp, 1, 3, &fp2);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_fpath_cmp(&fp2, "y2\\z3\\u4\\"), 0, "");
    ret = tt_fpath_get_sub(&fp, 2, 1, &fp2);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_fpath_cmp(&fp2, "z3\\"), 0, "");
    ret = tt_fpath_get_sub(&fp, 3, 2, &fp2);
    TT_UT_FAIL(ret, "");

    tt_fpath_destroy(&fp);
    tt_fpath_destroy(&fp2);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_fpath_normalize)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_fpath_t fp;

    TT_TEST_CASE_ENTER()
    // test start

    // empty
    TT_UT_SUCCESS(tt_fpath_create_cstr(&fp, "", TT_FPATH_SEP_UNIX), "");
    tt_fpath_normalize(&fp);
    TT_UT_STREQ(tt_fpath_render(&fp), "", "");
    TT_UT_TRUE(tt_fpath_empty(&fp), "");
    {
        tt_char_t buf[128] = {0};
        TT_UT_EQUAL(tt_fpath_pctencode_len(&fp, tt_g_uri_encode_table), 0, "");
        TT_UT_EQUAL(tt_fpath_pctencode(&fp, tt_g_uri_encode_table, buf), 0, "");
    }

    // "."
    TT_UT_SUCCESS(tt_fpath_parse(&fp, "."), "");
    tt_fpath_normalize(&fp);
    TT_UT_STREQ(tt_fpath_render(&fp), "./", "");
    TT_UT_FALSE(tt_fpath_empty(&fp), "");
    {
        tt_char_t buf[128] = {0};
        TT_UT_EQUAL(tt_fpath_pctencode_len(&fp, tt_g_uri_encode_table), 2, "");
        TT_UT_EQUAL(tt_fpath_pctencode(&fp, tt_g_uri_encode_table, buf), 2, "");
        TT_UT_STREQ(buf, "./", "");
    }

    TT_UT_SUCCESS(tt_fpath_parse(&fp, "/."), "");
    tt_fpath_normalize(&fp);
    TT_UT_STREQ(tt_fpath_render(&fp), "/", "");

    TT_UT_SUCCESS(tt_fpath_parse(&fp, "/././././."), "");
    tt_fpath_normalize(&fp);
    TT_UT_STREQ(tt_fpath_render(&fp), "/", "");

    TT_UT_SUCCESS(tt_fpath_parse(&fp, "/./a/./"), "");
    tt_fpath_normalize(&fp);
    TT_UT_STREQ(tt_fpath_render(&fp), "/a/", "");

    TT_UT_SUCCESS(tt_fpath_parse(&fp, "/./a/./b/."), "");
    tt_fpath_normalize(&fp);
    TT_UT_STREQ(tt_fpath_render(&fp), "/a/b/", "");

    TT_UT_SUCCESS(tt_fpath_parse(&fp, "././a/./b/./c"), "");
    tt_fpath_normalize(&fp);
    TT_UT_STREQ(tt_fpath_render(&fp), "a/b/c", "");

    // ".."
    TT_UT_SUCCESS(tt_fpath_parse(&fp, ".."), "");
    tt_fpath_normalize(&fp);
    TT_UT_STREQ(tt_fpath_render(&fp), "../", "");

    TT_UT_SUCCESS(tt_fpath_parse(&fp, "../../.."), "");
    tt_fpath_normalize(&fp);
    TT_UT_STREQ(tt_fpath_render(&fp), "../../../", "");

    TT_UT_SUCCESS(tt_fpath_parse(&fp, "/../a/../b"), "");
    tt_fpath_normalize(&fp);
    TT_UT_STREQ(tt_fpath_render(&fp), "/b", "");

    TT_UT_SUCCESS(tt_fpath_parse(&fp, "/.././a/./.././b/./.././../../c/"), "");
    tt_fpath_normalize(&fp);
    TT_UT_STREQ(tt_fpath_render(&fp), "/c/", "");

    tt_fpath_clear(&fp);
    TT_UT_TRUE(tt_fpath_empty(&fp), "");

    TT_UT_SUCCESS(tt_fpath_parse(&fp, "/a=b/c;d/e.f"), "");
    {
        tt_char_t buf[128] = {0};
        TT_UT_EQUAL(tt_fpath_pctencode_len(&fp, tt_g_uri_encode_table), 16, "");
        TT_UT_EQUAL(tt_fpath_pctencode(&fp, tt_g_uri_encode_table, buf),
                    16,
                    "");
        TT_UT_STREQ(buf, "/a%3db/c%3bd/e.f", "");
    }

    tt_fpath_destroy(&fp);

    tt_fpath_init(&fp, TT_FPATH_SEP_UNIX);
    TT_UT_TRUE(tt_fpath_empty(&fp), "");

    tt_fpath_destroy(&fp);

    {
        // resolve
        tt_fpath_t a, b, c;
        tt_fpath_init(&a, TT_FPATH_SEP_UNIX);
        tt_fpath_init(&b, TT_FPATH_SEP_UNIX);
        tt_fpath_init(&c, TT_FPATH_SEP_UNIX);

        TT_UT_SUCCESS(tt_fpath_parse(&a, "/a/b/c"), "");
        TT_UT_SUCCESS(tt_fpath_parse(&b, "/c/d/"), "");
        TT_UT_SUCCESS(tt_fpath_resolve(&a, &b, &c), "");
        TT_UT_EQUAL(tt_strcmp(tt_fpath_render(&c), "/c/d/"), 0, "");

        tt_fpath_clear(&b);
        TT_UT_SUCCESS(tt_fpath_resolve(&a, &b, &c), "");
        TT_UT_EQUAL(tt_strcmp(tt_fpath_render(&c), "/a/b/c"), 0, "");

        TT_UT_SUCCESS(tt_fpath_parse(&b, "c/d/"), "");
        TT_UT_SUCCESS(tt_fpath_resolve(&a, &b, &c), "");
        TT_UT_EQUAL(tt_strcmp(tt_fpath_render(&c), "/a/b/c/d/"), 0, "");

        TT_UT_SUCCESS(tt_fpath_parse(&a, "xxx/yy/"), "");
        TT_UT_SUCCESS(tt_fpath_parse(&b, "c/d.exe"), "");
        TT_UT_SUCCESS(tt_fpath_resolve(&a, &b, &c), "");
        TT_UT_EQUAL(tt_strcmp(tt_fpath_render(&c), "xxx/yy/c/d.exe"), 0, "");

        tt_fpath_destroy(&a);
        tt_fpath_destroy(&b);
        tt_fpath_destroy(&c);
    }

    // relativize
    {
        // empty
        tt_fpath_t a, b, c, tmp;
        tt_fpath_init(&a, TT_FPATH_SEP_UNIX);
        tt_fpath_init(&b, TT_FPATH_SEP_WINDOWS);
        tt_fpath_init(&c, TT_FPATH_SEP_WINDOWS);
        tt_fpath_init(&tmp, TT_FPATH_SEP_WINDOWS);

        TT_UT_FAIL(tt_fpath_relativize(&a, &b, &c), "");

        tt_fpath_init(&a, TT_FPATH_SEP_WINDOWS);
        tt_fpath_parse(&a, "c:\\a\b");
        tt_fpath_parse(&b, "x\\y\\");
        TT_UT_FAIL(tt_fpath_relativize(&a, &b, &c), "");

        tt_fpath_parse(&a, "c:\\a\b");
        tt_fpath_parse(&b, "d:\\x\\y\\");
        TT_UT_FAIL(tt_fpath_relativize(&a, &b, &c), "");

        // absolute
        tt_fpath_parse(&a, "c:");
        tt_fpath_parse(&b, "c:");
        TT_UT_SUCCESS(tt_fpath_relativize(&a, &b, &c), "");
        TT_UT_STREQ(tt_fpath_render(&c), "", "");

        tt_fpath_parse(&b, "c:x\\y\\z");
        TT_UT_SUCCESS(tt_fpath_relativize(&a, &b, &c), "");
        TT_UT_STREQ(tt_fpath_render(&c), "x\\y\\z", "");
        TT_UT_SUCCESS(tt_fpath_relativize(&b, &a, &c), "");
        TT_UT_STREQ(tt_fpath_render(&c), "..\\..\\", "");
        TT_UT_SUCCESS(tt_fpath_resolve(&b, &c, &tmp), "");
        TT_UT_STREQ(tt_fpath_render(&tmp), "c:\\x\\y\\..\\..\\", "");

        tt_fpath_parse(&b, "c:x\\y\\z\\");
        TT_UT_SUCCESS(tt_fpath_relativize(&a, &b, &c), "");
        TT_UT_STREQ(tt_fpath_render(&c), "x\\y\\z\\", "");
        TT_UT_SUCCESS(tt_fpath_relativize(&b, &a, &c), "");
        TT_UT_STREQ(tt_fpath_render(&c), "..\\..\\..\\", "");

        // relative
        tt_fpath_clear(&a);
        tt_fpath_clear(&b);
        TT_UT_SUCCESS(tt_fpath_relativize(&a, &b, &c), "");
        TT_UT_STREQ(tt_fpath_render(&c), "", "");

        tt_fpath_parse(&b, "x\\y\\z");
        TT_UT_SUCCESS(tt_fpath_relativize(&a, &b, &c), "");
        TT_UT_STREQ(tt_fpath_render(&c), "x\\y\\z", "");
        TT_UT_SUCCESS(tt_fpath_relativize(&b, &a, &c), "");
        TT_UT_STREQ(tt_fpath_render(&c), "..\\..\\", "");
        TT_UT_SUCCESS(tt_fpath_resolve(&b, &c, &tmp), "");
        TT_UT_STREQ(tt_fpath_render(&tmp), "x\\y\\..\\..\\", "");

        tt_fpath_parse(&b, "x\\y\\z\\");
        TT_UT_SUCCESS(tt_fpath_relativize(&a, &b, &c), "");
        TT_UT_STREQ(tt_fpath_render(&c), "x\\y\\z\\", "");
        TT_UT_SUCCESS(tt_fpath_relativize(&b, &a, &c), "");
        TT_UT_STREQ(tt_fpath_render(&c), "..\\..\\..\\", "");

        tt_fpath_destroy(&a);
        tt_fpath_destroy(&b);
        tt_fpath_destroy(&c);
        tt_fpath_destroy(&tmp);
    }

    // test end
    TT_TEST_CASE_LEAVE()
}
