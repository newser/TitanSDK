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

#define TT_HTTP_HDR_MAP(__ENTRY) __ENTRY(HOST, Host)

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

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

tt_export const tt_char_t *tt_g_http_hname[TT_HTTP_HNAME_NUM];

tt_export tt_u32_t tt_g_http_hname_len[TT_HTTP_HNAME_NUM];

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif /* __TT_HTTP_DEF__ */
