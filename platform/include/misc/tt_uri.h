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
*/

#ifndef __TT_URI__
#define __TT_URI__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_blobex.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct tt_uri_s
{
    tt_blobex_t scheme;
    tt_blobex_t specific;
    tt_blobex_t user_info;
    tt_blobex_t authority;
    tt_blobex_t host;
    tt_blobex_t path;
    tt_blobex_t query;
    tt_blobex_t fragment;
    tt_u16_t port;
} tt_uri_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export void tt_uri_init(IN tt_uri_t *uri);

tt_export tt_result_t tt_uri_create(IN tt_uri_t *uri,
                                    IN tt_char_t *str,
                                    IN tt_u32_t len);

tt_export void tt_uri_destroy(IN tt_uri_t *uri);

tt_export void tt_uri_clear(IN tt_uri_t *uri);

tt_export tt_result_t tt_uri_set(IN tt_uri_t *uri,
                                 IN tt_char_t *str,
                                 IN tt_u32_t len);

tt_export const tt_char_t *tt_uri_get_scheme(IN tt_uri_t *uri);

tt_export tt_result_t tt_uri_set_scheme(IN tt_uri_t *uri,
                                        IN const tt_char_t *scheme,
                                        IN tt_u32_t len);

tt_inline tt_result_t tt_uri_set_scheme_cstr(IN tt_uri_t *uri,
                                             IN const tt_char_t *scheme)
{
    return tt_uri_set_scheme(uri, scheme, (tt_u32_t)tt_strlen(scheme));
}

tt_export const tt_char_t *tt_uri_get_specific(IN tt_uri_t *uri);

tt_export tt_result_t tt_uri_set_specific(IN tt_uri_t *uri,
                                          IN const tt_char_t *specific,
                                          IN tt_u32_t len);

tt_inline tt_result_t tt_uri_set_specific_cstr(IN tt_uri_t *uri,
                                               IN const tt_char_t *specific)
{
    return tt_uri_set_specific(uri, specific, (tt_u32_t)tt_strlen(specific));
}

tt_export const tt_char_t *tt_uri_get_userinfo(IN tt_uri_t *uri);

tt_export tt_result_t tt_uri_set_userinfo(IN tt_uri_t *uri,
                                          IN const tt_char_t *userinfo,
                                          IN tt_u32_t len);

tt_inline tt_result_t tt_uri_set_userinfo_cstr(IN tt_uri_t *uri,
                                               IN const tt_char_t *userinfo)
{
    return tt_uri_set_userinfo(uri, userinfo, (tt_u32_t)tt_strlen(userinfo));
}

tt_export const tt_char_t *tt_uri_get_authority(IN tt_uri_t *uri);

tt_export tt_result_t tt_uri_set_authority(IN tt_uri_t *uri,
                                           IN const tt_char_t *authority,
                                           IN tt_u32_t len);

tt_inline tt_result_t tt_uri_set_authority_cstr(IN tt_uri_t *uri,
                                                IN const tt_char_t *authority)
{
    return tt_uri_set_authority(uri, authority, (tt_u32_t)tt_strlen(authority));
}

tt_export const tt_char_t *tt_uri_get_host(IN tt_uri_t *uri);

tt_export tt_result_t tt_uri_set_host(IN tt_uri_t *uri,
                                      IN const tt_char_t *host,
                                      IN tt_u32_t len);

tt_inline tt_result_t tt_uri_set_host_cstr(IN tt_uri_t *uri,
                                           IN const tt_char_t *host)
{
    return tt_uri_set_host(uri, host, (tt_u32_t)tt_strlen(host));
}

tt_export const tt_char_t *tt_uri_get_path(IN tt_uri_t *uri);

tt_export tt_result_t tt_uri_set_path(IN tt_uri_t *uri,
                                      IN const tt_char_t *path,
                                      IN tt_u32_t len);

tt_inline tt_result_t tt_uri_set_path_cstr(IN tt_uri_t *uri,
                                           IN const tt_char_t *path)
{
    return tt_uri_set_path(uri, path, (tt_u32_t)tt_strlen(path));
}

tt_export const tt_char_t *tt_uri_get_query(IN tt_uri_t *uri);

tt_export tt_result_t tt_uri_set_query(IN tt_uri_t *uri,
                                       IN const tt_char_t *query,
                                       IN tt_u32_t len);

tt_inline tt_result_t tt_uri_set_query_cstr(IN tt_uri_t *uri,
                                            IN const tt_char_t *query)
{
    return tt_uri_set_query(uri, query, (tt_u32_t)tt_strlen(query));
}

tt_export const tt_char_t *tt_uri_get_fragment(IN tt_uri_t *uri);

tt_export tt_result_t tt_uri_set_fragment(IN tt_uri_t *uri,
                                          IN const tt_char_t *fragment,
                                          IN tt_u32_t len);

tt_inline tt_result_t tt_uri_set_fragment_cstr(IN tt_uri_t *uri,
                                               IN const tt_char_t *fragment)
{
    return tt_uri_set_fragment(uri, fragment, (tt_u32_t)tt_strlen(fragment));
}

tt_inline tt_u16_t tt_uri_get_port(IN tt_uri_t *uri)
{
    return uri->port;
}

tt_inline void tt_uri_set_port(IN tt_uri_t *uri, IN tt_u16_t port)
{
    uri->port = port;
}

#endif /* __TT_URI__ */
