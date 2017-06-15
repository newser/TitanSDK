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
        tt_ptrq_attr_t q_attr;

        tt_ptrq_attr_default(&q_attr);
        q_attr.ptr_per_frame = TT_MAX(q_attr.ptr_per_frame, attr->max_entries);

        tt_ptrq_init(&cache->q, &q_attr);

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

    cache->max_entries = attr->max_entries;

    return cache;
}

void tt_ssl_cache_destroy(IN tt_ssl_cache_t *cache)
{
    TT_ASSERT(cache != NULL);

    if (cache->mode == 0) {
        mbedtls_ssl_session *s;
        while ((s = (mbedtls_ssl_session *)tt_ptrq_pop(&cache->q)) != NULL) {
            mbedtls_ssl_session_free(s);
            tt_free(s);
        }
        tt_ptrq_destroy(&cache->q);
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
    tt_ptrq_t *q = &cache->q;
    mbedtls_ssl_session *s = ssl->ctx.session;

    tt_spinlock_acquire(&cache->lock);

    // here it means everytime client finished handshake, it
    // need check the whole cache, would it be a performance
    // bottleneck?

    if ((s == NULL) || (__find_cache(cache, s->id, s->id_len) != NULL)) {
        goto done;
    }

    if (tt_ptrq_count(q) >= cache->max_entries) {
        s = (mbedtls_ssl_session *)tt_ptrq_pop(q);
        mbedtls_ssl_session_free(s);
    }

    s = tt_malloc(sizeof(mbedtls_ssl_session));
    if (s == NULL) {
        goto done;
    }
    mbedtls_ssl_session_init(s);

    if (mbedtls_ssl_get_session(&ssl->ctx, s) != 0) {
        tt_free(s);
        goto done;
    }

    if (!TT_OK(tt_ptrq_push(&cache->q, s))) {
        mbedtls_ssl_session_free(s);
        tt_free(s);
        goto done;
    }

done:
    tt_spinlock_release(&cache->lock);
}

void tt_ssl_cache_resume(IN tt_ssl_cache_t *cache, IN tt_ssl_t *ssl)
{
    tt_ptrq_t *q = &cache->q;
    mbedtls_ssl_session *s;

    tt_spinlock_acquire(&cache->lock);

    s = (mbedtls_ssl_session *)tt_ptrq_tail(q);
    if (s != NULL) {
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

mbedtls_ssl_session *__find_cache(IN tt_ssl_cache_t *cache,
                                  IN tt_u8_t *id,
                                  IN tt_size_t len)
{
    tt_ptrq_iter_t i;
    mbedtls_ssl_session *s;

    tt_ptrq_iter(&cache->q, &i);
    while ((s = (mbedtls_ssl_session *)tt_ptrq_iter_next(&i)) != NULL) {
        if ((s->id_len == len) && (tt_memcmp(s->id, id, len) == 0)) {
            return s;
        }
    }
    return NULL;
}
