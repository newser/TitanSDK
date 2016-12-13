/* Licensed to the Apache Software Foundation (ASF) under one or more
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
@file tt_ssl.h
@brief tt ssl

this file defines tt ssl
*/

#ifndef __TT_SSL__
#define __TT_SSL__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <network/ssl/tt_ssl_aio_cb.h>

#include <tt_ssl_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_skt_s;
struct tt_sslctx_s;

typedef struct tt_ssl_attr_s
{
    tt_bool_t from_alloc : 1;
    tt_bool_t session_resume : 1;
} tt_ssl_attr_t;

typedef struct tt_ssl_s
{
    tt_ssl_ntv_t sys_ssl;

    struct tt_skt_s *skt;
    struct tt_sslctx_s *sslctx;
    tt_ssl_exit_t exit;
    tt_ssl_attr_t attr;

    // ssl satisfying below values would try to share same session id:
    //  - tt_ssl_t::ssl_ctx
    //  - tt_ssl_t::session_mark
    //  - remote ip address
    //  - remote port
    tt_u32_t session_mark;

    tt_bool_t leaf_cert_verified : 1;
} tt_ssl_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern void tt_ssl_component_register();

// check ssl support during run time
tt_inline tt_bool_t tt_ssl_enabled()
{
    return tt_ssl_enabled_ntv();
}

extern void tt_ssl_stat_show(IN tt_u32_t flag);

#endif /* __TT_SSL__ */
