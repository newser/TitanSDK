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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_sshmsg_byte)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_sshmsg_boolean)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_sshmsg_u32)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_sshmsg_u64)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_sshmsg_string)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_sshmsg_mpint)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_sshmsg_namelist)

TT_TEST_ROUTINE_DECLARE(tt_unit_test_sshmsg_parse)

TT_TEST_ROUTINE_DECLARE(tt_unit_test_sshmsg_disconnect)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_sshmsg_ignore)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_sshms_newkeys)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_sshmsg_servreq)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_sshmsg_servacc)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(sshmsg_case)

TT_TEST_CASE("tt_unit_test_sshmsg_byte",
             "ssh msg parse byte",
             tt_unit_test_sshmsg_byte,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_sshmsg_boolean",
                 "ssh msg parse boolean",
                 tt_unit_test_sshmsg_boolean,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_sshmsg_u32",
                 "ssh msg parse uint32",
                 tt_unit_test_sshmsg_u32,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_sshmsg_u64",
                 "ssh msg parse uint64",
                 tt_unit_test_sshmsg_u64,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_sshmsg_string",
                 "ssh msg parse string",
                 tt_unit_test_sshmsg_string,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_sshmsg_mpint",
                 "ssh msg parse mpint",
                 tt_unit_test_sshmsg_mpint,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_sshmsg_namelist",
                 "ssh msg parse namelist",
                 tt_unit_test_sshmsg_namelist,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_sshmsg_parse",
                 "ssh msg parse",
                 tt_unit_test_sshmsg_parse,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_sshmsg_disconnect",
                 "ssh msg: disconnect",
                 tt_unit_test_sshmsg_disconnect,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_sshmsg_ignore",
                 "ssh msg: ignore",
                 tt_unit_test_sshmsg_ignore,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_sshms_newkeys",
                 "ssh msg: newkeys",
                 tt_unit_test_sshms_newkeys,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_sshmsg_servreq",
                 "ssh msg: service request",
                 tt_unit_test_sshmsg_servreq,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_sshmsg_servacc",
                 "ssh msg: service accept",
                 tt_unit_test_sshmsg_servacc,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_sshmsg_servacc",
                 "ssh msg: service accept",
                 tt_unit_test_sshmsg_servacc,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(sshmsg_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(SSH_MSG, 0, sshmsg_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(tt_unit_test_sshmsg_usrauthreq_pkey)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(tt_unit_test_sshmsg_byte)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_buf_t buf;
    tt_result_t ret;
    tt_u32_t n;
    tt_u8_t byte;

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&buf, NULL);

    n = tt_ssh_byte_render_prepare();
    TT_TEST_CHECK_EQUAL(n, 1, "");

    ret = tt_ssh_byte_render(&buf, 0x12);
    TT_TEST_CHECK_SUCCESS(ret, "");

    // check rendered data
    ret = tt_buf_get_u8(&buf, &byte);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_EQUAL(byte, 0x12, "");

    // parse
    ret = tt_ssh_byte_render(&buf, 0x12);
    TT_TEST_CHECK_SUCCESS(ret, "");
    ret = tt_ssh_byte_parse(&buf, &byte);
    TT_TEST_CHECK_EQUAL(byte, 0x12, "");
    TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&buf), 0, "");

    tt_buf_destroy(&buf);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_sshmsg_boolean)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_buf_t buf;
    tt_result_t ret;
    tt_u32_t n;
    tt_bool_t byte;

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&buf, NULL);

    n = tt_ssh_boolean_render_prepare();
    TT_TEST_CHECK_EQUAL(n, 1, "");

    ret = tt_ssh_boolean_render(&buf, TT_TRUE);
    TT_TEST_CHECK_SUCCESS(ret, "");
    ret = tt_buf_get_u8(&buf, &byte);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_EQUAL(byte, TT_TRUE, "");

    ret = tt_ssh_boolean_render(&buf, TT_FALSE);
    TT_TEST_CHECK_SUCCESS(ret, "");
    ret = tt_buf_get_u8(&buf, &byte);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_EQUAL(byte, TT_FALSE, "");

    ret = tt_ssh_boolean_render(&buf, 0x12);
    TT_TEST_CHECK_SUCCESS(ret, "");
    ret = tt_buf_get_u8(&buf, &byte);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_EQUAL(byte, TT_TRUE, "");

    // parse
    ret = tt_ssh_boolean_render(&buf, TT_TRUE);
    TT_TEST_CHECK_SUCCESS(ret, "");
    ret = tt_ssh_boolean_parse(&buf, &byte);
    TT_TEST_CHECK_EQUAL(byte, TT_TRUE, "");
    TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&buf), 0, "");

    ret = tt_ssh_boolean_render(&buf, TT_FALSE);
    TT_TEST_CHECK_SUCCESS(ret, "");
    ret = tt_ssh_boolean_parse(&buf, &byte);
    TT_TEST_CHECK_EQUAL(byte, TT_FALSE, "");
    TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&buf), 0, "");

    ret = tt_buf_put_u8(&buf, 0x23);
    TT_TEST_CHECK_SUCCESS(ret, "");
    ret = tt_ssh_boolean_parse(&buf, &byte);
    TT_TEST_CHECK_EQUAL(byte, TT_TRUE, "");
    TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&buf), 0, "");

    tt_buf_destroy(&buf);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_sshmsg_u32)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_buf_t buf;
    tt_result_t ret;
    tt_u32_t n;
    tt_u32_t val;
    tt_u32_t i;

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&buf, NULL);

    n = tt_ssh_uint32_render_prepare();
    TT_TEST_CHECK_EQUAL(n, 4, "");

    ret = tt_ssh_uint32_render(&buf, 0x12345678);
    TT_TEST_CHECK_SUCCESS(ret, "");

    // check rendered data, which is stored in network order
    ret = tt_buf_get_u32(&buf, &val);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_EQUAL(val, tt_hton32(0x12345678), "");

    // parse
    ret = tt_buf_put_u32_n(&buf, 0x12345678);
    TT_TEST_CHECK_SUCCESS(ret, "");
    ret = tt_ssh_uint32_parse(&buf, &val);
    TT_TEST_CHECK_EQUAL(val, 0x12345678, "");
    TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&buf), 0, "");

    for (i = 0; i < 3; ++i) {
        ret = tt_buf_put_u8(&buf, i + 1);
        TT_TEST_CHECK_SUCCESS(ret, "");

        ret = tt_ssh_uint32_parse(&buf, &val);
        TT_TEST_CHECK_EQUAL(ret, TT_BUFFER_INCOMPLETE, "");
        TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&buf), i + 1, "");
    }
    ret = tt_buf_put_u8(&buf, i + 1);
    TT_TEST_CHECK_SUCCESS(ret, "");
    ret = tt_ssh_uint32_parse(&buf, &val);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(val, 0x01020304, "");
    TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&buf), 0, "");

    tt_buf_destroy(&buf);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_sshmsg_u64)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_buf_t buf;
    tt_result_t ret;
    tt_u32_t n, i;
    tt_u64_t val;

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&buf, NULL);

    n = tt_ssh_uint32_render_prepare();
    TT_TEST_CHECK_EQUAL(n, 4, "");

    ret = tt_ssh_uint64_render(&buf, 0x12345678aabbccdd);
    TT_TEST_CHECK_SUCCESS(ret, "");

    // check rendered data, which is stored in network order
    ret = tt_buf_get_u64(&buf, &val);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_EQUAL(val, tt_hton64(0x12345678aabbccdd), "");

    // parse
    ret = tt_buf_put_u64_n(&buf, 0x12345678aabbccdd);
    TT_TEST_CHECK_SUCCESS(ret, "");
    ret = tt_ssh_uint64_parse(&buf, &val);
    TT_TEST_CHECK_EQUAL(val, 0x12345678aabbccdd, "");
    TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&buf), 0, "");

    for (i = 0; i < 7; ++i) {
        ret = tt_buf_put_u8(&buf, i + 1);
        TT_TEST_CHECK_SUCCESS(ret, "");

        ret = tt_ssh_uint64_parse(&buf, &val);
        TT_TEST_CHECK_EQUAL(ret, TT_BUFFER_INCOMPLETE, "");
        TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&buf), i + 1, "");
    }
    ret = tt_buf_put_u8(&buf, i + 1);
    TT_TEST_CHECK_SUCCESS(ret, "");
    ret = tt_ssh_uint64_parse(&buf, &val);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(val, 0x0102030405060708, "");
    TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&buf), 0, "");

    tt_buf_destroy(&buf);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_sshmsg_string)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_buf_t buf;
    tt_result_t ret;
    tt_u32_t n;
    tt_u32_t len;
    tt_u8_t cbuf[100];
    tt_u8_t *s;

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&buf, NULL);

    n = tt_ssh_string_render_prepare(NULL, 0);
    TT_TEST_CHECK_EQUAL(n, 4, "");

    n = tt_ssh_string_render_prepare(NULL, 3);
    TT_TEST_CHECK_EQUAL(n, 7, "");

    n = tt_ssh_string_render_prepare(NULL, 0x12345780);
    TT_TEST_CHECK_EQUAL(n, 0x12345784, "");

    // 1. null string
    ret = tt_ssh_string_render(&buf, NULL, 0);
    TT_TEST_CHECK_SUCCESS(ret, "");
    // check rendered data
    ret = tt_buf_get_u32_h(&buf, &len);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_EQUAL(len, 0, "");

    // 2. empty string
    ret = tt_ssh_string_render(&buf, (tt_u8_t *)"", 0);
    TT_TEST_CHECK_SUCCESS(ret, "");
    // check rendered data
    ret = tt_buf_get_u32_h(&buf, &len);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_EQUAL(len, 0, "");

    // 3. normal string string
    ret = tt_ssh_string_render(&buf, (tt_u8_t *)"12345", 5);
    TT_TEST_CHECK_SUCCESS(ret, "");
    // check rendered data
    ret = tt_buf_get_u32_h(&buf, &len);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_EQUAL(len, 5, "");
    ret = tt_buf_get(&buf, cbuf, 5);
    TT_TEST_CHECK_EQUAL(tt_memcmp(cbuf, "12345", 5), 0, "");

    // parse

    // 1. no string
    tt_buf_reset_rwp(&buf);
    ret = tt_ssh_string_parse(&buf, &s, &len);
    TT_TEST_CHECK_EQUAL(ret, TT_BUFFER_INCOMPLETE, "");

    // 2. incomplete string
    tt_buf_put_u32_n(&buf, 10);
    ret = tt_ssh_string_parse(&buf, &s, &len);
    TT_TEST_CHECK_EQUAL(ret, TT_BUFFER_INCOMPLETE, "");

    // 3. normal string
    tt_buf_reset_rwp(&buf);
    tt_buf_put_u32_n(&buf, 10);
    tt_buf_put(&buf, (tt_u8_t *)"123456789a", 10);
    ret = tt_ssh_string_parse(&buf, &s, &len);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(len, 10, "");
    TT_TEST_CHECK_EQUAL(tt_memcmp(s, (tt_u8_t *)"123456789a", 10), 0, "");

    // 4. malicious string, too large size
    tt_buf_reset_rwp(&buf);
    tt_buf_put_u32_n(&buf, ~0);
    tt_buf_put(&buf, (tt_u8_t *)"123456789a", 10);
    ret = tt_ssh_string_parse(&buf, &s, &len);
    TT_TEST_CHECK_EQUAL(ret, TT_BUFFER_INCOMPLETE, "");

    tt_buf_destroy(&buf);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_sshmsg_mpint)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_buf_t buf;
    tt_result_t ret;
    tt_u32_t n;
    tt_u32_t len;
    tt_u8_t cbuf[100];
    tt_u8_t *s, byte;

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&buf, NULL);

    // 0
    n = tt_ssh_mpint_render_prepare(NULL, 0, TT_TRUE);
    TT_TEST_CHECK_EQUAL(n, 4, "");
    n = tt_ssh_mpint_render_prepare(NULL, 0, TT_FALSE);
    TT_TEST_CHECK_EQUAL(n, 4, "");

    // negative
    cbuf[0] = 0xFF;
    n = tt_ssh_mpint_render_prepare(cbuf, 1, TT_TRUE);
    TT_TEST_CHECK_EQUAL(n, 5, "");

    // positive
    n = tt_ssh_mpint_render_prepare(cbuf, 1, TT_FALSE);
    TT_TEST_CHECK_EQUAL(n, 6, "");

    cbuf[0] = 0xFF;
    cbuf[1] = 0xEE;
    cbuf[2] = 0xDD;
    cbuf[3] = 0xCC;
    cbuf[4] = 0x11;
    cbuf[5] = 0x33;
    cbuf[6] = 0x55;
    cbuf[7] = 0x77;
    cbuf[8] = 0x22;
    cbuf[9] = 0x44;
    cbuf[10] = 0x66;
    cbuf[11] = 0x88;

    // 1. 0
    ret = tt_ssh_mpint_render(&buf, cbuf, 0, TT_TRUE);
    TT_TEST_CHECK_SUCCESS(ret, "");
    ret = tt_buf_get_u32_h(&buf, &len);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_EQUAL(len, 0, "");

    // 2. -0xFF
    ret = tt_ssh_mpint_render(&buf, cbuf, 1, TT_TRUE);
    TT_TEST_CHECK_SUCCESS(ret, "");
    ret = tt_buf_get_u32_h(&buf, &len);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_EQUAL(len, 1, "");
    ret = tt_buf_get_u8(&buf, &byte);
    TT_TEST_CHECK_EQUAL(byte, 0xFF, "");

    // 3. 0xFF
    ret = tt_ssh_mpint_render(&buf, cbuf, 1, TT_FALSE);
    TT_TEST_CHECK_SUCCESS(ret, "");
    ret = tt_buf_get_u32_h(&buf, &len);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_EQUAL(len, 2, "");
    ret = tt_buf_get_u8(&buf, &byte);
    TT_TEST_CHECK_EQUAL(byte, 0, "");
    ret = tt_buf_get_u8(&buf, &byte);
    TT_TEST_CHECK_EQUAL(byte, 0xFF, "");

    // 4. -long
    ret = tt_ssh_mpint_render(&buf, cbuf, 12, TT_TRUE);
    TT_TEST_CHECK_SUCCESS(ret, "");
    ret = tt_buf_get_u32_h(&buf, &len);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_EQUAL(len, 12, "");
    TT_TEST_CHECK_EQUAL(buf.addr[buf.rd_pos + 0], 0xFF, "");
    TT_TEST_CHECK_EQUAL(buf.addr[buf.rd_pos + 4], 0x11, "");
    TT_TEST_CHECK_EQUAL(buf.addr[buf.rd_pos + 11], 0x88, "");

    // 4. long
    tt_buf_reset_rwp(&buf);
    ret = tt_ssh_mpint_render(&buf, cbuf, 12, TT_FALSE);
    TT_TEST_CHECK_SUCCESS(ret, "");
    ret = tt_buf_get_u32_h(&buf, &len);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_EQUAL(len, 13, "");
    TT_TEST_CHECK_EQUAL(buf.addr[buf.rd_pos + 0], 0x0, "");
    TT_TEST_CHECK_EQUAL(buf.addr[buf.rd_pos + 5], 0x11, "");
    TT_TEST_CHECK_EQUAL(buf.addr[buf.rd_pos + 12], 0x88, "");
    TT_TEST_CHECK_EQUAL(buf.addr[buf.rd_pos + 1], 0xFF, "");
    TT_TEST_CHECK_EQUAL(buf.addr[buf.rd_pos + 5], 0x11, "");
    TT_TEST_CHECK_EQUAL(buf.addr[buf.rd_pos + 12], 0x88, "");

    // parse

    // 1. no mpint
    tt_buf_reset_rwp(&buf);
    ret = tt_ssh_mpint_parse(&buf, &s, &len);
    TT_TEST_CHECK_EQUAL(ret, TT_BUFFER_INCOMPLETE, "");

    // 2. incomplete mpint
    tt_buf_put_u32_n(&buf, 10);
    ret = tt_ssh_mpint_parse(&buf, &s, &len);
    TT_TEST_CHECK_EQUAL(ret, TT_BUFFER_INCOMPLETE, "");

    // 3. normal mpint
    tt_buf_reset_rwp(&buf);
    tt_buf_put_u32_n(&buf, 10);
    tt_buf_put(&buf, cbuf, 10);
    ret = tt_ssh_mpint_parse(&buf, &s, &len);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(len, 10, "");
    TT_TEST_CHECK_EQUAL(tt_memcmp(cbuf, s, 10), 0, "");

    // 4. malicious string, too large size
    tt_buf_reset_rwp(&buf);
    tt_buf_put_u32_n(&buf, ~0);
    tt_buf_put(&buf, cbuf, 10);
    ret = tt_ssh_mpint_parse(&buf, &s, &len);
    TT_TEST_CHECK_EQUAL(ret, TT_BUFFER_INCOMPLETE, "");

    tt_buf_destroy(&buf);

    // test end
    TT_TEST_CASE_LEAVE()
}

struct __nl_cb_param
{
    const tt_char_t **name;
    tt_u32_t idx;
};

tt_result_t __nl_cb(IN tt_char_t *name, IN tt_u32_t name_len, IN void *param)
{
    const tt_char_t *nm;

    if (param == NULL)
        return TT_FAIL;

    nm = ((struct __nl_cb_param *)param)
             ->name[((struct __nl_cb_param *)param)->idx];
    ((struct __nl_cb_param *)param)->idx += 1;

    if (name_len != (tt_u32_t)tt_strlen(nm)) {
        return TT_FAIL;
    }
    if (tt_strncmp(nm, name, name_len) != 0) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_sshmsg_namelist)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_buf_t buf;
    tt_result_t ret;
    tt_u32_t len;

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&buf, NULL);

    // 0 name
    len = tt_ssh_namelist_render_prepare(NULL, 0);
    TT_TEST_CHECK_EQUAL(len, 4, "");

    tt_buf_reset_rwp(&buf);
    ret = tt_ssh_namelist_render(&buf, NULL, 0);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_buf_get_u32_h(&buf, &len);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(len, 0, "");
    TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&buf), 0, "");

    // 1 name
    {
        const tt_char_t *nl[] = {"name1"};
        tt_u8_t *addr;

        len = tt_ssh_namelist_render_prepare(nl, 1);
        TT_TEST_CHECK_EQUAL(len, 9, "");

        tt_buf_reset_rwp(&buf);
        ret = tt_ssh_namelist_render(&buf, nl, 1);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        ret = tt_buf_get_u32_h(&buf, &len);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        TT_TEST_CHECK_EQUAL(len, 5, "");

        ret = tt_buf_getptr(&buf, &addr, 5);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        TT_TEST_CHECK_EQUAL(tt_memcmp(addr, nl[0], 5), 0, "");
        TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&buf), 0, "");
    }

    // 2 name
    {
        const tt_char_t *nl[] = {"name1", "another name"};
        tt_u8_t *addr;

        len = tt_ssh_namelist_render_prepare(nl, 2);
        TT_TEST_CHECK_EQUAL(len, 22, "");

        tt_buf_reset_rwp(&buf);
        ret = tt_ssh_namelist_render(&buf, nl, 2);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        ret = tt_buf_get_u32_h(&buf, &len);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        TT_TEST_CHECK_EQUAL(len, 18, "");

        ret = tt_buf_getptr(&buf, &addr, 18);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        TT_TEST_CHECK_EQUAL(tt_memcmp(addr, "name1,another name", 5), 0, "");
        TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&buf), 0, "");
    }

    ////////////////////////////////////////////
    // parse

    {
        // empty
        tt_buf_reset_rwp(&buf);
        tt_buf_put_u32(&buf, 0);

        ret = tt_ssh_namelist_parse(&buf, __nl_cb, NULL);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&buf), 0, "");
    }

    {
        // incomplete
        tt_buf_reset_rwp(&buf);
        tt_buf_put_u32(&buf, ~0);
        tt_buf_put(&buf, (tt_u8_t *)"12345", 5);

        ret = tt_ssh_namelist_parse(&buf, __nl_cb, NULL);
        TT_TEST_CHECK_EQUAL(ret, TT_BUFFER_INCOMPLETE, "");
    }

    {
        // 1 name
        const tt_char_t *nl[] = {"name1"};
        struct __nl_cb_param ncp = {0};

        tt_buf_reset_rwp(&buf);
        tt_buf_put_u32_n(&buf, 5);
        tt_buf_put(&buf, (tt_u8_t *)nl[0], 5);

        ncp.name = nl;
        ret = tt_ssh_namelist_parse(&buf, __nl_cb, &ncp);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&buf), 0, "");
    }

    {
        // 2 name
        const tt_char_t *nl[] = {"name1 ", " name2", " name3 "};
        struct __nl_cb_param ncp = {0};

        tt_buf_reset_rwp(&buf);
        tt_buf_put_u32_n(&buf, 21);
        tt_buf_put(&buf, (tt_u8_t *)nl[0], 6);
        tt_buf_put_u8(&buf, ',');
        tt_buf_put(&buf, (tt_u8_t *)nl[1], 6);
        tt_buf_put_u8(&buf, ',');
        tt_buf_put(&buf, (tt_u8_t *)nl[2], 7);

        ncp.name = nl;
        ret = tt_ssh_namelist_parse(&buf, __nl_cb, &ncp);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&buf), 0, "");
    }

    {
        // can not terminate with ','
        const tt_char_t *nl[] = {"name1"};
        struct __nl_cb_param ncp = {0};

        tt_buf_reset_rwp(&buf);
        tt_buf_put_u32_n(&buf, 6);
        tt_buf_put(&buf, (tt_u8_t *)nl[0], 5);
        tt_buf_put_u8(&buf, ',');

        ncp.name = nl;
        ret = tt_ssh_namelist_parse(&buf, __nl_cb, &ncp);
        TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");
    }

    {
        // "n1,,n2"
        const tt_char_t *nl[] = {"n1"};
        struct __nl_cb_param ncp = {0};

        tt_buf_reset_rwp(&buf);
        tt_buf_put_u32_n(&buf, 6);
        tt_buf_put(&buf, (tt_u8_t *)"n1,,n2", 6);

        ncp.name = nl;
        ret = tt_ssh_namelist_parse(&buf, __nl_cb, &ncp);
        TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");
    }

    {
        // can not terminate with ','
        const tt_char_t *nl[] = {"name1"};
        struct __nl_cb_param ncp = {0};

        tt_buf_reset_rwp(&buf);
        tt_buf_put_u32_n(&buf, 1);
        tt_buf_put_u8(&buf, ',');

        ncp.name = nl;
        ret = tt_ssh_namelist_parse(&buf, __nl_cb, &ncp);
        TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");
    }

    tt_buf_destroy(&buf);

    // test end
    TT_TEST_CASE_LEAVE()
}

struct __sshmsg_parse_vec_t
{
    tt_u8_t *data;
    tt_u32_t data_len;
    tt_result_t expect_ret;
    tt_u32_t left_len;
};

static tt_u8_t __incomp_1[] = {0x0};
static tt_u8_t __incomp_2[] = {0x0, 0x0, 0x0, 0x0};
static tt_u8_t __incomp_3[] = {0x0, 0x0, 0x0, 0x1};
static tt_u8_t __incomp_4[] = {0x0, 0x0, 0x0, 0x2, 0x2, 0x0};
static tt_u8_t __incomp_5[] = {0x0, 0x0, 0x0, 0x2, 0x1, 0x1};
static tt_u8_t __incomp_6[] = {0x0, 0x0, 0x0, 0x2, 0x3, 0x0};

// new keys
static tt_u8_t __incomp_7[] = {
    0x0, 0x0, 0x0, 0x4, 0x2, 21, 0x0, 0x0, 0x0,
};

static struct __sshmsg_parse_vec_t __sshmsg_parse_vec[] = {
    {__incomp_1, sizeof(__incomp_1), TT_BUFFER_INCOMPLETE, sizeof(__incomp_1)},
    {__incomp_2, sizeof(__incomp_2), TT_FAIL, sizeof(__incomp_2)},
    {__incomp_3, sizeof(__incomp_3), TT_BUFFER_INCOMPLETE, sizeof(__incomp_3)},
    {__incomp_4, sizeof(__incomp_4), TT_FAIL, sizeof(__incomp_4)},
    {__incomp_5, sizeof(__incomp_5), TT_FAIL, sizeof(__incomp_5)},
    {__incomp_6, sizeof(__incomp_6), TT_FAIL, sizeof(__incomp_7)},
    {__incomp_7, sizeof(__incomp_7), TT_SUCCESS, 1},
};

TT_TEST_ROUTINE_DEFINE(tt_unit_test_sshmsg_parse)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_buf_t buf;
    struct __sshmsg_parse_vec_t *v;
    tt_u32_t i;
    tt_result_t ret;
    tt_sshmsg_t *msg;

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&buf, NULL);

    for (i = 0; i < sizeof(__sshmsg_parse_vec) / sizeof(__sshmsg_parse_vec[0]);
         ++i) {
        v = &__sshmsg_parse_vec[i];

        tt_buf_reset_rwp(&buf);
        ret = tt_buf_put(&buf, v->data, v->data_len);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        ret = tt_sshmsg_parse(&buf, &msg);
        TT_TEST_CHECK_EQUAL(ret, v->expect_ret, "");

        // restore buf pos is done in __svrconn_handle_recv not in
        // tt_sshmsg_parse
        if (TT_OK(ret)) {
            TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&buf), v->left_len, "");
            tt_sshmsg_release(msg);
        }
    }

    for (i = 0; i < 100; ++i) {
        tt_buf_reset_rwp(&buf);
        tt_buf_put_rand(&buf, tt_rand_u32() % 1000 + 1);

        ret = tt_sshmsg_parse(&buf, &msg);
        if (TT_OK(ret)) {
            tt_sshmsg_release(msg);
        }
    }

    tt_buf_destroy(&buf);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_sshmsg_disconnect)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    tt_sshmsg_t *msg, *out_msg;
    tt_sshmsg_disconnect_t *msg_dis;
    tt_u32_t i;
    tt_u32_t rp, wp;

    TT_TEST_CASE_ENTER()
    // test start

    ////////////////////////////////////////////
    // 1.
    ////////////////////////////////////////////

    msg = tt_sshmsg_disconnect_create();
    TT_TEST_CHECK_NOT_EQUAL(msg, NULL, "");

    tt_sshmsg_disconnect_set_reason(msg, 0x123);
    tt_sshmsg_disconnect_set_reason(msg, 0x234);

    tt_sshmsg_disconnect_set_desc(msg, "1");
    // would be truncated
    tt_sshmsg_disconnect_set_desc(msg,
                                  "0123456789abcdef0123456789abcdef0123456789ab"
                                  "cdef0123456789abcdef0123456789abcdef01234567"
                                  "89abcdef");

    tt_sshmsg_dump(msg);

    ret = tt_sshmsg_render(msg, 0, NULL);
    TT_TEST_CHECK_SUCCESS(ret, "");

    tt_buf_backup_rwp(&msg->buf, &rp, &wp);
    ret = tt_sshmsg_parse(&msg->buf, &out_msg);
    TT_TEST_CHECK_SUCCESS(ret, "");
    tt_buf_restore_rwp(&msg->buf, &rp, &wp);

    TT_TEST_CHECK_EQUAL(out_msg->msg_id, TT_SSH_MSGID_DISCONNECT, "");
    msg_dis = TT_SSHMSG_CAST(out_msg, tt_sshmsg_disconnect_t);

    TT_TEST_CHECK_EQUAL(msg_dis->reason_code, 0x234, "");
    TT_TEST_CHECK_EQUAL(tt_strlen(msg_dis->description),
                        TT_SSHMSG_DISC_DESC_LEN,
                        "");
    TT_TEST_CHECK_EQUAL(tt_strncmp(msg_dis->description,
                                   "0123456789abcdef0123456789abcdef0123456789a"
                                   "bcdef0123456789abcdef0123456789abcdef012345"
                                   "6789abcdef",
                                   TT_SSHMSG_DISC_DESC_LEN),
                        0,
                        "");

    tt_sshmsg_release(out_msg);

    i = 0;
    while (i < TT_BUF_RLEN(&msg->buf)) {
        tt_u8_t bak = msg->buf.addr[msg->buf.rd_pos + i];

        msg->buf.addr[msg->buf.rd_pos + i] = (tt_u8_t)tt_rand_u32();
        tt_buf_backup_rwp(&msg->buf, &rp, &wp);
        ret = tt_sshmsg_parse(&msg->buf, &out_msg);
        tt_buf_restore_rwp(&msg->buf, &rp, &wp);
        if (TT_OK(ret)) {
            tt_sshmsg_release(out_msg);
        }
        msg->buf.addr[msg->buf.rd_pos + i] = bak;

        i += 1;
    }

    tt_sshmsg_release(msg);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_sshmsg_ignore)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    tt_sshmsg_t *msg, *out_msg;
    tt_sshmsg_ignore_t *msg_ign;
    tt_u32_t i;
    tt_u32_t rp, wp;

    TT_TEST_CASE_ENTER()
    // test start

    ////////////////////////////////////////////
    // 1.
    ////////////////////////////////////////////

    msg = tt_sshmsg_ignore_create();
    TT_TEST_CHECK_NOT_EQUAL(msg, NULL, "");

    msg_ign = TT_SSHMSG_CAST(msg, tt_sshmsg_ignore_t);
    TT_TEST_CHECK_EQUAL(msg_ign->data.addr, NULL, "");
    TT_TEST_CHECK_EQUAL(msg_ign->data.len, 0, "");

    tt_sshmsg_dump(msg);

    ret = tt_sshmsg_render(msg, 0, NULL);
    TT_TEST_CHECK_SUCCESS(ret, "");

    tt_buf_backup_rwp(&msg->buf, &rp, &wp);
    ret = tt_sshmsg_parse(&msg->buf, &out_msg);
    TT_TEST_CHECK_SUCCESS(ret, "");
    tt_buf_restore_rwp(&msg->buf, &rp, &wp);

    TT_TEST_CHECK_EQUAL(out_msg->msg_id, TT_SSH_MSGID_IGNORE, "");
    msg_ign = TT_SSHMSG_CAST(out_msg, tt_sshmsg_ignore_t);

    TT_TEST_CHECK_EQUAL(msg_ign->data.addr, NULL, "");
    TT_TEST_CHECK_EQUAL(msg_ign->data.len, 0, "");

    tt_sshmsg_release(out_msg);

    i = 0;
    while (i < TT_BUF_RLEN(&msg->buf)) {
        tt_u8_t bak = msg->buf.addr[msg->buf.rd_pos + i];

        msg->buf.addr[msg->buf.rd_pos + i] = (tt_u8_t)tt_rand_u32();
        tt_buf_backup_rwp(&msg->buf, &rp, &wp);
        ret = tt_sshmsg_parse(&msg->buf, &out_msg);
        tt_buf_restore_rwp(&msg->buf, &rp, &wp);
        if (TT_OK(ret)) {
            tt_sshmsg_release(out_msg);
        }
        msg->buf.addr[msg->buf.rd_pos + i] = bak;

        i += 1;
    }

    tt_sshmsg_release(msg);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_sshms_newkeys)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_sshmsg_t *msg, *out_msg;
    tt_result_t ret;
    tt_u32_t i;
    tt_u32_t rp, wp;

    TT_TEST_CASE_ENTER()
    // test start

    msg = tt_sshms_newkeys_create();
    TT_TEST_CHECK_NOT_EQUAL(msg, NULL, "");

    ret = tt_sshmsg_render(msg, 0, NULL);
    TT_TEST_CHECK_SUCCESS(ret, "");

    tt_buf_backup_rwp(&msg->buf, &rp, &wp);
    ret = tt_sshmsg_parse(&msg->buf, &out_msg);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_EQUAL(msg->msg_id, TT_SSH_MSGID_NEWKEYS, "");
    tt_buf_restore_rwp(&msg->buf, &rp, &wp);

    tt_sshmsg_release(out_msg);

    i = 0;
    while (i < TT_BUF_RLEN(&msg->buf)) {
        tt_u8_t bak = msg->buf.addr[msg->buf.rd_pos + i];

        msg->buf.addr[msg->buf.rd_pos + i] = (tt_u8_t)tt_rand_u32();
        tt_buf_backup_rwp(&msg->buf, &rp, &wp);
        ret = tt_sshmsg_parse(&msg->buf, &out_msg);
        tt_buf_restore_rwp(&msg->buf, &rp, &wp);
        if (TT_OK(ret)) {
            tt_sshmsg_release(out_msg);
        }
        msg->buf.addr[msg->buf.rd_pos + i] = bak;

        i += 1;
    }

    tt_sshmsg_release(msg);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_sshmsg_servreq)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_sshmsg_t *msg, *out_msg;
    tt_result_t ret;
    tt_u32_t i;
    tt_u32_t rp, wp;

    TT_TEST_CASE_ENTER()
    // test start

    // user auth
    msg = tt_sshmsg_servreq_create();
    TT_TEST_CHECK_NOT_EQUAL(msg, NULL, "");
    TT_TEST_CHECK_EQUAL(msg->msg_id, TT_SSH_MSGID_SERVICE_REQUEST, "");

    ret = tt_sshmsg_render(msg, 0, NULL);
    TT_TEST_CHECK_NOT_EQUAL(ret, TT_SUCCESS, "");

    tt_sshmsg_servreq_set_service(msg, TT_SSH_SERVICE_USERAUTH);
    ret = tt_sshmsg_render(msg, 0, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    tt_buf_backup_rwp(&msg->buf, &rp, &wp);
    ret = tt_sshmsg_parse(&msg->buf, &out_msg);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_EQUAL(msg->msg_id, TT_SSH_MSGID_SERVICE_REQUEST, "");
    tt_buf_restore_rwp(&msg->buf, &rp, &wp);

    TT_TEST_CHECK_EQUAL(tt_sshmsg_servreq_get_service(msg),
                        TT_SSH_SERVICE_USERAUTH,
                        "");

    tt_sshmsg_release(out_msg);

    // random content
    i = 0;
    while (i < TT_BUF_RLEN(&msg->buf)) {
        tt_u8_t bak = msg->buf.addr[msg->buf.rd_pos + i];

        msg->buf.addr[msg->buf.rd_pos + i] = (tt_u8_t)tt_rand_u32();
        tt_buf_backup_rwp(&msg->buf, &rp, &wp);
        ret = tt_sshmsg_parse(&msg->buf, &out_msg);
        tt_buf_restore_rwp(&msg->buf, &rp, &wp);
        if (TT_OK(ret)) {
            tt_sshmsg_release(out_msg);
        }
        msg->buf.addr[msg->buf.rd_pos + i] = bak;

        i += 1;
    }

    tt_sshmsg_release(msg);

    // connection
    msg = tt_sshmsg_servreq_create();
    TT_TEST_CHECK_NOT_EQUAL(msg, NULL, "");
    TT_TEST_CHECK_EQUAL(msg->msg_id, TT_SSH_MSGID_SERVICE_REQUEST, "");

    ret = tt_sshmsg_render(msg, 0, NULL);
    TT_TEST_CHECK_NOT_EQUAL(ret, TT_SUCCESS, "");

    tt_sshmsg_servreq_set_service(msg, TT_SSH_SERVICE_CONNECTION);
    ret = tt_sshmsg_render(msg, 0, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    tt_buf_backup_rwp(&msg->buf, &rp, &wp);
    ret = tt_sshmsg_parse(&msg->buf, &out_msg);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_EQUAL(msg->msg_id, TT_SSH_MSGID_SERVICE_REQUEST, "");
    tt_buf_restore_rwp(&msg->buf, &rp, &wp);

    TT_TEST_CHECK_EQUAL(tt_sshmsg_servreq_get_service(msg),
                        TT_SSH_SERVICE_CONNECTION,
                        "");

    tt_sshmsg_release(out_msg);

    // random content
    i = 0;
    while (i < TT_BUF_RLEN(&msg->buf)) {
        tt_u8_t bak = msg->buf.addr[msg->buf.rd_pos + i];

        msg->buf.addr[msg->buf.rd_pos + i] = (tt_u8_t)tt_rand_u32();
        tt_buf_backup_rwp(&msg->buf, &rp, &wp);
        ret = tt_sshmsg_parse(&msg->buf, &out_msg);
        tt_buf_restore_rwp(&msg->buf, &rp, &wp);
        if (TT_OK(ret)) {
            tt_sshmsg_release(out_msg);
        }
        msg->buf.addr[msg->buf.rd_pos + i] = bak;

        i += 1;
    }

    tt_sshmsg_release(msg);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_sshmsg_servacc)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_sshmsg_t *msg, *out_msg;
    tt_result_t ret;
    tt_u32_t i;
    tt_u32_t rp, wp;

    TT_TEST_CASE_ENTER()
    // test start

    // user auth
    msg = tt_sshmsg_servacc_create();
    TT_TEST_CHECK_NOT_EQUAL(msg, NULL, "");
    TT_TEST_CHECK_EQUAL(msg->msg_id, TT_SSH_MSGID_SERVICE_ACCEPT, "");

    ret = tt_sshmsg_render(msg, 0, NULL);
    TT_TEST_CHECK_NOT_EQUAL(ret, TT_SUCCESS, "");

    tt_sshmsg_servacc_set_service(msg, TT_SSH_SERVICE_USERAUTH);
    ret = tt_sshmsg_render(msg, 0, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    tt_buf_backup_rwp(&msg->buf, &rp, &wp);
    ret = tt_sshmsg_parse(&msg->buf, &out_msg);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_EQUAL(msg->msg_id, TT_SSH_MSGID_SERVICE_ACCEPT, "");
    tt_buf_restore_rwp(&msg->buf, &rp, &wp);

    TT_TEST_CHECK_EQUAL(tt_sshmsg_servacc_get_service(msg),
                        TT_SSH_SERVICE_USERAUTH,
                        "");

    tt_sshmsg_release(out_msg);

    // random content
    i = 0;
    while (i < TT_BUF_RLEN(&msg->buf)) {
        tt_u8_t bak = msg->buf.addr[msg->buf.rd_pos + i];

        msg->buf.addr[msg->buf.rd_pos + i] = (tt_u8_t)tt_rand_u32();
        tt_buf_backup_rwp(&msg->buf, &rp, &wp);
        ret = tt_sshmsg_parse(&msg->buf, &out_msg);
        tt_buf_restore_rwp(&msg->buf, &rp, &wp);
        if (TT_OK(ret)) {
            tt_sshmsg_release(out_msg);
        }
        msg->buf.addr[msg->buf.rd_pos + i] = bak;

        i += 1;
    }
    tt_sshmsg_release(msg);

    // connection
    msg = tt_sshmsg_servacc_create();
    TT_TEST_CHECK_NOT_EQUAL(msg, NULL, "");
    TT_TEST_CHECK_EQUAL(msg->msg_id, TT_SSH_MSGID_SERVICE_ACCEPT, "");

    ret = tt_sshmsg_render(msg, 0, NULL);
    TT_TEST_CHECK_NOT_EQUAL(ret, TT_SUCCESS, "");

    tt_sshmsg_servacc_set_service(msg, TT_SSH_SERVICE_CONNECTION);
    ret = tt_sshmsg_render(msg, 0, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    tt_buf_backup_rwp(&msg->buf, &rp, &wp);
    ret = tt_sshmsg_parse(&msg->buf, &out_msg);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_EQUAL(msg->msg_id, TT_SSH_MSGID_SERVICE_ACCEPT, "");
    tt_buf_restore_rwp(&msg->buf, &rp, &wp);

    TT_TEST_CHECK_EQUAL(tt_sshmsg_servacc_get_service(msg),
                        TT_SSH_SERVICE_CONNECTION,
                        "");

    tt_sshmsg_release(out_msg);

    // random content
    i = 0;
    while (i < TT_BUF_RLEN(&msg->buf)) {
        tt_u8_t bak = msg->buf.addr[msg->buf.rd_pos + i];

        msg->buf.addr[msg->buf.rd_pos + i] = (tt_u8_t)tt_rand_u32();
        tt_buf_backup_rwp(&msg->buf, &rp, &wp);
        ret = tt_sshmsg_parse(&msg->buf, &out_msg);
        tt_buf_restore_rwp(&msg->buf, &rp, &wp);
        if (TT_OK(ret)) {
            tt_sshmsg_release(out_msg);
        }
        msg->buf.addr[msg->buf.rd_pos + i] = bak;

        i += 1;
    }
    tt_sshmsg_release(msg);

    // test end
    TT_TEST_CASE_LEAVE()
}
