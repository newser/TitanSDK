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

#include <unit_test/tt_unit_test.h>

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
TT_TEST_ROUTINE_DECLARE(case_rbuf)
TT_TEST_ROUTINE_DECLARE(case_rbuf_stress)
TT_TEST_ROUTINE_DECLARE(case_rbuf_excep)
TT_TEST_ROUTINE_DECLARE(case_wbuf)
TT_TEST_ROUTINE_DECLARE(case_rwbuf)
TT_TEST_ROUTINE_DECLARE(case_rwbuf_raw)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(rbuf_case)
#if 1
TT_TEST_CASE(
    "case_rbuf", "testing read buffer", case_rbuf, NULL, NULL, NULL, NULL, NULL)
,

    TT_TEST_CASE("case_rbuf_stress",
                 "testing read buffer stress test",
                 case_rbuf_stress,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_rbuf_excep",
                 "testing read buffer exceptional test",
                 case_rbuf_excep,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_wbuf",
                 "testing write buffer",
                 case_wbuf,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_rwbuf",
                 "testing read write buffer",
                 case_rwbuf,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),
#endif

    TT_TEST_CASE("case_rwbuf_raw",
                 "testing read write buffer, no enc/dec",
                 case_rwbuf_raw,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(rbuf_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(ALG_UT_RBUF, 0, rbuf_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(case_rbuf)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    static void __gen_rbuf(tt_buf_t *data, tt_u8_t len, tt_u8_t b)
{
    tt_buf_put_u8(data, len); // data len
    tt_buf_put_u8(data, 0xaa); // magic
    tt_buf_put_rep(data, b, len); // data
    tt_buf_put_u8(data, 0xbb); // magic
}

static void __enc_rbuf(tt_buf_t *data, tt_buf_t *enc)
{
    tt_u32_t n = 0, len, s;
    tt_u8_t *p;

    tt_buf_get_rptr(data, &p, &len);
    while (n < len) {
        s = tt_rand_u32() % 10;
        if ((n + s) > len) {
            s = len - n;
        }
        tt_buf_put_u8(enc, s);
        tt_buf_put(enc, p + n, s);
        n += s;
    }
}

static tt_result_t __ut_dec_pre(IN tt_buf_t *raw,
                                OUT tt_u32_t *len,
                                IN void *param)
{
    tt_u8_t n;

    TT_ASSERT(param == (void *)1);

    TT_DO(tt_buf_get_u8(raw, &n));
    TT_DO(tt_buf_inc_rp(raw, n));

    *len = n + 1;
    return TT_SUCCESS;
}

static tt_result_t __ut_dec(IN tt_buf_t *raw,
                            IN tt_u32_t len,
                            OUT tt_buf_t *dec,
                            IN void *param)
{
    tt_u8_t n;

    TT_ASSERT(param == (void *)1);

    TT_DO(tt_buf_get_u8(raw, &n));
    TT_ASSERT(len == (n + 1));

    if (len != 1) {
        TT_DO(tt_buf_put(dec, TT_BUF_RPOS(raw), n));
        tt_buf_inc_rp(raw, n);
        return TT_SUCCESS;
    } else {
        return TT_E_PROCEED;
    }
}

static tt_result_t __ut_par_pre(IN tt_buf_t *buf,
                                OUT tt_u32_t *len,
                                IN void *param)
{
    tt_u8_t n;

    TT_ASSERT(param == (void *)2);

    TT_DO(tt_buf_get_u8(buf, &n));
    TT_DO(tt_buf_inc_rp(buf, n + 2));

    *len = n + 3;
    return TT_SUCCESS;
}

static tt_result_t __ut_par(IN tt_buf_t *buf,
                            IN tt_u32_t len,
                            OUT void **parse_ret,
                            IN void *param)
{
    tt_u8_t n, i, v = 0, t;
    tt_uintptr_t ret;

    TT_ASSERT(param == (void *)2);

    TT_DO(tt_buf_get_u8(buf, &n));
    TT_ASSERT(len == (n + 3));

    TT_DO(tt_buf_get_u8(buf, &t));
    if (t != 0xaa) {
        return TT_FAIL;
    }

    for (i = 0; i < n; ++i) {
        TT_DO(tt_buf_get_u8(buf, &t));
        if (i == 0) {
            v = t;
        } else if (t != v) {
            return TT_FAIL;
        }
    }

    TT_DO(tt_buf_get_u8(buf, &t));
    if (t != 0xbb) {
        return TT_FAIL;
    }

    if (v % 5 != 0) {
        ret = (tt_uintptr_t)v;
        *parse_ret = (void *)ret;
        return TT_SUCCESS;
    } else {
        return TT_E_PROCEED;
    }
}

static tt_result_t __ut_ret;
static tt_uintptr_t __ut_seq;

static void __ut_par_done(IN void *parse_ret, IN void *param)
{
    tt_uintptr_t v = (tt_uintptr_t)parse_ret;
    if (__ut_seq % 5 == 0) {
        ++__ut_seq;
    }

    if (v != __ut_seq) {
        __ut_ret = TT_FAIL;
    }
    ++__ut_seq;
}

TT_TEST_ROUTINE_DEFINE(case_rbuf)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_rbuf_t rbuf;
    tt_rbuf_decode_itf_t d_itf = {__ut_dec_pre, __ut_dec};
    tt_rbuf_parse_itf_t p_itf = {__ut_par_pre, __ut_par, __ut_par_done};
    tt_result_t ret;
    tt_u32_t i, len, n;
    tt_buf_t raw, enc;
    tt_u8_t *p;

    TT_TEST_CASE_ENTER()
    // test start

    tt_rbuf_init(&rbuf, &d_itf, (void *)1, &p_itf, (void *)2, NULL);
    tt_buf_init(&raw, NULL);
    tt_buf_init(&enc, NULL);

    for (i = 0; i < 100; ++i) {
        __gen_rbuf(&raw, i, i);
    }
    __enc_rbuf(&raw, &enc);

    ret = tt_rbuf_reserve(&rbuf, TT_BUF_RLEN(&enc));
    TT_UT_SUCCESS(ret, "");

    tt_rbuf_get_wptr(&rbuf, &p, &len);
    len = TT_BUF_RLEN(&enc);
    tt_memcpy(p, TT_BUF_RPOS(&enc), len);

    __ut_ret = TT_SUCCESS;
    __ut_seq = 0;

    ret = tt_rbuf_inc_wp(&rbuf, TT_BUF_RLEN(&enc));
    TT_UT_SUCCESS(ret, "");
    TT_UT_SUCCESS(__ut_ret, "");

    // again

    __ut_ret = TT_SUCCESS;
    __ut_seq = 0;
    len = TT_BUF_RLEN(&enc);
    i = 0;
    while (i < len) {
        tt_u32_t s = tt_rand_u32() % 10;
        if (i + s > len) {
            s = len - i;
        }

        ret = tt_rbuf_reserve(&rbuf, s);
        TT_UT_SUCCESS(ret, "");

        tt_rbuf_get_wptr(&rbuf, &p, &n);
        tt_memcpy(p, TT_BUF_RPOS(&enc) + i, s);
        ret = tt_rbuf_inc_wp(&rbuf, s);
        TT_UT_EXP(TT_OK(ret) || (ret == TT_E_BUF_NOBUFS), "");

        i += s;
    }
    TT_UT_SUCCESS(__ut_ret, "");

    tt_rbuf_destroy(&rbuf);
    tt_buf_destroy(&raw);
    tt_buf_destroy(&enc);

    // test end
    TT_TEST_CASE_LEAVE()
}

#ifdef __UT_LITE__
#define __ST_NUM 100
#else
#define __ST_NUM 1000
#endif

static void __ut_par_done2(IN void *parse_ret, IN void *param)
{
    tt_u32_t v = (tt_u32_t)(tt_uintptr_t)parse_ret;
    (void)v;
    // TT_INFO("parsed: %d", v);
}

TT_TEST_ROUTINE_DEFINE(case_rbuf_stress)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_rbuf_t rbuf;
    tt_rbuf_decode_itf_t d_itf = {__ut_dec_pre, __ut_dec};
    tt_rbuf_parse_itf_t p_itf = {__ut_par_pre, __ut_par, __ut_par_done2};
    tt_result_t ret;
    tt_u32_t i, len, n;
    tt_buf_t raw, enc;
    tt_u8_t *p;

    TT_TEST_CASE_ENTER()
    // test start

    tt_rbuf_init(&rbuf, &d_itf, (void *)1, &p_itf, (void *)2, NULL);
    tt_buf_init(&raw, NULL);
    tt_buf_init(&enc, NULL);

    for (i = 0; i < __ST_NUM; ++i) {
        tt_u32_t j, jn = tt_rand_u32() % 100 + 1, k;

        for (j = 0; j < jn; ++j) {
            __gen_rbuf(&raw, tt_rand_u32() % 100, (tt_u8_t)tt_rand_u32());
        }
        __enc_rbuf(&raw, &enc);
        // max len: 10000 bytes

        len = TT_BUF_RLEN(&enc);
        k = 0;
        while (k < len) {
            tt_u32_t s = tt_rand_u32() % 50;
            if (k + s > len) {
                s = len - k;
            }

            ret = tt_rbuf_reserve(&rbuf, s);
            TT_UT_SUCCESS(ret, "");

            tt_rbuf_get_wptr(&rbuf, &p, &n);
            TT_ASSERT(n >= s);
            tt_memcpy(p, TT_BUF_RPOS(&enc) + k, s);
            ret = tt_rbuf_inc_wp(&rbuf, s);
            TT_UT_EXP(TT_OK(ret) || (ret == TT_E_BUF_NOBUFS), "");

            k += s;
        }

        TT_UT_EQUAL(TT_BUF_RLEN(&rbuf.raw), 0, "");
        TT_UT_EQUAL(TT_BUF_RLEN(&rbuf.dec), 0, "");

        tt_buf_clear(&raw);
        tt_buf_clear(&enc);
    }

    tt_rbuf_destroy(&rbuf);
    tt_buf_destroy(&raw);
    tt_buf_destroy(&enc);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_rbuf_excep)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_rbuf_t rbuf;
    tt_rbuf_decode_itf_t d_itf = {__ut_dec_pre, __ut_dec};
    tt_rbuf_parse_itf_t p_itf = {__ut_par_pre, __ut_par, __ut_par_done2};
    tt_result_t ret;
    tt_u32_t i, len, n;
    tt_buf_t raw, enc;
    tt_u8_t *p;

    TT_TEST_CASE_ENTER()
    // test start

    tt_rbuf_init(&rbuf, &d_itf, (void *)1, &p_itf, (void *)2, NULL);
    tt_buf_init(&raw, NULL);
    tt_buf_init(&enc, NULL);

    for (i = 0; i < __ST_NUM; ++i) {
        tt_u32_t j, jn = tt_rand_u32() % 100 + 1, k;

        for (j = 0; j < jn; ++j) {
            __gen_rbuf(&raw, tt_rand_u32() % 100, (tt_u8_t)tt_rand_u32());
        }
        if (tt_rand_u32() % 2) {
            // 1: invalid data
            tt_buf_get_rptr(&raw, &p, &n);
            j = tt_rand_u32() % n;
            jn = tt_rand_u32() % (n - j);
            TT_ASSERT((j < n) && (j + jn <= n));
            for (k = j; k < j + jn; ++k) {
                p[k] = tt_rand_u32();
            }

            __enc_rbuf(&raw, &enc);
        } else {
            // 0: invalid  encoded data
            __enc_rbuf(&raw, &enc);

            tt_buf_get_rptr(&enc, &p, &n);
            j = tt_rand_u32() % n;
            jn = tt_rand_u32() % (n - j);
            TT_ASSERT((j < n) && (j + jn <= n));
            for (k = j; k < j + jn; ++k) {
                p[k] = tt_rand_u32();
            }
        }
        // max len: 10000 bytes

        len = TT_BUF_RLEN(&enc);
        k = 0;
        while (k < len) {
            tt_u32_t s = tt_rand_u32() % 50;
            if (k + s > len) {
                s = len - k;
            }

            ret = tt_rbuf_reserve(&rbuf, s);
            TT_UT_SUCCESS(ret, "");

            tt_rbuf_get_wptr(&rbuf, &p, &n);
            TT_ASSERT(n >= s);
            tt_memcpy(p, TT_BUF_RPOS(&enc) + k, s);
            tt_rbuf_inc_wp(&rbuf, s);

            k += s;
        }

        tt_rbuf_clear(&rbuf);
        tt_buf_clear(&raw);
        tt_buf_clear(&enc);
    }

    tt_rbuf_destroy(&rbuf);
    tt_buf_destroy(&raw);
    tt_buf_destroy(&enc);

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_u32_t __ut_rdr_pre(IN void *to_render, IN void *param)
{
    tt_u32_t v = (tt_u32_t)(tt_uintptr_t)to_render;
    TT_ASSERT(v <= 0xff);
    TT_ASSERT(param == (void *)0x4);
    return v + 3; // len, two magic
}

static tt_result_t __ut_rdr(IN tt_buf_t *buf,
                            IN tt_u32_t len,
                            IN void *to_render,
                            IN void *param)
{
    tt_u8_t v = (tt_u8_t)(tt_uintptr_t)to_render;

    TT_ASSERT(param == (void *)0x4);

    TT_ASSERT(len == (v + 3));
    tt_buf_put_u8(buf, v); // data len
    tt_buf_put_u8(buf, 0xaa); // magic
    tt_buf_put_rep(buf, v, v); // data
    tt_buf_put_u8(buf, 0xbb); // magic

    return TT_SUCCESS;
}

static tt_bool_t enc_all;

static tt_result_t __ut_enc_pre(IN tt_buf_t *raw,
                                OUT tt_u32_t *len,
                                IN void *param)
{
    tt_u32_t s;

    TT_ASSERT(param == (void *)0x3);

    if (enc_all) {
        s = TT_BUF_RLEN(raw);
    } else {
        s = tt_rand_u32() % 20 + 1;
    }

    if (TT_BUF_RLEN(raw) >= s) {
        *len = s;
        return TT_SUCCESS;
    } else {
        return TT_E_BUF_NOBUFS;
    }
}

static tt_result_t __ut_enc(IN tt_buf_t *raw,
                            IN tt_u32_t len,
                            OUT tt_buf_t *enc,
                            IN void *param)
{
    TT_ASSERT(param == (void *)0x3);
    TT_ASSERT(TT_BUF_RLEN(raw) >= len);

    tt_buf_put_u8(enc, len);
    tt_buf_put(enc, TT_BUF_RPOS(raw), len);

    tt_buf_inc_rp(raw, len);

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(case_wbuf)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_rbuf_t rbuf;
    tt_rbuf_decode_itf_t d_itf = {__ut_dec_pre, __ut_dec};
    tt_rbuf_parse_itf_t p_itf = {__ut_par_pre, __ut_par, __ut_par_done};
    tt_result_t ret;
    tt_u32_t i, len, n, wlen;
    tt_u8_t *p, *wp;

    tt_wbuf_t wbuf;
    tt_wbuf_encode_itf_t e_itf = {__ut_enc_pre, __ut_enc};
    tt_wbuf_render_itf_t r_itf = {__ut_rdr_pre, __ut_rdr};

    TT_TEST_CASE_ENTER()
    // test start

    tt_rbuf_init(&rbuf, &d_itf, (void *)1, &p_itf, (void *)2, NULL);

    /*
    for (i = 0; i < 100; ++i) {
        __gen_rbuf(&raw, i, i);
    }
    __enc_rbuf(&raw, &enc);
    */
    tt_wbuf_init(&wbuf, &e_itf, (void *)0x3, &r_itf, (void *)0x4, NULL);

    ret = tt_wbuf_reserve(&wbuf, 20);
    TT_UT_SUCCESS(ret, "");

    enc_all = TT_FALSE;
    for (i = 0; i < 100; ++i) {
        tt_u8_t *rendered;
        if (i == 99) {
            enc_all = TT_TRUE;
        }
        ret = tt_wbuf_render(&wbuf, (void *)(tt_uintptr_t)i, &rendered, &len);
        if (TT_OK(ret)) {
            TT_UT_EQUAL(len, i + 3, "");
            TT_UT_EQUAL(rendered[0], i, "");
            TT_UT_EQUAL(rendered[1], 0xaa, "");
            TT_UT_EQUAL(rendered[len - 1], 0xbb, "");
            if (len > 3) {
                TT_UT_EQUAL(rendered[len - 2], i, "");
            }
        } else {
            TT_UT_EQUAL(ret, TT_E_BUF_NOBUFS, "");
        }
    }

    tt_wbuf_get_rptr(&wbuf, &wp, &wlen);

    ret = tt_rbuf_reserve(&rbuf, wlen);
    TT_UT_SUCCESS(ret, "");

    tt_rbuf_get_wptr(&rbuf, &p, NULL);
    tt_memcpy(p, wp, wlen);

    __ut_ret = TT_SUCCESS;
    __ut_seq = 0;

    ret = tt_rbuf_inc_wp(&rbuf, wlen);
    TT_UT_SUCCESS(ret, "");
    TT_UT_SUCCESS(__ut_ret, "");

    // again

    __ut_ret = TT_SUCCESS;
    __ut_seq = 0;
    len = wlen;
    i = 0;
    while (i < len) {
        tt_u32_t s = tt_rand_u32() % 10;
        if (i + s > len) {
            s = len - i;
        }

        ret = tt_rbuf_reserve(&rbuf, s);
        TT_UT_SUCCESS(ret, "");

        tt_rbuf_get_wptr(&rbuf, &p, &n);
        tt_memcpy(p, wp + i, s);
        ret = tt_rbuf_inc_wp(&rbuf, s);
        if (ret != TT_SUCCESS && ret != TT_E_BUF_NOBUFS) {
            TT_UT_EXP(TT_OK(ret) || (ret == TT_E_BUF_NOBUFS), "");
        }

        i += s;
    }
    TT_UT_SUCCESS(__ut_ret, "");

    tt_rbuf_destroy(&rbuf);
    tt_wbuf_destroy(&wbuf);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_rwbuf)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_rbuf_t rbuf;
    tt_rbuf_decode_itf_t d_itf = {__ut_dec_pre, __ut_dec};
    tt_rbuf_parse_itf_t p_itf = {__ut_par_pre, __ut_par, __ut_par_done};
    tt_result_t ret;
    tt_u32_t i, len, wlen;
    tt_u8_t *p, *wp;

    tt_wbuf_t wbuf;
    tt_wbuf_encode_itf_t e_itf = {__ut_enc_pre, __ut_enc};
    tt_wbuf_render_itf_t r_itf = {__ut_rdr_pre, __ut_rdr};

    TT_TEST_CASE_ENTER()
    // test start

    tt_rbuf_init(&rbuf, &d_itf, (void *)1, &p_itf, (void *)2, NULL);

    /*
     for (i = 0; i < 100; ++i) {
     __gen_rbuf(&raw, i, i);
     }
     __enc_rbuf(&raw, &enc);
     */
    tt_wbuf_init(&wbuf, &e_itf, (void *)0x3, &r_itf, (void *)0x4, NULL);

    ret = tt_wbuf_reserve(&wbuf, 20);
    TT_UT_SUCCESS(ret, "");

    __ut_ret = TT_SUCCESS;
    __ut_seq = 0;
    enc_all = TT_FALSE;
    for (i = 0; i < 100; ++i) {
        tt_u8_t *rendered;
        if (i == 99) {
            enc_all = TT_TRUE;
        }
        ret = tt_wbuf_render(&wbuf, (void *)(tt_uintptr_t)i, &rendered, &len);
        if (TT_OK(ret)) {
            TT_UT_EQUAL(len, i + 3, "");
            TT_UT_EQUAL(rendered[0], i, "");
            TT_UT_EQUAL(rendered[1], 0xaa, "");
            TT_UT_EQUAL(rendered[len - 1], 0xbb, "");
            if (len > 3) {
                TT_UT_EQUAL(rendered[len - 2], i, "");
            }

            // each time wbuf has data, pass to rbuf
            tt_wbuf_get_rptr(&wbuf, &wp, &wlen);

            ret = tt_rbuf_reserve(&rbuf, wlen);
            TT_UT_SUCCESS(ret, "");

            tt_rbuf_get_wptr(&rbuf, &p, NULL);
            tt_memcpy(p, wp, wlen);

            ret = tt_rbuf_inc_wp(&rbuf, wlen);
            TT_UT_EXP(TT_OK(ret) || (ret == TT_E_BUF_NOBUFS), "");
            TT_UT_SUCCESS(__ut_ret, "");

            // after data in wbuf is processed, inc rp
            tt_wbuf_inc_rp(&wbuf, wlen);
        } else {
            TT_UT_EQUAL(ret, TT_E_BUF_NOBUFS, "");
        }
    }

    tt_rbuf_destroy(&rbuf);
    tt_wbuf_destroy(&wbuf);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_rwbuf_raw)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_rbuf_t rbuf;
    tt_rbuf_parse_itf_t p_itf = {__ut_par_pre, __ut_par, __ut_par_done};
    tt_result_t ret;
    tt_u32_t i, len, wlen;
    tt_u8_t *p, *wp;
    tt_buf_t saved;

    tt_wbuf_t wbuf;
    tt_wbuf_render_itf_t r_itf = {__ut_rdr_pre, __ut_rdr};

    TT_TEST_CASE_ENTER()
    // test start

    tt_rbuf_init(&rbuf, NULL, (void *)1, &p_itf, (void *)2, NULL);
    tt_buf_init(&saved, NULL);

    /*
     for (i = 0; i < 100; ++i) {
     __gen_rbuf(&raw, i, i);
     }
     __enc_rbuf(&raw, &enc);
     */
    tt_wbuf_init(&wbuf, NULL, (void *)0x3, &r_itf, (void *)0x4, NULL);

    ret = tt_wbuf_reserve(&wbuf, 20);
    TT_UT_SUCCESS(ret, "");

    __ut_ret = TT_SUCCESS;
    __ut_seq = 0;
    enc_all = TT_FALSE;
    for (i = 0; i < 100; ++i) {
        tt_u8_t *rendered;
        if (i == 99) {
            enc_all = TT_TRUE;
        }
        ret = tt_wbuf_render(&wbuf, (void *)(tt_uintptr_t)i, &rendered, &len);
        if (TT_OK(ret)) {
            TT_UT_EQUAL(len, i + 3, "");
            TT_UT_EQUAL(rendered[0], i, "");
            TT_UT_EQUAL(rendered[1], 0xaa, "");
            TT_UT_EQUAL(rendered[len - 1], 0xbb, "");
            if (len > 3) {
                TT_UT_EQUAL(rendered[len - 2], i, "");
            }
            tt_buf_put(&saved, rendered, len);

            // each time wbuf has data, pass to rbuf
            tt_wbuf_get_rptr(&wbuf, &wp, &wlen);

            ret = tt_rbuf_reserve(&rbuf, wlen);
            TT_UT_SUCCESS(ret, "");

            tt_rbuf_get_wptr(&rbuf, &p, NULL);
            tt_memcpy(p, wp, wlen);

            ret = tt_rbuf_inc_wp(&rbuf, wlen);
            TT_UT_EXP(TT_OK(ret) || (ret == TT_E_BUF_NOBUFS), "");
            TT_UT_SUCCESS(__ut_ret, "");

            // after data in wbuf is processed, inc rp
            tt_wbuf_inc_rp(&wbuf, wlen);
        } else {
            TT_UT_EQUAL(ret, TT_E_BUF_NOBUFS, "");
        }
    }

    // verify saved data
    tt_wbuf_put(&wbuf, TT_BUF_RPOS(&saved), TT_BUF_RLEN(&saved));
    tt_wbuf_get_rptr(&wbuf, &wp, &wlen);

    ret = tt_rbuf_reserve(&rbuf, wlen);
    TT_UT_SUCCESS(ret, "");

    tt_rbuf_get_wptr(&rbuf, &p, NULL);
    tt_memcpy(p, wp, wlen);

    __ut_ret = TT_SUCCESS;
    __ut_seq = 0;
    ret = tt_rbuf_inc_wp(&rbuf, wlen);
    TT_UT_EXP(TT_OK(ret) || (ret == TT_E_BUF_NOBUFS), "");
    TT_UT_SUCCESS(__ut_ret, "");

    tt_rbuf_destroy(&rbuf);
    tt_wbuf_destroy(&wbuf);
    tt_buf_destroy(&saved);

    // test end
    TT_TEST_CASE_LEAVE()
}
