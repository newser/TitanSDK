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
    tt_blob_t input;
    tt_blob_t output;
    tt_blob_t key;
    tt_aes_attr_t attr;
} __aes_test_vect_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

// === routine declarations ================
TT_TEST_ROUTINE_DECLARE(tt_unit_test_aes128_cbc_pkcs7)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_aes128_cbc_none)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_aes128_cbc_p7_rep)

TT_TEST_ROUTINE_DECLARE(tt_unit_test_aes256_cbc_pkcs7)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_aes256_cbc_none)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_aes256_cbc_p7_rep)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(crypto_aes_case)

TT_TEST_CASE("tt_unit_test_aes128_cbc_pkcs7",
             "crypto: AES128",
             tt_unit_test_aes128_cbc_pkcs7,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_aes128_cbc_none",
                 "crypto: AES128, none padding",
                 tt_unit_test_aes128_cbc_none,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_aes128_cbc_p7_rep",
                 "crypto: AES128, repeat use",
                 tt_unit_test_aes128_cbc_p7_rep,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_aes256_cbc_pkcs7",
                 "crypto: AES256",
                 tt_unit_test_aes256_cbc_pkcs7,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_aes256_cbc_none",
                 "crypto: AES256, none padding",
                 tt_unit_test_aes256_cbc_none,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_aes256_cbc_p7_rep",
                 "crypto: AES256, repeat use",
                 tt_unit_test_aes256_cbc_p7_rep,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(crypto_aes_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(CRYPTO_UT_AES, 0, crypto_aes_case)

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

    static tt_u8_t __aes128_in1[] = {0x31};
static tt_u8_t __aes128_key1[] = {0xab, 0xc1, 0x23};
static tt_u8_t __aes128_iv1[] =
    {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0};
static tt_u8_t __aes128_out1[] = {0x27,
                                  0x0f,
                                  0xa7,
                                  0x32,
                                  0x75,
                                  0x96,
                                  0x38,
                                  0x09,
                                  0x57,
                                  0x3d,
                                  0x10,
                                  0x10,
                                  0x3b,
                                  0xc6,
                                  0x6c,
                                  0xc8};

static tt_u8_t __aes128_in2[] = {
    0x31,
    0x32,
    0x33,
    0x34,
    0x35,
    0x36,
    0x37,
    0x38,
    0x39,
    0x61,
    0x62,
    0x63,
    0x64,
    0x65,
    0x66,
    0x30,
};
static tt_u8_t __aes128_key2[] = {0xab, 0xc1, 0x23};
static tt_u8_t __aes128_iv2[] = {0x12,
                                 0x34,
                                 0x56,
                                 0x78,
                                 0x9a,
                                 0xbc,
                                 0xde,
                                 0xf0,
                                 0x12,
                                 0x34,
                                 0x56,
                                 0x78,
                                 0x9a,
                                 0xbc,
                                 0xde,
                                 0xf0};
static tt_u8_t __aes128_out2[] = {
    0xf9, 0xca, 0x28, 0xfb, 0x2f, 0x1c, 0x11, 0x91, 0x50, 0xfd, 0x30,
    0xd7, 0x32, 0x4a, 0x01, 0x6a, 0xf4, 0xce, 0x82, 0xe3, 0xeb, 0xec,
    0x1c, 0x50, 0x7a, 0x7b, 0x18, 0x42, 0xb9, 0xd3, 0x9e, 0x13,
};

static tt_u8_t __aes128_in3[] = {
    0x4e, 0x41, 0x54, 0x20, 0x74, 0x72, 0x61, 0x76, 0x65, 0x72, 0x73, 0x61,
    0x6c, 0x20, 0x69, 0x73, 0x20, 0x61, 0x20, 0x63, 0x6f, 0x6d, 0x70, 0x75,
    0x74, 0x65, 0x72, 0x20, 0x6e, 0x65, 0x74, 0x77, 0x6f, 0x72, 0x6b, 0x69,
    0x6e, 0x67, 0x20, 0x6d, 0x65, 0x74, 0x68, 0x6f, 0x64, 0x6f, 0x6c, 0x6f,
    0x67, 0x79, 0x20, 0x77, 0x69, 0x74, 0x68, 0x20, 0x74, 0x68, 0x65, 0x20,
    0x67, 0x6f, 0x61, 0x6c, 0x20, 0x74, 0x6f, 0x20, 0x65, 0x73, 0x74, 0x61,
    0x62, 0x6c, 0x69, 0x73, 0x68, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x6d, 0x61,
    0x69, 0x6e, 0x74, 0x61, 0x69, 0x6e, 0x20, 0x49, 0x6e, 0x74, 0x65, 0x72,
    0x6e, 0x65, 0x74, 0x20, 0x70, 0x72, 0x6f, 0x74, 0x6f, 0x63, 0x6f, 0x6c,
    0x20, 0x63, 0x6f, 0x6e, 0x6e, 0x65, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x73,
    0x20, 0x61, 0x63, 0x72, 0x6f, 0x73, 0x73, 0x20, 0x67, 0x61, 0x74, 0x65,
    0x77, 0x61, 0x79, 0x73, 0x20, 0x74, 0x68, 0x61, 0x74, 0x20, 0x69, 0x6d,
    0x70, 0x6c, 0x65, 0x6d, 0x65, 0x6e, 0x74, 0x20, 0x6e, 0x65, 0x74, 0x77,
    0x6f, 0x72, 0x6b, 0x20, 0x61, 0x64, 0x64, 0x72, 0x65, 0x73, 0x73, 0x20,
    0x74, 0x72, 0x61, 0x6e, 0x73, 0x6c, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x20,
    0x28, 0x4e, 0x41, 0x54, 0x29, 0x2e, 0x20, 0x0a,
};
static tt_u8_t __aes128_out3[] = {
    0x09, 0x55, 0xb2, 0x6a, 0x6f, 0x90, 0xb5, 0x5b, 0x77, 0x48, 0xaf, 0x60,
    0x74, 0xe8, 0x6d, 0xb2, 0x0c, 0x99, 0x24, 0xe3, 0x76, 0x8a, 0xa0, 0xdb,
    0x97, 0x83, 0xdf, 0x58, 0x04, 0x27, 0x4c, 0x81, 0x90, 0x40, 0x6a, 0x4a,
    0xf8, 0xaf, 0x70, 0x14, 0x9a, 0x15, 0xcc, 0x22, 0x05, 0x92, 0x4c, 0x3e,
    0xa0, 0x64, 0x0a, 0x32, 0x9a, 0xd0, 0xe5, 0xb7, 0x1c, 0x39, 0x4a, 0x53,
    0x19, 0x8e, 0xe1, 0x69, 0xcd, 0xbc, 0x45, 0xe7, 0xaf, 0xe4, 0x59, 0x9a,
    0xf7, 0xc7, 0x2c, 0x42, 0xe5, 0x44, 0x8f, 0x38, 0x2e, 0xec, 0xd6, 0x39,
    0xd5, 0x70, 0x73, 0x9c, 0x48, 0xb8, 0xdf, 0x7b, 0x1e, 0x7d, 0xa9, 0x2a,
    0xe2, 0x1f, 0x0f, 0xab, 0xc8, 0xe0, 0x90, 0x7f, 0xe2, 0xd4, 0x20, 0x0d,
    0xa8, 0xf9, 0x0a, 0x02, 0x40, 0x05, 0x22, 0x66, 0x73, 0xfa, 0xf0, 0x4c,
    0x21, 0xd9, 0x9f, 0x4a, 0x7c, 0x15, 0x11, 0x19, 0x37, 0x1b, 0x8e, 0x07,
    0xb3, 0x7c, 0xac, 0xd5, 0x4f, 0xed, 0xa3, 0x1d, 0xd6, 0x20, 0x24, 0xb3,
    0x95, 0xf8, 0xa7, 0x74, 0x37, 0x67, 0x55, 0x2f, 0x2e, 0x26, 0x1d, 0xbe,
    0x57, 0xa9, 0x58, 0xa8, 0xac, 0x32, 0xad, 0x5d, 0xa3, 0x65, 0xb0, 0x2e,
    0x34, 0x9b, 0xb5, 0x4f, 0xd6, 0x7b, 0x5a, 0x76, 0xff, 0x89, 0x92, 0x2d,
    0xb0, 0x8e, 0x16, 0x7b, 0x95, 0x52, 0x2c, 0x4d, 0x75, 0x91, 0x89, 0xb6,
};

static __aes_test_vect_t aes_128_tv[] = {
    {
        {__aes128_in1, sizeof(__aes128_in1)},
        {__aes128_out1, sizeof(__aes128_out1)},
        {__aes128_key1, sizeof(__aes128_key1)},
        {TT_AES_PADDING_PKCS7, TT_AES_MODE_CBC},
    },
    {
        {__aes128_in2, sizeof(__aes128_in2)},
        {__aes128_out2, sizeof(__aes128_out2)},
        {__aes128_key2, sizeof(__aes128_key2)},
        {TT_AES_PADDING_PKCS7, TT_AES_MODE_CBC},
    },
    {
        {__aes128_in3, sizeof(__aes128_in3)},
        {__aes128_out3, sizeof(__aes128_out3)},
        {__aes128_key2, sizeof(__aes128_key2)},
        {TT_AES_PADDING_PKCS7, TT_AES_MODE_CBC},
    },
};
static tt_blob_t aes_128_tv_iv[] = {
    {__aes128_iv1, sizeof(__aes128_iv1)},
    {__aes128_iv2, sizeof(__aes128_iv2)},
    {__aes128_iv2, sizeof(__aes128_iv2)},
};

TT_TEST_ROUTINE_DEFINE(tt_unit_test_aes128_cbc_pkcs7)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_u8_t obuf[sizeof(__aes128_out3)];
    tt_blob_t o_tbuf = {obuf, sizeof(obuf)};
    tt_u8_t obuf2[sizeof(__aes128_out3)];
    tt_blob_t o_tbuf2 = {obuf2, sizeof(obuf2)};
    tt_result_t result;

    tt_u32_t i;

    TT_TEST_CASE_ENTER()
    // test start

    for (i = 0; i < sizeof(aes_128_tv) / sizeof(aes_128_tv[0]); ++i) {
        __aes_test_vect_t *tv = &aes_128_tv[i];

        // enc
        tt_memset(obuf, 0, sizeof(obuf));
        tt_memcpy(&tv->attr.cbc.ivec, &aes_128_tv_iv[i], sizeof(tt_blob_t));
        o_tbuf.len = sizeof(obuf);
        result = tt_aes(TT_TRUE,
                        &tv->key,
                        TT_AES_SIZE_128,
                        &tv->attr,
                        &tv->input,
                        o_tbuf.addr,
                        &o_tbuf.len);
        TT_UT_EQUAL(result, TT_SUCCESS, "");
        TT_UT_EQUAL(o_tbuf.len, tv->output.len, "");
        TT_UT_EQUAL(tt_memcmp(o_tbuf.addr, tv->output.addr, o_tbuf.len), 0, "");

        // dec
        tt_memset(obuf2, 0, sizeof(obuf2));
        o_tbuf2.len = sizeof(obuf2);
        result = tt_aes(TT_FALSE,
                        &tv->key,
                        TT_AES_SIZE_128,
                        &tv->attr,
                        &o_tbuf,
                        o_tbuf2.addr,
                        &o_tbuf2.len);
        // result = tt_aes(TT_FALSE, &o_tbuf,&tv->key,
        //                        TT_AES_SIZE_128,&tv->attr,
        //                        tv->padding,o_tbuf2.addr, &o_tbuf2.len);
        TT_UT_EQUAL(result, TT_SUCCESS, "");
        TT_UT_EQUAL(o_tbuf2.len, tv->input.len, "");
        TT_UT_EQUAL(tt_memcmp(o_tbuf2.addr, tv->input.addr, o_tbuf2.len),
                    0,
                    "");
    }

    for (i = 0; i < sizeof(aes_128_tv) / sizeof(aes_128_tv[0]); ++i) {
        __aes_test_vect_t *tv = &aes_128_tv[i];
        static tt_u8_t __tbuf[1000];
        tt_blob_t ib;
        tt_u32_t len;

        // enc

        // iv
        tt_memcpy(&tv->attr.cbc.ivec, &aes_128_tv_iv[i], sizeof(tt_blob_t));

        // input
        TT_UT_EXP(tv->input.len <= sizeof(__tbuf), "");
        tt_memcpy(__tbuf, tv->input.addr, tv->input.len);
        ib.addr = __tbuf;
        ib.len = tv->input.len;

        // output
        len = sizeof(__tbuf);

        result = tt_aes(TT_TRUE,
                        &tv->key,
                        TT_AES_SIZE_128,
                        &tv->attr,
                        &ib,
                        __tbuf,
                        &len);
        TT_UT_EQUAL(result, TT_SUCCESS, "");
        TT_UT_EQUAL(len, tv->output.len, "");
        TT_UT_EQUAL(tt_memcmp(__tbuf, tv->output.addr, len), 0, "");

        // dec

        // input
        ib.addr = __tbuf;
        ib.len = len;

        // output
        len = sizeof(__tbuf);

        result = tt_aes(TT_FALSE,
                        &tv->key,
                        TT_AES_SIZE_128,
                        &tv->attr,
                        &ib,
                        __tbuf,
                        &len);
        // result = tt_aes(TT_FALSE, &o_tbuf,&tv->key,
        //                        TT_AES_SIZE_128,&tv->attr,
        //                        tv->padding,o_tbuf2.addr, &o_tbuf2.len);
        TT_UT_EQUAL(result, TT_SUCCESS, "");
        TT_UT_EQUAL(len, tv->input.len, "");
        TT_UT_EQUAL(tt_memcmp(__tbuf, tv->input.addr, len), 0, "");
    }

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_u8_t __aes128_none_in1[] = {0x31,
                                      0x32,
                                      0x33,
                                      0x34,
                                      0x31,
                                      0x32,
                                      0x33,
                                      0x34,
                                      0x31,
                                      0x32,
                                      0x33,
                                      0x34,
                                      0x31,
                                      0x32,
                                      0x33,
                                      0x34};
static tt_u8_t __aes128_none_out1[] = {0x0c,
                                       0x6a,
                                       0x6f,
                                       0xef,
                                       0xa0,
                                       0xcf,
                                       0x3f,
                                       0x77,
                                       0x89,
                                       0x80,
                                       0xa2,
                                       0x5d,
                                       0x69,
                                       0xa2,
                                       0xf6,
                                       0xde};

static tt_u8_t __aes128_none_in2[] = {
    0x4e, 0x41, 0x54, 0x20, 0x74, 0x72, 0x61, 0x76, 0x65, 0x72, 0x73, 0x61,
    0x6c, 0x20, 0x69, 0x73, 0x20, 0x61, 0x20, 0x63, 0x6f, 0x6d, 0x70, 0x75,
    0x74, 0x65, 0x72, 0x20, 0x6e, 0x65, 0x74, 0x77, 0x6f, 0x72, 0x6b, 0x69,
    0x6e, 0x67, 0x20, 0x6d, 0x65, 0x74, 0x68, 0x6f, 0x64, 0x6f, 0x6c, 0x6f,
    0x67, 0x79, 0x20, 0x77, 0x69, 0x74, 0x68, 0x20, 0x74, 0x68, 0x65, 0x20,
    0x67, 0x6f, 0x61, 0x6c, 0x20, 0x74, 0x6f, 0x20, 0x65, 0x73, 0x74, 0x61,
    0x62, 0x6c, 0x69, 0x73, 0x68, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x6d, 0x61,
    0x69, 0x6e, 0x74, 0x61, 0x69, 0x6e, 0x20, 0x49, 0x6e, 0x74, 0x65, 0x72,
    0x6e, 0x65, 0x74, 0x20, 0x70, 0x72, 0x6f, 0x74, 0x6f, 0x63, 0x6f, 0x6c,
    0x20, 0x63, 0x6f, 0x6e, 0x6e, 0x65, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x73,
    0x20, 0x61, 0x63, 0x72, 0x6f, 0x73, 0x73, 0x20, 0x67, 0x61, 0x74, 0x65,
    0x77, 0x61, 0x79, 0x73, 0x20, 0x74, 0x68, 0x61, 0x74, 0x20, 0x69, 0x6d,
    0x70, 0x6c, 0x65, 0x6d, 0x65, 0x6e, 0x74, 0x20, 0x6e, 0x65, 0x74, 0x77,
    0x6f, 0x72, 0x6b, 0x20, 0x61, 0x64, 0x64, 0x72, 0x65, 0x73, 0x73, 0x20,
    0x74, 0x72, 0x61, 0x6e, 0x73, 0x6c, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x20,
    0x28, 0x4e, 0x41, 0x54, 0x29, 0x2e, 0x20, 0x66, 0x66, 0x66, 0x66, 0x66,
};

static tt_u8_t __aes128_none_out2[] = {
    0x09, 0x55, 0xb2, 0x6a, 0x6f, 0x90, 0xb5, 0x5b, 0x77, 0x48, 0xaf, 0x60,
    0x74, 0xe8, 0x6d, 0xb2, 0x0c, 0x99, 0x24, 0xe3, 0x76, 0x8a, 0xa0, 0xdb,
    0x97, 0x83, 0xdf, 0x58, 0x04, 0x27, 0x4c, 0x81, 0x90, 0x40, 0x6a, 0x4a,
    0xf8, 0xaf, 0x70, 0x14, 0x9a, 0x15, 0xcc, 0x22, 0x05, 0x92, 0x4c, 0x3e,
    0xa0, 0x64, 0x0a, 0x32, 0x9a, 0xd0, 0xe5, 0xb7, 0x1c, 0x39, 0x4a, 0x53,
    0x19, 0x8e, 0xe1, 0x69, 0xcd, 0xbc, 0x45, 0xe7, 0xaf, 0xe4, 0x59, 0x9a,
    0xf7, 0xc7, 0x2c, 0x42, 0xe5, 0x44, 0x8f, 0x38, 0x2e, 0xec, 0xd6, 0x39,
    0xd5, 0x70, 0x73, 0x9c, 0x48, 0xb8, 0xdf, 0x7b, 0x1e, 0x7d, 0xa9, 0x2a,
    0xe2, 0x1f, 0x0f, 0xab, 0xc8, 0xe0, 0x90, 0x7f, 0xe2, 0xd4, 0x20, 0x0d,
    0xa8, 0xf9, 0x0a, 0x02, 0x40, 0x05, 0x22, 0x66, 0x73, 0xfa, 0xf0, 0x4c,
    0x21, 0xd9, 0x9f, 0x4a, 0x7c, 0x15, 0x11, 0x19, 0x37, 0x1b, 0x8e, 0x07,
    0xb3, 0x7c, 0xac, 0xd5, 0x4f, 0xed, 0xa3, 0x1d, 0xd6, 0x20, 0x24, 0xb3,
    0x95, 0xf8, 0xa7, 0x74, 0x37, 0x67, 0x55, 0x2f, 0x2e, 0x26, 0x1d, 0xbe,
    0x57, 0xa9, 0x58, 0xa8, 0xac, 0x32, 0xad, 0x5d, 0xa3, 0x65, 0xb0, 0x2e,
    0x34, 0x9b, 0xb5, 0x4f, 0xd6, 0x7b, 0x5a, 0x76, 0x17, 0x6f, 0xc3, 0xbe,
    0x08, 0xa7, 0x34, 0xd9, 0x29, 0x82, 0xc9, 0x8d, 0x4d, 0x0a, 0x4a, 0xc9,
};

static __aes_test_vect_t aes_128_none_tv[] = {
    {
        {__aes128_none_in1, sizeof(__aes128_none_in1)},
        {__aes128_none_out1, sizeof(__aes128_none_out1)},
        {__aes128_key1, sizeof(__aes128_key1)},
        {TT_AES_PADDING_NONE, TT_AES_MODE_CBC},
    },
    {
        {__aes128_none_in2, sizeof(__aes128_none_in2)},
        {__aes128_none_out2, sizeof(__aes128_none_out2)},
        {__aes128_key1, sizeof(__aes128_key1)},
        {TT_AES_PADDING_NONE, TT_AES_MODE_CBC},
    },
};

TT_TEST_ROUTINE_DEFINE(tt_unit_test_aes128_cbc_none)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_u8_t obuf[sizeof(__aes128_out3)];
    tt_blob_t o_tbuf = {obuf, sizeof(obuf)};
    tt_u8_t obuf2[sizeof(__aes128_out3)];
    tt_blob_t o_tbuf2 = {obuf2, sizeof(obuf2)};
    tt_result_t result;

    tt_u32_t i;

    TT_TEST_CASE_ENTER()
    // test start

    for (i = 0; i < sizeof(aes_128_none_tv) / sizeof(aes_128_none_tv[0]); ++i) {
        __aes_test_vect_t *tv = &aes_128_none_tv[i];

        // enc
        tt_memset(obuf, 0, sizeof(obuf));
        tt_memcpy(&tv->attr.cbc.ivec, &aes_128_tv_iv[i], sizeof(tt_blob_t));
        o_tbuf.len = sizeof(obuf);
        result = tt_aes(TT_TRUE,
                        &tv->key,
                        TT_AES_SIZE_128,
                        &tv->attr,
                        &tv->input,
                        o_tbuf.addr,
                        &o_tbuf.len);
        TT_UT_EQUAL(result, TT_SUCCESS, "");
        TT_UT_EQUAL(o_tbuf.len, tv->output.len, "");
        TT_UT_EQUAL(tt_memcmp(o_tbuf.addr, tv->output.addr, o_tbuf.len), 0, "");

        // dec
        tt_memset(obuf2, 0, sizeof(obuf2));
        o_tbuf2.len = sizeof(obuf2);
        result = tt_aes(TT_FALSE,
                        &tv->key,
                        TT_AES_SIZE_128,
                        &tv->attr,
                        &o_tbuf,
                        o_tbuf2.addr,
                        &o_tbuf2.len);
        // result = tt_aes(TT_FALSE, &o_tbuf,&tv->key,
        //                        TT_AES_SIZE_128,&tv->attr,
        //                        tv->padding,o_tbuf2.addr, &o_tbuf2.len);
        TT_UT_EQUAL(result, TT_SUCCESS, "");
        TT_UT_EQUAL(o_tbuf2.len, tv->input.len, "");
        TT_UT_EQUAL(tt_memcmp(o_tbuf2.addr, tv->input.addr, o_tbuf2.len),
                    0,
                    "");
    }

    for (i = 0; i < sizeof(aes_128_none_tv) / sizeof(aes_128_none_tv[0]); ++i) {
        __aes_test_vect_t *tv = &aes_128_none_tv[i];
        static tt_u8_t __tbuf[1000];
        tt_blob_t ib;
        tt_u32_t len;

        // enc

        // iv
        tt_memcpy(&tv->attr.cbc.ivec, &aes_128_tv_iv[i], sizeof(tt_blob_t));

        // input
        TT_UT_EXP(tv->input.len <= sizeof(__tbuf), "");
        tt_memcpy(__tbuf, tv->input.addr, tv->input.len);
        ib.addr = __tbuf;
        ib.len = tv->input.len;

        // output
        len = sizeof(__tbuf);

        result = tt_aes(TT_TRUE,
                        &tv->key,
                        TT_AES_SIZE_128,
                        &tv->attr,
                        &ib,
                        __tbuf,
                        &len);
        TT_UT_EQUAL(result, TT_SUCCESS, "");
        TT_UT_EQUAL(len, tv->output.len, "");
        TT_UT_EQUAL(tt_memcmp(__tbuf, tv->output.addr, len), 0, "");

        // dec

        // input
        ib.addr = __tbuf;
        ib.len = len;

        // output
        len = sizeof(__tbuf);

        result = tt_aes(TT_FALSE,
                        &tv->key,
                        TT_AES_SIZE_128,
                        &tv->attr,
                        &ib,
                        __tbuf,
                        &len);
        // result = tt_aes(TT_FALSE, &o_tbuf,&tv->key,
        //                        TT_AES_SIZE_128,&tv->attr,
        //                        tv->padding,o_tbuf2.addr, &o_tbuf2.len);
        TT_UT_EQUAL(result, TT_SUCCESS, "");
        TT_UT_EQUAL(len, tv->input.len, "");
        TT_UT_EQUAL(tt_memcmp(__tbuf, tv->input.addr, len), 0, "");
    }

    // test end
    TT_TEST_CASE_LEAVE()
}

#define __aseg_size 500

TT_TEST_ROUTINE_DEFINE(tt_unit_test_aes128_cbc_p7_rep)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_u8_t ibuf[10000], key[16], iv[16];
    tt_u8_t buf1[__aseg_size + TT_AES_BLOCK_SIZE];
    tt_u8_t buf2[__aseg_size + TT_AES_BLOCK_SIZE];
    tt_u32_t ipos = 0, i;
    tt_u32_t ilen, olen, seg_len;
    tt_aes_t enc_aes, dec_aes;
    tt_result_t ret;
    tt_blob_t bkey;
    tt_aes_attr_t ma;

    TT_TEST_CASE_ENTER()
    // test start

    ilen = tt_rand_u32() % sizeof(key);
    if (ilen == 0)
        ++ilen;
    for (i = 0; i < ilen; ++i)
        key[i] = (tt_u8_t)tt_rand_u32();
    bkey.addr = key;
    bkey.len = ilen;

    ilen = tt_rand_u32() % sizeof(iv);
    if (ilen == 0)
        ++ilen;
    for (i = 0; i < ilen; ++i)
        key[i] = (tt_u8_t)tt_rand_u32();
    ma.padding = TT_AES_PADDING_PKCS7;
    ma.mode = TT_AES_MODE_CBC;
    ma.cbc.ivec.addr = iv;
    ma.cbc.ivec.len = ilen;

    ret = tt_aes_create(&enc_aes, TT_TRUE, &bkey, TT_AES_SIZE_128, &ma);
    TT_UT_SUCCESS(ret, "");
    ret = tt_aes_create(&dec_aes, TT_FALSE, &bkey, TT_AES_SIZE_128, &ma);
    TT_UT_SUCCESS(ret, "");

    for (i = 0; i < sizeof(ibuf); ++i)
        ibuf[i] = (tt_u8_t)tt_rand_u32();

    ipos = 0;
    while (ipos < sizeof(ibuf)) {
        if ((ipos + __aseg_size) < sizeof(ibuf)) {
            seg_len = tt_rand_u32() % __aseg_size;
            if (seg_len == 0)
                ++seg_len;
        } else {
            seg_len = sizeof(ibuf) - ipos;
        }

        ilen = seg_len;
        olen = sizeof(buf1);
        ret = tt_aes_encrypt(&enc_aes, &ibuf[ipos], ilen, buf1, &olen);
        TT_UT_SUCCESS(ret, "");

        ilen = olen;
        olen = sizeof(buf2);
        ret = tt_aes_decrypt(&dec_aes, buf1, ilen, buf2, &olen);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(olen, seg_len, "");
        TT_UT_EQUAL(tt_memcmp(buf2, &ibuf[ipos], seg_len), 0, "");

        ipos += seg_len;
    }
    TT_ASSERT(ipos == sizeof(ibuf));

    tt_aes_destroy(&enc_aes);
    tt_aes_destroy(&dec_aes);

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_u8_t __aes256_out1[] = {
    0x32,
    0xcc,
    0x1d,
    0x47,
    0x00,
    0x47,
    0xa1,
    0x3b,
    0x6a,
    0x7d,
    0x9d,
    0x2b,
    0xa8,
    0xa8,
    0x78,
    0x85,
};

static tt_u8_t __aes256_out2[] = {
    0xd7, 0xbe, 0x6a, 0xf9, 0x02, 0xdb, 0x78, 0xb3, 0xe8, 0x1d, 0xe1,
    0x22, 0x83, 0x63, 0xc5, 0xdd, 0xf6, 0x4f, 0xec, 0x28, 0x75, 0x06,
    0xa6, 0xe3, 0xcd, 0x51, 0x15, 0x4a, 0x3e, 0xd0, 0x21, 0x97,
};

static tt_u8_t __aes256_in3[] = {
    0x55, 0x6e, 0x6c, 0x65, 0x73, 0x73, 0x20, 0x74, 0x68, 0x65, 0x20, 0x73,
    0x69, 0x74, 0x75, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x77, 0x61, 0x72,
    0x72, 0x61, 0x6e, 0x74, 0x73, 0x20, 0x6f, 0x74, 0x68, 0x65, 0x72, 0x77,
    0x69, 0x73, 0x65, 0x2c, 0x20, 0x74, 0x68, 0x65, 0x20, 0x72, 0x65, 0x74,
    0x75, 0x72, 0x6e, 0x65, 0x64, 0x20, 0x6f, 0x62, 0x6a, 0x65, 0x63, 0x74,
    0x20, 0x64, 0x6f, 0x65, 0x73, 0x20, 0x6e, 0x6f, 0x74, 0x20, 0x63, 0x6f,
    0x70, 0x79, 0x20, 0x74, 0x68, 0x65, 0x20, 0x65, 0x78, 0x74, 0x65, 0x72,
    0x6e, 0x61, 0x6c, 0x20, 0x62, 0x75, 0x66, 0x66, 0x65, 0x72, 0x20, 0x74,
    0x6f, 0x20, 0x69, 0x6e, 0x74, 0x65, 0x72, 0x6e, 0x61, 0x6c, 0x20, 0x73,
    0x74, 0x6f, 0x72, 0x61, 0x67, 0x65, 0x20, 0x62, 0x75, 0x74, 0x20, 0x69,
    0x6e, 0x73, 0x74, 0x65, 0x61, 0x64, 0x20, 0x75, 0x73, 0x65, 0x73, 0x20,
    0x74, 0x68, 0x65, 0x20, 0x62, 0x75, 0x66, 0x66, 0x65, 0x72, 0x20, 0x61,
    0x73, 0x20, 0x69, 0x74, 0x73, 0x20, 0x62, 0x61, 0x63, 0x6b, 0x69, 0x6e,
    0x67, 0x20, 0x73, 0x74, 0x6f, 0x72, 0x65,
};
static tt_u8_t __aes256_out3[] = {
    0x49, 0x00, 0x35, 0x3d, 0xda, 0x1b, 0xf9, 0xba, 0x21, 0x5f, 0xcf, 0x81,
    0x05, 0xc3, 0x37, 0xa4, 0x19, 0xc8, 0x00, 0x01, 0x4e, 0x73, 0x70, 0x86,
    0xc9, 0x45, 0x66, 0x80, 0x5a, 0x85, 0xf0, 0xc2, 0x85, 0x67, 0xf5, 0x6f,
    0xcc, 0x58, 0xf3, 0xfd, 0xc6, 0xe0, 0xbf, 0xe5, 0x81, 0xc1, 0x41, 0x80,
    0x09, 0xef, 0xd1, 0x96, 0x24, 0x1e, 0x88, 0x99, 0x8c, 0xcb, 0x58, 0xea,
    0x01, 0x2b, 0xf3, 0xd4, 0xde, 0x36, 0x84, 0x9a, 0x15, 0xcd, 0x15, 0x86,
    0x68, 0xf0, 0x8d, 0xb4, 0x0e, 0x07, 0xaf, 0xf5, 0x2b, 0x4a, 0xf8, 0xd7,
    0xfd, 0xc3, 0x0a, 0xc0, 0x28, 0xa7, 0x29, 0xb7, 0xd1, 0x8e, 0x5f, 0xa9,
    0x88, 0xf7, 0x1a, 0xc8, 0xc3, 0x82, 0xf0, 0x5f, 0xb8, 0x81, 0x52, 0xdb,
    0x8f, 0x03, 0xd5, 0x6d, 0xab, 0x64, 0x2a, 0xf7, 0x48, 0xf2, 0x16, 0x10,
    0x83, 0xb8, 0x61, 0x1d, 0x1b, 0xd9, 0x8b, 0x35, 0xee, 0x1f, 0x06, 0xca,
    0xe2, 0x45, 0x6d, 0xfb, 0x73, 0xe4, 0x7d, 0xf8, 0x90, 0xe7, 0x98, 0xb4,
    0x10, 0xaa, 0x99, 0x14, 0x20, 0xed, 0x69, 0xa4, 0x2e, 0xf3, 0xfb, 0x1c,
    0xb4, 0x15, 0x24, 0x32, 0x13, 0x74, 0x49, 0x51, 0x5a, 0x81, 0x68, 0xf1,
    0xd1, 0x46, 0x3d, 0x03, 0x75, 0xf2, 0xef, 0xc6,
};

static __aes_test_vect_t aes_256_tv[] = {
    {
        {__aes128_in1, sizeof(__aes128_in1)},
        {__aes256_out1, sizeof(__aes256_out1)},
        {__aes128_key1, sizeof(__aes128_key1)},
        {TT_AES_PADDING_PKCS7, TT_AES_MODE_CBC},
    },
    {
        {__aes128_in2, sizeof(__aes128_in2)},
        {__aes256_out2, sizeof(__aes256_out2)},
        {__aes128_key2, sizeof(__aes128_key2)},
        {TT_AES_PADDING_PKCS7, TT_AES_MODE_CBC},
    },
    {
        {__aes256_in3, sizeof(__aes256_in3)},
        {__aes256_out3, sizeof(__aes256_out3)},
        {__aes128_key2, sizeof(__aes128_key2)},
        {TT_AES_PADDING_PKCS7, TT_AES_MODE_CBC},
    },
};
static tt_blob_t aes_256_tv_iv[] = {
    {__aes128_iv1, sizeof(__aes128_iv1)},
    {__aes128_iv2, sizeof(__aes128_iv2)},
    {__aes128_iv2, sizeof(__aes128_iv2)},
};

TT_TEST_ROUTINE_DEFINE(tt_unit_test_aes256_cbc_pkcs7)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_u8_t obuf[sizeof(__aes256_out3)];
    tt_blob_t o_tbuf = {obuf, sizeof(obuf)};
    tt_u8_t obuf2[sizeof(__aes256_out3)];
    tt_blob_t o_tbuf2 = {obuf2, sizeof(obuf2)};
    tt_result_t result;

    tt_u32_t i;

    TT_TEST_CASE_ENTER()
    // test start

    for (i = 0; i < sizeof(aes_256_tv) / sizeof(aes_256_tv[0]); ++i) {
        __aes_test_vect_t *tv = &aes_256_tv[i];

        // enc
        tt_memset(obuf, 0, sizeof(obuf));
        tt_memcpy(&tv->attr.cbc.ivec, &aes_256_tv_iv[i], sizeof(tt_blob_t));
        o_tbuf.len = sizeof(obuf);
        result = tt_aes(TT_TRUE,
                        &tv->key,
                        TT_AES_SIZE_256,
                        &tv->attr,
                        &tv->input,
                        o_tbuf.addr,
                        &o_tbuf.len);
        // result = tt_aes(TT_TRUE, &tv->input,&tv->key,
        //                        TT_AES_SIZE_256,&tv->attr,
        //                        tv->padding,o_tbuf.addr,&o_tbuf.len);
        TT_UT_EQUAL(result, TT_SUCCESS, "");
        TT_UT_EQUAL(o_tbuf.len, tv->output.len, "");
        TT_UT_EQUAL(tt_memcmp(o_tbuf.addr, tv->output.addr, o_tbuf.len), 0, "");

        // dec
        tt_memset(obuf2, 0, sizeof(obuf2));
        o_tbuf2.len = sizeof(obuf2);
        result = tt_aes(TT_FALSE,
                        &tv->key,
                        TT_AES_SIZE_256,
                        &tv->attr,
                        &o_tbuf,
                        o_tbuf2.addr,
                        &o_tbuf2.len);
        // result = tt_aes(TT_FALSE, &o_tbuf,&tv->key,
        //                        TT_AES_SIZE_256,&tv->attr,
        //                        tv->padding,o_tbuf2.addr,&o_tbuf2.len);
        TT_UT_EQUAL(result, TT_SUCCESS, "");
        TT_UT_EQUAL(o_tbuf2.len, tv->input.len, "");
        TT_UT_EQUAL(tt_memcmp(o_tbuf2.addr, tv->input.addr, o_tbuf2.len),
                    0,
                    "");
    }

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_u8_t __aes256_none_out1[] = {
    0xd8,
    0x81,
    0xd7,
    0x1b,
    0xe1,
    0x18,
    0x37,
    0x2c,
    0x90,
    0xf0,
    0x90,
    0x3f,
    0x70,
    0xa6,
    0xe4,
    0x3f,
};

static tt_u8_t __aes256_none_out2[] = {
    0x00, 0x3b, 0x5d, 0x59, 0xc0, 0x50, 0xaa, 0x75, 0x9f, 0x06, 0x40, 0xe9,
    0x03, 0x21, 0x69, 0x99, 0xa2, 0xce, 0x77, 0x30, 0xd5, 0x62, 0xb3, 0xd7,
    0xe8, 0x52, 0xd1, 0xf0, 0x15, 0xf5, 0x8d, 0xc9, 0x0e, 0x7b, 0x44, 0x19,
    0xc8, 0xeb, 0xc8, 0x80, 0x2a, 0x4f, 0x33, 0xe9, 0xbf, 0xba, 0x58, 0x3c,
    0xd4, 0x40, 0xb4, 0x32, 0x4b, 0xc0, 0x5c, 0x6c, 0xa9, 0x76, 0x51, 0xe0,
    0x30, 0x5c, 0x1a, 0x9b, 0x5f, 0xbc, 0xa0, 0xe0, 0xb9, 0x79, 0x61, 0xd5,
    0xc7, 0x0b, 0xea, 0x40, 0xa9, 0x76, 0x99, 0x79, 0xbb, 0x45, 0x56, 0x9e,
    0x06, 0x1b, 0xb9, 0x92, 0x3d, 0xde, 0xa7, 0x5c, 0x9a, 0xfa, 0x51, 0x33,
    0x5e, 0x81, 0xc8, 0x67, 0x05, 0x7d, 0x16, 0xe2, 0x86, 0x3b, 0xa6, 0x67,
    0x9c, 0x90, 0xba, 0x47, 0x90, 0x1c, 0xc8, 0xa2, 0x60, 0x5d, 0x59, 0xc7,
    0x55, 0x36, 0x5d, 0x9d, 0x40, 0x5f, 0x3f, 0xbb, 0x9c, 0x32, 0x44, 0xc2,
    0x3c, 0x54, 0x21, 0x4e, 0xb4, 0xb3, 0xc6, 0x56, 0x2b, 0xc8, 0x87, 0xe8,
    0xe8, 0xa7, 0x92, 0xeb, 0x02, 0xe7, 0x88, 0x45, 0xb6, 0xb3, 0x53, 0x44,
    0xfd, 0x48, 0x7f, 0xfa, 0xc1, 0x88, 0x3d, 0xfb, 0x97, 0x03, 0x1b, 0x12,
    0x5a, 0x19, 0x3b, 0xf0, 0x32, 0xec, 0x49, 0x35, 0x96, 0x49, 0x70, 0xb5,
    0x44, 0xf5, 0xf2, 0x2a, 0x42, 0x79, 0xd8, 0x87, 0xe1, 0x0c, 0x1a, 0x41,
};

static __aes_test_vect_t aes_256_none_tv[] = {
    {
        {__aes128_none_in1, sizeof(__aes128_none_in1)},
        {__aes256_none_out1, sizeof(__aes256_none_out1)},
        {__aes128_key1, sizeof(__aes128_key1)},
        {TT_AES_PADDING_NONE, TT_AES_MODE_CBC},
    },
    {
        {__aes128_none_in2, sizeof(__aes128_none_in2)},
        {__aes256_none_out2, sizeof(__aes256_none_out2)},
        {__aes128_key2, sizeof(__aes128_key2)},
        {TT_AES_PADDING_NONE, TT_AES_MODE_CBC},
    },
};

TT_TEST_ROUTINE_DEFINE(tt_unit_test_aes256_cbc_none)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_u8_t obuf[sizeof(__aes128_out3)];
    tt_blob_t o_tbuf = {obuf, sizeof(obuf)};
    tt_u8_t obuf2[sizeof(__aes128_out3)];
    tt_blob_t o_tbuf2 = {obuf2, sizeof(obuf2)};
    tt_result_t result;

    tt_u32_t i;

    TT_TEST_CASE_ENTER()
    // test start

    for (i = 0; i < sizeof(aes_256_none_tv) / sizeof(aes_256_none_tv[0]); ++i) {
        __aes_test_vect_t *tv = &aes_256_none_tv[i];

        // enc
        tt_memset(obuf, 0, sizeof(obuf));
        tt_memcpy(&tv->attr.cbc.ivec, &aes_128_tv_iv[i], sizeof(tt_blob_t));
        o_tbuf.len = sizeof(obuf);
        result = tt_aes(TT_TRUE,
                        &tv->key,
                        TT_AES_SIZE_256,
                        &tv->attr,
                        &tv->input,
                        o_tbuf.addr,
                        &o_tbuf.len);
        TT_UT_EQUAL(result, TT_SUCCESS, "");
        TT_UT_EQUAL(o_tbuf.len, tv->output.len, "");
        TT_UT_EQUAL(tt_memcmp(o_tbuf.addr, tv->output.addr, o_tbuf.len), 0, "");

        // dec
        tt_memset(obuf2, 0, sizeof(obuf2));
        o_tbuf2.len = sizeof(obuf2);
        result = tt_aes(TT_FALSE,
                        &tv->key,
                        TT_AES_SIZE_256,
                        &tv->attr,
                        &o_tbuf,
                        o_tbuf2.addr,
                        &o_tbuf2.len);
        // result = tt_aes(TT_FALSE, &o_tbuf,&tv->key,
        //                        TT_AES_SIZE_128,&tv->attr,
        //                        tv->padding,o_tbuf2.addr, &o_tbuf2.len);
        TT_UT_EQUAL(result, TT_SUCCESS, "");
        TT_UT_EQUAL(o_tbuf2.len, tv->input.len, "");
        TT_UT_EQUAL(tt_memcmp(o_tbuf2.addr, tv->input.addr, o_tbuf2.len),
                    0,
                    "");
    }

    for (i = 0; i < sizeof(aes_256_none_tv) / sizeof(aes_256_none_tv[0]); ++i) {
        __aes_test_vect_t *tv = &aes_256_none_tv[i];
        static tt_u8_t __tbuf[1000];
        tt_blob_t ib;
        tt_u32_t len;

        // enc

        // iv
        tt_memcpy(&tv->attr.cbc.ivec, &aes_128_tv_iv[i], sizeof(tt_blob_t));

        // input
        TT_UT_EXP(tv->input.len <= sizeof(__tbuf), "");
        tt_memcpy(__tbuf, tv->input.addr, tv->input.len);
        ib.addr = __tbuf;
        ib.len = tv->input.len;

        // output
        len = sizeof(__tbuf);

        result = tt_aes(TT_TRUE,
                        &tv->key,
                        TT_AES_SIZE_256,
                        &tv->attr,
                        &ib,
                        __tbuf,
                        &len);
        TT_UT_EQUAL(result, TT_SUCCESS, "");
        TT_UT_EQUAL(len, tv->output.len, "");
        TT_UT_EQUAL(tt_memcmp(__tbuf, tv->output.addr, len), 0, "");

        // dec

        // input
        ib.addr = __tbuf;
        ib.len = len;

        // output
        len = sizeof(__tbuf);

        result = tt_aes(TT_FALSE,
                        &tv->key,
                        TT_AES_SIZE_256,
                        &tv->attr,
                        &ib,
                        __tbuf,
                        &len);
        // result = tt_aes(TT_FALSE, &o_tbuf,&tv->key,
        //                        TT_AES_SIZE_128,&tv->attr,
        //                        tv->padding,o_tbuf2.addr, &o_tbuf2.len);
        TT_UT_EQUAL(result, TT_SUCCESS, "");
        TT_UT_EQUAL(len, tv->input.len, "");
        TT_UT_EQUAL(tt_memcmp(__tbuf, tv->input.addr, len), 0, "");
    }

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_aes256_cbc_p7_rep)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_u8_t ibuf[10000], key[16], iv[16];
    tt_u8_t buf1[__aseg_size + TT_AES_BLOCK_SIZE];
    tt_u8_t buf2[__aseg_size + TT_AES_BLOCK_SIZE];
    tt_u32_t ipos = 0, i;
    tt_u32_t ilen, olen, seg_len;
    tt_aes_t enc_aes, dec_aes;
    tt_result_t ret;
    tt_blob_t bkey;
    tt_aes_attr_t ma;

    TT_TEST_CASE_ENTER()
    // test start

    ilen = tt_rand_u32() % sizeof(key);
    if (ilen == 0)
        ++ilen;
    for (i = 0; i < ilen; ++i)
        key[i] = (tt_u8_t)tt_rand_u32();
    bkey.addr = key;
    bkey.len = ilen;

    ilen = tt_rand_u32() % sizeof(iv);
    if (ilen == 0)
        ++ilen;
    for (i = 0; i < ilen; ++i)
        key[i] = (tt_u8_t)tt_rand_u32();
    ma.padding = TT_AES_PADDING_PKCS7;
    ma.mode = TT_AES_MODE_CBC;
    ma.cbc.ivec.addr = iv;
    ma.cbc.ivec.len = ilen;

    ret = tt_aes_create(&enc_aes, TT_TRUE, &bkey, TT_AES_SIZE_256, &ma);
    TT_UT_SUCCESS(ret, "");
    ret = tt_aes_create(&dec_aes, TT_FALSE, &bkey, TT_AES_SIZE_256, &ma);
    TT_UT_SUCCESS(ret, "");

    for (i = 0; i < sizeof(ibuf); ++i)
        ibuf[i] = (tt_u8_t)tt_rand_u32();

    ipos = 0;
    while (ipos < sizeof(ibuf)) {
        if ((ipos + __aseg_size) < sizeof(ibuf)) {
            seg_len = tt_rand_u32() % __aseg_size;
            if (seg_len == 0)
                ++seg_len;
        } else {
            seg_len = sizeof(ibuf) - ipos;
        }

        ilen = seg_len;
        olen = sizeof(buf1);
        ret = tt_aes_encrypt(&enc_aes, &ibuf[ipos], ilen, buf1, &olen);
        TT_UT_SUCCESS(ret, "");

        ilen = olen;
        olen = sizeof(buf2);
        ret = tt_aes_decrypt(&dec_aes, buf1, ilen, buf2, &olen);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(olen, seg_len, "");
        TT_UT_EQUAL(tt_memcmp(buf2, &ibuf[ipos], seg_len), 0, "");

        ipos += seg_len;
    }
    TT_ASSERT(ipos == sizeof(ibuf));

    tt_aes_destroy(&enc_aes);
    tt_aes_destroy(&dec_aes);

    // test end
    TT_TEST_CASE_LEAVE()
}
