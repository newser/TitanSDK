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
@file tt_http_def.h
@brief http def

this file defines http
*/

#ifndef __TT_HTTP_DEF__
#define __TT_HTTP_DEF__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

#include <http_parser.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_HTTP_HDR_MAP(__ENTRY)                                               \
    __ENTRY(HOST, "Host")                                                      \
    __ENTRY(TXENC, "Transfer-Encoding")                                        \
    __ENTRY(CONTENC, "Content-Encoding")                                       \
    __ENTRY(ACCENC, "Accept-Encoding")                                         \
    __ENTRY(DATE, "Date")

#define TT_HTTP_TXENC_MAP(__ENTRY)                                             \
    __ENTRY(CHUNKED, "chunked")                                                \
    __ENTRY(COMPRESS, "compress")                                              \
    __ENTRY(DEFLATE, "deflate")                                                \
    __ENTRY(GZIP, "gzip")                                                      \
    __ENTRY(IDENTITY, "identity")

#define TT_HTTP_ENC_MAP(__ENTRY)                                               \
    __ENTRY(COMPRESS, "compress")                                              \
    __ENTRY(DEFLATE, "deflate")                                                \
    __ENTRY(BR, "br")                                                          \
    __ENTRY(GZIP, "gzip")                                                      \
    __ENTRY(IDENTITY, "identity")

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef enum {
#define __ENTRY(id, str) TT_HTTP_HDR_##id,
    TT_HTTP_HDR_MAP(__ENTRY)
#undef __ENTRY

        TT_HTTP_HNAME_NUM
} tt_http_hname_t;
#define TT_HTTP_HNAME_VALID(h) ((h) < TT_HTTP_HNAME_NUM)

typedef enum {
#define XX(num, name, string) TT_HTTP_MTD_##name,
    HTTP_METHOD_MAP(XX)
#undef XX

        TT_HTTP_METHOD_NUM
} tt_http_method_t;
#define TT_HTTP_METHOD_VALID(h) ((h) < TT_HTTP_METHOD_NUM)

typedef enum {
#define XX(num, name, string) TT_HTTP_STATUS_##name = num,
    HTTP_STATUS_MAP(XX)
#undef XX

        TT_HTTP_STATUS_INVALID
} tt_http_status_t;
#define TT_HTTP_STATUS_VALID(s) ((s) != TT_HTTP_STATUS_INVALID)

typedef enum {
    TT_HTTP_V0_9,
    TT_HTTP_V1_0,
    TT_HTTP_V1_1,
    TT_HTTP_V2_0,

    TT_HTTP_VER_NUM,
} tt_http_ver_t;
#define TT_HTTP_VER_VALID(v) ((v) < TT_HTTP_VER_NUM)

typedef enum {
    TT_HTTP_SHUT_RD,
    TT_HTTP_SHUT_WR,
    TT_HTTP_SHUT_RDWR,

    TT_HTTP_SHUT_NUM
} tt_http_shut_t;
#define TT_HTTP_SHUT_VALID(s) ((s) < TT_HTTP_SHUT_NUM)

typedef enum {
    TT_HTTP_CONN_NONE,
    TT_HTTP_CONN_CLOSE,
    TT_HTTP_CONN_KEEP_ALIVE,

    TT_HTTP_CONN_NUM
} tt_http_conn_t;
#define TT_HTTP_CONN_VALID(c) ((c) < TT_HTTP_CONN_NUM)

typedef enum {
#define __ENTRY(id, str) TT_HTTP_TXENC_##id,
    TT_HTTP_TXENC_MAP(__ENTRY)
#undef __ENTRY

        TT_HTTP_TXENC_NUM
} tt_http_txenc_t;
#define TT_HTTP_TXENC_VALID(h) ((h) < TT_HTTP_TXENC_NUM)

typedef enum {
#define __ENTRY(id, str) TT_HTTP_ENC_##id,
    TT_HTTP_ENC_MAP(__ENTRY)
#undef __ENTRY

        TT_HTTP_ENC_NUM
} tt_http_enc_t;
#define TT_HTTP_ENC_VALID(h) ((h) < TT_HTTP_ENC_NUM)

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

tt_export const tt_char_t *tt_g_http_hname[TT_HTTP_HNAME_NUM];

tt_export tt_u32_t tt_g_http_hname_len[TT_HTTP_HNAME_NUM];

tt_export const tt_char_t *tt_g_http_method[TT_HTTP_METHOD_NUM];

tt_export tt_u32_t tt_g_http_method_len[TT_HTTP_METHOD_NUM];

tt_export const tt_char_t *tt_g_http_verion[TT_HTTP_VER_NUM];

tt_export tt_u32_t tt_g_http_verion_len[TT_HTTP_VER_NUM];

tt_export const tt_char_t *tt_g_http_conn[TT_HTTP_CONN_NUM];

tt_export tt_u32_t tt_g_http_conn_len[TT_HTTP_CONN_NUM];

tt_export const tt_char_t *tt_g_http_txenc[TT_HTTP_TXENC_NUM];

tt_export tt_u32_t tt_g_http_txenc_len[TT_HTTP_TXENC_NUM];

tt_export const tt_char_t *tt_g_http_enc[TT_HTTP_ENC_NUM];

tt_export tt_u32_t tt_g_http_enc_len[TT_HTTP_ENC_NUM];

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export void tt_http_status_cstr(IN tt_http_status_t status,
                                   OUT const tt_char_t **s,
                                   OUT tt_u32_t *len);

#endif /* __TT_HTTP_DEF__ */
