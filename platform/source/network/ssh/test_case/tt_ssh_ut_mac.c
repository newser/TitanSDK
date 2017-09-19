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
TT_TEST_ROUTINE_DECLARE(case_sshmac_sha1)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(sshmac_case)

TT_TEST_CASE("case_sshmac_sha1",
             "ssh hmac: sha1",
             case_sshmac_sha1,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE_LIST_DEFINE_END(sshmac_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(SSH_MAC, 0, sshmac_case)

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

    TT_TEST_ROUTINE_DEFINE(case_sshmac_sha1)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_sshmac_t smac;
    tt_result_t ret;
    tt_u8_t hkey[] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
                      0x38, 0x39, 0x30, 0x61, 0x62, 0x63, 0x64,
                      0x65, 0x66, 0x31, 0x32, 0x33, 0x34};
    tt_u8_t dgst[20];
    tt_char_t data[] = "hello,world";

    tt_u8_t d1[] = {0x46, 0xc8, 0x39, 0x53, 0xe7, 0x40, 0x8e, 0x48, 0xfc, 0x86,
                    0x68, 0xd1, 0x29, 0x80, 0x9e, 0x4f, 0x09, 0x29, 0x49, 0x49};
    tt_u8_t d2[] = {0xc0, 0x41, 0x79, 0x53, 0xf7, 0xce, 0x8b, 0x47, 0xee, 0x78,
                    0x13, 0x44, 0x2a, 0x30, 0xe1, 0xbf, 0xb2, 0x54, 0xe6, 0xc7};

    TT_TEST_CASE_ENTER()
    // test start

    tt_sshmac_init(&smac);

    // can not sign and verify yet
    ret = tt_sshmac_sign(&smac,
                         0,
                         (tt_u8_t *)data,
                         (tt_u32_t)strlen(data),
                         dgst,
                         sizeof(dgst));
    TT_UT_EQUAL(ret, TT_FAIL, "");
    ret = tt_sshmac_verify(&smac,
                           0,
                           (tt_u8_t *)data,
                           (tt_u32_t)strlen(data),
                           dgst,
                           sizeof(dgst));
    TT_UT_EQUAL(ret, TT_FAIL, "");

    // set to sha1
    ret = tt_sshmac_setalg(&smac, TT_SSH_MAC_ALG_HMAC_SHA1, hkey, sizeof(hkey));
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    // 1
    ret = tt_sshmac_sign(&smac,
                         0,
                         (tt_u8_t *)data,
                         (tt_u32_t)strlen(data),
                         dgst,
                         sizeof(dgst));
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(tt_memcmp(dgst, d1, sizeof(d1)), 0, "");
    ret = tt_sshmac_verify(&smac,
                           0,
                           (tt_u8_t *)data,
                           (tt_u32_t)strlen(data),
                           d1,
                           sizeof(d1));
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    // 2
    ret = tt_sshmac_sign(&smac,
                         1,
                         (tt_u8_t *)data,
                         (tt_u32_t)strlen(data),
                         dgst,
                         sizeof(dgst));
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(tt_memcmp(dgst, d2, sizeof(d2)), 0, "");
    ret = tt_sshmac_verify(&smac,
                           1,
                           (tt_u8_t *)data,
                           (tt_u32_t)strlen(data),
                           d2,
                           sizeof(d2));
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_sshmac_destroy(&smac);

    // test end
    TT_TEST_CASE_LEAVE()
}
