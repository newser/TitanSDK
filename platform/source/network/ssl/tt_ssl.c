/* Copyright (C) 2017 haniu (niuhao.cn@gmail.com)
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
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

#include <network/ssl/tt_ssl.h>

#include <crypto/tt_public_key.h>
#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <io/tt_socket.h>
#include <log/layout/tt_log_layout_pattern.h>
#include <log/tt_log_init.h>
#include <memory/tt_memory_alloc.h>
#include <misc/tt_assert.h>
#include <misc/tt_util.h>
#include <network/ssl/tt_ssl_cache.h>
#include <network/ssl/tt_ssl_config.h>
#include <network/ssl/tt_x509_cert.h>
#include <network/ssl/tt_x509_crl.h>

#include <debug.h>
#include <net_sockets.h>

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

tt_logmgr_t tt_g_ssl_logmgr;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __ssl_component_init(IN tt_component_t *comp,
                                        IN tt_profile_t *profile);

static tt_result_t __ssl_log_component_init(IN tt_component_t *comp,
                                            IN tt_profile_t *profile);

static void __ssl_log_component_exit(IN tt_component_t *comp);

static int __ssl_send(void *ctx, const unsigned char *buf, size_t len);

static int __ssl_recv(void *ctx, unsigned char *buf, size_t len);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_ssl_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __ssl_component_init, __ssl_log_component_exit,
    };

    // init component
    tt_component_init(&comp, TT_COMPONENT_SSL, "SSL", NULL, &itf);

    // register component
    tt_component_register(&comp);
}

void tt_ssl_log_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __ssl_log_component_init,
    };

    // init component
    tt_component_init(&comp, TT_COMPONENT_SSL_LOG, "SSL Log", NULL, &itf);

    // register component
    tt_component_register(&comp);
}

tt_ssl_t *tt_ssl_create(IN tt_skt_t *skt, IN tt_ssl_config_t *sc)
{
    tt_ssl_t *ssl;
    int e;

    TT_ASSERT(skt != NULL);
    TT_ASSERT(sc != NULL);

    ssl = tt_malloc(sizeof(tt_ssl_t));
    if (ssl == NULL) {
        TT_ERROR("no mem for ssl");
        return NULL;
    }

    ssl->skt = skt;
    ssl->p_fev = NULL;
    ssl->p_tmr = NULL;

    mbedtls_ssl_init(&ssl->ctx);

    e = mbedtls_ssl_setup(&ssl->ctx, &sc->cfg);
    if (e != 0) {
        tt_ssl_error("fail to setup ssl");
        goto fail;
    }

    mbedtls_ssl_set_bio(&ssl->ctx, ssl, __ssl_send, __ssl_recv, NULL);

    return ssl;

fail:

    mbedtls_ssl_free(&ssl->ctx);

    tt_free(ssl);

    return NULL;
}

void tt_ssl_destroy(IN tt_ssl_t *ssl)
{
    TT_ASSERT(ssl != NULL);

    tt_skt_destroy(ssl->skt);

    mbedtls_ssl_free(&ssl->ctx);

    tt_free(ssl);
}

tt_result_t tt_ssl_handshake(IN tt_ssl_t *ssl,
                             OUT tt_fiber_ev_t **p_fev,
                             OUT struct tt_tmr_s **p_tmr)
{
    tt_ssl_config_t *sc;
    int e;

    TT_ASSERT(ssl != NULL);

    sc = tt_ssl_get_config(ssl);

    if ((sc->cache != NULL) && (sc->cfg.endpoint == MBEDTLS_SSL_IS_CLIENT)) {
        tt_ssl_cache_resume(sc->cache, ssl);
    }

    ssl->p_fev = p_fev;
    ssl->p_tmr = p_tmr;
    while ((e = mbedtls_ssl_handshake(&ssl->ctx)) != 0) {
        if (e == MBEDTLS_ERR_SSL_WANT_READ) {
            if ((*p_fev != NULL) || (*p_tmr != NULL)) {
                return TT_E_PROCEED;
            } else {
                continue;
            }
        } else if (e == MBEDTLS_ERR_SSL_WANT_WRITE) {
            continue;
        } else {
            tt_ssl_error("ssl handshake fail");
            return TT_FAIL;
        }
    }

    if ((sc->cache != NULL) && (sc->cfg.endpoint == MBEDTLS_SSL_IS_CLIENT)) {
        tt_ssl_cache_save(sc->cache, ssl);
    }

    return TT_SUCCESS;
}

tt_result_t tt_ssl_send(IN tt_ssl_t *ssl,
                        IN tt_u8_t *buf,
                        IN tt_u32_t len,
                        OUT tt_u32_t *sent)
{
    int e;

    while ((e = mbedtls_ssl_write(&ssl->ctx, buf, len)) <= 0) {
        if ((e != MBEDTLS_ERR_SSL_WANT_READ) &&
            (e != MBEDTLS_ERR_SSL_WANT_WRITE)) {
            tt_ssl_error("ssl send fail");
            return TT_FAIL;
        }
    }

    *sent = (tt_u32_t)e;
    return TT_SUCCESS;
}

tt_result_t tt_ssl_recv(IN tt_ssl_t *ssl,
                        OUT tt_u8_t *buf,
                        IN tt_u32_t len,
                        OUT tt_u32_t *recvd,
                        OUT tt_fiber_ev_t **p_fev,
                        OUT struct tt_tmr_s **p_tmr)
{
    int e;

    ssl->p_fev = p_fev;
    ssl->p_tmr = p_tmr;
    while ((e = mbedtls_ssl_read(&ssl->ctx, buf, len)) <= 0) {
        if ((e == MBEDTLS_ERR_SSL_WANT_READ) ||
            (e == MBEDTLS_ERR_SSL_WANT_WRITE) ||
            (e == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)) {
            continue;
        } else if (e == 0) {
            if ((*p_fev != NULL) || (*p_tmr != NULL)) {
                break;
            } else {
                return TT_E_END;
            }
        } else if ((e == MBEDTLS_ERR_SSL_CONN_EOF) ||
                   (e == MBEDTLS_ERR_NET_CONN_RESET)) {
            return TT_E_END;
        } else {
            tt_ssl_error("ssl send fail");
            return TT_FAIL;
        }
    }

    *recvd = (tt_u32_t)e;
    return TT_SUCCESS;
}

tt_result_t tt_ssl_shutdown(IN tt_ssl_t *ssl, IN tt_ssl_shut_t shut)
{
    tt_skt_shut_t sd;

    if ((shut == TT_SSL_SHUT_WR) || (shut == TT_SSL_SHUT_RDWR)) {
        int e;
        while ((e = mbedtls_ssl_close_notify(&ssl->ctx)) != 0) {
            if ((e != MBEDTLS_ERR_SSL_WANT_READ) &&
                (e != MBEDTLS_ERR_SSL_WANT_WRITE)) {
                tt_ssl_error("ssl send fail");
                return TT_FAIL;
            }
        }
    }

    if (shut == TT_SSL_SHUT_RD) {
        sd = TT_SKT_SHUT_RD;
    } else if (shut == TT_SSL_SHUT_WR) {
        sd = TT_SKT_SHUT_WR;
    } else {
        sd = TT_SKT_SHUT_RDWR;
    }
    if (!TT_OK(tt_skt_shutdown(ssl->skt, sd))) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_ssl_config_t *tt_ssl_get_config(IN tt_ssl_t *ssl)
{
    return TT_CONTAINER(ssl->ctx.conf, tt_ssl_config_t, cfg);
}

tt_result_t tt_ssl_set_hostname(IN tt_ssl_t *ssl, IN const tt_char_t *hostname)
{
    int e;

    TT_ASSERT(ssl != NULL);
    TT_ASSERT(hostname != NULL);

    e = mbedtls_ssl_set_hostname(&ssl->ctx, hostname);
    if (e != 0) {
        tt_ssl_error("fail to set host name");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_ssl_set_ca(IN tt_ssl_t *ssl,
                   IN OPT tt_x509cert_t *ca,
                   IN OPT tt_x509crl_t *crl)
{
    TT_ASSERT(ssl != NULL);

    mbedtls_ssl_set_hs_ca_chain(&ssl->ctx,
                                TT_COND(ca != NULL, &ca->crt, NULL),
                                TT_COND(crl != NULL, &crl->crl, NULL));
}

tt_result_t tt_ssl_set_cert(IN tt_ssl_t *ssl,
                            IN tt_x509cert_t *cert,
                            IN tt_pk_t *pk)
{
    int e;

    TT_ASSERT(ssl != NULL);
    TT_ASSERT(cert != NULL);
    TT_ASSERT(cert != NULL);

    e = mbedtls_ssl_set_hs_own_cert(&ssl->ctx, &cert->crt, &pk->ctx);
    if (e != 0) {
        tt_ssl_error("fail to set ssl cert");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_ssl_set_auth(IN tt_ssl_t *ssl, IN tt_ssl_auth_t auth)
{
    TT_ASSERT(ssl != NULL);
    TT_ASSERT(TT_SSL_AUTH_VALID(auth));

    if (auth == TT_SSL_AUTH_NONE) {
        mbedtls_ssl_set_hs_authmode(&ssl->ctx, MBEDTLS_SSL_VERIFY_NONE);
    } else if (auth == TT_SSL_AUTH_OPTIONAL) {
        mbedtls_ssl_set_hs_authmode(&ssl->ctx, MBEDTLS_SSL_VERIFY_OPTIONAL);
    } else {
        mbedtls_ssl_set_hs_authmode(&ssl->ctx, MBEDTLS_SSL_VERIFY_REQUIRED);
    }
}

tt_result_t __ssl_component_init(IN tt_component_t *comp,
                                 IN tt_profile_t *profile)
{
    // todo: config mbedtls debug level according to profile
    mbedtls_debug_set_threshold(0);

    return TT_SUCCESS;
}

tt_result_t __ssl_log_component_init(IN tt_component_t *comp,
                                     IN tt_profile_t *profile)
{
    tt_loglyt_t *lyt;

    if (!TT_OK(tt_logmgr_create(&tt_g_ssl_logmgr, "SSL", NULL))) {
        TT_ERROR("fail to create ssl log manager");
        return TT_FAIL;
    }

    lyt = tt_loglyt_pattern_create("[${logger}] ${content}\n");
    if (lyt == NULL) {
        TT_ERROR("fail to create ssl log layout");
        return TT_FAIL;
    }
    tt_logmgr_set_layout(&tt_g_ssl_logmgr, TT_LOG_LEVEL_NUM, lyt);
    tt_loglyt_release(lyt);

    if (!TT_OK(tt_logmgr_io_default(&tt_g_ssl_logmgr))) {
        TT_ERROR("fail to set ssl log io");
        return TT_FAIL;
    }

    // enable all log, the actual log level is controlled
    // by mbedtls_debug_set_threshold()
    tt_logmgr_set_level(&tt_g_ssl_logmgr, TT_LOG_DEBUG);

    return TT_SUCCESS;
}

void __ssl_log_component_exit(IN tt_component_t *comp)
{
    tt_logmgr_destroy(&tt_g_ssl_logmgr);
}

int __ssl_send(void *ctx, const unsigned char *buf, size_t len)
{
    tt_ssl_t *ssl = (tt_ssl_t *)ctx;
    tt_u32_t sent;

    if (TT_OK(tt_skt_send(ssl->skt, (tt_u8_t *)buf, (tt_u32_t)len, &sent))) {
        return (int)sent;
    } else {
        return MBEDTLS_ERR_NET_SEND_FAILED;
    }
}

int __ssl_recv(void *ctx, unsigned char *buf, size_t len)
{
    tt_ssl_t *ssl = (tt_ssl_t *)ctx;
    tt_u32_t recvd;
    tt_result_t result;

    result = tt_skt_recv(ssl->skt,
                         (tt_u8_t *)buf,
                         (tt_u32_t)len,
                         &recvd,
                         ssl->p_fev,
                         ssl->p_tmr);
    if (TT_OK(result)) {
        return (int)recvd;
    } else if (result == TT_E_END) {
        return 0;
    } else {
        return MBEDTLS_ERR_NET_RECV_FAILED;
    }
}
