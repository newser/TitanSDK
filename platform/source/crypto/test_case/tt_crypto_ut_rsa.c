/* Copyright (C) 2017 haniu (niuhao.cn@gmail.com)
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
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

#if TT_ENV_OS_IS_IOS

#if (TT_ENV_OS_FEATURE & TT_ENV_OS_FEATURE_IOS_SIMULATOR)
#define __PUB_PK8_FILE "/tmp/123xxxabc_rsa_pub"
#define __PRIV_PK8_FILE "/tmp/123xxxabc_rsa_priv"
#else
static tt_string_t pub_path, priv_path;
#define __PUB_PK8_FILE tt_string_cstr(&pub_path)
#define __PRIV_PK8_FILE tt_string_cstr(&priv_path)
#endif

#elif TT_ENV_OS_IS_ANDROID
#define __PUB_PK8_FILE                                                         \
    "/data/data/com.titansdk.titansdkunittest/123xxxabc_rsa_pub"
#define __PRIV_PK8_FILE                                                        \
    "/data/data/com.titansdk.titansdkunittest/123xxxabc_rsa_priv"
#else
#define __PUB_PK8_FILE "123xxxabc_rsa_pub"
#define __PRIV_PK8_FILE "123xxxabc_rsa_priv"
#endif

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

static void __rsa_prepare(void *);
static tt_bool_t has_keyfile;

extern tt_u8_t __rsa_pub_der[];
extern tt_u8_t __rsa_priv_der[];

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

// === routine declarations ================
TT_TEST_ROUTINE_DECLARE(case_rsa_encrypt)
TT_TEST_ROUTINE_DECLARE(case_rsa_sign)
TT_TEST_ROUTINE_DECLARE(case_rsa_gen)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(crypto_rsa_case)

TT_TEST_CASE("case_rsa_encrypt",
             "rsa encrypt and decrypt",
             case_rsa_encrypt,
             NULL,
             __rsa_prepare,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("case_rsa_sign",
                 "rsa sign and verify",
                 case_rsa_sign,
                 NULL,
                 __rsa_prepare,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_rsa_gen",
                 "rsa generated",
                 case_rsa_gen,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(crypto_rsa_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(CRYPTO_UT_RSA, 0, crypto_rsa_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(case_rsa_encrypt)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(case_rsa_encrypt)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_pk_t pub, priv;
    tt_rsa_t rpub, rpriv;
    tt_result_t ret;
    tt_u8_t ibuf[256], ebuf[256], dbuf[256];
    tt_u32_t len;

    TT_TEST_CASE_ENTER()
    // test start

    tt_pk_init(&pub);
    tt_pk_init(&priv);

    ret = tt_pk_load_public_file(&pub, __PUB_PK8_FILE);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_pk_get_type(&pub), TT_RSA, "");

    ret =
        tt_pk_load_private_file(&priv, __PRIV_PK8_FILE, (tt_u8_t *)"hahaha", 6);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_pk_get_type(&priv), TT_RSA, "");

    ret = tt_pk_check(&pub, &priv);
    TT_UT_SUCCESS(ret, "");

    tt_rsa_init(&rpub);
    tt_rsa_init(&rpriv);

    ret = tt_rsa_load(&rpub, &pub);
    TT_UT_SUCCESS(ret, "");
    ret = tt_rsa_load(&rpriv, &priv);
    TT_UT_SUCCESS(ret, "");

    // pkcs1
    tt_memset(ibuf, 6, 256);
    ret = tt_rsa_encrypt_pkcs1(&rpub, ibuf, 200, ebuf);
    TT_UT_SUCCESS(ret, "");
    len = 256;
    ret = tt_rsa_decrypt_pkcs1(&rpriv, ebuf, dbuf, &len);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(len, 200, "");
    TT_UT_EQUAL(tt_memcmp(dbuf, ibuf, len), 0, "");

    tt_memset(ibuf, 3, 256);
    ret = tt_rsa_encrypt_pkcs1(&rpub, ibuf, 1, ebuf);
    TT_UT_SUCCESS(ret, "");
    len = 256;
    ret = tt_rsa_decrypt_pkcs1(&rpriv, ebuf, dbuf, &len);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(len, 1, "");
    TT_UT_EQUAL(tt_memcmp(dbuf, ibuf, len), 0, "");

    // oaep
    tt_memset(ibuf, 0, 256);
    ret = tt_rsa_encrypt_oaep(&rpub,
                              ibuf,
                              100,
                              (tt_u8_t *)"1234",
                              4,
                              TT_SHA256,
                              ebuf);
    TT_UT_SUCCESS(ret, "");
    len = 256;
    ret = tt_rsa_decrypt_oaep(&rpriv,
                              ebuf,
                              (tt_u8_t *)"1234",
                              4,
                              TT_SHA256,
                              dbuf,
                              &len);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(len, 100, "");
    TT_UT_EQUAL(tt_memcmp(dbuf, ibuf, len), 0, "");

    tt_memset(ibuf, 0xf, 256);
    ret = tt_rsa_encrypt_oaep(&rpub,
                              ibuf,
                              128,
                              (tt_u8_t *)"",
                              0,
                              TT_RIPEMD160,
                              ebuf);
    TT_UT_SUCCESS(ret, "");
    len = 256;
    ret = tt_rsa_decrypt_oaep(&rpriv,
                              ebuf,
                              (tt_u8_t *)"",
                              0,
                              TT_RIPEMD160,
                              dbuf,
                              &len);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(len, 128, "");
    TT_UT_EQUAL(tt_memcmp(dbuf, ibuf, len), 0, "");

    tt_rsa_destroy(&rpub);
    tt_rsa_destroy(&rpriv);

    tt_pk_destroy(&pub);
    tt_pk_destroy(&priv);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_rsa_sign)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_pk_t pub, priv;
    tt_rsa_t rpub, rpriv;
    tt_result_t ret;
    tt_u8_t sig[256];

    TT_TEST_CASE_ENTER()
    // test start

    tt_pk_init(&pub);
    tt_pk_init(&priv);

    ret = tt_pk_load_public(&pub, __rsa_pub_der, 294);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_pk_get_type(&pub), TT_RSA, "");

    ret = tt_pk_load_private(&priv, __rsa_priv_der, 1192, NULL, 0);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_pk_get_type(&priv), TT_RSA, "");

    ret = tt_pk_check(&pub, &priv);
    TT_UT_SUCCESS(ret, "");

    tt_rsa_init(&rpub);
    tt_rsa_init(&rpriv);

    ret = tt_rsa_load(&rpub, &pub);
    TT_UT_SUCCESS(ret, "");
    ret = tt_rsa_load(&rpriv, &priv);
    TT_UT_SUCCESS(ret, "");

    // pkcs1 sign
    ret = tt_rsa_sign_pkcs1(&rpriv, (tt_u8_t *)"1234", 4, TT_SHA512, sig);
    TT_UT_SUCCESS(ret, "");
    ret = tt_rsa_verify_pkcs1(&rpub, (tt_u8_t *)"1234", 4, TT_SHA512, sig);
    TT_UT_SUCCESS(ret, "");
    ret = tt_rsa_verify_pkcs1(&rpub, (tt_u8_t *)"1234", 4, TT_SHA512, sig);
    TT_UT_SUCCESS(ret, "");

    ret = tt_rsa_verify_pkcs1(&rpub, (tt_u8_t *)"1234", 4, TT_SHA256, sig);
    TT_UT_FAIL(ret, "");

    ret = tt_rsa_verify_pkcs1(&rpub, (tt_u8_t *)"1234", 3, TT_SHA512, sig);
    TT_UT_FAIL(ret, "");

    sig[0]++;
    ret = tt_rsa_verify_pkcs1(&rpub, (tt_u8_t *)"1234", 4, TT_SHA512, sig);
    TT_UT_FAIL(ret, "");

    // pss sign
    ret = tt_rsa_sign_pss(&rpriv, (tt_u8_t *)"", 0, TT_MD2, sig);
    TT_UT_SUCCESS(ret, "");
    ret = tt_rsa_verify_pss(&rpub, (tt_u8_t *)"", 0, TT_MD2, sig);
    TT_UT_SUCCESS(ret, "");
    ret = tt_rsa_verify_pss(&rpub, (tt_u8_t *)"", 0, TT_MD2, sig);
    TT_UT_SUCCESS(ret, "");

    ret = tt_rsa_verify_pss(&rpub, (tt_u8_t *)"1", 1, TT_MD2, sig);
    TT_UT_FAIL(ret, "");

    ret = tt_rsa_verify_pss(&rpub, (tt_u8_t *)"", 0, TT_MD5, sig);
    TT_UT_FAIL(ret, "");

    sig[0]++;
    ret = tt_rsa_verify_pss(&rpub, (tt_u8_t *)"", 0, TT_MD2, sig);
    TT_UT_FAIL(ret, "");

    tt_rsa_destroy(&rpub);
    tt_rsa_destroy(&rpriv);

    tt_pk_destroy(&pub);
    tt_pk_destroy(&priv);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_rsa_gen)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_rsa_t rpub, rpriv;
    tt_result_t ret;
    tt_u8_t ibuf[256], ebuf[256], dbuf[256], sig[256];
    tt_u32_t len;

    TT_TEST_CASE_ENTER()
// test start

#ifdef __UT_LITE__
    return TT_SUCCESS;
#endif

    tt_rsa_init(&rpub);
    tt_rsa_init(&rpriv);

    ret = tt_rsa_generate(&rpriv, 1024, 0);
    TT_UT_FAIL(ret, "");

    ret = tt_rsa_generate(&rpriv, 1024, 3);
    TT_UT_SUCCESS(ret, "");
    ret = tt_rsa_topub(&rpriv, &rpub);
    TT_UT_SUCCESS(ret, "");

    ret = tt_rsa_check(&rpub, &rpriv);
    TT_UT_SUCCESS(ret, "");

    // pkcs1
    tt_memset(ibuf, 3, 256);
    ret = tt_rsa_encrypt_pkcs1(&rpub, ibuf, 1, ebuf);
    TT_UT_SUCCESS(ret, "");
    len = 256;
    ret = tt_rsa_decrypt_pkcs1(&rpriv, ebuf, dbuf, &len);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(len, 1, "");
    TT_UT_EQUAL(tt_memcmp(dbuf, ibuf, len), 0, "");

    // oaep
    tt_memset(ibuf, 0xf, 256);
    ret = tt_rsa_encrypt_oaep(&rpub,
                              ibuf,
                              21,
                              (tt_u8_t *)"",
                              0,
                              TT_RIPEMD160,
                              ebuf);
    TT_UT_SUCCESS(ret, "");
    len = 256;
    ret = tt_rsa_decrypt_oaep(&rpriv,
                              ebuf,
                              (tt_u8_t *)"",
                              0,
                              TT_RIPEMD160,
                              dbuf,
                              &len);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(len, 21, "");
    TT_UT_EQUAL(tt_memcmp(dbuf, ibuf, len), 0, "");

    // pkcs1 sign
    ret = tt_rsa_sign_pkcs1(&rpriv, (tt_u8_t *)"1234", 4, TT_SHA512, sig);
    TT_UT_SUCCESS(ret, "");
    ret = tt_rsa_verify_pkcs1(&rpub, (tt_u8_t *)"1234", 4, TT_SHA512, sig);
    TT_UT_SUCCESS(ret, "");

    // pss sign
    ret = tt_rsa_sign_pss(&rpriv, (tt_u8_t *)"1234", 4, TT_SHA224, sig);
    TT_UT_SUCCESS(ret, "");
    ret = tt_rsa_verify_pss(&rpub, (tt_u8_t *)"1234", 4, TT_SHA224, sig);
    TT_UT_SUCCESS(ret, "");

    //////////////////////////
    // again
    //////////////////////////

    // pkcs1
    tt_memset(ibuf, 3, 256);
    ret = tt_rsa_encrypt_pkcs1(&rpub, ibuf, 1, ebuf);
    TT_UT_SUCCESS(ret, "");
    len = 256;
    ret = tt_rsa_decrypt_pkcs1(&rpriv, ebuf, dbuf, &len);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(len, 1, "");
    TT_UT_EQUAL(tt_memcmp(dbuf, ibuf, len), 0, "");

    // oaep
    tt_memset(ibuf, 0xf, 256);
    ret = tt_rsa_encrypt_oaep(&rpub,
                              ibuf,
                              21,
                              (tt_u8_t *)"",
                              0,
                              TT_RIPEMD160,
                              ebuf);
    TT_UT_SUCCESS(ret, "");
    len = 256;
    ret = tt_rsa_decrypt_oaep(&rpriv,
                              ebuf,
                              (tt_u8_t *)"",
                              0,
                              TT_RIPEMD160,
                              dbuf,
                              &len);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(len, 21, "");
    TT_UT_EQUAL(tt_memcmp(dbuf, ibuf, len), 0, "");

    // pkcs1 sign
    ret = tt_rsa_sign_pkcs1(&rpriv, (tt_u8_t *)"1234", 4, TT_SHA512, sig);
    TT_UT_SUCCESS(ret, "");
    ret = tt_rsa_verify_pkcs1(&rpub, (tt_u8_t *)"1234", 4, TT_SHA512, sig);
    TT_UT_SUCCESS(ret, "");

    // pss sign
    ret = tt_rsa_sign_pss(&rpriv, (tt_u8_t *)"1234", 4, TT_SHA224, sig);
    TT_UT_SUCCESS(ret, "");
    ret = tt_rsa_verify_pss(&rpub, (tt_u8_t *)"1234", 4, TT_SHA224, sig);
    TT_UT_SUCCESS(ret, "");

    tt_rsa_destroy(&rpub);
    tt_rsa_destroy(&rpriv);

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_char_t __rsa_priv_pk8[] =
    "-----BEGIN ENCRYPTED PRIVATE KEY-----\n"
    "MIIFDjBABgkqhkiG9w0BBQ0wMzAbBgkqhkiG9w0BBQwwDgQIZEv/aHAw9L8CAggA\n"
    "MBQGCCqGSIb3DQMHBAhUyK2tWS1FKQSCBMjEDMDKX2nXY6kHV6FI4RSGS3du1P3t\n"
    "MjetNAB/CjkiAp0n8qwBRXULdTitZ+TUxVuLscpgeGwZSyjOS+wtCvxQTYhQOYX+\n"
    "Hvm8jI6fIZXXJruWgCgUyWVJ1JXtTk75p+fx/5NqQEnc39L7vROdA+qYL//4bTLp\n"
    "O03+jY2N0pnFDHVa6U+Hn7yH2iUlP5EOrwAPnVtdP35dWefM0dFwf2IUBm8XBaFq\n"
    "NZx/lP9L64MTctk7vR326Kf9fHDIIA0FaEBqZlRL82trK9PVrIf9DAYCXvn78NOF\n"
    "l403jd2D4HVFhL78h7crdRUCCgdFJVGlGLuKyiVN/NnN5ez1sOSHT1cQBRguLrW2\n"
    "rTXXTsm6VWfFJekD2l3nDkS4mI6xRu2TqD2ECKe+XJjsReI3r6aIT73GVIsi9Lra\n"
    "qDumGmlj467M3YGj3QjgoV+1GCMRSVDmwcLhjt0p5t4L0Qs4KfrEeeJd8Dgsrxz9\n"
    "OM6xokz4TIQp4tCoc2YRh/lq0GVWzyaKSFDD3z7l5fyOfrNIaz6xI0SdtnNjC00d\n"
    "ecuZZi+7E0V1PWaQtPoi/VMOWb0NCHrd3g8Lwn2RHGESjJBFPDeY0km7Celfbm5K\n"
    "LPvrNU193lg0HCNFq/69jwm0kICJVSia80UREbvJLHtZ/WkJ+6oX5KPpYe4a9/Qt\n"
    "u0cPdUNMZ/AZylyD+xC09MNfgzaxN6qOoiHpnGhThJbw5H5Nq6FTS7tzlqydHQ+L\n"
    "f8rH+Llr5PMFJOAwPjHzMMpBjhl27UL4aTBnq8vwSICri2d8tUHqozpIfRguG1wX\n"
    "caJYKxxr90cBNzSv6PQwYuBSe3Mvd3LHBHFm+WL1OM2i0AHsKCBnb0oCaxbSA1zV\n"
    "ZXnZAxHFmeLA21UQD3MnFoGpCG2CS2jPF6I2GmvDmfW2aKebJ68+tuFsIXQ0+fQ+\n"
    "0GFBI05jd3PaG/Kb5iF3zwYdlJ7+Ta/eBs+ejgudMBglbjNkZdTox0mmk41tIxDl\n"
    "S11cvgiWG4B7sqrrOJCBRoK8ijj9T76HWeh8/ghtzMpw3npSfwq+t2lhrbenwPua\n"
    "AIiM6KQ2bkemrNQW3xYVl+HeuYOaqxH/ln0s1TEd8XmvwUbxux8vwi1OMrnm2YQ3\n"
    "AFZRjiv/76X+fXPE8rPUbFi3bUlrhnmBaNf5rt8cQCPfoBnHRRNjE9FBmWe/X9fd\n"
    "tDyM4y/jeITjjcudFsqLXsQg2YpeThU6PDzvp738jQ0qUAvbJXqrod1xiImtWR2y\n"
    "6VYYOmnyEra/PZkdMmRHaf7bpXaade8DLp7N7URId9JUwZMR0W9PasTzETd6tZw2\n"
    "8i1yp3RgCZW3BxCCwdpNkbqQrJO9Aj3xjmyNZblwjIGQ+2z1FXAD8FbGkz+QOjPF\n"
    "OssXqXWMDzl0Sa5A6rC0aJlKAgaFydDejgXaMMUU3q8jmHbkw/CRuMSG5DKR/Rmh\n"
    "icfuTfIIXGXfqbzcsD1Fyf7QXPpDQylrGDHseOGq391Hrs8JQkGdpNxUP00PhmSt\n"
    "3PvCqhDhZ6CEbgfsEiP3UhAVzUKckXavdm7q5EXJZ+kabdeJJS94U291rLzDqBGS\n"
    "R8lUXwGbpURCyc0dfAcNMtOKSuMTp57lMIUjsUC+q9TC483buLtGXlLVhvgLAj8m\n"
    "+gg=\n"
    "-----END ENCRYPTED PRIVATE KEY-----\n";

static tt_char_t __rsa_pub_pk8[] =
    "-----BEGIN PUBLIC KEY-----\n"
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuKnySxKiKKd5gQmEpn4e\n"
    "kSfUrNbA0L9pRI1G6Xo74FR8wh2RLM551lhBN61BII11zf0mx9tYp9ek5aBWo0ik\n"
    "8YLJHYCLyUJGMF49DXq6xVyfFtenzPA8lMH/+Z42bQWp6ppV91G2dhTvBa72b7vD\n"
    "Ipenmmlg32DOICqgg7S7Xf0NPbcG5izJrqQJnzLdsLT9mxM6iuYcgSrc9lOUmnz+\n"
    "TnN1+wdOaQH+wA9JPxLg6PW9BwMz19DstLr/B8OjAWJyW4zecbEfNxkGru+RTpGo\n"
    "Y+jshDm/PHia0cPytv+6dOeQHcy2UslT3ACbd/YccU80/ONzs8LAfAq3gKgFQps+\n"
    "UQIDAQAB\n"
    "-----END PUBLIC KEY-----\n";

void __rsa_prepare(void *p)
{
    tt_file_t f;

    if (has_keyfile) {
        return;
    }

#if TT_ENV_OS_IS_IOS && !(TT_ENV_OS_FEATURE & TT_ENV_OS_FEATURE_IOS_SIMULATOR)
    {
        char *s;

        tt_string_init(&pub_path, NULL);
        tt_string_init(&priv_path, NULL);

        s = getenv("HOME");
        if (s != NULL) {
            tt_string_append(&pub_path, s);
            tt_string_append(&pub_path, "/Library/Caches/123xxxabc_rsa_pub");

            tt_string_append(&priv_path, s);
            tt_string_append(&priv_path, "/Library/Caches/123xxxabc_rsa_priv");
        }
    }
#endif

    // gen rsa pub
    if (!TT_OK(tt_fopen(&f,
                        __PUB_PK8_FILE,
                        TT_FO_WRITE | TT_FO_CREAT | TT_FO_TRUNC,
                        NULL))) {
        TT_ERROR("fail to open %s", __PUB_PK8_FILE);
        return;
    }
    if (!TT_OK(tt_fwrite(&f,
                         (tt_u8_t *)__rsa_pub_pk8,
                         sizeof(__rsa_pub_pk8) - 1,
                         NULL))) {
        TT_ERROR("fail to write %s", __PUB_PK8_FILE);
        return;
    }
    tt_fclose(&f);

    // gen rsa priv
    if (!TT_OK(tt_fopen(&f,
                        __PRIV_PK8_FILE,
                        TT_FO_WRITE | TT_FO_CREAT | TT_FO_TRUNC,
                        NULL))) {
        TT_ERROR("fail to open %s", __PRIV_PK8_FILE);
        return;
    }
    if (!TT_OK(tt_fwrite(&f,
                         (tt_u8_t *)__rsa_priv_pk8,
                         sizeof(__rsa_priv_pk8) - 1,
                         NULL))) {
        TT_ERROR("fail to write %s", __PRIV_PK8_FILE);
        return;
    }
    tt_fclose(&f);

    has_keyfile = TT_TRUE;
}

tt_u8_t __rsa_pub_der[294] =
    {0x30, 0x82, 0x01, 0x22, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86,
     0xf7, 0x0d, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x82, 0x01, 0x0f, 0x00,
     0x30, 0x82, 0x01, 0x0a, 0x02, 0x82, 0x01, 0x01, 0x00, 0xb8, 0xa9, 0xf2,
     0x4b, 0x12, 0xa2, 0x28, 0xa7, 0x79, 0x81, 0x09, 0x84, 0xa6, 0x7e, 0x1e,
     0x91, 0x27, 0xd4, 0xac, 0xd6, 0xc0, 0xd0, 0xbf, 0x69, 0x44, 0x8d, 0x46,
     0xe9, 0x7a, 0x3b, 0xe0, 0x54, 0x7c, 0xc2, 0x1d, 0x91, 0x2c, 0xce, 0x79,
     0xd6, 0x58, 0x41, 0x37, 0xad, 0x41, 0x20, 0x8d, 0x75, 0xcd, 0xfd, 0x26,
     0xc7, 0xdb, 0x58, 0xa7, 0xd7, 0xa4, 0xe5, 0xa0, 0x56, 0xa3, 0x48, 0xa4,
     0xf1, 0x82, 0xc9, 0x1d, 0x80, 0x8b, 0xc9, 0x42, 0x46, 0x30, 0x5e, 0x3d,
     0x0d, 0x7a, 0xba, 0xc5, 0x5c, 0x9f, 0x16, 0xd7, 0xa7, 0xcc, 0xf0, 0x3c,
     0x94, 0xc1, 0xff, 0xf9, 0x9e, 0x36, 0x6d, 0x05, 0xa9, 0xea, 0x9a, 0x55,
     0xf7, 0x51, 0xb6, 0x76, 0x14, 0xef, 0x05, 0xae, 0xf6, 0x6f, 0xbb, 0xc3,
     0x22, 0x97, 0xa7, 0x9a, 0x69, 0x60, 0xdf, 0x60, 0xce, 0x20, 0x2a, 0xa0,
     0x83, 0xb4, 0xbb, 0x5d, 0xfd, 0x0d, 0x3d, 0xb7, 0x06, 0xe6, 0x2c, 0xc9,
     0xae, 0xa4, 0x09, 0x9f, 0x32, 0xdd, 0xb0, 0xb4, 0xfd, 0x9b, 0x13, 0x3a,
     0x8a, 0xe6, 0x1c, 0x81, 0x2a, 0xdc, 0xf6, 0x53, 0x94, 0x9a, 0x7c, 0xfe,
     0x4e, 0x73, 0x75, 0xfb, 0x07, 0x4e, 0x69, 0x01, 0xfe, 0xc0, 0x0f, 0x49,
     0x3f, 0x12, 0xe0, 0xe8, 0xf5, 0xbd, 0x07, 0x03, 0x33, 0xd7, 0xd0, 0xec,
     0xb4, 0xba, 0xff, 0x07, 0xc3, 0xa3, 0x01, 0x62, 0x72, 0x5b, 0x8c, 0xde,
     0x71, 0xb1, 0x1f, 0x37, 0x19, 0x06, 0xae, 0xef, 0x91, 0x4e, 0x91, 0xa8,
     0x63, 0xe8, 0xec, 0x84, 0x39, 0xbf, 0x3c, 0x78, 0x9a, 0xd1, 0xc3, 0xf2,
     0xb6, 0xff, 0xba, 0x74, 0xe7, 0x90, 0x1d, 0xcc, 0xb6, 0x52, 0xc9, 0x53,
     0xdc, 0x00, 0x9b, 0x77, 0xf6, 0x1c, 0x71, 0x4f, 0x34, 0xfc, 0xe3, 0x73,
     0xb3, 0xc2, 0xc0, 0x7c, 0x0a, 0xb7, 0x80, 0xa8, 0x05, 0x42, 0x9b, 0x3e,
     0x51, 0x02, 0x03, 0x01, 0x00, 0x01};

tt_u8_t __rsa_priv_der[1192] =
    {0x30, 0x82, 0x04, 0xa4, 0x02, 0x01, 0x00, 0x02, 0x82, 0x01, 0x01, 0x00,
     0xb8, 0xa9, 0xf2, 0x4b, 0x12, 0xa2, 0x28, 0xa7, 0x79, 0x81, 0x09, 0x84,
     0xa6, 0x7e, 0x1e, 0x91, 0x27, 0xd4, 0xac, 0xd6, 0xc0, 0xd0, 0xbf, 0x69,
     0x44, 0x8d, 0x46, 0xe9, 0x7a, 0x3b, 0xe0, 0x54, 0x7c, 0xc2, 0x1d, 0x91,
     0x2c, 0xce, 0x79, 0xd6, 0x58, 0x41, 0x37, 0xad, 0x41, 0x20, 0x8d, 0x75,
     0xcd, 0xfd, 0x26, 0xc7, 0xdb, 0x58, 0xa7, 0xd7, 0xa4, 0xe5, 0xa0, 0x56,
     0xa3, 0x48, 0xa4, 0xf1, 0x82, 0xc9, 0x1d, 0x80, 0x8b, 0xc9, 0x42, 0x46,
     0x30, 0x5e, 0x3d, 0x0d, 0x7a, 0xba, 0xc5, 0x5c, 0x9f, 0x16, 0xd7, 0xa7,
     0xcc, 0xf0, 0x3c, 0x94, 0xc1, 0xff, 0xf9, 0x9e, 0x36, 0x6d, 0x05, 0xa9,
     0xea, 0x9a, 0x55, 0xf7, 0x51, 0xb6, 0x76, 0x14, 0xef, 0x05, 0xae, 0xf6,
     0x6f, 0xbb, 0xc3, 0x22, 0x97, 0xa7, 0x9a, 0x69, 0x60, 0xdf, 0x60, 0xce,
     0x20, 0x2a, 0xa0, 0x83, 0xb4, 0xbb, 0x5d, 0xfd, 0x0d, 0x3d, 0xb7, 0x06,
     0xe6, 0x2c, 0xc9, 0xae, 0xa4, 0x09, 0x9f, 0x32, 0xdd, 0xb0, 0xb4, 0xfd,
     0x9b, 0x13, 0x3a, 0x8a, 0xe6, 0x1c, 0x81, 0x2a, 0xdc, 0xf6, 0x53, 0x94,
     0x9a, 0x7c, 0xfe, 0x4e, 0x73, 0x75, 0xfb, 0x07, 0x4e, 0x69, 0x01, 0xfe,
     0xc0, 0x0f, 0x49, 0x3f, 0x12, 0xe0, 0xe8, 0xf5, 0xbd, 0x07, 0x03, 0x33,
     0xd7, 0xd0, 0xec, 0xb4, 0xba, 0xff, 0x07, 0xc3, 0xa3, 0x01, 0x62, 0x72,
     0x5b, 0x8c, 0xde, 0x71, 0xb1, 0x1f, 0x37, 0x19, 0x06, 0xae, 0xef, 0x91,
     0x4e, 0x91, 0xa8, 0x63, 0xe8, 0xec, 0x84, 0x39, 0xbf, 0x3c, 0x78, 0x9a,
     0xd1, 0xc3, 0xf2, 0xb6, 0xff, 0xba, 0x74, 0xe7, 0x90, 0x1d, 0xcc, 0xb6,
     0x52, 0xc9, 0x53, 0xdc, 0x00, 0x9b, 0x77, 0xf6, 0x1c, 0x71, 0x4f, 0x34,
     0xfc, 0xe3, 0x73, 0xb3, 0xc2, 0xc0, 0x7c, 0x0a, 0xb7, 0x80, 0xa8, 0x05,
     0x42, 0x9b, 0x3e, 0x51, 0x02, 0x03, 0x01, 0x00, 0x01, 0x02, 0x82, 0x01,
     0x00, 0x3c, 0xab, 0x00, 0xc2, 0x2f, 0x45, 0xfd, 0x2b, 0x40, 0x5c, 0xe7,
     0x5e, 0xa0, 0x40, 0x51, 0x14, 0x89, 0x4c, 0x72, 0xd8, 0x3f, 0x12, 0x95,
     0xc0, 0x24, 0xd8, 0x71, 0x52, 0xa5, 0x68, 0xe4, 0x83, 0x7d, 0x55, 0x5c,
     0xd7, 0x64, 0x2c, 0x15, 0x95, 0x18, 0xfb, 0x9c, 0x2a, 0x1c, 0x7a, 0x70,
     0x5f, 0x22, 0xa4, 0xa7, 0xc8, 0x03, 0x45, 0x36, 0x27, 0x3f, 0xb2, 0x7d,
     0x21, 0xd9, 0x95, 0x35, 0x33, 0x6f, 0x2f, 0x59, 0xb4, 0xf0, 0xb7, 0xd7,
     0x41, 0x2e, 0x04, 0xd7, 0xf4, 0x04, 0xbd, 0x35, 0xb7, 0xff, 0x40, 0x46,
     0xeb, 0x87, 0x05, 0xea, 0x31, 0x49, 0x27, 0x2a, 0xc0, 0x12, 0xcb, 0x70,
     0x8f, 0xe9, 0x56, 0x79, 0x25, 0xbe, 0xbf, 0xbe, 0x3d, 0x94, 0x65, 0x7e,
     0x9c, 0xd2, 0x0a, 0x92, 0xca, 0x5b, 0x9e, 0xa1, 0xb9, 0x3b, 0x2d, 0x48,
     0x25, 0x90, 0x25, 0xd1, 0x04, 0x54, 0x2d, 0x36, 0x6c, 0x2c, 0x9a, 0xd3,
     0xc4, 0x9b, 0xfd, 0xa8, 0x29, 0x4f, 0x00, 0xf4, 0x1e, 0x31, 0xbb, 0x27,
     0x48, 0x22, 0xf0, 0x10, 0xfe, 0x88, 0x67, 0xc4, 0x73, 0x26, 0x6d, 0x77,
     0xff, 0xf8, 0x69, 0x45, 0xec, 0xa5, 0x01, 0x4e, 0x25, 0x2a, 0x48, 0x5d,
     0xad, 0xdf, 0x12, 0x1b, 0x69, 0x37, 0x96, 0x57, 0x7d, 0x49, 0xba, 0x42,
     0xb6, 0xf2, 0xe4, 0x66, 0xf6, 0xeb, 0x0d, 0x3b, 0x81, 0x6f, 0x27, 0x03,
     0x3b, 0x39, 0xf3, 0xd5, 0x66, 0x05, 0x53, 0x22, 0x9a, 0x13, 0x7a, 0xe9,
     0xd6, 0x04, 0xe0, 0xd9, 0x76, 0xdd, 0x33, 0x79, 0x67, 0xc4, 0x8a, 0x59,
     0x31, 0xb2, 0x0f, 0x8e, 0x2b, 0x84, 0xde, 0x49, 0x3a, 0x7e, 0x23, 0xa0,
     0x4e, 0x2f, 0x69, 0x24, 0x1d, 0xe1, 0x05, 0x9c, 0xf8, 0x7d, 0x27, 0x7c,
     0x65, 0xb0, 0x21, 0x37, 0x74, 0xe2, 0x08, 0x41, 0x63, 0xdb, 0x33, 0xfd,
     0x79, 0xd5, 0x51, 0x79, 0x8d, 0x02, 0x81, 0x81, 0x00, 0xe3, 0x53, 0x55,
     0x15, 0x5b, 0x90, 0xd3, 0xec, 0x1a, 0xbd, 0xbb, 0x2e, 0x1f, 0x8a, 0xd3,
     0x51, 0x4a, 0x33, 0x8e, 0x4b, 0x3c, 0xa3, 0x9e, 0x4b, 0x97, 0xfd, 0xc2,
     0x56, 0xf7, 0x0c, 0x67, 0x12, 0xe1, 0x86, 0x36, 0x1b, 0x02, 0x52, 0x3f,
     0xe4, 0xfd, 0xa6, 0xaa, 0xaf, 0xd9, 0x21, 0x9c, 0xce, 0xb8, 0x58, 0x0b,
     0x82, 0xef, 0x6f, 0xfd, 0x32, 0x52, 0xec, 0x06, 0x1d, 0xe6, 0x86, 0x34,
     0xf9, 0x95, 0x46, 0x32, 0x7d, 0x95, 0xe9, 0x48, 0x52, 0x28, 0x07, 0x8a,
     0xfe, 0x10, 0x02, 0xd5, 0x04, 0x0b, 0x39, 0x02, 0xb1, 0xc1, 0x51, 0xdf,
     0x0a, 0x90, 0x38, 0x6b, 0x2f, 0xc9, 0x40, 0x0b, 0xa2, 0x2f, 0x21, 0xf0,
     0x5c, 0x73, 0x48, 0xc5, 0x40, 0x40, 0xce, 0xd6, 0xb3, 0x9a, 0xc5, 0xf9,
     0x26, 0x7d, 0x13, 0x21, 0x73, 0x7d, 0x20, 0x9d, 0x64, 0xb8, 0x74, 0x69,
     0x68, 0x62, 0xa9, 0xf0, 0x47, 0x02, 0x81, 0x81, 0x00, 0xcf, 0xf5, 0x01,
     0xf6, 0xae, 0x43, 0x7f, 0x71, 0xd0, 0x74, 0x38, 0x35, 0x6b, 0xe9, 0xeb,
     0xc3, 0x9e, 0x0c, 0x58, 0x00, 0x88, 0xbc, 0x65, 0xfe, 0x84, 0x71, 0x84,
     0x7d, 0x59, 0xe5, 0xf6, 0x94, 0x08, 0x00, 0xd4, 0x50, 0x95, 0x37, 0x89,
     0xf7, 0x45, 0x1d, 0x26, 0x1c, 0x92, 0x6a, 0x3c, 0x97, 0xd0, 0x87, 0x33,
     0xe3, 0x04, 0x23, 0x22, 0x49, 0x7d, 0x5d, 0xfd, 0x2e, 0x48, 0x7d, 0x93,
     0x26, 0x9a, 0x08, 0x84, 0xf4, 0xe4, 0xf1, 0x91, 0x2a, 0x69, 0x13, 0x50,
     0x29, 0x3f, 0xf9, 0x70, 0x0f, 0xe6, 0x23, 0x92, 0x84, 0xd7, 0x79, 0xa6,
     0x4c, 0x07, 0x75, 0xd0, 0xda, 0x15, 0xa0, 0xfe, 0x19, 0x43, 0x60, 0x71,
     0x85, 0xed, 0xa2, 0xa3, 0x91, 0xe7, 0x1c, 0xf8, 0x38, 0x87, 0x8b, 0x0c,
     0x77, 0xe8, 0x84, 0x4a, 0x6d, 0x26, 0x78, 0xb3, 0x18, 0x3a, 0x0c, 0x08,
     0xcd, 0x2f, 0x84, 0x80, 0xa7, 0x02, 0x81, 0x81, 0x00, 0x92, 0x26, 0xe6,
     0x81, 0xf7, 0xbf, 0xd2, 0x70, 0x6e, 0xf4, 0xe2, 0xde, 0x11, 0x8b, 0x78,
     0xb7, 0x3f, 0xc4, 0x8b, 0xcc, 0x95, 0x6d, 0x00, 0x7d, 0xcb, 0x77, 0x6b,
     0xb5, 0xeb, 0xd9, 0xbe, 0x7f, 0x26, 0x1d, 0xda, 0xb5, 0x5c, 0xb5, 0xb2,
     0xbc, 0xaf, 0xd3, 0xb2, 0xd3, 0xa1, 0xfe, 0xf9, 0x48, 0xf2, 0xbe, 0x30,
     0x90, 0x7b, 0x0e, 0x7e, 0x34, 0x17, 0x9a, 0x05, 0x88, 0xbd, 0xc8, 0x35,
     0x50, 0xed, 0x87, 0x17, 0x39, 0x24, 0xe3, 0xba, 0x0a, 0x90, 0x0f, 0xe1,
     0x6d, 0xdc, 0xaf, 0x65, 0x72, 0xd7, 0x20, 0x4d, 0x3a, 0x29, 0x77, 0x50,
     0xb6, 0x02, 0xe9, 0x33, 0x08, 0xa3, 0x14, 0x19, 0x26, 0xa9, 0xb4, 0xe1,
     0x4b, 0x1f, 0x17, 0x7f, 0x32, 0x03, 0x38, 0xb8, 0x52, 0xfa, 0xc4, 0x23,
     0x9e, 0x7a, 0x35, 0x8d, 0x81, 0x29, 0xc6, 0x89, 0x4a, 0xd3, 0x48, 0x8c,
     0xe8, 0x7e, 0xde, 0x91, 0x4b, 0x02, 0x81, 0x81, 0x00, 0xca, 0x1f, 0x26,
     0xc3, 0x6e, 0x68, 0xe8, 0x6b, 0xe4, 0xe1, 0xb0, 0xe8, 0x2b, 0x74, 0xf6,
     0xd3, 0xf0, 0xbf, 0xaa, 0xa1, 0x0a, 0x45, 0xa3, 0x14, 0x98, 0x02, 0x05,
     0x76, 0x18, 0x74, 0x6d, 0x0f, 0x40, 0x20, 0x1e, 0x3c, 0x1c, 0x6c, 0x18,
     0xca, 0x9f, 0x00, 0x4f, 0xd4, 0xa1, 0x72, 0x64, 0x66, 0x62, 0x25, 0xa7,
     0x06, 0xd5, 0x34, 0x98, 0xd4, 0xb8, 0xe7, 0x2b, 0x77, 0x5a, 0xb9, 0xc2,
     0xd6, 0x58, 0x0c, 0x43, 0xbe, 0x0a, 0xef, 0xc7, 0xda, 0x62, 0xba, 0x84,
     0xc5, 0x33, 0x0a, 0xec, 0x22, 0x67, 0xda, 0xd7, 0x44, 0x0a, 0x72, 0x90,
     0x71, 0x81, 0x26, 0xbf, 0xd1, 0xd8, 0x07, 0xdc, 0x31, 0x0d, 0xcd, 0xa5,
     0x49, 0x38, 0xc9, 0x45, 0x16, 0x40, 0x7d, 0x65, 0xbc, 0xfb, 0xf0, 0xe8,
     0x33, 0x7b, 0x69, 0x69, 0x35, 0x61, 0x99, 0x1a, 0xe9, 0x47, 0x31, 0xff,
     0x33, 0xff, 0x25, 0x58, 0x2d, 0x02, 0x81, 0x80, 0x35, 0x27, 0x9e, 0xc2,
     0x31, 0x7c, 0xe7, 0x2c, 0x31, 0xc8, 0x64, 0xda, 0x06, 0x2c, 0xa6, 0x2c,
     0x14, 0xb7, 0xff, 0x62, 0x17, 0x31, 0xb2, 0x5c, 0x2a, 0x9a, 0xd5, 0xd3,
     0xfa, 0x9e, 0xf8, 0xa0, 0x2e, 0x9f, 0x64, 0x06, 0x35, 0x43, 0x2f, 0xfb,
     0x3a, 0x8d, 0x3a, 0xb5, 0xea, 0x74, 0x6f, 0xad, 0x7f, 0x76, 0x9d, 0xc7,
     0x20, 0xdd, 0x11, 0x49, 0x18, 0x62, 0x6e, 0xac, 0x53, 0xe0, 0x3c, 0xbd,
     0x65, 0x07, 0x40, 0x99, 0x1a, 0xe7, 0x96, 0x10, 0x6e, 0x2e, 0x9e, 0xc7,
     0x15, 0x69, 0x2d, 0xc9, 0x71, 0x0d, 0x61, 0x0c, 0x6d, 0x50, 0x78, 0xb1,
     0x22, 0x35, 0xbc, 0x77, 0x6f, 0xae, 0x66, 0xbf, 0x01, 0x1f, 0x38, 0x02,
     0x33, 0xdf, 0xb2, 0x9b, 0xa3, 0xc7, 0x3a, 0x2c, 0xa4, 0xf9, 0x50, 0x3e,
     0x54, 0x42, 0x82, 0xa9, 0x82, 0x4e, 0xae, 0x7e, 0x79, 0x2b, 0x8c, 0x83,
     0x86, 0x0b, 0x38, 0x19};
