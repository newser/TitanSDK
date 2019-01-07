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

#include <param/html/bootstrap4/tt_param_bs4_page.h>

#include <algorithm/tt_buffer_format.h>
#include <param/tt_param_dir.h>

#define TT_PARAM_BS4_INTERNAL_USAGE
#include <param/html/bootstrap4/tt_param_bs4_internal_util.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __DEFAULT_LANG "en"

#define __DEFAULT_CSS                                                          \
    "<link rel=\"stylesheet\" href=\"/css/bootstrap.min.css\" />"

#define __DEFAULT_JS                                                           \
    "<script src=\"/js/jquery.slim.min.js\"></script>"                         \
    "<script src=\"/js/popper.min.js\"></script>"                              \
    "<script src=\"/js/bootstrap.min.js\"></script>"

#define __PG_START                                                             \
    "<!doctype html>"                                                          \
    "<html lang=\"%s\">"

#define __PG_END "</html>"

#define __PG_HEAD                                                              \
    "<head>"                                                                   \
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1, "  \
    "shrink-to-fit=no\" />"                                                    \
    "%s"                                                                       \
    "%s"                                                                       \
    "</head>"

#define __PG_BODY_START "<body>"

#define __PG_BODY_END "%s%s</body>"

#define __PG_MAIN_START                                                        \
    "<main class=\"container-fluid\">"                                         \
    "<div class=\"row\">"

#define __PG_MAIN_END                                                          \
    "</div>"                                                                   \
    "</main>"

#define __PG_FOOTER                                                            \
    "<footer class=\"container-fluid text-center %s\">"                        \
    "<p>%s</p>"                                                                \
    "</footer>"

#define __DEFAULT_FOOTER_TEXT "TitanSDK HTTP Module"

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __render_head(IN tt_param_bs4page_t *pg, OUT tt_buf_t *buf);

static tt_result_t __render_body(IN tt_param_bs4page_t *pg,
                                 IN OPT tt_param_t *root,
                                 IN tt_param_t *param,
                                 IN tt_param_bs4level_t lv,
                                 OUT tt_buf_t *buf);

static tt_result_t __render_body_header(IN tt_param_bs4page_t *pg,
                                        IN OPT tt_param_t *root,
                                        IN tt_param_t *param,
                                        OUT tt_buf_t *buf);

static tt_result_t __render_body_main(IN tt_param_bs4page_t *pg,
                                      IN OPT tt_param_t *root,
                                      IN tt_param_t *param,
                                      IN tt_param_bs4level_t lv,
                                      OUT tt_buf_t *buf);

static tt_result_t __render_body_footer(IN tt_param_bs4page_t *pg,
                                        OUT tt_buf_t *buf);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_param_bs4page_init(IN tt_param_bs4page_t *pg)
{
    tt_param_bs4nav_init(&pg->nav);
    tt_param_bs4sidebar_init(&pg->sidebar);
    tt_param_bs4content_init(&pg->content);

    pg->lang = __DEFAULT_LANG;
    pg->css = __DEFAULT_CSS;
    pg->js = __DEFAULT_JS;
    pg->js_extra = "";
    pg->head_extra = "";
    pg->footer_class = "bg-dark text-white";
    pg->footer_text = __DEFAULT_FOOTER_TEXT;
}

tt_result_t tt_param_bs4page_render(IN tt_param_bs4page_t *pg,
                                    IN OPT tt_param_t *root,
                                    IN tt_param_t *param,
                                    IN tt_param_bs4level_t lv,
                                    OUT tt_buf_t *buf)
{
    // only render page for directory
    if (param->type != TT_PARAM_DIR) {
        return TT_SUCCESS;
    }

    TT_DO(tt_buf_putf(buf, __PG_START, pg->lang));

    TT_DO(__render_head(pg, buf));

    TT_DO(__render_body(pg, root, param, lv, buf));

    TT_DO(__PUT_CSTR(buf, __PG_END));

    return TT_SUCCESS;
}

tt_result_t __render_head(IN tt_param_bs4page_t *pg, OUT tt_buf_t *buf)
{
    TT_DO(tt_buf_putf(buf, __PG_HEAD, pg->head_extra, pg->css));

    return TT_SUCCESS;
}

tt_result_t __render_body(IN tt_param_bs4page_t *pg,
                          IN OPT tt_param_t *root,
                          IN tt_param_t *param,
                          IN tt_param_bs4level_t lv,
                          OUT tt_buf_t *buf)
{
    TT_DO(__PUT_CSTR(buf, __PG_BODY_START));

    TT_DO(__render_body_header(pg, root, param, buf));

    TT_DO(__render_body_main(pg, root, param, lv, buf));

    TT_DO(__render_body_footer(pg, buf));

    TT_DO(tt_buf_putf(buf, __PG_BODY_END, pg->js, pg->js_extra));

    return TT_SUCCESS;
}

tt_result_t __render_body_header(IN tt_param_bs4page_t *pg,
                                 IN OPT tt_param_t *root,
                                 IN tt_param_t *param,
                                 OUT tt_buf_t *buf)
{
    TT_DO(__PUT_CSTR(buf, "<header>"));

    TT_DO(tt_param_bs4nav_render(&pg->nav,
                                 root,
                                 tt_param_parent(param),
                                 param,
                                 buf));

    TT_DO(__PUT_CSTR(buf, "</header>"));

    return TT_SUCCESS;
}

tt_result_t __render_body_main(IN tt_param_bs4page_t *pg,
                               IN OPT tt_param_t *root,
                               IN tt_param_t *param,
                               IN tt_param_bs4level_t lv,
                               OUT tt_buf_t *buf)
{
    TT_DO(__PUT_CSTR(buf, __PG_MAIN_START));

    TT_DO(tt_param_bs4sidebar_render(&pg->sidebar, param, buf));

    // TT_ASSERT(0);
    TT_DO(tt_param_bs4content_render(&pg->content, param, lv, buf));

    TT_DO(__PUT_CSTR(buf, __PG_MAIN_END));

    return TT_SUCCESS;
}

tt_result_t __render_body_footer(IN tt_param_bs4page_t *pg, OUT tt_buf_t *buf)
{
    if (pg->footer_text != NULL) {
        TT_DO(tt_buf_putf(buf, __PG_FOOTER, pg->footer_class, pg->footer_text));
    }

    return TT_SUCCESS;
}
