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

#include "tt_unit_test_case_config.h"
#include <unit_test/tt_unit_test.h>

#include <io/tt_data_node_blob.h>
#include <time/tt_time_reference.h>

// portlayer header files
#include <tt_cstd_api.h>

#include <time.h>

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
TT_TEST_ROUTINE_DECLARE(case_dtn_blob)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(dtnode_case)

TT_TEST_CASE("case_dtn_blob",
             "data node: blob",
             case_dtn_blob,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE_LIST_DEFINE_END(dtnode_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(TEST_UNIT_DATA_NODE, 0, dtnode_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(case_dtn_blob)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(case_dtn_blob)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_dtnode_t *d;
    tt_result_t ret;
    tt_char_t buf[] = "123456789", buf2[10] = "abcdefghi";
    tt_u8_t *p;
    tt_u32_t len, wlen;

    TT_TEST_CASE_ENTER()
    // test start

    // empty blob
    d = tt_dtnode_blob_create(buf, 0, TT_FALSE);
    TT_UT_NOT_NULL(d, "");

    ret = tt_dtnode_read(d, &p, &len);
    TT_UT_EQUAL(ret, TT_E_END, "");
    TT_UT_EQUAL(len, 0, "");

    tt_dtnode_read_update(d, 0);

    ret = tt_dtnode_write(d, (tt_u8_t *)buf2, 0, &wlen);
    TT_UT_EQUAL(ret, TT_E_END, "");
    ret = tt_dtnode_write(d, (tt_u8_t *)buf2, 1, &wlen);
    TT_UT_EQUAL(ret, TT_E_END, "");

    tt_dtnode_destroy(d);

    // read content
    d = tt_dtnode_blob_create(buf, sizeof(buf), TT_FALSE);
    TT_UT_NOT_NULL(d, "");

    ret = tt_dtnode_read(d, &p, &len);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(p, (tt_u8_t *)buf, "");
    TT_UT_EQUAL(len, sizeof(buf), "");
    ret = tt_dtnode_read(d, &p, &len);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(p, (tt_u8_t *)buf, "");
    TT_UT_EQUAL(len, sizeof(buf), "");

    tt_dtnode_read_update(d, 1);
    ret = tt_dtnode_read(d, &p, &len);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(p, (tt_u8_t *)buf + 1, "");
    TT_UT_EQUAL(len, sizeof(buf) - 1, "");

    tt_dtnode_read_update(d, sizeof(buf) - 1);
    ret = tt_dtnode_read(d, &p, &len);
    TT_UT_EQUAL(ret, TT_E_END, "");
    TT_UT_EQUAL(p, NULL, "");
    TT_UT_EQUAL(len, 0, "");

    tt_dtnode_destroy(d);

    // write content
    d = tt_dtnode_blob_create(buf, sizeof(buf), TT_FALSE);
    TT_UT_NOT_NULL(d, "");

    ret = tt_dtnode_write(d, (tt_u8_t *)buf2, 2, &wlen);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(wlen, 2, "");
    ret = tt_dtnode_write(d, (tt_u8_t *)buf2 + 2, 3, &wlen);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(wlen, 3, "");
    ret = tt_dtnode_write(d, (tt_u8_t *)buf2 + 5, 10, &wlen);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(wlen, 5, "");
    ret = tt_dtnode_write(d, (tt_u8_t *)buf2 + 5, 4, &wlen);
    TT_UT_EQUAL(ret, TT_E_END, "");
    TT_UT_EQUAL(wlen, 0, "");

    tt_dtnode_destroy(d);

    // to free
    d = tt_dtnode_blob_create(tt_malloc(10), 10, TT_TRUE);
    TT_UT_NOT_NULL(d, "");
    tt_dtnode_destroy(d);

    // test end
    TT_TEST_CASE_LEAVE()
}
