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
@file tt_ssl_def.h
@brief ssl definitions

this file defines ssl definitions
*/

#ifndef __TT_SSL_DEF__
#define __TT_SSL_DEF__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_SSL_SHUTDOWN_RD (1 << 0)
#define TT_SSL_SHUTDOWN_WR (1 << 1)
#define TT_SSL_SHUTDOWN_RDWR (TT_SSL_SHUTDOWN_RD | TT_SSL_SHUTDOWN_WR)

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef enum {
    TT_SSL_ROLE_CLIENT,
    TT_SSL_ROLE_SERVER,

    TT_SSL_ROLE_NUM,
} tt_ssl_role_t;
#define TT_SSL_ROLE_VALID(role) (role) < TT_SSL_ROLE_NUM

typedef enum {
    TT_SSL_CERT_FMT_X509,
    TT_SSL_CERT_FMT_PKCS7,
    TT_SSL_CERT_FMT_PKCS12,

    TT_SSL_CERT_FMT_NUM
} tt_ssl_cert_format_t;
#define TT_SSL_CERT_FMT_VALID(fmt) (fmt) < TT_SSL_CERT_FMT_NUM

typedef struct
{
    tt_bool_t pem_armor : 1;
    tt_bool_t encrypted : 1;

    const tt_char_t *password;
} tt_ssl_cert_attr_t;

typedef enum {
    TT_SSL_PRIVKEY_FMT_NONE,
    TT_SSL_PRIVKEY_FMT_PKCS8,

    TT_SSL_PRIVKEY_FMT_NUM
} tt_ssl_privkey_format_t;
#define TT_SSL_PRIVKEY_FMT_VALID(fmt) (fmt) < TT_SSL_PRIVKEY_FMT_NUM

typedef struct
{
    tt_bool_t pem_armor : 1;
    tt_bool_t encrypted : 1;

    const tt_char_t *password;
} tt_ssl_privkey_attr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif // __TT_SSL_DEF__
