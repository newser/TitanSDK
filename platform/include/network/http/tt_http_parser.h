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
@file tt_http_parser.h
@brief http parser

this file defines http parser APIs
*/

#ifndef __TT_HTTP_PARSER__
#define __TT_HTTP_PARSER__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_buffer.h>
#include <algorithm/tt_double_linked_list.h>
#include <network/http/def/tt_http_def.h>
#include <network/http/header/tt_http_hdr_accept_encoding.h>
#include <network/http/tt_http_content_type_map.h>
#include <network/http/tt_http_uri.h>

#include <http_parser.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_http_sconn_s;
struct tt_http_rawhdr_s;
struct tt_http_rawval_s;
struct tt_slab_s;

typedef struct tt_http_parser_s
{
    struct tt_http_sconn_s *c;
    struct tt_http_rawhdr_s *rh;
    struct tt_http_rawval_s *rv;

    struct tt_slab_s *rawhdr_slab;
    struct tt_slab_s *rawval_slab;
    tt_dlist_t rawhdr;
    tt_dlist_t trailing_rawhdr;
    tt_blobex_t *host;
    tt_http_contype_map_t *contype_map;
    tt_dlist_t hdr;

    tt_buf_t buf;
    tt_blobex_t rawuri;
    tt_http_uri_t uri;
    // the body stores a partial of parsed data, always reference buf content
    tt_blobex_t body;

    http_parser parser;

    tt_u32_t body_counter;
    tt_http_contype_t contype;
    tt_s32_t content_len;
    tt_http_accenc_t accenc;
    tt_u8_t txenc_num;
    tt_u8_t txenc[TT_HTTP_TXENC_NUM];
    tt_u8_t contenc_num;
    tt_u8_t contenc[TT_HTTP_ENC_NUM];
    tt_bool_t complete_line1 : 1;
    tt_bool_t complete_header : 1;
    tt_bool_t complete_message : 1;
    tt_bool_t complete_trailing_header : 1;
    tt_bool_t updated_host : 1;
    tt_bool_t updated_uri : 1;
    tt_bool_t updated_contype : 1;
    tt_bool_t updated_content_len : 1;
    tt_bool_t updated_txenc : 1;
    tt_bool_t updated_contenc : 1;
    tt_bool_t updated_accenc : 1;
    tt_bool_t miss_txenc : 1;
    tt_bool_t miss_contype : 1;
    tt_bool_t miss_content_len : 1;
    tt_bool_t miss_contenc : 1;
} tt_http_parser_t;

typedef struct
{
    tt_http_contype_map_t *contype_map;
    tt_buf_attr_t buf_attr;
} tt_http_parser_attr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_result_t tt_http_parser_create(IN tt_http_parser_t *hp,
                                            IN struct tt_slab_s *rawhdr_slab,
                                            IN struct tt_slab_s *rawval_slab,
                                            IN OPT tt_http_parser_attr_t *attr);

tt_export void tt_http_parser_destroy(IN tt_http_parser_t *hp);

tt_export void tt_http_parser_attr_default(IN tt_http_parser_attr_t *attr);

tt_export void tt_http_parser_clear(IN tt_http_parser_t *hp,
                                    IN tt_bool_t clear_recv_buf);

tt_inline void tt_http_parser_wpos(IN tt_http_parser_t *hp,
                                   OUT tt_u8_t **addr,
                                   OUT tt_u32_t *len)
{
    *addr = TT_BUF_WPOS(&hp->buf);
    *len = TT_BUF_WLEN(&hp->buf);
}

tt_inline tt_u32_t tt_http_parser_rlen(IN tt_http_parser_t *hp)
{
    return TT_BUF_RLEN(&hp->buf);
}

tt_inline void tt_http_parser_inc_wp(IN tt_http_parser_t *hp, IN tt_u32_t len)
{
    tt_buf_inc_wp(&hp->buf, len);
}

tt_export tt_result_t tt_http_parser_run(IN tt_http_parser_t *hp);

tt_export tt_http_method_t tt_http_parser_get_method(IN tt_http_parser_t *hp);

tt_inline tt_blobex_t *tt_http_parser_get_rawuri(IN tt_http_parser_t *hp)
{
    return &hp->rawuri;
}

tt_export tt_http_uri_t *tt_http_parser_get_uri(IN tt_http_parser_t *hp);

tt_export tt_http_ver_t tt_http_parser_get_version(IN tt_http_parser_t *hp);

tt_export tt_http_status_t tt_http_parser_get_status(IN tt_http_parser_t *hp);

tt_inline tt_bool_t tt_http_parser_should_keepalive(IN tt_http_parser_t *hp)
{
    return TT_BOOL(http_should_keep_alive(&hp->parser) != 0);
}

// - return NULL if there is no Host
// - return a empty blobex if Host header is empty
tt_export tt_blobex_t *tt_http_parser_get_host(IN tt_http_parser_t *hp);

tt_export tt_http_hdr_t *tt_http_parser_find_hdr(IN tt_http_parser_t *hp,
                                                 IN tt_http_hname_t hname);

tt_export tt_http_contype_t tt_http_parser_get_contype(IN tt_http_parser_t *hp);

tt_export tt_s32_t tt_http_parser_get_content_len(IN tt_http_parser_t *hp);

tt_export tt_u32_t tt_http_parser_get_txenc(IN tt_http_parser_t *hp,
                                            OUT tt_http_txenc_t *txenc);

tt_export tt_u32_t tt_http_parser_get_contenc(IN tt_http_parser_t *hp,
                                              OUT tt_http_enc_t *contenc);

tt_export tt_http_accenc_t *tt_http_parser_get_accenc(IN tt_http_parser_t *hp);

// ========================================
// helper
// ========================================

tt_export void tt_http_parse_weight(IN OUT tt_char_t **s,
                                    IN OUT tt_u32_t *len,
                                    OUT tt_float_t *q);

#endif /* __TT_HTTP_PARSER__ */
