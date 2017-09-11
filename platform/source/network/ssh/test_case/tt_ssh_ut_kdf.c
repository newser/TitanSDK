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
TT_TEST_ROUTINE_DECLARE(case_ssh_kdf)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(sshkdf_case)

TT_TEST_CASE("case_ssh_kdf",
             "ssh key derivation function",
             case_ssh_kdf,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE_LIST_DEFINE_END(sshkdf_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(SSH_KDF, 0, sshkdf_case)

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

    ///////////////////////////////////////////////////////
    // [SHA-1]
    // [shared secret length = 1024]
    // [IV length = 64]
    // [encryption key length = 192]

    static tt_u8_t k1[] = {
        0x00, 0x00, 0x00, 0x80, 0x55, 0xba, 0xe9, 0x31, 0xc0, 0x7f, 0xd8, 0x24,
        0xbf, 0x10, 0xad, 0xd1, 0x90, 0x2b, 0x6f, 0xbc, 0x7c, 0x66, 0x53, 0x47,
        0x38, 0x34, 0x98, 0xa6, 0x86, 0x92, 0x9f, 0xf5, 0xa2, 0x5f, 0x8e, 0x40,
        0xcb, 0x66, 0x45, 0xea, 0x81, 0x4f, 0xb1, 0xa5, 0xe0, 0xa1, 0x1f, 0x85,
        0x2f, 0x86, 0x25, 0x56, 0x41, 0xe5, 0xed, 0x98, 0x6e, 0x83, 0xa7, 0x8b,
        0xc8, 0x26, 0x94, 0x80, 0xea, 0xc0, 0xb0, 0xdf, 0xd7, 0x70, 0xca, 0xb9,
        0x2e, 0x7a, 0x28, 0xdd, 0x87, 0xff, 0x45, 0x24, 0x66, 0xd6, 0xae, 0x86,
        0x7c, 0xea, 0xd6, 0x3b, 0x36, 0x6b, 0x1c, 0x28, 0x6e, 0x6c, 0x48, 0x11,
        0xa9, 0xf1, 0x4c, 0x27, 0xae, 0xa1, 0x4c, 0x51, 0x71, 0xd4, 0x9b, 0x78,
        0xc0, 0x6e, 0x37, 0x35, 0xd3, 0x6e, 0x6a, 0x3b, 0xe3, 0x21, 0xdd, 0x5f,
        0xc8, 0x23, 0x08, 0xf3, 0x4e, 0xe1, 0xcb, 0x17, 0xfb, 0xa9, 0x4a, 0x59,
};
static tt_u8_t h1[] = {0xa4, 0xeb, 0xd4, 0x59, 0x34, 0xf5, 0x67,
                       0x92, 0xb5, 0x11, 0x2d, 0xcd, 0x75, 0xa1,
                       0x07, 0x5f, 0xdc, 0x88, 0x92, 0x45};
static tt_u8_t s1[] = {0xa4, 0xeb, 0xd4, 0x59, 0x34, 0xf5, 0x67,
                       0x92, 0xb5, 0x11, 0x2d, 0xcd, 0x75, 0xa1,
                       0x07, 0x5f, 0xdc, 0x88, 0x92, 0x45};

static tt_u8_t iv_c2s_1[] = {0xe2, 0xf6, 0x27, 0xc0, 0xb4, 0x3f, 0x1a, 0xc1};
static tt_u8_t iv_s2c_1[] = {0x58, 0x47, 0x14, 0x45, 0xf3, 0x42, 0xb1, 0x81};
static tt_u8_t enc_c2s_1[] = {0x1c, 0xa9, 0xd3, 0x10, 0xf8, 0x6d, 0x51, 0xf6,
                              0xcb, 0x8e, 0x70, 0x07, 0xcb, 0x2b, 0x22, 0x0d,
                              0x55, 0xc5, 0x28, 0x1c, 0xe6, 0x80, 0xb5, 0x33};
static tt_u8_t enc_s2c_1[] = {0x2c, 0x60, 0xdf, 0x86, 0x03, 0xd3, 0x4c, 0xc1,
                              0xdb, 0xb0, 0x3c, 0x11, 0xf7, 0x25, 0xa4, 0x4b,
                              0x44, 0x00, 0x88, 0x51, 0xc7, 0x3d, 0x68, 0x44};
static tt_u8_t mac_c2s_1[] = {0x47, 0x2e, 0xb8, 0xa2, 0x61, 0x66, 0xae,
                              0x6a, 0xa8, 0xe0, 0x68, 0x68, 0xe4, 0x5c,
                              0x3b, 0x26, 0xe6, 0xee, 0xed, 0x06};
static tt_u8_t mac_s2c_1[] = {0xe3, 0xe2, 0xfd, 0xb9, 0xd7, 0xbc, 0x21,
                              0x16, 0x5a, 0x3d, 0xbe, 0x47, 0xe1, 0xec,
                              0xeb, 0x77, 0x64, 0x39, 0x0b, 0xab};

///////////////////////////////////////////////////////
// [SHA-1]
// [shared secret length = 2048]
// [IV length = 128]
// [encryption key length = 128]

static tt_u8_t k2[] =
    {0x00, 0x00, 0x01, 0x01, 0x00, 0x85, 0xa6, 0x0b, 0xcc, 0xa8, 0x8b, 0x09,
     0x6c, 0x41, 0x8e, 0x82, 0x5f, 0x3d, 0xe4, 0xfd, 0x89, 0x20, 0xec, 0xce,
     0x61, 0x7a, 0xfa, 0xdd, 0xa2, 0xca, 0x80, 0x01, 0xe8, 0xeb, 0xa0, 0x7e,
     0x58, 0xe6, 0x0e, 0x70, 0xa9, 0xa0, 0x5b, 0x8d, 0xdc, 0x29, 0xd5, 0x63,
     0x6d, 0x33, 0xc4, 0x07, 0xd5, 0xf2, 0x39, 0x52, 0xb6, 0x53, 0x26, 0xe1,
     0x13, 0xf2, 0x8d, 0x89, 0xbc, 0x6b, 0xa3, 0xa4, 0xc3, 0xb7, 0x1a, 0xe2,
     0x39, 0xd6, 0xd1, 0xbd, 0x29, 0x54, 0x66, 0x68, 0x2d, 0x1c, 0x67, 0x5b,
     0xdb, 0x88, 0xa3, 0x25, 0x9f, 0x47, 0x4f, 0xe5, 0x4a, 0x0f, 0x40, 0x04,
     0xdd, 0xc4, 0x6b, 0x44, 0x24, 0x51, 0x65, 0x4e, 0x1e, 0x66, 0xd0, 0xc9,
     0x3d, 0x7b, 0x31, 0x0f, 0x28, 0xa8, 0xdb, 0x7b, 0x95, 0xee, 0xe7, 0xab,
     0xc6, 0x1e, 0x71, 0xde, 0xe3, 0x22, 0xb4, 0xe7, 0x32, 0xba, 0xf9, 0xec,
     0x7c, 0xe1, 0x89, 0xb8, 0x89, 0xd5, 0x36, 0xda, 0x1a, 0x55, 0xa2, 0xcc,
     0x29, 0xe1, 0x66, 0x6a, 0xa9, 0xc0, 0xe7, 0x02, 0xf4, 0x41, 0x22, 0x06,
     0xbd, 0x20, 0x73, 0x02, 0xfe, 0x84, 0x04, 0x3c, 0x66, 0x43, 0x94, 0xbd,
     0xe0, 0xe0, 0xa4, 0x7d, 0x0a, 0x7a, 0x94, 0x7c, 0x95, 0x99, 0x7e, 0x1d,
     0xba, 0xee, 0xcd, 0x2e, 0xfa, 0xe1, 0x2c, 0xac, 0xef, 0x8e, 0xab, 0x2f,
     0x6b, 0x24, 0x78, 0xde, 0xdc, 0xeb, 0xb3, 0x26, 0x48, 0x27, 0xcf, 0x22,
     0x6e, 0x13, 0xf8, 0x08, 0x29, 0x31, 0xdb, 0x41, 0x0f, 0xbc, 0x03, 0x35,
     0x2e, 0x7d, 0xde, 0x82, 0xfd, 0x1f, 0x58, 0xca, 0xab, 0x31, 0x15, 0xaa,
     0x06, 0x5a, 0xc6, 0xe2, 0xa1, 0xc7, 0xb1, 0xc1, 0xb2, 0xd5, 0xfa, 0x34,
     0x47, 0xbf, 0x98, 0x39, 0xd7, 0x6c, 0xfa, 0x58, 0x22, 0xb0, 0x97, 0xbf,
     0xf9, 0x10, 0x6f, 0x37, 0xeb, 0xa1, 0x25, 0x01, 0x45};
static tt_u8_t h2[] = {0xdd, 0xe6, 0xf8, 0xe0, 0x70, 0xef, 0x32,
                       0xa2, 0x7f, 0xf0, 0x4a, 0xd1, 0x04, 0x5c,
                       0x65, 0xb2, 0xdf, 0xa3, 0x3e, 0x03};
static tt_u8_t s2[] = {0xdd, 0xe6, 0xf8, 0xe0, 0x70, 0xef, 0x32,
                       0xa2, 0x7f, 0xf0, 0x4a, 0xd1, 0x04, 0x5c,
                       0x65, 0xb2, 0xdf, 0xa3, 0x3e, 0x03};

static tt_u8_t iv_c2s_2[] = {0x79,
                             0xc9,
                             0x19,
                             0x5e,
                             0x68,
                             0x3a,
                             0xe1,
                             0x07,
                             0x50,
                             0x96,
                             0x0c,
                             0xb5,
                             0x5c,
                             0x4d,
                             0x4c,
                             0x0b};
static tt_u8_t iv_s2c_2[] = {0xef,
                             0x00,
                             0xb4,
                             0x48,
                             0xab,
                             0x9f,
                             0xd6,
                             0x52,
                             0x3b,
                             0xb5,
                             0x14,
                             0x3a,
                             0x0a,
                             0x81,
                             0x87,
                             0x50};
static tt_u8_t enc_c2s_2[] = {0x51,
                              0xc8,
                              0xb4,
                              0xaa,
                              0xf5,
                              0xe4,
                              0x24,
                              0x43,
                              0xbe,
                              0x0a,
                              0xa3,
                              0xc5,
                              0x0a,
                              0xa7,
                              0xe1,
                              0xdd};
static tt_u8_t enc_s2c_2[] = {0x41,
                              0x53,
                              0xa5,
                              0x87,
                              0x39,
                              0x7f,
                              0xb1,
                              0x4d,
                              0xc3,
                              0xfa,
                              0xad,
                              0x02,
                              0x8f,
                              0xdb,
                              0x7e,
                              0xcc};
static tt_u8_t mac_c2s_2[] = {0xd2, 0x3e, 0x36, 0x34, 0x70, 0x52, 0xa1,
                              0xcf, 0xb4, 0xa7, 0x78, 0x9d, 0xf4, 0x86,
                              0x27, 0xe8, 0xa3, 0x13, 0x45, 0xc7};
static tt_u8_t mac_s2c_2[] = {0xc1, 0x28, 0x6e, 0x92, 0x65, 0x59, 0x12,
                              0xd9, 0x23, 0x15, 0x4c, 0x46, 0x07, 0x02,
                              0xa3, 0x14, 0x24, 0xbd, 0x6b, 0x01};

///////////////////////////////////////////////////////

typedef struct _sshkdf_tv_t
{
    tt_u8_t *k;
    tt_u32_t k_len;
    tt_u8_t *h;
    tt_u32_t h_len;
    tt_u8_t *s;
    tt_u32_t s_len;

    tt_u8_t *iv_c2s;
    tt_u32_t iv_c2s_len;
    tt_u8_t *iv_s2c;
    tt_u32_t iv_s2c_len;
    tt_u8_t *enc_c2s;
    tt_u32_t enc_c2s_len;
    tt_u8_t *enc_s2c;
    tt_u32_t enc_s2c_len;
    tt_u8_t *mac_c2s;
    tt_u32_t mac_c2s_len;
    tt_u8_t *mac_s2c;
    tt_u32_t mac_s2c_len;
} sshkdf_tv_t;

sshkdf_tv_t sshkdf_tv[] = {
    {
        k1,
        sizeof(k1),
        h1,
        sizeof(h1),
        s1,
        sizeof(s1),

        iv_c2s_1,
        sizeof(iv_c2s_1),
        iv_s2c_1,
        sizeof(iv_s2c_1),
        enc_c2s_1,
        sizeof(enc_c2s_1),
        enc_s2c_1,
        sizeof(enc_s2c_1),
        mac_c2s_1,
        sizeof(mac_c2s_1),
        mac_s2c_1,
        sizeof(mac_s2c_1),
    },
    {
        k2,
        sizeof(k2),
        h2,
        sizeof(h2),
        s2,
        sizeof(s2),

        iv_c2s_2,
        sizeof(iv_c2s_2),
        iv_s2c_2,
        sizeof(iv_s2c_2),
        enc_c2s_2,
        sizeof(enc_c2s_2),
        enc_s2c_2,
        sizeof(enc_s2c_2),
        mac_c2s_2,
        sizeof(mac_c2s_2),
        mac_s2c_2,
        sizeof(mac_s2c_2),
    },
};

TT_TEST_ROUTINE_DEFINE(case_ssh_kdf)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    tt_sshkdf_t kdf;
    tt_u32_t i;

    TT_TEST_CASE_ENTER()
    // test start


    for (i = 0; i < sizeof(sshkdf_tv) / sizeof(sshkdf_tv[0]); ++i) {
        sshkdf_tv_t *t = &sshkdf_tv[i];
        tt_blob_t k, h, s;

        tt_sshkdf_init(&kdf, TT_SSH_HASH_ALG_SHA1);

        k.addr = t->k;
        k.len = t->k_len;
        h.addr = t->h;
        h.len = t->h_len;
        s.addr = t->s;
        s.len = t->s_len;

        ret = tt_sshkdf_run(&kdf,
                            &k,
                            &h,
                            &s,
                            t->iv_c2s_len,
                            t->iv_s2c_len,
                            t->enc_c2s_len,
                            t->enc_s2c_len,
                            t->mac_c2s_len,
                            t->mac_s2c_len);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");

        TT_UT_EXP(TT_BUF_RLEN(&kdf.iv_c2s) >= t->iv_c2s_len, "");
        TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(&kdf.iv_c2s),
                              t->iv_c2s,
                              t->iv_c2s_len),
                    0,
                    "");

        TT_UT_EXP(TT_BUF_RLEN(&kdf.iv_s2c) >= t->iv_s2c_len, "");
        TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(&kdf.iv_s2c),
                              t->iv_s2c,
                              t->iv_s2c_len),
                    0,
                    "");

        TT_UT_EXP(TT_BUF_RLEN(&kdf.enc_c2s) >= t->enc_c2s_len, "");
        TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(&kdf.enc_c2s),
                              t->enc_c2s,
                              t->enc_c2s_len),
                    0,
                    "");

        TT_UT_EXP(TT_BUF_RLEN(&kdf.enc_s2c) >= t->enc_s2c_len, "");
        TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(&kdf.enc_s2c),
                              t->enc_s2c,
                              t->enc_s2c_len),
                    0,
                    "");

        TT_UT_EXP(TT_BUF_RLEN(&kdf.mac_c2s) >= t->mac_c2s_len, "");
        TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(&kdf.mac_c2s),
                              t->mac_c2s,
                              t->mac_c2s_len),
                    0,
                    "");

        TT_UT_EXP(TT_BUF_RLEN(&kdf.mac_s2c) >= t->mac_s2c_len, "");
        TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(&kdf.mac_s2c),
                              t->mac_s2c,
                              t->mac_s2c_len),
                    0,
                    "");

        tt_sshkdf_destroy(&kdf);
    }


    // test end
    TT_TEST_CASE_LEAVE()
}
