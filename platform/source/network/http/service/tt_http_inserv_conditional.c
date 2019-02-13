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

#include <network/http/service/tt_http_inserv_conditional.h>

#include <io/tt_file_system.h>
#include <network/http/def/tt_http_service_def.h>
#include <network/http/header/tt_http_hdr_etag.h>
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
    tt_u32_t reserved;
} tt_http_inserv_cond_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

tt_http_inserv_t *tt_g_http_inserv_cond;

static tt_result_t __create_ctx(IN tt_http_inserv_t *s, IN OPT void *ctx);

static void __clear_ctx(IN tt_http_inserv_t *s, IN void *ctx);

static tt_http_inserv_itf_t s_cond_itf = {NULL,
                                          NULL,
                                          __create_ctx,
                                          NULL,
                                          __clear_ctx};

static tt_http_inserv_action_t __s_cond_on_hdr(IN tt_http_inserv_t *s,
                                               IN void *ctx,
                                               IN tt_http_parser_t *req,
                                               OUT tt_http_resp_render_t *resp);

static tt_http_inserv_action_t __s_cond_on_complete(
    IN tt_http_inserv_t *s,
    IN void *ctx,
    IN tt_http_parser_t *req,
    OUT tt_http_resp_render_t *resp);

static tt_http_inserv_cb_t s_cond_cb = {
    NULL, __s_cond_on_hdr, NULL, NULL, __s_cond_on_complete, NULL,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __if_match(IN const tt_char_t *path,
                              IN tt_http_hdr_t *h,
                              OUT tt_bool_t *cond);

static tt_result_t __if_none_match(IN const tt_char_t *path,
                                   IN tt_http_hdr_t *h,
                                   OUT tt_bool_t *cond);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_http_inserv_cond_component_init(IN struct tt_component_s *comp,
                                               IN struct tt_profile_s *profile)
{
    tt_http_inserv_cond_attr_t attr;

    tt_http_inserv_cond_attr_default(&attr);

    tt_g_http_inserv_cond = tt_http_inserv_cond_create(&attr);
    if (tt_g_http_inserv_cond == NULL) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_http_inserv_cond_component_exit(IN struct tt_component_s *comp)
{
    tt_http_inserv_release(tt_g_http_inserv_cond);
}

tt_http_inserv_t *tt_http_inserv_cond_create(
    IN OPT tt_http_inserv_cond_attr_t *attr)
{
    tt_http_inserv_cond_attr_t __attr;
    tt_http_inserv_t *s;
    tt_http_inserv_cond_t *sc;

    if (attr == NULL) {
        tt_http_inserv_cond_attr_default(&__attr);
        attr = &__attr;
    }

    s = tt_http_inserv_create(TT_HTTP_INSERV_CONDITIONAL,
                              sizeof(tt_http_inserv_cond_t),
                              &s_cond_itf,
                              &s_cond_cb);
    if (s == NULL) {
        return NULL;
    }

    sc = TT_HTTP_INSERV_CAST(s, tt_http_inserv_cond_t);

    return s;
}

void tt_http_inserv_cond_attr_default(IN tt_http_inserv_cond_attr_t *attr)
{
    attr->reserved = 0;
}

tt_result_t __create_ctx(IN tt_http_inserv_t *s, IN OPT void *ctx)
{
    tt_http_inserv_cond_ctx_t *c = (tt_http_inserv_cond_ctx_t *)ctx;

    c->status = TT_HTTP_STATUS_INVALID;

    return TT_SUCCESS;
}

void __clear_ctx(IN tt_http_inserv_t *s, IN void *ctx)
{
    tt_http_inserv_cond_ctx_t *c = (tt_http_inserv_cond_ctx_t *)ctx;

    c->status = TT_HTTP_STATUS_INVALID;
}

tt_http_inserv_action_t __s_cond_on_hdr(IN tt_http_inserv_t *s,
                                        IN void *ctx,
                                        IN tt_http_parser_t *req,
                                        OUT tt_http_resp_render_t *resp)
{
    tt_http_inserv_cond_t *sc = TT_HTTP_INSERV_CAST(s, tt_http_inserv_cond_t);
    tt_http_inserv_cond_ctx_t *c = (tt_http_inserv_cond_ctx_t *)ctx;

    tt_http_uri_t *uri;
    tt_fpath_t *fp;
    const tt_char_t *path;
    tt_http_hdr_t *h;

    uri = tt_http_parser_get_uri(req);
    fp = TT_COND(uri != NULL, tt_http_uri_get_path(uri), NULL);
    path = TT_COND(fp != NULL, tt_fpath_render(fp), NULL);
    if (path == NULL) {
        tt_http_resp_render_set_status(resp,
                                       TT_HTTP_STATUS_INTERNAL_SERVER_ERROR);
        return TT_HTTP_INSERV_ACT_DISCARD;
    }

    if ((h = tt_http_parser_get_ifnmatch(req)) != NULL) {
        tt_result_t result;
        tt_bool_t cond;

        result = __if_none_match(path, h, &cond);
        if (!TT_OK(result)) {
            return TT_HTTP_INSERV_ACT_PASS;
        } else if (cond) {
            return TT_HTTP_INSERV_ACT_PASS;
        } else {
            /*
             An origin server MUST NOT perform the requested method if the
             condition evaluates to false; instead, the origin server MUST
             respond with either a) the 304 (Not Modified) status code if
             the request method is GET or HEAD or b) the 412 (Precondition
             Failed) status code for all other request methods.
             */
            tt_http_method_t mtd = tt_http_parser_get_method(req);
            if ((mtd == TT_HTTP_MTD_GET) || (mtd == TT_HTTP_MTD_HEAD)) {
                c->status = TT_HTTP_STATUS_NOT_MODIFIED;
            } else {
                c->status = TT_HTTP_STATUS_PRECONDITION_FAILED;
            }
            return TT_HTTP_INSERV_ACT_OWNER;
        }
    } else if ((h = tt_http_parser_get_ifmatch(req)) != NULL) {
        tt_result_t result;
        tt_bool_t cond;

        result = __if_match(path, h, &cond);
        if (!TT_OK(result)) {
            return TT_HTTP_INSERV_ACT_PASS;
        } else if (cond) {
            return TT_HTTP_INSERV_ACT_PASS;
        } else {
            /*
             An origin server MUST NOT perform the requested method if
             a received If-Match condition evaluates to false; instead,
             the origin server MUST respond with either a) the 412
             (Precondition Failed) status code or b) one of the 2xx
             (Successful) status codes if...
             */
            c->status = TT_HTTP_STATUS_PRECONDITION_FAILED;
            return TT_HTTP_INSERV_ACT_OWNER;
        }
    } else {
        return TT_HTTP_INSERV_ACT_PASS;
    }
}

tt_http_inserv_action_t __s_cond_on_complete(IN tt_http_inserv_t *s,
                                             IN void *ctx,
                                             IN tt_http_parser_t *req,
                                             OUT tt_http_resp_render_t *resp)
{
    tt_http_inserv_cond_t *sc = TT_HTTP_INSERV_CAST(s, tt_http_inserv_cond_t);
    tt_http_inserv_cond_ctx_t *c = (tt_http_inserv_cond_ctx_t *)ctx;

    TT_ASSERT(TT_HTTP_STATUS_VALID(c->status));
    tt_http_resp_render_set_status(resp, c->status);
    return TT_HTTP_INSERV_ACT_PASS;
}

tt_result_t __if_match(IN const tt_char_t *path,
                       IN tt_http_hdr_t *h,
                       OUT tt_bool_t *cond)
{
    tt_file_t f;
    tt_result_t result;

    result = tt_fopen(&f, path, TT_FO_READ, NULL);
    if (TT_OK(result)) {
        tt_char_t etag[40];
        tt_u32_t len;
        tt_bool_t b;
        tt_http_etag_t *he;

        if (!TT_OK(tt_http_file_etag(&f, etag, sizeof(etag)))) {
            tt_fclose(&f);
            return TT_FAIL;
        }
        len = (tt_u32_t)tt_strlen(etag);

        // can return true if one of etag matches:
        //  - file existence matches *
        //  - an etag matches
        b = TT_FALSE;
        he = tt_http_etag_head(h);
        while (he != NULL) {
            if (he->aster || (tt_blobex_memcmp(&he->etag, etag, len) == 0)) {
                b = TT_TRUE;
                break;
            }

            he = tt_http_etag_next(he);
        }

        tt_fclose(&f);
        *cond = b;
        return TT_SUCCESS;
    } else if (result == TT_E_NOEXIST) {
        tt_http_etag_t *he;

        he = tt_http_etag_head(h);
        while (he != NULL) {
            if (he->aster) {
                /*
                 If the field-value is "*", the condition is false if the
                 origin server does not have a current representation for
                 the target resource.
                 */
                *cond = TT_FALSE;
                return TT_SUCCESS;
            }

            he = tt_http_etag_next(he);
        }

        *cond = TT_TRUE;
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_result_t __if_none_match(IN const tt_char_t *path,
                            IN tt_http_hdr_t *h,
                            OUT tt_bool_t *cond)
{
    tt_file_t f;
    tt_result_t result;

    result = tt_fopen(&f, path, TT_FO_READ, NULL);
    if (TT_OK(result)) {
        tt_char_t etag[40];
        tt_u32_t len;
        tt_bool_t b;
        tt_http_etag_t *he;

        if (!TT_OK(tt_http_file_etag(&f, etag, sizeof(etag)))) {
            tt_fclose(&f);
            return TT_FAIL;
        }
        len = (tt_u32_t)tt_strlen(etag);

        b = TT_TRUE;
        he = tt_http_etag_head(h);
        while (he != NULL) {
            if (he->aster || (tt_blobex_memcmp(&he->etag, etag, len) == 0)) {
                /*
                 If the field-value is "*", the condition is false if the
                 origin server has a current representation for the target
                 resource

                 the condition is false if one of the listed tags match
                 the entity-tag of the selected representation.
                 */
                b = TT_FALSE;
                break;
            }

            he = tt_http_etag_next(he);
        }

        tt_fclose(&f);
        *cond = b;
        return TT_SUCCESS;
    } else if (result == TT_E_NOEXIST) {
        // unexisted file can match nothing
        *cond = TT_TRUE;
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}
