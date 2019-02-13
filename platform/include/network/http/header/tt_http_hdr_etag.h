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
@file tt_http_hdr_etag.h
 @brief http header: etag

this file defines http etag header
*/

#ifndef __TT_HTTP_HDR_ETAG__
#define __TT_HTTP_HDR_ETAG__

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

typedef struct tt_http_etag_s
{
    tt_blobex_t etag;
    tt_bool_t weak : 1;
    tt_bool_t aster : 1;
} tt_http_etag_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

// ========================================
// ETag
// ========================================

tt_export tt_http_hdr_t *tt_http_hdr_etag_create();

tt_export tt_result_t tt_http_hdr_etag_add_n(IN tt_http_hdr_t *h,
                                             IN tt_char_t *etag,
                                             IN tt_u32_t len,
                                             IN tt_bool_t weak);

tt_inline tt_result_t tt_http_hdr_etag_add(IN tt_http_hdr_t *h,
                                           IN tt_char_t *etag,
                                           IN tt_bool_t weak)
{
    return tt_http_hdr_etag_add_n(h, etag, (tt_u32_t)tt_strlen(etag), weak);
}

tt_inline tt_http_etag_t *tt_http_etag_head(IN tt_http_hdr_t *h)
{
    tt_http_hval_t *hv;

    TT_ASSERT((h->name == TT_HTTP_HDR_ETAG) ||
              (h->name == TT_HTTP_HDR_IF_MATCH) ||
              (h->name == TT_HTTP_HDR_IF_N_MATCH));

    hv = tt_http_hval_head(h);
    return TT_COND(hv != NULL, TT_HTTP_HVAL_CAST(hv, tt_http_etag_t), NULL);
}

tt_inline tt_http_etag_t *tt_http_etag_next(IN tt_http_etag_t *h)
{
    tt_http_hval_t *hv = tt_http_hval_next(
        TT_PTR_DEC(tt_http_hval_t, h, sizeof(tt_http_hval_t)));
    return TT_COND(hv != NULL, TT_HTTP_HVAL_CAST(hv, tt_http_etag_t), NULL);
}

// ========================================
// If-Match
// ========================================

tt_export tt_http_hdr_t *tt_http_hdr_ifmatch_create();

tt_export tt_result_t tt_http_hdr_ifmatch_add_n(IN tt_http_hdr_t *h,
                                                IN tt_char_t *etag,
                                                IN tt_u32_t len,
                                                IN tt_bool_t weak);

tt_inline tt_result_t tt_http_hdr_ifmatch_add(IN tt_http_hdr_t *h,
                                              IN tt_char_t *etag,
                                              IN tt_bool_t weak)
{
    return tt_http_hdr_ifmatch_add_n(h, etag, (tt_u32_t)tt_strlen(etag), weak);
}

tt_export tt_result_t tt_http_hdr_ifmatch_add_aster(IN tt_http_hdr_t *h);

// ========================================
// If-None-Match
// ========================================

tt_export tt_http_hdr_t *tt_http_hdr_ifnmatch_create();

tt_export tt_result_t tt_http_hdr_ifnmatch_add_n(IN tt_http_hdr_t *h,
                                                 IN tt_char_t *etag,
                                                 IN tt_u32_t len,
                                                 IN tt_bool_t weak);

tt_inline tt_result_t tt_http_hdr_ifnmatch_add(IN tt_http_hdr_t *h,
                                               IN tt_char_t *etag,
                                               IN tt_bool_t weak)
{
    return tt_http_hdr_ifnmatch_add_n(h, etag, (tt_u32_t)tt_strlen(etag), weak);
}

tt_export tt_result_t tt_http_hdr_ifnmatch_add_aster(IN tt_http_hdr_t *h);

#endif /* __TT_HTTP_HDR_ETAG__ */
