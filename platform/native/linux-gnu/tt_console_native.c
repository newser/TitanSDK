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

#include <tt_console_native.h>

#include <io/tt_console.h>
#include <misc/tt_assert.h>

#include <tt_cstd_api.h>
#include <tt_sys_error.h>

#include <termios.h>
#include <unistd.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __KEYCODE_LEN 4

#define __STDIN_BUF_SIZE 512

#define __STDIN_CTRL_C 0x3
#define __STDIN_CTRL_D 0x4

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef tt_result_t (*__cons_recv_t)(OUT tt_cons_ev_t *ev,
                                     OUT tt_cons_ev_data_t *ev_data);

typedef tt_result_t (*__cons_send_t)(IN tt_cons_ev_t ev,
                                     IN tt_cons_ev_data_t *ev_data);

typedef struct
{
    __cons_recv_t recv;
    __cons_send_t send;
} __cons_itf_t;

typedef struct
{
    tt_u8_t code[__KEYCODE_LEN];
    tt_u32_t len;
    tt_bool_t oneof : 1;
} __cons_keycode_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_console_input_mode_t __cons_imode;
static tt_console_output_mode_t __cons_omode;
static __cons_itf_t __cons_itf;

static tt_u8_t __cons_unprintable_subst;

static struct termios __stdin_termios;

static tt_u8_t __stdin_rbuf[__STDIN_BUF_SIZE];
static tt_u8_t __stdin_mbuf[__STDIN_BUF_SIZE];

// ========================================
// default mode
// ========================================

static __cons_keycode_t __def_ikeycode[TT_CONS_EXTKEY_NUM] = {
    {{0x1b, 0x5b, 0x41}, 3, TT_FALSE}, // TT_CONS_EXTKEY_UP
    {{0x1b, 0x5b, 0x42}, 3, TT_FALSE}, // TT_CONS_EXTKEY_DOWN
    {{0x1b, 0x5b, 0x43}, 3, TT_FALSE}, // TT_CONS_EXTKEY_RIGHT
    {{0x1b, 0x5b, 0x44}, 3, TT_FALSE}, // TT_CONS_EXTKEY_LEFT

    {{__STDIN_CTRL_C}, 1}, // TT_CONS_EXTKEY_CTRLC
    {{__STDIN_CTRL_D}, 1}, // TT_CONS_EXTKEY_CTRLD

    {{0x08, 0x7f}, 2, TT_TRUE}, // TT_CONS_EXTKEY_DELETE
    {{0x0a, 0x0d}, 2, TT_TRUE}, // TT_CONS_EXTKEY_CRLF
    {{0x09}, 1, TT_TRUE}, // TT_CONS_EXTKEY_TAB
};

static __cons_keycode_t __def_okeycode[TT_CONS_EXTKEY_NUM] = {
    {{0x1b, 0x5b, 0x41}, 3, TT_FALSE}, // TT_CONS_EXTKEY_UP
    {{0x1b, 0x5b, 0x42}, 3, TT_FALSE}, // TT_CONS_EXTKEY_DOWN
    {{0x1b, 0x5b, 0x43}, 3, TT_FALSE}, // TT_CONS_EXTKEY_RIGHT
    {{0x1b, 0x5b, 0x44}, 3, TT_FALSE}, // TT_CONS_EXTKEY_LEFT

    {{'^', 'C'}, 2, TT_FALSE}, // TT_CONS_EXTKEY_CTRLC
    {{'^', 'D'}, 2, TT_FALSE}, // TT_CONS_EXTKEY_CTRLD

    {{'\b', ' ', '\b'}, 3, TT_FALSE}, // TT_CONS_EXTKEY_DELETE
    {{'\r', '\n'}, 2, TT_FALSE}, // TT_CONS_EXTKEY_CRLF
    {{'^', 'T'}, 2, TT_FALSE}, // TT_CONS_EXTKEY_TAB
};

static tt_result_t __def_recv(OUT tt_cons_ev_t *ev,
                              OUT tt_cons_ev_data_t *ev_data);

static tt_result_t __def_send(IN tt_cons_ev_t ev,
                              IN tt_cons_ev_data_t *ev_data);

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static void __install_cons_itf(IN tt_console_input_mode_t imode,
                               IN tt_console_output_mode_t omode);

static tt_result_t __stdin_term_cbreak();

static tt_result_t __stdin_term_raw();

static tt_bool_t __recv_keycode(IN tt_u8_t *key,
                                IN tt_u32_t key_len,
                                IN __cons_keycode_t *kc_table,
                                OUT tt_u8_t *mapped,
                                OUT tt_u32_t *eaten);

static tt_result_t __send_keycode(IN tt_u8_t *key,
                                  IN tt_u32_t key_num,
                                  IN __cons_keycode_t *kc_table);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_console_init_ntv()
{
    // default send/recv
    __cons_imode = TT_CONSOLE_IMODE_DEFAULT;
    __cons_omode = TT_CONSOLE_OMODE_DEFAULT;
    __install_cons_itf(__cons_imode, __cons_omode);

    // default substitution of unprintable char
    __cons_unprintable_subst = '.';

    // save current stdin termios
    if (tcgetattr(STDIN_FILENO, &__stdin_termios) != 0) {
        TT_ERROR_NTV("fail to get current stdin termios");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_console_enter_ntv()
{
    // todo:
    // set term according to params configured by tt_console_config_ntv()

    if (!TT_OK(__stdin_term_cbreak())) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_console_exit_ntv()
{
    // restore stdin term
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &__stdin_termios) != 0) {
        TT_ERROR_NTV("fail to restore stdin termios");
    }
}

tt_result_t tt_console_config_ntv(IN struct tt_console_attr_s *attr)
{
    // mode
    __cons_imode = attr->input_mode;
    __cons_omode = attr->output_mode;
    __install_cons_itf(__cons_imode, __cons_omode);

    // unprintable substitutor
    __cons_unprintable_subst = attr->unprintable_substitutor;
    TT_ASSERT(TT_CONS_KEY_IS_BASIC(__cons_unprintable_subst));

    return TT_SUCCESS;
}

tt_result_t tt_console_recv_ntv(OUT tt_cons_ev_t *ev,
                                OUT tt_cons_ev_data_t *ev_data)
{
    return __cons_itf.recv(ev, ev_data);
}

tt_result_t tt_console_send_ntv(IN tt_cons_ev_t ev,
                                IN tt_cons_ev_data_t *ev_data)
{
    return __cons_itf.send(ev, ev_data);
}

void __install_cons_itf(IN tt_console_input_mode_t imode,
                        IN tt_console_output_mode_t omode)
{
    // input mode
    switch (imode) {
        case TT_CONSOLE_IMODE_DEFAULT:
        default:
            __cons_itf.recv = __def_recv;
            break;
    }

    // output mode
    switch (omode) {
        case TT_CONSOLE_OMODE_DEFAULT:
        default:
            __cons_itf.send = __def_send;
            break;
    }
}

tt_result_t __stdin_term_cbreak()
{
    long vdisable;
    struct termios termios;

    if ((vdisable = fpathconf(STDIN_FILENO, _PC_VDISABLE)) < 0) {
        TT_ERROR_NTV("fail to get vdisable");
        return TT_FAIL;
    }

    if (tcgetattr(STDIN_FILENO, &termios) != 0) {
        TT_ERROR_NTV("fail to get stdin attr");
        return TT_FAIL;
    }

    // local flags
    termios.c_lflag &= ~(ECHO | ICANON | ISIG);

    // control chars
    termios.c_cc[VMIN] = 1;
    termios.c_cc[VTIME] = 0;

    termios.c_cc[VINTR] = __STDIN_CTRL_C;
    if (termios.c_cc[VINTR] == vdisable) {
        TT_ERROR("can not enable console Ctrl+C");
        return TT_FAIL;
    }

    termios.c_cc[VEOF] = __STDIN_CTRL_D;
    if (termios.c_cc[VINTR] == vdisable) {
        TT_ERROR("can not enable console Ctrl+D");
        return TT_FAIL;
    }

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &termios) != 0) {
        TT_ERROR_NTV("fail to set stdin attr");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t __stdin_term_raw()
{
    long vdisable;
    struct termios termios;

    if ((vdisable = fpathconf(STDIN_FILENO, _PC_VDISABLE)) < 0) {
        TT_ERROR_NTV("fail to get vdisable");
        return TT_FAIL;
    }

    if (tcgetattr(STDIN_FILENO, &termios) != 0) {
        TT_ERROR_NTV("fail to get stdin attr");
        return TT_FAIL;
    }

    // input flags
    termios.c_iflag &=
        ~(BRKINT | INLCR | ICRNL | IXON | IXOFF | ISTRIP | INPCK);
    termios.c_iflag |= (IGNBRK);

    // local flags
    termios.c_lflag &=
        ~(ECHO | ECHOK | ECHOE | ECHONL | ICANON | IEXTEN | ISIG);

    // control flags
    termios.c_cflag &= ~(CSIZE | PARENB);
    termios.c_iflag |= (CS8);

    // output flags
    termios.c_oflag &= ~(OPOST);
    // termios.c_oflag |= (OPOST | ONLCR);

    // control chars
    termios.c_cc[VMIN] = 1;
    termios.c_cc[VTIME] = 0;

    termios.c_cc[VINTR] = __STDIN_CTRL_C;
    if (termios.c_cc[VINTR] == vdisable) {
        TT_ERROR("can not enable console Ctrl+C");
        return TT_FAIL;
    }

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &termios) != 0) {
        TT_ERROR_NTV("fail to get stdin attr");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_bool_t __recv_keycode(IN tt_u8_t *key,
                         IN tt_u32_t key_len,
                         IN __cons_keycode_t *kc_table,
                         OUT tt_u8_t *mapped,
                         OUT tt_u32_t *eaten)
{
    tt_u32_t i;

    if (key_len == 0) {
        return TT_FALSE;
    }

    for (i = 0; i < TT_CONS_EXTKEY_NUM; ++i) {
        __cons_keycode_t *kc = &kc_table[i];
        if (kc->oneof) {
            tt_u32_t j;
            for (j = 0; j < kc->len; ++j) {
                if (*key == kc->code[j]) {
                    *mapped = TT_CONS_EXTKEY_START + i;
                    *eaten = 1;
                    // printf("mapped to extkey: 0x%x\n", *mapped);
                    return TT_TRUE;
                }
            }
        } else if ((kc->len != 0) && (kc->len <= key_len) &&
                   (tt_memcmp(kc->code, key, kc->len) == 0)) {
            *mapped = TT_CONS_EXTKEY_START + i;
            *eaten = kc->len;
            // printf("mapped to extkey: 0x%x\n", *mapped);
            return TT_TRUE;
        }
    }

    return TT_FALSE;
}

tt_result_t __send_keycode(IN tt_u8_t *key,
                           IN tt_u32_t key_num,
                           IN __cons_keycode_t *kc_table)
{
    tt_u32_t head, pos;

#define __output(p, len)                                                       \
    do {                                                                       \
        if (pos > head) {                                                      \
            /*printf(">> %d\n", pos - head);*/                                 \
            write(STDOUT_FILENO, &key[head], pos - head);                      \
        }                                                                      \
                                                                               \
        /*printf(">> %d\n", len);*/                                            \
        write(STDOUT_FILENO, (p), (len));                                      \
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
            __cons_keycode_t *kc = &kc_table[TT_CONS_EXTKEY_IDX(k)];
            __output(kc->code, kc->len);
        } else {
            __output(&__cons_unprintable_subst, 1);
        }
    }
    if (pos > head) {
        /*printf(">> %d\n", pos - head);*/
        write(STDOUT_FILENO, &key[head], pos - head);
    }
#undef __output

    return TT_SUCCESS;
}

// ========================================
// default mode
// ========================================

tt_result_t __def_recv(OUT tt_cons_ev_t *ev, OUT tt_cons_ev_data_t *ev_data)
{
    ssize_t len;
    ssize_t i;
    tt_u32_t key_num;

    len = read(STDIN_FILENO, __stdin_rbuf, sizeof(__stdin_rbuf));
    if (len < 0) {
        if (len == 0) {
            return TT_E_END;
        } else {
            TT_ERROR_NTV("read fail");
            return TT_FAIL;
        }
    }
#if 0
    for (i = 0; i < len; ++i) {
        printf("0x%x ", __stdin_rbuf[i]);
    }
    printf("\n");
#endif

    i = 0;
    key_num = 0;
    while (i < len) {
        tt_u8_t k;
        tt_u32_t eaten;

        k = __stdin_rbuf[i];
        if ((k >= 0x20) && (k <= 0x7e)) {
            __stdin_mbuf[key_num++] = k;
            ++i;
            continue;
        }

        if (__recv_keycode(&__stdin_rbuf[i],
                           (tt_u32_t)(len - i),
                           __def_ikeycode,
                           &k,
                           &eaten)) {
            __stdin_mbuf[key_num++] = k;
            i += eaten;
            continue;
        }

        // not printable, not keycode
        __stdin_mbuf[key_num++] = __cons_unprintable_subst;
        ++i;
    }

    *ev = TT_CONS_EV_KEY;
    ev_data->key.key = __stdin_mbuf;
    ev_data->key.key_num = key_num;
    return TT_SUCCESS;
}

tt_result_t __def_send(IN tt_cons_ev_t ev, IN tt_cons_ev_data_t *ev_data)
{
    switch (ev) {
        case TT_CONS_EV_KEY:
            return __send_keycode(ev_data->key.key,
                                  ev_data->key.key_num,
                                  __def_okeycode);

        default:
            return TT_FAIL;
    }
}
