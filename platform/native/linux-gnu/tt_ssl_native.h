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
@file tt_ssl_native.h
@brief ts ssl portlayer

this file defines ts ssl portlayer
*/

#ifndef __TT_SSL_NATIVE__
#define __TT_SSL_NATIVE__

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
    SSL *ssl;
#else
    void *ssl;
#endif
} tt_ssl_ntv_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

extern int tt_g_x509_store_ctx_private_idx;

extern int tt_g_ssl_private_idx;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_ssl_component_init_ntv();

tt_inline tt_bool_t tt_ssl_enabled_ntv()
{
#ifdef TT_PLATFORM_SSL_ENABLE
    return TT_TRUE;
#else
    return TT_FALSE;
#endif
}

#endif /* __TT_SSL_NATIVE__ */
