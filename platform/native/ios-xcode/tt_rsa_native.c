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
#include <misc/tt_base64.h>
#include <misc/tt_der_decode.h>
#include <os/tt_atomic.h>

#include <tt_util_native.h>

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
        __addr = (tt_u8_t *)tt_malloc(__len);                                  \
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

extern void tt_osstatus_show(IN OSStatus osst);

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

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

static tt_atomic_s64_t tt_s_rsa_key_tag;

// rsaEncryption OID: "1.2.840.113549.1.1.1"
static tt_u8_t __oid_rsa_encryption[] =
    {0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __rsa_create(IN tt_rsa_ntv_t *sys_rsa,
                                IN tt_rsa_type_t key_type,
                                IN CFDataRef key_cfdata,
                                IN OPT tt_rsa_attr_t *attr);
static CFDataRef __pkcs1_public(IN tt_blob_t *key_data, IN tt_bool_t pem_armor);
static tt_result_t __pkcs1_pub_wrapped(IN tt_u8_t *der,
                                       IN tt_u32_t der_len,
                                       OUT tt_u8_t **bsafe_data,
                                       OUT tt_u32_t *bsafe_len);
static CFDataRef __pkcs1_private(IN tt_blob_t *key_data,
                                 IN tt_bool_t pem_armor);
static CFDataRef __pkcs8_bsafe(IN tt_blob_t *key_data, IN tt_bool_t pem_armor);

static tt_result_t __rsa_encrypt_padding(IN tt_rsa_ntv_t *sys_rsa,
                                         IN tt_u8_t *input,
                                         IN tt_u32_t input_len,
                                         OUT tt_u8_t *output,
                                         IN OUT tt_u32_t *output_len,
                                         SecPadding padding);
static tt_result_t __rsa_decrypt_padding(IN tt_rsa_ntv_t *sys_rsa,
                                         IN tt_u8_t *input,
                                         IN tt_u32_t input_len,
                                         OUT tt_u8_t *output,
                                         IN OUT tt_u32_t *output_len,
                                         SecPadding padding);
static tt_result_t __rsa_sign_padding(IN tt_rsa_ntv_t *sys_rsa,
                                      IN tt_u8_t *input,
                                      IN tt_u32_t input_len,
                                      OUT tt_u8_t *signature,
                                      IN OUT tt_u32_t *signature_len,
                                      SecPadding padding);
static tt_result_t __rsa_verify_padding(IN tt_rsa_ntv_t *sys_rsa,
                                        IN tt_u8_t *input,
                                        IN tt_u32_t input_len,
                                        IN tt_u8_t *signature,
                                        IN tt_u32_t signature_len,
                                        SecPadding padding);

static tt_result_t __rsa_number_get_public(
    IN CFDataRef key_data, IN struct tt_rsa_number_s *rsa_number);
static tt_result_t __rsa_number_get_private(
    IN CFDataRef key_data, IN struct tt_rsa_number_s *rsa_number);

static CFDataRef __gen_key_tag(IN tt_s64_t *key_tag);

static CFDataRef __rsa_b64_decode(IN tt_u8_t *key, IN tt_u32_t key_len);

static CFDataRef __rsa_data_get(IN tt_rsa_ntv_t *sys_rsa);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_rsa_component_init_ntv(IN tt_profile_t *profile)
{
    tt_atomic_s64_init(&tt_s_rsa_key_tag, 0);

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
            CFDataRef key_cfdata;
            tt_result_t result;

            if (key_type == TT_RSA_TYPE_PUBLIC) {
                key_cfdata = __pkcs1_public(key_data, attr->pem_armor);
            } else {
                key_cfdata = __pkcs1_private(key_data, attr->pem_armor);
            }
            if (key_cfdata == NULL) {
                TT_ERROR("fail to get rsa key BSAFE format");
                return TT_FAIL;
            }

            result = __rsa_create(sys_rsa, key_type, key_cfdata, attr);
            CFRelease(key_cfdata);

            return result;
        } break;
        case TT_RSA_FORMAT_PKCS8: {
            CFDataRef key_cfdata;
            tt_result_t result;

            key_cfdata = __pkcs8_bsafe(key_data, attr->pem_armor);
            if (key_cfdata == NULL) {
                TT_ERROR("fail to get rsa key BSAFE format");
                return TT_FAIL;
            }

            result = __rsa_create(sys_rsa, key_type, key_cfdata, attr);
            CFRelease(key_cfdata);

            return result;
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

tt_result_t __rsa_create(IN tt_rsa_ntv_t *sys_rsa,
                         IN tt_rsa_type_t key_type,
                         IN CFDataRef key_cfdata,
                         IN OPT tt_rsa_attr_t *attr)
{
    tt_rsa_t *rsa = TT_CONTAINER(sys_rsa, tt_rsa_t, sys_rsa);
    size_t key_size;
    tt_s64_t rsa_tag;

    CFMutableDictionaryRef attributes = NULL;
    CFTypeRef keys[6], values[6];
    CFDataRef key_tag = NULL;

    SecKeyRef key_ref = NULL;
    OSStatus osst;

    attributes = CFDictionaryCreateMutable(NULL,
                                           6,
                                           &kCFCopyStringDictionaryKeyCallBacks,
                                           &kCFTypeDictionaryValueCallBacks);
    if (attributes == NULL) {
        TT_ERROR("fail to create rsa attr dic");
        return TT_FAIL;
    }

    // class
    CFDictionaryAddValue(attributes, kSecClass, kSecClassKey);

    // key type
    CFDictionaryAddValue(attributes, kSecAttrKeyType, kSecAttrKeyTypeRSA);

    // key class
    if (key_type == TT_RSA_TYPE_PUBLIC) {
        CFDictionaryAddValue(attributes,
                             kSecAttrKeyClass,
                             kSecAttrKeyClassPublic);
    } else {
        CFDictionaryAddValue(attributes,
                             kSecAttrKeyClass,
                             kSecAttrKeyClassPrivate);
    }

    // tag
    key_tag = __gen_key_tag(&rsa_tag);
    if (key_tag == NULL) {
        TT_ERROR("fail to generate key tag");

        CFRelease(attributes);
        return TT_FAIL;
    }
    CFDictionaryAddValue(attributes, kSecAttrApplicationTag, key_tag);
    CFRelease(key_tag);

    // remove if already exsit
    SecItemDelete(attributes);

    // data
    CFDictionaryAddValue(attributes, kSecValueData, key_cfdata);

    // return type
    CFDictionaryAddValue(attributes, kSecReturnRef, kCFBooleanTrue);

    // import
    osst = SecItemAdd(attributes, (CFTypeRef *)&key_ref);
    CFRelease(attributes);
    if (osst != errSecSuccess) {
        TT_ERROR("fail to import rsa key");
        tt_osstatus_show(osst);
        return TT_FAIL;
    }
    if (key_ref == NULL) {
        TT_ERROR("fail to import rsa key");
        return TT_FAIL;
    }

    // rsa key size
    key_size = SecKeyGetBlockSize(key_ref);
    if (key_size == 128) {
        rsa->size = TT_RSA_SIZE_1024BIT;
    } else if (key_size == 256) {
        rsa->size = TT_RSA_SIZE_2048BIT;
    } else {
        TT_ERROR("unsupported rsa key size[%d]", key_size);

        CFRelease(key_ref);
        return TT_FAIL;
    }

    if (key_type == TT_RSA_TYPE_PUBLIC) {
        sys_rsa->type = TT_RSA_TYPE_PUBLIC;
        sys_rsa->pub_key = key_ref;
        sys_rsa->pub_key_tag = rsa_tag;
        sys_rsa->priv_key = NULL;
        sys_rsa->priv_key_tag = 0;
    } else if (key_type == TT_RSA_TYPE_PRIVATE) {
        sys_rsa->type = TT_RSA_TYPE_PRIVATE;
        sys_rsa->pub_key = NULL;
        sys_rsa->pub_key_tag = 0;
        sys_rsa->priv_key = key_ref;
        sys_rsa->priv_key_tag = rsa_tag;
    } else {
        TT_ERROR("incorrect rsa type");

        CFRelease(key_ref);
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

CFDataRef __pkcs1_public(IN tt_blob_t *key_data, IN tt_bool_t pem_armor)
{
    CFDataRef pkcs1_cfdata = NULL;
    tt_u8_t *der = key_data->addr;
    tt_u32_t der_len = key_data->len;
    tt_result_t result;

    tt_u8_t *bsafe_data;
    tt_u32_t bsafe_len;

    // may need do der decoding
    if (pem_armor) {
        tt_base64_decode_attr_t attr;

        tt_base64_decode_attr_default(&attr);
        attr.start_boundary = TT_BASE64_START_LF;
        attr.end_boundary = TT_BASE64_END_LF;

        result = tt_base64_decode_alloc(key_data->addr,
                                        key_data->len,
                                        &attr,
                                        &der,
                                        &der_len);
        if (!TT_OK(result)) {
            return NULL;
        }
    }

    /* assume it's wrapped publick key:
     SubjectPublicKeyInfo ::= SEQUENCE
     {
        algorithm AlgorithmIdentifier,
        subjectPublicKey BIT STRING
     }
     */
    result = __pkcs1_pub_wrapped(der, der_len, &bsafe_data, &bsafe_len);
    if (!TT_OK(result)) {
        // assume it's already of bsafe format
        bsafe_data = der;
        bsafe_len = der_len;
    }

    // return bsafe data
    pkcs1_cfdata = CFDataCreate(NULL, bsafe_data, bsafe_len);
    if (pkcs1_cfdata == NULL) {
        TT_ERROR("no mem for pkcs1 key");
        goto p1pub_out;
    }

p1pub_out:

    if (pem_armor) {
        tt_free(der);
    }

    return pkcs1_cfdata;
}

tt_result_t __pkcs1_pub_wrapped(IN tt_u8_t *der,
                                IN tt_u32_t der_len,
                                OUT tt_u8_t **bsafe_data,
                                OUT tt_u32_t *bsafe_len)
{
    tt_buf_t buf;
    tt_u8_t *p;
    tt_u32_t len, pad_bit_num;

    // strip wrapper, get pkcs1 data
    if (!TT_OK(tt_buf_create_nocopy(&buf, der, der_len, NULL))) {
        return TT_FAIL;
    }

    /*
     0:d=0   hl=3 l= 159 cons: SEQUENCE
     3:d=1   hl=2 l=  13 cons:  SEQUENCE
     5:d=2   hl=2 l=   9 prim:   OBJECT            :rsaEncryption
     16:d=2  hl=2 l=   0 prim:   NULL
     18:d=1  hl=3 l= 141 prim:  BIT STRING
     */

    TT_DO_G(p1pw_fail, tt_der_decode_sequence(&buf, NULL, 0));

    TT_DO_G(p1pw_fail, tt_der_decode_sequence(&buf, NULL, 0));
    TT_DO_G(p1pw_fail, tt_der_decode_oid(&buf, &p, &len, TT_DER_PASS_CONTENT));
    TT_DO_G(p1pw_fail, tt_der_decode_null(&buf, 0));
    if ((len != sizeof(__oid_rsa_encryption)) ||
        (tt_memcmp(p, __oid_rsa_encryption, len) != 0)) {
        TT_ERROR("not rsa key");
        goto p1pw_fail;
    }

    TT_DO_G(p1pw_fail, tt_der_decode_bitstr(&buf, &p, &len, &pad_bit_num, 0));
    if (pad_bit_num != 0) {
        TT_ERROR("padded bit should be 0");
        goto p1pw_fail;
    }

    tt_buf_destroy(&buf);

    *bsafe_data = p;
    *bsafe_len = len;
    return TT_SUCCESS;

p1pw_fail:

    tt_buf_destroy(&buf);

    return TT_FAIL;
}

CFDataRef __pkcs1_private(IN tt_blob_t *key_data, IN tt_bool_t pem_armor)
{
    CFDataRef pkcs1_cfdata = NULL;
    tt_u8_t *der = key_data->addr;
    tt_u32_t der_len = key_data->len;
    tt_result_t result;

    // may need do der decoding
    if (pem_armor) {
        tt_base64_decode_attr_t attr;

        tt_base64_decode_attr_default(&attr);
        attr.start_boundary = TT_BASE64_START_LF;
        attr.end_boundary = TT_BASE64_END_LF;

        result = tt_base64_decode_alloc(key_data->addr,
                                        key_data->len,
                                        &attr,
                                        &der,
                                        &der_len);
        if (!TT_OK(result)) {
            return NULL;
        }
    }

    // cfdata
    pkcs1_cfdata = CFDataCreate(NULL, der, der_len);

    if (pem_armor) {
        tt_free(der);
    }

    return pkcs1_cfdata;
}

CFDataRef __pkcs8_bsafe(IN tt_blob_t *key_data, IN tt_bool_t pem_armor)
{
    CFDataRef bsafe_cfdata = NULL;
    tt_u8_t *der = key_data->addr;
    tt_u32_t der_len = key_data->len;
    tt_result_t result;

    tt_u8_t *p;
    tt_u32_t len;
    tt_buf_t buf;

    // may need do der decoding
    if (pem_armor) {
        tt_base64_decode_attr_t attr;

        tt_base64_decode_attr_default(&attr);
        attr.start_boundary = TT_BASE64_START_LF;
        attr.end_boundary = TT_BASE64_END_LF;

        result = tt_base64_decode_alloc(key_data->addr,
                                        key_data->len,
                                        &attr,
                                        &der,
                                        &der_len);
        if (!TT_OK(result)) {
            return NULL;
        }
    }

    // pkcs8 to bsafe
    result = tt_buf_create_nocopy(&buf, der, der_len, NULL);
    if (!TT_OK(result)) {
        TT_ERROR("fail to create pkcs8 decode stream");

        if (pem_armor) {
            tt_free(der);
        }
        return NULL;
    }

    /*
     PrivateKeyInfo ::= SEQUENCE
     {
        version                   Version,
        privateKeyAlgorithm       PrivateKeyAlgorithmIdentifier,
        privateKey                PrivateKey,
        attributes           [0]  IMPLICIT Attributes OPTIONAL
     }

     PrivateKey ::= OCTET STRING
     */
    TT_DO_G(p8_out, tt_der_decode_sequence(&buf, NULL, 0));
    TT_DO_G(p8_out, tt_der_decode_s32(&buf, NULL, NULL, TT_DER_PASS_CONTENT));
    // ignore checking privateKeyAlgorithm??
    TT_DO_G(p8_out, tt_der_decode_sequence(&buf, NULL, TT_DER_PASS_CONTENT));
    TT_DO_G(p8_out, tt_der_decode_octstr(&buf, &p, &len, 0));
    // now p the private key

    // cfdata
    bsafe_cfdata = CFDataCreate(NULL, p, len);

p8_out:

    tt_buf_destroy(&buf);

    if (pem_armor) {
        tt_free(der);
    }

    return bsafe_cfdata;
}

tt_result_t __rsa_encrypt_padding(IN tt_rsa_ntv_t *sys_rsa,
                                  IN tt_u8_t *input,
                                  IN tt_u32_t input_len,
                                  OUT tt_u8_t *output,
                                  IN OUT tt_u32_t *output_len,
                                  SecPadding padding)
{
    tt_rsa_t *rsa = TT_CONTAINER(sys_rsa, tt_rsa_t, sys_rsa);
    tt_result_t result = TT_FAIL;

    OSStatus osst;
    size_t cipherTextLen = *output_len;

    osst = SecKeyEncrypt(sys_rsa->pub_key,
                         padding,
                         input,
                         input_len,
                         output,
                         &cipherTextLen);
    if (osst != errSecSuccess) {
        TT_ERROR("rsa enc failed");
        tt_osstatus_show(osst);
        return TT_FAIL;
    }

    *output_len = (tt_u32_t)cipherTextLen;
    return TT_SUCCESS;
}

tt_result_t __rsa_decrypt_padding(IN tt_rsa_ntv_t *sys_rsa,
                                  IN tt_u8_t *input,
                                  IN tt_u32_t input_len,
                                  OUT tt_u8_t *output,
                                  IN OUT tt_u32_t *output_len,
                                  SecPadding padding)
{
    tt_rsa_t *rsa = TT_CONTAINER(sys_rsa, tt_rsa_t, sys_rsa);
    tt_result_t result = TT_FAIL;

    OSStatus osst;
    size_t cipherTextLen = *output_len;

    osst = SecKeyDecrypt(sys_rsa->priv_key,
                         padding,
                         input,
                         input_len,
                         output,
                         &cipherTextLen);
    if (osst != errSecSuccess) {
        TT_ERROR("rsa dec failed");
        tt_osstatus_show(osst);
        return TT_FAIL;
    }

    *output_len = (tt_u32_t)cipherTextLen;
    return TT_SUCCESS;
}

tt_result_t __rsa_sign_padding(IN tt_rsa_ntv_t *sys_rsa,
                               IN tt_u8_t *input,
                               IN tt_u32_t input_len,
                               OUT tt_u8_t *signature,
                               IN OUT tt_u32_t *signature_len,
                               SecPadding padding)
{
    tt_rsa_t *rsa = TT_CONTAINER(sys_rsa, tt_rsa_t, sys_rsa);
    tt_result_t result = TT_FAIL;

    OSStatus osst;
    size_t cipherTextLen = *signature_len;

    osst = SecKeyRawSign(sys_rsa->priv_key,
                         padding,
                         input,
                         input_len,
                         signature,
                         &cipherTextLen);
    if (osst != errSecSuccess) {
        TT_ERROR("rsa sign failed");
        tt_osstatus_show(osst);
        return TT_FAIL;
    }

    *signature_len = (tt_u32_t)cipherTextLen;
    return TT_SUCCESS;
}

tt_result_t __rsa_verify_padding(IN tt_rsa_ntv_t *sys_rsa,
                                 IN tt_u8_t *input,
                                 IN tt_u32_t input_len,
                                 IN tt_u8_t *signature,
                                 IN tt_u32_t signature_len,
                                 SecPadding padding)
{
    tt_rsa_t *rsa = TT_CONTAINER(sys_rsa, tt_rsa_t, sys_rsa);
    tt_result_t result = TT_FAIL;

    OSStatus osst;

    osst = SecKeyRawVerify(sys_rsa->pub_key,
                           padding,
                           input,
                           input_len,
                           signature,
                           signature_len);
    if (osst != errSecSuccess) {
        TT_ERROR("rsa verify failed");
        tt_osstatus_show(osst);
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

// TT_RSA_PADDING_NONE
tt_result_t __rsa_encrypt_none(IN tt_rsa_ntv_t *sys_rsa,
                               IN tt_u8_t *input,
                               IN tt_u32_t input_len,
                               OUT tt_u8_t *output,
                               IN OUT tt_u32_t *output_len)
{
    tt_rsa_t *rsa = TT_CONTAINER(sys_rsa, tt_rsa_t, sys_rsa);

    if (input_len != rsa->block_size) {
        // ios sec lib does not return fail when none padding and input is
        // shorter than block size
        TT_ERROR("rsa none padding, input_len should be same as rsa size");
        return TT_FAIL;
    }

    return __rsa_encrypt_padding(sys_rsa,
                                 input,
                                 input_len,
                                 output,
                                 output_len,
                                 kSecPaddingNone);
}

tt_result_t __rsa_decrypt_none(IN tt_rsa_ntv_t *sys_rsa,
                               IN tt_u8_t *input,
                               IN tt_u32_t input_len,
                               OUT tt_u8_t *output,
                               IN OUT tt_u32_t *output_len)
{
    tt_rsa_t *rsa = TT_CONTAINER(sys_rsa, tt_rsa_t, sys_rsa);

    if (input_len != rsa->block_size) {
        // ios sec lib does not return fail when none padding and input is
        // shorter than block size
        TT_ERROR("rsa none padding, input_len should be same as rsa size");
        return TT_FAIL;
    }

    return __rsa_decrypt_padding(sys_rsa,
                                 input,
                                 input_len,
                                 output,
                                 output_len,
                                 kSecPaddingNone);
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
                                 kSecPaddingOAEP);
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
                                 kSecPaddingOAEP);
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
                                 kSecPaddingPKCS1);
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
                                 kSecPaddingPKCS1);
}

tt_result_t __rsa_sign_pkcs1(IN tt_rsa_ntv_t *sys_rsa,
                             IN tt_u8_t *input,
                             IN tt_u32_t input_len,
                             OUT tt_u8_t *output,
                             IN OUT tt_u32_t *output_len)
{
    uint8_t hash[CC_SHA1_DIGEST_LENGTH];

    // always use sha1
    CC_SHA1(input, input_len, hash);

    return __rsa_sign_padding(sys_rsa,
                              hash,
                              sizeof(hash),
                              output,
                              output_len,
                              kSecPaddingPKCS1SHA1);
}

tt_result_t __rsa_verify_pkcs1(IN tt_rsa_ntv_t *sys_rsa,
                               IN tt_u8_t *input,
                               IN tt_u32_t input_len,
                               IN tt_u8_t *signature,
                               IN tt_u32_t signature_len)
{
    uint8_t hash[CC_SHA1_DIGEST_LENGTH];

    // always use sha1
    CC_SHA1(input, input_len, hash);

    return __rsa_verify_padding(sys_rsa,
                                hash,
                                sizeof(hash),
                                signature,
                                signature_len,
                                kSecPaddingPKCS1SHA1);
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
        CFDataRef key_data;
        tt_result_t result;

        TT_ASSERT(sys_rsa->pub_key != NULL);
        key_data = __rsa_data_get(sys_rsa);
        if (key_data == NULL) {
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
#if 0 // todo use SecItemCopyMatching to export
        osst = SecItemExport(sys_rsa->priv_key,
                             kSecFormatBSAFE,
                             0,
                             NULL,
                             &key_data);
#else
        osst = errSecUserCanceled;
        key_data = NULL;
#endif
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
                                  (tt_u32_t)CFDataGetLength(key_data),
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
                                  (tt_u32_t)CFDataGetLength(key_data),
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

CFDataRef __gen_key_tag(IN tt_s64_t *key_tag)
{
    tt_s64_t new_tag = tt_atomic_s64_inc(&tt_s_rsa_key_tag);

    TT_SAFE_ASSIGN(key_tag, new_tag);
    return CFDataCreate(NULL, (const UInt8 *)&new_tag, sizeof(tt_s64_t));
}

CFDataRef __rsa_data_get(IN tt_rsa_ntv_t *sys_rsa)
{
    tt_rsa_t *rsa = TT_CONTAINER(sys_rsa, tt_rsa_t, sys_rsa);

    CFMutableDictionaryRef query;
    CFDataRef data;
    OSStatus osst;

    query = CFDictionaryCreateMutable(NULL,
                                      5,
                                      &kCFCopyStringDictionaryKeyCallBacks,
                                      &kCFTypeDictionaryValueCallBacks);
    if (query == NULL) {
        TT_ERROR("fail to create rsa query dic");
        return NULL;
    }

    // class
    CFDictionaryAddValue(query, kSecClass, kSecClassKey);

    // key type
    CFDictionaryAddValue(query, kSecAttrKeyType, kSecAttrKeyTypeRSA);

    // key class
    if (sys_rsa->type == TT_RSA_TYPE_PUBLIC) {
        CFDictionaryAddValue(query, kSecAttrKeyClass, kSecAttrKeyClassPublic);
    } else {
        CFDictionaryAddValue(query, kSecAttrKeyClass, kSecAttrKeyClassPrivate);
    }

    // tag
    if (sys_rsa->type == TT_RSA_TYPE_PUBLIC) {
        data = CFDataCreate(NULL,
                            (const UInt8 *)&sys_rsa->pub_key_tag,
                            sizeof(sys_rsa->pub_key_tag));
    } else {
        data = CFDataCreate(NULL,
                            (const UInt8 *)&sys_rsa->priv_key_tag,
                            sizeof(sys_rsa->pub_key_tag));
    }
    if (data == NULL) {
        TT_ERROR("fail to create key tag");

        CFRelease(query);
        return NULL;
    }
    CFDictionaryAddValue(query, kSecAttrApplicationTag, data);
    CFRelease(data);

    // return data
    CFDictionaryAddValue(query, kSecReturnData, kCFBooleanTrue);

    osst = SecItemCopyMatching(query, (CFTypeRef *)&data);
    CFRelease(query);
    if (osst != errSecSuccess) {
        TT_ERROR("rsa key not found");
        tt_osstatus_show(osst);
        return NULL;
    }

    return data;
}

#endif
