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

#include <algorithm/tt_buffer_format.h>
#include <crypto/tt_rsa.h>
#include <init/tt_profile.h>
#include <memory/tt_memory_alloc.h>
#include <misc/tt_der_decode.h>
#include <os/tt_atomic.h>

#ifdef TSCM_PLATFORM_CRYPTO_ENABLE

#include <CommonCrypto/CommonDigest.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define CFRelease_Safe(r)                                                      \
    do {                                                                       \
        if (r != NULL) {                                                       \
            CFRelease((r));                                                    \
        }                                                                      \
    } while (0)

#define __PARSE_INT(buf, blob, fail_label)                                     \
    do {                                                                       \
        tt_u8_t *__addr;                                                       \
        tt_u32_t __len;                                                        \
        tt_u8_t v8_1, v8_2;                                                    \
        tt_u32_t rd_pos, wr_pos;                                               \
                                                                               \
        TT_DO_G(fail_label,                                                    \
                tt_der_decode_head((buf), NULL, &__len, TT_FALSE));            \
        tt_buf_backup_rwp((buf), &rd_pos, &wr_pos);                            \
        TT_DO_G(fail_label, tt_buf_get_u8((buf), &v8_1));                      \
        TT_DO_G(fail_label, tt_buf_get_u8((buf), &v8_2));                      \
        tt_buf_restore_rwp((buf), &rd_pos, &wr_pos);                           \
        if ((v8_1 == 0) && (v8_2 > 0x80)) {                                    \
            /* ignore beginning 0 */                                           \
            tt_buf_inc_rp((buf), 1);                                           \
            __len -= 1;                                                        \
        }                                                                      \
                                                                               \
        __addr = (tt_u8_t *)tt_mem_alloc(__len);                               \
        if (__addr == NULL) {                                                  \
            TT_ERROR("no mem");                                                \
            goto fail_label;                                                   \
        }                                                                      \
        TT_DO_G(fail_label, tt_buf_get((buf), __addr, __len));                 \
        (blob)->addr = __addr;                                                 \
        (blob)->len = __len;                                                   \
    } while (0)

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

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_result_t __rsa_create_pkcs1(IN tt_rsa_ntv_t *sys_rsa,
                                      IN tt_rsa_type_t key_type,
                                      IN tt_blob_t *key_data,
                                      IN OPT tt_rsa_attr_t *attr);
static tt_result_t __rsa_create_pkcs8(IN tt_rsa_ntv_t *sys_rsa,
                                      IN tt_rsa_type_t key_type,
                                      IN tt_blob_t *key_data,
                                      IN OPT tt_rsa_attr_t *attr);
static tt_result_t __rsa_create_ntv(IN tt_rsa_ntv_t *sys_rsa,
                                    IN const tt_u8_t *key_data,
                                    IN tt_u32_t key_data_len,
                                    IN OPT tt_rsa_padding_attr_t *attr,
                                    IN SecExternalFormat inputFormat,
                                    IN SecExternalItemType itemType,
                                    IN SecItemImportExportFlags flags,
                                    IN const tt_char_t *password);

static tt_result_t __rsa_encrypt_padding(IN tt_rsa_ntv_t *sys_rsa,
                                         IN tt_u8_t *input,
                                         IN tt_u32_t input_len,
                                         OUT tt_u8_t *output,
                                         IN OUT tt_u32_t *output_len,
                                         CFTypeRef padding);
static tt_result_t __rsa_decrypt_padding(IN tt_rsa_ntv_t *sys_rsa,
                                         IN tt_u8_t *input,
                                         IN tt_u32_t input_len,
                                         OUT tt_u8_t *output,
                                         IN OUT tt_u32_t *output_len,
                                         CFTypeRef padding);

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
                                    OUT tt_u8_t *output,
                                    IN OUT tt_u32_t *output_len);
static tt_result_t __rsa_verify_pkcs1(IN tt_rsa_ntv_t *sys_rsa,
                                      IN tt_u8_t *input,
                                      IN tt_u32_t input_len,
                                      IN tt_u8_t *signature,
                                      IN tt_u32_t signature_len);

static __rsa_itf_t tt_s_rsa_itf[TT_RSA_PADDING_NUM] = {
    // TT_RSA_PADDING_NONE
    {
        __rsa_encrypt_none, __rsa_decrypt_none, NULL, NULL,
    },

    // TT_RSA_PADDING_OAEP
    {
        __rsa_encrypt_oaep, __rsa_decrypt_oaep, NULL, NULL,
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

static tt_result_t __rsa_number_get_public(
    IN CFDataRef key_data, IN struct tt_rsa_number_s *rsa_number);
static tt_result_t __rsa_number_get_private(
    IN CFDataRef key_data, IN struct tt_rsa_number_s *rsa_number);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_rsa_component_init_ntv(IN tt_profile_t *profile)
{
    return TT_SUCCESS;
}

tt_result_t tt_rsa_generate_ntv(OUT tt_rsa_ntv_t *sys_rsa,
                                IN tt_rsa_size_t size,
                                IN tt_rsa_attr_t *attr)
{
    CFDictionaryRef parameters;
    CFTypeRef keys[2];
    CFTypeRef values[2];
    OSStatus osst;
    CFErrorRef cf_error;

    SecTransformRef encrypter, decrypter;

    // generate rsa keys

    keys[0] = kSecAttrKeyType;
    values[0] = kSecAttrKeyTypeRSA;

    keys[1] = kSecAttrKeySizeInBits;
    if (size == TT_RSA_SIZE_1024BIT) {
        tt_s32_t n = 1024;
        values[1] = CFNumberCreate(NULL, kCFNumberSInt32Type, &n);
    } else // if (size == TT_RSA_SIZE_2048BIT)
    {
        tt_s32_t n = 2048;
        values[1] = CFNumberCreate(NULL, kCFNumberSInt32Type, &n);
    }

    parameters = CFDictionaryCreate(NULL,
                                    keys,
                                    values,
                                    2,
                                    &kCFCopyStringDictionaryKeyCallBacks,
                                    &kCFTypeDictionaryValueCallBacks);
    if (parameters == NULL) {
        TT_ERROR("fail to create rsa params");

        // release values[1]??
        return TT_FAIL;
    }

    osst =
        SecKeyGeneratePair(parameters, &sys_rsa->pub_key, &sys_rsa->priv_key);
    CFRelease(parameters);
    if (osst != errSecSuccess) {
        TT_ERROR("fail to generate rsa key");
        return TT_FAIL;
    }

    // it has both public and private key, so set it as private key
    sys_rsa->type = TT_RSA_TYPE_PRIVATE;

    return TT_SUCCESS;
}

tt_result_t tt_rsa_create_ntv(IN tt_rsa_ntv_t *sys_rsa,
                              IN tt_rsa_format_t key_format,
                              IN tt_rsa_type_t key_type,
                              IN tt_blob_t *key_data,
                              IN OPT tt_rsa_attr_t *attr)
{
    switch (key_format) {
        case TT_RSA_FORMAT_PKCS1: {
            return __rsa_create_pkcs1(sys_rsa, key_type, key_data, attr);
        } break;
        case TT_RSA_FORMAT_PKCS8: {
            return __rsa_create_pkcs8(sys_rsa, key_type, key_data, attr);
        } break;

        default: {
            TT_ERROR("invalid rsa format[%d]", key_format);
            return TT_FAIL;
        } break;
    }
}

void tt_rsa_destroy_ntv(IN tt_rsa_ntv_t *sys_rsa)
{
    if (sys_rsa->pub_key != NULL) {
        CFRelease(sys_rsa->pub_key);
    }
    if (sys_rsa->priv_key != NULL) {
        CFRelease(sys_rsa->priv_key);
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

tt_result_t __rsa_create_pkcs1(IN tt_rsa_ntv_t *sys_rsa,
                               IN tt_rsa_type_t key_type,
                               IN tt_blob_t *key_data,
                               IN OPT tt_rsa_attr_t *attr)
{
    // SecExternalFormat inputFormat = kSecFormatOpenSSL;
    SecExternalFormat inputFormat = kSecFormatUnknown;
    // the key can be either pkcs1 or wrapped with oid rsaEncryption, so
    // use unknown, mac os sec lib is able to detect the format

    SecExternalItemType itemType;
    SecItemImportExportFlags flags = 0;

    if (key_type == TT_RSA_TYPE_PUBLIC) {
        itemType = kSecItemTypePublicKey;
    } else {
        TT_ASSERT(key_type == TT_RSA_TYPE_PRIVATE);
        itemType = kSecItemTypePrivateKey;
    }

    if (attr->pem_armor) {
        flags |= kSecItemPemArmour;
    }

    return __rsa_create_ntv(sys_rsa,
                            key_data->addr,
                            key_data->len,
                            &attr->padding,
                            inputFormat,
                            itemType,
                            flags,
                            attr->password);

    return TT_SUCCESS;
}

tt_result_t __rsa_create_pkcs8(IN tt_rsa_ntv_t *sys_rsa,
                               IN tt_rsa_type_t key_type,
                               IN tt_blob_t *key_data,
                               IN OPT tt_rsa_attr_t *attr)
{
    SecExternalFormat inputFormat;
    SecExternalItemType itemType;
    SecItemImportExportFlags flags = 0;

    // for pkcs8 encrypted, key_data is already the plaintext
    inputFormat = kSecFormatBSAFE;

    if (key_type == TT_RSA_TYPE_PUBLIC) {
        itemType = kSecItemTypePublicKey;
    } else {
        TT_ASSERT(key_type == TT_RSA_TYPE_PRIVATE);
        itemType = kSecItemTypePrivateKey;
    }

    if (attr->pem_armor) {
        flags |= kSecItemPemArmour;
    }

    return __rsa_create_ntv(sys_rsa,
                            key_data->addr,
                            key_data->len,
                            &attr->padding,
                            inputFormat,
                            itemType,
                            flags,
                            attr->password);
}

tt_result_t __rsa_create_ntv(IN tt_rsa_ntv_t *sys_rsa,
                             IN const tt_u8_t *key_data,
                             IN tt_u32_t key_data_len,
                             IN OPT tt_rsa_padding_attr_t *attr,
                             IN SecExternalFormat inputFormat,
                             IN SecExternalItemType itemType,
                             IN SecItemImportExportFlags flags,
                             IN const tt_char_t *password)
{
    // remember to set rsa->size
    tt_rsa_t *rsa = TT_CONTAINER(sys_rsa, tt_rsa_t, sys_rsa);

    CFDataRef cf_key_data;
    SecItemImportExportKeyParameters keyParams = {0};
    CFArrayRef outItems = NULL;
    OSStatus osst;
    CFIndex i, n;

    SecKeyRef rsa_key_ref = NULL;
    CFErrorRef cf_error;
    size_t key_size;

    // create rsa key

    cf_key_data = CFDataCreate(NULL, (const UInt8 *)key_data, key_data_len);
    if (cf_key_data == NULL) {
        TT_ERROR("fail to create rsa key pem data");
        return TT_FAIL;
    }

    if (password != NULL) {
        keyParams.passphrase =
            CFStringCreateWithCString(NULL, password, kCFStringEncodingUTF8);
        if (keyParams.passphrase == NULL) {
            TT_ERROR("fail to create rsa key passphrase");

            CFRelease(cf_key_data);
            return TT_FAIL;
        }
    }

    osst = SecItemImport(cf_key_data,
                         NULL,
                         &inputFormat,
                         &itemType,
                         flags,
                         &keyParams,
                         NULL,
                         &outItems);
    CFRelease(cf_key_data);
    if (keyParams.passphrase != NULL) {
        CFRelease(keyParams.passphrase);
    }
    if ((osst != errSecSuccess) || (outItems == NULL)) {
        TT_ERROR("fail to import key");
        return TT_FAIL;
    }

    n = CFArrayGetCount(outItems);
    for (i = 0; i < n; ++i) {
        CFTypeRef r = CFArrayGetValueAtIndex(outItems, i);
        if ((r != NULL) && (CFGetTypeID(r) == SecKeyGetTypeID())) {
            rsa_key_ref = (SecKeyRef)r;
            CFRetain(r);
            break;
        }
    }
    CFRelease(outItems);
    if (rsa_key_ref == NULL) {
        TT_ERROR("no rsa key imported");
        return TT_FAIL;
    }

    key_size = SecKeyGetBlockSize(rsa_key_ref);
    if (key_size == 128) {
        rsa->size = TT_RSA_SIZE_1024BIT;
    } else if (key_size == 256) {
        rsa->size = TT_RSA_SIZE_2048BIT;
    } else {
        TT_ERROR("unsupported rsa key size[%d]", key_size);

        CFRelease(rsa_key_ref);
        return TT_FAIL;
    }

    if (itemType == kSecItemTypePublicKey) {
        sys_rsa->type = TT_RSA_TYPE_PUBLIC;
        sys_rsa->pub_key = rsa_key_ref;
        sys_rsa->priv_key = NULL;
    } else if (itemType == kSecItemTypePrivateKey) {
        sys_rsa->type = TT_RSA_TYPE_PRIVATE;
        sys_rsa->pub_key = NULL;
        sys_rsa->priv_key = rsa_key_ref;
    } else {
        TT_ERROR("incorrect rsa type");

        CFRelease(rsa_key_ref);
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t __rsa_encrypt_padding(IN tt_rsa_ntv_t *sys_rsa,
                                  IN tt_u8_t *input,
                                  IN tt_u32_t input_len,
                                  OUT tt_u8_t *output,
                                  IN OUT tt_u32_t *output_len,
                                  CFTypeRef padding)
{
    tt_rsa_t *rsa = TT_CONTAINER(sys_rsa, tt_rsa_t, sys_rsa);
    tt_result_t result = TT_FAIL;

    SecTransformRef encrypter = NULL;
    CFDataRef input_data = NULL;
    CFDataRef output_data = NULL;
    CFErrorRef cf_error;

    if (sys_rsa->pub_key == NULL) {
        TT_ERROR("no pub key");
        return TT_FAIL;
    }

    // secure transfrom would hang whenever it failed, so we have to
    // created one secure transform for each rsa operation

    // create secure transform
    encrypter = SecEncryptTransformCreate(sys_rsa->pub_key, &cf_error);
    if (encrypter == NULL) {
        TT_ERROR("fail to create pub transform");
        goto enc_out;
    }

    // set padding mode
    if (!SecTransformSetAttribute(encrypter,
                                  kSecPaddingKey,
                                  padding,
                                  &cf_error)) {
        TT_ERROR("fail to set kSecPaddingNoneKey");
        goto enc_out;
    }

    // prepare input
    input_data = CFDataCreate(NULL, (const UInt8 *)input, (CFIndex)input_len);
    if (input_data == NULL) {
        TT_ERROR("fail to create input data");
        goto enc_out;
    }

    if (!SecTransformSetAttribute(encrypter,
                                  kSecTransformInputAttributeName,
                                  input_data,
                                  &cf_error)) {
        TT_ERROR("fail to set input or output data");
        goto enc_out;
    }

    // run secure transform, get output
    output_data = SecTransformExecute(encrypter, &cf_error);
    if (output_data == NULL) {
        TT_ERROR("secure transfrom failed");
        goto enc_out;
    }

    // check output
    if (CFDataGetLength(output_data) > *output_len) {
        TT_ERROR("sec transform output data len[%d], exceed[%d]",
                 CFDataGetLength(output_data),
                 *output_len);
        goto enc_out;
    }

    memcpy(output, CFDataGetBytePtr(output_data), CFDataGetLength(output_data));
    *output_len = CFDataGetLength(output_data);
    result = TT_SUCCESS;

enc_out:

    if (output_data != NULL) {
        CFRelease(output_data);
    }

    if (input_data != NULL) {
        CFRelease(input_data);
    }

    if (encrypter != NULL) {
        CFRelease(encrypter);
    }

    return result;
}

tt_result_t __rsa_decrypt_padding(IN tt_rsa_ntv_t *sys_rsa,
                                  IN tt_u8_t *input,
                                  IN tt_u32_t input_len,
                                  OUT tt_u8_t *output,
                                  IN OUT tt_u32_t *output_len,
                                  CFTypeRef padding)
{
    tt_rsa_t *rsa = TT_CONTAINER(sys_rsa, tt_rsa_t, sys_rsa);
    tt_result_t result = TT_FAIL;

    SecTransformRef decrypter = NULL;
    CFDataRef input_data = NULL;
    CFDataRef output_data = NULL;
    CFErrorRef cf_error;

    if (sys_rsa->priv_key == NULL) {
        TT_ERROR("no pub key");
        return TT_FAIL;
    }

    // secure transfrom would hang whenever it failed, so we have to
    // created one secure transform for each rsa operation

    // create secure transform
    decrypter = SecDecryptTransformCreate(sys_rsa->priv_key, &cf_error);
    if (decrypter == NULL) {
        TT_ERROR("fail to create priv decrypt");
        goto dec_out;
    }

    // set padding mode
    if (!SecTransformSetAttribute(decrypter,
                                  kSecPaddingKey,
                                  padding,
                                  &cf_error)) {
        TT_ERROR("fail to set padding key");
        goto dec_out;
    }

    // prepare input
    input_data = CFDataCreate(NULL, (const UInt8 *)input, (CFIndex)input_len);
    if (input_data == NULL) {
        TT_ERROR("fail to create input data");
        goto dec_out;
    }

    if (!SecTransformSetAttribute(decrypter,
                                  kSecTransformInputAttributeName,
                                  input_data,
                                  &cf_error)) {
        TT_ERROR("fail to set input or output data");
        goto dec_out;
    }

    // run secure transform, get output
    output_data = SecTransformExecute(decrypter, &cf_error);
    if (output_data == NULL) {
        TT_ERROR("secure transfrom failed");
        goto dec_out;
    }

    // check output
    if (CFDataGetLength(output_data) > *output_len) {
        TT_ERROR("sec transform output data len[%d], exceed[%d]",
                 CFDataGetLength(output_data),
                 *output_len);
        goto dec_out;
    }

    memcpy(output, CFDataGetBytePtr(output_data), CFDataGetLength(output_data));
    *output_len = CFDataGetLength(output_data);
    result = TT_SUCCESS;

dec_out:

    if (output_data != NULL) {
        CFRelease(output_data);
    }

    if (input_data != NULL) {
        CFRelease(input_data);
    }

    if (decrypter != NULL) {
        CFRelease(decrypter);
    }

    return result;
}

tt_result_t __rsa_sign_padding(IN tt_rsa_ntv_t *sys_rsa,
                               IN tt_u8_t *input,
                               IN tt_u32_t input_len,
                               OUT tt_u8_t *signature,
                               IN OUT tt_u32_t *signature_len,
                               CFTypeRef padding)
{
    tt_rsa_t *rsa = TT_CONTAINER(sys_rsa, tt_rsa_t, sys_rsa);
    tt_result_t result = TT_FAIL;

    SecTransformRef signer = NULL;
    CFDataRef input_data = NULL;
    CFDataRef sig_data = NULL;
    CFErrorRef cf_error;

    if (sys_rsa->priv_key == NULL) {
        TT_ERROR("no priv key");
        return TT_FAIL;
    }

    // secure transfrom would hang whenever it failed, so we have to
    // created one secure transform for each rsa operation

    // create sign transform
    signer = SecSignTransformCreate(sys_rsa->priv_key, &cf_error);
    if (signer == NULL) {
        TT_ERROR("fail to create priv sign");
        goto sig_out;
    }

    // set padding mode
    if (!SecTransformSetAttribute(signer, kSecPaddingKey, padding, &cf_error)) {
        TT_ERROR("fail to set padding key");
        goto sig_out;
    }

    // always use sha1
    if (!SecTransformSetAttribute(signer,
                                  kSecDigestTypeAttribute,
                                  kSecDigestSHA1,
                                  &cf_error)) {
        TT_ERROR("fail to set sign digest");
        goto sig_out;
    }

    // prepare input
    input_data = CFDataCreate(NULL, (const UInt8 *)input, (CFIndex)input_len);
    if (input_data == NULL) {
        TT_ERROR("fail to create input data");
        goto sig_out;
    }

    if (!SecTransformSetAttribute(signer,
                                  kSecTransformInputAttributeName,
                                  input_data,
                                  &cf_error)) {
        TT_ERROR("fail to set input or output data");
        goto sig_out;
    }

    // run secure transform, get output
    sig_data = SecTransformExecute(signer, &cf_error);
    if (sig_data == NULL) {
        TT_ERROR("secure transfrom failed");
        goto sig_out;
    }

    // check output
    if (CFDataGetLength(sig_data) > *signature_len) {
        TT_ERROR("sec transform output data len[%d], exceed[%d]",
                 CFDataGetLength(sig_data),
                 *signature_len);
        goto sig_out;
    }

    memcpy(signature, CFDataGetBytePtr(sig_data), CFDataGetLength(sig_data));
    *signature_len = CFDataGetLength(sig_data);
    result = TT_SUCCESS;

sig_out:

    if (sig_data != NULL) {
        CFRelease(sig_data);
    }

    if (input_data != NULL) {
        CFRelease(input_data);
    }

    if (signer != NULL) {
        CFRelease(signer);
    }

    return result;
}

tt_result_t __rsa_verify_padding(IN tt_rsa_ntv_t *sys_rsa,
                                 IN tt_u8_t *input,
                                 IN tt_u32_t input_len,
                                 IN tt_u8_t *signature,
                                 IN tt_u32_t signature_len,
                                 CFTypeRef padding)
{
    tt_rsa_t *rsa = TT_CONTAINER(sys_rsa, tt_rsa_t, sys_rsa);
    tt_result_t result = TT_FAIL;

    SecTransformRef verifier = NULL;
    CFDataRef input_data = NULL;
    CFDataRef sig_data = NULL;
    CFBooleanRef verify_ret = NULL;
    CFErrorRef cf_error;

    if (sys_rsa->pub_key == NULL) {
        TT_ERROR("no pub key");
        return TT_FAIL;
    }

    // secure transfrom would hang whenever it failed, so we have to
    // created one secure transform for each rsa operation

    // prepare signature
    sig_data =
        CFDataCreate(NULL, (const UInt8 *)signature, (CFIndex)signature_len);
    if (sig_data == NULL) {
        TT_ERROR("fail to create signature data");
        goto pub_vfy_out;
    }

    // create secure transform
    verifier = SecVerifyTransformCreate(sys_rsa->pub_key, sig_data, &cf_error);
    if (verifier == NULL) {
        TT_ERROR("fail to create verify transform");
        goto pub_vfy_out;
    }

    // set padding mode
    if (!SecTransformSetAttribute(verifier,
                                  kSecPaddingKey,
                                  padding,
                                  &cf_error)) {
        TT_ERROR("fail to set kSecPaddingKey");
        goto pub_vfy_out;
    }

    // always use sha1
    if (!SecTransformSetAttribute(verifier,
                                  kSecDigestTypeAttribute,
                                  kSecDigestSHA1,
                                  &cf_error)) {
        TT_ERROR("fail to set verify digest");
        goto pub_vfy_out;
    }

    // prepare input
    input_data = CFDataCreate(NULL, (const UInt8 *)input, (CFIndex)input_len);
    if (input_data == NULL) {
        TT_ERROR("fail to create input data");
        goto pub_vfy_out;
    }

    if (!SecTransformSetAttribute(verifier,
                                  kSecTransformInputAttributeName,
                                  input_data,
                                  &cf_error)) {
        TT_ERROR("fail to set input or output data");
        goto pub_vfy_out;
    }

    // run secure transform, get output
    verify_ret = SecTransformExecute(verifier, &cf_error);
    if (verify_ret == kCFBooleanTrue) {
        result = TT_SUCCESS;
    }

pub_vfy_out:

    if (sig_data != NULL) {
        CFRelease(sig_data);
    }

    if (input_data != NULL) {
        CFRelease(input_data);
    }

    if (verifier != NULL) {
        CFRelease(verifier);
    }

    return result;
}

// TT_RSA_PADDING_NONE
tt_result_t __rsa_encrypt_none(IN tt_rsa_ntv_t *sys_rsa,
                               IN tt_u8_t *input,
                               IN tt_u32_t input_len,
                               OUT tt_u8_t *output,
                               IN OUT tt_u32_t *output_len)
{
    return __rsa_encrypt_padding(sys_rsa,
                                 input,
                                 input_len,
                                 output,
                                 output_len,
                                 kSecPaddingNoneKey);
}

tt_result_t __rsa_decrypt_none(IN tt_rsa_ntv_t *sys_rsa,
                               IN tt_u8_t *input,
                               IN tt_u32_t input_len,
                               OUT tt_u8_t *output,
                               IN OUT tt_u32_t *output_len)
{
    return __rsa_decrypt_padding(sys_rsa,
                                 input,
                                 input_len,
                                 output,
                                 output_len,
                                 kSecPaddingNoneKey);
}

// TT_RSA_PADDING_OAEP
tt_result_t __rsa_encrypt_oaep(IN tt_rsa_ntv_t *sys_rsa,
                               IN tt_u8_t *input,
                               IN tt_u32_t input_len,
                               OUT tt_u8_t *output,
                               IN OUT tt_u32_t *output_len)
{
    return __rsa_encrypt_padding(sys_rsa,
                                 input,
                                 input_len,
                                 output,
                                 output_len,
                                 kSecPaddingOAEPKey);
}

tt_result_t __rsa_decrypt_oaep(IN tt_rsa_ntv_t *sys_rsa,
                               IN tt_u8_t *input,
                               IN tt_u32_t input_len,
                               OUT tt_u8_t *output,
                               IN OUT tt_u32_t *output_len)
{
    return __rsa_decrypt_padding(sys_rsa,
                                 input,
                                 input_len,
                                 output,
                                 output_len,
                                 kSecPaddingOAEPKey);
}

// TT_RSA_PADDING_PKCS1
tt_result_t __rsa_encrypt_pkcs1(IN tt_rsa_ntv_t *sys_rsa,
                                IN tt_u8_t *input,
                                IN tt_u32_t input_len,
                                OUT tt_u8_t *output,
                                IN OUT tt_u32_t *output_len)
{
    // seems a special case: set padding to NULL but not pkcs1
    return __rsa_encrypt_padding(sys_rsa,
                                 input,
                                 input_len,
                                 output,
                                 output_len,
                                 NULL);
}

tt_result_t __rsa_decrypt_pkcs1(IN tt_rsa_ntv_t *sys_rsa,
                                IN tt_u8_t *input,
                                IN tt_u32_t input_len,
                                OUT tt_u8_t *output,
                                IN OUT tt_u32_t *output_len)
{
    // seems a special case: set padding to NULL but not pkcs1
    return __rsa_decrypt_padding(sys_rsa,
                                 input,
                                 input_len,
                                 output,
                                 output_len,
                                 NULL);
}

tt_result_t __rsa_sign_pkcs1(IN tt_rsa_ntv_t *sys_rsa,
                             IN tt_u8_t *input,
                             IN tt_u32_t input_len,
                             OUT tt_u8_t *output,
                             IN OUT tt_u32_t *output_len)
{
    return __rsa_sign_padding(sys_rsa,
                              input,
                              input_len,
                              output,
                              output_len,
                              NULL);
}

tt_result_t __rsa_verify_pkcs1(IN tt_rsa_ntv_t *sys_rsa,
                               IN tt_u8_t *input,
                               IN tt_u32_t input_len,
                               IN tt_u8_t *signature,
                               IN tt_u32_t signature_len)
{
    return __rsa_verify_padding(sys_rsa,
                                input,
                                input_len,
                                signature,
                                signature_len,
                                NULL);
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
        OSStatus osst;
        CFDataRef key_data;
        tt_result_t result;

        TT_ASSERT(sys_rsa->pub_key != NULL);
        osst = SecItemExport(sys_rsa->pub_key,
                             kSecFormatBSAFE,
                             0,
                             NULL,
                             &key_data);
        if (osst != errSecSuccess) {
            TT_INFO("  error");
            return TT_FAIL;
        }

        result = __rsa_number_get_public(key_data, rsa_number);
        CFRelease(key_data);

        return result;
    } else {
        OSStatus osst;
        CFDataRef key_data;
        tt_result_t result;

        TT_ASSERT(sys_rsa->type == TT_RSA_TYPE_PRIVATE);

        TT_ASSERT(sys_rsa->priv_key != NULL);
        osst = SecItemExport(sys_rsa->priv_key,
                             kSecFormatBSAFE,
                             0,
                             NULL,
                             &key_data);
        if (osst != errSecSuccess) {
            TT_INFO("  error");
            return TT_FAIL;
        }

        result = __rsa_number_get_private(key_data, rsa_number);
        CFRelease(key_data);

        return result;
    }
}

tt_result_t __rsa_number_get_public(IN CFDataRef key_data,
                                    IN struct tt_rsa_number_s *rsa_number)
{
    tt_rsa_pubnum_t *pub_num = &rsa_number->pubnum;

    tt_buf_t helper;
    tt_result_t result;

    /*
     kSecFormatBSAFE:

     RSAPublicKey ::= SEQUENCE {
        modulus INTEGER, -- n
        publicExponent INTEGER -- e
     }
     */

    result = tt_buf_create_nocopy(&helper,
                                  CFDataGetBytePtr(key_data),
                                  CFDataGetLength(key_data),
                                  NULL);
    if (!TT_OK(result)) {
        return TT_FAIL;
    }

    tt_rsa_number_init(rsa_number);

    TT_DO(tt_der_decode_head(&helper, NULL, NULL, TT_FALSE));

    // modulus
    __PARSE_INT(&helper, &pub_num->modulus, __gp_fail);

    // publicExponent
    __PARSE_INT(&helper, &pub_num->pub_exp, __gp_fail);

    tt_buf_destroy(&helper);
    return TT_SUCCESS;

__gp_fail:

    tt_buf_destroy(&helper);
    tt_rsa_number_destroy(rsa_number);
    return TT_FAIL;
}

tt_result_t __rsa_number_get_private(IN CFDataRef key_data,
                                     IN struct tt_rsa_number_s *rsa_number)
{
    tt_rsa_privnum_t *priv_num = &rsa_number->privnum;

    tt_buf_t helper;
    tt_result_t result;

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

    result = tt_buf_create_nocopy(&helper,
                                  CFDataGetBytePtr(key_data),
                                  CFDataGetLength(key_data),
                                  NULL);
    if (!TT_OK(result)) {
        return TT_FAIL;
    }

    tt_rsa_number_init(rsa_number);

    TT_DO(tt_der_decode_head(&helper, NULL, NULL, TT_FALSE));

    // version
    TT_DO(tt_der_decode_head(&helper, NULL, NULL, TT_TRUE));

    // modulus
    __PARSE_INT(&helper, &priv_num->modulus, __gp_fail);

    // publicExponent
    __PARSE_INT(&helper, &priv_num->pub_exp, __gp_fail);

    // privateExponent
    __PARSE_INT(&helper, &priv_num->priv_exp, __gp_fail);

    // prime1
    __PARSE_INT(&helper, &priv_num->prime1, __gp_fail);

    // prime2
    __PARSE_INT(&helper, &priv_num->prime2, __gp_fail);

    // exponent1
    __PARSE_INT(&helper, &priv_num->exp1, __gp_fail);

    // exponent2
    __PARSE_INT(&helper, &priv_num->exp2, __gp_fail);

    // coefficient
    __PARSE_INT(&helper, &priv_num->coefficient, __gp_fail);

    tt_buf_destroy(&helper);
    return TT_SUCCESS;

__gp_fail:

    tt_buf_destroy(&helper);
    tt_rsa_number_destroy(rsa_number);
    return TT_FAIL;
}

#endif
