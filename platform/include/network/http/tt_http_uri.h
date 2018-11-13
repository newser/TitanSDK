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

/**
@file tt_http_uri.h
@brief http uri API
 */

#ifndef __TT_HTTP_URI__
#define __TT_HTTP_URI__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_queue.h>
#include <io/tt_fpath.h>
#include <misc/tt_kv.h>
#include <misc/tt_uri.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct
{
    tt_queue_iter_t iter;
} tt_http_uri_param_iter_t;

typedef enum {
    TT_HTTP_SCHEME_UNDEFINED,
    TT_HTTP_SCHEME_HTTP,
    TT_HTTP_SCHEME_HTTPS,

    TT_HTTP_SCHEME_NUM
} tt_http_scheme_t;
#define TT_HTTP_SCHEME_VALID(s) ((s) < TT_HTTP_SCHEME_NUM)

typedef struct tt_http_uri_s
{
    tt_queue_t path_param;
    tt_queue_t query_param;
    tt_uri_t u;
    tt_fpath_t path;
    tt_http_scheme_t scheme;
    tt_bool_t path_modified : 1;
    tt_bool_t query_modified : 1;
} tt_http_uri_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export void tt_http_uri_init(IN tt_http_uri_t *hu);

tt_export tt_result_t tt_http_uri_create(IN tt_http_uri_t *hu,
                                         IN tt_char_t *str,
                                         IN tt_u32_t len);

tt_inline tt_result_t tt_http_uri_create_cstr(IN tt_http_uri_t *hu,
                                              IN tt_char_t *str)
{
    return tt_http_uri_create(hu, str, tt_strlen(str));
}

tt_export void tt_http_uri_destroy(IN tt_http_uri_t *hu);

tt_export void tt_http_uri_clear(IN tt_http_uri_t *hu);

tt_export tt_result_t tt_http_uri_set(IN tt_http_uri_t *hu,
                                      IN tt_char_t *str,
                                      IN tt_u32_t len);

tt_inline tt_result_t tt_http_uri_set_cstr(IN tt_http_uri_t *hu,
                                           IN tt_char_t *str)
{
    return tt_http_uri_set(hu, str, tt_strlen(str));
}

tt_export const tt_char_t *tt_http_uri_encode(IN tt_http_uri_t *hu);

tt_export tt_result_t tt_http_uri_encode2buf(IN tt_http_uri_t *hu,
                                             IN struct tt_buf_s *buf);

tt_inline tt_s32_t tt_http_uri_cmp(IN tt_http_uri_t *a, IN tt_http_uri_t *b)
{
    return tt_uri_cmp(&a->u, &b->u);
}

tt_inline tt_http_scheme_t tt_http_uri_get_scheme(IN tt_http_uri_t *hu)
{
    TT_ASSERT(TT_HTTP_SCHEME_VALID(hu->scheme));
    return hu->scheme;
}

tt_export tt_result_t tt_http_uri_set_scheme(IN tt_http_uri_t *hu,
                                             IN tt_http_scheme_t scheme);

tt_inline const tt_char_t *tt_http_uri_get_userinfo(IN tt_http_uri_t *hu)
{
    return tt_uri_get_userinfo(&hu->u);
}

tt_inline tt_result_t tt_http_uri_set_userinfo(IN tt_http_uri_t *hu,
                                               IN const tt_char_t *userinfo,
                                               IN tt_u32_t len)
{
    return tt_uri_set_userinfo(&hu->u, userinfo, len);
}

tt_inline tt_result_t tt_http_uri_set_userinfo_cstr(
    IN tt_http_uri_t *hu, IN const tt_char_t *userinfo)
{
    return tt_http_uri_set_userinfo(hu,
                                    userinfo,
                                    (tt_u32_t)tt_strlen(userinfo));
}

tt_inline const tt_char_t *tt_http_uri_get_authority(IN tt_http_uri_t *hu)
{
    return tt_uri_get_authority(&hu->u);
}

tt_inline tt_result_t tt_http_uri_decode_authority(
    IN tt_http_uri_t *hu, IN const tt_char_t *authority, IN tt_u32_t len)
{
    return tt_uri_decode_authority(&hu->u, authority, len);
}

tt_inline tt_result_t tt_http_uri_decode_authority_cstr(
    IN tt_http_uri_t *hu, IN const tt_char_t *authority)
{
    return tt_http_uri_decode_authority(hu,
                                        authority,
                                        (tt_u32_t)tt_strlen(authority));
}

tt_inline const tt_char_t *tt_http_uri_get_host(IN tt_http_uri_t *hu)
{
    return tt_uri_get_host(&hu->u);
}

tt_inline tt_result_t tt_http_uri_set_host(IN tt_http_uri_t *hu,
                                           IN const tt_char_t *host,
                                           IN tt_u32_t len)
{
    return tt_uri_set_host(&hu->u, host, len);
}

tt_inline tt_result_t tt_http_uri_set_host_cstr(IN tt_http_uri_t *hu,
                                                IN const tt_char_t *host)
{
    return tt_http_uri_set_host(hu, host, (tt_u32_t)tt_strlen(host));
}

tt_inline tt_fpath_t *tt_http_uri_get_path(IN tt_http_uri_t *hu)
{
    return &hu->path;
}

tt_inline void tt_http_uri_path_param_iter(IN tt_http_uri_t *hu,
                                           OUT tt_http_uri_param_iter_t *iter)
{
    tt_queue_iter(&hu->path_param, &iter->iter);
}

tt_export tt_kv_t *tt_http_uri_find_path_param(IN tt_http_uri_t *hu,
                                               IN const tt_char_t *name);

tt_export tt_result_t tt_http_uri_add_path_param(IN tt_http_uri_t *hu,
                                                 IN const tt_char_t *name);

tt_export tt_result_t tt_http_uri_add_path_param_nv(IN tt_http_uri_t *hu,
                                                    IN const tt_char_t *name,
                                                    IN const tt_char_t *value);

tt_export tt_bool_t tt_http_uri_remove_path_param(IN tt_http_uri_t *hu,
                                                  IN const tt_char_t *name);

tt_export tt_bool_t tt_http_uri_remove_path_param_nv(IN tt_http_uri_t *hu,
                                                     IN const tt_char_t *name,
                                                     IN const tt_char_t *value);

tt_inline void tt_http_uri_query_param_iter(IN tt_http_uri_t *hu,
                                            OUT tt_http_uri_param_iter_t *iter)
{
    tt_queue_iter(&hu->query_param, &iter->iter);
}

tt_export tt_kv_t *tt_http_uri_find_query_param(IN tt_http_uri_t *hu,
                                                IN const tt_char_t *name);

tt_export tt_result_t tt_http_uri_add_query_param(IN tt_http_uri_t *hu,
                                                  IN const tt_char_t *name);

tt_export tt_result_t tt_http_uri_add_query_param_nv(IN tt_http_uri_t *hu,
                                                     IN const tt_char_t *name,
                                                     IN const tt_char_t *value);

tt_export tt_bool_t tt_http_uri_remove_query_param(IN tt_http_uri_t *hu,
                                                   IN const tt_char_t *name);

tt_export tt_bool_t tt_http_uri_remove_query_param_nv(
    IN tt_http_uri_t *hu, IN const tt_char_t *name, IN const tt_char_t *value);

tt_inline const tt_char_t *tt_http_uri_get_fragment(IN tt_http_uri_t *hu)
{
    return tt_uri_get_fragment(&hu->u);
}

tt_inline tt_result_t tt_http_uri_set_fragment(IN tt_http_uri_t *hu,
                                               IN const tt_char_t *fragment,
                                               IN tt_u32_t len)
{
    return tt_uri_set_fragment(&hu->u, fragment, len);
}

tt_inline tt_result_t tt_http_uri_set_fragment_cstr(
    IN tt_http_uri_t *hu, IN const tt_char_t *fragment)
{
    return tt_http_uri_set_fragment(hu,
                                    fragment,
                                    (tt_u32_t)tt_strlen(fragment));
}

tt_inline tt_u16_t tt_http_uri_get_port(IN tt_http_uri_t *hu)
{
    return tt_uri_get_port(&hu->u);
}

tt_inline void tt_http_uri_set_port(IN tt_http_uri_t *hu, IN tt_u16_t port)
{
    return tt_uri_set_port(&hu->u, port);
}

tt_inline tt_bool_t tt_http_uri_is_absolute(IN tt_http_uri_t *hu)
{
    return tt_uri_is_absolute(&hu->u);
}

tt_inline tt_bool_t tt_http_uri_is_opaque(IN tt_http_uri_t *hu)
{
    return tt_uri_is_opaque(&hu->u);
}

// ========================================
// http uri param
// ========================================

tt_export tt_kv_t *tt_http_uri_param_next(IN tt_http_uri_param_iter_t *iter);

#endif /* __TT_HTTP_URI__ */
