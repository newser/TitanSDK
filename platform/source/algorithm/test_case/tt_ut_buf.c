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
TT_TEST_ROUTINE_DECLARE(case_buf_null)

TT_TEST_ROUTINE_DECLARE(case_buf_get_basic)
TT_TEST_ROUTINE_DECLARE(case_buf_get_rand)
TT_TEST_ROUTINE_DECLARE(case_buf_get_hexstr)

TT_TEST_ROUTINE_DECLARE(case_bufp_str_basic)
TT_TEST_ROUTINE_DECLARE(case_bufp_str_rand)

TT_TEST_ROUTINE_DECLARE(case_buftok)

TT_TEST_ROUTINE_DECLARE(case_buf_put_basic)
TT_TEST_ROUTINE_DECLARE(case_buf_put_rand)

TT_TEST_ROUTINE_DECLARE(case_buf_remove)

TT_TEST_ROUTINE_DECLARE(case_iobuf)
TT_TEST_ROUTINE_DECLARE(case_iobuf_format)
TT_TEST_ROUTINE_DECLARE(case_iobuf_format_cp)

TT_TEST_ROUTINE_DECLARE(case_blob)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(buf_case)

TT_TEST_CASE("case_buf_null",
             "testing basic buf api, null buff",
             case_buf_null,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("case_buf_get_basic",
                 "testing basic buf parser API",
                 case_buf_get_basic,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_buf_get_rand",
                 "testing buf parser random input",
                 case_buf_get_rand,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_buf_put_basic",
                 "testing basic buf put API",
                 case_buf_put_basic,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_buf_put_rand",
                 "testing buf put random input",
                 case_buf_put_rand,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_buf_remove",
                 "testing buf remove",
                 case_buf_remove,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_buf_get_hexstr",
                 "testing buf get hex string",
                 case_buf_get_hexstr,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_buftok",
                 "testing buf tokenize",
                 case_buftok,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_blob",
                 "testing blob api",
                 case_blob,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(buf_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(ALG_UT_BUF, 0, buf_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(case_blob)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(case_buf_null)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_buf_t buf, buf2;
    tt_result_t ret;
    tt_u8_t *p;
    tt_u32_t n;
    tt_u8_t bn[20] = {0};

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&buf, NULL);
    tt_buf_init(&buf2, NULL);

    TT_UT_EQUAL(tt_buf_empty(&buf), TT_TRUE, "");

    // rp
    tt_buf_get_rptr(&buf, &p, &n);
    TT_UT_NOT_EQUAL(p, NULL, "");
    TT_UT_EQUAL(n, 0, "");
    // tt_buf_set_rptr(&buf, (tt_u8_t *)&ret);
    // TT_UT_NOT_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_buf_inc_rp(&buf, 100);
    TT_UT_NOT_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_buf_dec_rp(&buf, 1);
    TT_UT_NOT_EQUAL(ret, TT_SUCCESS, "");

    // wp
    tt_buf_get_wptr(&buf, &p, &n);
    TT_UT_NOT_EQUAL(p, NULL, "");
    TT_UT_EQUAL(n, TT_BUF_INIT_SIZE, "");
    ret = tt_buf_inc_wp(&buf, 100);
    TT_UT_NOT_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_buf_dec_wp(&buf, 1);
    TT_UT_NOT_EQUAL(ret, TT_SUCCESS, "");

    // get
    ret = tt_buf_get(&buf, bn, 1);
    TT_UT_EQUAL(ret, TT_BUFFER_INCOMPLETE, "");
    ret = tt_buf_get_nocopy(&buf, &p, 1);
    TT_UT_EQUAL(ret, TT_BUFFER_INCOMPLETE, "");
    n = tt_buf_get_hexstr(&buf, NULL, 1);
    TT_UT_EQUAL(n, 0, "");
    n = tt_buf_get_hexstr(&buf, (tt_char_t *)bn, 1);
    TT_UT_EQUAL(n, 0, "");
    ret = tt_buf_peek(&buf, bn, 1);
    TT_UT_EQUAL(ret, TT_BUFFER_INCOMPLETE, "");

    ret = tt_buf_compress(&buf);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(buf.p, buf.initbuf, "");
    TT_UT_EQUAL(buf.rpos, 0, "");
    TT_UT_EQUAL(buf.wpos, 0, "");

    // common
    n = tt_buf_cmp(&buf, &buf);
    TT_UT_EQUAL(n, 0, "");
    n = tt_buf_cmp(&buf, &buf2);
    TT_UT_EQUAL(n, 0, "");

    tt_buf_remove(&buf, 0);
    tt_buf_remove_range(&buf, 0, 1);

    // format
    ret = tt_buf_put_cstr2hex(&buf, "");
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(tt_buf_empty(&buf), TT_TRUE, "");

    ret = tt_buf_put_hex2cstr(&buf, bn, 0);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_buf_clear(&buf);
    TT_UT_EQUAL(tt_buf_empty(&buf), TT_TRUE, "");

    tt_buf_try_refine(&buf, 0);
    TT_UT_EQUAL(tt_buf_empty(&buf), TT_TRUE, "");

    tt_buf_destroy(&buf);
    tt_buf_destroy(&buf2);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_buf_get_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_u32_t v32 = 0x12345678, v32_ret;
    tt_u16_t v16 = 0x1234, v16_ret;
    tt_u8_t v8[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    tt_u8_t v8_ret[sizeof(v8)];

    tt_result_t ret;
    tt_buf_t buf;

    TT_TEST_CASE_ENTER()
    // test start

    // u32
    ret = tt_buf_create_copy(&buf, (tt_u8_t *)&v32, sizeof(v32), NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_buf_get_u32(&buf, &v32_ret);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(v32_ret, v32, "");
    TT_UT_EQUAL(buf.wpos, sizeof(v32), "");
    TT_UT_EQUAL(buf.rpos, sizeof(v32), "");
    TT_UT_EQUAL(TT_BUF_RLEN(&buf), 0, "");

    ret = tt_buf_dec_rp(&buf, sizeof(tt_u32_t) * 2);
    TT_UT_NOT_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_buf_dec_rp(&buf, sizeof(tt_u32_t));
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_buf_get_u32(&buf, &v32_ret);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(v32_ret, v32, "");
    TT_UT_EQUAL(buf.wpos, sizeof(v32), "");
    TT_UT_EQUAL(buf.rpos, sizeof(v32), "");
    TT_UT_EQUAL(TT_BUF_RLEN(&buf), 0, "");

    // no data
    ret = tt_buf_get_u32(&buf, &v32_ret);
    TT_UT_EQUAL(ret, TT_BUFFER_INCOMPLETE, "");
    TT_UT_EQUAL(buf.wpos, sizeof(v32), "");
    TT_UT_EQUAL(buf.rpos, sizeof(v32), "");

    tt_buf_destroy(&buf);

    // invalid u32
    ret = tt_buf_create_copy(&buf, (tt_u8_t *)&v32, sizeof(v32) - 1, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_buf_get_u32(&buf, &v32_ret);
    TT_UT_EQUAL(ret, TT_BUFFER_INCOMPLETE, "");
    TT_UT_EQUAL(buf.wpos, sizeof(v32) - 1, "");
    TT_UT_EQUAL(buf.rpos, 0, "");
    tt_buf_destroy(&buf);

    // u32_h
    ret = tt_buf_create_copy(&buf, (tt_u8_t *)&v32, sizeof(v32), NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_buf_get_u32_h(&buf, &v32_ret);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(v32_ret, tt_ntoh32(v32), "");
    TT_UT_EQUAL(buf.wpos, sizeof(v32), "");
    TT_UT_EQUAL(buf.rpos, sizeof(v32), "");
    TT_UT_EQUAL(TT_BUF_RLEN(&buf), 0, "");
    tt_buf_destroy(&buf);

    // invalid u32
    ret = tt_buf_create_copy(&buf, (tt_u8_t *)&v32, sizeof(v32) - 1, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_buf_get_u32_h(&buf, &v32_ret);
    TT_UT_EQUAL(ret, TT_BUFFER_INCOMPLETE, "");
    TT_UT_EQUAL(buf.wpos, sizeof(v32) - 1, "");
    TT_UT_EQUAL(buf.rpos, 0, "");
    tt_buf_destroy(&buf);

    // u16
    ret = tt_buf_create_copy(&buf, (tt_u8_t *)&v16, sizeof(v16), NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_buf_get_u16(&buf, &v16_ret);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(v16_ret, (v16), "");
    TT_UT_EQUAL(buf.wpos, sizeof(v16), "");
    TT_UT_EQUAL(buf.rpos, sizeof(v16), "");
    TT_UT_EQUAL(TT_BUF_RLEN(&buf), 0, "");
    tt_buf_destroy(&buf);

    // invalid u16
    ret = tt_buf_create_copy(&buf, (tt_u8_t *)&v16, sizeof(v16) - 1, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_buf_get_u16(&buf, &v16_ret);
    TT_UT_EQUAL(ret, TT_BUFFER_INCOMPLETE, "");
    TT_UT_EQUAL(buf.wpos, sizeof(v16) - 1, "");
    TT_UT_EQUAL(buf.rpos, 0, "");
    tt_buf_destroy(&buf);

    // u16
    ret = tt_buf_create_copy(&buf, (tt_u8_t *)&v16, sizeof(v16), NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_buf_get_u16_h(&buf, &v16_ret);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(v16_ret, tt_ntoh16(v16), "");
    TT_UT_EQUAL(buf.wpos, sizeof(v16), "");
    TT_UT_EQUAL(buf.rpos, sizeof(v16), "");
    TT_UT_EQUAL(TT_BUF_RLEN(&buf), 0, "");
    tt_buf_destroy(&buf);

    // invalid u16
    ret = tt_buf_create_copy(&buf, (tt_u8_t *)&v16, sizeof(v16) - 1, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_buf_get_u16_h(&buf, &v16_ret);
    TT_UT_EQUAL(ret, TT_BUFFER_INCOMPLETE, "");
    TT_UT_EQUAL(buf.wpos, sizeof(v16) - 1, "");
    TT_UT_EQUAL(buf.rpos, 0, "");
    tt_buf_destroy(&buf);

    // u8
    ret = tt_buf_create_copy(&buf, v8, sizeof(v8), NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_buf_get(&buf, v8_ret, sizeof(v8));
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(tt_memcmp(v8, v8_ret, sizeof(v8)), 0, "");
    TT_UT_EQUAL(buf.wpos, sizeof(v8), "");
    TT_UT_EQUAL(buf.rpos, sizeof(v8), "");
    TT_UT_EQUAL(TT_BUF_RLEN(&buf), 0, "");
    tt_buf_destroy(&buf);

    // invalid u8
    ret = tt_buf_create_copy(&buf, v8, sizeof(v8) - 1, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_buf_get(&buf, v8_ret, sizeof(v8));
    TT_UT_EQUAL(ret, TT_BUFFER_INCOMPLETE, "");
    TT_UT_EQUAL(buf.wpos, sizeof(v8) - 1, "");
    TT_UT_EQUAL(buf.rpos, 0, "");
    tt_buf_destroy(&buf);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_buf_get_rand)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_u32_t test_num = 1000, i, j;
    tt_u8_t *test_buf;
    tt_u32_t test_buf_size;

    TT_TEST_CASE_ENTER()
    // test start

    for (i = 0; i < test_num; ++i) {
        test_buf_size = tt_rand_u32() % 4000 + 1;
        test_buf = tt_malloc(test_buf_size);
        TT_UT_NOT_EQUAL(test_buf, NULL, "");

        for (j = 0; j < test_buf_size; ++j) {
            test_buf[j] = tt_rand_u32() & 0xff;
        }

        do {
            tt_buf_t buf;
            tt_result_t ret;

            tt_u32_t __r_u32;
            tt_u16_t __r_u16;
            tt_u8_t __r_u8[100];

            tt_u32_t org_rd_pos;
            tt_u32_t org_len;

            ret = tt_buf_create_copy(&buf, test_buf, test_buf_size, NULL);
            TT_UT_EQUAL(ret, TT_SUCCESS, "");
            tt_free(test_buf);

            while (1) {
                tt_u32_t __n = tt_rand_u32() % 5;

                org_rd_pos = buf.rpos;
                org_len = TT_BUF_RLEN(&buf);

                switch (__n) {
                    case 0:
                        ret = tt_buf_get_u32(&buf, &__r_u32);
                        if (!TT_OK(ret)) {
                            goto __gr_out;
                        }

                        TT_UT_EQUAL(buf.rpos, org_rd_pos + 4, "");
                        TT_UT_EQUAL(TT_BUF_RLEN(&buf), org_len - 4, "");
                        break;
                    case 1:
                        ret = tt_buf_get_u32_h(&buf, &__r_u32);
                        if (!TT_OK(ret))
                            goto __gr_out;

                        TT_UT_EQUAL(buf.rpos, org_rd_pos + 4, "");
                        TT_UT_EQUAL(TT_BUF_RLEN(&buf), org_len - 4, "");
                        break;
                    case 2:
                        ret = tt_buf_get_u16(&buf, &__r_u16);
                        if (!TT_OK(ret))
                            goto __gr_out;

                        TT_UT_EQUAL(buf.rpos, org_rd_pos + 2, "");
                        TT_UT_EQUAL(TT_BUF_RLEN(&buf), org_len - 2, "");
                        break;
                    case 3:
                        ret = tt_buf_get_u16_h(&buf, &__r_u16);
                        if (!TT_OK(ret))
                            goto __gr_out;

                        TT_UT_EQUAL(buf.rpos, org_rd_pos + 2, "");
                        TT_UT_EQUAL(TT_BUF_RLEN(&buf), org_len - 2, "");
                        break;
                    case 4: {
                        tt_u32_t __r_len = tt_rand_u32() % 99 + 1;
                        ret = tt_buf_get(&buf, __r_u8, __r_len);
                        if (!TT_OK(ret))
                            goto __gr_out;

                        TT_UT_EQUAL(buf.rpos, org_rd_pos + __r_len, "");
                        TT_UT_EQUAL(TT_BUF_RLEN(&buf), org_len - __r_len, "");
                    } break;

                    default:
                        break;
                }
            }
        __gr_out:
            TT_UT_EXP(buf.rpos <= buf.wpos, "");
            TT_UT_EXP(buf.wpos <= buf.size, "");

            tt_buf_destroy(&buf);
        } while (0);
    }

    // test end
    TT_TEST_CASE_LEAVE()
}

/////////////////////////////
/////////////////////////////

TT_TEST_ROUTINE_DEFINE(case_buf_put_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_u32_t v32 = 0x12345678, v32_ret;
    tt_u16_t v16 = 0x1234, v16_ret;
    tt_u8_t v8[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    tt_u8_t v8_ret[sizeof(v8)];

    tt_result_t ret;
    tt_buf_t buf;

    TT_TEST_CASE_ENTER()
    // test start

    // u32
    ret = tt_buf_create(&buf, 0, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_buf_put_u32(&buf, v32);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(buf.wpos, sizeof(v32), "");
    TT_UT_EQUAL(buf.rpos, 0, "");
    TT_UT_EQUAL(TT_BUF_RLEN(&buf), sizeof(v32), "");
    ret = tt_buf_get_u32(&buf, &v32_ret);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(v32_ret, v32, "");
    TT_UT_EQUAL(buf.wpos, sizeof(v32), "");
    TT_UT_EQUAL(buf.rpos, sizeof(v32), "");

    ret = tt_buf_dec_wp(&buf, 1);
    TT_UT_NOT_EQUAL(ret, TT_SUCCESS, "");

    // put, dec, put
    tt_buf_reset_rwp(&buf);
    ret = tt_buf_put_u32(&buf, v32);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(buf.wpos, sizeof(v32), "");
    TT_UT_EQUAL(buf.rpos, 0, "");
    TT_UT_EQUAL(TT_BUF_RLEN(&buf), sizeof(v32), "");
    TT_UT_EQUAL(tt_buf_empty(&buf), TT_FALSE, "");
    ret = tt_buf_dec_wp(&buf, sizeof(tt_u32_t));
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_buf_put_u32(&buf, v32);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(buf.wpos, sizeof(v32), "");
    TT_UT_EQUAL(buf.rpos, 0, "");
    TT_UT_EQUAL(TT_BUF_RLEN(&buf), sizeof(v32), "");
    ret = tt_buf_get_u32(&buf, &v32_ret);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(v32_ret, v32, "");
    TT_UT_EQUAL(buf.wpos, sizeof(v32), "");
    TT_UT_EQUAL(buf.rpos, sizeof(v32), "");
    tt_buf_destroy(&buf);

    // u32_n
    ret = tt_buf_create(&buf, 0, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_buf_put_u32_n(&buf, v32);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(buf.wpos, sizeof(v32), "");
    TT_UT_EQUAL(buf.rpos, 0, "");
    TT_UT_EQUAL(TT_BUF_RLEN(&buf), sizeof(v32), "");
    ret = tt_buf_get_u32(&buf, &v32_ret);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(v32_ret, tt_hton32(v32), "");
    TT_UT_EQUAL(buf.wpos, sizeof(v32), "");
    TT_UT_EQUAL(buf.rpos, sizeof(v32), "");
    tt_buf_destroy(&buf);

    // u16
    ret = tt_buf_create(&buf, 0, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_buf_put_u16(&buf, v16);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(buf.wpos, sizeof(v16), "");
    TT_UT_EQUAL(buf.rpos, 0, "");
    TT_UT_EQUAL(TT_BUF_RLEN(&buf), sizeof(v16), "");
    ret = tt_buf_get_u16(&buf, &v16_ret);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(v16_ret, v16, "");
    TT_UT_EQUAL(buf.wpos, sizeof(v16), "");
    TT_UT_EQUAL(buf.rpos, sizeof(v16), "");
    tt_buf_destroy(&buf);

    // u16_n
    ret = tt_buf_create(&buf, 0, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_buf_put_u16_n(&buf, v16);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(buf.wpos, sizeof(v16), "");
    TT_UT_EQUAL(buf.rpos, 0, "");
    TT_UT_EQUAL(TT_BUF_RLEN(&buf), sizeof(v16), "");
    ret = tt_buf_get_u16(&buf, &v16_ret);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(v16_ret, tt_hton16(v16), "");
    TT_UT_EQUAL(buf.wpos, sizeof(v16), "");
    TT_UT_EQUAL(buf.rpos, sizeof(v16), "");
    tt_buf_destroy(&buf);

    // u8
    ret = tt_buf_create(&buf, 0, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_buf_put(&buf, v8, sizeof(v8));
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(buf.wpos, sizeof(v8), "");
    TT_UT_EQUAL(buf.rpos, 0, "");
    TT_UT_EQUAL(TT_BUF_RLEN(&buf), sizeof(v8), "");
    ret = tt_buf_get(&buf, v8_ret, sizeof(v8_ret));
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(tt_memcmp(v8, v8_ret, sizeof(v8)), 0, "");
    TT_UT_EQUAL(buf.wpos, sizeof(v8), "");
    TT_UT_EQUAL(buf.rpos, sizeof(v8), "");
    tt_buf_destroy(&buf);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_buf_put_rand)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_u32_t test_num = 1000, i;

    TT_TEST_CASE_ENTER()
    // test start

    for (i = 0; i < test_num; ++i) {
        do {
            tt_buf_t buf;
            tt_result_t ret;
            tt_buf_attr_t attr;

            tt_u8_t __r_u8[100];

            tt_u32_t org_wr_pos;
            tt_u32_t org_len;

            tt_buf_attr_default(&attr);
            attr.min_extend = 7;
            attr.max_extend = 8;
            attr.max_limit = 12; // max 4k

            ret = tt_buf_create(&buf, 0, &attr);
            TT_UT_EQUAL(ret, TT_SUCCESS, "");

            while (1) {
                tt_u32_t __n = tt_rand_u32() % 6;

                org_wr_pos = buf.wpos;
                org_len = TT_BUF_WLEN(&buf);

                switch (__n) {
                    case 0:
                        ret = tt_buf_put_u32(&buf, org_len);
                        if (!TT_OK(ret))
                            goto __gr_out;

                        TT_UT_EQUAL(buf.wpos, org_wr_pos + 4, "");
                        break;
                    case 1:
                        ret = tt_buf_put_u32_n(&buf, org_len);
                        if (!TT_OK(ret))
                            goto __gr_out;

                        TT_UT_EQUAL(buf.wpos, org_wr_pos + 4, "");
                        break;
                    case 2:
                        ret = tt_buf_put_u16(&buf, (tt_u16_t)org_len);
                        if (!TT_OK(ret))
                            goto __gr_out;

                        TT_UT_EQUAL(buf.wpos, org_wr_pos + 2, "");
                        break;
                    case 3:
                        ret = tt_buf_put_u16_n(&buf, (tt_u16_t)org_len);
                        if (!TT_OK(ret))
                            goto __gr_out;

                        TT_UT_EQUAL(buf.wpos, org_wr_pos + 2, "");
                        break;
                    case 4: {
                        tt_u32_t __r_len = tt_rand_u32() % 99 + 1;
                        ret = tt_buf_put(&buf, __r_u8, __r_len);
                        if (!TT_OK(ret))
                            goto __gr_out;

                        TT_UT_EQUAL(buf.wpos, org_wr_pos + __r_len, "");
                        break;
                    } break;
                    case 5: {
                        tt_u32_t n = TT_BUF_RLEN(&buf);

                        ret = tt_buf_compress(&buf);
                        TT_UT_EQUAL(ret, TT_SUCCESS, "");
                        TT_UT_EQUAL(TT_BUF_RLEN(&buf), n, "");
                        TT_UT_EQUAL(buf.rpos, 0, "");
                        TT_UT_EQUAL(buf.wpos, n, "");
                    } break;

                    default:
                        break;
                }
            }
        __gr_out:
            TT_INFO("put out");
            TT_UT_EXP(buf.rpos <= buf.wpos, "");
            TT_UT_EXP(buf.wpos <= buf.size, "");

            tt_buf_destroy(&buf);
        } while (0);
    }

    // test end
    TT_TEST_CASE_LEAVE()
}

#if 0
//////////////////////
// str
//////////////////////

tt_result_t __stm_get_strchr(tt_buf_t *stm,
                               tt_char_t c, tt_u32_t max_len)
{
    TT_BUFP_START(pos, len);
    TT_BUFP_CHR(max_len, c);
    TT_BUFP_END(pos, len);
    TT_BUFP_RETURN();
}

tt_result_t __stm_get_strchr_next(tt_buf_t *stm,
                                    tt_char_t c, tt_u32_t max_len)
{
    TT_BUFP_START(pos, len);
    TT_BUFP_CHR_NEXT(max_len, c);
    TT_BUFP_END(pos, len);
    TT_BUFP_RETURN();
}

TT_TEST_ROUTINE_DEFINE(case_bufp_str_basic)
{
    //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    const tt_char_t __t_str[] = "aaabbacd";
    tt_result_t __t_status;
    
    tt_u8_t *pos;
    tt_u32_t len;
    
    TT_TEST_CASE_ENTER()
    // test start
    
    ///////////////////////////////
    // strchr
    
    // found
    pos = (tt_u8_t*)__t_str;
    len = sizeof(__t_str);
    __t_status = __stm_get_strchr(&pos, &len, 'b', 10);
    TT_UT_EQUAL(__t_status, TT_SUCCESS, "");
    TT_UT_EQUAL(pos, TT_PTR_INC(tt_u8_t, __t_str, 3), "");
    TT_UT_EQUAL(len, sizeof(__t_str) - 3, "");

    // not exist
    pos = (tt_u8_t*)__t_str;
    len = sizeof(__t_str);
    __t_status = __stm_get_strchr(&pos, &len, 'x', 10);
    TT_UT_EQUAL(__t_status, TT_BUFP_STATUS_INCOMPLETE, "");
    TT_UT_EQUAL(pos, (tt_u8_t*)__t_str, "");
    TT_UT_EQUAL(len, sizeof(__t_str), "");

    // exceed max len
    pos = (tt_u8_t*)__t_str;
    len = sizeof(__t_str);
    __t_status = __stm_get_strchr(&pos, &len, 'b', 2);
    TT_UT_EQUAL(__t_status, TT_BUFP_STATUS_DISCARD_ALL, "");
    TT_UT_EQUAL(pos, TT_PTR_INC(tt_u8_t, __t_str, sizeof(__t_str)), "");
    TT_UT_EQUAL(len, 0, "");

    ///////////////////////////////
    // strchr_next
    
    // found
    pos = (tt_u8_t*)__t_str;
    len = sizeof(__t_str);
    __t_status = __stm_get_strchr_next(&pos, &len, 'b', 10);
    TT_UT_EQUAL(__t_status, TT_SUCCESS, "");
    TT_UT_EQUAL(pos, TT_PTR_INC(tt_u8_t, __t_str, 4), "");
    TT_UT_EQUAL(len, sizeof(__t_str) - 4, "");
    
    // not exist
    pos = (tt_u8_t*)__t_str;
    len = sizeof(__t_str);
    __t_status = __stm_get_strchr_next(&pos, &len, 'x', 10);
    TT_UT_EQUAL(__t_status, TT_BUFP_STATUS_INCOMPLETE, "");
    TT_UT_EQUAL(pos, (tt_u8_t*)__t_str, "");
    TT_UT_EQUAL(len, sizeof(__t_str), "");
    
    // exceed max len
    pos = (tt_u8_t*)__t_str;
    len = sizeof(__t_str);
    __t_status = __stm_get_strchr_next(&pos, &len, 'b', 2);
    TT_UT_EQUAL(__t_status, TT_BUFP_STATUS_DISCARD_ALL, "");
    TT_UT_EQUAL(pos, TT_PTR_INC(tt_u8_t, __t_str, sizeof(__t_str)), "");
    TT_UT_EQUAL(len, 0, "");
    
    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_bufp_str_rand)
{
    //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_u32_t n = 1000, i;
    tt_u8_t *buf = NULL;
    tt_u32_t buf_size = 0;
    
    TT_TEST_CASE_ENTER()
    // test start
    
    buf_size = tt_rand_u32() % 1000;
    buf = (tt_u8_t*)tt_malloc(buf_size);
    TT_UT_NOT_EQUAL(buf, NULL, "");
    
    for (i = 0; i < buf_size; ++i)
    {
        buf[i] = (tt_u8_t)tt_rand_u32() % 30;
        // val: 0-30
    }
    
    for (i = 0; i < n; ++i)
    {
        do {
            tt_u8_t *__t = buf;
            tt_u32_t __l = buf_size;
            tt_u32_t op = tt_rand_u32() %2;
            
            tt_u8_t *org_pos;
            tt_u32_t org_len;
            
            TT_BUFP_START(&__t, &__l);
            while (1)
            {
                switch (op) {
                    case 0:
                    {
                        // strchr
                        tt_u8_t __rc = (tt_u8_t)tt_rand_u32()%30;
                        tt_u32_t __ml = tt_rand_u32() % 100;
                        org_pos = TT_BUFP_POS; org_len = TT_BUFP_LEFT_LEN;
                        TT_BUFP_DO(__stm_get_strchr(&TT_BUFP_POS, &TT_BUFP_LEFT_LEN,
                                                 __rc, __ml));
                        TT_UT_EQUAL(*TT_BUFP_POS, __rc, "");
                        TT_UT_EQUAL(TT_PTR_DIFF(org_pos, TT_BUFP_POS),
                                            org_len - TT_BUFP_LEFT_LEN, "");
                    }
                        break;

                    case 1:
                    {
                        // strchr
                        tt_u8_t __rc = (tt_u8_t)tt_rand_u32()%30;
                        tt_u32_t __ml = tt_rand_u32() % 100;
                        org_pos = TT_BUFP_POS; org_len = TT_BUFP_LEFT_LEN;
                        TT_BUFP_DO(__stm_get_strchr_next(&TT_BUFP_POS, &TT_BUFP_LEFT_LEN,
                                                 __rc, __ml));
                        if (TT_BUFP_POS != org_pos) {
                            TT_UT_EQUAL(*(TT_BUFP_POS-1), __rc, "");
                        }
                        TT_UT_EQUAL(TT_PTR_DIFF(org_pos, TT_BUFP_POS),
                                            org_len - TT_BUFP_LEFT_LEN, "");
                    }
                        break;

                    default:
                        break;
                }
            }
            TT_BUFP_END(&__t, &__l);
            
            TT_UT_EXP(__t <= TT_PTR_INC(tt_u8_t, buf, buf_size), "");
            if (__l > buf_size) {
                TT_ASSERT(0);
            }
            TT_UT_EXP(__l <= buf_size, "");
        } while (0);
    }
    
    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_u32_t __btp_inval_tnum;
static tt_u32_t __btp_tnum;

tt_result_t __btb_counter(IN const tt_u8_t *start,
                               IN const tt_u8_t *end,
                               IN void *param)
{
    TT_ASSERT(param == &__btp_tnum);
    
    if (start == end) ++__btp_inval_tnum;
    ++__btp_tnum;
    
    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(case_bufp_byte_tok)
{
    //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    const tt_char_t *c1[] = {
        ".",
        ".xx.xx.",
        "..",
        "xxx",
        "xx..",
        "",
    };
    tt_u32_t n1[sizeof(c1)/sizeof(void*)] = {2,4,3,1,3,1};
    tt_u32_t n1_inv_bs[sizeof(c1)/sizeof(void*)] = {
        1, // second token is "\0", valid
        1,
        2,0,1,0};
    tt_u32_t n1_inv_str[sizeof(c1)/sizeof(void*)] = {
        2, // second token is invalid
        2,
        3,0,2,
        1 // only an ending flag
    };
    
    tt_u32_t i;
    
    TT_TEST_CASE_ENTER()
    // test start
    
    // byte sequence
    for (i = 0; i < sizeof(c1)/sizeof(void*); ++i)
    {
        tt_u8_t *buf = (tt_u8_t*)c1[i];
        tt_u32_t len = (tt_u32_t)strlen(c1[i])+1;
        tt_u8_t *end = TT_PTR_INC(tt_u8_t, buf, len);
        tt_result_t ret;
        
        __btp_inval_tnum = __btp_tnum = 0;
        ret = tt_bufp_byte_tok(&buf, &len, '.',
                                  TT_BUFP_BYTE_TOK_NOEND,
                                  __btb_counter, &__btp_tnum);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(__btp_tnum, n1[i], "");
        TT_UT_EQUAL(__btp_inval_tnum, n1_inv_bs[i], "");
        
        TT_UT_EQUAL(buf, end, "");
        TT_UT_EQUAL(len, 0, "");
    }

    // string like, ending null before len
    for (i = 0; i < sizeof(c1)/sizeof(void*); ++i)
    {
        tt_u8_t *buf = (tt_u8_t*)c1[i];
        tt_u32_t len = (tt_u32_t)strlen(c1[i]) + 1;
        tt_u8_t *end = TT_PTR_INC(tt_u8_t, buf, len);
        tt_result_t ret;
        
        __btp_inval_tnum = __btp_tnum = 0;
        ret = tt_bufp_byte_tok(&buf, &len, '.', 0, __btb_counter, &__btp_tnum);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(__btp_tnum, n1[i], "");
        TT_UT_EQUAL(__btp_inval_tnum, n1_inv_str[i], "");
        
        TT_UT_EQUAL(buf, end, "");
        TT_UT_EQUAL(len, 0, ""); // should pass end_flag
    }

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_bufp_byteseq_tok)
{
    //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    const tt_char_t *c1[] = {
        "111",
        "111xx111xx1111",
        "1111111",
        "xxx",
        "xx11111",
        "",
    };
    tt_u32_t n1[sizeof(c1)/sizeof(void*)] = {2,5,4,1,3,1};
    tt_u32_t n1_inv_bs[sizeof(c1)/sizeof(void*)] = {
        1, // second token is "\0", valid
        2, // final token is "\0"
        3,0,1,0};
    tt_u32_t n1_inv_str[sizeof(c1)/sizeof(void*)] = {
        1, // second token is invalid
        3,
        3,0,1,
        1 // only an ending flag
    };
    
    tt_u32_t i;
    
    TT_TEST_CASE_ENTER()
    // test start
    
    // byte sequence
    for (i = 0; i < sizeof(c1)/sizeof(void*); ++i)
    {
        tt_u8_t *buf = (tt_u8_t*)c1[i];
        tt_u32_t len = (tt_u32_t)strlen(c1[i])+1;
        tt_u8_t *end = TT_PTR_INC(tt_u8_t, buf, len);
        tt_result_t ret;
        
        __btp_inval_tnum = __btp_tnum = 0;
        ret = tt_bufp_byteseq_tok(&buf, &len, (const tt_u8_t*)"11", 2,
                                  NULL, 0,
                                  __btb_counter, &__btp_tnum);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(__btp_tnum, n1[i], "");
        TT_UT_EQUAL(__btp_inval_tnum, n1_inv_bs[i], "");
        
        TT_UT_EQUAL(buf, end, "");
        TT_UT_EQUAL(len, 0, "");
    }
    
    // string like, ending null before len
    for (i = 0; i < sizeof(c1)/sizeof(void*); ++i)
    {
        tt_u8_t *buf = (tt_u8_t*)c1[i];
        tt_u32_t len = (tt_u32_t)strlen(c1[i]) + 1;
        tt_u8_t *end = TT_PTR_INC(tt_u8_t, buf, len);
        tt_result_t ret;
        
        __btp_inval_tnum = __btp_tnum = 0;
        ret = tt_bufp_byteseq_tok(&buf, &len, (const tt_u8_t*)"11", 2,
                                     (const tt_u8_t*)"", 1,
                                     __btb_counter, &__btp_tnum);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(__btp_tnum, n1[i], "");
        TT_UT_EQUAL(__btp_inval_tnum, n1_inv_str[i], "");
        
        TT_UT_EQUAL(buf, end, "");
        TT_UT_EQUAL(len, 0, ""); // should pass end_flag
    }
    
    // test end
    TT_TEST_CASE_LEAVE()
}
#endif

TT_TEST_ROUTINE_DEFINE(case_buf_remove)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_u8_t data[5] = {0, 1, 2, 3, 4};
    tt_buf_t buf;
    tt_result_t ret;

    TT_TEST_CASE_ENTER()
    // test start

    // remove

    ret = tt_buf_create(&buf, sizeof(data), NULL);
    TT_UT_SUCCESS(ret, "");

    ret = tt_buf_put(&buf, data, sizeof(data));
    TT_UT_SUCCESS(ret, "");

    // out of range
    tt_buf_remove(&buf, 5);

    tt_buf_remove(&buf, 4);
    TT_UT_EQUAL(TT_BUF_RLEN(&buf), 4, "");
    TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(&buf), data, 4), 0, "");
    // => [0,1,2,3]

    tt_buf_remove(&buf, 0);
    TT_UT_EQUAL(TT_BUF_RLEN(&buf), 3, "");
    TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(&buf), &data[1], 3), 0, "");
    // => [1,2,3]

    {
        tt_u8_t aa[2] = {1, 3};
        tt_buf_remove(&buf, 1);
        TT_UT_EQUAL(TT_BUF_RLEN(&buf), 2, "");
        TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(&buf), aa, 2), 0, "");
        // => [1,2,3]
    }

    tt_buf_destroy(&buf);

    // remove n

    ret = tt_buf_create(&buf, sizeof(data), NULL);
    TT_UT_SUCCESS(ret, "");

    ret = tt_buf_put(&buf, data, sizeof(data));
    TT_UT_SUCCESS(ret, "");

    tt_buf_remove_range(&buf, 3, 5);
    TT_UT_EQUAL(TT_BUF_RLEN(&buf), 3, "");
    TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(&buf), data, 3), 0, "");
    // => [0,1,2]

    tt_buf_remove_range(&buf, 0, 2);
    TT_UT_EQUAL(TT_BUF_RLEN(&buf), 1, "");
    TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(&buf), &data[2], 1), 0, "");
    // => [2]

    tt_buf_destroy(&buf);

    ret = tt_buf_create(&buf, sizeof(data), NULL);
    TT_UT_SUCCESS(ret, "");
    ret = tt_buf_put(&buf, data, sizeof(data));
    TT_UT_SUCCESS(ret, "");

    {
        tt_u8_t aa[2] = {0, 4};
        tt_buf_remove_range(&buf, 1, 4);
        TT_UT_EQUAL(TT_BUF_RLEN(&buf), 2, "");
        TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(&buf), aa, 2), 0, "");
    }

    tt_buf_destroy(&buf);

    // invalid pos
    ret = tt_buf_create(&buf, sizeof(data), NULL);
    TT_UT_SUCCESS(ret, "");

    ret = tt_buf_put(&buf, data, sizeof(data));
    TT_UT_SUCCESS(ret, "");
    // => [0,1,2,3,4]

    tt_buf_remove_range(&buf, 0, 100);
    TT_UT_EQUAL(TT_BUF_RLEN(&buf), 0, "");
    tt_buf_destroy(&buf);

    // invalid pos2
    ret = tt_buf_create(&buf, sizeof(data), NULL);
    TT_UT_SUCCESS(ret, "");

    ret = tt_buf_put(&buf, data, sizeof(data));
    TT_UT_SUCCESS(ret, "");
    // => [0,1,2,3,4]

    tt_buf_remove_range(&buf, 2, 100);
    TT_UT_EQUAL(TT_BUF_RLEN(&buf), 2, "");
    TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(&buf), data, 2), 0, "");
    tt_buf_destroy(&buf);

    // insert
    ret = tt_buf_create(&buf, sizeof(data), NULL);
    TT_UT_SUCCESS(ret, "");
    ret = tt_buf_put(&buf, data, sizeof(data));
    TT_UT_SUCCESS(ret, "");
    // [0,1,2,3,4]

    ret = tt_buf_insert(&buf, 0, data, sizeof(data));
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(TT_BUF_RLEN(&buf), 10, "");
    TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(&buf), data, 5), 0, "");
    TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(&buf) + 5, data, 5), 0, "");
    // [0,1,2,3,4,0,1,2,3,4]

    ret = tt_buf_insert(&buf, 10, data, sizeof(data));
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(TT_BUF_RLEN(&buf), 15, "");
    TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(&buf), data, 5), 0, "");
    TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(&buf) + 5, data, 5), 0, "");
    TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(&buf) + 10, data, 5), 0, "");
    // [0,1,2,3,4,0,1,2,3,4,0,1,2,3,4]

    {
        tt_u8_t aa[] = {0, 1, 0, 1, 2, 3, 4, 2, 3, 4,
                        0, 1, 2, 3, 4, 0, 1, 2, 3, 4};
        ret = tt_buf_insert(&buf, 2, data, sizeof(data));
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(TT_BUF_RLEN(&buf), sizeof(aa), "");
        TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(&buf), aa, sizeof(aa)), 0, "");
    }

    tt_buf_destroy(&buf);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_buf_get_hexstr)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_buf_t buf, buf2;
    tt_result_t ret;
    tt_char_t c_buf[100];
    tt_u32_t clen;
    tt_u8_t h[] = {0, 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xcd, 0xef};
    const tt_char_t *cstr = "00123456789abccdef";

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&buf, NULL);
    tt_buf_init(&buf2, NULL);

    clen = tt_buf_get_hexstr(&buf, NULL, 0);
    TT_UT_EQUAL(clen, 0, "");

    ret = tt_buf_put(&buf, h, sizeof(h));
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    // cstr2hex
    ret = tt_buf_put_cstr2hex(&buf2, cstr);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(tt_buf_cmp(&buf, &buf2), 0, "");

    tt_buf_reset_rwp(&buf2);
    ret = tt_buf_put_hex2cstr(&buf2, h, sizeof(h));
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(TT_BUF_RLEN(&buf2), (tt_u32_t)tt_strlen(cstr), "");
    TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(&buf2), cstr, (tt_u32_t)tt_strlen(cstr)),
                0,
                "");

    clen = tt_buf_get_hexstr(&buf, c_buf, 0);
    TT_UT_EQUAL(clen, sizeof(h) * 2, "");

    // get all
    clen = tt_buf_get_hexstr(&buf, c_buf, sizeof(c_buf));
    TT_UT_EQUAL(clen, sizeof(h) * 2, "");
    TT_UT_EQUAL(tt_memcmp(c_buf, cstr, sizeof(cstr) - 1), 0, "");

    // get partial
    clen = tt_buf_get_hexstr(&buf, c_buf, sizeof(h));
    TT_UT_EQUAL(clen, sizeof(h), "");
    TT_UT_EQUAL(tt_memcmp(c_buf, cstr, sizeof(h)), 0, "");

    tt_buf_print_hexstr(&buf, 0);
    tt_buf_destroy(&buf);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_buftok)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_buf_t buf;
    tt_result_t ret;
    tt_u8_t *tok;
    tt_u32_t tok_len;
    tt_u32_t i;

    TT_TEST_CASE_ENTER()
    // test start

    // normal string
    ret = tt_buf_create_copy(&buf, (tt_u8_t *)";1.2.3?", 7, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tok = NULL;
    tok_len = 0;
    i = 0;
    while (tt_buf_tok(&buf, (tt_u8_t *)";?.", 3, 0, &tok, &tok_len) != TT_END) {
        if (i == 0) {
            TT_UT_EQUAL(tok_len, 0, "");
        } else if (i == 1) {
            TT_UT_EQUAL(tok_len, 1, "");
            TT_UT_EQUAL(*tok, '1', "");
        } else if (i == 2) {
            TT_UT_EQUAL(tok_len, 1, "");
            TT_UT_EQUAL(*tok, '2', "");
        } else if (i == 3) {
            TT_UT_EQUAL(tok_len, 1, "");
            TT_UT_EQUAL(*tok, '3', "");
        } else {
            TT_UT_EQUAL(tok_len, 0, "");
        }

        ++i;
    }
    TT_UT_EQUAL(i, 5, "");

    // empty
    tok = NULL;
    tok_len = 0;
    i = 0;
    tt_buf_reset_rwp(&buf);
    while (tt_buf_tok(&buf, (tt_u8_t *)";?.", 3, 0, &tok, &tok_len) != TT_END) {
        TT_UT_EQUAL(tok_len, 0, "");

        ++i;
    }
    TT_UT_EQUAL(i, 1, "");

    // 1 seperator
    tok = NULL;
    tok_len = 0;
    i = 0;
    tt_buf_reset_rwp(&buf);
    tt_buf_put_u8(&buf, '.');
    while (tt_buf_tok(&buf, (tt_u8_t *)";?.", 3, 0, &tok, &tok_len) != TT_END) {
        TT_UT_EQUAL(tok_len, 0, "");

        ++i;
    }
    TT_UT_EQUAL(i, 2, "");

    // 2 seperator
    tok = NULL;
    tok_len = 0;
    i = 0;
    tt_buf_reset_rwp(&buf);
    tt_buf_put_u8(&buf, '?');
    tt_buf_put_u8(&buf, ';');
    while (tt_buf_tok(&buf, (tt_u8_t *)";?.", 3, 0, &tok, &tok_len) != TT_END) {
        TT_UT_EQUAL(tok_len, 0, "");

        ++i;
    }
    TT_UT_EQUAL(i, 3, "");

    tt_buf_destroy(&buf);

    // only sep string
    ret = tt_buf_create_copy(&buf, (tt_u8_t *)";..;??", 6, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tok = NULL;
    tok_len = 0;
    i = 0;
    while (tt_buf_tok(&buf, (tt_u8_t *)";?.", 3, 0, &tok, &tok_len) != TT_END) {
        TT_UT_EQUAL(tok_len, 0, "");

        ++i;
    }
    TT_UT_EQUAL(i, 7, "");

    tt_buf_destroy(&buf);

    // ignore empty token
    ret = tt_buf_create_nocopy(&buf, (tt_u8_t *)";..;??", 6, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tok = NULL;
    tok_len = 0;
    i = 0;
    while (tt_buf_tok(&buf,
                      (tt_u8_t *)";?.",
                      3,
                      TT_BUFTOK_NOEMPTY,
                      &tok,
                      &tok_len) != TT_END) {
        TT_UT_EQUAL(tok_len, 0, "");

        ++i;
    }
    TT_UT_EQUAL(i, 0, "");

    // empty
    tok = NULL;
    tok_len = 0;
    i = 0;
    tt_buf_reset_rwp(&buf);
    while (tt_buf_tok(&buf,
                      (tt_u8_t *)";?.",
                      3,
                      TT_BUFTOK_NOEMPTY,
                      &tok,
                      &tok_len) != TT_END) {
        ++i;
    }
    TT_UT_EQUAL(i, 0, "");

    // 1 seperator
    tok = NULL;
    tok_len = 0;
    i = 0;
    tt_buf_reset_rwp(&buf);
    tt_buf_put_u8(&buf, '.');
    while (tt_buf_tok(&buf,
                      (tt_u8_t *)";?.",
                      3,
                      TT_BUFTOK_NOEMPTY,
                      &tok,
                      &tok_len) != TT_END) {
        ++i;
    }
    TT_UT_EQUAL(i, 0, "");

    // 2 seperator
    tok = NULL;
    tok_len = 0;
    i = 0;
    tt_buf_reset_rwp(&buf);
    tt_buf_put_u8(&buf, '?');
    tt_buf_put_u8(&buf, ';');
    while (tt_buf_tok(&buf,
                      (tt_u8_t *)";?.",
                      3,
                      TT_BUFTOK_NOEMPTY,
                      &tok,
                      &tok_len) != TT_END) {
        ++i;
    }
    TT_UT_EQUAL(i, 0, "");

    tt_buf_destroy(&buf);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_blob)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_blob_t b, b2 = {0};
    tt_result_t ret;
    char c[] = "123";

    TT_TEST_CASE_ENTER()
    // test start

    tt_blob_init(&b);
    tt_blob_destroy(&b);

    ret = tt_blob_create(&b, NULL, (tt_u32_t)tt_strlen(c));
    TT_UT_SUCCESS(ret, "");

    tt_memcpy(b.addr, c, tt_strlen(c));
    TT_UT_EQUAL(tt_strncmp((tt_char_t *)b.addr, c, 3), 0, "");
    TT_UT_EQUAL(tt_blob_strcmp(&b, c), 0, "");
    TT_UT_EXP(tt_blob_strcmp(&b, "") > 0, "");
    TT_UT_EXP(tt_blob_strcmp(&b, "1234") < 0, "");

    ret = tt_blob_create(&b2, (tt_u8_t *)"", 0);
    TT_UT_EXP(tt_blob_cmp(&b, &b2) > 0, "");
    tt_blob_destroy(&b2);

    ret = tt_blob_create(&b2, (tt_u8_t *)"1234", 4);
    TT_UT_EXP(tt_blob_cmp(&b, &b2) < 0, "");
    tt_blob_destroy(&b2);

    ret = tt_blob_create(&b2, (tt_u8_t *)"123", 3);
    TT_UT_EXP(tt_blob_cmp(&b, &b2) == 0, "");
    tt_blob_destroy(&b2);

    tt_blob_destroy(&b);

    // test end
    TT_TEST_CASE_LEAVE()
}
