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

#include <network/http/service/tt_http_inserv_file.h>
#include <network/http/tt_http_parser.h>
#include <network/http/tt_http_raw_header.h>
#include <network/http/tt_http_render.h>

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
    TT_TEST_ROUTINE_DEFINE(case_http_inserv_file)
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

    TT_TEST_CASE_ENTER()
    // test start

    TT_UT_SUCCESS(tt_slab_create(&rh, sizeof(tt_http_rawhdr_t), NULL), "");
    TT_UT_SUCCESS(tt_slab_create(&rv, sizeof(tt_http_rawval_t), NULL), "");
    TT_UT_SUCCESS(tt_http_parser_create(&req, &rh, &rv, NULL), "");
    tt_http_resp_render_init(&resp, NULL);

    tt_http_inserv_file_attr_default(&a);
    a.can_have_path_param = TT_FALSE;
    a.can_have_query_param = TT_FALSE;
    a.def_name = "i.php";
    a.def_name_len = sizeof("i.php") - 1;
    a.chunk_size = 16; // test
    a.process_post = TT_TRUE;

    is = tt_http_inserv_file_create(&a);
    TT_UT_NOT_NULL(is, "");
    tt_http_inserv_clear(is);
    tt_http_inserv_release(is);

    is = tt_http_inserv_file_create(&a);
    TT_UT_NOT_NULL(is, "");

    req.parser.method = TT_HTTP_MTD_PUT;
    act = tt_http_inserv_on_uri(is, &req, &resp);
    TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");
    act = tt_http_inserv_on_header(is, &req, &resp);
    TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");

    req.parser.method = TT_HTTP_MTD_POST;
    act = tt_http_inserv_on_uri(is, &req, &resp);
    TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");
    act = tt_http_inserv_on_header(is, &req, &resp);
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
        act = tt_http_inserv_on_header(is, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");

        // ignore as it has query param
        tt_blobex_set(&req.rawuri,
                      (tt_u8_t *)"/a/b?q1=v1",
                      sizeof("/a/b?q1=v1") - 1,
                      TT_FALSE);
        req.updated_uri = TT_FALSE;
        uri = tt_http_parser_get_uri(&req);
        TT_UT_NOT_NULL(uri, "");
        act = tt_http_inserv_on_header(is, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");

        // can process but file unexist
        tt_blobex_set(&req.rawuri,
                      (tt_u8_t *)"/a/b/",
                      sizeof("/a/b/") - 1,
                      TT_FALSE);
        req.updated_uri = TT_FALSE;
        uri = tt_http_parser_get_uri(&req);
        TT_UT_NOT_NULL(uri, "");
        act = tt_http_inserv_on_header(is, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_OWNER, "");
        TT_UT_STREQ(tt_fpath_render(&uri->path), "/a/b/i.php", "");

        act = tt_http_inserv_on_complete(is, &req, &resp);
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

        tt_blobex_set(&req.rawuri,
                      (tt_u8_t *)tt_string_cstr(&s),
                      tt_string_len(&s),
                      TT_FALSE);
        req.updated_uri = TT_FALSE;
        uri = tt_http_parser_get_uri(&req);
        TT_UT_NOT_NULL(uri, "");
        act = tt_http_inserv_on_header(is, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_OWNER, "");

        // this service does not handle body
        act = tt_http_inserv_on_body(is, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");
        act = tt_http_inserv_on_trailing(is, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");

        // will send a 200 ok
        act = tt_http_inserv_on_complete(is, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_BODY, "");
        TT_UT_EQUAL(tt_http_resp_render_get_status(&resp),
                    TT_HTTP_STATUS_OK,
                    "");
        TT_UT_EQUAL(resp.render.content_len, sizeof(b), "");

        act = tt_http_inserv_get_body(is, &req, &resp, &bbuf);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_BODY, "");
        TT_UT_EQUAL(TT_BUF_RLEN(&bbuf), sizeof(b), "");
        TT_UT_STREQ(TT_BUF_RPOS(&bbuf), b, "");

        // end
        act = tt_http_inserv_get_body(is, &req, &resp, &bbuf);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");

        tt_string_destroy(&s);
        tt_buf_destroy(&bbuf);
    }

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
        act = tt_http_inserv_on_header(is, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_OWNER, "");

        // this service does not handle body
        act = tt_http_inserv_on_body(is, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");
        act = tt_http_inserv_on_trailing(is, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");

        // will send a 200 ok
        act = tt_http_inserv_on_complete(is, &req, &resp);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_BODY, "");
        TT_UT_EQUAL(tt_http_resp_render_get_status(&resp),
                    TT_HTTP_STATUS_OK,
                    "");
        TT_UT_EXP(resp.render.content_len < 0, "");
        TT_UT_EQUAL(resp.render.txenc_num, 1, "");
        TT_UT_EQUAL(resp.render.txenc[0], TT_HTTP_TXENC_CHUNKED, "");

        // chunk size is set to 16
        act = tt_http_inserv_get_body(is, &req, &resp, &bbuf);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_BODY, "");
        TT_UT_EQUAL(TT_BUF_RLEN(&bbuf), 16, "");
        TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(&bbuf), b, 16), 0, "");

        // left 4 bytes
        tt_buf_clear(&bbuf);
        act = tt_http_inserv_get_body(is, &req, &resp, &bbuf);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_BODY, "");
        TT_UT_EQUAL(TT_BUF_RLEN(&bbuf), 5, "");
        TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(&bbuf), b + 16, 5), 0, "");

        // end
        act = tt_http_inserv_get_body(is, &req, &resp, &bbuf);
        TT_UT_EQUAL(act, TT_HTTP_INSERV_ACT_PASS, "");

        tt_string_destroy(&s);
        tt_buf_destroy(&bbuf);
    }

    tt_http_inserv_release(is);

    tt_http_parser_destroy(&req);
    tt_http_resp_render_destroy(&resp);

    // test end
    TT_TEST_CASE_LEAVE()
}
