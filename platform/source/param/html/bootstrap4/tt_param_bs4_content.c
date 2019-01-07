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

#include <param/html/bootstrap4/tt_param_bs4_content.h>

#include <algorithm/tt_buffer_format.h>
#include <param/html/bootstrap4/tt_param_bs4_control_render.h>
#include <param/tt_param_dir.h>

#define TT_PARAM_BS4_INTERNAL_USAGE
#include <param/html/bootstrap4/tt_param_bs4_internal_util.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __C_START "<div class=\"col-md-10\">"

#define __C_END "</div>"

#define __GROUP_START "<form class=\"form border-bottom p-4 %s %s\" id=\"%s\">"

#define __GROUP_END "</form>"

#define __GROUP_TITLE "<div class=\"h5 %s\"><label>%s</label></div>"

#define __GROUP_DATA_START "<div class=\"d-flex flex-wrap\">"

#define __GROUP_DATA_END "</div>"

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __render_dir(IN tt_param_bs4_content_t *ct,
                                IN tt_param_t *p,
                                IN tt_param_bs4_level_t lv,
                                IN OUT tt_bool_t *group0,
                                OUT tt_buf_t *buf);

static tt_bool_t __param_group_head(IN tt_param_dir_t *dir,
                                    OUT tt_param_t **pp1,
                                    OUT tt_param_t **pp2,
                                    IN tt_param_bs4_level_t lv);

static tt_bool_t __param_group_next(IN OUT tt_param_t **pp1,
                                    IN OUT tt_param_t **pp2,
                                    IN tt_param_bs4_level_t lv);

static tt_bool_t __param_pair(IN tt_param_t *pos,
                              IN OUT tt_param_t **pp1,
                              OUT tt_param_t **pp2,
                              IN tt_param_bs4_level_t lv);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_param_bs4_content_init(IN tt_param_bs4_content_t *ct)
{
    ct->group_class = "";
    ct->group0_class = NULL;
    ct->group1_class = NULL;
    ct->title_class = "text-dark";
    ct->name_class = "text-dark";
    ct->val_class = "text-dark";
}

tt_result_t tt_param_bs4_content_render(IN tt_param_bs4_content_t *ct,
                                        IN tt_param_t *param,
                                        IN tt_param_bs4_level_t lv,
                                        OUT tt_buf_t *buf)
{
    tt_param_dir_t *dir;
    tt_param_t *p;
    tt_bool_t group0 = TT_TRUE;

    // only render content for directory
    if (param->type != TT_PARAM_DIR) {
        return TT_SUCCESS;
    }
    dir = TT_PARAM_CAST(param, tt_param_dir_t);

    TT_DO(__PUT_CSTR(buf, __C_START));

    // render child non-dir params
    TT_DO(__render_dir(ct, param, lv, &group0, buf));

    // render child dir params
    for (p = tt_param_dir_head(dir); p != NULL; p = tt_param_dir_next(p)) {
        if (p->type == TT_PARAM_DIR) {
            TT_DO(__render_dir(ct, p, lv, &group0, buf));
        }
    }

    TT_DO(__PUT_CSTR(buf, __C_END));

    return TT_SUCCESS;
}

tt_result_t __render_dir(IN tt_param_bs4_content_t *ct,
                         IN tt_param_t *p,
                         IN tt_param_bs4_level_t lv,
                         IN OUT tt_bool_t *group0,
                         OUT tt_buf_t *buf)
{
    tt_param_dir_t *dir;
    tt_bool_t g0 = *group0;
    tt_param_t *p1, *p2;

    TT_ASSERT(p->type == TT_PARAM_DIR);
    dir = TT_PARAM_CAST(p, tt_param_dir_t);

    if (!__param_group_head(dir, &p1, &p2, lv)) {
        // no data param
        return TT_SUCCESS;
    }

    if ((ct->group0_class != NULL) && (ct->group1_class != NULL)) {
        TT_DO(tt_buf_putf(buf,
                          __GROUP_START,
                          ct->group_class,
                          TT_COND(g0, ct->group0_class, ct->group1_class),
                          tt_param_name(p)));
    } else {
        TT_DO(tt_buf_putf(buf,
                          __GROUP_START,
                          ct->group_class,
                          "",
                          tt_param_name(p)));
    }
    *group0 = !g0;

    TT_DO(tt_buf_putf(buf, __GROUP_TITLE, ct->title_class, __param_display(p)));

    TT_DO(__PUT_CSTR(buf, __GROUP_DATA_START));

    do {
        TT_ASSERT(p1 != NULL);
        if (p2 != NULL) {
            TT_DO(tt_param_bs4_ctrl_render_pair(ct, p1, p2, lv, buf));
        } else {
            TT_DO(tt_param_bs4_ctrl_render(ct, p1, lv, buf));
        }
    } while (__param_group_next(&p1, &p2, lv));

    TT_DO(__PUT_CSTR(buf, __GROUP_DATA_END));

    TT_DO(__PUT_CSTR(buf, __GROUP_END));

    return TT_SUCCESS;
}

tt_bool_t __param_group_head(IN tt_param_dir_t *dir,
                             OUT tt_param_t **pp1,
                             OUT tt_param_t **pp2,
                             IN tt_param_bs4_level_t lv)
{
    return __param_pair(tt_param_dir_head(dir), pp1, pp2, lv);
}

tt_bool_t __param_group_next(IN OUT tt_param_t **pp1,
                             IN OUT tt_param_t **pp2,
                             IN tt_param_bs4_level_t lv)
{
    tt_param_t *pos;

    TT_ASSERT(*pp1 != NULL);
    if (*pp2 == NULL) {
        pos = tt_param_dir_next(*pp1);
    } else {
        pos = tt_param_dir_next(*pp2);
    }

    return __param_pair(pos, pp1, pp2, lv);
}

tt_bool_t __param_pair(IN tt_param_t *pos,
                       IN OUT tt_param_t **pp1,
                       OUT tt_param_t **pp2,
                       IN tt_param_bs4_level_t lv)
{
#define __HIDDEN(p, lv) (tt_param_bs4_display(p, lv) == TT_PARAM_BS4_DISP_HIDE)

    tt_param_t *p1, *p2;

    if (pos == NULL) {
        return TT_FALSE;
    }

    p1 = pos;
    TT_ASSERT(p1 != NULL);
    while ((p1 != NULL) && ((p1->type == TT_PARAM_DIR) ||
                            (p1->type == TT_PARAM_EXE) || __HIDDEN(p1, lv))) {
        // dir and exe params won't be rendered
        p1 = tt_param_dir_next(p1);
    }
    if (p1 == NULL) {
        *pp1 = NULL;
        *pp2 = NULL;
        return TT_FALSE;
    }

    if (p1->bs4_ctrl.whole_line) {
        *pp1 = p1;
        *pp2 = NULL;
        return TT_TRUE;
    }

    p2 = tt_param_dir_next(p1);
    while ((p2 != NULL) && ((p2->type == TT_PARAM_DIR) ||
                            (p2->type == TT_PARAM_EXE) || __HIDDEN(p2, lv))) {
        p2 = tt_param_dir_next(p2);
    }
    if ((p2 == NULL) || p2->bs4_ctrl.whole_line) {
        *pp1 = p1;
        *pp2 = NULL;
        return TT_TRUE;
    }

    *pp1 = p1;
    *pp2 = p2;
    return TT_TRUE;
}
