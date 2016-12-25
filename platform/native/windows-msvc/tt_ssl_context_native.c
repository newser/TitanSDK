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
#include <network/ssl/tt_ssl_context.h>

#include <tt_ssl_native.h>
#include <tt_wchar.h>

#ifdef TT_PLATFORM_SSL_ENABLE

#include <schannel.h>
#include <wincrypt.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define TT_ASSERT_SCTX TT_ASSERT

// no official doc specified invalid value of CtxtHandle
#define __HCRED_INIT(hcred)                                                    \
    do {                                                                       \
        memcpy((hcred), &__s_hcred_invalid, sizeof(CredHandle));               \
    } while (0)
#define __HCRED_VALID(hcred)                                                   \
    (memcmp((hcred), &__s_hcred_invalid, sizeof(CredHandle)) != 0)
#define __HCRED_SET(hcred, new_hcred)                                          \
    do {                                                                       \
        memcpy((hcred), (new_hcred), sizeof(CredHandle));                      \
    } while (0)

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static CredHandle __s_hcred_invalid = {0};

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
                                      OUT HCERTSTORE *cert_store);
static tt_result_t __cert_import_pkcs7(IN tt_blob_t *cert_der,
                                       IN tt_ssl_cert_attr_t *cert_attr,
                                       IN HCERTSTORE cert_collection);
static tt_result_t __cert_import_pkcs12(IN tt_blob_t *cert_der,
                                        IN tt_ssl_cert_attr_t *cert_attr,
                                        OUT HCERTSTORE *cert_store);

static tt_result_t __privkey_import_pkcs8(
    IN tt_blob_t *privkey_der,
    IN tt_ssl_privkey_attr_t *privkey_attr,
    OUT HCRYPTPROV *prov);

static void __cert_store_clean(IN HCERTSTORE cert_store);
// static
void __cert_store_show(IN HCERTSTORE cert_store, IN const tt_char_t *prefix);
static void __cert_show(IN PCCERT_CONTEXT cert, IN const tt_char_t *prefix);

static PCCERT_CONTEXT __cert_store_find_dn(IN HCERTSTORE cert_store,
                                           IN tt_distname_t *dn);
static PCCERT_CONTEXT __cert_store_find(IN HCERTSTORE cert_store,
                                        IN tt_u32_t flag);
#define __FIND_CERT_WITH_PRIVKEY (1 << 0)

static PCCERT_CONTEXT __find_cert_with_privkey(IN HCERTSTORE cert_store,
                                               IN tt_u32_t flag);

static void __sslctx_show(IN tt_sslctx_ntv_t *ctx);

static void __sslctx_commit_clean(IN tt_sslctx_ntv_t *ctx);

static tt_result_t __config_schannel_cred(IN tt_sslctx_ntv_t *ctx,
                                          IN PCCERT_CONTEXT cert,
                                          OUT SCHANNEL_CRED *pAuthData);

static tt_result_t __base64_decode(IN tt_blob_t *base64_data,
                                   IN tt_u32_t reserved,
                                   OUT tt_blob_t *der_data);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_sslctx_create_ntv(IN tt_sslctx_ntv_t *ctx, IN tt_ssl_role_t role)
{
    tt_memset(ctx, 0, sizeof(tt_sslctx_ntv_t));

    ctx->role = role;

    // certificate
    ctx->cert_store = CertOpenStore(CERT_STORE_PROV_COLLECTION,
                                    0,
                                    (HCRYPTPROV_LEGACY)NULL,
                                    0,
                                    NULL);
    if (ctx->cert_store == NULL) {
        TT_ERROR_NTV("fail to create cert store");
        goto __scc_fail;
    }
#if 0
    if (flag & TT_SSL_CTX_LOAD_MY_CERT)
    {
        HCERTSTORE my_store = \
            CertOpenStore(CERT_STORE_PROV_SYSTEM_W,
                          0,
                          (HCRYPTPROV_LEGACY)NULL,
                          CERT_SYSTEM_STORE_CURRENT_USER,
                          L"My");
        if (my_store != NULL)
        {
            CertAddStoreToCollection(ctx->ca_store, my_store, 0, 0);
            CertCloseStore(my_store, 0);
        }
        else
        {
            TT_WARN("fail to load my cert store");
            //goto __scc_fail;
        }
    }
#endif

    // ca
    ctx->ca_store = CertOpenStore(CERT_STORE_PROV_COLLECTION,
                                  0,
                                  (HCRYPTPROV_LEGACY)NULL,
                                  0,
                                  NULL);
    if (ctx->ca_store == NULL) {
        TT_ERROR_NTV("fail to create ca store");
        goto __scc_fail;
    }
#if 0
    if (flag & TT_SSL_CTX_LOAD_MY_CA)
    {
        HCERTSTORE my_store = \
            CertOpenStore(CERT_STORE_PROV_SYSTEM_W,
                          0,
                          (HCRYPTPROV_LEGACY)NULL,
                          CERT_SYSTEM_STORE_CURRENT_USER,
                          L"My");
        if (my_store != NULL)
        {
            CertAddStoreToCollection(ctx->ca_store, my_store, 0, 0);
            CertCloseStore(my_store, 0);
        }
        else
        {
            TT_WARN("fail to load my ca store");
            //goto __scc_fail;
        }
    }
#endif

    // commited data
    __HCRED_INIT(&ctx->hcred);
    ctx->cert = NULL;
    ctx->chain_engine = NULL;

    return TT_SUCCESS;

__scc_fail:

    if (ctx->ca_store != NULL) {
        __cert_store_clean(ctx->ca_store);
    }

    if (ctx->cert_store != NULL) {
        __cert_store_clean(ctx->cert_store);
    }

    return TT_FAIL;
}

void tt_sslctx_destroy_ntv(IN tt_sslctx_ntv_t *ctx)
{
    __sslctx_commit_clean(ctx);

    __cert_store_clean(ctx->cert_store);
    __cert_store_clean(ctx->ca_store);
}

tt_result_t tt_sslctx_commit_ntv(IN tt_sslctx_ntv_t *ctx,
                                 IN tt_distname_t *dn,
                                 IN tt_u32_t flag)
{
    SECURITY_STATUS sec_st;

    CredHandle hcred;
    ULONG fCredentialUse;
    SCHANNEL_CRED AuthData;
    PCCERT_CONTEXT cert = NULL;

    HCERTCHAINENGINE chain_engine = NULL;
    CERT_CHAIN_ENGINE_CONFIG Config;

    TT_ASSERT(ctx != NULL);

    //__cert_store_show(ctx->cert_store);
    //__cert_store_show(ctx->ca_store);

    // credential
    __HCRED_INIT(&hcred);

    if (ctx->role == TT_SSL_ROLE_CLIENT) {
        fCredentialUse = SECPKG_CRED_OUTBOUND;
    } else {
        fCredentialUse = SECPKG_CRED_INBOUND;
    }

    cert = __cert_store_find_dn(ctx->cert_store, dn);
    if (!TT_OK(__config_schannel_cred(ctx, cert, &AuthData))) {
        goto __cm_fail;
    }

    sec_st = tt_g_sspi->AcquireCredentialsHandle(NULL,
                                                 UNISP_NAME,
                                                 fCredentialUse,
                                                 NULL,
                                                 &AuthData,
                                                 NULL,
                                                 NULL,
                                                 &hcred,
                                                 NULL);
    if (sec_st != SEC_E_OK) {
        TT_ERROR_NTV("fail to acquire cred");
        goto __cm_fail;
    }

    // cert chain engine
    memset(&Config, 0, sizeof(CERT_CHAIN_ENGINE_CONFIG));
    Config.cbSize = sizeof(CERT_CHAIN_ENGINE_CONFIG);
    Config.hExclusiveRoot = ctx->ca_store;
    Config.hExclusiveTrustedPeople = ctx->ca_store;

    if (!CertCreateCertificateChainEngine(&Config, &chain_engine)) {
        TT_ERROR_NTV("fail to create cert chain engine");
        goto __cm_fail;
    }

    // re-commit data
    __sslctx_commit_clean(ctx);
    __HCRED_SET(&ctx->hcred, &hcred);
    ctx->cert = cert;
    ctx->chain_engine = chain_engine;

    __sslctx_show(ctx);

    return TT_SUCCESS;

__cm_fail:

    if (chain_engine != NULL) {
        CertFreeCertificateChainEngine(chain_engine);
    }

    if (__HCRED_VALID(&hcred)) {
        tt_g_sspi->FreeCredentialsHandle(&hcred);
    }

    if (cert != NULL) {
        CertFreeCertificateContext(cert);
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
    tt_sslctx_t *ssl_ctx = TT_CONTAINER(ctx, tt_sslctx_t, sys_ctx);
    tt_ssl_verify_t *ssl_verify = &ssl_ctx->verify;

    HCERTSTORE new_store = NULL;
    PCCERT_CONTEXT pCertContext = NULL, last_cert = NULL;
    PCCERT_CHAIN_CONTEXT chain_context = NULL;

    CERT_CHAIN_PARA chain_para = {0};
    CERT_ENHKEY_USAGE EnhkeyUsage;
    CERT_USAGE_MATCH CertUsage;
    tt_bool_t result = TT_FALSE;
    tt_u32_t i, j;

    CERT_CHAIN_POLICY_PARA PolicyPara = {0};
    CERT_CHAIN_POLICY_STATUS PolicyStatus = {0};
    DWORD dwErrorStatus;

    // this is a test function for cert verification

    if (!TT_OK(__cert_import_x509(cert, cert_attr, &new_store))) {
        goto __v_done;
    }

    // build chain for each cert
    while ((pCertContext =
                CertEnumCertificatesInStore(new_store, pCertContext)) != NULL) {
        chain_para.cbSize = sizeof(CERT_CHAIN_PARA);

        EnhkeyUsage.cUsageIdentifier = 0;
        EnhkeyUsage.rgpszUsageIdentifier = NULL;
        CertUsage.dwType = USAGE_MATCH_TYPE_AND;
        CertUsage.Usage = EnhkeyUsage;
        chain_para.RequestedUsage = CertUsage;

        if (!CertGetCertificateChain(
                ctx->chain_engine,
                pCertContext,
                NULL,
                new_store,
                &chain_para,
                CERT_CHAIN_ENABLE_PEER_TRUST |
                    CERT_CHAIN_DISABLE_AUTH_ROOT_AUTO_UPDATE,
                0,
                &chain_context)) {
            goto __v_done;
        }

        for (i = 0; i < chain_context->cChain; ++i) {
            PCERT_SIMPLE_CHAIN sch = chain_context->rgpChain[i];

            TT_INFO("\nsimple chain[%d]", i);
            for (j = 0; j < sch->cElement; ++j) {
                PCERT_CHAIN_ELEMENT e = sch->rgpElement[j];
                __cert_show(e->pCertContext, "  ");
            }
            TT_INFO("");
        }

        TT_INFO("Error status for the chain:");
        switch (chain_context->TrustStatus.dwErrorStatus) {
            case CERT_TRUST_NO_ERROR:
                TT_INFO("No error found for this certificate or chain.");
                break;
            case CERT_TRUST_IS_NOT_TIME_VALID:
                TT_INFO(
                    "This certificate or one of the certificates in the "
                    "certificate chain is not time-valid.");
                break;
            case CERT_TRUST_IS_NOT_TIME_NESTED:
                TT_INFO(
                    "Certificates in the chain are not properly "
                    "time-nested.");
                break;
            case CERT_TRUST_IS_REVOKED:
                TT_INFO(
                    "Trust for this certificate or one of the certificates "
                    "in the certificate chain has been revoked.");
                break;
            case CERT_TRUST_IS_NOT_SIGNATURE_VALID:
                TT_INFO(
                    "The certificate or one of the certificates in the "
                    "certificate chain does not have a valid signature.");
                break;
            case CERT_TRUST_IS_NOT_VALID_FOR_USAGE:
                TT_INFO(
                    "The certificate or certificate chain is not valid "
                    "in its proposed usage.");
                break;
            case CERT_TRUST_IS_UNTRUSTED_ROOT:
                TT_INFO(
                    "The certificate or certificate chain is based "
                    "on an untrusted root.");
                break;
            case CERT_TRUST_REVOCATION_STATUS_UNKNOWN:
                TT_INFO(
                    "The revocation status of the certificate or one of the"
                    "certificates in the certificate chain is unknown.");
                break;
            case CERT_TRUST_IS_CYCLIC:
                TT_INFO(
                    "One of the certificates in the chain was issued by a "
                    "certification authority that the original certificate "
                    "had certified.");
                break;
            case CERT_TRUST_IS_PARTIAL_CHAIN:
                TT_INFO("The certificate chain is not complete.");
                break;
            case CERT_TRUST_CTL_IS_NOT_TIME_VALID:
                TT_INFO("A CTL used to create this chain was not time-valid.");
                break;
            case CERT_TRUST_CTL_IS_NOT_SIGNATURE_VALID:
                TT_INFO(
                    "A CTL used to create this chain did not have a valid "
                    "signature.");
                break;
            case CERT_TRUST_CTL_IS_NOT_VALID_FOR_USAGE:
                TT_INFO(
                    "A CTL used to create this chain is not valid for this "
                    "usage.");
        } // End switch

        TT_INFO("Info status for the chain:");
        switch (chain_context->TrustStatus.dwInfoStatus) {
            case 0:
                TT_INFO("No information status reported.");
                break;
            case CERT_TRUST_HAS_EXACT_MATCH_ISSUER:
                TT_INFO(
                    "An exact match issuer certificate has been found for "
                    "this certificate.");
                break;
            case CERT_TRUST_HAS_KEY_MATCH_ISSUER:
                TT_INFO(
                    "A key match issuer certificate has been found for this "
                    "certificate.");
                break;
            case CERT_TRUST_HAS_NAME_MATCH_ISSUER:
                TT_INFO(
                    "A name match issuer certificate has been found for this "
                    "certificate.");
                break;
            case CERT_TRUST_IS_SELF_SIGNED:
                TT_INFO("This certificate is self-signed.");
                break;
            case CERT_TRUST_IS_COMPLEX_CHAIN:
                TT_INFO("The certificate chain created is a complex chain.");
                break;
        } // end switch

        /*
        PolicyPara.cbSize = sizeof(PolicyPara);
        PolicyStatus.cbSize = sizeof(PolicyStatus);
        if (CertVerifyCertificateChainPolicy(
                CERT_CHAIN_POLICY_BASE,
                chain_context,
                &PolicyPara,
                &PolicyStatus))
        {
            if (PolicyStatus.dwError == S_OK)
            {
                result = TT_TRUE;
            }
        }
        */
        dwErrorStatus = chain_context->TrustStatus.dwErrorStatus;
        if ((dwErrorStatus == CERT_TRUST_NO_ERROR) ||
            ((dwErrorStatus & CERT_TRUST_IS_PARTIAL_CHAIN) &&
             (chain_context->TrustStatus.dwInfoStatus &
              CERT_TRUST_IS_FROM_EXCLUSIVE_TRUST_STORE))) {
            dwErrorStatus &= ~CERT_TRUST_IS_PARTIAL_CHAIN;
        }
        if (dwErrorStatus & CERT_TRUST_IS_NOT_TIME_VALID) {
            if (ssl_verify->allow_expired_cert) {
                dwErrorStatus &= ~CERT_TRUST_IS_NOT_TIME_VALID;
            }
        }

        if (dwErrorStatus == CERT_TRUST_NO_ERROR) {
            result |= TT_TRUE;
        } else {
            result &= TT_FALSE;
        }

        CertFreeCertificateChain(chain_context);
    }

__v_done:

    if (pCertContext != NULL) {
        CertFreeCertificateContext(pCertContext);
    }

    if (new_store != NULL) {
        CertCloseStore(new_store, 0);
    }

    if (result)
        return TT_SUCCESS;
    else
        return TT_FAIL;
}

tt_result_t __sslctx_add_cert(IN tt_sslctx_ntv_t *ctx,
                              IN tt_ssl_cert_format_t cert_fmt,
                              IN tt_blob_t *cert,
                              IN tt_ssl_cert_attr_t *cert_attr,
                              IN tt_ssl_privkey_format_t privkey_fmt,
                              IN tt_blob_t *privkey,
                              IN tt_ssl_privkey_attr_t *privkey_attr)
{
    HCERTSTORE new_store = NULL;
    tt_result_t result;

    // cert
    switch (cert_fmt) {
        case TT_SSL_CERT_FMT_X509: {
            result = __cert_import_x509(cert, cert_attr, &new_store);
            if (!TT_OK(result)) {
                goto __uc_fail;
            }
            TT_ASSERT_SCTX(new_store != NULL);
        } break;
        case TT_SSL_CERT_FMT_PKCS7: {
            result = __cert_import_pkcs7(cert, cert_attr, ctx->cert_store);
            if (!TT_OK(result)) {
                goto __uc_fail;
            }
        } break;
        case TT_SSL_CERT_FMT_PKCS12: {
            result = __cert_import_pkcs12(cert, cert_attr, &new_store);
            if (!TT_OK(result)) {
                goto __uc_fail;
            }
            TT_ASSERT_SCTX(new_store != NULL);
        } break;

        default: {
            TT_ERROR("invalid cert format: %d", cert_fmt);
            goto __uc_fail;
        } break;
    }

    // private key
    switch (privkey_fmt) {
        case TT_SSL_PRIVKEY_FMT_NONE: {
        } break;
        case TT_SSL_PRIVKEY_FMT_PKCS8: {
            HCRYPTPROV cprov = (HCRYPTPROV)NULL;
            PCCERT_CONTEXT leaf_cert = NULL;

            // currently import pkcs8 is not supported, so it always
            // return failure
            result = __privkey_import_pkcs8(privkey, privkey_attr, &cprov);
            if (!TT_OK(result)) {
                goto __uc_fail;
            }
            // TT_ASSERT_SCTX(cprov != NULL);

            leaf_cert = __find_cert_with_privkey(new_store, 0);
            if (leaf_cert == NULL) {
                CryptReleaseContext(cprov, 0);
                goto __uc_fail;
            }

            // bind private key to leaf certificate
            if (!CertSetCertificateContextProperty(leaf_cert,
                                                   CERT_KEY_PROV_HANDLE_PROP_ID,
                                                   0,
                                                   &cprov)) {
                CertFreeCertificateContext(leaf_cert);
                CryptReleaseContext(cprov, 0);

                TT_ERROR_NTV("fail to bind private key");
                goto __uc_fail;
            }
            CertFreeCertificateContext(leaf_cert);

            // MSDN:
            //   If CERT_STORE_NO_CRYPT_RELEASE_FLAG is not set,
            //   the hCryptProv value is implicitly released either
            //   when the property is set to NULL or on the final
            //   freeing of the CERT_CONTEXT structure
        } break;

        default: {
            TT_ERROR("invalid privkey format: %d", privkey_fmt);
            goto __uc_fail;
        } break;
    }

    // add to cert store
    if (new_store != NULL) {
        if (!CertAddStoreToCollection(ctx->cert_store, new_store, 0, 1)) {
            TT_ERROR("fail to add new store");
            goto __uc_fail;
        }
        CertCloseStore(new_store, 0);
        new_store = NULL;
    }

    return TT_SUCCESS;

__uc_fail:

    if (new_store != NULL) {
        CertCloseStore(new_store, 0);
    }

    return TT_FAIL;
}

tt_result_t __sslctx_add_ca(IN tt_sslctx_ntv_t *ctx,
                            IN tt_ssl_cert_format_t ca_fmt,
                            IN tt_blob_t *ca,
                            IN tt_ssl_cert_attr_t *ca_attr)
{
    HCERTSTORE new_store = NULL;
    tt_result_t result;

    // cert
    switch (ca_fmt) {
        case TT_SSL_CERT_FMT_X509: {
            result = __cert_import_x509(ca, ca_attr, &new_store);
            if (!TT_OK(result)) {
                goto __uc_fail;
            }
            TT_ASSERT_SCTX(new_store != NULL);
        } break;
        case TT_SSL_CERT_FMT_PKCS7: {
            result = __cert_import_pkcs7(ca, ca_attr, ctx->ca_store);
            if (!TT_OK(result)) {
                goto __uc_fail;
            }
        } break;
        case TT_SSL_CERT_FMT_PKCS12: {
            result = __cert_import_pkcs12(ca, ca_attr, &new_store);
            if (!TT_OK(result)) {
                goto __uc_fail;
            }
            TT_ASSERT_SCTX(new_store != NULL);
        } break;

        default: {
            TT_ERROR("invalid ca format: %d", ca_fmt);
            goto __uc_fail;
        } break;
    }

    // add to cert store
    if (new_store != NULL) {
        if (!CertAddStoreToCollection(ctx->ca_store, new_store, 0, 1)) {
            TT_ERROR("fail to add new store");
            goto __uc_fail;
        }
        CertCloseStore(new_store, 0);
        new_store = NULL;
    }

    return TT_SUCCESS;

__uc_fail:

    if (new_store != NULL) {
        CertCloseStore(new_store, 0);
    }

    return TT_FAIL;
}


tt_result_t __cert_import_x509(IN tt_blob_t *cert,
                               IN tt_ssl_cert_attr_t *cert_attr,
                               OUT HCERTSTORE *cert_store)
{
    HCERTSTORE __store = NULL;

    __store = CertOpenStore(CERT_STORE_PROV_MEMORY,
                            X509_ASN_ENCODING,
                            (HCRYPTPROV)NULL,
                            CERT_STORE_CREATE_NEW_FLAG,
                            NULL);
    if (__store == NULL) {
        TT_ERROR("fail to create cert store");
        goto __xi_fail;
    }

    if (cert_attr->pem_armor) {
        tt_char_t __backup;
        tt_char_t *__cur_pos;

        // the pem content passed in is not assumed to be null
        // terminated, so we set the ending char to 0 before
        // strstr() and restore it before doing decoding
        __backup = cert->addr[cert->len - 1];
        cert->addr[cert->len - 1] = 0;

#define __PEM_TAG "-----BEGIN"
#define __PEM_TAG_LEN (sizeof(__PEM_TAG) - 1)

        __cur_pos = (tt_char_t *)cert->addr;
        while ((__cur_pos = strstr(__cur_pos, __PEM_TAG)) != NULL) {
            tt_blob_t __cur_cert;
            tt_blob_t __cur_cert_der;
            BOOL ret;

            __cur_cert.addr = (tt_u8_t *)__cur_pos;
            __cur_cert.len =
                cert->len - (tt_u32_t)TT_PTR_DIFF(cert->addr, __cur_pos);

            cert->addr[cert->len - 1] = __backup;
            if (!TT_OK(__base64_decode(&__cur_cert, 0, &__cur_cert_der))) {
                goto __xi_fail;
            }
            cert->addr[cert->len - 1] = 0;

            ret = CertAddEncodedCertificateToStore(__store,
                                                   X509_ASN_ENCODING,
                                                   __cur_cert_der.addr,
                                                   __cur_cert_der.len,
                                                   CERT_STORE_ADD_NEW,
                                                   NULL);
            tt_free(__cur_cert_der.addr);
            if (!ret) {
                TT_ERROR_NTV("fail to add cert to store");
                goto __xi_fail;
            }

            // next part
            __cur_pos += __PEM_TAG_LEN;
        }

        *cert_store = __store;
        return TT_SUCCESS;
    } else {
        if (!CertAddEncodedCertificateToStore(__store,
                                              X509_ASN_ENCODING,
                                              cert->addr,
                                              cert->len,
                                              CERT_STORE_ADD_NEW,
                                              NULL)) {
            TT_ERROR_NTV("fail to add cert to store");
            goto __xi_fail;
        }

        *cert_store = __store;
        return TT_SUCCESS;
    }

__xi_fail:

    if (__store != NULL) {
        CertCloseStore(__store, 0);
    }

    return TT_FAIL;
}

tt_result_t __cert_import_pkcs7(IN tt_blob_t *cert,
                                IN tt_ssl_cert_attr_t *cert_attr,
                                IN HCERTSTORE cert_collection)
{
    if (cert_attr->pem_armor) {
        tt_char_t __backup;
        tt_char_t *__cur_pos;

        // the pem content passed in is not assumed to be null
        // terminated, so we set the ending char to 0 before
        // strstr() and restore it before doing decoding
        __backup = cert->addr[cert->len - 1];
        cert->addr[cert->len - 1] = 0;

#define __PEM_TAG "-----BEGIN"
#define __PEM_TAG_LEN (sizeof(__PEM_TAG) - 1)

        __cur_pos = (tt_char_t *)cert->addr;
        while ((__cur_pos = strstr(__cur_pos, __PEM_TAG)) != NULL) {
            tt_blob_t __cur_cert;
            tt_blob_t __cur_cert_der;

            HCERTSTORE __store = NULL;
            CERT_BLOB cert_blob;
            DWORD dwExpectedContentTypeFlags;
            BOOL ret;

            __cur_cert.addr = (tt_u8_t *)__cur_pos;
            __cur_cert.len =
                cert->len - (tt_u32_t)TT_PTR_DIFF(cert->addr, __cur_pos);

            cert->addr[cert->len - 1] = __backup;
            if (!TT_OK(__base64_decode(&__cur_cert, 0, &__cur_cert_der))) {
                continue;
            }
            cert->addr[cert->len - 1] = 0;

            cert_blob.cbData = __cur_cert_der.len;
            cert_blob.pbData = __cur_cert_der.addr;
            dwExpectedContentTypeFlags =
                CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED |
                CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED_EMBED |
                CERT_QUERY_CONTENT_FLAG_PKCS7_UNSIGNED;
            ret = CryptQueryObject(CERT_QUERY_OBJECT_BLOB,
                                   &cert_blob,
                                   dwExpectedContentTypeFlags,
                                   CERT_QUERY_FORMAT_FLAG_ALL,
                                   0,
                                   NULL,
                                   NULL,
                                   NULL,
                                   &__store,
                                   NULL,
                                   NULL);
            tt_free(__cur_cert_der.addr);

            if (ret && (__store != NULL)) {
                __cert_store_show(__store, "");

                CertAddStoreToCollection(cert_collection, __store, 0, 1);
                CertCloseStore(__store, 0);
            } else {
                TT_ERROR_NTV("fail to import pkcs7 cert");
                // but continue checking other certs
            }

            // next part
            __cur_pos += __PEM_TAG_LEN;
        }

        //*cert_store = __store;
        return TT_SUCCESS;
    } else {
        HCERTSTORE __store = NULL;
        CERT_BLOB cert_blob;
        DWORD dwExpectedContentTypeFlags;
        BOOL ret;

        cert_blob.cbData = cert->len;
        cert_blob.pbData = cert->addr;
        dwExpectedContentTypeFlags =
            CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED |
            CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED_EMBED |
            CERT_QUERY_CONTENT_FLAG_PKCS7_UNSIGNED;
        ret = CryptQueryObject(CERT_QUERY_OBJECT_BLOB,
                               &cert_blob,
                               dwExpectedContentTypeFlags,
                               CERT_QUERY_FORMAT_FLAG_ALL,
                               0,
                               NULL,
                               NULL,
                               NULL,
                               &__store,
                               NULL,
                               NULL);
        if (ret && (__store != NULL)) {
            __cert_store_show(__store, "");

            CertAddStoreToCollection(cert_collection, __store, 0, 1);
            CertCloseStore(__store, 0);
            return TT_SUCCESS;
        } else {
            TT_ERROR_NTV("fail to import pkcs7 cert");
            return TT_FAIL;
        }
    }
}

tt_result_t __cert_import_pkcs12(IN tt_blob_t *cert,
                                 IN tt_ssl_cert_attr_t *cert_attr,
                                 OUT HCERTSTORE *cert_store)
{
    HCERTSTORE __store = NULL;
    CRYPT_DATA_BLOB PFX;
    wchar_t *w_passwd;

    PFX.pbData = (BYTE *)cert->addr;
    PFX.cbData = (DWORD)cert->len;

    w_passwd = tt_wchar_create(cert_attr->password, NULL);
    if (w_passwd == NULL) {
        goto __p12i_fail;
    }

    // leaf certificate would fail to find matching private key
    // when PKCS12_NO_PERSIST_KEY is set
    __store = PFXImportCertStore(&PFX, w_passwd, 0);
    tt_wchar_destroy(w_passwd);
    if (__store == NULL) {
        TT_ERROR_NTV("fail to import pkcs12 cert");
        goto __p12i_fail;
    }

    *cert_store = __store;
    return TT_SUCCESS;

__p12i_fail:

    if (__store != NULL) {
        CertCloseStore(__store, 0);
    }

    return TT_FAIL;
}

tt_result_t __privkey_import_pkcs8(IN tt_blob_t *privkey_der,
                                   IN tt_ssl_privkey_attr_t *privkey_attr,
                                   OUT HCRYPTPROV *prov)
{
    // how to decrypt the pkcs8 file??

    // to be consist with other platforms, only support pkcs12
    return TT_FAIL;
}

void __cert_store_clean(IN HCERTSTORE cert_store)
{
    if (!CertCloseStore(cert_store, CERT_CLOSE_STORE_CHECK_FLAG)) {
        TT_ERROR_NTV("fail to close cert store");
    }
}

void __cert_store_show(IN HCERTSTORE cert_store, IN const tt_char_t *prefix)
{
    PCCERT_CONTEXT cert = NULL;
    while ((cert = CertEnumCertificatesInStore(cert_store, cert)) != NULL) {
        __cert_show(cert, prefix);
    }
}

void __cert_show(IN PCCERT_CONTEXT cert, IN const tt_char_t *prefix)
{
    DWORD dwFlags;
    wchar_t sub[100 + 1] = {0};
    wchar_t iss[100 + 1] = {0};

    if (cert == NULL) {
        return;
    }

    dwFlags = 0;
    CertGetNameString(cert,
                      CERT_NAME_SIMPLE_DISPLAY_TYPE,
                      dwFlags,
                      NULL,
                      sub,
                      sizeof(sub) - 1);

    dwFlags = CERT_NAME_ISSUER_FLAG;
    CertGetNameString(cert,
                      CERT_NAME_SIMPLE_DISPLAY_TYPE,
                      dwFlags,
                      NULL,
                      iss,
                      sizeof(iss) - 1);

    TT_INFO("%s [%ls] issued by [%ls]", prefix, sub, iss);
}

PCCERT_CONTEXT __cert_store_find_dn(IN HCERTSTORE cert_store,
                                    IN tt_distname_t *dn)
{
    // temporarily find a cert which has private key
    return __cert_store_find(cert_store, __FIND_CERT_WITH_PRIVKEY);
}

PCCERT_CONTEXT __cert_store_find(IN HCERTSTORE cert_store, IN tt_u32_t flag)
{
    PCCERT_CONTEXT cert = NULL;
    DWORD dwCertEncodingType = X509_ASN_ENCODING | PKCS_7_ASN_ENCODING;
    DWORD dwFindFlags = 0;
    DWORD dwFindType = 0;
    const void *pvFindPara = NULL;
    PCCERT_CONTEXT pPrevCertContext = NULL;

    if (flag & __FIND_CERT_WITH_PRIVKEY) {
        return __find_cert_with_privkey(cert_store, flag);
    }

    // todo: set find flags, find type

    return CertFindCertificateInStore(cert_store,
                                      dwCertEncodingType,
                                      dwFindFlags,
                                      dwFindType,
                                      pvFindPara,
                                      pPrevCertContext);
}

PCCERT_CONTEXT __find_cert_with_privkey(IN HCERTSTORE cert_store,
                                        IN tt_u32_t flag)
{
    PCCERT_CONTEXT cert = NULL;
    while ((cert = CertEnumCertificatesInStore(cert_store, cert)) != NULL) {
        HCRYPTPROV_OR_NCRYPT_KEY_HANDLE hCryptProvOrNCryptKey;
        DWORD dwKeySpec;
        BOOL fCallerFreeProvOrNCryptKey;

        // just check if the cert has private key
        if (CryptAcquireCertificatePrivateKey(cert,
                                              0,
                                              NULL,
                                              &hCryptProvOrNCryptKey,
                                              &dwKeySpec,
                                              &fCallerFreeProvOrNCryptKey)) {
            if (fCallerFreeProvOrNCryptKey) {
                if (dwKeySpec & CERT_NCRYPT_KEY_SPEC) {
                    NCryptFreeObject(hCryptProvOrNCryptKey);
                } else {
                    CryptReleaseContext(hCryptProvOrNCryptKey, 0);
                }
            }

            return cert;
        }
    }
    return NULL;
}

void __sslctx_commit_clean(IN tt_sslctx_ntv_t *ctx)
{
    if (__HCRED_VALID(&ctx->hcred)) {
        tt_g_sspi->FreeCredentialsHandle(&ctx->hcred);
        __HCRED_INIT(&ctx->hcred);
    }

    if (ctx->cert != NULL) {
        CertFreeCertificateContext(ctx->cert);
        ctx->cert = NULL;
    }

    if (ctx->chain_engine != NULL) {
        CertFreeCertificateChainEngine(ctx->chain_engine);
        ctx->chain_engine = NULL;
    }
}

tt_result_t __config_schannel_cred(IN tt_sslctx_ntv_t *ctx,
                                   IN PCCERT_CONTEXT cert,
                                   OUT SCHANNEL_CRED *pAuthData)
{
    memset(pAuthData, 0, sizeof(SCHANNEL_CRED));

    // version
    pAuthData->dwVersion = SCHANNEL_CRED_VERSION;

    // cert
    if (cert != NULL) {
        pAuthData->cCreds = 1;
        pAuthData->paCred = &cert;
    } else if (ctx->role == TT_SSL_ROLE_CLIENT) {
        // allow client without cert
        pAuthData->cCreds = 0;
        pAuthData->paCred = NULL;
    } else {
        TT_ERROR("server must have a certifcate");
        return TT_BAD_PARAM;
    }

    // ca

    // algorithm, todo

    // protocol, todo
    if (ctx->role == TT_SSL_ROLE_CLIENT) {
        // pAuthData->grbitEnabledProtocols = SP_PROT_TLS1_CLIENT;
    } else {
        // pAuthData->grbitEnabledProtocols = SP_PROT_TLS1_SERVER;
    }

    // cipher strength
    // set zero to use default

    // session lifespan
    // set zero to use default

    // flags, todo
    if (ctx->role == TT_SSL_ROLE_CLIENT) {
        // manually verify server cert
        pAuthData->dwFlags |= SCH_CRED_MANUAL_CRED_VALIDATION;

        // no default cert, use specified cert
        pAuthData->dwFlags |= SCH_CRED_NO_DEFAULT_CREDS;

        // do not verify server cert
        pAuthData->dwFlags |= SCH_CRED_NO_SERVERNAME_CHECK;

        // always send all cert
        // pAuthData->dwFlags |= SCH_SEND_ROOT_CERT;
    } else {
        // do not map client to user
        pAuthData->dwFlags |= SCH_CRED_NO_SYSTEM_MAPPER;

        // always send all cert
        // pAuthData->dwFlags |= SCH_SEND_ROOT_CERT;
    }

    return TT_SUCCESS;
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
    __cert_store_show(ctx->cert_store, "   ");

    TT_INFO("  CA:");
    __cert_store_show(ctx->ca_store, "   ");

    TT_INFO("  active cert:");
    if (ctx->cert != NULL) {
        __cert_show(ctx->cert, "   ");
    }
}

// length of data specified in base64_data can include terminating
// 0 or not
tt_result_t __base64_decode(IN tt_blob_t *base64_data,
                            IN tt_u32_t reserved,
                            OUT tt_blob_t *der_data)
{
    BYTE *pbBinary = NULL;
    DWORD cbBinary = 0;

    if (!CryptStringToBinaryA((LPCSTR)base64_data->addr,
                              base64_data->len,
                              CRYPT_STRING_BASE64HEADER,
                              NULL,
                              &cbBinary,
                              NULL,
                              NULL) ||
        (cbBinary <= 0)) {
        TT_ERROR("unable to calc binary content length");
        return TT_FAIL;
    }

    pbBinary = (BYTE *)tt_malloc(cbBinary);
    if (pbBinary == NULL) {
        TT_ERROR("no memory for binary content");
        return TT_FAIL;
    }

    if (!CryptStringToBinaryA((LPCSTR)base64_data->addr,
                              base64_data->len,
                              CRYPT_STRING_BASE64HEADER,
                              pbBinary,
                              &cbBinary,
                              NULL,
                              NULL)) {
        TT_ERROR("unable to calc binary content length");
        return TT_FAIL;
    }

    der_data->addr = pbBinary;
    der_data->len = cbBinary;
    return TT_SUCCESS;
}

#endif
