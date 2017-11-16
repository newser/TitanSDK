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
@file tt_ssl_cache.h
@brief ssl cache

this file defines ssl cache APIs
*/

#ifndef __TT_SSL_CACHE__
#define __TT_SSL_CACHE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <os/tt_spinlock.h>

#include <ssl_cache.h>
#include <ssl_ticket.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_ssl_config_s;
struct tt_ssl_s;

typedef struct tt_ssl_cache_attr_s
{
    tt_u32_t time_out;
    tt_u32_t max_entries;
} tt_ssl_cache_attr_t;

typedef struct tt_ssl_cache_s
{
    union
    {
        mbedtls_ssl_session s;
        mbedtls_ssl_cache_context c;
        mbedtls_ssl_ticket_context t;
    };
    tt_spinlock_t lock;
    tt_bool_t mode : 2;
} tt_ssl_cache_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_ssl_cache_t *tt_ssl_cache_create(IN struct tt_ssl_config_s *sc,
                                              IN tt_bool_t use_ticket,
                                              IN OPT tt_ssl_cache_attr_t *attr);

tt_export void tt_ssl_cache_destroy(IN tt_ssl_cache_t *cache);

tt_export void tt_ssl_cache_attr_default(IN tt_ssl_cache_attr_t *attr);

tt_export void tt_ssl_cache_save(IN tt_ssl_cache_t *cache,
                                 IN struct tt_ssl_s *ssl);

tt_export void tt_ssl_cache_resume(IN tt_ssl_cache_t *cache,
                                   IN struct tt_ssl_s *ssl);

#endif /* __TT_SSL_CACHE__ */
