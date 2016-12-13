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
@file tt_ssl_session.h
@brief ssl session

this file defines ssl session APIs
*/

#ifndef __TT_SSL_SESSION_NATIVE__
#define __TT_SSL_SESSION_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

#ifdef TT_PLATFORM_SSL_ENABLE
#include <openssl/ssl.h>
#endif

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct
{
#ifdef TT_PLATFORM_SSL_ENABLE
    SSL_SESSION *openssl_session;
#else
    void *openssl_session;
#endif
} tt_sslpeer_ntv_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_inline void tt_sslpeer_destroy_ntv(IN tt_sslpeer_ntv_t *sys_peer)
{
#ifdef TT_PLATFORM_SSL_ENABLE
    SSL_SESSION_free(sys_peer->openssl_session);
#endif
}

#endif /* __TT_SSL_SESSION_NATIVE__ */
