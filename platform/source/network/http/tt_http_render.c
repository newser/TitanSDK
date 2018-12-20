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
#include <network/http/header/tt_http_hdr_content_encoding.h>
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

static void __render_init(IN tt_http_render_t *r,
                          IN tt_http_render_attr_t *attr);

static void __render_destroy(IN tt_http_render_t *r);

static void __render_attr_default(IN tt_http_render_attr_t *attr);

static void __render_clear(IN tt_http_render_t *r);

static void __render_add_hdr(IN tt_http_render_t *r, IN tt_http_hdr_t *h);

static tt_http_hdr_t *__rend_find_hdr(IN tt_http_render_t *render,
                                      IN tt_http_hname_t name);

static tt_u32_t __common_render_len(IN tt_http_render_t *render);

static void __common_render(IN tt_http_render_t *render, IN tt_buf_t *buf);

static void __render_set_txenc(IN tt_http_render_t *render,
                               IN OPT tt_http_txenc_t *txenc,
                               IN tt_u32_t txenc_num);

static tt_result_t __render_set_contenc(IN tt_http_render_t *render,
                                        IN OPT tt_http_enc_t *enc,
                                        IN tt_u32_t enc_num);

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

    __render_init(&req->render, &attr->render_attr);
}

void tt_http_req_render_destroy(IN tt_http_req_render_t *req)
{
    tt_uri_destroy(&req->uri);

    __render_destroy(&req->render);
}

void tt_http_req_render_attr_default(IN tt_http_req_render_attr_t *attr)
{
    __render_attr_default(&attr->render_attr);
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

        n += __common_render_len(render);
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

        __common_render(render, buf);
    }

    *data = (tt_char_t *)TT_BUF_RPOS(buf);
    *len = TT_BUF_RLEN(buf);
    return TT_SUCCESS;
}

void tt_http_req_render_set_txenc(IN tt_http_req_render_t *req,
                                  IN OPT tt_http_txenc_t *txenc,
                                  IN tt_u32_t txenc_num)
{
    __render_set_txenc(&req->render, txenc, txenc_num);
}

tt_result_t tt_http_req_render_set_contenc(IN tt_http_req_render_t *req,
                                           IN OPT tt_http_enc_t *enc,
                                           IN tt_u32_t enc_num)
{
    return __render_set_contenc(&req->render, enc, enc_num);
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

    __render_init(&resp->render, &attr->render_attr);
}

void tt_http_resp_render_destroy(IN tt_http_resp_render_t *resp)
{
    __render_destroy(&resp->render);
}

void tt_http_resp_render_attr_default(IN tt_http_resp_render_attr_t *attr)
{
    __render_attr_default(&attr->render_attr);
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

tt_result_t tt_http_resp_render(IN tt_http_resp_render_t *resp,
                                OUT tt_char_t **data,
                                OUT tt_u32_t *len)
{
    tt_http_render_t *render;
    tt_buf_t *buf;
    tt_u32_t n;
    const tt_char_t *status;
    tt_u32_t status_len;

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

    n += __common_render_len(render);

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

    __common_render(render, buf);

    *data = (tt_char_t *)TT_BUF_RPOS(buf);
    *len = TT_BUF_RLEN(buf);
    return TT_SUCCESS;
}

void tt_http_resp_render_set_txenc(IN tt_http_resp_render_t *resp,
                                   IN OPT tt_http_txenc_t *txenc,
                                   IN tt_u32_t txenc_num)
{
    __render_set_txenc(&resp->render, txenc, txenc_num);
}

tt_result_t tt_http_resp_render_set_contenc(IN tt_http_resp_render_t *resp,
                                            IN OPT tt_http_enc_t *enc,
                                            IN tt_u32_t enc_num)
{
    return __render_set_contenc(&resp->render, enc, enc_num);
}

void __render_init(IN tt_http_render_t *r, IN tt_http_render_attr_t *attr)
{
    tt_u32_t i;

    r->content_len = -1;
    r->c = NULL;
    r->contype_map = attr->contype_map;
    tt_dlist_init(&r->hdr);

    tt_buf_init(&r->buf, &attr->buf_attr);

    r->contype = TT_HTTP_CONTYPE_NUM;

    r->txenc_num = 0;
    for (i = 0; i < TT_HTTP_TXENC_NUM; ++i) {
        r->txenc[i] = TT_HTTP_TXENC_NUM;
    }

    r->version = TT_HTTP_VER_NUM;
    r->conn = TT_HTTP_CONN_NONE;
}

void __render_destroy(IN tt_http_render_t *r)
{
    tt_dnode_t *node;

    while ((node = tt_dlist_pop_head(&r->hdr)) != NULL) {
        tt_http_hdr_destroy(TT_CONTAINER(node, tt_http_hdr_t, dnode));
    }

    tt_buf_destroy(&r->buf);
}

void __render_attr_default(IN tt_http_render_attr_t *attr)
{
    attr->contype_map = &tt_g_http_contype_map;
    tt_buf_attr_default(&attr->buf_attr);
}

void __render_clear(IN tt_http_render_t *r)
{
    tt_dnode_t *node;
    tt_u32_t i;

    r->content_len = -1;

    // do not change r->c

    while ((node = tt_dlist_pop_head(&r->hdr)) != NULL) {
        tt_http_hdr_destroy(TT_CONTAINER(node, tt_http_hdr_t, dnode));
    }

    tt_buf_clear(&r->buf);

    r->contype = TT_HTTP_CONTYPE_NUM;

    r->txenc_num = 0;
    for (i = 0; i < TT_HTTP_TXENC_NUM; ++i) {
        r->txenc[i] = TT_HTTP_TXENC_NUM;
    }

    r->version = TT_HTTP_VER_NUM;
    r->conn = TT_HTTP_CONN_NONE;
}

void __render_add_hdr(IN tt_http_render_t *r, IN tt_http_hdr_t *h)
{
    tt_dlist_push_tail(&r->hdr, &h->dnode);
}

tt_http_hdr_t *__rend_find_hdr(IN tt_http_render_t *render,
                               IN tt_http_hname_t name)
{
    tt_dnode_t *dn = tt_dlist_head(&render->hdr);
    while (dn != NULL) {
        tt_http_hdr_t *h = TT_CONTAINER(dn, tt_http_hdr_t, dnode);
        if (h->name == name) {
            return h;
        }
        dn = dn->next;
    }
    return NULL;
}

tt_u32_t __common_render_len(IN tt_http_render_t *render)
{
    tt_u32_t n = 0;
    tt_dnode_t *dn;

    // "Connection: close\r\n"
    if (render->conn != TT_HTTP_CONN_NONE) {
        n += sizeof("Connection: ") - 1;
        n += tt_g_http_conn_len[render->conn];
        n += 2;
    }

    // "Content-Type: xx\r\n"
    if (render->contype != TT_HTTP_CONTYPE_NUM) {
        tt_http_contype_entry_t *e =
            tt_http_contype_map_find_type(render->contype_map, render->contype);
        TT_ASSERT(e != NULL);

        n += sizeof("Content-Type: ") - 1;
        n += e->name_len;
        n += 2;
    }

    // "Transfer-Encoding: gzip, deflate\r\n"
    if (render->txenc_num != 0) {
        tt_u32_t i;

        n += sizeof("Transfer-Encoding: ") - 1;
        for (i = 0; i < render->txenc_num; ++i) {
            // "chunked, "
            n += tt_g_http_txenc_len[render->txenc[i]] + 2;
        }
        // ", " would be replaced by "\r\n"
    }

    // "Content-Length: xxx\r\n"
    if (render->content_len >= 0) {
        // give a estimated value, max: 2,147,483,647
        n += 13;
    }

    // "Host: aa.com\r\n"
    // "\r\n"
    dn = tt_dlist_head(&render->hdr);
    while (dn != NULL) {
        n += tt_http_hdr_render_len(TT_CONTAINER(dn, tt_http_hdr_t, dnode));

        dn = dn->next;
    }
    n += 2;

    return n;
}

void __common_render(IN tt_http_render_t *render, IN tt_buf_t *buf)
{
    tt_dnode_t *dn;

    // "Connection: close\r\n"
    if (render->conn != TT_HTTP_CONN_NONE) {
        tt_buf_put(buf, (tt_u8_t *)"Connection: ", sizeof("Connection: ") - 1);
        tt_buf_put(buf,
                   (tt_u8_t *)tt_g_http_conn[render->conn],
                   tt_g_http_conn_len[render->conn]);
        tt_buf_put(buf, (tt_u8_t *)"\r\n", 2);
    }

    // "Content-Type: xx\r\n"
    if (render->contype != TT_HTTP_CONTYPE_NUM) {
        tt_http_contype_entry_t *e =
            tt_http_contype_map_find_type(render->contype_map, render->contype);
        TT_ASSERT(e != NULL);

        tt_buf_put(buf,
                   (tt_u8_t *)"Content-Type: ",
                   sizeof("Content-Type: ") - 1);
        tt_buf_put(buf, (tt_u8_t *)e->name, e->name_len);
        tt_buf_put(buf, (tt_u8_t *)"\r\n", 2);
    }

    if (render->txenc_num != 0) {
        tt_u32_t i;

        tt_buf_put(buf,
                   (tt_u8_t *)"Transfer-Encoding: ",
                   sizeof("Transfer-Encoding: ") - 1);
        for (i = 0; i < render->txenc_num; ++i) {
            // "chunked, "
            tt_buf_put(buf,
                       (tt_u8_t *)tt_g_http_txenc[render->txenc[i]],
                       tt_g_http_txenc_len[render->txenc[i]]);
            if (i != (render->txenc_num - 1)) {
                tt_buf_put(buf, (tt_u8_t *)", ", 2);
            }
        }
        tt_buf_put(buf, (tt_u8_t *)"\r\n", 2);
    }

    // "Content-Length: xxx\r\n"
    if (render->content_len >= 0) {
        tt_char_t s[11] = {0};
        tt_buf_put(buf,
                   (tt_u8_t *)"Content-Length: ",
                   sizeof("Content-Length: ") - 1);
        tt_snprintf(s, sizeof(s) - 1, "%d", render->content_len);
        tt_buf_put_cstr(buf, s);
        tt_buf_put(buf, (tt_u8_t *)"\r\n", 2);
    }

    // "Host: aa.com\r\n"
    // "\r\n"
    dn = tt_dlist_head(&render->hdr);
    while (dn != NULL) {
        tt_u32_t n = tt_http_hdr_render(TT_CONTAINER(dn, tt_http_hdr_t, dnode),
                                        (tt_char_t *)TT_BUF_WPOS(buf));
        tt_buf_inc_wp(buf, n);

        dn = dn->next;
    }
    tt_buf_put(buf, (tt_u8_t *)"\r\n", 2);
}

void __render_set_txenc(IN tt_http_render_t *render,
                        IN OPT tt_http_txenc_t *txenc,
                        IN tt_u32_t txenc_num)
{
    tt_u32_t num, i, k;

    TT_ASSERT(txenc_num <= TT_HTTP_TXENC_NUM);

    if (txenc == NULL) {
        render->txenc_num = 0;
        return;
    }

    num = 0;
    for (i = 0; i < txenc_num; ++i) {
        TT_ASSERT(TT_HTTP_TXENC_VALID(txenc[i]));

        for (k = 0; k < i; ++k) {
            if (render->txenc[k] == txenc[i]) {
                TT_WARN("txenc[%d] already exist", i);
                break;
            }
        }

        if (k == i) {
            render->txenc[num++] = (tt_u8_t)txenc[i];
        }
    }
    render->txenc_num = num;
}

tt_result_t __render_set_contenc(IN tt_http_render_t *render,
                                 IN OPT tt_http_enc_t *enc,
                                 IN tt_u32_t enc_num)
{
    tt_http_hdr_t *h = __rend_find_hdr(render, TT_HTTP_HDR_CONTENC);
    if (h == NULL) {
        h = tt_http_hdr_contenc_create();
        if (h == NULL) {
            return TT_FAIL;
        }
        __render_add_hdr(render, h);
    }

    tt_http_hdr_contenc_set(h, enc, enc_num);
    return TT_SUCCESS;
}
