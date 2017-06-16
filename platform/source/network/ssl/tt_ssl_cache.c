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

#include <network/ssl/tt_ssl_cache.h>

#include <crypto/tt_ctr_drbg.h>
#include <memory/tt_memory_alloc.h>
#include <network/ssl/tt_ssl.h>
#include <network/ssl/tt_ssl_config.h>

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

static int __ticket_write(void *p_ticket,
                          const mbedtls_ssl_session *session,
                          unsigned char *start,
                          const unsigned char *end,
                          size_t *tlen,
                          uint32_t *lifetime);

static int __ticket_parse(void *p_ticket,
                          mbedtls_ssl_session *session,
                          unsigned char *buf,
                          size_t len);

static int __get_cache(void *data, mbedtls_ssl_session *session);

static int __set_cache(void *data, const mbedtls_ssl_session *session);

static mbedtls_ssl_session *__find_cache(IN tt_ssl_cache_t *cache,
                                         IN tt_u8_t *id,
                                         IN tt_size_t len);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_ssl_cache_t *tt_ssl_cache_create(IN tt_ssl_config_t *sc,
                                    IN tt_bool_t use_ticket,
                                    IN OPT tt_ssl_cache_attr_t *attr)
{
    tt_ssl_cache_attr_t __attr;
    tt_ssl_cache_t *cache;
    mbedtls_ssl_config *cfg;

    TT_ASSERT(sc != NULL);

    if (attr == NULL) {
        tt_ssl_cache_attr_default(&__attr);
        attr = &__attr;
    }

    cache = tt_malloc(sizeof(tt_ssl_cache_t));
    if (cache == NULL) {
        TT_ERROR("no mem for ssl cache");
        return NULL;
    }

    if (!TT_OK(tt_spinlock_create(&cache->lock, NULL))) {
        TT_ERROR("fail to create cache lock");
        tt_free(cache);
        return NULL;
    }

    cfg = &sc->cfg;
    if (cfg->endpoint == MBEDTLS_SSL_IS_CLIENT) {
        mbedtls_ssl_session_init(&cache->s);

        mbedtls_ssl_conf_session_tickets(cfg,
                                         MBEDTLS_SSL_SESSION_TICKETS_ENABLED);

        cache->mode = 0;
    } else if (use_ticket) {
        mbedtls_ssl_ticket_context *t = &cache->t;
        int e;

        mbedtls_ssl_ticket_init(t);

        e = mbedtls_ssl_ticket_setup(t,
                                     tt_ctr_drbg,
                                     tt_current_ctr_drbg(),
                                     MBEDTLS_CIPHER_AES_256_GCM,
                                     attr->time_out);
        if (e != 0) {
            tt_ssl_error("fail to setup ssl ticket");
            tt_spinlock_destroy(&cache->lock);
            tt_free(cache);
            return NULL;
        }

        mbedtls_ssl_conf_session_tickets_cb(cfg,
                                            __ticket_write,
                                            __ticket_parse,
                                            cache);

        cache->mode = 1;
    } else {
        mbedtls_ssl_cache_context *c = &cache->c;

        mbedtls_ssl_cache_init(c);

        mbedtls_ssl_cache_set_timeout(c, (int)attr->time_out);

        mbedtls_ssl_cache_set_max_entries(c, (int)attr->max_entries);

        mbedtls_ssl_conf_session_cache(cfg, cache, __get_cache, __set_cache);

        cache->mode = 2;
    }

    return cache;
}

void tt_ssl_cache_destroy(IN tt_ssl_cache_t *cache)
{
    TT_ASSERT(cache != NULL);

    if (cache->mode == 0) {
        mbedtls_ssl_session_free(&cache->s);
    } else if (cache->mode == 1) {
        mbedtls_ssl_ticket_free(&cache->t);
    } else {
        mbedtls_ssl_cache_free(&cache->c);
    }

    tt_spinlock_destroy(&cache->lock);

    tt_free(cache);
}

void tt_ssl_cache_attr_default(IN tt_ssl_cache_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->time_out = MBEDTLS_SSL_CACHE_DEFAULT_TIMEOUT;

    attr->max_entries = MBEDTLS_SSL_CACHE_DEFAULT_MAX_ENTRIES;
}

void tt_ssl_cache_save(IN tt_ssl_cache_t *cache, IN tt_ssl_t *ssl)
{
    mbedtls_ssl_session *new_s, *cur_s;
    tt_bool_t save = TT_TRUE;

    new_s = ssl->ctx.session;
    if ((new_s == NULL) || ((new_s->id_len == 0) && (new_s->ticket_len == 0))) {
        return;
    }

    cur_s = &cache->s;

    tt_spinlock_acquire(&cache->lock);

    if ((new_s->id_len != 0) && (new_s->id_len == cur_s->id_len) &&
        (tt_memcmp(new_s->id, cur_s->id, new_s->id_len) == 0)) {
        save = TT_FALSE;
    }

    if ((new_s->ticket_len != 0) && (new_s->ticket_len == cur_s->ticket_len) &&
        (tt_memcmp(new_s->ticket, cur_s->ticket, new_s->ticket_len) == 0)) {
        save = TT_FALSE;
    }

    if (save) {
        mbedtls_ssl_session_free(cur_s);
        mbedtls_ssl_get_session(&ssl->ctx, cur_s);
    }

    tt_spinlock_release(&cache->lock);
}

void tt_ssl_cache_resume(IN tt_ssl_cache_t *cache, IN tt_ssl_t *ssl)
{
    mbedtls_ssl_session *s = &cache->s;

    tt_spinlock_acquire(&cache->lock);

    if ((s->id_len != 0) || (s->ticket_len != 0)) {
        mbedtls_ssl_set_session(&ssl->ctx, s);
    }

    tt_spinlock_release(&cache->lock);
}

int __ticket_write(void *p_ticket,
                   const mbedtls_ssl_session *session,
                   unsigned char *start,
                   const unsigned char *end,
                   size_t *tlen,
                   uint32_t *lifetime)
{
    tt_ssl_cache_t *cache = (tt_ssl_cache_t *)p_ticket;
    int e;

    tt_spinlock_acquire(&cache->lock);
    e = mbedtls_ssl_ticket_write(&cache->t,
                                 session,
                                 start,
                                 end,
                                 tlen,
                                 lifetime);
    tt_spinlock_release(&cache->lock);

    return e;
}

int __ticket_parse(void *p_ticket,
                   mbedtls_ssl_session *session,
                   unsigned char *buf,
                   size_t len)
{
    tt_ssl_cache_t *cache = (tt_ssl_cache_t *)p_ticket;
    int e;

    tt_spinlock_acquire(&cache->lock);
    e = mbedtls_ssl_ticket_parse(&cache->t, session, buf, len);
    tt_spinlock_release(&cache->lock);

    return e;
}

int __get_cache(void *data, mbedtls_ssl_session *session)
{
    tt_ssl_cache_t *cache = (tt_ssl_cache_t *)data;
    int e;

    tt_spinlock_acquire(&cache->lock);
    e = mbedtls_ssl_cache_get(&cache->c, session);
    tt_spinlock_release(&cache->lock);

    return e;
}

int __set_cache(void *data, const mbedtls_ssl_session *session)
{
    tt_ssl_cache_t *cache = (tt_ssl_cache_t *)data;
    int e;

    tt_spinlock_acquire(&cache->lock);
    e = mbedtls_ssl_cache_set(&cache->c, session);
    tt_spinlock_release(&cache->lock);

    return e;
}
