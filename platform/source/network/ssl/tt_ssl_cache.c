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

#include <memory/tt_memory_alloc.h>
#include <network/ssl/tt_ssl.h>

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

tt_ssl_cache_t *tt_ssl_cache_create(IN OPT tt_ssl_cache_attr_t *attr)
{
    tt_ssl_cache_attr_t __attr;
    tt_ssl_cache_t *cache;

    if (attr == NULL) {
        tt_ssl_cache_attr_default(&__attr);
        attr = &__attr;
    }

    cache = tt_malloc(sizeof(tt_ssl_cache_t));
    if (cache == NULL) {
        TT_ERROR("no mem for ssl cache");
        return NULL;
    }

    mbedtls_ssl_cache_init(&cache->ctx);
    mbedtls_ssl_cache_set_timeout(&cache->ctx, (int)attr->time_out);
    mbedtls_ssl_cache_set_max_entries(&cache->ctx, (int)attr->max_entries);

    if (!TT_OK(tt_spinlock_create(&cache->lock, NULL))) {
        TT_ERROR("fail to create cache lock");
        tt_free(cache);
        return NULL;
    }

    return cache;
}

void tt_ssl_cache_destroy(IN tt_ssl_cache_t *cache)
{
    TT_ASSERT(cache != NULL);

    mbedtls_ssl_cache_free(&cache->ctx);

    tt_spinlock_destroy(&cache->lock);

    tt_free(cache);
}

void tt_ssl_cache_attr_default(IN tt_ssl_cache_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->time_out = MBEDTLS_SSL_CACHE_DEFAULT_TIMEOUT;

    attr->max_entries = MBEDTLS_SSL_CACHE_DEFAULT_MAX_ENTRIES;
}

int tt_ssl_cache_get(void *data, mbedtls_ssl_session *session)
{
    tt_ssl_cache_t *cache = (tt_ssl_cache_t *)data;
    int e;

    tt_spinlock_acquire(&cache->lock);
    e = mbedtls_ssl_cache_get(&cache->ctx, session);
    tt_spinlock_release(&cache->lock);

    return e;
}

int tt_ssl_cache_set(void *data, const mbedtls_ssl_session *session)
{
    tt_ssl_cache_t *cache = (tt_ssl_cache_t *)data;
    int e;

    tt_spinlock_acquire(&cache->lock);
    e = mbedtls_ssl_cache_set(&cache->ctx, session);
    tt_spinlock_release(&cache->lock);

    return e;
}

void tt_ssl_resume(IN tt_ssl_t *ssl)
{
    tt_ssl_config_t *sc;
    mbedtls_ssl_cache_context *cache;

    // this function access the internal data of mbedtls_ssl_cache_context
    // as there is no proper api, may be modfified for future mbedtls versions

    sc = TT_CONTAINER(ssl->ctx.conf, tt_ssl_config_t, cfg);
    if ((sc->cfg.endpoint != MBEDTLS_SSL_IS_CLIENT) || (sc->cache == NULL)) {
        // only for cache enabled ssl client
        return;
    }

    tt_spinlock_acquire(&sc->cache->lock);
    cache = &sc->cache->ctx;
    if (cache->chain != NULL) {
        int e = mbedtls_ssl_set_session(&ssl->ctx, &cache->chain->session);
        if (e != 0) {
            tt_ssl_error("fail to set ssl session");
        }
    }
    tt_spinlock_release(&sc->cache->lock);
}
