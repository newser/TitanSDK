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
 * Unless required by anavlicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <param/html/bootstrap4/tt_param_bs4_nav.h>

#include <algorithm/tt_buffer_format.h>
#include <param/tt_param_dir.h>
#include <param/tt_param_path.h>

#define TT_PARAM_BS4_INTERNAL_USAGE
#include <param/html/bootstrap4/tt_param_bs4_internal_util.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __NAV_START                                                            \
    "<nav class=\"d-flex flex-md-row-reverse navbar navbar-expand-md w-100 "   \
    "%s\">"                                                                    \
    "<a class=\"btn %s\" href=\"%s\">%s</a>"                                   \
                                                                               \
    "<button class=\"navbar-toggler\" type=\"button\" "                        \
    "data-toggle=\"collapse\" data-target=\"#collapseTarget\">"                \
    "<span class=\"navbar-toggler-icon\"></span>"                              \
    "</button>"                                                                \
                                                                               \
    "<div class=\"collapse navbar-collapse\" id=\"collapseTarget\">"           \
    "<ul class=\"navbar-nav\">"

#define __NAV_BRAND_LINK "<a class=\"navbar-brand %s\" href=\"%s\">%s</a>"

#define __NAV_BRAND "<span class=\"navbar-brand %s\">%s</span>"

#define __NAV_LINK                                                             \
    "<li class=\"nav-item\">"                                                  \
    "<a class=\"nav-link\" href=\"%s\">%s</a>"                                 \
    "</li>"

#define __NAV_LINK_ACTIVE                                                      \
    "<li class=\"nav-item\">"                                                  \
    "<a class=\"nav-link active\" href=\"%s\">%s</a>"                          \
    "</li>"

#define __NAV_BUTTON                                                           \
    "<li class=\"nav-item\">"                                                  \
    "<a class=\"nav-link\" href=\"#\" id=\"nav-%s\">%s</a>"                    \
    "</li>"

#define __NAV_BUTTON_ACTIVE                                                    \
    "<li class=\"nav-item\">"                                                  \
    "<a class=\"nav-link active\" href=\"#\" id=\"nav-%s\">%s</a>"             \
    "</li>"

#define __NAV_END "</ul></div></nav>"

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_param_bs4nav_init(IN tt_param_bs4nav_t *nav)
{
    nav->nav_class = "navbar-dark bg-dark";
    nav->account_class = "btn-success";
    nav->account_href = "#";
    nav->account_text = "Admin";
    nav->brand_class = "";
    nav->brand_href = NULL;
    nav->brand_text = NULL;
    nav->button_style = TT_FALSE;
}

tt_result_t tt_param_bs4nav_render(IN tt_param_bs4nav_t *nav,
                                   IN OPT tt_param_t *root,
                                   IN tt_param_t *parent,
                                   IN OPT tt_param_t *active, OUT tt_buf_t *buf)
{
    tt_param_dir_t *dir;
    tt_param_t *p;
    tt_buf_t path;

    TT_ASSERT((parent != NULL) && (parent->type == TT_PARAM_DIR));
    dir = TT_PARAM_CAST(parent, tt_param_dir_t);
    TT_ASSERT((active == NULL) || tt_list_contain(&dir->child, &active->node));

    TT_DO(tt_buf_putf(buf, __NAV_START, nav->nav_class, nav->account_class,
                      nav->account_href, nav->account_text));

    if (nav->brand_text != NULL) {
        if (nav->brand_href != NULL) {
            TT_DO(tt_buf_putf(buf, __NAV_BRAND_LINK, nav->brand_class,
                              nav->brand_href, nav->brand_text));
        } else {
            TT_DO(tt_buf_putf(buf, __NAV_BRAND, nav->brand_class,
                              nav->brand_text));
        }
    }

    tt_buf_init(&path, NULL);
    for (p = tt_param_dir_head(dir); p != NULL; p = tt_param_dir_next(p)) {
        if (p->type != TT_PARAM_DIR) { continue; }

        if (nav->button_style) {
            TT_DO_G(fail, tt_buf_putf(buf,
                                      TT_COND(p == active, __NAV_BUTTON_ACTIVE,
                                              __NAV_BUTTON),
                                      tt_param_name(p), __param_display(p)));
        } else {
            tt_buf_clear(&path);
            TT_DO_G(fail, tt_param_path_n2p(root, p, &path));
            TT_DO_G(fail, tt_buf_put_u8(&path, 0));

            TT_DO_G(fail, tt_buf_putf(buf,
                                      TT_COND(p == active, __NAV_LINK_ACTIVE,
                                              __NAV_LINK),
                                      TT_COND(tt_buf_empty(&path), "#",
                                              (tt_char_t *)TT_BUF_RPOS(&path)),
                                      __param_display(p)));
        }
    }
    tt_buf_destroy(&path);

    TT_DO(__PUT_CSTR(buf, __NAV_END));

    return TT_SUCCESS;

fail:
    tt_buf_destroy(&path);
    return TT_FAIL;
}

/*
 <nav class="d-flex flex-md-row-reverse navbar navbar-expand-md w-100
 navbar-dark
 bg-dark">
 <a class="btn btn-success" href="#">Admin</a>
 <button class="navbar-toggler" type="button" data-toggle="collapse"
 data-target="#collapseTarget">
 <span class="navbar-toggler-icon"></span>
 </button>
 <div class="collapse navbar-collapse" id="collapseTarget">
 <ul class="navbar-nav">
 <li class="nav-item">
 <a class="nav-link" href="#">Link 1</a>
 </li>
 <li class="nav-item">
 <a class="nav-link" href="#">Link 2</a>
 </li>
 <li class="nav-item">
 <a class="nav-link" href="#">Link 3</a>
 </li>
 </ul>
 </div>
 </nav>
 */
