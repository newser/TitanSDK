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

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_uri_init(IN tt_uri_t *uri)
{
    tt_blobex_init(&uri->scheme, NULL, 0);
    tt_blobex_init(&uri->specific, NULL, 0);
    tt_blobex_init(&uri->user_info, NULL, 0);
    tt_blobex_init(&uri->authority, NULL, 0);
    tt_blobex_init(&uri->host, NULL, 0);
    tt_blobex_init(&uri->path, NULL, 0);
    tt_blobex_init(&uri->query, NULL, 0);
    tt_blobex_init(&uri->fragment, NULL, 0);
    uri->port = 0;
}

tt_result_t tt_uri_create(IN tt_uri_t *uri, IN tt_char_t *str, IN tt_u32_t len)
{
    tt_uri_init(uri);
    return TT_COND(len > 0, __parse_uri(uri, str, len), TT_SUCCESS);
}

void tt_uri_destroy(IN tt_uri_t *uri)
{
    tt_blobex_destroy(&uri->scheme);
    tt_blobex_destroy(&uri->specific);
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

tt_result_t __parse_uri(IN tt_uri_t *uri, IN tt_char_t *str, IN tt_u32_t len)
{
    tt_char_t *cur = str, *end = str + len, *scheme, *fragment;
    tt_u32_t n;

    // [scheme:]scheme-specific-part[#fragment]

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
                       __percent_decode(&uri->specific, cur, len),
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

const tt_char_t *tt_uri_get_scheme(IN tt_uri_t *uri)
{
    const tt_char_t *p = (const tt_char_t *)tt_blobex_addr(&uri->scheme);
    return TT_COND(p != NULL, p, "");
}

tt_result_t tt_uri_set_scheme(IN tt_uri_t *uri,
                              IN const tt_char_t *scheme,
                              IN tt_u32_t len)
{
    return TT_SUCCESS;
}

const tt_char_t *tt_uri_get_specific(IN tt_uri_t *uri)
{
    const tt_char_t *p = (const tt_char_t *)tt_blobex_addr(&uri->specific);
    return TT_COND(p != NULL, p, "");
}

tt_result_t tt_uri_set_specific(IN tt_uri_t *uri,
                                IN const tt_char_t *specific,
                                IN tt_u32_t len)
{
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
    return TT_SUCCESS;
}

const tt_char_t *tt_uri_get_authority(IN tt_uri_t *uri)
{
    const tt_char_t *p = (const tt_char_t *)tt_blobex_addr(&uri->authority);
    return TT_COND(p != NULL, p, "");
}

tt_result_t tt_uri_set_authority(IN tt_uri_t *uri,
                                 IN const tt_char_t *authority,
                                 IN tt_u32_t len)
{
    return TT_SUCCESS;
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
    return TT_SUCCESS;
}
