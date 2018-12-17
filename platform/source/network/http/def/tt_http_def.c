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

#include <network/http/def/tt_http_def.h>

#include <misc/tt_assert.h>

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

const tt_char_t *tt_g_http_hname[TT_HTTP_HNAME_NUM] = {
#define __ENTRY(id, str) #str,
    TT_HTTP_HDR_MAP(__ENTRY)
#undef __ENTRY
};

tt_u32_t tt_g_http_hname_len[TT_HTTP_HNAME_NUM] = {
#define __ENTRY(id, str) (tt_u32_t)sizeof(#str) - 1,
    TT_HTTP_HDR_MAP(__ENTRY)
#undef __ENTRY
};

const tt_char_t *tt_g_http_method[TT_HTTP_METHOD_NUM] = {
#define XX(num, name, string) #string,
    HTTP_METHOD_MAP(XX)
#undef XX
};

tt_u32_t tt_g_http_method_len[TT_HTTP_METHOD_NUM] = {
#define XX(num, name, string) sizeof(#string) - 1,
    HTTP_METHOD_MAP(XX)
#undef XX
};

const tt_char_t *tt_g_http_verion[TT_HTTP_VER_NUM] = {
    "", "HTTP/1.0", "HTTP/1.1", "HTTP/2.0",
};

tt_u32_t tt_g_http_verion_len[TT_HTTP_VER_NUM] = {
    sizeof("") - 1,
    sizeof("HTTP/1.0") - 1,
    sizeof("HTTP/1.1") - 1,
    sizeof("HTTP/2.0") - 1,
};

const tt_char_t *tt_g_http_conn[TT_HTTP_CONN_NUM] = {
    "", "close", "keep-alive",
};

tt_u32_t tt_g_http_conn_len[TT_HTTP_CONN_NUM] = {
    sizeof("") - 1, sizeof("close") - 1, sizeof("keep-alive") - 1,
};

const tt_char_t *tt_g_http_txenc[TT_HTTP_TXENC_NUM] = {
#define __ENTRY(id, str) str,
    TT_HTTP_TXENC_MAP(__ENTRY)
#undef __ENTRY
};

tt_u32_t tt_g_http_txenc_len[TT_HTTP_TXENC_NUM] = {
#define __ENTRY(id, str) sizeof(str) - 1,
    TT_HTTP_TXENC_MAP(__ENTRY)
#undef __ENTRY
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_http_status_cstr(IN tt_http_status_t status,
                         OUT const tt_char_t **s,
                         OUT tt_u32_t *len)
{
    TT_ASSERT(TT_HTTP_STATUS_VALID(status));

    switch (status) {
#define XX(num, name, string)                                                  \
    case TT_HTTP_STATUS_##name:                                                \
        *s = #num " " #string;                                                 \
        *len = sizeof(#num " " #string) - 1;                                   \
        break;

        HTTP_STATUS_MAP(XX)
#undef XX

        default:
            TT_ASSERT_ALWAYS(0);
            break;
    }
}
