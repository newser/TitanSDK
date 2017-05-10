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

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

// === routine declarations ================
TT_TEST_ROUTINE_DECLARE(tt_unit_test_crypto_pad_none)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_crypto_pad_pkcs7)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(crypto_pad_case)

TT_TEST_CASE("tt_unit_test_crypto_pad_none",
             "crypto pad: NONE",
             tt_unit_test_crypto_pad_none,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_crypto_pad_pkcs7",
                 "crypto pad: pkcs7",
                 tt_unit_test_crypto_pad_pkcs7,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(crypto_pad_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(CRYPTO_UT_PAD, 0, crypto_pad_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(tt_unit_test_crypto_pad_pkcs7)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(tt_unit_test_crypto_pad_none)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_u8_t buf[100], tail[100];
    tt_result_t ret;
    tt_u32_t data_len, tail_len;

    TT_TEST_CASE_ENTER()
    // test start

    data_len = 100;
    ret =
        tt_crypto_pad(TT_CRYPTO_PAD_NONE, 10, buf, &data_len, tail, &tail_len);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tail_len, 0, "");

    data_len = 100;
    ret = tt_crypto_unpad(TT_CRYPTO_PAD_NONE, 10, buf, &data_len);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(data_len, 100, "");

    data_len = 101;
    ret =
        tt_crypto_pad(TT_CRYPTO_PAD_NONE, 10, buf, &data_len, tail, &tail_len);
    TT_UT_FAIL(ret, "");

    data_len = 99;
    ret =
        tt_crypto_pad(TT_CRYPTO_PAD_NONE, 10, buf, &data_len, tail, &tail_len);
    TT_UT_FAIL(ret, "");

    data_len = 99;
    ret = tt_crypto_unpad(TT_CRYPTO_PAD_NONE, 10, buf, &data_len);
    TT_UT_FAIL(ret, "");

    data_len = 0;
    ret =
        tt_crypto_pad(TT_CRYPTO_PAD_NONE, 10, buf, &data_len, tail, &tail_len);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tail_len, 0, "");

    data_len = 0;
    ret = tt_crypto_unpad(TT_CRYPTO_PAD_NONE, 10, buf, &data_len);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(data_len, 0, "");

    data_len = 1;
    ret =
        tt_crypto_pad(TT_CRYPTO_PAD_NONE, 10, buf, &data_len, tail, &tail_len);
    TT_UT_FAIL(ret, "");

    data_len = 10;
    ret =
        tt_crypto_pad(TT_CRYPTO_PAD_NONE, 10, buf, &data_len, tail, &tail_len);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tail_len, 0, "");

    data_len = 10;
    ret = tt_crypto_unpad(TT_CRYPTO_PAD_NONE, 10, buf, &data_len);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(data_len, 10, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_crypto_pad_pkcs7)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_u8_t buf[100], tail[100], padded[200];
    tt_result_t ret;
    tt_u32_t data_len, tail_len;

    TT_TEST_CASE_ENTER()
    // test start

    // pad a whole block
    data_len = 100;
    ret =
        tt_crypto_pad(TT_CRYPTO_PAD_PKCS7, 10, buf, &data_len, tail, &tail_len);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(data_len, 100, "");
    TT_UT_EQUAL(tail_len, 10, "");
    TT_UT_EQUAL(tail[0], 10, "");
    TT_UT_EQUAL(tail[5], 10, "");
    TT_UT_EQUAL(tail[9], 10, "");

    tt_memset(&padded[100], 10, 10);
    data_len = 110;
    ret = tt_crypto_unpad(TT_CRYPTO_PAD_PKCS7, 10, padded, &data_len);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(data_len, 100, "");

    // pad partial block
    buf[90] = 0xab;
    data_len = 91;
    ret =
        tt_crypto_pad(TT_CRYPTO_PAD_PKCS7, 10, buf, &data_len, tail, &tail_len);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(data_len, 90, "");
    TT_UT_EQUAL(tail_len, 10, "");
    TT_UT_EQUAL(tail[0], 0xab, "");
    TT_UT_EQUAL(tail[1], 9, "");
    TT_UT_EQUAL(tail[5], 9, "");
    TT_UT_EQUAL(tail[9], 9, "");

    tt_memset(&padded[91], 9, 9);
    data_len = 100;
    ret = tt_crypto_unpad(TT_CRYPTO_PAD_PKCS7, 10, padded, &data_len);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(data_len, 91, "");

    // pad 1 byte
    buf[90] = 0xab;
    buf[98] = 0xcd;
    data_len = 99;
    ret =
        tt_crypto_pad(TT_CRYPTO_PAD_PKCS7, 10, buf, &data_len, tail, &tail_len);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(data_len, 90, "");
    TT_UT_EQUAL(tail_len, 10, "");
    TT_UT_EQUAL(tail[0], 0xab, "");
    TT_UT_EQUAL(tail[8], 0xcd, "");
    TT_UT_EQUAL(tail[9], 1, "");

    tt_memset(&padded[99], 1, 1);
    data_len = 100;
    ret = tt_crypto_unpad(TT_CRYPTO_PAD_PKCS7, 10, padded, &data_len);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(data_len, 99, "");

    // invalid unpad
    tt_memset(&padded[99], 0, 1);
    data_len = 100;
    ret = tt_crypto_unpad(TT_CRYPTO_PAD_PKCS7, 10, padded, &data_len);
    TT_UT_FAIL(ret, "");

    tt_memset(&padded[99], 250, 1);
    data_len = 100;
    ret = tt_crypto_unpad(TT_CRYPTO_PAD_PKCS7, 10, padded, &data_len);
    TT_UT_FAIL(ret, "");

    data_len = 0;
    ret = tt_crypto_unpad(TT_CRYPTO_PAD_PKCS7, 10, padded, &data_len);
    TT_UT_FAIL(ret, "");

    data_len = 11;
    ret = tt_crypto_unpad(TT_CRYPTO_PAD_PKCS7, 10, padded, &data_len);
    TT_UT_FAIL(ret, "");

    // test end
    TT_TEST_CASE_LEAVE()
}
