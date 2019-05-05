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
@file tt_http_out_service.s
@brief http service

this file defines http outgoing service APIs
*/

#ifndef __TT_HTTP_OUT_SERVICE__
#define __TT_HTTP_OUT_SERVICE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_double_linked_list.h>
#include <memory/tt_memory_alloc.h>
#include <misc/tt_reference_counter.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_HTTP_OUTSERV_CAST(s, name)                                          \
    TT_PTR_INC(name, s, sizeof(tt_http_outserv_t))

#define tt_http_outserv_ref(os) TT_REF_ADD(tt_http_outserv_t, os, ref)

#define tt_http_outserv_release(os)                                            \
    TT_REF_RELEASE(tt_http_outserv_t, os, ref, __http_outserv_destroy)

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_http_outserv_s;
struct tt_http_parser_s;
struct tt_http_resp_render_s;
struct tt_buf_s;

typedef void (*tt_http_outserv_destroy_t)(IN struct tt_http_outserv_s *s);

typedef void (*tt_http_outserv_clear_t)(IN struct tt_http_outserv_s *s);

typedef struct
{
    tt_http_outserv_destroy_t destroy;
    tt_http_outserv_clear_t clear;
} tt_http_outserv_itf_t;

typedef tt_result_t (*tt_http_outserv_on_header_t)(
    IN struct tt_http_outserv_s *s, IN struct tt_http_parser_s *req,
    IN OUT struct tt_http_resp_render_s *resp);

typedef struct
{
    tt_http_outserv_on_header_t on_header;
} tt_http_outserv_cb_t;

typedef struct tt_http_outserv_s
{
    tt_http_outserv_itf_t *itf;
    tt_http_outserv_cb_t *cb;
    tt_atomic_s32_t ref;
} tt_http_outserv_t;

typedef enum
{
    TT_HTTP_OUTSERV_DEFAULT,

    TT_HTTP_OUTSERV_ID_NUM
} tt_http_outserv_id_t;
#define TT_HTTP_OUTSERV_ID_VAILID(i) ((i) < TT_HTTP_OUTSERV_ID_NUM)

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_http_outserv_t *tt_http_outserv_create(
    IN tt_u32_t extra_size, IN tt_http_outserv_itf_t *itf,
    IN tt_http_outserv_cb_t *cb);

tt_inline void __http_outserv_destroy(IN tt_http_outserv_t *s)
{
    if (s->itf->destroy != NULL) { s->itf->destroy(s); }

    tt_free(s);
}

tt_inline void tt_http_outserv_clear(IN tt_http_outserv_t *s)
{
    if (s->itf->clear != NULL) { s->itf->clear(s); }
}

tt_inline tt_result_t tt_http_outserv_on_header(
    IN tt_http_outserv_t *s, IN struct tt_http_parser_s *req,
    IN OUT struct tt_http_resp_render_s *resp)
{
    if (s->cb->on_header != NULL) {
        return s->cb->on_header(s, req, resp);
    } else {
        return TT_SUCCESS;
    }
}

#endif /* __TT_HTTP_OUT_SERVICE__ */
