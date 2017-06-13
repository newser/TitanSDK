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
@file tt_ssl_config.h
@brief ssl configuration

this file defines ssl configuration APIs
*/

#ifndef __TT_SSL_CONFIG__
#define __TT_SSL_CONFIG__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

#include <ssl.h>

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

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_ssl_config_create(IN tt_ssl_config_t *sc,
                                        IN tt_ssl_role_t role,
                                        IN tt_ssl_transport_t transport,
                                        IN tt_ssl_preset_t preset);

extern void tt_ssl_config_destroy(IN tt_ssl_config_t *sc);

extern void tt_ssl_config_version(IN tt_ssl_config_t *sc,
                                  IN tt_ssl_ver_t min,
                                  IN tt_ssl_ver_t max);

extern void tt_ssl_config_auth(IN tt_ssl_config_t *sc, IN tt_ssl_auth_t auth);

extern void tt_ssl_config_ca(IN tt_ssl_config_t *sc,
                             IN OPT struct tt_x509cert_s *ca,
                             IN OPT struct tt_x509crl_s *crl);

extern tt_result_t tt_ssl_config_cert(IN tt_ssl_config_t *sc,
                                      IN struct tt_x509cert_s *ca,
                                      IN struct tt_pk_s *pk);

extern tt_result_t tt_ssl_config_alpn(IN tt_ssl_config_t *sc,
                                      IN const tt_char_t **alpn);

extern void tt_ssl_config_trunc_hmac(IN tt_ssl_config_t *sc,
                                     IN tt_bool_t enable);

extern void tt_ssl_config_session_ticket(IN tt_ssl_config_t *sc,
                                         IN tt_bool_t enable);

extern void tt_ssl_config_sni(IN tt_ssl_config_t *sc,
                              IN tt_ssl_on_sni_t on_sni,
                              IN void *param);

extern tt_result_t tt_ssl_config_cache(IN tt_ssl_config_t *sc,
                                       IN OPT struct tt_ssl_cache_attr_s *attr);

#endif /* __TT_SSL_CONFIG__ */
