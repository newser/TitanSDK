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

#include <network/http/header/tt_http_hdr_auth.h>

#include <network/http/tt_http_parser.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef struct
{
    tt_http_auth_t auth;
    tt_bool_t first : 1;
} __auth_int_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_result_t __auth_parse(IN tt_http_hdr_t *h,
                                IN const tt_char_t *val,
                                IN tt_u32_t len);

static tt_u32_t __auth_render_len(IN tt_http_hdr_t *h);

static tt_u32_t __auth_render(IN tt_http_hdr_t *h, IN tt_char_t *dst);

static tt_http_hdr_itf_t __auth_itf = {
    NULL, __auth_parse, __auth_render_len, __auth_render,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static void __auth_set(IN tt_http_auth_t *dst, IN tt_http_auth_t *src);

static void __auth_destroy(IN tt_http_auth_t *ha);

static tt_result_t __parse_param(IN __auth_int_t *ai,
                                 IN const tt_char_t *name,
                                 IN tt_u32_t name_len,
                                 IN const tt_char_t *val,
                                 IN tt_u32_t val_len);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_http_hdr_t *tt_http_hdr_auth_create()
{
    tt_http_hdr_t *h;
    __auth_int_t *ai;

    h = tt_http_hdr_create_cs(sizeof(tt_http_auth_t),
                              TT_HTTP_HDR_AUTH,
                              &__auth_itf);
    if (h == NULL) {
        return NULL;
    }

    ai = TT_HTTP_HDR_CAST(h, __auth_int_t);

    tt_http_auth_init(&ai->auth);
    // todo: set render/parse controller
    // todo: destroy

    return h;
}

void tt_http_auth_init(IN tt_http_auth_t *ha)
{
    tt_blobex_init(&ha->realm, NULL, 0);
    tt_blobex_init(&ha->domain, NULL, 0);
    tt_blobex_init(&ha->nonce, NULL, 0);
    tt_blobex_init(&ha->opaque, NULL, 0);
    tt_blobex_init(&ha->response, NULL, 0);
    tt_blobex_init(&ha->username, NULL, 0);
    tt_blobex_init(&ha->uri, NULL, 0);
    tt_blobex_init(&ha->cnonce, NULL, 0);
    tt_blobex_init(&ha->nc, NULL, 0);
    ha->stale = TT_HTTP_STALE_NUM;
    ha->alg = TT_HTTP_AUTH_ALG_NUM;
    ha->qop = TT_HTTP_QOP_NUM;
}

tt_http_auth_t *tt_http_hdr_auth_get(IN tt_http_hdr_t *h)
{
    __auth_int_t *ai = TT_HTTP_HDR_CAST(h, __auth_int_t);

    TT_ASSERT(h->name == TT_HTTP_HDR_AUTH);

    return &ai->auth;
}

void tt_http_hdr_auth_set(IN tt_http_hdr_t *h, IN TO tt_http_auth_t *auth)
{
    __auth_int_t *ai = TT_HTTP_HDR_CAST(h, __auth_int_t);
    tt_http_auth_t *ha = &ai->auth;

    TT_ASSERT(h->name == TT_HTTP_HDR_AUTH);

    __auth_set(ha, auth);
}

void __auth_set(IN tt_http_auth_t *dst, IN tt_http_auth_t *src)
{
    __auth_destroy(dst);
    // note copying src will take over src's data, thus no need to destroy src
    tt_memcpy(dst, src, sizeof(tt_http_auth_t));
}

void __auth_destroy(IN tt_http_auth_t *ha)
{
    tt_blobex_destroy(&ha->realm);
    tt_blobex_destroy(&ha->domain);
    tt_blobex_destroy(&ha->nonce);
    tt_blobex_destroy(&ha->opaque);
    tt_blobex_destroy(&ha->response);
    tt_blobex_destroy(&ha->username);
    tt_blobex_destroy(&ha->uri);
    tt_blobex_destroy(&ha->cnonce);
    tt_blobex_destroy(&ha->nc);
}

tt_result_t __parse_param(IN __auth_int_t *ai,
                          IN const tt_char_t *name,
                          IN tt_u32_t name_len,
                          IN const tt_char_t *val,
                          IN tt_u32_t val_len)
{
    return TT_SUCCESS;
}

tt_result_t __auth_parse(IN tt_http_hdr_t *h,
                         IN const tt_char_t *val,
                         IN tt_u32_t len)
{
    __auth_int_t *ai = TT_HTTP_HDR_CAST(h, __auth_int_t);
    tt_http_auth_t *ha = &ai->auth;
    const tt_char_t *n, *v, *p;
    tt_u32_t nlen, vlen, i;

    /*
     challenge   = auth-scheme [ 1*SP ( token68 / #auth-param ) ]
     auth-param     = token BWS "=" BWS ( token / quoted-string )
     */

    // parse scheme
    if (ai->first) {
        n = val;
        p = tt_memchr(n, ' ', len);
        if (p != NULL) {
            nlen = p - val;
        } else {
            nlen = len;
        }

        val = n + nlen;
        len -= nlen;

        tt_trim_lr((tt_u8_t **)&n, &nlen, ' ');
        if (tt_strnicmp(n, "basic", nlen) == 0) {
            ha->scheme = TT_HTTP_AUTH_BASIC;
        } else if (tt_strnicmp(n, "digest", nlen) == 0) {
            ha->scheme = TT_HTTP_AUTH_DIGEST;
        } else {
            ha->scheme = TT_HTTP_AUTH_SCHEME_NUM;
        }

        ai->first = TT_FALSE;
    }

    // parse param name
    n = val;
    p = tt_memchr(val, '=', len);
    if (p != NULL) {
        nlen = p - val;
        val = p + 1;
        len -= (nlen + 1);
    } else {
        nlen = len;
        val += len;
        len = 0;
    }
    tt_trim_lr((tt_u8_t **)&n, &nlen, ' ');

    // parse param value, try removing quotes
    tt_trim_lr((tt_u8_t **)&val, &len, ' ');
    if ((len > 1) && (val[0] == '"') && (val[len - 1] == '"')) {
        ++val;
        len -= 2;
    }

    return TT_SUCCESS;
}

tt_u32_t __auth_render_len(IN tt_http_hdr_t *h)
{
    tt_http_auth_t *ha = TT_HTTP_HDR_CAST(h, tt_http_auth_t);
    tt_u32_t n, i;
    tt_bool_t empty = TT_TRUE;

    // always render "Authorization: "
    n = tt_g_http_hname_len[TT_HTTP_HDR_AUTH] + 2;

    if (empty) {
        // ending "\r\n"
        n += 2;
    }

    return n;
}

tt_u32_t __auth_render(IN tt_http_hdr_t *h, IN tt_char_t *dst)
{
    tt_http_auth_t *ha = TT_HTTP_HDR_CAST(h, tt_http_auth_t);
    tt_char_t *p;
    tt_u32_t n, i;
    tt_bool_t empty = TT_TRUE;

    // "Authorization: "
    p = dst;
    n = tt_g_http_hname_len[TT_HTTP_HDR_AUTH];
    tt_memcpy(p, tt_g_http_hname[TT_HTTP_HDR_AUTH], n);
    p += n;
    *p++ = ':';
    *p++ = ' ';

    // "\r\n"
    if (!empty) {
        p -= 2;
    }
    *p++ = '\r';
    *p++ = '\n';

    return p - dst;
}
