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

#define __DEFAULT_NAV_HEAD                                                     \
    "<nav class=\"d-flex flex-md-row-reverse navbar navbar-expand-md w-100 "   \
    "%s\">"                                                                    \
    "<a class=\"btn %s\" href=\"%s\">%s</a>"                                   \
    "<button class=\"navbar-toggler\" type=\"button\" "                        \
    "data-toggle=\"collapse\" data-target=\"#collapseTarget\">"                \
    "<span class=\"navbar-toggler-icon\"></span></button>"                     \
    "<div class=\"collapse navbar-collapse\" id=\"collapseTarget\">"           \
    "<ul class=\"navbar-nav\">"

#define __DEFAULT_NAV_ENTRY                                                    \
    "<li class=\"nav-item\">"                                                  \
    "<a class=\"nav-link\" href=\"%s\">%s</a>"                                 \
    "</li>"

#define __DEFAULT_NAV_ENTRY_UNCLICKABLE                                        \
    "<li class=\"nav-item\">"                                                  \
    "<a class=\"nav-link\">%s</a>"                                             \
    "</li>"

#define __DEFAULT_NAV_TAIL "</ul></div></nav>"

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

void tt_param_bs4_nav_init(IN tt_param_bs4_nav_t *nav)
{
    nav->nav_class = NULL;
    nav->usr_class = NULL;
    nav->usr_href = NULL;
    nav->usr_text = NULL;
}

tt_result_t tt_param_bs4_nav_render(IN tt_param_bs4_nav_t *nav,
                                    IN OPT tt_param_t *root,
                                    IN tt_param_t *param,
                                    OUT tt_buf_t *buf)
{
    const tt_char_t *display;

    TT_DO(tt_buf_putf(buf,
                      __DEFAULT_NAV_HEAD,
                      __NULL_EMPTY(nav->nav_class),
                      __NULL_EMPTY(nav->usr_class),
                      __NULL_EMPTY(nav->usr_href),
                      __NULL_EMPTY(nav->usr_text)));

    if (param->type == TT_PARAM_DIR) {
        tt_param_dir_t *dir = TT_PARAM_CAST(param, tt_param_dir_t);
        tt_param_t *p;

        p = tt_param_dir_head(dir);
        if (p != NULL) {
            tt_buf_t path;

            tt_buf_init(&path, NULL);
            do {
                tt_buf_clear(&path);
                if (!TT_OK(tt_param_path_n2p(root, p, &path)) ||
                    !TT_OK(tt_buf_put_u8(&path, 0)) ||
                    !TT_OK(tt_buf_putf(buf,
                                       __DEFAULT_NAV_ENTRY,
                                       TT_COND(tt_buf_empty(&path),
                                               "#",
                                               (tt_char_t *)TT_BUF_RPOS(&path)),
                                       __param_display(p)))) {
                    tt_buf_destroy(&path);
                    return TT_FAIL;
                }
            } while ((p = tt_param_dir_next(p)) != NULL);
            tt_buf_destroy(&path);
        } else {
            // empty directory
            TT_DO(tt_buf_putf(buf,
                              __DEFAULT_NAV_ENTRY_UNCLICKABLE,
                              __param_display(param)));
        }
    } else {
        TT_DO(tt_buf_putf(buf,
                          __DEFAULT_NAV_ENTRY_UNCLICKABLE,
                          __param_display(param)));
    }

    TT_DO(tt_buf_put(buf, __DEFAULT_NAV_TAIL, sizeof(__DEFAULT_NAV_TAIL) - 1));

    return TT_SUCCESS;
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
