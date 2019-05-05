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

#include <network/http/service/tt_http_inserv_param.h>

#include <json/tt_json_document.h>
#include <json/tt_json_value.h>
#include <misc/tt_percent_encode.h>
#include <network/http/def/tt_http_service_def.h>
#include <network/http/tt_http_parser.h>
#include <network/http/tt_http_render.h>
#include <param/tt_param_bool.h>
#include <param/tt_param_dir.h>
#include <param/tt_param_path.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define DEFAULT_PARAM_PATH "/param"

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef struct
{
    tt_param_t *root;
    tt_char_t *path;
    tt_u32_t path_len;
} tt_http_inserv_param_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

tt_http_inserv_t *tt_g_http_inserv_param;

static tt_result_t __create_ctx(IN tt_http_inserv_t *s, IN OPT void *ctx);

static void __destroy_ctx(IN tt_http_inserv_t *s, IN void *ctx);

static void __clear_ctx(IN tt_http_inserv_t *s, IN void *ctx);

static tt_http_inserv_itf_t s_param_itf = {NULL, NULL, __create_ctx,
                                           __destroy_ctx, __clear_ctx};

static tt_http_inserv_action_t __s_param_on_uri(
    IN tt_http_inserv_t *s, IN void *ctx, IN tt_http_parser_t *req,
    OUT tt_http_resp_render_t *resp);

static tt_http_inserv_action_t __s_param_on_header(
    IN tt_http_inserv_t *s, IN void *ctx, IN tt_http_parser_t *req,
    OUT tt_http_resp_render_t *resp);

static tt_http_inserv_action_t __s_param_on_body(
    IN tt_http_inserv_t *s, IN void *ctx, IN tt_http_parser_t *req,
    OUT tt_http_resp_render_t *resp);

static tt_http_inserv_action_t __s_param_on_complete(
    IN tt_http_inserv_t *s, IN void *ctx, IN tt_http_parser_t *req,
    OUT tt_http_resp_render_t *resp);

static tt_http_inserv_action_t __s_param_get_body(
    IN tt_http_inserv_t *s, IN void *ctx, IN tt_http_parser_t *req,
    IN tt_http_resp_render_t *resp, OUT tt_buf_t *buf);

static tt_http_inserv_cb_t s_param_cb = {
    __s_param_on_uri,      __s_param_on_header, __s_param_on_body, NULL,
    __s_param_on_complete, __s_param_get_body,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __get_param(IN tt_http_inserv_t *s,
                               IN tt_http_inserv_param_ctx_t *c,
                               IN tt_http_parser_t *req,
                               OUT tt_http_resp_render_t *resp,
                               IN tt_param_t *p);

// static
tt_http_inserv_action_t __post_param(IN tt_http_inserv_t *s,
                                     IN tt_http_inserv_param_ctx_t *c,
                                     IN tt_http_parser_t *req,
                                     OUT tt_http_resp_render_t *resp,
                                     IN tt_param_t *p);

static tt_result_t __param2json(IN tt_param_t *param, IN tt_jdoc_t *jd,
                                IN tt_buf_t *buf);

static tt_result_t __post_single_param(IN const tt_char_t *beg, IN tt_u32_t len,
                                       IN tt_param_t *root, IN tt_jdoc_t *jd,
                                       IN tt_buf_t *buf);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_http_inserv_param_component_init(IN struct tt_component_s *comp,
                                                IN struct tt_profile_s *profile)
{
    tt_http_inserv_param_attr_t attr;

    tt_http_inserv_param_attr_default(&attr);

    tt_g_http_inserv_param =
        tt_http_inserv_param_create(tt_g_param_root, &attr);
    if (tt_g_http_inserv_param == NULL) { return TT_FAIL; }

    return TT_SUCCESS;
}

void tt_http_inserv_param_component_exit(IN struct tt_component_s *comp)
{
    tt_http_inserv_release(tt_g_http_inserv_param);
}

tt_http_inserv_t *tt_http_inserv_param_create(
    IN tt_param_t *root, IN OPT tt_http_inserv_param_attr_t *attr)
{
    tt_http_inserv_param_attr_t __attr;
    tt_http_inserv_t *s;
    tt_http_inserv_param_t *sp;

    TT_ASSERT(root != NULL);

    if (attr == NULL) {
        tt_http_inserv_param_attr_default(&__attr);
        attr = &__attr;
    }

    s = tt_http_inserv_create(TT_HTTP_INSERV_PARAM,
                              sizeof(tt_http_inserv_param_t) + attr->path_len +
                                  1,
                              &s_param_itf, &s_param_cb);
    if (s == NULL) { return NULL; }

    sp = TT_HTTP_INSERV_CAST(s, tt_http_inserv_param_t);

    sp->root = root;

    sp->path = TT_PTR_INC(tt_char_t, sp, sizeof(tt_http_inserv_param_t));
    tt_memcpy(sp->path, attr->path, attr->path_len);
    sp->path[attr->path_len] = 0;
    sp->path_len = attr->path_len;

    return s;
}

void tt_http_inserv_param_attr_default(IN tt_http_inserv_param_attr_t *attr)
{
    attr->path = DEFAULT_PARAM_PATH;
    attr->path_len = sizeof(DEFAULT_PARAM_PATH) - 1;
}

tt_result_t __create_ctx(IN tt_http_inserv_t *s, IN OPT void *ctx)
{
    tt_http_inserv_param_ctx_t *c = (tt_http_inserv_param_ctx_t *)ctx;
    tt_jdoc_t *jd = &c->jdoc;

    if (!TT_OK(tt_jdoc_create(jd))) { return TT_FAIL; }

    tt_jval_set_obj(tt_jdoc_get_root(jd));

    tt_string_init(&c->body, NULL);

    tt_buf_init(&c->buf, NULL);

    return TT_SUCCESS;
}

void __destroy_ctx(IN tt_http_inserv_t *s, IN void *ctx)
{
    tt_http_inserv_param_ctx_t *c = (tt_http_inserv_param_ctx_t *)ctx;

    tt_jdoc_destroy(&c->jdoc);

    tt_string_destroy(&c->body);

    tt_buf_destroy(&c->buf);
}

void __clear_ctx(IN tt_http_inserv_t *s, IN void *ctx)
{
    tt_http_inserv_param_ctx_t *c = (tt_http_inserv_param_ctx_t *)ctx;
    tt_jdoc_t *jd = &c->jdoc;

    // tt_jdoc_clear(jd);
    // tt_jval_set_obj() would clear the jd
    tt_jval_set_obj(tt_jdoc_get_root(jd));

    tt_string_clear(&c->body);

    tt_buf_clear(&c->buf);
}

tt_http_inserv_action_t __s_param_on_uri(IN tt_http_inserv_t *s, IN void *ctx,
                                         IN tt_http_parser_t *req,
                                         OUT tt_http_resp_render_t *resp)
{
    tt_http_inserv_param_t *sp = TT_HTTP_INSERV_CAST(s, tt_http_inserv_param_t);

    tt_http_method_t mtd;
    tt_http_uri_t *uri;
    tt_fpath_t *fp;
    const tt_char_t *path;

    mtd = tt_http_parser_get_method(req);
    if ((mtd != TT_HTTP_MTD_GET) && (mtd != TT_HTTP_MTD_POST) &&
        (mtd != TT_HTTP_MTD_PUT)) {
        return TT_HTTP_INSERV_ACT_PASS;
    }

    uri = tt_http_parser_get_uri(req);
    fp = TT_COND(uri != NULL, tt_http_uri_get_path(uri), NULL);
    path = TT_COND(fp != NULL, tt_fpath_render(fp), NULL);
    if (path == NULL) {
        tt_http_resp_render_set_status(resp,
                                       TT_HTTP_STATUS_INTERNAL_SERVER_ERROR);
        return TT_HTTP_INSERV_ACT_DISCARD;
    }

#if 0
    if (tt_strncmp(path, sp->path, sp->path_len) == 0) {
        return TT_HTTP_INSERV_ACT_OWNER;
    }
#else
    // both of path and sp->path are null-terminated
    if (tt_strcmp(path, sp->path) == 0) { return TT_HTTP_INSERV_ACT_OWNER; }
#endif

    return TT_HTTP_INSERV_ACT_PASS;
}

tt_http_inserv_action_t __s_param_on_header(IN tt_http_inserv_t *s,
                                            IN void *ctx,
                                            IN tt_http_parser_t *req,
                                            OUT tt_http_resp_render_t *resp)
{
    tt_http_method_t mtd;

    // check content type, currently only support urlencoded, but json may
    // be supported in future
    mtd = tt_http_parser_get_method(req);
    if ((mtd == TT_HTTP_MTD_POST) || (mtd == TT_HTTP_MTD_PUT)) {
        tt_http_contype_t ct = tt_http_parser_get_contype(req);

        if (ct != TT_HTTP_CONTYPE_APP_URLENC) {
            tt_http_resp_render_set_status(
                resp, TT_HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE);
            return TT_HTTP_INSERV_ACT_DISCARD;
        }
    }

    return TT_HTTP_INSERV_ACT_PASS;
}

tt_http_inserv_action_t __s_param_on_body(IN tt_http_inserv_t *s, IN void *ctx,
                                          IN tt_http_parser_t *req,
                                          OUT tt_http_resp_render_t *resp)
{
    tt_http_inserv_param_t *sp = TT_HTTP_INSERV_CAST(s, tt_http_inserv_param_t);
    tt_http_inserv_param_ctx_t *c = (tt_http_inserv_param_ctx_t *)ctx;

    if (!TT_OK(tt_string_append_n(&c->body, tt_blobex_addr(&req->body),
                                  tt_blobex_len(&req->body)))) {
        tt_http_resp_render_set_status(resp,
                                       TT_HTTP_STATUS_INTERNAL_SERVER_ERROR);
        return TT_HTTP_INSERV_ACT_DISCARD;
    }

    return TT_HTTP_INSERV_ACT_PASS;
}

tt_http_inserv_action_t __s_param_on_complete(IN tt_http_inserv_t *s,
                                              IN void *ctx,
                                              IN tt_http_parser_t *req,
                                              OUT tt_http_resp_render_t *resp)
{
    tt_http_inserv_param_t *sp = TT_HTTP_INSERV_CAST(s, tt_http_inserv_param_t);
    tt_http_inserv_param_ctx_t *c = (tt_http_inserv_param_ctx_t *)ctx;

    tt_http_uri_t *uri;
    tt_fpath_t *fp;
    const tt_char_t *path, *pos;
    tt_param_t *param;
    tt_http_method_t mtd;

    uri = tt_http_parser_get_uri(req);
    fp = TT_COND(uri != NULL, tt_http_uri_get_path(uri), NULL);
    path = TT_COND(fp != NULL, tt_fpath_render(fp), NULL);
    if (path == NULL) {
        tt_http_resp_render_set_status(resp,
                                       TT_HTTP_STATUS_INTERNAL_SERVER_ERROR);
        return TT_HTTP_INSERV_ACT_DISCARD;
    }

    TT_ASSERT(tt_strncmp(path, sp->path, sp->path_len) == 0);
    pos = path + sp->path_len;
    if (pos[0] == 0) {
        // "example.com/param" == "example.com/param/"
        pos = "/";
    }

    param =
        tt_param_path_p2n(sp->root, sp->root, pos, (tt_u32_t)tt_strlen(pos));
    if (param == NULL) {
        tt_http_resp_render_set_status(resp, TT_HTTP_STATUS_NOT_FOUND);
        return TT_HTTP_INSERV_ACT_DISCARD;
    }

    mtd = tt_http_parser_get_method(req);
    if (mtd == TT_HTTP_MTD_GET) {
        return __get_param(s, c, req, resp, param);
    } else {
        TT_ASSERT((mtd == TT_HTTP_MTD_POST) || (mtd == TT_HTTP_MTD_PUT));
        return __post_param(s, c, req, resp, param);
    }
}

tt_http_inserv_action_t __s_param_get_body(IN tt_http_inserv_t *s, IN void *ctx,
                                           IN tt_http_parser_t *req,
                                           IN tt_http_resp_render_t *resp,
                                           OUT tt_buf_t *buf)
{
    tt_http_inserv_param_ctx_t *c = (tt_http_inserv_param_ctx_t *)ctx;
    tt_jdoc_render_attr_t attr;

    tt_jdoc_render_attr_default(&attr);

    if (!TT_OK(tt_jdoc_render(&c->jdoc, buf, &attr))) {
        return TT_HTTP_INSERV_ACT_CLOSE;
    }

    return TT_HTTP_INSERV_ACT_PASS;
}

tt_result_t __get_param(IN tt_http_inserv_t *s,
                        IN tt_http_inserv_param_ctx_t *c,
                        IN tt_http_parser_t *req,
                        OUT tt_http_resp_render_t *resp, IN tt_param_t *p)
{
    tt_http_txenc_t txenc = TT_HTTP_TXENC_CHUNKED;

    if (!TT_OK(__param2json(p, &c->jdoc, &c->buf))) {
        tt_http_resp_render_set_status(resp,
                                       TT_HTTP_STATUS_INTERNAL_SERVER_ERROR);
        return TT_HTTP_INSERV_ACT_DISCARD;
    }

    tt_http_resp_render_set_status(resp, TT_HTTP_STATUS_OK);
    tt_http_resp_render_set_contype(resp, TT_HTTP_CONTYPE_APP_JSON);
    // content length is unknown
    tt_http_resp_render_set_txenc(resp, &txenc, 1);

    return TT_HTTP_INSERV_ACT_BODY;
}

tt_http_inserv_action_t __post_param(IN tt_http_inserv_t *s,
                                     IN tt_http_inserv_param_ctx_t *c,
                                     IN tt_http_parser_t *req,
                                     OUT tt_http_resp_render_t *resp,
                                     IN tt_param_t *p)
{
    const tt_char_t *body, *end, *prev, *pos;
    tt_bool_t all_done = TT_TRUE;
    tt_http_txenc_t txenc = TT_HTTP_TXENC_CHUNKED;

    body = tt_string_cstr(&c->body);
    end = body + tt_string_len(&c->body);

    prev = body;
    while ((pos = tt_strchr(prev, '&')) != NULL) {
        if (!TT_OK(__post_single_param(prev, (tt_u32_t)(pos - prev), p,
                                       &c->jdoc, &c->buf))) {
            all_done = TT_FALSE;
        }

        prev = pos + 1;
    }
    TT_ASSERT(prev <= end);
    if (prev < end) {
        if (!TT_OK(__post_single_param(prev, (tt_u32_t)(end - prev), p,
                                       &c->jdoc, &c->buf))) {
            all_done = TT_FALSE;
        }
    }

    if (all_done) {
        tt_http_resp_render_set_status(resp, TT_HTTP_STATUS_OK);
    } else {
        tt_http_resp_render_set_status(resp, TT_HTTP_STATUS_ACCEPTED);
    }

    tt_http_resp_render_set_contype(resp, TT_HTTP_CONTYPE_APP_JSON);
    // content length is unknown
    tt_http_resp_render_set_txenc(resp, &txenc, 1);

    return TT_HTTP_INSERV_ACT_BODY;
}

tt_result_t __param2json(IN tt_param_t *param, IN tt_jdoc_t *jd,
                         IN tt_buf_t *buf)
{
    switch (param->type) {
    case TT_PARAM_BOOL:
    case TT_PARAM_U32:
    case TT_PARAM_S32:
    case TT_PARAM_STRING:
    case TT_PARAM_FLOAT: {
        tt_char_t tid[12] = {0};

        tt_snprintf(tid, sizeof(tid), "%d", tt_param_tid(param));

        tt_buf_clear(buf);
        TT_DO(tt_param_read(param, buf));

        tt_jobj_add_strn(tt_jdoc_get_root(jd), tid, (tt_u32_t)tt_strlen(tid),
                         TT_TRUE, (tt_char_t *)TT_BUF_RPOS(buf),
                         TT_BUF_RLEN(buf), TT_TRUE, jd);
    } break;

    case TT_PARAM_DIR: {
        tt_param_t *p = tt_param_dir_head(TT_PARAM_CAST(param, tt_param_dir_t));
        for (p = tt_param_dir_head(TT_PARAM_CAST(param, tt_param_dir_t));
             p != NULL; p = tt_param_dir_next(p)) {
            if (!TT_OK(__param2json(p, jd, buf))) { return TT_FAIL; }
        }
    } break;

    default: {
    } break;
    }

    return TT_SUCCESS;
}

tt_result_t __post_single_param(IN const tt_char_t *beg, IN tt_u32_t len,
                                IN tt_param_t *root, IN tt_jdoc_t *jd,
                                IN tt_buf_t *buf)
{
    const tt_char_t *eq, *name, *val;
    tt_u32_t name_len, val_len;
    tt_char_t name_buf[12] = {0};
    tt_s32_t tid;
    tt_param_t *p;
    tt_result_t result;

    eq = tt_memchr(beg, '=', len);
    if (eq == NULL) { return TT_FAIL; }

    name = beg;
    name_len = (tt_u32_t)(eq - beg);
    tt_trim_lr((tt_u8_t **)&name, &name_len, ' ');

    if (name_len >= sizeof(name_buf)) { return TT_FAIL; }
    tt_memcpy(name_buf, name, name_len);
    if (!TT_OK(tt_strtos32(name_buf, NULL, 10, &tid))) { return TT_FAIL; }

    p = tt_param_find_tid(root, tid);
    if (p == NULL) { return TT_FAIL; }

    val = eq + 1;
    TT_ASSERT(val <= (beg + len));
    val_len = (tt_u32_t)(beg + len - val);
    // tt_trim_lr((tt_u8_t**)&val, &val_len, ' ');

    // must do percent decoding for string parameters
    if (p->type == TT_PARAM_STRING) {
        tt_u32_t n = tt_percent_decode_len(val, val_len);
        TT_ASSERT(n <= val_len);
        tt_buf_clear(buf);
        TT_DO(tt_buf_reserve(buf, n));
        n = tt_percent_decode(val, val_len, TT_TRUE,
                              (tt_char_t *)TT_BUF_RPOS(buf));
        val = (tt_char_t *)TT_BUF_RPOS(buf);
        val_len = n;
    }

    result = tt_param_write(p, (tt_u8_t *)val, val_len);
    // should return the param value to caller
    __param2json(p, jd, buf);
    return result;
}
