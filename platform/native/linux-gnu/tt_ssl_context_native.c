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

#include <tt_ssl_context_native.h>

#include <memory/tt_memory_alloc.h>
#include <misc/tt_assert.h>
#include <misc/tt_distinguished_name.h>
#include <network/ssl/tt_ssl.h>
#include <network/ssl/tt_ssl_context.h>

#include <tt_util_native.h>

#ifdef TT_PLATFORM_SSL_ENABLE

#include <openssl/err.h>
#include <openssl/objects.h>
#include <openssl/pem.h>
#include <openssl/pkcs12.h>
#include <openssl/pkcs7.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define TT_ASSERT_SCTX TT_ASSERT

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

// this functions guarantees cert and privkey are properly
// processed whether it returns TT_SUCCESS
typedef tt_result_t (*__import_cert_t)(IN TO X509 *cert,
                                       IN TO EVP_PKEY *privkey,
                                       IN STACK_OF(X509) * cert_sk,
                                       IN void *param);

typedef int (*__openssl_verify_cb_t)(int, X509_STORE_CTX *);

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __sslctx_add_cert(IN tt_sslctx_ntv_t *ctx,
                                     IN tt_ssl_cert_format_t cert_fmt,
                                     IN tt_blob_t *cert,
                                     IN tt_ssl_cert_attr_t *cert_attr,
                                     IN tt_ssl_privkey_format_t privkey_fmt,
                                     IN tt_blob_t *privkey,
                                     IN tt_ssl_privkey_attr_t *privkey_attr);
static tt_result_t __sslctx_add_ca(IN tt_sslctx_ntv_t *ctx,
                                   IN tt_ssl_cert_format_t ca_fmt,
                                   IN tt_blob_t *ca,
                                   IN tt_ssl_cert_attr_t *ca_attr);

static tt_result_t __cert_import_x509(IN tt_blob_t *cert_der,
                                      IN tt_ssl_cert_attr_t *cert_attr,
                                      IN __import_cert_t import_cert,
                                      IN void *import_param);
static tt_result_t __cert_import_pkcs7(IN tt_blob_t *cert_der,
                                       IN tt_ssl_cert_attr_t *cert_attr,
                                       IN __import_cert_t import_cert,
                                       IN void *import_param);
static tt_result_t __cert_import_pkcs12(IN tt_blob_t *cert_der,
                                        IN tt_ssl_cert_attr_t *cert_attr,
                                        IN __import_cert_t import_cert,
                                        IN void *import_param);
static tt_result_t __privkey_import_pkcs8(
    IN tt_blob_t *privkey_der,
    IN tt_ssl_privkey_attr_t *privkey_attr,
    IN __import_cert_t import_cert,
    IN void *import_param);
static tt_result_t __import_cert_node(IN TO X509 *cert,
                                      IN TO EVP_PKEY *privkey,
                                      IN STACK_OF(X509) * cert_sk,
                                      IN void *param);
static tt_result_t __import_cert_store(IN TO X509 *cert,
                                       IN TO EVP_PKEY *privkey,
                                       IN STACK_OF(X509) * cert_sk,
                                       IN void *param);

static void __sslctx_list_clean(IN tt_list_t *sc_list);
static tt_sslctx_node_t *__sslctx_list_find_dn(IN tt_list_t *sc_list,
                                               IN tt_distname_t *dn);

static void __sslctx_node_init(IN tt_sslctx_node_t *sctx_node);
static void __sslctx_node_clean(IN tt_sslctx_node_t *sctx_node);
static void __sslctx_node_destroy(IN tt_sslctx_node_t *sctx_node);

static void __sslctx_commit_clean(IN tt_sslctx_ntv_t *ctx);

static tt_result_t __openssl_ctx_config_verify(IN SSL_CTX *openssl_ctx,
                                               IN tt_sslctx_ntv_t *ctx);
static tt_result_t __openssl_ctx_config_cert(IN SSL_CTX *ctx,
                                             IN tt_sslctx_node_t *sctx_node);
static int __openssl_verify_cb(int preverify_ok, X509_STORE_CTX *x509_ctx);

static void __mv_x509_stack(IN STACK_OF(X509) * dst, IN STACK_OF(X509) * src);

static void __sslctx_show(IN tt_sslctx_ntv_t *ctx);
static void __sslctx_list_show(IN tt_list_t *sc_list);
static void __sslctx_node_show(IN tt_sslctx_node_t *sctx_node,
                               IN const tt_char_t *prefix);
static void __ssl_x509_store_show(IN X509_STORE *store);
static void __x509_show(IN X509 *cert, IN const tt_char_t *prefix);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_sslctx_create_ntv(IN tt_sslctx_ntv_t *ctx, IN tt_ssl_role_t role)
{
    ctx->role = role;

    // cert list
    tt_list_init(&ctx->cert_list);

    // ca list
    ctx->ca_store = NULL;

    ctx->openssl_ctx = NULL;
    ctx->cert_privkey = NULL;

    return TT_SUCCESS;
}

void tt_sslctx_destroy_ntv(IN tt_sslctx_ntv_t *ctx)
{
    __sslctx_list_clean(&ctx->cert_list);

    if (ctx->ca_store != NULL) {
        X509_STORE_free(ctx->ca_store);
    }

    __sslctx_commit_clean(ctx);
}

tt_result_t tt_sslctx_commit_ntv(IN tt_sslctx_ntv_t *ctx,
                                 IN tt_distname_t *dn,
                                 IN tt_u32_t flag)
{
    SSL_CTX *openssl_ctx = NULL;
    tt_sslctx_node_t *sctx_node;

    TT_ASSERT(ctx != NULL);

    //__sslctx_list_show(&ctx->cert_list);
    //__sslctx_list_show(&ctx->ca_list);

    // create ssl ctx
    if (ctx->role == TT_SSL_ROLE_CLIENT) {
        openssl_ctx = SSL_CTX_new(SSLv23_client_method());
    } else {
        openssl_ctx = SSL_CTX_new(SSLv23_server_method());
    }
    if (openssl_ctx == NULL) {
        __SSL_ERROR("fail to create ssl ctx");
        goto __cm_fail;
    }

    if (!TT_OK(__openssl_ctx_config_verify(openssl_ctx, ctx))) {
        goto __cm_fail;
    }

    // the cert to use, may be NULL when app is a ssl client
    sctx_node = __sslctx_list_find_dn(&ctx->cert_list, dn);
    if (sctx_node != NULL) {
        if (!TT_OK(__openssl_ctx_config_cert(openssl_ctx, sctx_node))) {
            goto __cm_fail;
        }
    }

    // re-commit data
    __sslctx_commit_clean(ctx);
    ctx->openssl_ctx = openssl_ctx;
    ctx->cert_privkey = sctx_node;

    __sslctx_show(ctx);

    return TT_SUCCESS;

__cm_fail:

    if (ctx != NULL) {
        SSL_CTX_free(openssl_ctx);
    }

    return TT_FAIL;
}

tt_result_t tt_sslctx_add_cert_ntv(IN tt_sslctx_ntv_t *ctx,
                                   IN tt_ssl_cert_format_t cert_fmt,
                                   IN tt_blob_t *cert,
                                   IN tt_ssl_cert_attr_t *cert_attr,
                                   IN tt_ssl_privkey_format_t privkey_fmt,
                                   IN tt_blob_t *privkey,
                                   IN tt_ssl_privkey_attr_t *privkey_attr)
{
    TT_ASSERT(ctx != NULL);

    // certificate data
    TT_ASSERT(TT_SSL_CERT_FMT_VALID(cert_fmt));
    TT_ASSERT(cert != NULL);
    TT_ASSERT(cert->addr != NULL);
    TT_ASSERT(cert->len > 0);
    TT_ASSERT(cert_attr != NULL);

    // private key data
    if (cert_fmt != TT_SSL_CERT_FMT_PKCS12) {
        TT_ASSERT(TT_SSL_PRIVKEY_FMT_VALID(privkey_fmt));
        TT_ASSERT(privkey != NULL);
        TT_ASSERT(privkey->addr != NULL);
        TT_ASSERT(privkey->len > 0);
        TT_ASSERT(privkey_attr != NULL);
    }

    // set it
    return __sslctx_add_cert(ctx,
                             cert_fmt,
                             cert,
                             cert_attr,
                             privkey_fmt,
                             privkey,
                             privkey_attr);
}

tt_result_t tt_sslctx_add_ca_ntv(IN tt_sslctx_ntv_t *ctx,
                                 IN tt_ssl_cert_format_t ca_fmt,
                                 IN tt_blob_t *ca,
                                 IN tt_ssl_cert_attr_t *ca_attr)
{
    TT_ASSERT(ctx != NULL);

    // certificate data
    TT_ASSERT(TT_SSL_CERT_FMT_VALID(ca_fmt));
    TT_ASSERT(ca != NULL);
    TT_ASSERT(ca->addr != NULL);
    TT_ASSERT(ca->len > 0);
    TT_ASSERT(ca_attr != NULL);

    // set it
    return __sslctx_add_ca(ctx, ca_fmt, ca, ca_attr);
}

static int __sslctx_verify(int ok, X509_STORE_CTX *ctx)
{
    tt_sslctx_t *sslctx =
        X509_STORE_CTX_get_ex_data(ctx, tt_g_x509_store_ctx_private_idx);
    tt_ssl_verify_t *verify = &sslctx->verify;

    if (!ok) {
        int cert_error = X509_STORE_CTX_get_error(ctx);
        switch (cert_error) {
            case X509_V_ERR_CERT_NOT_YET_VALID:
            case X509_V_ERR_CERT_HAS_EXPIRED: {
                if (verify->allow_expired_cert) {
                    ok = 1;
                }
            } break;
            case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT: {
                // find the issuer in store, but no root cert. otherwise
                // openssl return X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY
                X509 *x = X509_STORE_CTX_get_current_cert(ctx);
                if (x) {
                    __x509_show(x, "current cert: ");
                    ok = 1; // todo
                }
            } break;

            default: {
            } break;
        }
        TT_INFO("%serror %d at %d depth lookup:%s\n",
                ok ? "[ignored]" : "",
                cert_error,
                X509_STORE_CTX_get_error_depth(ctx),
                X509_verify_cert_error_string(cert_error));
    }

    return ok;
}

tt_result_t tt_sslctx_verify_ntv(IN tt_sslctx_ntv_t *ctx,
                                 IN tt_ssl_cert_format_t cert_fmt,
                                 IN tt_blob_t *cert,
                                 IN tt_ssl_cert_attr_t *cert_attr,
                                 IN OPT tt_distname_t *name)
{
    tt_sslctx_t *sslctx = TT_CONTAINER(ctx, tt_sslctx_t, sys_ctx);
    tt_result_t result = TT_FAIL;

    int idx;
    tt_sslctx_node_t sctx_node;
    X509_STORE_CTX *st_ctx = NULL;
    X509 *leaf = NULL;
    STACK_OF(X509) *chain = NULL;

    // dummy node
    __sslctx_node_init(&sctx_node);

    if (!TT_OK(__cert_import_x509(cert,
                                  cert_attr,
                                  __import_cert_node,
                                  &sctx_node))) {
        goto __v_out;
    }

    leaf = sk_X509_value(sctx_node.cert, 0);
    if (leaf == NULL) {
        goto __v_out;
    }
    (void)sk_X509_delete(sctx_node.cert, 0);

    chain = sctx_node.cert;

    st_ctx = X509_STORE_CTX_new();
    if (st_ctx == NULL) {
        TT_ERROR("fail to new x509 store ctx");
        goto __v_out;
    }

    do {
        X509_STORE *ca_store = SSL_CTX_get_cert_store(ctx->openssl_ctx);
        if (ca_store == NULL) {
            TT_ERROR("no x509 store in ssl ctx");
            goto __v_out;
        }

        X509_STORE_set_verify_cb_func(ca_store, __sslctx_verify);

        if (X509_STORE_CTX_init(st_ctx, ca_store, leaf, chain) != 1) {
            TT_ERROR("fail to init x509 store ctx");
            goto __v_out;
        }
    } while (0);

    if (X509_STORE_CTX_set_ex_data(st_ctx,
                                   tt_g_x509_store_ctx_private_idx,
                                   sslctx) != 1) {
        TT_ERROR("fail to new x509 store ctx");
        goto __v_out;
    }

    if (X509_verify_cert(st_ctx) != 1) {
        TT_ERROR("fail to build cert chain");
        goto __v_out;
    }
    result = TT_SUCCESS;

__v_out:

    if (leaf != NULL) {
        X509_free(leaf);
    }

    if (st_ctx != NULL) {
        X509_STORE_CTX_free(st_ctx);
    }

    __sslctx_node_clean(&sctx_node);

    return result;
}

tt_result_t __sslctx_add_cert(IN tt_sslctx_ntv_t *ctx,
                              IN tt_ssl_cert_format_t cert_fmt,
                              IN tt_blob_t *cert,
                              IN tt_ssl_cert_attr_t *cert_attr,
                              IN tt_ssl_privkey_format_t privkey_fmt,
                              IN tt_blob_t *privkey,
                              IN tt_ssl_privkey_attr_t *privkey_attr)
{
    tt_sslctx_node_t *sctx_node;
    tt_result_t result;

    sctx_node = (tt_sslctx_node_t *)tt_malloc(sizeof(tt_sslctx_node_t));
    if (sctx_node == NULL) {
        TT_ERROR("no mem for ctx node");
        return TT_FAIL;
    }
    __sslctx_node_init(sctx_node);

    // cert
    switch (cert_fmt) {
        case TT_SSL_CERT_FMT_X509: {
            result = __cert_import_x509(cert,
                                        cert_attr,
                                        __import_cert_node,
                                        sctx_node);
            if (!TT_OK(result)) {
                goto __uc_fail;
            }
            TT_ASSERT_SCTX(sctx_node->cert != NULL);
        } break;
        case TT_SSL_CERT_FMT_PKCS7: {
            result = __cert_import_pkcs7(cert,
                                         cert_attr,
                                         __import_cert_node,
                                         sctx_node);
            if (!TT_OK(result)) {
                goto __uc_fail;
            }
            TT_ASSERT_SCTX(sctx_node->cert != NULL);
        } break;
        case TT_SSL_CERT_FMT_PKCS12: {
            result = __cert_import_pkcs12(cert,
                                          cert_attr,
                                          __import_cert_node,
                                          sctx_node);
            if (!TT_OK(result)) {
                goto __uc_fail;
            }
            TT_ASSERT_SCTX(sctx_node->cert != NULL);
        } break;

        default: {
            TT_ERROR("invalid cert format: %d", cert_fmt);
            goto __uc_fail;
        } break;
    }

    // private key
    switch (privkey_fmt) {
        case TT_SSL_PRIVKEY_FMT_NONE: {
            // ignored
        } break;
        case TT_SSL_PRIVKEY_FMT_PKCS8: {
            result = __privkey_import_pkcs8(privkey,
                                            privkey_attr,
                                            __import_cert_node,
                                            sctx_node);
            if (!TT_OK(result)) {
                goto __uc_fail;
            }
            TT_ASSERT_SCTX(sctx_node->privkey != NULL);
        } break;

        default: {
            TT_ERROR("invalid privkey format: %d", privkey_fmt);
            goto __uc_fail;
        } break;
    }

    // add to cert list
    tt_list_addtail(&ctx->cert_list, &sctx_node->node);

    return TT_SUCCESS;

__uc_fail:

    if (sctx_node != NULL) {
        __sslctx_node_destroy(sctx_node);
    }

    return TT_FAIL;
}

tt_result_t __sslctx_add_ca(IN tt_sslctx_ntv_t *ctx,
                            IN tt_ssl_cert_format_t ca_fmt,
                            IN tt_blob_t *ca,
                            IN tt_ssl_cert_attr_t *ca_attr)
{
    tt_result_t result;

    if (ctx->ca_store == NULL) {
        ctx->ca_store = X509_STORE_new();
        if (ctx->ca_store == NULL) {
            TT_ERROR("fail to create ca store");
            return TT_FAIL;
        }
    }

    // cert
    switch (ca_fmt) {
        case TT_SSL_CERT_FMT_X509: {
            result = __cert_import_x509(ca,
                                        ca_attr,
                                        __import_cert_store,
                                        ctx->ca_store);
            if (!TT_OK(result)) {
                goto __uc_fail;
            }
        } break;
        case TT_SSL_CERT_FMT_PKCS7: {
            result = __cert_import_pkcs7(ca,
                                         ca_attr,
                                         __import_cert_store,
                                         ctx->ca_store);
            if (!TT_OK(result)) {
                goto __uc_fail;
            }
        } break;
        case TT_SSL_CERT_FMT_PKCS12: {
            result = __cert_import_pkcs12(ca,
                                          ca_attr,
                                          __import_cert_store,
                                          ctx->ca_store);
            if (!TT_OK(result)) {
                goto __uc_fail;
            }
        } break;

        default: {
            TT_ERROR("invalid ca format: %d", ca_fmt);
            goto __uc_fail;
        } break;
    }

    return TT_SUCCESS;

__uc_fail:

    if (ctx->ca_store == NULL) {
        X509_STORE_free(ctx->ca_store);
        ctx->ca_store = NULL;
    }

    return TT_FAIL;
}


tt_result_t __cert_import_x509(IN tt_blob_t *cert,
                               IN tt_ssl_cert_attr_t *cert_attr,
                               IN __import_cert_t import_cert,
                               IN void *import_param)
{
#if 0
    if (cert_attr->encrypted)
    {
        TT_ERROR("x509 cert can not be encrypted");
        return TT_FAIL;
    }
#endif

    if (cert_attr->pem_armor) {
        tt_char_t __backup;
        tt_u8_t *__cur_pos;

        // the pem content passed in is not assumed to be null
        // terminated, so we set the ending char to 0 before
        // strstr() and restore it before doing decoding
        __backup = cert->addr[cert->len - 1];
        cert->addr[cert->len - 1] = 0;

#define __PEM_TAG "-----BEGIN"
#define __PEM_TAG_LEN (sizeof(__PEM_TAG) - 1)

        __cur_pos = cert->addr;
        while ((__cur_pos = (tt_u8_t *)strstr((tt_char_t *)__cur_pos,
                                              __PEM_TAG)) != NULL) {
            BIO *bio = NULL;
            X509 *x = NULL;

            cert->addr[cert->len - 1] = __backup;

            TT_ASSERT_SCTX(__cur_pos >= cert->addr);
            TT_ASSERT_SCTX(cert->len >= (__cur_pos - cert->addr));
            bio = BIO_new_mem_buf(__cur_pos,
                                  cert->len - (__cur_pos - cert->addr));
            if (bio == NULL) {
                TT_ERROR("fail to new mem bio");
                return TT_FAIL;
            }

            cert->addr[cert->len - 1] = 0;

            if (cert_attr->encrypted) {
                x = PEM_read_bio_X509(bio,
                                      NULL,
                                      NULL,
                                      (void *)cert_attr->password);
            } else {
                x = PEM_read_bio_X509(bio, NULL, NULL, NULL);
            }
            BIO_free_all(bio);
            if (x == NULL) {
                __SSL_ERROR("fail to read pem cert");
                return TT_FAIL;
            }

            import_cert(x, NULL, NULL, import_param);

            // next part
            __cur_pos += __PEM_TAG_LEN;
        }
    } else {
        X509 *x = NULL;
        const unsigned char *cert_content;

        cert_content = (const unsigned char *)cert->addr;
        x = d2i_X509(NULL, &cert_content, cert->len);
        if (x == NULL) {
            __SSL_ERROR("fail to read pem cert");

            return TT_FAIL;
        }

        import_cert(x, NULL, NULL, import_param);
    }

    return TT_SUCCESS;
}

tt_result_t __cert_import_pkcs7(IN tt_blob_t *cert,
                                IN tt_ssl_cert_attr_t *cert_attr,
                                IN __import_cert_t import_cert,
                                IN void *import_param)
{
    PKCS7 *p7 = NULL;
    STACK_OF(X509) *sk = NULL;
    int nid;

#if 0
    if (cert_attr->encrypted)
    {
        TT_ERROR("pkcs7 cert can not be encrypted");
        return TT_FAIL;
    }
#endif

    if (cert_attr->pem_armor) {
        BIO *bio = NULL;

        bio = BIO_new_mem_buf(cert->addr, cert->len);
        if (bio == NULL) {
            TT_ERROR("fail to new mem bio");
            goto __p7i_fail;
        }

        if (cert_attr->encrypted) {
            p7 = PEM_read_bio_PKCS7(bio,
                                    NULL,
                                    NULL,
                                    (void *)cert_attr->password);
        } else {
            p7 = PEM_read_bio_PKCS7(bio, NULL, NULL, NULL);
        }
        BIO_free_all(bio);
    } else {
        p7 = d2i_PKCS7(NULL, (const unsigned char **)&cert->addr, cert->len);
    }
    if (p7 == NULL) {
        __SSL_ERROR("fail to read pem cert");
        goto __p7i_fail;
    }

    nid = OBJ_obj2nid(p7->type);
    if (nid == NID_pkcs7_signed) {
        sk = p7->d.sign->cert;
    } else if (nid == NID_pkcs7_signedAndEnveloped) {
        sk = p7->d.signed_and_enveloped->cert;
    } else {
        TT_ERROR("invalid pkcs7 nid: %d", nid);
        goto __p7i_fail;
    }

    import_cert(NULL, NULL, sk, import_param);

    PKCS7_free(p7);

    return TT_SUCCESS;

__p7i_fail:

    if (p7 != NULL) {
        PKCS7_free(p7);
    }

    return TT_FAIL;
}

tt_result_t __cert_import_pkcs12(IN tt_blob_t *cert,
                                 IN tt_ssl_cert_attr_t *cert_attr,
                                 IN __import_cert_t import_cert,
                                 IN void *import_param)
{
    PKCS12 *p12 = NULL;
    const unsigned char *cert_content;
    X509 *leaf = NULL;
    EVP_PKEY *privkey = NULL;
    STACK_OF(X509) *extra = NULL;
    int ret;

    if (cert_attr->pem_armor) {
        TT_ERROR("pkcs12 cert can not be PEM format");
        return TT_FAIL;
    }

    cert_content = (const unsigned char *)cert->addr;
    p12 = d2i_PKCS12(NULL, &cert_content, cert->len);
    if (p12 == NULL) {
        __SSL_ERROR("fail to read pkcs12 cert");
        goto __p12i_fail;
    }

    if (cert_attr->encrypted) {
        ret = PKCS12_parse(p12, cert_attr->password, &privkey, &leaf, &extra);
    } else {
        ret = PKCS12_parse(p12, NULL, &privkey, &leaf, &extra);
    }
    if (ret != 1) {
        __SSL_ERROR("fail to parse pkcs12");
        goto __p12i_fail;
    }

    import_cert(leaf, privkey, extra, import_param);

    sk_X509_pop_free(extra, X509_free);
    PKCS12_free(p12);

    return TT_SUCCESS;

__p12i_fail:

    if (leaf != NULL) {
        X509_free(leaf);
    }

    if (extra != NULL) {
        sk_X509_pop_free(extra, X509_free);
    }

    if (privkey != NULL) {
        EVP_PKEY_free(privkey);
    }

    if (p12 != NULL) {
        PKCS12_free(p12);
    }

    return TT_FAIL;
}

tt_result_t __import_cert_node(IN TO X509 *cert,
                               IN TO EVP_PKEY *privkey,
                               IN STACK_OF(X509) * cert_sk,
                               IN void *param)
{
    tt_sslctx_node_t *sctx_node = (tt_sslctx_node_t *)param;

    if (sctx_node->cert == NULL) {
        sctx_node->cert = sk_X509_new_null();
        if (sctx_node->cert == NULL) {
            TT_ERROR("fail to new x509 stack");

            X509_free(cert);
            EVP_PKEY_free(privkey);
            return TT_FAIL;
        }
    }

    if (cert != NULL) {
        sk_X509_push(sctx_node->cert, cert);
    }

    if (privkey != NULL) {
        // need free original sctx_node->privkey?

        sctx_node->privkey = privkey;
    }

    if (cert_sk != NULL) {
        __mv_x509_stack(sctx_node->cert, cert_sk);
    }

    return TT_SUCCESS;
}

tt_result_t __import_cert_store(IN TO X509 *cert,
                                IN TO EVP_PKEY *privkey,
                                IN STACK_OF(X509) * cert_sk,
                                IN void *param)
{
    X509_STORE *store = (X509_STORE *)param;

    if ((cert != NULL) && (X509_STORE_add_cert(store, cert) == 0)) {
        __SSL_ERROR("fail to add cert to store");

        X509_free(cert);
        EVP_PKEY_free(privkey);
        return TT_FAIL;
    }
    // X509_STORE_add_cert would add ref of cert
    X509_free(cert);

    if (privkey != NULL) {
        // cert store does not need private key
        EVP_PKEY_free(privkey);
    }

    if (cert_sk != NULL) {
        while (sk_X509_num(cert_sk) != 0) {
            X509 *x = sk_X509_value(cert_sk, 0);
            TT_ASSERT_SCTX(x != NULL);

            (void)sk_X509_delete(cert_sk, 0);

            if ((X509_STORE_add_cert(store, x) == 0)) {
                __SSL_ERROR("fail to add cert to store");
                // continue...
            }
            X509_free(x);
        }
    }

    return TT_SUCCESS;
}

tt_result_t __privkey_import_pkcs8(IN tt_blob_t *privkey_der,
                                   IN tt_ssl_privkey_attr_t *privkey_attr,
                                   IN __import_cert_t import_cert,
                                   IN void *import_param)
{
    // to be consist with other platforms, only support pkcs12
    return TT_FAIL;
}

void __sslctx_list_clean(IN tt_list_t *sc_list)
{
    tt_lnode_t *node;
    while ((node = tt_list_pophead(sc_list)) != NULL) {
        tt_sslctx_node_t *sctx_node =
            TT_CONTAINER(node, tt_sslctx_node_t, node);

        __sslctx_node_destroy(sctx_node);
    }
}

void __sslctx_list_show(IN tt_list_t *sc_list)
{
    tt_lnode_t *node = tt_list_head(sc_list);
    while (node != NULL) {
        tt_sslctx_node_t *sctx_node =
            TT_CONTAINER(node, tt_sslctx_node_t, node);

        TT_INFO("    ssl node: ");
        __sslctx_node_show(sctx_node, "     ");

        node = node->next;
    }
}

tt_sslctx_node_t *__sslctx_list_find_dn(IN tt_list_t *sc_list,
                                        IN tt_distname_t *dn)
{
    tt_lnode_t *node = tt_list_head(sc_list);

    if (dn != NULL) {
        node = tt_list_head(sc_list);
#if 0 // todo
        while (node != NULL)
        {
            // dump cert
            
            node = node->next;
        }
#else

#endif
    }
    // else use the head cert

    if (node != NULL) {
        return TT_CONTAINER(node, tt_sslctx_node_t, node);
    } else {
        return NULL;
    }
}

void __sslctx_node_init(IN tt_sslctx_node_t *sctx_node)
{
    tt_lnode_init(&sctx_node->node);
    sctx_node->cert = NULL;
    sctx_node->privkey = NULL;
}

void __sslctx_node_clean(IN tt_sslctx_node_t *sctx_node)
{
    if (sctx_node->cert != NULL) {
        sk_X509_pop_free(sctx_node->cert, X509_free);
        sctx_node->cert = NULL;
    }

    if (sctx_node->privkey != NULL) {
        EVP_PKEY_free(sctx_node->privkey);
        sctx_node->privkey = NULL;
    }
}

void __sslctx_node_destroy(IN tt_sslctx_node_t *sctx_node)
{
    __sslctx_node_clean(sctx_node);
    tt_free(sctx_node);
}

void __sslctx_node_show(IN tt_sslctx_node_t *sctx_node,
                        IN const tt_char_t *prefix)
{
    int i, n;

    if (sctx_node->cert != NULL) {
        n = sk_X509_num(sctx_node->cert);
        for (i = 0; i < n; ++i) {
            __x509_show(sk_X509_value(sctx_node->cert, i), prefix);
        }
    }

    // key
    if (sctx_node->privkey) {
        TT_INFO("%s has private key", prefix);
    }
}

void __sslctx_commit_clean(IN tt_sslctx_ntv_t *ctx)
{
    if (ctx->openssl_ctx != NULL) {
        SSL_CTX_free(ctx->openssl_ctx);
        ctx->openssl_ctx = NULL;
    }

    // the node is choosed from cert_list, no need to free it
    ctx->cert_privkey = NULL;
}

tt_result_t __openssl_ctx_config_cert(IN SSL_CTX *sys_ctx,
                                      IN tt_sslctx_node_t *sctx_node)
{
    X509 *x = NULL;
    int cert_num, i;

    TT_ASSERT_SCTX(sctx_node->cert != NULL);

    if (sctx_node->cert == NULL) {
        TT_ERROR("no cert");
        return TT_FAIL;
    }
    if (sctx_node->privkey == NULL) {
        TT_ERROR("no private key");
        return TT_FAIL;
    }

    // cert
    cert_num = sk_X509_num(sctx_node->cert);
    if (cert_num == 0) {
        TT_ERROR("no x509 cert");
        return TT_FAIL;
    }

    x = sk_X509_value(sctx_node->cert, 0);
    TT_ASSERT_SCTX(x != NULL);
    if (SSL_CTX_use_certificate(sys_ctx, x) != 1) {
        __SSL_ERROR("fail to use cert");
        return TT_FAIL;
    }

    while (sk_X509_num(sctx_node->cert) > 1) {
        x = sk_X509_value(sctx_node->cert, 1);
        TT_ASSERT_SCTX(x != NULL);

        // per openssl manual:
        //  The x509 certificate provided to SSL_CTX_add_extra_chain_cert()
        //  will be freed by the library when the SSL_CTX is destroyed. An
        //  application should not free the x509 object.
        (void)sk_X509_delete(sctx_node->cert, 1);

        if (SSL_CTX_add_extra_chain_cert(sys_ctx, x) != 1) {
            TT_WARN("fail to add extra chain cert");
        }
    }

    // key
    if (SSL_CTX_use_PrivateKey(sys_ctx, sctx_node->privkey) != 1) {
        __SSL_ERROR("fail to use private key");
        return TT_FAIL;
    }

    // check
    if (!SSL_CTX_check_private_key(sys_ctx)) {
        __SSL_ERROR("private key error");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t __openssl_ctx_config_verify(IN SSL_CTX *openssl_ctx,
                                        IN tt_sslctx_ntv_t *ctx)
{
    tt_sslctx_t *sslctx = TT_CONTAINER(ctx, tt_sslctx_t, sys_ctx);
    tt_ssl_verify_t *ssl_verify = &sslctx->verify;

    int mode = SSL_VERIFY_NONE;
    __openssl_verify_cb_t verify_cb = NULL;

    // verify callback
    if ((ctx->role == TT_SSL_ROLE_CLIENT) && (ssl_verify->verify_peer)) {
        mode = SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT;
        verify_cb = __openssl_verify_cb;
    }
    if ((ctx->role == TT_SSL_ROLE_SERVER) && (ssl_verify->verify_peer)) {
        mode = SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT;
        verify_cb = __openssl_verify_cb;
    }
    SSL_CTX_set_verify(openssl_ctx, mode, verify_cb);

    // verify cert
    if (ctx->ca_store != NULL) {
        SSL_CTX_set_cert_store(openssl_ctx, ctx->ca_store);

        // ownership of ctx->ca_store is passed to the sslctx
        ctx->ca_store = NULL;
    }

    return TT_SUCCESS;
}

static tt_result_t __ssl_verify_leaf(IN STACK_OF(X509) * cert_chain,
                                     IN tt_ssl_verify_t *ssl_verify)
{
    X509 *leaf;

    if ((cert_chain == NULL) || (sk_X509_num(cert_chain) == 0)) {
        return TT_FAIL;
    }
    leaf = sk_X509_value(cert_chain, 0);

    if (ssl_verify->common_name != NULL) {
        char *cn;
        int cn_len;

        cn_len = X509_NAME_get_text_by_NID(X509_get_subject_name(leaf),
                                           NID_commonName,
                                           NULL,
                                           0);
        if (cn_len <= 0) {
            __SSL_ERROR("fail to get common name len");
            return TT_FAIL;
        }

        cn = tt_malloc(cn_len + 2);
        if (cn == NULL) {
            TT_ERROR("fail to alloc cn mem");
            return TT_FAIL;
        }
        cn[cn_len + 1] = 0;

        X509_NAME_get_text_by_NID(X509_get_subject_name(leaf),
                                  NID_commonName,
                                  cn,
                                  cn_len + 1);

        // these two strings should already be null terminated
        if (strcmp(ssl_verify->common_name, cn) != 0) {
            tt_free(cn);
            return TT_FAIL;
        }

        tt_free(cn);
    }

    // all verified
    return TT_SUCCESS;
}

int __openssl_verify_cb(int preverify_ok, X509_STORE_CTX *x509_ctx)
{
    SSL *openssl;
    tt_ssl_t *ssl;
    tt_sslctx_t *sslctx;
    tt_ssl_verify_t *verify;
    int verify_ok = preverify_ok;

    openssl = X509_STORE_CTX_get_ex_data(x509_ctx,
                                         SSL_get_ex_data_X509_STORE_CTX_idx());
    if (openssl == NULL) {
        TT_ERROR("can not get ssl from x509 store ctx");
        return TT_FAIL;
    }

    ssl = SSL_get_ex_data(openssl, tt_g_ssl_private_idx);
    if (ssl == NULL) {
        TT_ERROR("can not get ssl ctx from openssl ctx");
        return TT_FAIL;
    }

    sslctx = ssl->sslctx;
    verify = &sslctx->verify;

    if (!ssl->leaf_cert_verified) {
        STACK_OF(X509) *cert_chain = X509_STORE_CTX_get1_chain(x509_ctx);
        if (!TT_OK(__ssl_verify_leaf(cert_chain, verify))) {
            return 0;
        }

        ssl->leaf_cert_verified = TT_TRUE;
    }

    if (!verify_ok) {
        int cert_error = X509_STORE_CTX_get_error(x509_ctx);
        switch (cert_error) {
            case X509_V_ERR_CERT_NOT_YET_VALID:
            case X509_V_ERR_CERT_HAS_EXPIRED: {
                if (verify->allow_expired_cert) {
                    verify_ok = 1;
                }
            } break;

            case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT: {
                // find the issuer in store, but no root(self-signed) cert.
                // note X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY is
                // returned if the chain highest cert is not from the store
                verify_ok = 1;
            } break;

            // more to be added

            default: {
            } break;
        }

        // debug info
        do {
            X509 *cur_cert = X509_STORE_CTX_get_current_cert(x509_ctx);
            if (cur_cert != NULL) {
                __x509_show(cur_cert, "cert verify error: ");
            }

            TT_INFO("%serror %d at %d depth lookup:%s\n",
                    verify_ok ? "[ignored] " : "",
                    cert_error,
                    X509_STORE_CTX_get_error_depth(x509_ctx),
                    X509_verify_cert_error_string(cert_error));
        } while (0);
    }

    return verify_ok;
}

void __mv_x509_stack(IN STACK_OF(X509) * dst, IN STACK_OF(X509) * src)
{
    while (sk_X509_num(src) != 0) {
        X509 *x = sk_X509_value(src, 0);
        TT_ASSERT_SCTX(x != NULL);
        (void)sk_X509_delete(src, 0);
        sk_X509_push(dst, x);
    }
}

void __sslctx_show(IN tt_sslctx_ntv_t *ctx)
{
    TT_INFO("ssl context:");

    if (ctx->role == TT_SSL_ROLE_CLIENT) {
        TT_INFO("  role: client");
    } else if (ctx->role == TT_SSL_ROLE_SERVER) {
        TT_INFO("  role: server");
    } else {
        TT_INFO("  role: any");
    }

    TT_INFO("  certificate:");
    __sslctx_list_show(&ctx->cert_list);

    TT_INFO("  CA:");
    __ssl_x509_store_show(ctx->ca_store);

    if (ctx->cert_privkey != NULL) {
        TT_INFO("  using:");
        __sslctx_node_show(ctx->cert_privkey, "   ");
    }
}

void __ssl_x509_store_show(IN X509_STORE *store)
{
    int i, n;

    if ((store == NULL) || (store->objs == NULL)) {
        return;
    }

    // no api to get cert in store, we have to access the
    // member, and this may cause compile error when openssl
    // upgrades
    n = sk_X509_OBJECT_num(store->objs);
    for (i = 0; i < n; ++i) {
        X509_OBJECT *obj = sk_X509_OBJECT_value(store->objs, i);
        if (obj->type == X509_LU_X509) {
            __x509_show(obj->data.x509, "    ");
        }
    }
}

void __x509_show(IN X509 *cert, IN const tt_char_t *prefix)
{
    tt_char_t sub[100 + 1] = {0};
    tt_char_t iss[100 + 1] = {0};

    X509_NAME_oneline(X509_get_subject_name(cert), sub, sizeof(sub) - 1);
    X509_NAME_oneline(X509_get_issuer_name(cert), iss, sizeof(iss) - 1);
    TT_INFO("%s [%s] issued by [%s]", prefix, sub, iss);
}

#endif
