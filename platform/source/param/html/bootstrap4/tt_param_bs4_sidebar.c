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

#include <param/html/bootstrap4/tt_param_bs4_sidebar.h>

#include <algorithm/tt_buffer_format.h>
#include <param/tt_param_dir.h>
#include <param/tt_param_path.h>

#define TT_PARAM_BS4_INTERNAL_USAGE
#include <param/html/bootstrap4/tt_param_bs4_internal_util.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __SIDEBAR_START                                                        \
    "<nav class=\"d-none d-md-block col-2 navbar bg-light %s\">"               \
    "<ul class=\"navbar-nav\">"

#define __SIDEBAR_ENTRY                                                        \
    "<li class=\"nav-item\"><a class=\"nav-link text-dark\" "                  \
    "href=\"#%s\">%s</a></li>"

#define __SIDEBAR_END "</ul></nav>"

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

void tt_param_bs4_sidebar_init(IN tt_param_bs4_sidebar_t *nav)
{
    nav->nav_class = "";
}

tt_result_t tt_param_bs4_sidebar_render(IN tt_param_bs4_sidebar_t *nav,
                                        IN OPT tt_param_t *root,
                                        IN tt_param_t *param,
                                        OUT tt_buf_t *buf)
{
    tt_param_dir_t *dir;
    tt_bool_t has_dir_child;
    tt_param_t *p;

    // only render sidebar for directory
    if (param->type != TT_PARAM_DIR) {
        return TT_SUCCESS;
    }
    dir = TT_PARAM_CAST(param, tt_param_dir_t);

    // only render sidebar for dir with child directories
    has_dir_child = TT_FALSE;
    for (p = tt_param_dir_head(dir); p != NULL; p = tt_param_dir_next(p)) {
        if (p->type == TT_PARAM_DIR) {
            has_dir_child = TT_TRUE;
            break;
        }
    }
    if (!has_dir_child) {
        return TT_SUCCESS;
    }

    TT_DO(tt_buf_putf(buf, __SIDEBAR_START, nav->nav_class));

    for (p = tt_param_dir_head(dir); p != NULL; p = tt_param_dir_next(p)) {
        if (p->type == TT_PARAM_DIR) {
            TT_DO(tt_buf_putf(buf,
                              __SIDEBAR_ENTRY,
                              tt_param_name(p),
                              __param_display(p)));
        }
    }

    TT_DO(__PUT_CSTR(buf, __SIDEBAR_END));

    return TT_SUCCESS;
}

/*
 <nav class="d-none d-md-block col-2 navbar">
 <ul class="navbar-nav">
 <li class="nav-item"><a class="nav-link" href="#platform">platform</a></li>
 <li class="nav-item"><a class="nav-link" href="#app1">app1</a></li>
 </ul>
 </nav>
 */
