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

#include <ssl.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define tt_ssl_error(...)                                                      \
    do {                                                                       \
        extern void mbedtls_strerror(int, char *, size_t);                     \
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
struct tt_fiber_ev_s;
struct tt_tmr_s;

typedef struct
{
    struct tt_skt_s *skt;
    struct tt_fiber_ev_s **p_fev;
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

extern tt_logmgr_t tt_g_ssl_logmgr;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/**
@fn void tt_ssl_component_register()
register ts crypto component
*/
extern void tt_ssl_component_register();

extern void tt_ssl_log_component_register();

extern tt_ssl_t *tt_ssl_create(IN struct tt_skt_s *skt,
                               IN struct tt_ssl_config_s *sc);

extern void tt_ssl_destroy(IN tt_ssl_t *ssl);

extern tt_result_t tt_ssl_handshake(IN tt_ssl_t *ssl,
                                    OUT struct tt_fiber_ev_s **p_fev,
                                    OUT struct tt_tmr_s **p_tmr);

extern tt_result_t tt_ssl_send(IN tt_ssl_t *ssl,
                               IN tt_u8_t *buf,
                               IN tt_u32_t len,
                               OUT tt_u32_t *sent);

extern tt_result_t tt_ssl_recv(IN tt_ssl_t *ssl,
                               OUT tt_u8_t *buf,
                               IN tt_u32_t len,
                               OUT tt_u32_t *recvd,
                               OUT struct tt_fiber_ev_s **p_fev,
                               OUT struct tt_tmr_s **p_tmr);

extern tt_result_t tt_ssl_shutdown(IN tt_ssl_t *ssl, IN tt_ssl_shut_t shut);

extern tt_result_t tt_ssl_set_hostname(IN tt_ssl_t *ssl,
                                       IN const tt_char_t *hostname);

#endif /* __TT_SSL__ */
