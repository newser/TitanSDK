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

#include <os/tt_thread.h>
#include <time/tt_time_reference.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __SCHEME_BASIC "Basic"
#define __SCHEME_DIGEST "Digest"

#define __P_REALM "realm"
#define __P_DOMAIN "domain"
#define __P_NONCE "nonce"
#define __P_OPAQUE "opaque"
#define __P_STALE "stale"
#define __P_ALG "algorithm"
#define __P_QOP "qop"
#define __P_RESP "response"
#define __P_USERNAME "username"
#define __P_URI "uri"
#define __P_CNONCE "cnonce"
#define __P_NC "nc"

#define __STALE_TRUE "true"
#define __STALE_FALSE "false"

#define __ALG_MD5 "MD5"
#define __ALG_MD5_SESS "MD5-sess"

#define __QOP_AUTH "auth"
#define __QOP_AUTH_INT "auth-int"

#define __VIEQ(val, len, cstr)                                                 \
    ((len == (sizeof(cstr) - 1)) && (tt_strnicmp(val, cstr, len) == 0))

#define __MD5_DLEN 16

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef struct
{
    tt_http_auth_t auth;
    tt_bool_t first : 1;

    tt_bool_t has_realm : 1;
    tt_bool_t has_domain : 1;
    tt_bool_t has_nonce : 1;
    tt_bool_t has_opaque : 1;
    tt_bool_t has_response : 1;
    tt_bool_t has_username : 1;
    tt_bool_t has_uri : 1;
    tt_bool_t has_cnonce : 1;
    tt_bool_t has_nc : 1;
    tt_bool_t has_qop : 1;
    tt_bool_t qop_list : 1;
    tt_bool_t has_stale : 1;
    tt_bool_t has_alg : 1;
} __auth_int_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static void __auth_destroy(IN tt_http_hdr_t *h);

static tt_result_t __auth_parse(IN tt_http_hdr_t *h,
                                IN const tt_char_t *val,
                                IN tt_u32_t len);

static tt_u32_t __auth_render_len(IN tt_http_hdr_t *h);

static tt_u32_t __auth_render(IN tt_http_hdr_t *h, IN tt_char_t *dst);

static tt_http_hdr_itf_t __auth_itf = {
    __auth_destroy, __auth_parse, __auth_render_len, __auth_render,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_bool_t __check_auth(IN tt_http_auth_t *ha);

static void __set_auth(IN tt_http_auth_t *dst, IN tt_http_auth_t *src);

static tt_result_t __parse_param(IN __auth_int_t *ai,
                                 IN const tt_char_t *name,
                                 IN tt_u32_t name_len,
                                 IN const tt_char_t *val,
                                 IN tt_u32_t val_len);

static tt_result_t __parse_qop(IN __auth_int_t *ai,
                               IN const tt_char_t *val,
                               IN tt_u32_t val_len);

static tt_result_t __calc_md5(IN tt_md_t *md,
                              IN tt_http_auth_t *ha,
                              IN void *password,
                              IN tt_u32_t password_len,
                              IN tt_u32_t qop,
                              IN void *method,
                              IN tt_u32_t method_len,
                              IN OPT void *body,
                              IN tt_u32_t body_len,
                              OUT tt_char_t *response);

static tt_result_t __calc_md5_sess(IN tt_md_t *md,
                                   IN tt_http_auth_t *ha,
                                   IN void *password,
                                   IN tt_u32_t password_len,
                                   IN tt_u32_t qop,
                                   IN void *method,
                                   IN tt_u32_t method_len,
                                   IN OPT void *body,
                                   IN tt_u32_t body_len,
                                   OUT tt_char_t *response);

static tt_result_t __calc_md5_resp(IN tt_char_t *s_a1,
                                   IN tt_md_t *md,
                                   IN tt_http_auth_t *ha,
                                   IN tt_u32_t qop,
                                   IN void *method,
                                   IN tt_u32_t method_len,
                                   IN OPT void *body,
                                   IN tt_u32_t body_len,
                                   OUT tt_char_t *response);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_http_hdr_t *tt_http_hdr_www_auth_create()
{
    tt_http_hdr_t *h;
    __auth_int_t *ai;

    h = tt_http_hdr_create_csq(sizeof(__auth_int_t),
                               TT_HTTP_HDR_WWW_AUTH,
                               &__auth_itf);
    if (h == NULL) {
        return NULL;
    }

    ai = TT_HTTP_HDR_CAST(h, __auth_int_t);

    tt_http_auth_init(&ai->auth);
    ai->first = TT_TRUE;

    ai->has_realm = TT_TRUE;
    ai->has_domain = TT_TRUE;
    ai->has_nonce = TT_TRUE;
    ai->has_opaque = TT_TRUE;
    ai->has_response = TT_FALSE;
    ai->has_username = TT_FALSE;
    ai->has_uri = TT_FALSE;
    ai->has_cnonce = TT_FALSE;
    ai->has_nc = TT_FALSE;
    ai->has_qop = TT_TRUE;
    ai->qop_list = TT_TRUE;
    ai->has_stale = TT_TRUE;
    ai->has_alg = TT_TRUE;

    return h;
}

tt_http_hdr_t *tt_http_hdr_auth_create()
{
    tt_http_hdr_t *h;
    __auth_int_t *ai;

    h = tt_http_hdr_create_csq(sizeof(__auth_int_t),
                               TT_HTTP_HDR_AUTH,
                               &__auth_itf);
    if (h == NULL) {
        return NULL;
    }

    ai = TT_HTTP_HDR_CAST(h, __auth_int_t);

    tt_http_auth_init(&ai->auth);
    ai->first = TT_TRUE;

    ai->has_realm = TT_TRUE;
    ai->has_domain = TT_FALSE;
    ai->has_nonce = TT_TRUE;
    ai->has_opaque = TT_TRUE;
    ai->has_response = TT_TRUE;
    ai->has_username = TT_TRUE;
    ai->has_uri = TT_TRUE;
    ai->has_cnonce = TT_TRUE;
    ai->has_nc = TT_TRUE;
    ai->has_qop = TT_TRUE;
    ai->qop_list = TT_FALSE;
    ai->has_stale = TT_FALSE;
    ai->has_alg = TT_TRUE;

    return h;
}

tt_http_hdr_t *tt_http_hdr_proxy_authenticate_create()
{
    tt_http_hdr_t *h;
    __auth_int_t *ai;

    h = tt_http_hdr_create_csq(sizeof(__auth_int_t),
                               TT_HTTP_HDR_PROXY_AUTHENTICATE,
                               &__auth_itf);
    if (h == NULL) {
        return NULL;
    }

    ai = TT_HTTP_HDR_CAST(h, __auth_int_t);

    tt_http_auth_init(&ai->auth);
    ai->first = TT_TRUE;

    ai->has_realm = TT_TRUE;
    ai->has_domain = TT_TRUE;
    ai->has_nonce = TT_TRUE;
    ai->has_opaque = TT_TRUE;
    ai->has_response = TT_FALSE;
    ai->has_username = TT_FALSE;
    ai->has_uri = TT_FALSE;
    ai->has_cnonce = TT_FALSE;
    ai->has_nc = TT_FALSE;
    ai->has_qop = TT_TRUE;
    ai->qop_list = TT_TRUE;
    ai->has_stale = TT_TRUE;
    ai->has_alg = TT_TRUE;

    return h;
}

tt_http_hdr_t *tt_http_hdr_proxy_authorization_create()
{
    tt_http_hdr_t *h;
    __auth_int_t *ai;

    h = tt_http_hdr_create_csq(sizeof(__auth_int_t),
                               TT_HTTP_HDR_PROXY_AUTHORIZATION,
                               &__auth_itf);
    if (h == NULL) {
        return NULL;
    }

    ai = TT_HTTP_HDR_CAST(h, __auth_int_t);

    tt_http_auth_init(&ai->auth);
    ai->first = TT_TRUE;

    ai->has_realm = TT_TRUE;
    ai->has_domain = TT_FALSE;
    ai->has_nonce = TT_TRUE;
    ai->has_opaque = TT_TRUE;
    ai->has_response = TT_TRUE;
    ai->has_username = TT_TRUE;
    ai->has_uri = TT_TRUE;
    ai->has_cnonce = TT_TRUE;
    ai->has_nc = TT_TRUE;
    ai->has_qop = TT_TRUE;
    ai->qop_list = TT_FALSE;
    ai->has_stale = TT_FALSE;
    ai->has_alg = TT_TRUE;

    return h;
}

tt_http_auth_t *tt_http_hdr_auth_get(IN tt_http_hdr_t *h)
{
    __auth_int_t *ai = TT_HTTP_HDR_CAST(h, __auth_int_t);

    TT_ASSERT((h->name >= TT_HTTP_HDR_AUTH) &&
              (h->name <= TT_HTTP_HDR_PROXY_AUTHENTICATE));

    return &ai->auth;
}

tt_result_t tt_http_hdr_auth_set(IN tt_http_hdr_t *h,
                                 IN tt_http_auth_t *auth,
                                 IN tt_bool_t shallow_copy)
{
    __auth_int_t *ai = TT_HTTP_HDR_CAST(h, __auth_int_t);

    TT_ASSERT((h->name >= TT_HTTP_HDR_AUTH) &&
              (h->name <= TT_HTTP_HDR_PROXY_AUTHENTICATE));

    if (shallow_copy) {
        tt_http_auth_shallow_copy(&ai->auth, auth);
        return TT_SUCCESS;
    } else {
        return tt_http_auth_smart_copy(&ai->auth, auth);
    }
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
    tt_blobex_init(&ha->raw_qop, NULL, 0);
    ha->qop_mask = 0;
    ha->scheme = TT_HTTP_AUTH_SCHEME_NUM;
    ha->stale = TT_HTTP_STALE_NUM;
    ha->alg = TT_HTTP_AUTH_ALG_NUM;
}

void tt_http_auth_destroy(IN tt_http_auth_t *ha)
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
    tt_blobex_destroy(&ha->raw_qop);
}

tt_bool_t tt_http_auth_valid(IN tt_http_auth_t *ha)
{
    return __check_auth(ha);
}

void tt_http_auth_shallow_copy(IN tt_http_auth_t *dst, IN tt_http_auth_t *src)
{
#define __SHALLOW_CP(member)                                                   \
    tt_blobex_set(&dst->member,                                                \
                  tt_blobex_addr(&src->member),                                \
                  tt_blobex_len(&src->member),                                 \
                  TT_FALSE)
    __SHALLOW_CP(realm);
    __SHALLOW_CP(domain);
    __SHALLOW_CP(nonce);
    __SHALLOW_CP(opaque);
    __SHALLOW_CP(response);
    __SHALLOW_CP(username);
    __SHALLOW_CP(uri);
    __SHALLOW_CP(cnonce);
    __SHALLOW_CP(nc);
    __SHALLOW_CP(raw_qop);
    dst->qop_mask = src->qop_mask;
    dst->scheme = src->scheme;
    dst->stale = src->stale;
    dst->alg = src->alg;
#undef __SHALLOW_CP
}

tt_result_t tt_http_auth_smart_copy(IN tt_http_auth_t *dst,
                                    IN tt_http_auth_t *src)
{
#define __SMART_CP(member)                                                     \
    TT_DO(tt_blobex_smart_copy(&dst->member, &src->member))
    __SMART_CP(realm);
    __SMART_CP(domain);
    __SMART_CP(nonce);
    __SMART_CP(opaque);
    __SMART_CP(response);
    __SMART_CP(username);
    __SMART_CP(uri);
    __SMART_CP(cnonce);
    __SMART_CP(nc);
    __SMART_CP(raw_qop);
    dst->qop_mask = src->qop_mask;
    dst->scheme = src->scheme;
    dst->stale = src->stale;
    dst->alg = src->alg;
    return TT_SUCCESS;
#undef __SMART_CP
}

void tt_http_auth_ctx_init(IN tt_http_auth_ctx_t *ctx)
{
    ctx->type = TT_MD_TYPE_NUM;
    ctx->nonce_len = 0;
}

void tt_http_auth_ctx_destroy(IN tt_http_auth_ctx_t *ctx)
{
    if (ctx->type != TT_MD_TYPE_NUM) {
        tt_md_destroy(&ctx->md);
    }
}

void tt_http_auth_ctx_new_nonce(IN tt_http_auth_ctx_t *ctx)
{
    tt_snprintf(ctx->nonce,
                sizeof(ctx->nonce),
                "%x%x",
                tt_rand_u32(),
                (tt_u32_t)tt_time_ref());
    ctx->nonce_len = tt_strlen(ctx->nonce);
}

tt_u32_t tt_http_auth_ctx_digest_len(IN tt_http_auth_ctx_t *ctx)
{
    TT_ASSERT(TT_MD_TYPE_VALID(ctx->type));
    return tt_md_size(&ctx->md);
}

tt_result_t tt_http_auth_ctx_calc(IN tt_http_auth_ctx_t *ctx,
                                  IN tt_http_auth_t *ha,
                                  IN void *password,
                                  IN tt_u32_t password_len,
                                  IN tt_u32_t qop,
                                  IN void *method,
                                  IN tt_u32_t method_len,
                                  IN OPT void *body,
                                  IN tt_u32_t body_len,
                                  OUT tt_char_t *response)
{
    static tt_md_type_t map[TT_HTTP_AUTH_ALG_NUM] = {TT_MD5, TT_MD5};

    tt_http_auth_alg_t alg;
    tt_md_type_t md_type;
    tt_md_t *md;

    if (!tt_http_auth_valid(ha)) {
        return TT_E_BADARG;
    }

    alg = ha->alg;
    md_type = map[alg];
    md = &ctx->md;
    if (ctx->type != md_type) {
        if (ctx->type != TT_MD_TYPE_NUM) {
            tt_md_destroy(md);
            ctx->type = TT_MD_TYPE_NUM;
        }
        TT_DO(tt_md_create(md, md_type));
        ctx->type = md_type;
    } else {
        TT_DO(tt_md_reset(md));
    }

    if (alg == TT_HTTP_AUTH_MD5) {
        return __calc_md5(md,
                          ha,
                          password,
                          password_len,
                          qop,
                          method,
                          method_len,
                          body,
                          body_len,
                          response);
    } else if (alg == TT_HTTP_AUTH_MD5_SESS) {
        return __calc_md5_sess(md,
                               ha,
                               password,
                               password_len,
                               qop,
                               method,
                               method_len,
                               body,
                               body_len,
                               response);
    } else {
        TT_ASSERT(0);
        return TT_E_BADARG;
    }
}

tt_bool_t __check_auth(IN tt_http_auth_t *ha)
{
    if (!TT_HTTP_AUTH_ALG_VALID(ha->alg)) {
        // TT_ERROR("invalid auth alg: %d", ha->alg);
        return TT_FALSE;
    }

    return TT_TRUE;
}

void __set_auth(IN tt_http_auth_t *dst, IN tt_http_auth_t *src)
{
    tt_http_auth_destroy(dst);
    // note copying src will take over src's data, thus no need to destroy src
    tt_memcpy(dst, src, sizeof(tt_http_auth_t));
}

tt_result_t __parse_param(IN __auth_int_t *ai,
                          IN const tt_char_t *name,
                          IN tt_u32_t name_len,
                          IN const tt_char_t *val,
                          IN tt_u32_t val_len)
{
    tt_http_auth_t *ha = &ai->auth;

    // not owner during parsing
    if (__VIEQ(name, name_len, __P_REALM)) {
        if (ai->has_realm) {
            tt_blobex_set(&ha->realm, (tt_u8_t *)val, val_len, TT_FALSE);
        }
    } else if (__VIEQ(name, name_len, __P_DOMAIN)) {
        if (ai->has_domain) {
            tt_blobex_set(&ha->domain, (tt_u8_t *)val, val_len, TT_FALSE);
        }
    } else if (__VIEQ(name, name_len, __P_NONCE)) {
        if (ai->has_nonce) {
            tt_blobex_set(&ha->nonce, (tt_u8_t *)val, val_len, TT_FALSE);
        }
    } else if (__VIEQ(name, name_len, __P_OPAQUE)) {
        if (ai->has_opaque) {
            tt_blobex_set(&ha->opaque, (tt_u8_t *)val, val_len, TT_FALSE);
        }
    } else if (__VIEQ(name, name_len, __P_STALE)) {
        if (ai->has_stale) {
            if (__VIEQ(val, val_len, __STALE_TRUE)) {
                ha->stale = TT_HTTP_STALE_TRUE;
            } else if (__VIEQ(val, val_len, __STALE_FALSE)) {
                ha->stale = TT_HTTP_STALE_FALSE;
            } else {
                ha->stale = TT_HTTP_STALE_NUM;
            }
        }
    } else if (__VIEQ(name, name_len, __P_ALG)) {
        if (ai->has_alg) {
            if (__VIEQ(val, val_len, __ALG_MD5)) {
                ha->alg = TT_HTTP_AUTH_MD5;
            } else if (__VIEQ(val, val_len, __ALG_MD5_SESS)) {
                ha->alg = TT_HTTP_AUTH_MD5_SESS;
            } else {
                ha->alg = TT_HTTP_AUTH_ALG_NUM;
            }
        }
    } else if (__VIEQ(name, name_len, __P_QOP)) {
        if (ai->has_qop) {
            tt_blobex_set(&ha->raw_qop, (tt_u8_t *)val, val_len, TT_FALSE);
            return __parse_qop(ai, val, val_len);
        }
    } else if (__VIEQ(name, name_len, __P_RESP)) {
        if (ai->has_response) {
            tt_blobex_set(&ha->response, (tt_u8_t *)val, val_len, TT_FALSE);
        }
    } else if (__VIEQ(name, name_len, __P_USERNAME)) {
        if (ai->has_username) {
            tt_blobex_set(&ha->username, (tt_u8_t *)val, val_len, TT_FALSE);
        }
    } else if (__VIEQ(name, name_len, __P_URI)) {
        if (ai->has_uri) {
            tt_blobex_set(&ha->uri, (tt_u8_t *)val, val_len, TT_FALSE);
        }
    } else if (__VIEQ(name, name_len, __P_CNONCE)) {
        if (ai->has_cnonce) {
            tt_blobex_set(&ha->cnonce, (tt_u8_t *)val, val_len, TT_FALSE);
        }
    } else if (__VIEQ(name, name_len, __P_NC)) {
        if (ai->has_nc) {
            tt_blobex_set(&ha->nc, (tt_u8_t *)val, val_len, TT_FALSE);
        }
    }

    return TT_SUCCESS;
}

tt_result_t __parse_qop(IN __auth_int_t *ai,
                        IN const tt_char_t *val,
                        IN tt_u32_t val_len)
{
    tt_http_auth_t *ha = &ai->auth;
    const tt_char_t *end, *p;
    tt_u32_t n;

    ha->qop_mask = 0;

    end = val + val_len;
    while ((val < end) && ((p = tt_memchr(val, ',', end - val)) != NULL)) {
        n = p - val;
        tt_trim_lr((tt_u8_t **)&val, &n, ' ');

        if (__VIEQ(val, n, __QOP_AUTH)) {
            ha->qop_mask |= TT_HTTP_QOP_AUTH;
        } else if (__VIEQ(val, n, __QOP_AUTH_INT)) {
            ha->qop_mask |= TT_HTTP_QOP_AUTH_INT;
        }

        val = p + 1;
    }
    if (val < end) {
        n = end - val;
        tt_trim_lr((tt_u8_t **)&val, &n, ' ');

        if (__VIEQ(val, n, __QOP_AUTH)) {
            ha->qop_mask |= TT_HTTP_QOP_AUTH;
        } else if (__VIEQ(val, n, __QOP_AUTH_INT)) {
            ha->qop_mask |= TT_HTTP_QOP_AUTH_INT;
        }
    }

    return TT_SUCCESS;
}

void __auth_destroy(IN tt_http_hdr_t *h)
{
    __auth_int_t *ai = TT_HTTP_HDR_CAST(h, __auth_int_t);

    tt_http_auth_destroy(&ai->auth);
}

tt_result_t __auth_parse(IN tt_http_hdr_t *h,
                         IN const tt_char_t *val,
                         IN tt_u32_t len)
{
    __auth_int_t *ai = TT_HTTP_HDR_CAST(h, __auth_int_t);
    tt_http_auth_t *ha = &ai->auth;
    const tt_char_t *n, *p;
    tt_u32_t nlen, i;

    /*
     challenge = auth-scheme [ 1*SP ( token68 / #auth-param ) ]
     auth-param = token BWS "=" BWS ( token / quoted-string )
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
        if (__VIEQ(n, nlen, __SCHEME_BASIC)) {
            ha->scheme = TT_HTTP_AUTH_BASIC;
        } else if (__VIEQ(n, nlen, __SCHEME_DIGEST)) {
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
        TT_ASSERT(len >= (nlen + 1));
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

    return __parse_param(ai, n, nlen, val, len);
}

tt_u32_t __auth_render_len(IN tt_http_hdr_t *h)
{
    __auth_int_t *ai = TT_HTTP_HDR_CAST(h, __auth_int_t);
    tt_http_auth_t *ha = &ai->auth;
    tt_u32_t n, len;
    tt_bool_t empty = TT_TRUE;
    tt_u8_t qop_mask;

// length of [="", ] is 5
#define __ARL(has, member, cstr)                                               \
    do {                                                                       \
        if (ai->has) {                                                         \
            len = tt_blobex_len(&ha->member);                                  \
            if (len != 0) {                                                    \
                n += sizeof(cstr) + 4 + len;                                   \
                empty = TT_FALSE;                                              \
            }                                                                  \
        }                                                                      \
    } while (0)

// length of [=, ] is 3
#define __ARL_NQ(has, member, cstr)                                            \
    do {                                                                       \
        if (ai->has) {                                                         \
            len = tt_blobex_len(&ha->member);                                  \
            if (len != 0) {                                                    \
                n += sizeof(cstr) + 2 + len;                                   \
                empty = TT_FALSE;                                              \
            }                                                                  \
        }                                                                      \
    } while (0)

    /*
     For historical reasons, a sender MUST only generate the quoted string
     syntax values for the following parameters: realm, domain, nonce,
     opaque, and qop.

     For historical reasons, a sender MUST NOT generate the quoted string
     syntax values for the following parameters: stale and algorithm.

     For historical reasons, a sender MUST only generate the quoted string
     syntax for the following parameters: username, realm, nonce, uri,
     response, cnonce, and opaque.

     For historical reasons, a sender MUST NOT generate the quoted string
     syntax for the following parameters: algorithm, qop, and nc.
     */

    // always render "Authorization: "
    n = tt_g_http_hname_len[h->name] + 2;

    if (ha->scheme == TT_HTTP_AUTH_BASIC) {
        // "basic "
        n += sizeof(__SCHEME_BASIC);
    } else if (ha->scheme == TT_HTTP_AUTH_DIGEST) {
        n += sizeof(__SCHEME_DIGEST);
    }

    __ARL(has_realm, realm, __P_REALM);
    __ARL(has_domain, domain, __P_DOMAIN);
    __ARL(has_nonce, nonce, __P_NONCE);
    __ARL(has_opaque, opaque, __P_OPAQUE);
    __ARL(has_response, response, __P_RESP);
    __ARL(has_username, username, __P_USERNAME);
    __ARL(has_uri, uri, __P_URI);
    __ARL(has_cnonce, cnonce, __P_CNONCE);

    __ARL_NQ(has_nc, nc, __P_NC);

    qop_mask = ha->qop_mask;
    if (qop_mask != 0) {
        if (ai->qop_list) {
            // qop[="", ]
            len = sizeof(__P_QOP) + 4;
            // "auth,"
            len += TT_COND(qop_mask & TT_HTTP_QOP_AUTH, sizeof(__QOP_AUTH), 0);
            // "auth-int,"
            len += TT_COND(qop_mask & TT_HTTP_QOP_AUTH_INT,
                           sizeof(__QOP_AUTH_INT),
                           0);
            --len;
        } else {
            // qop[=, ]
            len = sizeof(__P_QOP) + 2;
            if (qop_mask & TT_HTTP_QOP_AUTH) {
                len += sizeof(__QOP_AUTH) - 1;
            } else if (qop_mask & TT_HTTP_QOP_AUTH_INT) {
                len += sizeof(__QOP_AUTH_INT) - 1;
            }
        }
        n += len;
        empty = TT_FALSE;
    }

    if (ai->has_stale) {
        if (ha->stale == TT_HTTP_STALE_TRUE) {
            // "stale=true, "
            n += sizeof(__P_STALE "=" __STALE_TRUE) + 1;
            empty = TT_FALSE;
        } else if (ha->stale == TT_HTTP_STALE_FALSE) {
            n += sizeof(__P_STALE "=" __STALE_FALSE) + 1;
            empty = TT_FALSE;
        }
    }

    if (ai->has_alg) {
        if (ha->alg == TT_HTTP_AUTH_MD5) {
            // algorithm=md5,
            n += sizeof(__P_ALG "=" __ALG_MD5) + 1;
            empty = TT_FALSE;
        } else if (ha->alg == TT_HTTP_AUTH_MD5_SESS) {
            n += sizeof(__P_ALG "=" __ALG_MD5_SESS) + 1;
            empty = TT_FALSE;
        }
    }

    if (empty) {
        // ending "\r\n"
        n += 2;
    }

    return n;
}

tt_u32_t __auth_render(IN tt_http_hdr_t *h, IN tt_char_t *dst)
{
    __auth_int_t *ai = TT_HTTP_HDR_CAST(h, __auth_int_t);
    tt_http_auth_t *ha = &ai->auth;
    tt_char_t *p;
    tt_u32_t n;
    tt_bool_t empty = TT_TRUE;
    tt_u8_t qop_mask;

    // "Authorization: "
    p = dst;
    n = tt_g_http_hname_len[h->name];
    tt_memcpy(p, tt_g_http_hname[h->name], n);
    p += n;
    *p++ = ':';
    *p++ = ' ';

#define __CPCSTR(cstr)                                                         \
    do {                                                                       \
        tt_memcpy(p, cstr, sizeof(cstr) - 1);                                  \
        p += sizeof(cstr) - 1;                                                 \
    } while (0)

#define __AR(has, member, cstr)                                                \
    do {                                                                       \
        if (ai->has) {                                                         \
            n = tt_blobex_len(&ha->member);                                    \
            if (n != 0) {                                                      \
                __CPCSTR(cstr "=\"");                                          \
                tt_memcpy(p, tt_blobex_addr(&ha->member), n);                  \
                p += n;                                                        \
                *p++ = '"';                                                    \
                *p++ = ',';                                                    \
                *p++ = ' ';                                                    \
                empty = TT_FALSE;                                              \
            }                                                                  \
        }                                                                      \
    } while (0)

#define __AR_NQ(has, member, cstr)                                             \
    do {                                                                       \
        if (ai->has) {                                                         \
            n = tt_blobex_len(&ha->member);                                    \
            if (n != 0) {                                                      \
                __CPCSTR(cstr "=");                                            \
                tt_memcpy(p, tt_blobex_addr(&ha->member), n);                  \
                p += n;                                                        \
                *p++ = ',';                                                    \
                *p++ = ' ';                                                    \
                empty = TT_FALSE;                                              \
            }                                                                  \
        }                                                                      \
    } while (0)

    if (ha->scheme == TT_HTTP_AUTH_BASIC) {
        __CPCSTR(__SCHEME_BASIC " ");
    } else if (ha->scheme == TT_HTTP_AUTH_DIGEST) {
        __CPCSTR(__SCHEME_DIGEST " ");
    }

    __AR(has_realm, realm, __P_REALM);
    __AR(has_domain, domain, __P_DOMAIN);
    __AR(has_nonce, nonce, __P_NONCE);
    __AR(has_opaque, opaque, __P_OPAQUE);
    __AR(has_response, response, __P_RESP);
    __AR(has_username, username, __P_USERNAME);
    __AR(has_uri, uri, __P_URI);
    __AR(has_cnonce, cnonce, __P_CNONCE);

    __AR_NQ(has_nc, nc, __P_NC);

    qop_mask = ha->qop_mask;
    if (qop_mask != 0) {
        if (ai->qop_list) {
            __CPCSTR("qop=\"");
            if (qop_mask & TT_HTTP_QOP_AUTH) {
                __CPCSTR(__QOP_AUTH ",");
            }
            if (qop_mask & TT_HTTP_QOP_AUTH_INT) {
                __CPCSTR(__QOP_AUTH_INT ",");
            }
            p -= 1;
            *p++ = '\"';
            *p++ = ',';
            *p++ = ' ';
        } else {
            __CPCSTR("qop=");
            if (qop_mask & TT_HTTP_QOP_AUTH) {
                __CPCSTR(__QOP_AUTH ", ");
            } else if (qop_mask & TT_HTTP_QOP_AUTH_INT) {
                __CPCSTR(__QOP_AUTH_INT ", ");
            }
        }
        empty = TT_FALSE;
    }

    if (ai->has_stale) {
        if (ha->stale == TT_HTTP_STALE_TRUE) {
            __CPCSTR(__P_STALE "=" __STALE_TRUE ", ");
            empty = TT_FALSE;
        } else if (ha->stale == TT_HTTP_STALE_FALSE) {
            __CPCSTR(__P_STALE "=" __STALE_FALSE ", ");
            empty = TT_FALSE;
        }
    }

    if (ai->has_alg) {
        if (ha->alg == TT_HTTP_AUTH_MD5) {
            __CPCSTR(__P_ALG "=" __ALG_MD5 ", ");
            empty = TT_FALSE;
        } else if (ha->alg == TT_HTTP_AUTH_MD5_SESS) {
            __CPCSTR(__P_ALG "=" __ALG_MD5_SESS ", ");
            empty = TT_FALSE;
        }
    }

    // "\r\n"
    if (!empty) {
        p -= 2;
    }
    *p++ = '\r';
    *p++ = '\n';

    return p - dst;
}

tt_result_t __calc_md5(IN tt_md_t *md,
                       IN tt_http_auth_t *ha,
                       IN void *password,
                       IN tt_u32_t password_len,
                       IN tt_u32_t qop,
                       IN void *method,
                       IN tt_u32_t method_len,
                       IN OPT void *body,
                       IN tt_u32_t body_len,
                       OUT tt_char_t *response)
{
    tt_u8_t a1[__MD5_DLEN];
    tt_char_t s_a1[__MD5_DLEN * 2];

    // HA1 = MD5(username:realm:password)
    tt_md_reset(md);
    tt_md_update(md,
                 tt_blobex_addr(&ha->username),
                 tt_blobex_len(&ha->username));
    tt_md_update(md, (tt_u8_t *)":", 1);
    tt_md_update(md, tt_blobex_addr(&ha->realm), tt_blobex_len(&ha->realm));
    tt_md_update(md, (tt_u8_t *)":", 1);
    tt_md_update(md, password, password_len);
    tt_md_final(md, a1);

    tt_hex2str(a1, __MD5_DLEN, s_a1);

    return __calc_md5_resp(s_a1,
                           md,
                           ha,
                           qop,
                           method,
                           method_len,
                           body,
                           body_len,
                           response);
}

tt_result_t __calc_md5_sess(IN tt_md_t *md,
                            IN tt_http_auth_t *ha,
                            IN void *password,
                            IN tt_u32_t password_len,
                            IN tt_u32_t qop,
                            IN void *method,
                            IN tt_u32_t method_len,
                            IN OPT void *body,
                            IN tt_u32_t body_len,
                            OUT tt_char_t *response)
{
    tt_u8_t a1[__MD5_DLEN];
    tt_char_t s_a1[__MD5_DLEN * 2];

    // HA1 = MD5(MD5(username:realm:password):nonce:cnonce)
    tt_md_reset(md);
    tt_md_update(md,
                 tt_blobex_addr(&ha->username),
                 tt_blobex_len(&ha->username));
    tt_md_update(md, (tt_u8_t *)":", 1);
    tt_md_update(md, tt_blobex_addr(&ha->realm), tt_blobex_len(&ha->realm));
    tt_md_update(md, (tt_u8_t *)":", 1);
    tt_md_update(md, password, password_len);
    tt_md_final(md, a1);

    tt_md_reset(md);
    tt_md_update(md, a1, __MD5_DLEN);
    tt_md_update(md, (tt_u8_t *)":", 1);
    tt_md_update(md, tt_blobex_addr(&ha->nonce), tt_blobex_len(&ha->nonce));
    tt_md_update(md, (tt_u8_t *)":", 1);
    tt_md_update(md, tt_blobex_addr(&ha->cnonce), tt_blobex_len(&ha->cnonce));
    tt_md_final(md, a1);

    tt_hex2str(a1, __MD5_DLEN, s_a1);

    return __calc_md5_resp(s_a1,
                           md,
                           ha,
                           qop,
                           method,
                           method_len,
                           body,
                           body_len,
                           response);
}

tt_result_t __calc_md5_resp(IN tt_char_t *s_a1,
                            IN tt_md_t *md,
                            IN tt_http_auth_t *ha,
                            IN tt_u32_t qop,
                            IN void *method,
                            IN tt_u32_t method_len,
                            IN OPT void *body,
                            IN tt_u32_t body_len,
                            OUT tt_char_t *response)
{
    tt_u8_t a2[__MD5_DLEN], resp[__MD5_DLEN];
    tt_char_t s_a2[__MD5_DLEN * 2];

    if (qop == TT_HTTP_QOP_AUTH_INT) {
        // HA2 = MD5(method:digestURI:MD5(entityBody))
        tt_u8_t hbody[__MD5_DLEN];
        tt_md_reset(md);
        tt_md_update(md, body, body_len);
        tt_md_final(md, hbody);

        tt_md_reset(md);
        tt_md_update(md, method, method_len);
        tt_md_update(md, (tt_u8_t *)":", 1);
        tt_md_update(md, tt_blobex_addr(&ha->uri), tt_blobex_len(&ha->uri));
        tt_md_update(md, (tt_u8_t *)":", 1);
        tt_md_update(md, hbody, __MD5_DLEN);
        tt_md_final(md, a2);
    } else {
        // auth or not specified

        // HA2 = MD5(method:digestURI)
        tt_md_reset(md);
        tt_md_update(md, method, method_len);
        tt_md_update(md, (tt_u8_t *)":", 1);
        tt_md_update(md, tt_blobex_addr(&ha->uri), tt_blobex_len(&ha->uri));
        tt_md_final(md, a2);
    }

    tt_hex2str(a2, __MD5_DLEN, s_a2);

    if (qop != 0) {
        // auth or auth-int

        // response = MD5(HA1:nonce:nonceCount:cnonce:qop:HA2)
        tt_md_reset(md);
        tt_md_update(md, (tt_u8_t *)s_a1, __MD5_DLEN * 2);
        tt_md_update(md, (tt_u8_t *)":", 1);
        tt_md_update(md, tt_blobex_addr(&ha->nonce), tt_blobex_len(&ha->nonce));
        tt_md_update(md, (tt_u8_t *)":", 1);
        tt_md_update(md, tt_blobex_addr(&ha->nc), tt_blobex_len(&ha->nc));
        tt_md_update(md, (tt_u8_t *)":", 1);
        tt_md_update(md,
                     tt_blobex_addr(&ha->cnonce),
                     tt_blobex_len(&ha->cnonce));
        tt_md_update(md, (tt_u8_t *)":", 1);
        tt_md_update(md,
                     tt_blobex_addr(&ha->raw_qop),
                     tt_blobex_len(&ha->raw_qop));
        tt_md_update(md, (tt_u8_t *)":", 1);
        tt_md_update(md, (tt_u8_t *)s_a2, __MD5_DLEN * 2);
        tt_md_final(md, resp);
    } else {
        // not specified

        // response = MD5(HA1:nonce:HA2)
        tt_md_reset(md);
        tt_md_update(md, (tt_u8_t *)s_a1, __MD5_DLEN * 2);
        tt_md_update(md, (tt_u8_t *)":", 1);
        tt_md_update(md, tt_blobex_addr(&ha->nonce), tt_blobex_len(&ha->nonce));
        tt_md_update(md, (tt_u8_t *)":", 1);
        tt_md_update(md, (tt_u8_t *)s_a2, __MD5_DLEN * 2);
        tt_md_final(md, resp);
    }

    tt_hex2str(resp, __MD5_DLEN, response);

    return TT_SUCCESS;
}
