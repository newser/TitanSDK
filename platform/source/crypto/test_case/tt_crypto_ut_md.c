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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_md)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(crypto_md_case)

TT_TEST_CASE("tt_unit_test_md",
             "crypto: message digest",
             tt_unit_test_md,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE_LIST_DEFINE_END(crypto_md_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(CRYPTO_UT_MD, 0, crypto_md_case)

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

    typedef struct
{
    const tt_char_t *in;
    const tt_char_t *out[TT_MD_TYPE_NUM];
} __md_tv_t;

// md test vector
static __md_tv_t __md_tv[] = {
    {"The quick brown fox jumps over the lazy dog",
     {
         /* md2 */ "03d85a0d629d2c442e987525319fc471",
         /* md4 */ "1bee69a46ba811185c194762abaeae90",
         /* md5 */ "9e107d9d372bb6826bd81d3542a419d6",
         /* sha1 */ "2fd4e1c67a2d28fced849ee1bb76e7391b93eb12",
         /* sha224 */
         "730e109bd7a8a32b1cb9d9a09aa2325d2430587ddbc0c38bad911525",
         /* sha256 */
         "d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592",
         /* sha384 */ "ca737f1014a48f4c0b6dd43cb177b0afd9e5169367544c49"
                      "4011e3317dbf9a509cb1e5dc1e85a941bbee3d7f2afbc9b1",
         /* sha512 */
         "07e547d9586f6a73f73fbac0435ed76951218fb7d0c8d788a309d785436bbb64"
         "2e93a252a954f23912547d1e8a3b5ed6e1bfd7097821233fa0538f3db854fee6",
         /* ripemd160 */ "37f332f68db77bd9d7edd4969571ad671cf9dd3b",
     }},
    {
        "Test vector from febooti.com",
        {
            /* md2 */ "db128d6e0d20a1192a6bd1fade401150",
            /* md4 */ "6578f2664bc56e0b5b3f85ed26ecc67b",
            /* md5 */ "500ab6613c6db7fbd30c62f5ff573d0f",
            /* sha1 */ "a7631795f6d59cd6d14ebd0058a6394a4b93d868",
            /* sha224 */
            "3628b402254caa96827e3c79c0a559e4558da8ee2b65f1496578137d",
            /* sha256 */
            "077b18fe29036ada4890bdec192186e10678597a67880290521df70df4bac9ab",
            /* sha384 */ "388bb2d487de48740f45fcb44152b0b665428c49def1aaf7"
                         "c7f09a40c10aff1cd7c3fe3325193c4dd35d4eaa032f49b0",
            /* sha512 */
            "09fb898bc97319a243a63f6971747f8e102481fb8d5346c55cb44855adc2e0e9"
            "8f304e552b0db1d4eeba8a5c8779f6a3010f0e1a2beb5b9547a13b6edca11e8a",
            /* ripemd160 */ "4e1ff644ca9f6e86167ccb30ff27e0d84ceb2a61",
        },
    },
    {
        "",
        {
            /* md2 */ "8350e5a3e24c153df2275c9f80692773",
            /* md4 */ "31d6cfe0d16ae931b73c59d7e0c089c0",
            /* md5 */ "d41d8cd98f00b204e9800998ecf8427e",
            /* sha1 */ "da39a3ee5e6b4b0d3255bfef95601890afd80709",
            /* sha224 */
            "d14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f",
            /* sha256 */
            "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855",
            /* sha384 */ "38b060a751ac96384cd9327eb1b1e36a21fdb71114be0743"
                         "4c0cc7bf63f6e1da274edebfe76f65fbd51ad2f14898b95b",
            /* sha512 */
            "cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce"
            "47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e",
            /* ripemd160 */ "9c1185a5c5e9fc54612808977ee8f548b2258d31",
        },
    },
};

TT_TEST_ROUTINE_DEFINE(tt_unit_test_md)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_buf_t output, expect;
    __md_tv_t *tv;
    tt_u32_t n = sizeof(__md_tv) / sizeof(__md_tv[0]);
    tt_u32_t i;
    tt_result_t ret;

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&output, NULL);
    tt_buf_init(&expect, NULL);

    for (i = 0; i < 1; ++i) {
        tt_md_t md;
        tt_md_type_t t = TT_MD2;
        tt_u32_t len;

        tv = &__md_tv[i];
        len = (tt_u32_t)tt_strlen(tv->in);

        for (; t < TT_MD_TYPE_NUM; ++t) {
            tt_u32_t pos, seg;

            tt_buf_clear(&expect);
            tt_buf_put_cstr2hex(&expect, tv->out[t]);

            ret = tt_md_create(&md, t);
            TT_UT_SUCCESS(ret, "");

            pos = 0;
            while (pos < len) {
                seg = tt_rand_u32() % (len - pos) + 1;
                ret = tt_md_update(&md, (tt_u8_t *)tv->in + pos, seg);
                TT_UT_SUCCESS(ret, "");
                pos += seg;
            }
            TT_ASSERT(pos == len);

            tt_buf_clear(&output);
            ret = tt_md_final_buf(&md, &output);
            TT_UT_SUCCESS(ret, "");
            TT_UT_EQUAL(TT_BUF_RLEN(&output), tt_md_size(&md), "");
            TT_UT_EQUAL(tt_buf_cmp(&output, &expect), 0, "");

            // reset
            tt_md_reset(&md);
            ret = tt_md_update(&md, (tt_u8_t *)tv->in, len);
            TT_UT_SUCCESS(ret, "");
            tt_buf_clear(&output);
            ret = tt_md_final_buf(&md, &output);
            TT_UT_SUCCESS(ret, "");
            // TT_UT_EQUAL(tt_buf_cmp(&output, &expect), 0, "");

            tt_md_destroy(&md);
        }
    }

    tt_buf_destroy(&output);
    tt_buf_destroy(&expect);

    // test end
    TT_TEST_CASE_LEAVE()
}
