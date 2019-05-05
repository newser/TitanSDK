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
    "<nav class=\"col-2 navbar bg-light %s\" id=\"sb-%s\" %s>"                 \
    "<ul class=\"navbar-nav\">"

#define __SIDEBAR_ENTRY                                                        \
    "<li class=\"nav-item\">"                                                  \
    "<a class=\"nav-link text-dark\" href=\"#%s\">%s</a>"                      \
    "</li>"

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

void tt_param_bs4sidebar_init(IN tt_param_bs4sidebar_t *sb)
{
    sb->nav_class = "";
    sb->hide = TT_FALSE;
}

tt_result_t tt_param_bs4sidebar_render(IN tt_param_bs4sidebar_t *sb,
                                       IN tt_param_t *param, OUT tt_buf_t *buf)
{
    tt_param_dir_t *dir;
    tt_param_t *p;

    TT_ASSERT(param->type == TT_PARAM_DIR);
    dir = TT_PARAM_CAST(param, tt_param_dir_t);

    TT_DO(tt_buf_putf(buf, __SIDEBAR_START, sb->nav_class, tt_param_name(param),
                      TT_COND(sb->hide, "style=\"display:none\"", "")));

    for (p = tt_param_dir_head(dir); p != NULL; p = tt_param_dir_next(p)) {
        if (p->type == TT_PARAM_DIR) {
            TT_DO(tt_buf_putf(buf, __SIDEBAR_ENTRY, tt_param_name(p),
                              __param_display(p)));
        }
    }

    TT_DO(__PUT_CSTR(buf, __SIDEBAR_END));

    return TT_SUCCESS;
}

/*
 <sb class="d-none d-md-block col-2 navbar">
 <ul class="navbar-nav">
 <li class="nav-item"><a class="nav-link" href="#platform">platform</a></li>
 <li class="nav-item"><a class="nav-link" href="#app1">app1</a></li>
 </ul>
 </sb>
 */
