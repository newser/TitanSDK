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
TT_TEST_ROUTINE_DECLARE(case_sshvx_render)

TT_TEST_ROUTINE_DECLARE(case_sshvx_parse)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(sshmsg_vx_case)

TT_TEST_CASE("case_sshvx_render",
             "ssh msg: version exchange render",
             case_sshvx_render,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("case_sshvx_parse",
                 "ssh msg: version exchange parse",
                 case_sshvx_parse,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(sshmsg_vx_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(SSH_MSG_VERXCHG, 0, sshmsg_vx_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(case_sshvx_parse)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(case_sshvx_render)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    tt_sshmsg_t *msg, *out_msg;
    tt_sshms_verxchg_t *vx;
    tt_u32_t rp, wp;

    TT_TEST_CASE_ENTER()
    // test start

    ////////////////////////////////////////////
    // 1.
    ////////////////////////////////////////////

    msg = tt_sshms_verxchg_create();
    TT_UT_NOT_EQUAL(msg, NULL, "");

    tt_sshms_verxchg_set_protover(msg, TT_SSH_VER_1_0);
    tt_sshms_verxchg_set_protover(msg, TT_SSH_VER_1_99);
    tt_sshms_verxchg_set_protover(msg, TT_SSH_VER_2_0);

    ret = tt_sshms_verxchg_set_swver(msg, "test-1");
    TT_UT_SUCCESS(ret, "");
    ret = tt_sshms_verxchg_set_swver(msg, "");
    TT_UT_FAIL(ret, "");
    ret = tt_sshms_verxchg_set_swver(msg, "test-2");
    TT_UT_SUCCESS(ret, "");
    ret = tt_sshms_verxchg_set_swver(msg, "");
    TT_UT_FAIL(ret, "");

    ret = tt_sshms_verxchg_set_comment(msg, "cm-1");
    TT_UT_SUCCESS(ret, "");
    ret = tt_sshms_verxchg_set_comment(msg, NULL);
    TT_UT_SUCCESS(ret, "");
    ret = tt_sshms_verxchg_set_comment(msg, "cm-2");
    TT_UT_SUCCESS(ret, "");
    ret = tt_sshms_verxchg_set_comment(msg, "cm-3");
    TT_UT_SUCCESS(ret, "");

    tt_sshmsg_dump(msg);

    ret = tt_sshmsg_render(msg, 0, NULL);
    TT_UT_SUCCESS(ret, "");

    tt_buf_backup_rwp(&msg->buf, &rp, &wp);
    ret = tt_sshmsg_parse_verxchg(&msg->buf, &out_msg);
    tt_buf_restore_rwp(&msg->buf, &rp, &wp);
    TT_UT_SUCCESS(ret, "");

    // should saved
    TT_UT_EQUAL(TT_BUF_RLEN(&out_msg->buf), TT_BUF_RLEN(&msg->buf), "");
    TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(&out_msg->buf),
                          TT_BUF_RPOS(&msg->buf),
                          TT_BUF_RLEN(&msg->buf)),
                0,
                "");
    TT_UT_EQUAL(out_msg->msg_id, TT_SSH_MSGID_VERXCHG, "");

    // check msg id
    TT_UT_EQUAL(out_msg->msg_id, TT_SSH_MSGID_VERXCHG, "");
    vx = TT_SSHMSG_CAST(out_msg, tt_sshms_verxchg_t);

    // check ssh version
    TT_UT_EQUAL(vx->protover, TT_SSH_VER_2_0, "");

#if 0 // it does not parse other area
    // check software version
    TT_UT_EQUAL(vx->swver.len , 6, "");
    TT_UT_EQUAL(tt_memcmp(vx->swver.addr, "test-2", 6), 0, "");

    // check comments
    TT_UT_EQUAL(vx->comment.len , 4, "");
    TT_UT_EQUAL(tt_memcmp(vx->comment.addr, "cm-3", 4), 0, "");
#endif

    // test end
    TT_TEST_CASE_LEAVE()
}

struct __sshvx_parse_vec_t
{
    const tt_char_t *data;
    tt_result_t expect_ret;
    tt_ssh_ver_t ver;
};

struct __sshvx_parse_vec_t __sshvx_parse_vec[] = {
    {
        "SSH", // incomplete
        TT_E_BUF_NOBUFS,
        TT_SSH_VER_NUM,
    },
    {
        "SSH-", // incomplete
        TT_E_BUF_NOBUFS,
        TT_SSH_VER_NUM,
    },
    {
        "SSH-\r", // incomplete
        TT_E_BUF_NOBUFS,
        TT_SSH_VER_NUM,
    },
    {
        "SSH-\r\n", // invalid
        TT_FAIL,
        TT_SSH_VER_NUM,
    },
    {
        "123456", // incomplete
        TT_E_BUF_NOBUFS,
        TT_SSH_VER_NUM,
    },
    {
        "123456\r\n", // incomplete
        TT_E_BUF_NOBUFS,
        TT_SSH_VER_NUM,
    },
    {
        "123456SSH-2.0-sw comments", // no cr lf
        TT_E_BUF_NOBUFS,
        TT_SSH_VER_NUM,
    },
    {
        "SSH-1.9-sw comments\r\n", // invalid version
        TT_FAIL,
        TT_SSH_VER_NUM,
    },
    {
        "123456SSH-1.0-sw comments\r\n", TT_SUCCESS, TT_SSH_VER_1_0,
    },
    {
        "123456SSH-1.99-sw comments\n", // \n should be accepted
        TT_SUCCESS,
        TT_SSH_VER_1_99,
    },
    {
        "SSH-2.0-sw comments\n", TT_SUCCESS, TT_SSH_VER_2_0,
    },
    {
        "SSH-2.01-sw comments\n", // not supported version
        TT_FAIL,
        TT_SSH_VER_NUM,
    },
    {
        "SSH-2.0.1-sw comments\n", // not supported version
        TT_FAIL,
        TT_SSH_VER_NUM,
    },
};

TT_TEST_ROUTINE_DEFINE(case_sshvx_parse)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_buf_t buf;
    struct __sshvx_parse_vec_t *v;
    tt_u32_t i;
    tt_result_t ret;
    tt_sshmsg_t *msg;
    const tt_char_t *long_str = "a long string";

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&buf, NULL);

    // a long string would finally make tt_sshmsg_parse_verxchg() fail
    i = (tt_u32_t)tt_strlen(long_str);
    do {
        ret = tt_buf_put(&buf, (tt_u8_t *)long_str, i);
        TT_UT_SUCCESS(ret, "");

        ret = tt_sshmsg_parse_verxchg(&buf, &msg);
        TT_UT_EXP((ret == TT_E_BUF_NOBUFS) || (ret == TT_FAIL), "");
    } while (ret == TT_E_BUF_NOBUFS);
    TT_UT_EQUAL(ret, TT_FAIL, "");

    // a long string would finally make tt_sshmsg_parse_verxchg() fail
    ret = tt_buf_put(&buf, (tt_u8_t *)"SSH-", 4);
    TT_UT_SUCCESS(ret, "");
    do {
        ret = tt_buf_put(&buf, (tt_u8_t *)long_str, i);
        TT_UT_SUCCESS(ret, "");

        ret = tt_sshmsg_parse_verxchg(&buf, &msg);
        TT_UT_EXP((ret == TT_E_BUF_NOBUFS) || (ret == TT_FAIL), "");
    } while (ret == TT_E_BUF_NOBUFS);
    TT_UT_EQUAL(ret, TT_FAIL, "");

    for (i = 0; i < sizeof(__sshvx_parse_vec) / sizeof(__sshvx_parse_vec[0]);
         ++i) {
        v = &__sshvx_parse_vec[i];

        tt_buf_reset_rwp(&buf);
        ret =
            tt_buf_put(&buf, (tt_u8_t *)v->data, (tt_u32_t)tt_strlen(v->data));
        TT_UT_EQUAL(ret, TT_SUCCESS, "");

        ret = tt_sshmsg_parse_verxchg(&buf, &msg);
        TT_UT_EQUAL(ret, v->expect_ret, "");

        // restore buf pos is done in __svrconn_handle_recv not in
        // tt_sshmsg_parse
        if (TT_OK(ret)) {
            tt_sshms_verxchg_t *vx = TT_SSHMSG_CAST(msg, tt_sshms_verxchg_t);

            TT_UT_EQUAL(vx->protover, v->ver, "");
            tt_sshmsg_release(msg);
        }
    }

    tt_buf_destroy(&buf);

    // test end
    TT_TEST_CASE_LEAVE()
}
