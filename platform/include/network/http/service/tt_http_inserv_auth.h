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
@file tt_http_inserv_auth.h
@brief http service

this file defines http incoming service: auth auth
*/

#ifndef __TT_HTTP_INSERVICE_DIGEST__
#define __TT_HTTP_INSERVICE_DIGEST__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <network/http/header/tt_http_hdr_auth.h>
#include <network/http/tt_http_in_service.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct
{
    tt_result_t (*get_pwd)(IN tt_char_t *username,
                           IN tt_u32_t username_len,
                           IN void *param,
                           OUT tt_char_t **pwd,
                           OUT tt_u32_t *pwd_len);
    union
    {
        void *get_pwd_param;
        const tt_char_t *pwd;
    };

#if 1 // test
    const tt_char_t *fixed_nonce;
#endif

    tt_char_t *realm;
    tt_char_t *domain;
    tt_u32_t realm_len;
    tt_u32_t domain_len;
    tt_http_auth_alg_t alg;
    tt_u8_t qop_mask;
    tt_bool_t realm_cstr : 1;
    tt_bool_t domain_cstr : 1;
    tt_bool_t new_nonce : 1;
} tt_http_inserv_auth_attr_t;

typedef struct
{
    tt_http_auth_ctx_t auth_ctx;
} tt_http_inserv_auth_ctx_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_http_inserv_t *tt_http_inserv_auth_create(
    IN OPT tt_http_inserv_auth_attr_t *attr);

tt_export void tt_http_inserv_auth_attr_default(
    IN tt_http_inserv_auth_attr_t *attr);

#endif /* __TT_HTTP_INSERVICE_DIGEST__ */
