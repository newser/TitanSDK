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

#include <tt_platform.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef struct
{
    const tt_char_t *input;
    const tt_char_t *output;
} __c_test_vect_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

// === routine declarations ================
TT_TEST_ROUTINE_DECLARE(tt_unit_test_rsa_1024)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_rsa_1024_oaep)

TT_TEST_ROUTINE_DECLARE(tt_unit_test_rsa_2048_enc_pkcs8)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_rsa_2048_padpkcs1)

TT_TEST_ROUTINE_DECLARE(tt_unit_test_rsa_1024_num)

TT_TEST_ROUTINE_DECLARE(tt_unit_test_rsa_2048_sign_pkcs1)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(rsa_case)

TT_TEST_CASE("tt_unit_test_rsa_1024",
             "crypto: RSA",
             tt_unit_test_rsa_1024,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
, TT_TEST_CASE("tt_unit_test_rsa_1024_oaep",
               "crypto: RSA OAEP",
               tt_unit_test_rsa_1024_oaep,
               NULL,
               NULL,
               NULL,
               NULL,
               NULL),

    TT_TEST_CASE("tt_unit_test_rsa_2048_passphrase",
                 "crypto: RSA2048, pkcs8, with passphrase",
                 tt_unit_test_rsa_2048_enc_pkcs8,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_rsa_2048_padpkcs1",
                 "crypto: RSA2048, enc/dec pkcs1 padding",
                 tt_unit_test_rsa_2048_padpkcs1,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_rsa_1024_num",
                 "crypto: RSA",
                 tt_unit_test_rsa_1024_num,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_rsa_2048_sign_pkcs1",
                 "crypto: RSA2048, sign & verify",
                 tt_unit_test_rsa_2048_sign_pkcs1,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(rsa_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(CRYPTO_UT_RSA, 0, rsa_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(name)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    static const tt_char_t *__rsa_priv_1024 =
        "-----BEGIN RSA PRIVATE KEY-----\n"
        "MIICXQIBAAKBgQC8sWYASJLH2pIyP5bkttP2IwPL/q6aGfSA7jNdbLViAB+vxueb\n"
        "fN4te1uOklOYAGd5qqhgYP1iCFRyHp02xH/V7/yN9U40/kE7mldtVMvNPQGiajFD\n"
        "D0fhLPhCPyCe8MHnMCmvP3jR3VHHRcmrCviFMLzpxdz58pirootiojH5iQIDAQAB\n"
        "AoGATYcZ4L3wyPqOHOUxq1FPL49dbQB8XsaRRjQYmKcks1pXqpIv8RvR8aAAk0id\n"
        "3hM0cLHksozHLvM7NxRLBgigeOJUq9aODsEy9Mm2V8w/zLFjjtPX+9sXo+eUbe35\n"
        "uvWsxwsyzcyrD6xjej1EBH6jNgzHAgKfMvhYim2x+FsGJwECQQDozWdfe0Oj3uwo\n"
        "C6Hduyim4qPKBCJ9Mym8GltnYrgeEiyGkC9ywnNGY6IOhf6zcC4Qdd1P5zffboo9\n"
        "ArYBAxTpAkEAz37M4f8epwFT4UWvss/e+k4vrc+2ZMZDZ1GDht4OOfcrN+K2zcJu\n"
        "TPjwh4zS4yhs4O48DDqEmKGP1t/wuFtboQJBAN6g95du2KuQloRhQ2U5A1I88OdS\n"
        "9uyGDgZNqlV/KZXQSXZR0p5LaFPLGgILkBxnThX9sSwlcOAh5N8dhZIIMWkCQQDE\n"
        "AW/g/HIkKGjoID/8lch4gmsfsVsGjxMVAjdQq2qc7xNwU0WD0OsI4w+zvB9qXcGc\n"
        "KO+oTY7774JE5H4saICBAkAPQ3nH677x3KAMUsSnfk1BKMyjcfXM4CnFVexyZpXU\n"
        "xvlkfrLnC6dnYkatKVcE31ZkHWNI+7rBhvQ3d+LePLHd\n"
        "-----END RSA PRIVATE KEY-----\n";

static const tt_char_t *__rsa_pub_1024_wrapped =
    "-----BEGIN PUBLIC KEY-----\n"
    "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQC8sWYASJLH2pIyP5bkttP2IwPL\n"
    "/q6aGfSA7jNdbLViAB+vxuebfN4te1uOklOYAGd5qqhgYP1iCFRyHp02xH/V7/yN\n"
    "9U40/kE7mldtVMvNPQGiajFDD0fhLPhCPyCe8MHnMCmvP3jR3VHHRcmrCviFMLzp\n"
    "xdz58pirootiojH5iQIDAQAB\n"
    "-----END PUBLIC KEY-----\n";
static const tt_u8_t __rsa_pub_1024_bsafe[] =
    {0x30, 0x81, 0x89, 0x02, 0x81, 0x81, 0x00, 0xbc, 0xb1, 0x66, 0x00, 0x48,
     0x92, 0xc7, 0xda, 0x92, 0x32, 0x3f, 0x96, 0xe4, 0xb6, 0xd3, 0xf6, 0x23,
     0x03, 0xcb, 0xfe, 0xae, 0x9a, 0x19, 0xf4, 0x80, 0xee, 0x33, 0x5d, 0x6c,
     0xb5, 0x62, 0x00, 0x1f, 0xaf, 0xc6, 0xe7, 0x9b, 0x7c, 0xde, 0x2d, 0x7b,
     0x5b, 0x8e, 0x92, 0x53, 0x98, 0x00, 0x67, 0x79, 0xaa, 0xa8, 0x60, 0x60,
     0xfd, 0x62, 0x08, 0x54, 0x72, 0x1e, 0x9d, 0x36, 0xc4, 0x7f, 0xd5, 0xef,
     0xfc, 0x8d, 0xf5, 0x4e, 0x34, 0xfe, 0x41, 0x3b, 0x9a, 0x57, 0x6d, 0x54,
     0xcb, 0xcd, 0x3d, 0x01, 0xa2, 0x6a, 0x31, 0x43, 0x0f, 0x47, 0xe1, 0x2c,
     0xf8, 0x42, 0x3f, 0x20, 0x9e, 0xf0, 0xc1, 0xe7, 0x30, 0x29, 0xaf, 0x3f,
     0x78, 0xd1, 0xdd, 0x51, 0xc7, 0x45, 0xc9, 0xab, 0x0a, 0xf8, 0x85, 0x30,
     0xbc, 0xe9, 0xc5, 0xdc, 0xf9, 0xf2, 0x98, 0xab, 0xa2, 0x8b, 0x62, 0xa2,
     0x31, 0xf9, 0x89, 0x02, 0x03, 0x01, 0x00, 0x01};

static tt_u8_t __rsa_pub_1024_wrapped_mod[] = {
    0xbc, 0xb1, 0x66, 0x00, 0x48, 0x92, 0xc7, 0xda, 0x92, 0x32, 0x3f, 0x96,
    0xe4, 0xb6, 0xd3, 0xf6, 0x23, 0x03, 0xcb, 0xfe, 0xae, 0x9a, 0x19, 0xf4,
    0x80, 0xee, 0x33, 0x5d, 0x6c, 0xb5, 0x62, 0x00, 0x1f, 0xaf, 0xc6, 0xe7,
    0x9b, 0x7c, 0xde, 0x2d, 0x7b, 0x5b, 0x8e, 0x92, 0x53, 0x98, 0x00, 0x67,
    0x79, 0xaa, 0xa8, 0x60, 0x60, 0xfd, 0x62, 0x08, 0x54, 0x72, 0x1e, 0x9d,
    0x36, 0xc4, 0x7f, 0xd5, 0xef, 0xfc, 0x8d, 0xf5, 0x4e, 0x34, 0xfe, 0x41,
    0x3b, 0x9a, 0x57, 0x6d, 0x54, 0xcb, 0xcd, 0x3d, 0x01, 0xa2, 0x6a, 0x31,
    0x43, 0x0f, 0x47, 0xe1, 0x2c, 0xf8, 0x42, 0x3f, 0x20, 0x9e, 0xf0, 0xc1,
    0xe7, 0x30, 0x29, 0xaf, 0x3f, 0x78, 0xd1, 0xdd, 0x51, 0xc7, 0x45, 0xc9,
    0xab, 0x0a, 0xf8, 0x85, 0x30, 0xbc, 0xe9, 0xc5, 0xdc, 0xf9, 0xf2, 0x98,
    0xab, 0xa2, 0x8b, 0x62, 0xa2, 0x31, 0xf9, 0x89,
};

static tt_u8_t __rsa_pub_1024_wrapped_exp[] = {
    0x01, 0x00, 0x01,
};

static tt_char_t __rsa_1024_in[128 + 128 + 128] =
    "0123456789abcdef"
    "0123456789abcdef"
    "0123456789abcdef"
    "0123456789abcdef"
    "0123456789abcdef"
    "0123456789abcdef"
    "0123456789abcdef"
    "0123456789abcdef"
    "0123456789abcdef"
    "0123456789abcdef"
    "0123456789abcdef"
    "0123456789abcdef"
    "0123456789abcdef"
    "0123456789abcdef"
    "0123456789abcdef"
    "0123456789abcdef";

static tt_char_t __rsa_1024_out[128] = {
    0xb3, 0x97, 0x2e, 0x0a, 0xda, 0xf2, 0xd5, 0x23, 0xac, 0x7a, 0x1c, 0x22,
    0xce, 0xd4, 0x05, 0x16, 0xbd, 0x93, 0xfa, 0x86, 0xd3, 0xed, 0x07, 0xa7,
    0x61, 0x94, 0x75, 0x2c, 0xc8, 0x7c, 0xb4, 0x9d, 0x7f, 0xec, 0x4e, 0xf0,
    0xa3, 0x6c, 0x1c, 0x48, 0xf2, 0xe5, 0xf9, 0xcc, 0xfc, 0xd9, 0x51, 0xd1,
    0xff, 0x38, 0x68, 0x91, 0xe5, 0xf5, 0xef, 0x49, 0x67, 0x97, 0xca, 0xf2,
    0x2c, 0x99, 0xf5, 0x74, 0xc5, 0x05, 0x81, 0x0b, 0x26, 0x58, 0x73, 0x4b,
    0x99, 0xd0, 0x89, 0x35, 0xcf, 0xce, 0xcd, 0x33, 0x56, 0xce, 0x5e, 0x9e,
    0xa5, 0x4a, 0x8a, 0xe0, 0x84, 0xb4, 0xc4, 0x4c, 0x18, 0xd4, 0xf9, 0x32,
    0xb7, 0x0f, 0xe4, 0x97, 0xb0, 0xec, 0x50, 0x84, 0xb2, 0x19, 0xed, 0x72,
    0x53, 0x26, 0xee, 0xa7, 0x79, 0x30, 0xcc, 0x00, 0xbc, 0xb4, 0xe3, 0x3b,
    0x90, 0xc9, 0x23, 0xaa, 0x77, 0xef, 0xcd, 0x6f,
};

TT_TEST_ROUTINE_DEFINE(tt_unit_test_rsa_1024)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_rsa_t pub, priv, key;
    tt_result_t ret;
    tt_rsa_number_t rn;

    tt_buf_t output, output2;
    tt_u8_t buf[128];
    tt_u8_t buf2[128];
    tt_u32_t n;

    tt_blob_t key_data;

    TT_TEST_CASE_ENTER()
    // test start

    // rsa 1024 encrypt
    key_data.addr = (tt_u8_t *)__rsa_pub_1024_wrapped;
    key_data.len = (tt_u32_t)strlen(__rsa_pub_1024_wrapped);
    ret = tt_rsa_create(&pub,
                        TT_RSA_FORMAT_PKCS1,
                        TT_RSA_TYPE_PUBLIC,
                        &key_data,
                        NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    tt_rsa_show(&pub);

    ret = tt_rsa_get_number(&pub, &rn);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    TT_UT_EQUAL(rn.pubnum.modulus.len, sizeof(__rsa_pub_1024_wrapped_mod), "");
    TT_UT_EQUAL(tt_memcmp(rn.pubnum.modulus.addr,
                          __rsa_pub_1024_wrapped_mod,
                          sizeof(__rsa_pub_1024_wrapped_mod)),
                0,
                "");

    TT_UT_EQUAL(rn.pubnum.pub_exp.len, sizeof(__rsa_pub_1024_wrapped_exp), "");
    TT_UT_EQUAL(tt_memcmp(rn.pubnum.pub_exp.addr,
                          __rsa_pub_1024_wrapped_exp,
                          sizeof(__rsa_pub_1024_wrapped_exp)),
                0,
                "");

    tt_buf_init(&output, NULL);

    // buf

    tt_buf_reset_rwp(&output);
    ret = tt_rsa_encrypt_buf(&pub, (tt_u8_t *)__rsa_1024_in, 0, &output);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(TT_BUF_RLEN(&output), 0, "");

    // size must be 128
    tt_buf_reset_rwp(&output);
    ret = tt_rsa_encrypt_buf(&pub, (tt_u8_t *)__rsa_1024_in, 127, &output);
    TT_UT_EQUAL(ret, TT_FAIL, "");

    ret = tt_rsa_encrypt_buf(&pub, (tt_u8_t *)__rsa_1024_in, 128, &output);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    TT_UT_EQUAL(TT_BUF_RLEN(&output), 128, "");
    TT_UT_EQUAL(tt_memcmp(__rsa_1024_out, TT_BUF_RPOS(&output), 128), 0, "");

    // rsa 1024 decrypt
    key_data.addr = (tt_u8_t *)__rsa_priv_1024;
    key_data.len = (tt_u32_t)strlen(__rsa_priv_1024);
    ret = tt_rsa_create(&priv,
                        TT_RSA_FORMAT_PKCS1,
                        TT_RSA_TYPE_PRIVATE,
                        &key_data,
                        NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    // tt_rsa_show(&priv);

    tt_buf_reset_rwp(&output);
    ret = tt_rsa_decrypt_buf(&priv, (tt_u8_t *)__rsa_1024_out, 0, &output);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(TT_BUF_RLEN(&output), 0, "");

    tt_buf_reset_rwp(&output);
    ret = tt_rsa_decrypt_buf(&priv, (tt_u8_t *)__rsa_1024_out, 128, &output);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    TT_UT_EQUAL(TT_BUF_RLEN(&output), 128, "");
    TT_UT_EQUAL(tt_memcmp(__rsa_1024_in, TT_BUF_RPOS(&output), 128), 0, "");

    tt_rsa_destroy(&pub);
    tt_rsa_destroy(&priv);

    // generate rsa keys
    ret = tt_rsa_generate(&key, TT_RSA_SIZE_1024BIT, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_buf_reset_rwp(&output);
    n = sizeof(buf);
    ret = tt_rsa_encrypt(&key, (tt_u8_t *)__rsa_1024_in, 128, buf, &n);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(n, 128, "");

    // encrypt 0
    n = sizeof(buf);
    ret = tt_rsa_encrypt(&key, (tt_u8_t *)__rsa_1024_in, 0, buf, &n);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(n, 0, "");

    tt_buf_init(&output2, NULL);
    n = sizeof(buf2);
    ret = tt_rsa_decrypt(&key, buf, 128, buf2, &n);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(n, 128, "");

    // decrypt 0
    n = sizeof(buf2);
    ret = tt_rsa_decrypt(&key, buf, 0, buf2, &n);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(n, 0, "");

    TT_UT_EQUAL(tt_memcmp(__rsa_1024_in, buf2, 128), 0, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_rsa_1024_oaep)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_rsa_t pub, priv, key;
    tt_result_t ret;
    tt_rsa_number_t rn;

    tt_buf_t output, output2;
    tt_u8_t buf[128];
    tt_u8_t buf2[128];
    tt_u32_t n, n2;

    tt_blob_t key_data;
    tt_rsa_attr_t attr;

    TT_TEST_CASE_ENTER()
    // test start

    tt_rsa_attr_default(&attr);
    attr.padding.mode = TT_RSA_PADDING_OAEP;
    attr.pem_armor = TT_FALSE;

    // rsa 1024 oaep encrypt
    key_data.addr = (tt_u8_t *)__rsa_pub_1024_bsafe;
    key_data.len = (tt_u32_t)sizeof(__rsa_pub_1024_bsafe);
    ret = tt_rsa_create(&pub,
                        TT_RSA_FORMAT_PKCS1,
                        TT_RSA_TYPE_PUBLIC,
                        &key_data,
                        &attr);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    tt_rsa_show(&pub);

    ret = tt_rsa_get_number(&pub, &rn);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_buf_init(&output, NULL);
    ret = tt_rsa_encrypt_buf(&pub, (tt_u8_t *)__rsa_1024_in, 201, &output);
    TT_UT_EQUAL(ret, TT_FAIL, "");

    // 86 is max data size for rsa1024_oaep
    ret = tt_rsa_encrypt_buf(&pub, (tt_u8_t *)__rsa_1024_in, 86, &output);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    TT_UT_EQUAL(TT_BUF_RLEN(&output), 128, "");
    // oaep encryption including a random seed, so output is not constant

    // rsa 1024 decrypt
    key_data.addr = (tt_u8_t *)__rsa_priv_1024;
    key_data.len = (tt_u32_t)strlen(__rsa_priv_1024);
    attr.pem_armor = TT_TRUE;
    ret = tt_rsa_create(&priv,
                        TT_RSA_FORMAT_PKCS1,
                        TT_RSA_TYPE_PRIVATE,
                        &key_data,
                        &attr);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    // tt_rsa_show(&priv);

    tt_buf_init(&output2, NULL);
    ret = tt_rsa_decrypt_buf(&priv,
                             (tt_u8_t *)TT_BUF_RPOS(&output),
                             TT_BUF_RLEN(&output),
                             &output2);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    TT_UT_EQUAL(TT_BUF_RLEN(&output2), 86, "");
    TT_UT_EQUAL(tt_memcmp(__rsa_1024_in, TT_BUF_RPOS(&output2), 86), 0, "");

    tt_rsa_destroy(&pub);
    tt_rsa_destroy(&priv);

    // generate rsa keys
    ret = tt_rsa_generate(&key, TT_RSA_SIZE_1024BIT, &attr);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    n = sizeof(buf);
    ret = tt_rsa_encrypt(&key, (tt_u8_t *)__rsa_1024_in, 80, buf, &n);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(n, 128, "");

    n2 = sizeof(buf2);
    ret = tt_rsa_decrypt(&key, buf, n, buf2, &n2);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(n2, 80, "");

    TT_UT_EQUAL(tt_memcmp(__rsa_1024_in, buf2, n2), 0, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

static const tt_char_t *__rsa_pub_2048_enc =
    "-----BEGIN PUBLIC KEY-----\n"
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAvv66a0SGXEBqV1cwAmbg\n"
    "oCKfN4AM/jUJRaXxfZk8O3nBFT+LMu8Hou3GfB9B9iYIBUInm5sPF9a0+F6XUojX\n"
    "ErwyYhE0Nv2WK7HgcVTk1QQQQQEKAMBEjCXkxhV6Tqxr5xuy9/zDDZkcO2S1n7E3\n"
    "KGbh4CUzXfX+3maHeU5Bvus9Zf37tKq8kJM9AwVsoCjagqv7AVQpOu0wkjm29hCU\n"
    "J/IKg6liDeREdzka5ULzWH2z1/TwkwFh2f31UD26EvpgnCfrZExp5+D8yhVSgbeh\n"
    "xI9jvkRq9ROdiLlasx9zxZY20667IzQrhK3ABXx4C3qcMMgH4WiOLQiHsVfGKA7Z\n"
    "MQIDAQAB\n"
    "-----END PUBLIC KEY-----\n";

static const tt_char_t *__rsa_priv_2048_pkcs8 =
    "-----BEGIN PRIVATE KEY-----\n"
    "MIIEvAIBADANBgkqhkiG9w0BAQEFAASCBKYwggSiAgEAAoIBAQC+/rprRIZcQGpX\n"
    "VzACZuCgIp83gAz+NQlFpfF9mTw7ecEVP4sy7wei7cZ8H0H2JggFQiebmw8X1rT4\n"
    "XpdSiNcSvDJiETQ2/ZYrseBxVOTVBBBBAQoAwESMJeTGFXpOrGvnG7L3/MMNmRw7\n"
    "ZLWfsTcoZuHgJTNd9f7eZod5TkG+6z1l/fu0qryQkz0DBWygKNqCq/sBVCk67TCS\n"
    "Obb2EJQn8gqDqWIN5ER3ORrlQvNYfbPX9PCTAWHZ/fVQPboS+mCcJ+tkTGnn4PzK\n"
    "FVKBt6HEj2O+RGr1E52IuVqzH3PFljbTrrsjNCuErcAFfHgLepwwyAfhaI4tCIex\n"
    "V8YoDtkxAgMBAAECggEAKrfKdidzbbqyjipgAImGAAr+VWl8fnbaRguhgZf+Usce\n"
    "mmQF/DReHuDTnftXw3gqHxL2zOTXcVuCSnLNoZYdlIEnO6vhY9M//THhozd7nvEz\n"
    "uXZECHr4Weg3zN4M5v/FeoFFfBW6gpYWLGqP/NGkJpMsPffUD1Xv1UpVTppkNhD/\n"
    "44u9ROANtLfEqGhJjmjuysbc9EoLDEkoUn9MyQyDUVhUovaw+BhWu2KgbezBKOiE\n"
    "5UjLMV4iabqNJX2dD7BkU0j3/wvEoTXrQhLSbXo7Be1OISZpaMaVB6O8uVjaUIkf\n"
    "a9PJ/Ka/mwQnER/hoRRqy9FIWVTzawBDf+YOK3Eg6QKBgQDzPHF61vNKGc7MJV1K\n"
    "9KFB/3cVyoBrLn5YgoFe3jhaD3ZTl7ij/ax4LDgjRBrKNBUHvarAOm8zbciwBow7\n"
    "ct9d6i/ETGKfgKu8qNTRXYn8uXNWtj1xJT/LWuItJG7HJpsT2RdMMgzAC9OwaTXS\n"
    "WxA5pLOixwxCBCFEhYU5gwIxiwKBgQDJBH6xINQUUJksgVscsqWUU3a1KmjedSYT\n"
    "IjRbcLfXNkjSfle3abhTMkjkDyoz3x0oWeb65pJ28G9p0tlf7zIrLzRrRqdBUmFb\n"
    "y1u9q1hJADSs5ZO7nb0D6kXmG23t80a5rx9HQQ90GpSndm41niWBdLXWVZ0UPPVm\n"
    "cocrep8/swKBgCXfF1wk86Rp+ETY3Hm7NRBtkkEWUoBdsNZfG32F5AhBuA6z5M/D\n"
    "w4LCf8sd433wNgYKhfeAIahEQ6cEnfjznculluJdck2bXyH0t44RnlZ3GXo6zb48\n"
    "Kn6jG+wjbAwd+q9QccwvTsToEoG5H+QswY34EvAZRdNRiUx97YxS1ZiZAoGAbzia\n"
    "dJzKWxchORu1ctJxhPkNND3r2jINSAbj2zgpr9zsO/MFy4imIPowlGbDngGz21Cq\n"
    "2WXb4g00R+QOu+LX6kbrBPQJtL3E+ooxD2ePqIyBmKxJqkQjgdgMpr3EfCTfm/uG\n"
    "HeGkkEPoviNr5STEcbVGqvmhuMHlsbJqkab+F7sCgYBwKWzTeun0RLkbMUDomAW/\n"
    "Tf3s+rmtNNi+E5e4/Oaje36FseT4JEHRllE+6UZ3pY8ZZuvBoHn0GweeWCRwDtBn\n"
    "3YSmd3JgWuztEOVmZJ5V6FgAKhdcOq7rRuK/mv4YwYObZUZnMjQNO2/dFsrimbbG\n"
    "3tEFLDBpNkH7em9e+faoeg==\n"
    "-----END PRIVATE KEY-----\n";

static const tt_char_t *__rsa_priv_2048_pkcs8_enc =
    "-----BEGIN ENCRYPTED PRIVATE KEY-----\n"
    "MIIFHzBJBgkqhkiG9w0BBQ0wPDAbBgkqhkiG9w0BBQwwDgQIdE8ghPaeRnMCAggA\n"
    "MB0GCWCGSAFlAwQBAgQQCxSeXB5CDIBYtcjTWcDDWwSCBNAbs09d0YEkiUXCrKMu\n"
    "Br5ZKZxlMPjUQzpM1mjuYG79TIoZfqXioitntGx9YPghln/iqIUoy/ZbWmz1E+Wg\n"
    "6/xGY4+Ei8dyvmIHbgcv10SJbdoSU10ZMKLfBdnjwuZCZC7NMsMhpGhv8/ZhWMbG\n"
    "eVUQfq1lcYp4R3zU00AF6yKtYiXflyDcsq4cb9ywlkLtlQ7nEj3dP0jn+/FXHCl4\n"
    "f2FRQQ89jKTlHArkFqgkYOi0HNyqisNY6t+2Dy9QeqLyd/kNvZWbURtMLEQ3+K+2\n"
    "q5GjBjYswcbBrHjxCzSSkUpuULNYfk6FVMcsYWKh3ju4kBOTR5auxRL1lHdJU1/Q\n"
    "h+LI2NmFoF/WyLvdEbYW6mX5/j5etbZrHNbM+ToQLZF8e5OurVRmFEjk0JymlN2B\n"
    "c0npIaZiAT2dgRDgeUky6+vDs667vbsqu+IlrtwlrgPeuBQcwH4NmevhMBLEuzU2\n"
    "OxYyNDIp752N5abWx9clk6kXfAAhIYoE48uy32Fzw5u1MHbYkUt7lRnUjYba4kA8\n"
    "jDrg6mjHugd9iq6MIkPVLhZ5VzCiVK+7l3Afhqzd0nrbqdgDv3+G/tkqwZ+17BIp\n"
    "8pGL1Ms2FuUwk9V56dTi89vKQWDgorYchXcmsBXthJI05qhoZNxbXQXnqpeX6vx6\n"
    "1Z7Za40/3Qkh8rosaXUpMiQGw2whjXr8R1h9O5cKPnDH2Nho/jEAEYZb+nKEqn7k\n"
    "JbV/Vo+xQT/ipCozu+edWJjnuY7chiDP1k9IKIhz/9pFfzmb/iGy64p0EQ4/Gh6v\n"
    "SDXHJhRaU0rjouJwZMui3642b9W/qLbazh/cudxPYtFptCuE2y4YLq7epusQdprl\n"
    "O8g8PW88KV7S6BmmbWViFW1pQ6GBycd5MZvtWFtBQdNZ0F24xy25ycwNjXFcg2cq\n"
    "rmtYQmIWxAhNQ4xw+h6THLsz0fN86rbDG9BrSXk7UTOXJ+5avvydPLSLZP582p+p\n"
    "qEAuJBRGqCuwt8hNw/WxvVy8JFtQqwKKFQOJg0KUIr3X5nm3h3wPp/tmgJlJg8+D\n"
    "bAKSGqiMoLcc8OEu7SEFgWqLh31F5YHR4WcHjGly9cWPptXUeEcYbLyOQ3ihiVxm\n"
    "rcrOYX7RQ+hPS3QnXLmee4knVTEiP2MDENj0bQ1ArRZBCBr000nzkWuxwnMbqICz\n"
    "R7KLbLPdSZ6S29XaaF5qrX4WQkLndOzD6/iBHHeR+jVoo+tAHQdsuLYSBRysoCxT\n"
    "Qa54P4pixcmEH8KVueM9uy27PdMJVlHgHjKoveV7psbdouTpOc91bFCSvcLrGWC+\n"
    "zzUJEaVox1JIuJ0fEf7RYpLzGcKHnEPneFygYX6vZfSq6b1XtWuK1ovXSopoen9m\n"
    "dPAKU1k4lZqF+WPp9co7aYwnOm+xJWH16uHbfuVYlGM9VmJ9UQp1N2AlhQ75xF6+\n"
    "/zO/m2aSFcV/zHlw//rYVP2gPFYwp0I0lJ9GnSVFzcWIWL745HMlBhim50qbPhtU\n"
    "1d4RNL/MuFqiscyz0UVfzWU4zl+X57RveVhzoRQCH62UYn0SApscc/aGA9oWmhe2\n"
    "n+l10MjGIiu3fqTChDsJ3VecQ2g2erXHoeWaHWhCO/BwTPdOftImOw3WNkGyCWA1\n"
    "gALQDIvVfw4eXRP+HrmPoz0Vgw==\n"
    "-----END ENCRYPTED PRIVATE KEY-----\n";

static tt_u8_t __rsa_pub_2048_mod[] =
    {0xbe, 0xfe, 0xba, 0x6b, 0x44, 0x86, 0x5c, 0x40, 0x6a, 0x57, 0x57, 0x30,
     0x02, 0x66, 0xe0, 0xa0, 0x22, 0x9f, 0x37, 0x80, 0x0c, 0xfe, 0x35, 0x09,
     0x45, 0xa5, 0xf1, 0x7d, 0x99, 0x3c, 0x3b, 0x79, 0xc1, 0x15, 0x3f, 0x8b,
     0x32, 0xef, 0x07, 0xa2, 0xed, 0xc6, 0x7c, 0x1f, 0x41, 0xf6, 0x26, 0x08,
     0x05, 0x42, 0x27, 0x9b, 0x9b, 0x0f, 0x17, 0xd6, 0xb4, 0xf8, 0x5e, 0x97,
     0x52, 0x88, 0xd7, 0x12, 0xbc, 0x32, 0x62, 0x11, 0x34, 0x36, 0xfd, 0x96,
     0x2b, 0xb1, 0xe0, 0x71, 0x54, 0xe4, 0xd5, 0x04, 0x10, 0x41, 0x01, 0x0a,
     0x00, 0xc0, 0x44, 0x8c, 0x25, 0xe4, 0xc6, 0x15, 0x7a, 0x4e, 0xac, 0x6b,
     0xe7, 0x1b, 0xb2, 0xf7, 0xfc, 0xc3, 0x0d, 0x99, 0x1c, 0x3b, 0x64, 0xb5,
     0x9f, 0xb1, 0x37, 0x28, 0x66, 0xe1, 0xe0, 0x25, 0x33, 0x5d, 0xf5, 0xfe,
     0xde, 0x66, 0x87, 0x79, 0x4e, 0x41, 0xbe, 0xeb, 0x3d, 0x65, 0xfd, 0xfb,
     0xb4, 0xaa, 0xbc, 0x90, 0x93, 0x3d, 0x03, 0x05, 0x6c, 0xa0, 0x28, 0xda,
     0x82, 0xab, 0xfb, 0x01, 0x54, 0x29, 0x3a, 0xed, 0x30, 0x92, 0x39, 0xb6,
     0xf6, 0x10, 0x94, 0x27, 0xf2, 0x0a, 0x83, 0xa9, 0x62, 0x0d, 0xe4, 0x44,
     0x77, 0x39, 0x1a, 0xe5, 0x42, 0xf3, 0x58, 0x7d, 0xb3, 0xd7, 0xf4, 0xf0,
     0x93, 0x01, 0x61, 0xd9, 0xfd, 0xf5, 0x50, 0x3d, 0xba, 0x12, 0xfa, 0x60,
     0x9c, 0x27, 0xeb, 0x64, 0x4c, 0x69, 0xe7, 0xe0, 0xfc, 0xca, 0x15, 0x52,
     0x81, 0xb7, 0xa1, 0xc4, 0x8f, 0x63, 0xbe, 0x44, 0x6a, 0xf5, 0x13, 0x9d,
     0x88, 0xb9, 0x5a, 0xb3, 0x1f, 0x73, 0xc5, 0x96, 0x36, 0xd3, 0xae, 0xbb,
     0x23, 0x34, 0x2b, 0x84, 0xad, 0xc0, 0x05, 0x7c, 0x78, 0x0b, 0x7a, 0x9c,
     0x30, 0xc8, 0x07, 0xe1, 0x68, 0x8e, 0x2d, 0x08, 0x87, 0xb1, 0x57, 0xc6,
     0x28, 0x0e, 0xd9, 0x31};

static tt_char_t __rsa_2048_out[256] = {
    0xb3, 0xc3, 0x85, 0x7f, 0xd2, 0xa9, 0xc9, 0xca, 0x0b, 0xc5, 0x5c, 0x0f,
    0xd5, 0x74, 0xf7, 0xed, 0x0c, 0x1f, 0x39, 0xaf, 0xb0, 0xfa, 0xb9, 0xd9,
    0x29, 0x16, 0x45, 0xa1, 0x19, 0x4c, 0x1e, 0x0c, 0xe8, 0x62, 0xaf, 0x79,
    0x49, 0x9e, 0xb6, 0xe9, 0xef, 0x5f, 0xc8, 0xb4, 0x00, 0xe1, 0x7a, 0x55,
    0x4f, 0x38, 0xe9, 0xa5, 0x76, 0xab, 0xbf, 0xe0, 0x6e, 0x19, 0x45, 0x76,
    0xe6, 0x37, 0x45, 0x3e, 0xc8, 0x5b, 0xde, 0x9a, 0xc5, 0xdb, 0x99, 0x11,
    0x8e, 0x26, 0xa4, 0x09, 0x59, 0x15, 0xc6, 0xff, 0x62, 0xeb, 0x66, 0xdb,
    0x4e, 0x7d, 0xfd, 0x64, 0xd9, 0xbd, 0x84, 0x3e, 0xf8, 0xd2, 0x8c, 0x0d,
    0x47, 0xc9, 0x3a, 0xec, 0xf5, 0xb4, 0xe3, 0x0f, 0xb1, 0xac, 0x0f, 0x5b,
    0xf4, 0xae, 0x08, 0x2a, 0xf0, 0x33, 0xc8, 0xe7, 0xdc, 0x34, 0x09, 0x89,
    0x4b, 0x8a, 0x4a, 0xbe, 0x6b, 0xd6, 0x97, 0x43, 0xe3, 0xb4, 0xbe, 0x58,
    0x24, 0x32, 0x5c, 0x34, 0xdd, 0xe9, 0xa1, 0xf2, 0xd1, 0x21, 0xdc, 0x1d,
    0x7b, 0xa2, 0xd9, 0xb4, 0x7e, 0x5a, 0x5b, 0xfe, 0xe6, 0xd2, 0xce, 0xdf,
    0x70, 0x17, 0x1e, 0xcd, 0xbf, 0x24, 0xc1, 0xe2, 0x36, 0xc0, 0x01, 0xde,
    0xf9, 0x12, 0x89, 0x10, 0x99, 0x8c, 0x8c, 0x54, 0xdf, 0xec, 0x9f, 0xc2,
    0x5d, 0xf2, 0xb9, 0x7e, 0x74, 0x3b, 0xd4, 0x4f, 0x63, 0x9b, 0x7d, 0xba,
    0x06, 0x1a, 0xd6, 0x93, 0x1c, 0xf8, 0x86, 0x7c, 0xf6, 0xe6, 0xc6, 0x8d,
    0x02, 0x2b, 0xde, 0xd1, 0x3d, 0x9e, 0x93, 0x06, 0xd3, 0x89, 0xdc, 0x09,
    0xbf, 0x6f, 0x85, 0xec, 0x3b, 0xb7, 0xa6, 0x7d, 0x99, 0xb1, 0x3f, 0x30,
    0x08, 0xc5, 0x5d, 0x0e, 0x0a, 0x04, 0xbf, 0x1e, 0x46, 0xa0, 0x02, 0x15,
    0x85, 0x47, 0x36, 0x53, 0x1f, 0x2a, 0xf0, 0x4a, 0x80, 0xc9, 0xd6, 0x47,
    0x5b, 0x5d, 0x7b, 0x9c,
};

TT_TEST_ROUTINE_DEFINE(tt_unit_test_rsa_2048_enc_pkcs8)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_rsa_t pub, priv, key;
    tt_result_t ret;
    tt_rsa_number_t rn;

    tt_buf_t output, output2;
    tt_u8_t buf[256];
    tt_u8_t buf2[256];
    tt_u32_t n;

    tt_blob_t key_data;
    tt_rsa_attr_t attr;

    TT_TEST_CASE_ENTER()
    // test start

    tt_rsa_attr_default(&attr);
    attr.password = "abc123";

    // rsa 2048 encrypt
    key_data.addr = (tt_u8_t *)__rsa_pub_2048_enc;
    key_data.len = (tt_u32_t)strlen(__rsa_pub_2048_enc);
    ret = tt_rsa_create(&pub,
                        TT_RSA_FORMAT_PKCS1,
                        TT_RSA_TYPE_PUBLIC,
                        &key_data,
                        NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    tt_rsa_show(&pub);

    ret = tt_rsa_get_number(&pub, &rn);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    TT_UT_EQUAL(rn.pubnum.modulus.len, sizeof(__rsa_pub_2048_mod), "");
    TT_UT_EQUAL(tt_memcmp(rn.pubnum.modulus.addr,
                          __rsa_pub_2048_mod,
                          sizeof(__rsa_pub_2048_mod)),
                0,
                "");

    // same as 1024: 65536
    TT_UT_EQUAL(rn.pubnum.pub_exp.len, sizeof(__rsa_pub_1024_wrapped_exp), "");
    TT_UT_EQUAL(tt_memcmp(rn.pubnum.pub_exp.addr,
                          __rsa_pub_1024_wrapped_exp,
                          sizeof(__rsa_pub_1024_wrapped_exp)),
                0,
                "");

    tt_buf_init(&output, NULL);

    // buf

    // size must be 128
    ret = tt_rsa_encrypt_buf(&pub, (tt_u8_t *)__rsa_1024_in, 200, &output);
    TT_UT_EQUAL(ret, TT_FAIL, "");

    ret = tt_rsa_encrypt_buf(&pub, (tt_u8_t *)__rsa_1024_in, 256, &output);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    TT_UT_EQUAL(TT_BUF_RLEN(&output), 256, "");
    TT_UT_EQUAL(tt_memcmp(__rsa_2048_out, TT_BUF_RPOS(&output), 256), 0, "");
    // printf("%s\n", __rsa_pub_2048_enc);
    // printf("%s\n", __rsa_priv_2048_pkcs8_enc);

    // rsa 2048 decrypt
    key_data.addr = (tt_u8_t *)__rsa_priv_2048_pkcs8_enc;
    key_data.len = (tt_u32_t)strlen(__rsa_priv_2048_pkcs8_enc);
    ret = tt_rsa_create(&priv,
                        TT_RSA_FORMAT_PKCS8,
                        TT_RSA_TYPE_PRIVATE,
                        &key_data,
                        &attr);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    // tt_rsa_show(&priv);

    tt_buf_reset_rwp(&output);
    ret = tt_rsa_decrypt_buf(&priv, (tt_u8_t *)__rsa_2048_out, 256, &output);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    TT_UT_EQUAL(TT_BUF_RLEN(&output), 256, "");
    TT_UT_EQUAL(tt_memcmp(__rsa_1024_in, TT_BUF_RPOS(&output), 256), 0, "");
    tt_rsa_destroy(&priv);

    // rsa 2048(pkcs8 non-crypted) decrypt
    key_data.addr = (tt_u8_t *)__rsa_priv_2048_pkcs8;
    key_data.len = (tt_u32_t)strlen(__rsa_priv_2048_pkcs8);
    attr.password = NULL;
    ret = tt_rsa_create(&priv,
                        TT_RSA_FORMAT_PKCS8,
                        TT_RSA_TYPE_PRIVATE,
                        &key_data,
                        &attr);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    // tt_rsa_show(&priv);

    tt_buf_reset_rwp(&output);
    ret = tt_rsa_decrypt_buf(&priv, (tt_u8_t *)__rsa_2048_out, 256, &output);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    TT_UT_EQUAL(TT_BUF_RLEN(&output), 256, "");
    TT_UT_EQUAL(tt_memcmp(__rsa_1024_in, TT_BUF_RPOS(&output), 256), 0, "");

    tt_rsa_destroy(&pub);
    tt_rsa_destroy(&priv);

    // generate rsa keys
    ret = tt_rsa_generate(&key, TT_RSA_SIZE_2048BIT, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_buf_reset_rwp(&output);
    n = sizeof(buf);
    ret = tt_rsa_encrypt(&key, (tt_u8_t *)__rsa_1024_in, 256, buf, &n);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(n, 256, "");

    tt_buf_init(&output2, NULL);
    n = sizeof(buf2);
    ret = tt_rsa_decrypt(&key, buf, 256, buf2, &n);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(n, 256, "");

    TT_UT_EQUAL(tt_memcmp(__rsa_1024_in, buf2, 256), 0, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_rsa_2048_padpkcs1)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_rsa_t pub, priv;
    tt_result_t ret;

    tt_buf_t output, output2;
    tt_u32_t n;

    tt_blob_t key_data;
    tt_rsa_attr_t attr;

    TT_TEST_CASE_ENTER()
    // test start

    tt_rsa_attr_default(&attr);
    attr.padding.mode = TT_RSA_PADDING_PKCS1;

    // rsa 2048 encrypt
    key_data.addr = (tt_u8_t *)__rsa_pub_2048_enc;
    key_data.len = (tt_u32_t)strlen(__rsa_pub_2048_enc);
    ret = tt_rsa_create(&pub,
                        TT_RSA_FORMAT_PKCS1,
                        TT_RSA_TYPE_PUBLIC,
                        &key_data,
                        &attr);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    tt_rsa_show(&pub);

    tt_buf_init(&output, NULL);
    tt_buf_init(&output2, NULL);

    // rsa 2048 decrypt
    key_data.addr = (tt_u8_t *)__rsa_priv_2048_pkcs8_enc;
    key_data.len = (tt_u32_t)strlen(__rsa_priv_2048_pkcs8_enc);
    attr.password = "abc123";
    ret = tt_rsa_create(&priv,
                        TT_RSA_FORMAT_PKCS8,
                        TT_RSA_TYPE_PRIVATE,
                        &key_data,
                        &attr);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    // buf

    // size: 1
    n = 1;
    tt_buf_reset_rwp(&output);
    ret = tt_rsa_encrypt_buf(&pub, (tt_u8_t *)__rsa_1024_in, n, &output);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(TT_BUF_RLEN(&output), 256, "");

    tt_buf_reset_rwp(&output2);
    ret = tt_rsa_decrypt_buf(&priv, TT_BUF_RPOS(&output), 256, &output2);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(&output2), (tt_u8_t *)__rsa_1024_in, n),
                0,
                "");

    // size: 245
    n = 245;
    tt_buf_reset_rwp(&output);
    ret = tt_rsa_encrypt_buf(&pub, (tt_u8_t *)__rsa_1024_in, n, &output);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(TT_BUF_RLEN(&output), 256, "");

    tt_buf_reset_rwp(&output2);
    ret = tt_rsa_decrypt_buf(&priv, TT_BUF_RPOS(&output), 256, &output2);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(&output2), (tt_u8_t *)__rsa_1024_in, n),
                0,
                "");

    // random:
    while ((n = tt_rand_u32() % 246) == 0)
        ;
    tt_buf_reset_rwp(&output);
    ret = tt_rsa_encrypt_buf(&pub, (tt_u8_t *)__rsa_1024_in, n, &output);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(TT_BUF_RLEN(&output), 256, "");

    tt_buf_reset_rwp(&output2);
    ret = tt_rsa_decrypt_buf(&priv, TT_BUF_RPOS(&output), 256, &output2);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(&output2), (tt_u8_t *)__rsa_1024_in, n),
                0,
                "");

    // size: 246, exceed pkcs1 padding length
    n = 246;
    tt_buf_reset_rwp(&output);
    ret = tt_rsa_encrypt_buf(&pub, (tt_u8_t *)__rsa_1024_in, n, &output);
    TT_UT_EQUAL(ret, TT_FAIL, "");

    tt_buf_destroy(&output);
    tt_buf_destroy(&output2);

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_u8_t __pub_m[] =
    {0x00, 0xbc, 0xb1, 0x66, 0x00, 0x48, 0x92, 0xc7, 0xda, 0x92, 0x32, 0x3f,
     0x96, 0xe4, 0xb6, 0xd3, 0xf6, 0x23, 0x03, 0xcb, 0xfe, 0xae, 0x9a, 0x19,
     0xf4, 0x80, 0xee, 0x33, 0x5d, 0x6c, 0xb5, 0x62, 0x00, 0x1f, 0xaf, 0xc6,
     0xe7, 0x9b, 0x7c, 0xde, 0x2d, 0x7b, 0x5b, 0x8e, 0x92, 0x53, 0x98, 0x00,
     0x67, 0x79, 0xaa, 0xa8, 0x60, 0x60, 0xfd, 0x62, 0x08, 0x54, 0x72, 0x1e,
     0x9d, 0x36, 0xc4, 0x7f, 0xd5, 0xef, 0xfc, 0x8d, 0xf5, 0x4e, 0x34, 0xfe,
     0x41, 0x3b, 0x9a, 0x57, 0x6d, 0x54, 0xcb, 0xcd, 0x3d, 0x01, 0xa2, 0x6a,
     0x31, 0x43, 0x0f, 0x47, 0xe1, 0x2c, 0xf8, 0x42, 0x3f, 0x20, 0x9e, 0xf0,
     0xc1, 0xe7, 0x30, 0x29, 0xaf, 0x3f, 0x78, 0xd1, 0xdd, 0x51, 0xc7, 0x45,
     0xc9, 0xab, 0x0a, 0xf8, 0x85, 0x30, 0xbc, 0xe9, 0xc5, 0xdc, 0xf9, 0xf2,
     0x98, 0xab, 0xa2, 0x8b, 0x62, 0xa2, 0x31, 0xf9, 0x89};

static tt_u8_t __pub_e[] = {0x01, 0x00, 0x01};

static tt_u8_t __priv_privexp[] =
    {0x4d, 0x87, 0x19, 0xe0, 0xbd, 0xf0, 0xc8, 0xfa, 0x8e, 0x1c, 0xe5, 0x31,
     0xab, 0x51, 0x4f, 0x2f, 0x8f, 0x5d, 0x6d, 0x00, 0x7c, 0x5e, 0xc6, 0x91,
     0x46, 0x34, 0x18, 0x98, 0xa7, 0x24, 0xb3, 0x5a, 0x57, 0xaa, 0x92, 0x2f,
     0xf1, 0x1b, 0xd1, 0xf1, 0xa0, 0x00, 0x93, 0x48, 0x9d, 0xde, 0x13, 0x34,
     0x70, 0xb1, 0xe4, 0xb2, 0x8c, 0xc7, 0x2e, 0xf3, 0x3b, 0x37, 0x14, 0x4b,
     0x06, 0x08, 0xa0, 0x78, 0xe2, 0x54, 0xab, 0xd6, 0x8e, 0x0e, 0xc1, 0x32,
     0xf4, 0xc9, 0xb6, 0x57, 0xcc, 0x3f, 0xcc, 0xb1, 0x63, 0x8e, 0xd3, 0xd7,
     0xfb, 0xdb, 0x17, 0xa3, 0xe7, 0x94, 0x6d, 0xed, 0xf9, 0xba, 0xf5, 0xac,
     0xc7, 0x0b, 0x32, 0xcd, 0xcc, 0xab, 0x0f, 0xac, 0x63, 0x7a, 0x3d, 0x44,
     0x04, 0x7e, 0xa3, 0x36, 0x0c, 0xc7, 0x02, 0x02, 0x9f, 0x32, 0xf8, 0x58,
     0x8a, 0x6d, 0xb1, 0xf8, 0x5b, 0x06, 0x27, 0x01};

static tt_u8_t __priv_prim1[] = {0x00, 0xe8, 0xcd, 0x67, 0x5f, 0x7b, 0x43, 0xa3,
                                 0xde, 0xec, 0x28, 0x0b, 0xa1, 0xdd, 0xbb, 0x28,
                                 0xa6, 0xe2, 0xa3, 0xca, 0x04, 0x22, 0x7d, 0x33,
                                 0x29, 0xbc, 0x1a, 0x5b, 0x67, 0x62, 0xb8, 0x1e,
                                 0x12, 0x2c, 0x86, 0x90, 0x2f, 0x72, 0xc2, 0x73,
                                 0x46, 0x63, 0xa2, 0x0e, 0x85, 0xfe, 0xb3, 0x70,
                                 0x2e, 0x10, 0x75, 0xdd, 0x4f, 0xe7, 0x37, 0xdf,
                                 0x6e, 0x8a, 0x3d, 0x02, 0xb6, 0x01, 0x03, 0x14,
                                 0xe9};

static tt_u8_t __priv_prim2[] = {0x00, 0xcf, 0x7e, 0xcc, 0xe1, 0xff, 0x1e, 0xa7,
                                 0x01, 0x53, 0xe1, 0x45, 0xaf, 0xb2, 0xcf, 0xde,
                                 0xfa, 0x4e, 0x2f, 0xad, 0xcf, 0xb6, 0x64, 0xc6,
                                 0x43, 0x67, 0x51, 0x83, 0x86, 0xde, 0x0e, 0x39,
                                 0xf7, 0x2b, 0x37, 0xe2, 0xb6, 0xcd, 0xc2, 0x6e,
                                 0x4c, 0xf8, 0xf0, 0x87, 0x8c, 0xd2, 0xe3, 0x28,
                                 0x6c, 0xe0, 0xee, 0x3c, 0x0c, 0x3a, 0x84, 0x98,
                                 0xa1, 0x8f, 0xd6, 0xdf, 0xf0, 0xb8, 0x5b, 0x5b,
                                 0xa1};

static tt_u8_t __priv_e1[] = {0x00, 0xde, 0xa0, 0xf7, 0x97, 0x6e, 0xd8, 0xab,
                              0x90, 0x96, 0x84, 0x61, 0x43, 0x65, 0x39, 0x03,
                              0x52, 0x3c, 0xf0, 0xe7, 0x52, 0xf6, 0xec, 0x86,
                              0x0e, 0x06, 0x4d, 0xaa, 0x55, 0x7f, 0x29, 0x95,
                              0xd0, 0x49, 0x76, 0x51, 0xd2, 0x9e, 0x4b, 0x68,
                              0x53, 0xcb, 0x1a, 0x02, 0x0b, 0x90, 0x1c, 0x67,
                              0x4e, 0x15, 0xfd, 0xb1, 0x2c, 0x25, 0x70, 0xe0,
                              0x21, 0xe4, 0xdf, 0x1d, 0x85, 0x92, 0x08, 0x31,
                              0x69};

static tt_u8_t __priv_e2[] = {0x00, 0xc4, 0x01, 0x6f, 0xe0, 0xfc, 0x72, 0x24,
                              0x28, 0x68, 0xe8, 0x20, 0x3f, 0xfc, 0x95, 0xc8,
                              0x78, 0x82, 0x6b, 0x1f, 0xb1, 0x5b, 0x06, 0x8f,
                              0x13, 0x15, 0x02, 0x37, 0x50, 0xab, 0x6a, 0x9c,
                              0xef, 0x13, 0x70, 0x53, 0x45, 0x83, 0xd0, 0xeb,
                              0x08, 0xe3, 0x0f, 0xb3, 0xbc, 0x1f, 0x6a, 0x5d,
                              0xc1, 0x9c, 0x28, 0xef, 0xa8, 0x4d, 0x8e, 0xfb,
                              0xef, 0x82, 0x44, 0xe4, 0x7e, 0x2c, 0x68, 0x80,
                              0x81};

static tt_u8_t __priv_coeff[] = {0x0f, 0x43, 0x79, 0xc7, 0xeb, 0xbe, 0xf1,
                                 0xdc, 0xa0, 0x0c, 0x52, 0xc4, 0xa7, 0x7e,
                                 0x4d, 0x41, 0x28, 0xcc, 0xa3, 0x71, 0xf5,
                                 0xcc, 0xe0, 0x29, 0xc5, 0x55, 0xec, 0x72,
                                 0x66, 0x95, 0xd4, 0xc6, 0xf9, 0x64, 0x7e,
                                 0xb2, 0xe7, 0x0b, 0xa7, 0x67, 0x62, 0x46,
                                 0xad, 0x29, 0x57, 0x04, 0xdf, 0x56, 0x64,
                                 0x1d, 0x63, 0x48, 0xfb, 0xba, 0xc1, 0x86,
                                 0xf4, 0x37, 0x77, 0xe2, 0xde, 0x3c, 0xb1,
                                 0xdd};

TT_TEST_ROUTINE_DEFINE(tt_unit_test_rsa_1024_num)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_rsa_t pub, priv;
    tt_result_t ret;
    tt_rsa_number_t rn;

    tt_buf_t output;

    TT_TEST_CASE_ENTER()
    // test start

    rn.type = TT_RSA_TYPE_PUBLIC;
    rn.pubnum.modulus.addr = __pub_m;
    rn.pubnum.modulus.len = sizeof(__pub_m);
    rn.pubnum.pub_exp.addr = __pub_e;
    rn.pubnum.pub_exp.len = sizeof(__pub_e);

    ret = tt_rsa_create_number(&pub, &rn, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_buf_init(&output, NULL);

    // buf

    // size must be 128
    ret = tt_rsa_encrypt_buf(&pub, (tt_u8_t *)__rsa_1024_in, 127, &output);
    TT_UT_EQUAL(ret, TT_FAIL, "");

    ret = tt_rsa_encrypt_buf(&pub, (tt_u8_t *)__rsa_1024_in, 128, &output);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    TT_UT_EQUAL(TT_BUF_RLEN(&output), 128, "");
    TT_UT_EQUAL(tt_memcmp(__rsa_1024_out, TT_BUF_RPOS(&output), 128), 0, "");

    // rsa 1024 decrypt
    rn.type = TT_RSA_TYPE_PRIVATE;
    rn.privnum.modulus.addr = __pub_m;
    rn.privnum.modulus.len = sizeof(__pub_m);
    rn.privnum.pub_exp.addr = __pub_e;
    rn.privnum.pub_exp.len = sizeof(__pub_e);
    rn.privnum.priv_exp.addr = __priv_privexp;
    rn.privnum.priv_exp.len = sizeof(__priv_privexp);
    rn.privnum.prime1.addr = __priv_prim1;
    rn.privnum.prime1.len = sizeof(__priv_prim1);
    rn.privnum.prime2.addr = __priv_prim2;
    rn.privnum.prime2.len = sizeof(__priv_prim2);
    rn.privnum.exp1.addr = __priv_e1;
    rn.privnum.exp1.len = sizeof(__priv_e1);
    rn.privnum.exp2.addr = __priv_e2;
    rn.privnum.exp2.len = sizeof(__priv_e2);
    rn.privnum.coefficient.addr = __priv_coeff;
    rn.privnum.coefficient.len = sizeof(__priv_coeff);

    ret = tt_rsa_create_number(&priv, &rn, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_buf_reset_rwp(&output);
    ret = tt_rsa_decrypt_buf(&priv, (tt_u8_t *)__rsa_1024_out, 128, &output);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    TT_UT_EQUAL(TT_BUF_RLEN(&output), 128, "");
    TT_UT_EQUAL(tt_memcmp(__rsa_1024_in, TT_BUF_RPOS(&output), 128), 0, "");

    tt_rsa_destroy(&pub);
    tt_rsa_destroy(&priv);

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_u8_t __rsa_sign_1[] =
    {0xb6, 0x11, 0xee, 0xc8, 0x0c, 0xa0, 0xa7, 0x90, 0xc5, 0x8f, 0x32, 0x48,
     0xb0, 0xc2, 0xef, 0x4c, 0xe6, 0xe9, 0x85, 0xb8, 0x67, 0x85, 0xd1, 0x45,
     0xff, 0xfe, 0xb0, 0x17, 0xe9, 0x4d, 0x50, 0xc9, 0x58, 0xcb, 0x82, 0x5a,
     0x99, 0xdc, 0x66, 0xec, 0xe3, 0x76, 0x95, 0x38, 0xff, 0x3c, 0x27, 0x91,
     0xfa, 0x38, 0x12, 0xaa, 0xcc, 0x57, 0xcb, 0x00, 0xf0, 0x40, 0xe3, 0x70,
     0x2f, 0x34, 0x77, 0xd3, 0x45, 0x49, 0xa1, 0xa0, 0x67, 0x69, 0x97, 0x02,
     0xe4, 0x6d, 0xdd, 0xbd, 0x3d, 0xe7, 0x9e, 0x53, 0xe3, 0x33, 0x82, 0x0e,
     0xee, 0xbb, 0x46, 0xcc, 0x3d, 0xdf, 0xbd, 0x5c, 0x74, 0x99, 0x79, 0xfd,
     0x62, 0xe8, 0xd8, 0xa5, 0xa4, 0x6c, 0xdb, 0x38, 0x7d, 0x4c, 0xf8, 0x2b,
     0x89, 0xd8, 0x5c, 0xcb, 0x22, 0xc2, 0x95, 0x24, 0x9b, 0x24, 0xc7, 0xd0,
     0xd2, 0x68, 0x39, 0xab, 0x2f, 0xe7, 0x70, 0x48, 0x57, 0x19, 0x98, 0xe2,
     0xc7, 0x49, 0x22, 0x69, 0x0e, 0x35, 0xde, 0x6b, 0x29, 0x34, 0x46, 0x83,
     0x86, 0xf1, 0xb1, 0x3f, 0xc6, 0x37, 0x56, 0x88, 0x30, 0xec, 0x79, 0xe0,
     0xf5, 0xf2, 0x7f, 0x51, 0x60, 0xb3, 0x84, 0xc3, 0x78, 0x97, 0xb5, 0x2f,
     0x9d, 0x07, 0x90, 0xc9, 0xa0, 0x85, 0xcb, 0x61, 0xe7, 0xc1, 0x94, 0x01,
     0x26, 0x33, 0x53, 0x39, 0x83, 0x65, 0x91, 0x3f, 0x4c, 0x3c, 0x78, 0x51,
     0xc4, 0xe4, 0x8c, 0x8c, 0x7a, 0x99, 0x6a, 0x8e, 0x3e, 0x91, 0x12, 0xdd,
     0xdc, 0x47, 0x6d, 0x4e, 0x5b, 0x78, 0xb6, 0x14, 0x12, 0x5a, 0x79, 0xd9,
     0x1b, 0x85, 0x63, 0x7b, 0xb1, 0x38, 0xf0, 0x58, 0xc0, 0xf9, 0xa9, 0xf2,
     0x8e, 0xa5, 0x6d, 0x6d, 0xe0, 0xac, 0xb0, 0xb4, 0xf2, 0xe9, 0xe9, 0x42,
     0xf8, 0xa0, 0x0d, 0x2a, 0x72, 0x52, 0x4c, 0x1d, 0xb9, 0x04, 0x7f, 0xa3,
     0x9e, 0xe0, 0x2d, 0x08};

static tt_u8_t __rsa_sign_100[] = {
    0x7f, 0xfc, 0x0e, 0x14, 0xe8, 0x18, 0xec, 0xe2, 0x0a, 0xdb, 0x27, 0x25,
    0x9c, 0x47, 0x72, 0x5f, 0x5c, 0xad, 0x6b, 0x81, 0xa7, 0xf6, 0xff, 0x93,
    0xae, 0x63, 0x9c, 0x3f, 0x85, 0xd1, 0xa6, 0x93, 0xd7, 0xb3, 0x25, 0x89,
    0x20, 0xfe, 0xff, 0x29, 0x7b, 0x73, 0xdc, 0x84, 0x4e, 0xfb, 0x94, 0xeb,
    0xd4, 0xde, 0x71, 0x9a, 0x12, 0xe7, 0x18, 0xfa, 0x9c, 0x24, 0xc0, 0x52,
    0x51, 0x63, 0xc4, 0x2d, 0x41, 0xe7, 0x4c, 0x72, 0x44, 0x0b, 0x4d, 0x8c,
    0xd9, 0x00, 0xbb, 0x8c, 0x76, 0x2f, 0x99, 0x5c, 0x21, 0x2d, 0xc3, 0xed,
    0xa7, 0x45, 0xce, 0x3c, 0xd8, 0x9c, 0x15, 0x32, 0x21, 0x2a, 0x7f, 0xd1,
    0x58, 0x4f, 0x45, 0xd5, 0x1c, 0x1b, 0xb3, 0x2c, 0x0c, 0x72, 0xa3, 0x84,
    0x25, 0x6d, 0xbe, 0x77, 0x6f, 0xbb, 0x95, 0xd6, 0x1d, 0x92, 0xfe, 0xa2,
    0x41, 0x85, 0x0d, 0x09, 0x31, 0x93, 0x2b, 0x52, 0xe6, 0x9b, 0xf8, 0xa5,
    0x89, 0x31, 0x6a, 0x9f, 0x05, 0x68, 0x7d, 0x81, 0x8b, 0xc8, 0x84, 0x2b,
    0x79, 0xa5, 0x2e, 0x3e, 0x0a, 0xc4, 0xfa, 0x43, 0xff, 0x4d, 0x56, 0xed,
    0x83, 0x64, 0x2f, 0x95, 0xbe, 0xbd, 0xc8, 0xc3, 0x0a, 0x2d, 0x07, 0xb7,
    0x23, 0x5f, 0x63, 0x31, 0xb7, 0x58, 0xf5, 0x2e, 0xba, 0x41, 0xde, 0x3f,
    0x62, 0xd1, 0x1d, 0x2f, 0xaf, 0x96, 0x84, 0xab, 0xe3, 0x57, 0x28, 0x88,
    0x14, 0x35, 0xa9, 0x09, 0x54, 0xb7, 0x75, 0x30, 0xb0, 0xf6, 0xde, 0xe3,
    0x92, 0x13, 0x54, 0xc6, 0xa4, 0xa1, 0x85, 0x25, 0xe5, 0xcc, 0x95, 0x8c,
    0x10, 0x62, 0x0b, 0xe4, 0x2e, 0x9b, 0x8b, 0x39, 0xe8, 0xbc, 0x40, 0x83,
    0xec, 0x2b, 0xf9, 0xed, 0xfd, 0x77, 0x4f, 0x3a, 0x3f, 0x0d, 0x46, 0x54,
    0x19, 0x76, 0xd5, 0x8d, 0x4c, 0x10, 0x9c, 0x3f, 0x7c, 0x00, 0xfb, 0x03,
    0x62, 0x9a, 0x97, 0xdd,
};

static tt_u8_t __rsa_sign_245[] = {
    0x37, 0xe2, 0xe7, 0x1a, 0x0e, 0x35, 0x4a, 0xb8, 0x90, 0xfa, 0xc6, 0x5c,
    0x41, 0x45, 0xcd, 0x6d, 0x0c, 0xa0, 0x8b, 0x2e, 0xfc, 0x7a, 0x48, 0x32,
    0x99, 0x3e, 0x0f, 0xbf, 0x17, 0x87, 0x20, 0x2d, 0xe3, 0x18, 0xb3, 0x10,
    0x33, 0xde, 0x56, 0x01, 0xe0, 0x4f, 0xc6, 0x38, 0xa2, 0x93, 0x52, 0xed,
    0xcb, 0x7d, 0x32, 0x86, 0x7c, 0xd4, 0x3d, 0x02, 0x64, 0x22, 0x81, 0xf9,
    0x2d, 0x05, 0x89, 0x50, 0x08, 0x95, 0x07, 0x68, 0xcd, 0xfe, 0x88, 0xa5,
    0xf0, 0x9d, 0xce, 0x01, 0xe8, 0x04, 0xdb, 0x71, 0x5f, 0x97, 0xa8, 0x90,
    0x32, 0x7a, 0x0d, 0x65, 0x82, 0x8d, 0x07, 0xbf, 0xb0, 0x8f, 0x4a, 0x5e,
    0x80, 0xe1, 0x23, 0x2c, 0x87, 0x13, 0x9b, 0x93, 0xda, 0x8c, 0xc8, 0xa6,
    0xd1, 0xe5, 0xf9, 0x70, 0xcb, 0x74, 0x43, 0x8c, 0x07, 0xe1, 0x73, 0x9f,
    0xca, 0xcd, 0xf4, 0xe8, 0xac, 0xbf, 0x2d, 0x60, 0xae, 0xf8, 0x48, 0xff,
    0xa2, 0xe5, 0xec, 0xfc, 0x34, 0x27, 0xc8, 0x86, 0x25, 0x4e, 0x0b, 0xdb,
    0x73, 0x5d, 0x95, 0xe8, 0x6f, 0x2a, 0x94, 0xc1, 0x42, 0xd1, 0xc7, 0x5a,
    0xcd, 0xe8, 0x0d, 0x78, 0x85, 0x48, 0x74, 0x2b, 0xbb, 0xbf, 0x03, 0xf5,
    0xde, 0xb2, 0xa8, 0xb3, 0xbb, 0xe9, 0xcb, 0xd7, 0x80, 0xb3, 0x87, 0xfe,
    0xfe, 0xee, 0xfa, 0xe3, 0xb8, 0xd4, 0x69, 0xd7, 0xa8, 0xb2, 0x7d, 0x02,
    0x72, 0xa3, 0xad, 0x77, 0xe7, 0xd3, 0xbc, 0x81, 0x12, 0x79, 0x91, 0x31,
    0x15, 0x89, 0x1c, 0x34, 0xba, 0x84, 0xf0, 0xe2, 0x70, 0xb1, 0x40, 0x2b,
    0xb7, 0xf4, 0xf1, 0xe1, 0x50, 0x22, 0xbb, 0x4c, 0xf3, 0x2c, 0x74, 0x83,
    0xef, 0x02, 0x83, 0x3b, 0x4c, 0xec, 0x1e, 0x13, 0x51, 0x67, 0xc2, 0xe4,
    0xdd, 0xc9, 0x4b, 0xfe, 0x31, 0x53, 0x1b, 0xde, 0xe2, 0xf8, 0x5b, 0x4b,
    0xca, 0xdc, 0xac, 0xa4,
};

TT_TEST_ROUTINE_DEFINE(tt_unit_test_rsa_2048_sign_pkcs1)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_rsa_t pub, priv;
    tt_result_t ret;

    tt_buf_t output, output2;
    tt_u32_t n;

    tt_blob_t key_data;
    tt_rsa_attr_t attr;

    TT_TEST_CASE_ENTER()
    // test start

    tt_rsa_attr_default(&attr);
    attr.padding.mode = TT_RSA_PADDING_PKCS1;

    // rsa 2048 encrypt
    key_data.addr = (tt_u8_t *)__rsa_pub_2048_enc;
    key_data.len = (tt_u32_t)strlen(__rsa_pub_2048_enc);
    ret = tt_rsa_create(&pub,
                        TT_RSA_FORMAT_PKCS1,
                        TT_RSA_TYPE_PUBLIC,
                        &key_data,
                        &attr);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    tt_rsa_show(&pub);

    tt_buf_init(&output, NULL);
    tt_buf_init(&output2, NULL);

    // rsa 2048 decrypt
    key_data.addr = (tt_u8_t *)__rsa_priv_2048_pkcs8_enc;
    key_data.len = (tt_u32_t)strlen(__rsa_priv_2048_pkcs8_enc);
    attr.password = "abc123";
    ret = tt_rsa_create(&priv,
                        TT_RSA_FORMAT_PKCS8,
                        TT_RSA_TYPE_PRIVATE,
                        &key_data,
                        &attr);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    // buf

    // size: 1
    n = 1;
    tt_buf_reset_rwp(&output);
    ret = tt_rsa_sign_buf(&priv, (tt_u8_t *)__rsa_1024_in, n, &output);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(TT_BUF_RLEN(&output), 256, "");
    TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(&output), __rsa_sign_1, 256), 0, "");

    ret = tt_rsa_verify(&pub,
                        (tt_u8_t *)__rsa_1024_in,
                        n,
                        TT_BUF_RPOS(&output),
                        256);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    output.p[tt_rand_u32() % 256] += 1;
    ret = tt_rsa_verify(&pub,
                        (tt_u8_t *)__rsa_1024_in,
                        n,
                        TT_BUF_RPOS(&output),
                        256);
    TT_UT_EQUAL(ret, TT_FAIL, "");

    // size: 100
    n = 100;
    tt_buf_reset_rwp(&output);
    ret = tt_rsa_sign_buf(&priv, (tt_u8_t *)__rsa_1024_in, n, &output);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(TT_BUF_RLEN(&output), 256, "");
    TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(&output), __rsa_sign_100, 256), 0, "");

    ret = tt_rsa_verify(&pub,
                        (tt_u8_t *)__rsa_1024_in,
                        n,
                        TT_BUF_RPOS(&output),
                        256);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    output.p[tt_rand_u32() % 256] += 1;
    ret = tt_rsa_verify(&pub,
                        (tt_u8_t *)__rsa_1024_in,
                        n,
                        TT_BUF_RPOS(&output),
                        256);
    TT_UT_EQUAL(ret, TT_FAIL, "");

    // size: 245
    n = 245;
    tt_buf_reset_rwp(&output);
    ret = tt_rsa_sign_buf(&priv, (tt_u8_t *)__rsa_1024_in, n, &output);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(TT_BUF_RLEN(&output), 256, "");
    TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(&output), __rsa_sign_245, 256), 0, "");

    ret = tt_rsa_verify(&pub,
                        (tt_u8_t *)__rsa_1024_in,
                        n,
                        TT_BUF_RPOS(&output),
                        256);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    output.p[tt_rand_u32() % 256] += 1;
    ret = tt_rsa_verify(&pub,
                        (tt_u8_t *)__rsa_1024_in,
                        n,
                        TT_BUF_RPOS(&output),
                        256);
    TT_UT_EQUAL(ret, TT_FAIL, "");

    // size: random
    n = tt_rand_u32() % (sizeof(__rsa_1024_in) - 1) + 1;
    tt_buf_reset_rwp(&output);
    ret = tt_rsa_sign_buf(&priv, (tt_u8_t *)__rsa_1024_in, n, &output);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(TT_BUF_RLEN(&output), 256, "");

    ret = tt_rsa_verify(&pub,
                        (tt_u8_t *)__rsa_1024_in,
                        n,
                        TT_BUF_RPOS(&output),
                        256);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    output.p[tt_rand_u32() % 256] += 1;
    ret = tt_rsa_verify(&pub,
                        (tt_u8_t *)__rsa_1024_in,
                        n,
                        TT_BUF_RPOS(&output),
                        256);
    TT_UT_EQUAL(ret, TT_FAIL, "");

    // out
    tt_buf_destroy(&output);

    tt_rsa_destroy(&pub);
    tt_rsa_destroy(&priv);

    // test end
    TT_TEST_CASE_LEAVE()
}
