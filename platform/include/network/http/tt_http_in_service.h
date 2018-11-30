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

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_HTTP_INSERV_CAST(hv, name)                                          \
    TT_PTR_INC(name, hv, sizeof(tt_http_inserv_t))

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_http_inserv_s;
struct tt_http_parser_s;
struct tt_http_resp_render_s;

typedef enum {
    TT_HTTP_INSERV_ACT_CLOSE,
    TT_HTTP_INSERV_ACT_SHUTDOWN,
    TT_HTTP_INSERV_ACT_DISCARD,
    TT_HTTP_INSERV_ACT_IGNORE,
    TT_HTTP_INSERV_ACT_INTERESTED,
    TT_HTTP_INSERV_ACT_OWNER,

    TT_HTTP_INSERV_ACT_NUM
} tt_http_inserv_action_t;
#define TT_HTTP_INSERV_ACT_VALID(a) ((a) < TT_HTTP_INSERV_ACT_NUM)

typedef void (*tt_http_inserv_destroy_t)(IN struct tt_http_inserv_s *s);

typedef void (*tt_http_inserv_clear_t)(IN struct tt_http_inserv_s *s);

typedef struct
{
    tt_http_inserv_destroy_t destroy;
    tt_http_inserv_clear_t clear;
} tt_http_inserv_itf_t;

typedef tt_http_inserv_action_t (*tt_http_inserv_on_ev_t)(
    IN struct tt_http_inserv_s *s,
    IN struct tt_http_parser_s *req,
    OUT struct tt_http_resp_render_s *resp);

typedef struct
{
    tt_http_inserv_on_ev_t on_uri;
    tt_http_inserv_on_ev_t on_header;
    tt_http_inserv_on_ev_t on_body;
    tt_http_inserv_on_ev_t on_trailing;
    tt_http_inserv_on_ev_t on_complete;
} tt_http_inserv_cb_t;

typedef struct tt_http_inserv_s
{
    tt_http_inserv_itf_t *itf;
    tt_http_inserv_cb_t *cb;
    tt_dnode_t dnode;
} tt_http_inserv_t;

typedef enum {
    TT_HTTP_INSERV_DEFAULT,

    TT_HTTP_INSERV_ID_NUM
} tt_http_inserv_id_t;
#define TT_HTTP_INSERV_ID_VAILID(i) ((i) < TT_HTTP_INSERV_ID_NUM)

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_http_inserv_t *tt_http_inserv_init(IN tt_u32_t extra_size,
                                                IN tt_http_inserv_itf_t *itf,
                                                IN tt_http_inserv_cb_t *cb);

tt_inline void tt_http_inserv_destroy(IN tt_http_inserv_t *s)
{
    TT_ASSERT(!tt_dnode_in_dlist(&s->dnode));

    if (s->itf->destroy != NULL) {
        s->itf->destroy(s);
    }

    tt_free(s);
}

tt_inline void tt_http_inserv_clear(IN tt_http_inserv_t *s)
{
    if (s->itf->clear != NULL) {
        s->itf->clear(s);
    }
}

tt_inline tt_http_inserv_action_t
tt_http_inserv_on_uri(IN tt_http_inserv_t *s,
                      IN struct tt_http_parser_s *req,
                      OUT struct tt_http_resp_render_s *resp)
{
    if (s->cb->on_uri != NULL) {
        return s->cb->on_uri(s, req, resp);
    } else {
        return TT_HTTP_INSERV_ACT_IGNORE;
    }
}

tt_inline tt_http_inserv_action_t
tt_http_inserv_on_header(IN tt_http_inserv_t *s,
                         IN struct tt_http_parser_s *req,
                         OUT struct tt_http_resp_render_s *resp)
{
    if (s->cb->on_header != NULL) {
        return s->cb->on_header(s, req, resp);
    } else {
        return TT_HTTP_INSERV_ACT_IGNORE;
    }
}

tt_inline tt_http_inserv_action_t
tt_http_inserv_on_body(IN tt_http_inserv_t *s,
                       IN struct tt_http_parser_s *req,
                       OUT struct tt_http_resp_render_s *resp)
{
    if (s->cb->on_body != NULL) {
        return s->cb->on_body(s, req, resp);
    } else {
        return TT_HTTP_INSERV_ACT_IGNORE;
    }
}

tt_inline tt_http_inserv_action_t
tt_http_inserv_on_trailing(IN tt_http_inserv_t *s,
                           IN struct tt_http_parser_s *req,
                           OUT struct tt_http_resp_render_s *resp)
{
    if (s->cb->on_trailing != NULL) {
        return s->cb->on_trailing(s, req, resp);
    } else {
        return TT_HTTP_INSERV_ACT_IGNORE;
    }
}

tt_inline tt_http_inserv_action_t
tt_http_inserv_on_complete(IN tt_http_inserv_t *s,
                           IN struct tt_http_parser_s *req,
                           OUT struct tt_http_resp_render_s *resp)
{
    if (s->cb->on_complete != NULL) {
        return s->cb->on_complete(s, req, resp);
    } else {
        return TT_HTTP_INSERV_ACT_IGNORE;
    }
}

#endif /* __TT_HTTP_IN_SERVICE__ */
