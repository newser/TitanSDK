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

#define __1ENTRY_DIV_START                                                     \
    "<div class=\"form-group row col-12 px-0\">"                               \
    "<label class=\"col-4 col-lg-2 text-right pr-0 col-form-label"             \
    " col-form-label-sm %s\">%s :</label>"                                     \
    "<div class=\"%s\">"

#define __1ENTRY_DIV_END "</div></div>"

#define __2ENTRY_DIV_START                                                     \
    "<div class=\"form-group row col-12 col-lg-6 px-0\">"                      \
    "<label class=\"col-4 text-right pr-0 col-form-label col-form-label-sm"    \
    " %s\">%s :</label>"                                                       \
    "<div class=\"%s\">"

#define __2ENTRY_DIV_END "</div></div>"

#define __WHOLE_LINE(p)                                                        \
    TT_COND(p->bs4_ctrl.whole_line, "col-auto col-lg-8", "col-auto")

// ========================================
// param bs4 control: input
// ========================================

#define __1ENTRY_INPUT_RD                                                      \
    __1ENTRY_DIV_START                                                         \
    "<input type=\"text\" class=\"form-control-plaintext form-control-sm "     \
    "%s\" id=\"%s\" tid=\"%d\" readonly />" __1ENTRY_DIV_END

#define __2ENTRY_INPUT_RD                                                      \
    __2ENTRY_DIV_START                                                         \
    "<input type=\"text\" class=\"form-control-plaintext form-control-sm "     \
    "%s\" id=\"%s\" tid=\"%d\" readonly />" __2ENTRY_DIV_END

#define __INPUT_WR_START                                                       \
    "<input type=\"%s\" class=\"form-control form-control-sm %s\" "            \
    "id=\"%s\" tid=\"%d\""

#define __INPUT_WR_END " />"

#define __1ENTRY_INPUT_WR_START __1ENTRY_DIV_START __INPUT_WR_START

#define __1ENTRY_INPUT_WR_END __INPUT_WR_END __1ENTRY_DIV_END

#define __2ENTRY_INPUT_WR_START __2ENTRY_DIV_START __INPUT_WR_START

#define __2ENTRY_INPUT_WR_END __INPUT_WR_END __2ENTRY_DIV_END

// ========================================
// param bs4 control: select
// ========================================

#define __SELECT_WR_START                                                      \
    "<select class=\"custom-select custom-select-sm %s\" id=\"%s\" "           \
    "tid=\"%d\">"

#define __SELECT_END "</select>"

#define __1ENTRY_SELECT_WR_START __1ENTRY_DIV_START __SELECT_WR_START

#define __1ENTRY_SELECT_WR_END __SELECT_END __1ENTRY_DIV_END

#define __2ENTRY_SELECT_WR_START __2ENTRY_DIV_START __SELECT_WR_START

#define __2ENTRY_SELECT_WR_END __SELECT_END __2ENTRY_DIV_END

// ========================================
// param bs4 control: textarea
// ========================================

#define __TEXTAREA_WR_START                                                    \
    "<textarea class=\"form-control form-control-sm %s\" %s id=\"%s\" "        \
    "tid=\"%d\">"

#define __TEXTAREA_END "</textarea>"

#define __1ENTRY_TEXTAREA_WR_START __1ENTRY_DIV_START __TEXTAREA_WR_START

#define __1ENTRY_TEXTAREA_WR_END __TEXTAREA_END __1ENTRY_DIV_END

#define __2ENTRY_TEXTAREA_WR_START __2ENTRY_DIV_START __TEXTAREA_WR_START

#define __2ENTRY_TEXTAREA_WR_END __TEXTAREA_END __2ENTRY_DIV_END

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

enum
{
    PTN_RD,
    PTN_INPUT_WR_START,
    PTN_INPUT_WR_END,
    PTN_SELECT_WR_START,
    PTN_SELECT_WR_END,
    PTN_TEXTAREA_WR_START,
    PTN_TEXTAREA_WR_END,

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

static tt_result_t __render_param(IN tt_param_bs4content_t *ct,
                                  IN tt_param_t *p, IN tt_param_bs4level_t lv,
                                  OUT tt_buf_t *buf, IN const tt_char_t **ptn);

static tt_result_t __render_input(IN tt_param_bs4content_t *ct,
                                  IN tt_param_t *p, IN tt_param_bs4level_t lv,
                                  OUT tt_buf_t *buf, IN tt_param_bs4input_t *i,
                                  IN const tt_char_t **ptn);

static const tt_char_t *__input_type(IN tt_param_t *p);

static tt_result_t __render_select(IN tt_param_bs4content_t *ct,
                                   IN tt_param_t *p, IN tt_param_bs4level_t lv,
                                   OUT tt_buf_t *buf,
                                   IN tt_param_bs4select_t *s,
                                   IN const tt_char_t **ptn);

static tt_result_t __render_textarea(
    IN tt_param_bs4content_t *ct, IN tt_param_t *p, IN tt_param_bs4level_t lv,
    OUT tt_buf_t *buf, IN tt_param_bs4textarea_t *t, IN const tt_char_t **ptn);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_param_bs4ctrl_render(IN tt_param_bs4content_t *ct,
                                    IN tt_param_t *param,
                                    IN tt_param_bs4level_t lv,
                                    OUT tt_buf_t *buf)
{
    static const tt_char_t *ptn[PTN_NUM] = {
        __1ENTRY_INPUT_RD,        __1ENTRY_INPUT_WR_START,
        __1ENTRY_INPUT_WR_END,    __1ENTRY_SELECT_WR_START,
        __1ENTRY_SELECT_WR_END,   __1ENTRY_TEXTAREA_WR_START,
        __1ENTRY_TEXTAREA_WR_END,
    };

    TT_DO(__render_param(ct, param, lv, buf, ptn));

    return TT_SUCCESS;
}

tt_result_t tt_param_bs4ctrl_render_pair(IN tt_param_bs4content_t *ct,
                                         IN tt_param_t *p1, IN tt_param_t *p2,
                                         IN tt_param_bs4level_t lv,
                                         OUT tt_buf_t *buf)
{
    static const tt_char_t *ptn[PTN_NUM] = {
        __2ENTRY_INPUT_RD,        __2ENTRY_INPUT_WR_START,
        __2ENTRY_INPUT_WR_END,    __2ENTRY_SELECT_WR_START,
        __2ENTRY_SELECT_WR_END,   __2ENTRY_TEXTAREA_WR_START,
        __2ENTRY_TEXTAREA_WR_END,
    };

    TT_DO(__render_param(ct, p1, lv, buf, ptn));

    TT_DO(__render_param(ct, p2, lv, buf, ptn));

    return TT_SUCCESS;
}

tt_result_t __render_param(IN tt_param_bs4content_t *ct, IN tt_param_t *p,
                           IN tt_param_bs4level_t lv, OUT tt_buf_t *buf,
                           IN const tt_char_t **ptn)
{
    tt_param_bs4display_t disp;

    disp = tt_param_bs4display(p, lv);
    if (disp == TT_PARAM_BS4_DISP_RD) {
        // always show readonly param as an input control
        TT_DO(tt_buf_putf(buf, ptn[PTN_RD], ct->name_class, __param_display(p),
                          __WHOLE_LINE(p), ct->val_class, tt_param_name(p),
                          tt_param_tid(p)));
    } else if (disp == TT_PARAM_BS4_DISP_WR) {
        tt_param_bs4ctrl_t *ctrl;

        ctrl = &p->bs4_ctrl;
        if (ctrl->type == TT_PARAM_BS4_INPUT) {
            TT_DO(__render_input(ct, p, lv, buf, &ctrl->input, ptn));
        } else if (ctrl->type == TT_PARAM_BS4_SELECT) {
            TT_DO(__render_select(ct, p, lv, buf, &ctrl->select, ptn));
        } else if (ctrl->type == TT_PARAM_BS4_TEXTAREA) {
            TT_DO(__render_textarea(ct, p, lv, buf, &ctrl->textarea, ptn));
        }
    }

    return TT_SUCCESS;
}

tt_result_t __render_input(IN tt_param_bs4content_t *ct, IN tt_param_t *p,
                           IN tt_param_bs4level_t lv, OUT tt_buf_t *buf,
                           IN tt_param_bs4input_t *i, IN const tt_char_t **ptn)
{
    TT_DO(tt_buf_putf(buf, ptn[PTN_INPUT_WR_START], ct->name_class,
                      __param_display(p), __WHOLE_LINE(p), __input_type(p),
                      ct->val_class, tt_param_name(p), tt_param_tid(p)));

    if (i->pattern != NULL) {
        TT_DO(tt_buf_putf(buf, "pattern=\"%s\" ", i->pattern));
    }

    if (i->minlen[0] != 0) {
        TT_DO(tt_buf_putf(buf, "minlength=\"%s\" ", i->minlen));
    }

    if (i->maxlen[0] != 0) {
        TT_DO(tt_buf_putf(buf, "maxlength=\"%s\" ", i->maxlen));
    }

    if (i->min[0] != 0) { TT_DO(tt_buf_putf(buf, "min=\"%s\" ", i->min)); }

    if (i->max[0] != 0) { TT_DO(tt_buf_putf(buf, "max=\"%s\" ", i->max)); }

    if (i->step[0] != 0) { TT_DO(tt_buf_putf(buf, "step=\"%s\" ", i->step)); }

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

tt_result_t __render_select(IN tt_param_bs4content_t *ct, IN tt_param_t *p,
                            IN tt_param_bs4level_t lv, OUT tt_buf_t *buf,
                            IN tt_param_bs4select_t *s,
                            IN const tt_char_t **ptn)
{
    tt_u8_t n;

    TT_DO(tt_buf_putf(buf, ptn[PTN_SELECT_WR_START], ct->name_class,
                      __param_display(p), __WHOLE_LINE(p), ct->val_class,
                      tt_param_name(p), tt_param_tid(p)));

    if (s->selected != NULL) {
        TT_DO(tt_buf_putf(buf, "<option selected>%s</option>", s->selected));
    }

    for (n = 0; n < s->num; ++n) {
        TT_DO(tt_buf_putf(buf, "<option value=\"%s\">%s</option>", s->value[n],
                          s->display[n]));
    }

    TT_DO(tt_buf_put_cstr(buf, ptn[PTN_SELECT_WR_END]));

    return TT_SUCCESS;
}

tt_result_t __render_textarea(IN tt_param_bs4content_t *ct, IN tt_param_t *p,
                              IN tt_param_bs4level_t lv, OUT tt_buf_t *buf,
                              IN tt_param_bs4textarea_t *t,
                              IN const tt_char_t **ptn)
{
    tt_char_t rows[16] = {0};
    tt_u8_t n;

    if (t->rows[0] != 0) {
        tt_snprintf(rows, sizeof(rows), "rows=\"%s\"", t->rows);
    }

    TT_DO(tt_buf_putf(buf, ptn[PTN_TEXTAREA_WR_START], ct->name_class,
                      __param_display(p), __WHOLE_LINE(p), ct->val_class, rows,
                      tt_param_name(p), tt_param_tid(p)));

    TT_DO(tt_buf_put_cstr(buf, ptn[PTN_TEXTAREA_WR_END]));

    return TT_SUCCESS;
}
