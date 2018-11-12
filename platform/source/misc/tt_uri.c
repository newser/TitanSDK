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
#include <algorithm/tt_string_common.h>

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
                             IN tt_u32_t len);

tt_result_t __percent_encode(IN tt_blobex_t *bex,
                             IN tt_char_t *str,
                             IN tt_u32_t len,
                             IN OPT tt_char_t *enc_tbl);

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
    uri->authority_modified = TT_FALSE;
    uri->uri_modified = TT_FALSE;
}

tt_result_t tt_uri_create(IN tt_uri_t *uri, IN tt_char_t *str, IN tt_u32_t len)
{
    tt_uri_init(uri);
    return TT_COND(len > 0, __parse_uri(uri, str, len), TT_SUCCESS);
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
    tt_uri_init(uri);
}

tt_result_t tt_uri_set(IN tt_uri_t *uri, IN tt_char_t *str, IN tt_u32_t len)
{
    tt_uri_clear(uri);
    return TT_COND(len > 0, __parse_uri(uri, str, len), TT_SUCCESS);
}

const tt_char_t *tt_uri_encode(IN tt_uri_t *uri, IN OPT tt_char_t *enc_tbl)
{
    if (uri->uri_modified) {
#define __ENCODE_COMPONENT(name)                                               \
    do {                                                                       \
        TT_DO_G(out,                                                           \
                __percent_encode(&b,                                           \
                                 tt_blobex_addr(&uri->name),                   \
                                 tt_blobex_len(&uri->name) - 1,                \
                                 enc_tbl));                                    \
        TT_DO_G(out,                                                           \
                tt_string_append_n(s,                                          \
                                   tt_blobex_addr(&b),                         \
                                   tt_blobex_len(&b) - 1));                    \
                                                                               \
    } while (0)

        tt_string_t *s = &uri->uri;
        tt_blobex_t b;
        const tt_char_t *p;

        if (enc_tbl == NULL) {
            enc_tbl = tt_g_uri_encode_table;
        }

        tt_string_clear(s);
        tt_blobex_init(&b, NULL, 0);

        // scheme
        p = tt_uri_get_scheme(uri);
        if (p[0] != 0) {
            __ENCODE_COMPONENT(scheme);
            TT_DO_G(out, tt_string_append_c(s, ':'));
        }

        // opaque
        p = tt_uri_get_opaque(uri);
        if (p[0] != 0) {
            __ENCODE_COMPONENT(opaque);
        } else {
            if ((tt_uri_get_userinfo(uri)[0] != 0) ||
                (tt_uri_get_host(uri)[0] != 0) || (tt_uri_get_port(uri) != 0)) {
                TT_DO_G(out, tt_string_append_n(s, "//", 2));

                // user info
                p = tt_uri_get_userinfo(uri);
                if (p[0] != 0) {
                    __ENCODE_COMPONENT(user_info);
                    TT_DO_G(out, tt_string_append_c(s, '@'));
                }

                // host
                p = tt_uri_get_host(uri);
                if (p[0] != 0) {
                    __ENCODE_COMPONENT(host);
                }

                // port
                if (uri->port != 0) {
                    tt_char_t port[20] = {0};
                    tt_snprintf(port, sizeof(port) - 1, "%d", uri->port);
                    TT_DO_G(out, tt_string_append_c(s, ':'));
                    TT_DO_G(out, tt_string_append(s, port));
                }
            }

            p = tt_uri_get_path(uri);
            if (p[0] != 0) {
                __ENCODE_COMPONENT(path);
            }

            p = tt_uri_get_query(uri);
            if (p[0] != 0) {
                TT_DO_G(out, tt_string_append_c(s, '?'));
                __ENCODE_COMPONENT(query);
            }
        }

        // fragment
        p = tt_uri_get_fragment(uri);
        if (p[0] != 0) {
            TT_DO_G(out, tt_string_append_c(s, '#'));
            __ENCODE_COMPONENT(fragment);
        }

        // done
        uri->uri_modified = TT_FALSE;

    out:
        tt_blobex_destroy(&b);

        if (uri->uri_modified) {
            // something failed
            tt_string_clear(&uri->uri);
        }

#undef __ENCODE_COMPONENT
    }

    return tt_string_cstr(&uri->uri);
}

tt_result_t tt_uri_encode2buf(IN tt_uri_t *uri,
                              IN tt_buf_t *buf,
                              IN OPT tt_char_t *enc_tbl)
{
    tt_u8_t *p = (tt_u8_t *)tt_uri_encode(uri, enc_tbl);
    if (uri->uri_modified) {
        // todo, now a trick: true uri_modified after tt_uri_encode() means
        // failure
        return TT_FAIL;
    }

    if (p[0] == 0) {
        // empty uri;
        return TT_SUCCESS;
    }

    return tt_buf_put(buf,
                      (tt_u8_t *)tt_string_cstr(&uri->uri),
                      tt_string_len(&uri->uri));
}

const tt_char_t *tt_uri_get_scheme(IN tt_uri_t *uri)
{
    const tt_char_t *p = (const tt_char_t *)tt_blobex_addr(&uri->scheme);
    return TT_COND(p != NULL, p, "");
}

tt_result_t tt_uri_set_scheme(IN tt_uri_t *uri,
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

tt_result_t tt_uri_set_opaque(IN tt_uri_t *uri,
                              IN const tt_char_t *opaque,
                              IN tt_u32_t len)
{
    tt_char_t *p;

    uri->uri_modified = TT_TRUE;

    // optional: consider as a opaque uri, so clear hierarchical parts
    tt_blobex_clear(&uri->authority);
    uri->authority_modified = TT_FALSE;
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

tt_result_t tt_uri_set_userinfo(IN tt_uri_t *uri,
                                IN const tt_char_t *userinfo,
                                IN tt_u32_t len)
{
    tt_char_t *p;

    uri->uri_modified = TT_TRUE;
    uri->authority_modified = TT_TRUE;

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

    if (uri->authority_modified) {
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
            TT_DO(tt_blobex_set(&uri->authority, NULL, len, TT_TRUE));

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
        uri->authority_modified = TT_FALSE;
    }

    p = (const tt_char_t *)tt_blobex_addr(&uri->authority);
    return TT_COND(p != NULL, p, "");
}

tt_result_t tt_uri_decode_authority(IN tt_uri_t *uri,
                                    IN const tt_char_t *authority,
                                    IN tt_u32_t len)
{
    uri->uri_modified = TT_TRUE;
    uri->authority_modified = TT_TRUE;

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

tt_result_t tt_uri_set_host(IN tt_uri_t *uri,
                            IN const tt_char_t *host,
                            IN tt_u32_t len)
{
    tt_char_t *p;

    uri->uri_modified = TT_TRUE;
    uri->authority_modified = TT_TRUE;

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

tt_result_t tt_uri_set_path(IN tt_uri_t *uri,
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

tt_result_t tt_uri_set_query(IN tt_uri_t *uri,
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

tt_result_t tt_uri_set_fragment(IN tt_uri_t *uri,
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

tt_result_t __parse_uri(IN tt_uri_t *uri, IN tt_char_t *str, IN tt_u32_t len)
{
    tt_char_t *cur = str, *end = str + len, *scheme, *fragment;
    tt_u32_t n;

    uri->authority_modified = TT_TRUE;
    uri->uri_modified = TT_TRUE;

    // [scheme:]scheme-opaque-part[#fragment]

    // scheme
    if ((scheme = tt_memchr(cur, ':', len)) != NULL) {
        n = (tt_u32_t)(scheme - cur);
        if (n > 0) {
            TT_DO(__percent_decode(&uri->scheme, cur, n));
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
            TT_DO(__percent_decode(&uri->fragment, fragment, n));
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
                       __percent_decode(&uri->opaque, cur, len),
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
        TT_DO(__percent_decode(&uri->path, path, path_len));
    }

    if ((query != NULL) && (query_len != 0)) {
        TT_DO(__percent_decode(&uri->query, query, query_len));
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
        TT_DO(__percent_decode(&uri->path, path, path_len));
    }

    if ((query != NULL) && (query_len != 0)) {
        TT_DO(__percent_decode(&uri->query, query, query_len));
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
            TT_DO(__percent_decode(&uri->user_info, cur, n));
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
        TT_DO(__percent_decode(&uri->host, cur, len));
    }

    return TT_SUCCESS;
}

tt_result_t __percent_decode(IN tt_blobex_t *bex,
                             IN tt_char_t *str,
                             IN tt_u32_t len)
{
    tt_char_t *p, *end = str + len, *dst;
    tt_u32_t n = 0;

    p = str;
    while (p < end) {
        if ((*p == '%') && ((p + 2) < end) && tt_isxdigit(p[1]) &&
            tt_isxdigit(p[2])) {
            p += 3;
        } else {
            ++p;
        }
        ++n;
    }
    TT_ASSERT(p == end);
    // reserve space
    TT_DO(tt_blobex_set(bex, NULL, n + 1, TT_TRUE));

    p = str;
    dst = tt_blobex_addr(bex);
    while (p < end) {
        if ((*p == '%') && ((p + 2) < end) && tt_isxdigit(p[1]) &&
            tt_isxdigit(p[2])) {
            *dst++ = (tt_c2h(p[1], 0) << 4) | tt_c2h(p[2], 0);
            p += 3;
        } else {
            *dst++ = *p;
            ++p;
        }
    }
    *dst++ = 0;
    TT_ASSERT(dst == ((tt_char_t *)tt_blobex_addr(bex) + tt_blobex_len(bex)));

    return TT_SUCCESS;
}

tt_result_t __percent_encode(IN tt_blobex_t *bex,
                             IN tt_char_t *str,
                             IN tt_u32_t len,
                             IN OPT tt_char_t *enc_tbl)
{
    static tt_char_t c2s[] = "0123456789abcdef";
    tt_u32_t i, n = 0;
    tt_char_t *dst;

    for (i = 0; i < len; ++i) {
        // if NULL, do percent encoding: "%xx" otherwise just 1 char
        n += TT_COND(enc_tbl[(tt_u8_t)str[i]] != 0, 1, 3);
    }
    // reserve space
    TT_DO(tt_blobex_set(bex, NULL, n + 1, TT_TRUE));

    dst = tt_blobex_addr(bex);
    for (i = 0; i < len; ++i) {
        // if NULL, do percent encoding: "%xx" otherwise just 1 char
        tt_char_t c = str[i];
        if (enc_tbl[(tt_u8_t)c] != 0) {
            *dst++ = c;
        } else {
            *dst++ = '%';
            *dst++ = c2s[c >> 4];
            *dst++ = c2s[c & 0xF];
        }
    }
    *dst++ = 0;
    TT_ASSERT(dst == ((tt_char_t *)tt_blobex_addr(bex) + tt_blobex_len(bex)));

    return TT_SUCCESS;
}
