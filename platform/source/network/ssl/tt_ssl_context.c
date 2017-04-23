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

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <network/ssl/tt_ssl_context.h>

#include <io/tt_file_system.h>
#include <misc/tt_assert.h>

#include <tt_ssl_context_native.h>

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

tt_result_t tt_sslctx_create(IN tt_sslctx_t *ctx,
                             IN tt_ssl_role_t role,
                             IN OPT tt_sslctx_attr_t *attr)
{
    tt_result_t result;

    TT_ASSERT(ctx != NULL);
    TT_ASSERT(TT_SSL_ROLE_VALID(role));

    // attr
    if (attr == NULL) {
        tt_sslctx_attr_default(&ctx->attr);
    } else {
        tt_memcpy(&ctx->attr, attr, sizeof(tt_sslctx_attr_t));
    }
    attr = &ctx->attr;

    // ssl cache
    result = tt_sslcache_create(&ctx->ssl_cache, &attr->cache_attr);
    if (!TT_OK(result)) {
        return TT_FAIL;
    }

    // verify
    tt_ssl_verify_init(&ctx->verify);

    // native
    result = tt_sslctx_create_ntv(&ctx->sys_ctx, role);
    if (!TT_OK(result)) {
        tt_sslcache_destroy(&ctx->ssl_cache);

        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_sslctx_destroy(IN tt_sslctx_t *ctx)
{
    TT_ASSERT(ctx != NULL);

    tt_sslcache_destroy(&ctx->ssl_cache);

    tt_sslctx_destroy_ntv(&ctx->sys_ctx);
}

void tt_sslctx_attr_default(IN tt_sslctx_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    tt_sslcache_attr_default(&attr->cache_attr);
}

tt_result_t tt_sslctx_add_cert_path(IN tt_sslctx_t *ctx,
                                    IN tt_ssl_cert_format_t cert_fmt,
                                    IN const tt_char_t *cert_path,
                                    IN tt_ssl_cert_attr_t *cert_attr,
                                    IN tt_ssl_privkey_format_t privkey_fmt,
                                    IN const tt_char_t *privkey_path,
                                    IN tt_ssl_privkey_attr_t *privkey_attr)
{
    tt_blob_t cert, privkey;
    tt_result_t result;

    TT_ASSERT(cert_path != NULL);
    TT_ASSERT(privkey_path != NULL);

    result = TT_FAIL; // tt_fcontent(cert_path, &cert);
    if (!TT_OK(result)) {
        TT_ERROR("fail to read cert file: %s", cert_path);
        return TT_FAIL;
    }

    result = TT_FAIL; // tt_fcontent(privkey_path, &privkey);
    if (!TT_OK(result)) {
        TT_ERROR("fail to read privkey file: %s", cert_path);

        tt_blob_destroy(&cert);
        return TT_FAIL;
    }

    result = tt_sslctx_add_cert_ntv(&ctx->sys_ctx,
                                    cert_fmt,
                                    &cert,
                                    cert_attr,
                                    privkey_fmt,
                                    &privkey,
                                    privkey_attr);
    tt_blob_destroy(&cert);
    tt_blob_destroy(&privkey);

    return result;
}

tt_result_t tt_sslctx_add_ca_path(IN tt_sslctx_t *ctx,
                                  IN tt_ssl_cert_format_t ca_fmt,
                                  IN const tt_char_t *ca_path,
                                  IN tt_ssl_cert_attr_t *ca_attr)
{
    tt_blob_t ca;
    tt_result_t result;

    TT_ASSERT(ca_path != NULL);

    result = TT_FAIL; // tt_fcontent(ca_path, &ca);
    if (!TT_OK(result)) {
        TT_ERROR("fail to read ca file: %s", ca_path);
        return TT_FAIL;
    }

    result = tt_sslctx_add_ca_ntv(&ctx->sys_ctx, ca_fmt, &ca, ca_attr);
    tt_blob_destroy(&ca);

    return result;
}

void tt_ssl_verify_init(IN tt_ssl_verify_t *ssl_verify)
{
    tt_memset(ssl_verify, 0, sizeof(tt_ssl_verify_t));
}
