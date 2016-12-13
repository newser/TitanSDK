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
@file tt_ssl_context_native.h
@brief ssl context native

this file defines ssl context native APIs
*/

#ifndef __TT_SSL_CONTEXT_NATIVE__
#define __TT_SSL_CONTEXT_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <network/ssl/tt_ssl_def.h>

#ifdef TT_PLATFORM_SSL_ENABLE
#include <wincrypt.h>

// required by sspi.h
#ifdef SECURITY_KERNEL
#undef SECURITY_KERNEL
#endif
#define SECURITY_WIN32
#include <sspi.h>
#endif

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_distname_s;

typedef struct
{
    tt_ssl_role_t role;

#ifdef TT_PLATFORM_SSL_ENABLE
    // certificate
    HCERTSTORE cert_store;

    // ca
    HCERTSTORE ca_store;

    // committed data
    CredHandle hcred;
    PCCERT_CONTEXT cert;
    HCERTCHAINENGINE chain_engine;
#endif
} tt_sslctx_ntv_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_sslctx_create_ntv(IN tt_sslctx_ntv_t *ctx,
                                        IN tt_ssl_role_t role);

extern void tt_sslctx_destroy_ntv(IN tt_sslctx_ntv_t *ctx);

extern tt_result_t tt_sslctx_commit_ntv(IN tt_sslctx_ntv_t *ctx,
                                        IN struct tt_distname_s *dn,
                                        IN tt_u32_t flag);

extern tt_result_t tt_sslctx_add_cert_ntv(
    IN tt_sslctx_ntv_t *ctx,
    IN tt_ssl_cert_format_t cert_fmt,
    IN tt_blob_t *cert,
    IN tt_ssl_cert_attr_t *cert_attr,
    IN tt_ssl_privkey_format_t privkey_fmt,
    IN tt_blob_t *privkey,
    IN tt_ssl_privkey_attr_t *privkey_attr);

extern tt_result_t tt_sslctx_add_ca_ntv(IN tt_sslctx_ntv_t *ctx,
                                        IN tt_ssl_cert_format_t ca_fmt,
                                        IN tt_blob_t *ca,
                                        IN tt_ssl_cert_attr_t *ca_attr);

extern tt_result_t tt_sslctx_verify_ntv(IN tt_sslctx_ntv_t *ctx,
                                        IN tt_ssl_cert_format_t cert_fmt,
                                        IN tt_blob_t *cert,
                                        IN tt_ssl_cert_attr_t *cert_attr,
                                        IN OPT struct tt_distname_s *name);

#endif /* __TT_SSL_CONTEXT_NATIVE__ */
