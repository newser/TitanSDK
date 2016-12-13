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

#include <crypto/tt_pkcs8.h>

#include <algorithm/tt_buffer.h>
#include <crypto/tt_aes.h>
#include <crypto/tt_aes_def.h>
#include <crypto/tt_pkcs5.h>
#include <memory/tt_memory_alloc.h>
#include <misc/tt_asn1_def.h>
#include <misc/tt_der_decode.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef struct
{
    tt_blob_t salt;
    tt_u32_t iter_count;
} __pbkdf2_param_t;

typedef struct
{
    tt_blob_t aes_iv;
} __pbes2_aes_param_t;

typedef struct
{
    // key derivation
    tt_pbkdf_ver_t ver;
    union
    {
        __pbkdf2_param_t pbkdf2;
    };

    // encryption scheme
    tt_u32_t enc_scheme;
#define __PBES2_ENC_AES_128_CBC 1
#define __PBES2_ENC_AES_256_CBC 2
    union
    {
        __pbes2_aes_param_t aes;
    };
} __pbes2_param_t;

typedef struct
{
    tt_pbes_ver_t ver;
    union
    {
        __pbes2_param_t pbes2;
    };
} __pbes_param_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

// PBES2 OID: "1.2.840.113549.1.5.13"
static tt_u8_t __oid_pbes2[] =
    {0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x05, 0x0d};
// PBDK2 OID: "1.2.840.113549.1.5.12"
static tt_u8_t __oid_pbkdf2[] =
    {0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x05, 0x0c};

// aes-128-cbc OID: "2.16.840.1.101.3.4.1.2"
static tt_u8_t __oid_aes_128_cbc[] =
    {0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x01, 0x02};
// aes-256-cbc OID: "2.16.840.1.101.3.4.1.42"
static tt_u8_t __oid_aes_256_cbc[] =
    {0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x01, 0x2a};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __decode_encryptionAlgorithm(IN tt_buf_t *buf,
                                                OUT __pbes_param_t *pbes_param);
static tt_result_t __decode_PBES2_params(IN tt_buf_t *buf,
                                         OUT __pbes2_param_t *pbes2_param);
static tt_result_t __decode_keyDerivationFunc(IN tt_buf_t *buf,
                                              OUT __pbes2_param_t *pbes2_param);
static tt_result_t __decode_PBKDF2_params(IN tt_buf_t *buf,
                                          OUT __pbkdf2_param_t *pbkdf2_param);
static tt_result_t __decode_encryptionScheme(IN tt_buf_t *buf,
                                             OUT __pbes2_param_t *pbes2_param);
static tt_result_t __decode_aes_param(IN tt_buf_t *buf,
                                      OUT __pbes2_aes_param_t *aes_param);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_pkcs8_decrypt(IN tt_blob_t *ciphertext,
                             IN tt_blob_t *password,
                             OUT tt_u8_t *plaintext,
                             IN OUT tt_u32_t *plaintext_len)
{
    tt_buf_t buf;
    tt_result_t result;

    // key derivation
    __pbes_param_t pbes_param;
    tt_blob_t enc_data;
    tt_u8_t derived_key_buf[TT_AES_BLOCK_SIZE] = {0};

    // aes decryption
    tt_blob_t aes_key = {derived_key_buf, TT_AES_BLOCK_SIZE};
    tt_aes_size_t aes_size;
    tt_aes_attr_t aes_attr;

    TT_ASSERT(ciphertext != NULL);
    TT_ASSERT(password != NULL);

    if (ciphertext->len == 0) {
        *plaintext_len = 0;
        return TT_SUCCESS;
    }

    /*
     example by openssl asn1parse -i -in <pkcs8 encrypted file>:

     SEQUENCE
      SEQUENCE
       OBJECT            :PBES2
       SEQUENCE
        SEQUENCE
         OBJECT            :PBKDF2
         SEQUENCE
          OCTET STRING            [HEX DUMP]:744F2084F69E4673
          INTEGER                 :0800
        SEQUENCE
         OBJECT                  :aes-128-cbc
         OCTET STRING            [HEX DUMP]:0B149E5C1E420C8058B5C8D359C0C35B
      OCTET STRING            [HEX DUMP]:encrypted data
     */

    result =
        tt_buf_create_nocopy(&buf, ciphertext->addr, ciphertext->len, NULL);
    if (!TT_OK(result)) {
        return TT_FAIL;
    }

    /*
     EncryptedPrivateKeyInfo ::= SEQUENCE
     {
        encryptionAlgorithm  EncryptionAlgorithmIdentifier,
        encryptedData        EncryptedData
     }
     */
    TT_DO_G(p8_fail, tt_der_decode_sequence(&buf, NULL, 0));
    TT_DO_G(p8_fail, __decode_encryptionAlgorithm(&buf, &pbes_param));
    TT_DO_G(p8_fail,
            tt_der_decode_octstr(&buf, &enc_data.addr, &enc_data.len, 0));
    if (pbes_param.ver != TT_PBES_VER_2) {
        TT_ERROR("now only support PBES2");
        goto p8_fail;
    }

    // check aes parameters
    if (pbes_param.pbes2.enc_scheme == __PBES2_ENC_AES_128_CBC) {
        aes_size = TT_AES_SIZE_128;
    } else if (pbes_param.pbes2.enc_scheme == __PBES2_ENC_AES_256_CBC) {
        aes_size = TT_AES_SIZE_256;
    } else {
        TT_ERROR("now only support AES");
        goto p8_fail;
    }

    if (pbes_param.pbes2.aes.aes_iv.len > TT_AES_BLOCK_SIZE) {
        TT_ERROR("too long AES IV[%d]", pbes_param.pbes2.aes.aes_iv.len);
        goto p8_fail;
    }
    aes_attr.padding = TT_AES_PADDING_PKCS7;
    aes_attr.mode = TT_AES_MODE_CBC;
    aes_attr.cbc.ivec.addr = pbes_param.pbes2.aes.aes_iv.addr;
    aes_attr.cbc.ivec.len = pbes_param.pbes2.aes.aes_iv.len;

    // any param decribing padding scheme? currently it's assumed pkcs7
    // padding scheme...

    // key derivation
    result = tt_pbkdf2(TT_PKCS5_PRF_HMAC_SHA1,
                       password,
                       &pbes_param.pbes2.pbkdf2.salt,
                       pbes_param.pbes2.pbkdf2.iter_count,
                       sizeof(derived_key_buf),
                       derived_key_buf);
    if (!TT_OK(result)) {
        goto p8_fail;
    }

    // aes decryption
    result = tt_aes(TT_FALSE,
                    &aes_key,
                    aes_size,
                    &aes_attr,
                    &enc_data,
                    plaintext,
                    plaintext_len);
    if (!TT_OK(result)) {
        goto p8_fail;
    }

    tt_buf_destroy(&buf);

    return TT_SUCCESS;

p8_fail:

    tt_buf_destroy(&buf);

    return TT_FAIL;
}

tt_result_t __decode_encryptionAlgorithm(IN tt_buf_t *buf,
                                         OUT __pbes_param_t *pbes_param)
{
    tt_u8_t *oid;
    tt_u32_t oid_len;

    /*
     AlgorithmIdentifier { ALGORITHM-IDENTIFIER:InfoObjectSet } ::= SEQUENCE
     {
        algorithm ALGORITHM-IDENTIFIER.&id({InfoObjectSet}),
        parameters ALGORITHM-IDENTIFIER.&Type({InfoObjectSet}{@algorithm})
     OPTIONAL
     }
     */

    tt_memset(pbes_param, 0, sizeof(__pbes_param_t));

    TT_DO(tt_der_decode_sequence(buf, NULL, 0));

    // only support pbes2
    TT_DO(tt_der_decode_oid(buf, &oid, &oid_len, TT_DER_PASS_CONTENT));
    if ((oid_len != sizeof(__oid_pbes2)) ||
        (tt_memcmp(oid, __oid_pbes2, oid_len) != 0)) {
        TT_ERROR("only support PBES2 now");
        return TT_FAIL;
    }
    pbes_param->ver = TT_PBES_VER_2;

    // decode pbes2 params
    TT_DO(__decode_PBES2_params(buf, &pbes_param->pbes2));

    return TT_SUCCESS;
}

tt_result_t __decode_PBES2_params(IN tt_buf_t *buf,
                                  OUT __pbes2_param_t *pbes2_param)
{
    /*
     PBES2-params ::=  SEQUENCE
     {
        keyDerivationFunc AlgorithmIdentifier {{PBES2-KDFs}},
        encryptionScheme AlgorithmIdentifier {{PBES2-Encs}}
     }
     */

    TT_DO(tt_der_decode_sequence(buf, NULL, 0));
    TT_DO(__decode_keyDerivationFunc(buf, pbes2_param));
    TT_DO(__decode_encryptionScheme(buf, pbes2_param));

    return TT_SUCCESS;
}

tt_result_t __decode_keyDerivationFunc(IN tt_buf_t *buf,
                                       OUT __pbes2_param_t *pbes2_param)
{
    tt_u8_t *oid;
    tt_u32_t oid_len;

    /*
     AlgorithmIdentifier { ALGORITHM-IDENTIFIER:InfoObjectSet } ::= SEQUENCE
     {
        algorithm ALGORITHM-IDENTIFIER.&id({InfoObjectSet}),
        parameters ALGORITHM-IDENTIFIER.&Type({InfoObjectSet}{@algorithm})
     OPTIONAL
     }
     */

    TT_DO(tt_der_decode_sequence(buf, NULL, 0));

    // only support pbkdf2
    TT_DO(tt_der_decode_oid(buf, &oid, &oid_len, TT_DER_PASS_CONTENT));
    if ((oid_len != sizeof(__oid_pbkdf2)) ||
        (tt_memcmp(oid, __oid_pbkdf2, oid_len) != 0)) {
        TT_ERROR("only support PBKDF2 now");
        return TT_FAIL;
    }
    pbes2_param->ver = TT_PBKDF_VER_2;

    // decode pbkdf2 params
    TT_DO(__decode_PBKDF2_params(buf, &pbes2_param->pbkdf2));

    return TT_SUCCESS;
}

tt_result_t __decode_PBKDF2_params(IN tt_buf_t *buf,
                                   OUT __pbkdf2_param_t *pbkdf2_param)
{
    tt_u32_t seq_length;
    tt_u32_t rd_pos, wr_pos;

    tt_s32_t iter_count;

    /*
     PBKDF2-params ::= SEQUENCE
     {
        salt CHOICE
        {
            specified OCTET STRING,
            otherSource AlgorithmIdentifier {{PBKDF2-SaltSources}}
        },
        iterationCount INTEGER (1..MAX),
        keyLength INTEGER (1..MAX) OPTIONAL,
        prf AlgorithmIdentifier {{PBKDF2-PRFs}} DEFAULT algid-hmacWithSHA1
     }
     */

    TT_DO(tt_der_decode_sequence(buf, &seq_length, 0));
    tt_buf_backup_rwp(buf, &rd_pos, &wr_pos);

    // salt, only support octet string
    TT_DO(tt_der_decode_octstr(buf,
                               &pbkdf2_param->salt.addr,
                               &pbkdf2_param->salt.len,
                               TT_DER_PASS_CONTENT));

    // iterationCount
    TT_DO(tt_der_decode_s32(buf, NULL, &iter_count, TT_DER_PASS_CONTENT));
    if (iter_count <= 0) {
        TT_ERROR("invalid iter_count[%d]", iter_count);
        return TT_FAIL;
    }
    pbkdf2_param->iter_count = (tt_u32_t)iter_count;

    // ignore later content
    tt_buf_restore_rwp(buf, &rd_pos, &wr_pos);
    TT_DO(tt_buf_inc_rp(buf, seq_length));

    return TT_SUCCESS;
}

tt_result_t __decode_encryptionScheme(IN tt_buf_t *buf,
                                      OUT __pbes2_param_t *pbes2_param)
{
    tt_u32_t seq_length, oid_len;
    tt_u8_t *oid;

    tt_u32_t rd_pos, wr_pos;

    /*
     for AES:

     -- AES using CBC-chaining mode for key sizes of 128, 192, 256

     id-aes128-CBC OBJECT IDENTIFIER ::= { aes 2 }
     id-aes192-CBC OBJECT IDENTIFIER ::= { aes 22 }
     id-aes256-CBC OBJECT IDENTIFIER ::= { aes 42 }

     -- AES-IV is a the parameter for all the above object identifiers.

     AES-IV ::= OCTET STRING (SIZE(16))
     */

    TT_DO(tt_der_decode_sequence(buf, &seq_length, 0));
    tt_buf_backup_rwp(buf, &rd_pos, &wr_pos);

    // only support aes-128-cbc/aes-256-cbc
    TT_DO(tt_der_decode_oid(buf, &oid, &oid_len, TT_DER_PASS_CONTENT));
    if ((oid_len == sizeof(__oid_aes_128_cbc)) &&
        (tt_memcmp(oid, __oid_aes_128_cbc, oid_len) == 0)) {
        pbes2_param->enc_scheme = __PBES2_ENC_AES_128_CBC;
        TT_DO(__decode_aes_param(buf, &pbes2_param->aes));
    } else if ((oid_len == sizeof(__oid_aes_256_cbc)) &&
               (tt_memcmp(oid, __oid_aes_256_cbc, oid_len) == 0)) {
        pbes2_param->enc_scheme = __PBES2_ENC_AES_256_CBC;
        TT_DO(__decode_aes_param(buf, &pbes2_param->aes));
    } else {
        TT_ERROR("only support AES CBC now");
        return TT_FAIL;
    }

    tt_buf_restore_rwp(buf, &rd_pos, &wr_pos);
    TT_DO(tt_buf_inc_rp(buf, seq_length));

    return TT_SUCCESS;
}

tt_result_t __decode_aes_param(IN tt_buf_t *buf,
                               OUT __pbes2_aes_param_t *aes_param)
{
    TT_DO(tt_der_decode_octstr(buf,
                               &aes_param->aes_iv.addr,
                               &aes_param->aes_iv.len,
                               TT_DER_PASS_CONTENT));

    return TT_SUCCESS;
}
