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
@file tt_console_event.h
@brief console event

this file defines console event
*/

#ifndef __TT_CONSOLE_EVENT__
#define __TT_CONSOLE_EVENT__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

// key from 0 to 255 are divided into:
//  - basic:    [0, 127]
//  - extended: [128, TT_CONS_EXTKEY_END)
//  - unknown:  [TT_CONS_EXTKEY_END, 255]
#define TT_CONS_KEY_IS_BASIC(k) ((k) < TT_CONS_EXTKEY_START)
#define TT_CONS_KEY_IS_EXTENDED(k) TT_CONS_EXTKEY_VALID(k)
#define TT_CONS_KEY_IS_UNKNOWN(k) ((k) >= TT_CONS_EXTKEY_END)

// basic
#define TT_CONS_BASKEY_NUM (TT_CONS_EXTKEY_START)

// extended
#define TT_CONS_EXTKEY_NUM (TT_CONS_EXTKEY_END - TT_CONS_EXTKEY_START)
#define TT_CONS_EXTKEY_IDX(k) ((k)-TT_CONS_EXTKEY_START)

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

// ========================================
// console event: key
// ========================================

enum
{
    TT_CONS_EXTKEY_START = 0x80,

    TT_CONS_EXTKEY_UP = TT_CONS_EXTKEY_START,
    TT_CONS_EXTKEY_DOWN,
    TT_CONS_EXTKEY_RIGHT,
    TT_CONS_EXTKEY_LEFT,

    TT_CONS_EXTKEY_CTRLC,
    TT_CONS_EXTKEY_CTRLD,

    TT_CONS_EXTKEY_DELETE,
    TT_CONS_EXTKEY_CRLF,
    TT_CONS_EXTKEY_TAB,

    TT_CONS_EXTKEY_END
};
#define TT_CONS_EXTKEY_VALID(k)                                                \
    (((k) >= TT_CONS_EXTKEY_START) && ((k) < TT_CONS_EXTKEY_END))

typedef struct tt_cons_ev_key_s
{
    tt_u8_t *key;
    tt_u32_t key_num;
} tt_cons_ev_key_t;

// ========================================
// console event
// ========================================

typedef enum tt_cons_ev_s {
    TT_CONS_EV_KEY,

    TT_CONS_EV_NUM
} tt_cons_ev_t;
#define TT_CONS_EV_VALID(e) ((e) < TT_CONS_EV_NUM)

typedef union tt_cons_ev_data_s
{
    tt_cons_ev_key_t key;
} tt_cons_ev_data_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif // __TT_CONSOLE_EVENT__
