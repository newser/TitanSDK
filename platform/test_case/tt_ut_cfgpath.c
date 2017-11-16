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

#include <log/tt_log.h>
#include <os/tt_spinlock.h>
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
TT_TEST_ROUTINE_DECLARE(case_cfgpath_p2n_abs)
TT_TEST_ROUTINE_DECLARE(case_cfgpath_p2n_rel)
TT_TEST_ROUTINE_DECLARE(case_cfgpath_p2n_dot)
TT_TEST_ROUTINE_DECLARE(case_cfgpath_n2p)
TT_TEST_ROUTINE_DECLARE(case_cfgpath_comp)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(cfgpath_case)

TT_TEST_CASE("case_cfgpath_p2n_abs",
             "testing cfgpath: absolute to node",
             case_cfgpath_p2n_abs,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("case_cfgpath_p2n_rel",
                 "testing cfgpath: relative to node",
                 case_cfgpath_p2n_rel,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_cfgpath_p2n_dot",
                 "testing cfgpath: dotted path",
                 case_cfgpath_p2n_dot,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_cfgpath_n2p",
                 "testing cfgpath: path to node",
                 case_cfgpath_n2p,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_cfgpath_comp",
                 "config path, auto complete",
                 case_cfgpath_comp,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(cfgpath_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(TEST_UNIT_CFGPATH, 0, cfgpath_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(case_cfgpath_p2n)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(case_cfgpath_p2n_abs)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_cfgobj_t *root, *g1, *g11, *g12, *g121, *c1211;
    tt_u32_t val = 0;
    tt_cfgobj_t *n;
    tt_string_t path;

    TT_TEST_CASE_ENTER()
    // test start

    tt_string_init(&path, NULL);

    root = tt_cfgdir_create("", NULL);
    TT_UT_NOT_EQUAL(root, NULL, "");
    g1 = tt_cfgdir_create("g1", NULL);
    TT_UT_NOT_EQUAL(g1, NULL, "");
    g11 = tt_cfgdir_create("g11", NULL);
    TT_UT_NOT_EQUAL(g11, NULL, "");
    g12 = tt_cfgdir_create("g12", NULL);
    TT_UT_NOT_EQUAL(g12, NULL, "");
    g121 = tt_cfgdir_create("g121", NULL);
    TT_UT_NOT_EQUAL(g121, NULL, "");
    c1211 = tt_cfgu32_create("c1211", &val, NULL, NULL);
    TT_UT_NOT_EQUAL(g121, NULL, "");

    tt_cfgdir_add(TT_CFGOBJ_CAST(root, tt_cfgdir_t), g1);
    tt_cfgdir_add(TT_CFGOBJ_CAST(g1, tt_cfgdir_t), g11);
    tt_cfgdir_add(TT_CFGOBJ_CAST(g1, tt_cfgdir_t), g12);
    tt_cfgdir_add(TT_CFGOBJ_CAST(g12, tt_cfgdir_t), g121);
    tt_cfgdir_add(TT_CFGOBJ_CAST(g121, tt_cfgdir_t), c1211);

    //////////////////////////////////
    // absolute path
    //////////////////////////////////

    // null path
    n = tt_cfgpath_p2n_cstr(root, root, "");
    TT_UT_EQUAL(n, NULL, "");

    // invalid current
    n = tt_cfgpath_p2n_cstr(c1211, root, "/");
    TT_UT_EQUAL(n, NULL, "");

    // one slash
    n = tt_cfgpath_p2n_cstr(root, root, "/");
    TT_UT_EQUAL(n, root, "");

    // two slash
    n = tt_cfgpath_p2n_cstr(root, root, "//");
    TT_UT_EQUAL(n, NULL, "");

    // multiple slash
    n = tt_cfgpath_p2n_cstr(root, root, "/////");
    TT_UT_EQUAL(n, NULL, "");

    // invalid path
    n = tt_cfgpath_p2n_cstr(root, root, "/g");
    TT_UT_EQUAL(n, NULL, "");

    // valid path
    n = tt_cfgpath_p2n_cstr(root, root, "/g1");
    TT_UT_EQUAL(n, g1, "");

    // valid path
    n = tt_cfgpath_p2n_cstr(root, root, "/g1/");
    TT_UT_EQUAL(n, g1, "");

    // invalid path
    n = tt_cfgpath_p2n_cstr(root, root, "/g1/g1");
    TT_UT_EQUAL(n, NULL, "");

    // valid path
    n = tt_cfgpath_p2n_cstr(root, root, "/g1/g11");
    TT_UT_EQUAL(n, g11, "");

    // valid path
    n = tt_cfgpath_p2n_cstr(root, root, "/g1/g12/");
    TT_UT_EQUAL(n, g12, "");

    // valid path
    n = tt_cfgpath_p2n_cstr(root, root, "/g1/g12/g121/c1211");
    TT_UT_EQUAL(n, c1211, "");

    // invalid path
    n = tt_cfgpath_p2n_cstr(root, root, "/g1/g12xx/g121/c1211");
    TT_UT_EQUAL(n, NULL, "");

    // invalid path
    n = tt_cfgpath_p2n_cstr(root, root, "/g1xx/g12/g121/c1211");
    TT_UT_EQUAL(n, NULL, "");

    // invalid path
    n = tt_cfgpath_p2n_cstr(root, root, "/g1/g12/g121xx/c1211");
    TT_UT_EQUAL(n, NULL, "");

    // valid path
    n = tt_cfgpath_p2n_cstr(g12, g12, "/g121/c1211");
    TT_UT_EQUAL(n, c1211, "");

    // valid path
    n = tt_cfgpath_p2n_cstr(g1, g1, "/g12/g121/");
    TT_UT_EQUAL(n, g121, "");

    // invalid path
    n = tt_cfgpath_p2n_cstr(root, root, "/g1/g12/g121/c1211/");
    TT_UT_EQUAL(n, NULL, "");

    // invalid path
    n = tt_cfgpath_p2n_cstr(root, root, "/g1/g12/g121//c1211");
    TT_UT_EQUAL(n, NULL, "");

    tt_cfgobj_destroy(root);
    tt_string_destroy(&path);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_cfgpath_p2n_rel)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_cfgobj_t *root, *g1, *g11, *g12, *g121, *c1211;
    tt_u32_t val = 0;
    tt_cfgobj_t *n;
    tt_string_t path;

    TT_TEST_CASE_ENTER()
    // test start

    tt_string_init(&path, NULL);

    root = tt_cfgdir_create("", NULL);
    TT_UT_NOT_EQUAL(root, NULL, "");
    g1 = tt_cfgdir_create("g1", NULL);
    TT_UT_NOT_EQUAL(g1, NULL, "");
    g11 = tt_cfgdir_create("g11", NULL);
    TT_UT_NOT_EQUAL(g11, NULL, "");
    g12 = tt_cfgdir_create("g12", NULL);
    TT_UT_NOT_EQUAL(g12, NULL, "");
    g121 = tt_cfgdir_create("g121", NULL);
    TT_UT_NOT_EQUAL(g121, NULL, "");
    c1211 = tt_cfgu32_create("c1211", &val, NULL, NULL);
    TT_UT_NOT_EQUAL(g121, NULL, "");

    tt_cfgdir_add(TT_CFGOBJ_CAST(root, tt_cfgdir_t), g1);
    tt_cfgdir_add(TT_CFGOBJ_CAST(g1, tt_cfgdir_t), g11);
    tt_cfgdir_add(TT_CFGOBJ_CAST(g1, tt_cfgdir_t), g12);
    tt_cfgdir_add(TT_CFGOBJ_CAST(g12, tt_cfgdir_t), g121);
    tt_cfgdir_add(TT_CFGOBJ_CAST(g121, tt_cfgdir_t), c1211);

    //////////////////////////////////
    // absolute path
    //////////////////////////////////

    // null path
    n = tt_cfgpath_p2n_cstr(root, root, "");
    TT_UT_EQUAL(n, NULL, "");

    // one slash
    n = tt_cfgpath_p2n_cstr(root, root, "/");
    TT_UT_EQUAL(n, root, "");

    // invalid path
    n = tt_cfgpath_p2n_cstr(root, root, "g");
    TT_UT_EQUAL(n, NULL, "");

    // invalid path
    n = tt_cfgpath_p2n_cstr(root, c1211, ".");
    TT_UT_EQUAL(n, NULL, "");

    // valid path
    n = tt_cfgpath_p2n_cstr(root, root, "g1");
    TT_UT_EQUAL(n, g1, "");

    // valid path
    n = tt_cfgpath_p2n_cstr(root, root, "g1/");
    TT_UT_EQUAL(n, g1, "");

    // invalid path
    n = tt_cfgpath_p2n_cstr(root, g1, "g1");
    TT_UT_EQUAL(n, NULL, "");

    // valid path
    n = tt_cfgpath_p2n_cstr(root, g1, "g11");
    TT_UT_EQUAL(n, g11, "");

    // valid path
    n = tt_cfgpath_p2n_cstr(root, g1, "g12/");
    TT_UT_EQUAL(n, g12, "");

    // valid path
    n = tt_cfgpath_p2n_cstr(root, g1, "g12/g121/c1211");
    TT_UT_EQUAL(n, c1211, "");

    // valid path
    n = tt_cfgpath_p2n_cstr(root, g12, "g121/c1211");
    TT_UT_EQUAL(n, c1211, "");

    // valid path
    n = tt_cfgpath_p2n_cstr(root, g12, "g121/../g121/./c1211");
    TT_UT_EQUAL(n, c1211, "");

    // valid path
    n = tt_cfgpath_p2n_cstr(root, g12, "g121/../g121/.");
    TT_UT_EQUAL(n, g121, "");

    // valid path
    n = tt_cfgpath_p2n_cstr(root, g12, "g121/../g121/./..");
    TT_UT_EQUAL(n, g12, "");

    // valid path
    n = tt_cfgpath_p2n_cstr(root, g12, "g121/../g121/./../");
    TT_UT_EQUAL(n, g12, "");

    // invalid path
    n = tt_cfgpath_p2n_cstr(root, g1, "g12/g121/c1211/");
    TT_UT_EQUAL(n, NULL, "");

    // invalid path
    n = tt_cfgpath_p2n_cstr(root, g12, "g121//c1211");
    TT_UT_EQUAL(n, NULL, "");

    // invalid path
    n = tt_cfgpath_p2n_cstr(root, c1211, "./");
    TT_UT_EQUAL(n, NULL, "");

    tt_cfgobj_destroy(root);
    tt_string_destroy(&path);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_cfgpath_p2n_dot)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_cfgobj_t *root, *g1, *g11, *g12, *g121, *c1211;
    tt_u32_t val = 0;
    tt_cfgobj_t *n;
    tt_string_t path;

    TT_TEST_CASE_ENTER()
    // test start

    tt_string_init(&path, NULL);

    root = tt_cfgdir_create("", NULL);
    TT_UT_NOT_EQUAL(root, NULL, "");
    g1 = tt_cfgdir_create("g1", NULL);
    TT_UT_NOT_EQUAL(g1, NULL, "");
    g11 = tt_cfgdir_create("g11", NULL);
    TT_UT_NOT_EQUAL(g11, NULL, "");
    g12 = tt_cfgdir_create("g12", NULL);
    TT_UT_NOT_EQUAL(g12, NULL, "");
    g121 = tt_cfgdir_create("g121", NULL);
    TT_UT_NOT_EQUAL(g121, NULL, "");
    c1211 = tt_cfgu32_create("c1211", &val, NULL, NULL);
    TT_UT_NOT_EQUAL(g121, NULL, "");

    tt_cfgdir_add(TT_CFGOBJ_CAST(root, tt_cfgdir_t), g1);
    tt_cfgdir_add(TT_CFGOBJ_CAST(g1, tt_cfgdir_t), g11);
    tt_cfgdir_add(TT_CFGOBJ_CAST(g1, tt_cfgdir_t), g12);
    tt_cfgdir_add(TT_CFGOBJ_CAST(g12, tt_cfgdir_t), g121);
    tt_cfgdir_add(TT_CFGOBJ_CAST(g121, tt_cfgdir_t), c1211);

    //////////////////////////////////
    // absolute path
    //////////////////////////////////

    // null path
    tt_string_clear(&path);
    n = tt_cfgpath_p2n_cstr(root, root, "");
    TT_UT_EQUAL(n, NULL, "");

    // valid slash
    n = tt_cfgpath_p2n_cstr(root, root, "/././././");
    TT_UT_EQUAL(n, root, "");

    // valid slash
    n = tt_cfgpath_p2n_cstr(root, root, "/././././.");
    TT_UT_EQUAL(n, root, "");

    // invalid slash
    n = tt_cfgpath_p2n_cstr(root, root, "/./././././..");
    TT_UT_EQUAL(n, NULL, "");

    // invalid path
    n = tt_cfgpath_p2n_cstr(root, root, "/./../");
    TT_UT_EQUAL(n, NULL, "");

    // invalid path
    n = tt_cfgpath_p2n_cstr(root, root, "/./g");
    TT_UT_EQUAL(n, NULL, "");

    // valid path
    n = tt_cfgpath_p2n_cstr(root, root, "/g1/../g1");
    TT_UT_EQUAL(n, g1, "");

    // valid path
    n = tt_cfgpath_p2n_cstr(root, root, "./g1/../g1/g12/../../g1");
    TT_UT_EQUAL(n, g1, "");

    // valid path
    n = tt_cfgpath_p2n_cstr(root, g12, "./../../g1");
    TT_UT_EQUAL(n, g1, "");

    // valid path
    n = tt_cfgpath_p2n_cstr(root, c1211, "/g1/g11");
    TT_UT_EQUAL(n, g11, "");

    // invalid path
    n = tt_cfgpath_p2n_cstr(root, c1211, "//g1/g11");
    TT_UT_EQUAL(n, NULL, "");

    // invalid path
    n = tt_cfgpath_p2n_cstr(root, g12, "./g121/c1211/...");
    TT_UT_EQUAL(n, NULL, "");

    // valid path
    n = tt_cfgpath_p2n_cstr(root, g121, "../../../g1/g12/g121/c1211");
    TT_UT_EQUAL(n, c1211, "");

    // valid path
    n = tt_cfgpath_p2n_cstr(root, g121, "../../../g1/g12/g121/c1211");
    TT_UT_EQUAL(n, c1211, "");

    // invalid path
    n = tt_cfgpath_p2n_cstr(
        root, g121, "../../../.../g1/g12/g121/c1211/.../g1/g12/g121/c1211");
    TT_UT_EQUAL(n, NULL, "");

    // valid path
    n = tt_cfgpath_p2n_cstr(root,
                            g121,
                            "../../../g1/g12/g121/../../g12/g121/c1211");
    TT_UT_EQUAL(n, c1211, "");

    // valid path
    n = tt_cfgpath_p2n_cstr(root, g121, "../../../g1/g12/g121/../../../");
    TT_UT_EQUAL(n, root, "");

    tt_cfgobj_destroy(root);
    tt_string_destroy(&path);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_cfgpath_n2p)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_cfgobj_t *root, *g1, *g11, *g12, *g121, *c1211;
    tt_u32_t val = 0;
    tt_buf_t path;
    tt_result_t ret;
    tt_s32_t cmp_ret;

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&path, NULL);

    root = tt_cfgdir_create("", NULL);
    TT_UT_NOT_EQUAL(root, NULL, "");
    g1 = tt_cfgdir_create("g1", NULL);
    TT_UT_NOT_EQUAL(g1, NULL, "");
    g11 = tt_cfgdir_create("g11", NULL);
    TT_UT_NOT_EQUAL(g11, NULL, "");
    g12 = tt_cfgdir_create("g12", NULL);
    TT_UT_NOT_EQUAL(g12, NULL, "");
    g121 = tt_cfgdir_create("g121", NULL);
    TT_UT_NOT_EQUAL(g121, NULL, "");
    c1211 = tt_cfgu32_create("c1211", &val, NULL, NULL);
    TT_UT_NOT_EQUAL(g121, NULL, "");

    tt_cfgdir_add(TT_CFGOBJ_CAST(root, tt_cfgdir_t), g1);
    tt_cfgdir_add(TT_CFGOBJ_CAST(g1, tt_cfgdir_t), g11);
    tt_cfgdir_add(TT_CFGOBJ_CAST(g1, tt_cfgdir_t), g12);
    tt_cfgdir_add(TT_CFGOBJ_CAST(g12, tt_cfgdir_t), g121);
    tt_cfgdir_add(TT_CFGOBJ_CAST(g121, tt_cfgdir_t), c1211);

    tt_buf_clear(&path);
    ret = tt_cfgpath_n2p(root, root, &path);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&path, "");
    TT_UT_EQUAL(cmp_ret, 0, "");

    tt_buf_clear(&path);
    ret = tt_cfgpath_n2p(root, g1, &path);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&path, "g1");
    TT_UT_EQUAL(cmp_ret, 0, "");

    tt_buf_clear(&path);
    ret = tt_cfgpath_n2p(root, g121, &path);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&path, "g1/g12/g121");
    TT_UT_EQUAL(cmp_ret, 0, "");

    tt_buf_clear(&path);
    ret = tt_cfgpath_n2p(root, c1211, &path);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&path, "g1/g12/g121/c1211");
    TT_UT_EQUAL(cmp_ret, 0, "");

    // not from root
    tt_buf_clear(&path);
    ret = tt_cfgpath_n2p(g1, c1211, &path);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&path, "g12/g121/c1211");
    TT_UT_EQUAL(cmp_ret, 0, "");

    tt_buf_clear(&path);
    ret = tt_cfgpath_n2p(g1, g121, &path);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&path, "g12/g121");
    TT_UT_EQUAL(cmp_ret, 0, "");

    tt_buf_clear(&path);
    ret = tt_cfgpath_n2p(g1, g1, &path);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&path, "");
    TT_UT_EQUAL(cmp_ret, 0, "");

    tt_buf_clear(&path);
    ret = tt_cfgpath_n2p(g12, g12, &path);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&path, "");
    TT_UT_EQUAL(cmp_ret, 0, "");

    // not real root
    tt_buf_clear(&path);
    ret = tt_cfgpath_n2p(g11, g121, &path);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&path, "/g1/g12/g121");
    TT_UT_EQUAL(cmp_ret, 0, "");

    tt_buf_clear(&path);
    ret = tt_cfgpath_n2p(NULL, c1211, &path);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&path, "/g1/g12/g121/c1211");
    TT_UT_EQUAL(cmp_ret, 0, "");

    tt_buf_clear(&path);
    ret = tt_cfgpath_n2p(c1211, g121, &path);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    cmp_ret = tt_buf_cmp_cstr(&path, "/g1/g12/g121");
    TT_UT_EQUAL(cmp_ret, 0, "");

    tt_cfgobj_destroy(root);
    tt_buf_destroy(&path);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_cfgpath_comp)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_cfgobj_t *root, *g1, *g11, *g12, *g121, *c1211;
    tt_u32_t val = 0, status;
    tt_buf_t output;
    tt_result_t ret;
    tt_s32_t cmp_ret;

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&output, NULL);

    root = tt_cfgdir_create("", NULL);
    TT_UT_NOT_EQUAL(root, NULL, "");
    g1 = tt_cfgdir_create("g1", NULL);
    TT_UT_NOT_EQUAL(g1, NULL, "");
    g11 = tt_cfgdir_create("g11", NULL);
    TT_UT_NOT_EQUAL(g11, NULL, "");
    g12 = tt_cfgdir_create("g12", NULL);
    TT_UT_NOT_EQUAL(g12, NULL, "");
    g121 = tt_cfgdir_create("g121", NULL);
    TT_UT_NOT_EQUAL(g121, NULL, "");
    c1211 = tt_cfgu32_create("c1211", &val, NULL, NULL);
    TT_UT_NOT_EQUAL(g121, NULL, "");

    tt_cfgdir_add(TT_CFGOBJ_CAST(root, tt_cfgdir_t), g1);
    tt_cfgdir_add(TT_CFGOBJ_CAST(g1, tt_cfgdir_t), g11);
    tt_cfgdir_add(TT_CFGOBJ_CAST(g1, tt_cfgdir_t), g12);
    tt_cfgdir_add(TT_CFGOBJ_CAST(g12, tt_cfgdir_t), g121);
    tt_cfgdir_add(TT_CFGOBJ_CAST(g121, tt_cfgdir_t), c1211);

    //////////////////////////////////////////////////////

    {
        tt_blob_t path = {(tt_u8_t *)"", 0};
        path.len = (tt_u32_t)tt_strlen((tt_char_t *)path.addr);
        tt_buf_clear(&output);
        ret = tt_cfgpath_complete(root,
                                  root,
                                  (tt_char_t *)path.addr,
                                  path.len,
                                  &status,
                                  &output);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(status, TT_CFGPCP_FULL_MORE, "");

        // list all child
        TT_UT_EQUAL(TT_BUF_RLEN(&output), 3, "");
        cmp_ret = tt_strncmp((tt_char_t *)TT_BUF_RPOS(&output), "g1/", 3);
        TT_UT_EQUAL(cmp_ret, 0, "");

        ret = tt_cfgpath_complete(root,
                                  c1211,
                                  (tt_char_t *)path.addr,
                                  path.len,
                                  &status,
                                  &output);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(status, TT_CFGPCP_NONE, "");
    }

    {
        tt_blob_t path = {(tt_u8_t *)"g1", 0};
        path.len = (tt_u32_t)tt_strlen((tt_char_t *)path.addr);
        tt_buf_clear(&output);
        ret = tt_cfgpath_complete(root,
                                  root,
                                  (tt_char_t *)path.addr,
                                  path.len,
                                  &status,
                                  &output);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(status, TT_CFGPCP_FULL_MORE, "");

        // complete a slash
        TT_UT_EQUAL(TT_BUF_RLEN(&output), 1, "");
        cmp_ret = tt_strncmp((tt_char_t *)TT_BUF_RPOS(&output), "/", 1);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    {
        tt_blob_t path = {(tt_u8_t *)"g1", 0};
        path.len = (tt_u32_t)tt_strlen((tt_char_t *)path.addr);
        tt_buf_clear(&output);
        ret = tt_cfgpath_complete(root,
                                  g1,
                                  (tt_char_t *)path.addr,
                                  path.len,
                                  &status,
                                  &output);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(status, TT_CFGPCP_NONE, "");

        // list all
        TT_UT_EQUAL(TT_BUF_RLEN(&output), 7, "");
        cmp_ret = tt_strncmp((tt_char_t *)TT_BUF_RPOS(&output), "g11 g12", 7);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    {
        tt_blob_t path = {(tt_u8_t *)"g", 0};
        path.len = (tt_u32_t)tt_strlen((tt_char_t *)path.addr);
        tt_buf_clear(&output);
        ret = tt_cfgpath_complete(root,
                                  g1,
                                  (tt_char_t *)path.addr,
                                  path.len,
                                  &status,
                                  &output);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(status, TT_CFGPCP_PARTIAL, "");

        // partial complete
        TT_UT_EQUAL(TT_BUF_RLEN(&output), 1, "");
        cmp_ret = tt_strncmp((tt_char_t *)TT_BUF_RPOS(&output), "1", 1);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    {
        tt_blob_t path = {(tt_u8_t *)"/g1/g12/g121/c12", 0};
        path.len = (tt_u32_t)tt_strlen((tt_char_t *)path.addr);
        tt_buf_clear(&output);
        ret = tt_cfgpath_complete(root,
                                  g121,
                                  (tt_char_t *)path.addr,
                                  path.len,
                                  &status,
                                  &output);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(status, TT_CFGPCP_FULL, "");

        // full complete
        TT_UT_EQUAL(TT_BUF_RLEN(&output), 2, "");
        cmp_ret = tt_strncmp((tt_char_t *)TT_BUF_RPOS(&output), "11", 1);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // case1.1
    {
        tt_blob_t path = {(tt_u8_t *)"/g1/g12/g121/", 0};
        path.len = (tt_u32_t)tt_strlen((tt_char_t *)path.addr);
        tt_buf_clear(&output);
        ret = tt_cfgpath_complete(root,
                                  g121,
                                  (tt_char_t *)path.addr,
                                  path.len,
                                  &status,
                                  &output);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(status, TT_CFGPCP_FULL, "");

        // full complete
        TT_UT_EQUAL(TT_BUF_RLEN(&output), 5, "");
        cmp_ret = tt_strncmp((tt_char_t *)TT_BUF_RPOS(&output), "c1211", 1);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // case1.2
    {
        tt_blob_t path = {(tt_u8_t *)"/g1/g12/", 0};
        path.len = (tt_u32_t)tt_strlen((tt_char_t *)path.addr);
        tt_buf_clear(&output);
        ret = tt_cfgpath_complete(root,
                                  g121,
                                  (tt_char_t *)path.addr,
                                  path.len,
                                  &status,
                                  &output);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(status, TT_CFGPCP_FULL_MORE, "");

        // full complete
        TT_UT_EQUAL(TT_BUF_RLEN(&output), 5, "");
        cmp_ret = tt_strncmp((tt_char_t *)TT_BUF_RPOS(&output), "g121/", 1);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    // case3
    {
        tt_blob_t path = {(tt_u8_t *)"g12", 0};
        path.len = (tt_u32_t)tt_strlen((tt_char_t *)path.addr);
        tt_buf_clear(&output);
        ret = tt_cfgpath_complete(root,
                                  g12,
                                  (tt_char_t *)path.addr,
                                  path.len,
                                  &status,
                                  &output);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(status, TT_CFGPCP_FULL_MORE, "");

        // full complete
        TT_UT_EQUAL(TT_BUF_RLEN(&output), 2, "");
        cmp_ret = tt_strncmp((tt_char_t *)TT_BUF_RPOS(&output), "1/", 1);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    {
        tt_blob_t path = {(tt_u8_t *)".", 0};
        path.len = (tt_u32_t)tt_strlen((tt_char_t *)path.addr);
        tt_buf_clear(&output);
        ret = tt_cfgpath_complete(root,
                                  g1,
                                  (tt_char_t *)path.addr,
                                  path.len,
                                  &status,
                                  &output);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(status, TT_CFGPCP_FULL_MORE, "");

        // complete a slash
        TT_UT_EQUAL(TT_BUF_RLEN(&output), 1, "");
        cmp_ret = tt_strncmp((tt_char_t *)TT_BUF_RPOS(&output), "/", 1);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    {
        tt_blob_t path = {(tt_u8_t *)"/g1/g12/..", 0};
        path.len = (tt_u32_t)tt_strlen((tt_char_t *)path.addr);
        tt_buf_clear(&output);
        ret = tt_cfgpath_complete(root,
                                  g121,
                                  (tt_char_t *)path.addr,
                                  path.len,
                                  &status,
                                  &output);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(status, TT_CFGPCP_FULL_MORE, "");

        // complete a slash
        TT_UT_EQUAL(TT_BUF_RLEN(&output), 1, "");
        cmp_ret = tt_strncmp((tt_char_t *)TT_BUF_RPOS(&output), "/", 1);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    {
        tt_blob_t path = {(tt_u8_t *)"/g1/g12/...", 0};
        path.len = (tt_u32_t)tt_strlen((tt_char_t *)path.addr);
        tt_buf_clear(&output);
        ret = tt_cfgpath_complete(root,
                                  g121,
                                  (tt_char_t *)path.addr,
                                  path.len,
                                  &status,
                                  &output);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(status, TT_CFGPCP_NONE, "");

        // none match
        TT_UT_EQUAL(TT_BUF_RLEN(&output), 0, "");
    }

    {
        // invalid path
        tt_blob_t path = {(tt_u8_t *)"/g1/g12/x121/c12", 0};
        path.len = (tt_u32_t)tt_strlen((tt_char_t *)path.addr);
        tt_buf_clear(&output);
        ret = tt_cfgpath_complete(root,
                                  g121,
                                  (tt_char_t *)path.addr,
                                  path.len,
                                  &status,
                                  &output);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(status, TT_CFGPCP_NONE, "");

        TT_UT_EQUAL(TT_BUF_RLEN(&output), 0, "");
    }

    {
        // invalid path
        tt_blob_t path = {(tt_u8_t *)"/g1/g12/g121/c1211/", 0};
        path.len = (tt_u32_t)tt_strlen((tt_char_t *)path.addr);
        tt_buf_clear(&output);
        ret = tt_cfgpath_complete(root,
                                  g121,
                                  (tt_char_t *)path.addr,
                                  path.len,
                                  &status,
                                  &output);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(status, TT_CFGPCP_NONE, "");

        TT_UT_EQUAL(TT_BUF_RLEN(&output), 0, "");
    }

    {
        // invalid path
        tt_blob_t path = {(tt_u8_t *)"/g1/g12/g121/c1299", 0};
        path.len = (tt_u32_t)tt_strlen((tt_char_t *)path.addr);
        tt_buf_clear(&output);
        ret = tt_cfgpath_complete(root,
                                  g121,
                                  (tt_char_t *)path.addr,
                                  path.len,
                                  &status,
                                  &output);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(status, TT_CFGPCP_NONE, "");

        TT_UT_EQUAL(TT_BUF_RLEN(&output), 0, "");
    }

    // no input, but only 1 child value
    {
        tt_blob_t path = {(tt_u8_t *)"", 0};
        path.len = (tt_u32_t)tt_strlen((tt_char_t *)path.addr);
        tt_buf_clear(&output);
        ret = tt_cfgpath_complete(root,
                                  g121,
                                  (tt_char_t *)path.addr,
                                  path.len,
                                  &status,
                                  &output);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(status, TT_CFGPCP_FULL, "");

        // list all child
        TT_UT_EQUAL(TT_BUF_RLEN(&output), 5, "");
        cmp_ret = tt_strncmp((tt_char_t *)TT_BUF_RPOS(&output), "c1211", 5);
        TT_UT_EQUAL(cmp_ret, 0, "");
    }

    //////////////////////////////////////////////////////

    tt_cfgobj_destroy(root);

    // test end
    TT_TEST_CASE_LEAVE()
}
