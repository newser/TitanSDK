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
@file tt_http_hdr_auth.h
 @brief http header: auth

this file defines http auth headers
*/

#ifndef __TT_HTTP_HDR_AUTH__
#define __TT_HTTP_HDR_AUTH__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_blobex.h>
#include <network/http/tt_http_header.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef enum {
    TT_HTTP_AUTH_BASIC,
    TT_HTTP_AUTH_DIGEST,

    TT_HTTP_AUTH_SCHEME_NUM
} tt_http_auth_scheme_t;
#define TT_HTTP_AUTH_SCHEME_VALID(a) ((a) < TT_HTTP_AUTH_SCHEME_NUM)

typedef enum {
    TT_HTTP_AUTH_MD5,
    TT_HTTP_AUTH_MD5_SESS,

    TT_HTTP_AUTH_ALG_NUM
} tt_http_auth_alg_t;
#define TT_HTTP_AUTH_ALG_VALID(a) ((a) < TT_HTTP_AUTH_ALG_NUM)

typedef enum {
    TT_HTTP_QOP_AUTH,
    TT_HTTP_QOP_AUTH_INT,

    TT_HTTP_QOP_NUM
} tt_http_qop_t;
#define TT_HTTP_QOP_VALID(a) ((a) < TT_HTTP_QOP_NUM)

typedef enum {
    TT_HTTP_STALE_FALSE,
    TT_HTTP_STALE_TRUE,

    TT_HTTP_STALE_NUM
} tt_http_stale_t;
#define TT_HTTP_STALE_VALID(a) ((a) < TT_HTTP_STALE_NUM)

typedef struct tt_http_auth_s
{
    tt_blobex_t realm;
    tt_blobex_t domain;
    tt_blobex_t nonce;
    tt_blobex_t opaque;
    tt_blobex_t response;
    tt_blobex_t username;
    tt_blobex_t uri;
    tt_blobex_t cnonce;
    tt_blobex_t nc;
    tt_http_auth_scheme_t scheme : 2;
    tt_http_stale_t stale : 2;
    tt_http_auth_alg_t alg : 4;
    tt_http_qop_t qop : 2;
} tt_http_auth_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_http_hdr_t *tt_http_hdr_auth_create();

tt_export void tt_http_auth_init(IN tt_http_auth_t *ha);

tt_export tt_http_auth_t *tt_http_hdr_auth_get(IN tt_http_hdr_t *h);

tt_export void tt_http_hdr_auth_set(IN tt_http_hdr_t *h,
                                    IN TO tt_http_auth_t *auth);

#endif /* __TT_HTTP_HDR_AUTH__ */
