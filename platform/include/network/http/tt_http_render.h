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
@file tt_http_render.h
@brief http render

this file defines http
*/

#ifndef __TT_HTTP_RENDER__
#define __TT_HTTP_RENDER__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_buffer.h>
#include <algorithm/tt_double_linked_list.h>
#include <misc/tt_uri.h>
#include <network/http/def/tt_http_def.h>
#include <network/http/tt_http_content_type_map.h>
#include <network/http/tt_http_header.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_http_sconn_s;

typedef struct
{
    struct tt_http_sconn_s *c;
    tt_http_contype_map_t *contype_map;
    tt_dlist_t hdr;
    tt_buf_t buf;
    tt_http_contype_t contype;
    tt_http_ver_t version : 3;
    tt_http_conn_t conn : 2;
} tt_http_render_t;

typedef struct
{
    tt_http_contype_map_t *contype_map;
    tt_buf_attr_t buf_attr;
} tt_http_render_attr_t;

typedef struct tt_http_req_render_s
{
    tt_uri_t uri;
    tt_http_method_t method;
    tt_http_render_t render;
} tt_http_req_render_t;

typedef struct
{
    tt_http_render_attr_t render_attr;
} tt_http_req_render_attr_t;

typedef struct tt_http_resp_render_s
{
    tt_http_render_t render;
    tt_http_status_t status;
} tt_http_resp_render_t;

typedef struct
{
    tt_http_render_attr_t render_attr;
} tt_http_resp_render_attr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

// ========================================
// request
// ========================================

tt_export void tt_http_req_render_init(IN tt_http_req_render_t *req,
                                       IN OPT tt_http_req_render_attr_t *attr);

tt_export void tt_http_req_render_destroy(IN tt_http_req_render_t *req);

tt_export void tt_http_req_render_attr_default(
    IN tt_http_req_render_attr_t *attr);

tt_export void tt_http_req_render_clear(IN tt_http_req_render_t *req);

tt_inline tt_http_method_t
tt_http_req_render_get_method(IN tt_http_req_render_t *req)
{
    return req->method;
}

tt_inline void tt_http_req_render_set_method(IN tt_http_req_render_t *req,
                                             IN tt_http_method_t method)
{
    TT_ASSERT(TT_HTTP_METHOD_VALID(method));
    req->method = method;
}

tt_inline tt_uri_t *tt_http_req_render_get_uri(IN tt_http_req_render_t *req)
{
    return &req->uri;
}

tt_inline tt_http_ver_t
tt_http_req_render_get_version(IN tt_http_req_render_t *req)
{
    return req->render.version;
}

tt_inline void tt_http_req_render_set_version(IN tt_http_req_render_t *req,
                                              IN tt_http_ver_t v)
{
    TT_ASSERT(TT_HTTP_VER_VALID(v));
    req->render.version = v;
}

tt_export void tt_http_req_render_add_hdr(IN tt_http_req_render_t *req,
                                          IN tt_http_hdr_t *h);

tt_export tt_result_t
tt_http_req_render_add_line_n(IN tt_http_req_render_t *req,
                              IN tt_http_hname_t name,
                              IN tt_char_t *val,
                              IN tt_u32_t len);

tt_inline tt_result_t tt_http_req_render_add_line(IN tt_http_req_render_t *req,
                                                  IN tt_http_hname_t name,
                                                  IN const tt_char_t *val)
{
    return tt_http_req_render_add_line_n(req,
                                         name,
                                         (tt_char_t *)val,
                                         tt_strlen(val));
}

tt_export tt_result_t tt_http_req_render_add_cs(IN tt_http_req_render_t *req,
                                                IN tt_http_hname_t name,
                                                IN struct tt_blobex_s *val,
                                                IN tt_u32_t num);

tt_export tt_result_t tt_http_req_render(IN tt_http_req_render_t *req,
                                         OUT tt_char_t **data,
                                         OUT tt_u32_t *len);

tt_inline void tt_http_req_render_set_conn(IN tt_http_req_render_t *req,
                                           IN tt_http_conn_t c)
{
    TT_ASSERT(TT_HTTP_CONN_VALID(c) || (c == TT_HTTP_CONN_NUM));
    req->render.conn = c;
}

tt_inline void tt_http_req_render_set_contype(IN tt_http_req_render_t *req,
                                              IN tt_http_contype_t t)
{
    TT_ASSERT(TT_HTTP_CONTYPE_VALID(t) || (t == TT_HTTP_CONTYPE_NUM));
    req->render.contype = t;
}

// ========================================
// response
// ========================================

tt_export void tt_http_resp_render_init(
    IN tt_http_resp_render_t *resp, IN OPT tt_http_resp_render_attr_t *attr);

tt_export void tt_http_resp_render_destroy(IN tt_http_resp_render_t *resp);

tt_export void tt_http_resp_render_attr_default(
    IN tt_http_resp_render_attr_t *attr);

tt_export void tt_http_resp_render_clear(IN tt_http_resp_render_t *resp);

tt_inline tt_http_status_t
tt_http_resp_render_get_status(IN tt_http_resp_render_t *resp)
{
    return resp->status;
}

tt_inline void tt_http_resp_render_set_status(IN tt_http_resp_render_t *resp,
                                              IN tt_http_status_t status)
{
    TT_ASSERT(TT_HTTP_STATUS_VALID(status));
    resp->status = status;
}

tt_inline tt_http_ver_t
tt_http_resp_render_get_version(IN tt_http_resp_render_t *resp)
{
    return resp->render.version;
}

tt_inline void tt_http_resp_render_set_version(IN tt_http_resp_render_t *resp,
                                               IN tt_http_ver_t v)
{
    TT_ASSERT(TT_HTTP_VER_VALID(v));
    resp->render.version = v;
}

tt_export void tt_http_resp_render_add_hdr(IN tt_http_resp_render_t *resp,
                                           IN tt_http_hdr_t *h);

tt_export tt_result_t
tt_http_resp_render_add_line_n(IN tt_http_resp_render_t *resp,
                               IN tt_http_hname_t name,
                               IN tt_char_t *val,
                               IN tt_u32_t len);

tt_inline tt_result_t
tt_http_resp_render_add_line(IN tt_http_resp_render_t *resp,
                             IN tt_http_hname_t name,
                             IN const tt_char_t *val)
{
    return tt_http_resp_render_add_line_n(resp,
                                          name,
                                          (tt_char_t *)val,
                                          tt_strlen(val));
}

tt_export tt_result_t tt_http_resp_render_add_cs(IN tt_http_resp_render_t *resp,
                                                 IN tt_http_hname_t name,
                                                 IN struct tt_blobex_s *val,
                                                 IN tt_u32_t num);

tt_export tt_result_t tt_http_resp_render(IN tt_http_resp_render_t *resp,
                                          OUT tt_char_t **data,
                                          OUT tt_u32_t *len);

tt_inline void tt_http_resp_render_set_conn(IN tt_http_resp_render_t *resp,
                                            IN tt_http_conn_t c)
{
    TT_ASSERT(TT_HTTP_CONN_VALID(c) || (c == TT_HTTP_CONN_NUM));
    resp->render.conn = c;
}

tt_inline void tt_http_resp_render_set_contype(IN tt_http_resp_render_t *resp,
                                               IN tt_http_contype_t t)
{
    TT_ASSERT(TT_HTTP_CONTYPE_VALID(t) || (t == TT_HTTP_CONTYPE_NUM));
    resp->render.contype = t;
}

#endif /* __TT_HTTP_RENDER__ */
