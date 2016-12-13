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
    tt_blob_t key;
    tt_blob_t output;
} __hmac_test_vect_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

// === routine declarations ================
TT_TEST_ROUTINE_DECLARE(tt_unit_test_hmac_sha1)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(crypto_hmac_case)

TT_TEST_CASE("tt_unit_test_hmac_sha1",
             "crypto: HMAC SHA1",
             tt_unit_test_hmac_sha1,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE_LIST_DEFINE_END(crypto_hmac_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(CRYPTO_UT_HMAC, 0, crypto_hmac_case)

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

    static tt_char_t hmac_input1[] = "Hi There";
static tt_u8_t hmac_key1[] = {
    0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
    0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
};
static tt_u8_t hmac_digest1_sha1[] = {0xb6, 0x17, 0x31, 0x86, 0x55, 0x05, 0x72,
                                      0x64, 0xe2, 0x8b, 0xc0, 0xb6, 0xfb, 0x37,
                                      0x8c, 0x8e, 0xf1, 0x46, 0xbe, 0x00};

static tt_char_t hmac_input2[] = "what do ya want for nothing?";
static tt_u8_t hmac_key2[] = {
    'J', 'e', 'f', 'e',
};
static tt_u8_t hmac_digest2_sha1[] = {0xef, 0xfc, 0xdf, 0x6a, 0xe5, 0xeb, 0x2f,
                                      0xa2, 0xd2, 0x74, 0x16, 0xd5, 0xf1, 0x84,
                                      0xdf, 0x9c, 0x25, 0x9a, 0x7c, 0x79};

static tt_u8_t hmac_input3[] = {
    0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
    0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
    0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
    0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
    0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
};
static tt_u8_t hmac_key3[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                              0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
                              0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
                              0x16, 0x17, 0x18, 0x19};
static tt_u8_t hmac_digest3_sha1[] = {0x4c, 0x90, 0x07, 0xf4, 0x02, 0x62, 0x50,
                                      0xc6, 0xbc, 0x84, 0x14, 0xf9, 0xbf, 0x50,
                                      0xc8, 0x6c, 0x2d, 0x72, 0x35, 0xda};

static tt_char_t hmac_input4[] =
    "Test Using Larger Than Block-Size Key and Larger Than One Block-Size Data";
static tt_u8_t hmac_key4[] = {
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
};
static tt_u8_t hmac_digest4_sha1[] = {0xe8, 0xe9, 0x9d, 0x0f, 0x45, 0x23, 0x7d,
                                      0x78, 0x6d, 0x6b, 0xba, 0xa7, 0x96, 0x5c,
                                      0x78, 0x08, 0xbb, 0xff, 0x1a, 0x91};

// sha1 test vector
static __hmac_test_vect_t __hmac_sha1_tv[] = {
    {
        {(tt_u8_t *)hmac_input1, sizeof(hmac_input1) - 1},
        {hmac_key1, sizeof(hmac_key1)},
        {hmac_digest1_sha1, sizeof(hmac_digest1_sha1)},
    },
    {
        {(tt_u8_t *)hmac_input2, sizeof(hmac_input2) - 1},
        {hmac_key2, sizeof(hmac_key2)},
        {hmac_digest2_sha1, sizeof(hmac_digest2_sha1)},
    },
    {
        {(tt_u8_t *)hmac_input3, sizeof(hmac_input3)},
        {hmac_key3, sizeof(hmac_key3)},
        {hmac_digest3_sha1, sizeof(hmac_digest3_sha1)},
    },
    {
        {(tt_u8_t *)hmac_input4, sizeof(hmac_input4) - 1},
        {hmac_key4, sizeof(hmac_key4)},
        {hmac_digest4_sha1, sizeof(hmac_digest4_sha1)},
    },
};

TT_TEST_ROUTINE_DEFINE(tt_unit_test_hmac_sha1)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_buf_t output;
    __hmac_test_vect_t *tv;
    tt_u32_t n = sizeof(__hmac_sha1_tv) / sizeof(__hmac_sha1_tv[0]);
    tt_u32_t i, k = 0;
    tt_hmac_t hmac;
    tt_result_t ret;

    TT_TEST_CASE_ENTER()
    // test start

    for (i = 0; i < n; ++i) {
        tv = &__hmac_sha1_tv[i];

        ret = tt_hmac_create(&hmac, TT_HMAC_VER_SHA1, &tv->key);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    sag:

        ret = tt_hmac_update(&hmac, (tt_u8_t *)tv->input.addr, 0);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        ret = tt_hmac_update(&hmac, (tt_u8_t *)tv->input.addr, tv->input.len);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        if (i % 4 == 0) {
            tt_buf_init(&output, NULL);

            ret = tt_hmac_final_buf(&hmac, &output);
            TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

            TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&output),
                                TT_HMAC_SHA1_DIGEST_LENGTH,
                                "");
            TT_TEST_CHECK_EQUAL(tt_memcmp(TT_BUF_RPOS(&output),
                                          tv->output.addr,
                                          TT_HMAC_SHA1_DIGEST_LENGTH),
                                0,
                                "");

            tt_buf_destroy(&output);
        } else if (i % 4 == 1) {
            tt_u8_t hmac_sha1_ret[TT_HMAC_SHA1_DIGEST_LENGTH];
            ret = tt_hmac_final(&hmac, hmac_sha1_ret);
            TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

            TT_TEST_CHECK_EQUAL(0,
                                tt_memcmp(hmac_sha1_ret,
                                          tv->output.addr,
                                          TT_HMAC_SHA1_DIGEST_LENGTH),
                                "");
        } else if (i % 4 == 2) {
            tt_u8_t hmac_sha1_ret[TT_HMAC_SHA1_DIGEST_LENGTH];
            ret =
                tt_hmac(TT_HMAC_VER_SHA1, &tv->key, &tv->input, hmac_sha1_ret);
            TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

            TT_TEST_CHECK_EQUAL(0,
                                tt_memcmp(hmac_sha1_ret,
                                          tv->output.addr,
                                          TT_HMAC_SHA1_DIGEST_LENGTH),
                                "");
        } else {
            tt_blob_t bufs[2];
            tt_u32_t num;
            tt_u8_t hmac_sha1_ret[TT_HMAC_SHA1_DIGEST_LENGTH];

            if (tv->input.len > 5) {
                bufs[0].addr = tv->input.addr;
                bufs[0].len = 5;
                bufs[1].addr = tv->input.addr + 5;
                bufs[1].len = tv->input.len - 5;
                num = 2;
            } else {
                bufs[0].addr = tv->input.addr;
                bufs[0].len = tv->input.len;
                num = 1;
            }

            ret = tt_hmac_gather(TT_HMAC_VER_SHA1,
                                 &tv->key,
                                 bufs,
                                 num,
                                 hmac_sha1_ret);
            TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

            TT_TEST_CHECK_EQUAL(0,
                                tt_memcmp(hmac_sha1_ret,
                                          tv->output.addr,
                                          TT_HMAC_SHA1_DIGEST_LENGTH),
                                "");
        }

        if (k++ % 2 == 0) {
            tt_hmac_reset(&hmac);
            goto sag;
        } else {
            tt_hmac_destroy(&hmac);
        }
    }
    // test end
    TT_TEST_CASE_LEAVE()
}
