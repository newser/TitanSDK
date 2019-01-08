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

#include <param/html/bootstrap4/tt_param_bs4_spa.h>

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

#define __SPA_START                                                            \
    "<!doctype html>"                                                          \
    "<html lang=\"%s\">"

#define __SPA_END "</html>"

#define __SPA_HEAD                                                             \
    "<head>"                                                                   \
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1, "  \
    "shrink-to-fit=no\" />"                                                    \
    "%s"                                                                       \
    "%s"                                                                       \
    "</head>"

#define __SPA_BODY_START "<body>"

#define __SPA_BODY_END "%s%s</body>"

#define __SPA_MAIN_START                                                       \
    "<main class=\"container-fluid\">"                                         \
    "<div class=\"row\">"

#define __SPA_MAIN_END                                                         \
    "</div>"                                                                   \
    "</main>"

#define __SPA_FOOTER                                                           \
    "<footer class=\"container-fluid text-center %s\">"                        \
    "<p>%s</p>"                                                                \
    "</footer>"

#define __DEFAULT_FOOTER_TEXT "TitanSDK HTTP Module"

#define __JS_START                                                             \
    "var cur=\"\";"                                                            \
    "function activate(n){"                                                    \
    "if(n==cur){return;}"                                                      \
    "if(cur.length>0){"                                                        \
    "$(\"#nav-\".concat(cur)).removeClass(\"active\");"                        \
    "$(\"#sb-\".concat(cur)).removeClass(\"d-lg-block\");"                     \
    "$(\"#ct-\".concat(cur)).addClass(\"d-none\");"                            \
    "}"                                                                        \
    "$(\"#nav-\".concat(n)).addClass(\"active\");"                             \
    "$(\"#sb-\".concat(n)).addClass(\"d-lg-block\");"                          \
    "$(\"#ct-\".concat(n)).removeClass(\"d-none\");"                           \
    "cur=n;"                                                                   \
    "}"                                                                        \
    "$(document).ready(function(){"                                            \
    "$('[data-toggle=\"tooltip\"]').tooltip();"

#define __JS_END "});"

#define __JS_ACTIVE "activate(\"%s\");"

#define __JS_CLICK "$(\"#nav-%s\").click(function(){activate(\"%s\");});"

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static void __prepare_render(IN tt_param_bs4spa_t *spa);

static tt_result_t __render_head(IN tt_param_bs4spa_t *spa, OUT tt_buf_t *buf);

static tt_result_t __render_body(IN tt_param_bs4spa_t *spa,
                                 IN tt_param_t *param,
                                 IN tt_param_bs4level_t lv,
                                 OUT tt_buf_t *buf);

static tt_result_t __render_body_header(IN tt_param_bs4spa_t *spa,
                                        IN tt_param_t *param,
                                        OUT tt_buf_t *buf);

static tt_result_t __render_body_main(IN tt_param_bs4spa_t *spa,
                                      IN tt_param_t *param,
                                      IN tt_param_bs4level_t lv,
                                      OUT tt_buf_t *buf);

static tt_result_t __render_body_footer(IN tt_param_bs4spa_t *spa,
                                        OUT tt_buf_t *buf);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_param_bs4spa_init(IN tt_param_bs4spa_t *spa)
{
    tt_param_bs4nav_init(&spa->nav);
    tt_param_bs4sidebar_init(&spa->sidebar);
    tt_param_bs4content_init(&spa->content);

    spa->lang = __DEFAULT_LANG;
    spa->css = __DEFAULT_CSS;
    spa->js = __DEFAULT_JS;
    spa->js_extra = "";
    spa->head_extra = "";
    spa->footer_class = "bg-dark text-white";
    spa->footer_text = __DEFAULT_FOOTER_TEXT;
}

tt_result_t tt_param_bs4spa_render(IN tt_param_bs4spa_t *spa,
                                   IN tt_param_t *param,
                                   IN tt_param_bs4level_t lv,
                                   OUT tt_buf_t *buf)
{
    // only render spa for directory
    if (param->type != TT_PARAM_DIR) {
        return TT_SUCCESS;
    }

    __prepare_render(spa);

    TT_DO(tt_buf_putf(buf, __SPA_START, spa->lang));

    TT_DO(__render_head(spa, buf));

    TT_DO(__render_body(spa, param, lv, buf));

    TT_DO(__PUT_CSTR(buf, __SPA_END));

    return TT_SUCCESS;
}

tt_result_t tt_param_bs4spa_render_display_js(IN tt_param_bs4spa_t *spa,
                                              IN struct tt_param_s *param,
                                              IN tt_param_bs4level_t lv,
                                              OUT struct tt_buf_s *buf)
{
    tt_param_dir_t *dir;
    tt_param_t *p;

    // only render spa for directory
    if (param->type != TT_PARAM_DIR) {
        return TT_SUCCESS;
    }
    dir = TT_PARAM_CAST(param, tt_param_dir_t);

    __prepare_render(spa);

    __PUT_CSTR(buf, __JS_START);

    p = tt_param_dir_head(dir);
    if (p != NULL) {
        TT_DO(tt_buf_putf(buf, __JS_ACTIVE, tt_param_name(p)));
    }

    for (; p != NULL; p = tt_param_dir_next(p)) {
        TT_DO(tt_buf_putf(buf, __JS_CLICK, tt_param_name(p), tt_param_name(p)));
    }

    __PUT_CSTR(buf, __JS_END);

    return TT_SUCCESS;
}

void __prepare_render(IN tt_param_bs4spa_t *spa)
{
    tt_param_bs4nav_t *nav;
    tt_param_bs4sidebar_t *sb;
    tt_param_bs4content_t *ct;

    nav = &spa->nav;
    nav->button_style = TT_TRUE;

    sb = &spa->sidebar;
    sb->hide = TT_TRUE;

    ct = &spa->content;
    ct->hide = TT_TRUE;
}

tt_result_t __render_head(IN tt_param_bs4spa_t *spa, OUT tt_buf_t *buf)
{
    TT_DO(tt_buf_putf(buf, __SPA_HEAD, spa->head_extra, spa->css));

    return TT_SUCCESS;
}

tt_result_t __render_body(IN tt_param_bs4spa_t *spa,
                          IN tt_param_t *param,
                          IN tt_param_bs4level_t lv,
                          OUT tt_buf_t *buf)
{
    TT_DO(__PUT_CSTR(buf, __SPA_BODY_START));

    TT_DO(__render_body_header(spa, param, buf));

    TT_DO(__render_body_main(spa, param, lv, buf));

    TT_DO(__render_body_footer(spa, buf));

    TT_DO(tt_buf_putf(buf, __SPA_BODY_END, spa->js, spa->js_extra));

    return TT_SUCCESS;
}

tt_result_t __render_body_header(IN tt_param_bs4spa_t *spa,
                                 IN tt_param_t *param,
                                 OUT tt_buf_t *buf)
{
    tt_param_dir_t *dir;

    TT_ASSERT(param->type == TT_PARAM_DIR);
    dir = TT_PARAM_CAST(param, tt_param_dir_t);

    TT_DO(__PUT_CSTR(buf, "<header>"));

    TT_DO(tt_param_bs4nav_render(&spa->nav, NULL, param, NULL, buf));

    TT_DO(__PUT_CSTR(buf, "</header>"));

    return TT_SUCCESS;
}

tt_result_t __render_body_main(IN tt_param_bs4spa_t *spa,
                               IN tt_param_t *param,
                               IN tt_param_bs4level_t lv,
                               OUT tt_buf_t *buf)
{
    tt_param_dir_t *dir;
    tt_param_t *p;

    TT_ASSERT(param->type == TT_PARAM_DIR);
    dir = TT_PARAM_CAST(param, tt_param_dir_t);

    TT_DO(__PUT_CSTR(buf, __SPA_MAIN_START));

    for (p = tt_param_dir_head(dir); p != NULL; p = tt_param_dir_next(p)) {
        TT_DO(tt_param_bs4sidebar_render(&spa->sidebar, p, buf));
    }

    for (p = tt_param_dir_head(dir); p != NULL; p = tt_param_dir_next(p)) {
        TT_DO(tt_param_bs4content_render(&spa->content, p, lv, buf));
    }

    TT_DO(__PUT_CSTR(buf, __SPA_MAIN_END));

    return TT_SUCCESS;
}

tt_result_t __render_body_footer(IN tt_param_bs4spa_t *spa, OUT tt_buf_t *buf)
{
    if (spa->footer_text != NULL) {
        TT_DO(tt_buf_putf(buf,
                          __SPA_FOOTER,
                          spa->footer_class,
                          spa->footer_text));
    }

    return TT_SUCCESS;
}
