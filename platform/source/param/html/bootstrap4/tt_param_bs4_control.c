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

#include <param/html/bootstrap4/tt_param_bs4_control.h>

#include <misc/tt_assert.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// internal macro
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

void tt_param_bs4ctrl_init(IN tt_param_bs4ctrl_t *ctrl)
{
    tt_memset(ctrl, 0, sizeof(tt_param_bs4ctrl_t));

    ctrl->type = TT_PARAM_BS4_INPUT;
}

void tt_param_bs4ctrl_set_type(IN tt_param_bs4ctrl_t *ctrl,
                               IN tt_param_bs4type_t type)
{
    TT_ASSERT(TT_PARAM_BS4_TYPE_VALID(type));
    ctrl->type = type;
}

// ========================================
// param bs4 control: input
// ========================================

void tt_param_bs4input_init(IN tt_param_bs4input_t *i)
{
    tt_memset(i, 0, sizeof(tt_param_bs4input_t));
}

void tt_param_bs4input_set_pattern(IN tt_param_bs4input_t *i,
                                   IN const tt_char_t *pattern)
{
    i->pattern = pattern;
}

void tt_param_bs4input_clear_pattern(IN tt_param_bs4input_t *i)
{
    i->pattern = NULL;
}

void tt_param_bs4input_set_min(IN tt_param_bs4input_t *i, IN tt_s32_t min)
{
    tt_memset(i->min, 0, sizeof(i->min));
    tt_snprintf(i->min, sizeof(i->min) - 1, "%d", min);
}

void tt_param_bs4input_clear_min(IN tt_param_bs4input_t *i)
{
    i->min[0] = 0;
}

void tt_param_bs4input_set_max(IN tt_param_bs4input_t *i, IN tt_s32_t max)
{
    tt_memset(i->max, 0, sizeof(i->max));
    tt_snprintf(i->max, sizeof(i->max) - 1, "%d", max);
}

void tt_param_bs4input_clear_max(IN tt_param_bs4input_t *i)
{
    i->max[0] = 0;
}

void tt_param_bs4input_set_minlen(IN tt_param_bs4input_t *i, IN tt_u32_t len)
{
    TT_ASSERT(len <= 99999);

    tt_memset(i->minlen, 0, sizeof(i->minlen));
    tt_snprintf(i->minlen, sizeof(i->minlen) - 1, "%u", len);
}

void tt_param_bs4input_clear_minlen(IN tt_param_bs4input_t *i)
{
    i->minlen[0] = 0;
}

void tt_param_bs4input_set_maxlen(IN tt_param_bs4input_t *i, IN tt_u32_t len)
{
    TT_ASSERT(len <= 99999);
    tt_memset(i->maxlen, 0, sizeof(i->maxlen));
    tt_snprintf(i->maxlen, sizeof(i->maxlen) - 1, "%u", len);
}

void tt_param_bs4input_clear_maxlen(IN tt_param_bs4input_t *i)
{
    i->maxlen[0] = 0;
}

void tt_param_bs4input_set_step(IN tt_param_bs4input_t *i, IN tt_u8_t step)
{
    if (step > 0) {
        tt_u8_t n;

        TT_ASSERT(step <= 5); // "0.00001"

        i->step[0] = '0';
        i->step[1] = '.';
        for (n = 2; n <= step; ++n) { i->step[n] = '0'; }
        TT_ASSERT(n <= 7);
        i->step[n++] = '1';
        i->step[n] = 0;
    } else {
        tt_param_bs4input_clear_step(i);
    }
}

void tt_param_bs4input_clear_step(IN tt_param_bs4input_t *i)
{
    i->step[0] = 0;
}

void tt_param_bs4input_clear(IN tt_param_bs4input_t *i)
{
    tt_param_bs4input_clear_pattern(i);
    tt_param_bs4input_clear_min(i);
    tt_param_bs4input_clear_max(i);
    tt_param_bs4input_clear_minlen(i);
    tt_param_bs4input_clear_maxlen(i);
    tt_param_bs4input_clear_step(i);
}

// ========================================
// param bs4 control: select
// ========================================

void tt_param_bs4select_init(IN tt_param_bs4select_t *s)
{
    tt_memset(s, 0, sizeof(tt_param_bs4select_t));
}

void tt_param_bs4select_set_selected(IN tt_param_bs4select_t *s,
                                     IN const tt_char_t *selected)
{
    s->selected = selected;
}

void tt_param_bs4select_set_option(IN tt_param_bs4select_t *s,
                                   IN const tt_char_t **value,
                                   IN const tt_char_t **display, IN tt_u8_t num)
{
    s->value = value;
    s->display = display;
    s->num = num;
}

void tt_param_bs4select_set_multiple(IN tt_param_bs4select_t *s,
                                     IN tt_bool_t multiple)
{
    s->multiple = multiple;
}

void tt_param_bs4select_clear(IN tt_param_bs4select_t *s)
{
    tt_param_bs4select_init(s);
}

// ========================================
// param bs4 control: textarea
// ========================================

void tt_param_bs4textarea_set_rows(IN tt_param_bs4textarea_t *t,
                                   IN tt_u8_t rows)
{
    tt_snprintf(t->rows, sizeof(t->rows), "%u", rows);
}
