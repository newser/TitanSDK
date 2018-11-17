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

#include <http_parser.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_slab_s;

typedef struct
{
    struct tt_slab_s *rawhdr_slab;
    struct tt_slab_s *rawval_slab;
    tt_dlist_t rawhdr;
    tt_buf_t rbuf;
    tt_u32_t refind_threshold;
    http_parser parser;
} tt_http_parser_t;

typedef struct
{
    tt_buf_attr_t rbuf_attr;
    tt_u32_t rbuf_refind_threshold;
} tt_http_parser_attr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export void tt_http_parser_init(IN tt_http_parser_t *hp,
                                   IN tt_bool_t request,
                                   IN struct tt_slab_s *rawhdr_slab,
                                   IN struct tt_slab_s *rawval_slab,
                                   IN OPT tt_http_parser_attr_t *attr);

tt_export void tt_http_parser_destroy(IN tt_http_parser_t *hp);

tt_export void tt_http_parser_attr_default(IN tt_http_parser_attr_t *attr);

tt_export void tt_http_parser_clear(IN tt_http_parser_t *hp);

tt_inline tt_result_t tt_http_parser_reserve(IN tt_http_parser_t *hp,
                                             IN tt_u32_t len)
{
    return tt_buf_reserve(&hp->rbuf, len);
}

tt_inline void tt_http_parser_rbuf(IN tt_http_parser_t *hp,
                                   OUT tt_u8_t **addr,
                                   OUT tt_u32_t *len)
{
    *addr = TT_BUF_RPOS(&hp->rbuf);
    *len = TT_BUF_RLEN(&hp->rbuf);
}

tt_export tt_result_t tt_http_parser_run(IN tt_http_parser_t *hp,
                                         IN tt_u32_t len);

#endif /* __TT_HTTP_PARSER__ */
