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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_sshmsg_usrauthreq_pkey)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_sshmsg_usrauthreq_pwd)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_sshmsg_usrauth_success)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_sshmsg_usrauth_fail)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_sshmsg_usrauth_banner)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(sshauthmsg_case)

TT_TEST_CASE("tt_unit_test_sshmsg_usrauthreq_pkey",
             "ssh msg: user auth req: pubkey",
             tt_unit_test_sshmsg_usrauthreq_pkey,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_sshmsg_usrauthreq_pwd",
                 "ssh msg: user auth req: password",
                 tt_unit_test_sshmsg_usrauthreq_pwd,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_sshmsg_usrauth_success",
                 "ssh msg: user auth success",
                 tt_unit_test_sshmsg_usrauth_success,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_sshmsg_usrauth_fail",
                 "ssh msg: user auth failure",
                 tt_unit_test_sshmsg_usrauth_fail,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_sshmsg_usrauth_banner",
                 "ssh msg: user auth banner",
                 tt_unit_test_sshmsg_usrauth_banner,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(sshauthmsg_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(SSH_AUTH_MSG, 0, sshauthmsg_case)

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

    TT_TEST_ROUTINE_DEFINE(tt_unit_test_sshmsg_usrauthreq_pkey)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_sshmsg_t *msg, *out_msg;
    tt_sshmsg_uar_t *uar;
    tt_result_t ret;
    tt_u32_t i;
    tt_u32_t rp, wp;
    tt_u8_t pkey[] = {1, 2, 3, 4};
    tt_u8_t sig[] = {0, 2, 3, 7};

    TT_TEST_CASE_ENTER()
    // test start

    msg = tt_sshmsg_uar_create();
    uar = TT_SSHMSG_CAST(msg, tt_sshmsg_uar_t);
    TT_TEST_CHECK_NOT_EQUAL(msg, NULL, "");
    ret = tt_sshmsg_render(msg, 0, NULL);
    TT_TEST_CHECK_NOT_EQUAL(ret, TT_SUCCESS, "");

    // user
    ret = tt_sshmsg_uar_set_user(msg, "test");
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(uar->user.len, 4, "");
    TT_TEST_CHECK_EQUAL(tt_memcmp(uar->user.addr, "test", 4), 0, "");

    ret = tt_sshmsg_uar_set_user(msg, "");
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(uar->user.len, 0, "");
    TT_TEST_CHECK_EQUAL(uar->user.addr, NULL, "");

    ret = tt_sshmsg_render(msg, 0, NULL);
    TT_TEST_CHECK_NOT_EQUAL(ret, TT_SUCCESS, "");

    // service
    tt_sshmsg_uar_set_service(msg, TT_SSH_SERVICE_CONNECTION);
    TT_TEST_CHECK_EQUAL(uar->service, TT_SSH_SERVICE_CONNECTION, "");

    tt_sshmsg_uar_set_service(msg, TT_SSH_SERVICE_USERAUTH);
    TT_TEST_CHECK_EQUAL(uar->service, TT_SSH_SERVICE_USERAUTH, "");

    ret = tt_sshmsg_render(msg, 0, NULL);
    TT_TEST_CHECK_NOT_EQUAL(ret, TT_SUCCESS, "");

    // auth
    tt_sshmsg_uar_set_auth(msg, TT_SSH_AUTH_PASSWORD);
    TT_TEST_CHECK_EQUAL(uar->auth, TT_SSH_AUTH_PASSWORD, "");

    tt_sshmsg_uar_set_auth(msg, TT_SSH_AUTH_PUBLICKEY);
    TT_TEST_CHECK_EQUAL(uar->auth, TT_SSH_AUTH_PUBLICKEY, "");

    ret = tt_sshmsg_render(msg, 0, NULL);
    TT_TEST_CHECK_NOT_EQUAL(ret, TT_SUCCESS, "");

    // can not set pwd...
    ret = tt_sshmsg_uar_set_pwd(msg, "pwd");
    TT_TEST_CHECK_NOT_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_sshmsg_uar_set_newpwd(msg, "new pwd");
    TT_TEST_CHECK_NOT_EQUAL(ret, TT_SUCCESS, "");

    // pkey alg
    ret = tt_sshmsg_uar_set_pubkey_alg(msg, TT_SSH_PUBKEY_ALG_RSA);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_EQUAL(uar->auth_u.pubkey.pubkey_alg,
                        TT_SSH_PUBKEY_ALG_RSA,
                        "");
    ret = tt_sshmsg_render(msg, 0, NULL);
    TT_TEST_CHECK_NOT_EQUAL(ret, TT_SUCCESS, "");

    // pkey
    ret = tt_sshmsg_uar_set_pubkey(msg, pkey, sizeof(pkey));
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_EQUAL(uar->auth_u.pubkey.pubkey.len, sizeof(pkey), "");
    TT_TEST_CHECK_EQUAL(tt_memcmp(uar->auth_u.pubkey.pubkey.addr,
                                  pkey,
                                  sizeof(pkey)),
                        0,
                        "");

    // sig
    ret = tt_sshmsg_uar_set_signature(msg, sig, sizeof(sig));
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_EQUAL(uar->auth_u.pubkey.signature.len, sizeof(sig), "");
    TT_TEST_CHECK_EQUAL(tt_memcmp(uar->auth_u.pubkey.signature.addr,
                                  sig,
                                  sizeof(sig)),
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

    uar = TT_SSHMSG_CAST(msg, tt_sshmsg_uar_t);

    TT_TEST_CHECK_EQUAL(uar->user.addr, NULL, "");
    TT_TEST_CHECK_EQUAL(uar->user.len, 0, "");
    TT_TEST_CHECK_EQUAL(uar->service, TT_SSH_SERVICE_USERAUTH, "");
    TT_TEST_CHECK_EQUAL(uar->auth, TT_SSH_AUTH_PUBLICKEY, "");
    TT_TEST_CHECK_EQUAL(uar->auth_u.pubkey.pubkey_alg,
                        TT_SSH_PUBKEY_ALG_RSA,
                        "");
    TT_TEST_CHECK_EQUAL(uar->auth_u.pubkey.pubkey.len, sizeof(pkey), "");
    TT_TEST_CHECK_EQUAL(tt_memcmp(uar->auth_u.pubkey.pubkey.addr,
                                  pkey,
                                  sizeof(pkey)),
                        0,
                        "");
    TT_TEST_CHECK_EQUAL(uar->auth_u.pubkey.signature.len, sizeof(sig), "");
    TT_TEST_CHECK_EQUAL(tt_memcmp(uar->auth_u.pubkey.signature.addr,
                                  sig,
                                  sizeof(sig)),
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

TT_TEST_ROUTINE_DEFINE(tt_unit_test_sshmsg_usrauthreq_pwd)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_sshmsg_t *msg, *out_msg;
    tt_sshmsg_uar_t *uar;
    tt_result_t ret;
    tt_u32_t i, rp, wp;

    TT_TEST_CASE_ENTER()
    // test start

    msg = tt_sshmsg_uar_create();
    uar = TT_SSHMSG_CAST(msg, tt_sshmsg_uar_t);
    TT_TEST_CHECK_NOT_EQUAL(msg, NULL, "");
    ret = tt_sshmsg_render(msg, 0, NULL);
    TT_TEST_CHECK_NOT_EQUAL(ret, TT_SUCCESS, "");

    // user
    ret = tt_sshmsg_uar_set_user(msg, "test");
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(uar->user.len, 4, "");
    TT_TEST_CHECK_EQUAL(tt_memcmp(uar->user.addr, "test", 4), 0, "");

    ret = tt_sshmsg_uar_set_user(msg, "helloworld");
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(uar->user.len, sizeof("helloworld") - 1, "");
    TT_TEST_CHECK_EQUAL(tt_memcmp(uar->user.addr, "helloworld", uar->user.len),
                        0,
                        "");

    ret = tt_sshmsg_render(msg, 0, NULL);
    TT_TEST_CHECK_NOT_EQUAL(ret, TT_SUCCESS, "");

    // service
    tt_sshmsg_uar_set_service(msg, TT_SSH_SERVICE_CONNECTION);
    TT_TEST_CHECK_EQUAL(uar->service, TT_SSH_SERVICE_CONNECTION, "");

    tt_sshmsg_uar_set_service(msg, TT_SSH_SERVICE_USERAUTH);
    TT_TEST_CHECK_EQUAL(uar->service, TT_SSH_SERVICE_USERAUTH, "");

    ret = tt_sshmsg_render(msg, 0, NULL);
    TT_TEST_CHECK_NOT_EQUAL(ret, TT_SUCCESS, "");

    // auth
    tt_sshmsg_uar_set_auth(msg, TT_SSH_AUTH_PUBLICKEY);
    TT_TEST_CHECK_EQUAL(uar->auth, TT_SSH_AUTH_PUBLICKEY, "");

    tt_sshmsg_uar_set_auth(msg, TT_SSH_AUTH_PASSWORD);
    TT_TEST_CHECK_EQUAL(uar->auth, TT_SSH_AUTH_PASSWORD, "");

    ret = tt_sshmsg_render(msg, 0, NULL);
    TT_TEST_CHECK_NOT_EQUAL(ret, TT_SUCCESS, "");

    // can not set pkey
    ret = tt_sshmsg_uar_set_pubkey_alg(msg, TT_SSH_PUBKEY_ALG_RSA);
    TT_TEST_CHECK_NOT_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_sshmsg_uar_set_pubkey(msg, (tt_u8_t *)&ret, sizeof(ret));
    TT_TEST_CHECK_NOT_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_sshmsg_uar_set_signature(msg, (tt_u8_t *)&ret, sizeof(ret));
    TT_TEST_CHECK_NOT_EQUAL(ret, TT_SUCCESS, "");

    // set pwd and new pwd
    ret = tt_sshmsg_uar_set_pwd(msg, "pwd");
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(uar->auth_u.pwd.pwd.len, 3, "");
    TT_TEST_CHECK_EQUAL(tt_memcmp(uar->auth_u.pwd.pwd.addr, "pwd", 3), 0, "");

    ret = tt_sshmsg_uar_set_newpwd(msg, "new pwd");
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(uar->auth_u.pwd.new_pwd.len, 7, "");
    TT_TEST_CHECK_EQUAL(tt_memcmp(uar->auth_u.pwd.new_pwd.addr, "new pwd", 7),
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

    uar = TT_SSHMSG_CAST(msg, tt_sshmsg_uar_t);

    TT_TEST_CHECK_EQUAL(uar->user.len, sizeof("helloworld") - 1, "");
    TT_TEST_CHECK_EQUAL(tt_memcmp(uar->user.addr,
                                  "helloworld",
                                  sizeof("helloworld") - 1),
                        0,
                        "");
    TT_TEST_CHECK_EQUAL(uar->service, TT_SSH_SERVICE_USERAUTH, "");
    TT_TEST_CHECK_EQUAL(uar->auth, TT_SSH_AUTH_PASSWORD, "");
    TT_TEST_CHECK_EQUAL(uar->auth_u.pwd.pwd.len, 3, "");
    TT_TEST_CHECK_EQUAL(tt_memcmp(uar->auth_u.pwd.pwd.addr, "pwd", 3), 0, "");
    TT_TEST_CHECK_EQUAL(uar->auth_u.pwd.new_pwd.len, 7, "");
    TT_TEST_CHECK_EQUAL(tt_memcmp(uar->auth_u.pwd.new_pwd.addr, "new pwd", 7),
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

TT_TEST_ROUTINE_DEFINE(tt_unit_test_sshmsg_usrauth_success)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_sshmsg_t *msg, *out_msg;
    tt_result_t ret;
    tt_u32_t i;
    tt_u32_t rp, wp;

    TT_TEST_CASE_ENTER()
    // test start

    msg = tt_sshmsg_uas_create();
    TT_TEST_CHECK_NOT_EQUAL(msg, NULL, "");
    TT_TEST_CHECK_EQUAL(msg->msg_id, TT_SSH_MSGID_USERAUTH_SUCCESS, "");

    ret = tt_sshmsg_render(msg, 0, NULL);
    TT_TEST_CHECK_SUCCESS(ret, "");

    tt_buf_backup_rwp(&msg->buf, &rp, &wp);
    ret = tt_sshmsg_parse(&msg->buf, &out_msg);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_EQUAL(msg->msg_id, TT_SSH_MSGID_USERAUTH_SUCCESS, "");
    tt_buf_restore_rwp(&msg->buf, &rp, &wp);

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

TT_TEST_ROUTINE_DEFINE(tt_unit_test_sshmsg_usrauth_fail)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_sshmsg_t *msg, *out_msg;
    tt_result_t ret;
    tt_u32_t i;
    tt_u32_t rp, wp;
    tt_sshmsg_uaf_t *uaf;

    TT_TEST_CASE_ENTER()
    // test start

    msg = tt_sshmsg_uaf_create();
    TT_TEST_CHECK_NOT_EQUAL(msg, NULL, "");
    TT_TEST_CHECK_EQUAL(msg->msg_id, TT_SSH_MSGID_USERAUTH_FAILURE, "");

    tt_sshmsg_uaf_add_auth(msg, TT_SSH_AUTH_PASSWORD);
    tt_sshmsg_uaf_add_auth(msg, TT_SSH_AUTH_PASSWORD);
    tt_sshmsg_uaf_add_auth(msg, TT_SSH_AUTH_PUBLICKEY);
    tt_sshmsg_uaf_add_auth(msg, TT_SSH_AUTH_PUBLICKEY);
    tt_sshmsg_uaf_add_auth(msg, TT_SSH_AUTH_HOSTBASED);
    tt_sshmsg_uaf_add_auth(msg, TT_SSH_AUTH_NONE);

    tt_sshmsg_uaf_set_parial_succ(msg, TT_TRUE);

    ret = tt_sshmsg_render(msg, 0, NULL);
    TT_TEST_CHECK_SUCCESS(ret, "");

    tt_buf_backup_rwp(&msg->buf, &rp, &wp);
    ret = tt_sshmsg_parse(&msg->buf, &out_msg);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_EQUAL(msg->msg_id, TT_SSH_MSGID_USERAUTH_FAILURE, "");
    tt_buf_restore_rwp(&msg->buf, &rp, &wp);

    uaf = TT_SSHMSG_CAST(msg, tt_sshmsg_uaf_t);
    TT_TEST_CHECK_EQUAL(uaf->auth_num, 4, "");
    TT_TEST_CHECK_EQUAL(uaf->auth[0], TT_SSH_AUTH_PASSWORD, "");
    TT_TEST_CHECK_EQUAL(uaf->auth[1], TT_SSH_AUTH_PUBLICKEY, "");
    TT_TEST_CHECK_EQUAL(uaf->auth[2], TT_SSH_AUTH_HOSTBASED, "");
    TT_TEST_CHECK_EQUAL(uaf->auth[3], TT_SSH_AUTH_NONE, "");
    TT_TEST_CHECK_EQUAL(uaf->partial_success, TT_TRUE, "");

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

TT_TEST_ROUTINE_DEFINE(tt_unit_test_sshmsg_usrauth_banner)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_sshmsg_t *msg, *out_msg;
    tt_result_t ret;
    tt_u32_t i;
    tt_u32_t rp, wp;
    tt_sshmsg_uab_t *uab;

    TT_TEST_CASE_ENTER()
    // test start

    msg = tt_sshmsg_uab_create();
    TT_TEST_CHECK_NOT_EQUAL(msg, NULL, "");
    TT_TEST_CHECK_EQUAL(msg->msg_id, TT_SSH_MSGID_USERAUTH_BANNER, "");

    ret = tt_sshmsg_uab_set_banner(msg, "ban!!!!");
    TT_TEST_CHECK_SUCCESS(ret, "");

    ret = tt_sshmsg_uab_set_banner(msg, "banggggg");
    TT_TEST_CHECK_SUCCESS(ret, "");

    ret = tt_sshmsg_render(msg, 0, NULL);
    TT_TEST_CHECK_SUCCESS(ret, "");

    tt_buf_backup_rwp(&msg->buf, &rp, &wp);
    ret = tt_sshmsg_parse(&msg->buf, &out_msg);
    TT_TEST_CHECK_SUCCESS(ret, "");
    TT_TEST_CHECK_EQUAL(msg->msg_id, TT_SSH_MSGID_USERAUTH_BANNER, "");
    tt_buf_restore_rwp(&msg->buf, &rp, &wp);

    uab = TT_SSHMSG_CAST(msg, tt_sshmsg_uab_t);
    TT_TEST_CHECK_EQUAL(TT_BUF_RLEN(&uab->banner),
                        (tt_u32_t)sizeof("banggggg") - 1,
                        "");
    TT_TEST_CHECK_EQUAL(tt_memcmp(TT_BUF_RPOS(&uab->banner),
                                  "banggggg",
                                  (tt_u32_t)sizeof("banggggg") - 1),
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
