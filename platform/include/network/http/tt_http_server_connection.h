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
@file tt_http_server_connection.h
@brief http server connection

this file defines http server connection
*/

#ifndef __TT_HTTP_SERVER_CONNECTION__
#define __TT_HTTP_SERVER_CONNECTION__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <network/http/tt_http_parser.h>
#include <network/http/tt_http_render.h>
#include <network/http/tt_http_service_manager.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_http_sconn_s;
struct tt_skt_s;
struct tt_ssl_s;
struct tt_tmr_s;

typedef struct tt_http_sconn_s
{
    void *itf;
    void *itf_opaque;
    struct tt_tmr_s *tmr;
    tt_http_svcmgr_t svcmgr;
    tt_http_parser_t parser;
    tt_http_resp_render_t render;
    tt_bool_t discarding : 1;
} tt_http_sconn_t;

typedef struct
{
    tt_http_parser_attr_t parser_attr;
    tt_http_resp_render_attr_t render_attr;
} tt_http_sconn_attr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_result_t
tt_http_sconn_create_skt(IN tt_http_sconn_t *c,
                         IN TO struct tt_skt_s *s,
                         IN OPT tt_http_sconn_attr_t *attr);

tt_export tt_result_t
tt_http_sconn_create_ssl(IN tt_http_sconn_t *c,
                         IN TO struct tt_ssl_s *s,
                         IN OPT tt_http_sconn_attr_t *attr);

tt_export void tt_http_sconn_destroy(IN tt_http_sconn_t *c);

tt_export void tt_http_sconn_attr_default(IN tt_http_sconn_attr_t *attr);

// false to close connection
tt_export tt_bool_t tt_http_sconn_run(IN tt_http_sconn_t *c);

tt_export tt_result_t tt_http_sconn_send(IN tt_http_sconn_t *c,
                                         IN tt_u8_t *buf,
                                         IN tt_u32_t len,
                                         OUT OPT tt_u32_t *sent);

#endif /* __TT_HTTP_SERVER_CONNECTION__ */
