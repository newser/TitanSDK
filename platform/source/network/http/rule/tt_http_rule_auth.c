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

#include <network/http/rule/tt_http_rule_auth.h>

#include <network/http/tt_http_in_service.h>
#include <network/http/tt_http_uri.h>

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

static void __rauth_destroy(IN tt_http_rule_t *r);

static tt_bool_t __rauth_match(IN tt_http_rule_t *r,
                               IN tt_http_uri_t *uri,
                               IN tt_string_t *path,
                               IN tt_http_rule_ctx_t *ctx);

static tt_http_rule_result_t __rauth_pre(IN tt_http_rule_t *r,
                                         IN OUT tt_http_uri_t *uri,
                                         IN OUT tt_string_t *path,
                                         IN OUT tt_http_rule_ctx_t *ctx);

static tt_http_rule_itf_t __rauth_itf = {
    __rauth_destroy, __rauth_match, __rauth_pre, NULL,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_http_rule_t *tt_http_rule_auth_create_n(IN const tt_char_t *prefix,
                                           IN tt_u32_t prefix_len,
                                           IN tt_http_inserv_t *auth)
{
    tt_http_rule_t *r;
    tt_http_rule_auth_t *ra;
    tt_u8_t *p;

    TT_ASSERT((prefix != NULL) && (prefix_len != 0));
    TT_ASSERT((auth != NULL) && (auth->type == TT_HTTP_INSERV_AUTH));

    r = tt_http_rule_create(sizeof(tt_http_rule_auth_t) + prefix_len,
                            &__rauth_itf,
                            TT_HTTP_RULE_NEXT);
    if (r == NULL) {
        return NULL;
    }

    ra = TT_HTTP_RULE_CAST(r, tt_http_rule_auth_t);

    tt_http_inserv_ref(auth);
    ra->auth = auth;

    p = TT_PTR_INC(tt_u8_t, ra, sizeof(tt_http_rule_auth_t));
    tt_memcpy(p, prefix, prefix_len);
    ra->prefix = p;
    ra->prefix_len = prefix_len;

    return r;
}

void __rauth_destroy(IN tt_http_rule_t *r)
{
    tt_http_rule_auth_t *ra = TT_HTTP_RULE_CAST(r, tt_http_rule_auth_t);

    tt_http_inserv_release(ra->auth);
}

tt_bool_t __rauth_match(IN tt_http_rule_t *r,
                        IN tt_http_uri_t *uri,
                        IN tt_string_t *path,
                        IN tt_http_rule_ctx_t *ctx)
{
    tt_http_rule_auth_t *ra = TT_HTTP_RULE_CAST(r, tt_http_rule_auth_t);
    tt_u32_t pos, len, prefix_len;

    pos = ctx->path_pos;
    len = tt_string_len(path);
    TT_ASSERT(pos <= len);
    len -= pos;

    prefix_len = ra->prefix_len;
    if ((prefix_len <= len) &&
        (tt_memcmp(ra->prefix, tt_string_cstr(path) + pos, prefix_len) == 0)) {
        return TT_TRUE;
    } else {
        return TT_FALSE;
    }
}

tt_http_rule_result_t __rauth_pre(IN tt_http_rule_t *r,
                                  IN OUT tt_http_uri_t *uri,
                                  IN OUT tt_string_t *path,
                                  IN OUT tt_http_rule_ctx_t *ctx)
{
    tt_http_rule_auth_t *ra = TT_HTTP_RULE_CAST(r, tt_http_rule_auth_t);

    if (ctx->inserv_auth != ra->auth) {
        if (ctx->inserv_auth != NULL) {
            tt_http_inserv_release(ctx->inserv_auth);
            ctx->inserv_auth = NULL;
        }

        tt_http_inserv_ref(ra->auth);
        ctx->inserv_auth = ra->auth;
    }

    return TT_HTTP_RULE_NEXT;
}
