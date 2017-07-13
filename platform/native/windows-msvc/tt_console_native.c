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

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_console_native.h>

#include <io/tt_console.h>
#include <log/tt_log.h>
#include <misc/tt_assert.h>

#include <tt_cstd_api.h>
#include <tt_sys_error.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __KEYCODE_LEN 4

#define __STDIN_SIZE 32

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef tt_result_t (*__send_extkey_t)();

typedef struct
{
    tt_char_t code[__KEYCODE_LEN];
    tt_u32_t code_len;
} __cons_keycode_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static HANDLE __stdin_hdl;
static DWORD __stdin_mode;

static HANDLE __stdout_hdl;
static DWORD __stdout_mode;

static HANDLE __stderr_hdl;
static DWORD __stderr_mode;

static tt_console_input_mode_t __cons_imode;
static tt_console_output_mode_t __cons_omode;
static tt_u8_t __cons_unprintable_subst;

static INPUT_RECORD __stdin_evbuf[__STDIN_SIZE];
static tt_u8_t __stdin_keybuf[__STDIN_SIZE];

static __cons_keycode_t __cons_keycode[TT_CONS_EXTKEY_NUM] = {
    {{'^', 'U'}, 2}, // TT_CONS_EXTKEY_UP
    {{'^', 'D'}, 2}, // TT_CONS_EXTKEY_DOWN
    {{'^', 'R'}, 2}, // TT_CONS_EXTKEY_RIGHT
    {{'^', 'L'}, 2}, // TT_CONS_EXTKEY_LEFT

    {{'^', 'C'}, 2}, // TT_CONS_EXTKEY_CTRLC
    {{'^', 'D'}, 2}, // TT_CONS_EXTKEY_CTRLD

    {{'\b', ' ', '\b'}, 3}, // TT_CONS_EXTKEY_DELETE
    {{'\r', '\n'}, 2}, // TT_CONS_EXTKEY_CRLF
    {{'^', 'T'}, 2}, // TT_CONS_EXTKEY_TAB
};

static tt_result_t __send_right();

static tt_result_t __send_left();

static __send_extkey_t __send_extkey[TT_CONS_EXTKEY_NUM] = {
    NULL, // TT_CONS_EXTKEY_UP
    NULL, // TT_CONS_EXTKEY_DOWN
    __send_right, // TT_CONS_EXTKEY_RIGHT
    __send_left, // TT_CONS_EXTKEY_LEFT

    NULL, // TT_CONS_EXTKEY_CTRLC
    NULL, // TT_CONS_EXTKEY_CTRLD

    NULL, // TT_CONS_EXTKEY_DELETE
    NULL, // TT_CONS_EXTKEY_CRLF
    NULL, // TT_CONS_EXTKEY_TAB
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __stdin_config();

static tt_u8_t __key_map_vk(IN WORD vk);

static tt_u8_t __key_map_ctrl(IN CHAR c);

static tt_u8_t __key_map_basic(IN CHAR c);

static tt_result_t __cons_send_key(IN tt_u8_t *key, IN tt_u32_t key_num);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_console_init_ntv()
{
    __stdin_hdl = GetStdHandle(STD_INPUT_HANDLE);
    if (__stdin_hdl == INVALID_HANDLE_VALUE) {
        TT_ERROR_NTV("fail to get stdin handle");
        return TT_FAIL;
    }
    if (!GetConsoleMode(__stdin_hdl, &__stdin_mode)) {
        TT_ERROR_NTV("fail to get stdin mode");
        return TT_FAIL;
    }

    __stdout_hdl = GetStdHandle(STD_OUTPUT_HANDLE);
    if (__stdout_hdl == INVALID_HANDLE_VALUE) {
        TT_ERROR_NTV("fail to get stdout handle");
        return TT_FAIL;
    }
    if (!GetConsoleMode(__stdout_hdl, &__stdout_mode)) {
        TT_ERROR_NTV("fail to get stdout mode");
        return TT_FAIL;
    }

    __stderr_hdl = GetStdHandle(STD_ERROR_HANDLE);
    if (__stderr_hdl == INVALID_HANDLE_VALUE) {
        TT_ERROR_NTV("fail to get stderr handle");
        return TT_FAIL;
    }
    if (!GetConsoleMode(__stderr_hdl, &__stderr_mode)) {
        TT_ERROR_NTV("fail to get stderr mode");
        return TT_FAIL;
    }

    __cons_imode = TT_CONSOLE_IMODE_DEFAULT;
    __cons_omode = TT_CONSOLE_OMODE_DEFAULT;
    __cons_unprintable_subst = '.';

    return TT_SUCCESS;
}

tt_result_t tt_console_enter_ntv()
{
    if (!TT_OK(__stdin_config())) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_console_exit_ntv()
{
    // restore stdin term
    if (!SetConsoleMode(__stdin_hdl, __stdin_mode)) {
        TT_ERROR_NTV("fail to set stdin mode");
    }
}

tt_result_t tt_console_config_ntv(IN struct tt_console_attr_s *attr)
{
    // mode
    __cons_imode = attr->input_mode;
    __cons_omode = attr->output_mode;

    // unprintable substitutor
    __cons_unprintable_subst = attr->unprintable_substitutor;
    TT_ASSERT(TT_CONS_KEY_IS_BASIC(__cons_unprintable_subst));

    return TT_SUCCESS;
}

tt_result_t tt_console_recv_ntv(OUT tt_cons_ev_t *ev,
                                OUT tt_cons_ev_data_t *ev_data)
{
    DWORD NumberOfEventsRead = 0;
    DWORD i;
    tt_u32_t key_num;

    if (!ReadConsoleInputA(__stdin_hdl,
                           __stdin_evbuf,
                           __STDIN_SIZE,
                           &NumberOfEventsRead) ||
        (NumberOfEventsRead == 0)) {
        TT_ERROR_NTV("fail to read console input");
        return TT_FAIL;
    }
    // TT_DEBUG("%d console events", NumberOfEventsRead);

    key_num = 0;
    for (i = 0; i < NumberOfEventsRead; ++i) {
        WORD EventType = __stdin_evbuf[i].EventType;
        if (EventType == KEY_EVENT) {
            KEY_EVENT_RECORD *ke = &__stdin_evbuf[i].Event.KeyEvent;
            tt_u8_t c;

            // TT_INFO("KEY_EVENT");
            // TT_INFO("bKeyDown: %d", ke->bKeyDown);
            // TT_INFO("wRepeatCount: %d", ke->wRepeatCount);
            // TT_INFO("AsciiChar: %d", ke->uChar.AsciiChar);
            // TT_INFO("dwControlKeyState: %d", ke->dwControlKeyState);

            // only care key down
            if (!ke->bKeyDown) {
                continue;
            }

            // check if can handle the virtual key
            c = __key_map_vk(ke->wVirtualKeyCode);
            if (!TT_CONS_KEY_IS_UNKNOWN(c)) {
                __stdin_keybuf[key_num++] = c;
                continue;
            }

            // as observed, keys that can not be represented by
            // ascii code will be set to 0...
            if (ke->uChar.AsciiChar == 0) {
                continue;
            }

            // handle ctrl events, note it should return even if
            // it can not handle it
            if ((ke->dwControlKeyState & LEFT_CTRL_PRESSED) ||
                (ke->dwControlKeyState & RIGHT_CTRL_PRESSED)) {
                c = __key_map_ctrl(ke->uChar.AsciiChar);
                if (!TT_CONS_KEY_IS_UNKNOWN(c)) {
                    __stdin_keybuf[key_num++] = c;
                }

                continue;
            }

            c = __key_map_basic(ke->uChar.AsciiChar);
            if (!TT_CONS_KEY_IS_UNKNOWN(c)) {
                __stdin_keybuf[key_num++] = c;
            } else {
                __stdin_keybuf[key_num++] = __cons_unprintable_subst;
            }
        }
    }

    *ev = TT_CONS_EV_KEY;
    ev_data->key.key = __stdin_keybuf;
    TT_ASSERT(key_num <= __STDIN_SIZE);
    ev_data->key.key_num = key_num;
    return TT_SUCCESS;
}

tt_result_t tt_console_send_ntv(IN tt_cons_ev_t ev,
                                IN tt_cons_ev_data_t *ev_data)
{
    switch (ev) {
        case TT_CONS_EV_KEY:
            return __cons_send_key(ev_data->key.key, ev_data->key.key_num);

        default:
            return TT_FAIL;
    }
}

tt_result_t __stdin_config()
{
    DWORD mode = __stdin_mode;

    mode &= ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT | ENABLE_MOUSE_INPUT |
              ENABLE_PROCESSED_INPUT | ENABLE_QUICK_EDIT_MODE |
              ENABLE_WINDOW_INPUT);
    mode |= ENABLE_INSERT_MODE;

    if (!SetConsoleMode(__stdin_hdl, mode)) {
        TT_ERROR_NTV("fail to set stdin mode");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_u8_t __key_map_vk(IN WORD vk)
{
    switch (vk) {
        case VK_LEFT:
            return TT_CONS_EXTKEY_LEFT;
        case VK_RIGHT:
            return TT_CONS_EXTKEY_RIGHT;
        case VK_UP:
            return TT_CONS_EXTKEY_UP;
        case VK_DOWN:
            return TT_CONS_EXTKEY_DOWN;

        default:
            return TT_CONS_EXTKEY_END;
    }
}

tt_u8_t __key_map_ctrl(IN CHAR c)
{
    switch (c) {
        case 0x3:
            return TT_CONS_EXTKEY_CTRLC;
        case 0x4:
            return TT_CONS_EXTKEY_CTRLD;

        default:
            return TT_CONS_EXTKEY_END;
    }
}

tt_u8_t __key_map_basic(IN CHAR c)
{
    if ((c >= 0x20) && (c <= 0x7e)) {
        return c;
    }

    switch (c) {
        case 0x08:
        case 0x7f:
            return TT_CONS_EXTKEY_DELETE;

        case 0x0a:
        case 0x0d:
            return TT_CONS_EXTKEY_CRLF;

        case 0x09:
            return TT_CONS_EXTKEY_TAB;

        default:
            return TT_CONS_EXTKEY_END;
    }
}

tt_result_t __cons_send_key(IN tt_u8_t *key, IN tt_u32_t key_num)
{
    tt_u32_t head, pos;
    DWORD NumberOfCharsWritten;

#define __output(p, len)                                                       \
    do {                                                                       \
        if (pos > head) {                                                      \
            WriteConsoleA(__stdout_hdl,                                        \
                          &key[head],                                          \
                          pos - head,                                          \
                          &NumberOfCharsWritten,                               \
                          NULL);                                               \
        }                                                                      \
                                                                               \
        if (p != NULL) {                                                       \
            WriteConsoleA(__stdout_hdl,                                        \
                          (p),                                                 \
                          (len),                                               \
                          &NumberOfCharsWritten,                               \
                          NULL);                                               \
        }                                                                      \
                                                                               \
        ++pos;                                                                 \
        head = pos;                                                            \
    } while (0)

    head = 0;
    pos = 0;
    while (pos < key_num) {
        tt_u8_t k = key[pos];
        if (TT_CONS_KEY_IS_BASIC(k)) {
            ++pos;
        } else if (TT_CONS_KEY_IS_EXTENDED(k)) {
            tt_u32_t i = TT_CONS_EXTKEY_IDX(k);

            if (__send_extkey[i] != NULL) {
                __output(NULL, 0);
                __send_extkey[i]();
            } else {
                __cons_keycode_t *kc = &__cons_keycode[i];
                __output(kc->code, kc->code_len);
            }
        } else {
            __output(&__cons_unprintable_subst, 1);
        }
    }
    if (pos > head) {
        WriteConsoleA(__stdout_hdl,
                      &key[head],
                      pos - head,
                      &NumberOfCharsWritten,
                      NULL);
    }
#undef __output

    return TT_SUCCESS;
}

tt_result_t __send_right()
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD *pos;

    if (!GetConsoleScreenBufferInfo(__stdout_hdl, &csbi)) {
        TT_ERROR("fail to get console info");
        return TT_FAIL;
    }

    pos = &csbi.dwCursorPosition;
    if (pos->X < csbi.dwSize.X) {
        ++pos->X;
    }

    if (!SetConsoleCursorPosition(__stdout_hdl, *pos)) {
        TT_ERROR("fail to set curso pos");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t __send_left()
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD *pos;

    if (!GetConsoleScreenBufferInfo(__stdout_hdl, &csbi)) {
        TT_ERROR("fail to get console info");
        return TT_FAIL;
    }

    pos = &csbi.dwCursorPosition;
    if (pos->X > 0) {
        --pos->X;
    }

    if (!SetConsoleCursorPosition(__stdout_hdl, *pos)) {
        TT_ERROR("fail to set curso pos");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}
