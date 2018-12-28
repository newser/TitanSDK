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

#include <math.h>

#include <param/html/tt_param_nav.h>

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
TT_TEST_ROUTINE_DECLARE(case_param_html_nav)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(param_html_case)

TT_TEST_CASE("case_param_html_nav",
             "param html: nav",
             case_param_html_nav,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE_LIST_DEFINE_END(param_html_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(PARAM_UT_HTML, 0, param_html_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(case_param_html_nav)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(case_param_html_nav)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_param_hnav_t h;
    tt_buf_t b;
    tt_param_t *pm, *c, *d2;

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&b, NULL);

    pm = tt_param_u32_create("test", NULL, NULL, NULL);
    TT_UT_NOT_NULL(pm, "");

    tt_param_hnav_init(&h);

#define __s1_no_disp                                                           \
    "<nav class=\"d-flex flex-md-row-reverse navbar navbar-expand-md w-100 "   \
    "\"><a class=\"btn \" href=\"\"></a><button class=\"navbar-toggler\" "     \
    "type=\"button\" data-toggle=\"collapse\" "                                \
    "data-target=\"#collapseTarget\"><span "                                   \
    "class=\"navbar-toggler-icon\"></span></button><div class=\"collapse "     \
    "navbar-collapse\" id=\"collapseTarget\"><ul class=\"navbar-nav\"><li "    \
    "class=\"nav-item\"><a class=\"nav-link\">test</a></li></ul></div></nav>"
    TT_UT_SUCCESS(tt_param_hnav_render(&h, NULL, pm, &b), "");
    TT_UT_EQUAL(tt_buf_cmp_cstr(&b, __s1_no_disp), 0, "");

#define __s2_disp                                                              \
    "<nav class=\"d-flex flex-md-row-reverse navbar navbar-expand-md w-100 "   \
    "\"><a class=\"btn \" href=\"\"></a><button class=\"navbar-toggler\" "     \
    "type=\"button\" data-toggle=\"collapse\" "                                \
    "data-target=\"#collapseTarget\"><span "                                   \
    "class=\"navbar-toggler-icon\"></span></button><div class=\"collapse "     \
    "navbar-collapse\" id=\"collapseTarget\"><ul class=\"navbar-nav\"><li "    \
    "class=\"nav-item\"><a class=\"nav-link\">ddd</a></li></ul></div></nav>"
    pm->display = "ddd";
    tt_buf_clear(&b);
    TT_UT_SUCCESS(tt_param_hnav_render(&h, NULL, pm, &b), "");
    TT_UT_EQUAL(tt_buf_cmp_cstr(&b, __s2_disp), 0, "");

    tt_param_destroy(pm);

    pm = tt_param_dir_create("dir1", NULL);
    TT_UT_NOT_NULL(pm, "");

#define __s3_empty_dir                                                         \
    "<nav class=\"d-flex flex-md-row-reverse navbar navbar-expand-md w-100 "   \
    "\"><a class=\"btn \" href=\"\"></a><button class=\"navbar-toggler\" "     \
    "type=\"button\" data-toggle=\"collapse\" "                                \
    "data-target=\"#collapseTarget\"><span "                                   \
    "class=\"navbar-toggler-icon\"></span></button><div class=\"collapse "     \
    "navbar-collapse\" id=\"collapseTarget\"><ul class=\"navbar-nav\"><li "    \
    "class=\"nav-item\"><a class=\"nav-link\">dir1</a></li></ul></div></nav>"
    tt_buf_clear(&b);
    TT_UT_SUCCESS(tt_param_hnav_render(&h, NULL, pm, &b), "");
    TT_UT_EQUAL(tt_buf_cmp_cstr(&b, __s3_empty_dir), 0, "");

#define __s4_1child                                                            \
    "<nav class=\"d-flex flex-md-row-reverse navbar navbar-expand-md w-100 "   \
    "\"><a class=\"btn \" href=\"\"></a><button class=\"navbar-toggler\" "     \
    "type=\"button\" data-toggle=\"collapse\" "                                \
    "data-target=\"#collapseTarget\"><span "                                   \
    "class=\"navbar-toggler-icon\"></span></button><div class=\"collapse "     \
    "navbar-collapse\" id=\"collapseTarget\"><ul class=\"navbar-nav\"><li "    \
    "class=\"nav-item\"><a class=\"nav-link\" "                                \
    "href=\"dir1/UUU\">UUU</a></li></ul></div></nav>"
    c = tt_param_u32_create("UUU", NULL, NULL, NULL);
    TT_UT_NOT_NULL(c, "");
    tt_param_dir_add(TT_PARAM_CAST(pm, tt_param_dir_t), c);
    tt_buf_clear(&b);
    TT_UT_SUCCESS(tt_param_hnav_render(&h, NULL, pm, &b), "");
    TT_UT_EQUAL(tt_buf_cmp_cstr(&b, __s4_1child), 0, "");

    d2 = tt_param_dir_create("DDD", NULL);
    tt_param_dir_add(TT_PARAM_CAST(pm, tt_param_dir_t), d2);
    c = tt_param_u32_create("1", NULL, NULL, NULL);
    TT_UT_NOT_NULL(c, "");
    tt_param_dir_add(TT_PARAM_CAST(d2, tt_param_dir_t), c);
    c = tt_param_u32_create("2", NULL, NULL, NULL);
    TT_UT_NOT_NULL(c, "");
    tt_param_dir_add(TT_PARAM_CAST(d2, tt_param_dir_t), c);

#define __s5_2child                                                            \
    "<nav class=\"d-flex flex-md-row-reverse navbar navbar-expand-md w-100 "   \
    "\"><a class=\"btn \" href=\"\"></a><button class=\"navbar-toggler\" "     \
    "type=\"button\" data-toggle=\"collapse\" "                                \
    "data-target=\"#collapseTarget\"><span "                                   \
    "class=\"navbar-toggler-icon\"></span></button><div class=\"collapse "     \
    "navbar-collapse\" id=\"collapseTarget\"><ul class=\"navbar-nav\"><li "    \
    "class=\"nav-item\"><a class=\"nav-link\" "                                \
    "href=\"dir1/DDD\">DDD</a></li><li class=\"nav-item\"><a "                 \
    "class=\"nav-link\" href=\"dir1/UUU\">UUU</a></li></ul></div></nav>"
    tt_buf_clear(&b);
    TT_UT_SUCCESS(tt_param_hnav_render(&h, NULL, pm, &b), "");
    TT_UT_EQUAL(tt_buf_cmp_cstr(&b, __s5_2child), 0, "");

#define __s6_customized                                                        \
    "<nav class=\"d-flex flex-md-row-reverse navbar navbar-expand-md w-100 "   \
    "navbar-dark bg-dark\"><a class=\"btn btn-success\" "                      \
    "href=\"admin/login\">hahah</a><button class=\"navbar-toggler\" "          \
    "type=\"button\" data-toggle=\"collapse\" "                                \
    "data-target=\"#collapseTarget\"><span "                                   \
    "class=\"navbar-toggler-icon\"></span></button><div class=\"collapse "     \
    "navbar-collapse\" id=\"collapseTarget\"><ul class=\"navbar-nav\"><li "    \
    "class=\"nav-item\"><a class=\"nav-link\" "                                \
    "href=\"dir1/DDD\">DDD</a></li><li class=\"nav-item\"><a "                 \
    "class=\"nav-link\" href=\"dir1/UUU\">UUU</a></li></ul></div></nav>"
    h.nav_class = "navbar-dark bg-dark";
    h.usr_class = "btn-success";
    h.usr_href = "admin/login";
    h.usr_text = "hahah";
    tt_buf_clear(&b);
    TT_UT_SUCCESS(tt_param_hnav_render(&h, NULL, pm, &b), "");
    TT_UT_EQUAL(tt_buf_cmp_cstr(&b, __s6_customized), 0, "");

    tt_buf_destroy(&b);
    tt_param_destroy(pm);

    // test end
    TT_TEST_CASE_LEAVE()
}
