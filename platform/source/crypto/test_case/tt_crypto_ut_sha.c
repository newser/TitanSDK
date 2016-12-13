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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_sha1)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_md5)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(crypto_case)

TT_TEST_CASE("tt_unit_test_sha1",
             "crypto: SHA1",
             tt_unit_test_sha1,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_md5",
                 "crypto: MD5",
                 tt_unit_test_md5,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(crypto_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(CRYPTO_UT_SHA, 0, crypto_case)

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

    // sha1 test vector
    static __c_test_vect_t __sha1_tv[] = {
        {"The quick brown fox jumps over the lazy dog",
         "2fd4e1c67a2d28fced849ee1bb76e7391b93eb12"},
        {"Test vector from febooti.com",
         "a7631795f6d59cd6d14ebd0058a6394a4b93d868"},
        {"", "da39a3ee5e6b4b0d3255bfef95601890afd80709"},
};

TT_TEST_ROUTINE_DEFINE(tt_unit_test_sha1)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_buf_t output, expect;
    __c_test_vect_t *tv;
    tt_u32_t n = sizeof(__sha1_tv) / sizeof(__sha1_tv[0]);
    tt_u32_t i;
    tt_sha_t sha1;
    tt_result_t ret;

    TT_TEST_CASE_ENTER()
    // test start

    for (i = 0; i < n; ++i) {
        tv = &__sha1_tv[i];

        ret = tt_sha_create(&sha1, TT_SHA_VER_SHA1);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        // update 0
        ret = tt_sha_update(&sha1, (tt_u8_t *)tv->input, 0);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        ret = tt_sha_update(&sha1,
                            (tt_u8_t *)tv->input,
                            (tt_u32_t)strlen(tv->input));
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        if (i % 2 == 0) {
            tt_buf_init(&output, NULL);
            tt_buf_init(&expect, NULL);

            ret = tt_sha_final_buf(&sha1, &output);
            TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

            tt_buf_put_cstr2hex(&expect, tv->output);
            TT_TEST_CHECK_EQUAL(0, tt_buf_cmp(&output, &expect), "");

            tt_buf_destroy(&expect);
            tt_buf_destroy(&output);
        } else {
            tt_u8_t sha1_ret[TT_SHA1_DIGEST_LENGTH];
            ret = tt_sha_final(&sha1, sha1_ret);
            TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

            tt_buf_init(&expect, NULL);
            tt_buf_put_hex2cstr(&expect, sha1_ret, TT_SHA1_DIGEST_LENGTH);

            TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&expect),
                                (tt_u32_t)tt_strlen(tv->output),
                                "");
            TT_TEST_CHECK_EQUAL(0,
                                tt_memcmp(TT_BUF_RPOS(&expect),
                                          tv->output,
                                          TT_SHA1_DIGEST_LENGTH),
                                "");
        }

        tt_sha_destroy(&sha1);
    }
    // test end
    TT_TEST_CASE_LEAVE()
}

// md5 test vector
static __c_test_vect_t __md5_tv[] = {
    {"The quick brown fox jumps over the lazy dog",
     "9e107d9d372bb6826bd81d3542a419d6"},
    {"Test vector from febooti.com", "500ab6613c6db7fbd30c62f5ff573d0f"},
    {"", "d41d8cd98f00b204e9800998ecf8427e"},
};

TT_TEST_ROUTINE_DEFINE(tt_unit_test_md5)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_buf_t output, expect;
    __c_test_vect_t *tv;
    tt_u32_t n = sizeof(__md5_tv) / sizeof(__md5_tv[0]);
    tt_u32_t i;
    tt_md5_t md5;
    tt_result_t ret;

    TT_TEST_CASE_ENTER()
    // test start

    for (i = 0; i < n; ++i) {
        tv = &__md5_tv[i];

        ret = tt_md5_create(&md5);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        ret = tt_md5_update(&md5,
                            (tt_u8_t *)tv->input,
                            (tt_u32_t)strlen(tv->input));
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        if (i % 2 == 0) {
            tt_buf_init(&output, NULL);
            tt_buf_init(&expect, NULL);

            ret = tt_md5_final_buf(&md5, &output);
            TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

            tt_buf_put_cstr2hex(&expect, tv->output);
            TT_TEST_CHECK_EQUAL(0, tt_buf_cmp(&output, &expect), "");

            tt_buf_destroy(&expect);
            tt_buf_destroy(&output);
        } else {
            tt_u8_t md5_ret[TT_MD5_DIGEST_LENGTH];
            ret = tt_md5_final(&md5, md5_ret);
            TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

            tt_buf_init(&expect, NULL);
            tt_buf_put_hex2cstr(&expect, md5_ret, TT_MD5_DIGEST_LENGTH);

            TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&expect),
                                (tt_u32_t)tt_strlen(tv->output),
                                "");
            TT_TEST_CHECK_EQUAL(0,
                                tt_memcmp(TT_BUF_RPOS(&expect),
                                          tv->output,
                                          TT_SHA1_DIGEST_LENGTH),
                                "");
        }

        tt_md5_destroy(&md5);
    }
    // test end
    TT_TEST_CASE_LEAVE()
}
