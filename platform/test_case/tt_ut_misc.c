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

//#define TT_VERSION_REQUIRE_MAJOR 256
//#define TT_VERSION_REQUIRE_MINOR 256

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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_version)

TT_TEST_ROUTINE_DECLARE(tt_unit_test_high_bit_1)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_low_bit_1)

TT_TEST_ROUTINE_DECLARE(tt_unit_test_base64_dec)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_der_enc)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_strtol)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_c2h)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_align)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(misc_case)

TT_TEST_CASE("tt_unit_test_version",
             "testing version info APIs",
             tt_unit_test_version,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_high_bit_1",
                 "testing tt_high_bit_1()",
                 tt_unit_test_high_bit_1,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_low_bit_1",
                 "testing tt_low_bit_1()",
                 tt_unit_test_low_bit_1,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_base64_dec",
                 "base64 decode",
                 tt_unit_test_base64_dec,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_der_enc",
                 "asn1 der encode",
                 tt_unit_test_der_enc,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_strtol",
                 "ts strtol",
                 tt_unit_test_strtol,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_c2h",
                 "ts char/hex to hex/char",
                 tt_unit_test_c2h,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_align",
                 "testing value alignment",
                 tt_unit_test_align,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(misc_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(TEST_UNIT_MISC, 0, misc_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(tt_unit_test_c2h)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(tt_unit_test_version)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    tt_ver_t ver;
    tt_char_t buf[100];
    tt_char_t test_buf[100];

    TT_TEST_CASE_ENTER()
    // test start

    ver = tt_ver_major();
    TT_TEST_CHECK_EQUAL(ver, TT_VERSION_MAJOR, "");
    ver = tt_ver_minor();
    TT_TEST_CHECK_EQUAL(ver, TT_VERSION_MINOR, "");
    ver = tt_ver_revision();
    TT_TEST_CHECK_EQUAL(ver, TT_VERSION_REVISION, "");

    tt_ver_format(buf, sizeof(buf), TT_VER_FORMAT_BASIC);
    tt_snprintf(test_buf,
                sizeof(test_buf),
                "%u.%u",
                TT_VERSION_MAJOR,
                TT_VERSION_MINOR);
    TT_TEST_CHECK_EQUAL(tt_strncmp(buf, test_buf, sizeof(buf)), 0, "");

    tt_ver_format(buf, sizeof(buf), TT_VER_FORMAT_STANDARD);
    tt_snprintf(test_buf,
                sizeof(test_buf),
                "%u.%u.%u",
                TT_VERSION_MAJOR,
                TT_VERSION_MINOR,
                TT_VERSION_REVISION);
    TT_TEST_CHECK_EQUAL(tt_strncmp(buf, test_buf, sizeof(buf)), 0, "");

    ret = tt_ver_require_major(TT_VERSION_MAJOR + 1);
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");
    ret = tt_ver_require_major(TT_VERSION_MAJOR - 1);
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");
    ret = tt_ver_require_major(TT_VERSION_MAJOR);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_ver_require_minor(TT_VERSION_MINOR + 1);
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");
    ret = tt_ver_require_minor(TT_VERSION_MINOR - 1);
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");
    ret = tt_ver_require_minor(TT_VERSION_MINOR);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_ver_require(TT_VERSION_MAJOR + 1, TT_VERSION_MINOR);
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");
    ret = tt_ver_require(TT_VERSION_MAJOR, TT_VERSION_MINOR - 1);
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");
    ret = tt_ver_require(TT_VERSION_MAJOR, TT_VERSION_MINOR);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_high_bit_1)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    int n = 0;
    int total = 100;

    tt_u32_t test_v = 0;
    tt_u32_t pos;

    TT_TEST_CASE_ENTER()
    // test start

    // return fail for 0
    TT_TEST_CHECK_EQUAL(tt_high_bit_1(test_v, &pos), TT_FAIL, "");

    for (n = 0; n < total; ++n) {
        int i;

        test_v = rand();

        // find the highest 1 one by one
        for (i = (sizeof(test_v) << 3) - 1; i >= 0; --i) {
            if (test_v & (1 << i))
                break;
        }

        if (TT_OK(tt_high_bit_1(test_v, &pos))) {
            TT_TEST_CHECK_EQUAL(pos, i, "");
        } else {
            TT_TEST_CHECK_EQUAL(test_v, 0, "");
        }
    }

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_low_bit_1)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    int n = 0;
    int total = 100;

    tt_u32_t test_v = 0;
    tt_u32_t pos;

    TT_TEST_CASE_ENTER()
    // test start

    // return fail for 0
    TT_TEST_CHECK_EQUAL(tt_low_bit_1(test_v, &pos), TT_FAIL, "");

    for (n = 0; n < total; ++n) {
        int i;

        test_v = rand();

        // find the highest 1 one by one
        for (i = 0; i <= (sizeof(test_v) << 3) - 1; ++i) {
            if (test_v & (1 << i))
                break;
        }

        if (TT_OK(tt_low_bit_1(test_v, &pos))) {
            TT_TEST_CHECK_EQUAL(pos, i, "");
        } else {
            TT_TEST_CHECK_EQUAL(test_v, 0, "");
        }
    }

    // test end
    TT_TEST_CASE_LEAVE()
}

//////////////////////////////////////////////////////////////
// base 64

// "2=, no new line"
static tt_char_t d1[] = "\n";
static tt_char_t e1[] = "Cg==";

// "no=, 2 new line"
static tt_u8_t d2[] = {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c,
    0x0d, 0x0e, 0x0f, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
    0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
    0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x01, 0x02, 0x03,
    0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c,
    0x0d, 0x0e, 0x0f, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
    0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
    0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x01, 0x02, 0x03,
    0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
};
static tt_char_t e2[] =
    "AQIDBAUGBwgJCgsMDQ4PAQIDBAUGBwgJCgsMDQ4PAQIDBAUGBwgJCgsMDQ4PAQID\r\n"
    "BAUGBwgJCgsMDQ4PAQIDBAUGBwgJCgsMDQ4PAQIDBAUGBwgJCgsMDQ4PAQIDBAUG\r\n"
    "BwgJCgsMDQ4PAQIDBAUGBwgJCgsMDQ4P";

// "1=, new lines, boundary"
static tt_u8_t d3[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                       0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x01, 0x02, 0x03,
                       0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c,
                       0x0d, 0x0e, 0x0f, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                       0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                       0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                       0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x01, 0x02, 0x03,
                       0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c,
                       0x0d, 0x0e, 0x0f, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                       0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                       0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                       0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x01, 0x02, 0x03,
                       0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c,
                       0x0d, 0x0e, 0x0f, 0x01, 0x02};
static tt_char_t e3[] =
    "----- start -----\n"
    "AQIDBAUGBwgJCgsMDQ4PAQIDBAUGBwgJCgsMDQ4PAQIDBAUGBwgJCgsMDQ4PAQID\n"
    "BAUGBwgJCgsMDQ4PAQIDBAUGBwgJCgsMDQ4PAQIDBAUGBwgJCgsMDQ4PAQIDBAUG\n"
    "BwgJCgsMDQ4PAQIDBAUGBwgJCgsMDQ4PAQI=\n"
    "----- end -----\n";

typedef struct
{
    tt_u8_t *decoded;
    tt_u32_t decoded_len;
    const tt_char_t *encoded;

    const tt_char_t *start_bd;
    const tt_char_t *end_bd;
} b64_dec_tv_t;

b64_dec_tv_t b64_dec_tv[] = {
    {(tt_u8_t *)d1, sizeof(d1) - 1, e1, NULL, NULL},
    {d2, sizeof(d2), e2, NULL, NULL},
    {d3, sizeof(d3), e3, "-----\n", "\n-----"},
    {NULL, 0, NULL, NULL, NULL},
};

TT_TEST_ROUTINE_DEFINE(tt_unit_test_base64_dec)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    b64_dec_tv_t *tv;
    tt_result_t result;
    tt_base64_decode_attr_t attr;
    tt_u32_t decoded_len;
    tt_u8_t buf[200];
    tt_u8_t *decoded;

    TT_TEST_CASE_ENTER()
    // test start

    // 0 input len
    result = tt_base64_decode((tt_u8_t *)"123", 0, NULL, NULL, &decoded_len);
    TT_TEST_CHECK_EQUAL(result, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(decoded_len, 0, "");

    // 0 output len
    decoded_len = 0;
    result = tt_base64_decode((tt_u8_t *)"123", 3, NULL, buf, &decoded_len);
    TT_TEST_CHECK_EQUAL(result, TT_SUCCESS, "");

    result = tt_base64_decode_alloc((tt_u8_t *)"123",
                                    0,
                                    NULL,
                                    &decoded,
                                    &decoded_len);
    TT_TEST_CHECK_EQUAL(result, TT_SUCCESS, "");

    tv = &b64_dec_tv[0];
    while (tv->decoded != NULL) {
        tt_base64_decode_attr_default(&attr);
        attr.start_boundary = tv->start_bd;
        attr.end_boundary = tv->end_bd;

        decoded_len = 0;
        result = tt_base64_decode((tt_u8_t *)tv->encoded,
                                  (tt_u32_t)tt_strlen(tv->encoded),
                                  &attr,
                                  NULL,
                                  &decoded_len);
        TT_TEST_CHECK_EQUAL(result, TT_SUCCESS, "");
        TT_TEST_CHECK_EQUAL(decoded_len, tv->decoded_len, "");

        decoded_len = sizeof(buf);
        result = tt_base64_decode((tt_u8_t *)tv->encoded,
                                  (tt_u32_t)tt_strlen(tv->encoded),
                                  &attr,
                                  buf,
                                  &decoded_len);
        TT_TEST_CHECK_EQUAL(result, TT_SUCCESS, "");
        TT_TEST_CHECK_EQUAL(decoded_len, tv->decoded_len, "");
        TT_TEST_CHECK_EQUAL(tt_memcmp(buf, tv->decoded, decoded_len), 0, "");

        // decode alloc
        result = tt_base64_decode_alloc((tt_u8_t *)tv->encoded,
                                        (tt_u32_t)tt_strlen(tv->encoded),
                                        &attr,
                                        &decoded,
                                        &decoded_len);
        TT_TEST_CHECK_EQUAL(result, TT_SUCCESS, "");
        TT_TEST_CHECK_EQUAL(decoded_len, tv->decoded_len, "");
        TT_TEST_CHECK_EQUAL(tt_memcmp(decoded, tv->decoded, decoded_len),
                            0,
                            "");
        tt_free(decoded);

        ++tv;
    }

    // test end
    TT_TEST_CASE_LEAVE()
}

// base 64
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
// asn.1

TT_TEST_ROUTINE_DEFINE(tt_unit_test_der_enc)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_buf_t buf;
    tt_result_t ret;

    TT_TEST_CASE_ENTER()
    // test start

    // http://asn1-playground.oss.com/
    /* type:
     World-Schema DEFINITIONS ::=
     BEGIN
        Human ::= SEQUENCE {
            name OCTET STRING,
            bname BIT STRING,
            age1 INTEGER,
            nn NULL,
            oid1 OBJECT IDENTIFIER,
            child SEQUENCE{
                name OCTET STRING,
                bname BIT STRING,
                age INTEGER,
                oid1 OBJECT IDENTIFIER,
                son SEQUENCE{
                    age INTEGER
            }
        },
        child2 SEQUENCE{
            name OCTET STRING,
            bname BIT STRING,
        age INTEGER
        }
     }
     END
     */

    /* value:
     first-man Human ::=
     {
        name '101100011'H,
        bname '10110001111'B,
        age1 0,
        nn NULL,
        oid1 { 1 3 6 1 4 1 311 21 20 },
        child {
            name '0'H,
            bname '11110000'B,
            age -128
            oid1 { 1 3 6 1 4 1 311 21 20 },
            son {
                age 16711680
            }
        }
        child2 {
            name '101100011123456'H,
            bname '111100001'B,
            age -8388609
        }
     }
     */

    /* der encoded:
     30520405 10110001 10030305 B1E00201 00050006 092B0601 04018237 1514301D
     04010003 0200F002 01800609 2B060104 01823715 14300602 0400FF00 00301504
     08101100 01112345 60030307 F0800204 FF7FFFFF
     */

    static tt_u8_t __der_encoded[] = {0x30, 0x52, 0x04, 0x05, 0x10, 0x11, 0x00,
                                      0x01, 0x10, 0x03, 0x03, 0x05, 0xB1, 0xE0,
                                      0x02, 0x01, 0x00, 0x05, 0x00, 0x06, 0x09,
                                      0x2B, 0x06, 0x01, 0x04, 0x01, 0x82, 0x37,
                                      0x15, 0x14, 0x30, 0x1D, 0x04, 0x01, 0x00,
                                      0x03, 0x02, 0x00, 0xF0, 0x02, 0x01, 0x80,
                                      0x06, 0x09, 0x2B, 0x06, 0x01, 0x04, 0x01,
                                      0x82, 0x37, 0x15, 0x14, 0x30, 0x06, 0x02,
                                      0x04, 0x00, 0xFF, 0x00, 0x00, 0x30, 0x15,
                                      0x04, 0x08, 0x10, 0x11, 0x00, 0x01, 0x11,
                                      0x23, 0x45, 0x60, 0x03, 0x03, 0x07, 0xF0,
                                      0x80, 0x02, 0x04, 0xFF, 0x7F, 0xFF, 0xFF};

    tt_u8_t human_name[] = {0x10, 0x11, 0x00, 0x01, 0x10};
    tt_u8_t human_bname[] = {0xb1, 0xe0};
    tt_u32_t human_bname_pad = 5;
    tt_s32_t human_age = 0;
    tt_u8_t human_oid[] =
        {0x2b, 0x06, 0x01, 0x04, 0x01, 0x82, 0x37, 0x15, 0x14};
    // 1.3.6.1.4.1.311.21.20

    tt_u8_t c_name[] = {0};
    tt_u8_t c_bname[] = {0xf0};
    tt_u32_t c_bname_pad = 0;
    tt_s32_t c_age = -128;

    tt_u8_t c2_name[] = {0x10, 0x11, 0x00, 0x01, 0x11, 0x23, 0x45, 0x60};
    tt_u8_t c2_bname[] = {0xf0, 0x80};
    tt_u32_t c2_bname_pad = 7;
    tt_s32_t c2_age = -8388609;

    tt_s32_t s_age = 16711680;

    tt_u32_t s_datalen = tt_der_s32_len(s_age);
    tt_u32_t s_len = tt_der_sequence_len(s_datalen);

    tt_u32_t c_datalen =
        tt_der_octstr_len(c_name, sizeof(c_name)) +
        tt_der_bitstr_len(c_bname, sizeof(c_bname), c_bname_pad) +
        tt_der_s32_len(c_age) + tt_der_oid_len(human_oid, sizeof(human_oid)) +
        s_len;
    tt_u32_t c_len = tt_der_sequence_len(c_datalen);

    tt_u32_t c2_datalen =
        tt_der_octstr_len(c2_name, sizeof(c2_name)) +
        tt_der_bitstr_len(c2_bname, sizeof(c2_bname), c2_bname_pad) +
        tt_der_s32_len(c2_age);
    tt_u32_t c2_len = tt_der_sequence_len(c2_datalen);

    tt_u32_t h_datalen =
        tt_der_octstr_len(human_name, sizeof(human_name)) +
        tt_der_bitstr_len(human_bname, sizeof(human_bname), human_bname_pad) +
        tt_der_s32_len(human_age) + tt_der_null_len() +
        tt_der_oid_len(human_oid, sizeof(human_oid)) + c_len + c2_len;
    tt_u32_t h_len = tt_der_sequence_len(h_datalen);

    tt_buf_init(&buf, NULL);

    ret = tt_der_encode_sequence(&buf, h_datalen, 0);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_der_encode_octstr(&buf, human_name, sizeof(human_name), 0);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_der_encode_bitstr(&buf,
                               human_bname,
                               sizeof(human_bname),
                               human_bname_pad,
                               0);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_der_encode_s32(&buf, human_age, 0);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_der_encode_null(&buf, 0);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_der_encode_oid(&buf, human_oid, sizeof(human_oid), 0);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // child
    ret = tt_der_encode_sequence(&buf, c_datalen, 0);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_der_encode_octstr(&buf, c_name, sizeof(c_name), 0);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_der_encode_bitstr(&buf, c_bname, sizeof(c_bname), c_bname_pad, 0);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_der_encode_s32(&buf, c_age, 0);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_der_encode_oid(&buf, human_oid, sizeof(human_oid), 0);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // son
    ret = tt_der_encode_sequence(&buf, s_datalen, 0);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_der_encode_s32(&buf, s_age, 0);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // child2
    ret = tt_der_encode_sequence(&buf, c2_datalen, 0);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_der_encode_octstr(&buf, c2_name, sizeof(c2_name), 0);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret =
        tt_der_encode_bitstr(&buf, c2_bname, sizeof(c2_bname), c2_bname_pad, 0);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_der_encode_s32(&buf, c2_age, 0);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    do {
        tt_char_t numstr[1000] = {0};
        tt_buf_get_hexstr(&buf, numstr, sizeof(numstr) - 1);
        TT_INFO("%s", numstr);
    } while (0);

    TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&buf), sizeof(__der_encoded), "");
    TT_TEST_CHECK_EQUAL(tt_memcmp(TT_BUF_RPOS(&buf),
                                  __der_encoded,
                                  TT_BUF_RLEN(&buf)),
                        0,
                        "");

    // test length encoding
    do {
        tt_buf_reset_rwp(&buf);
        tt_der_encode_sequence(&buf, 127, 0);
        TT_TEST_CHECK_EQUAL(buf.p[buf.rpos + 1], 0x7f, "");

        tt_buf_reset_rwp(&buf);
        tt_der_encode_sequence(&buf, 128, 0);
        TT_TEST_CHECK_EQUAL(buf.p[buf.rpos + 1], 0x81, "");
        TT_TEST_CHECK_EQUAL(buf.p[buf.rpos + 2], 0x80, "");

        tt_buf_reset_rwp(&buf);
        tt_der_encode_sequence(&buf, 0x1234, 0);
        TT_TEST_CHECK_EQUAL(buf.p[buf.rpos + 1], 0x82, "");
        TT_TEST_CHECK_EQUAL(buf.p[buf.rpos + 2], 0x12, "");
        TT_TEST_CHECK_EQUAL(buf.p[buf.rpos + 3], 0x34, "");

        tt_buf_reset_rwp(&buf);
        tt_der_encode_sequence(&buf, 0x123456, 0);
        TT_TEST_CHECK_EQUAL(buf.p[buf.rpos + 1], 0x83, "");
        TT_TEST_CHECK_EQUAL(buf.p[buf.rpos + 2], 0x12, "");
        TT_TEST_CHECK_EQUAL(buf.p[buf.rpos + 3], 0x34, "");
        TT_TEST_CHECK_EQUAL(buf.p[buf.rpos + 4], 0x56, "");

        tt_buf_reset_rwp(&buf);
        tt_der_encode_sequence(&buf, 0x12345678, 0);
        TT_TEST_CHECK_EQUAL(buf.p[buf.rpos + 1], 0x84, "");
        TT_TEST_CHECK_EQUAL(buf.p[buf.rpos + 2], 0x12, "");
        TT_TEST_CHECK_EQUAL(buf.p[buf.rpos + 3], 0x34, "");
        TT_TEST_CHECK_EQUAL(buf.p[buf.rpos + 4], 0x56, "");
        TT_TEST_CHECK_EQUAL(buf.p[buf.rpos + 5], 0x78, "");
    } while (0);

    // bit string
    do {
        tt_buf_reset_rwp(&buf);
        tt_der_encode_bitstr(&buf, NULL, 126, 0, 0);
        TT_TEST_CHECK_EQUAL(buf.p[buf.rpos + 1], 0x7f, "");

        tt_buf_reset_rwp(&buf);
        tt_der_encode_bitstr(&buf, NULL, 127, 0, 0);
        TT_TEST_CHECK_EQUAL(buf.p[buf.rpos + 1], 0x81, "");
        TT_TEST_CHECK_EQUAL(buf.p[buf.rpos + 2], 0x80, "");
    } while (0);

    // test end
    TT_TEST_CASE_LEAVE()
}

// asn.1
//////////////////////////////////////////////////////////////

TT_TEST_ROUTINE_DEFINE(tt_unit_test_strtol)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t result;
    tt_s32_t s32_val;
    tt_char_t *endptr;
    tt_u32_t u32_val;

    TT_TEST_CASE_ENTER()
    // test start

    // s32

    result = tt_strtos32("  -0x123 ", &endptr, 0, &s32_val);
    TT_TEST_CHECK_EQUAL(result, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(*endptr, ' ', "");
    TT_TEST_CHECK_EQUAL(s32_val, -0x123, "");

    result = tt_strtos32("  -0x123x ", &endptr, 0, &s32_val);
    TT_TEST_CHECK_EQUAL(result, TT_FAIL, "");

    result = tt_strtos32("   ", &endptr, 0, &s32_val);
    TT_TEST_CHECK_EQUAL(result, TT_FAIL, "");

    result = tt_strtos32("", &endptr, 0, &s32_val);
    TT_TEST_CHECK_EQUAL(result, TT_FAIL, "");

    result = tt_strtos32("-", &endptr, 0, &s32_val);
    TT_TEST_CHECK_EQUAL(result, TT_FAIL, "");

    result = tt_strtos32("0", &endptr, 0, &s32_val);
    TT_TEST_CHECK_EQUAL(result, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(*endptr, 0, "");
    TT_TEST_CHECK_EQUAL(s32_val, 0, "");

    result = tt_strtos32("-0x0", &endptr, 0, &s32_val);
    TT_TEST_CHECK_EQUAL(result, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(*endptr, 0, "");
    TT_TEST_CHECK_EQUAL(s32_val, 0, "");

    result = tt_strtos32("-111111111111111111111111111111111111",
                         &endptr,
                         0,
                         &s32_val);
    TT_TEST_CHECK_EQUAL(result, TT_FAIL, "");
    result = tt_strtos32("+111111111111111111111111111111111111",
                         &endptr,
                         0,
                         &s32_val);
    TT_TEST_CHECK_EQUAL(result, TT_FAIL, "");

    result = tt_strtos32("  +0x7fffffff", &endptr, 0, &s32_val);
    TT_TEST_CHECK_EQUAL(result, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(*endptr, 0, "");
    TT_TEST_CHECK_EQUAL(s32_val, 0x7fffffff, "");

    result = tt_strtos32("-0x80000000\t", &endptr, 0, &s32_val);
    TT_TEST_CHECK_EQUAL(result, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(*endptr, '\t', "");
    TT_TEST_CHECK_EQUAL(s32_val, 0x80000000, "");

    // u32

    result = tt_strtou32("  0x123 ", &endptr, 0, &u32_val);
    TT_TEST_CHECK_EQUAL(result, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(*endptr, ' ', "");
    TT_TEST_CHECK_EQUAL(u32_val, 0x123, "");

    result = tt_strtou32("  -0x123 ", &endptr, 0, &u32_val);
    TT_TEST_CHECK_EQUAL(result, TT_FAIL, "");

    result = tt_strtou32("  0x123x ", &endptr, 0, &u32_val);
    TT_TEST_CHECK_EQUAL(result, TT_FAIL, "");

    result = tt_strtou32("   ", &endptr, 0, &u32_val);
    TT_TEST_CHECK_EQUAL(result, TT_FAIL, "");

    result = tt_strtou32("", &endptr, 0, &u32_val);
    TT_TEST_CHECK_EQUAL(result, TT_FAIL, "");

    result = tt_strtou32("+", &endptr, 0, &u32_val);
    TT_TEST_CHECK_EQUAL(result, TT_FAIL, "");

    result = tt_strtou32("0", &endptr, 0, &u32_val);
    TT_TEST_CHECK_EQUAL(result, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(*endptr, 0, "");
    TT_TEST_CHECK_EQUAL(u32_val, 0, "");

    result = tt_strtou32("+0x0", &endptr, 0, &u32_val);
    TT_TEST_CHECK_EQUAL(result, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(*endptr, 0, "");
    TT_TEST_CHECK_EQUAL(u32_val, 0, "");

    result = tt_strtou32("-111111111111111111111111111111111111",
                         &endptr,
                         0,
                         &u32_val);
    TT_TEST_CHECK_EQUAL(result, TT_FAIL, "");
    result = tt_strtou32("+111111111111111111111111111111111111",
                         &endptr,
                         0,
                         &u32_val);
    TT_TEST_CHECK_EQUAL(result, TT_FAIL, "");

    result = tt_strtou32("  +0x7fffffff", &endptr, 0, &u32_val);
    TT_TEST_CHECK_EQUAL(result, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(*endptr, 0, "");
    TT_TEST_CHECK_EQUAL(u32_val, 0x7fffffff, "");

    result = tt_strtou32("+0xffffffff\t", &endptr, 0, &u32_val);
    TT_TEST_CHECK_EQUAL(result, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(*endptr, '\t', "");
    TT_TEST_CHECK_EQUAL(u32_val, 0xffffffff, "");

    //////////////////////////////////////////////

    {
        tt_char_t *p;
        char p1[] = "12341234";
        char p2[] = "1234234";
        char p3[] = "12341234123";

        p = tt_strrstr("", "");
        TT_TEST_CHECK_NOT_EQUAL(p, NULL, "");
        TT_TEST_CHECK_EQUAL(tt_strcmp(p, ""), 0, "");

        p = tt_strrstr("", "123");
        TT_TEST_CHECK_EQUAL(p, NULL, "");

        p = tt_strrstr("123", "");
        TT_TEST_CHECK_NOT_EQUAL(p, NULL, "");
        TT_TEST_CHECK_EQUAL(tt_strcmp(p, "123"), 0, "");

        p = tt_strrstr("123", "123");
        TT_TEST_CHECK_NOT_EQUAL(p, NULL, "123");
        TT_TEST_CHECK_EQUAL(tt_strcmp(p, "123"), 0, "");

        p = tt_strrstr("", "1234");
        TT_TEST_CHECK_EQUAL(p, NULL, "");

        p = tt_strrstr(p1, "1234");
        TT_TEST_CHECK_EQUAL(p, &p1[4], "");

        p = tt_strrstr(p2, "1234");
        TT_TEST_CHECK_EQUAL(p, &p2[0], "");

        p = tt_strrstr(p3, "1234");
        TT_TEST_CHECK_EQUAL(p, &p3[4], "");
    }

    // test end
    TT_TEST_CASE_LEAVE()
}

#if 0
TT_TEST_ROUTINE_DEFINE(tt_unit_test_sc_gb2sc)
{    
    //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    wchar_t sc[100];
    int len, test_len;
    tt_result_t ret;
    
    TT_TEST_CASE_ENTER()
    // test start

    // gb2312 to sys char
    {
        tt_u8_t test[] = { 0xb9, 0xfe, 0xc5, 0xa3, 0 };
        
		test_len = sizeof(sc);
		ret = tt_sys_char_convert(TT_CHARSET_GB2312,test,sizeof(test),
                                       NULL, (tt_u32_t*)&test_len);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, ""); 

		len = sizeof(sc);
        ret = tt_sys_char_convert(TT_CHARSET_GB2312,test,sizeof(test),
                                  sc, (tt_u32_t*)&len);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

#ifdef _WIN32
        TT_TEST_CHECK_EQUAL(test_len, 6, ""); 
        TT_TEST_CHECK_EQUAL(len, 6, ""); 
        
		// 6 bytes for 2 chinese and ending 0
        TT_TEST_CHECK_EQUAL(sc[0], 0x54c8, "");
        TT_TEST_CHECK_EQUAL(sc[1], 0x725b, "");
        TT_TEST_CHECK_EQUAL(sc[2], 0, "");
#else
        TT_TEST_CHECK_EQUAL(test_len, 7, ""); 
        TT_TEST_CHECK_EQUAL(len, 7, ""); 
        
        TT_TEST_CHECK_EQUAL(sc[0], (tt_sys_char_t)0xe5, "");
        TT_TEST_CHECK_EQUAL(sc[1], (tt_sys_char_t)(tt_u8_t)0x93, "");
        TT_TEST_CHECK_EQUAL(sc[2], (tt_sys_char_t)0x88, "");
        TT_TEST_CHECK_EQUAL(sc[3], (tt_sys_char_t)0xe7, "");
        TT_TEST_CHECK_EQUAL(sc[4], (tt_sys_char_t)0x89, "");
        TT_TEST_CHECK_EQUAL(sc[5], (tt_sys_char_t)0x9b, "");
        TT_TEST_CHECK_EQUAL(sc[6], (tt_sys_char_t)0, "");
#endif
    }

    // gbk to sys char
    {
        tt_u8_t test[] = { 0xb9, 0xfe, 0xc5, 0xa3, 0 };
        
		test_len = sizeof(sc);
		ret = tt_sys_char_convert(TT_CHARSET_GB2312,test,sizeof(test),
                                       NULL, (tt_u32_t*)&test_len);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, ""); 

		len = sizeof(sc);
        ret = tt_sys_char_convert(TT_CHARSET_GBK,test,sizeof(test),
                                  sc, (tt_u32_t*)&len);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

#ifdef _WIN32
        TT_TEST_CHECK_EQUAL(test_len, 6, ""); 
        TT_TEST_CHECK_EQUAL(len, 6, ""); 
        
		// 6 bytes for 2 chinese and ending 0
        TT_TEST_CHECK_EQUAL(sc[0], 0x54c8, "");
        TT_TEST_CHECK_EQUAL(sc[1], 0x725b, "");
        TT_TEST_CHECK_EQUAL(sc[2], 0, "");
#else
        TT_TEST_CHECK_EQUAL(test_len, 7, ""); 
        TT_TEST_CHECK_EQUAL(len, 7, ""); 
        
        TT_TEST_CHECK_EQUAL(sc[0], (tt_sys_char_t)0xe5, "");
        TT_TEST_CHECK_EQUAL(sc[1], (tt_sys_char_t)0x93, "");
        TT_TEST_CHECK_EQUAL(sc[2], (tt_sys_char_t)0x88, "");
        TT_TEST_CHECK_EQUAL(sc[3], (tt_sys_char_t)0xe7, "");
        TT_TEST_CHECK_EQUAL(sc[4], (tt_sys_char_t)0x89, "");
        TT_TEST_CHECK_EQUAL(sc[5], (tt_sys_char_t)0x9b, "");
        TT_TEST_CHECK_EQUAL(sc[6], (tt_sys_char_t)0, "");
#endif
    }

    // gb18030 to sys char
    {
        tt_u8_t test[] = { 0xb9, 0xfe, 0xc5, 0xa3, 0 };
        
		test_len = sizeof(sc);
		ret = tt_sys_char_convert(TT_CHARSET_GB2312,test,sizeof(test),
                                       NULL, (tt_u32_t*)&test_len);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, ""); 

		len = sizeof(sc);
        ret = tt_sys_char_convert(TT_CHARSET_GB18030,test,sizeof(test),
                                  sc, (tt_u32_t*)&len);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

#ifdef _WIN32
        TT_TEST_CHECK_EQUAL(test_len, 6, ""); 
        TT_TEST_CHECK_EQUAL(len, 6, ""); 
        
		// 6 bytes for 2 chinese and ending 0
        TT_TEST_CHECK_EQUAL(sc[0], 0x54c8, "");
        TT_TEST_CHECK_EQUAL(sc[1], 0x725b, "");
        TT_TEST_CHECK_EQUAL(sc[2], 0, "");
#else
        TT_TEST_CHECK_EQUAL(test_len, 7, ""); 
        TT_TEST_CHECK_EQUAL(len, 7, ""); 
        
        TT_TEST_CHECK_EQUAL(sc[0], (tt_sys_char_t)0xe5, "");
        TT_TEST_CHECK_EQUAL(sc[1], (tt_sys_char_t)0x93, "");
        TT_TEST_CHECK_EQUAL(sc[2], (tt_sys_char_t)0x88, "");
        TT_TEST_CHECK_EQUAL(sc[3], (tt_sys_char_t)0xe7, "");
        TT_TEST_CHECK_EQUAL(sc[4], (tt_sys_char_t)0x89, "");
        TT_TEST_CHECK_EQUAL(sc[5], (tt_sys_char_t)0x9b, "");
        TT_TEST_CHECK_EQUAL(sc[6], (tt_sys_char_t)0, "");
#endif        
    }

    // test end
    TT_TEST_CASE_LEAVE()
}
#endif

TT_TEST_ROUTINE_DEFINE(tt_unit_test_c2h)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_char_t c;
    tt_u8_t h;

    TT_TEST_CASE_ENTER()
    // test start

    c = tt_h2c(0, '?');
    TT_TEST_CHECK_EQUAL(c, '0', "");
    c = tt_h2c(9, '?');
    TT_TEST_CHECK_EQUAL(c, '9', "");
    c = tt_h2c(0xa, '?');
    TT_TEST_CHECK_EQUAL(c, 'a', "");
    c = tt_h2c(0xf, '?');
    TT_TEST_CHECK_EQUAL(c, 'f', "");
    c = tt_h2c(~0, '?');
    TT_TEST_CHECK_EQUAL(c, '?', "");

    h = tt_c2h('0', 0xFF);
    TT_TEST_CHECK_EQUAL(h, 0, "");
    h = tt_c2h('9', 0xFF);
    TT_TEST_CHECK_EQUAL(h, 9, "");
    h = tt_c2h('a', 0xFF);
    TT_TEST_CHECK_EQUAL(h, 0xa, "");
    h = tt_c2h('f', 0xFF);
    TT_TEST_CHECK_EQUAL(h, 0xf, "");
    h = tt_c2h('A', 0xFF);
    TT_TEST_CHECK_EQUAL(h, 0xa, "");
    h = tt_c2h('F', 0xFF);
    TT_TEST_CHECK_EQUAL(h, 0xf, "");
    h = tt_c2h('?', 0xFF);
    TT_TEST_CHECK_EQUAL(h, 0xFF, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_align)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_u32_t val;
    tt_ptr_t pval;
    TT_TEST_CASE_ENTER()
    // test start

    // u32 align

    val = 0;
    TT_U32_ALIGN_INC(val, 3);
    TT_TEST_CHECK_EQUAL(val, 0, "");
    val = 0;
    TT_U32_ALIGN_DEC(val, 3);
    TT_TEST_CHECK_EQUAL(val, 0, "");

    val = 100;
    TT_U32_ALIGN_INC(val, 3);
    TT_TEST_CHECK_EQUAL(val, 104, "");
    val = 100;
    TT_U32_ALIGN_DEC(val, 3);
    TT_TEST_CHECK_EQUAL(val, 96, "");

    val = ~0;
    TT_U32_ALIGN_INC(val, 30);
    TT_TEST_CHECK_EQUAL(val, 0, "");
    val = ~0;
    TT_U32_ALIGN_DEC(val, 30);
    TT_TEST_CHECK_EQUAL(val, 0xC0000000, "");

    // ptr align

    pval = (tt_ptr_t)0;
    TT_PTR_ALIGN_INC(pval, 1);
    TT_TEST_CHECK_EQUAL(pval, 0, "");
    pval = (tt_ptr_t)0;
    TT_PTR_ALIGN_DEC(pval, 1);
    TT_TEST_CHECK_EQUAL(pval, 0, "");

    pval = (tt_ptr_t)0x12345678;
    TT_PTR_ALIGN_INC(pval, 7);
    TT_TEST_CHECK_EQUAL(pval, (tt_ptr_t)0x12345680, "");
    pval = (tt_ptr_t)0x12345678;
    TT_PTR_ALIGN_DEC(pval, 7);
    TT_TEST_CHECK_EQUAL(pval, (tt_ptr_t)0x12345600, "");

    pval = (tt_ptr_t)~0;
    TT_PTR_ALIGN_INC(pval, 30);
    TT_TEST_CHECK_EQUAL(pval, 0, "");
    pval = (tt_ptr_t)~0;
    TT_PTR_ALIGN_DEC(pval, 30);
    TT_TEST_CHECK_EQUAL(pval, (tt_ptr_t)0xFFFFFFFFC0000000, "");

    // test end
    TT_TEST_CASE_LEAVE()
}
