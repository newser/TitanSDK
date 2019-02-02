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

#include <network/http/def/tt_http_service_def.h>
#include <network/http/service/tt_http_encserv_chunked.h>
#include <network/http/service/tt_http_inserv_auth.h>
#include <network/http/service/tt_http_inserv_conditional.h>
#include <network/http/service/tt_http_inserv_file.h>
#include <network/http/service/tt_http_inserv_param.h>
#include <network/http/tt_http_parser.h>
#include <network/http/tt_http_raw_header.h>
#include <network/http/tt_http_render.h>
#include <network/http/tt_http_util.h>

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
TT_TEST_ROUTINE_DECLARE(case_http_inserv_file)
TT_TEST_ROUTINE_DECLARE(case_http_inserv_cond)
TT_TEST_ROUTINE_DECLARE(case_http_inserv_param)
TT_TEST_ROUTINE_DECLARE(case_http_inserv_auth)
TT_TEST_ROUTINE_DECLARE(case_http_post_param)

TT_TEST_ROUTINE_DECLARE(case_http_encserv_chunked)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(http_inserv_case)

TT_TEST_CASE("case_http_inserv_file",
             "http uri service: file",
             case_http_inserv_file,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("case_http_inserv_cond",
                 "http uri service: conditional",
                 case_http_inserv_cond,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_http_inserv_param",
                 "http uri service: parameter",
                 case_http_inserv_param,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_http_inserv_auth",
                 "http uri service: auth",
                 case_http_inserv_auth,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_http_post_param",
                 "http post parameter",
                 case_http_post_param,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_http_encserv_chunked",
                 "http encoding service: chunked",
                 case_http_encserv_chunked,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(http_inserv_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(HTTP_UT_INSERV, 0, http_inserv_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(case_http_inserv_auth)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(case_http_inserv_file)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_http_inserv_t *is;
    tt_http_inserv_file_attr_t a;
    tt_http_parser_t req;
    tt_http_resp_render_t resp;
    tt_slab_t rh, rv;
    tt_u8_t *p;
    tt_u32_t len;
    tt_http_inserv_action_t act;
    tt_http_inserv_file_ctx_t fctx;

    TT_TEST_CASE_ENTER()
    // test start

    TT_UT_SUCCESS(tt_slab_create(&rh, sizeof(tt_http_rawhdr_t), NULL), "");
    TT_UT_SUCCESS(tt_slab_create(&rv, sizeof(tt_http_rawval_t), NULL), "");
    TT_UT_SUCCESS(tt_http_parser_create(&req, &rh, &rv, NULL), "");
    tt_http_resp_render_init(&resp, NULL);

    tt_http_inserv_file_attr_default(&a);
    a.can_have_path_param = TT_FALSE;
    a.can_have_query_param = TT_FALSE;
    a.chunk_size = 16; // test
    a.process_post = TT_TRUE;

    is = tt_http_inserv_file_create(&a);
    TT_UT_NOT_NULL(is, "");
    tt_http_inserv_clear(is);
    tt_http_inserv_release(is);

    is = tt_http_inserv_file_create(&a);
    TT_UT_NOT_NULL(is, "");

    TT_UT_SUCCESS(tt_http_inserv_create_ctx(is, &fctx), "");

    req.parser.method = TT_HTTP_MTD_PUT;
    act = tt_http_inserv_on_uri(is, &fctx, &req, &resp);
    TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");
    act = tt_http_inserv_on_header(is, &fctx, &req, &resp);
    TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");

    req.parser.method = TT_HTTP_MTD_POST;
    act = tt_http_inserv_on_uri(is, &fctx, &req, &resp);
    TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");
    act = tt_http_inserv_on_header(is, &fctx, &req, &resp);
    TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");

    {
        tt_http_uri_t *uri;

        // ignore as it has path param
        tt_blobex_set(&req.rawuri,
                      (tt_u8_t *)"/a/b;p1=v1",
                      sizeof("/a/b;p1=v1") - 1,
                      TT_FALSE);
        req.updated_uri = TT_FALSE;
        uri = tt_http_parser_get_uri(&req);
        TT_UT_NOT_NULL(uri, "");
        act = tt_http_inserv_on_header(is, &fctx, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");

        // ignore as it has query param
        tt_blobex_set(&req.rawuri,
                      (tt_u8_t *)"/a/b?q1=v1",
                      sizeof("/a/b?q1=v1") - 1,
                      TT_FALSE);
        req.updated_uri = TT_FALSE;
        uri = tt_http_parser_get_uri(&req);
        TT_UT_NOT_NULL(uri, "");
        act = tt_http_inserv_on_header(is, &fctx, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");

        tt_http_inserv_clear_ctx(is, &fctx);

        // can process but file unexist
        tt_blobex_set(&req.rawuri,
                      (tt_u8_t *)"/a/b/",
                      sizeof("/a/b/") - 1,
                      TT_FALSE);
        req.updated_uri = TT_FALSE;
        uri = tt_http_parser_get_uri(&req);
        TT_UT_NOT_NULL(uri, "");
        act = tt_http_inserv_on_header(is, &fctx, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_OWNER, "");
        TT_UT_STREQ(tt_fpath_render(&uri->path), "/a/b/", "");

        act = tt_http_inserv_on_complete(is, &fctx, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_DISCARD, "");
        TT_UT_EQUAL(tt_http_resp_render_get_status(&resp),
                    TT_HTTP_STATUS_NOT_FOUND,
                    "");
    }

    // test with existing file
    {
        tt_char_t *cp = tt_current_path(TT_TRUE);
        tt_string_t s;
        tt_file_t f;
        tt_char_t b[] = "1234567890";
        tt_http_uri_t *uri;
        tt_buf_t bbuf;

        tt_buf_init(&bbuf, NULL);

        tt_string_init(&s, NULL);
        tt_string_append(&s, cp);
        tt_free(cp);
        tt_string_append(&s, "a.txt");

        tt_fremove(tt_string_cstr(&s));
        TT_UT_SUCCESS(tt_fopen(&f,
                               tt_string_cstr(&s),
                               TT_FO_WRITE | TT_FO_CREAT | TT_FO_EXCL,
                               NULL),
                      "");
        TT_UT_SUCCESS(tt_fwrite(&f, (tt_u8_t *)b, sizeof(b), NULL), "");
        tt_fclose(&f);

        tt_http_inserv_clear_ctx(is, &fctx);

        tt_blobex_set(&req.rawuri,
                      (tt_u8_t *)tt_string_cstr(&s),
                      tt_string_len(&s),
                      TT_FALSE);
        req.updated_uri = TT_FALSE;
        uri = tt_http_parser_get_uri(&req);
        TT_UT_NOT_NULL(uri, "");
        act = tt_http_inserv_on_header(is, &fctx, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_OWNER, "");

        // this service does not handle body
        act = tt_http_inserv_on_body(is, &fctx, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");
        act = tt_http_inserv_on_trailing(is, &fctx, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");

        // will send a 200 ok
        act = tt_http_inserv_on_complete(is, &fctx, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_BODY, "");
        TT_UT_EQUAL(tt_http_resp_render_get_status(&resp),
                    TT_HTTP_STATUS_OK,
                    "");
        TT_UT_EQUAL(resp.render.content_len, sizeof(b), "");

        act = tt_http_inserv_get_body(is, &fctx, &req, &resp, &bbuf);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_BODY, "");
        TT_UT_EQUAL(TT_BUF_RLEN(&bbuf), sizeof(b), "");
        TT_UT_STREQ(TT_BUF_RPOS(&bbuf), b, "");

        // end
        act = tt_http_inserv_get_body(is, &fctx, &req, &resp, &bbuf);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");

        tt_string_destroy(&s);
        tt_buf_destroy(&bbuf);
    }

    tt_http_inserv_clear_ctx(is, &fctx);
    tt_http_inserv_clear(is);
    tt_http_parser_clear(&req, TT_TRUE);
    tt_http_resp_render_clear(&resp);

    req.parser.method = TT_HTTP_MTD_GET;

    // test with existing file which is larger than chunk size
    {
        tt_char_t *cp = tt_current_path(TT_TRUE);
        tt_string_t s;
        tt_file_t f;
        tt_char_t b[] = "12345678901234567890";
        tt_http_uri_t *uri;
        tt_buf_t bbuf;

        tt_buf_init(&bbuf, NULL);

        tt_string_init(&s, NULL);
        tt_string_append(&s, cp);
        tt_free(cp);
        tt_string_append(&s, "a2.txt");

        tt_fremove(tt_string_cstr(&s));
        TT_UT_SUCCESS(tt_fopen(&f,
                               tt_string_cstr(&s),
                               TT_FO_WRITE | TT_FO_CREAT | TT_FO_EXCL,
                               NULL),
                      "");
        TT_UT_SUCCESS(tt_fwrite(&f, (tt_u8_t *)b, sizeof(b), NULL), "");
        tt_fclose(&f);

        tt_blobex_set(&req.rawuri,
                      (tt_u8_t *)tt_string_cstr(&s),
                      tt_string_len(&s),
                      TT_FALSE);
        req.updated_uri = TT_FALSE;
        uri = tt_http_parser_get_uri(&req);
        TT_UT_NOT_NULL(uri, "");
        act = tt_http_inserv_on_header(is, &fctx, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_OWNER, "");

        // this service does not handle body
        act = tt_http_inserv_on_body(is, &fctx, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");
        act = tt_http_inserv_on_trailing(is, &fctx, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");

        // will send a 200 ok
        act = tt_http_inserv_on_complete(is, &fctx, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_BODY, "");
        TT_UT_EQUAL(tt_http_resp_render_get_status(&resp),
                    TT_HTTP_STATUS_OK,
                    "");
        TT_UT_EXP(resp.render.content_len < 0, "");
        TT_UT_EQUAL(resp.render.txenc_num, 1, "");
        TT_UT_EQUAL(resp.render.txenc[0], TT_HTTP_TXENC_CHUNKED, "");

        // chunk size is set to 16
        act = tt_http_inserv_get_body(is, &fctx, &req, &resp, &bbuf);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_BODY, "");
        TT_UT_EQUAL(TT_BUF_RLEN(&bbuf), 16, "");
        TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(&bbuf), b, 16), 0, "");

        // left 4 bytes
        tt_buf_clear(&bbuf);
        act = tt_http_inserv_get_body(is, &fctx, &req, &resp, &bbuf);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_BODY, "");
        TT_UT_EQUAL(TT_BUF_RLEN(&bbuf), 5, "");
        TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(&bbuf), b + 16, 5), 0, "");

        // end
        act = tt_http_inserv_get_body(is, &fctx, &req, &resp, &bbuf);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");

        tt_string_destroy(&s);
        tt_buf_destroy(&bbuf);
    }

    tt_http_inserv_destroy_ctx(is, &fctx);
    tt_http_inserv_release(is);

    tt_http_parser_destroy(&req);
    tt_http_resp_render_destroy(&resp);

    tt_slab_destroy(&rh);
    tt_slab_destroy(&rv);

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_bool_t __mk_parser(tt_http_parser_t *p, const tt_char_t *msg)
{
    tt_u8_t *addr;
    tt_u32_t len, msglen;

    tt_http_parser_clear(p, TT_TRUE);

    tt_http_parser_wpos(p, &addr, &len);
    msglen = tt_strlen(msg);
    if (msglen > len) {
        return TT_FALSE;
    }
    tt_memcpy(addr, msg, msglen);
    tt_http_parser_inc_wp(p, msglen);

    if (!TT_OK(tt_http_parser_run(p))) {
        return TT_FALSE;
    }

    return TT_TRUE;
}

TT_TEST_ROUTINE_DEFINE(case_http_inserv_cond)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_http_inserv_t *is;
    tt_http_parser_t req;
    tt_http_resp_render_t resp;
    tt_slab_t rh, rv;
    tt_u8_t *p;
    tt_u32_t len;
    tt_http_inserv_action_t act;
    tt_http_inserv_cond_attr_t a;
    tt_http_inserv_cond_ctx_t cond_ctx;

    TT_TEST_CASE_ENTER()
    // test start

    TT_UT_SUCCESS(tt_slab_create(&rh, sizeof(tt_http_rawhdr_t), NULL), "");
    TT_UT_SUCCESS(tt_slab_create(&rv, sizeof(tt_http_rawval_t), NULL), "");
    TT_UT_SUCCESS(tt_http_parser_create(&req, &rh, &rv, NULL), "");
    tt_http_resp_render_init(&resp, NULL);

    tt_http_inserv_cond_attr_default(&a);

    is = tt_http_inserv_cond_create(&a);
    TT_UT_NOT_NULL(is, "");
    tt_http_inserv_clear(is);
    tt_http_inserv_release(is);

    is = tt_http_inserv_cond_create(&a);
    TT_UT_NOT_NULL(is, "");

    TT_UT_SUCCESS(tt_http_inserv_create_ctx(is, &cond_ctx), "");

    {
        tt_char_t *cp = tt_current_path(TT_TRUE);
        tt_string_t s, s2;
        tt_file_t f;
        tt_char_t b[] = "12345678901234567890";
        tt_http_uri_t *uri;
        tt_char_t etag[40];

        // make a file
        tt_string_init(&s, NULL);
        tt_string_init(&s2, NULL);
        tt_string_append(&s, cp);
        tt_free(cp);
        tt_string_append(&s, "a2.txt");

        tt_fremove(tt_string_cstr(&s));
        TT_UT_SUCCESS(tt_fopen(&f,
                               tt_string_cstr(&s),
                               TT_FO_WRITE | TT_FO_CREAT | TT_FO_EXCL,
                               NULL),
                      "");
        TT_UT_SUCCESS(tt_fwrite(&f, (tt_u8_t *)b, sizeof(b), NULL), "");
        tt_fclose(&f);

        TT_UT_SUCCESS(tt_fopen(&f, tt_string_cstr(&s), TT_FO_READ, NULL), "");
        TT_UT_SUCCESS(tt_http_file_etag(&f, etag, sizeof(etag)), "");
        tt_fclose(&f);

        // make a req: if-match, one etag match, return pass
        {
            tt_string_clear(&s2);
            tt_string_append(&s2, "GET / HTTP/1.1\r\nIf-match: \"123\", \"");
            tt_string_append(&s2, etag);
            tt_string_append(&s2, "\"\r\n\r\n");

            TT_UT_TRUE(__mk_parser(&req, tt_string_cstr(&s2)), "");
            // set uri
            tt_blobex_set(&req.rawuri,
                          (tt_u8_t *)tt_string_cstr(&s),
                          tt_string_len(&s),
                          TT_FALSE);
            req.updated_uri = TT_FALSE;
            uri = tt_http_parser_get_uri(&req);
            TT_UT_NOT_NULL(uri, "");

            tt_http_resp_render_clear(&resp);

            tt_http_inserv_clear_ctx(is, &cond_ctx);
            act = tt_http_inserv_on_header(is, &cond_ctx, &req, &resp);
            TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");
        }

        // make a req: if-match, none etag match, return owner and 412
        {
            tt_string_clear(&s2);
            tt_string_append(&s2,
                             "GET / HTTP/1.1\r\nIf-match: \"123\", \"456\", ");
            tt_string_append(&s2, "\r\n\r\n");

            TT_UT_TRUE(__mk_parser(&req, tt_string_cstr(&s2)), "");
            // set uri
            tt_blobex_set(&req.rawuri,
                          (tt_u8_t *)tt_string_cstr(&s),
                          tt_string_len(&s),
                          TT_FALSE);
            req.updated_uri = TT_FALSE;
            uri = tt_http_parser_get_uri(&req);
            TT_UT_NOT_NULL(uri, "");

            tt_http_resp_render_clear(&resp);

            tt_http_inserv_clear_ctx(is, &cond_ctx);
            act = tt_http_inserv_on_header(is, &cond_ctx, &req, &resp);
            TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_OWNER, "");
            tt_http_inserv_on_complete(is, &cond_ctx, &req, &resp);
            TT_UT_EQUAL(resp.status, TT_HTTP_STATUS_PRECONDITION_FAILED, "");
        }

        // make a req: if-match, an aster match, return pass
        {
            tt_string_clear(&s2);
            tt_string_append(&s2, "GET / HTTP/1.1\r\nIf-match: \"123\", *, ");
            tt_string_append(&s2, "\r\n\r\n");

            TT_UT_TRUE(__mk_parser(&req, tt_string_cstr(&s2)), "");
            // set uri
            tt_blobex_set(&req.rawuri,
                          (tt_u8_t *)tt_string_cstr(&s),
                          tt_string_len(&s),
                          TT_FALSE);
            req.updated_uri = TT_FALSE;
            uri = tt_http_parser_get_uri(&req);
            TT_UT_NOT_NULL(uri, "");

            tt_http_resp_render_clear(&resp);

            tt_http_inserv_clear_ctx(is, &cond_ctx);
            act = tt_http_inserv_on_header(is, &cond_ctx, &req, &resp);
            TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");
        }

        // make a req: if-none-match, one etag match, return 304
        {
            tt_string_clear(&s2);
            tt_string_append(&s2,
                             "GET / HTTP/1.1\r\nIf-none-match: \"123\", \"");
            tt_string_append(&s2, etag);
            tt_string_append(&s2, "\"\r\n\r\n");

            TT_UT_TRUE(__mk_parser(&req, tt_string_cstr(&s2)), "");
            // set uri
            tt_blobex_set(&req.rawuri,
                          (tt_u8_t *)tt_string_cstr(&s),
                          tt_string_len(&s),
                          TT_FALSE);
            req.updated_uri = TT_FALSE;
            uri = tt_http_parser_get_uri(&req);
            TT_UT_NOT_NULL(uri, "");

            tt_http_resp_render_clear(&resp);

            tt_http_inserv_clear_ctx(is, &cond_ctx);
            act = tt_http_inserv_on_header(is, &cond_ctx, &req, &resp);
            TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_OWNER, "");
            tt_http_inserv_on_complete(is, &cond_ctx, &req, &resp);
            TT_UT_EQUAL(resp.status, TT_HTTP_STATUS_NOT_MODIFIED, "");
        }

        // make a req: if-none-match, aster match, return 412
        {
            tt_string_clear(&s2);
            tt_string_append(&s2,
                             "POST / HTTP/1.1\r\nIf-none-match: \"123\", *");
            tt_string_append(&s2, "\r\n\r\n");

            TT_UT_TRUE(__mk_parser(&req, tt_string_cstr(&s2)), "");
            // set uri
            tt_blobex_set(&req.rawuri,
                          (tt_u8_t *)tt_string_cstr(&s),
                          tt_string_len(&s),
                          TT_FALSE);
            req.updated_uri = TT_FALSE;
            uri = tt_http_parser_get_uri(&req);
            TT_UT_NOT_NULL(uri, "");

            tt_http_resp_render_clear(&resp);

            tt_http_inserv_clear_ctx(is, &cond_ctx);
            act = tt_http_inserv_on_header(is, &cond_ctx, &req, &resp);
            TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_OWNER, "");
            tt_http_inserv_on_complete(is, &cond_ctx, &req, &resp);
            TT_UT_EQUAL(resp.status, TT_HTTP_STATUS_PRECONDITION_FAILED, "");
        }

        // make a req: if-none-match, none etag match, return pass
        {
            tt_string_clear(&s2);
            tt_string_append(
                &s2, "GET / HTTP/1.1\r\nIf-none-match: \"123\", \"456\", ");
            tt_string_append(&s2, "\r\n\r\n");

            TT_UT_TRUE(__mk_parser(&req, tt_string_cstr(&s2)), "");
            // set uri
            tt_blobex_set(&req.rawuri,
                          (tt_u8_t *)tt_string_cstr(&s),
                          tt_string_len(&s),
                          TT_FALSE);
            req.updated_uri = TT_FALSE;
            uri = tt_http_parser_get_uri(&req);
            TT_UT_NOT_NULL(uri, "");

            tt_http_resp_render_clear(&resp);

            tt_http_inserv_clear_ctx(is, &cond_ctx);
            act = tt_http_inserv_on_header(is, &cond_ctx, &req, &resp);
            TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");
        }

        // make a req: if-none-match, unexist, return pass
        {
            tt_string_clear(&s2);
            tt_string_append(
                &s2, "GET /abc HTTP/1.1\r\nIf-none-match: \"123\", \"456\", ");
            tt_string_append(&s2, "\r\n\r\n");

            TT_UT_TRUE(__mk_parser(&req, tt_string_cstr(&s2)), "");

            tt_http_resp_render_clear(&resp);

            tt_http_inserv_clear_ctx(is, &cond_ctx);
            act = tt_http_inserv_on_header(is, &cond_ctx, &req, &resp);
            TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");
        }

        // make a req: if-match, unexist, return pass
        {
            tt_string_clear(&s2);
            tt_string_append(
                &s2, "GET /abc HTTP/1.1\r\nIf-match: \"123\", \"456\", ");
            tt_string_append(&s2, "\r\n\r\n");

            TT_UT_TRUE(__mk_parser(&req, tt_string_cstr(&s2)), "");

            tt_http_resp_render_clear(&resp);

            tt_http_inserv_clear_ctx(is, &cond_ctx);
            act = tt_http_inserv_on_header(is, &cond_ctx, &req, &resp);
            TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");
        }

        // make a req: if-match with aster, unexist, return 412
        {
            tt_string_clear(&s2);
            tt_string_append(
                &s2, "GET /abc HTTP/1.1\r\nIf-match: \"123\", *, \"456\", ");
            tt_string_append(&s2, "\r\n\r\n");

            TT_UT_TRUE(__mk_parser(&req, tt_string_cstr(&s2)), "");

            tt_http_resp_render_clear(&resp);

            tt_http_inserv_clear_ctx(is, &cond_ctx);
            act = tt_http_inserv_on_header(is, &cond_ctx, &req, &resp);
            TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_OWNER, "");
            tt_http_inserv_on_complete(is, &cond_ctx, &req, &resp);
            TT_UT_EQUAL(resp.status, TT_HTTP_STATUS_PRECONDITION_FAILED, "");
        }

        tt_string_destroy(&s);
        tt_string_destroy(&s2);
    }

    tt_http_inserv_destroy_ctx(is, &cond_ctx);

    tt_http_inserv_release(is);

    tt_http_parser_destroy(&req);
    tt_http_resp_render_destroy(&resp);

    tt_slab_destroy(&rh);
    tt_slab_destroy(&rv);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_http_inserv_param)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_http_inserv_t *is;
    tt_param_t *dir, *p, *d2;
    tt_u32_t u32_val = 999;
    tt_http_inserv_param_attr_t a;
    tt_http_inserv_param_ctx_t c;
    tt_string_t s2;
    tt_http_inserv_action_t act;
    tt_buf_t b;

    tt_http_parser_t req;
    tt_http_resp_render_t resp;
    tt_slab_t rh, rv;

    TT_TEST_CASE_ENTER()
    // test start

    tt_string_init(&s2, NULL);
    tt_buf_init(&b, NULL);
    TT_UT_SUCCESS(tt_slab_create(&rh, sizeof(tt_http_rawhdr_t), NULL), "");
    TT_UT_SUCCESS(tt_slab_create(&rv, sizeof(tt_http_rawval_t), NULL), "");
    TT_UT_SUCCESS(tt_http_parser_create(&req, &rh, &rv, NULL), "");
    tt_http_resp_render_init(&resp, NULL);

    d2 = tt_param_dir_create("d2", NULL);
    p = tt_param_u32_create("u32-1", &u32_val, NULL, NULL);
    tt_param_dir_add(TT_PARAM_CAST(d2, tt_param_dir_t), p);
    // test
    p->tid = 101;

    dir = tt_param_dir_create("dir", NULL);
    tt_param_dir_add(TT_PARAM_CAST(dir, tt_param_dir_t), d2);
    p = tt_param_u32_create("u32-2", &u32_val, NULL, NULL);
    tt_param_dir_add(TT_PARAM_CAST(dir, tt_param_dir_t), p);
    // test
    p->tid = 201;

    tt_http_inserv_param_attr_default(&a);
    a.path = "/config";
    a.path_len = sizeof("/config") - 1;

    is = tt_http_inserv_param_create(dir, &a);
    TT_UT_NOT_NULL(is, "");
    tt_http_inserv_release(is);

    is = tt_http_inserv_param_create(dir, &a);
    TT_UT_NOT_NULL(is, "");

    TT_UT_SUCCESS(tt_http_inserv_create_ctx(is, &c), "");

    // unsupported method
    {
        tt_string_clear(&s2);
        tt_string_append(&s2, "DELETE /confi HTTP/1.1\r\n\r\n");

        TT_UT_TRUE(__mk_parser(&req, tt_string_cstr(&s2)), "");

        tt_http_resp_render_clear(&resp);

        act = tt_http_inserv_on_uri(is, &c, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");
        act = tt_http_inserv_on_header(is, &c, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");
    }

    {
        tt_string_clear(&s2);
        tt_string_append(&s2, "GET /confi HTTP/1.1\r\n\r\n");

        TT_UT_TRUE(__mk_parser(&req, tt_string_cstr(&s2)), "");

        tt_http_resp_render_clear(&resp);

        act = tt_http_inserv_on_uri(is, &c, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");
    }

    {
        tt_http_inserv_clear(is);
        tt_http_inserv_clear_ctx(is, &c);

        tt_string_clear(&s2);
        tt_string_append(&s2, "GET /config HTTP/1.1\r\n\r\n");

        TT_UT_TRUE(__mk_parser(&req, tt_string_cstr(&s2)), "");

        tt_http_resp_render_clear(&resp);

        act = tt_http_inserv_on_uri(is, &c, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_OWNER, "");

        act = tt_http_inserv_on_header(is, &c, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");

        act = tt_http_inserv_on_complete(is, &c, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_BODY, "");
        TT_UT_EQUAL(tt_http_resp_render_get_status(&resp),
                    TT_HTTP_STATUS_OK,
                    "");
        TT_UT_EQUAL(resp.render.contype, TT_HTTP_CONTYPE_APP_JSON, "");

        tt_buf_clear(&b);
        act = tt_http_inserv_get_body(is, &c, &req, &resp, &b);
        tt_buf_print_cstr(&b, 0);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");
    }

    {
        const tt_char_t *h =
            "PUT /config HTTP/1.1\r\n"
            "Content-Type: text/html\r\n\r\n";

        tt_http_inserv_clear(is);
        tt_http_inserv_clear_ctx(is, &c);

        tt_string_clear(&s2);
        tt_string_append(&s2, h);

        TT_UT_TRUE(__mk_parser(&req, tt_string_cstr(&s2)), "");

        tt_http_resp_render_clear(&resp);

        act = tt_http_inserv_on_uri(is, &c, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_OWNER, "");

        act = tt_http_inserv_on_header(is, &c, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_DISCARD, "");
        TT_UT_EQUAL(tt_http_resp_render_get_status(&resp),
                    TT_HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE,
                    "");
    }

    {
        const tt_char_t *h =
            "PUT /config HTTP/1.1\r\n"
            "Content-Type: application/x-www-form-urlencoded\r\n"
            "Content-length: 22\r\n\r\n"
            "101 = 1234 & 201= 4567";

        tt_http_inserv_clear(is);
        tt_http_inserv_clear_ctx(is, &c);

        tt_string_clear(&s2);
        tt_string_append(&s2, h);

        TT_UT_TRUE(__mk_parser(&req, tt_string_cstr(&s2)), "");

        tt_http_resp_render_clear(&resp);

        act = tt_http_inserv_on_uri(is, &c, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_OWNER, "");

        act = tt_http_inserv_on_header(is, &c, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");

        act = tt_http_inserv_on_body(is, &c, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");
        TT_UT_EQUAL(tt_string_cmp(&c.body, "101 = 1234 & 201= 4567"), 0, "");

        act = tt_http_inserv_on_complete(is, &c, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_BODY, "");
        TT_UT_EQUAL(tt_http_resp_render_get_status(&resp),
                    TT_HTTP_STATUS_OK,
                    "");

        tt_buf_clear(&b);
        act = tt_http_inserv_get_body(is, &c, &req, &resp, &b);
        tt_buf_print_cstr(&b, 0);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");
    }

    {
        tt_http_inserv_clear(is);
        tt_http_inserv_clear_ctx(is, &c);

        tt_string_clear(&s2);
        tt_string_append(
            &s2, "GET /config HTTP/1.1\r\nContent-Type: text/html\r\n\r\n");

        TT_UT_TRUE(__mk_parser(&req, tt_string_cstr(&s2)), "");

        tt_http_resp_render_clear(&resp);

        act = tt_http_inserv_on_uri(is, &c, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_OWNER, "");

        act = tt_http_inserv_on_header(is, &c, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");

        act = tt_http_inserv_on_complete(is, &c, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_BODY, "");
        TT_UT_EQUAL(tt_http_resp_render_get_status(&resp),
                    TT_HTTP_STATUS_OK,
                    "");
        TT_UT_EQUAL(resp.render.contype, TT_HTTP_CONTYPE_APP_JSON, "");

        tt_buf_clear(&b);
        act = tt_http_inserv_get_body(is, &c, &req, &resp, &b);
        tt_buf_print_cstr(&b, 0);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");
    }

    tt_http_inserv_destroy_ctx(is, &c);
    tt_http_inserv_release(is);

    tt_param_destroy(dir);

    tt_string_destroy(&s2);
    tt_buf_destroy(&b);

    tt_http_parser_destroy(&req);
    tt_http_resp_render_destroy(&resp);
    tt_slab_destroy(&rh);
    tt_slab_destroy(&rv);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_http_encserv_chunked)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_http_encserv_t *es;
    tt_buf_t b, *outb;

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&b, NULL);

    es = tt_http_encserv_chunked_create();
    TT_UT_NOT_NULL(es, "");
    tt_http_encserv_clear(es);
    tt_http_encserv_release(es);

    es = tt_http_encserv_chunked_create();
    TT_UT_NOT_NULL(es, "");

    tt_buf_clear(&b);
    TT_UT_SUCCESS(tt_http_encserv_pre_body(es, NULL, NULL, &b, &outb), "");
    TT_UT_EQUAL(outb, &b, "");

    tt_buf_clear(&b);
    TT_UT_SUCCESS(tt_http_encserv_on_body(es, NULL, NULL, &b, &outb), "");
    TT_UT_EQUAL(outb, &b, "");
    TT_UT_EQUAL(tt_buf_cmp_cstr(outb, "0\r\n\r\n"), 0, "");

    tt_buf_clear(&b);
    tt_buf_put_u8(&b, 'x');
    TT_UT_SUCCESS(tt_http_encserv_on_body(es, NULL, NULL, &b, &outb), "");
    TT_UT_EQUAL(outb, &b, "");
    TT_UT_EQUAL(tt_buf_cmp_cstr(outb, "1\r\nx\r\n"), 0, "");

    tt_buf_clear(&b);
    tt_buf_put(&b, "yz", 2);
    TT_UT_SUCCESS(tt_http_encserv_on_body(es, NULL, NULL, &b, &outb), "");
    TT_UT_EQUAL(outb, &b, "");
    TT_UT_EQUAL(tt_buf_cmp_cstr(outb, "2\r\nyz\r\n"), 0, "");

    tt_buf_clear(&b);
    TT_UT_SUCCESS(tt_http_encserv_post_body(es, NULL, NULL, &b, &outb), "");
    TT_UT_EQUAL(outb, &b, "");
    TT_UT_EQUAL(tt_buf_cmp_cstr(outb, "0\r\n\r\n"), 0, "");

    tt_http_encserv_clear(es);
    tt_http_encserv_release(es);

    tt_buf_clear(&b);

    // test end
    TT_TEST_CASE_LEAVE()
}

extern tt_result_t __post_param(IN tt_http_inserv_t *s,
                                IN tt_http_inserv_param_ctx_t *c,
                                IN tt_http_parser_t *req,
                                OUT tt_http_resp_render_t *resp,
                                IN tt_param_t *p);

TT_TEST_ROUTINE_DEFINE(case_http_post_param)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_http_inserv_param_ctx_t c;
    tt_http_resp_render_t resp;
    tt_param_t *root;
    tt_param_t *dir, *p, *d2;
    tt_u32_t u32_val = 999;
    tt_s32_t s32_val = 8888;
    tt_http_inserv_t *is;

    TT_TEST_CASE_ENTER()
    // test start

    tt_http_resp_render_init(&resp, NULL);

    d2 = tt_param_dir_create("d2", NULL);
    p = tt_param_u32_create("u32-1", &u32_val, NULL, NULL);
    tt_param_dir_add(TT_PARAM_CAST(d2, tt_param_dir_t), p);
    // test
    p->tid = 1010;

    dir = tt_param_dir_create("dir", NULL);
    tt_param_dir_add(TT_PARAM_CAST(dir, tt_param_dir_t), d2);
    p = tt_param_s32_create("s32-2", &s32_val, NULL, NULL);
    tt_param_dir_add(TT_PARAM_CAST(dir, tt_param_dir_t), p);
    // test
    p->tid = 20199;

    is = tt_http_inserv_param_create(dir, NULL);
    TT_UT_NOT_NULL(is, "");

    TT_UT_SUCCESS(tt_http_inserv_create_ctx(is, &c), "");

    // empty body
    {
        tt_http_inserv_clear_ctx(is, &c);
        tt_http_resp_render_clear(&resp);

        tt_string_clear(&c.body);
        TT_UT_EQUAL(__post_param(is, &c, NULL, &resp, dir),
                    TT_HTTP_INSERV_ACT_BODY,
                    "");

        TT_UT_EQUAL(tt_http_resp_render_get_status(&resp),
                    TT_HTTP_STATUS_OK,
                    "");
        TT_UT_EQUAL(resp.render.contype, TT_HTTP_CONTYPE_APP_JSON, "");
        TT_UT_EQUAL(resp.render.txenc[0], TT_HTTP_TXENC_CHUNKED, "");
        TT_UT_EQUAL(resp.render.txenc_num, 1, "");
    }

    // 1 entry
    {
        tt_http_inserv_clear_ctx(is, &c);
        tt_http_resp_render_clear(&resp);

        tt_string_set(&c.body, "1010 = 8080");
        TT_UT_EQUAL(__post_param(is, &c, NULL, &resp, dir),
                    TT_HTTP_INSERV_ACT_BODY,
                    "");

        TT_UT_EQUAL(tt_http_resp_render_get_status(&resp),
                    TT_HTTP_STATUS_OK,
                    "");
        TT_UT_EQUAL(resp.render.contype, TT_HTTP_CONTYPE_APP_JSON, "");
        TT_UT_EQUAL(resp.render.txenc[0], TT_HTTP_TXENC_CHUNKED, "");
        TT_UT_EQUAL(resp.render.txenc_num, 1, "");

        TT_UT_EQUAL(u32_val, 8080, "");

        tt_buf_clear(&c.buf);
        tt_jdoc_render(&c.jdoc, &c.buf, NULL);
        TT_UT_EQUAL(tt_buf_cmp_cstr(&c.buf, "{\"1010\":\"8080\"}"), 0, "");
    }

    // 1 entry
    {
        tt_string_t ss;
        tt_param_t *ps;

        tt_string_init(&ss, NULL);

        ps = tt_param_str_create("haha", &ss, NULL, NULL);
        TT_UT_NOT_NULL(ps, "");
        ps->tid = 666;

        tt_http_inserv_clear_ctx(is, &c);
        tt_http_resp_render_clear(&resp);

        tt_string_set(&c.body, "666= 1%2022+333");
        TT_UT_EQUAL(__post_param(is, &c, NULL, &resp, ps),
                    TT_HTTP_INSERV_ACT_BODY,
                    "");

        TT_UT_EQUAL(tt_http_resp_render_get_status(&resp),
                    TT_HTTP_STATUS_OK,
                    "");
        TT_UT_EQUAL(resp.render.contype, TT_HTTP_CONTYPE_APP_JSON, "");
        TT_UT_EQUAL(resp.render.txenc[0], TT_HTTP_TXENC_CHUNKED, "");
        TT_UT_EQUAL(resp.render.txenc_num, 1, "");

        TT_UT_EQUAL(tt_string_cmp(&ss, " 1 22 333"), 0, "");

        tt_buf_clear(&c.buf);
        tt_jdoc_render(&c.jdoc, &c.buf, NULL);
        TT_UT_EQUAL(tt_buf_cmp_cstr(&c.buf, "{\"666\":\" 1 22 333\"}"), 0, "");

        tt_param_destroy(ps);
        tt_string_destroy(&ss);
    }

    // 1 entry, invalid format
    u32_val = 6767;
    {
        tt_http_inserv_clear_ctx(is, &c);
        tt_http_resp_render_clear(&resp);

        tt_string_set(&c.body, "1010-8080");
        TT_UT_EQUAL(__post_param(is, &c, NULL, &resp, dir),
                    TT_HTTP_INSERV_ACT_BODY,
                    "");

        TT_UT_EQUAL(tt_http_resp_render_get_status(&resp),
                    TT_HTTP_STATUS_ACCEPTED,
                    "");
        TT_UT_EQUAL(resp.render.contype, TT_HTTP_CONTYPE_APP_JSON, "");
        TT_UT_EQUAL(resp.render.txenc[0], TT_HTTP_TXENC_CHUNKED, "");
        TT_UT_EQUAL(resp.render.txenc_num, 1, "");

        TT_UT_EQUAL(u32_val, 6767, "");

        tt_buf_clear(&c.buf);
        tt_jdoc_render(&c.jdoc, &c.buf, NULL);
        TT_UT_EQUAL(tt_buf_cmp_cstr(&c.buf, "{}"), 0, "");
    }

    // 1 entry, invalid format
    {
        tt_http_inserv_clear_ctx(is, &c);
        tt_http_resp_render_clear(&resp);

        tt_string_set(&c.body, "  2147683477=8080  ");
        TT_UT_EQUAL(__post_param(is, &c, NULL, &resp, dir),
                    TT_HTTP_INSERV_ACT_BODY,
                    "");

        TT_UT_EQUAL(tt_http_resp_render_get_status(&resp),
                    TT_HTTP_STATUS_ACCEPTED,
                    "");

        TT_UT_EQUAL(u32_val, 6767, "");

        tt_buf_clear(&c.buf);
        tt_jdoc_render(&c.jdoc, &c.buf, NULL);
        TT_UT_EQUAL(tt_buf_cmp_cstr(&c.buf, "{}"), 0, "");
    }

    // 1 entry, tid unexist
    {
        tt_http_inserv_clear_ctx(is, &c);
        tt_http_resp_render_clear(&resp);

        tt_string_set(&c.body, "  89899 =8080  ");
        TT_UT_EQUAL(__post_param(is, &c, NULL, &resp, dir),
                    TT_HTTP_INSERV_ACT_BODY,
                    "");

        TT_UT_EQUAL(tt_http_resp_render_get_status(&resp),
                    TT_HTTP_STATUS_ACCEPTED,
                    "");

        TT_UT_EQUAL(u32_val, 6767, "");

        tt_buf_clear(&c.buf);
        tt_jdoc_render(&c.jdoc, &c.buf, NULL);
        TT_UT_EQUAL(tt_buf_cmp_cstr(&c.buf, "{}"), 0, "");
    }

    // 1 entry, invalid value
    {
        tt_http_inserv_clear_ctx(is, &c);
        tt_http_resp_render_clear(&resp);

        tt_string_set(&c.body, "1010=8080808080808080  ");
        TT_UT_EQUAL(__post_param(is, &c, NULL, &resp, dir),
                    TT_HTTP_INSERV_ACT_BODY,
                    "");

        TT_UT_EQUAL(tt_http_resp_render_get_status(&resp),
                    TT_HTTP_STATUS_ACCEPTED,
                    "");

        TT_UT_EQUAL(u32_val, 6767, "");

        tt_buf_clear(&c.buf);
        tt_jdoc_render(&c.jdoc, &c.buf, NULL);
        TT_UT_EQUAL(tt_buf_cmp_cstr(&c.buf, "{\"1010\":\"6767\"}"), 0, "");
    }

    // 2 entry, both valid
    {
        tt_http_inserv_clear_ctx(is, &c);
        tt_http_resp_render_clear(&resp);

        tt_string_set(&c.body, " & 1010 = 9876 && &&&  20199= -333&  & &&&");
        TT_UT_EQUAL(__post_param(is, &c, NULL, &resp, dir),
                    TT_HTTP_INSERV_ACT_BODY,
                    "");

        TT_UT_EQUAL(tt_http_resp_render_get_status(&resp),
                    TT_HTTP_STATUS_ACCEPTED,
                    "");

        TT_UT_EQUAL(u32_val, 9876, "");
        TT_UT_EQUAL(s32_val, -333, "");

        tt_buf_clear(&c.buf);
        tt_jdoc_render(&c.jdoc, &c.buf, NULL);
        TT_UT_EQUAL(tt_buf_cmp_cstr(&c.buf,
                                    "{\"1010\":\"9876\",\"20199\":\"-333\"}"),
                    0,
                    "");
    }

    // 2 entry, 1 invalid
    {
        tt_http_inserv_clear_ctx(is, &c);
        tt_http_resp_render_clear(&resp);

        tt_string_set(&c.body, "1010=-1111&20199=2222");
        TT_UT_EQUAL(__post_param(is, &c, NULL, &resp, dir),
                    TT_HTTP_INSERV_ACT_BODY,
                    "");

        TT_UT_EQUAL(tt_http_resp_render_get_status(&resp),
                    TT_HTTP_STATUS_ACCEPTED,
                    "");

        TT_UT_EQUAL(u32_val, 9876, "");
        TT_UT_EQUAL(s32_val, 2222, "");

        tt_buf_clear(&c.buf);
        tt_jdoc_render(&c.jdoc, &c.buf, NULL);
        TT_UT_EQUAL(tt_buf_cmp_cstr(&c.buf,
                                    "{\"1010\":\"9876\",\"20199\":\"2222\"}"),
                    0,
                    "");
    }

    tt_http_inserv_destroy_ctx(is, &c);

    tt_http_inserv_release(is);

    tt_param_destroy(dir);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_http_inserv_auth)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_http_inserv_t *is;
    tt_http_inserv_auth_attr_t a;
    tt_http_inserv_auth_ctx_t c;
    tt_string_t s2;
    tt_http_inserv_action_t act;
    tt_buf_t b;

    tt_http_parser_t req;
    tt_http_resp_render_t resp;
    tt_slab_t rh, rv;
    tt_http_auth_t *ha;

    const tt_char_t *msg =
        "GET /confi HTTP/1.1\r\n"
        "Authorization: Digest username=\"Mufasa\","
        "realm=\"testrealm@host.com\","
        "uri=\"/dir/index.html\","
        "algorithm=MD5,"
        "nonce=\"dcd98b7102dd2f0e8b11d0f600bfb0c093\","
        "nc=00000001,"
        "cnonce=\"0a4f113b\","
        "qop=auth,"
        "response=\"6629fae49393a05397450978507c4ef1\","
        "opaque=\"5ccc069c403ebaf9f0171e9517f40e41\"\r\n\r\n";

    TT_TEST_CASE_ENTER()
    // test start

    tt_string_init(&s2, NULL);
    tt_buf_init(&b, NULL);
    TT_UT_SUCCESS(tt_slab_create(&rh, sizeof(tt_http_rawhdr_t), NULL), "");
    TT_UT_SUCCESS(tt_slab_create(&rv, sizeof(tt_http_rawval_t), NULL), "");
    TT_UT_SUCCESS(tt_http_parser_create(&req, &rh, &rv, NULL), "");
    tt_http_resp_render_init(&resp, NULL);

    tt_http_inserv_auth_attr_default(&a);

    is = tt_http_inserv_auth_create(&a);
    TT_UT_NOT_NULL(is, "");
    tt_http_inserv_release(is);

    a.get_pwd = NULL;
    a.get_pwd_param = NULL;
    is = tt_http_inserv_auth_create(&a);
    TT_UT_NULL(is, "");

    a.realm = "testrealm@host.com";
    a.realm_len = sizeof("testrealm@host.com") - 1;
    a.domain = "domain";
    a.domain_len = sizeof("domain") - 1;
    a.get_pwd = NULL;
    a.get_pwd_param = "Circle Of Life";
    a.fixed_nonce = "dcd98b7102dd2f0e8b11d0f600bfb0c093";
    is = tt_http_inserv_auth_create(&a);
    TT_UT_NOT_NULL(is, "");

    TT_UT_SUCCESS(tt_http_inserv_create_ctx(is, &c), "");

    {
        tt_string_clear(&s2);
        tt_string_append(&s2, "DELETE /confi HTTP/1.1\r\n\r\n");

        TT_UT_TRUE(__mk_parser(&req, tt_string_cstr(&s2)), "");

        tt_http_resp_render_clear(&resp);

        act = tt_http_inserv_on_uri(is, &c, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");
        // no auth
        act = tt_http_inserv_on_header(is, &c, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_DISCARD, "");

        TT_UT_EQUAL(tt_http_resp_render_get_status(&resp),
                    TT_HTTP_STATUS_UNAUTHORIZED,
                    "");
        ha = tt_http_render_get_www_auth(&resp.render);
        TT_UT_NOT_NULL(ha, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&ha->realm, "testrealm@host.com"), 0, "");
        TT_UT_EQUAL(tt_blobex_strcmp(&ha->domain, "domain"), 0, "");
        TT_UT_NOT_NULL(tt_blobex_addr(&ha->nonce), "");
    }

    {
        const tt_char_t *msg =
            "GET /confi HTTP/1.1\r\n"
            "Authorization: Digest username=\"Mufasa\","
            "realm=\"realm\","
            "uri=\"/dir/index.html\","
            "algorithm=MD5,"
            "nonce=\"dcd98b7102dd2f0e8b11d0f600bfb0c093\","
            "nc=00000001,"
            "cnonce=\"0a4f113b\","
            "qop=auth,"
            "response=\"6629fae49393a05397450978507c4ef1\","
            "opaque=\"5ccc069c403ebaf9f0171e9517f40e41\"\r\n\r\n";

        tt_string_clear(&s2);
        tt_string_append(&s2, msg);

        TT_UT_TRUE(__mk_parser(&req, tt_string_cstr(&s2)), "");

        tt_http_resp_render_clear(&resp);

        act = tt_http_inserv_on_uri(is, &c, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");
        // different realm
        act = tt_http_inserv_on_header(is, &c, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_DISCARD, "");

        TT_UT_EQUAL(tt_http_resp_render_get_status(&resp),
                    TT_HTTP_STATUS_UNAUTHORIZED,
                    "");
        ha = tt_http_render_get_www_auth(&resp.render);
        TT_UT_NOT_NULL(ha, "");
    }

    {
        const tt_char_t *msg =
            "GET /confi HTTP/1.1\r\n"
            "Authorization: Digest username=\"Mufasa\","
            "realm=\"testrealm@host.com\","
            "uri=\"/dir/index.html\","
            "algorithm=MD5,"
            "nonce=\"nonce\","
            "nc=00000001,"
            "cnonce=\"0a4f113b\","
            "qop=auth,"
            "response=\"6629fae49393a05397450978507c4ef1\","
            "opaque=\"5ccc069c403ebaf9f0171e9517f40e41\"\r\n\r\n";

        tt_string_clear(&s2);
        tt_string_append(&s2, msg);

        TT_UT_TRUE(__mk_parser(&req, tt_string_cstr(&s2)), "");

        tt_http_resp_render_clear(&resp);

        act = tt_http_inserv_on_uri(is, &c, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");
        // different nonce
        act = tt_http_inserv_on_header(is, &c, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_DISCARD, "");

        TT_UT_EQUAL(tt_http_resp_render_get_status(&resp),
                    TT_HTTP_STATUS_UNAUTHORIZED,
                    "");
        ha = tt_http_render_get_www_auth(&resp.render);
        TT_UT_NOT_NULL(ha, "");
    }

    {
        tt_string_clear(&s2);
        tt_string_append(&s2, msg);
        tt_u32_t i = 0;

    ag:
        TT_UT_TRUE(__mk_parser(&req, tt_string_cstr(&s2)), "");

        tt_http_resp_render_clear(&resp);

        act = tt_http_inserv_on_uri(is, &c, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");
        act = tt_http_inserv_on_header(is, &c, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");

        tt_http_inserv_clear(is);
        if (i++ < 5) {
            // repeat, all ok as fixed_nonce
            goto ag;
        }
    }

    {
        const tt_char_t *msg =
            "GET /confi HTTP/1.1\r\n"
            "Authorization: Digest username=\"Mufasa\","
            "realm=\"testrealm@host.com\","
            "uri=\"/dir/index.html\","
            "algorithm=MD5,"
            "nonce=\"dcd98b7102dd2f0e8b11d0f600bfb0c093\","
            "nc=00000001,"
            "cnonce=\"0a4f113b\","
            "qop=auth-int,"
            "response=\"6629fae49393a05397450978507c4ef1\","
            "opaque=\"5ccc069c403ebaf9f0171e9517f40e41\"\r\n\r\n";

        tt_string_clear(&s2);
        tt_string_append(&s2, msg);

        TT_UT_TRUE(__mk_parser(&req, tt_string_cstr(&s2)), "");

        tt_http_resp_render_clear(&resp);

        act = tt_http_inserv_on_uri(is, &c, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");
        // auth-int is not implemented yet
        act = tt_http_inserv_on_header(is, &c, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_DISCARD, "");

        TT_UT_EQUAL(tt_http_resp_render_get_status(&resp),
                    TT_HTTP_STATUS_NOT_IMPLEMENTED,
                    "");
    }

    // invalid response
    {
        const tt_char_t *msg =
            "GET /confi HTTP/1.1\r\n"
            "Authorization: Digest username=\"Mufasa\","
            "realm=\"testrealm@host.com\","
            "uri=\"/dir/index.html\","
            "algorithm=MD5,"
            "nonce=\"dcd98b7102dd2f0e8b11d0f600bfb0c093\","
            "nc=00000001,"
            "cnonce=\"0a4f113b\","
            "qop=auth,"
            "response=\"response\","
            "opaque=\"5ccc069c403ebaf9f0171e9517f40e41\"\r\n\r\n";

        tt_string_clear(&s2);
        tt_string_append(&s2, msg);

        TT_UT_TRUE(__mk_parser(&req, tt_string_cstr(&s2)), "");

        tt_http_resp_render_clear(&resp);

        act = tt_http_inserv_on_uri(is, &c, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");
        // auth-int is not implemented yet
        act = tt_http_inserv_on_header(is, &c, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_DISCARD, "");

        TT_UT_EQUAL(tt_http_resp_render_get_status(&resp),
                    TT_HTTP_STATUS_UNAUTHORIZED,
                    "");
        ha = tt_http_render_get_www_auth(&resp.render);
        TT_UT_NOT_NULL(ha, "");
    }

    tt_http_inserv_destroy_ctx(is, &c);
    tt_http_inserv_release(is);

    tt_string_destroy(&s2);
    tt_buf_destroy(&b);

    tt_http_parser_destroy(&req);
    tt_http_resp_render_destroy(&resp);
    tt_slab_destroy(&rh);
    tt_slab_destroy(&rv);

    // test end
    TT_TEST_CASE_LEAVE()
}
