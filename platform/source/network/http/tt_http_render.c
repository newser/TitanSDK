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

#include <network/http/tt_http_render.h>

#include <algorithm/tt_buffer_format.h>
#include <network/http/tt_http_header.h>

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

// tt_g_uri_encode_table with "/;=&" unescaped
static tt_char_t tt_s_path_enc_tbl[256] = {
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   '&', 0,   0,   0,   0,   0,   0,
    '-', '.', '/', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 0,   ';',
    0,   '=', 0,   0,   0,   'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y',
    'Z', 0,   0,   0,   0,   '_', 0,   'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
    'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w',
    'x', 'y', 'z', 0,   0,   0,   '~', 0,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static void __render_init(IN tt_http_render_t *r, IN tt_buf_attr_t *buf_attr);

static void __render_destroy(IN tt_http_render_t *r);

static void __render_attr_default(IN tt_http_req_render_attr_t *attr);

static void __render_clear(IN tt_http_render_t *r);

static void __render_add_hdr(IN tt_http_render_t *r, IN tt_http_hdr_t *h);

static tt_result_t __render_add_line_n(IN tt_http_render_t *r,
                                       IN tt_http_hname_t name,
                                       IN tt_char_t *val,
                                       IN tt_u32_t len);

static tt_result_t __render_add_cs(IN tt_http_render_t *r,
                                   IN tt_http_hname_t name,
                                   IN tt_blobex_t *val,
                                   IN tt_u32_t num);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_http_req_render_init(IN tt_http_req_render_t *req,
                             IN OPT tt_http_req_render_attr_t *attr)
{
    tt_http_req_render_attr_t __attr;

    TT_ASSERT(req != NULL);

    if (attr == NULL) {
        tt_http_req_render_attr_default(&__attr);
        attr = &__attr;
    }

    tt_uri_init(&req->uri);

    req->method = TT_HTTP_METHOD_NUM;

    __render_init(&req->render, &attr->buf_attr);
}

void tt_http_req_render_destroy(IN tt_http_req_render_t *req)
{
    tt_uri_destroy(&req->uri);

    __render_destroy(&req->render);
}

void tt_http_req_render_attr_default(IN tt_http_req_render_attr_t *attr)
{
    tt_buf_attr_default(&attr->buf_attr);
}

void tt_http_req_render_clear(IN tt_http_req_render_t *req)
{
    tt_uri_clear(&req->uri);

    req->method = TT_HTTP_METHOD_NUM;

    __render_clear(&req->render);
}

void tt_http_req_render_add_hdr(IN tt_http_req_render_t *req,
                                IN tt_http_hdr_t *h)
{
    __render_add_hdr(&req->render, h);
}

tt_result_t tt_http_req_render_add_line_n(IN tt_http_req_render_t *req,
                                          IN tt_http_hname_t name,
                                          IN tt_char_t *val,
                                          IN tt_u32_t len)
{
    return __render_add_line_n(&req->render, name, val, len);
}

tt_result_t tt_http_req_render_add_cs(IN tt_http_req_render_t *req,
                                      IN tt_http_hname_t name,
                                      IN tt_blobex_t *val,
                                      IN tt_u32_t num)
{
    return __render_add_cs(&req->render, name, val, num);
}

tt_result_t tt_http_req_render(IN tt_http_req_render_t *req,
                               OUT tt_char_t **data,
                               OUT tt_u32_t *len)
{
    tt_http_render_t *render;
    tt_buf_t *buf;
    tt_u32_t n;
    tt_uri_encode_table_t uet;
    tt_string_t *u;
    tt_dnode_t *dn;

    TT_ASSERT(TT_HTTP_METHOD_VALID(req->method));
    TT_ASSERT(TT_HTTP_VER_VALID(req->render.version));

    render = &req->render;
    buf = &render->buf;

    tt_buf_clear(buf);
    n = 0;

    // ========================================
    // render len
    // ========================================

    // "GET "
    n += tt_g_http_method_len[req->method] + 1;

    // "GET /a/b/c"
    tt_uri_encode_table_default(&uet);
    uet.path = tt_s_path_enc_tbl;

    if (tt_uri_render(&req->uri, &uet) == NULL) {
        return TT_FAIL;
    }
    u = &req->uri.uri;
    n += tt_string_len(u);

    if (render->version != TT_HTTP_V0_9) {
        // "GET /a/b/c HTTP/1.1\r\n"
        n += 3;
        n += tt_g_http_verion_len[render->version];

        // "GET /a/b/c HTTP/1.1\r\n"
        // "Host: aa.com\r\n"
        // "\r\n"
        dn = tt_dlist_head(&render->hdr);
        while (dn != NULL) {
            n += tt_http_hdr_render_len(TT_CONTAINER(dn, tt_http_hdr_t, dnode));

            dn = dn->next;
        }
        n += 2;
    }

    if (!TT_OK(tt_buf_reserve(buf, n))) {
        return TT_E_NOMEM;
    }

    // ========================================
    // render
    // ========================================

    // "GET "
    tt_buf_put(buf,
               (tt_u8_t *)tt_g_http_method[req->method],
               tt_g_http_method_len[req->method]);
    tt_buf_put_u8(buf, ' ');

    // "GET /a/b/c"
    tt_buf_put(buf, (tt_u8_t *)tt_string_cstr(u), tt_string_len(u));

    if (render->version != TT_HTTP_V0_9) {
        // "GET /a/b/c HTTP/1.1\r\n"
        tt_buf_put_u8(buf, ' ');
        tt_buf_put(buf,
                   (tt_u8_t *)tt_g_http_verion[render->version],
                   tt_g_http_verion_len[render->version]);
        tt_buf_put(buf, (tt_u8_t *)"\r\n", 2);

        // "GET /a/b/c HTTP/1.1\r\n"
        // "Host: aa.com\r\n"
        // "\r\n"
        dn = tt_dlist_head(&render->hdr);
        while (dn != NULL) {
            n = tt_http_hdr_render(TT_CONTAINER(dn, tt_http_hdr_t, dnode),
                                   (tt_char_t *)TT_BUF_WPOS(buf));
            tt_buf_inc_wp(buf, n);

            dn = dn->next;
        }
        tt_buf_put(buf, (tt_u8_t *)"\r\n", 2);
    }

    *data = (tt_char_t *)TT_BUF_RPOS(buf);
    *len = TT_BUF_RLEN(buf);
    return TT_SUCCESS;
}

void tt_http_resp_render_init(IN tt_http_resp_render_t *resp,
                              IN OPT tt_http_resp_render_attr_t *attr)
{
    tt_http_resp_render_attr_t __attr;

    TT_ASSERT(resp != NULL);

    if (attr == NULL) {
        tt_http_resp_render_attr_default(&__attr);
        attr = &__attr;
    }

    resp->status = TT_HTTP_STATUS_INVALID;

    __render_init(&resp->render, &attr->buf_attr);
}

void tt_http_resp_render_destroy(IN tt_http_resp_render_t *resp)
{
    __render_destroy(&resp->render);
}

void tt_http_resp_render_attr_default(IN tt_http_resp_render_attr_t *attr)
{
    tt_buf_attr_default(&attr->buf_attr);
}

void tt_http_resp_render_clear(IN tt_http_resp_render_t *resp)
{
    resp->status = TT_HTTP_STATUS_INVALID;

    __render_clear(&resp->render);
}

void tt_http_resp_render_add_hdr(IN tt_http_resp_render_t *resp,
                                 IN tt_http_hdr_t *h)
{
    __render_add_hdr(&resp->render, h);
}

tt_result_t tt_http_resp_render_add_line_n(IN tt_http_resp_render_t *resp,
                                           IN tt_http_hname_t name,
                                           IN tt_char_t *val,
                                           IN tt_u32_t len)
{
    return __render_add_line_n(&resp->render, name, val, len);
}

tt_result_t tt_http_resp_render_add_cs(IN tt_http_resp_render_t *resp,
                                       IN tt_http_hname_t name,
                                       IN tt_blobex_t *val,
                                       IN tt_u32_t num)
{
    return __render_add_cs(&resp->render, name, val, num);
}

tt_result_t tt_http_resp_render(IN tt_http_resp_render_t *resp,
                                OUT tt_char_t **data,
                                OUT tt_u32_t *len)
{
    tt_http_render_t *render;
    tt_buf_t *buf;
    tt_u32_t n;
    const tt_char_t *status;
    tt_u32_t status_len;
    tt_dnode_t *dn;

    TT_ASSERT(TT_HTTP_STATUS_VALID(resp->status));
    TT_ASSERT(TT_HTTP_VER_VALID(resp->render.version) &&
              (resp->render.version != TT_HTTP_V0_9));

    render = &resp->render;
    buf = &render->buf;

    tt_buf_clear(buf);
    n = 0;

    // ========================================
    // render len
    // ========================================

    // "HTTP/1.1 "
    n += tt_g_http_verion_len[render->version];
    n += 1;

    // "HTTP/1.1 200 OK\r\n"
    tt_http_status_cstr(resp->status, &status, &status_len);
    n += 3;
    n += status_len;

    dn = tt_dlist_head(&render->hdr);
    while (dn != NULL) {
        n += tt_http_hdr_render_len(TT_CONTAINER(dn, tt_http_hdr_t, dnode));

        dn = dn->next;
    }
    n += 2;

    if (!TT_OK(tt_buf_reserve(buf, n))) {
        return TT_E_NOMEM;
    }

    // ========================================
    // render
    // ========================================

    // "HTTP/1.1 "
    tt_buf_put(buf,
               (tt_u8_t *)tt_g_http_verion[render->version],
               tt_g_http_verion_len[render->version]);
    tt_buf_put_u8(buf, ' ');

    // "HTTP/1.1 200 OK\r\n"
    tt_buf_put(buf, (tt_u8_t *)status, status_len);
    tt_buf_put(buf, (tt_u8_t *)"\r\n", 2);

    // "HTTP/1.1 200 OK\r\n"
    // "Host: aa.com\r\n"
    // "\r\n"
    dn = tt_dlist_head(&render->hdr);
    while (dn != NULL) {
        n = tt_http_hdr_render(TT_CONTAINER(dn, tt_http_hdr_t, dnode),
                               (tt_char_t *)TT_BUF_WPOS(buf));
        tt_buf_inc_wp(buf, n);

        dn = dn->next;
    }
    tt_buf_put(buf, (tt_u8_t *)"\r\n", 2);

    *data = (tt_char_t *)TT_BUF_RPOS(buf);
    *len = TT_BUF_RLEN(buf);
    return TT_SUCCESS;
}

void __render_init(IN tt_http_render_t *r, IN tt_buf_attr_t *buf_attr)
{
    tt_dlist_init(&r->hdr);

    tt_buf_init(&r->buf, buf_attr);

    r->version = TT_HTTP_VER_NUM;
}

void __render_destroy(IN tt_http_render_t *r)
{
    tt_dnode_t *node;

    while ((node = tt_dlist_pop_head(&r->hdr)) != NULL) {
        tt_http_hdr_destroy(TT_CONTAINER(node, tt_http_hdr_t, dnode));
    }

    tt_buf_destroy(&r->buf);
}

void __render_clear(IN tt_http_render_t *r)
{
    tt_dnode_t *node;

    while ((node = tt_dlist_pop_head(&r->hdr)) != NULL) {
        tt_http_hdr_destroy(TT_CONTAINER(node, tt_http_hdr_t, dnode));
    }

    tt_buf_clear(&r->buf);

    r->version = TT_HTTP_VER_NUM;
}

void __render_add_hdr(IN tt_http_render_t *r, IN tt_http_hdr_t *h)
{
    tt_dlist_push_tail(&r->hdr, &h->dnode);
}

tt_result_t __render_add_line_n(IN tt_http_render_t *r,
                                IN tt_http_hname_t name,
                                IN tt_char_t *val,
                                IN tt_u32_t len)
{
    tt_http_hdr_t *h;

    h = tt_http_hdr_create_line_n(name, val, len);
    if (h == NULL) {
        return TT_E_NOMEM;
    }

    __render_add_hdr(r, h);

    return TT_SUCCESS;
}

tt_result_t __render_add_cs(IN tt_http_render_t *r,
                            IN tt_http_hname_t name,
                            IN tt_blobex_t *val,
                            IN tt_u32_t num)
{
    tt_http_hdr_t *h;

    h = tt_http_hdr_create_cs(name, val, num);
    if (h == NULL) {
        return TT_E_NOMEM;
    }

    __render_add_hdr(r, h);

    return TT_SUCCESS;
}
