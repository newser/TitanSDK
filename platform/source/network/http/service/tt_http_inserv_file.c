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
#include <network/http/tt_http_parser.h>
#include <network/http/tt_http_render.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef struct
{
    tt_char_t *def_name;
    tt_file_t f;
    tt_s32_t size;
    tt_s32_t chunk_size;
    tt_bool_t can_have_path_param : 1;
    // can_have_query_param only take effect on uri query params, not on
    // body of POST even process_post is enabled
    tt_bool_t can_have_query_param : 1;
    tt_bool_t process_post : 1;
    tt_bool_t f_valid : 1;
} tt_http_inserv_file_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static void __inserv_file_clear(IN tt_http_inserv_t *s);

static void __inserv_file_destroy(IN tt_http_inserv_t *s);

static tt_http_inserv_itf_t s_inserv_file_itf = {
    __inserv_file_destroy, __inserv_file_clear,
};

static tt_http_inserv_action_t __inserv_file_on_hdr(
    IN tt_http_inserv_t *s,
    IN tt_http_parser_t *req,
    OUT tt_http_resp_render_t *resp);

static tt_http_inserv_action_t __inserv_file_on_complete(
    IN tt_http_inserv_t *s,
    IN tt_http_parser_t *req,
    OUT tt_http_resp_render_t *resp);

static tt_http_inserv_action_t __inserv_file_get_body(
    IN tt_http_inserv_t *s,
    IN tt_http_parser_t *req,
    IN tt_http_resp_render_t *resp,
    OUT tt_buf_t *buf);

static tt_http_inserv_cb_t s_inserv_file_cb = {
    NULL,
    __inserv_file_on_hdr,
    NULL,
    NULL,
    __inserv_file_on_complete,
    __inserv_file_get_body,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

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

    s = tt_http_inserv_create(sizeof(tt_http_inserv_file_t) +
                                  attr->def_name_len + 1,
                              &s_inserv_file_itf,
                              &s_inserv_file_cb);
    if (s == NULL) {
        return NULL;
    }

    sf = TT_HTTP_INSERV_CAST(s, tt_http_inserv_file_t);

    if (attr->def_name != NULL) {
        sf->def_name = TT_PTR_INC(tt_char_t, sf, sizeof(tt_http_inserv_file_t));
        tt_memcpy(sf->def_name, attr->def_name, attr->def_name_len);
        sf->def_name[attr->def_name_len] = 0;
    } else {
        sf->def_name = NULL;
    }

    sf->size = -1;
    sf->chunk_size = attr->chunk_size;
    sf->can_have_path_param = attr->can_have_path_param;
    sf->can_have_query_param = attr->can_have_query_param;
    sf->process_post = attr->process_post;
    sf->f_valid = TT_FALSE;

    return s;
}

void tt_http_inserv_file_attr_default(IN tt_http_inserv_file_attr_t *attr)
{
    attr->def_name = (tt_char_t *)"index.html";
    attr->def_name_len = sizeof("index.html") - 1;
    attr->chunk_size = 1 << 14; // 16k
    attr->can_have_path_param = TT_FALSE;
    attr->can_have_path_param = TT_FALSE;
    attr->process_post = TT_FALSE;
}

void __inserv_file_clear(IN tt_http_inserv_t *s)
{
    tt_http_inserv_file_t *sf = TT_HTTP_INSERV_CAST(s, tt_http_inserv_file_t);

    sf->size = -1;

    if (sf->f_valid) {
        tt_fclose(&sf->f);
        sf->f_valid = TT_FALSE;
    }
}

void __inserv_file_destroy(IN tt_http_inserv_t *s)
{
    tt_http_inserv_file_t *sf = TT_HTTP_INSERV_CAST(s, tt_http_inserv_file_t);

    if (sf->f_valid) {
        tt_fclose(&sf->f);
    }
}

tt_http_inserv_action_t __inserv_file_on_hdr(IN tt_http_inserv_t *s,
                                             IN tt_http_parser_t *req,
                                             OUT tt_http_resp_render_t *resp)
{
    tt_http_inserv_file_t *sf = TT_HTTP_INSERV_CAST(s, tt_http_inserv_file_t);

    tt_http_method_t mtd;
    tt_http_uri_t *uri;
    tt_http_status_t status = TT_HTTP_STATUS_INTERNAL_SERVER_ERROR;
    tt_fpath_t *fp;
    const tt_char_t *path;

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

    fp = tt_http_uri_get_path(uri);
    if (tt_fpath_is_dir(fp)) {
        if (sf->def_name == NULL) {
            return TT_HTTP_INSERV_ACT_PASS;
        }

        // note the side effect is that all following server would see the
        // modified uri
        if (!TT_OK(tt_fpath_set_filename(fp, sf->def_name))) {
            status = TT_HTTP_STATUS_INTERNAL_SERVER_ERROR;
            goto fail;
        }
    }

    path = tt_fpath_render(fp);
    if (path == NULL) {
        status = TT_HTTP_STATUS_INTERNAL_SERVER_ERROR;
        goto fail;
    }

#if 0
    if (sf->f_valid) {
        tt_fclose(&sf->f);
        sf->f_valid = TT_FALSE;
    }
#else
    TT_ASSERT(!sf->f_valid);
#endif

    if (!TT_OK(tt_fopen(&sf->f, path, TT_FO_READ | TT_FO_SEQUENTIAL, NULL))) {
#if 0
        // may not exist, ignore or 404?
        return TT_HTTP_INSERV_ACT_PASS;
#else
        status = TT_HTTP_STATUS_NOT_FOUND;
        goto fail;
#endif
    }
    sf->f_valid = TT_TRUE;

    // will handle this request
    return TT_HTTP_INSERV_ACT_OWNER;

fail:
    tt_http_resp_render_set_status(resp, status);
    return TT_HTTP_INSERV_ACT_PASS;
}

tt_http_inserv_action_t __inserv_file_on_complete(
    IN tt_http_inserv_t *s,
    IN tt_http_parser_t *req,
    OUT tt_http_resp_render_t *resp)
{
    tt_http_inserv_file_t *sf = TT_HTTP_INSERV_CAST(s, tt_http_inserv_file_t);
    tt_u64_t size;

    TT_ASSERT(sf->f_valid);

    if (!TT_OK(tt_fsize(&sf->f, &size))) {
        tt_http_resp_render_set_status(resp,
                                       TT_HTTP_STATUS_INTERNAL_SERVER_ERROR);
        return TT_HTTP_INSERV_ACT_DISCARD;
    }

    if (size >= sf->chunk_size) {
        tt_http_txenc_t txenc[1] = {TT_HTTP_TXENC_CHUNKED};
        tt_http_resp_render_set_txenc(resp,
                                      txenc,
                                      sizeof(txenc) / sizeof(txenc[0]));
        // keep sf->size -1 to indicate it's not using content-length
    } else {
        sf->size = size;
        tt_http_resp_render_set_content_len(resp, size);
    }

    tt_http_resp_render_set_status(resp, TT_HTTP_STATUS_OK);

    return TT_COND(size > 0, TT_HTTP_INSERV_ACT_BODY, TT_HTTP_INSERV_ACT_PASS);
}

tt_http_inserv_action_t __inserv_file_get_body(IN tt_http_inserv_t *s,
                                               IN tt_http_parser_t *req,
                                               IN tt_http_resp_render_t *resp,
                                               OUT tt_buf_t *buf)
{
    tt_http_inserv_file_t *sf = TT_HTTP_INSERV_CAST(s, tt_http_inserv_file_t);
    tt_u32_t len, n;
    tt_result_t result;

    if (sf->size >= 0) {
        TT_ASSERT(sf->size > 0);
        TT_ASSERT(sf->size < sf->chunk_size);
        len = sf->size;
    } else {
        len = sf->chunk_size;
    }
    if (!TT_OK(tt_buf_reserve(buf, len))) {
        // close or continue?
        return TT_HTTP_INSERV_ACT_CLOSE;
    }

    // limit how many bytes to read
    n = TT_BUF_WLEN(buf);
    len = TT_MIN(len, n);

    result = tt_fread(&sf->f, TT_BUF_WPOS(buf), len, &n);
    if (TT_OK(result)) {
        tt_buf_inc_wp(buf, n);
        return TT_HTTP_INSERV_ACT_BODY;
    } else if (result == TT_E_END) {
        return TT_HTTP_INSERV_ACT_PASS;
    } else {
        return TT_HTTP_INSERV_ACT_CLOSE;
    }
}
