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

#include <stdlib.h>

#include <network/http/tt_http_parser.h>
#include <network/http/tt_http_raw_header.h>
#include <network/http/tt_http_render.h>
#include <network/http/tt_http_server_connection.h>
#include <network/http/tt_http_service_manager.h>

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
TT_TEST_ROUTINE_DECLARE(case_http_sconn_basic)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(http_sconn_case)

TT_TEST_CASE("case_http_sconn_basic",
             "http server conn basic",
             case_http_sconn_basic,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE_LIST_DEFINE_END(http_sconn_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(HTTP_UT_SCONN, 0, http_sconn_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(case_http_sconn_basic)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    typedef tt_result_t (*__sconn_send_t)(IN tt_http_sconn_t *c,
                                          IN tt_u8_t *buf,
                                          IN tt_u32_t len,
                                          OUT tt_u32_t *sent);

typedef tt_result_t (*__sconn_recv_t)(IN tt_http_sconn_t *c,
                                      OUT tt_u8_t *buf,
                                      IN tt_u32_t len,
                                      OUT tt_u32_t *recvd,
                                      OUT tt_fiber_ev_t **p_fev,
                                      OUT tt_tmr_t **p_tmr);

typedef tt_result_t (*__sconn_shut_t)(IN tt_http_sconn_t *c,
                                      IN tt_http_shut_t shut);

typedef void (*__sconn_destroy_t)(IN tt_http_sconn_t *c);

typedef struct
{
    __sconn_send_t send;
    __sconn_recv_t recv;
    __sconn_shut_t shut;
    __sconn_destroy_t destroy;
} __sconn_itf_t;

// ========================================
// simple itf
// ========================================

static tt_result_t __simu_send(IN tt_http_sconn_t *c,
                               IN tt_u8_t *buf,
                               IN tt_u32_t len,
                               OUT tt_u32_t *sent);

static tt_result_t __simu_recv(IN tt_http_sconn_t *c,
                               OUT tt_u8_t *buf,
                               IN tt_u32_t len,
                               OUT tt_u32_t *recvd,
                               OUT tt_fiber_ev_t **p_fev,
                               OUT tt_tmr_t **p_tmr);

static tt_result_t __simu_shut(IN tt_http_sconn_t *c, IN tt_http_shut_t shut);

static void __simu_destroy(IN tt_http_sconn_t *c);

static __sconn_itf_t __sconn_skt_itf = {
    __simu_send, __simu_recv, __simu_shut, __simu_destroy,
};

static tt_buf_t __simu_in;
static tt_buf_t __simu_out;

tt_result_t __simu_send(IN tt_http_sconn_t *c,
                        IN tt_u8_t *buf,
                        IN tt_u32_t len,
                        OUT tt_u32_t *sent)
{
    tt_buf_put(&__simu_out, buf, len);

    TT_SAFE_ASSIGN(sent, len);
    return TT_SUCCESS;
}

static tt_u32_t __simu_recv_num;

tt_result_t __simu_recv(IN tt_http_sconn_t *c,
                        OUT tt_u8_t *buf,
                        IN tt_u32_t len,
                        OUT tt_u32_t *recvd,
                        OUT tt_fiber_ev_t **p_fev,
                        OUT tt_tmr_t **p_tmr)
{
    tt_u32_t n, rd;
    tt_u8_t *p;

    n = TT_BUF_RLEN(&__simu_in);
    TT_ASSERT(n >= __simu_recv_num);
    n -= __simu_recv_num;
    if (n == 0) {
        return TT_E_END;
    }

    rd = tt_rand_u32() % 20 + 1;
    rd = TT_MIN(rd, n);
    rd = TT_MIN(rd, len);

    p = TT_BUF_RPOS(&__simu_in) + __simu_recv_num;
    tt_memcpy(buf, p, rd);
    *recvd = rd;
    __simu_recv_num += rd;

    TT_SAFE_ASSIGN(p_fev, NULL);
    TT_SAFE_ASSIGN(p_tmr, NULL);

    return TT_SUCCESS;
}

static tt_http_shut_t __simu_shut_val;
tt_result_t __simu_shut(IN tt_http_sconn_t *c, IN tt_http_shut_t shut)
{
    __simu_shut_val = shut;
    return TT_SUCCESS;
}

static tt_bool_t __simu_destroy_val;
void __simu_destroy(IN tt_http_sconn_t *c)
{
    __simu_destroy_val = TT_TRUE;
}

static __sconn_itf_t simu_itf1 = {
    __simu_send, __simu_recv, __simu_shut, __simu_destroy,
};

void __simu1_start()
{
    tt_buf_init(&__simu_in, NULL);
    tt_buf_init(&__simu_out, NULL);
    __simu_shut_val = TT_HTTP_SHUT_NUM;
    __simu_destroy_val = TT_FALSE;
    __simu_recv_num = 0;
}

void __simu1_clear()
{
    tt_buf_clear(&__simu_in);
    tt_buf_clear(&__simu_out);
    __simu_shut_val = TT_HTTP_SHUT_NUM;
    __simu_destroy_val = TT_FALSE;
    __simu_recv_num = 0;
}

void __simu1_end()
{
    tt_buf_destroy(&__simu_in);
    tt_buf_destroy(&__simu_out);
}

// ========================================
// simple service
// ========================================

#define __MAX_SIMP1_NUM 10
static tt_u32_t __simp1_idx;

static const tt_char_t *__simp1_uri[__MAX_SIMP1_NUM];
static tt_bool_t __simp1_uri_ok[__MAX_SIMP1_NUM];
static tt_http_inserv_action_t __simp1_on_uri(
    IN struct tt_http_inserv_s *s,
    IN struct tt_http_parser_s *req,
    OUT struct tt_http_resp_render_s *resp)
{
    if (tt_blobex_strcmp(&req->uri, __simp1_uri[__simp1_idx]) == 0) {
        __simp1_uri_ok[__simp1_idx] = TT_TRUE;
    } else {
        __simp1_uri_ok[__simp1_idx] = TT_FALSE;
    }

    return TT_HTTP_INSERV_ACT_OWNER;
}

#define __check(list, n, v, ok)                                                \
    do {                                                                       \
        tt_http_rawhdr_t *rh = tt_http_rawhdr_find(&req->list, n);             \
        tt_http_rawval_t *rv;                                                  \
        if (rh == NULL) {                                                      \
            ok = TT_FALSE;                                                     \
            break;                                                             \
        };                                                                     \
        rv = tt_http_rawval_find(&rh->val, v);                                 \
        if (rv == NULL) {                                                      \
            ok = TT_FALSE;                                                     \
            break;                                                             \
        };                                                                     \
        ok = TT_TRUE;                                                          \
    } while (0)

struct simp1_nv
{
    const tt_char_t *name;
    const tt_char_t *val;
};

struct simp1_nv *simp1_nv_ar[__MAX_SIMP1_NUM];
static tt_bool_t simp1_nv_ok[__MAX_SIMP1_NUM];

static tt_http_inserv_action_t __simp1_on_header(
    IN struct tt_http_inserv_s *s,
    IN struct tt_http_parser_s *req,
    OUT struct tt_http_resp_render_s *resp)
{
    struct simp1_nv *nv = simp1_nv_ar[__simp1_idx];
    while (nv->name != NULL) {
        __check(rawhdr, nv->name, nv->val, simp1_nv_ok[__simp1_idx]);
        if (!simp1_nv_ok[__simp1_idx]) {
            break;
        }
        ++nv;
    }

    return TT_HTTP_INSERV_ACT_OWNER;
}

static tt_buf_t __simp1_body[__MAX_SIMP1_NUM];

static tt_http_inserv_action_t __simp1_on_body(
    IN struct tt_http_inserv_s *s,
    IN struct tt_http_parser_s *req,
    OUT struct tt_http_resp_render_s *resp)
{
    tt_buf_put(&__simp1_body[__simp1_idx],
               tt_blobex_addr(&req->body),
               tt_blobex_len(&req->body));
    return TT_HTTP_INSERV_ACT_OWNER;
}

struct simp1_nv *simp1_trail_ar[__MAX_SIMP1_NUM];
static tt_bool_t simp1_trail_ok[__MAX_SIMP1_NUM];
static tt_http_inserv_action_t __simp1_on_trailing(
    IN struct tt_http_inserv_s *s,
    IN struct tt_http_parser_s *req,
    OUT struct tt_http_resp_render_s *resp)
{
    struct simp1_nv *nv = simp1_trail_ar[__simp1_idx];
    while (nv->name != NULL) {
        __check(trailing_rawhdr,
                nv->name,
                nv->val,
                simp1_trail_ok[__simp1_idx]);
        if (!simp1_trail_ok[__simp1_idx]) {
            break;
        }
        ++nv;
    }

    return TT_HTTP_INSERV_ACT_OWNER;
}

static tt_s32_t simp1_comp[__MAX_SIMP1_NUM];

static tt_http_inserv_action_t __simp1_on_complete(
    IN struct tt_http_inserv_s *s,
    IN struct tt_http_parser_s *req,
    OUT struct tt_http_resp_render_s *resp)
{
    ++simp1_comp[__simp1_idx];
    ++__simp1_idx;

    tt_http_resp_render_set_status(resp, TT_HTTP_STATUS_OK);
    return TT_HTTP_INSERV_ACT_OWNER;
}

static tt_http_inserv_cb_t __simp1_cb = {__simp1_on_uri,
                                         __simp1_on_header,
                                         __simp1_on_body,
                                         __simp1_on_trailing,
                                         __simp1_on_complete};

static struct simp1_nv simp1_nv_s[1] = {{NULL, NULL}};

static void __simp1_start()
{
    tt_u32_t i;

    __simp1_idx = 0;

    for (i = 0; i < __MAX_SIMP1_NUM; ++i) {
        __simp1_uri[i] = "";
        __simp1_uri_ok[i] = TT_TRUE;

        tt_buf_init(&__simp1_body[i], NULL);

        simp1_nv_ar[i] = simp1_nv_s;
        simp1_nv_ok[i] = TT_TRUE;

        simp1_trail_ar[i] = simp1_nv_s;
        simp1_trail_ok[i] = TT_TRUE;

        simp1_comp[i] = 0;
    }
}

void __simp1_clear()
{
    tt_u32_t i;

    __simp1_idx = 0;

    for (i = 0; i < __MAX_SIMP1_NUM; ++i) {
        __simp1_uri[i] = "";
        __simp1_uri_ok[i] = TT_TRUE;

        tt_buf_clear(&__simp1_body[i]);

        simp1_nv_ar[i] = simp1_nv_s;
        simp1_nv_ok[i] = TT_TRUE;

        simp1_trail_ar[i] = simp1_nv_s;
        simp1_trail_ok[i] = TT_TRUE;

        simp1_comp[i] = 0;
    }
}

void __simp1_end()
{
    tt_u32_t i;

    for (i = 0; i < __MAX_SIMP1_NUM; ++i) {
        tt_buf_destroy(&__simp1_body[i]);
    }
}

TT_TEST_ROUTINE_DEFINE(case_http_sconn_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_http_sconn_t c;
    tt_result_t ret;
    tt_http_inserv_t *s;
    tt_http_inserv_itf_t s_itf = {0};
    tt_bool_t bret;
    tt_u32_t i;

    TT_TEST_CASE_ENTER()
    // test start

    __simu1_start();
    __simp1_start();

    ret = tt_http_sconn_create(&c, &simu_itf1, NULL, NULL);
    TT_UT_SUCCESS(ret, "");
    tt_http_sconn_destroy(&c);

    ret = tt_http_sconn_create(&c, &simu_itf1, NULL, NULL);
    TT_UT_SUCCESS(ret, "");

    s = tt_http_inserv_create(0, &s_itf, &__simp1_cb);
    tt_http_sconn_add_inserv(&c, s);

    // put a http request
    {
        const tt_char_t *h =
            "POST /a/b/c HTTP/1.1\r\n"
            "Content-Type: text/plain\r\n"
            "Transfer-Encoding: chunked\r\n"
            "\r\n"
            "25  \r\n"
            "This is the data in the first chunk\r\n"
            "\r\n"
            "1C\r\n"
            "and this is the second one\r\n"
            "\r\n"
            "1C\r\n"
            "and this is the third1 one\r\n"
            "\r\n"
            "0  \r\n"
            "X-Vary: *\r\n"
            "X-Content-Type: text/plain\r\n"
            "X-xxx:\r\n"
            "\r\n"
            "PATCH /file.txt HTTP/1.1\r\n"
            "Host: www.example.com\r\n"
            "Content-Type: application/example\r\n"
            "If-Match: \"e0023aa4e\"\r\n"
            "Content-Length: 10\r\n"
            "\r\n"
            "1234567890"
            "GET /favicon.ico HTTP/1.1\r\n"
            "Host: 0.0.0.0=5000\r\n"
            "User-Agent: Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.9) "
            "Gecko/2008061015 Firefox/3.0\r\n"
            "Accept: "
            "text/html,application/xhtml+xml,application/xml;q=0.9,*/"
            "*;q=0.8\r\n"
            "Accept-Language: en-us,en;q=0.5\r\n"
            "Accept-Encoding: gzip,deflate\r\n"
            "Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r\n"
            "Keep-Alive: 300\r\n"
            "Connection: keep-alive\r\n"
            "\r\n";

        const tt_char_t *bd =
            "This is the data in the first chunk\r\n"
            "and this is the second one\r\n"
            "and this is the third1 one\r\n";
        const tt_char_t *bd2 = "1234567890";

        struct simp1_nv nv[] = {{"Content-Type", "text/plain"},
                                {"Transfer-Encoding", "chunked"},
                                {NULL, NULL}};

        struct simp1_nv nv2[] = {{"X-Vary", "*"},
                                 {"X-Content-Type", "text/plain"},
                                 {"X-xxx", ""},
                                 {NULL, NULL}};

        tt_buf_put_cstr(&__simu_in, h);

        __simp1_uri[0] = "/a/b/c";
        simp1_nv_ar[0] = nv;
        simp1_trail_ar[0] = nv2;

        {
            static struct simp1_nv nv_2[] = {{"Host", "www.example.com"},
                                             {"Content-Type",
                                              "application/example"},
                                             {"If-Match", "\"e0023aa4e\""},
                                             {"Content-Length", "10"},
                                             {NULL, NULL}};

            __simp1_uri[1] = "/file.txt";
            simp1_nv_ar[1] = nv_2;
        }

        {
            static struct simp1_nv nv_3[] = {{"Host", "0.0.0.0=5000"},
                                             {"Accept",
                                              "text/html,application/"
                                              "xhtml+xml,application/"
                                              "xml;q=0.9,*/*;q=0.8"},
                                             {"Connection", "keep-alive"},
                                             {NULL, NULL}};

            __simp1_uri[2] = "/favicon.ico";
            simp1_nv_ar[2] = nv_3;
        }

        bret = tt_http_sconn_run(&c);
        TT_UT_SUCCESS(ret, "");

        for (i = 0; i < 3; ++i) {
            TT_UT_TRUE(__simp1_uri_ok[i], "");
            TT_UT_TRUE(simp1_nv_ok[i], "");
            TT_UT_TRUE(simp1_trail_ok[i], "");
            TT_UT_EQUAL(simp1_comp[i], 1, "");
        }

        TT_UT_EQUAL(tt_buf_cmp_cstr(&__simp1_body[0], bd), 0, "");
        TT_UT_EQUAL(tt_buf_cmp_cstr(&__simp1_body[1], bd2), 0, "");
        TT_UT_EQUAL(tt_buf_cmp_cstr(&__simp1_body[2], ""), 0, "");
    }

    tt_http_sconn_destroy(&c);

    __simp1_end();
    __simu1_end();

    // test end
    TT_TEST_CASE_LEAVE()
}
