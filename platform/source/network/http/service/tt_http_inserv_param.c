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
#include <network/http/def/tt_http_service_def.h>
#include <network/http/tt_http_parser.h>
#include <network/http/tt_http_render.h>
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

static tt_http_inserv_itf_t s_inserv_param_itf = {NULL,
                                                  NULL,
                                                  __create_ctx,
                                                  __destroy_ctx,
                                                  __clear_ctx};

static tt_http_inserv_action_t __inserv_param_on_hdr(
    IN tt_http_inserv_t *s,
    IN void *ctx,
    IN tt_http_parser_t *req,
    OUT tt_http_resp_render_t *resp);

static tt_http_inserv_action_t __inserv_param_on_complete(
    IN tt_http_inserv_t *s,
    IN void *ctx,
    IN tt_http_parser_t *req,
    OUT tt_http_resp_render_t *resp);

static tt_http_inserv_action_t __inserv_param_get_body(
    IN tt_http_inserv_t *s,
    IN void *ctx,
    IN tt_http_parser_t *req,
    IN tt_http_resp_render_t *resp,
    OUT tt_buf_t *buf);

static tt_http_inserv_cb_t s_inserv_param_cb = {
    NULL,
    __inserv_param_on_hdr,
    NULL,
    NULL,
    __inserv_param_on_complete,
    __inserv_param_get_body,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __param2json(IN tt_param_t *param,
                                IN tt_jdoc_t *jd,
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
    if (tt_g_http_inserv_param == NULL) {
        return TT_FAIL;
    }

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

    s = tt_http_inserv_create(sizeof(tt_http_inserv_param_t) + attr->path_len +
                                  1,
                              &s_inserv_param_itf,
                              &s_inserv_param_cb);
    if (s == NULL) {
        return NULL;
    }

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

    if (!TT_OK(tt_jdoc_create(jd))) {
        return TT_FAIL;
    }

    tt_jval_set_obj(tt_jdoc_get_root(jd));

    return TT_SUCCESS;
}

void __destroy_ctx(IN tt_http_inserv_t *s, IN void *ctx)
{
    tt_http_inserv_param_ctx_t *c = (tt_http_inserv_param_ctx_t *)ctx;

    tt_jdoc_destroy(&c->jdoc);
}

void __clear_ctx(IN tt_http_inserv_t *s, IN void *ctx)
{
    tt_http_inserv_param_ctx_t *c = (tt_http_inserv_param_ctx_t *)ctx;
    tt_jdoc_t *jd = &c->jdoc;

    // tt_jdoc_clear(jd);
    // tt_jval_set_obj() would clear the jd
    tt_jval_set_obj(tt_jdoc_get_root(jd));
}

tt_http_inserv_action_t __inserv_param_on_hdr(IN tt_http_inserv_t *s,
                                              IN void *ctx,
                                              IN tt_http_parser_t *req,
                                              OUT tt_http_resp_render_t *resp)
{
    tt_http_inserv_param_t *sp = TT_HTTP_INSERV_CAST(s, tt_http_inserv_param_t);

    tt_http_uri_t *uri;
    tt_fpath_t *fp;
    const tt_char_t *path;

    uri = tt_http_parser_get_uri(req);
    fp = TT_COND(uri != NULL, tt_http_uri_get_path(uri), NULL);
    path = TT_COND(fp != NULL, tt_fpath_render(fp), NULL);
    if (path == NULL) {
        tt_http_resp_render_set_status(resp,
                                       TT_HTTP_STATUS_INTERNAL_SERVER_ERROR);
        return TT_HTTP_INSERV_ACT_DISCARD;
    }

    if (tt_strncmp(path, sp->path, sp->path_len) == 0) {
        return TT_HTTP_INSERV_ACT_OWNER;
    }

    return TT_HTTP_INSERV_ACT_PASS;
}

tt_http_inserv_action_t __inserv_param_on_complete(
    IN tt_http_inserv_t *s,
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
    tt_buf_t buf;
    tt_result_t result;

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

    param = tt_param_path_p2n(sp->root, sp->root, pos, tt_strlen(pos));
    if (param == NULL) {
        tt_http_resp_render_set_status(resp, TT_HTTP_STATUS_NOT_FOUND);
        return TT_HTTP_INSERV_ACT_DISCARD;
    }

    tt_buf_init(&buf, NULL);
    result = __param2json(param, &c->jdoc, &buf);
    tt_buf_destroy(&buf);
    if (!TT_OK(result)) {
        tt_http_resp_render_set_status(resp,
                                       TT_HTTP_STATUS_INTERNAL_SERVER_ERROR);
        return TT_HTTP_INSERV_ACT_DISCARD;
    }

    tt_http_resp_render_set_contype(resp, TT_HTTP_CONTYPE_APP_JSON);

    return TT_HTTP_INSERV_ACT_BODY;
}

tt_http_inserv_action_t __inserv_param_get_body(IN tt_http_inserv_t *s,
                                                IN void *ctx,
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

tt_result_t __param2json(IN tt_param_t *param,
                         IN tt_jdoc_t *jd,
                         IN tt_buf_t *buf)
{
    switch (param->type) {
        case TT_PARAM_BOOL:
        case TT_PARAM_U32:
        case TT_PARAM_S32:
        case TT_PARAM_STRING:
        case TT_PARAM_FLOAT: {
            tt_char_t tid[12] = {0};
            tt_jval_t name, val;

            tt_snprintf(tid, sizeof(tid), "%d", tt_param_tid(param));
            tt_jval_create_str(&name, tid, jd);

            tt_buf_clear(buf);
            if (!TT_OK(tt_param_read(param, buf))) {
                tt_jval_destroy(&name);
                return TT_FAIL;
            }
            tt_jval_create_strn(&val,
                                (tt_char_t *)TT_BUF_RPOS(buf),
                                TT_BUF_RLEN(buf),
                                jd);

            tt_jobj_add_nv(tt_jdoc_get_root(jd), &name, &val, jd);
        } break;

        case TT_PARAM_DIR: {
            tt_param_t *p =
                tt_param_dir_head(TT_PARAM_CAST(param, tt_param_dir_t));
            for (p = tt_param_dir_head(TT_PARAM_CAST(param, tt_param_dir_t));
                 p != NULL;
                 p = tt_param_dir_next(p)) {
                if (!TT_OK(__param2json(p, jd, buf))) {
                    return TT_FAIL;
                }
            }
        } break;

        default: {
        } break;
    }

    return TT_SUCCESS;
}
