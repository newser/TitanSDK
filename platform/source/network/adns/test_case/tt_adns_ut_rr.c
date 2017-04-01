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

#include <network/adns/tt_adns_packet.h>
#include <network/adns/tt_adns_question.h>
#include <network/adns/tt_adns_rr.h>
#include <network/adns/tt_adns_rr_a_in.h>
#include <network/adns/tt_adns_rr_aaaa_in.h>
#include <network/adns/tt_adns_rr_cname_in.h>

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
TT_TEST_ROUTINE_DECLARE(tt_adns_ut_rr_basic)

TT_TEST_ROUTINE_DECLARE(tt_adns_ut_rr_question)
TT_TEST_ROUTINE_DECLARE(tt_adns_ut_rr_a_in)
TT_TEST_ROUTINE_DECLARE(tt_adns_ut_rr_aaaa_in)
TT_TEST_ROUTINE_DECLARE(tt_adns_ut_rr_cname_in)

TT_TEST_ROUTINE_DECLARE(tt_adns_ut_pkt_basic)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(adns_rr_case)

TT_TEST_CASE("tt_adns_ut_rr_basic",
             "adns rr basic",
             tt_adns_ut_rr_basic,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_adns_ut_rr_question",
                 "adns question render & parse",
                 tt_adns_ut_rr_question,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_adns_ut_rr_a_in",
                 "adns rr [A IN] render & parse",
                 tt_adns_ut_rr_a_in,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_adns_ut_rr_aaaa_in",
                 "adns rr [AAAA IN] render & parse",
                 tt_adns_ut_rr_aaaa_in,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_adns_ut_rr_cname_in",
                 "adns rr [cname IN] render & parse",
                 tt_adns_ut_rr_cname_in,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_adns_ut_pkt_basic",
                 "adns packte render & parse",
                 tt_adns_ut_pkt_basic,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),


    TT_TEST_CASE_LIST_DEFINE_END(adns_rr_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(ADNS_UT_RR, 0, adns_rr_case)

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

    TT_TEST_ROUTINE_DEFINE(tt_adns_ut_rr_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_adns_rr_t *rr, *rr1;
    tt_dlist_t l1, l2;
    tt_adns_rr_itf_t rr_itf = {0};

    TT_TEST_CASE_ENTER()
    // test start

    // create
    rr = tt_adns_rr_create(0,
                           &rr_itf,
                           "example.com",
                           TT_ADNS_RR_COPY_NAME,
                           TT_ADNS_RR_A_IN,
                           100);
    TT_TEST_CHECK_NOT_EQUAL(rr, NULL, "");
    TT_TEST_CHECK_EQUAL(strcmp(rr->name, "example.com"), 0, "");
    TT_TEST_CHECK_EQUAL(rr->name_len, strlen("example.com") + 1, "");
    TT_TEST_CHECK_EQUAL(rr->type, TT_ADNS_RR_A_IN, "");
    TT_TEST_CHECK_EQUAL(rr->ttl, 100, "");

    rr1 = tt_adns_rr_copy(rr);
    TT_TEST_CHECK_NOT_EQUAL(rr1, NULL, "");
    TT_TEST_CHECK_NOT_EQUAL(rr1->name, rr->name, "");
    TT_TEST_CHECK_EQUAL(strcmp(rr1->name, "example.com"), 0, "");
    TT_TEST_CHECK_EQUAL(rr1->name_len, strlen("example.com") + 1, "");

    tt_dlist_init(&l1);
    tt_dlist_init(&l2);

    tt_dlist_push_tail(&l1, &rr->node);
    tt_dlist_push_tail(&l1, &rr1->node);

    // change name
    TT_TEST_CHECK_EQUAL(tt_adns_rr_set_name(rr1,
                                            "aaa",
                                            0,
                                            TT_ADNS_RR_COPY_NAME),
                        TT_SUCCESS,
                        "");
    TT_TEST_CHECK_EQUAL(rr1->name_len, 4, "");
    TT_TEST_CHECK_EQUAL(strcmp(rr1->name, "aaa"), 0, "");
    TT_TEST_CHECK_EQUAL(rr1->name_owner, TT_TRUE, "");

    // change name
    TT_TEST_CHECK_EQUAL(tt_adns_rr_set_name(rr1,
                                            "example.com",
                                            12,
                                            TT_ADNS_RR_REF_NAME),
                        TT_SUCCESS,
                        "");
    TT_TEST_CHECK_EQUAL(rr1->name_len, 12, "");
    TT_TEST_CHECK_EQUAL(strcmp(rr1->name, "example.com"), 0, "");
    TT_TEST_CHECK_EQUAL(rr1->name_owner, TT_FALSE, "");

    // change name
    do {
        tt_char_t *nn = tt_malloc(sizeof("12345"));
        TT_TEST_CHECK_NOT_EQUAL(nn, NULL, "");

        tt_memcpy(nn, "12345", sizeof("12345"));
        TT_TEST_CHECK_EQUAL(tt_adns_rr_set_name(rr1,
                                                nn,
                                                6,
                                                TT_ADNS_RR_TAKE_NAME),
                            TT_SUCCESS,
                            "");
        TT_TEST_CHECK_EQUAL(rr1->name_len, 6, "");
        TT_TEST_CHECK_EQUAL(strcmp(rr1->name, "12345"), 0, "");
        TT_TEST_CHECK_EQUAL(rr1->name_owner, TT_TRUE, "");
    } while (0);

    // change name
    TT_TEST_CHECK_EQUAL(tt_adns_rr_set_name(rr1,
                                            "example.com",
                                            12,
                                            TT_ADNS_RR_REF_NAME),
                        TT_SUCCESS,
                        "");
    TT_TEST_CHECK_EQUAL(rr1->name_len, 12, "");
    TT_TEST_CHECK_EQUAL(strcmp(rr1->name, "example.com"), 0, "");
    TT_TEST_CHECK_EQUAL(rr1->name_owner, TT_FALSE, "");

    tt_adns_rrlist_copy(&l2, &l1, TT_ADNS_RRLIST_COPY_ALL);
    do {
        tt_dnode_t *node = tt_dlist_head(&l2);
        while (node != NULL) {
            tt_adns_rr_t *__rr2 = TT_CONTAINER(node, tt_adns_rr_t, node);
            TT_TEST_CHECK_EQUAL(strcmp(__rr2->name, "example.com"), 0, "");
            TT_TEST_CHECK_EQUAL(__rr2->name_len, strlen("example.com") + 1, "");

            node = node->next;
        }
    } while (0);
    tt_adns_rrlist_destroy(&l2);

    // copy top
    tt_adns_rrlist_copy(&l2, &l1, 0);
    do {
        tt_dnode_t *node = tt_dlist_head(&l2);
        while (node != NULL) {
            tt_adns_rr_t *__rr2 = TT_CONTAINER(node, tt_adns_rr_t, node);
            TT_TEST_CHECK_EQUAL(strcmp(__rr2->name, "example.com"), 0, "");
            TT_TEST_CHECK_EQUAL(__rr2->name_len, strlen("example.com") + 1, "");
            node = node->next;
        }
    } while (0);
    tt_adns_rrlist_destroy(&l2);

    // destroy
    tt_adns_rrlist_destroy(&l1);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_adns_ut_rr_question)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_adns_rr_t *rr;

    tt_u8_t raw_data[] =
        "\x3"
        "aaa"
        "\0"
        "\x0\x1\x0\x1";
    tt_blob_t pkt_data = {raw_data, sizeof(raw_data)};
    tt_u32_t len, expect_len;
    tt_result_t ret;
    tt_result_t st;
    tt_buf_t ds;

    TT_TEST_CASE_ENTER()
    // test start

    rr = tt_adns_quest_create("aaa", TT_ADNS_RR_REF_NAME, TT_ADNS_RR_A_IN);
    TT_TEST_CHECK_NOT_EQUAL(rr, NULL, "");
    TT_TEST_CHECK_EQUAL(rr->is_quest, TT_TRUE, "");

    // render

    ret = tt_adns_quest_render_prepare(rr, &len);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(len, sizeof(raw_data) - 1, "");

    expect_len = len;
    tt_buf_create(&ds, 0, NULL);
    st = tt_adns_quest_render(rr, &ds);
    TT_TEST_CHECK_EQUAL(st, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&ds), expect_len, "");
    TT_TEST_CHECK_EQUAL(tt_memcmp(ds.p, raw_data, expect_len), 0, "");

    tt_adns_rr_destroy(rr);

    // parse

    st = tt_adns_quest_parse_prepare(&ds);
    TT_TEST_CHECK_EQUAL(st, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&ds), 0, "");

    tt_buf_reset_rp(&ds);
    rr = tt_adns_quest_parse(&ds, pkt_data.addr, pkt_data.len);
    TT_TEST_CHECK_NOT_EQUAL(rr, NULL, "");
    TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&ds), 0, "");

    TT_TEST_CHECK_EQUAL(rr->is_quest, TT_TRUE, "");
    TT_TEST_CHECK_EQUAL(rr->name_len, 4, "");
    TT_TEST_CHECK_EQUAL(tt_memcmp(rr->name, "aaa", 4), 0, "");
    TT_TEST_CHECK_EQUAL(rr->type, TT_ADNS_RR_A_IN, "");

    tt_adns_rr_destroy(rr);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_adns_ut_rr_a_in)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_adns_rr_t *rr;
    tt_adrr_a_t rdata;

    tt_u8_t raw_data[] =
        "\x3"
        "aaa"
        "\0"
        "\x0\x1\x0\x1"
        "\x0\x0\x0\xA"
        "\x0\x4"
        "\x1\x2\x3\x4";
    tt_blob_t pkt_data = {raw_data, sizeof(raw_data)};
    tt_u32_t len, expect_len;
    tt_result_t ret;
    tt_result_t st;
    tt_buf_t ds;

    TT_TEST_CASE_ENTER()
    // test start

    tt_sktaddr_ip_p2n(TT_NET_AF_INET, "1.2.3.4", &rdata.addr);

    rr = tt_adrr_a_create("aaa", TT_ADNS_RR_REF_NAME, 10, &rdata);
    TT_TEST_CHECK_NOT_EQUAL(rr, NULL, "");

    // render

    ret = tt_adns_rr_render_prepare(rr, &len);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(len, sizeof(raw_data) - 1, "");

    expect_len = len;
    tt_buf_create(&ds, 0, NULL);
    st = tt_adns_rr_render(rr, &ds);
    TT_TEST_CHECK_EQUAL(st, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&ds), expect_len, "");
    TT_TEST_CHECK_EQUAL(tt_memcmp(ds.p, raw_data, expect_len), 0, "");

    tt_adns_rr_destroy(rr);

    // parse

    st = tt_adns_rr_parse_prepare(&ds);
    TT_TEST_CHECK_EQUAL(st, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&ds), 0, "");

    tt_buf_reset_rp(&ds);
    rr = tt_adns_rr_parse(&ds, pkt_data.addr, pkt_data.len);
    TT_TEST_CHECK_NOT_EQUAL(rr, NULL, "");
    TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&ds), 0, "");

    TT_TEST_CHECK_EQUAL(rr->name_len, 4, "");
    TT_TEST_CHECK_EQUAL(tt_memcmp(rr->name, "aaa", 4), 0, "");
    TT_TEST_CHECK_EQUAL(rr->type, TT_ADNS_RR_A_IN, "");
    TT_TEST_CHECK_EQUAL(rr->ttl, 10, "");
    TT_TEST_CHECK_EQUAL(rr->rdlength, 4, "");
    TT_TEST_CHECK_EQUAL(tt_memcmp(&(TT_ADRR_CAST(rr, tt_adrr_a_t)->addr),
                                  &rdata.addr,
                                  4),
                        0,
                        "");

    tt_adns_rr_destroy(rr);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_adns_ut_rr_aaaa_in)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_adns_rr_t *rr;
    tt_adrr_aaaa_t rdata;

    tt_u8_t raw_data[] =
        "\x4"
        "ipv6"
        "\x3"
        "com"
        "\0"
        "\x0\x1c\x0\x1"
        "\x01\x02\x03\x04"
        "\x0\x10"
        "\x1\x2\x3\x4\x21\x22\x23\x24\x31\x32\x33\x34\x41\x42\x43\x44";
    tt_blob_t pkt_data = {raw_data, sizeof(raw_data)};
    tt_u32_t len, expect_len;
    tt_result_t ret;
    tt_result_t st;
    tt_buf_t ds;

    TT_TEST_CASE_ENTER()
    // test start

    tt_sktaddr_ip_p2n(TT_NET_AF_INET6,
                      "0102:0304:2122:2324:3132:3334:4142:4344",
                      &rdata.addr);

    rr = tt_adrr_aaaa_create("ipv6.com", 0, 0x1020304, &rdata);
    TT_TEST_CHECK_NOT_EQUAL(rr, NULL, "");

    // render

    ret = tt_adns_rr_render_prepare(rr, &len);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(len, sizeof(raw_data) - 1, "");

    expect_len = len;
    tt_buf_create(&ds, 0, NULL);
    st = tt_adns_rr_render(rr, &ds);
    TT_TEST_CHECK_EQUAL(st, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&ds), expect_len, "");
    TT_TEST_CHECK_EQUAL(tt_memcmp(ds.p, raw_data, expect_len), 0, "");

    tt_adns_rr_destroy(rr);

    // parse

    st = tt_adns_rr_parse_prepare(&ds);
    TT_TEST_CHECK_EQUAL(st, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&ds), 0, "");

    tt_buf_reset_rp(&ds);
    rr = tt_adns_rr_parse(&ds, pkt_data.addr, pkt_data.len);
    TT_TEST_CHECK_NOT_EQUAL(rr, NULL, "");
    TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&ds), 0, "");

    TT_TEST_CHECK_EQUAL(rr->name_len, 9, "");
    TT_TEST_CHECK_EQUAL(tt_memcmp(rr->name, "ipv6.com", 9), 0, "");
    TT_TEST_CHECK_EQUAL(rr->type, TT_ADNS_RR_AAAA_IN, "");
    TT_TEST_CHECK_EQUAL(rr->ttl, 0x1020304, "");
    TT_TEST_CHECK_EQUAL(rr->rdlength, sizeof(tt_adrr_aaaa_t), "");
    TT_TEST_CHECK_EQUAL(tt_memcmp(&TT_ADRR_CAST(rr, tt_adrr_aaaa_t)->addr,
                                  &rdata.addr,
                                  16),
                        0,
                        "");

    tt_adns_rr_destroy(rr);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_adns_ut_rr_cname_in)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_adns_rr_t *rr;
    tt_adrr_cname_t rdata = {"X.Y.Z", 6};

    tt_u8_t raw_data[] =
        "\x5"
        "cname"
        "\x7"
        "example"
        "\0"
        "\x0\x5\x0\x1"
        "\x0\x0\x0\xA"
        "\x0\x7"
        "\x1"
        "X"
        "\x1"
        "Y"
        "\x1"
        "Z"
        "\0";
    tt_blob_t pkt_data = {raw_data, sizeof(raw_data)};
    tt_u32_t len, expect_len;
    tt_result_t ret;
    tt_result_t st;
    tt_buf_t ds;

    TT_TEST_CASE_ENTER()
    // test start

    rr = tt_adrr_cname_create("cname.example", 0, 10, &rdata);
    TT_TEST_CHECK_NOT_EQUAL(rr, NULL, "");

    // render

    ret = tt_adns_rr_render_prepare(rr, &len);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(len, sizeof(raw_data) - 1, "");

    expect_len = len;
    tt_buf_create(&ds, 0, NULL);
    st = tt_adns_rr_render(rr, &ds);
    TT_TEST_CHECK_EQUAL(st, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&ds), expect_len, "");
    TT_TEST_CHECK_EQUAL(tt_memcmp(ds.p, raw_data, expect_len), 0, "");

    tt_adns_rr_destroy(rr);

    // parse

    st = tt_adns_rr_parse_prepare(&ds);
    TT_TEST_CHECK_EQUAL(st, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&ds), 0, "");

    tt_buf_reset_rp(&ds);
    rr = tt_adns_rr_parse(&ds, pkt_data.addr, pkt_data.len);
    TT_TEST_CHECK_NOT_EQUAL(rr, NULL, "");
    TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&ds), 0, "");

    TT_TEST_CHECK_EQUAL(rr->name_len, 14, "");
    TT_TEST_CHECK_EQUAL(tt_memcmp(rr->name, "cname.example", 14), 0, "");
    TT_TEST_CHECK_EQUAL(rr->type, TT_ADNS_RR_CNAME_IN, "");
    TT_TEST_CHECK_EQUAL(rr->ttl, 10, "");
    {
        tt_adrr_cname_t *rdd = TT_ADRR_CAST(rr, tt_adrr_cname_t);
        TT_TEST_CHECK_EQUAL(rr->rdlength, 7, "");
        TT_TEST_CHECK_EQUAL(strcmp(rdd->cname, rdata.cname), 0, "");
        TT_TEST_CHECK_EQUAL(rdd->cname_len, strlen(rdd->cname) + 1, "");
    }

    tt_adns_rr_destroy(rr);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_adns_ut_pkt_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    const tt_u8_t raw_pkt[] = {
        0x74, 0x3a, 0x81, 0x80, 0x00, 0x01, 0x00, 0x02, 0x00, 0x02, 0x00, 0x0c,
        0x05, 0x72, 0x65, 0x63, 0x64, 0x6d, 0x04, 0x63, 0x73, 0x64, 0x6e, 0x03,
        0x6e, 0x65, 0x74, 0x00, 0x00, 0x01, 0x00, 0x01, 0xc0, 0x0c, 0x00, 0x05,
        0x00, 0x01, 0x00, 0x00, 0x01, 0xc0, 0x00, 0x2d, 0x20, 0x62, 0x35, 0x32,
        0x33, 0x37, 0x30, 0x34, 0x64, 0x30, 0x30, 0x31, 0x63, 0x36, 0x37, 0x62,
        0x66, 0x37, 0x36, 0x35, 0x61, 0x35, 0x34, 0x66, 0x63, 0x30, 0x35, 0x32,
        0x39, 0x61, 0x33, 0x65, 0x66, 0x06, 0x37, 0x63, 0x6e, 0x61, 0x6d, 0x65,
        0x03, 0x63, 0x6f, 0x6d, 0x00, 0xc0, 0x2c, 0x00, 0x01, 0x00, 0x01, 0x00,
        0x00, 0x01, 0xc0, 0x00, 0x04, 0x79, 0x0c, 0x7c, 0x66, 0xc0, 0x4d, 0x00,
        0x02, 0x00, 0x01, 0x00, 0x00, 0x01, 0x78, 0x00, 0x0c, 0x03, 0x6e, 0x73,
        0x34, 0x05, 0x64, 0x6e, 0x73, 0x76, 0x33, 0xc0, 0x54, 0xc0, 0x4d, 0x00,
        0x02, 0x00, 0x01, 0x00, 0x00, 0x01, 0x78, 0x00, 0x06, 0x03, 0x6e, 0x73,
        0x33, 0xc0, 0x79, 0xc0, 0x8d, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x09,
        0x60, 0x00, 0x04, 0x70, 0x5a, 0x8f, 0x23, 0xc0, 0x8d, 0x00, 0x01, 0x00,
        0x01, 0x00, 0x01, 0x09, 0x60, 0x00, 0x04, 0x77, 0xa7, 0xc3, 0x0a, 0xc0,
        0x8d, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x09, 0x60, 0x00, 0x04, 0xb4,
        0x99, 0x0a, 0xa6, 0xc0, 0x8d, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x09,
        0x60, 0x00, 0x04, 0xb6, 0x8c, 0xa7, 0xa8, 0xc0, 0x8d, 0x00, 0x01, 0x00,
        0x01, 0x00, 0x01, 0x09, 0x60, 0x00, 0x04, 0xb7, 0x3c, 0x39, 0xb2, 0xc0,
        0x8d, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x09, 0x60, 0x00, 0x04, 0xb7,
        0x3c, 0x3b, 0xe7, 0xc0, 0x75, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x09,
        0x60, 0x00, 0x04, 0x73, 0xec, 0x97, 0x8b, 0xc0, 0x75, 0x00, 0x01, 0x00,
        0x01, 0x00, 0x01, 0x09, 0x60, 0x00, 0x04, 0x73, 0xec, 0x97, 0xb1, 0xc0,
        0x75, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x09, 0x60, 0x00, 0x04, 0x7d,
        0x27, 0xd5, 0xa7, 0xc0, 0x75, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x09,
        0x60, 0x00, 0x04, 0xb4, 0x99, 0x0a, 0xa8, 0xc0, 0x75, 0x00, 0x01, 0x00,
        0x01, 0x00, 0x01, 0x09, 0x60, 0x00, 0x04, 0xb4, 0x99, 0xa2, 0x98, 0xc0,
        0x75, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x09, 0x60, 0x00, 0x04, 0xdd,
        0xcc, 0xba, 0x09,
    };

    tt_u8_t *pos = (tt_u8_t *)raw_pkt;
    tt_u32_t len = sizeof(raw_pkt);
    tt_adns_pkt_t *pkt;
    tt_dnode_t *node;
    tt_buf_t ds;

    tt_bool_t rendered = TT_FALSE;
    tt_u32_t status = 0;

    TT_TEST_CASE_ENTER()
// test start

// parse

__parse : {
    tt_buf_t ds1;
    tt_result_t st;

    tt_buf_create_nocopy(&ds1, pos, len, NULL);
    st = tt_adns_pkt_parse_prepare(&ds1);
    TT_TEST_CHECK_EQUAL(st, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&ds1), 0, "");
    tt_buf_destroy(&ds1);
}

    {
        tt_buf_t ds1;
        tt_result_t st;

        tt_buf_create_nocopy(&ds1, pos, tt_rand_u32() % len, NULL);
        st = tt_adns_pkt_parse_prepare(&ds1);
        TT_TEST_CHECK_EQUAL(st, TT_BUFFER_INCOMPLETE, "");
        tt_buf_destroy(&ds1);
    }

    tt_buf_create_nocopy(&ds, pos, len, NULL);
    pkt = tt_adns_pkt_parse(&ds, TT_ADNS_PKT_PARSE_ALL);
    TT_TEST_CHECK_NOT_EQUAL(pkt, NULL, "");

    TT_TEST_CHECK_EQUAL(pkt->__id, 0x743a, "");
    TT_TEST_CHECK_EQUAL(pkt->opcode, 0, "");
    TT_TEST_CHECK_NOT_EQUAL(pkt->flag & __ADNS_PKT_RESP, 0, "");
    TT_TEST_CHECK_EQUAL(pkt->flag & __ADNS_PKT_AUTH_ANS, 0, "");
    TT_TEST_CHECK_NOT_EQUAL(pkt->flag & __ADNS_PKT_RECUR_DESIRED, 0, "");
    TT_TEST_CHECK_NOT_EQUAL(pkt->flag & __ADNS_PKT_RECUR_AVAIL, 0, "");
    TT_TEST_CHECK_EQUAL(pkt->rcode, 0, "");

    TT_TEST_CHECK_EQUAL(pkt->question_num, 1, "");
    TT_TEST_CHECK_EQUAL(pkt->answer_num, 2, "");
    TT_TEST_CHECK_EQUAL(pkt->authority_num, 0, ""); // it does not know NS rr
    TT_TEST_CHECK_EQUAL(pkt->additional_num, 12, "");

    node = tt_dlist_head(&pkt->answer);
    while (node != NULL) {
        tt_adns_rr_t *rr = TT_CONTAINER(node, tt_adns_rr_t, node);

        if (rr->type == TT_ADNS_RR_A_IN) {
            TT_TEST_CHECK_EQUAL(
                strcmp(rr->name, "b523704d001c67bf765a54fc0529a3ef.7cname.com"),
                0,
                "");
            TT_TEST_CHECK_EQUAL(
                strlen("b523704d001c67bf765a54fc0529a3ef.7cname.com") + 1,
                rr->name_len,
                "");

            TT_TEST_CHECK_EQUAL(rr->ttl, 0x1c0, "");
            TT_TEST_CHECK_EQUAL(TT_ADRR_CAST(rr, tt_adrr_a_t)->addr.a32.__u32,
                                tt_ntoh32(0x790c7c66),
                                "");

            status |= 1;
        } else if (rr->type == TT_ADNS_RR_CNAME_IN) {
            TT_TEST_CHECK_EQUAL(strcmp(rr->name, "recdm.csdn.net"), 0, "");
            TT_TEST_CHECK_EQUAL(strlen("recdm.csdn.net") + 1, rr->name_len, "");

            TT_TEST_CHECK_EQUAL(rr->ttl, 0x1c0, "");
            TT_TEST_CHECK_EQUAL(
                strcmp(TT_ADRR_CAST(rr, tt_adrr_cname_t)->cname,
                       "b523704d001c67bf765a54fc0529a3ef.7cname.com"),
                0,
                "");
            TT_TEST_CHECK_EQUAL(
                strlen("b523704d001c67bf765a54fc0529a3ef.7cname.com") + 1,
                TT_ADRR_CAST(rr, tt_adrr_cname_t)->cname_len,
                "");

            status |= 2;
        } else {
            status |= 4;
        }

        node = node->next;
    }
    TT_TEST_CHECK_EQUAL(status, 3, "");

    // render
    if (!rendered) {
        TT_TEST_CHECK_EQUAL(tt_adns_pkt_generate(pkt), TT_SUCCESS, "");

        pos = pkt->buf.p;
        len = TT_BUF_RLEN(&pkt->buf);

        // hack
        pkt->buf.p = pkt->buf.initbuf;
        pkt->buf.size = TT_BUF_INIT_SIZE;
        tt_buf_reset_rwp(&pkt->buf);

        rendered = TT_TRUE;

        tt_adns_pkt_release(pkt);
        goto __parse;
    }

    tt_free(pos);
    tt_adns_pkt_release(pkt);

    // test end
    TT_TEST_CASE_LEAVE()
}
