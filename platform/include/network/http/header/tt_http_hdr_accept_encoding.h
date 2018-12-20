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
@file tt_http_hdr_accept_encoding.h
 @brief http header: accept encoding

this file defines http accept encoding header
*/

#ifndef __TT_HTTP_HDR_ACCEPT_ENCODING__
#define __TT_HTTP_HDR_ACCEPT_ENCODING__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <network/http/tt_http_header.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct tt_http_accenc_s
{
    tt_float_t weight[TT_HTTP_ENC_NUM];
    tt_float_t aster_weight;
    tt_bool_t has[TT_HTTP_ENC_NUM];
    tt_bool_t has_aster;
} tt_http_accenc_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_http_hdr_t *tt_http_hdr_accenc_create();

tt_export tt_http_accenc_t *tt_http_hdr_accenc_get(IN tt_http_hdr_t *h);

tt_export void tt_http_hdr_accenc_set(IN tt_http_hdr_t *h,
                                      IN tt_http_accenc_t *accenc);

tt_inline void tt_http_accenc_init(IN tt_http_accenc_t *ha)
{
    tt_u32_t i;
    for (i = 0; i < TT_HTTP_ENC_NUM; ++i) {
        ha->weight[i] = -1.0f;
        ha->has[i] = TT_FALSE;
    }
    ha->aster_weight = -1.0f;
    ha->has_aster = TT_FALSE;
}

// @has indicate whether the encoding exist
// @weight: nega val means unexist
tt_inline void tt_http_accenc_set(IN tt_http_accenc_t *ha,
                                  IN tt_http_enc_t enc,
                                  IN tt_bool_t has,
                                  IN tt_float_t weight)
{
    weight = TT_MIN(weight, 1.0f);

    ha->has[enc] = has;
    ha->weight[enc] = weight;
}

tt_inline void tt_http_accenc_set_aster(IN tt_http_accenc_t *ha,
                                        IN tt_bool_t has,
                                        IN tt_float_t weight)
{
    weight = TT_MIN(weight, 1.0f);

    ha->has_aster = has;
    ha->aster_weight = weight;
}

#endif /* __TT_HTTP_HDR_ACCEPT_ENCODING__ */
