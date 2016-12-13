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
#include <misc/tt_base64.h>
#include <misc/tt_distinguished_name.h>
#include <network/ssl/tt_ssl.h>
#include <network/ssl/tt_ssl_context.h>

#include <tt_util_native.h>

#ifdef TT_PLATFORM_SSL_ENABLE

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define TT_ASSERT_SCTX TT_ASSERT

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

#if TT_ENV_OS_IS_IOS
// test purpose
int use_cert_verify_date;
double cert_verify_date;
#endif

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

static tt_result_t __cert_import(IN tt_ssl_cert_format_t cert_fmt,
                                 IN tt_blob_t *cert,
                                 IN tt_ssl_cert_attr_t *cert_attr,
                                 OUT tt_sslctx_node_t *sctx_node);
static tt_result_t __cert_import_x509der(IN tt_u8_t *cert,
                                         IN tt_u32_t cert_len,
                                         OUT CFMutableArrayRef cert_array);
static tt_result_t __cert_import_pkcs12(IN tt_blob_t *cert_der,
                                        IN tt_ssl_cert_attr_t *cert_attr,
                                        OUT tt_sslctx_node_t *sctx_node);
static tt_result_t __cert_import_node(IN tt_ssl_cert_format_t cert_fmt,
                                      IN tt_blob_t *cert,
                                      IN tt_ssl_cert_attr_t *cert_attr,
                                      IN tt_sslctx_node_t *sctx_node);

static tt_result_t __privkey_import_pkcs8(
    IN tt_blob_t *privkey_der,
    IN tt_ssl_privkey_attr_t *privkey_attr,
    OUT tt_sslctx_node_t *sctx_node);
static tt_result_t __privkey_import_node(IN tt_ssl_privkey_format_t privkey_fmt,
                                         IN tt_blob_t *privkey_der,
                                         IN tt_ssl_privkey_attr_t *privkey_attr,
                                         OUT tt_sslctx_node_t *sctx_node);

static void __sslctx_list_clean(IN tt_list_t *sc_list);
static tt_sslctx_node_t *__sslctx_list_find_dn(IN tt_list_t *sc_list,
                                               IN struct tt_distname_s *dn);

static void __sslctx_node_init(IN tt_sslctx_node_t *sctx_node);
static void __sslctx_node_clean(IN tt_sslctx_node_t *sctx_node);
static void __sslctx_node_destroy(IN tt_sslctx_node_t *sctx_node);

static void __sslctx_commit_clean(IN tt_sslctx_ntv_t *ctx);

#define __get_cert_subject(cert)                                               \
    CFStringCreateWithCString(NULL, "ios not support", kCFStringEncodingUTF8)
#define __get_cert_issuer(cert)                                                \
    CFStringCreateWithCString(NULL, "ios not support", kCFStringEncodingUTF8)

static const char *__cfstring_ptr(CFStringRef str);

static CFArrayRef __sslctx_gen_policies(IN tt_sslctx_ntv_t *ctx);

static void __sslctx_show(IN tt_sslctx_ntv_t *ctx);
static void __sslctx_list_show(IN tt_list_t *sc_list);
static void __sslctx_node_show(IN tt_sslctx_node_t *sctx_node,
                               IN const tt_char_t *prefix);
static tt_bool_t __cert_array_show(IN CFArrayRef ar,
                                   IN const tt_char_t *prefix);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_sslctx_create_ntv(IN tt_sslctx_ntv_t *ctx, IN tt_ssl_role_t role)
{
    ctx->role = role;

    // cert list
    tt_list_init(&ctx->cert_list);

    // ca list
    tt_list_init(&ctx->ca_list);

    ctx->cert_privkey = NULL;
    ctx->policies = NULL;

    return TT_SUCCESS;
}

void tt_sslctx_destroy_ntv(IN tt_sslctx_ntv_t *ctx)
{
    __sslctx_list_clean(&ctx->cert_list);
    __sslctx_list_clean(&ctx->ca_list);
    __sslctx_commit_clean(ctx);
}

tt_result_t tt_sslctx_commit_ntv(IN tt_sslctx_ntv_t *ctx,
                                 IN tt_distname_t *dn,
                                 IN tt_u32_t flag)
{
    tt_sslctx_node_t *sctx_node;
    CFArrayRef policies = NULL;

    TT_ASSERT(ctx != NULL);

    //__sslctx_list_show(&ctx->cert_list);
    //__sslctx_list_show(&ctx->ca_list);

    // the cert to use, may be NULL when app is a ssl client
    sctx_node = __sslctx_list_find_dn(&ctx->cert_list, dn);

    // generate policies
    policies = __sslctx_gen_policies(ctx);

    // re-commit data
    __sslctx_commit_clean(ctx);
    ctx->cert_privkey = sctx_node;
    ctx->policies = policies;

    __sslctx_show(ctx);

    return TT_SUCCESS;
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
    if (privkey_fmt != TT_SSL_PRIVKEY_FMT_NONE) {
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

tt_result_t tt_sslctx_verify_ntv(IN tt_sslctx_ntv_t *ctx,
                                 IN tt_ssl_cert_format_t cert_fmt,
                                 IN tt_blob_t *cert,
                                 IN tt_ssl_cert_attr_t *cert_attr,
                                 IN OPT tt_distname_t *name)
{
    tt_sslctx_node_t sctx_node;
    tt_result_t result;
    tt_lnode_t *node;

    // a dummy node
    __sslctx_node_init(&sctx_node);

    // cert
    result = __cert_import_node(cert_fmt, cert, cert_attr, &sctx_node);
    if (!TT_OK(result)) {
        return TT_FAIL;
    }

    result = TT_FAIL;

    // do verify
    node = tt_list_head(&ctx->ca_list);
    while (node != NULL) {
        tt_sslctx_node_t *__sc_node =
            TT_CONTAINER(node, tt_sslctx_node_t, node);
        CFMutableArrayRef ca_ar = __sc_node->cert;
        CFArrayRef anc;

        SecTrustRef trust = NULL;
        OSStatus osst;
        SecTrustResultType trust_result = kSecTrustResultOtherError;

        node = node->next;

        TT_CF_REF_SHOW(ctx->policies);
        osst = SecTrustCreateWithCertificates(sctx_node.cert,
                                              ctx->policies,
                                              &trust);
        if (osst != errSecSuccess) {
            // tt_osstatus_show(osst);
            continue;
        }

        TT_ASSERT(ctx->policies != NULL);
        SecTrustSetPolicies(trust, ctx->policies);

#if TT_ENV_OS_IS_IOS
        do {
            if (use_cert_verify_date) {
                CFDateRef date = CFDateCreate(NULL, cert_verify_date);
                SecTrustSetVerifyDate(trust, date);
                TT_CF_REF_SHOW(date);
                CFRelease(date);
            }
        } while (0);
#endif

        // set anchor cert
        osst = SecTrustSetAnchorCertificates(trust, ca_ar);
        if (osst != errSecSuccess) {
            CFRelease(trust);
            tt_osstatus_show(osst);
            continue;
        }
        SecTrustSetAnchorCertificatesOnly(trust, false);

        // todo: keychain, only anchor, date, ...

        // test
        TT_INFO("verify ssl cert");
        TT_INFO("  ca");
        __cert_array_show(ca_ar, "    ");
        TT_INFO("  cert");
        __cert_array_show(sctx_node.cert, "    ");

        // verify cert
        osst = SecTrustEvaluate(trust, &trust_result);
        CFRelease(trust);
        if (osst != errSecSuccess) {
            break;
        }

        switch (trust_result) {
            case kSecTrustResultProceed:
            case kSecTrustResultUnspecified: {
                result = TT_SUCCESS;
            } break;

            case kSecTrustResultRecoverableTrustFailure: {
            } break;

            default: {
            } break;
        }

        if (TT_OK(result)) {
            break;
        }
    }

    __sslctx_node_clean(&sctx_node);

    return result;
}

tt_result_t tt_sslctx_config_trust(IN tt_sslctx_t *ssl_ctx,
                                   IN SecTrustRef trust)
{
    tt_sslctx_ntv_t *ctx = &ssl_ctx->sys_ctx;
    tt_lnode_t *node;

    CFMutableArrayRef anchor_cert;
    OSStatus osst;

    // set anchor certs
    anchor_cert = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);
    if (anchor_cert == NULL) {
        TT_ERROR("fail to create anchor cert array");
        return TT_FAIL;
    }

    node = tt_list_head(&ctx->ca_list);
    while (node != NULL) {
        tt_sslctx_node_t *sctx_node =
            TT_CONTAINER(node, tt_sslctx_node_t, node);

        node = node->next;

        CFArrayAppendArray(anchor_cert,
                           sctx_node->cert,
                           CFRangeMake(0, CFArrayGetCount(sctx_node->cert)));
    }

    osst = SecTrustSetAnchorCertificates(trust, anchor_cert);
    TT_CF_REF_SHOW(anchor_cert);
    CFRelease(anchor_cert);
    if (osst != noErr) {
        TT_ERROR("fail to set anchor certs");
        tt_osstatus_show(osst);
        return TT_FAIL;
    }

    // on ios, it's not able to get subject name of cert, so we'll
    // use some pre-generated policies to verify peer name. see
    // __sslctx_gen_policies()
    osst = SecTrustSetPolicies(trust, ssl_ctx->sys_ctx.policies);
    if (osst != noErr) {
        TT_ERROR("fail to set anchor certs");
        tt_osstatus_show(osst);
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t __cert_import_node(IN tt_ssl_cert_format_t cert_fmt,
                               IN tt_blob_t *cert,
                               IN tt_ssl_cert_attr_t *cert_attr,
                               IN tt_sslctx_node_t *sctx_node)
{
    switch (cert_fmt) {
        case TT_SSL_CERT_FMT_X509: {
            return __cert_import(TT_SSL_CERT_FMT_X509,
                                 cert,
                                 cert_attr,
                                 sctx_node);
        } break;
        case TT_SSL_CERT_FMT_PKCS7: {
            return __cert_import(TT_SSL_CERT_FMT_PKCS7,
                                 cert,
                                 cert_attr,
                                 sctx_node);
        } break;
        case TT_SSL_CERT_FMT_PKCS12: {
            return __cert_import_pkcs12(cert, cert_attr, sctx_node);
        } break;

        default: {
            TT_ERROR("invalid cert format: %d", cert_fmt);
            return TT_FAIL;
        } break;
    }
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

    sctx_node = (tt_sslctx_node_t *)tt_mem_alloc(sizeof(tt_sslctx_node_t));
    if (sctx_node == NULL) {
        TT_ERROR("no mem for ctx node");
        return TT_FAIL;
    }
    __sslctx_node_init(sctx_node);

    // cert
    result = __cert_import_node(cert_fmt, cert, cert_attr, sctx_node);
    if (!TT_OK(result)) {
        goto __uc_fail;
    }

    // private key
    result =
        __privkey_import_node(privkey_fmt, privkey, privkey_attr, sctx_node);
    if (!TT_OK(result)) {
        goto __uc_fail;
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
    tt_sslctx_node_t *sctx_node;
    tt_result_t result;

    sctx_node = (tt_sslctx_node_t *)tt_mem_alloc(sizeof(tt_sslctx_node_t));
    if (sctx_node == NULL) {
        TT_ERROR("no mem for ctx node");
        return TT_FAIL;
    }
    __sslctx_node_init(sctx_node);

    // cert
    result = __cert_import_node(ca_fmt, ca, ca_attr, sctx_node);
    if (!TT_OK(result)) {
        goto __uc_fail;
    }

    // add to cert list
    tt_list_addtail(&ctx->ca_list, &sctx_node->node);

    return TT_SUCCESS;

__uc_fail:

    if (sctx_node != NULL) {
        __sslctx_node_destroy(sctx_node);
    }

    return TT_FAIL;
}

tt_result_t __cert_import(IN tt_ssl_cert_format_t cert_fmt,
                          IN tt_blob_t *cert,
                          IN tt_ssl_cert_attr_t *cert_attr,
                          OUT tt_sslctx_node_t *sctx_node)
{
    CFMutableArrayRef cert_ar = NULL;

    // on ios, it only support creating a x509 certificate
    if (cert_fmt != TT_SSL_CERT_FMT_X509) {
        TT_ERROR("ios only support x509 cert");
        return TT_FAIL;
    }

    cert_ar = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);
    if (cert_ar == NULL) {
        TT_ERROR("fail to create cert array");
        return TT_FAIL;
    }

    if (cert_attr->pem_armor) {
        tt_char_t *addr, *begin, *end;
        tt_result_t result = TT_SUCCESS;
        tt_char_t __backup;

        // the pem content passed in is not assumed to be null
        // terminated, so we set the ending char to 0 before
        // strstr() and restore it before doing decoding
        __backup = cert->addr[cert->len - 1];
        cert->addr[cert->len - 1] = 0;

#define __SBD "-----BEGIN CERTIFICATE-----"
#define __EBD "-----END CERTIFICATE-----"
        addr = (tt_char_t *)cert->addr;
        while (((begin = strstr(addr, __SBD)) != NULL) &&
               ((end = strstr(begin, __EBD)) != NULL)) {
            tt_u8_t *der;
            tt_u32_t der_len;

            // decode cert
            begin += sizeof(__SBD) - 1;
            TT_ASSERT(end > begin);
            result = tt_base64_decode((tt_u8_t *)begin,
                                      (tt_u32_t)(end - begin),
                                      NULL,
                                      NULL,
                                      &der_len);
            if (!TT_OK(result)) {
                TT_ERROR("fail to decode base64 certificate");
                break;
            }

            der = (tt_u8_t *)tt_mem_alloc(der_len);
            if (der == NULL) {
                TT_ERROR("no mem to do base64 decode");
                result = TT_FAIL;
                break;
            }

            tt_base64_decode((tt_u8_t *)begin,
                             (tt_u32_t)(end - begin),
                             NULL,
                             der,
                             &der_len);

            // import cert
            result = __cert_import_x509der(der, der_len, cert_ar);
            tt_mem_free(der);
            if (!TT_OK(result)) {
                break;
            }

            // next cert
            addr = end + sizeof(__EBD) - 1;
        }
        cert->addr[cert->len - 1] = __backup;

        if (!TT_OK(result)) {
            CFRelease(cert_ar);
            return TT_FAIL;
        }

        sctx_node->cert = cert_ar;
        return TT_SUCCESS;
    } else {
        if (!TT_OK(__cert_import_x509der(cert->addr, cert->len, cert_ar))) {
            CFRelease(cert_ar);
            return TT_FAIL;
        }

        sctx_node->cert = cert_ar;
        return TT_SUCCESS;
    }
}

tt_result_t __cert_import_x509der(IN tt_u8_t *cert,
                                  IN tt_u32_t cert_len,
                                  OUT CFMutableArrayRef cert_array)
{
    CFDataRef cert_data = NULL;
    SecCertificateRef cert_ref = NULL;

    cert_data = CFDataCreate(NULL, cert, cert_len);
    if (cert_data == NULL) {
        TT_ERROR("fail to create cert data");
        return TT_FAIL;
    }

    cert_ref = SecCertificateCreateWithData(NULL, cert_data);
    CFRelease(cert_data);
    if (cert_ref == NULL) {
        TT_ERROR("fail to create cert");
        return TT_FAIL;
    }

    CFArrayAppendValue(cert_array, cert_ref);
    CFRelease(cert_ref);
    return TT_SUCCESS;
}

tt_result_t __cert_import_pkcs12(IN tt_blob_t *cert,
                                 IN tt_ssl_cert_attr_t *cert_attr,
                                 OUT tt_sslctx_node_t *sctx_node)
{
    CFDataRef cert_data = NULL;
    CFMutableDictionaryRef options = NULL;
    CFArrayRef items = NULL;

    OSStatus osst;
    CFDictionaryRef items_dic = NULL;
    SecTrustRef trust = NULL;
    SecIdentityRef identity = NULL;
    CFIndex i, n;
    CFMutableArrayRef cert_array = NULL;

    if (cert_attr->pem_armor) {
        TT_ERROR("pkcs12 certificate can not be PEM encoded");
        return TT_FAIL;
    }

    // import pkcs12 certificate

    cert_data = CFDataCreate(NULL, cert->addr, cert->len);
    if (cert_data == NULL) {
        TT_ERROR("fail to create cert data");
        goto __p12_fail;
    }

    options = CFDictionaryCreateMutable(NULL,
                                        1, // only store password
                                        &kCFCopyStringDictionaryKeyCallBacks,
                                        &kCFTypeDictionaryValueCallBacks);
    if (options == NULL) {
        TT_ERROR("fail to create options");
        goto __p12_fail;
    }

    if ((cert_attr->encrypted) && (cert_attr->password != NULL) &&
        (cert_attr->password[0] != 0)) {
        CFStringRef password = CFStringCreateWithCString(NULL,
                                                         cert_attr->password,
                                                         kCFStringEncodingUTF8);
        if (password == NULL) {
            TT_ERROR("fail to create password");
            goto __p12_fail;
        }

        CFDictionaryAddValue(options, kSecImportExportPassphrase, password);
        TT_CF_REF_SHOW(password);
        CFRelease(password);
    }

    osst = SecPKCS12Import(cert_data, options, &items);
    if ((osst != errSecSuccess) || (items == NULL)) {
        TT_ERROR("fail to import pkcs12 cerfiticate");
        tt_osstatus_show(osst);
        goto __p12_fail;
    }

    // extract data and make them in expected format

    items_dic = CFArrayGetValueAtIndex(items, 0);
    if (items_dic == NULL) {
        TT_ERROR("fail to get pkcs12 certificate trust");
        goto __p12_fail;
    }

    trust = (SecTrustRef)CFDictionaryGetValue(items_dic, kSecImportItemTrust);
    if (trust == NULL) {
        TT_ERROR("fail to get pkcs12 certificate trust");
        goto __p12_fail;
    }

    identity =
        (SecIdentityRef)CFDictionaryGetValue(items_dic, kSecImportItemIdentity);
    if (identity == NULL) {
        TT_ERROR("fail to get pkcs12 certificate identity");
        goto __p12_fail;
    }

    cert_array = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);
    if (cert_array == NULL) {
        TT_ERROR("fail to create formatted certificate");
        goto __p12_fail;
    }

    // the begining element must be an identity
    CFArrayAppendValue(cert_array, identity);

    // following elements are parent cerfiticates
    n = SecTrustGetCertificateCount(trust);
    for (i = 1; i < n; ++i) {
        CFTypeID r = SecTrustGetCertificateAtIndex(trust, i);
        if (CFGetTypeID(r) == SecCertificateGetTypeID()) {
            CFArrayAppendValue(cert_array, r);
        }
    }

    CFRelease(items);
    CFRelease(options);
    CFRelease(cert_data);

    sctx_node->cert = cert_array;
    return TT_SUCCESS;

__p12_fail:

    if (cert_array != NULL) {
        CFRelease(cert_array);
    }

    if (items != NULL) {
        CFRelease(items);
    }

    if (options != NULL) {
        CFRelease(options);
    }

    if (cert_data != NULL) {
        CFRelease(cert_data);
    }

    return TT_FAIL;
}

tt_result_t __privkey_import_node(IN tt_ssl_privkey_format_t privkey_fmt,
                                  IN tt_blob_t *privkey,
                                  IN tt_ssl_privkey_attr_t *privkey_attr,
                                  OUT tt_sslctx_node_t *sctx_node)
{
    switch (privkey_fmt) {
        case TT_SSL_PRIVKEY_FMT_PKCS8: {
            return __privkey_import_pkcs8(privkey, privkey_attr, sctx_node);
        } break;
        case TT_SSL_PRIVKEY_FMT_NONE: {
            // ignored
            return TT_SUCCESS;
        } break;

        default: {
            TT_ERROR("invalid privkey format: %d", privkey_fmt);
            return TT_FAIL;
        } break;
    }
}

tt_result_t __privkey_import_pkcs8(IN tt_blob_t *privkey_der,
                                   IN tt_ssl_privkey_attr_t *privkey_attr,
                                   OUT tt_sslctx_node_t *sctx_node)
{
    // only support pkcs12
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
}

void __sslctx_node_clean(IN tt_sslctx_node_t *sctx_node)
{
    if (sctx_node->cert != NULL) {
        CFRelease(sctx_node->cert);
        sctx_node->cert = NULL;
    }
}

void __sslctx_node_destroy(IN tt_sslctx_node_t *sctx_node)
{
    __sslctx_node_clean(sctx_node);

    tt_mem_free(sctx_node);
}

void __sslctx_node_show(IN tt_sslctx_node_t *sctx_node,
                        IN const tt_char_t *prefix)
{
    tt_bool_t has_privkey = TT_FALSE;

    if (sctx_node->cert != NULL) {
        has_privkey = __cert_array_show(sctx_node->cert, prefix);
    }

    // key
    if (has_privkey) {
        TT_INFO("%s has private key", prefix);
    }
}

tt_bool_t __cert_array_show(IN CFArrayRef ar, IN const tt_char_t *prefix)
{
    CFIndex i, n;
    tt_bool_t has_privkey = TT_FALSE;

    n = CFArrayGetCount(ar);
    for (i = 0; i < n; ++i) {
        CFTypeRef r = CFArrayGetValueAtIndex(ar, i);
        SecCertificateRef cert = NULL;
        tt_bool_t release_cert = TT_FALSE;

        CFStringRef subject;
        char *subject_ptr = "fail";
        tt_u32_t subject_len = 0;

        CFStringRef issuer;
        char *issuer_ptr = "fail";
        tt_u32_t issuer_len = 0;

        if (CFGetTypeID(r) == SecCertificateGetTypeID()) {
            cert = (SecCertificateRef)r;
        } else if (CFGetTypeID(r) == SecIdentityGetTypeID()) {
            SecIdentityCopyCertificate((SecIdentityRef)r, &cert);

            if (cert == NULL) {
                continue;
            }
            release_cert = TT_TRUE;

            has_privkey = TT_TRUE;
        }

        subject = __get_cert_subject(cert);
        if (subject != NULL) {
            subject_ptr = tt_cfstring_ptr(subject, &subject_len);
            CFRelease(subject);
        }

        issuer = __get_cert_issuer(cert);
        if (issuer != NULL) {
            issuer_ptr = tt_cfstring_ptr(issuer, &issuer_len);
            CFRelease(issuer);
        }

        TT_INFO("%s [%s] issued by [%s]", prefix, subject_ptr, issuer_ptr);

        if (subject_ptr != NULL) {
            tt_mem_free(subject_ptr);
        }
        if (issuer_ptr != NULL) {
            tt_mem_free(issuer_ptr);
        }

        if (release_cert) {
            CFRelease(cert);
        }
    }

    return has_privkey;
}

void __sslctx_commit_clean(IN tt_sslctx_ntv_t *ctx)
{
    // the node is choosed from cert_list, no need to free it
    ctx->cert_privkey = NULL;

    if (ctx->policies != NULL) {
        CFRelease(ctx->policies);
        ctx->policies = NULL;
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
    __sslctx_list_show(&ctx->ca_list);

    if (ctx->cert_privkey != NULL) {
        TT_INFO("  using:");
        __sslctx_node_show(ctx->cert_privkey, "   ");
    }
}

CFArrayRef __sslctx_gen_policies(IN tt_sslctx_ntv_t *ctx)
{
    tt_sslctx_t *ssl_ctx = TT_CONTAINER(ctx, tt_sslctx_t, sys_ctx);
    tt_ssl_verify_t *ssl_verify = &ssl_ctx->verify;

    CFMutableArrayRef ar = NULL;
    Boolean server;
    SecPolicyRef policy = NULL;

    ar = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);
    if (ar == NULL) {
        TT_ERROR("fail to create policy array");
        return NULL;
    }

    if (ssl_ctx->sys_ctx.role == TT_SSL_ROLE_CLIENT) {
        server = false;
    } else {
        server = true;
    }

    if (ssl_verify->verify_peer && (ssl_verify->common_name != NULL) &&
        (ssl_verify->common_name[0] != 0)) {
        CFStringRef hostname =
            CFStringCreateWithCString(NULL,
                                      ssl_verify->common_name,
                                      kCFStringEncodingUTF8);
        if (hostname == NULL) {
            TT_ERROR("fail to create ssl host name str");

            CFRelease(ar);
            return NULL;
        }

        policy = SecPolicyCreateSSL(server, hostname);
        CFRelease(hostname);
    } else {
        policy = SecPolicyCreateSSL(server, NULL);
    }
    if (policy == NULL) {
        CFRelease(ar);
        return NULL;
    }
    CFArrayAppendValue(ar, policy);
    CFRelease(policy);

    return ar;
}

void __test_sec_trust(tt_blob_t *ca,
                      tt_u32_t ca_num,
                      tt_blob_t *cert,
                      tt_u32_t cert_num)
{
    CFArrayRef ca_ar;
    CFArrayRef cert_ar;
    SecTrustRef trust;
    SecPolicyRef policy;
    tt_u32_t i;
    SecTrustResultType result;
    CFTypeRef r[10];

    for (i = 0; i < cert_num; ++i) {
        CFDataRef data = CFDataCreate(NULL, cert[i].addr, cert[i].len);
        SecCertificateRef cert = SecCertificateCreateWithData(NULL, data);
        r[i] = cert;
        CFRelease(data);
    }
    cert_ar = CFArrayCreate(NULL, r, cert_num, &kCFTypeArrayCallBacks);

    for (i = 0; i < ca_num; ++i) {
        CFDataRef data = CFDataCreate(NULL, ca[i].addr, ca[i].len);
        SecCertificateRef cert = SecCertificateCreateWithData(NULL, data);
        r[i] = cert;
        CFRelease(data);
    }
    CFTypeRef certs[3] = {};
    ca_ar = CFArrayCreate(NULL, r, ca_num, &kCFTypeArrayCallBacks);

    policy = SecPolicyCreateBasicX509();
    // policy = SecPolicyCreateSSL(false, NULL);

    __cert_array_show(cert_ar, "--");
    __cert_array_show(ca_ar, "++");
    SecTrustCreateWithCertificates(cert_ar, policy, &trust);

    /*SecTrustSetOptions(trust,
                       kSecTrustOptionAllowExpired |
                       kSecTrustOptionLeafIsCA |
                       kSecTrustOptionAllowExpiredRoot);*/
    /*SecTrustSetOptions(trust,
                       kSecTrustOptionLeafIsCA |
                       kSecTrustOptionImplicitAnchors);*/
    // SecTrustSetAnchorCertificates(trust, ca_ar);
    // SecTrustSetAnchorCertificates(trust, cert_ar);

    SecTrustEvaluate(trust, &result);

    TT_INFO("trust result: %d", result);

    CFRelease(trust);
    CFRelease(policy);
    CFRelease(cert_ar);
    CFRelease(ca_ar);
}

#endif
