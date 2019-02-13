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

#include <network/http/header/tt_http_hdr_accept_encoding.h>

#include <network/http/tt_http_parser.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_result_t __accenc_parse(IN tt_http_hdr_t *h,
                                  IN const tt_char_t *val,
                                  IN tt_u32_t len);

static tt_u32_t __accenc_render_len(IN tt_http_hdr_t *h);

static tt_u32_t __accenc_render(IN tt_http_hdr_t *h, IN tt_char_t *dst);

static tt_http_hdr_itf_t __accenc_itf = {
    NULL, NULL, __accenc_parse, NULL, __accenc_render_len, __accenc_render,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_http_hdr_t *tt_http_hdr_accenc_create()
{
    tt_http_hdr_t *h;
    tt_http_accenc_t *ha;

    h = tt_http_hdr_create_cs(sizeof(tt_http_accenc_t),
                              TT_HTTP_HDR_ACCENC,
                              &__accenc_itf);
    if (h == NULL) {
        return NULL;
    }

    ha = TT_HTTP_HDR_CAST(h, tt_http_accenc_t);

    tt_http_accenc_init(ha);

    return h;
}

tt_http_accenc_t *tt_http_hdr_accenc_get(IN tt_http_hdr_t *h)
{
    tt_http_accenc_t *ha = TT_HTTP_HDR_CAST(h, tt_http_accenc_t);

    TT_ASSERT(h->name == TT_HTTP_HDR_ACCENC);

    return ha;
}

void tt_http_hdr_accenc_set(IN tt_http_hdr_t *h, IN tt_http_accenc_t *accenc)
{
    tt_http_accenc_t *ha = TT_HTTP_HDR_CAST(h, tt_http_accenc_t);

    TT_ASSERT(h->name == TT_HTTP_HDR_ACCENC);

    tt_memcpy(ha, accenc, sizeof(tt_http_accenc_t));
}

tt_result_t __accenc_parse(IN tt_http_hdr_t *h,
                           IN const tt_char_t *val,
                           IN tt_u32_t len)
{
    tt_http_accenc_t *ha = TT_HTTP_HDR_CAST(h, tt_http_accenc_t);
    tt_float_t weight;
    tt_u32_t i;

    tt_http_parse_weight((tt_char_t **)&val, &len, &weight);

    for (i = 0; i < TT_HTTP_ENC_NUM; ++i) {
        if ((len == tt_g_http_enc_len[i]) &&
            (tt_strnicmp(val, tt_g_http_enc[i], len) == 0)) {
            tt_http_accenc_set(ha, i, TT_TRUE, weight);
            return TT_SUCCESS;
        }

        if ((len == 1) && (*val == '*')) {
            tt_http_accenc_set_aster(ha, TT_TRUE, weight);
            return TT_SUCCESS;
        }
    }

    // unknown enc, set missed
    h->missed_field = TT_TRUE;
    return TT_SUCCESS;
}

tt_u32_t __accenc_render_len(IN tt_http_hdr_t *h)
{
    tt_http_accenc_t *ha = TT_HTTP_HDR_CAST(h, tt_http_accenc_t);
    tt_u32_t n, i;
    tt_bool_t empty = TT_TRUE;

    // always render "Accept-Encoding: "
    n = tt_g_http_hname_len[TT_HTTP_HDR_ACCENC] + 2;

    for (i = 0; i < TT_HTTP_ENC_NUM; ++i) {
        if (ha->has[i]) {
            // "gzip, " or "gzip\r\n"
            n += tt_g_http_enc_len[i] + 2;
            if (ha->weight[i] >= 0) {
                // always reserve space for ";q=x.xxx"
                n += 8;
            }
            empty = TT_FALSE;
        }
    }

    if (ha->has_aster) {
        // "*, " or "*\r\n"
        n += 3;
        if (ha->aster_weight >= 0) {
            // always reserve space for ";q=x.xxx"
            n += 8;
        }
        empty = TT_FALSE;
    }

    if (empty) {
        // ending "\r\n"
        n += 2;
    }

    return n;
}

tt_u32_t __accenc_render(IN tt_http_hdr_t *h, IN tt_char_t *dst)
{
    tt_http_accenc_t *ha = TT_HTTP_HDR_CAST(h, tt_http_accenc_t);
    tt_char_t *p;
    tt_u32_t n, i;
    tt_bool_t empty = TT_TRUE;

    // "Accept-Encoding: "
    p = dst;
    n = tt_g_http_hname_len[TT_HTTP_HDR_ACCENC];
    tt_memcpy(p, tt_g_http_hname[TT_HTTP_HDR_ACCENC], n);
    p += n;
    *p++ = ':';
    *p++ = ' ';

    for (i = 0; i < TT_HTTP_ENC_NUM; ++i) {
        if (ha->has[i]) {
            // "gzip, " or "gzip\r\n"
            n = tt_g_http_enc_len[i];
            tt_memcpy(p, tt_g_http_enc[i], n);
            p += n;

            if (ha->weight[i] >= 0) {
                tt_float_t w = ha->weight[i];
                tt_char_t buf[6] = {(tt_u8_t)w + '0',
                                    '.',
                                    (tt_u8_t)(((tt_u32_t)(w * 10)) % 10) + '0',
                                    (tt_u8_t)(((tt_u32_t)(w * 100)) % 10) + '0',
                                    (tt_u8_t)(((tt_u32_t)(w * 1000)) % 10) +
                                        '0',
                                    0};
                for (n = 4; (n != 1) && (buf[n] == '0'); --n)
                    ;
                if (n == 1) {
                    --n;
                }
                ++n;

                tt_memcpy(p, ";q=", 3);
                p += 3;
                tt_memcpy(p, buf, n);
                p += n;
            }

            *p++ = ',';
            *p++ = ' ';

            empty = TT_FALSE;
        }
    }

    if (ha->has_aster) {
        // "*, " or "*\r\n"
        *p++ = '*';

        if (ha->aster_weight >= 0) {
            tt_float_t w = ha->aster_weight;
            tt_char_t buf[6] = {(tt_u8_t)w + '0',
                                '.',
                                (tt_u8_t)(((tt_u32_t)(w * 10)) % 10) + '0',
                                (tt_u8_t)(((tt_u32_t)(w * 100)) % 10) + '0',
                                (tt_u8_t)(((tt_u32_t)(w * 1000)) % 10) + '0',
                                0};
            for (n = 4; (n != 1) && (buf[n] == '0'); --n)
                ;
            if (n == 1) {
                --n;
            }
            ++n;

            tt_memcpy(p, ";q=", 3);
            p += 3;
            tt_memcpy(p, buf, n);
            p += n;
        }

        *p++ = ',';
        *p++ = ' ';

        empty = TT_FALSE;
    }

    // "\r\n"
    if (!empty) {
        p -= 2;
    }
    *p++ = '\r';
    *p++ = '\n';

    return (tt_u32_t)(p - dst);
}
