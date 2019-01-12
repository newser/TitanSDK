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

#include <network/http/service/tt_http_inserv_host.h>

#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <network/http/tt_http_host_set.h>
#include <network/http/tt_http_parser.h>
#include <network/http/tt_http_raw_header.h>
#include <network/http/tt_http_render.h>
#include <network/http/tt_http_server_connection.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef tt_http_inserv_host_attr_t tt_http_inserv_host_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

tt_http_inserv_t *tt_g_http_inserv_host;

static tt_http_inserv_itf_t s_inserv_host_itf = {0};

static tt_http_inserv_action_t __inserv_host_on_hdr(
    IN tt_http_inserv_t *s,
    IN void *ctx,
    IN tt_http_parser_t *req,
    OUT tt_http_resp_render_t *resp);

static tt_http_inserv_cb_t s_inserv_host_cb = {
    NULL, __inserv_host_on_hdr, NULL, NULL, NULL, NULL,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_http_inserv_host_component_init(IN tt_component_t *comp,
                                               IN tt_profile_t *profile)
{
    tt_http_inserv_host_attr_t attr;

    tt_http_inserv_host_attr_default(&attr);

    tt_g_http_inserv_host = tt_http_inserv_host_create(&attr);
    if (tt_g_http_inserv_host == NULL) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_http_inserv_host_component_exit(IN tt_component_t *comp)
{
    tt_http_inserv_release(tt_g_http_inserv_host);
}

tt_http_inserv_t *tt_http_inserv_host_create(
    IN OPT tt_http_inserv_host_attr_t *attr)
{
    tt_http_inserv_host_attr_t __attr;
    tt_http_inserv_t *s;
    tt_http_inserv_host_t *sh;

    if (attr == NULL) {
        tt_http_inserv_host_attr_default(&__attr);
        attr = &__attr;
    }

    s = tt_http_inserv_create(sizeof(tt_http_inserv_host_attr_t),
                              &s_inserv_host_itf,
                              &s_inserv_host_cb);
    if (s == NULL) {
        return NULL;
    }

    sh = TT_HTTP_INSERV_CAST(s, tt_http_inserv_host_t);
    tt_memcpy(sh, attr, sizeof(tt_http_inserv_host_t));

    return s;
}

void tt_http_inserv_host_attr_default(IN tt_http_inserv_host_attr_t *attr)
{
    attr->no_host = TT_HTTP_STATUS_BAD_REQUEST;
    attr->more_host = TT_HTTP_STATUS_BAD_REQUEST;
    attr->host_not_found = TT_HTTP_STATUS_NOT_FOUND;
}

tt_http_inserv_action_t __inserv_host_on_hdr(IN tt_http_inserv_t *s,
                                             IN void *ctx,
                                             IN tt_http_parser_t *req,
                                             OUT tt_http_resp_render_t *resp)
{
    tt_http_inserv_host_t *sh = TT_HTTP_INSERV_CAST(s, tt_http_inserv_host_t);

    tt_http_sconn_t *c = req->c;
    tt_http_uri_t *uri;
    tt_http_rule_result_t rule_result;
    tt_http_status_t status = TT_HTTP_STATUS_INTERNAL_SERVER_ERROR;

    // jobs of this service:
    //  - install host for req
    //  - let host process the req

    if (c->host == NULL) {
        tt_u32_t n;
        tt_blobex_t *name;
        tt_http_hostset_t *hostset;
        tt_http_host_t *host;

        // RFC7230:
        // A server MUST respond with a 400 (Bad Request) status code to any
        // HTTP/1.1 request message that lacks a Host header field and to any
        // request message that contains more than one Host header field or a
        // Host header field with an invalid field-value.

        n = tt_http_rawhdr_count_name(&req->rawhdr, "Host");
        if (n != 1) {
            status = TT_COND(n == 0, sh->no_host, sh->more_host);
            goto fail;
        }

        name = tt_http_parser_get_host(req);
        if (name == NULL) {
            // should not be null
            status = TT_HTTP_STATUS_INTERNAL_SERVER_ERROR;
            goto fail;
        }

        hostset = tt_current_http_hostset(TT_FALSE);
        if (hostset == NULL) {
            status = TT_HTTP_STATUS_INTERNAL_SERVER_ERROR;
            goto fail;
        }

        host = tt_http_hostset_match_n(hostset,
                                       tt_blobex_addr(name),
                                       tt_blobex_len(name));
        if (host == NULL) {
            // 400, 500, 502?
            status = sh->host_not_found;
            goto fail;
        }

        c->host = host;
    }
    TT_ASSERT(c->host != NULL);

    uri = tt_http_parser_get_uri(req);
    if (uri == NULL) {
        status = TT_HTTP_STATUS_INTERNAL_SERVER_ERROR;
        goto fail;
    }

    rule_result = tt_http_host_apply(c->host, uri);
    if (rule_result == TT_HTTP_RULE_ERROR) {
        status = TT_HTTP_STATUS_INTERNAL_SERVER_ERROR;
        goto fail;
    }

    return TT_HTTP_INSERV_ACT_PASS;

fail:
    tt_http_resp_render_set_status(resp, status);
    return TT_HTTP_INSERV_ACT_SHUTDOWN;
}
