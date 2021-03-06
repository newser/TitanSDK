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
@file tt_ssl_config.h
@brief ssl configuration

this file defines ssl configuration APIs
*/

#ifndef __TT_SSL_CONFIG__
#define __TT_SSL_CONFIG__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/ptr/tt_ptr_queue.h>
#include <os/tt_spinlock.h>

#include <ssl.h>
#include <ssl_cache.h>
#include <ssl_ticket.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_ssl_s;
struct tt_x509cert_s;
struct tt_x509crl_s;
struct tt_pk_s;
struct tt_ssl_cache_s;
struct tt_ssl_cache_attr_s;

typedef tt_result_t (*tt_ssl_on_sni_t)(IN struct tt_ssl_s *ssl,
                                       IN const tt_u8_t *sni,
                                       IN tt_u32_t len,
                                       IN void *param);

// a ssl config is for a single ssl server
typedef struct tt_ssl_config_s
{
    tt_ssl_on_sni_t on_sni;
    void *on_sni_param;
    struct tt_ssl_cache_s *cache;
    mbedtls_ssl_config cfg;
} tt_ssl_config_t;

typedef enum {
    TT_SSL_CLIENT,
    TT_SSL_SERVER,

    TT_SSL_ROLE_NUM,
} tt_ssl_role_t;
#define TT_SSL_ROLE_VALID(s) ((s) < TT_SSL_ROLE_NUM)

typedef enum {
    TT_SSL_TRANSPORT_STREAM,
    TT_SSL_TRANSPORT_DATAGRAM,

    TT_SSL_TRANSPORT_NUM,
} tt_ssl_transport_t;
#define TT_SSL_TRANSPORT_VALID(t) ((t) < TT_SSL_TRANSPORT_NUM)

typedef enum {
    TT_SSL_PRESET_DEFAULT,
    TT_SSL_PRESET_SUITEB,

    TT_SSL_PRESET_NUM,
} tt_ssl_preset_t;
#define TT_SSL_PRESET_VALID(p) ((p) < TT_SSL_PRESET_NUM)

typedef enum {
    TT_SSL_AUTH_NONE,
    TT_SSL_AUTH_OPTIONAL,
    TT_SSL_AUTH_REQUIRED,

    TT_SSL_AUTH_NUM,
} tt_ssl_auth_t;
#define TT_SSL_AUTH_VALID(a) ((a) < TT_SSL_AUTH_NUM)

typedef enum {
    TT_SSL_V3_0,
    TT_SSL_V3_1,
    TT_TLS_V1_0 = TT_SSL_V3_1,
    TT_SSL_V3_2,
    TT_TLS_V1_1 = TT_SSL_V3_2,
    TT_DTLS_V1_0 = TT_SSL_V3_2,
    TT_SSL_V3_3,
    TT_TLS_V1_2 = TT_SSL_V3_3,
    TT_DTLS_V1_2 = TT_SSL_V3_3,

    TT_SSL_VER_NUM
} tt_ssl_ver_t;
#define TT_SSL_VER_VALID(v) ((v) < TT_SSL_VER_NUM)

typedef struct
{
    tt_u32_t lifetime;
} tt_ssl_ticket_attr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_result_t tt_ssl_config_create(IN tt_ssl_config_t *sc,
                                           IN tt_ssl_role_t role,
                                           IN tt_ssl_transport_t transport,
                                           IN tt_ssl_preset_t preset);

tt_export void tt_ssl_config_destroy(IN tt_ssl_config_t *sc);

tt_export void tt_ssl_config_version(IN tt_ssl_config_t *sc,
                                     IN tt_ssl_ver_t min,
                                     IN tt_ssl_ver_t max);

tt_export void tt_ssl_config_auth(IN tt_ssl_config_t *sc,
                                  IN tt_ssl_auth_t auth);

tt_export void tt_ssl_config_ca(IN tt_ssl_config_t *sc,
                                IN OPT struct tt_x509cert_s *ca,
                                IN OPT struct tt_x509crl_s *crl);

tt_export tt_result_t tt_ssl_config_cert(IN tt_ssl_config_t *sc,
                                         IN struct tt_x509cert_s *ca,
                                         IN struct tt_pk_s *pk);

tt_export tt_result_t tt_ssl_config_alpn(IN tt_ssl_config_t *sc,
                                         IN const tt_char_t **alpn);

tt_export void tt_ssl_config_trunc_hmac(IN tt_ssl_config_t *sc,
                                        IN tt_bool_t enable);

tt_export void tt_ssl_config_sni(IN tt_ssl_config_t *sc,
                                 IN tt_ssl_on_sni_t on_sni,
                                 IN void *param);

tt_export tt_result_t
tt_ssl_config_cache(IN tt_ssl_config_t *sc,
                    IN tt_bool_t use_ticket,
                    IN OPT struct tt_ssl_cache_attr_s *attr);

tt_export void tt_ssl_config_encrypt_then_mac(IN tt_ssl_config_t *sc,
                                              IN tt_bool_t enable);

tt_export void tt_ssl_config_extended_master_secret(IN tt_ssl_config_t *sc,
                                                    IN tt_bool_t enable);

#endif /* __TT_SSL_CONFIG__ */
