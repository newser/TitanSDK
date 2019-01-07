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

#include <param/html/bootstrap4/tt_param_bs4_content.h>
#include <param/html/bootstrap4/tt_param_bs4_control_render.h>
#include <param/html/bootstrap4/tt_param_bs4_nav.h>
#include <param/html/bootstrap4/tt_param_bs4_page.h>
#include <param/html/bootstrap4/tt_param_bs4_sidebar.h>

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
TT_TEST_ROUTINE_DECLARE(case_param_html_bs4_nav)
TT_TEST_ROUTINE_DECLARE(case_param_html_bs4_sidebar)
TT_TEST_ROUTINE_DECLARE(case_param_html_bs4_content)
TT_TEST_ROUTINE_DECLARE(case_param_html_bs4_page)
TT_TEST_ROUTINE_DECLARE(case_param_html_bs4_ctrl)
TT_TEST_ROUTINE_DECLARE(case_param_html_bs4_ctrl_render)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(param_html_case)

TT_TEST_CASE("case_param_html_bs4_nav",
             "param html: nav",
             case_param_html_bs4_nav,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("case_param_html_bs4_sidebar",
                 "param html: sidebar",
                 case_param_html_bs4_sidebar,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

#if 0
    TT_TEST_CASE("case_param_html_bs4_content",
                 "param html: content",
                 case_param_html_bs4_content,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),
#endif

    TT_TEST_CASE("case_param_html_bs4_page",
                 "param html: page",
                 case_param_html_bs4_page,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_param_html_bs4_ctrl",
                 "param html: contrl",
                 case_param_html_bs4_ctrl,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_param_html_bs4_ctrl_render",
                 "param html: contrl render",
                 case_param_html_bs4_ctrl_render,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

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
    TT_TEST_ROUTINE_DEFINE(case_param_html_bs4_ctrl_render)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(case_param_html_bs4_nav)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_param_bs4_nav_t h;
    tt_buf_t b;
    tt_param_t *pm, *c, *d2;
    tt_u32_t val_u32 = 199;

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&b, NULL);

    pm = tt_param_u32_create("test", NULL, NULL, NULL);
    TT_UT_NOT_NULL(pm, "");

    tt_param_bs4_nav_init(&h);

    tt_buf_clear(&b);
    TT_UT_SUCCESS(tt_param_bs4_nav_render(&h, NULL, NULL, pm, &b), "");
    TT_UT_TRUE(tt_buf_empty(&b), "");

    tt_param_destroy(pm);

    pm = tt_param_dir_create("Root", NULL);
    TT_UT_NOT_NULL(pm, "");
    d2 = tt_param_dir_create("Parent", NULL);
    TT_UT_NOT_NULL(d2, "");
    tt_param_dir_add(TT_PARAM_CAST(pm, tt_param_dir_t), d2);
    c = tt_param_dir_create("Child-1", NULL);
    tt_param_dir_add(TT_PARAM_CAST(d2, tt_param_dir_t), c);
    c = tt_param_u32_create("Child-2", &val_u32, NULL, NULL);
    tt_param_dir_add(TT_PARAM_CAST(d2, tt_param_dir_t), c);
    c = tt_param_dir_create("Child-3", NULL);
    tt_param_dir_add(TT_PARAM_CAST(d2, tt_param_dir_t), c);

#define __s1_def                                                               \
    "<nav class=\"d-flex flex-md-row-reverse navbar navbar-expand-md w-100 "   \
    "navbar-dark bg-dark\">"                                                   \
    "<a class=\"btn btn-success\" href=\"#\">Admin</a>"                        \
    "<button class=\"navbar-toggler\" type=\"button\" "                        \
    "data-toggle=\"collapse\" data-target=\"#collapseTarget\">"                \
    "<span class=\"navbar-toggler-icon\"></span>"                              \
    "</button>"                                                                \
    "<div class=\"collapse navbar-collapse\" id=\"collapseTarget\">"           \
    "<ul class=\"navbar-nav\">"                                                \
    "<li class=\"nav-item\">"                                                  \
    "<a class=\"nav-link\" href=\"Parent/Child-1\">Child-1</a>"                \
    "</li>"                                                                    \
    "<li class=\"nav-item\">"                                                  \
    "<a class=\"nav-link active\" href=\"Parent/Child-3\">Child-3</a>"         \
    "</li>"                                                                    \
    "</ul>"                                                                    \
    "</div>"                                                                   \
    "</nav>"
    tt_buf_clear(&b);
    TT_UT_SUCCESS(tt_param_bs4_nav_render(&h, pm, d2, c, &b), "");
    TT_UT_EQUAL(tt_buf_cmp_cstr(&b, __s1_def), 0, "");

#define __s2_nondef                                                            \
    "<nav class=\"d-flex flex-md-row-reverse navbar navbar-expand-md w-100 "   \
    "11\">"                                                                    \
    "<a class=\"btn 22\" href=\"33\">44</a>"                                   \
    "<button class=\"navbar-toggler\" type=\"button\" "                        \
    "data-toggle=\"collapse\" data-target=\"#collapseTarget\">"                \
    "<span class=\"navbar-toggler-icon\"></span>"                              \
    "</button>"                                                                \
    "<div class=\"collapse navbar-collapse\" id=\"collapseTarget\">"           \
    "<ul class=\"navbar-nav\">"                                                \
    "<a class=\"navbar-brand 55\" href=\"66\">77</a>"                          \
    "<li class=\"nav-item\">"                                                  \
    "<a class=\"nav-link\" href=\"Root/Parent/Child-1\">Child-1</a>"           \
    "</li>"                                                                    \
    "<li class=\"nav-item\">"                                                  \
    "<a class=\"nav-link active\" href=\"Root/Parent/Child-3\">Child-3</a>"    \
    "</li>"                                                                    \
    "</ul>"                                                                    \
    "</div>"                                                                   \
    "</nav>"
    tt_buf_clear(&b);
    h.nav_class = "11";
    h.account_class = "22";
    h.account_href = "33";
    h.account_text = "44";
    h.brand_class = "55";
    h.brand_href = "66";
    h.brand_text = "77";
    TT_UT_SUCCESS(tt_param_bs4_nav_render(&h, NULL, d2, c, &b), "");
    TT_UT_EQUAL(tt_buf_cmp_cstr(&b, __s2_nondef), 0, "");

    tt_buf_destroy(&b);
    tt_param_destroy(pm);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_param_html_bs4_sidebar)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_param_bs4_sidebar_t h;
    tt_buf_t b;
    tt_param_t *pm, *c, *d2;

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&b, NULL);

    pm = tt_param_u32_create("test", NULL, NULL, NULL);
    TT_UT_NOT_NULL(pm, "");

    tt_param_bs4_sidebar_init(&h);

    // non-dir
    tt_buf_clear(&b);
    TT_UT_SUCCESS(tt_param_bs4_sidebar_render(&h, NULL, pm, &b), "");
    TT_UT_TRUE(tt_buf_empty(&b), "");

    // empty dir
    tt_param_destroy(pm);
    pm = tt_param_dir_create("te", NULL);
    tt_buf_clear(&b);
    TT_UT_SUCCESS(tt_param_bs4_sidebar_render(&h, NULL, pm, &b), "");
    TT_UT_TRUE(tt_buf_empty(&b), "");

    // dir with non-dir child
    c = tt_param_u32_create("xxx", NULL, NULL, NULL);
    tt_param_dir_add(TT_PARAM_CAST(pm, tt_param_dir_t), c);
    tt_buf_clear(&b);
    TT_UT_SUCCESS(tt_param_bs4_sidebar_render(&h, NULL, pm, &b), "");
    TT_UT_TRUE(tt_buf_empty(&b), "");

#define __t1_2child                                                            \
    "<nav class=\"d-none d-md-block col-2 navbar bg-light \">"                 \
    "<ul class=\"navbar-nav\">"                                                \
    "<li class=\"nav-item\">"                                                  \
    "<a class=\"nav-link text-dark\" href=\"#aaa\">aaa</a>"                    \
    "</li>"                                                                    \
    "</ul>"                                                                    \
    "</nav>"
    c = tt_param_u32_create("uuu", NULL, NULL, NULL);
    tt_param_dir_add(TT_PARAM_CAST(pm, tt_param_dir_t), c);
    c = tt_param_dir_create("aaa", NULL);
    tt_param_dir_add(TT_PARAM_CAST(pm, tt_param_dir_t), c);
    tt_buf_clear(&b);
    TT_UT_SUCCESS(tt_param_bs4_sidebar_render(&h, NULL, pm, &b), "");
    TT_UT_EQUAL(tt_buf_cmp_cstr(&b, __t1_2child), 0, "");

#define __t1_2child_cus                                                        \
    "<nav class=\"d-none d-md-block col-2 navbar bg-light "                    \
    "111\">"                                                                   \
    "<ul class=\"navbar-nav\">"                                                \
    "<li class=\"nav-item\">"                                                  \
    "<a class=\"nav-link text-dark\" href=\"#aaa\">aaa</a>"                    \
    "</li>"                                                                    \
    "<li class=\"nav-item\">"                                                  \
    "<a class=\"nav-link text-dark\" href=\"#bbb\">bbb</a>"                    \
    "</li>"                                                                    \
    "</ul>"                                                                    \
    "</nav>"
    h.nav_class = "111";
    tt_buf_clear(&b);
    c = tt_param_dir_create("bbb", NULL);
    tt_param_dir_add(TT_PARAM_CAST(pm, tt_param_dir_t), c);
    TT_UT_SUCCESS(tt_param_bs4_sidebar_render(&h, NULL, pm, &b), "");
    TT_UT_EQUAL(tt_buf_cmp_cstr(&b, __t1_2child_cus), 0, "");

    tt_buf_destroy(&b);
    tt_param_destroy(pm);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_param_html_bs4_content)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_param_bs4_content_t h;
    tt_buf_t b;
    tt_param_t *pm, *c, *d2;
    tt_u32_t val_u32 = 1024;
    tt_s32_t val_s32 = 65536;
    tt_float_t val_float = 3.1415926;
    tt_string_t s;
    tt_param_bs4_level_t lv = TT_PARAM_BS4_LV_ADMIN;

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&b, NULL);
    tt_string_init(&s, NULL);
    tt_string_set(&s, "Ad leggings keytar, brunch id art party dolor labore.");

    pm = tt_param_u32_create("test", NULL, NULL, NULL);
    TT_UT_NOT_NULL(pm, "");

    tt_param_bs4_content_init(&h);

    // non-dir
    tt_buf_clear(&b);
    TT_UT_SUCCESS(tt_param_bs4_content_render(&h, pm, lv, &b), "");
    TT_UT_TRUE(tt_buf_empty(&b), "");

// empty dir
#define __c1_empty "<div class=\"col-md-10 px-4\"></div>"
    tt_param_destroy(pm);
    pm = tt_param_dir_create("te", NULL);
    tt_buf_clear(&b);
    TT_UT_SUCCESS(tt_param_bs4_content_render(&h, pm, lv, &b), "");
    TT_UT_EQUAL(tt_buf_cmp_cstr(&b, __c1_empty), 0, "");

#define __c2_1val                                                              \
    "<div class=\"col-md-10 px-4\">"                                           \
    "<div class=\"container-fluid border-bottom p-4  \" id=\"te\">"            \
    "<div class=\"row h5 \">"                                                  \
    "<label class=\"col-12\">te</label>"                                       \
    "</div>"                                                                   \
    "<div class=\"row\">"                                                      \
    "<label class=\"col-4 col-md-2 text-right pr-0 \" id=\"u32\">u32 "         \
    ":</label>"                                                                \
    "<label class=\"col-8 col-md-10 \">1024</label>"                           \
    "</div></div></div>"
    c = tt_param_u32_create("u32", &val_u32, NULL, NULL);
    tt_param_dir_add(TT_PARAM_CAST(pm, tt_param_dir_t), c);
    tt_buf_clear(&b);
    TT_UT_SUCCESS(tt_param_bs4_content_render(&h, pm, lv, &b), "");
    TT_UT_EQUAL(tt_buf_cmp_cstr(&b, __c2_1val), 0, "");

#define __c3_2val                                                              \
    "<div class=\"col-md-10 px-4\">"                                           \
    "<div class=\"container-fluid border-bottom p-4 11 22\" id=\"te\">"        \
    "<div class=\"row h5 44\">"                                                \
    "<label class=\"col-12\">te</label>"                                       \
    "</div>"                                                                   \
    "<div class=\"row\">"                                                      \
    "<label class=\"col-4 col-md-2 text-right pr-0 55\" id=\"u32\">u32 "       \
    ":</label>"                                                                \
    "<label class=\"col-8 col-md-10 66\">1024</label>"                         \
    "</div>"                                                                   \
    "<div class=\"row\">"                                                      \
    "<label class=\"col-4 col-md-2 text-right pr-0 55\" "                      \
    "id=\"u32-follow-str\">u32-follow-str :</label>"                           \
    "<label class=\"col-8 col-md-10 66\">Ad leggings keytar, brunch id art "   \
    "party dolor labore.</label>"                                              \
    "</div></div></div>"
    c = tt_param_str_create("u32-follow-str", &s, NULL, NULL);
    tt_param_dir_add(TT_PARAM_CAST(pm, tt_param_dir_t), c);
    tt_buf_clear(&b);
    h.group_class = "11";
    h.group0_class = "22";
    h.group1_class = "33";
    h.title_class = "44";
    h.name_class = "55";
    h.val_class = "66";
    TT_UT_SUCCESS(tt_param_bs4_content_render(&h, pm, lv, &b), "");
    TT_UT_EQUAL(tt_buf_cmp_cstr(&b, __c3_2val), 0, "");

    tt_param_bs4_content_init(&h);

#define __c4_xx                                                                \
    "<div class=\"col-md-10 px-4\"><div class=\"container-fluid "              \
    "border-bottom "                                                           \
    "p-4  \" id=\"te\"><div class=\"row h5 \"><label "                         \
    "class=\"col-12\">te</label></div><div class=\"row\"><label "              \
    "class=\"col-4 col-md-2 text-right pr-0 \" id=\"u32\">u32 "                \
    ":</label><label class=\"col-8 col-md-10 \">1024</label></div><div "       \
    "class=\"row\"><label class=\"col-4 col-md-2 text-right pr-0 \" "          \
    "id=\"u32-follow-str\">u32-follow-str :</label><label class=\"col-8 "      \
    "col-md-10 \">Ad leggings keytar, brunch id art party dolor "              \
    "labore.</label></div><div class=\"row\"><label class=\"col-4 col-md-2 "   \
    "text-right pr-0 \" id=\"x-last-s32-1\">x-last-s32-1 :</label><label "     \
    "class=\"col-8 col-md-4 \">65536</label><label class=\"col-4 col-md-2 "    \
    "text-right pr-0 \" id=\"x-last-s32-2\">x-last-s32-2 :</label><label "     \
    "class=\"col-8 col-md-4 \">65536</label></div></div></div>"
    c = tt_param_s32_create("x-last-s32-1", &val_s32, NULL, NULL);
    tt_param_dir_add(TT_PARAM_CAST(pm, tt_param_dir_t), c);
    c = tt_param_s32_create("x-last-s32-2", &val_s32, NULL, NULL);
    tt_param_dir_add(TT_PARAM_CAST(pm, tt_param_dir_t), c);
    tt_buf_clear(&b);
    TT_UT_SUCCESS(tt_param_bs4_content_render(&h, pm, lv, &b), "");
    TT_UT_EQUAL(tt_buf_cmp_cstr(&b, __c4_xx), 0, "");

    d2 = tt_param_dir_create("the-second-directory", NULL);
    c = tt_param_s32_create("x-last-s32-1", &val_s32, NULL, NULL);
    tt_param_dir_add(TT_PARAM_CAST(d2, tt_param_dir_t), c);
    c = tt_param_s32_create("x-last-s32-2", &val_s32, NULL, NULL);
    tt_param_dir_add(TT_PARAM_CAST(d2, tt_param_dir_t), c);
    tt_param_dir_add(TT_PARAM_CAST(pm, tt_param_dir_t), d2);
    tt_buf_clear(&b);
    TT_UT_SUCCESS(tt_param_bs4_content_render(&h, pm, lv, &b), "");
// TT_UT_EQUAL(tt_buf_cmp_cstr(&b, __c4_xx), 0, "");

#define __c5_2dir                                                              \
    "<div class=\"col-md-10 px-4\"><div class=\"container-fluid "              \
    "border-bottom "                                                           \
    "p-4  \" id=\"te\"><div class=\"row h5 \"><label "                         \
    "class=\"col-12\">te</label></div><div class=\"row\"><label "              \
    "class=\"col-4 col-md-2 text-right pr-0 \" id=\"u32\">u32 "                \
    ":</label><label class=\"col-8 col-md-10 \">1024</label></div><div "       \
    "class=\"row\"><label class=\"col-4 col-md-2 text-right pr-0 \" "          \
    "id=\"u32-follow-str\">u32-follow-str :</label><label class=\"col-8 "      \
    "col-md-10 \">Ad leggings keytar, brunch id art party dolor "              \
    "labore.</label></div><div class=\"row\"><label class=\"col-4 col-md-2 "   \
    "text-right pr-0 \" id=\"x-last-s32-1\">x-last-s32-1 :</label><label "     \
    "class=\"col-8 col-md-4 \">65536</label><label class=\"col-4 col-md-2 "    \
    "text-right pr-0 \" id=\"x-last-s32-2\">x-last-s32-2 :</label><label "     \
    "class=\"col-8 col-md-4 \">65536</label></div></div><div "                 \
    "class=\"container-fluid border-bottom p-4  \" "                           \
    "id=\"the-second-directory\"><div class=\"row h5 \"><label "               \
    "class=\"col-12\">the-second-directory</label></div><div "                 \
    "class=\"row\"><label class=\"col-4 col-md-2 text-right pr-0 \" "          \
    "id=\"x-last-s32-1\">x-last-s32-1 :</label><label class=\"col-8 col-md-4 " \
    "\">65536</label><label class=\"col-4 col-md-2 text-right pr-0 \" "        \
    "id=\"x-last-s32-2\">x-last-s32-2 :</label><label class=\"col-8 col-md-4 " \
    "\">65536</label></div></div><div class=\"container-fluid border-bottom "  \
    "p-4  \" id=\"the-third-directory\"><div class=\"row h5 "                  \
    "\"><label class=\"col-12\">the-third-directory</label></div><div "        \
    "class=\"row\"><label class=\"col-4 col-md-2 text-right pr-0 \" "          \
    "id=\"x-last-s32-1\">x-last-s32-1 :</label><label class=\"col-8 col-md-4 " \
    "\">65536</label><label class=\"col-4 col-md-2 text-right pr-0 \" "        \
    "id=\"x-last-s32-2\">x-last-s32-2 :</label><label class=\"col-8 col-md-4 " \
    "\">65536</label></div></div></div>"
    d2 = tt_param_dir_create("the-third-directory", NULL);
    c = tt_param_s32_create("x-last-s32-1", &val_s32, NULL, NULL);
    tt_param_dir_add(TT_PARAM_CAST(d2, tt_param_dir_t), c);
    c = tt_param_s32_create("x-last-s32-2", &val_s32, NULL, NULL);
    tt_param_dir_add(TT_PARAM_CAST(d2, tt_param_dir_t), c);
    tt_param_dir_add(TT_PARAM_CAST(pm, tt_param_dir_t), d2);
    tt_buf_clear(&b);
    TT_UT_SUCCESS(tt_param_bs4_content_render(&h, pm, lv, &b), "");
    TT_UT_EQUAL(tt_buf_cmp_cstr(&b, __c5_2dir), 0, "");

    tt_buf_destroy(&b);
    tt_param_destroy(pm);
    tt_string_destroy(&s);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_param_html_bs4_page)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_buf_t b;
    tt_param_t *pm, *p1, *p2, *c;
    tt_u32_t val_u32 = 100;
    tt_s32_t val_s32 = 101;
    tt_string_t val_str;
    tt_param_bs4_page_t pg;
    tt_bool_t t = TT_TRUE, f = TT_FALSE;

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&b, NULL);
    tt_string_init(&val_str, NULL);
    tt_string_set(&val_str, "this is a string variable");

    pm = tt_param_dir_create("root", NULL);
    p1 = tt_param_dir_create("Status", NULL);
    tt_param_dir_add(TT_PARAM_CAST(pm, tt_param_dir_t), p1);
    p2 = tt_param_dir_create("Platform", NULL);
    tt_param_dir_add(TT_PARAM_CAST(pm, tt_param_dir_t), p2);
    p2 = tt_param_dir_create("Http-Server", NULL);
    tt_param_dir_add(TT_PARAM_CAST(pm, tt_param_dir_t), p2);
    p2 = tt_param_dir_create("My-Application", NULL);
    tt_param_dir_add(TT_PARAM_CAST(pm, tt_param_dir_t), p2);

    p2 = tt_param_dir_create("Platform", NULL);
    tt_param_dir_add(TT_PARAM_CAST(p1, tt_param_dir_t), p2);
    c = tt_param_u32_create("Val-U32-Secure", &val_u32, NULL, NULL);
    c->level = TT_PARAM_LV_SECURE_INFO;
    tt_param_dir_add(TT_PARAM_CAST(p2, tt_param_dir_t), c);
    c = tt_param_s32_create("a-S32-var", &val_s32, NULL, NULL);
    tt_param_dir_add(TT_PARAM_CAST(p2, tt_param_dir_t), c);
    c = tt_param_str_create("Status", &val_str, NULL, NULL);
    tt_param_dir_add(TT_PARAM_CAST(p2, tt_param_dir_t), c);
    c = tt_param_bool_create("bool-val", &t, NULL, NULL);
    tt_param_dir_add(TT_PARAM_CAST(p2, tt_param_dir_t), c);
    c = tt_param_bool_create("bool-f", &f, NULL, NULL);
    tt_param_dir_add(TT_PARAM_CAST(p2, tt_param_dir_t), c);

    p2 = tt_param_dir_create("Http-Server", NULL);
    tt_param_dir_add(TT_PARAM_CAST(p1, tt_param_dir_t), p2);
    c = tt_param_u32_create("Val-U32", &val_u32, NULL, NULL);
    tt_param_dir_add(TT_PARAM_CAST(p2, tt_param_dir_t), c);
    c = tt_param_s32_create("a-S32-var", &val_s32, NULL, NULL);
    tt_param_dir_add(TT_PARAM_CAST(p2, tt_param_dir_t), c);
    c = tt_param_str_create("Status", &val_str, NULL, NULL);
    tt_param_dir_add(TT_PARAM_CAST(p2, tt_param_dir_t), c);

    p2 = tt_param_dir_create("My-Application", NULL);
    tt_param_dir_add(TT_PARAM_CAST(p1, tt_param_dir_t), p2);
    c = tt_param_u32_create("ValU32", &val_u32, NULL, NULL);
    tt_param_dir_add(TT_PARAM_CAST(p2, tt_param_dir_t), c);
    c = tt_param_s32_create("a-S32-var", &val_s32, NULL, NULL);
    tt_param_dir_add(TT_PARAM_CAST(p2, tt_param_dir_t), c);
    c = tt_param_str_create("Status", &val_str, NULL, NULL);
    c->bs4_ctrl.whole_line = TT_TRUE;
    tt_param_dir_add(TT_PARAM_CAST(p2, tt_param_dir_t), c);
    c = tt_param_bool_create("bool-f", &f, NULL, NULL);
    tt_param_dir_add(TT_PARAM_CAST(p2, tt_param_dir_t), c);
    c = tt_param_str_create("Status-2", &val_str, NULL, NULL);
    tt_param_bs4_ctrl_set_type(&c->bs4_ctrl, TT_PARAM_BS4_TEXTAREA);
    tt_param_bs4_textarea_set_rows(&c->bs4_ctrl.textarea, 2);
    tt_param_dir_add(TT_PARAM_CAST(p2, tt_param_dir_t), c);
    c = tt_param_str_create("Status-3", &val_str, NULL, NULL);
    c->bs4_ctrl.whole_line = TT_TRUE;
    tt_param_bs4_ctrl_set_type(&c->bs4_ctrl, TT_PARAM_BS4_TEXTAREA);
    tt_param_dir_add(TT_PARAM_CAST(p2, tt_param_dir_t), c);

    tt_param_bs4_page_init(&pg);
    tt_buf_clear(&b);
    TT_UT_SUCCESS(tt_param_bs4_page_render(&pg,
                                           pm,
                                           p1,
                                           TT_PARAM_BS4_LV_USER,
                                           &b),
                  "");

    tt_buf_clear(&b);
    TT_UT_SUCCESS(tt_param_bs4_page_render(&pg,
                                           pm,
                                           p1,
                                           TT_PARAM_BS4_LV_ADMIN,
                                           &b),
                  "");

    tt_buf_destroy(&b);
    tt_param_destroy(pm);
    tt_string_destroy(&val_str);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_param_html_bs4_ctrl)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_param_bs4_input_t bi;

    TT_TEST_CASE_ENTER()
    // test start

    tt_param_bs4_input_init(&bi);
    TT_UT_EQUAL(bi.pattern, NULL, "");
    TT_UT_EQUAL(bi.min[0], 0, "");
    TT_UT_EQUAL(bi.max[0], 0, "");
    TT_UT_EQUAL(bi.minlen[0], 0, "");
    TT_UT_EQUAL(bi.maxlen[0], 0, "");
    TT_UT_EQUAL(bi.step[0], 0, "");

    tt_param_bs4_input_set_pattern(&bi, "123");
    TT_UT_STREQ(bi.pattern, "123", "");
    tt_param_bs4_input_clear_pattern(&bi);
    TT_UT_EQUAL(bi.pattern, NULL, "");

    tt_param_bs4_input_set_minlen(&bi, 99999);
    TT_UT_STREQ(bi.minlen, "99999", "");
    tt_param_bs4_input_clear_minlen(&bi);
    TT_UT_EQUAL(bi.minlen[0], 0, "");

    tt_param_bs4_input_set_maxlen(&bi, 99999);
    TT_UT_STREQ(bi.maxlen, "99999", "");
    tt_param_bs4_input_clear_maxlen(&bi);
    TT_UT_EQUAL(bi.maxlen[0], 0, "");

    tt_param_bs4_input_set_min(&bi, 0x80000000);
    TT_UT_STREQ(bi.min, "-2147483648", "");
    tt_param_bs4_input_clear_min(&bi);
    TT_UT_EQUAL(bi.min[0], 0, "");

    tt_param_bs4_input_set_max(&bi, 0x7fffffff);
    TT_UT_STREQ(bi.max, "2147483647", "");
    tt_param_bs4_input_clear_max(&bi);
    TT_UT_EQUAL(bi.max[0], 0, "");

    tt_param_bs4_input_set_step(&bi, 5);
    TT_UT_STREQ(bi.step, "0.00001", "");
    tt_param_bs4_input_set_step(&bi, 1);
    TT_UT_STREQ(bi.step, "0.1", "");
    tt_param_bs4_input_set_step(&bi, 0);
    TT_UT_EQUAL(bi.step[0], 0, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_param_html_bs4_ctrl_render)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_param_t *p1, *p2;
    tt_u32_t val_u32 = 1;
    tt_s32_t val_s32 = 2;
    tt_param_bs4_input_t *i;
    tt_buf_t b;
    tt_param_bs4_content_t ct;
    const tt_char_t *s;

    TT_TEST_CASE_ENTER()
    // test start

    p1 = tt_param_s32_create("s32", &val_s32, NULL, NULL);
    TT_UT_NOT_NULL(p1, "");
    p2 = tt_param_u32_create("u32", &val_u32, NULL, NULL);
    TT_UT_NOT_NULL(p1, "");

    tt_param_bs4_content_init(&ct);
    tt_buf_init(&b, NULL);

    p1->bs4_ctrl.type = TT_PARAM_BS4_INPUT;
    i = &p1->bs4_ctrl.input;
    tt_param_bs4_input_set_pattern(i, "123|234");
    tt_param_bs4_input_set_min(i, 1);
    tt_param_bs4_input_set_max(i, 999);
    tt_param_bs4_input_set_minlen(i, 1);
    tt_param_bs4_input_set_maxlen(i, 1999);
    tt_param_bs4_input_set_step(i, 1);

    tt_buf_clear(&b);
    TT_UT_SUCCESS(tt_param_bs4_ctrl_render_pair(&ct,
                                                p1,
                                                p2,
                                                TT_PARAM_BS4_LV_ADMIN,
                                                &b),
                  "");
    tt_buf_put_u8(&b, 0);
    s = (tt_char_t *)TT_BUF_RPOS(&b);
    TT_UT_NOT_NULL(tt_strstr(s, "pattern=\"123|234\""), "");
    TT_UT_NOT_NULL(tt_strstr(s, "min=\"1\""), "");
    TT_UT_NOT_NULL(tt_strstr(s, "max=\"999\""), "");
    TT_UT_NOT_NULL(tt_strstr(s, "minlength=\"1\""), "");
    TT_UT_NOT_NULL(tt_strstr(s, "maxlength=\"1999\""), "");
    TT_UT_NOT_NULL(tt_strstr(s, "step=\"0.1\""), "");

    tt_buf_clear(&b);
    TT_UT_SUCCESS(tt_param_bs4_ctrl_render(&ct, p1, TT_PARAM_BS4_LV_ADMIN, &b),
                  "");
    tt_buf_put_u8(&b, 0);
    s = (tt_char_t *)TT_BUF_RPOS(&b);
    TT_UT_NOT_NULL(tt_strstr(s, "pattern=\"123|234\""), "");
    TT_UT_NOT_NULL(tt_strstr(s, "min=\"1\""), "");
    TT_UT_NOT_NULL(tt_strstr(s, "max=\"999\""), "");
    TT_UT_NOT_NULL(tt_strstr(s, "minlength=\"1\""), "");
    TT_UT_NOT_NULL(tt_strstr(s, "maxlength=\"1999\""), "");
    TT_UT_NOT_NULL(tt_strstr(s, "step=\"0.1\""), "");

    tt_buf_clear(&b);
    TT_UT_SUCCESS(tt_param_bs4_ctrl_render(&ct, p2, TT_PARAM_BS4_LV_ADMIN, &b),
                  NULL);
    tt_buf_put_u8(&b, 0);
    s = (tt_char_t *)TT_BUF_RPOS(&b);
    TT_UT_NULL(tt_strstr(s, "pattern="), "");
    TT_UT_NULL(tt_strstr(s, "min="), "");
    TT_UT_NULL(tt_strstr(s, "max="), "");
    TT_UT_NULL(tt_strstr(s, "minlength="), "");
    TT_UT_NULL(tt_strstr(s, "maxlength="), "");
    TT_UT_NULL(tt_strstr(s, "step="), "");

    {
        const tt_char_t *opt[3] = {"1", "22", "333"};
        tt_param_bs4_select_t *sl;

        p2->bs4_ctrl.type = TT_PARAM_BS4_SELECT;
        sl = &p2->bs4_ctrl.select;
        tt_param_bs4_select_set_selected(sl, "choose...");
        tt_param_bs4_select_set_option(sl, opt, 3);

        tt_buf_clear(&b);
        TT_UT_SUCCESS(tt_param_bs4_ctrl_render(&ct,
                                               p2,
                                               TT_PARAM_BS4_LV_ADMIN,
                                               &b),
                      NULL);
        tt_buf_put_u8(&b, 0);
        s = (tt_char_t *)TT_BUF_RPOS(&b);
        TT_UT_NOT_NULL(tt_strstr(s, "<select class=\"custom-select"), "");
        TT_UT_NOT_NULL(tt_strstr(s, "<option selected>choose..."), "");
        TT_UT_NOT_NULL(tt_strstr(s, "<option value=\"0\">1"), "");
        TT_UT_NOT_NULL(tt_strstr(s, "<option value=\"1\">22"), "");
        TT_UT_NOT_NULL(tt_strstr(s, "<option value=\"2\">333"), "");
    }

    {
        tt_param_bs4_textarea_t *ta;

        tt_param_bs4_ctrl_init(&p2->bs4_ctrl);
        p2->bs4_ctrl.type = TT_PARAM_BS4_TEXTAREA;
        ta = &p2->bs4_ctrl.textarea;

        tt_buf_clear(&b);
        TT_UT_SUCCESS(tt_param_bs4_ctrl_render(&ct,
                                               p2,
                                               TT_PARAM_BS4_LV_ADMIN,
                                               &b),
                      NULL);
        tt_buf_put_u8(&b, 0);
        s = (tt_char_t *)TT_BUF_RPOS(&b);
        TT_UT_NOT_NULL(tt_strstr(s, "<textarea class=\"form-control"), "");
        TT_UT_NULL(tt_strstr(s, "rows="), "");

        tt_param_bs4_textarea_set_rows(ta, 255);
        tt_buf_clear(&b);
        TT_UT_SUCCESS(tt_param_bs4_ctrl_render(&ct,
                                               p2,
                                               TT_PARAM_BS4_LV_ADMIN,
                                               &b),
                      NULL);
        tt_buf_put_u8(&b, 0);
        s = (tt_char_t *)TT_BUF_RPOS(&b);
        TT_UT_NOT_NULL(tt_strstr(s, "<textarea class=\"form-control"), "");
        TT_UT_NOT_NULL(tt_strstr(s, "rows=\"255\""), "");
    }

    tt_buf_destroy(&b);
    tt_param_destroy(p1);
    tt_param_destroy(p2);

    // test end
    TT_TEST_CASE_LEAVE()
}
