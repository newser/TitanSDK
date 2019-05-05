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
@file tt_http_server.h
@brief http server

this file defines http server
*/

#ifndef __TT_HTTP_SERVER__
#define __TT_HTTP_SERVER__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <io/tt_socket.h>
#include <network/ssl/tt_ssl.h>
#include <os/tt_fiber.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct
{
    union
    {
        tt_skt_t *skt;
        tt_ssl_t *ssl;
    };
    tt_ssl_config_t *ssl_server_cfg;
    tt_skt_attr_t new_skt_attr;
    tt_fiber_attr_t conn_fiber_attr;
    tt_bool_t https : 1;
} tt_http_server_t;

typedef struct
{
    tt_skt_attr_t new_skt_attr;
    tt_fiber_attr_t conn_fiber_attr;
} tt_http_server_attr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_result_t
tt_http_server_create_skt(IN tt_http_server_t *svr, IN TO tt_skt_t *skt,
                          IN OPT tt_http_server_attr_t *attr);

tt_export tt_result_t tt_http_server_create_ssl(
    IN tt_http_server_t *svr, IN TO tt_ssl_t *ssl,
    IN tt_ssl_config_t *ssl_server_cfg, IN OPT tt_http_server_attr_t *attr);

tt_export void tt_http_server_destroy(IN tt_http_server_t *svr);

tt_export void tt_http_server_attr_default(IN tt_http_server_attr_t *attr);

tt_export tt_result_t tt_http_server_run_fiber(IN tt_http_server_t *svr);

#endif /* __TT_HTTP_SERVER__ */
