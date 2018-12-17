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

#include <network/http/tt_http_header.h>

#include <algorithm/tt_blobex.h>
#include <memory/tt_memory_alloc.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef struct
{
    tt_blobex_t bex;
} __hval_blobex_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_http_hval_t *__hv_bex_create(IN tt_http_hdr_t *h);

static void __hv_bex_destroy(IN tt_http_hval_t *hv);

static tt_result_t __hv_bex_parse(IN tt_http_hval_t *hv,
                                  IN const tt_char_t *val,
                                  IN tt_u32_t len);

static tt_u32_t __hv_bex_render_len(IN tt_http_hval_t *hv);

static tt_u32_t __hv_bex_render(IN tt_http_hval_t *hv, IN tt_char_t *dst);

tt_http_hval_itf_t tt_g_http_hval_blob_itf = {__hv_bex_create,
                                              __hv_bex_destroy,
                                              __hv_bex_parse,
                                              __hv_bex_render_len,
                                              __hv_bex_render};

// ========================================
// header: whole line
// ========================================

static tt_result_t __h_line_parse(IN tt_http_hdr_t *h,
                                  IN const tt_char_t *val,
                                  IN tt_u32_t len);

static tt_u32_t __h_line_render_len(IN tt_http_hdr_t *h);

static tt_u32_t __h_line_render(IN tt_http_hdr_t *h, IN tt_char_t *dst);

tt_http_hdr_itf_t tt_g_http_hdr_line_itf = {__h_line_parse,
                                            __h_line_render_len,
                                            __h_line_render};

// ========================================
// header: comma separated
// ========================================

static tt_result_t __h_cs_parse(IN tt_http_hdr_t *h,
                                IN const tt_char_t *val,
                                IN tt_u32_t len);

static tt_u32_t __h_cs_render_len(IN tt_http_hdr_t *h);

static tt_u32_t __h_cs_render(IN tt_http_hdr_t *h, IN tt_char_t *dst);

tt_http_hdr_itf_t tt_g_http_hdr_cs_itf = {__h_cs_parse,
                                          __h_cs_render_len,
                                          __h_cs_render};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static void __clear_hval(IN tt_http_hdr_t *h);

static tt_result_t __add_hval(IN tt_http_hdr_t *h,
                              IN const tt_char_t *val,
                              IN tt_u32_t len);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_http_hval_t *tt_http_hval_create(IN tt_u32_t extra_size)
{
    tt_http_hval_t *hv;

    hv = tt_malloc(sizeof(tt_http_hval_t) + extra_size);
    if (hv == NULL) {
        TT_ERROR("no mem for http val");
        return NULL;
    }

    tt_dnode_init(&hv->dnode);

    return hv;
}

void tt_http_hval_destroy(IN tt_http_hval_t *hv)
{
    TT_ASSERT(!tt_dnode_in_dlist(&hv->dnode));

    tt_free(hv);
}

tt_http_hdr_t *tt_http_hdr_create(IN tt_u32_t extra_size,
                                  IN tt_http_hname_t name,
                                  IN tt_http_hdr_itf_t *itf,
                                  IN tt_http_hval_itf_t *val_itf)
{
    tt_http_hdr_t *h;

    TT_ASSERT(TT_HTTP_HNAME_VALID(name));
    TT_ASSERT((itf != NULL) && (itf->parse != NULL) &&
              (itf->render_len != NULL) && (itf->render != NULL));
    TT_ASSERT((val_itf != NULL) && (val_itf->create != NULL) &&
              (val_itf->destroy != NULL) && (val_itf->parse != NULL) &&
              (val_itf->render_len != NULL) && (val_itf->render != NULL));

    h = tt_malloc(sizeof(tt_http_hdr_t) + extra_size);
    if (h == NULL) {
        TT_ERROR("no mem for http hdr");
        return NULL;
    }

    h->itf = itf;
    h->val_itf = val_itf;
    tt_dlist_init(&h->val);
    tt_dnode_init(&h->dnode);
    h->name = name;

    return h;
}

void tt_http_hdr_destroy(IN tt_http_hdr_t *h)
{
    TT_ASSERT(h != NULL);
    TT_ASSERT(!tt_dnode_in_dlist(&h->dnode));

    __clear_hval(h);

    tt_free(h);
}

void tt_http_hdr_clear(IN tt_http_hdr_t *h)
{
    TT_ASSERT(h != NULL);

    __clear_hval(h);
}

tt_http_hdr_t *tt_http_hdr_create_line_n(IN tt_http_hname_t name,
                                         IN tt_char_t *val,
                                         IN tt_u32_t len)
{
    tt_http_hdr_t *h;

    h = tt_http_hdr_create(0,
                           name,
                           &tt_g_http_hdr_line_itf,
                           &tt_g_http_hval_blob_itf);
    if (h == NULL) {
        return NULL;
    }

    if (!TT_OK(__add_hval(h, val, len))) {
        tt_http_hdr_destroy(h);
        return NULL;
    }

    return h;
}

tt_http_hdr_t *tt_http_hdr_create_cs(IN tt_http_hname_t name,
                                     IN tt_blobex_t *val,
                                     IN tt_u32_t num)
{
    tt_http_hdr_t *h;
    tt_u32_t i;

    h = tt_http_hdr_create(0,
                           name,
                           &tt_g_http_hdr_cs_itf,
                           &tt_g_http_hval_blob_itf);
    if (h == NULL) {
        return NULL;
    }

    for (i = 0; i < num; ++i) {
        tt_blobex_t *bex = &val[i];
        if (!TT_OK(__add_hval(h, tt_blobex_addr(bex), tt_blobex_len(bex)))) {
            tt_http_hdr_destroy(h);
            return NULL;
        }
    }

    return h;
}

void __clear_hval(IN tt_http_hdr_t *h)
{
    tt_dnode_t *node;
    while ((node = tt_dlist_pop_head(&h->val)) != NULL) {
        h->val_itf->destroy(TT_CONTAINER(node, tt_http_hval_t, dnode));
    }
}

tt_result_t __add_hval(IN tt_http_hdr_t *h,
                       IN const tt_char_t *val,
                       IN tt_u32_t len)
{
    tt_http_hval_t *hv = h->val_itf->create(h);
    if (hv == NULL) {
        return TT_FAIL;
    }

    if (!TT_OK(h->val_itf->parse(hv, val, len))) {
        h->val_itf->destroy(hv);
        return TT_FAIL;
    }

    tt_http_hdr_add(h, hv);
    return TT_SUCCESS;
}

// ========================================
// header value: a blob
// ========================================

tt_http_hval_t *__hv_bex_create(IN tt_http_hdr_t *h)
{
    tt_http_hval_t *hv;
    __hval_blobex_t *hvb;

    hv = tt_http_hval_create(sizeof(__hval_blobex_t));
    if (hv == NULL) {
        TT_ERROR("no mem for hval blob");
        return NULL;
    }

    hvb = TT_HTTP_HVAL_CAST(hv, __hval_blobex_t);

    tt_blobex_init(&hvb->bex, NULL, 0);

    return hv;
}

void __hv_bex_destroy(IN tt_http_hval_t *hv)
{
    __hval_blobex_t *hvb = TT_HTTP_HVAL_CAST(hv, __hval_blobex_t);

    tt_blobex_destroy(&hvb->bex);

    tt_http_hval_destroy(hv);
}

tt_result_t __hv_bex_parse(IN tt_http_hval_t *hv,
                           IN const tt_char_t *val,
                           IN tt_u32_t len)
{
    __hval_blobex_t *hvb = TT_HTTP_HVAL_CAST(hv, __hval_blobex_t);

    return tt_blobex_set(&hvb->bex, (tt_u8_t *)val, len, TT_TRUE);
}

tt_u32_t __hv_bex_render_len(IN tt_http_hval_t *hv)
{
    __hval_blobex_t *hvb = TT_HTTP_HVAL_CAST(hv, __hval_blobex_t);

    return tt_blobex_len(&hvb->bex);
}

tt_u32_t __hv_bex_render(IN tt_http_hval_t *hv, IN tt_char_t *dst)
{
    __hval_blobex_t *hvb = TT_HTTP_HVAL_CAST(hv, __hval_blobex_t);
    tt_u32_t len = tt_blobex_len(&hvb->bex);

    tt_memcpy(dst, tt_blobex_addr(&hvb->bex), len);
    return len;
}

// ========================================
// header: whole line
// ========================================

tt_result_t __h_line_parse(IN tt_http_hdr_t *h,
                           IN const tt_char_t *val,
                           IN tt_u32_t len)
{
    return __add_hval(h, val, len);
}

tt_u32_t __h_line_render_len(IN tt_http_hdr_t *h)
{
    tt_u32_t namelen, len;
    tt_dnode_t *node;

    // "Host" + ": "
    namelen = tt_g_http_hname_len[h->name] + 2;

    len = 0;
    node = tt_dlist_head(&h->val);
    while (node != NULL) {
        tt_http_hval_t *hv = TT_CONTAINER(node, tt_http_hval_t, dnode);

        node = node->next;

        len += namelen; // => "Host: "
        len += h->val_itf->render_len(hv); // => "Host: xxx"
        len += 2; // => "Host: xxx\r\n"
    }

    return len;
}

tt_u32_t __h_line_render(IN tt_http_hdr_t *h, IN tt_char_t *dst)
{
    const tt_char_t *name;
    tt_u32_t namelen;
    tt_dnode_t *node;
    tt_char_t *p = dst;

    name = tt_g_http_hname[h->name];
    namelen = tt_g_http_hname_len[h->name];

    node = tt_dlist_head(&h->val);
    while (node != NULL) {
        tt_http_hval_t *hv = TT_CONTAINER(node, tt_http_hval_t, dnode);

        node = node->next;

        tt_memcpy(p, name, namelen);
        p += namelen; // => "Host"

        *p++ = ':';
        *p++ = ' '; // => "Host: "

        p += h->val_itf->render(hv, p); // => "Host: xxx"

        *p++ = '\r';
        *p++ = '\n';
    }

    return (tt_u32_t)(p - dst);
}

// ========================================
// header: comma separated
// ========================================

tt_result_t __h_cs_parse(IN tt_http_hdr_t *h,
                         IN const tt_char_t *val,
                         IN tt_u32_t len)
{
    tt_char_t *p, *end, *prev;
    tt_u32_t n;

    p = (tt_char_t *)val;
    end = (tt_char_t *)val + len;
    prev = p;
    n = len;
    while (p < end) {
        p = tt_memchr(p, ',', n);
        if (p == NULL) {
            break;
        }

        if ((p > prev) && !TT_OK(__add_hval(h, prev, (tt_u32_t)(p - prev)))) {
            // continue, as there may be successfully parsed hvals
            TT_FATAL("lost a http value");
        }

        ++p;
        prev = p;
        TT_ASSERT(p <= end);
        n = (tt_u32_t)(end - p);
    }
    TT_ASSERT(prev <= end);
    if (prev < end) {
        if (!TT_OK(__add_hval(h, prev, (tt_u32_t)(end - prev)))) {
            TT_FATAL("lost a http value");
        }
    }

    return TT_SUCCESS;
}

tt_u32_t __h_cs_render_len(IN tt_http_hdr_t *h)
{
    tt_u32_t len;
    tt_dnode_t *node;

    // "Host" + ": "
    len = tt_g_http_hname_len[h->name] + 2;

    node = tt_dlist_head(&h->val);
    while (node != NULL) {
        tt_http_hval_t *hv = TT_CONTAINER(node, tt_http_hval_t, dnode);

        node = node->next;

        len += h->val_itf->render_len(hv); // => "Host: xxx"
        len += 2; // => "Host: xxx, "
    }

    // final 2 chars(", ") are replaced with "\r\n"
    return len;
}

tt_u32_t __h_cs_render(IN tt_http_hdr_t *h, IN tt_char_t *dst)
{
    tt_u32_t namelen;
    tt_dnode_t *node;
    tt_char_t *p = dst;
    tt_bool_t first = TT_TRUE;

    namelen = tt_g_http_hname_len[h->name];
    tt_memcpy(p, tt_g_http_hname[h->name], namelen);
    p += namelen; // => "Host"

    *p++ = ':';
    *p++ = ' '; // => "Host: "

    node = tt_dlist_head(&h->val);
    while (node != NULL) {
        tt_http_hval_t *hv = TT_CONTAINER(node, tt_http_hval_t, dnode);

        node = node->next;

        if (first) {
            first = TT_FALSE;
        } else {
            *p++ = ',';
            *p++ = ' ';
        }
        p += h->val_itf->render(hv, p); // => "Host: xxx, yyy"
    }

    *p++ = '\r';
    *p++ = '\n';

    return (tt_u32_t)(p - dst);
}