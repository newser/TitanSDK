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

#include <network/http/service/tt_http_inserv_file.h>

#include <io/tt_file_system.h>
#include <network/http/def/tt_http_service_def.h>
#include <network/http/tt_http_content_type_map.h>
#include <network/http/tt_http_parser.h>
#include <network/http/tt_http_render.h>
#include <network/http/tt_http_util.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef struct
{
    tt_http_contype_map_t *contype_map;
    tt_s32_t chunk_size;
    tt_bool_t can_have_path_param : 1;
    // can_have_query_param only take effect on uri query params, not on
    // body of POST even process_post is enabled
    tt_bool_t can_have_query_param : 1;
    tt_bool_t process_post : 1;
    tt_bool_t enable_etag : 1;
} tt_http_inserv_file_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

tt_http_inserv_t *tt_g_http_inserv_file;

static tt_result_t __create_ctx(IN tt_http_inserv_t *s, IN OPT void *ctx);

static void __destroy_ctx(IN tt_http_inserv_t *s, IN void *ctx);

static void __clear_ctx(IN tt_http_inserv_t *s, IN void *ctx);

static tt_http_inserv_itf_t s_file_itf = {NULL,
                                          NULL,
                                          __create_ctx,
                                          __destroy_ctx,
                                          __clear_ctx};

static tt_http_inserv_action_t __s_file_on_hdr(IN tt_http_inserv_t *s,
                                               IN void *ctx,
                                               IN tt_http_parser_t *req,
                                               OUT tt_http_resp_render_t *resp);

static tt_http_inserv_action_t __s_file_on_complete(
    IN tt_http_inserv_t *s,
    IN void *ctx,
    IN tt_http_parser_t *req,
    OUT tt_http_resp_render_t *resp);

static tt_http_inserv_action_t __s_file_get_body(IN tt_http_inserv_t *s,
                                                 IN void *ctx,
                                                 IN tt_http_parser_t *req,
                                                 IN tt_http_resp_render_t *resp,
                                                 OUT tt_buf_t *buf);

static tt_http_inserv_cb_t s_file_cb = {
    NULL, __s_file_on_hdr, NULL, NULL, __s_file_on_complete, __s_file_get_body,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_http_inserv_file_component_init(IN struct tt_component_s *comp,
                                               IN struct tt_profile_s *profile)
{
    tt_http_inserv_file_attr_t attr;

    tt_http_inserv_file_attr_default(&attr);

    tt_g_http_inserv_file = tt_http_inserv_file_create(&attr);
    if (tt_g_http_inserv_file == NULL) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_http_inserv_file_component_exit(IN struct tt_component_s *comp)
{
    tt_http_inserv_release(tt_g_http_inserv_file);
}

tt_http_inserv_t *tt_http_inserv_file_create(
    IN OPT tt_http_inserv_file_attr_t *attr)
{
    tt_http_inserv_file_attr_t __attr;
    tt_http_inserv_t *s;
    tt_http_inserv_file_t *sf;

    if (attr == NULL) {
        tt_http_inserv_file_attr_default(&__attr);
        attr = &__attr;
    }

    s = tt_http_inserv_create(TT_HTTP_INSERV_FILE,
                              sizeof(tt_http_inserv_file_t),
                              &s_file_itf,
                              &s_file_cb);
    if (s == NULL) {
        return NULL;
    }

    sf = TT_HTTP_INSERV_CAST(s, tt_http_inserv_file_t);

    sf->contype_map = attr->contype_map;
    sf->chunk_size = attr->chunk_size;
    sf->can_have_path_param = attr->can_have_path_param;
    sf->can_have_query_param = attr->can_have_query_param;
    sf->process_post = attr->process_post;
    sf->enable_etag = attr->enable_etag;

    return s;
}

void tt_http_inserv_file_attr_default(IN tt_http_inserv_file_attr_t *attr)
{
    attr->contype_map = &tt_g_http_contype_map;
    attr->chunk_size = 0; // 1 << 14; // 16k
    attr->can_have_path_param = TT_FALSE;
    attr->can_have_path_param = TT_FALSE;
    attr->process_post = TT_FALSE;
    attr->enable_etag = TT_TRUE;
}

tt_result_t __create_ctx(IN tt_http_inserv_t *s, IN OPT void *ctx)
{
    tt_http_inserv_file_ctx_t *c = (tt_http_inserv_file_ctx_t *)ctx;

    c->size = -1;
    c->f_valid = TT_FALSE;

    return TT_SUCCESS;
}

void __destroy_ctx(IN tt_http_inserv_t *s, IN void *ctx)
{
    tt_http_inserv_file_ctx_t *c = (tt_http_inserv_file_ctx_t *)ctx;

    if (c->f_valid) {
        tt_fclose(&c->f);
    }
}

void __clear_ctx(IN tt_http_inserv_t *s, IN void *ctx)
{
    tt_http_inserv_file_ctx_t *c = (tt_http_inserv_file_ctx_t *)ctx;

    c->size = -1;

    if (c->f_valid) {
        tt_fclose(&c->f);
        c->f_valid = TT_FALSE;
    }
}

tt_http_inserv_action_t __s_file_on_hdr(IN tt_http_inserv_t *s,
                                        IN void *ctx,
                                        IN tt_http_parser_t *req,
                                        OUT tt_http_resp_render_t *resp)
{
    tt_http_inserv_file_t *sf = TT_HTTP_INSERV_CAST(s, tt_http_inserv_file_t);
    tt_http_inserv_file_ctx_t *c = (tt_http_inserv_file_ctx_t *)ctx;

    tt_http_method_t mtd;
    tt_http_uri_t *uri;
    tt_http_status_t status = TT_HTTP_STATUS_INTERNAL_SERVER_ERROR;
    tt_fpath_t *fp;
    // const tt_char_t *path;

    mtd = tt_http_parser_get_method(req);
    if (mtd == TT_HTTP_MTD_GET) {
    } else if (mtd == TT_HTTP_MTD_POST) {
        if (!sf->process_post) {
            return TT_HTTP_INSERV_ACT_PASS;
        }
    } else {
        return TT_HTTP_INSERV_ACT_PASS;
    }

    uri = tt_http_parser_get_uri(req);
    if (uri == NULL) {
        status = TT_HTTP_STATUS_INTERNAL_SERVER_ERROR;
        goto fail;
    }

    if (!sf->can_have_path_param && !tt_http_uri_pparam_empty(uri)) {
        return TT_HTTP_INSERV_ACT_PASS;
    }

    if (!sf->can_have_query_param && !tt_http_uri_qparam_empty(uri)) {
        return TT_HTTP_INSERV_ACT_PASS;
    }

    // we won't do more things, job of this function is only check if this
    // serivce can handle the request
    return TT_HTTP_INSERV_ACT_OWNER;

fail:
    tt_http_resp_render_set_status(resp, status);
    return TT_HTTP_INSERV_ACT_PASS;
}

tt_http_inserv_action_t __s_file_on_complete(IN tt_http_inserv_t *s,
                                             IN void *ctx,
                                             IN tt_http_parser_t *req,
                                             OUT tt_http_resp_render_t *resp)
{
    tt_http_inserv_file_t *sf = TT_HTTP_INSERV_CAST(s, tt_http_inserv_file_t);
    tt_http_inserv_file_ctx_t *c = (tt_http_inserv_file_ctx_t *)ctx;

    tt_http_uri_t *uri;
    tt_fpath_t *fp;
    const tt_char_t *path;
    tt_u64_t size;

    uri = tt_http_parser_get_uri(req);
    fp = TT_COND(uri != NULL, tt_http_uri_get_path(uri), NULL);
    path = TT_COND(fp != NULL, tt_fpath_render(fp), NULL);
    if (path == NULL) {
        tt_http_resp_render_set_status(resp,
                                       TT_HTTP_STATUS_INTERNAL_SERVER_ERROR);
        return TT_HTTP_INSERV_ACT_DISCARD;
    }

    TT_ASSERT(!c->f_valid);
    if (!TT_OK(tt_fopen(&c->f, path, TT_FO_READ | TT_FO_SEQUENTIAL, NULL))) {
        tt_http_resp_render_set_status(resp, TT_HTTP_STATUS_NOT_FOUND);
        return TT_HTTP_INSERV_ACT_DISCARD;
    }
    c->f_valid = TT_TRUE;

    if (!TT_OK(tt_fsize(&c->f, &size))) {
        tt_http_resp_render_set_status(resp,
                                       TT_HTTP_STATUS_INTERNAL_SERVER_ERROR);
        return TT_HTTP_INSERV_ACT_DISCARD;
    }

    if ((sf->chunk_size != 0) && (size >= sf->chunk_size)) {
        tt_http_txenc_t txenc[1] = {TT_HTTP_TXENC_CHUNKED};
        tt_http_resp_render_set_txenc(resp,
                                      txenc,
                                      sizeof(txenc) / sizeof(txenc[0]));
        // keep sf->size -1 to indicate it's not using content-length
    } else {
        c->size = size;
        tt_http_resp_render_set_content_len(resp, size);
    }

    tt_http_resp_render_set_status(resp, TT_HTTP_STATUS_OK);

    if (sf->contype_map != NULL) {
        tt_http_uri_t *uri;
        tt_fpath_t *fp;
        const tt_char_t *ext;
        tt_http_contype_entry_t *e;

        uri = tt_http_parser_get_uri(req);
        fp = TT_COND(uri != NULL, tt_http_uri_get_path(uri), NULL);
        ext = TT_COND(fp != NULL, tt_fpath_get_extension(fp), "");
        e = tt_http_contype_map_find_ext(sf->contype_map, ext);
        if (e != NULL) {
            tt_http_resp_render_set_contype(resp, e->type);
        }
    }

    if (sf->enable_etag) {
        tt_char_t etag[40] = {0};
        if (TT_OK(tt_http_file_etag(&c->f, etag, sizeof(etag)))) {
            tt_http_resp_render_add_etag(resp, etag, TT_FALSE);
        }
    }

    return TT_COND(size > 0, TT_HTTP_INSERV_ACT_BODY, TT_HTTP_INSERV_ACT_PASS);
}

tt_http_inserv_action_t __s_file_get_body(IN tt_http_inserv_t *s,
                                          IN void *ctx,
                                          IN tt_http_parser_t *req,
                                          IN tt_http_resp_render_t *resp,
                                          OUT tt_buf_t *buf)
{
    tt_http_inserv_file_t *sf = TT_HTTP_INSERV_CAST(s, tt_http_inserv_file_t);
    tt_http_inserv_file_ctx_t *c = (tt_http_inserv_file_ctx_t *)ctx;
    tt_u32_t len, n, chunk_head;
    tt_result_t result;

    if (c->size >= 0) {
        TT_ASSERT(c->size > 0);
        TT_ASSERT((sf->chunk_size == 0) || (c->size < sf->chunk_size));
        len = c->size;
        chunk_head = 0;
    } else {
        TT_ASSERT(sf->chunk_size != 0);
        len = sf->chunk_size;
        chunk_head = 16; // "1C\r\n...\r\n"
    }
    if (!TT_OK(tt_buf_reserve(buf, len + chunk_head))) {
        // close or continue?
        return TT_HTTP_INSERV_ACT_CLOSE;
    }
    // put hole so that chunk-size can be put by tt_buf_put_head()
    tt_buf_put_hole(buf, chunk_head);

    // limit how many bytes to read
    n = TT_BUF_WLEN(buf);
    len = TT_MIN(len, n);

    result = tt_fread(&c->f, TT_BUF_WPOS(buf), len, &n);
    if (TT_OK(result)) {
        tt_buf_inc_wp(buf, n);
        return TT_HTTP_INSERV_ACT_BODY;
    } else if (result == TT_E_END) {
        return TT_HTTP_INSERV_ACT_PASS;
    } else {
        return TT_HTTP_INSERV_ACT_CLOSE;
    }
}
