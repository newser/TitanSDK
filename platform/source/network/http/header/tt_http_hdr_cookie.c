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

#include <network/http/header/tt_http_hdr_cookie.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef struct
{
    tt_u8_t cookie_num;
    tt_u8_t cookie[TT_HTTP_TXENC_NUM];
} __hdr_cookie_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_result_t __cookie_parse(IN tt_http_hdr_t *h,
                                  IN const tt_char_t *val,
                                  IN tt_u32_t len);

static tt_u32_t __cookie_render_len(IN tt_http_hdr_t *h);

static tt_u32_t __cookie_render(IN tt_http_hdr_t *h, IN tt_char_t *dst);

static tt_http_hdr_itf_t __cookie_itf = {
    NULL, __cookie_parse, __cookie_render_len, __cookie_render,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_http_hdr_t *tt_http_hdr_cookie_create()
{
    tt_http_hdr_t *h;
    __hdr_cookie_t *ht;
    tt_u32_t i;

    h = tt_http_hdr_create_cs(sizeof(__hdr_cookie_t),
                              TT_HTTP_HDR_TXENC,
                              &__cookie_itf);
    if (h == NULL) {
        return NULL;
    }

    ht = TT_HTTP_HDR_CAST(h, __hdr_cookie_t);

    ht->cookie_num = 0;
    for (i = 0; i < TT_HTTP_TXENC_NUM; ++i) {
        ht->cookie[i] = TT_HTTP_TXENC_NUM;
    }

    return h;
}

tt_u32_t tt_http_hdr_cookie_get(IN tt_http_hdr_t *h, OUT tt_u8_t *cookie)
{
    __hdr_cookie_t *ht = TT_HTTP_HDR_CAST(h, __hdr_cookie_t);
    tt_u32_t i;

    TT_ASSERT(h->name == TT_HTTP_HDR_TXENC);

    for (i = 0; i < ht->cookie_num; ++i) {
        cookie[i] = (tt_u8_t)ht->cookie[i];
    }
    return ht->cookie_num;
}

tt_result_t __cookie_parse(IN tt_http_hdr_t *h,
                           IN const tt_char_t *val,
                           IN tt_u32_t len)
{
    __hdr_cookie_t *ht = TT_HTTP_HDR_CAST(h, __hdr_cookie_t);
    tt_u32_t i;


    // unknown cookie, set missed
    h->missed_field = TT_TRUE;
    return TT_SUCCESS;
}

tt_u32_t __cookie_render_len(IN tt_http_hdr_t *h)
{
    TT_WARN(
        "try tt_http_req_render_set_cookie() to render Transfer-Encoding "
        "instead");
    return 0;
}

tt_u32_t __cookie_render(IN tt_http_hdr_t *h, IN tt_char_t *dst)
{
    TT_WARN(
        "try tt_http_req_render_set_cookie() to render Transfer-Encoding "
        "instead");
    return 0;
}
