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

#include <network/http/header/tt_http_hdr_etag.h>

#include <network/http/tt_http_parser.h>

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

// ========================================
// etag value interface
// ========================================

static tt_http_hval_t *__etag_val_create(IN tt_http_hdr_t *h);

static void __etag_val_destroy(IN tt_http_hval_t *hv);

static tt_result_t __etag_val_parse(IN tt_http_hval_t *hv,
                                    IN const tt_char_t *val,
                                    IN tt_u32_t len);

static tt_u32_t __etag_val_render_len(IN tt_http_hval_t *hv);

static tt_u32_t __etag_val_render(IN tt_http_hval_t *hv, IN tt_char_t *dst);

tt_http_hval_itf_t __etag_val_itf = {__etag_val_create,
                                     __etag_val_destroy,
                                     __etag_val_parse,
                                     __etag_val_render_len,
                                     __etag_val_render};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static void __etag_init(IN tt_http_etag_t *e);

static void __etag_destroy(IN tt_http_etag_t *e);

static tt_result_t __add_etag(IN tt_http_hdr_t *h,
                              IN tt_char_t *etag,
                              IN tt_u32_t len,
                              IN tt_bool_t weak);

static tt_result_t __add_aster(IN tt_http_hdr_t *h);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_http_hdr_t *tt_http_hdr_etag_create()
{
    tt_http_hdr_t *h;

    h = tt_http_hdr_create(0,
                           TT_HTTP_HDR_ETAG,
                           &tt_g_http_hdr_cs_itf,
                           &__etag_val_itf);
    if (h == NULL) {
        return NULL;
    }

    return h;
}

tt_result_t tt_http_hdr_etag_add_n(IN tt_http_hdr_t *h,
                                   IN tt_char_t *etag,
                                   IN tt_u32_t len,
                                   IN tt_bool_t weak)
{
    TT_ASSERT(h->name == TT_HTTP_HDR_ETAG);

    return __add_etag(h, etag, len, weak);
}

void __etag_init(IN tt_http_etag_t *e)
{
    tt_blobex_init(&e->etag, NULL, 0);
    e->weak = TT_FALSE;
    e->aster = TT_FALSE;
}

void __etag_destroy(IN tt_http_etag_t *e)
{
    tt_blobex_destroy(&e->etag);
}

tt_result_t __add_etag(IN tt_http_hdr_t *h,
                       IN tt_char_t *etag,
                       IN tt_u32_t len,
                       IN tt_bool_t weak)
{
    tt_http_hval_t *hv;
    tt_http_etag_t *e;
    tt_char_t *p;

    hv = __etag_val_create(h);
    if (hv == NULL) {
        return TT_E_NOMEM;
    }
    e = TT_HTTP_HVAL_CAST(hv, tt_http_etag_t);

    if (!TT_OK(tt_blobex_set(&e->etag, (tt_u8_t *)etag, len, TT_TRUE))) {
        __etag_val_destroy(hv);
        return TT_FAIL;
    }

    e->weak = weak;

    tt_http_hdr_add(h, hv);
    return TT_SUCCESS;
}

tt_result_t __add_aster(IN tt_http_hdr_t *h)
{
    tt_http_hval_t *hv;
    tt_http_etag_t *e;

    hv = __etag_val_create(h);
    if (hv == NULL) {
        return TT_E_NOMEM;
    }
    e = TT_HTTP_HVAL_CAST(hv, tt_http_etag_t);

    e->aster = TT_TRUE;

    tt_http_hdr_add(h, hv);
    return TT_SUCCESS;
}

// ========================================
// If-Match
// ========================================

tt_http_hdr_t *tt_http_hdr_ifmatch_create()
{
    tt_http_hdr_t *h;

    h = tt_http_hdr_create(0,
                           TT_HTTP_HDR_IF_MATCH,
                           &tt_g_http_hdr_cs_itf,
                           &__etag_val_itf);
    if (h == NULL) {
        return NULL;
    }

    return h;
}

tt_result_t tt_http_hdr_ifmatch_add_n(IN tt_http_hdr_t *h,
                                      IN tt_char_t *etag,
                                      IN tt_u32_t len,
                                      IN tt_bool_t weak)
{
    TT_ASSERT(h->name == TT_HTTP_HDR_IF_MATCH);

    return __add_etag(h, etag, len, weak);
}

tt_result_t tt_http_hdr_ifmatch_add_aster(IN tt_http_hdr_t *h)
{
    TT_ASSERT(h->name == TT_HTTP_HDR_IF_MATCH);

    return __add_aster(h);
}

// ========================================
// If-None-Match
// ========================================

tt_http_hdr_t *tt_http_hdr_ifnmatch_create()
{
    tt_http_hdr_t *h;

    h = tt_http_hdr_create(0,
                           TT_HTTP_HDR_IF_N_MATCH,
                           &tt_g_http_hdr_cs_itf,
                           &__etag_val_itf);
    if (h == NULL) {
        return NULL;
    }

    return h;
}

tt_result_t tt_http_hdr_ifnmatch_add_n(IN tt_http_hdr_t *h,
                                       IN tt_char_t *etag,
                                       IN tt_u32_t len,
                                       IN tt_bool_t weak)
{
    TT_ASSERT(h->name == TT_HTTP_HDR_IF_N_MATCH);

    return __add_etag(h, etag, len, weak);
}

tt_result_t tt_http_hdr_ifnmatch_add_aster(IN tt_http_hdr_t *h)
{
    TT_ASSERT(h->name == TT_HTTP_HDR_IF_N_MATCH);

    return __add_aster(h);
}

// ========================================
// etag value interface
// ========================================

tt_http_hval_t *__etag_val_create(IN tt_http_hdr_t *h)
{
    tt_http_hval_t *hv;
    tt_http_etag_t *e;

    hv = tt_http_hval_create(sizeof(tt_http_etag_t));
    if (hv == NULL) {
        return NULL;
    }

    e = TT_HTTP_HVAL_CAST(hv, tt_http_etag_t);

    __etag_init(e);

    return hv;
}

void __etag_val_destroy(IN tt_http_hval_t *hv)
{
    tt_http_etag_t *e = TT_HTTP_HVAL_CAST(hv, tt_http_etag_t);

    __etag_destroy(e);

    tt_http_hval_destroy(hv);
}

tt_result_t __etag_val_parse(IN tt_http_hval_t *hv,
                             IN const tt_char_t *val,
                             IN tt_u32_t len)
{
    tt_http_etag_t *e = TT_HTTP_HVAL_CAST(hv, tt_http_etag_t);
    tt_u8_t *p = (tt_u8_t *)val;

    /*
     ETag       = entity-tag

     entity-tag = [ weak ] opaque-tag
     weak       = %x57.2F ; "W/", case-sensitive
     opaque-tag = DQUOTE *etagc DQUOTE
     etagc      = %x21 / %x23-7E / obs-text
     */

    // to make it usable when parsing if-match
    if ((len == 1) && (p[0] == '*')) {
        e->aster = TT_TRUE;
        return TT_SUCCESS;
    }

    if (len < 2) {
        return TT_FAIL;
    }

    if ((p[0] == 'W') && (p[1] == '/')) {
        e->weak = TT_TRUE;
        p += 2;
        len -= 2;
    }

    // should has double quotes
    if ((len < 2) || (p[0] != '"') || (p[len - 1] != '"')) {
        return TT_FAIL;
    }
    p += 1;
    len -= 2;
    if (len == 0) {
        // ignore empty
        return TT_FAIL;
    }

    // the value comes from the raw header, so just reference the val
    tt_blobex_set(&e->etag, p, len, TT_FALSE);

    return TT_SUCCESS;
}

tt_u32_t __etag_val_render_len(IN tt_http_hval_t *hv)
{
    tt_http_etag_t *e = TT_HTTP_HVAL_CAST(hv, tt_http_etag_t);
    tt_u32_t n = 0;

    if (e->aster) {
        return 1;
    }

    if (e->weak) {
        n += 2; // "W/"
    }

    n += tt_blobex_len(&e->etag);
    n += 2; // double quotes

    return n;
}

tt_u32_t __etag_val_render(IN tt_http_hval_t *hv, IN tt_char_t *dst)
{
    tt_http_etag_t *e = TT_HTTP_HVAL_CAST(hv, tt_http_etag_t);
    tt_char_t *p = dst;
    tt_u32_t n;

    if (e->aster) {
        *dst = '*';
        return 1;
    }

    if (e->weak) {
        // "W/"
        *p++ = 'W';
        *p++ = '/';
    }

    *p++ = '"';
    n = tt_blobex_len(&e->etag);
    tt_memcpy(p, tt_blobex_addr(&e->etag), n);
    p += n;
    *p++ = '"';

    return p - dst;
}
