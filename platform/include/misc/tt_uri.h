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
@file tt_uri.h
@brief uri

this file defines uri APIs

 - tt_uri_t store each uri part as a string, including terminating 0
 - tt_uri_t::uri is only updated when calling tt_uri_render(), other set() only
 mark tt_uri_t::uri_modified
 - tt_uri_t::authority is only updated when calling tt_uri_get_authority()，
setting
 userinfo/host/port only mark tt_uri_t::auth_modified
 - application who want to render path(or other). can directly set
tt_uri_t::path
 and mark tt_uri_t::uri_modified
*/

#ifndef __TT_URI__
#define __TT_URI__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_blobex.h>
#include <algorithm/tt_string.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct tt_uri_s
{
    tt_string_t uri;
    tt_blobex_t scheme;
    tt_blobex_t opaque;
    tt_blobex_t user_info;
    tt_blobex_t authority;
    tt_blobex_t host;
    tt_blobex_t path;
    tt_blobex_t query;
    tt_blobex_t fragment;
    tt_u16_t port;
    tt_bool_t auth_modified : 1;
    tt_bool_t uri_modified : 1;
} tt_uri_t;

typedef struct
{
    tt_char_t *generic;
    tt_char_t *path;
    tt_char_t *query;
} tt_uri_encode_table_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

tt_export tt_char_t tt_g_uri_encode_table[256];

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export void tt_uri_init(IN tt_uri_t *uri);

tt_export tt_result_t tt_uri_create(IN tt_uri_t *uri, IN tt_char_t *str,
                                    IN tt_u32_t len);

tt_export void tt_uri_destroy(IN tt_uri_t *uri);

tt_export void tt_uri_clear(IN tt_uri_t *uri);

// ========================================
// parse
// ========================================

tt_export tt_result_t tt_uri_parse_n(IN tt_uri_t *uri, IN tt_char_t *str,
                                     IN tt_u32_t len);

tt_inline tt_result_t tt_uri_parse(IN tt_uri_t *uri, IN tt_char_t *str)
{
    return tt_uri_parse_n(uri, (tt_char_t *)str, (tt_u32_t)tt_strlen(str));
}

// ========================================
// render
// ========================================

tt_export void tt_uri_encode_table_default(IN tt_uri_encode_table_t *uet);

tt_export const tt_char_t *tt_uri_render(IN tt_uri_t *uri,
                                         IN OPT tt_uri_encode_table_t *uet);

tt_export tt_result_t tt_uri_render2buf(IN tt_uri_t *uri,
                                        IN struct tt_buf_s *buf,
                                        IN OPT tt_uri_encode_table_t *uet);

// ========================================
// scheme
// ========================================

tt_export const tt_char_t *tt_uri_get_scheme(IN tt_uri_t *uri);

tt_export tt_result_t tt_uri_set_scheme_n(IN tt_uri_t *uri,
                                          IN const tt_char_t *scheme,
                                          IN tt_u32_t len);

tt_inline tt_result_t tt_uri_set_scheme(IN tt_uri_t *uri,
                                        IN const tt_char_t *scheme)
{
    return tt_uri_set_scheme_n(uri, scheme, (tt_u32_t)tt_strlen(scheme));
}

// ========================================
// opaque
// ========================================

// return empty string if uri is hierarchical
tt_export const tt_char_t *tt_uri_get_opaque(IN tt_uri_t *uri);

// auth, usr, host, port, path, query will be cleared
tt_export tt_result_t tt_uri_set_opaque_n(IN tt_uri_t *uri,
                                          IN const tt_char_t *specific,
                                          IN tt_u32_t len);

tt_inline tt_result_t tt_uri_set_opaque(IN tt_uri_t *uri,
                                        IN const tt_char_t *specific)
{
    return tt_uri_set_opaque_n(uri, specific, (tt_u32_t)tt_strlen(specific));
}

// ========================================
// user info
// ========================================

tt_export const tt_char_t *tt_uri_get_userinfo(IN tt_uri_t *uri);

tt_export tt_result_t tt_uri_set_userinfo_n(IN tt_uri_t *uri,
                                            IN const tt_char_t *userinfo,
                                            IN tt_u32_t len);

tt_inline tt_result_t tt_uri_set_userinfo(IN tt_uri_t *uri,
                                          IN const tt_char_t *userinfo)
{
    return tt_uri_set_userinfo_n(uri, userinfo, (tt_u32_t)tt_strlen(userinfo));
}

// ========================================
// authority
// ========================================

tt_export const tt_char_t *tt_uri_get_authority(IN tt_uri_t *uri);

tt_export tt_result_t tt_uri_parse_authority_n(IN tt_uri_t *uri,
                                               IN const tt_char_t *authority,
                                               IN tt_u32_t len);

tt_inline tt_result_t tt_uri_parse_authority(IN tt_uri_t *uri,
                                             IN const tt_char_t *authority)
{
    return tt_uri_parse_authority_n(uri, authority,
                                    (tt_u32_t)tt_strlen(authority));
}

// ========================================
// host
// ========================================

tt_export const tt_char_t *tt_uri_get_host(IN tt_uri_t *uri);

tt_export tt_result_t tt_uri_set_host_n(IN tt_uri_t *uri,
                                        IN const tt_char_t *host,
                                        IN tt_u32_t len);

tt_inline tt_result_t tt_uri_set_host(IN tt_uri_t *uri,
                                      IN const tt_char_t *host)
{
    return tt_uri_set_host_n(uri, host, (tt_u32_t)tt_strlen(host));
}

// ========================================
// path
// ========================================

tt_export const tt_char_t *tt_uri_get_path(IN tt_uri_t *uri);

tt_export tt_result_t tt_uri_set_path_n(IN tt_uri_t *uri,
                                        IN const tt_char_t *path,
                                        IN tt_u32_t len);

tt_inline tt_result_t tt_uri_set_path(IN tt_uri_t *uri,
                                      IN const tt_char_t *path)
{
    return tt_uri_set_path_n(uri, path, (tt_u32_t)tt_strlen(path));
}

// ========================================
// query
// ========================================

tt_export const tt_char_t *tt_uri_get_query(IN tt_uri_t *uri);

tt_export tt_result_t tt_uri_set_query_n(IN tt_uri_t *uri,
                                         IN const tt_char_t *query,
                                         IN tt_u32_t len);

tt_inline tt_result_t tt_uri_set_query(IN tt_uri_t *uri,
                                       IN const tt_char_t *query)
{
    return tt_uri_set_query_n(uri, query, (tt_u32_t)tt_strlen(query));
}

// ========================================
// fragment
// ========================================

tt_export const tt_char_t *tt_uri_get_fragment(IN tt_uri_t *uri);

tt_export tt_result_t tt_uri_set_fragment_n(IN tt_uri_t *uri,
                                            IN const tt_char_t *fragment,
                                            IN tt_u32_t len);

tt_inline tt_result_t tt_uri_set_fragment(IN tt_uri_t *uri,
                                          IN const tt_char_t *fragment)
{
    return tt_uri_set_fragment_n(uri, fragment, (tt_u32_t)tt_strlen(fragment));
}

// ========================================
// port
// ========================================

tt_inline tt_u16_t tt_uri_get_port(IN tt_uri_t *uri)
{
    return uri->port;
}

tt_inline void tt_uri_set_port(IN tt_uri_t *uri, IN tt_u16_t port)
{
    uri->uri_modified = TT_TRUE;
    uri->auth_modified = TT_TRUE;
    uri->port = port;
}

// ========================================
// common
// ========================================

tt_export tt_s32_t tt_uri_cmp(IN tt_uri_t *a, IN tt_uri_t *b);

tt_inline tt_bool_t tt_uri_is_absolute(IN tt_uri_t *uri)
{
    return TT_BOOL(tt_uri_get_scheme(uri)[0] != 0);
}

tt_inline tt_bool_t tt_uri_is_opaque(IN tt_uri_t *uri)
{
    return TT_BOOL(tt_uri_get_opaque(uri)[0] != 0);
}

#endif /* __TT_URI__ */
