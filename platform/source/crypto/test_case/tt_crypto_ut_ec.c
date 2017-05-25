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

#define __EC_PUB_PK8_FILE "123xxxabc_ec_pub"
#define __EC_PRIV_PK8_FILE "123xxxabc_ec_priv"

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

static void __ec_prepare(void *);
static tt_bool_t has_ec;

extern tt_u8_t __ec_pub_der[];
extern tt_u8_t __ec_priv_der[];

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

// === routine declarations ================
TT_TEST_ROUTINE_DECLARE(tt_unit_test_crypto_ecdh)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_crypto_ecdsa)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(crypto_ecdh_case)

TT_TEST_CASE("tt_unit_test_crypto_ecdh",
             "crypto: ecdh",
             tt_unit_test_crypto_ecdh,
             NULL,
             __ec_prepare,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_crypto_ecdsa",
                 "crypto: ecdsa",
                 tt_unit_test_crypto_ecdsa,
                 NULL,
                 __ec_prepare,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(crypto_ecdh_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(CRYPTO_UT_EC, 0, crypto_ecdh_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(tt_unit_test_crypto_ecdh)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    static tt_u8_t __ec_pub[] = {
        0x04, 0x47, 0x27, 0xd7, 0x78, 0x07, 0x39, 0x5b, 0xb2, 0x07, 0xd0,
        0xd3, 0x3b, 0x89, 0xaf, 0x30, 0x3e, 0xdb, 0x88, 0x3c, 0xa3, 0x99,
        0x04, 0x8b, 0xb1, 0x38, 0xec, 0x66, 0x5c, 0xa5, 0x73, 0xe8, 0x17,
        0x3a, 0x97, 0x25, 0x76, 0xff, 0xbb, 0xcb, 0x05, 0x08, 0xea, 0x53,
        0x38, 0x59, 0x2f, 0x08, 0x42, 0x76, 0x2b, 0x36, 0xce, 0x47, 0x12,
        0x15, 0x75, 0xdd, 0x10, 0x90, 0xf6, 0xd0, 0xa4, 0xc3, 0xe1,
};

TT_TEST_ROUTINE_DEFINE(tt_unit_test_crypto_ecdh)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_pk_t priv, pub;
    tt_result_t ret;
    tt_ecdh_t ecdh, e2;
    tt_u8_t buf[100], pub1[100], pub2[100], s1[100], s2[100];
    tt_u32_t len, n1, n2, sn1, sn2;

    TT_TEST_CASE_ENTER()
    // test start

    tt_pk_init(&pub);
    tt_pk_init(&priv);

    ret = tt_pk_load_public_file(&pub, __EC_PUB_PK8_FILE);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_pk_get_type(&pub), TT_ECKEY, "");

    ret =
        tt_pk_load_private_file(&priv, __EC_PRIV_PK8_FILE, (tt_u8_t *)"123", 3);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_pk_get_type(&priv), TT_ECKEY, "");

    ret = tt_pk_check(&pub, &priv);
    TT_UT_SUCCESS(ret, "");

    // ecdh
    tt_ecdh_init(&ecdh);
    tt_ecdh_init(&e2);

    ret = tt_ecdh_load(&ecdh, &priv);
    TT_UT_SUCCESS(ret, "");

    // local pub
    n1 = sizeof(pub1);
    ret = tt_ecdh_get_pub(&ecdh, TT_TRUE, TT_FALSE, pub1, &n1);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(n1, sizeof(__ec_pub), "");
    TT_UT_EQUAL(tt_memcmp(pub1, __ec_pub, n1), 0, "");

    // remote pub
    len = sizeof(buf);
    ret = tt_ecdh_get_pub(&ecdh, TT_FALSE, TT_TRUE, buf, &len);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(len, 1, "");
    TT_UT_EQUAL(buf[0], 0, "");

    // generate
    ret = tt_ecdh_generate(&e2, TT_ECGRP_SECP256K1);
    TT_UT_SUCCESS(ret, "");

    // local pub
    n2 = sizeof(pub2);
    ret = tt_ecdh_get_pub(&e2, TT_TRUE, TT_FALSE, pub2, &n2);
    TT_UT_SUCCESS(ret, "");

    // remote pub
    len = sizeof(buf);
    ret = tt_ecdh_get_pub(&e2, TT_FALSE, TT_TRUE, buf, &len);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(len, 1, "");
    TT_UT_EQUAL(buf[0], 0, "");

    // exchange pub
    ret = tt_ecdh_set_pub(&ecdh, TT_FALSE, pub2, n2);
    TT_UT_SUCCESS(ret, "");
    ret = tt_ecdh_derive(&ecdh);
    TT_UT_SUCCESS(ret, "");
    sn1 = sizeof(s1);
    ret = tt_ecdh_get_secret(&ecdh, s1, 32);
    TT_UT_SUCCESS(ret, "");

    ret = tt_ecdh_set_pub(&e2, TT_FALSE, pub1, n1);
    TT_UT_SUCCESS(ret, "");
    ret = tt_ecdh_derive(&e2);
    TT_UT_SUCCESS(ret, "");
    sn2 = sizeof(s2);
    ret = tt_ecdh_get_secret(&e2, s2, 32);
    TT_UT_SUCCESS(ret, "");

    // compare shared secret
    TT_UT_EQUAL(tt_memcmp(s1, s2, 32), 0, "");
    tt_hex_dump(s1, 32, 8);

    {
        tt_u8_t ex1[100], ex2[100];

        // exchange private key
        n1 = sizeof(ex1);
        ret = tt_ecdh_get_pub(&ecdh, TT_FALSE, TT_FALSE, ex1, &n1);
        TT_UT_SUCCESS(ret, "");
        n2 = sizeof(ex2);
        ret = tt_ecdh_get_pub(&e2, TT_FALSE, TT_FALSE, ex2, &n2);
        TT_UT_SUCCESS(ret, "");

        ret = tt_ecdh_set_pub(&ecdh, TT_FALSE, ex2, n2);
        TT_UT_SUCCESS(ret, "");
        ret = tt_ecdh_derive(&ecdh);
        TT_UT_SUCCESS(ret, "");

        ret = tt_ecdh_set_pub(&e2, TT_FALSE, ex1, n1);
        TT_UT_SUCCESS(ret, "");
        ret = tt_ecdh_derive(&e2);
        TT_UT_SUCCESS(ret, "");

        n1 = sizeof(ex1);
        ret = tt_ecdh_get_secret(&ecdh, ex1, 32);
        TT_UT_SUCCESS(ret, "");
        n2 = sizeof(ex2);
        ret = tt_ecdh_get_secret(&ecdh, ex2, 32);
        TT_UT_SUCCESS(ret, "");

        TT_UT_EQUAL(tt_memcmp(ex1, ex2, 32), 0, "");
    }

    tt_ecdh_destroy(&ecdh);
    tt_ecdh_destroy(&e2);

    tt_pk_destroy(&pub);
    tt_pk_destroy(&priv);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_crypto_ecdsa)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_pk_t priv, pub;
    tt_result_t ret;
    tt_ecdsa_t ecdsa, e2;
    tt_u8_t buf[100], pub1[100], pub2[100], s1[100], s2[100];
    tt_u32_t len, n1, n2, sn1, sn2;

    TT_TEST_CASE_ENTER()
    // test start

    tt_pk_init(&pub);
    tt_pk_init(&priv);

    ret = tt_pk_load_public_file(&pub, __EC_PUB_PK8_FILE);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_pk_get_type(&pub), TT_ECKEY, "");

    ret =
        tt_pk_load_private_file(&priv, __EC_PRIV_PK8_FILE, (tt_u8_t *)"123", 3);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_pk_get_type(&priv), TT_ECKEY, "");

    ret = tt_pk_check(&pub, &priv);
    TT_UT_SUCCESS(ret, "");

    // ecdsa
    tt_ecdsa_init(&ecdsa);
    tt_ecdsa_init(&e2);

    ret = tt_ecdsa_load(&ecdsa, &priv);
    TT_UT_SUCCESS(ret, "");
    ret = tt_ecdsa_load(&e2, &pub);
    TT_UT_SUCCESS(ret, "");

    sn1 = sizeof(s1);
    ret = tt_ecdsa_sign(&ecdsa, (tt_u8_t *)"", 0, TT_SHA224, TT_SHA1, s1, &sn1);
    TT_UT_SUCCESS(ret, "");
    tt_hex_dump(s1, sn1, 8);
    ret = tt_ecdsa_verify(&e2, (tt_u8_t *)"", 0, TT_SHA224, s1, sn1);
    TT_UT_SUCCESS(ret, "");

    ret = tt_ecdsa_verify(&e2, (tt_u8_t *)"", 0, TT_SHA1, s1, sn1);
    TT_UT_FAIL(ret, "");
    ret = tt_ecdsa_verify(&e2, (tt_u8_t *)"", 0, TT_SHA224, s1, sn1 - 1);
    TT_UT_FAIL(ret, "");
    s1[sn1 - 1]++;
    ret = tt_ecdsa_verify(&e2, (tt_u8_t *)"", 0, TT_SHA224, s1, sn1);
    TT_UT_FAIL(ret, "");

    tt_ecdsa_destroy(&ecdsa);
    tt_ecdsa_destroy(&e2);

    tt_pk_destroy(&pub);
    tt_pk_destroy(&priv);

    // generate ecdsa
    ret = tt_ecdsa_generate(&ecdsa, TT_ECGRP_SECP256K1);
    TT_UT_SUCCESS(ret, "");

    sn1 = sizeof(s1);
    ret = tt_ecdsa_sign(&ecdsa, (tt_u8_t *)"123", 3, TT_SHA512, TT_MD5, s1, &sn1);
    TT_UT_SUCCESS(ret, "");
    tt_hex_dump(s1, sn1, 8);
    ret = tt_ecdsa_verify(&ecdsa, (tt_u8_t *)"123", 3, TT_SHA512, s1, sn1);
    TT_UT_SUCCESS(ret, "");

    ret = tt_ecdsa_verify(&e2, (tt_u8_t *)"", 0, TT_SHA512, s1, sn1);
    TT_UT_FAIL(ret, "");
    ret = tt_ecdsa_verify(&e2, (tt_u8_t *)"", 0, TT_SHA1, s1, sn1 - 1);
    TT_UT_FAIL(ret, "");
    s1[sn1 - 1]++;
    ret = tt_ecdsa_verify(&e2, (tt_u8_t *)"", 0, TT_SHA512, s1, sn1);
    TT_UT_FAIL(ret, "");

    tt_ecdsa_destroy(&ecdsa);

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_char_t __ec_priv_pk8[] =
    "-----BEGIN ENCRYPTED PRIVATE KEY-----\n"
    "MIHNMEAGCSqGSIb3DQEFDTAzMBsGCSqGSIb3DQEFDDAOBAiQS7lkWX5DmgICCAAw\n"
    "FAYIKoZIhvcNAwcECAbxIkK5f0E4BIGI8iL+Yi3aSPx3Mdj1+qZhu7BVFkyf0wtp\n"
    "JptxRqC7ILTVKSmDGjHHoKY8blHCQ48afysnEpd3IbTslHyoVLyzma1bMYSlUOAT\n"
    "fWjcziBZokgNL2XYU8fs1KwzU8Iy52SIcz7cw4U0jec1DF2QZW/FgP8TTM7lNhWT\n"
    "hMaPLnUai9lBrgpMuVziTQ==\n"
    "-----END ENCRYPTED PRIVATE KEY-----";

static tt_char_t __ec_pub_pk8[] =
    "-----BEGIN PUBLIC KEY-----\n"
    "MFYwEAYHKoZIzj0CAQYFK4EEAAoDQgAERyfXeAc5W7IH0NM7ia8wPtuIPKOZBIux\n"
    "OOxmXKVz6Bc6lyV2/7vLBQjqUzhZLwhCdis2zkcSFXXdEJD20KTD4Q==\n"
    "-----END PUBLIC KEY-----\n";

void __ec_prepare(void *p)
{
    tt_file_t f;

    if (has_ec) {
        return;
    }

    // gen rsa pub
    if (!TT_OK(tt_fopen(&f,
                        __EC_PUB_PK8_FILE,
                        TT_FO_WRITE | TT_FO_CREAT | TT_FO_TRUNC,
                        NULL))) {
        TT_ERROR("fail to open %s", __EC_PUB_PK8_FILE);
        return;
    }
    if (!TT_OK(tt_fwrite(&f,
                         (tt_u8_t *)__ec_pub_pk8,
                         sizeof(__ec_pub_pk8) - 1,
                         NULL))) {
        TT_ERROR("fail to write %s", __EC_PUB_PK8_FILE);
        return;
    }
    tt_fclose(&f);

    // gen rsa priv
    if (!TT_OK(tt_fopen(&f,
                        __EC_PRIV_PK8_FILE,
                        TT_FO_WRITE | TT_FO_CREAT | TT_FO_TRUNC,
                        NULL))) {
        TT_ERROR("fail to open %s", __EC_PRIV_PK8_FILE);
        return;
    }
    if (!TT_OK(tt_fwrite(&f,
                         (tt_u8_t *)__ec_priv_pk8,
                         sizeof(__ec_priv_pk8) - 1,
                         NULL))) {
        TT_ERROR("fail to write %s", __EC_PRIV_PK8_FILE);
        return;
    }
    tt_fclose(&f);

    has_ec = TT_TRUE;
}
