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
@file tt_http_hdr_cookie.h
 @brief http header: cookie

this file defines http cookie header
*/

#ifndef __TT_HTTP_HDR_COOKIE__
#define __TT_HTTP_HDR_COOKIE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_blobex.h>
#include <algorithm/tt_double_linked_list.h>
#include <network/http/tt_http_header.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct tt_http_cookie_s
{
    tt_dnode_t node;
    tt_blobex_t name;
    tt_blobex_t val;
    tt_blobex_t expires;
    tt_u32_t max_age;
    tt_bool_t secure : 1;
    tt_bool_t httponly : 1;
    tt_blobex_t domain;
    tt_blobex_t path;
    tt_blobex_t extension;
} tt_http_cookie_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_http_hdr_t *tt_http_hdr_cookie_create();

tt_export tt_http_hdr_t *tt_http_hdr_set_cookie_create();

// return null if no matching name, return an empty blobex if no value
tt_export tt_http_cookie_t *tt_http_hdr_find_cookie_n(IN tt_http_hdr_t *h,
                                                      IN const tt_char_t *name,
                                                      IN tt_u32_t name_len);

tt_inline tt_http_cookie_t *tt_http_hdr_find_cookie(IN tt_http_hdr_t *h,
                                                    IN const tt_char_t *name)
{
    return tt_http_hdr_find_cookie_n(h, name, (tt_u32_t)tt_strlen(name));
}

tt_export void tt_http_hdr_add_cookie(IN tt_http_hdr_t *h,
                                      IN TO tt_http_cookie_t *c);

// ========================================
// http cookie api
// ========================================

tt_export tt_http_cookie_t *tt_http_cookie_create();

tt_export void tt_http_cookie_destroy(IN tt_http_cookie_t *c);

tt_export void tt_http_cookie_clear(IN tt_http_cookie_t *c);

tt_export tt_http_cookie_t *tt_http_cookie_head(IN tt_http_hdr_t *h);

tt_export tt_http_cookie_t *tt_http_cookie_next(IN tt_http_cookie_t *c);

#endif /* __TT_HTTP_HDR_COOKIE__ */
