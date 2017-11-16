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
@file tt_cli_def.h
@brief command line interface definitions

this file defines command line interface
*/

#ifndef __TT_CLI_DEF__
#define __TT_CLI_DEF__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_CLI_EV_NUM (TT_CLI_EV_END - TT_CLI_EV_START)

#define TT_CLI_EV_IDX(ev) ((ev)-TT_CLI_EV_START)

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_cli_s;
struct tt_buf_s;

typedef enum tt_cli_ev_s {
    TT_CLI_EV_START = 0x80,

    TT_CLI_EV_UP = TT_CLI_EV_START,
    TT_CLI_EV_DOWN,
    TT_CLI_EV_RIGHT,
    TT_CLI_EV_LEFT,
    TT_CLI_EV_INTR,
    TT_CLI_EV_QUIT,
    TT_CLI_EV_DELETE,
    TT_CLI_EV_ENTER,
    TT_CLI_EV_TAB,

    TT_CLI_EV_END
} tt_cli_ev_t;
#define TT_CLI_EV_VALID(k) (((k) >= TT_CLI_EV_START) && ((k) < TT_CLI_EV_END))

typedef enum tt_cli_mode_s {
    // default mode requirement
    //  - won't echo anything
    //  - output a char won't move following chars
    //  - output a char would also move cursor to right
    //  - output delete would also move cursor to left
    //  - output enter is crlf
    TT_CLI_MODE_DEFAUTL,

    TT_CLI_MODE_NUM
} tt_cli_mode_t;
#define TT_CLI_MODE_VALID(m) ((m) < TT_CLI_MODE_NUM)

// NULL cmd means cli would exit
typedef tt_u32_t (*tt_cli_on_cmd_t)(IN struct tt_cli_s *cli,
                                    IN void *param,
                                    IN const tt_char_t *cmd,
                                    IN struct tt_buf_s *output);
// returned flags
#define TT_CLIOC_NOOUT 0
#define TT_CLIOC_OUT 1
#define TT_CLIOC_END 2

// - return TT_CLICP_NONE, output includes candidates, but may be empty if no
//   candidate
// - return TT_CLICP_FULL when fully matched, output includes completed content
//   but also may be null if nothing need be completed
// - return TT_CLICP_PARTIAL when something could be completed and stored in
//   output
typedef tt_u32_t (*tt_cli_on_complete_t)(IN struct tt_cli_s *cli,
                                         IN void *param,
                                         IN tt_u8_t *cur,
                                         IN tt_u32_t cur_len,
                                         IN tt_bool_t wait4cmd,
                                         IN struct tt_buf_s *output);
#define TT_CLICP_NONE 0
#define TT_CLICP_PARTIAL 1
#define TT_CLICP_FULL 2
#define TT_CLICP_FULL_MORE 3

// return true if has output
typedef tt_bool_t (*tt_cli_on_quit_t)(IN struct tt_cli_s *cli,
                                      IN struct tt_buf_s *output);

typedef struct
{
    void *param;

    tt_cli_on_cmd_t on_cmd;
    tt_cli_on_complete_t on_complete;
    tt_cli_on_quit_t on_quit;
} tt_cli_cb_t;

typedef tt_result_t (*tt_cli_send_t)(IN struct tt_cli_s *cli,
                                     IN void *param,
                                     IN tt_u8_t *ev,
                                     IN tt_u32_t ev_num);

typedef struct
{
    void *param;

    tt_cli_send_t send;
} tt_cli_itf_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif /* __TT_CLI_DEF__ */
