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
@file tt_ssl_context.h
@brief ssl context

this file defines ssl context APIs
*/

#ifndef __TT_SSL_CONTEXT__
#define __TT_SSL_CONTEXT__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <network/ssl/tt_ssl_cache.h>
#include <network/ssl/tt_ssl_def.h>

#include <tt_ssl_context_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_distname_s;

typedef struct
{
    tt_sslcache_attr_t cache_attr;
} tt_sslctx_attr_t;

typedef struct
{
    tt_bool_t verify_peer : 1;

    // allow_expired_cert can not take effect on ios
    tt_bool_t allow_expired_cert : 1;

    // ========================================
    // verify certificate fields
    // ========================================

    // verify common name:
    //  - assume only 1 common name in a cert
    //  - assume the common name is ASCII encoded
    //  - v3 externsion like altName is not supported yet
    tt_char_t *common_name;
} tt_ssl_verify_t;

typedef struct tt_sslctx_s
{
    tt_sslctx_attr_t attr;

    // the peer cache is somewhat different with the ssl session
    // cache, the key of the cache is constructed by peer values
    // such as peer ip address and port.
    tt_sslcache_t ssl_cache;

    tt_ssl_verify_t verify;

    tt_sslctx_ntv_t sys_ctx;
} tt_sslctx_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_sslctx_create(IN tt_sslctx_t *ctx,
                                    IN tt_ssl_role_t role,
                                    IN OPT tt_sslctx_attr_t *attr);

extern void tt_sslctx_destroy(IN tt_sslctx_t *ctx);

extern void tt_sslctx_attr_default(IN tt_sslctx_attr_t *attr);

tt_inline tt_result_t tt_sslctx_commit(IN tt_sslctx_t *ctx,
                                       IN struct tt_distname_s *dn,
                                       IN tt_u32_t flag)
{
    // commit operation generally means ssl_ctx has been modified,
    // so let's clean the ssl peer cache
    tt_sslcache_clear(&ctx->ssl_cache);

    return tt_sslctx_commit_ntv(&ctx->sys_ctx, dn, flag);
}
// flag
#define TT_SSL_CTX_LOAD_MY_CERT (1 << 0)
#define TT_SSL_CTX_LOAD_MY_CA (1 << 1)

// there can only be one certificate or one certifcate chain(with root)
//  - windows: must be a trusted or self signed.. todo
tt_inline tt_result_t tt_sslctx_add_cert(IN tt_sslctx_t *ctx,
                                         IN tt_ssl_cert_format_t cert_fmt,
                                         IN tt_blob_t *cert,
                                         IN tt_ssl_cert_attr_t *cert_attr,
                                         IN tt_ssl_privkey_format_t privkey_fmt,
                                         IN tt_blob_t *privkey,
                                         IN tt_ssl_privkey_attr_t *privkey_attr)
{
    return tt_sslctx_add_cert_ntv(&ctx->sys_ctx,
                                  cert_fmt,
                                  cert,
                                  cert_attr,
                                  privkey_fmt,
                                  privkey,
                                  privkey_attr);
}

extern tt_result_t tt_sslctx_add_cert_path(
    IN tt_sslctx_t *ctx,
    IN tt_ssl_cert_format_t cert_fmt,
    IN const tt_char_t *cert_path,
    IN tt_ssl_cert_attr_t *cert_attr,
    IN tt_ssl_privkey_format_t privkey_fmt,
    IN const tt_char_t *privkey_path,
    IN tt_ssl_privkey_attr_t *privkey_attr);

tt_inline tt_result_t tt_sslctx_add_ca(IN tt_sslctx_t *ctx,
                                       IN tt_ssl_cert_format_t ca_fmt,
                                       IN tt_blob_t *ca,
                                       IN tt_ssl_cert_attr_t *ca_attr)
{
    return tt_sslctx_add_ca_ntv(&ctx->sys_ctx, ca_fmt, ca, ca_attr);
}

extern tt_result_t tt_sslctx_add_ca_path(IN tt_sslctx_t *ctx,
                                         IN tt_ssl_cert_format_t ca_fmt,
                                         IN const tt_char_t *ca_path,
                                         IN tt_ssl_cert_attr_t *ca_attr);

tt_inline tt_result_t tt_sslctx_verify(IN tt_sslctx_t *ctx,
                                       IN tt_ssl_cert_format_t cert_fmt,
                                       IN tt_blob_t *cert,
                                       IN tt_ssl_cert_attr_t *cert_attr,
                                       IN OPT struct tt_distname_s *name)
{
    return tt_sslctx_verify_ntv(&ctx->sys_ctx, cert_fmt, cert, cert_attr, name);
}

extern void tt_ssl_verify_init(IN tt_ssl_verify_t *ssl_verify);

tt_inline void tt_sslctx_set_verify(IN tt_sslctx_t *ctx,
                                    IN tt_ssl_verify_t *verify)
{
    tt_memcpy(&ctx->verify, verify, sizeof(tt_ssl_verify_t));
}

#endif // __TT_SSL_CONTEXT__
