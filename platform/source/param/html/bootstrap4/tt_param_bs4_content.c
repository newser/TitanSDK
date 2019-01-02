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
#include <param/tt_param_dir.h>

#define TT_PARAM_BS4_INTERNAL_USAGE
#include <param/html/bootstrap4/tt_param_bs4_internal_util.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __C_START "<div class=\"col-md-10 p-0\">"

#define __C_END "</div>"

#define __GROUP_START                                                          \
    "<div class=\"container-fluid border-bottom p-4 %s %s\" id=\"%s\">"

#define __GROUP_END "</div>"

#define __GROUP_TITLE                                                          \
    "<div class=\"row h5 %s\"><label class=\"col-12\">%s</label></div>"

#define __ROW_START "<div class=\"row\">"

#define __ROW_END "</div>"

#define __2_ENTRY_START                                                        \
    "<label class=\"col-4 col-md-2 text-right pr-0 %s\" id=\"%s\">%s "         \
    ":</label><label class=\"col-8 col-md-4 %s\">"

#define __1_ENTRY_START                                                        \
    "<label class=\"col-4 col-md-2 text-right pr-0 %s\" id=\"%s\">%s "         \
    ":</label><label class=\"col-8 col-md-10 %s\">"

#define __ENTRY_END "</label>"

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
                                IN OUT tt_bool_t *group0,
                                OUT tt_buf_t *buf);

static tt_result_t __render_2param(IN tt_param_bs4_content_t *ct,
                                   IN tt_param_t *p1,
                                   IN tt_param_t *p2,
                                   OUT tt_buf_t *buf);

static tt_result_t __render_1param(IN tt_param_bs4_content_t *ct,
                                   IN tt_param_t *p,
                                   OUT tt_buf_t *buf);

static tt_bool_t __param_group_head(IN tt_param_dir_t *dir,
                                    OUT tt_param_t **pp1,
                                    OUT tt_param_t **pp2);

static tt_bool_t __param_group_next(IN OUT tt_param_t **pp1,
                                    IN OUT tt_param_t **pp2);

static tt_bool_t __param_pair(IN tt_param_t *pos,
                              IN OUT tt_param_t **pp1,
                              OUT tt_param_t **pp2);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_param_bs4_content_init(IN tt_param_bs4_content_t *ct)
{
    ct->group_class = "";
    ct->group0_class = "bg-light";
    ct->group1_class = "";
    ct->title_class = "";
    ct->name_class = "";
    ct->val_class = "";
}

tt_result_t tt_param_bs4_content_render(IN tt_param_bs4_content_t *ct,
                                        IN tt_param_t *param,
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

    TT_DO(tt_buf_put(buf, __C_START, sizeof(__C_START) - 1));

    // render child non-dir params
    TT_DO(__render_dir(ct, param, &group0, buf));

    // render child dir params
    for (p = tt_param_dir_head(dir); p != NULL; p = tt_param_dir_next(p)) {
        if (p->type == TT_PARAM_DIR) {
            TT_DO(__render_dir(ct, p, &group0, buf));
        }
    }

    TT_DO(tt_buf_put(buf, __C_END, sizeof(__C_END) - 1));

    return TT_SUCCESS;
}

tt_result_t __render_dir(IN tt_param_bs4_content_t *ct,
                         IN tt_param_t *p,
                         IN OUT tt_bool_t *group0,
                         OUT tt_buf_t *buf)
{
    tt_param_dir_t *dir;
    tt_bool_t g0 = *group0;
    tt_param_t *p1, *p2;

    TT_ASSERT(p->type == TT_PARAM_DIR);
    dir = TT_PARAM_CAST(p, tt_param_dir_t);

    if (!__param_group_head(dir, &p1, &p2)) {
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

    do {
        TT_ASSERT(p1 != NULL);
        if (p2 != NULL) {
            TT_DO(__render_2param(ct, p1, p2, buf));
        } else {
            TT_DO(__render_1param(ct, p1, buf));
        }
    } while (__param_group_next(&p1, &p2));

    TT_DO(tt_buf_put(buf, __GROUP_END, sizeof(__GROUP_END) - 1));

    return TT_SUCCESS;
}

tt_result_t __render_2param(IN tt_param_bs4_content_t *ct,
                            IN tt_param_t *p1,
                            IN tt_param_t *p2,
                            OUT tt_buf_t *buf)
{
    TT_DO(tt_buf_put(buf, __ROW_START, sizeof(__ROW_START) - 1));

    // left
    TT_DO(tt_buf_putf(buf,
                      __2_ENTRY_START,
                      ct->name_class,
                      tt_param_name(p1),
                      __param_display(p1),
                      ct->val_class));
    TT_DO(tt_param_read(p1, buf));
    TT_DO(tt_buf_put(buf, __ENTRY_END, sizeof(__ENTRY_END) - 1));

    // right
    TT_DO(tt_buf_putf(buf,
                      __2_ENTRY_START,
                      ct->name_class,
                      tt_param_name(p2),
                      __param_display(p2),
                      ct->val_class));
    TT_DO(tt_param_read(p2, buf));
    TT_DO(tt_buf_put(buf, __ENTRY_END, sizeof(__ENTRY_END) - 1));

    TT_DO(tt_buf_put(buf, __ROW_END, sizeof(__ROW_END) - 1));

    return TT_SUCCESS;
}

tt_result_t __render_1param(IN tt_param_bs4_content_t *ct,
                            IN tt_param_t *p,
                            OUT tt_buf_t *buf)
{
    TT_DO(tt_buf_put(buf, __ROW_START, sizeof(__ROW_START) - 1));

    TT_DO(tt_buf_putf(buf,
                      __1_ENTRY_START,
                      ct->name_class,
                      tt_param_name(p),
                      __param_display(p),
                      ct->val_class));
    TT_DO(tt_param_read(p, buf));
    TT_DO(tt_buf_put(buf, __ENTRY_END, sizeof(__ENTRY_END) - 1));

    TT_DO(tt_buf_put(buf, __ROW_END, sizeof(__ROW_END) - 1));

    return TT_SUCCESS;
}

tt_bool_t __param_group_head(IN tt_param_dir_t *dir,
                             OUT tt_param_t **pp1,
                             OUT tt_param_t **pp2)
{
    return __param_pair(tt_param_dir_head(dir), pp1, pp2);
}

tt_bool_t __param_group_next(IN OUT tt_param_t **pp1, IN OUT tt_param_t **pp2)
{
    tt_param_t *pos;

    TT_ASSERT(*pp1 != NULL);
    if (*pp2 == NULL) {
        pos = tt_param_dir_next(*pp1);
    } else {
        pos = tt_param_dir_next(*pp2);
    }

    return __param_pair(pos, pp1, pp2);
}

tt_bool_t __param_pair(IN tt_param_t *pos,
                       IN OUT tt_param_t **pp1,
                       OUT tt_param_t **pp2)
{
    tt_param_t *p1, *p2;

    if (pos == NULL) {
        return TT_FALSE;
    }

    p1 = pos;
    TT_ASSERT(p1 != NULL);
    while ((p1 != NULL) &&
           ((p1->type == TT_PARAM_DIR) || (p1->type == TT_PARAM_EXE))) {
        // dir and exe params won't be rendered
        p1 = tt_param_dir_next(p1);
    }
    if (p1 == NULL) {
        *pp1 = NULL;
        *pp2 = NULL;
        return TT_FALSE;
    }

    if (p1->type == TT_PARAM_STRING) {
        // string will be occupy a whole line
        *pp1 = p1;
        *pp2 = NULL;
        return TT_TRUE;
    }

    p2 = tt_param_dir_next(p1);
    while ((p2 != NULL) &&
           ((p2->type == TT_PARAM_DIR) || (p2->type == TT_PARAM_EXE))) {
        p2 = tt_param_dir_next(p2);
    }
    if ((p2 == NULL) || (p2->type == TT_PARAM_STRING)) {
        *pp1 = p1;
        *pp2 = NULL;
        return TT_TRUE;
    }

    *pp1 = p1;
    *pp2 = p2;
    return TT_TRUE;
}

/*
 <div class="col-md-8">
 <div class="container-fluid border-bottom " id="xx">

 <div class="row pl-3 pt-3 ">
 <label class="col-12 h2 ">platform</label>
 </div>

 <div class="row">
 <label class="col-4 col-md-2 text-right pr-0 ">option 1 :</label>
 <label class="col-8 col-md-4">102</label>
 <label class="col-4 col-md-2 text-right pr-0 ">option 2 :</label>
 <label class="col-8 col-md-4">98</label>
 </div>

 <div class="row">
 <label class="col-4 col-md-2 text-right pr-0">long option :</label>
 <label class="col-8">long long long long long long content...</label>
 </div>

 </div>
 */
