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

/**
@file tt_cli_line.h
@brief command line editor

this file defines command line editor interface
*/

#ifndef __TT_CLI_LINE__
#define __TT_CLI_LINE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_string.h>
#include <cli/tt_cli_def.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_cline_s;

typedef tt_result_t (*tt_cline_on_char_t)(IN struct tt_cline_s *cline,
                                          IN tt_char_t c);

typedef tt_result_t (*tt_cline_on_ev_t)(IN struct tt_cline_s *cline);

typedef struct tt_cline_s
{
    tt_buf_t *outbuf;
    tt_cline_on_char_t on_char;
    tt_cline_on_ev_t on_ev[TT_CLI_EV_NUM];
    tt_string_t line;
    tt_cli_mode_t mode;
    tt_u32_t cursor;
} tt_cline_t;

typedef struct
{
    tt_string_attr_t line_attr;
} tt_cline_attr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_result_t tt_cline_create(IN tt_cline_t *cline,
                                      IN tt_cli_mode_t mode,
                                      IN tt_buf_t *outbuf,
                                      IN OPT tt_cline_attr_t *attr);

tt_export void tt_cline_destroy(IN tt_cline_t *cline);

tt_export void tt_cline_attr_default(IN tt_cline_attr_t *attr);

tt_export tt_result_t tt_cline_input(IN tt_cline_t *cline,
                                     IN tt_u8_t *ev,
                                     IN tt_u32_t ev_num);

tt_inline tt_result_t tt_cline_input_ev(IN tt_cline_t *cline, IN tt_u8_t ev)
{
    return tt_cline_input(cline, &ev, 1);
}

tt_inline const tt_char_t *tt_cline_cstr(IN tt_cline_t *cline)
{
    return tt_string_cstr(&cline->line);
}

tt_inline void tt_cline_reset(IN tt_cline_t *cline)
{
    tt_string_clear(&cline->line);
    cline->cursor = 0;
}

// - return true if cline is waiting for a command: # ad[]
// - len in @ref data may be 0 when: # [] / # add []
tt_export tt_bool_t tt_cline_cursor_data(IN tt_cline_t *cline,
                                         OUT tt_blob_t *data);

#endif /* __TT_CLI_LINE__ */
