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
TT_TEST_ROUTINE_DECLARE(case_sshenc_aes128)
TT_TEST_ROUTINE_DECLARE(case_sshenc_aes256)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(sshenc_case)

TT_TEST_CASE("case_sshenc_aes128",
             "ssh encrypt & decrypt: aes128",
             case_sshenc_aes128,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("case_sshenc_aes256",
                 "ssh encrypt & decrypt: aes256",
                 case_sshenc_aes256,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(sshenc_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(SSH_ENC, 0, sshenc_case)

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

    static tt_u8_t __aes128_in1[] = {
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
static tt_u8_t __aes128_out1[] = {
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
static tt_u8_t __aes128_key1[] = {0xab, 0xc1, 0x23};
static tt_u8_t __aes128_iv1[] = {0x12,
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

TT_TEST_ROUTINE_DEFINE(case_sshenc_aes128)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    tt_sshenc_t enc, dec;
    tt_blob_t iv, key;
    tt_buf_t buf;

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&buf, NULL);

    tt_sshenc_init(&enc);
    TT_UT_EQUAL(enc.alg, TT_SSH_ENC_ALG_NUM, "");
    tt_sshenc_init(&dec);
    TT_UT_EQUAL(dec.alg, TT_SSH_ENC_ALG_NUM, "");

    iv.addr = __aes128_iv1;
    iv.len = sizeof(__aes128_iv1);

    key.addr = __aes128_key1;
    key.len = sizeof(__aes128_key1);

    tt_buf_put(&buf, __aes128_in1, sizeof(__aes128_in1));

    // can not encrypt
    ret = tt_sshenc_encrypt(&enc, TT_BUF_RPOS(&buf), TT_BUF_RLEN(&buf));
    TT_UT_EQUAL(ret, TT_FAIL, "");

    // can not decrypt
    ret = tt_sshenc_decrypt(&dec, TT_BUF_RPOS(&buf), TT_BUF_RLEN(&buf));
    TT_UT_EQUAL(ret, TT_FAIL, "");

    // set alg to aes128
    ret = tt_sshenc_setalg(&enc,
                           TT_SSH_ENC_ALG_AES128_CBC,
                           TT_TRUE,
                           iv.addr,
                           iv.len,
                           key.addr,
                           key.len);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(enc.alg, TT_SSH_ENC_ALG_AES128_CBC, "");
    TT_UT_NOT_EQUAL(enc.block_len, 0, "");

    ret = tt_sshenc_setalg(&dec,
                           TT_SSH_ENC_ALG_AES128_CBC,
                           TT_FALSE,
                           iv.addr,
                           iv.len,
                           key.addr,
                           key.len);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(dec.alg, TT_SSH_ENC_ALG_AES128_CBC, "");
    TT_UT_NOT_EQUAL(dec.block_len, 0, "");

    // can not set again
    ret = tt_sshenc_setalg(&enc,
                           TT_SSH_ENC_ALG_AES256_CBC,
                           TT_TRUE,
                           iv.addr,
                           iv.len,
                           key.addr,
                           key.len);
    TT_UT_NOT_EQUAL(ret, TT_SUCCESS, "");

    // can encrypt now
    ret = tt_sshenc_encrypt(&enc, TT_BUF_RPOS(&buf), TT_BUF_RLEN(&buf));
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(TT_BUF_RLEN(&buf), sizeof(__aes128_out1), "");
    TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(&buf),
                          __aes128_out1,
                          sizeof(__aes128_out1)),
                0,
                "");

    // encrypt 0
    ret = tt_sshenc_encrypt(&enc, TT_BUF_RPOS(&buf), 0);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    // can decrypt now
    ret = tt_sshenc_decrypt(&dec, TT_BUF_RPOS(&buf), TT_BUF_RLEN(&buf));
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(TT_BUF_RLEN(&buf), sizeof(__aes128_in1), "");
    TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(&buf),
                          __aes128_in1,
                          sizeof(__aes128_in1)),
                0,
                "");

    // decrypt 0
    ret = tt_sshenc_decrypt(&dec, TT_BUF_RPOS(&buf), 0);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_sshenc_destroy(&enc);
    tt_sshenc_destroy(&dec);
    tt_buf_destroy(&buf);

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_u8_t __aes256_out2[] = {
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

TT_TEST_ROUTINE_DEFINE(case_sshenc_aes256)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    tt_sshenc_t enc, dec;
    tt_blob_t iv, key;
    tt_buf_t buf;

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&buf, NULL);

    tt_sshenc_init(&enc);
    TT_UT_EQUAL(enc.alg, TT_SSH_ENC_ALG_NUM, "");
    tt_sshenc_init(&dec);
    TT_UT_EQUAL(dec.alg, TT_SSH_ENC_ALG_NUM, "");

    iv.addr = __aes128_iv1;
    iv.len = sizeof(__aes128_iv1);

    key.addr = __aes128_key1;
    key.len = sizeof(__aes128_key1);

    tt_buf_put(&buf, __aes128_in1, sizeof(__aes128_in1));

    // can not encrypt
    ret = tt_sshenc_encrypt(&enc, TT_BUF_RPOS(&buf), TT_BUF_RLEN(&buf));
    TT_UT_EQUAL(ret, TT_FAIL, "");

    // can not decrypt
    ret = tt_sshenc_decrypt(&enc, TT_BUF_RPOS(&buf), TT_BUF_RLEN(&buf));
    TT_UT_EQUAL(ret, TT_FAIL, "");

    // set alg to aes256
    ret = tt_sshenc_setalg(&enc,
                           TT_SSH_ENC_ALG_AES256_CBC,
                           TT_TRUE,
                           iv.addr,
                           iv.len,
                           key.addr,
                           key.len);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(enc.alg, TT_SSH_ENC_ALG_AES256_CBC, "");
    TT_UT_NOT_EQUAL(enc.block_len, 0, "");

    ret = tt_sshenc_setalg(&dec,
                           TT_SSH_ENC_ALG_AES256_CBC,
                           TT_FALSE,
                           iv.addr,
                           iv.len,
                           key.addr,
                           key.len);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(dec.alg, TT_SSH_ENC_ALG_AES256_CBC, "");
    TT_UT_NOT_EQUAL(dec.block_len, 0, "");

    // can not set again
    ret = tt_sshenc_setalg(&enc,
                           TT_SSH_ENC_ALG_AES128_CBC,
                           TT_TRUE,
                           iv.addr,
                           iv.len,
                           key.addr,
                           key.len);
    TT_UT_NOT_EQUAL(ret, TT_SUCCESS, "");

    // can encrypt now
    ret = tt_sshenc_encrypt(&enc, TT_BUF_RPOS(&buf), TT_BUF_RLEN(&buf));
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(TT_BUF_RLEN(&buf), sizeof(__aes256_out2), "");
    TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(&buf),
                          __aes256_out2,
                          sizeof(__aes256_out2)),
                0,
                "");

    // can decrypt now
    ret = tt_sshenc_decrypt(&dec, TT_BUF_RPOS(&buf), TT_BUF_RLEN(&buf));
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(TT_BUF_RLEN(&buf), sizeof(__aes128_in1), "");
    TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(&buf),
                          __aes128_in1,
                          sizeof(__aes128_in1)),
                0,
                "");

    tt_sshenc_destroy(&enc);
    tt_sshenc_destroy(&dec);
    tt_buf_destroy(&buf);

    // test end
    TT_TEST_CASE_LEAVE()
}
