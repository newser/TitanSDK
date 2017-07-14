/* Licensed to the Apache Software Foundation (ASF) under one or more
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
@file tt_cli.h
@brief command line interface

this file defines command line interface APIs
*/

#ifndef __TT_CLI__
#define __TT_CLI__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <cli/tt_cli_line.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_cli_s;

typedef tt_result_t (*tt_cli_on_ev_t)(IN struct tt_cli_s *cli);

typedef tt_result_t (*tt_cli_newline_t)(IN struct tt_cli_s *cli,
                                        IN tt_bool_t crlf,
                                        IN tt_bool_t prefix,
                                        IN tt_bool_t line);

// return true to continue reading
typedef tt_u32_t (*tt_cli_on_read_t)(IN struct tt_cli_s *cli,
                                     IN const tt_char_t *content,
                                     IN tt_buf_t *output);
#define TT_CLIOR_DONE 0
#define TT_CLIOR_MORE 1
#define TT_CLIOR_END 2

typedef struct
{
    const tt_char_t *title;
    const tt_char_t *sub_title;
    tt_char_t seperator;

    tt_buf_attr_t outbuf_attr;
    tt_cline_attr_t cline_attr;
} tt_cli_attr_t;

typedef struct tt_cli_s
{
    tt_cli_cb_t cb;
    tt_cli_itf_t itf;
    tt_cli_on_ev_t on_ev[TT_CLI_EV_NUM];
    tt_cli_newline_t newline;
    tt_cli_on_read_t on_read;

    const tt_char_t *title;
    const tt_char_t *sub_title;
    tt_string_t prefix;

    tt_buf_t outbuf;
    tt_buf_t acbuf;
    tt_cline_t cline;

    tt_char_t seperator;
    tt_u8_t read_mode : 2;
#define __CLI_NOT_READ 0
#define __CLI_READ_LINE 1
} tt_cli_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_cli_create(IN tt_cli_t *cli,
                                 IN tt_cli_mode_t mode,
                                 IN tt_cli_cb_t *cb,
                                 IN tt_cli_itf_t *itf,
                                 IN OPT tt_cli_attr_t *attr);

extern void tt_cli_destroy(IN tt_cli_t *cli);

extern void tt_cli_attr_default(IN tt_cli_attr_t *attr);

extern tt_result_t tt_cli_start(IN tt_cli_t *cli);

extern tt_result_t tt_cli_update_prefix(IN tt_cli_t *cli,
                                        IN OPT const tt_char_t *title,
                                        IN OPT const tt_char_t *sub_title,
                                        IN OPT tt_char_t seperator);

// return TT_END, when cli should exit
extern tt_result_t tt_cli_input(IN tt_cli_t *cli,
                                IN tt_u8_t *ev,
                                IN tt_u32_t ev_num);

tt_inline tt_result_t tt_cli_input_ev(IN tt_cli_t *cli, IN tt_u8_t ev)
{
    return tt_cli_input(cli, &ev, 1);
}

tt_inline void tt_cli_read_line(IN tt_cli_t *cli,
                                IN OPT tt_cli_on_read_t on_read)
{
    cli->read_mode = __CLI_READ_LINE;
    cli->on_read = on_read;
}

extern tt_result_t tt_cli_output(IN tt_cli_t *cli,
                                 IN tt_u8_t *ev,
                                 IN tt_u32_t ev_num);

extern void tt_cli_set_cb(IN tt_cli_t *cli, IN OPT tt_cli_cb_t *cb);

extern void tt_cli_set_itf(IN tt_cli_t *cli, IN OPT tt_cli_itf_t *itf);

extern tt_result_t tt_cli_complete(IN tt_blob_t *cursor_data,
                                   IN const tt_char_t **option,
                                   IN tt_u32_t option_num,
                                   OUT tt_u32_t *status,
                                   OUT tt_buf_t *outbuf);

#endif /* __TT_CLI__ */
