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
@file tt_http_encoding_service.h
@brief http encoding service

this file defines http encoding service APIs
*/

#ifndef __TT_HTTP_ENCODING_SERVICE__
#define __TT_HTTP_ENCODING_SERVICE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <memory/tt_memory_alloc.h>
#include <misc/tt_reference_counter.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_HTTP_ENCSERV_CAST(s, type)                                          \
    TT_PTR_INC(type, s, sizeof(tt_http_encserv_t))

#define tt_http_encserv_ref(es) TT_REF_ADD(tt_http_encserv_t, es, ref)

#define tt_http_encserv_release(es)                                            \
    TT_REF_RELEASE(tt_http_encserv_t, es, ref, __http_encserv_destroy)

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_http_encserv_s;
struct tt_http_parser_s;
struct tt_http_resp_render_s;
struct tt_buf_s;

typedef void (*tt_http_encserv_destroy_t)(IN struct tt_http_encserv_s *s);

typedef void (*tt_http_encserv_clear_t)(IN struct tt_http_encserv_s *s);

typedef struct
{
    tt_http_encserv_destroy_t destroy;
    tt_http_encserv_clear_t clear;
} tt_http_encserv_itf_t;

// - need not to consume all data in @input, just update consumed bytes
// - must set output if returning TT_SUCCESS, do *output = input if no operation
// - input may be NULL when called as pre_body and post_body
typedef tt_result_t (*tt_http_encserv_on_body_t)(
    IN struct tt_http_encserv_s *s,
    IN struct tt_http_parser_s *req,
    IN struct tt_http_resp_render_s *resp,
    IN OUT struct tt_buf_s *input,
    OUT struct tt_buf_s **output);

typedef struct
{
    tt_http_encserv_on_body_t pre_body;
    tt_http_encserv_on_body_t on_body;
    tt_http_encserv_on_body_t post_body;
} tt_http_encserv_cb_t;

typedef struct tt_http_encserv_s
{
    tt_http_encserv_itf_t *itf;
    tt_http_encserv_cb_t *cb;
    tt_atomic_s32_t ref;
} tt_http_encserv_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_http_encserv_t *tt_http_encserv_create(
    IN tt_u32_t extra_size,
    IN tt_http_encserv_itf_t *itf,
    IN tt_http_encserv_cb_t *cb);

tt_inline void __http_encserv_destroy(IN tt_http_encserv_t *s)
{
    if (s->itf->destroy != NULL) {
        s->itf->destroy(s);
    }

    tt_free(s);
}

tt_inline void tt_http_encserv_clear(IN tt_http_encserv_t *s)
{
    if (s->itf->clear != NULL) {
        s->itf->clear(s);
    }
}

tt_inline tt_result_t
tt_http_encserv_pre_body(IN tt_http_encserv_t *s,
                         IN struct tt_http_parser_s *req,
                         IN struct tt_http_resp_render_s *resp,
                         IN OUT OPT struct tt_buf_s *input,
                         OUT struct tt_buf_s **output)
{
    if (s->cb->pre_body != NULL) {
        return s->cb->pre_body(s, req, resp, input, output);
    } else {
        *output = input;
        return TT_SUCCESS;
    }
}

tt_inline tt_result_t
tt_http_encserv_on_body(IN tt_http_encserv_t *s,
                        IN struct tt_http_parser_s *req,
                        IN struct tt_http_resp_render_s *resp,
                        IN struct tt_buf_s *input,
                        OUT struct tt_buf_s **output)
{
    if (s->cb->on_body != NULL) {
        return s->cb->on_body(s, req, resp, input, output);
    } else {
        *output = input;
        return TT_SUCCESS;
    }
}

tt_inline tt_result_t
tt_http_encserv_post_body(IN tt_http_encserv_t *s,
                          IN struct tt_http_parser_s *req,
                          IN struct tt_http_resp_render_s *resp,
                          IN OUT OPT struct tt_buf_s *input,
                          OUT struct tt_buf_s **output)
{
    if (s->cb->post_body != NULL) {
        return s->cb->post_body(s, req, resp, input, output);
    } else {
        *output = input;
        return TT_SUCCESS;
    }
}

#endif /* __TT_HTTP_ENCODING_SERVICE__ */
