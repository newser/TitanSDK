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
TT_TEST_ROUTINE_DECLARE(case_sshkex_basic)
TT_TEST_ROUTINE_DECLARE(case_sshkex_set)
TT_TEST_ROUTINE_DECLARE(case_sshkex_dh)

TT_TEST_ROUTINE_DECLARE(case_sshpubk)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(sshctx_case)

TT_TEST_CASE("case_sshkex_basic", "ssh context kex basic", case_sshkex_basic,
             NULL, NULL, NULL, NULL, NULL)
,

    TT_TEST_CASE("case_sshkex_set", "ssh context kex set values",
                 case_sshkex_set, NULL, NULL, NULL, NULL, NULL),

    TT_TEST_CASE("case_sshkex_dh", "ssh context kex DH", case_sshkex_dh, NULL,
                 NULL, NULL, NULL, NULL),

    TT_TEST_CASE("case_sshpubk", "ssh context public key", case_sshpubk, NULL,
                 NULL, NULL, NULL, NULL),

    TT_TEST_CASE_LIST_DEFINE_END(sshctx_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(SSH_CTX, 0, sshctx_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(case_sshpubk)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(case_sshkex_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    tt_sshctx_t ctx;
    tt_sshctx_attr_t attr;
    tt_u8_t tmp[100];
    tt_blob_t blob = {tmp, sizeof(tmp)};

    TT_TEST_CASE_ENTER()
    // test start

    attr.is_server = TT_TRUE;

    ret = tt_sshctx_create(&ctx, &attr);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    // can not create kex as is created in tt_sshctx_create()
    ret = tt_sshctx_kex_create(&ctx);
    TT_UT_EQUAL(ret, TT_FAIL, "");

    tt_sshctx_kex_destroy(&ctx);
    TT_UT_EQUAL(ctx.kex, NULL, "");

    // can not set anything as kex is destroyed
    ret = tt_sshctx_kex_setalg(&ctx, TT_SSH_KEX_ALG_DH_G14_SHA1);
    TT_UT_EQUAL(ret, TT_FAIL, "");
    ret = tt_sshctx_kex_setks(&ctx, tmp, 10, TT_FALSE);
    TT_UT_EQUAL(ret, TT_FAIL, "");
    ret = tt_sshctx_kex_setks_rsa(&ctx, &blob, &blob);
    TT_UT_EQUAL(ret, TT_FAIL, "");
    ret = tt_sshctx_kex_setks_rsa(&ctx, &blob, &blob);
    TT_UT_EQUAL(ret, TT_FAIL, "");
    ret = tt_sshctx_kex_setvc(&ctx, tmp, 10, TT_FALSE);
    TT_UT_EQUAL(ret, TT_FAIL, "");
    ret = tt_sshctx_kex_setvs(&ctx, tmp, 10, TT_FALSE);
    TT_UT_EQUAL(ret, TT_FAIL, "");
    ret = tt_sshctx_kex_setic(&ctx, tmp, 10, TT_FALSE);
    TT_UT_EQUAL(ret, TT_FAIL, "");
    ret = tt_sshctx_kex_setis(&ctx, tmp, 10, TT_FALSE);
    TT_UT_EQUAL(ret, TT_FAIL, "");
    ret = tt_sshctx_kex_calc_h(&ctx);
    TT_UT_EQUAL(ret, TT_FAIL, "");
    ret = tt_sshctx_load_session_id(&ctx);
    TT_UT_EQUAL(ret, TT_FAIL, "");

    // create kex again
    ret = tt_sshctx_kex_create(&ctx);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    // not supported alg
    ret = tt_sshctx_kex_setalg(&ctx, TT_SSH_KEX_ALG_RSA1024_SHA1);
    TT_UT_EQUAL(ret, TT_FAIL, "");
    ret = tt_sshctx_kex_setalg(&ctx, TT_SSH_KEX_ALG_RSA2048_SHA256);
    TT_UT_EQUAL(ret, TT_FAIL, "");

    ret = tt_sshctx_kex_setalg(&ctx, TT_SSH_KEX_ALG_DH_G14_SHA1);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(ctx.kex->alg, TT_SSH_KEX_ALG_DH_G14_SHA1, "");
    // can not setalg again
    ret = tt_sshctx_kex_setalg(&ctx, TT_SSH_KEX_ALG_DH_G1_SHA1);
    TT_UT_EQUAL(ret, TT_FAIL, "");

    tt_sshctx_destroy(&ctx);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_sshkex_set)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    tt_sshctx_t ctx;
    tt_sshkex_t *kex;
    tt_sshctx_attr_t attr;
    tt_u8_t tmp[1000];
    tt_blob_t blob = {tmp, sizeof(tmp)};
    tt_u32_t i, len;
    tt_buf_t buf;

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&buf, NULL);

    attr.is_server = TT_TRUE;

    ret = tt_sshctx_create(&ctx, &attr);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    kex = ctx.kex;

    for (i = 0; i < sizeof(tmp); ++i) { tmp[i] = (tt_u8_t)tt_rand_u32(); }

    // ks
    len = tt_rand_u32() % (sizeof(tmp) - 1) + 1;
    ret = tt_sshctx_kex_setks(&ctx, tmp, len, TT_FALSE);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(TT_BUF_RLEN(&kex->k_s), len, "");
    TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(&kex->k_s), tmp, len), 0, "");

    ret = tt_sshctx_kex_setks(&ctx, tmp, len, TT_TRUE);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    tt_buf_reset_rwp(&buf);
    tt_ssh_string_render(&buf, tmp, len);
    TT_UT_EQUAL(TT_BUF_RLEN(&kex->k_s), TT_BUF_RLEN(&buf), "");
    TT_UT_EQUAL(tt_buf_cmp(&buf, &kex->k_s), 0, "");

    // vc
    len = tt_rand_u32() % (sizeof(tmp) - 1) + 1;
    ret = tt_sshctx_kex_setvc(&ctx, tmp, len, TT_FALSE);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(TT_BUF_RLEN(&kex->v_c), len, "");
    TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(&kex->v_c), tmp, len), 0, "");

    ret = tt_sshctx_kex_setvc(&ctx, tmp, len, TT_TRUE);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    tt_buf_reset_rwp(&buf);
    tt_ssh_string_render(&buf, tmp, len);
    TT_UT_EQUAL(TT_BUF_RLEN(&kex->v_c), TT_BUF_RLEN(&buf), "");
    TT_UT_EQUAL(tt_buf_cmp(&buf, &kex->v_c), 0, "");

    // vs
    len = tt_rand_u32() % (sizeof(tmp) - 1) + 1;
    ret = tt_sshctx_kex_setvs(&ctx, tmp, len, TT_FALSE);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(TT_BUF_RLEN(&kex->v_s), len, "");
    TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(&kex->v_s), tmp, len), 0, "");

    ret = tt_sshctx_kex_setvs(&ctx, tmp, len, TT_TRUE);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    tt_buf_reset_rwp(&buf);
    tt_ssh_string_render(&buf, tmp, len);
    TT_UT_EQUAL(TT_BUF_RLEN(&kex->v_s), TT_BUF_RLEN(&buf), "");
    TT_UT_EQUAL(tt_buf_cmp(&buf, &kex->v_s), 0, "");

    // ic
    len = tt_rand_u32() % (sizeof(tmp) - 1) + 1;
    ret = tt_sshctx_kex_setic(&ctx, tmp, len, TT_FALSE);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(TT_BUF_RLEN(&kex->i_c), len, "");
    TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(&kex->i_c), tmp, len), 0, "");

    ret = tt_sshctx_kex_setic(&ctx, tmp, len, TT_TRUE);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    tt_buf_reset_rwp(&buf);
    tt_ssh_string_render(&buf, tmp, len);
    TT_UT_EQUAL(TT_BUF_RLEN(&kex->i_c), TT_BUF_RLEN(&buf), "");
    TT_UT_EQUAL(tt_buf_cmp(&buf, &kex->i_c), 0, "");

    // is
    len = tt_rand_u32() % (sizeof(tmp) - 1) + 1;
    ret = tt_sshctx_kex_setis(&ctx, tmp, len, TT_FALSE);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(TT_BUF_RLEN(&kex->i_s), len, "");
    TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(&kex->i_s), tmp, len), 0, "");

    ret = tt_sshctx_kex_setis(&ctx, tmp, len, TT_TRUE);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    tt_buf_reset_rwp(&buf);
    tt_ssh_string_render(&buf, tmp, len);
    TT_UT_EQUAL(TT_BUF_RLEN(&kex->i_s), TT_BUF_RLEN(&buf), "");
    TT_UT_EQUAL(tt_buf_cmp(&buf, &kex->i_s), 0, "");

    // not able to calc h, as k,e,f is not set
    ret = tt_sshctx_kex_calc_h(&ctx);
    TT_UT_EQUAL(ret, TT_FAIL, "");

    // no h
    ret = tt_sshctx_load_session_id(&ctx);
    TT_UT_EQUAL(ret, TT_FAIL, "");

    tt_sshctx_destroy(&ctx);
    tt_buf_destroy(&buf);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_sshkex_dh)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    tt_sshctx_t ctx;
    tt_sshctx_attr_t attr;
    tt_u8_t tmp[1000];
    tt_blob_t blob = {tmp, sizeof(tmp)};
    tt_u32_t i, len;
    tt_buf_t buf, *pbuf;

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&buf, NULL);

    attr.is_server = TT_TRUE;

    ret = tt_sshctx_create(&ctx, &attr);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    for (i = 0; i < sizeof(tmp); ++i) { tmp[i] = (tt_u8_t)tt_rand_u32(); }

    tt_sshctx_kex_destroy(&ctx);

    // before kex create, all would fail
    ret = tt_sshctx_kexdh_compute(&ctx, tmp, 100);
    TT_UT_EQUAL(ret, TT_FAIL, "");
    ret = tt_sshctx_kexdh_set_e(&ctx, tmp, 100, TT_TRUE);
    TT_UT_EQUAL(ret, TT_FAIL, "");
    ret = tt_sshctx_kexdh_load_e(&ctx);
    TT_UT_EQUAL(ret, TT_FAIL, "");
    ret = tt_sshctx_kexdh_set_f(&ctx, tmp, 100, TT_TRUE);
    TT_UT_EQUAL(ret, TT_FAIL, "");
    ret = tt_sshctx_kexdh_load_f(&ctx);
    TT_UT_EQUAL(ret, TT_FAIL, "");
    ret = tt_sshctx_kexdh_get_k(&ctx);
    TT_UT_EQUAL(ret, TT_FAIL, "");

    ret = tt_sshctx_kex_create(&ctx);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    // before set alg, all would fail
    ret = tt_sshctx_kexdh_compute(&ctx, tmp, 100);
    TT_UT_EQUAL(ret, TT_FAIL, "");
    ret = tt_sshctx_kexdh_set_e(&ctx, tmp, 100, TT_TRUE);
    TT_UT_EQUAL(ret, TT_FAIL, "");
    ret = tt_sshctx_kexdh_load_e(&ctx);
    TT_UT_EQUAL(ret, TT_FAIL, "");
    ret = tt_sshctx_kexdh_set_f(&ctx, tmp, 100, TT_TRUE);
    TT_UT_EQUAL(ret, TT_FAIL, "");
    ret = tt_sshctx_kexdh_load_f(&ctx);
    TT_UT_EQUAL(ret, TT_FAIL, "");
    ret = tt_sshctx_kexdh_get_k(&ctx);
    TT_UT_EQUAL(ret, TT_FAIL, "");

    ret = tt_sshctx_kex_setalg(&ctx, TT_SSH_KEX_ALG_DH_G1_SHA1);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    // ks
    len = tt_rand_u32() % (sizeof(tmp) - 1) + 1;
    ret = tt_sshctx_kex_setks(&ctx, tmp, len, TT_FALSE);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    // vc
    len = tt_rand_u32() % (sizeof(tmp) - 1) + 1;
    ret = tt_sshctx_kex_setvc(&ctx, tmp, len, TT_FALSE);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    // vs
    len = tt_rand_u32() % (sizeof(tmp) - 1) + 1;
    ret = tt_sshctx_kex_setvs(&ctx, tmp, len, TT_FALSE);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    // ic
    len = tt_rand_u32() % (sizeof(tmp) - 1) + 1;
    ret = tt_sshctx_kex_setic(&ctx, tmp, len, TT_FALSE);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    // is
    len = tt_rand_u32() % (sizeof(tmp) - 1) + 1;
    ret = tt_sshctx_kex_setis(&ctx, tmp, len, TT_FALSE);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    // as a client

    // load e
    ret = tt_sshctx_kexdh_load_e(&ctx);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    pbuf = &ctx.kex->alg_u.kexdh.e;
    TT_UT_NOT_EQUAL(pbuf->p, NULL, "");
    TT_UT_NOT_EQUAL(TT_BUF_RLEN(pbuf), 0, "");

    // set f
    len = tt_rand_u32() % (sizeof(tmp) - 1) + 1;
    ret = tt_sshctx_kexdh_set_f(&ctx, tmp, len, TT_FALSE);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    pbuf = &ctx.kex->alg_u.kexdh.f;
    TT_UT_EQUAL(TT_BUF_RLEN(pbuf), len, "");
    TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(pbuf), tmp, len), 0, "");

    ret = tt_sshctx_kexdh_set_f(&ctx, tmp, len, TT_TRUE);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    tt_buf_reset_rwp(&buf);
    tt_ssh_mpint_render(&buf, tmp, len, TT_FALSE);
    TT_UT_EQUAL(TT_BUF_RLEN(pbuf), TT_BUF_RLEN(&buf), "");
    TT_UT_EQUAL(tt_buf_cmp(&buf, pbuf), 0, "");

    ret = tt_sshctx_kexdh_compute(&ctx, tmp, len);
    if (TT_OK(ret)) {
        ret = tt_sshctx_kexdh_get_k(&ctx);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");

        ret = tt_sshctx_kex_calc_h(&ctx);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_NOT_EQUAL(ctx.kex->h.p, NULL, "");
        TT_UT_NOT_EQUAL(TT_BUF_RLEN(&ctx.kex->h), 0, "");

        ret = tt_sshctx_load_session_id(&ctx);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");

        // can not set again
        ret = tt_sshctx_load_session_id(&ctx);
        TT_UT_EQUAL(ret, TT_FAIL, "");
    }

    // as a server

    // load f
    ret = tt_sshctx_kexdh_load_f(&ctx);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    pbuf = &ctx.kex->alg_u.kexdh.f;
    TT_UT_NOT_EQUAL(pbuf->p, NULL, "");
    TT_UT_NOT_EQUAL(TT_BUF_RLEN(pbuf), 0, "");

    // set e
    len = tt_rand_u32() % (sizeof(tmp) - 1) + 1;
    ret = tt_sshctx_kexdh_set_e(&ctx, tmp, len, TT_FALSE);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    pbuf = &ctx.kex->alg_u.kexdh.e;
    TT_UT_EQUAL(TT_BUF_RLEN(pbuf), len, "");
    TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(pbuf), tmp, len), 0, "");

    ret = tt_sshctx_kexdh_set_e(&ctx, tmp, len, TT_TRUE);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    tt_buf_reset_rwp(&buf);
    tt_ssh_mpint_render(&buf, tmp, len, TT_FALSE);
    TT_UT_EQUAL(TT_BUF_RLEN(pbuf), TT_BUF_RLEN(&buf), "");
    TT_UT_EQUAL(tt_buf_cmp(&buf, pbuf), 0, "");

    ret = tt_sshctx_kexdh_compute(&ctx, tmp, len);
    if (TT_OK(ret)) {
        ret = tt_sshctx_kexdh_get_k(&ctx);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");

        ret = tt_sshctx_kex_calc_h(&ctx);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_NOT_EQUAL(ctx.kex->h.p, NULL, "");
        TT_UT_NOT_EQUAL(TT_BUF_RLEN(&ctx.kex->h), 0, "");

        ret = tt_sshctx_load_session_id(&ctx);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");

        // can not set again
        ret = tt_sshctx_load_session_id(&ctx);
        TT_UT_EQUAL(ret, TT_FAIL, "");
    }

    tt_sshctx_destroy(&ctx);
    tt_buf_destroy(&buf);

    // test end
    TT_TEST_CASE_LEAVE()
}

static const tt_char_t *__ssh_rsapriv =
    "-----BEGIN ENCRYPTED PRIVATE KEY-----\n"
    "MIIFHzBJBgkqhkiG9w0BBQ0wPDAbBgkqhkiG9w0BBQwwDgQIdE8ghPaeRnMCAggA\n"
    "MB0GCWCGSAFlAwQBAgQQCxSeXB5CDIBYtcjTWcDDWwSCBNAbs09d0YEkiUXCrKMu\n"
    "Br5ZKZxlMPjUQzpM1mjuYG79TIoZfqXioitntGx9YPghln/iqIUoy/ZbWmz1E+Wg\n"
    "6/xGY4+Ei8dyvmIHbgcv10SJbdoSU10ZMKLfBdnjwuZCZC7NMsMhpGhv8/ZhWMbG\n"
    "eVUQfq1lcYp4R3zU00AF6yKtYiXflyDcsq4cb9ywlkLtlQ7nEj3dP0jn+/FXHCl4\n"
    "f2FRQQ89jKTlHArkFqgkYOi0HNyqisNY6t+2Dy9QeqLyd/kNvZWbURtMLEQ3+K+2\n"
    "q5GjBjYswcbBrHjxCzSSkUpuULNYfk6FVMcsYWKh3ju4kBOTR5auxRL1lHdJU1/Q\n"
    "h+LI2NmFoF/WyLvdEbYW6mX5/j5etbZrHNbM+ToQLZF8e5OurVRmFEjk0JymlN2B\n"
    "c0npIaZiAT2dgRDgeUky6+vDs667vbsqu+IlrtwlrgPeuBQcwH4NmevhMBLEuzU2\n"
    "OxYyNDIp752N5abWx9clk6kXfAAhIYoE48uy32Fzw5u1MHbYkUt7lRnUjYba4kA8\n"
    "jDrg6mjHugd9iq6MIkPVLhZ5VzCiVK+7l3Afhqzd0nrbqdgDv3+G/tkqwZ+17BIp\n"
    "8pGL1Ms2FuUwk9V56dTi89vKQWDgorYchXcmsBXthJI05qhoZNxbXQXnqpeX6vx6\n"
    "1Z7Za40/3Qkh8rosaXUpMiQGw2whjXr8R1h9O5cKPnDH2Nho/jEAEYZb+nKEqn7k\n"
    "JbV/Vo+xQT/ipCozu+edWJjnuY7chiDP1k9IKIhz/9pFfzmb/iGy64p0EQ4/Gh6v\n"
    "SDXHJhRaU0rjouJwZMui3642b9W/qLbazh/cudxPYtFptCuE2y4YLq7epusQdprl\n"
    "O8g8PW88KV7S6BmmbWViFW1pQ6GBycd5MZvtWFtBQdNZ0F24xy25ycwNjXFcg2cq\n"
    "rmtYQmIWxAhNQ4xw+h6THLsz0fN86rbDG9BrSXk7UTOXJ+5avvydPLSLZP582p+p\n"
    "qEAuJBRGqCuwt8hNw/WxvVy8JFtQqwKKFQOJg0KUIr3X5nm3h3wPp/tmgJlJg8+D\n"
    "bAKSGqiMoLcc8OEu7SEFgWqLh31F5YHR4WcHjGly9cWPptXUeEcYbLyOQ3ihiVxm\n"
    "rcrOYX7RQ+hPS3QnXLmee4knVTEiP2MDENj0bQ1ArRZBCBr000nzkWuxwnMbqICz\n"
    "R7KLbLPdSZ6S29XaaF5qrX4WQkLndOzD6/iBHHeR+jVoo+tAHQdsuLYSBRysoCxT\n"
    "Qa54P4pixcmEH8KVueM9uy27PdMJVlHgHjKoveV7psbdouTpOc91bFCSvcLrGWC+\n"
    "zzUJEaVox1JIuJ0fEf7RYpLzGcKHnEPneFygYX6vZfSq6b1XtWuK1ovXSopoen9m\n"
    "dPAKU1k4lZqF+WPp9co7aYwnOm+xJWH16uHbfuVYlGM9VmJ9UQp1N2AlhQ75xF6+\n"
    "/zO/m2aSFcV/zHlw//rYVP2gPFYwp0I0lJ9GnSVFzcWIWL745HMlBhim50qbPhtU\n"
    "1d4RNL/MuFqiscyz0UVfzWU4zl+X57RveVhzoRQCH62UYn0SApscc/aGA9oWmhe2\n"
    "n+l10MjGIiu3fqTChDsJ3VecQ2g2erXHoeWaHWhCO/BwTPdOftImOw3WNkGyCWA1\n"
    "gALQDIvVfw4eXRP+HrmPoz0Vgw==\n"
    "-----END ENCRYPTED PRIVATE KEY-----\n";

static const tt_char_t *__ssh_rsapub =
    "-----BEGIN PUBLIC KEY-----\n"
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAvv66a0SGXEBqV1cwAmbg\n"
    "oCKfN4AM/jUJRaXxfZk8O3nBFT+LMu8Hou3GfB9B9iYIBUInm5sPF9a0+F6XUojX\n"
    "ErwyYhE0Nv2WK7HgcVTk1QQQQQEKAMBEjCXkxhV6Tqxr5xuy9/zDDZkcO2S1n7E3\n"
    "KGbh4CUzXfX+3maHeU5Bvus9Zf37tKq8kJM9AwVsoCjagqv7AVQpOu0wkjm29hCU\n"
    "J/IKg6liDeREdzka5ULzWH2z1/TwkwFh2f31UD26EvpgnCfrZExp5+D8yhVSgbeh\n"
    "xI9jvkRq9ROdiLlasx9zxZY20667IzQrhK3ABXx4C3qcMMgH4WiOLQiHsVfGKA7Z\n"
    "MQIDAQAB\n"
    "-----END PUBLIC KEY-----\n";

TT_TEST_ROUTINE_DEFINE(case_sshpubk)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    tt_sshctx_t ctx;
    tt_sshctx_attr_t attr;
    tt_u8_t tmp[1000];
    tt_rsa_t rsa, rsa_pub;
    tt_blob_t key_data;
    tt_rsa_attr_t rsa_attr;
    tt_u32_t i;

    TT_TEST_CASE_ENTER()
    // test start

    for (i = 0; i < sizeof(tmp); ++i) { tmp[i] = (tt_u8_t)tt_rand_u32(); }

    attr.is_server = TT_TRUE;

    ret = tt_sshctx_create(&ctx, &attr);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_sshctx_pubk_destroy(&ctx);

    // all would fail as there is no pubk created
    ret = tt_sshctx_pubk_setalg(&ctx, TT_SSH_PUBKEY_ALG_RSA, tmp);
    TT_UT_EQUAL(ret, TT_FAIL, "");
    ret = tt_sshctx_pubk_sign(&ctx, tmp, 100);
    TT_UT_EQUAL(ret, TT_FAIL, "");

    // rsa
    key_data.addr = (tt_u8_t *)__ssh_rsapriv;
    key_data.len = (tt_u32_t)tt_strlen(__ssh_rsapriv);

    tt_rsa_attr_default(&rsa_attr);
    rsa_attr.password = "abc123";
    rsa_attr.pem_armor = TT_TRUE;
    rsa_attr.padding.mode = TT_RSA_PADDING_PKCS1;

    ret = tt_rsa_create(&rsa, TT_RSA_FORMAT_PKCS8, TT_RSA_TYPE_PRIVATE,
                        &key_data, &rsa_attr);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    // rsa pub
    key_data.addr = (tt_u8_t *)__ssh_rsapub;
    key_data.len = (tt_u32_t)tt_strlen(__ssh_rsapub);

    tt_rsa_attr_default(&rsa_attr);
    rsa_attr.pem_armor = TT_TRUE;
    rsa_attr.padding.mode = TT_RSA_PADDING_PKCS1;

    ret = tt_rsa_create(&rsa_pub, TT_RSA_FORMAT_PKCS1, TT_RSA_TYPE_PUBLIC,
                        &key_data, &rsa_attr);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_sshctx_pubk_create(&ctx);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    // set rsa
    ret = tt_sshctx_pubk_setalg(&ctx, TT_SSH_PUBKEY_ALG_RSA, &rsa);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(ctx.pubk->alg, TT_SSH_PUBKEY_ALG_RSA, "");
    TT_UT_EQUAL(ctx.pubk->alg_u.rsa, &rsa, "");

    // sign
    ret = tt_sshctx_pubk_sign(&ctx, tmp, sizeof(tmp));
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_rsa_verify(&rsa_pub, tmp, sizeof(tmp),
                        TT_BUF_RPOS(&ctx.pubk->signature),
                        TT_BUF_RLEN(&ctx.pubk->signature));
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_rsa_destroy(&rsa);
    tt_rsa_destroy(&rsa_pub);
    tt_sshctx_destroy(&ctx);

    // test end
    TT_TEST_CASE_LEAVE()
}
