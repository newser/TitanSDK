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

#include <network/adns/tt_adns_rr.h>

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
TT_TEST_ROUTINE_DECLARE(tt_adns_ut_dm_name_verify)
TT_TEST_ROUTINE_DECLARE(tt_adns_ut_dm_name_render_parse_basic)

TT_TEST_ROUTINE_DECLARE(tt_adns_ut_dm_name_parse_ptr)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(adns_dmn_case)

TT_TEST_CASE("tt_adns_ut_dm_name_verify",
             "adns domain name verify",
             tt_adns_ut_dm_name_verify,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_adns_ut_dm_name_render_parse_basic",
                 "adns domain name render parse basic",
                 tt_adns_ut_dm_name_render_parse_basic,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_adns_ut_dm_name_parse_ptr",
                 "adns domain name render parse pointer",
                 tt_adns_ut_dm_name_parse_ptr,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(adns_dmn_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(ADNS_UT_DOMAIN_NAME, 0, adns_dmn_case)

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

    TT_TEST_ROUTINE_DEFINE(tt_adns_ut_dm_name_verify)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_u32_t i;

    struct __dmn_e
    {
        const tt_char_t *p;
        tt_bool_t valid;
    } __dmn_cases[] = {
        {"fdas", TT_TRUE},
        {"fsafd.", TT_FALSE},
        {".fdsa", TT_FALSE},
        {"fdsafa..fas", TT_FALSE},
        {" ", TT_TRUE},
        {"1a.com", TT_TRUE}, // allow beginning with num
        {"2b-.com", TT_FALSE},
        {"a.b.c", TT_TRUE},
    };

    TT_TEST_CASE_ENTER()
    // test start

    for (i = 0; i < sizeof(__dmn_cases) / sizeof(struct __dmn_e); ++i) {
        struct __dmn_e *__case = &__dmn_cases[i];
        TT_UT_EQUAL(tt_adns_name_verify(__case->p,
                                        (tt_u32_t)tt_strlen(__case->p) + 1),
                    __case->valid,
                    "");
    }

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_adns_ut_dm_name_render_parse_basic)
{
    tt_u32_t i;
    tt_buf_t ds;

    struct __dmn_e
    {
        const tt_char_t *p;
        tt_u32_t len;
        const tt_char_t *rendered;
    } __dmn_cases[] = {
        {"abbb.ccc.ddd",
         14,
         "\x4"
         "abbb"
         "\x3"
         "ccc"
         "\x3"
         "ddd"},
        {"aaa",
         5,
         "\x3"
         "aaa"},
        {" ",
         3,
         "\x1"
         " "},
    };

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_create(&ds, 0, NULL);

    for (i = 0; i < sizeof(__dmn_cases) / sizeof(struct __dmn_e); ++i) {
        struct __dmn_e *__case = &__dmn_cases[i];
        tt_u32_t l;
        tt_result_t ret;
        tt_result_t st;
        tt_char_t *parsed;
        tt_u32_t parsed_len;

        ret = tt_adns_name_render_prepare(__case->p,
                                          (tt_u32_t)strlen(__case->p) + 1,
                                          0,
                                          &l);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(l, __case->len, "");

        st = tt_adns_name_render(&ds,
                                 __case->p,
                                 (tt_u32_t)strlen(__case->p) + 1,
                                 0);
        TT_UT_EQUAL(st, TT_SUCCESS, "");
        TT_UT_EQUAL(TT_BUF_RLEN(&ds), l, "");
        TT_UT_EQUAL(tt_memcmp(ds.p, __case->rendered, l), 0, "");

        st = tt_adns_name_parse_prepare(&ds);
        TT_UT_EQUAL(st, TT_SUCCESS, "");
        TT_UT_EQUAL(TT_BUF_RLEN(&ds), 0, "");

        if (i % 2) {
            tt_buf_reset_rp(&ds);
            parsed = tt_adns_name_parse(&ds,
                                        NULL,
                                        &parsed_len,
                                        TT_BUF_RPOS(&ds),
                                        TT_BUF_RLEN(&ds));
            TT_UT_NOT_EQUAL(parsed, NULL, "");
            TT_UT_EQUAL(parsed_len, strlen(__case->p) + 1, "");
            TT_UT_EQUAL(tt_memcmp(parsed, __case->p, strlen(__case->p) + 1),
                        0,
                        "");
            TT_UT_EQUAL(TT_BUF_RLEN(&ds), 0, "");

            tt_free(parsed);
        } else {
            tt_u32_t n;
            tt_char_t *new_p;

            tt_buf_reset_rp(&ds);
            n = tt_adns_name_parse_len(&ds, TT_BUF_RPOS(&ds), TT_BUF_RLEN(&ds));
            TT_UT_EQUAL(n, strlen(__case->p) + 1, "");

            new_p = (tt_char_t *)tt_malloc(n + 10);
            TT_UT_NOT_EQUAL(new_p, NULL, "");

            tt_buf_reset_rp(&ds);
            parsed_len = n + 10;
            parsed = tt_adns_name_parse(&ds,
                                        new_p,
                                        &parsed_len,
                                        TT_BUF_RPOS(&ds),
                                        TT_BUF_RLEN(&ds));
            TT_UT_EQUAL(parsed, new_p, "");
            TT_UT_EQUAL(parsed_len, n, "");
            TT_UT_EQUAL(tt_memcmp(parsed, __case->p, strlen(__case->p) + 1),
                        0,
                        "");
            TT_UT_EQUAL(TT_BUF_RLEN(&ds), 0, "");

            tt_free(parsed);
        }

        tt_buf_reset_rwp(&ds);
    }

    tt_buf_destroy(&ds);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_adns_ut_dm_name_parse_ptr)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_char_t __c1[] =
        "\x0\x4"
        "aaaa"
        "\x3"
        "com\0"
        "\x1"
        "b"
        "\xC0\x01"
        "\x2"
        "cc"
        "\xc0"
        "\x0b";
    const tt_char_t *__c1_parsed = "cc.b.aaaa.com";
    tt_u8_t *p1 = (tt_u8_t *)__c1 + 1;
    tt_u32_t s1 = sizeof(__c1) - 2;
    tt_blob_t b1 = {(tt_u8_t *)__c1, sizeof(__c1)};
    tt_u32_t i, n;
    tt_buf_t ds;

    tt_char_t *ret;
    tt_u32_t ret_len;

    tt_u8_t rand_name[200];

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_create_nocopy(&ds, (tt_u8_t *)__c1 + 1, sizeof(__c1) - 2, NULL);

    // aaaa.com
    ret = tt_adns_name_parse(&ds, NULL, &ret_len, b1.addr, b1.len);
    TT_UT_NOT_EQUAL(ret, NULL, "");
    TT_UT_EQUAL(ret_len, 9, "");
    TT_UT_EQUAL(tt_memcmp(ret, "aaaa.com", ret_len), 0, "");
    tt_free(ret);

    // continue parsing: b.aaaa.com
    ret = tt_adns_name_parse(&ds, NULL, &ret_len, b1.addr, b1.len);
    TT_UT_NOT_EQUAL(ret, NULL, "");
    TT_UT_EQUAL(ret_len, 11, "");
    TT_UT_EQUAL(tt_memcmp(ret, "b.aaaa.com", ret_len), 0, "");
    tt_free(ret);

    // continue parsing: cc.b.aaaa.com
    ret = tt_adns_name_parse(&ds, NULL, &ret_len, b1.addr, b1.len);
    TT_UT_NOT_EQUAL(ret, NULL, "");
    TT_UT_EQUAL(ret_len, 14, "");
    TT_UT_EQUAL(tt_memcmp(ret, "cc.b.aaaa.com", ret_len), 0, "");
    tt_free(ret);

    tt_buf_destroy(&ds);

    for (n = 0; n < 1000; ++n) {
        tt_u8_t ll = 0;
        tt_u16_t last_ptr = 0, ptr = 0;

        rand_name[0] = 3;
        rand_name[1] = 'c';
        rand_name[2] = 'o';
        rand_name[3] = 'm';
        rand_name[4] = 0;

        for (i = 5; i < sizeof(rand_name); ++i) {
            if (ll == 2) {
                rand_name[i] = last_ptr >> 8 | 0xC0;
                --ll;
            } else if (ll == 1) {
                rand_name[i] = last_ptr & 0xFF;
                --ll;
            } else if (ll == 0) {
                // len(1) + label(>=1) + ptr(2)
                ll = tt_rand_u32() % 30 + 1 + 3;

                // ptr(2) + len(1) + lable(ll)
                if (i + ll > sizeof(rand_name)) {
                    break;
                }

                rand_name[i] = ll - 3;

                last_ptr = ptr;
                ptr = i;
                p1 = &rand_name[i];
                s1 = ll;

                --ll;
            } else {
                rand_name[i] = tt_rand_u32() % 26 + 'a';
                --ll;
            }
        }

        tt_buf_create_nocopy(&ds, p1, s1, NULL);
        b1.addr = rand_name;
        b1.len = i;

        ret = tt_adns_name_parse(&ds, NULL, &ret_len, b1.addr, b1.len);
        TT_UT_NOT_EQUAL(ret, NULL, "");
        TT_UT_EQUAL(strlen(ret) + 1, ret_len, "");
        TT_UT_EQUAL(TT_BUF_RLEN(&ds), 0, "");
        tt_free(ret);
    }

    // test end
    TT_TEST_CASE_LEAVE()
}
