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

#include <param/html/bootstrap4/tt_param_bs4_control_render.h>

#include <algorithm/tt_buffer_format.h>
#include <param/html/bootstrap4/tt_param_bs4_content.h>

#define TT_PARAM_BS4_INTERNAL_USAGE
#include <param/html/bootstrap4/tt_param_bs4_internal_util.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __1ENTRY_INPUT_RD                                                      \
    "<div class=\"form-group row col-12 px-0 py-1\">"                          \
    "<label class=\"col-4 col-md-2 text-right pr-0 col-form-label %s\">%s "    \
    ":</label>"                                                                \
    "<div class=\"col-8 col-md-8\">"                                           \
    "<input type=\"text\" class=\"form-control-plaintext %s\" id=\"%s\" "      \
    "readonly />"                                                              \
    "</div></div>"

#define __1ENTRY_INPUT_WR_START                                                \
    "<div class=\"form-group row col-12 px-0 py-1\">"                          \
    "<label class=\"col-4 col-md-2 text-right pr-0 col-form-label %s\">%s "    \
    ":</label>"                                                                \
    "<div class=\"col-8 col-md-8\">"                                           \
    "<input type=\"text\" class=\"form-control %s\" id=\"%s\""

#define __1ENTRY_INPUT_WR_END "\" /></div></div>"

#define __2ENTRY_INPUT_RD                                                      \
    "<div class=\"form-group row col-12 col-md-6 px-0 py-1\">"                 \
    "<label class=\"col-4 text-right pr-0 col-form-label %s\">%s :</label>"    \
    "<div class=\"col-6\">"                                                    \
    "<input type=\"text\" class=\"form-control-plaintext %s\" id=\"%s\" "      \
    "readonly />"                                                              \
    "</div></div>"

#define __2ENTRY_INPUT_WR_START                                                \
    "<div class=\"form-group row col-12 col-md-6 px-0 py-1\">"                 \
    "<label class=\"col-4 text-right pr-0 col-form-label %s\">%s :</label>"    \
    "<div class=\"col-6\">"                                                    \
    "<input type=\"%s\" class=\"form-control %s\" id=\"%s\" "

#define __2ENTRY_INPUT_WR_END "/></div></div>"

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

enum
{
    PTN_RD,
    PTN_INPUT_WR_START,
    PTN_INPUT_WR_END,

    PTN_NUM
};

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __render_param(IN tt_param_bs4_content_t *ct,
                                  IN tt_param_t *p,
                                  IN tt_param_bs4_level_t lv,
                                  OUT tt_buf_t *buf,
                                  IN const tt_char_t **ptn);

static tt_result_t __render_input(IN tt_param_bs4_content_t *ct,
                                  IN tt_param_t *p,
                                  IN tt_param_bs4_level_t lv,
                                  OUT tt_buf_t *buf,
                                  IN tt_param_bs4_input_t *i,
                                  IN const tt_char_t **ptn);

static const tt_char_t *__input_type(IN tt_param_t *p);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_param_bs4_ctrl_render(IN tt_param_bs4_content_t *ct,
                                     IN tt_param_t *param,
                                     IN tt_param_bs4_level_t lv,
                                     OUT tt_buf_t *buf)
{
    static const tt_char_t *ptn[PTN_NUM] = {
        __1ENTRY_INPUT_RD, __1ENTRY_INPUT_WR_START, __1ENTRY_INPUT_WR_END,
    };

    TT_DO(__render_param(ct, param, lv, buf, ptn));

    return TT_SUCCESS;
}

tt_result_t tt_param_bs4_ctrl_render_pair(IN tt_param_bs4_content_t *ct,
                                          IN tt_param_t *p1,
                                          IN tt_param_t *p2,
                                          IN tt_param_bs4_level_t lv,
                                          OUT tt_buf_t *buf)
{
    static const tt_char_t *ptn[PTN_NUM] = {
        __2ENTRY_INPUT_RD, __2ENTRY_INPUT_WR_START, __2ENTRY_INPUT_WR_END,
    };

    TT_DO(__render_param(ct, p1, lv, buf, ptn));

    TT_DO(__render_param(ct, p2, lv, buf, ptn));

    return TT_SUCCESS;
}

tt_result_t __render_param(IN tt_param_bs4_content_t *ct,
                           IN tt_param_t *p,
                           IN tt_param_bs4_level_t lv,
                           OUT tt_buf_t *buf,
                           IN const tt_char_t **ptn)
{
    tt_param_bs4_display_t disp;

    disp = tt_param_bs4_display(p, lv);
    if (disp == TT_PARAM_BS4_DISP_RD) {
        // always show readonly param as an input control
        TT_DO(tt_buf_putf(buf,
                          ptn[PTN_RD],
                          ct->name_class,
                          __param_display(p),
                          __input_type(p),
                          ct->val_class,
                          tt_param_name(p)));
    } else if (disp == TT_PARAM_BS4_DISP_WR) {
        tt_param_bs4_ctrl_t *ctrl;

        ctrl = &p->bs4_ctrl;
        if (ctrl->type == TT_PARAM_BS4_INPUT) {
            TT_DO(__render_input(ct, p, lv, buf, &ctrl->input, ptn));
        }
    }

    return TT_SUCCESS;
}

tt_result_t __render_input(IN tt_param_bs4_content_t *ct,
                           IN tt_param_t *p,
                           IN tt_param_bs4_level_t lv,
                           OUT tt_buf_t *buf,
                           IN tt_param_bs4_input_t *i,
                           IN const tt_char_t **ptn)
{
    TT_DO(tt_buf_putf(buf,
                      ptn[PTN_INPUT_WR_START],
                      ct->name_class,
                      __param_display(p),
                      __input_type(p),
                      ct->val_class,
                      tt_param_name(p)));

    if (i->pattern != NULL) {
        TT_DO(tt_buf_putf(buf, "pattern=\"%s\" ", i->pattern));
    }

    if (i->minlen[0] != 0) {
        TT_DO(tt_buf_putf(buf, "minlength=\"%s\" ", i->minlen));
    }

    if (i->maxlen[0] != 0) {
        TT_DO(tt_buf_putf(buf, "maxlength=\"%s\" ", i->maxlen));
    }

    if (i->min[0] != 0) {
        TT_DO(tt_buf_putf(buf, "min=\"%s\" ", i->min));
    }

    if (i->max[0] != 0) {
        TT_DO(tt_buf_putf(buf, "max=\"%s\" ", i->max));
    }

    if (i->step[0] != 0) {
        TT_DO(tt_buf_putf(buf, "step=\"%s\" ", i->step));
    }

    TT_DO(tt_buf_put_cstr(buf, ptn[PTN_INPUT_WR_END]));

    return TT_SUCCESS;
}

const tt_char_t *__input_type(IN tt_param_t *p)
{
    static const tt_char_t *map[TT_PARAM_TYPE_NUM] = {
        "number", "number", NULL, "text", NULL, "number", NULL,
    };

    const tt_char_t *t = map[p->type];
    return TT_COND(t != NULL, t, "text");
}
