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
@file tt_console.h
@brief console io

this file specifies console APIs

console api should be able to read:
- [basic] all printable character
- [ext] backspace, delete, enter
- [ext] 4 arrow keys
- [ext] ctrl+c, ctrl+d

console api should be able to write:
- [basic] all printable character
- [ext] backspace, delete, enter
*/

#ifndef __TT_CONSOLE__
#define __TT_CONSOLE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <io/tt_console_event.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef enum {
    TT_CONSOLE_IMODE_DEFAULT,
    TT_CONSOLE_IMODE_RAW,

    TT_CONSOLE_IMODE_NUM
} tt_console_input_mode_t;
#define TT_CONSOLE_IMODE_VALID(m) ((m) < TT_CONSOLE_IMODE_NUM)

typedef enum {
    TT_CONSOLE_OMODE_DEFAULT,
    TT_CONSOLE_OMODE_RAW,

    TT_CONSOLE_OMODE_NUM
} tt_console_output_mode_t;
#define TT_CONSOLE_OMODE_VALID(m) ((m) < TT_CONSOLE_OMODE_NUM)

typedef struct tt_console_attr_s
{
    tt_console_input_mode_t input_mode;
    tt_console_output_mode_t output_mode;
    tt_char_t unprintable_substitutor;
} tt_console_attr_t;

/*
 @return
 - TT_SUCCESS, event processed
 - TT_END, should quit
 - otherwise, error occured
 */
typedef tt_result_t (*tt_console_ev_handler_t)(IN void *param,
                                               IN tt_cons_ev_t ev,
                                               IN tt_cons_ev_data_t *ev_data);

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern void tt_console_component_register();

extern void tt_console_attr_default(IN tt_console_attr_t *attr);

extern tt_result_t tt_console_config(IN tt_console_attr_t *attr);

extern void tt_console_run(IN tt_console_ev_handler_t ev_handler,
                           IN void *param,
                           IN tt_bool_t local);

extern tt_result_t tt_console_send(IN tt_cons_ev_t ev,
                                   IN tt_cons_ev_data_t *ev_data);

tt_inline tt_result_t tt_console_send_key(IN tt_u8_t *key, IN tt_u32_t key_num)
{
    if (key_num > 0) {
        tt_cons_ev_data_t ev_data;

        ev_data.key.key = key;
        ev_data.key.key_num = key_num;
        return tt_console_send(TT_CONS_EV_KEY, &ev_data);
    } else {
        return TT_SUCCESS;
    }
}

#endif // __TT_CONSOLE__
