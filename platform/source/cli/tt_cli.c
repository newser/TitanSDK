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

#include <cli/tt_cli.h>

#include <algorithm/tt_buffer_format.h>
#include <algorithm/tt_string_common.h>

#include <tt_cstd_api.h>

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

static void __load_internal_cb(IN tt_cli_t *cli, IN tt_cli_mode_t mode);

static tt_result_t __cli_flush(IN tt_cli_t *cli);

static tt_result_t __cli_read_line(IN tt_cli_t *cli,
                                   IN tt_u8_t *ev,
                                   IN tt_u32_t ev_num);

static tt_u32_t __cli_on_cmd(IN tt_cli_t *cli,
                             IN const tt_char_t *cmd,
                             IN tt_buf_t *output);

static tt_bool_t __cli_on_quit(IN tt_cli_t *cli, IN tt_buf_t *output);

// ========================================
// default mode
// ========================================

static tt_result_t __def_on_up(IN tt_cli_t *cli);

static tt_result_t __def_on_down(IN tt_cli_t *cli);

static tt_result_t __def_on_right(IN tt_cli_t *cli);

static tt_result_t __def_on_left(IN tt_cli_t *cli);

static tt_result_t __def_on_intr(IN tt_cli_t *cli);

static tt_result_t __def_on_quit(IN tt_cli_t *cli);

static tt_result_t __def_on_delete(IN tt_cli_t *cli);

static tt_result_t __def_on_enter(IN tt_cli_t *cli);

static tt_result_t __def_on_tab(IN tt_cli_t *cli);

static tt_result_t __def_newline(IN tt_cli_t *cli,
                                 IN tt_bool_t crlf,
                                 IN tt_bool_t prefix,
                                 IN tt_bool_t line);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_cli_create(IN tt_cli_t *cli,
                          IN tt_cli_mode_t mode,
                          IN OPT tt_cli_cb_t *cb,
                          IN OPT tt_cli_itf_t *itf,
                          IN OPT tt_cli_attr_t *attr)
{
    tt_cli_attr_t __attr;

    tt_u32_t __done = 0;
#define __CC_PREFIX (1 << 0)
#define __CC_LINE (1 << 1)
#define __CC_OUTBUF (1 << 2)
#define __CC_ACBUF (1 << 3)

    TT_ASSERT(cli != NULL);
    TT_ASSERT(TT_CLI_MODE_VALID(mode));

    if (attr == NULL) {
        tt_cli_attr_default(&__attr);
        attr = &__attr;
    }

    tt_cli_set_cb(cli, cb);
    tt_cli_set_itf(cli, itf);
    __load_internal_cb(cli, mode);
    cli->on_readline = NULL;

    cli->title = attr->title;
    cli->sub_title = attr->sub_title;
    cli->seperator = attr->seperator;
    if (cli->seperator == 0) {
        cli->seperator = '#';
    }

    tt_string_init(&cli->prefix, NULL);
    __done |= __CC_PREFIX;
    if (attr->title != NULL) {
        TT_DO_G(fail, tt_string_append(&cli->prefix, attr->title));
    }
    if ((attr->sub_title != NULL) && (attr->sub_title[0] != 0)) {
        TT_DO_G(fail, tt_string_append_c(&cli->prefix, ':'));
        TT_DO_G(fail, tt_string_append(&cli->prefix, attr->sub_title));
    }
    TT_DO_G(fail, tt_string_append_c(&cli->prefix, attr->seperator));
    TT_DO_G(fail, tt_string_append_c(&cli->prefix, ' '));

    // must creat outbuf before cline
    tt_buf_init(&cli->outbuf, &attr->outbuf_attr);
    __done |= __CC_OUTBUF;

    tt_buf_init(&cli->acbuf, NULL);
    __done |= __CC_ACBUF;

    if (!TT_OK(tt_cline_create(&cli->cline,
                               mode,
                               &cli->outbuf,
                               &attr->cline_attr))) {
        TT_ERROR("fail to create cli line");
        goto fail;
    }
    __done |= __CC_LINE;

    cli->read_mode = __CLI_NOT_READ;

    return TT_SUCCESS;

fail:

    if (__done & __CC_LINE) {
        tt_cline_destroy(&cli->cline);
    }

    if (__done & __CC_ACBUF) {
        tt_buf_destroy(&cli->acbuf);
    }

    if (__done & __CC_OUTBUF) {
        tt_buf_destroy(&cli->outbuf);
    }

    if (__done & __CC_PREFIX) {
        tt_string_destroy(&cli->prefix);
    }

    return TT_FAIL;
}

void tt_cli_destroy(IN tt_cli_t *cli)
{
    TT_ASSERT(cli != NULL);

    tt_string_destroy(&cli->prefix);

    // outbuf must be destroy after cline
    tt_cline_destroy(&cli->cline);
    tt_buf_destroy(&cli->outbuf);
    tt_buf_destroy(&cli->acbuf);
}

void tt_cli_attr_default(IN tt_cli_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->title = "titansdk";
    attr->sub_title = NULL;
    attr->seperator = '$';

    tt_buf_attr_default(&attr->outbuf_attr);
    tt_cline_attr_default(&attr->cline_attr);
}

tt_result_t tt_cli_start(IN tt_cli_t *cli)
{
    TT_ASSERT(cli != NULL);

    if (!TT_OK(cli->newline(cli, TT_TRUE, TT_TRUE, TT_TRUE))) {
        return TT_FAIL;
    }

    if (!TT_OK(__cli_flush(cli))) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_cli_update_prefix(IN tt_cli_t *cli,
                                 IN OPT const tt_char_t *title,
                                 IN OPT const tt_char_t *sub_title,
                                 IN OPT tt_char_t seperator)
{
    tt_string_t *prefix;

    TT_ASSERT(cli != NULL);
    prefix = &cli->prefix;

    tt_string_clear(prefix);

    if (title != NULL) {
        TT_DO(tt_string_append(prefix, title));
        cli->title = title;
    } else if (cli->title != NULL) {
        TT_DO(tt_string_append(prefix, cli->title));
    }

    if ((sub_title != NULL) && (sub_title[0] != 0)) {
        TT_DO(tt_string_append_c(prefix, ':'));
        TT_DO(tt_string_append(prefix, sub_title));
        cli->sub_title = sub_title;
    } else if ((cli->sub_title != NULL) && (cli->sub_title[0] != 0)) {
        TT_DO(tt_string_append_c(prefix, ':'));
        TT_DO(tt_string_append(prefix, cli->sub_title));
    }

    if (seperator != 0) {
        TT_DO(tt_string_append_c(prefix, seperator));
        cli->seperator = seperator;
    } else if (cli->seperator != 0) {
        TT_DO(tt_string_append_c(prefix, cli->seperator));
    } else {
        TT_DO(tt_string_append_c(prefix, '#'));
    }
    TT_DO(tt_string_append_c(prefix, ' '));

    return TT_SUCCESS;
}

tt_result_t tt_cli_input(IN tt_cli_t *cli, IN tt_u8_t *ev, IN tt_u32_t ev_num)
{
    tt_u32_t i;

    TT_ASSERT(cli != NULL);
    TT_ASSERT(ev != NULL);

    if (ev_num == 0) {
        return TT_SUCCESS;
    }

    if (cli->read_mode == __CLI_READ_LINE) {
        return __cli_read_line(cli, ev, ev_num);
    }

    for (i = 0; i < ev_num; ++i) {
        tt_u8_t e = ev[i];
        if ((e >= 0x20) && (e <= 0x7e)) {
            TT_DO(tt_cline_input(&cli->cline, &e, 1));
        } else if (TT_CLI_EV_VALID(e)) {
            tt_cli_on_ev_t on_ev = cli->on_ev[TT_CLI_EV_IDX(e)];
            if (on_ev != NULL) {
                tt_result_t result = on_ev(cli);
                if (TT_OK(result)) {
                } else if (result == TT_E_END) {
                    __cli_flush(cli);
                    return TT_E_END;
                } else {
                    TT_ERROR("fail to handle cli event[0x%x]", e);
                }
            }
        } else {
            TT_WARN("discarded cli ev[0x%x]", e);
        }
    }
    __cli_flush(cli);

    return TT_SUCCESS;
}

tt_result_t tt_cli_output(IN tt_cli_t *cli, IN tt_u8_t *ev, IN tt_u32_t ev_num)
{
    TT_ASSERT(cli != NULL);
    TT_ASSERT(ev != NULL);

    if (ev_num == 0) {
        return TT_SUCCESS;
    }

    if (cli->itf.send == NULL) {
        return TT_SUCCESS;
    }
    return cli->itf.send(cli, cli->itf.param, ev, ev_num);
}

void tt_cli_set_cb(IN tt_cli_t *cli, IN OPT tt_cli_cb_t *cb)
{
    TT_ASSERT(cli != NULL);

    if (cb != NULL) {
        tt_memcpy(&cli->cb, cb, sizeof(tt_cli_cb_t));
    } else {
        tt_memset(&cli->cb, 0, sizeof(tt_cli_cb_t));
    }
}

void tt_cli_set_itf(IN tt_cli_t *cli, IN OPT tt_cli_itf_t *itf)
{
    TT_ASSERT(cli != NULL);

    if (itf != NULL) {
        tt_memcpy(&cli->itf, itf, sizeof(tt_cli_itf_t));
    } else {
        tt_memset(&cli->itf, 0, sizeof(tt_cli_itf_t));
    }
}

tt_result_t tt_cli_complete(IN tt_u8_t *cur,
                            IN tt_u32_t cur_len,
                            IN const tt_char_t **option,
                            IN tt_u32_t option_num,
                            OUT tt_u32_t *status,
                            OUT tt_buf_t *outbuf)
{
    tt_u32_t i;
    tt_bool_t head = TT_TRUE;

    if (cur == NULL) {
        // put all options
        for (i = 0; i < option_num; ++i) {
            if (head) {
                head = TT_FALSE;
            } else {
                TT_DO(tt_buf_put_u8(outbuf, ' '));
            }
            TT_DO(tt_buf_put(outbuf,
                             (tt_u8_t *)option[i],
                             (tt_u32_t)tt_strlen(option[i])));
        }
        *status = TT_CLICP_NONE;
    } else {
        tt_u32_t match_num = 0, common_len = 0;
        const tt_char_t *common = NULL;

        // only put matching options
        for (i = 0; i < option_num; ++i) {
            const tt_char_t *opt = option[i];

            if (tt_strncmp(opt, (tt_char_t *)cur, cur_len) == 0) {
                if (match_num == 0) {
                    // init the common part
                    common = opt + cur_len;
                    common_len = (tt_u32_t)tt_strlen(opt) - cur_len;
                } else {
                    // reduce common part
                    tt_u32_t k = 0;
                    tt_u32_t n =
                        TT_MIN(common_len, (tt_u32_t)tt_strlen(opt) - cur_len);
                    while ((k < n) && (common[k] == opt[cur_len + k])) {
                        ++k;
                    }
                    common_len -= (n - k);
                }

                ++match_num;
            }
        }

        if (match_num == 0) {
            *status = TT_CLICP_NONE;
        } else if (match_num == 1) {
            for (i = 0; i < option_num; ++i) {
                if (tt_strncmp(option[i], (tt_char_t *)cur, cur_len) == 0) {
                    TT_DO(tt_buf_put(outbuf,
                                     TT_PTR_INC(tt_u8_t, option[i], cur_len),
                                     (tt_u32_t)tt_strlen(option[i]) - cur_len));
                    break;
                }
            }
            *status = TT_CLICP_FULL;
        } else if (common_len > 0) {
            TT_DO(tt_buf_put(outbuf, (tt_u8_t *)common, common_len));
            *status = TT_CLICP_PARTIAL;
        } else {
            for (i = 0; i < option_num; ++i) {
                if (tt_strncmp(option[i], (tt_char_t *)cur, cur_len) == 0) {
                    if (head) {
                        head = TT_FALSE;
                    } else {
                        TT_DO(tt_buf_put_u8(outbuf, ' '));
                    }
                    TT_DO(tt_buf_put(outbuf,
                                     (tt_u8_t *)option[i],
                                     (tt_u32_t)tt_strlen(option[i])));
                }
            }
            *status = TT_CLICP_NONE;
        }
    }

    return TT_SUCCESS;
}

void __load_internal_cb(IN tt_cli_t *cli, IN tt_cli_mode_t mode)
{
    TT_ASSERT(TT_CLI_MODE_VALID(mode));

    switch (mode) {
        case TT_CLI_MODE_DEFAUTL:
        default: {
            cli->on_ev[TT_CLI_EV_IDX(TT_CLI_EV_UP)] = __def_on_up;
            cli->on_ev[TT_CLI_EV_IDX(TT_CLI_EV_DOWN)] = __def_on_down;
            cli->on_ev[TT_CLI_EV_IDX(TT_CLI_EV_RIGHT)] = __def_on_right;
            cli->on_ev[TT_CLI_EV_IDX(TT_CLI_EV_LEFT)] = __def_on_left;
            cli->on_ev[TT_CLI_EV_IDX(TT_CLI_EV_INTR)] = __def_on_intr;
            cli->on_ev[TT_CLI_EV_IDX(TT_CLI_EV_QUIT)] = __def_on_quit;
            cli->on_ev[TT_CLI_EV_IDX(TT_CLI_EV_DELETE)] = __def_on_delete;
            cli->on_ev[TT_CLI_EV_IDX(TT_CLI_EV_ENTER)] = __def_on_enter;
            cli->on_ev[TT_CLI_EV_IDX(TT_CLI_EV_TAB)] = __def_on_tab;

            cli->newline = __def_newline;
        } break;
    }
}

tt_result_t __cli_flush(IN tt_cli_t *cli)
{
    tt_buf_t *outbuf = &cli->outbuf;

    if (cli->itf.send == NULL) {
        return TT_SUCCESS;
    }

    if (TT_BUF_RLEN(outbuf) > 0) {
        TT_DO(cli->itf.send(cli,
                            cli->itf.param,
                            TT_BUF_RPOS(outbuf),
                            TT_BUF_RLEN(outbuf)));
        tt_buf_clear(outbuf);
    }
    TT_ASSERT(outbuf->rpos == 0);

    return TT_SUCCESS;
}

tt_result_t __cli_read_line(IN tt_cli_t *cli,
                            IN tt_u8_t *ev,
                            IN tt_u32_t ev_num)
{
    tt_u32_t i;

    for (i = 0; i < ev_num; ++i) {
        tt_u8_t e = ev[i];
        if ((e >= 0x20) && (e <= 0x7e)) {
            TT_DO(tt_cline_input(&cli->cline, &e, 1));
        } else if ((e == TT_CLI_EV_LEFT) || (e == TT_CLI_EV_RIGHT) ||
                   (e == TT_CLI_EV_DELETE)) {
            TT_DO(tt_cline_input(&cli->cline, &e, 1));
        } else if (e == TT_CLI_EV_ENTER) {
            tt_buf_t *outbuf = &cli->outbuf;
            tt_cline_t *cline = &cli->cline;
            tt_u32_t status = TT_CLIOR_DONE;

            TT_DO(tt_buf_put_u8(outbuf, TT_CLI_EV_ENTER));

            if (cli->on_readline != NULL) {
                status = cli->on_readline(cli, tt_cline_cstr(cline), outbuf);
            }
            tt_cline_reset(&cli->cline);

            if (status == TT_CLIOR_DONE) {
                // all left data are discarded
                TT_DO(cli->newline(cli, TT_FALSE, TT_TRUE, TT_FALSE));

                cli->read_mode = __CLI_NOT_READ;
                break;
            } else if (status == TT_CLIOR_END) {
                if (__cli_on_quit(cli, outbuf)) {
                    TT_DO(tt_buf_put_u8(outbuf, TT_CLI_EV_ENTER));
                }
                __cli_flush(cli);
                return TT_E_END;
            } else {
                TT_ASSERT(status == TT_CLIOR_MORE);
                // continue reading
            }
        } else {
            TT_WARN("discarded cli ev[0x%x]", e);
        }
    }
    __cli_flush(cli);

    return TT_SUCCESS;
}

tt_u32_t __cli_on_cmd(IN tt_cli_t *cli,
                      IN const tt_char_t *cmd,
                      IN tt_buf_t *output)
{
    if (cli->cb.on_cmd != NULL) {
        return cli->cb.on_cmd(cli, cli->cb.param, cmd, output);
    } else {
        return 0;
    }
}

tt_bool_t __cli_on_quit(IN tt_cli_t *cli, IN tt_buf_t *output)
{
    if (cli->cb.on_quit != NULL) {
        return cli->cb.on_quit(cli, output);
    } else {
        return TT_FALSE;
    }
}

// ========================================
// default mode
// ========================================

tt_result_t __def_on_up(IN tt_cli_t *cli)
{
    // todo
    return TT_SUCCESS;
}

tt_result_t __def_on_down(IN tt_cli_t *cli)
{
    // todo
    return TT_SUCCESS;
}

tt_result_t __def_on_right(IN tt_cli_t *cli)
{
    TT_DO(tt_cline_input_ev(&cli->cline, TT_CLI_EV_RIGHT));

    return TT_SUCCESS;
}

tt_result_t __def_on_left(IN tt_cli_t *cli)
{
    TT_DO(tt_cline_input_ev(&cli->cline, TT_CLI_EV_LEFT));

    return TT_SUCCESS;
}

tt_result_t __def_on_intr(IN tt_cli_t *cli)
{
    tt_cline_t *cline = &cli->cline;

    TT_DO(tt_cline_input_ev(cline, TT_CLI_EV_INTR));
    tt_cline_reset(cline);

    TT_DO(cli->newline(cli, TT_TRUE, TT_TRUE, TT_TRUE));

    return TT_SUCCESS;
}

tt_result_t __def_on_quit(IN tt_cli_t *cli)
{
    tt_cline_t *cline = &cli->cline;
    tt_buf_t *outbuf = &cli->outbuf;

    TT_DO(tt_cline_input_ev(cline, TT_CLI_EV_QUIT));

    TT_DO(tt_buf_put_u8(outbuf, TT_CLI_EV_ENTER));
    if (__cli_on_quit(cli, outbuf)) {
        TT_DO(tt_buf_put_u8(outbuf, TT_CLI_EV_ENTER));
    }

    return TT_E_END;
}

tt_result_t __def_on_delete(IN tt_cli_t *cli)
{
    TT_DO(tt_cline_input_ev(&cli->cline, TT_CLI_EV_DELETE));

    return TT_SUCCESS;
}

tt_result_t __def_on_enter(IN tt_cli_t *cli)
{
    tt_buf_t *outbuf = &cli->outbuf;
    const tt_char_t *cmd = tt_cline_cstr(&cli->cline);
    tt_u32_t status;

    // echo the enter
    TT_DO(tt_buf_put_u8(outbuf, TT_CLI_EV_ENTER));

    if (cmd[0] != 0) {
        status = __cli_on_cmd(cli, cmd, outbuf);
    } else {
        status = TT_CLIOC_NOOUT;
    }
    tt_cline_reset(&cli->cline);

    if (status == TT_CLIOC_NOOUT) {
        if (cli->read_mode == __CLI_NOT_READ) {
            // no output, so no need print a crlf
            TT_DO(cli->newline(cli, TT_FALSE, TT_TRUE, TT_TRUE));
        }
        return TT_SUCCESS;
    } else if (status == TT_CLIOC_OUT) {
        if (cli->read_mode == __CLI_NOT_READ) {
            TT_DO(cli->newline(cli, TT_TRUE, TT_TRUE, TT_TRUE));
        }
        return TT_SUCCESS;
    } else {
        TT_ASSERT(status == TT_CLIOC_END);
        if (__cli_on_quit(cli, outbuf)) {
            TT_DO(tt_buf_put_u8(outbuf, TT_CLI_EV_ENTER));
        }
        return TT_E_END;
    }
}

tt_result_t __def_on_tab(IN tt_cli_t *cli)
{
    tt_blob_t cursor_data;
    tt_bool_t wait4cmd;
    tt_cline_t *cline = &cli->cline;
    tt_buf_t *acbuf = &cli->acbuf;
    tt_u32_t status;

    if (cli->cb.on_complete == NULL) {
        // can not complete
        return TT_SUCCESS;
    }

    wait4cmd = tt_cline_cursor_data(cline, &cursor_data);

    tt_buf_clear(&cli->acbuf);
    status = cli->cb.on_complete(cli,
                                 cli->cb.param,
                                 cursor_data.addr,
                                 cursor_data.len,
                                 wait4cmd,
                                 acbuf);
    if (status == TT_CLICP_FULL) {
        TT_DO(tt_cline_input(cline, TT_BUF_RPOS(acbuf), TT_BUF_RLEN(acbuf)));
        TT_DO(tt_cline_input_ev(cline, ' '));
    } else if (status == TT_CLICP_FULL_MORE) {
        TT_DO(tt_cline_input(cline, TT_BUF_RPOS(acbuf), TT_BUF_RLEN(acbuf)));
        // TT_CLICP_FULL_MORE indicates a following sp is not needed
    } else if (status == TT_CLICP_PARTIAL) {
        TT_DO(tt_cline_input(cline, TT_BUF_RPOS(acbuf), TT_BUF_RLEN(acbuf)));
    } else {
        TT_ASSERT(status == TT_CLICP_NONE);
        if (TT_BUF_RLEN(acbuf) > 0) {
            TT_DO(cli->newline(cli, TT_TRUE, TT_FALSE, TT_FALSE));
            TT_DO(tt_buf_put(&cli->outbuf,
                             TT_BUF_RPOS(acbuf),
                             TT_BUF_RLEN(acbuf)));
            TT_DO(cli->newline(cli, TT_TRUE, TT_TRUE, TT_TRUE));
        }
        // else do nothing as none is matching
    }
    return TT_SUCCESS;
}

tt_result_t __def_newline(IN tt_cli_t *cli,
                          IN tt_bool_t crlf,
                          IN tt_bool_t prefix,
                          IN tt_bool_t line)
{
    tt_buf_t *outbuf = &cli->outbuf;

    if (crlf) {
        TT_DO(tt_buf_put_u8(outbuf, TT_CLI_EV_ENTER));
    }

    if (prefix) {
        TT_DO(tt_buf_put(outbuf,
                         (tt_u8_t *)tt_string_cstr(&cli->prefix),
                         tt_string_len(&cli->prefix)));
    }

    if (line) {
        tt_string_t *s = &cli->cline.line;
        tt_u32_t s_len = tt_string_len(s);

        TT_DO(tt_buf_put(outbuf, (tt_u8_t *)tt_string_cstr(s), s_len));

        TT_ASSERT(cli->cline.cursor <= s_len);
        TT_DO(
            tt_buf_put_rep(outbuf, TT_CLI_EV_LEFT, s_len - cli->cline.cursor));
    }

    return TT_SUCCESS;
}
