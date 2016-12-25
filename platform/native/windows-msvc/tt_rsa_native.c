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

#include <tt_rsa_native.h>

#include <algorithm/tt_buffer.h>
#include <algorithm/tt_buffer_format.h>
#include <crypto/tt_rsa.h>
#include <init/tt_profile.h>
#include <memory/tt_memory_alloc.h>
#include <misc/tt_util.h>

#include <tt_crypto_native.h>
#include <tt_sys_error.h>

#ifdef TSCM_PLATFORM_CRYPTO_ENABLE

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define NCRYPT_INVALID_HANDLE -1

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef tt_result_t (*__rsa_encrypt_t)(IN tt_rsa_ntv_t *sys_rsa,
                                       IN tt_u8_t *input,
                                       IN tt_u32_t input_len,
                                       OUT tt_u8_t *output,
                                       IN OUT tt_u32_t *output_len);
typedef tt_result_t (*__rsa_decrypt_t)(IN tt_rsa_ntv_t *sys_rsa,
                                       IN tt_u8_t *input,
                                       IN tt_u32_t input_len,
                                       OUT tt_u8_t *output,
                                       IN OUT tt_u32_t *output_len);

typedef tt_result_t (*__rsa_sign_t)(IN tt_rsa_ntv_t *sys_rsa,
                                    IN tt_u8_t *input,
                                    IN tt_u32_t input_len,
                                    OUT tt_u8_t *signature,
                                    IN OUT tt_u32_t *signature_len);
typedef tt_result_t (*__rsa_verify_t)(IN tt_rsa_ntv_t *sys_rsa,
                                      IN tt_u8_t *input,
                                      IN tt_u32_t input_len,
                                      IN tt_u8_t *signature,
                                      IN tt_u32_t signature_len);

typedef struct
{
    __rsa_encrypt_t encrypt;
    __rsa_decrypt_t decrypt;

    __rsa_sign_t sign;
    __rsa_verify_t verify;
} __rsa_itf_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_sha1_ntv(IN tt_u8_t *data,
                               IN tt_u32_t data_len,
                               OUT tt_u8_t *hash_val);

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static BCRYPT_ALG_HANDLE tt_s_crypt_prov_rsa;
static NCRYPT_PROV_HANDLE tt_s_ncrypt_prov_rsa;

static tt_result_t __rsa_public(IN tt_rsa_ntv_t *sys_rsa,
                                IN tt_blob_t *key_data,
                                IN struct tt_rsa_attr_s *attr);
static tt_result_t __rsa_private_pkcs1(IN tt_rsa_ntv_t *sys_rsa,
                                       IN tt_blob_t *key_data,
                                       IN struct tt_rsa_attr_s *attr);
static tt_result_t __rsa_private_pkcs8(IN tt_rsa_ntv_t *sys_rsa,
                                       IN tt_blob_t *key_data,
                                       IN struct tt_rsa_attr_s *attr);

// TT_RSA_PADDING_NONE
static tt_result_t __rsa_encrypt_none(IN tt_rsa_ntv_t *sys_rsa,
                                      IN tt_u8_t *input,
                                      IN tt_u32_t input_len,
                                      OUT tt_u8_t *output,
                                      IN OUT tt_u32_t *output_len);
static tt_result_t __rsa_decrypt_none(IN tt_rsa_ntv_t *sys_rsa,
                                      IN tt_u8_t *input,
                                      IN tt_u32_t input_len,
                                      OUT tt_u8_t *output,
                                      IN OUT tt_u32_t *output_len);

// TT_RSA_PADDING_OAEP
static tt_result_t __rsa_encrypt_oaep(IN tt_rsa_ntv_t *sys_rsa,
                                      IN tt_u8_t *input,
                                      IN tt_u32_t input_len,
                                      OUT tt_u8_t *output,
                                      IN OUT tt_u32_t *output_len);
static tt_result_t __rsa_decrypt_oaep(IN tt_rsa_ntv_t *sys_rsa,
                                      IN tt_u8_t *input,
                                      IN tt_u32_t input_len,
                                      OUT tt_u8_t *output,
                                      IN OUT tt_u32_t *output_len);

// TT_RSA_PADDING_PKCS1
static tt_result_t __rsa_encrypt_pkcs1(IN tt_rsa_ntv_t *sys_rsa,
                                       IN tt_u8_t *input,
                                       IN tt_u32_t input_len,
                                       OUT tt_u8_t *output,
                                       IN OUT tt_u32_t *output_len);
static tt_result_t __rsa_decrypt_pkcs1(IN tt_rsa_ntv_t *sys_rsa,
                                       IN tt_u8_t *input,
                                       IN tt_u32_t input_len,
                                       OUT tt_u8_t *output,
                                       IN OUT tt_u32_t *output_len);
static tt_result_t __rsa_sign_pkcs1(IN tt_rsa_ntv_t *sys_rsa,
                                    IN tt_u8_t *input,
                                    IN tt_u32_t input_len,
                                    OUT tt_u8_t *signature,
                                    IN OUT tt_u32_t *signature_len);
static tt_result_t __rsa_verify_pkcs1(IN tt_rsa_ntv_t *sys_rsa,
                                      IN tt_u8_t *input,
                                      IN tt_u32_t input_len,
                                      IN tt_u8_t *signature,
                                      IN tt_u32_t signature_len);

static __rsa_itf_t tt_s_rsa_itf[TT_RSA_PADDING_NUM] = {
    // TT_RSA_PADDING_NONE
    {
        __rsa_encrypt_none,
        __rsa_decrypt_none,

        NULL,
        NULL,
    },

    // TT_RSA_PADDING_OAEP
    {
        __rsa_encrypt_oaep,
        __rsa_decrypt_oaep,

        NULL,
        NULL,
    },

    // TT_RSA_PADDING_PKCS1
    {
        __rsa_encrypt_pkcs1,
        __rsa_decrypt_pkcs1,

        __rsa_sign_pkcs1,
        __rsa_verify_pkcs1,
    },
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////


static tt_result_t __rsa_number_get_public(IN BCRYPT_KEY_HANDLE rsa,
                                           IN tt_rsa_pubnum_t *pub_n);
static tt_result_t __rsa_number_get_private(IN BCRYPT_KEY_HANDLE rsa,
                                            IN tt_rsa_privnum_t *priv_n);

// although it's defined in another file, but it's protected by
// another macro
static tt_result_t __base64_decode(IN tt_blob_t *base64_data,
                                   IN tt_u32_t reserved,
                                   OUT tt_blob_t *der_data);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_rsa_component_init_ntv(IN tt_profile_t *profile)
{
    NTSTATUS ntst;
    // ULONG n;

    // provider
    ntst = BCryptOpenAlgorithmProvider(&tt_s_crypt_prov_rsa,
                                       BCRYPT_RSA_ALGORITHM,
                                       MS_PRIMITIVE_PROVIDER,
                                       0);
    if ((ntst != STATUS_SUCCESS) || (tt_s_crypt_prov_rsa == NULL)) {
        TT_ERROR("fail to create RSA provider");
        return TT_FAIL;
    }

    // ncrypt provider
    ntst = NCryptOpenStorageProvider(&tt_s_ncrypt_prov_rsa,
                                     MS_KEY_STORAGE_PROVIDER,
                                     0);
    if (ntst != STATUS_SUCCESS) {
        TT_ERROR("fail to create ncrypt RSA provider");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_rsa_generate_ntv(OUT tt_rsa_ntv_t *sys_rsa,
                                IN tt_rsa_size_t size,
                                IN struct tt_rsa_attr_s *attr)
{
    ULONG bits = 0;
    BCRYPT_KEY_HANDLE rsa = NULL;
    NTSTATUS ntst;

    switch (size) {
        case TT_RSA_SIZE_1024BIT: {
            bits = 1024;
        } break;
        case TT_RSA_SIZE_2048BIT: {
            bits = 2048;
        } break;

        default: {
            TT_ERROR("unsupported rsa size: %d", size);
            return TT_FAIL;
        } break;
    }

    ntst = BCryptGenerateKeyPair(tt_s_crypt_prov_rsa, &rsa, bits, 0);
    if (ntst != STATUS_SUCCESS) {
        TT_ERROR("fail to get generate RSA key pair");
        return TT_FAIL;
    }

#if 0 // is this mandatory??
    // set padding mode
    TT_ASSERT(TT_RSA_PADDING_VALID(attr->padding.mode));
    if (attr->padding.mode == TT_RSA_PADDING_OAEP)
    {
        DWORD padding = BCRYPT_SUPPORTED_PAD_OAEP;
        
        ntst = BCryptSetProperty(rsa, 
                                 BCRYPT_PADDING_SCHEMES, 
                                 (PUCHAR)&padding, 
                                 sizeof(padding), 
                                 0);
        if (ntst != STATUS_SUCCESS)
        {
            TT_ERROR("fail to set rsa oaep padding");

            BCryptDestroyKey(rsa);
            tt_free(rsa, tt_s_crypt_rsa_size);
            
            return TT_FAIL;
        }
    }
#endif

    ntst = BCryptFinalizeKeyPair(rsa, 0);
    if (ntst != STATUS_SUCCESS) {
        TT_ERROR("fail to finalize RSA key pair");

        BCryptDestroyKey(rsa);
        return TT_FAIL;
    }

    sys_rsa->rsa = rsa;
    sys_rsa->type = TT_RSA_TYPE_PRIVATE;

    return TT_SUCCESS;
}

tt_result_t tt_rsa_create_ntv(IN tt_rsa_ntv_t *sys_rsa,
                              IN tt_rsa_format_t key_format,
                              IN tt_rsa_type_t key_type,
                              IN tt_blob_t *key_data,
                              IN struct tt_rsa_attr_s *attr)
{
    tt_rsa_t *rsa = TT_CONTAINER(sys_rsa, tt_rsa_t, sys_rsa);
    tt_result_t result = TT_FAIL;

    NTSTATUS ntst;
    DWORD key_size = 0;
    ULONG n;

    // import key
    if (key_type == TT_RSA_TYPE_PUBLIC) {
        result = __rsa_public(sys_rsa, key_data, attr);
    } else {
        switch (key_format) {
            case TT_RSA_FORMAT_PKCS1: {
                result = __rsa_private_pkcs1(sys_rsa, key_data, attr);
            } break;
            case TT_RSA_FORMAT_PKCS8: {
                result = __rsa_private_pkcs8(sys_rsa, key_data, attr);
            } break;

            default: {
                TT_ERROR("invalid rsa format[%d]", key_format);
            } break;
        }
    }
    if (!TT_OK(result)) {
        return TT_FAIL;
    }

    // check key size
    ntst = BCryptGetProperty(sys_rsa->rsa,
                             BCRYPT_BLOCK_LENGTH,
                             (PUCHAR)&key_size,
                             sizeof(key_size),
                             &n,
                             0);
    if (ntst == STATUS_SUCCESS) {
        switch (key_size) {
            case 128: {
                rsa->size = TT_RSA_SIZE_1024BIT;
                return TT_SUCCESS;
            } break;
            case 256: {
                rsa->size = TT_RSA_SIZE_2048BIT;
                return TT_SUCCESS;
            } break;

            default: {
                TT_ERROR("unsupported rsa key size: %d", key_size);
            } break;
        }
    } else {
        TT_ERROR("fail to get RSA block size");
    }

    BCryptDestroyKey(sys_rsa->rsa);
    sys_rsa->rsa = NULL;
    return TT_FAIL;
}

void tt_rsa_destroy_ntv(IN tt_rsa_ntv_t *sys_rsa)
{
    if (sys_rsa->rsa != NULL) {
        BCryptDestroyKey(sys_rsa->rsa);
    }
}

tt_result_t tt_rsa_encrypt_ntv(IN tt_rsa_ntv_t *sys_rsa,
                               IN tt_u8_t *input,
                               IN tt_u32_t input_len,
                               OUT tt_u8_t *output,
                               IN OUT tt_u32_t *output_len)
{
    tt_rsa_t *rsa;
    __rsa_encrypt_t encrypt;

    rsa = TT_CONTAINER(sys_rsa, tt_rsa_t, sys_rsa);
    encrypt = tt_s_rsa_itf[rsa->attr.padding.mode].encrypt;
    if (encrypt != NULL) {
        return encrypt(sys_rsa, input, input_len, output, output_len);
    } else {
        TT_ERROR("can not encrypt in padding: %d", rsa->attr.padding.mode);
        return TT_FAIL;
    }
}

tt_result_t tt_rsa_decrypt_ntv(IN tt_rsa_ntv_t *sys_rsa,
                               IN tt_u8_t *input,
                               IN tt_u32_t input_len,
                               OUT tt_u8_t *output,
                               IN OUT tt_u32_t *output_len)
{
    tt_rsa_t *rsa;
    __rsa_decrypt_t decrypt;

    rsa = TT_CONTAINER(sys_rsa, tt_rsa_t, sys_rsa);
    decrypt = tt_s_rsa_itf[rsa->attr.padding.mode].decrypt;
    if (decrypt != NULL) {
        return decrypt(sys_rsa, input, input_len, output, output_len);
    } else {
        TT_ERROR("can not decrypt in padding: %d", rsa->attr.padding.mode);
        return TT_FAIL;
    }
}

tt_result_t tt_rsa_sign_ntv(IN tt_rsa_ntv_t *sys_rsa,
                            IN tt_u8_t *input,
                            IN tt_u32_t input_len,
                            OUT tt_u8_t *signature,
                            IN OUT tt_u32_t *signature_len)
{
    tt_rsa_t *rsa;
    __rsa_sign_t sign;

    rsa = TT_CONTAINER(sys_rsa, tt_rsa_t, sys_rsa);
    sign = tt_s_rsa_itf[rsa->attr.padding.mode].sign;
    if (sign != NULL) {
        return sign(sys_rsa, input, input_len, signature, signature_len);
    } else {
        TT_ERROR("can not sign in padding: %d", rsa->attr.padding.mode);
        return TT_FAIL;
    }
}

tt_result_t tt_rsa_verify_ntv(IN tt_rsa_ntv_t *sys_rsa,
                              IN tt_u8_t *input,
                              IN tt_u32_t input_len,
                              IN tt_u8_t *signature,
                              IN tt_u32_t signature_len)
{
    tt_rsa_t *rsa;
    __rsa_verify_t verify;

    rsa = TT_CONTAINER(sys_rsa, tt_rsa_t, sys_rsa);
    verify = tt_s_rsa_itf[rsa->attr.padding.mode].verify;
    if (verify != NULL) {
        return verify(sys_rsa, input, input_len, signature, signature_len);
    } else {
        TT_ERROR("can not verify in padding: %d", rsa->attr.padding.mode);
        return TT_FAIL;
    }
}
tt_result_t __rsa_public(IN tt_rsa_ntv_t *sys_rsa,
                         IN tt_blob_t *key_data,
                         IN struct tt_rsa_attr_s *attr)
{
    BCRYPT_KEY_HANDLE h_rsa = NULL;
    NTSTATUS ntst;

    tt_blob_t key_data_der = {0};
    CERT_PUBLIC_KEY_INFO *key_blob = NULL;
    BCRYPT_RSAKEY_BLOB *cng_key_blob = NULL;
    DWORD key_blob_size = 0;

    // base64 decoding
    tt_memcpy(&key_data_der, key_data, sizeof(tt_blob_t));
    if (attr->pem_armor) {
        if (!TT_OK(__base64_decode(key_data, 0, &key_data_der))) {
            TT_ERROR("fail to decode pem key data");
            goto rpub_fail;
        }
    }
    // tt_hex_dump(key_data_der.addr, key_data_der.len, 8);

    // may include beginning OID before decoding pub key
    if (CryptDecodeObjectEx(X509_ASN_ENCODING,
                            X509_PUBLIC_KEY_INFO,
                            key_data_der.addr,
                            key_data_der.len,
                            CRYPT_DECODE_ALLOC_FLAG,
                            NULL,
                            &key_blob,
                            &key_blob_size) &&
        CryptDecodeObjectEx(X509_ASN_ENCODING,
                            CNG_RSA_PUBLIC_KEY_BLOB,
                            key_blob->PublicKey.pbData,
                            key_blob->PublicKey.cbData,
                            CRYPT_DECODE_ALLOC_FLAG,
                            NULL,
                            &cng_key_blob,
                            &key_blob_size)) {
    } else if (CryptDecodeObjectEx(X509_ASN_ENCODING,
                                   CNG_RSA_PUBLIC_KEY_BLOB,
                                   key_data_der.addr,
                                   key_data_der.len,
                                   CRYPT_DECODE_ALLOC_FLAG,
                                   NULL,
                                   &cng_key_blob,
                                   &key_blob_size)) {
    } else {
        TT_ERROR_NTV("fail to convert to BCRYPT_RSAKEY_BLOB");
        goto rpub_fail;
    }
    if (key_blob != NULL) {
        LocalFree(key_blob);
        key_blob = NULL;
    }
    if (attr->pem_armor) {
        tt_free(key_data_der.addr);
        key_data_der.addr = NULL;
        key_data_der.len = 0;
    }

    // import pub key
    ntst = BCryptImportKeyPair(tt_s_crypt_prov_rsa,
                               NULL,
                               BCRYPT_RSAPUBLIC_BLOB,
                               &h_rsa,
                               (PUCHAR)cng_key_blob,
                               key_blob_size,
                               0);
    if (ntst != STATUS_SUCCESS) {
        TT_ERROR("fail to get import RSA public key");
        goto rpub_fail;
    }
    LocalFree(cng_key_blob);
    cng_key_blob = NULL;

    sys_rsa->rsa = h_rsa;
    sys_rsa->type = TT_RSA_TYPE_PUBLIC;

    return TT_SUCCESS;

rpub_fail:

    if (h_rsa != NULL) {
        BCryptDestroyKey(h_rsa);
    }

    if (cng_key_blob != NULL) {
        LocalFree(cng_key_blob);
    }

    if (key_blob != NULL) {
        LocalFree(key_blob);
    }

    if (attr->pem_armor && (key_data_der.addr != NULL)) {
        tt_free(key_data_der.addr);
    }

    return TT_FAIL;
}

tt_result_t __rsa_private_pkcs1(IN tt_rsa_ntv_t *sys_rsa,
                                IN tt_blob_t *key_data,
                                IN struct tt_rsa_attr_s *attr)
{
    BCRYPT_KEY_HANDLE h_rsa = NULL;
    NTSTATUS ntst;

    tt_blob_t key_data_der = {0};
#if 0 //#ifdef CNG_RSA_PRIVATE_KEY_BLOB
    LPCWSTR lpszStructType = CNG_RSA_PRIVATE_KEY_BLOB;
    BCRYPT_RSAKEY_BLOB *cng_key_blob = NULL;
    LPCWSTR pszBlobType = BCRYPT_RSAPRIVATE_BLOB;
#else
    LPCSTR lpszStructType = PKCS_RSA_PRIVATE_KEY;
    LPBYTE *cng_key_blob = NULL;
    LPCWSTR pszBlobType = LEGACY_RSAPRIVATE_BLOB;
#endif
    DWORD key_blob_size = 0;

    // base64 decoding
    tt_memcpy(&key_data_der, key_data, sizeof(tt_blob_t));
    if (attr->pem_armor) {
        if (!TT_OK(__base64_decode(key_data, 0, &key_data_der))) {
            TT_ERROR("fail to decode pem key data");
            goto rpriv_fail;
        }
    }
    // tt_hex_dump(key_data_der.addr, key_data_der.len, 8);

    // decode priv key
    if (!CryptDecodeObjectEx(X509_ASN_ENCODING,
                             lpszStructType,
                             key_data_der.addr,
                             key_data_der.len,
                             CRYPT_DECODE_ALLOC_FLAG,
                             NULL,
                             &cng_key_blob,
                             &key_blob_size)) {
        TT_ERROR_NTV("fail to convert to decode priv key");
        goto rpriv_fail;
    }
    if (attr->pem_armor) {
        tt_free(key_data_der.addr);
        key_data_der.addr = NULL;
        key_data_der.len = 0;
    }

    // import priv key
    ntst = BCryptImportKeyPair(tt_s_crypt_prov_rsa,
                               NULL,
                               pszBlobType,
                               &h_rsa,
                               (PUCHAR)cng_key_blob,
                               key_blob_size,
                               0);
    if (ntst != STATUS_SUCCESS) {
        TT_ERROR("fail to get import RSA private key");
        goto rpriv_fail;
    }
    LocalFree(cng_key_blob);
    cng_key_blob = NULL;

    sys_rsa->rsa = h_rsa;
    sys_rsa->type = TT_RSA_TYPE_PRIVATE;

    return TT_SUCCESS;

rpriv_fail:

    if (h_rsa != NULL) {
        BCryptDestroyKey(h_rsa);
    }

    if (cng_key_blob != NULL) {
        LocalFree(cng_key_blob);
    }

    if (attr->pem_armor && (key_data_der.addr != NULL)) {
        tt_free(key_data_der.addr);
    }

    return TT_FAIL;
}

tt_result_t __rsa_private_pkcs8(IN tt_rsa_ntv_t *sys_rsa,
                                IN tt_blob_t *key_data,
                                IN struct tt_rsa_attr_s *attr)
{
    BCRYPT_KEY_HANDLE h_rsa = NULL;
    NTSTATUS ntst;

    tt_blob_t key_data_der = {0};
    CRYPT_PRIVATE_KEY_INFO *pkcs8_blob = NULL;
    DWORD pkcs8_size = 0;
    CRYPT_PRIVATE_KEY_INFO *priv_key_blob = NULL;
    DWORD priv_key_size = 0;

    // base64 decoding
    tt_memcpy(&key_data_der, key_data, sizeof(tt_blob_t));
    if (attr->pem_armor) {
        if (!TT_OK(__base64_decode(key_data, 0, &key_data_der))) {
            TT_ERROR("fail to decode pem key data");
            goto rpriv_fail;
        }
    }
    // tt_hex_dump(key_data_der.addr, key_data_der.len, 8);

    // decode pkcs8
    if (!CryptDecodeObjectEx(X509_ASN_ENCODING,
                             PKCS_PRIVATE_KEY_INFO,
                             key_data_der.addr,
                             key_data_der.len,
                             CRYPT_DECODE_ALLOC_FLAG,
                             NULL,
                             &pkcs8_blob,
                             &pkcs8_size)) {
        TT_ERROR_NTV("fail to decode pkcs8 priv key");
        goto rpriv_fail;
    }
#if 0    
	tt_hex_dump(pkcs8_blob->PrivateKey.pbData, 
                pkcs8_blob->PrivateKey.cbData,
                8);
#endif
    if (attr->pem_armor) {
        tt_free(key_data_der.addr);
        key_data_der.addr = NULL;
        key_data_der.len = 0;
    }

    // decode priv key
    if (!CryptDecodeObjectEx(X509_ASN_ENCODING,
                             PKCS_RSA_PRIVATE_KEY,
                             pkcs8_blob->PrivateKey.pbData,
                             pkcs8_blob->PrivateKey.cbData,
                             CRYPT_DECODE_ALLOC_FLAG,
                             NULL,
                             &priv_key_blob,
                             &priv_key_size)) {
        TT_ERROR_NTV("fail to convert to decode priv key");
        goto rpriv_fail;
    }
    LocalFree(pkcs8_blob);
    pkcs8_blob = NULL;

    // import priv key
    ntst = BCryptImportKeyPair(tt_s_crypt_prov_rsa,
                               NULL,
                               LEGACY_RSAPRIVATE_BLOB,
                               &h_rsa,
                               (PUCHAR)priv_key_blob,
                               priv_key_size,
                               0);
    if (ntst != STATUS_SUCCESS) {
        TT_ERROR("fail to get import RSA private key");
        goto rpriv_fail;
    }
    LocalFree(priv_key_blob);
    priv_key_blob = NULL;

    sys_rsa->rsa = h_rsa;
    sys_rsa->type = TT_RSA_TYPE_PRIVATE;

    return TT_SUCCESS;

rpriv_fail:

    if (h_rsa != NULL) {
        BCryptDestroyKey(h_rsa);
    }

    if (priv_key_blob != NULL) {
        LocalFree(priv_key_blob);
    }

    if (pkcs8_blob != NULL) {
        LocalFree(pkcs8_blob);
    }

    if (attr->pem_armor && (key_data_der.addr != NULL)) {
        tt_free(key_data_der.addr);
    }

    return TT_FAIL;
}

// TT_RSA_PADDING_NONE
tt_result_t __rsa_encrypt_none(IN tt_rsa_ntv_t *sys_rsa,
                               IN tt_u8_t *input,
                               IN tt_u32_t input_len,
                               OUT tt_u8_t *output,
                               IN OUT tt_u32_t *output_len)
{
    NTSTATUS ntst;
    ULONG pcbResult = 0;

    ntst = BCryptEncrypt(sys_rsa->rsa,
                         input,
                         input_len,
                         NULL,
                         NULL,
                         0,
                         output,
                         *output_len,
                         &pcbResult,
                         0);
    if (ntst != STATUS_SUCCESS) {
        TT_ERROR("rsa encrypt failed");
        return TT_FAIL;
    }
    *output_len = pcbResult;

    return TT_SUCCESS;
}

tt_result_t __rsa_decrypt_none(IN tt_rsa_ntv_t *sys_rsa,
                               IN tt_u8_t *input,
                               IN tt_u32_t input_len,
                               OUT tt_u8_t *output,
                               IN OUT tt_u32_t *output_len)
{
    NTSTATUS ntst;
    ULONG pcbResult = 0;

    ntst = BCryptDecrypt(sys_rsa->rsa,
                         input,
                         input_len,
                         NULL,
                         NULL,
                         0,
                         output,
                         *output_len,
                         &pcbResult,
                         0);
    if (ntst != STATUS_SUCCESS) {
        TT_ERROR("rsa decrypt failed");
        return TT_FAIL;
    }
    *output_len = pcbResult;

    return TT_SUCCESS;
}

// TT_RSA_PADDING_OAEP
tt_result_t __rsa_encrypt_oaep(IN tt_rsa_ntv_t *sys_rsa,
                               IN tt_u8_t *input,
                               IN tt_u32_t input_len,
                               OUT tt_u8_t *output,
                               IN OUT tt_u32_t *output_len)
{
    NTSTATUS ntst;
    ULONG pcbResult = 0;
    BCRYPT_OAEP_PADDING_INFO oaep_info;

    // always use sha1
    memset(&oaep_info, 0, sizeof(BCRYPT_OAEP_PADDING_INFO));
    oaep_info.pszAlgId = BCRYPT_SHA1_ALGORITHM;

    ntst = BCryptEncrypt(sys_rsa->rsa,
                         input,
                         input_len,
                         &oaep_info,
                         NULL,
                         0,
                         output,
                         *output_len,
                         &pcbResult,
                         BCRYPT_PAD_OAEP);
    if (ntst != STATUS_SUCCESS) {
        TT_ERROR("rsa encrypt failed");
        return TT_FAIL;
    }
    *output_len = pcbResult;

    return TT_SUCCESS;
}

tt_result_t __rsa_decrypt_oaep(IN tt_rsa_ntv_t *sys_rsa,
                               IN tt_u8_t *input,
                               IN tt_u32_t input_len,
                               OUT tt_u8_t *output,
                               IN OUT tt_u32_t *output_len)
{
    NTSTATUS ntst;
    ULONG pcbResult = 0;
    BCRYPT_OAEP_PADDING_INFO oaep_info;

    // always use sha1
    memset(&oaep_info, 0, sizeof(BCRYPT_OAEP_PADDING_INFO));
    oaep_info.pszAlgId = BCRYPT_SHA1_ALGORITHM;

    ntst = BCryptDecrypt(sys_rsa->rsa,
                         input,
                         input_len,
                         &oaep_info,
                         NULL,
                         0,
                         output,
                         *output_len,
                         &pcbResult,
                         BCRYPT_PAD_OAEP);
    if (ntst != STATUS_SUCCESS) {
        TT_ERROR("rsa decrypt failed");
        return TT_FAIL;
    }
    *output_len = pcbResult;

    return TT_SUCCESS;
}

// TT_RSA_PADDING_PKCS1
tt_result_t __rsa_encrypt_pkcs1(IN tt_rsa_ntv_t *sys_rsa,
                                IN tt_u8_t *input,
                                IN tt_u32_t input_len,
                                OUT tt_u8_t *output,
                                IN OUT tt_u32_t *output_len)
{
    NTSTATUS ntst;
    ULONG pcbResult = 0;

    ntst = BCryptEncrypt(sys_rsa->rsa,
                         input,
                         input_len,
                         NULL,
                         NULL,
                         0,
                         output,
                         *output_len,
                         &pcbResult,
                         BCRYPT_PAD_PKCS1);
    if (ntst != STATUS_SUCCESS) {
        TT_ERROR("rsa encrypt failed");
        return TT_FAIL;
    }
    *output_len = pcbResult;

    return TT_SUCCESS;
}

tt_result_t __rsa_decrypt_pkcs1(IN tt_rsa_ntv_t *sys_rsa,
                                IN tt_u8_t *input,
                                IN tt_u32_t input_len,
                                OUT tt_u8_t *output,
                                IN OUT tt_u32_t *output_len)
{
    NTSTATUS ntst;
    ULONG pcbResult = 0;

    ntst = BCryptDecrypt(sys_rsa->rsa,
                         input,
                         input_len,
                         NULL,
                         NULL,
                         0,
                         output,
                         *output_len,
                         &pcbResult,
                         BCRYPT_PAD_PKCS1);
    if (ntst != STATUS_SUCCESS) {
        TT_ERROR("rsa decrypt failed");
        return TT_FAIL;
    }
    *output_len = pcbResult;

    return TT_SUCCESS;
}

tt_result_t __rsa_sign_pkcs1(IN tt_rsa_ntv_t *sys_rsa,
                             IN tt_u8_t *input,
                             IN tt_u32_t input_len,
                             OUT tt_u8_t *signature,
                             IN OUT tt_u32_t *signature_len)
{
    NTSTATUS ntst;
    BCRYPT_PKCS1_PADDING_INFO pkcs1_info;
    tt_u8_t hash[20]; // sha1 digest length
    DWORD cbOutput = *signature_len;
    DWORD pcbResult = 0;

    pkcs1_info.pszAlgId = BCRYPT_SHA1_ALGORITHM;

    if (!TT_OK(tt_sha1_ntv(input, input_len, hash))) {
        return TT_FAIL;
    }

    ntst = BCryptSignHash(sys_rsa->rsa,
                          &pkcs1_info,
                          hash,
                          sizeof(hash),
                          signature,
                          cbOutput,
                          &pcbResult,
                          BCRYPT_PAD_PKCS1);
    if (ntst != STATUS_SUCCESS) {
        TT_ERROR("rsa sign failed");
        return TT_FAIL;
    }
    *signature_len = pcbResult;

    return TT_SUCCESS;
}

tt_result_t __rsa_verify_pkcs1(IN tt_rsa_ntv_t *sys_rsa,
                               IN tt_u8_t *input,
                               IN tt_u32_t input_len,
                               IN tt_u8_t *signature,
                               IN tt_u32_t signature_len)
{
    NTSTATUS ntst;
    BCRYPT_PKCS1_PADDING_INFO pkcs1_info;
    tt_u8_t hash[20]; // sha1 digest length

    pkcs1_info.pszAlgId = BCRYPT_SHA1_ALGORITHM;

    if (!TT_OK(tt_sha1_ntv(input, input_len, hash))) {
        return TT_FAIL;
    }

    ntst = BCryptVerifySignature(sys_rsa->rsa,
                                 &pkcs1_info,
                                 hash,
                                 sizeof(hash),
                                 signature,
                                 signature_len,
                                 BCRYPT_PAD_PKCS1);
    if (ntst != STATUS_SUCCESS) {
        // TT_ERROR("rsa verify failed");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}
tt_result_t tt_rsa_show_ntv(IN tt_rsa_ntv_t *sys_rsa)
{
    tt_rsa_number_t rsa_number;
    tt_buf_t printable;

    if (!TT_OK(tt_rsa_get_number_ntv(sys_rsa, &rsa_number))) {
        return TT_FAIL;
    }

    tt_buf_init(&printable, NULL);

    if (sys_rsa->type == TT_RSA_TYPE_PUBLIC) {
        tt_rsa_pubnum_t *pub_num = &rsa_number.pubnum;

        tt_buf_reset_rwp(&printable);
        tt_buf_put_hex2cstr(&printable,
                            pub_num->modulus.addr,
                            pub_num->modulus.len);
        tt_buf_put_u8(&printable, 0);
        TT_INFO("  modulus: %s", TT_BUF_RPOS(&printable));

        tt_buf_reset_rwp(&printable);
        tt_buf_put_hex2cstr(&printable,
                            pub_num->pub_exp.addr,
                            pub_num->pub_exp.len);
        tt_buf_put_u8(&printable, 0);
        TT_INFO("  pub_exp: %s", TT_BUF_RPOS(&printable));
    } else {
        tt_rsa_privnum_t *priv_num = &rsa_number.privnum;

        tt_buf_reset_rwp(&printable);
        tt_buf_put_hex2cstr(&printable,
                            priv_num->modulus.addr,
                            priv_num->modulus.len);
        tt_buf_put_u8(&printable, 0);
        TT_INFO("  modulus: %s", TT_BUF_RPOS(&printable));

        tt_buf_reset_rwp(&printable);
        tt_buf_put_hex2cstr(&printable,
                            priv_num->pub_exp.addr,
                            priv_num->pub_exp.len);
        tt_buf_put_u8(&printable, 0);
        TT_INFO("  pub_exp: %s", TT_BUF_RPOS(&printable));

        tt_buf_reset_rwp(&printable);
        tt_buf_put_hex2cstr(&printable,
                            priv_num->priv_exp.addr,
                            priv_num->priv_exp.len);
        tt_buf_put_u8(&printable, 0);
        TT_INFO("  priv_exp: %s", TT_BUF_RPOS(&printable));

        tt_buf_reset_rwp(&printable);
        tt_buf_put_hex2cstr(&printable,
                            priv_num->prime1.addr,
                            priv_num->prime1.len);
        tt_buf_put_u8(&printable, 0);
        TT_INFO("  prime1: %s", TT_BUF_RPOS(&printable));

        tt_buf_reset_rwp(&printable);
        tt_buf_put_hex2cstr(&printable,
                            priv_num->prime2.addr,
                            priv_num->prime2.len);
        tt_buf_put_u8(&printable, 0);
        TT_INFO("  prime2: %s", TT_BUF_RPOS(&printable));

        tt_buf_reset_rwp(&printable);
        tt_buf_put_hex2cstr(&printable,
                            priv_num->exp1.addr,
                            priv_num->exp1.len);
        tt_buf_put_u8(&printable, 0);
        TT_INFO("  exp1: %s", TT_BUF_RPOS(&printable));

        tt_buf_reset_rwp(&printable);
        tt_buf_put_hex2cstr(&printable,
                            priv_num->exp2.addr,
                            priv_num->exp2.len);
        tt_buf_put_u8(&printable, 0);
        TT_INFO("  exp2: %s", TT_BUF_RPOS(&printable));

        tt_buf_reset_rwp(&printable);
        tt_buf_put_hex2cstr(&printable,
                            priv_num->coefficient.addr,
                            priv_num->coefficient.len);
        tt_buf_put_u8(&printable, 0);
        TT_INFO("  coefficient: %s", TT_BUF_RPOS(&printable));
    }

    tt_rsa_number_destroy(&rsa_number);
    tt_buf_destroy(&printable);

    return TT_SUCCESS;
}

tt_result_t tt_rsa_get_number_ntv(IN tt_rsa_ntv_t *sys_rsa,
                                  IN struct tt_rsa_number_s *rsa_number)
{
    if (sys_rsa->type == TT_RSA_TYPE_PUBLIC) {
        rsa_number->type = TT_RSA_TYPE_PUBLIC;
        return __rsa_number_get_public(sys_rsa->rsa, &rsa_number->pubnum);
    } else {
        TT_ASSERT(sys_rsa->type == TT_RSA_TYPE_PRIVATE);

        rsa_number->type = TT_RSA_TYPE_PRIVATE;
        return __rsa_number_get_private(sys_rsa->rsa, &rsa_number->privnum);
    }
}

tt_result_t __rsa_number_get_public(IN BCRYPT_KEY_HANDLE rsa,
                                    IN tt_rsa_pubnum_t *pub_n)
{
    NTSTATUS ntst;
    ULONG n;
    BCRYPT_RSAKEY_BLOB *key_blob = NULL;
    tt_u32_t key_blob_size = 0;

    /*
     kSecFormatBSAFE:

     RSAPublicKey ::= SEQUENCE {
        modulus INTEGER, -- n
        publicExponent INTEGER -- e
     }
     */

    tt_memset(pub_n, 0, sizeof(tt_rsa_pubnum_t));

    // export
    ntst = BCryptExportKey(rsa, NULL, BCRYPT_RSAPUBLIC_BLOB, NULL, 0, &n, 0);
    if (ntst != STATUS_SUCCESS) {
        TT_ERROR("fail to export rsa pub key");
        goto gp_fail;
    }

    key_blob_size = n;
    key_blob = (BCRYPT_RSAKEY_BLOB *)tt_malloc(key_blob_size);
    if (key_blob == NULL) {
        TT_ERROR("no mem for exporting rsa pub key");
        goto gp_fail;
    }

    ntst = BCryptExportKey(rsa,
                           NULL,
                           BCRYPT_RSAPUBLIC_BLOB,
                           (PUCHAR)key_blob,
                           key_blob_size,
                           &n,
                           0);
    if (ntst != STATUS_SUCCESS) {
        TT_ERROR("fail to export rsa pub key");
        goto gp_fail;
    }

    // copy
    /*
    BCRYPT_RSAKEY_BLOB
    PublicExponent[cbPublicExp] // Big endian.
    Modulus[cbModulus] // Big endian.
    */

    // modulus
    if (!TT_OK(tt_blob_create(&pub_n->modulus,
                              TT_PTR_INC(tt_u8_t,
                                         key_blob,
                                         sizeof(BCRYPT_RSAKEY_BLOB) +
                                             key_blob->cbPublicExp),
                              key_blob->cbModulus))) {
        goto gp_fail;
    }

    // pub_exp
    if (!TT_OK(tt_blob_create(&pub_n->pub_exp,
                              TT_PTR_INC(tt_u8_t,
                                         key_blob,
                                         sizeof(BCRYPT_RSAKEY_BLOB)),
                              key_blob->cbPublicExp))) {
        goto gp_fail;
    }

    tt_free(key_blob);

    return TT_SUCCESS;

gp_fail:

    if (pub_n->modulus.addr != NULL) {
        tt_free(pub_n->modulus.addr);
    }

    if (pub_n->pub_exp.addr != NULL) {
        tt_free(pub_n->pub_exp.addr);
    }

    if (key_blob != NULL) {
        tt_free(key_blob);
    }

    return TT_FAIL;
}

tt_result_t __rsa_number_get_private(IN BCRYPT_KEY_HANDLE rsa,
                                     IN tt_rsa_privnum_t *priv_n)
{
    /*
     kSecFormatBSAFE:

     RSAPrivateKey ::= SEQUENCE {
        version Version,
        modulus INTEGER, -- n
        publicExponent INTEGER, -- e
        privateExponent INTEGER, -- d
        prime1 INTEGER, -- p
        prime2 INTEGER, -- q
        exponent1 INTEGER, -- d mod (p-1)
        exponent2 INTEGER, -- d mod (q-1)
        coefficient INTEGER, -- (inverse of q) mod p
        otherPrimeInfos OtherPrimeInfos OPTIONAL
     }
     */

    tt_memset(priv_n, 0, sizeof(tt_rsa_privnum_t));

    // use bcryptexportkey

    return TT_SUCCESS;

#if 0
  gp_fail:

    if (priv_n->modulus.addr != NULL)
    {
        tt_free(priv_n->modulus.addr, priv_n->modulus.len);
    }

    if (priv_n->pub_exp.addr != NULL)
    {
        tt_free(priv_n->pub_exp.addr, priv_n->pub_exp.len);
    }

    if (priv_n->priv_exp.addr != NULL)
    {
        tt_free(priv_n->priv_exp.addr, priv_n->priv_exp.len);
    }

    if (priv_n->prime1.addr != NULL)
    {
        tt_free(priv_n->prime1.addr, priv_n->prime1.len);
    }

    if (priv_n->prime2.addr != NULL)
    {
        tt_free(priv_n->prime2.addr, priv_n->prime2.len);
    }

    if (priv_n->exp1.addr != NULL)
    {
        tt_free(priv_n->exp1.addr, priv_n->exp1.len);
    }

    if (priv_n->exp2.addr != NULL)
    {
        tt_free(priv_n->exp2.addr, priv_n->exp2.len);
    }

    if (priv_n->coefficient.addr != NULL)
    {
        tt_free(priv_n->coefficient.addr, priv_n->coefficient.len);
    }

    return TT_FAIL;
#endif
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
