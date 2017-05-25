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

#define __DH_PARAM_FILE "123xxxabc_DH_param"

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

static void __dh_prepare(void *);
static tt_bool_t has_dh;

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

// === routine declarations ================
TT_TEST_ROUTINE_DECLARE(tt_unit_test_dh)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(crypto_dh_case)

TT_TEST_CASE("tt_unit_test_dh",
             "crypto: dh",
             tt_unit_test_dh,
             NULL,
             __dh_prepare,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE_LIST_DEFINE_END(crypto_dh_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(CRYPTO_UT_DH, 0, crypto_dh_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(tt_unit_test_dh)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(tt_unit_test_dh)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_dh_t dh, d2;
    tt_u8_t pub[128], pub2[128], s[128], s2[128];
    tt_result_t ret;
    tt_u32_t n1, n2, sn, sn2;

    TT_TEST_CASE_ENTER()
    // test start

    tt_dh_init(&dh);
    tt_dh_init(&d2);
    TT_UT_EQUAL(tt_dh_size(&dh), 0, "");

    ret = tt_dh_load_param_file(&dh, __DH_PARAM_FILE);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_dh_size(&dh), 128, "");
    ret = tt_dh_load_param_file(&d2, __DH_PARAM_FILE);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_dh_size(&d2), 128, "");

    // pub1
    n1 = sizeof(pub);
    ret = tt_dh_generate_pub(&dh, 128, pub, n1);
    TT_UT_SUCCESS(ret, "");

    n2 = tt_dh_size(&dh);
    TT_UT_EXP(sizeof(pub2) >= tt_dh_size(&dh), "");
    ret = tt_dh_get_pub(&dh, TT_TRUE, pub2, n2);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(n1, n2, "");
    TT_UT_EQUAL(tt_memcmp(pub, pub2, n1), 0, "");

    ret = tt_dh_get_pub(&dh, TT_FALSE, pub2, tt_dh_size(&dh));
    TT_UT_SUCCESS(ret, "");

    // pub2
    n2 = sizeof(pub2);
    ret = tt_dh_generate_pub(&d2, 128, pub2, n2);
    TT_UT_SUCCESS(ret, "");

    // exchange pub
    ret = tt_dh_set_pub(&dh, pub2, n2);
    TT_UT_SUCCESS(ret, "");
    ret = tt_dh_set_pub(&d2, pub, n1);
    TT_UT_SUCCESS(ret, "");

    // derive
    sn = sizeof(s);
    ret = tt_dh_derive(&dh, s, &sn);
    TT_UT_SUCCESS(ret, "");

    sn2 = sizeof(s2);
    ret = tt_dh_derive(&dh, s2, &sn2);
    TT_UT_SUCCESS(ret, "");

    TT_UT_EQUAL(sn, sn2, "");
    TT_UT_EQUAL(tt_memcmp(s, s2, sn), 0, "");

    // get secret
    TT_UT_EXP(sizeof(s2) >= tt_dh_size(&dh), "");
    ret = tt_dh_get_secret(&dh, s2, tt_dh_size(&dh));
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(sn, sn2, "");
    TT_UT_EQUAL(tt_memcmp(s, s2, sn), 0, "");

    tt_dh_destroy(&dh);
    tt_dh_destroy(&d2);

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_char_t __dh_param[] =
    "-----BEGIN DH PARAMETERS-----\n"
    "MIGHAoGBAODifjDEOwRSU7mrzhoorTRDm7sLX0kEiolCN8RW69y5TWeyP7MCx06W\n"
    "YgchyzNy4O5wqCEuQvWZPE3sgrCl+xh6VhQxMnc/zA7c/Yw/lEtwL+FXzeKgtI9F\n"
    "F/H+6BONZEKT/1rlrBEmDVB/Zozgm8a/SmtNzR/w0iCQ/p8/FlprAgEC\n"
    "-----END DH PARAMETERS-----\n";

void __dh_prepare(void *p)
{
    tt_file_t f;

    if (has_dh) {
        return;
    }

    // gen rsa priv
    if (!TT_OK(tt_fopen(&f,
                        __DH_PARAM_FILE,
                        TT_FO_WRITE | TT_FO_CREAT | TT_FO_TRUNC,
                        NULL))) {
        TT_ERROR("fail to open %s", __DH_PARAM_FILE);
        return;
    }
    if (!TT_OK(tt_fwrite(&f,
                         (tt_u8_t *)__dh_param,
                         sizeof(__dh_param) - 1,
                         NULL))) {
        TT_ERROR("fail to write %s", __DH_PARAM_FILE);
        return;
    }
    tt_fclose(&f);

    has_dh = TT_TRUE;
}
