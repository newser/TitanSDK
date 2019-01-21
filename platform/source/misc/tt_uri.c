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
 * See the License for the opaque language governing permissions and
 * limitations under the License.
 */

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <misc/tt_uri.h>

#include <algorithm/tt_algorithm_def.h>
#include <algorithm/tt_buffer_format.h>
#include <algorithm/tt_string_common.h>
#include <misc/tt_percent_encode.h>

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

// RFC3986:
// unreserved  = ALPHA / DIGIT / "-" / "." / "_" / "~"
// only print first 128 chars, later are left NULL
tt_char_t tt_g_uri_encode_table[256] = {
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    '-', '.', 0,   '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 0,   0,
    0,   0,   0,   0,   0,   'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y',
    'Z', 0,   0,   0,   0,   '_', 0,   'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
    'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w',
    'x', 'y', 'z', 0,   0,   0,   '~', 0,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __parse_uri(IN tt_uri_t *uri,
                               IN tt_char_t *str,
                               IN tt_u32_t len);

static tt_result_t __parse_absolute(IN tt_uri_t *uri,
                                    IN tt_char_t *str,
                                    IN tt_u32_t len);

static tt_result_t __parse_relative(IN tt_uri_t *uri,
                                    IN tt_char_t *str,
                                    IN tt_u32_t len);

static tt_result_t __parse_authority(IN tt_uri_t *uri,
                                     IN tt_char_t *str,
                                     IN tt_u32_t len);

tt_result_t __percent_decode(IN tt_blobex_t *bex,
                             IN tt_char_t *str,
                             IN tt_u32_t len,
                             IN tt_bool_t plus2sp);

static tt_s32_t __uri_blobex_cmp(IN tt_blobex_t *a, IN tt_blobex_t *b);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_uri_init(IN tt_uri_t *uri)
{
    tt_string_init(&uri->uri, NULL);
    tt_blobex_init(&uri->scheme, NULL, 0);
    tt_blobex_init(&uri->opaque, NULL, 0);
    tt_blobex_init(&uri->user_info, NULL, 0);
    tt_blobex_init(&uri->authority, NULL, 0);
    tt_blobex_init(&uri->host, NULL, 0);
    tt_blobex_init(&uri->path, NULL, 0);
    tt_blobex_init(&uri->query, NULL, 0);
    tt_blobex_init(&uri->fragment, NULL, 0);
    uri->port = 0;
    uri->auth_modified = TT_FALSE;
    uri->uri_modified = TT_FALSE;
}

tt_result_t tt_uri_create(IN tt_uri_t *uri, IN tt_char_t *str, IN tt_u32_t len)
{
    tt_uri_init(uri);

    if (len == 0) {
        return TT_SUCCESS;
    }

    if (!TT_OK(__parse_uri(uri, str, len))) {
        // must destroy as this is a create function
        tt_uri_destroy(uri);
    }

    return TT_SUCCESS;
}

void tt_uri_destroy(IN tt_uri_t *uri)
{
    tt_string_destroy(&uri->uri);
    tt_blobex_destroy(&uri->scheme);
    tt_blobex_destroy(&uri->opaque);
    tt_blobex_destroy(&uri->user_info);
    tt_blobex_destroy(&uri->authority);
    tt_blobex_destroy(&uri->host);
    tt_blobex_destroy(&uri->path);
    tt_blobex_destroy(&uri->query);
    tt_blobex_destroy(&uri->fragment);
}

void tt_uri_clear(IN tt_uri_t *uri)
{
    tt_uri_destroy(uri);

    // keep on_path and on_query
    tt_string_init(&uri->uri, NULL);
    tt_blobex_init(&uri->scheme, NULL, 0);
    tt_blobex_init(&uri->opaque, NULL, 0);
    tt_blobex_init(&uri->user_info, NULL, 0);
    tt_blobex_init(&uri->authority, NULL, 0);
    tt_blobex_init(&uri->host, NULL, 0);
    tt_blobex_init(&uri->path, NULL, 0);
    tt_blobex_init(&uri->query, NULL, 0);
    tt_blobex_init(&uri->fragment, NULL, 0);
    uri->port = 0;
    uri->auth_modified = TT_FALSE;
    uri->uri_modified = TT_FALSE;
}

tt_result_t tt_uri_parse_n(IN tt_uri_t *uri, IN tt_char_t *str, IN tt_u32_t len)
{
    tt_uri_clear(uri);
    return TT_COND(len > 0, __parse_uri(uri, str, len), TT_SUCCESS);
}

void tt_uri_encode_table_default(IN tt_uri_encode_table_t *uet)
{
    uet->generic = tt_g_uri_encode_table;
    uet->path = tt_g_uri_encode_table;
    uet->query = tt_g_uri_encode_table;
}

const tt_char_t *tt_uri_render(IN tt_uri_t *uri, IN tt_uri_encode_table_t *uet)
{
    if (uri->uri_modified) {
        // must use tt_buf_clear() but not tt_string_clear()
        tt_buf_clear(&uri->uri.buf);
        if (TT_OK(tt_uri_render2buf(uri, &uri->uri.buf, uet))) {
            uri->uri_modified = TT_FALSE;
        } else {
            tt_string_clear(&uri->uri);
        }
    }

    return tt_string_cstr(&uri->uri);
}

tt_result_t tt_uri_render2buf(IN tt_uri_t *uri,
                              IN tt_buf_t *buf,
                              IN OPT tt_uri_encode_table_t *uet)
{
#define __ENCODE_LEN(name, t)                                                  \
    tt_percent_encode_len(tt_blobex_addr(&uri->name),                          \
                          tt_blobex_len(&uri->name) - 1,                       \
                          t)

#define __ENCODE(name, t)                                                      \
    do {                                                                       \
        tt_u32_t en = tt_percent_encode(tt_blobex_addr(&uri->name),            \
                                        tt_blobex_len(&uri->name) - 1,         \
                                        t,                                     \
                                        (tt_char_t *)TT_BUF_WPOS(buf));        \
        tt_buf_inc_wp(buf, en);                                                \
    } while (0)

    tt_uri_encode_table_t __uet;
    const tt_char_t *p;
    tt_u32_t n = 0, port_len;
    tt_char_t port[20] = {0};

    if (uet == NULL) {
        tt_uri_encode_table_default(&__uet);
        uet = &__uet;
    }

    // ========================================
    // compute encode length
    // ========================================

    // scheme
    p = tt_uri_get_scheme(uri);
    if (p[0] != 0) {
        n += __ENCODE_LEN(scheme, uet->generic);
        n += 1; // ":"
    }

    // opaque
    p = tt_uri_get_opaque(uri);
    if (p[0] != 0) {
        n += __ENCODE_LEN(opaque, uet->generic);
    } else {
        if ((tt_uri_get_userinfo(uri)[0] != 0) ||
            (tt_uri_get_host(uri)[0] != 0) || (tt_uri_get_port(uri) != 0)) {
            n += 2; // "//"

            // user info
            p = tt_uri_get_userinfo(uri);
            if (p[0] != 0) {
                n += __ENCODE_LEN(user_info, uet->generic);
                n += 1; // "@"
            }

            // host
            p = tt_uri_get_host(uri);
            if (p[0] != 0) {
                n += __ENCODE_LEN(host, uet->generic);
            }

            // port
            if (uri->port != 0) {
                tt_snprintf(port, sizeof(port) - 1, "%d", uri->port);
                port_len = tt_strlen(port);

                n += 1; // ":"
                n += port_len;
            }
        }

        p = tt_uri_get_path(uri);
        if (p[0] != 0) {
            n += __ENCODE_LEN(path, uet->path);
        }

        p = tt_uri_get_query(uri);
        if (p[0] != 0) {
            n += 1; // "?"
            n += __ENCODE_LEN(query, uet->query);
        }
    }

    // fragment
    p = tt_uri_get_fragment(uri);
    if (p[0] != 0) {
        n += 1; // "#"
        n += __ENCODE_LEN(fragment, uet->generic);
    }

    // terminating 0
    n += 1;

    // ========================================
    // encode
    // ========================================

    TT_DO(tt_buf_reserve(buf, n + 1));

    // scheme
    p = tt_uri_get_scheme(uri);
    if (p[0] != 0) {
        __ENCODE(scheme, uet->generic);
        tt_buf_put_u8(buf, ':');
    }

    // opaque
    p = tt_uri_get_opaque(uri);
    if (p[0] != 0) {
        __ENCODE(opaque, uet->generic);
    } else {
        if ((tt_uri_get_userinfo(uri)[0] != 0) ||
            (tt_uri_get_host(uri)[0] != 0) || (tt_uri_get_port(uri) != 0)) {
            tt_buf_put_rep(buf, '/', 2);

            // user info
            p = tt_uri_get_userinfo(uri);
            if (p[0] != 0) {
                __ENCODE(user_info, uet->generic);
                tt_buf_put_u8(buf, '@');
            }

            // host
            p = tt_uri_get_host(uri);
            if (p[0] != 0) {
                __ENCODE(host, uet->generic);
            }

            // port
            if (uri->port != 0) {
                tt_buf_put_u8(buf, ':');
                tt_buf_put(buf, (tt_u8_t *)port, port_len);
            }
        }

        p = tt_uri_get_path(uri);
        if (p[0] != 0) {
            __ENCODE(path, uet->path);
        }

        p = tt_uri_get_query(uri);
        if (p[0] != 0) {
            tt_buf_put_u8(buf, '?');
            __ENCODE(query, uet->query);
        }
    }

    // fragment
    p = tt_uri_get_fragment(uri);
    if (p[0] != 0) {
        tt_buf_put_u8(buf, '#');
        __ENCODE(fragment, uet->generic);
    }

    tt_buf_put_u8(buf, 0);
    TT_ASSERT(TT_BUF_RLEN(buf) == n);

    return TT_SUCCESS;

#undef __ENCODE_LEN
#undef __ENCODE
}

tt_s32_t __uri_blobex_cmp(IN tt_blobex_t *a, IN tt_blobex_t *b)
{
    if ((a->addr == NULL) && (b->addr == NULL)) {
        return 0;
    } else if (a->addr == NULL) {
        // then a is "\0", b is "xxx.." => a < b
        return -1;
    } else if (b->addr == NULL) {
        return 1;
    } else {
        return tt_blobex_cmp(a, b);
    }
}

const tt_char_t *tt_uri_get_scheme(IN tt_uri_t *uri)
{
    const tt_char_t *p = (const tt_char_t *)tt_blobex_addr(&uri->scheme);
    return TT_COND(p != NULL, p, "");
}

tt_result_t tt_uri_set_scheme_n(IN tt_uri_t *uri,
                                IN const tt_char_t *scheme,
                                IN tt_u32_t len)
{
    tt_char_t *p;

    uri->uri_modified = TT_TRUE;

    if ((scheme == NULL) || (scheme[0] == 0) || (len == 0)) {
        tt_blobex_clear(&uri->scheme);
        return TT_SUCCESS;
    }

    TT_DO(tt_blobex_set(&uri->scheme, NULL, len + 1, TT_TRUE));
    p = tt_blobex_addr(&uri->scheme);
    tt_memcpy(p, scheme, len);
    p[len] = 0;
    return TT_SUCCESS;
}

const tt_char_t *tt_uri_get_opaque(IN tt_uri_t *uri)
{
    const tt_char_t *p = (const tt_char_t *)tt_blobex_addr(&uri->opaque);
    return TT_COND(p != NULL, p, "");
}

tt_result_t tt_uri_set_opaque_n(IN tt_uri_t *uri,
                                IN const tt_char_t *opaque,
                                IN tt_u32_t len)
{
    tt_char_t *p;

    uri->uri_modified = TT_TRUE;

    // optional: consider as a opaque uri, so clear hierarchical parts
    tt_blobex_clear(&uri->authority);
    uri->auth_modified = TT_FALSE;
    tt_blobex_clear(&uri->user_info);
    tt_blobex_clear(&uri->host);
    uri->port = 0;
    tt_blobex_clear(&uri->path);
    tt_blobex_clear(&uri->query);

    TT_DO(tt_blobex_set(&uri->opaque, NULL, len + 1, TT_TRUE));
    p = tt_blobex_addr(&uri->opaque);
    tt_memcpy(p, opaque, len);
    p[len] = 0;
    return TT_SUCCESS;
}

const tt_char_t *tt_uri_get_userinfo(IN tt_uri_t *uri)
{
    const tt_char_t *p = (const tt_char_t *)tt_blobex_addr(&uri->user_info);
    return TT_COND(p != NULL, p, "");
}

tt_result_t tt_uri_set_userinfo_n(IN tt_uri_t *uri,
                                  IN const tt_char_t *userinfo,
                                  IN tt_u32_t len)
{
    tt_char_t *p;

    uri->uri_modified = TT_TRUE;
    uri->auth_modified = TT_TRUE;

    // consider as a hierarchical uri, so must clear opaque parts
    tt_blobex_clear(&uri->opaque);

    if ((userinfo == NULL) || (userinfo[0] == 0) || (len == 0)) {
        tt_blobex_clear(&uri->user_info);
        return TT_SUCCESS;
    }

    TT_DO(tt_blobex_set(&uri->user_info, NULL, len + 1, TT_TRUE));
    p = tt_blobex_addr(&uri->user_info);
    tt_memcpy(p, userinfo, len);
    p[len] = 0;
    return TT_SUCCESS;
}

const tt_char_t *tt_uri_get_authority(IN tt_uri_t *uri)
{
    const tt_char_t *p;

    if (uri->auth_modified) {
        const tt_char_t *usrinfo, *host;
        tt_u32_t usrinfo_len, host_len, port_len, len;
        tt_char_t port[20] = {0}, *p;

        usrinfo = tt_uri_get_userinfo(uri);
        usrinfo_len = tt_strlen(usrinfo);

        host = tt_uri_get_host(uri);
        host_len = tt_strlen(host);

        if (uri->port != 0) {
            tt_snprintf(port, sizeof(port) - 1, "%d", uri->port);
        }
        port_len = tt_strlen(port);

        // "userinfo@host:port"
        len = usrinfo_len + TT_COND(usrinfo_len != 0, 1, 0);
        len += host_len;
        len += port_len + TT_COND(port_len != 0, 1, 0);
        if (len == 0) {
            tt_blobex_clear(&uri->authority);
        } else {
            len += 1;
            TT_DO_R("", tt_blobex_set(&uri->authority, NULL, len, TT_TRUE));

            p = tt_blobex_addr(&uri->authority);
            if (usrinfo_len != 0) {
                tt_memcpy(p, usrinfo, usrinfo_len);
                p += usrinfo_len;
                *p++ = '@';
            }
            if (host_len != 0) {
                tt_memcpy(p, host, host_len);
                p += host_len;
            }
            if (port_len != 0) {
                *p++ = ':';
                tt_memcpy(p, port, port_len);
                p += port_len;
            }
            *p++ = 0;
            TT_ASSERT(p == ((tt_char_t *)tt_blobex_addr(&uri->authority) +
                            tt_blobex_len(&uri->authority)));
        }
        uri->auth_modified = TT_FALSE;
    }

    p = (const tt_char_t *)tt_blobex_addr(&uri->authority);
    return TT_COND(p != NULL, p, "");
}

tt_result_t tt_uri_parse_authority_n(IN tt_uri_t *uri,
                                     IN const tt_char_t *authority,
                                     IN tt_u32_t len)
{
    uri->uri_modified = TT_TRUE;
    uri->auth_modified = TT_TRUE;

    // consider as a hierarchical uri, so must clear opaque parts
    tt_blobex_clear(&uri->opaque);

    tt_blobex_clear(&uri->user_info);
    tt_blobex_clear(&uri->host);
    uri->port = 0;

    return __parse_authority(uri, (tt_char_t *)authority, len);
}

const tt_char_t *tt_uri_get_host(IN tt_uri_t *uri)
{
    const tt_char_t *p = (const tt_char_t *)tt_blobex_addr(&uri->host);
    return TT_COND(p != NULL, p, "");
}

tt_result_t tt_uri_set_host_n(IN tt_uri_t *uri,
                              IN const tt_char_t *host,
                              IN tt_u32_t len)
{
    tt_char_t *p;

    uri->uri_modified = TT_TRUE;
    uri->auth_modified = TT_TRUE;

    // consider as a hierarchical uri, so must clear opaque parts
    tt_blobex_clear(&uri->opaque);

    if ((host == NULL) || (host[0] == 0) || (len == 0)) {
        tt_blobex_clear(&uri->host);
        return TT_SUCCESS;
    }

    TT_DO(tt_blobex_set(&uri->host, NULL, len + 1, TT_TRUE));
    p = tt_blobex_addr(&uri->host);
    tt_memcpy(p, host, len);
    p[len] = 0;
    return TT_SUCCESS;
}

const tt_char_t *tt_uri_get_path(IN tt_uri_t *uri)
{
    const tt_char_t *p = (const tt_char_t *)tt_blobex_addr(&uri->path);
    return TT_COND(p != NULL, p, "");
}

tt_result_t tt_uri_set_path_n(IN tt_uri_t *uri,
                              IN const tt_char_t *path,
                              IN tt_u32_t len)
{
    tt_char_t *p;

    uri->uri_modified = TT_TRUE;

    // consider as a hierarchical uri, so must clear opaque parts
    tt_blobex_clear(&uri->opaque);

    if ((path == NULL) || (path[0] == 0) || (len == 0)) {
        tt_blobex_clear(&uri->path);
        return TT_SUCCESS;
    }

    TT_DO(tt_blobex_set(&uri->path, NULL, len + 1, TT_TRUE));
    p = tt_blobex_addr(&uri->path);
    tt_memcpy(p, path, len);
    p[len] = 0;
    return TT_SUCCESS;
}

const tt_char_t *tt_uri_get_query(IN tt_uri_t *uri)
{
    const tt_char_t *p = (const tt_char_t *)tt_blobex_addr(&uri->query);
    return TT_COND(p != NULL, p, "");
}

tt_result_t tt_uri_set_query_n(IN tt_uri_t *uri,
                               IN const tt_char_t *query,
                               IN tt_u32_t len)
{
    tt_char_t *p;

    uri->uri_modified = TT_TRUE;

    // consider as a hierarchical uri, so must clear opaque parts
    tt_blobex_clear(&uri->opaque);

    if ((query == NULL) || (query[0] == 0) || (len == 0)) {
        tt_blobex_clear(&uri->query);
        return TT_SUCCESS;
    }

    TT_DO(tt_blobex_set(&uri->query, NULL, len + 1, TT_TRUE));
    p = tt_blobex_addr(&uri->query);
    tt_memcpy(p, query, len);
    p[len] = 0;
    return TT_SUCCESS;
}

const tt_char_t *tt_uri_get_fragment(IN tt_uri_t *uri)
{
    const tt_char_t *p = (const tt_char_t *)tt_blobex_addr(&uri->fragment);
    return TT_COND(p != NULL, p, "");
}

tt_result_t tt_uri_set_fragment_n(IN tt_uri_t *uri,
                                  IN const tt_char_t *fragment,
                                  IN tt_u32_t len)
{
    tt_char_t *p;

    uri->uri_modified = TT_TRUE;

    if ((fragment == NULL) || (fragment[0] == 0) || (len == 0)) {
        tt_blobex_clear(&uri->fragment);
        return TT_SUCCESS;
    }

    TT_DO(tt_blobex_set(&uri->fragment, NULL, len + 1, TT_TRUE));
    p = tt_blobex_addr(&uri->fragment);
    tt_memcpy(p, fragment, len);
    p[len] = 0;
    return TT_SUCCESS;
}

tt_s32_t tt_uri_cmp(IN tt_uri_t *a, IN tt_uri_t *b)
{
    tt_s32_t n;
    tt_bool_t a_opq, b_opq;

#define __CMP(name)                                                            \
    do {                                                                       \
        n = __uri_blobex_cmp(&a->name, &b->name);                              \
        if (n != 0) {                                                          \
            return n;                                                          \
        }                                                                      \
    } while (0)

    __CMP(scheme);

    a_opq = tt_uri_is_opaque(a);
    b_opq = tt_uri_is_opaque(b);
    if (a_opq != b_opq) {
        // hierarchical < opaque
        return TT_COND(a_opq, 1, -1);
    }

    if (a_opq) {
        __CMP(opaque);
    } else {
        __CMP(user_info);
        __CMP(host);
        if (a->port != b->port) {
            return a->port - b->port;
        }
        __CMP(path);
        __CMP(query);
    }

    __CMP(fragment);

    return 0;
#undef __CMP
}

tt_result_t __parse_uri(IN tt_uri_t *uri, IN tt_char_t *str, IN tt_u32_t len)
{
    tt_char_t *cur = str, *end = str + len, *scheme, *fragment;
    tt_u32_t n;

    uri->auth_modified = TT_TRUE;
    uri->uri_modified = TT_TRUE;

    // [scheme:]scheme-opaque-part[#fragment]

    // scheme
    if ((scheme = tt_memchr(cur, ':', len)) != NULL) {
        n = (tt_u32_t)(scheme - cur);
        if (n > 0) {
            TT_DO(__percent_decode(&uri->scheme, cur, n, TT_FALSE));
        }
        cur = scheme + 1;
        len -= (n + 1);
    }
    if (cur >= end) {
        return TT_SUCCESS;
    }

    // fragment
    if ((fragment = tt_memchr(cur, '#', len)) != NULL) {
        fragment += 1;
        n = (tt_u32_t)(end - fragment);
        if (n > 0) {
            TT_DO(__percent_decode(&uri->fragment, fragment, n, TT_FALSE));
        }
        len -= (n + 1);
    }

    if (scheme != NULL) {
        // has scheme, absolute uri
        return __parse_absolute(uri, cur, len);
    } else {
        // no scheme, relative uri
        return __parse_relative(uri, cur, len);
    }
}

tt_result_t __parse_absolute(IN tt_uri_t *uri,
                             IN tt_char_t *str,
                             IN tt_u32_t len)
{
    tt_char_t *cur = str, *end = str + len, *authority, *path, *query;
    tt_u32_t authority_len, path_len, query_len;

    if (*cur != '/') {
        // opaque uri
        return TT_COND(len > 0,
                       __percent_decode(&uri->opaque, cur, len, TT_FALSE),
                       TT_SUCCESS);
    }
    // hierarchical uri: [user-info@]host[:port][path][?query]

    if (((cur + 1) < end) && (cur[1] == '/')) {
        // authority
        cur += 2;
        len -= 2;
        authority = cur;

        path = tt_memchr(cur, '/', len);
        query = tt_memchr(cur, '?', len);
        if ((path != NULL) && (query != NULL) && (path >= query)) {
            // '/' in query is allowd
            path = NULL;
        }

        if ((path != NULL) && (query != NULL)) {
            authority_len = (tt_u32_t)(path - authority);
            path_len = (tt_u32_t)(query - path);
            ++query;
            query_len = (tt_u32_t)(end - query);
        } else if (path != NULL) {
            authority_len = (tt_u32_t)(path - authority);
            path_len = (tt_u32_t)(end - path);
            query_len = 0;
        } else if (query != NULL) {
            authority_len = (tt_u32_t)(query - authority);
            path_len = 0;
            ++query;
            query_len = (tt_u32_t)(end - query);
        } else {
            authority_len = (tt_u32_t)(end - authority);
            path_len = 0;
            query_len = 0;
        }
    } else {
        authority = NULL;
        authority_len = 0;

        // the beginning '/' must be kept
        path = cur;
        query = tt_memchr(cur, '?', len);
        if (query != NULL) {
            path_len = (tt_u32_t)(query - path);
            ++query;
            query_len = (tt_u32_t)(end - query);
        } else {
            path_len = (tt_u32_t)(end - path);
            query_len = 0;
        }
    }

    if ((authority != NULL) && (authority_len != 0)) {
        TT_DO(__parse_authority(uri, authority, authority_len));
    }

    if ((path != NULL) && (path_len != 0)) {
        TT_DO(__percent_decode(&uri->path, path, path_len, TT_FALSE));
    }

    if ((query != NULL) && (query_len != 0)) {
        // note we can decode '+' to ' ' in query part
        TT_DO(__percent_decode(&uri->query, query, query_len, TT_TRUE));
    }

    return TT_SUCCESS;
}

tt_result_t __parse_relative(IN tt_uri_t *uri,
                             IN tt_char_t *str,
                             IN tt_u32_t len)
{
    tt_char_t *cur = str, *end = str + len, *path, *query;
    tt_u32_t path_len, query_len;

    // hierarchical uri: [path][?query]

    path = cur;
    query = tt_memchr(cur, '?', len);
    if (query != NULL) {
        path_len = (tt_u32_t)(query - path);
        ++query;
        query_len = (tt_u32_t)(end - query);
    } else {
        path_len = (tt_u32_t)(end - path);
        query_len = 0;
    }

    if ((path != NULL) && (path_len != 0)) {
        TT_DO(__percent_decode(&uri->path, path, path_len, TT_FALSE));
    }

    if ((query != NULL) && (query_len != 0)) {
        TT_DO(__percent_decode(&uri->query, query, query_len, TT_TRUE));
    }

    return TT_SUCCESS;
}

tt_result_t __parse_authority(IN tt_uri_t *uri,
                              IN tt_char_t *str,
                              IN tt_u32_t len)
{
    tt_char_t *cur = str, *end = str + len, *at, *port;
    tt_u32_t n;

    // [user-info@]host[:port]

    // user-info
    if ((at = tt_memchr(cur, '@', len)) != NULL) {
        n = (tt_u32_t)(at - cur);
        if (n > 0) {
            TT_DO(__percent_decode(&uri->user_info, cur, n, TT_FALSE));
        }
        cur = at + 1;
        len -= (n + 1);
    }
    if (cur >= end) {
        return TT_SUCCESS;
    }

    // port
    if ((port = tt_memchr(cur, ':', len)) != NULL) {
        tt_u32_t port_val;

        port += 1;
        n = (tt_u32_t)(end - port);
        if (n == 0) {
            TT_ERROR("unspecified uri port");
            return TT_E_BADARG;
        } else if (n > 5) {
            // max u16 65535
            TT_ERROR("too large uri port");
            return TT_E_BADARG;
        } else {
            tt_char_t tmp[6] = {0};
            tt_memcpy(tmp, port, n);
            if (!TT_OK(tt_strtou32(tmp, NULL, 10, &port_val)) ||
                (port_val > 0xFFFF)) {
                TT_ERROR("invalid uri port");
                return TT_E_BADARG;
            }
        }
        uri->port = port_val;

        len -= (n + 1);
    }

    // left are considered as host part
    if (len > 0) {
        TT_DO(__percent_decode(&uri->host, cur, len, TT_FALSE));
    }

    return TT_SUCCESS;
}

tt_result_t __percent_decode(IN tt_blobex_t *bex,
                             IN tt_char_t *str,
                             IN tt_u32_t len,
                             IN tt_bool_t plus2sp)
{
    tt_u32_t n;
    tt_char_t *dst;

    n = tt_percent_decode_len(str, len);
    TT_DO(tt_blobex_set(bex, NULL, n + 1, TT_TRUE));

    dst = (tt_char_t *)tt_blobex_addr(bex);
    n = tt_percent_decode(str, len, plus2sp, dst);
    TT_ASSERT(n == (tt_blobex_len(bex) - 1));
    dst[n] = 0;

    return TT_SUCCESS;
}
