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

#include <cli/tt_cli_line.h>

#include <algorithm/tt_buffer_format.h>
#include <algorithm/tt_string_common.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
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

static void __load_cline_cb(IN tt_cline_t *cline, IN tt_cli_mode_t mode);

// ========================================
// default mode
// ========================================

static tt_result_t __def_on_char(IN tt_cline_t *cline, IN tt_char_t c);

static tt_result_t __def_on_left(IN tt_cline_t *cline);

static tt_result_t __def_on_right(IN tt_cline_t *cline);

static tt_result_t __def_on_intr(IN tt_cline_t *cline);

static tt_result_t __def_on_quit(IN tt_cline_t *cline);

static tt_result_t __def_on_delete(IN tt_cline_t *cline);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_cline_create(IN tt_cline_t *cline, IN tt_cli_mode_t mode,
                            IN tt_buf_t *outbuf, IN OPT tt_cline_attr_t *attr)
{
    tt_cline_attr_t __attr;

    TT_ASSERT(cline != NULL);
    TT_ASSERT(TT_CLI_MODE_VALID(mode));
    TT_ASSERT(outbuf != NULL);

    if (attr == NULL) {
        tt_cline_attr_default(&__attr);
        attr = &__attr;
    }

    cline->outbuf = outbuf;
    __load_cline_cb(cline, mode);
    tt_string_init(&cline->line, &attr->line_attr);
    cline->mode = mode;
    cline->cursor = 0;

    return TT_SUCCESS;
}

void tt_cline_destroy(IN tt_cline_t *cline)
{
    TT_ASSERT(cline != NULL);

    tt_string_destroy(&cline->line);
}

void tt_cline_attr_default(IN tt_cline_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    tt_string_attr_default(&attr->line_attr);
}

tt_result_t tt_cline_input(IN tt_cline_t *cline, IN tt_u8_t *ev,
                           IN tt_u32_t ev_num)
{
    tt_u32_t i;

    TT_ASSERT(cline != NULL);
    TT_ASSERT(ev != NULL);

    if (ev_num == 0) { return TT_SUCCESS; }

    for (i = 0; i < ev_num; ++i) {
        tt_u8_t e = ev[i];

        if ((e >= 0x20) && (e <= 0x7e)) {
            if ((cline->on_char != NULL) && !TT_OK(cline->on_char(cline, e))) {
                TT_WARN("fail to process cli char[%c]", e);
            }
        } else if (TT_CLI_EV_VALID(e)) {
            tt_cline_on_ev_t on_ev = cline->on_ev[TT_CLI_EV_IDX(e)];
            if ((on_ev != NULL) && !TT_OK(on_ev(cline))) {
                TT_WARN("fail to process cli ev[0x%x]", e);
            }
        } else {
            TT_WARN("discard unrecognized cli ev[0x%x]", e);
        }
    }

    return TT_SUCCESS;
}

tt_bool_t tt_cline_cursor_data(IN tt_cline_t *cline, OUT tt_blob_t *data)
{
    tt_u32_t i, cursor = cline->cursor;
    const tt_char_t *p = tt_string_cstr(&cline->line);
    tt_bool_t see_char = TT_FALSE, see_sp = TT_FALSE;

    TT_ASSERT(cursor <= tt_string_len(&cline->line));

    i = 0;
    while ((i < cursor) && (p[i] == ' ')) { ++i; }
    while ((i < cursor) && (p[i] != ' ')) {
        see_char = TT_TRUE;
        ++i;
    }
    while ((i < cursor) && (p[i] == ' ')) {
        see_sp = TT_TRUE;
        ++i;
    }

    i = cursor - 1;
    while ((i != ~0) && (p[i] != ' ')) { --i; }
    ++i;
    TT_ASSERT(i <= cursor);
    if (i < cursor) {
        data->addr = (tt_u8_t *)&p[i];
        data->len = cursor - i;
    } else {
        data->addr = NULL;
        data->len = 0;
    }

    // see both char and space means not a cmd
    return !(see_char && see_sp);
}

void __load_cline_cb(IN tt_cline_t *cline, IN tt_cli_mode_t mode)
{
    TT_ASSERT(TT_CLI_MODE_VALID(mode));

    switch (mode) {
    case TT_CLI_MODE_DEFAUTL:
    default: {
        cline->on_char = __def_on_char;
        cline->on_ev[TT_CLI_EV_IDX(TT_CLI_EV_UP)] = NULL;
        cline->on_ev[TT_CLI_EV_IDX(TT_CLI_EV_DOWN)] = NULL;
        cline->on_ev[TT_CLI_EV_IDX(TT_CLI_EV_RIGHT)] = __def_on_right;
        cline->on_ev[TT_CLI_EV_IDX(TT_CLI_EV_LEFT)] = __def_on_left;
        cline->on_ev[TT_CLI_EV_IDX(TT_CLI_EV_INTR)] = __def_on_intr;
        cline->on_ev[TT_CLI_EV_IDX(TT_CLI_EV_QUIT)] = __def_on_quit;
        cline->on_ev[TT_CLI_EV_IDX(TT_CLI_EV_DELETE)] = __def_on_delete;
        cline->on_ev[TT_CLI_EV_IDX(TT_CLI_EV_ENTER)] = NULL;
        cline->on_ev[TT_CLI_EV_IDX(TT_CLI_EV_TAB)] = NULL;
    } break;
    }
}

// ========================================
// default mode
// ========================================

tt_result_t __def_on_char(IN tt_cline_t *cline, IN tt_char_t c)
{
    tt_string_t *line = &cline->line;
    tt_u32_t len = tt_string_len(line);

    TT_ASSERT(cline->cursor <= len);
    if (cline->cursor < len) {
        tt_u8_t *tail;
        tt_u32_t tail_len;

        TT_DO(tt_string_insert_c(line, cline->cursor, c));
        ++cline->cursor;

        // reprint following chars as in default mode, following chars
        // won't be moved automatically
        tail = (tt_u8_t *)tt_string_subcstr(line, cline->cursor, &tail_len);
        TT_DO(tt_buf_put_u8(cline->outbuf, c));
        TT_DO(tt_buf_put(cline->outbuf, tail, tail_len));
        TT_DO(tt_buf_put_rep(cline->outbuf, TT_CLI_EV_LEFT, tail_len));
    } else {
        TT_DO(tt_string_append_c(line, c));
        ++cline->cursor;

        TT_DO(tt_buf_put_u8(cline->outbuf, c));
    }

    return TT_SUCCESS;
}

tt_result_t __def_on_left(IN tt_cline_t *cline)
{
    if (cline->cursor == 0) { return TT_SUCCESS; }

    TT_DO(tt_buf_put_u8(cline->outbuf, TT_CLI_EV_LEFT));
    --cline->cursor;

    return TT_SUCCESS;
}

tt_result_t __def_on_right(IN tt_cline_t *cline)
{
    if (cline->cursor == tt_string_len(&cline->line)) { return TT_SUCCESS; }
    TT_ASSERT(cline->cursor < tt_string_len(&cline->line));

    TT_DO(tt_buf_put_u8(cline->outbuf, TT_CLI_EV_RIGHT));
    ++cline->cursor;

    return TT_SUCCESS;
}

tt_result_t __def_on_intr(IN tt_cline_t *cline)
{
    tt_u8_t c[2] = {'^', 'C'};

    TT_DO(tt_buf_put(cline->outbuf, c, sizeof(c)));

    return TT_SUCCESS;
}

tt_result_t __def_on_quit(IN tt_cline_t *cline)
{
    tt_u8_t c[2] = {'^', 'D'};

    TT_DO(tt_buf_put(cline->outbuf, c, sizeof(c)));

    return TT_SUCCESS;
}

tt_result_t __def_on_delete(IN tt_cline_t *cline)
{
    tt_string_t *line = &cline->line;
    tt_buf_t *outbuf = cline->outbuf;
    tt_u8_t *p;
    tt_u32_t n;

    if (cline->cursor == 0) { return TT_SUCCESS; }

    // update line
    --cline->cursor;
    tt_string_remove_range(line, cline->cursor, 1);

    // update output:
    // delete char left to cursor
    TT_DO(tt_buf_put_u8(outbuf, TT_CLI_EV_DELETE));
    // print following char
    p = (tt_u8_t *)tt_string_subcstr(line, cline->cursor, &n);
    if (n > 0) {
        TT_DO(tt_buf_put(outbuf, p, n));
        // delete ending char
        TT_DO(tt_buf_put_u8(outbuf, ' '));
        // back to original cursor
        TT_DO(tt_buf_put_rep(outbuf, TT_CLI_EV_LEFT, n + 1));
    }

    return TT_SUCCESS;
}
