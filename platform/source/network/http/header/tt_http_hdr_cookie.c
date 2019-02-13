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

#include <network/http/header/tt_http_hdr_cookie.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __CK_EXPIRES "Expires"
#define __CK_MAX_AGE "Max-Age"
#define __CK_DOMAIN "Domain"
#define __CK_PATH "Path"
#define __CK_SECURE "Secure"
#define __CK_HTTPONLY "HttpOnly"

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef struct
{
    tt_dlist_t ck_lst;
    tt_http_cookie_t *cur;
} __hdr_cookie_t;

typedef struct
{
    tt_dnode_t node;
    tt_blobex_t name;
    tt_blobex_t val;
} __ck_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static void __hc_destroy(IN tt_http_hdr_t *h);

static tt_result_t __hc_parse(IN tt_http_hdr_t *h,
                              IN const tt_char_t *val,
                              IN tt_u32_t len);

static tt_u32_t __hc_render_len(IN tt_http_hdr_t *h);

static tt_u32_t __hc_render(IN tt_http_hdr_t *h, IN tt_char_t *dst);

static tt_http_hdr_itf_t __hc_itf = {
    __hc_destroy, NULL, __hc_parse, NULL, __hc_render_len, __hc_render,
};

static tt_result_t __hsc_pre_parse(IN tt_http_hdr_t *h);

static tt_result_t __hsc_parse(IN tt_http_hdr_t *h,
                               IN const tt_char_t *val,
                               IN tt_u32_t len);

static tt_result_t __hsc_post_parse(IN tt_http_hdr_t *h);

static tt_u32_t __hsc_render_len(IN tt_http_hdr_t *h);

static tt_u32_t __hsc_render(IN tt_http_hdr_t *h, IN tt_char_t *dst);

static tt_http_hdr_itf_t __hsc_itf = {
    __hc_destroy,
    __hsc_pre_parse,
    __hsc_parse,
    __hsc_post_parse,
    __hsc_render_len,
    __hsc_render,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_http_hdr_t *__hdr_create(IN tt_http_hname_t name,
                                   IN tt_http_hdr_itf_t *itf);

static tt_http_cookie_t *__find_ck(IN tt_dlist_t *ck_lst,
                                   IN const tt_char_t *name,
                                   IN tt_u32_t name_len);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_http_hdr_t *tt_http_hdr_cookie_create()
{
    return __hdr_create(TT_HTTP_HDR_COOKIE, &__hc_itf);
}

tt_http_hdr_t *tt_http_hdr_set_cookie_create()
{
    return __hdr_create(TT_HTTP_HDR_SET_COOKIE, &__hsc_itf);
}

tt_http_cookie_t *tt_http_hdr_find_cookie_n(IN tt_http_hdr_t *h,
                                            IN const tt_char_t *name,
                                            IN tt_u32_t name_len)
{
    __hdr_cookie_t *hc = TT_HTTP_HDR_CAST(h, __hdr_cookie_t);

    TT_ASSERT((h->name == TT_HTTP_HDR_COOKIE) ||
              (h->name == TT_HTTP_HDR_SET_COOKIE));

    return __find_ck(&hc->ck_lst, name, name_len);
}

void tt_http_hdr_add_cookie(IN tt_http_hdr_t *h, IN TO tt_http_cookie_t *c)
{
    __hdr_cookie_t *hc = TT_HTTP_HDR_CAST(h, __hdr_cookie_t);

    tt_dlist_push_tail(&hc->ck_lst, &c->node);
}

void __hc_destroy(IN tt_http_hdr_t *h)
{
    __hdr_cookie_t *hc = TT_HTTP_HDR_CAST(h, __hdr_cookie_t);
    tt_dnode_t *node;

    while ((node = tt_dlist_pop_head(&hc->ck_lst)) != NULL) {
        tt_http_cookie_destroy(TT_CONTAINER(node, tt_http_cookie_t, node));
    }

    if (hc->cur != NULL) {
        tt_http_cookie_destroy(hc->cur);
    }
}

tt_result_t __hc_parse(IN tt_http_hdr_t *h,
                       IN const tt_char_t *val,
                       IN tt_u32_t len)
{
    __hdr_cookie_t *hc = TT_HTTP_HDR_CAST(h, __hdr_cookie_t);
    const tt_char_t *n, *v;
    tt_u32_t nlen, vlen;
    tt_http_cookie_t *c;

    n = val;
    v = tt_memchr(n, '=', len);
    if (v != NULL) {
        nlen = (tt_u32_t)(v - n);
        ++v;
        TT_ASSERT(len >= (nlen + 1));
        vlen = len - nlen - 1;
    } else {
        nlen = len;
        vlen = 0;
    }

    if (nlen == 0) {
        return TT_SUCCESS;
    }

    /*
     create a tt_http_cookie_t for each val of Cookie header

     cookie-header = "Cookie:" OWS cookie-string OWS
     cookie-string = cookie-pair *( ";" SP cookie-pair )
     */

    c = tt_http_cookie_create();
    if (c == NULL) {
        return TT_FAIL;
    }

    tt_blobex_set(&c->name, (tt_u8_t *)n, nlen, TT_FALSE);

    if (vlen > 0) {
        tt_blobex_set(&c->val, (tt_u8_t *)v, vlen, TT_FALSE);
    }

    tt_dlist_push_tail(&hc->ck_lst, &c->node);
    return TT_SUCCESS;
}

tt_u32_t __hc_render_len(IN tt_http_hdr_t *h)
{
    __hdr_cookie_t *hc = TT_HTTP_HDR_CAST(h, __hdr_cookie_t);
    tt_u32_t n;
    tt_http_cookie_t *c;
    tt_bool_t empty;

    // always render "Cookie: "
    n = tt_g_http_hname_len[h->name] + 2;

    c = tt_http_cookie_head(h);
    empty = TT_BOOL(c == NULL);
    for (; c != NULL; c = tt_http_cookie_next(c)) {
        tt_u32_t nlen = tt_blobex_len(&c->name);
        tt_u32_t vlen = tt_blobex_len(&c->val);

        if (nlen == 0) {
            continue;
        } else if (vlen > 0) {
            // "name=val; "
            n += nlen + vlen + 3;
        } else {
            // "name; "
            n += nlen + 2;
        }

        vlen = tt_blobex_len(&c->extension);
        if (vlen != 0) {
            // "xx; "
            n += vlen + 2;
        }
    }

    if (empty) {
        n += 2;
    }

    return n;
}

tt_u32_t __hc_render(IN tt_http_hdr_t *h, IN tt_char_t *dst)
{
    __hdr_cookie_t *hc = TT_HTTP_HDR_CAST(h, __hdr_cookie_t);
    tt_char_t *p;
    tt_u32_t n;
    tt_http_cookie_t *c;
    tt_bool_t empty;

    // "Cookie: "
    p = dst;
    n = tt_g_http_hname_len[h->name];
    tt_memcpy(p, tt_g_http_hname[h->name], n);
    p += n;
    *p++ = ':';
    *p++ = ' ';

    c = tt_http_cookie_head(h);
    empty = TT_BOOL(c == NULL);
    for (; c != NULL; c = tt_http_cookie_next(c)) {
        tt_u32_t nlen = tt_blobex_len(&c->name);
        tt_u32_t vlen = tt_blobex_len(&c->val);

        if (nlen == 0) {
            continue;
        } else if (vlen > 0) {
            // "name=val; "
            tt_memcpy(p, tt_blobex_addr(&c->name), nlen);
            p += nlen;
            *p++ = '=';
            tt_memcpy(p, tt_blobex_addr(&c->val), vlen);
            p += vlen;
            *p++ = ';';
            *p++ = ' ';
        } else {
            // "name; "
            tt_memcpy(p, tt_blobex_addr(&c->name), nlen);
            p += nlen;
            *p++ = ';';
            *p++ = ' ';
        }

        vlen = tt_blobex_len(&c->extension);
        if (vlen != 0) {
            // "xx; "
            tt_memcpy(p, tt_blobex_addr(&c->extension), vlen);
            p += vlen;
            *p++ = ';';
            *p++ = ' ';
        }
    }

    // "\r\n"
    if (!empty) {
        p -= 2;
    }
    *p++ = '\r';
    *p++ = '\n';

    return (tt_u32_t)(p - dst);
}

tt_result_t __hsc_pre_parse(IN tt_http_hdr_t *h)
{
    __hdr_cookie_t *hc = TT_HTTP_HDR_CAST(h, __hdr_cookie_t);

    if (hc->cur != NULL) {
        tt_http_cookie_clear(hc->cur);
    } else {
        tt_http_cookie_t *c = tt_http_cookie_create();
        if (c == NULL) {
            return TT_FAIL;
        }
        hc->cur = c;
    }

    return TT_SUCCESS;
}

tt_result_t __hsc_parse(IN tt_http_hdr_t *h,
                        IN const tt_char_t *val,
                        IN tt_u32_t len)
{
    __hdr_cookie_t *hc = TT_HTTP_HDR_CAST(h, __hdr_cookie_t);
    const tt_char_t *n, *v;
    tt_u32_t nlen, vlen;
    tt_http_cookie_t *c;

    n = val;
    v = tt_memchr(n, '=', len);
    if (v != NULL) {
        nlen = (tt_u32_t)(v - n);
        ++v;
        TT_ASSERT(len >= (nlen + 1));
        vlen = len - nlen - 1;
    } else {
        nlen = len;
        vlen = 0;
    }

    if (nlen == 0) {
        return TT_SUCCESS;
    }

    c = hc->cur;
    TT_ASSERT(c != NULL);

    if (tt_blobex_len(&c->name) == 0) {
        tt_blobex_set(&c->name, (tt_u8_t *)n, nlen, TT_FALSE);

        if (vlen > 0) {
            tt_blobex_set(&c->val, (tt_u8_t *)v, vlen, TT_FALSE);
        }
    } else {
#define __IS_CKV(name)                                                         \
    (nlen == (sizeof(name) - 1)) && (tt_strnicmp(n, name, nlen) == 0)

        if (__IS_CKV(__CK_EXPIRES)) {
            tt_blobex_set(&c->expires, (tt_u8_t *)v, vlen, TT_FALSE);
        } else if (__IS_CKV(__CK_MAX_AGE)) {
            tt_char_t tmp[16];
            if (vlen >= sizeof(tmp)) {
                return TT_FAIL;
            }
            tt_memcpy(tmp, v, vlen);
            tmp[vlen] = 0;

            if (!TT_OK(tt_strtou32(tmp, NULL, 10, &c->max_age))) {
                c->max_age = 0;
                return TT_FAIL;
            }
        } else if (__IS_CKV(__CK_DOMAIN)) {
            tt_blobex_set(&c->domain, (tt_u8_t *)v, vlen, TT_FALSE);
        } else if (__IS_CKV(__CK_PATH)) {
            tt_blobex_set(&c->path, (tt_u8_t *)v, vlen, TT_FALSE);
        } else if (__IS_CKV(__CK_SECURE)) {
            c->secure = TT_TRUE;
        } else if (__IS_CKV(__CK_HTTPONLY)) {
            c->httponly = TT_TRUE;
        } else {
            /* todo: optimize
            tt_blobex_memcat(&c->extension, (tt_u8_t*)val, len);
            tt_blobex_memcat(&c->extension, (tt_u8_t*)"; ", 2);
             */
        }
    }

    return TT_SUCCESS;
}

tt_result_t __hsc_post_parse(IN tt_http_hdr_t *h)
{
    __hdr_cookie_t *hc = TT_HTTP_HDR_CAST(h, __hdr_cookie_t);
    tt_http_cookie_t *c;

    TT_ASSERT(hc->cur != NULL);
    c = hc->cur;

    if (tt_blobex_len(&c->name) > 0) {
        tt_dlist_push_tail(&hc->ck_lst, &c->node);
        hc->cur = NULL;
    }
    // else left hc->cur there and will be either cleared or destroyed

    return TT_SUCCESS;
}

tt_u32_t __hsc_render_len(IN tt_http_hdr_t *h)
{
    __hdr_cookie_t *hc = TT_HTTP_HDR_CAST(h, __hdr_cookie_t);
    tt_u32_t n;
    tt_http_cookie_t *c;

#define __CRL(name, val)                                                       \
    do {                                                                       \
        vlen = tt_blobex_len(&c->val);                                         \
        if (vlen != 0) {                                                       \
            /* "Expires=xx; " */                                               \
            n += sizeof(name) + vlen + 2;                                      \
        }                                                                      \
    } while (0)

    // render a set-cookie header for each cookie

    n = 0;
    for (c = tt_http_cookie_head(h); c != NULL; c = tt_http_cookie_next(c)) {
        tt_u32_t nlen = tt_blobex_len(&c->name);
        tt_u32_t vlen = tt_blobex_len(&c->val);

        if (nlen == 0) {
            continue;
        }
        // "Set-Cookie: "
        n += tt_g_http_hname_len[h->name] + 2;

        if (vlen > 0) {
            // "name=val; "
            n += nlen + vlen + 3;
        } else {
            // "name; "
            n += nlen + 2;
        }

        __CRL(__CK_EXPIRES, expires);

        if (c->max_age != 0) {
            // max u32 has about 10 decimal numbers
            n += sizeof(__CK_MAX_AGE) + 12;
        }

        __CRL(__CK_DOMAIN, domain);
        __CRL(__CK_PATH, path);

        vlen = tt_blobex_len(&c->extension);
        if (vlen != 0) {
            // "xx; "
            n += vlen + 2;
        }

        if (c->secure) {
            // "Secure; "
            n += sizeof(__CK_SECURE) + 1;
        }

        if (c->httponly) {
            // "HttpOnly; "
            n += sizeof(__CK_HTTPONLY) + 1;
        }
    }

    return n;
}

tt_u32_t __hsc_render(IN tt_http_hdr_t *h, IN tt_char_t *dst)
{
    __hdr_cookie_t *hc = TT_HTTP_HDR_CAST(h, __hdr_cookie_t);
    tt_char_t *p;
    tt_u32_t n;
    tt_http_cookie_t *c;

#define __CR(name, val)                                                        \
    do {                                                                       \
        vlen = tt_blobex_len(&c->val);                                         \
        if (vlen != 0) {                                                       \
            tt_memcpy(p, name, sizeof(name) - 1);                              \
            p += sizeof(name) - 1;                                             \
            *p++ = '=';                                                        \
            tt_memcpy(p, tt_blobex_addr(&c->val), vlen);                       \
            p += vlen;                                                         \
            *p++ = ';';                                                        \
            *p++ = ' ';                                                        \
        }                                                                      \
    } while (0)

    p = dst;
    for (c = tt_http_cookie_head(h); c != NULL; c = tt_http_cookie_next(c)) {
        tt_u32_t nlen = tt_blobex_len(&c->name);
        tt_u32_t vlen = tt_blobex_len(&c->val);

        if (nlen == 0) {
            continue;
        }

        // "Set-Cookie: "
        n = tt_g_http_hname_len[h->name];
        tt_memcpy(p, tt_g_http_hname[h->name], n);
        p += n;
        *p++ = ':';
        *p++ = ' ';

        if (vlen > 0) {
            // "name=val; "
            tt_memcpy(p, tt_blobex_addr(&c->name), nlen);
            p += nlen;
            *p++ = '=';
            tt_memcpy(p, tt_blobex_addr(&c->val), vlen);
            p += vlen;
            *p++ = ';';
            *p++ = ' ';
        } else {
            // "name; "
            tt_memcpy(p, tt_blobex_addr(&c->name), nlen);
            p += nlen;
            *p++ = ';';
            *p++ = ' ';
        }

        __CR(__CK_EXPIRES, expires);

        if (c->max_age != 0) {
            tt_memcpy(p, __CK_MAX_AGE, sizeof(__CK_MAX_AGE) - 1);
            p += sizeof(__CK_MAX_AGE) - 1;
            *p++ = '=';
            p += tt_snprintf(p, 11, "%u", c->max_age);
            *p++ = ';';
            *p++ = ' ';
        }

        __CR(__CK_DOMAIN, domain);
        __CR(__CK_PATH, path);

        vlen = tt_blobex_len(&c->extension);
        if (vlen != 0) {
            // "xx; "
            tt_memcpy(p, tt_blobex_addr(&c->extension), vlen);
            p += vlen;
            *p++ = ';';
            *p++ = ' ';
        }

        if (c->secure) {
            // "Secure; "
            tt_memcpy(p, __CK_SECURE, sizeof(__CK_SECURE) - 1);
            p += sizeof(__CK_SECURE) - 1;
            *p++ = ';';
            *p++ = ' ';
        }

        if (c->httponly) {
            // "HttpOnly; "
            tt_memcpy(p, __CK_HTTPONLY, sizeof(__CK_HTTPONLY) - 1);
            p += sizeof(__CK_HTTPONLY) - 1;
            *p++ = ';';
            *p++ = ' ';
        }

        // "\r\n"
        p -= 2;
        *p++ = '\r';
        *p++ = '\n';
    }

    return (tt_u32_t)(p - dst);
}

tt_http_hdr_t *__hdr_create(IN tt_http_hname_t name, IN tt_http_hdr_itf_t *itf)
{
    tt_http_hdr_t *h;
    __hdr_cookie_t *hc;

    h = tt_http_hdr_create_scs(sizeof(__hdr_cookie_t), name, itf);
    if (h == NULL) {
        return NULL;
    }

    hc = TT_HTTP_HDR_CAST(h, __hdr_cookie_t);

    tt_dlist_init(&hc->ck_lst);
    hc->cur = NULL;

    return h;
}

tt_http_cookie_t *__find_ck(IN tt_dlist_t *ck_lst,
                            IN const tt_char_t *name,
                            IN tt_u32_t name_len)
{
    tt_dnode_t *node;

    for (node = tt_dlist_head(ck_lst); node != NULL; node = node->next) {
        tt_http_cookie_t *c = TT_CONTAINER(node, tt_http_cookie_t, node);
        if (tt_blobex_memcmp(&c->name, (tt_u8_t *)name, name_len) == 0) {
            return c;
        }
    }
    return NULL;
}

// ========================================
// http cookie api
// ========================================

tt_http_cookie_t *tt_http_cookie_create()
{
    tt_http_cookie_t *c;

    c = tt_malloc(sizeof(tt_http_cookie_t));
    if (c == NULL) {
        TT_ERROR("no mem for http cookie");
        return NULL;
    }

    tt_dnode_init(&c->node);
    tt_blobex_init(&c->name, NULL, 0);
    tt_blobex_init(&c->val, NULL, 0);
    tt_blobex_init(&c->expires, NULL, 0);
    c->max_age = 0;
    c->secure = TT_FALSE;
    c->httponly = TT_FALSE;
    tt_blobex_init(&c->domain, NULL, 0);
    tt_blobex_init(&c->path, NULL, 0);
    tt_blobex_init(&c->extension, NULL, 0);

    return c;
}

void tt_http_cookie_destroy(IN tt_http_cookie_t *c)
{
    TT_ASSERT(!tt_dnode_in_dlist(&c->node));

    tt_blobex_destroy(&c->name);
    tt_blobex_destroy(&c->val);
    tt_blobex_destroy(&c->expires);
    tt_blobex_destroy(&c->domain);
    tt_blobex_destroy(&c->path);
    tt_blobex_destroy(&c->extension);

    tt_free(c);
}

void tt_http_cookie_clear(IN tt_http_cookie_t *c)
{
    // no need to modify c->node
    tt_blobex_clear(&c->name);
    tt_blobex_clear(&c->val);
    tt_blobex_clear(&c->expires);
    c->max_age = 0;
    c->secure = TT_FALSE;
    c->httponly = TT_FALSE;
    tt_blobex_clear(&c->domain);
    tt_blobex_clear(&c->path);
    tt_blobex_clear(&c->extension);
}

tt_http_cookie_t *tt_http_cookie_head(IN tt_http_hdr_t *h)
{
    __hdr_cookie_t *hc = TT_HTTP_HDR_CAST(h, __hdr_cookie_t);
    tt_dnode_t *node;

    TT_ASSERT((h->name == TT_HTTP_HDR_COOKIE) ||
              (h->name == TT_HTTP_HDR_SET_COOKIE));

    node = tt_dlist_head(&hc->ck_lst);
    return TT_COND(node != NULL,
                   TT_CONTAINER(node, tt_http_cookie_t, node),
                   NULL);
}

tt_http_cookie_t *tt_http_cookie_next(IN tt_http_cookie_t *c)
{
    tt_dnode_t *node = c->node.next;
    return TT_COND(node != NULL,
                   TT_CONTAINER(node, tt_http_cookie_t, node),
                   NULL);
}
