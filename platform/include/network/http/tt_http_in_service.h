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
@file tt_http_in_service.s
@brief http service

this file defines http incoming service APIs
*/

#ifndef __TT_HTTP_IN_SERVICE__
#define __TT_HTTP_IN_SERVICE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_double_linked_list.h>
#include <memory/tt_memory_alloc.h>
#include <misc/tt_reference_counter.h>
#include <network/http/def/tt_http_service_def.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_HTTP_INSERV_CAST(s, name)                                           \
    TT_PTR_INC(name, s, sizeof(tt_http_inserv_t))

#define tt_http_inserv_ref(is) TT_REF_ADD(tt_http_inserv_t, is, ref)

#define tt_http_inserv_release(is)                                             \
    TT_REF_RELEASE(tt_http_inserv_t, is, ref, __http_inserv_destroy)

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_http_inserv_s;
struct tt_http_parser_s;
struct tt_http_resp_render_s;
struct tt_buf_s;

typedef enum
{
    /*
     service should: n/a
     caller should: close connection
     */
    TT_HTTP_INSERV_ACT_CLOSE,

    /*
     service should: optionally set response status code
     caller should: send response if set, then shutdown connection
     */
    TT_HTTP_INSERV_ACT_SHUTDOWN,

    /*
     service should: optionally set response status code
     caller should: recv entire request, send response(500 if not set), then
     shutdown connection
     */
    TT_HTTP_INSERV_ACT_DISCARD,

    /*
     service should: n/a
     caller should: n/a
     */
    TT_HTTP_INSERV_ACT_PASS,

    // TT_HTTP_INSERV_ACT_INTERESTED,

    /*
     service should: set status code in on_complete()
     caller should: recv entire request, send response(500 if not set), then
     shutdown connection
     */
    TT_HTTP_INSERV_ACT_OWNER,

    /*
     service should: set status code in on_complete()
     caller should: send response(500 if not set), then send body
     */
    TT_HTTP_INSERV_ACT_BODY,

    TT_HTTP_INSERV_ACT_NUM
} tt_http_inserv_action_t;
#define TT_HTTP_INSERV_ACT_VALID(a) ((a) < TT_HTTP_INSERV_ACT_NUM)

typedef void (*tt_http_inserv_destroy_t)(IN struct tt_http_inserv_s *s);

typedef void (*tt_http_inserv_clear_t)(IN struct tt_http_inserv_s *s);

typedef tt_result_t (*tt_http_inserv_create_ctx_t)(
    IN struct tt_http_inserv_s *s, IN OPT void *ctx);

typedef void (*tt_http_inserv_destroy_ctx_t)(IN struct tt_http_inserv_s *s,
                                             IN void *ctx);

typedef void (*tt_http_inserv_clear_ctx_t)(IN struct tt_http_inserv_s *s,
                                           IN void *ctx);

typedef struct
{
    tt_http_inserv_destroy_t destroy;
    tt_http_inserv_clear_t clear;

    tt_http_inserv_create_ctx_t create_ctx;
    tt_http_inserv_destroy_ctx_t destroy_ctx;
    tt_http_inserv_clear_ctx_t clear_ctx;
} tt_http_inserv_itf_t;

typedef tt_http_inserv_action_t (*tt_http_inserv_on_ev_t)(
    IN struct tt_http_inserv_s *s, IN void *ctx,
    IN struct tt_http_parser_s *req, OUT struct tt_http_resp_render_s *resp);

// return TT_HTTP_INSERV_ACT_BODY if there are more data
typedef tt_http_inserv_action_t (*tt_http_inserv_get_body_t)(
    IN struct tt_http_inserv_s *s, IN void *ctx,
    IN struct tt_http_parser_s *req, IN struct tt_http_resp_render_s *resp,
    OUT struct tt_buf_s *buf);

typedef struct
{
    tt_http_inserv_on_ev_t on_uri;
    tt_http_inserv_on_ev_t on_header;
    tt_http_inserv_on_ev_t on_body;
    tt_http_inserv_on_ev_t on_trailing;
    tt_http_inserv_on_ev_t on_complete;
    tt_http_inserv_get_body_t get_body;
} tt_http_inserv_cb_t;

typedef struct tt_http_inserv_s
{
    tt_http_inserv_itf_t *itf;
    tt_http_inserv_cb_t *cb;
    tt_atomic_s32_t ref;
    tt_http_inserv_type_t type : 8;
} tt_http_inserv_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_http_inserv_t *tt_http_inserv_create(IN tt_http_inserv_type_t type,
                                                  IN tt_u32_t extra_size,
                                                  IN tt_http_inserv_itf_t *itf,
                                                  IN tt_http_inserv_cb_t *cb);

tt_inline void __http_inserv_destroy(IN tt_http_inserv_t *s)
{
    if (s->itf->destroy != NULL) { s->itf->destroy(s); }

    tt_free(s);
}

tt_inline void tt_http_inserv_clear(IN tt_http_inserv_t *s)
{
    if (s->itf->clear != NULL) { s->itf->clear(s); }
}

tt_inline tt_result_t tt_http_inserv_create_ctx(IN tt_http_inserv_t *s,
                                                IN OPT void *ctx)
{
    if ((s->itf->create_ctx != NULL) && (ctx != NULL)) {
        return s->itf->create_ctx(s, ctx);
    } else {
        return TT_SUCCESS;
    }
}

tt_inline void tt_http_inserv_destroy_ctx(IN tt_http_inserv_t *s, IN void *ctx)
{
    if ((s->itf->destroy_ctx != NULL) && (ctx != NULL)) {
        s->itf->destroy_ctx(s, ctx);
    }
}

tt_inline void tt_http_inserv_clear_ctx(IN tt_http_inserv_t *s, IN void *ctx)
{
    if ((s->itf->clear_ctx != NULL) && (ctx != NULL)) {
        s->itf->clear_ctx(s, ctx);
    }
}

tt_inline tt_http_inserv_action_t tt_http_inserv_on_uri(
    IN tt_http_inserv_t *s, IN void *ctx, IN struct tt_http_parser_s *req,
    OUT struct tt_http_resp_render_s *resp)
{
    if (s->cb->on_uri != NULL) {
        return s->cb->on_uri(s, ctx, req, resp);
    } else {
        return TT_HTTP_INSERV_ACT_PASS;
    }
}

tt_inline tt_http_inserv_action_t tt_http_inserv_on_header(
    IN tt_http_inserv_t *s, IN void *ctx, IN struct tt_http_parser_s *req,
    OUT struct tt_http_resp_render_s *resp)
{
    if (s->cb->on_header != NULL) {
        return s->cb->on_header(s, ctx, req, resp);
    } else {
        return TT_HTTP_INSERV_ACT_PASS;
    }
}

tt_inline tt_http_inserv_action_t tt_http_inserv_on_body(
    IN tt_http_inserv_t *s, IN void *ctx, IN struct tt_http_parser_s *req,
    OUT struct tt_http_resp_render_s *resp)
{
    if (s->cb->on_body != NULL) {
        return s->cb->on_body(s, ctx, req, resp);
    } else {
        return TT_HTTP_INSERV_ACT_PASS;
    }
}

tt_inline tt_http_inserv_action_t tt_http_inserv_on_trailing(
    IN tt_http_inserv_t *s, IN void *ctx, IN struct tt_http_parser_s *req,
    OUT struct tt_http_resp_render_s *resp)
{
    if (s->cb->on_trailing != NULL) {
        return s->cb->on_trailing(s, ctx, req, resp);
    } else {
        return TT_HTTP_INSERV_ACT_PASS;
    }
}

tt_inline tt_http_inserv_action_t tt_http_inserv_on_complete(
    IN tt_http_inserv_t *s, IN void *ctx, IN struct tt_http_parser_s *req,
    OUT struct tt_http_resp_render_s *resp)
{
    if (s->cb->on_complete != NULL) {
        return s->cb->on_complete(s, ctx, req, resp);
    } else {
        return TT_HTTP_INSERV_ACT_PASS;
    }
}

tt_inline tt_http_inserv_action_t tt_http_inserv_get_body(
    IN tt_http_inserv_t *s, IN void *ctx, IN struct tt_http_parser_s *req,
    IN struct tt_http_resp_render_s *resp, OUT struct tt_buf_s *buf)
{
    if (s->cb->get_body != NULL) {
        return s->cb->get_body(s, ctx, req, resp, buf);
    } else {
        return TT_HTTP_INSERV_ACT_PASS;
    }
}

#endif /* __TT_HTTP_IN_SERVICE__ */
