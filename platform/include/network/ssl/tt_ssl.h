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
@file tt_ssl.h
@brief ssl initialization

this file defines ssl initialization APIs
*/

#ifndef __TT_SSL__
#define __TT_SSL__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <log/tt_log.h>
#include <log/tt_log_manager.h>
#include <network/ssl/tt_ssl_config.h>
#include <os/tt_fiber_event.h>

#include <ssl.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define tt_ssl_error(...)                                                      \
    do {                                                                       \
        tt_export void mbedtls_strerror(int, char *, size_t);                  \
        tt_char_t buf[256];                                                    \
        TT_ERROR(__VA_ARGS__);                                                 \
        mbedtls_strerror(e, buf, sizeof(buf) - 1);                             \
        TT_ERROR("%s", buf);                                                   \
    } while (0)

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_skt_s;
struct tt_ssl_config_s;
struct tt_tmr_s;

typedef struct tt_ssl_s
{
    struct tt_skt_s *skt;
    tt_fiber_ev_t **p_fev;
    struct tt_tmr_s **p_tmr;

    mbedtls_ssl_context ctx;
} tt_ssl_t;

typedef enum {
    TT_SSL_SHUT_RD,
    TT_SSL_SHUT_WR,
    TT_SSL_SHUT_RDWR,

    TT_SSL_SHUT_NUM
} tt_ssl_shut_t;
#define TT_SSL_SHUT_VALID(s) ((s) < TT_SSL_SHUT_NUM)

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

tt_export tt_logmgr_t tt_g_ssl_logmgr;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/**
@fn void tt_ssl_component_register()
register ts crypto component
*/
tt_export void tt_ssl_component_register();

tt_export void tt_ssl_log_component_register();

tt_export tt_ssl_t *tt_ssl_create(IN struct tt_skt_s *skt,
                                  IN struct tt_ssl_config_s *sc);

tt_export void tt_ssl_destroy(IN tt_ssl_t *ssl);

tt_export tt_result_t tt_ssl_handshake(IN tt_ssl_t *ssl,
                                       OUT tt_fiber_ev_t **p_fev,
                                       OUT struct tt_tmr_s **p_tmr);

tt_export tt_result_t tt_ssl_send(IN tt_ssl_t *ssl,
                                  IN tt_u8_t *buf,
                                  IN tt_u32_t len,
                                  OUT tt_u32_t *sent);

tt_export tt_result_t tt_ssl_recv(IN tt_ssl_t *ssl,
                                  OUT tt_u8_t *buf,
                                  IN tt_u32_t len,
                                  OUT tt_u32_t *recvd,
                                  OUT tt_fiber_ev_t **p_fev,
                                  OUT struct tt_tmr_s **p_tmr);

tt_export tt_result_t tt_ssl_shutdown(IN tt_ssl_t *ssl, IN tt_ssl_shut_t shut);

tt_export struct tt_ssl_config_s *tt_ssl_get_config(IN tt_ssl_t *ssl);

tt_export tt_result_t tt_ssl_set_hostname(IN tt_ssl_t *ssl,
                                          IN const tt_char_t *hostname);

tt_export void tt_ssl_set_ca(IN tt_ssl_t *ssl,
                             IN OPT struct tt_x509cert_s *ca,
                             IN OPT struct tt_x509crl_s *crl);

tt_export tt_result_t tt_ssl_set_cert(IN tt_ssl_t *ssl,
                                      IN struct tt_x509cert_s *cert,
                                      IN struct tt_pk_s *pk);

tt_export void tt_ssl_set_auth(IN tt_ssl_t *ssl, IN tt_ssl_auth_t auth);

tt_inline const tt_char_t *tt_ssl_get_alpn(IN tt_ssl_t *ssl)
{
    return mbedtls_ssl_get_alpn_protocol(&ssl->ctx);
}

#endif /* __TT_SSL__ */
