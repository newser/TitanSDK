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

#include <network/http/tt_http_server_connection.h>

#include <io/tt_socket.h>
#include <memory/tt_slab.h>
#include <network/http/tt_http_raw_header.h>
#include <network/ssl/tt_ssl.h>
#include <os/tt_fiber_event.h>
#include <os/tt_thread.h>
#include <time/tt_time_reference.h>
#include <time/tt_timer.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef enum {
    TT_HTTP_SERVER_NO_SPACE,
    TT_HTTP_SERVER_PARSE_FAIL,

    TT_HTTP_SERVER_ERROR_NUM
} tt_http_server_error_t;
#define TT_HTTP_SERVER_ERROR_VALID(e) ((e) < TT_HTTP_SERVER_ERROR_NUM)

typedef tt_result_t (*__sconn_send_t)(IN tt_http_sconn_t *c,
                                      IN tt_u8_t *buf,
                                      IN tt_u32_t len,
                                      OUT tt_u32_t *sent);

typedef tt_result_t (*__sconn_recv_t)(IN tt_http_sconn_t *c,
                                      OUT tt_u8_t *buf,
                                      IN tt_u32_t len,
                                      OUT tt_u32_t *recvd,
                                      OUT tt_fiber_ev_t **p_fev,
                                      OUT tt_tmr_t **p_tmr);

typedef tt_result_t (*__sconn_shut_t)(IN tt_http_sconn_t *c,
                                      IN tt_http_shut_t shut);

typedef void (*__sconn_destroy_t)(IN tt_http_sconn_t *c);

typedef struct
{
    __sconn_send_t send;
    __sconn_recv_t recv;
    __sconn_shut_t shut;
    __sconn_destroy_t destroy;
} __sconn_itf_t;

// ========================================
// socket
// ========================================

static tt_result_t __skt_send(IN tt_http_sconn_t *c,
                              IN tt_u8_t *buf,
                              IN tt_u32_t len,
                              OUT tt_u32_t *sent);

static tt_result_t __skt_recv(IN tt_http_sconn_t *c,
                              OUT tt_u8_t *buf,
                              IN tt_u32_t len,
                              OUT tt_u32_t *recvd,
                              OUT tt_fiber_ev_t **p_fev,
                              OUT tt_tmr_t **p_tmr);

static tt_result_t __skt_shut(IN tt_http_sconn_t *c, IN tt_http_shut_t shut);

static void __skt_destroy(IN tt_http_sconn_t *c);

static __sconn_itf_t __sconn_skt_itf = {
    __skt_send, __skt_recv, __skt_shut, __skt_destroy,
};

// ========================================
// ssl
// ========================================

static tt_result_t __ssl_send(IN tt_http_sconn_t *c,
                              IN tt_u8_t *buf,
                              IN tt_u32_t len,
                              OUT tt_u32_t *sent);

static tt_result_t __ssl_recv(IN tt_http_sconn_t *c,
                              OUT tt_u8_t *buf,
                              IN tt_u32_t len,
                              OUT tt_u32_t *recvd,
                              OUT tt_fiber_ev_t **p_fev,
                              OUT tt_tmr_t **p_tmr);

static tt_result_t __ssl_shut(IN tt_http_sconn_t *c, IN tt_http_shut_t shut);

static void __ssl_destroy(IN tt_http_sconn_t *c);

static __sconn_itf_t __sconn_ssl_itf = {
    __ssl_send, __ssl_recv, __ssl_shut, __ssl_destroy,
};

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static void __sconn_set_itf(IN tt_http_sconn_t *c,
                            IN __sconn_itf_t *itf,
                            IN void *itf_opaque);

static tt_result_t __sconn_create(IN tt_http_sconn_t *c,
                                  IN tt_http_sconn_attr_t *attr);

static void __sconn_destroy(IN tt_http_sconn_t *c);

static tt_slab_t *__local_rawhdr_slab();

static tt_slab_t *__local_rawval_slab();

static void __sconn_on_error(IN tt_http_sconn_t *c,
                             IN tt_http_server_error_t e);

static tt_result_t __sconn_send_resp(IN tt_http_sconn_t *c);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_http_sconn_create_skt(IN tt_http_sconn_t *c,
                                     IN tt_skt_t *s,
                                     IN OPT tt_http_sconn_attr_t *attr)
{
    tt_http_sconn_attr_t __attr;

    if (attr == NULL) {
        tt_http_sconn_attr_default(&__attr);
        attr = &__attr;
    }

    __sconn_set_itf(c, &__sconn_skt_itf, s);

    if (TT_OK(__sconn_create(c, attr))) {
        return TT_SUCCESS;
    } else {
        tt_skt_destroy(s);
        return TT_FAIL;
    }
}

tt_result_t tt_http_sconn_create_ssl(IN tt_http_sconn_t *c,
                                     IN tt_ssl_t *s,
                                     IN OPT tt_http_sconn_attr_t *attr)
{
    tt_http_sconn_attr_t __attr;

    if (attr == NULL) {
        tt_http_sconn_attr_default(&__attr);
        attr = &__attr;
    }

    __sconn_set_itf(c, &__sconn_ssl_itf, s);

    if (TT_OK(__sconn_create(c, attr))) {
        return TT_SUCCESS;
    } else {
        tt_ssl_destroy(s);
        return TT_FAIL;
    }
}

void tt_http_sconn_destroy(IN tt_http_sconn_t *c)
{
    __sconn_itf_t *itf = (__sconn_itf_t *)c->itf;

    itf->destroy(c);

    __sconn_destroy(c);
}

void tt_http_sconn_attr_default(IN tt_http_sconn_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    tt_http_parser_attr_default(&attr->parser_attr);

    tt_http_resp_render_attr_default(&attr->render_attr);
}

tt_bool_t tt_http_sconn_run(IN tt_http_sconn_t *c)
{
    tt_http_parser_t *hp = &c->parser;
    __sconn_itf_t *itf = (__sconn_itf_t *)c->itf;

    while (1) {
        tt_u8_t *p;
        tt_u32_t len, recvd;
        tt_fiber_ev_t *ev;
        tt_tmr_t *tmr;
        tt_result_t result;
        tt_http_inserv_action_t action;

        tt_http_parser_wpos(hp, &p, &len);
        if (len == 0) {
            __sconn_on_error(c, TT_HTTP_SERVER_NO_SPACE);
            return TT_FALSE;
        }

        TT_TIMEOUT_BEGIN(c->tmr, 5000);
        result = itf->recv(c, p, len, &recvd, &ev, &tmr);
        TT_TIMEOUT_IF_ERROR(result = TT_FAIL);
        if (result == TT_E_END) {
            // remote shutdown write, to close connection
            itf->shut(c, TT_HTTP_SHUT_WR);
            return TT_FALSE;
        } else if (!TT_OK(result)) {
            // connection is broken, won't send response
            return TT_FALSE;
        }

        if (recvd > 0) {
            tt_http_parser_inc_wp(hp, recvd);

            while (tt_http_parser_rlen(hp) > 0) {
                tt_bool_t prev_uri = hp->complete_line1;
                tt_bool_t prev_header = hp->complete_header;
                tt_u32_t body_num = hp->body_counter;
                tt_bool_t prev_trailing = hp->complete_trailing_header;
                tt_bool_t prev_msg = hp->complete_message;

                if (!TT_OK(tt_http_parser_run(hp))) {
                    __sconn_on_error(c, TT_HTTP_SERVER_PARSE_FAIL);
                    return TT_FALSE;
                }

                if (!prev_uri && hp->complete_line1) {
                    action = tt_http_svcmgr_on_uri(&c->svcmgr, hp, &c->render);
                    switch (action) {
                        case TT_HTTP_INSERV_ACT_CLOSE: {
                            return TT_FALSE;
                        } break;
                        case TT_HTTP_INSERV_ACT_SHUTDOWN: {
                            __sconn_send_resp(c);
                            itf->shut(c, TT_HTTP_SHUT_WR);
                            return TT_FALSE;
                        } break;
                        case TT_HTTP_INSERV_ACT_DISCARD: {
                            c->discarding = TT_TRUE;
                        } break;

                        default:
                            break;
                    }
                }

                if (!prev_header && hp->complete_header) {
                }

                if (body_num < hp->body_counter) {
                }

                if (!prev_trailing && hp->complete_trailing_header) {
                }

                if (!prev_msg && hp->complete_message) {
                }
            }
        }

        if (ev != NULL) {
        }

        if (tmr != NULL) {
        }

        TT_TIMEOUT_END();
    }

    return TT_SUCCESS;
}

tt_result_t tt_http_sconn_send(IN tt_http_sconn_t *c,
                               IN tt_u8_t *buf,
                               IN tt_u32_t len,
                               OUT OPT tt_u32_t *sent)
{
    __sconn_itf_t *itf = (__sconn_itf_t *)c->itf;
    return itf->send(c, buf, len, sent);
}

void __sconn_set_itf(IN tt_http_sconn_t *c,
                     IN __sconn_itf_t *itf,
                     IN void *itf_opaque)
{
    c->itf = itf;
    c->itf_opaque = itf_opaque;
}

tt_result_t __sconn_create(IN tt_http_sconn_t *c, IN tt_http_sconn_attr_t *attr)
{
    tt_slab_t *rhs, *rvs;

    tt_u32_t __done = 0;
#define __SC_TMR (1 << 0)
#define __SC_SVCMGR (1 << 1)
#define __SC_PARSER (1 << 2)
#define __SC_RENDER (1 << 3)

    c->tmr = tt_tmr_create(0, 0, NULL);
    if (c->tmr == NULL) {
        TT_ERROR("fail to create http server timer");
        goto fail;
    }
    __done |= __SC_TMR;

    tt_http_svcmgr_init(&c->svcmgr);
    __done |= __SC_SVCMGR;

    rhs = __local_rawhdr_slab();
    rvs = __local_rawval_slab();
    if ((rhs == NULL) || (rvs == NULL)) {
        goto fail;
    }

    if (!TT_OK(
            tt_http_parser_create(&c->parser, rhs, rvs, &attr->parser_attr))) {
        goto fail;
    }
    __done |= __SC_PARSER;

    tt_http_resp_render_init(&c->render, &attr->render_attr);
    __done |= __SC_RENDER;

    return TT_SUCCESS;

fail:

    if (__done & __SC_TMR) {
        tt_tmr_destroy(c->tmr);
    }

    if (__done & __SC_SVCMGR) {
        tt_http_svcmgr_destroy(&c->svcmgr);
    }

    if (__done & __SC_PARSER) {
        tt_http_parser_destroy(&c->parser);
    }

    if (__done & __SC_RENDER) {
        tt_http_resp_render_destroy(&c->render);
    }

    return TT_FAIL;
}

void __sconn_destroy(IN tt_http_sconn_t *c)
{
    tt_tmr_destroy(c->tmr);

    tt_http_svcmgr_destroy(&c->svcmgr);

    tt_http_parser_destroy(&c->parser);

    tt_http_resp_render_destroy(&c->render);
}

tt_slab_t *__local_rawhdr_slab()
{
    tt_thread_t *t = tt_current_thread();
    if (t->http_rawhdr != NULL) {
        tt_slab_t *s;
        tt_slab_attr_t attr;

        s = tt_malloc(sizeof(tt_slab_t));
        if (s == NULL) {
            TT_ERROR("no mem for http_rawhdr slab");
            return NULL;
        }

        tt_slab_attr_default(&attr);
        attr.bulk_num = 16; // to be adjusted
        if (!TT_OK(tt_slab_create(s, sizeof(tt_http_rawhdr_t), &attr))) {
            tt_free(s);
            return NULL;
        }

        t->http_rawhdr = s;
    }
    return t->http_rawhdr;
}

tt_slab_t *__local_rawval_slab()
{
    tt_thread_t *t = tt_current_thread();
    if (t->http_rawval != NULL) {
        tt_slab_t *s;
        tt_slab_attr_t attr;

        s = tt_malloc(sizeof(tt_slab_t));
        if (s == NULL) {
            TT_ERROR("no mem for http_rawval slab");
            return NULL;
        }

        tt_slab_attr_default(&attr);
        attr.bulk_num = 16; // to be adjusted
        if (!TT_OK(tt_slab_create(s, sizeof(tt_http_rawval_t), &attr))) {
            tt_free(s);
            return NULL;
        }

        t->http_rawval = s;
    }
    return t->http_rawval;
}

void __sconn_on_error(IN tt_http_sconn_t *c, IN tt_http_server_error_t e)
{
    tt_http_parser_t *req = &c->parser;
    tt_http_resp_render_t *resp = &c->render;

    switch (e) {
        case TT_HTTP_SERVER_NO_SPACE: {
            if (!req->complete_line1) {
                tt_http_resp_render_set_status(resp,
                                               TT_HTTP_STATUS_URI_TOO_LONG);
            } else if (!req->complete_header) {
                tt_http_resp_render_set_status(
                    resp, TT_HTTP_STATUS_REQUEST_HEADER_FIELDS_TOO_LARGE);
            } else {
                tt_http_resp_render_set_status(
                    resp, TT_HTTP_STATUS_PAYLOAD_TOO_LARGE);
            }
        } break;

        default: {
            static tt_http_status_t status_errmap[TT_HTTP_SERVER_ERROR_NUM] = {
                TT_HTTP_STATUS_INVALID, TT_HTTP_STATUS_BAD_REQUEST,
            };

            tt_http_resp_render_set_status(resp, status_errmap[e]);
        } break;
    }

    __sconn_send_resp(c);
}

tt_result_t __sconn_send_resp(IN tt_http_sconn_t *c)
{
    tt_http_parser_t *req = &c->parser;
    tt_http_resp_render_t *resp = &c->render;
    tt_http_status_t status;
    tt_http_ver_t ver;

    status = tt_http_resp_render_get_status(resp);
    if (!TT_HTTP_STATUS_VALID(status)) {
#if 0
        TT_WARN("no status set, send 500 Internal Server Error");
        tt_http_resp_render_set_status(resp,
                                       TT_HTTP_STATUS_INTERNAL_SERVER_ERROR);
#else
        // no valid resp status means no need to send response
        return TT_SUCCESS;
#endif
    }

    ver = tt_http_parser_get_version(req);
    if (TT_HTTP_VER_VALID(ver)) {
        tt_http_resp_render_set_version(resp, ver);
    } else {
        // use default
        tt_http_resp_render_set_version(resp, TT_HTTP_V1_1);
    }

    tt_http_svcmgr_on_resp(&c->svcmgr,
                           TT_COND(req->complete_header, req, NULL),
                           resp);

    // todo: send resp

    return TT_SUCCESS;
}

// ========================================
// socket
// ========================================

tt_result_t __skt_send(IN tt_http_sconn_t *c,
                       IN tt_u8_t *buf,
                       IN tt_u32_t len,
                       OUT tt_u32_t *sent)
{
    return tt_skt_send((tt_skt_t *)c->itf_opaque, buf, len, sent);
}

tt_result_t __skt_recv(IN tt_http_sconn_t *c,
                       OUT tt_u8_t *buf,
                       IN tt_u32_t len,
                       OUT tt_u32_t *recvd,
                       OUT tt_fiber_ev_t **p_fev,
                       OUT tt_tmr_t **p_tmr)
{
    return tt_skt_recv((tt_skt_t *)c->itf_opaque,
                       buf,
                       len,
                       recvd,
                       p_fev,
                       p_tmr);
}

tt_result_t __skt_shut(IN tt_http_sconn_t *c, IN tt_http_shut_t shut)
{
    static tt_skt_shut_t shut_map[TT_HTTP_SHUT_NUM] = {
        TT_SKT_SHUT_RD, TT_SKT_SHUT_WR, TT_SKT_SHUT_RDWR,
    };
    return tt_skt_shutdown((tt_skt_t *)c->itf_opaque, shut_map[shut]);
}

void __skt_destroy(IN tt_http_sconn_t *c)
{
    tt_skt_destroy((tt_skt_t *)c->itf_opaque);
}

// ========================================
// ssl
// ========================================

tt_result_t __ssl_send(IN tt_http_sconn_t *c,
                       IN tt_u8_t *buf,
                       IN tt_u32_t len,
                       OUT tt_u32_t *sent)
{
    return tt_ssl_send((tt_ssl_t *)c->itf_opaque, buf, len, sent);
}

tt_result_t __ssl_recv(IN tt_http_sconn_t *c,
                       OUT tt_u8_t *buf,
                       IN tt_u32_t len,
                       OUT tt_u32_t *recvd,
                       OUT tt_fiber_ev_t **p_fev,
                       OUT tt_tmr_t **p_tmr)
{
    return tt_ssl_recv((tt_ssl_t *)c->itf_opaque,
                       buf,
                       len,
                       recvd,
                       p_fev,
                       p_tmr);
}

tt_result_t __ssl_shut(IN tt_http_sconn_t *c, IN tt_http_shut_t shut)
{
    static tt_ssl_shut_t shut_map[TT_HTTP_SHUT_NUM] = {
        TT_SSL_SHUT_RD, TT_SSL_SHUT_WR, TT_SSL_SHUT_RDWR,
    };
    return tt_ssl_shutdown((tt_ssl_t *)c->itf_opaque, shut_map[shut]);
}

void __ssl_destroy(IN tt_http_sconn_t *c)
{
    tt_ssl_destroy((tt_ssl_t *)c->itf_opaque);
}
