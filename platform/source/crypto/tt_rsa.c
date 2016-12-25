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

#include <crypto/tt_rsa.h>

#include <algorithm/tt_buffer_format.h>
#include <crypto/tt_pkcs8.h>
#include <init/tt_component.h>
#include <memory/tt_memory_alloc.h>
#include <misc/tt_base64.h>
#include <misc/tt_der_encode.h>

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

static tt_u8_t __oid_rsa_enc[] =
    {0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_u32_t __rsa_block_size(IN tt_rsa_t *rsa);
static tt_u32_t __rsa_min_padding_size(IN tt_rsa_t *rsa);

static tt_result_t __rsa_pkcs8_decrypt(IN tt_blob_t *encrypted,
                                       IN tt_rsa_attr_t *rsa_attr,
                                       OUT tt_u8_t *decrypted,
                                       IN OUT tt_u32_t *decrypted_len);

static tt_result_t __rsa_pub_der(IN tt_buf_t *buf, IN tt_rsa_pubnum_t *num);
static tt_result_t __rsa_pub_der_keyinfo(IN tt_buf_t *buf,
                                         IN tt_rsa_pubnum_t *num);
static tt_result_t __rsa_priv_der(IN tt_buf_t *buf, IN tt_rsa_privnum_t *num);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_rsa_generate(OUT tt_rsa_t *rsa,
                            IN tt_rsa_size_t size,
                            IN OPT tt_rsa_attr_t *attr)
{
    TT_ASSERT(rsa != NULL);
    TT_ASSERT(TT_RSA_SIZE_VALID(size));

    rsa->size = size;

    if (attr != NULL) {
        tt_memcpy(&rsa->attr, attr, sizeof(tt_rsa_attr_t));
    } else {
        tt_rsa_attr_default(&rsa->attr);
    }
    TT_ASSERT(TT_RSA_PADDING_VALID(rsa->attr.padding.mode));

    rsa->block_size = __rsa_block_size(rsa);
    rsa->min_padding_size = __rsa_min_padding_size(rsa);
    TT_ASSERT(rsa->block_size > rsa->min_padding_size);
    rsa->max_data_size = rsa->block_size - rsa->min_padding_size;

    return tt_rsa_generate_ntv(&rsa->sys_rsa, size, &rsa->attr);
}

tt_result_t tt_rsa_create(IN tt_rsa_t *rsa,
                          IN tt_rsa_format_t key_format,
                          IN tt_rsa_type_t key_type,
                          IN tt_blob_t *key_data,
                          IN OPT tt_rsa_attr_t *attr)
{
    tt_result_t result;

    TT_ASSERT(rsa != NULL);
    TT_ASSERT(TT_RSA_FORMAT_VALID(key_format));
    TT_ASSERT(TT_RSA_TYPE_VALID(key_type));
    TT_ASSERT((key_data != NULL) && (key_data->addr != NULL) &&
              (key_data->len != 0));

    // set to invalid
    rsa->size = TT_RSA_SIZE_NUM;

    if (attr != NULL) {
        tt_memcpy(&rsa->attr, attr, sizeof(tt_rsa_attr_t));
    } else {
        tt_rsa_attr_default(&rsa->attr);
    }
    TT_ASSERT(TT_RSA_PADDING_VALID(rsa->attr.padding.mode));

    if (rsa->attr.password != NULL) {
        tt_blob_t __key_data;
        tt_u8_t *dec_buf;
        tt_u32_t dec_len, dec_data_len;

        dec_len = key_data->len;
        dec_buf = (tt_u8_t *)tt_malloc(dec_len);
        if (dec_buf == NULL) {
            TT_ERROR("no mem to decrypt rsa pksc8 priv key");
            return TT_FAIL;
        }

        dec_data_len = dec_len;
        result =
            __rsa_pkcs8_decrypt(key_data, &rsa->attr, dec_buf, &dec_data_len);
        if (!TT_OK(result)) {
            tt_free(dec_buf);
            return TT_FAIL;
        }

        // could we change rsa->attr??
        rsa->attr.password = NULL;
        rsa->attr.pem_armor = TT_FALSE;

        // remember to set rsa->size
        __key_data.addr = dec_buf;
        __key_data.len = dec_data_len;
        result = tt_rsa_create_ntv(&rsa->sys_rsa,
                                   key_format,
                                   key_type,
                                   &__key_data,
                                   &rsa->attr);
        tt_free(dec_buf);
    } else {
        // remember to set rsa->size
        result = tt_rsa_create_ntv(&rsa->sys_rsa,
                                   key_format,
                                   key_type,
                                   key_data,
                                   &rsa->attr);
    }
    if (TT_OK(result)) {
        TT_ASSERT(TT_RSA_SIZE_VALID(rsa->size));

        rsa->block_size = __rsa_block_size(rsa);
        rsa->min_padding_size = __rsa_min_padding_size(rsa);
        TT_ASSERT(rsa->block_size > rsa->min_padding_size);
        rsa->max_data_size = rsa->block_size - rsa->min_padding_size;

        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_result_t tt_rsa_create_number(IN tt_rsa_t *rsa,
                                 IN tt_rsa_number_t *rsa_num,
                                 IN OPT tt_rsa_attr_t *attr)
{
    tt_buf_t buf;
    tt_result_t result = TT_FAIL;

    TT_ASSERT(rsa != NULL);
    TT_ASSERT(rsa_num != NULL);

    // not all platforms support import raw rsa numbers into a rsa key
    // so the solution is encode these numbers as a asn1 structure and
    // pass it to tt_rsa_create();

    tt_buf_init(&buf, NULL);

    if (rsa_num->type == TT_RSA_TYPE_PUBLIC) {
        // note openssl can not recognize PublicKeyInfo
        result = __rsa_pub_der(&buf, &rsa_num->pubnum);
        // result = __rsa_pub_der_keyinfo(&buf, &rsa_num->pubnum);
    } else {
        TT_ASSERT(rsa_num->type == TT_RSA_TYPE_PRIVATE);
        result = __rsa_priv_der(&buf, &rsa_num->privnum);
    }

    if (TT_OK(result)) {
        tt_blob_t key_data;
        tt_rsa_attr_t __attr;

        tt_buf_getptr_rpblob(&buf, &key_data);

        if (attr == NULL) {
            tt_rsa_attr_default(&__attr);
            attr = &__attr;
        }
        attr->pem_armor = TT_FALSE;

        result = tt_rsa_create(rsa,
                               TT_RSA_FORMAT_PKCS1,
                               rsa_num->type,
                               &key_data,
                               attr);
    }

    tt_buf_destroy(&buf);

    return result;
}

void tt_rsa_destroy(IN tt_rsa_t *rsa)
{
    TT_ASSERT(rsa != NULL);

    tt_rsa_destroy_ntv(&rsa->sys_rsa);
}

void tt_rsa_attr_default(IN tt_rsa_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->password = NULL;

    tt_rsa_padding_attr_default(&attr->padding);

    attr->pem_armor = TT_TRUE;
}

void tt_rsa_padding_attr_default(IN tt_rsa_padding_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    tt_memset(attr, 0, sizeof(tt_rsa_padding_attr_t));

    attr->mode = TT_RSA_PADDING_NONE;
}

tt_result_t tt_rsa_encrypt(IN tt_rsa_t *rsa,
                           IN tt_u8_t *input,
                           IN tt_u32_t input_len,
                           OUT tt_u8_t *output,
                           IN OUT tt_u32_t *output_len)
{
    TT_ASSERT(rsa != NULL);
    TT_ASSERT(input != NULL);
    TT_ASSERT(output != NULL);
    TT_ASSERT(output_len != NULL);

    if (input_len == 0) {
        *output_len = 0;
        return TT_SUCCESS;
    }

    if (input_len > rsa->max_data_size) {
        TT_ERROR("input[%d] exceed max rsa data size[%d]",
                 input_len,
                 rsa->max_data_size);
        return TT_FAIL;
    }

    if (*output_len < rsa->block_size) {
        TT_ERROR("output[%d] is less than rsa block size[%d]",
                 *output_len,
                 rsa->block_size);
        return TT_FAIL;
    }

    return tt_rsa_encrypt_ntv(&rsa->sys_rsa,
                              input,
                              input_len,
                              output,
                              output_len);
}

tt_result_t tt_rsa_encrypt_buf(IN tt_rsa_t *rsa,
                               IN tt_u8_t *input,
                               IN tt_u32_t input_len,
                               OUT tt_buf_t *output)
{
    tt_u32_t output_len;

    TT_ASSERT(rsa != NULL);
    TT_ASSERT(input != NULL);
    TT_ASSERT(output != NULL);

    if (input_len == 0) {
        return TT_SUCCESS;
    }

    if (input_len > rsa->max_data_size) {
        TT_ERROR("input[%d] exceed max rsa data size[%d]",
                 input_len,
                 rsa->max_data_size);
        return TT_FAIL;
    }

    output_len = rsa->block_size;
    if (!TT_OK(tt_buf_reserve(output, output_len))) {
        TT_ERROR("fail to reserve encrypt output space");
        return TT_FAIL;
    }

    if (!TT_OK(tt_rsa_encrypt_ntv(&rsa->sys_rsa,
                                  input,
                                  input_len,
                                  TT_BUF_WPOS(output),
                                  &output_len))) {
        return TT_FAIL;
    }
    TT_ASSERT(output_len <= rsa->block_size);
    tt_buf_inc_wp(output, output_len);

    return TT_SUCCESS;
}

tt_result_t tt_rsa_decrypt(IN tt_rsa_t *rsa,
                           IN tt_u8_t *input,
                           IN tt_u32_t input_len,
                           OUT tt_u8_t *output,
                           IN OUT tt_u32_t *output_len)
{
    TT_ASSERT(rsa != NULL);
    TT_ASSERT(input != NULL);
    TT_ASSERT(output != NULL);
    TT_ASSERT(output_len != NULL);

    if (input_len == 0) {
        *output_len = 0;
        return TT_SUCCESS;
    }

    if (input_len != rsa->block_size) {
        TT_ERROR("input[%d] should be rsa block size[%d]",
                 input_len,
                 rsa->block_size);
        return TT_FAIL;
    }

    if (*output_len < rsa->block_size) {
        TT_ERROR("output[%d] is less than rsa block size[%d]",
                 *output_len,
                 rsa->block_size);
        return TT_FAIL;
    }

    return tt_rsa_decrypt_ntv(&rsa->sys_rsa,
                              input,
                              input_len,
                              output,
                              output_len);
}

tt_result_t tt_rsa_decrypt_buf(IN tt_rsa_t *rsa,
                               IN tt_u8_t *input,
                               IN tt_u32_t input_len,
                               OUT tt_buf_t *output)
{
    tt_u32_t output_len;

    TT_ASSERT(rsa != NULL);
    TT_ASSERT(input != NULL);
    TT_ASSERT(output != NULL);

    if (input_len == 0) {
        return TT_SUCCESS;
    }

    if (input_len != rsa->block_size) {
        TT_ERROR("input[%d] should be rsa block size[%d]",
                 input_len,
                 rsa->block_size);
        return TT_FAIL;
    }

    output_len = rsa->block_size;
    if (!TT_OK(tt_buf_reserve(output, output_len))) {
        TT_ERROR("fail to reserve decrypt output space");
        return TT_FAIL;
    }

    if (!TT_OK(tt_rsa_decrypt_ntv(&rsa->sys_rsa,
                                  input,
                                  input_len,
                                  TT_BUF_WPOS(output),
                                  &output_len))) {
        return TT_FAIL;
    }
    TT_ASSERT(output_len <= rsa->block_size);
    tt_buf_inc_wp(output, output_len);

    return TT_SUCCESS;
}

tt_result_t tt_rsa_sign(IN tt_rsa_t *rsa,
                        IN tt_u8_t *input,
                        IN tt_u32_t input_len,
                        OUT tt_u8_t *signature,
                        IN OUT tt_u32_t *signature_len)
{
    TT_ASSERT(rsa != NULL);
    TT_ASSERT(input != NULL);
    TT_ASSERT(input_len != 0);
    TT_ASSERT(signature != NULL);
    TT_ASSERT(signature_len != NULL);

    if (*signature_len < rsa->block_size) {
        TT_ERROR("signature[%d] is less than rsa block size[%d]",
                 *signature_len,
                 rsa->block_size);
        return TT_FAIL;
    }

    return tt_rsa_sign_ntv(&rsa->sys_rsa,
                           input,
                           input_len,
                           signature,
                           signature_len);
}

tt_result_t tt_rsa_sign_buf(IN tt_rsa_t *rsa,
                            IN tt_u8_t *input,
                            IN tt_u32_t input_len,
                            OUT tt_buf_t *signature)
{
    tt_u32_t signature_len;

    TT_ASSERT(rsa != NULL);
    TT_ASSERT(input != NULL);
    TT_ASSERT(input_len != 0);
    TT_ASSERT(signature != NULL);

    signature_len = rsa->block_size;
    if (!TT_OK(tt_buf_reserve(signature, signature_len))) {
        TT_ERROR("fail to reserve signature space");
        return TT_FAIL;
    }

    if (!TT_OK(tt_rsa_sign_ntv(&rsa->sys_rsa,
                               input,
                               input_len,
                               TT_BUF_WPOS(signature),
                               &signature_len))) {
        return TT_FAIL;
    }
    TT_ASSERT(signature_len <= rsa->block_size);
    tt_buf_inc_wp(signature, signature_len);

    return TT_SUCCESS;
}

tt_result_t tt_rsa_verify(IN tt_rsa_t *rsa,
                          IN tt_u8_t *input,
                          IN tt_u32_t input_len,
                          IN tt_u8_t *signature,
                          IN tt_u32_t signature_len)
{
    TT_ASSERT(rsa != NULL);
    TT_ASSERT(input != NULL);
    TT_ASSERT(input_len != 0);
    TT_ASSERT(signature != NULL);
    TT_ASSERT(signature_len != 0);

    if (signature_len != rsa->block_size) {
        TT_ERROR("signature[%d] should be rsa block size[%d]",
                 input_len,
                 rsa->block_size);
        return TT_FAIL;
    }

    return tt_rsa_verify_ntv(&rsa->sys_rsa,
                             input,
                             input_len,
                             signature,
                             signature_len);
}

tt_result_t tt_rsa_show(IN tt_rsa_t *rsa)
{
    TT_ASSERT(rsa != NULL);

    TT_INFO("rsa key:");

    if (rsa->attr.padding.mode == TT_RSA_PADDING_NONE) {
        TT_INFO("  padding: none");
    } else if (rsa->attr.padding.mode == TT_RSA_PADDING_OAEP) {
        TT_INFO("  padding: oaep");
    } else {
        TT_INFO("  padding: unknown(error)");
    }

    return tt_rsa_show_ntv(&rsa->sys_rsa);
}

tt_result_t tt_rsa_get_number(IN tt_rsa_t *rsa, IN tt_rsa_number_t *rsa_number)
{
    TT_ASSERT(rsa != NULL);
    TT_ASSERT(rsa_number != NULL);

    return tt_rsa_get_number_ntv(&rsa->sys_rsa, rsa_number);
}

void tt_rsa_number_destroy(IN tt_rsa_number_t *rsa_number)
{
    TT_ASSERT(rsa_number != NULL);

    if (rsa_number->type == TT_RSA_TYPE_PUBLIC) {
        tt_blob_destroy(&rsa_number->pubnum.modulus);
        tt_blob_destroy(&rsa_number->pubnum.pub_exp);
    } else {
        TT_ASSERT(rsa_number->type == TT_RSA_TYPE_PRIVATE);

        tt_blob_destroy(&rsa_number->privnum.modulus);
        tt_blob_destroy(&rsa_number->privnum.pub_exp);
        tt_blob_destroy(&rsa_number->privnum.priv_exp);
        tt_blob_destroy(&rsa_number->privnum.prime1);
        tt_blob_destroy(&rsa_number->privnum.prime2);
        tt_blob_destroy(&rsa_number->privnum.exp1);
        tt_blob_destroy(&rsa_number->privnum.exp2);
        tt_blob_destroy(&rsa_number->privnum.coefficient);
    }
}

tt_u32_t __rsa_block_size(IN tt_rsa_t *rsa)
{
    TT_ASSERT(TT_RSA_SIZE_VALID(rsa->size));
    switch (rsa->size) {
        case TT_RSA_SIZE_1024BIT: {
            return 128;
        } break;
        case TT_RSA_SIZE_2048BIT: {
            return 256;
        } break;

        default: {
            // never reach here
            return 0;
        } break;
    }
}

tt_u32_t __rsa_min_padding_size(IN tt_rsa_t *rsa)
{
    TT_ASSERT(TT_RSA_PADDING_VALID(rsa->attr.padding.mode));
    switch (rsa->attr.padding.mode) {
        case TT_RSA_PADDING_NONE: {
            return 0;
        } break;
        case TT_RSA_PADDING_OAEP: {
            // 2hLen + 2
            // as we always use sha1, so hLen is 20
            return 42;
        } break;
        case TT_RSA_PADDING_PKCS1: {
            // EM = 0x00 || 0x02 || PS || 0x00 || M
            // The length of PS will be at least eight octets
            return 11;
        } break;

        default: {
            // never reach here
            TT_ERROR("unknown rsa padding: %d", rsa->attr.padding.mode);
            return 0;
        } break;
    }
}

tt_result_t __rsa_pkcs8_decrypt(IN tt_blob_t *encrypted,
                                IN tt_rsa_attr_t *rsa_attr,
                                OUT tt_u8_t *decrypted,
                                IN OUT tt_u32_t *decrypted_len)
{
    tt_blob_t password;
    tt_result_t result;

    password.addr = (tt_u8_t *)rsa_attr->password;
    password.len = (tt_u32_t)tt_strlen(rsa_attr->password);

    if (rsa_attr->pem_armor) {
        tt_blob_t der_data;
        tt_u8_t *der_buf;
        tt_u32_t der_len, der_data_len;
        tt_base64_decode_attr_t attr;

        der_len = encrypted->len;
        der_buf = (tt_u8_t *)tt_malloc(der_len);
        if (der_buf == NULL) {
            TT_ERROR("no mem for base64 decoding");
            return TT_FAIL;
        }

        tt_base64_decode_attr_default(&attr);
        attr.start_boundary = "-----\n"; // to use "\r\n" ??
        attr.end_boundary = "\n-----";

        der_data_len = der_len;
        result = tt_base64_decode(encrypted->addr,
                                  encrypted->len,
                                  &attr,
                                  der_buf,
                                  &der_data_len);
        if (!TT_OK(result)) {
            TT_ERROR("rsa pkcs8 base64 decoding fail");

            tt_free(der_buf);
            return TT_FAIL;
        }
        // tt_hex_dump(der_buf, der_data_len, 16);

        der_data.addr = der_buf;
        der_data.len = der_data_len;
        result =
            tt_pkcs8_decrypt(&der_data, &password, decrypted, decrypted_len);
        tt_free(der_buf);

        return result;
    } else {
        return tt_pkcs8_decrypt(encrypted, &password, decrypted, decrypted_len);
    }
}

tt_result_t __rsa_pub_der(IN tt_buf_t *buf, IN tt_rsa_pubnum_t *num)
{
    tt_u32_t key_datalen, key_len;

    /*
     pkcs1:

     RSAPublicKey ::= SEQUENCE {
        modulus           INTEGER,  -- n
        publicExponent    INTEGER   -- e
     }
     */

    // ========================================
    // calc len
    // ========================================

    key_datalen = tt_der_integer_len(num->modulus.addr, num->modulus.len);
    key_datalen += tt_der_integer_len(num->pub_exp.addr, num->pub_exp.len);
    key_len = tt_der_sequence_len(key_datalen);

    // ========================================
    // encode
    // ========================================

    TT_DO(tt_buf_reserve(buf, key_len));

    TT_DO(tt_der_encode_sequence(buf, key_datalen, 0));
    TT_DO(tt_der_encode_integer(buf, num->modulus.addr, num->modulus.len, 0));
    TT_DO(tt_der_encode_integer(buf, num->pub_exp.addr, num->pub_exp.len, 0));

    return TT_SUCCESS;
}

tt_result_t __rsa_pub_der_keyinfo(IN tt_buf_t *buf, IN tt_rsa_pubnum_t *num)
{
    tt_u32_t alg_datalen, alg_len;
    tt_u32_t key_datalen, key_len;
    tt_u32_t info_datalen, info_len;

    /*
     PublicKeyInfo ::= SEQUENCE {
        algorithm       AlgorithmIdentifier,
        PublicKey       BIT STRING
     }

     AlgorithmIdentifier ::= SEQUENCE {
        algorithm       OBJECT IDENTIFIER,
        parameters      ANY DEFINED BY algorithm OPTIONAL
     }

     RSAPublicKey ::= SEQUENCE {
        modulus           INTEGER,  -- n
        publicExponent    INTEGER   -- e
     }
     */

    // ========================================
    // calc len
    // ========================================

    // alg
    alg_datalen = tt_der_oid_len(__oid_rsa_enc, sizeof(__oid_rsa_enc));
    alg_datalen += tt_der_null_len();
    alg_len = tt_der_sequence_len(alg_datalen);

    // key
    key_datalen = tt_der_integer_len(num->modulus.addr, num->modulus.len);
    key_datalen += tt_der_integer_len(num->pub_exp.addr, num->pub_exp.len);
    key_len = tt_der_sequence_len(key_datalen);

    // keyinfo
    info_datalen = alg_len + tt_der_bitstr_len(NULL, key_len, 0);
    info_len = tt_der_sequence_len(info_datalen);

    // ========================================
    // encode
    // ========================================

    TT_DO(tt_buf_reserve(buf, info_len));

    // keyinfo
    TT_DO(tt_der_encode_sequence(buf, info_datalen, 0));

    // alg
    TT_DO(tt_der_encode_sequence(buf, alg_datalen, 0));
    TT_DO(tt_der_encode_oid(buf, __oid_rsa_enc, sizeof(__oid_rsa_enc), 0));
    TT_DO(tt_der_encode_null(buf, 0));

    // key
    TT_DO(tt_der_encode_bitstr(buf, NULL, key_len, 0, 0));
    TT_DO(tt_der_encode_sequence(buf, key_datalen, 0));
    TT_DO(tt_der_encode_integer(buf, num->modulus.addr, num->modulus.len, 0));
    TT_DO(tt_der_encode_integer(buf, num->pub_exp.addr, num->pub_exp.len, 0));

    return TT_SUCCESS;
}

tt_result_t __rsa_priv_der(IN tt_buf_t *buf, IN tt_rsa_privnum_t *num)
{
    tt_u32_t key_datalen, key_len;

    /*
     Version ::= INTEGER { two-prime(0), multi(1) }

     RSAPrivateKey ::= SEQUENCE {
        version           Version,
        modulus           INTEGER,  -- n
        publicExponent    INTEGER,  -- e
        privateExponent   INTEGER,  -- d
        prime1            INTEGER,  -- p
        prime2            INTEGER,  -- q
        exponent1         INTEGER,  -- d mod (p-1)
        exponent2         INTEGER,  -- d mod (q-1)
        coefficient       INTEGER,  -- (inverse of q) mod p
        otherPrimeInfos   OtherPrimeInfos OPTIONAL
     }
     */

    // use version 0
    key_datalen = tt_der_s32_len(0);
    key_datalen += tt_der_integer_len(num->modulus.addr, num->modulus.len);
    key_datalen += tt_der_integer_len(num->pub_exp.addr, num->pub_exp.len);
    key_datalen += tt_der_integer_len(num->priv_exp.addr, num->priv_exp.len);
    key_datalen += tt_der_integer_len(num->prime1.addr, num->prime1.len);
    key_datalen += tt_der_integer_len(num->prime2.addr, num->prime2.len);
    key_datalen += tt_der_integer_len(num->exp1.addr, num->exp1.len);
    key_datalen += tt_der_integer_len(num->exp2.addr, num->exp2.len);
    key_datalen +=
        tt_der_integer_len(num->coefficient.addr, num->coefficient.len);
    key_len = tt_der_sequence_len(key_datalen);

    TT_DO(tt_buf_reserve(buf, key_len));

    TT_DO(tt_der_encode_sequence(buf, key_datalen, 0));
    TT_DO(tt_der_encode_s32(buf, 0, 0));
    TT_DO(tt_der_encode_integer(buf, num->modulus.addr, num->modulus.len, 0));
    TT_DO(tt_der_encode_integer(buf, num->pub_exp.addr, num->pub_exp.len, 0));
    TT_DO(tt_der_encode_integer(buf, num->priv_exp.addr, num->priv_exp.len, 0));
    TT_DO(tt_der_encode_integer(buf, num->prime1.addr, num->prime1.len, 0));
    TT_DO(tt_der_encode_integer(buf, num->prime2.addr, num->prime2.len, 0));
    TT_DO(tt_der_encode_integer(buf, num->exp1.addr, num->exp1.len, 0));
    TT_DO(tt_der_encode_integer(buf, num->exp2.addr, num->exp2.len, 0));
    TT_DO(tt_der_encode_integer(buf,
                                num->coefficient.addr,
                                num->coefficient.len,
                                0));

    return TT_SUCCESS;
}
