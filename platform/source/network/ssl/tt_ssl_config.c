/* Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file exc__endpointt in compliance with
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

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <network/ssl/tt_ssl_config.h>

#include <crypto/tt_ctr_drbg.h>
#include <crypto/tt_public_key.h>
#include <misc/tt_assert.h>
#include <misc/tt_util.h>
#include <network/ssl/tt_ssl.h>
#include <network/ssl/tt_x509_cert.h>
#include <network/ssl/tt_x509_crl.h>

#include <ssl_cache.h>
#include <ssl_ticket.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_ssl_config_create(IN tt_ssl_config_t *sc,
                                 IN tt_ssl_side_t side,
                                 IN tt_ssl_transport_t transport,
                                 IN tt_ssl_preset_t preset)
{
    mbedtls_ssl_config *cfg;
    int ep, tp, ps, e;

    TT_ASSERT(sc != NULL);
    TT_ASSERT(TT_SSL_SIDE_VALID(side));
    TT_ASSERT(TT_SSL_TRANSPORT_VALID(transport));
    TT_ASSERT(TT_SSL_PRESET_VALID(preset));

    cfg = &sc->cfg;

    mbedtls_ssl_config_init(cfg);

    if (side == TT_SSL_SIDE_CLIENT) {
        ep = MBEDTLS_SSL_IS_CLIENT;
    } else {
        ep = MBEDTLS_SSL_IS_SERVER;
    }

    if (transport == TT_SSL_TRANSPORT_STREAM) {
        tp = MBEDTLS_SSL_TRANSPORT_STREAM;
    } else {
        tp = MBEDTLS_SSL_TRANSPORT_DATAGRAM;
    }

    if (preset == TT_SSL_PRESET_SUITEB) {
        ps = MBEDTLS_SSL_PRESET_SUITEB;
    } else {
        ps = MBEDTLS_SSL_PRESET_DEFAULT;
    }

    e = mbedtls_ssl_config_defaults(cfg, ep, tp, ps);
    if (e != 0) {
        tt_ssl_error("fail to load default config");
        return TT_FAIL;
    }

#if 0 // todo
    if (side == TT_SSL_SIDE_SERVER) {
        mbedtls_ssl_conf_session_tickets_cb(cfg,
                                            mbedtls_ssl_ticket_write,
                                            mbedtls_ssl_ticket_parse,
                                            NULL);

        mbedtls_ssl_conf_session_cache(cfg,
                                       NULL,
                                       mbedtls_ssl_cache_get,
                                       mbedtls_ssl_cache_set);
    }
#endif

    mbedtls_ssl_conf_rng(&sc->cfg, tt_ctr_drbg, tt_current_ctr_drbg());

    // mbedtls_ssl_conf_dbg(&sc->cfg, ...);

    return TT_SUCCESS;
}

void tt_ssl_config_destroy(IN tt_ssl_config_t *sc)
{
    TT_ASSERT(sc != NULL);

    mbedtls_ssl_config_free(&sc->cfg);
}

void tt_ssl_config_auth(IN tt_ssl_config_t *sc, IN tt_ssl_auth_t auth)
{
    TT_ASSERT(sc != NULL);
    TT_ASSERT(TT_SSL_AUTH_VALID(auth));

    if (auth == TT_SSL_AUTH_NONE) {
        mbedtls_ssl_conf_authmode(&sc->cfg, MBEDTLS_SSL_VERIFY_NONE);
    } else if (auth == TT_SSL_AUTH_OPTIONAL) {
        mbedtls_ssl_conf_authmode(&sc->cfg, MBEDTLS_SSL_VERIFY_OPTIONAL);
    } else {
        mbedtls_ssl_conf_authmode(&sc->cfg, MBEDTLS_SSL_VERIFY_REQUIRED);
    }
}

void tt_ssl_config_ca(IN tt_ssl_config_t *sc,
                      IN OPT tt_x509cert_t *ca,
                      IN OPT tt_x509crl_t *crl)
{
    TT_ASSERT(sc != NULL);

    mbedtls_ssl_conf_ca_chain(&sc->cfg,
                              TT_COND(ca != NULL, &ca->crt, NULL),
                              TT_COND(crl != NULL, &crl->crl, NULL));
}

tt_result_t tt_ssl_config_cert(IN tt_ssl_config_t *sc,
                               IN tt_x509cert_t *cert,
                               IN tt_pk_t *pk)
{
    int e;

    TT_ASSERT(sc != NULL);
    TT_ASSERT(cert != NULL);
    TT_ASSERT(pk != NULL);

    e = mbedtls_ssl_conf_own_cert(&sc->cfg, &cert->crt, &pk->ctx);
    if (e != 0) {
        tt_ssl_error("fail to conf own cert");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_ssl_config_alpn(IN tt_ssl_config_t *sc,
                               IN const tt_char_t **alpn)
{
    int e;

    TT_ASSERT(sc != NULL);
    TT_ASSERT(alpn != NULL);

    e = mbedtls_ssl_conf_alpn_protocols(&sc->cfg, alpn);
    if (e != 0) {
        tt_ssl_error("fail to conf alpn");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_ssl_config_version(IN tt_ssl_config_t *sc,
                           IN tt_ssl_version_t min,
                           IN tt_ssl_version_t max)
{
    int major, minor;

    TT_ASSERT(sc != NULL);
    TT_ASSERT(TT_SSL_VERSION_VALID(min) && TT_SSL_VERSION_VALID(max));
    TT_ASSERT(min <= max);

    major = MBEDTLS_SSL_MAJOR_VERSION_3;

    if (min == TT_SSL_V3_0) {
        minor = MBEDTLS_SSL_MINOR_VERSION_0;
    } else if (min == TT_SSL_V3_1) {
        minor = MBEDTLS_SSL_MINOR_VERSION_1;
    } else if (min == TT_SSL_V3_2) {
        minor = MBEDTLS_SSL_MINOR_VERSION_2;
    } else {
        minor = MBEDTLS_SSL_MINOR_VERSION_3;
    }
    mbedtls_ssl_conf_min_version(&sc->cfg, major, minor);

    if (max == TT_SSL_V3_0) {
        minor = MBEDTLS_SSL_MINOR_VERSION_0;
    } else if (max == TT_SSL_V3_1) {
        minor = MBEDTLS_SSL_MINOR_VERSION_1;
    } else if (max == TT_SSL_V3_2) {
        minor = MBEDTLS_SSL_MINOR_VERSION_2;
    } else {
        minor = MBEDTLS_SSL_MINOR_VERSION_3;
    }
    mbedtls_ssl_conf_max_version(&sc->cfg, major, minor);
}

void tt_ssl_config_trunc_hmac(IN tt_ssl_config_t *sc, IN tt_bool_t enable)
{
    TT_ASSERT(sc != NULL);

    mbedtls_ssl_conf_truncated_hmac(&sc->cfg,
                                    TT_COND(enable,
                                            MBEDTLS_SSL_TRUNC_HMAC_ENABLED,
                                            MBEDTLS_SSL_TRUNC_HMAC_DISABLED));
}

void tt_ssl_config_session_ticket(IN tt_ssl_config_t *sc, IN tt_bool_t enable)
{
    TT_ASSERT(sc != NULL);

    mbedtls_ssl_conf_session_tickets(
        &sc->cfg,
        TT_COND(enable,
                MBEDTLS_SSL_SESSION_TICKETS_ENABLED,
                MBEDTLS_SSL_SESSION_TICKETS_DISABLED));
}
