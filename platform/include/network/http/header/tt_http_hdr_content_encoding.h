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
@file tt_http_hdr_content_encoding.h
 @brief http header: content encoding

this file defines http content encoding header
*/

#ifndef __TT_HTTP_HDR_CONTENT_ENCODING__
#define __TT_HTTP_HDR_CONTENT_ENCODING__

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

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_http_hdr_t *tt_http_hdr_contenc_create();

tt_export tt_u32_t tt_http_hdr_contenc_get(IN tt_http_hdr_t *h,
                                           OUT tt_u8_t *contenc);

tt_export void tt_http_hdr_contenc_set(IN tt_http_hdr_t *h,
                                       IN tt_http_enc_t *contenc,
                                       IN tt_u32_t num);

#endif /* __TT_HTTP_HDR_CONTENT_ENCODING__ */
