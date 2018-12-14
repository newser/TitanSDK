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
@file tt_http_content_type_def.h
@brief http content type def

this file defines http content type
*/

#ifndef __TT_HTTP_CONTENT_TYPE_DEF__
#define __TT_HTTP_CONTENT_TYPE_DEF__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_HTTP_CONTYPE_MAP(__ENTRY)                                           \
    __ENTRY(APP_JS, "application/javascript", "js")                            \
    __ENTRY(APP_OCTET, "application/octet-stream", NULL)                       \
    __ENTRY(TXT_PLAIN, "text/plain", "txt")                                    \
    __ENTRY(TXT_CSS, "text/css", "css")                                        \
    __ENTRY(TXT_HTML, "text/html", "htm;html;htx;xhtml")

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef enum {
#define __ENTRY(id, str, ext) TT_HTTP_CONTYPE_##id,
    TT_HTTP_CONTYPE_MAP(__ENTRY)
#undef __ENTRY

        TT_HTTP_CONTYPE_NUM
} tt_http_contype_t;
#define TT_HTTP_CONTYPE_VALID(h) ((h) < TT_HTTP_CONTYPE_NUM)

typedef struct
{
    tt_http_contype_t type;
    const tt_char_t *name;
    const tt_char_t *ext;
    tt_u32_t name_len;
    tt_u32_t ext_len;
} tt_http_contype_entry_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

tt_export tt_http_contype_entry_t tt_g_http_contype_static[TT_HTTP_CONTYPE_NUM];

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif /* __TT_HTTP_CONTENT_TYPE_DEF__ */
