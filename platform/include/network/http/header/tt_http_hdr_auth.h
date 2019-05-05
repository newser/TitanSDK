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
#include <crypto/tt_message_digest.h>
#include <network/http/tt_http_header.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_HTTP_QOP_AUTH (1 << 0)
#define TT_HTTP_QOP_AUTH_INT (1 << 1)

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef enum
{
    TT_HTTP_AUTH_BASIC,
    TT_HTTP_AUTH_DIGEST,

    TT_HTTP_AUTH_SCHEME_NUM
} tt_http_auth_scheme_t;
#define TT_HTTP_AUTH_SCHEME_VALID(a) ((a) < TT_HTTP_AUTH_SCHEME_NUM)

typedef enum
{
    TT_HTTP_AUTH_MD5,
    TT_HTTP_AUTH_MD5_SESS,

    TT_HTTP_AUTH_ALG_NUM
} tt_http_auth_alg_t;
#define TT_HTTP_AUTH_ALG_VALID(a) ((a) < TT_HTTP_AUTH_ALG_NUM)

typedef enum
{
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
    tt_blobex_t raw_qop;
    tt_u8_t qop_mask;
    tt_http_auth_scheme_t scheme : 2;
    tt_http_stale_t stale : 2;
    tt_http_auth_alg_t alg : 4;
} tt_http_auth_t;

typedef struct
{
    tt_md_type_t type;
    tt_u32_t nonce_len;
    tt_md_t md;
    tt_char_t nonce[40]; // can be 24b, 40b is for ut(fixed nonce)
} tt_http_auth_ctx_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_http_hdr_t *tt_http_hdr_www_auth_create();

tt_export tt_http_hdr_t *tt_http_hdr_auth_create();

tt_export tt_http_hdr_t *tt_http_hdr_proxy_authenticate_create();

tt_export tt_http_hdr_t *tt_http_hdr_proxy_authorization_create();

tt_export tt_http_auth_t *tt_http_hdr_auth_get(IN tt_http_hdr_t *h);

// if @ auth is not a temporay, e.g. on stack, caller can set @shallow_copy to
// TT_TRUE and then there won't be new memory allocated
tt_export tt_result_t tt_http_hdr_auth_set(IN tt_http_hdr_t *h,
                                           IN tt_http_auth_t *auth,
                                           IN tt_bool_t shallow_copy);

// ========================================
// http auth
// ========================================

tt_export void tt_http_auth_init(IN tt_http_auth_t *ha);

tt_export void tt_http_auth_destroy(IN tt_http_auth_t *ha);

tt_export tt_bool_t tt_http_auth_valid(IN tt_http_auth_t *ha);

// the @dst simply reference memory of @src
tt_export void tt_http_auth_shallow_copy(IN tt_http_auth_t *dst,
                                         IN tt_http_auth_t *src);

tt_export tt_result_t tt_http_auth_smart_copy(IN tt_http_auth_t *dst,
                                              IN tt_http_auth_t *src);

// ========================================
// http auth context
// ========================================

tt_export void tt_http_auth_ctx_init(IN tt_http_auth_ctx_t *ctx);

tt_export void tt_http_auth_ctx_destroy(IN tt_http_auth_ctx_t *ctx);

tt_export void tt_http_auth_ctx_new_nonce(IN tt_http_auth_ctx_t *ctx);

tt_export tt_u32_t tt_http_auth_ctx_digest_len(IN tt_http_auth_ctx_t *ctx);

// - qop can be specfied, although ha has raw_qop and qop_mask. set qop
//   to 0 if wanna calc resp as qop param is not provided
// - response must have enough size, e.g., md5 require 16bytes,
//   sha512 require 64bytes
tt_export tt_result_t tt_http_auth_ctx_calc(
    IN tt_http_auth_ctx_t *ctx, IN tt_http_auth_t *ha, IN void *password,
    IN tt_u32_t password_len, IN tt_u32_t qop, IN void *method,
    IN tt_u32_t method_len, IN OPT void *body, IN tt_u32_t body_len,
    OUT tt_char_t *response);

#endif /* __TT_HTTP_HDR_AUTH__ */
