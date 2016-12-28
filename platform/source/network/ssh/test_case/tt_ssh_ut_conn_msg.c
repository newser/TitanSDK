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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_sshmsg_conn_glbreq)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_sshmsg_conn_reqsucc)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_sshmsg_conn_reqfail)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_sshmsg_conn_chopen)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_sshmsg_conn_chopc)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_sshmsg_conn_chopf)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_sshmsg_conn_winadj)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_sshmsg_conn_chdata)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_sshmsg_conn_chclose)

TT_TEST_ROUTINE_DECLARE(tt_unit_test_sshmsg_conn_chreq)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_sshmsg_conn_chsucc)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_sshmsg_conn_chfail)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(sshconnmsg_case)

TT_TEST_CASE("tt_unit_test_sshmsg_conn_glbreq",
             "ssh msg: global request",
             tt_unit_test_sshmsg_conn_glbreq,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_sshmsg_conn_reqsucc",
                 "ssh msg: request success",
                 tt_unit_test_sshmsg_conn_reqsucc,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_sshmsg_conn_reqfail",
                 "ssh msg: request failure",
                 tt_unit_test_sshmsg_conn_reqfail,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_sshmsg_conn_chopen",
                 "ssh msg: channel open",
                 tt_unit_test_sshmsg_conn_chopen,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_sshmsg_conn_chopc",
                 "ssh msg: channel open cfm",
                 tt_unit_test_sshmsg_conn_chopc,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_sshmsg_conn_chopf",
                 "ssh msg: channel open fail",
                 tt_unit_test_sshmsg_conn_chopf,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_sshmsg_conn_winadj",
                 "ssh msg: channel window adjust",
                 tt_unit_test_sshmsg_conn_winadj,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_sshmsg_conn_chdata",
                 "ssh msg: channel data",
                 tt_unit_test_sshmsg_conn_chdata,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_sshmsg_conn_chclose",
                 "ssh msg: channel close",
                 tt_unit_test_sshmsg_conn_chclose,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_sshmsg_conn_chreq",
                 "ssh msg: channel request",
                 tt_unit_test_sshmsg_conn_chreq,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_sshmsg_conn_chsucc",
                 "ssh msg: channel success",
                 tt_unit_test_sshmsg_conn_chsucc,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_sshmsg_conn_chfail",
                 "ssh msg: channel failure",
                 tt_unit_test_sshmsg_conn_chfail,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(sshconnmsg_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(SSH_CONN_MSG, 0, sshconnmsg_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(tt_unit_test_sshmsg_conn_glbreq)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(tt_unit_test_sshmsg_conn_glbreq)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_sshmsg_t *msg, *out_msg;
    tt_sshmsg_glbreq_t *gr;
    tt_result_t ret;
    tt_u32_t i;
    tt_u32_t rp, wp;
    tt_u8_t pkey[] = {1, 2, 3, 4};
    tt_u8_t sig[] = {0, 2, 3, 7};

    TT_TEST_CASE_ENTER()
    // test start

    msg = tt_sshmsg_glbreq_create();
    gr = TT_SSHMSG_CAST(msg, tt_sshmsg_glbreq_t);
    TT_TEST_CHECK_NOT_EQUAL(msg, NULL, "");

    // request name
    ret = tt_sshmsg_glbreq_set_reqname(msg, "test");
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(gr->req_name.len, 4, "");
    TT_TEST_CHECK_EQUAL(tt_memcmp(gr->req_name.addr, "test", 4), 0, "");

    ret = tt_sshmsg_glbreq_set_reqname(msg, "");
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(gr->req_name.len, 0, "");
    TT_TEST_CHECK_EQUAL(gr->req_name.addr, NULL, "");

    // want reply
    tt_sshmsg_glbreq_set_wantreply(msg, TT_TRUE);
    TT_TEST_CHECK_EQUAL(gr->want_reply, TT_TRUE, "");

    // render
    ret = tt_sshmsg_render(msg, 0, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // parse
    tt_buf_backup_rwp(&msg->buf, &rp, &wp);
    ret = tt_sshmsg_parse(&msg->buf, &out_msg);
    tt_buf_restore_rwp(&msg->buf, &rp, &wp);
    TT_TEST_CHECK_SUCCESS(ret, "");

    gr = TT_SSHMSG_CAST(out_msg, tt_sshmsg_glbreq_t);

    TT_TEST_CHECK_EQUAL(gr->req_name.addr, NULL, "");
    TT_TEST_CHECK_EQUAL(gr->req_name.len, 0, "");
    TT_TEST_CHECK_EQUAL(gr->want_reply, TT_TRUE, "");

    tt_sshmsg_release(out_msg);

    i = 0;
    while (i < TT_BUF_RLEN(&msg->buf)) {
        tt_u8_t bak = msg->buf.p[msg->buf.rpos + i];

        msg->buf.p[msg->buf.rpos + i] = (tt_u8_t)tt_rand_u32();
        tt_buf_backup_rwp(&msg->buf, &rp, &wp);
        ret = tt_sshmsg_parse(&msg->buf, &out_msg);
        tt_buf_restore_rwp(&msg->buf, &rp, &wp);
        if (TT_OK(ret)) {
            tt_sshmsg_release(out_msg);
        }
        msg->buf.p[msg->buf.rpos + i] = bak;

        i += 1;
    }

    tt_sshmsg_release(msg);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_sshmsg_conn_reqsucc)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_sshmsg_t *msg, *out_msg;
    tt_sshmsg_reqsucc_t *rs;
    tt_result_t ret;
    tt_u32_t i;
    tt_u32_t rp, wp;
    tt_u8_t pkey[] = {1, 2, 3, 4};
    tt_u8_t sig[] = {0, 2, 3, 7};

    TT_TEST_CASE_ENTER()
    // test start

    msg = tt_sshmsg_reqsucc_create();
    rs = TT_SSHMSG_CAST(msg, tt_sshmsg_reqsucc_t);
    TT_TEST_CHECK_NOT_EQUAL(msg, NULL, "");
    (void)rs;

    // render
    ret = tt_sshmsg_render(msg, 0, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // parse
    tt_buf_backup_rwp(&msg->buf, &rp, &wp);
    ret = tt_sshmsg_parse(&msg->buf, &out_msg);
    tt_buf_restore_rwp(&msg->buf, &rp, &wp);
    TT_TEST_CHECK_SUCCESS(ret, "");

    TT_TEST_CHECK_EQUAL(out_msg->msg_id, TT_SSH_MSGID_REQUEST_SUCCESS, "");
    rs = TT_SSHMSG_CAST(out_msg, tt_sshmsg_reqsucc_t);

    tt_sshmsg_release(out_msg);

    i = 0;
    while (i < TT_BUF_RLEN(&msg->buf)) {
        tt_u8_t bak = msg->buf.p[msg->buf.rpos + i];

        msg->buf.p[msg->buf.rpos + i] = (tt_u8_t)tt_rand_u32();
        tt_buf_backup_rwp(&msg->buf, &rp, &wp);
        ret = tt_sshmsg_parse(&msg->buf, &out_msg);
        tt_buf_restore_rwp(&msg->buf, &rp, &wp);
        if (TT_OK(ret)) {
            tt_sshmsg_release(out_msg);
        }
        msg->buf.p[msg->buf.rpos + i] = bak;

        i += 1;
    }

    tt_sshmsg_release(msg);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_sshmsg_conn_reqfail)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_sshmsg_t *msg, *out_msg;
    tt_result_t ret;
    tt_u32_t i;
    tt_u32_t rp, wp;

    TT_TEST_CASE_ENTER()
    // test start

    msg = tt_sshmsg_reqfail_create();
    TT_TEST_CHECK_NOT_EQUAL(msg, NULL, "");

    // render
    ret = tt_sshmsg_render(msg, 0, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // parse
    tt_buf_backup_rwp(&msg->buf, &rp, &wp);
    ret = tt_sshmsg_parse(&msg->buf, &out_msg);
    tt_buf_restore_rwp(&msg->buf, &rp, &wp);
    TT_TEST_CHECK_SUCCESS(ret, "");

    TT_TEST_CHECK_EQUAL(out_msg->msg_id, TT_SSH_MSGID_REQUEST_FAILURE, "");

    tt_sshmsg_release(out_msg);

    i = 0;
    while (i < TT_BUF_RLEN(&msg->buf)) {
        tt_u8_t bak = msg->buf.p[msg->buf.rpos + i];

        msg->buf.p[msg->buf.rpos + i] = (tt_u8_t)tt_rand_u32();
        tt_buf_backup_rwp(&msg->buf, &rp, &wp);
        ret = tt_sshmsg_parse(&msg->buf, &out_msg);
        tt_buf_restore_rwp(&msg->buf, &rp, &wp);
        if (TT_OK(ret)) {
            tt_sshmsg_release(out_msg);
        }
        msg->buf.p[msg->buf.rpos + i] = bak;

        i += 1;
    }

    tt_sshmsg_release(msg);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_sshmsg_conn_chopen)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_sshmsg_t *msg, *out_msg;
    tt_sshmsg_chopen_t *chop;
    tt_result_t ret;
    tt_u32_t i;
    tt_u32_t rp, wp;

    TT_TEST_CASE_ENTER()
    // test start

    msg = tt_sshmsg_chopen_create();
    chop = TT_SSHMSG_CAST(msg, tt_sshmsg_chopen_t);
    TT_TEST_CHECK_NOT_EQUAL(msg, NULL, "");

    // render fail: no type
    ret = tt_sshmsg_render(msg, 0, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");

    // type
    tt_sshmsg_chopen_set_type(msg, TT_SSH_CHTYPE_SESSION);
    TT_TEST_CHECK_EQUAL(chop->type, TT_SSH_CHTYPE_SESSION, "");

    // send channel
    tt_sshmsg_chopen_set_sndchnum(msg, ~0);
    TT_TEST_CHECK_EQUAL(chop->snd_chnum, ~0, "");

    // winsize
    tt_sshmsg_chopen_set_winsize(msg, ~0);
    TT_TEST_CHECK_EQUAL(chop->window_size, ~0, "");

    // pktsize
    tt_sshmsg_chopen_set_pktsize(msg, ~0);
    TT_TEST_CHECK_EQUAL(chop->packet_size, ~0, "");

    // render
    ret = tt_sshmsg_render(msg, 0, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // parse
    tt_buf_backup_rwp(&msg->buf, &rp, &wp);
    ret = tt_sshmsg_parse(&msg->buf, &out_msg);
    tt_buf_restore_rwp(&msg->buf, &rp, &wp);
    TT_TEST_CHECK_SUCCESS(ret, "");

    chop = TT_SSHMSG_CAST(out_msg, tt_sshmsg_chopen_t);

    TT_TEST_CHECK_EQUAL(chop->type, TT_SSH_CHTYPE_SESSION, "");
    TT_TEST_CHECK_EQUAL(chop->snd_chnum, ~0, "");
    TT_TEST_CHECK_EQUAL(chop->window_size, ~0, "");
    TT_TEST_CHECK_EQUAL(chop->packet_size, ~0, "");

    tt_sshmsg_release(out_msg);

    i = 0;
    while (i < TT_BUF_RLEN(&msg->buf)) {
        tt_u8_t bak = msg->buf.p[msg->buf.rpos + i];

        msg->buf.p[msg->buf.rpos + i] = (tt_u8_t)tt_rand_u32();
        tt_buf_backup_rwp(&msg->buf, &rp, &wp);
        ret = tt_sshmsg_parse(&msg->buf, &out_msg);
        tt_buf_restore_rwp(&msg->buf, &rp, &wp);
        if (TT_OK(ret)) {
            tt_sshmsg_release(out_msg);
        }
        msg->buf.p[msg->buf.rpos + i] = bak;

        i += 1;
    }

    tt_sshmsg_release(msg);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_sshmsg_conn_chopc)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_sshmsg_t *msg, *out_msg;
    tt_sshmsg_chopc_t *chopc;
    tt_result_t ret;
    tt_u32_t i;
    tt_u32_t rp, wp;

    TT_TEST_CASE_ENTER()
    // test start

    msg = tt_sshmsg_chopc_create();
    chopc = TT_SSHMSG_CAST(msg, tt_sshmsg_chopc_t);
    TT_TEST_CHECK_NOT_EQUAL(msg, NULL, "");

    // recv channel
    tt_sshmsg_chopc_set_rcvchnum(msg, 1);
    TT_TEST_CHECK_EQUAL(chopc->rcv_chnum, 1, "");

    // send channel
    tt_sshmsg_chopc_set_sndchnum(msg, 2);
    TT_TEST_CHECK_EQUAL(chopc->snd_chnum, 2, "");

    // winsize
    tt_sshmsg_chopc_set_winsize(msg, 3);
    TT_TEST_CHECK_EQUAL(chopc->window_size, 3, "");

    // pktsize
    tt_sshmsg_chopc_set_pktsize(msg, 4);
    TT_TEST_CHECK_EQUAL(chopc->packet_size, 4, "");

    // render
    ret = tt_sshmsg_render(msg, 0, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // parse
    tt_buf_backup_rwp(&msg->buf, &rp, &wp);
    ret = tt_sshmsg_parse(&msg->buf, &out_msg);
    tt_buf_restore_rwp(&msg->buf, &rp, &wp);
    TT_TEST_CHECK_SUCCESS(ret, "");

    chopc = TT_SSHMSG_CAST(out_msg, tt_sshmsg_chopc_t);

    TT_TEST_CHECK_EQUAL(chopc->rcv_chnum, 1, "");
    TT_TEST_CHECK_EQUAL(chopc->snd_chnum, 2, "");
    TT_TEST_CHECK_EQUAL(chopc->window_size, 3, "");
    TT_TEST_CHECK_EQUAL(chopc->packet_size, 4, "");

    tt_sshmsg_release(out_msg);

    i = 0;
    while (i < TT_BUF_RLEN(&msg->buf)) {
        tt_u8_t bak = msg->buf.p[msg->buf.rpos + i];

        msg->buf.p[msg->buf.rpos + i] = (tt_u8_t)tt_rand_u32();
        tt_buf_backup_rwp(&msg->buf, &rp, &wp);
        ret = tt_sshmsg_parse(&msg->buf, &out_msg);
        tt_buf_restore_rwp(&msg->buf, &rp, &wp);
        if (TT_OK(ret)) {
            tt_sshmsg_release(out_msg);
        }
        msg->buf.p[msg->buf.rpos + i] = bak;

        i += 1;
    }

    tt_sshmsg_release(msg);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_sshmsg_conn_chopf)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_sshmsg_t *msg, *out_msg;
    tt_sshmsg_chopf_t *chopf;
    tt_result_t ret;
    tt_u32_t i;
    tt_u32_t rp, wp;

    TT_TEST_CASE_ENTER()
    // test start

    msg = tt_sshmsg_chopf_create();
    chopf = TT_SSHMSG_CAST(msg, tt_sshmsg_chopf_t);
    TT_TEST_CHECK_NOT_EQUAL(msg, NULL, "");

    // recv channel
    tt_sshmsg_chopf_set_rcvchnum(msg, 1);
    TT_TEST_CHECK_EQUAL(chopf->rcv_chnum, 1, "");

    // reason
    tt_sshmsg_chopf_set_reason(msg, 250);
    TT_TEST_CHECK_EQUAL(chopf->reason_code, 250, "");

    // description
    tt_sshmsg_chopf_set_desc(msg, "chopf");
    TT_TEST_CHECK_EQUAL(tt_memcmp(chopf->desc.addr, "chopf", 5), 0, "");
    TT_TEST_CHECK_EQUAL(chopf->desc.len, 5, "");

    // render
    ret = tt_sshmsg_render(msg, 0, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // parse
    tt_buf_backup_rwp(&msg->buf, &rp, &wp);
    ret = tt_sshmsg_parse(&msg->buf, &out_msg);
    tt_buf_restore_rwp(&msg->buf, &rp, &wp);
    TT_TEST_CHECK_SUCCESS(ret, "");

    chopf = TT_SSHMSG_CAST(out_msg, tt_sshmsg_chopf_t);

    TT_TEST_CHECK_EQUAL(chopf->rcv_chnum, 1, "");
    TT_TEST_CHECK_EQUAL(chopf->reason_code, 250, "");
    TT_TEST_CHECK_EQUAL(tt_memcmp(chopf->desc.addr, "chopf", 5), 0, "");
    TT_TEST_CHECK_EQUAL(chopf->desc.len, 5, "");

    tt_sshmsg_release(out_msg);

    i = 0;
    while (i < TT_BUF_RLEN(&msg->buf)) {
        tt_u8_t bak = msg->buf.p[msg->buf.rpos + i];

        msg->buf.p[msg->buf.rpos + i] = (tt_u8_t)tt_rand_u32();
        tt_buf_backup_rwp(&msg->buf, &rp, &wp);
        ret = tt_sshmsg_parse(&msg->buf, &out_msg);
        tt_buf_restore_rwp(&msg->buf, &rp, &wp);
        if (TT_OK(ret)) {
            tt_sshmsg_release(out_msg);
        }
        msg->buf.p[msg->buf.rpos + i] = bak;

        i += 1;
    }

    tt_sshmsg_release(msg);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_sshmsg_conn_winadj)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_sshmsg_t *msg, *out_msg;
    tt_sshmsg_chwinadj_t *wa;
    tt_result_t ret;
    tt_u32_t i;
    tt_u32_t rp, wp;

    TT_TEST_CASE_ENTER()
    // test start

    msg = tt_sshmsg_chwinadj_create();
    wa = TT_SSHMSG_CAST(msg, tt_sshmsg_chwinadj_t);
    TT_TEST_CHECK_NOT_EQUAL(msg, NULL, "");

    // recv channel
    tt_sshmsg_chwinadj_set_rcvchnum(msg, 1);
    TT_TEST_CHECK_EQUAL(wa->rcv_chnum, 1, "");

    // reason
    tt_sshmsg_chwinadj_set_capacity(msg, 250);
    TT_TEST_CHECK_EQUAL(wa->capacity, 250, "");

    // render
    ret = tt_sshmsg_render(msg, 0, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // parse
    tt_buf_backup_rwp(&msg->buf, &rp, &wp);
    ret = tt_sshmsg_parse(&msg->buf, &out_msg);
    tt_buf_restore_rwp(&msg->buf, &rp, &wp);
    TT_TEST_CHECK_SUCCESS(ret, "");

    wa = TT_SSHMSG_CAST(out_msg, tt_sshmsg_chwinadj_t);

    TT_TEST_CHECK_EQUAL(wa->rcv_chnum, 1, "");
    TT_TEST_CHECK_EQUAL(wa->capacity, 250, "");

    tt_sshmsg_release(out_msg);

    i = 0;
    while (i < TT_BUF_RLEN(&msg->buf)) {
        tt_u8_t bak = msg->buf.p[msg->buf.rpos + i];

        msg->buf.p[msg->buf.rpos + i] = (tt_u8_t)tt_rand_u32();
        tt_buf_backup_rwp(&msg->buf, &rp, &wp);
        ret = tt_sshmsg_parse(&msg->buf, &out_msg);
        tt_buf_restore_rwp(&msg->buf, &rp, &wp);
        if (TT_OK(ret)) {
            tt_sshmsg_release(out_msg);
        }
        msg->buf.p[msg->buf.rpos + i] = bak;

        i += 1;
    }

    tt_sshmsg_release(msg);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_sshmsg_conn_chdata)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_sshmsg_t *msg, *out_msg;
    tt_sshmsg_chdata_t *chdata;
    tt_result_t ret;
    tt_u32_t i;
    tt_u32_t rp, wp;
    tt_u8_t data1[100], data2[100];

    TT_TEST_CASE_ENTER()
    // test start

    msg = tt_sshmsg_chdata_create();
    chdata = TT_SSHMSG_CAST(msg, tt_sshmsg_chdata_t);
    TT_TEST_CHECK_NOT_EQUAL(msg, NULL, "");

    // recv channel
    tt_sshmsg_chdata_set_rcvchnum(msg, 1);
    TT_TEST_CHECK_EQUAL(chdata->rcv_chnum, 1, "");

    // data
    for (i = 0; i < sizeof(data1); ++i) {
        data1[i] = (tt_u8_t)tt_rand_u32();
    }
    for (i = 0; i < sizeof(data2); ++i) {
        data2[i] = (tt_u8_t)tt_rand_u32();
    }

    ret = tt_sshmsg_chdata_set_data(msg, data1, sizeof(data1), TT_TRUE);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&chdata->data), sizeof(data1), "");
    TT_TEST_CHECK_EQUAL(tt_memcmp(TT_BUF_RPOS(&chdata->data),
                                  data1,
                                  sizeof(data1)),
                        0,
                        "");

    ret = tt_sshmsg_chdata_set_data(msg, data2, sizeof(data2), TT_FALSE);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&chdata->data), sizeof(data2), "");
    TT_TEST_CHECK_EQUAL(tt_memcmp(TT_BUF_RPOS(&chdata->data),
                                  data2,
                                  sizeof(data2)),
                        0,
                        "");

    ret = tt_sshmsg_chdata_set_data(msg, data1, sizeof(data1), TT_TRUE);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&chdata->data), sizeof(data1), "");
    TT_TEST_CHECK_EQUAL(tt_memcmp(TT_BUF_RPOS(&chdata->data),
                                  data1,
                                  sizeof(data1)),
                        0,
                        "");

    ret = tt_sshmsg_chdata_set_data(msg, data2, sizeof(data2), TT_FALSE);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&chdata->data), sizeof(data2), "");
    TT_TEST_CHECK_EQUAL(tt_memcmp(TT_BUF_RPOS(&chdata->data),
                                  data2,
                                  sizeof(data2)),
                        0,
                        "");

    // render
    ret = tt_sshmsg_render(msg, 0, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // parse
    tt_buf_backup_rwp(&msg->buf, &rp, &wp);
    ret = tt_sshmsg_parse(&msg->buf, &out_msg);
    tt_buf_restore_rwp(&msg->buf, &rp, &wp);
    TT_TEST_CHECK_SUCCESS(ret, "");

    chdata = TT_SSHMSG_CAST(out_msg, tt_sshmsg_chdata_t);

    TT_TEST_CHECK_EQUAL(chdata->rcv_chnum, 1, "");
    TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&chdata->data), sizeof(data2), "");
    TT_TEST_CHECK_EQUAL(tt_memcmp(TT_BUF_RPOS(&chdata->data),
                                  data2,
                                  sizeof(data2)),
                        0,
                        "");

    tt_sshmsg_release(out_msg);

    i = 0;
    while (i < TT_BUF_RLEN(&msg->buf)) {
        tt_u8_t bak = msg->buf.p[msg->buf.rpos + i];

        msg->buf.p[msg->buf.rpos + i] = (tt_u8_t)tt_rand_u32();
        tt_buf_backup_rwp(&msg->buf, &rp, &wp);
        ret = tt_sshmsg_parse(&msg->buf, &out_msg);
        tt_buf_restore_rwp(&msg->buf, &rp, &wp);
        if (TT_OK(ret)) {
            tt_sshmsg_release(out_msg);
        }
        msg->buf.p[msg->buf.rpos + i] = bak;

        i += 1;
    }

    tt_sshmsg_release(msg);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_sshmsg_conn_chclose)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_sshmsg_t *msg, *out_msg;
    tt_sshmsg_chclose_t *chc;
    tt_result_t ret;
    tt_u32_t i;
    tt_u32_t rp, wp;

    TT_TEST_CASE_ENTER()
    // test start

    msg = tt_sshmsg_chclose_create();
    chc = TT_SSHMSG_CAST(msg, tt_sshmsg_chclose_t);
    TT_TEST_CHECK_NOT_EQUAL(msg, NULL, "");

    // recv channel
    tt_sshmsg_chclose_set_rcvchnum(msg, 1);
    TT_TEST_CHECK_EQUAL(chc->rcv_chnum, 1, "");

    // render
    ret = tt_sshmsg_render(msg, 0, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // parse
    tt_buf_backup_rwp(&msg->buf, &rp, &wp);
    ret = tt_sshmsg_parse(&msg->buf, &out_msg);
    tt_buf_restore_rwp(&msg->buf, &rp, &wp);
    TT_TEST_CHECK_SUCCESS(ret, "");

    chc = TT_SSHMSG_CAST(out_msg, tt_sshmsg_chclose_t);

    TT_TEST_CHECK_EQUAL(chc->rcv_chnum, 1, "");

    tt_sshmsg_release(out_msg);

    i = 0;
    while (i < TT_BUF_RLEN(&msg->buf)) {
        tt_u8_t bak = msg->buf.p[msg->buf.rpos + i];

        msg->buf.p[msg->buf.rpos + i] = (tt_u8_t)tt_rand_u32();
        tt_buf_backup_rwp(&msg->buf, &rp, &wp);
        ret = tt_sshmsg_parse(&msg->buf, &out_msg);
        tt_buf_restore_rwp(&msg->buf, &rp, &wp);
        if (TT_OK(ret)) {
            tt_sshmsg_release(out_msg);
        }
        msg->buf.p[msg->buf.rpos + i] = bak;

        i += 1;
    }

    tt_sshmsg_release(msg);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_sshmsg_conn_chreq)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_sshmsg_t *msg, *out_msg;
    tt_sshmsg_chreq_t *chr;
    tt_result_t ret;
    tt_u32_t i;
    tt_u32_t rp, wp;

    TT_TEST_CASE_ENTER()
    // test start

    msg = tt_sshmsg_chreq_create();
    chr = TT_SSHMSG_CAST(msg, tt_sshmsg_chreq_t);
    TT_TEST_CHECK_NOT_EQUAL(msg, NULL, "");

    // render fail
    ret = tt_sshmsg_render(msg, 0, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");

    // recv channel
    tt_sshmsg_chreq_set_rcvchnum(msg, 1);
    TT_TEST_CHECK_EQUAL(chr->rcv_chnum, 1, "");

    // render fail
    ret = tt_sshmsg_render(msg, 0, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");

    // want reply
    tt_sshmsg_chreq_set_wantreply(msg, TT_TRUE);
    TT_TEST_CHECK_EQUAL(chr->want_reply, TT_TRUE, "");

    // render fail
    ret = tt_sshmsg_render(msg, 0, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");

    // req type
    tt_sshmsg_chreq_set_reqtype(msg, TT_SSH_CHREQTYPE_EXITSIGNAL);
    TT_TEST_CHECK_EQUAL(chr->req_type, TT_SSH_CHREQTYPE_EXITSIGNAL, "");

    // render
    ret = tt_sshmsg_render(msg, 0, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // parse
    tt_buf_backup_rwp(&msg->buf, &rp, &wp);
    ret = tt_sshmsg_parse(&msg->buf, &out_msg);
    tt_buf_restore_rwp(&msg->buf, &rp, &wp);
    TT_TEST_CHECK_SUCCESS(ret, "");

    chr = TT_SSHMSG_CAST(out_msg, tt_sshmsg_chreq_t);

    TT_TEST_CHECK_EQUAL(chr->rcv_chnum, 1, "");
    TT_TEST_CHECK_EQUAL(chr->req_type, TT_SSH_CHREQTYPE_EXITSIGNAL, "");
    TT_TEST_CHECK_EQUAL(chr->want_reply, TT_TRUE, "");

    tt_sshmsg_release(out_msg);

    i = 0;
    while (i < TT_BUF_RLEN(&msg->buf)) {
        tt_u8_t bak = msg->buf.p[msg->buf.rpos + i];

        msg->buf.p[msg->buf.rpos + i] = (tt_u8_t)tt_rand_u32();
        tt_buf_backup_rwp(&msg->buf, &rp, &wp);
        ret = tt_sshmsg_parse(&msg->buf, &out_msg);
        tt_buf_restore_rwp(&msg->buf, &rp, &wp);
        if (TT_OK(ret)) {
            tt_sshmsg_release(out_msg);
        }
        msg->buf.p[msg->buf.rpos + i] = bak;

        i += 1;
    }

    tt_sshmsg_release(msg);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_sshmsg_conn_chsucc)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_sshmsg_t *msg, *out_msg;
    tt_sshmsg_chsucc_t *chs;
    tt_result_t ret;
    tt_u32_t i;
    tt_u32_t rp, wp;

    TT_TEST_CASE_ENTER()
    // test start

    msg = tt_sshmsg_chsucc_create();
    chs = TT_SSHMSG_CAST(msg, tt_sshmsg_chsucc_t);
    TT_TEST_CHECK_NOT_EQUAL(msg, NULL, "");

    // recv channel
    tt_sshmsg_chsucc_set_rcvchnum(msg, 1);
    TT_TEST_CHECK_EQUAL(chs->rcv_chnum, 1, "");

    // render
    ret = tt_sshmsg_render(msg, 0, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // parse
    tt_buf_backup_rwp(&msg->buf, &rp, &wp);
    ret = tt_sshmsg_parse(&msg->buf, &out_msg);
    tt_buf_restore_rwp(&msg->buf, &rp, &wp);
    TT_TEST_CHECK_SUCCESS(ret, "");

    chs = TT_SSHMSG_CAST(out_msg, tt_sshmsg_chsucc_t);

    TT_TEST_CHECK_EQUAL(chs->rcv_chnum, 1, "");

    tt_sshmsg_release(out_msg);

    i = 0;
    while (i < TT_BUF_RLEN(&msg->buf)) {
        tt_u8_t bak = msg->buf.p[msg->buf.rpos + i];

        msg->buf.p[msg->buf.rpos + i] = (tt_u8_t)tt_rand_u32();
        tt_buf_backup_rwp(&msg->buf, &rp, &wp);
        ret = tt_sshmsg_parse(&msg->buf, &out_msg);
        tt_buf_restore_rwp(&msg->buf, &rp, &wp);
        if (TT_OK(ret)) {
            tt_sshmsg_release(out_msg);
        }
        msg->buf.p[msg->buf.rpos + i] = bak;

        i += 1;
    }

    tt_sshmsg_release(msg);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_sshmsg_conn_chfail)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_sshmsg_t *msg, *out_msg;
    tt_sshmsg_chfail_t *chf;
    tt_result_t ret;
    tt_u32_t i;
    tt_u32_t rp, wp;

    TT_TEST_CASE_ENTER()
    // test start

    msg = tt_sshmsg_chfail_create();
    chf = TT_SSHMSG_CAST(msg, tt_sshmsg_chfail_t);
    TT_TEST_CHECK_NOT_EQUAL(msg, NULL, "");

    // recv channel
    tt_sshmsg_chfail_set_rcvchnum(msg, 1);
    TT_TEST_CHECK_EQUAL(chf->rcv_chnum, 1, "");

    // render
    ret = tt_sshmsg_render(msg, 0, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // parse
    tt_buf_backup_rwp(&msg->buf, &rp, &wp);
    ret = tt_sshmsg_parse(&msg->buf, &out_msg);
    tt_buf_restore_rwp(&msg->buf, &rp, &wp);
    TT_TEST_CHECK_SUCCESS(ret, "");

    chf = TT_SSHMSG_CAST(out_msg, tt_sshmsg_chfail_t);

    TT_TEST_CHECK_EQUAL(chf->rcv_chnum, 1, "");

    tt_sshmsg_release(out_msg);

    i = 0;
    while (i < TT_BUF_RLEN(&msg->buf)) {
        tt_u8_t bak = msg->buf.p[msg->buf.rpos + i];

        msg->buf.p[msg->buf.rpos + i] = (tt_u8_t)tt_rand_u32();
        tt_buf_backup_rwp(&msg->buf, &rp, &wp);
        ret = tt_sshmsg_parse(&msg->buf, &out_msg);
        tt_buf_restore_rwp(&msg->buf, &rp, &wp);
        if (TT_OK(ret)) {
            tt_sshmsg_release(out_msg);
        }
        msg->buf.p[msg->buf.rpos + i] = bak;

        i += 1;
    }

    tt_sshmsg_release(msg);

    // test end
    TT_TEST_CASE_LEAVE()
}
