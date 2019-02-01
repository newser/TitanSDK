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

 - http uri only itself hold path_param, query_param and scheme, other are
 all stored in tt_http_uri_t::u
 - any time changing scheme will also update tt_http_uri_t::u::scheme
 - after parsing, path_param does not allocate memory but only reference memory
 stored in tt_http_uri_t::u, adding new param will alloc new mem, removing param
 only mark param removed. each time rendering, if path or path_param is changed
 tt_http_uri_t will first OWN all param and then start rendering, otherwise
 tt_http_uri_t::u::path is returned as nothing is changed
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

struct tt_http_inserv_s;

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

tt_inline tt_string_t *tt_http_uri_str(IN tt_http_uri_t *hu)
{
    return &hu->u.uri;
}

// ========================================
// parse
// ========================================

tt_export tt_result_t tt_http_uri_parse_n(IN tt_http_uri_t *hu,
                                          IN tt_char_t *str,
                                          IN tt_u32_t len);

tt_inline tt_result_t tt_http_uri_parse(IN tt_http_uri_t *hu, IN tt_char_t *str)
{
    return tt_http_uri_parse_n(hu, str, tt_strlen(str));
}

// ========================================
// render
// ========================================

tt_export const tt_char_t *tt_http_uri_render(IN tt_http_uri_t *hu);

tt_export tt_result_t tt_http_uri_render2buf(IN tt_http_uri_t *hu,
                                             IN struct tt_buf_s *buf);

// ========================================
// scheme
// ========================================

tt_export tt_http_scheme_t tt_http_uri_get_scheme(IN tt_http_uri_t *hu);

tt_export tt_result_t tt_http_uri_set_scheme(IN tt_http_uri_t *hu,
                                             IN tt_http_scheme_t scheme);

// ========================================
// user info
// ========================================

tt_inline const tt_char_t *tt_http_uri_get_userinfo(IN tt_http_uri_t *hu)
{
    return tt_uri_get_userinfo(&hu->u);
}

tt_inline tt_result_t tt_http_uri_set_userinfo_n(IN tt_http_uri_t *hu,
                                                 IN const tt_char_t *userinfo,
                                                 IN tt_u32_t len)
{
    return tt_uri_set_userinfo_n(&hu->u, userinfo, len);
}

tt_inline tt_result_t tt_http_uri_set_userinfo(IN tt_http_uri_t *hu,
                                               IN const tt_char_t *userinfo)
{
    return tt_http_uri_set_userinfo_n(hu,
                                      userinfo,
                                      (tt_u32_t)tt_strlen(userinfo));
}

// ========================================
// authority
// ========================================

tt_inline const tt_char_t *tt_http_uri_get_authority(IN tt_http_uri_t *hu)
{
    return tt_uri_get_authority(&hu->u);
}

tt_inline tt_result_t tt_http_uri_parse_authority_n(
    IN tt_http_uri_t *hu, IN const tt_char_t *authority, IN tt_u32_t len)
{
    return tt_uri_parse_authority_n(&hu->u, authority, len);
}

tt_inline tt_result_t tt_http_uri_parse_authority(IN tt_http_uri_t *hu,
                                                  IN const tt_char_t *authority)
{
    return tt_http_uri_parse_authority_n(hu,
                                         authority,
                                         (tt_u32_t)tt_strlen(authority));
}

// ========================================
// host
// ========================================

tt_inline const tt_char_t *tt_http_uri_get_host(IN tt_http_uri_t *hu)
{
    return tt_uri_get_host(&hu->u);
}

tt_inline tt_result_t tt_http_uri_set_host_n(IN tt_http_uri_t *hu,
                                             IN const tt_char_t *host,
                                             IN tt_u32_t len)
{
    return tt_uri_set_host_n(&hu->u, host, len);
}

tt_inline tt_result_t tt_http_uri_set_host(IN tt_http_uri_t *hu,
                                           IN const tt_char_t *host)
{
    return tt_http_uri_set_host_n(hu, host, (tt_u32_t)tt_strlen(host));
}

// ========================================
// path
// ========================================

tt_inline tt_fpath_t *tt_http_uri_get_path(IN tt_http_uri_t *hu)
{
    return &hu->path;
}

// this function must be called if someone changed tt_fpath_t returned by
// tt_http_uri_get_path()
tt_inline void tt_http_uri_update_path(IN tt_http_uri_t *hu)
{
    hu->path_modified = TT_TRUE;
}

tt_inline tt_bool_t tt_http_uri_pparam_empty(IN tt_http_uri_t *hu)
{
    return tt_queue_empty(&hu->path_param);
}

tt_inline void tt_http_uri_pparam_iter(IN tt_http_uri_t *hu,
                                       OUT tt_http_uri_param_iter_t *iter)
{
    tt_queue_iter(&hu->path_param, &iter->iter);
}

tt_export tt_kv_t *tt_http_uri_find_pparam(IN tt_http_uri_t *hu,
                                           IN const tt_char_t *name);

tt_export tt_result_t tt_http_uri_add_pparam(IN tt_http_uri_t *hu,
                                             IN const tt_char_t *name);

tt_export tt_result_t tt_http_uri_add_pparam_nv(IN tt_http_uri_t *hu,
                                                IN const tt_char_t *name,
                                                IN const tt_char_t *value);

tt_export tt_bool_t tt_http_uri_remove_pparam(IN tt_http_uri_t *hu,
                                              IN const tt_char_t *name);

tt_export tt_bool_t tt_http_uri_remove_pparam_nv(IN tt_http_uri_t *hu,
                                                 IN const tt_char_t *name,
                                                 IN const tt_char_t *value);

// ========================================
// query
// ========================================

tt_inline tt_bool_t tt_http_uri_qparam_empty(IN tt_http_uri_t *hu)
{
    return tt_queue_empty(&hu->query_param);
}

tt_inline void tt_http_uri_qparam_iter(IN tt_http_uri_t *hu,
                                       OUT tt_http_uri_param_iter_t *iter)
{
    tt_queue_iter(&hu->query_param, &iter->iter);
}

tt_export tt_kv_t *tt_http_uri_find_qparam(IN tt_http_uri_t *hu,
                                           IN const tt_char_t *name);

tt_export tt_result_t tt_http_uri_add_qparam(IN tt_http_uri_t *hu,
                                             IN const tt_char_t *name);

tt_export tt_result_t tt_http_uri_add_qparam_nv(IN tt_http_uri_t *hu,
                                                IN const tt_char_t *name,
                                                IN const tt_char_t *value);

tt_export tt_bool_t tt_http_uri_remove_qparam(IN tt_http_uri_t *hu,
                                              IN const tt_char_t *name);

tt_export tt_bool_t tt_http_uri_remove_qparam_nv(IN tt_http_uri_t *hu,
                                                 IN const tt_char_t *name,
                                                 IN const tt_char_t *value);

// ========================================
// fragment
// ========================================

tt_inline const tt_char_t *tt_http_uri_get_fragment(IN tt_http_uri_t *hu)
{
    return tt_uri_get_fragment(&hu->u);
}

tt_inline tt_result_t tt_http_uri_set_fragment_n(IN tt_http_uri_t *hu,
                                                 IN const tt_char_t *fragment,
                                                 IN tt_u32_t len)
{
    return tt_uri_set_fragment_n(&hu->u, fragment, len);
}

tt_inline tt_result_t tt_http_uri_set_fragment(IN tt_http_uri_t *hu,
                                               IN const tt_char_t *fragment)
{
    return tt_http_uri_set_fragment_n(hu,
                                      fragment,
                                      (tt_u32_t)tt_strlen(fragment));
}

// ========================================
// port
// ========================================

tt_inline tt_u16_t tt_http_uri_get_port(IN tt_http_uri_t *hu)
{
    return tt_uri_get_port(&hu->u);
}

tt_inline void tt_http_uri_set_port(IN tt_http_uri_t *hu, IN tt_u16_t port)
{
    tt_uri_set_port(&hu->u, port);
}

// ========================================
// common
// ========================================

tt_inline tt_s32_t tt_http_uri_cmp(IN tt_http_uri_t *a, IN tt_http_uri_t *b)
{
    return tt_uri_cmp(&a->u, &b->u);
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
