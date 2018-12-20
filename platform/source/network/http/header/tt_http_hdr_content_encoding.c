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

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <network/http/header/tt_http_hdr_content_encoding.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef struct
{
    tt_u8_t enc_num;
    tt_u8_t enc[TT_HTTP_ENC_NUM];
} __hdr_contenc_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_result_t __contenc_parse(IN tt_http_hdr_t *h,
                                   IN const tt_char_t *val,
                                   IN tt_u32_t len);

static tt_u32_t __contenc_render_len(IN tt_http_hdr_t *h);

static tt_u32_t __contenc_render(IN tt_http_hdr_t *h, IN tt_char_t *dst);

static tt_http_hdr_itf_t __contenc_itf = {
    __contenc_parse, __contenc_render_len, __contenc_render,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_http_hdr_t *tt_http_hdr_contenc_create()
{
    tt_http_hdr_t *h;
    __hdr_contenc_t *hc;
    tt_u32_t i;

    h = tt_http_hdr_create_cs(sizeof(__hdr_contenc_t),
                              TT_HTTP_HDR_CONTENC,
                              &__contenc_itf);
    if (h == NULL) {
        return NULL;
    }

    hc = TT_HTTP_HDR_CAST(h, __hdr_contenc_t);

    hc->enc_num = 0;
    for (i = 0; i < TT_HTTP_ENC_NUM; ++i) {
        hc->enc[i] = TT_HTTP_ENC_NUM;
    }

    return h;
}

tt_u32_t tt_http_hdr_contenc_get(IN tt_http_hdr_t *h, OUT tt_u8_t *contenc)
{
    __hdr_contenc_t *hc = TT_HTTP_HDR_CAST(h, __hdr_contenc_t);
    tt_u32_t i;

    TT_ASSERT(h->name == TT_HTTP_HDR_CONTENC);

    for (i = 0; i < hc->enc_num; ++i) {
        contenc[i] = (tt_u8_t)hc->enc[i];
    }
    return hc->enc_num;
}

void tt_http_hdr_contenc_set(IN tt_http_hdr_t *h,
                             IN tt_http_enc_t *contenc,
                             IN tt_u32_t num)
{
    __hdr_contenc_t *hc = TT_HTTP_HDR_CAST(h, __hdr_contenc_t);
    tt_u32_t i;

    TT_ASSERT(h->name == TT_HTTP_HDR_CONTENC);
    TT_ASSERT(num <= TT_HTTP_ENC_NUM);

    for (i = 0; i < num; ++i) {
        hc->enc[i] = (tt_u8_t)contenc[i];
    }
    hc->enc_num = num;
}

tt_result_t __contenc_parse(IN tt_http_hdr_t *h,
                            IN const tt_char_t *val,
                            IN tt_u32_t len)
{
    __hdr_contenc_t *hc = TT_HTTP_HDR_CAST(h, __hdr_contenc_t);
    tt_u32_t i;

    for (i = 0; i < TT_HTTP_ENC_NUM; ++i) {
        // All content-coding names are case-insensitive
        if ((len == tt_g_http_enc_len[i]) &&
            (tt_strnicmp(val, tt_g_http_enc[i], len) == 0)) {
            if (hc->enc_num < TT_HTTP_ENC_NUM) {
                hc->enc[hc->enc_num++] = i;
                return TT_SUCCESS;
            } else {
                // full enc, set missed
                h->missed_field = TT_TRUE;
            }
        }
    }

    // unknown enc, set missed
    h->missed_field = TT_TRUE;
    return TT_SUCCESS;
}

tt_u32_t __contenc_render_len(IN tt_http_hdr_t *h)
{
    __hdr_contenc_t *hc = TT_HTTP_HDR_CAST(h, __hdr_contenc_t);
    tt_u32_t n, i;

    if (hc->enc_num == 0) {
        return 0;
    }

    // "Content-Encoding: "
    n = tt_g_http_hname_len[TT_HTTP_HDR_CONTENC] + 2;

    for (i = 0; i < hc->enc_num; ++i) {
        // "gzip, " or "gzip\r\n"
        n += tt_g_http_enc_len[hc->enc[i]] + 2;
    }

    return n;
}

tt_u32_t __contenc_render(IN tt_http_hdr_t *h, IN tt_char_t *dst)
{
    __hdr_contenc_t *hc = TT_HTTP_HDR_CAST(h, __hdr_contenc_t);
    tt_char_t *p;
    tt_u32_t n, i;

    if (hc->enc_num == 0) {
        return 0;
    }

    // "Content-Encoding: "
    p = dst;
    n = tt_g_http_hname_len[TT_HTTP_HDR_CONTENC];
    tt_memcpy(p, tt_g_http_hname[TT_HTTP_HDR_CONTENC], n);
    p += n;
    *p++ = ':';
    *p++ = ' ';

    // "gzip, "
    for (i = 0; i < hc->enc_num; ++i) {
        tt_http_enc_t e = hc->enc[i];
        n = tt_g_http_enc_len[e];
        tt_memcpy(p, tt_g_http_enc[e], n);
        p += n;
        *p++ = ',';
        *p++ = ' ';
    }

    // "\r\n"
    p -= 2;
    *p++ = '\r';
    *p++ = '\n';

    return p - dst;
}
