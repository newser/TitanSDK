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

#include <network/http/rule/tt_http_rule_startwith.h>

#include <network/http/service/tt_http_inserv_host.h>
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

static tt_bool_t __r_prefix_match(IN tt_http_rule_t *r,
                                  IN tt_http_uri_t *uri,
                                  IN tt_string_t *path,
                                  IN tt_http_inserv_host_ctx_t *ctx);

static tt_http_rule_result_t __r_prefix_pre(IN tt_http_rule_t *r,
                                            IN OUT tt_http_uri_t *uri,
                                            IN OUT tt_string_t *path,
                                            IN OUT
                                                tt_http_inserv_host_ctx_t *ctx);

static tt_http_rule_itf_t __r_prefix_itf = {
    NULL, __r_prefix_match, __r_prefix_pre, NULL,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_http_rule_t *tt_http_rule_startwith_create_n(IN const tt_char_t *prefix,
                                                IN tt_u32_t prefix_len,
                                                IN OPT const tt_char_t *replace,
                                                IN tt_u32_t replace_len,
                                                IN tt_http_rule_result_t
                                                    default_result)
{
    tt_http_rule_t *r;
    tt_http_rule_startwith_t *rp;
    tt_u8_t *p;

    TT_ASSERT((prefix != NULL) && (prefix_len != 0));
    TT_ASSERT((replace != NULL) || (replace_len == 0));

    r = tt_http_rule_create(sizeof(tt_http_rule_startwith_t) + prefix_len +
                                replace_len,
                            &__r_prefix_itf,
                            default_result);
    if (r == NULL) {
        return NULL;
    }

    rp = TT_HTTP_RULE_CAST(r, tt_http_rule_startwith_t);

    p = TT_PTR_INC(tt_u8_t, rp, sizeof(tt_http_rule_startwith_t));
    tt_memcpy(p, prefix, prefix_len);
    rp->prefix = p;
    rp->prefix_len = prefix_len;

    if (replace != NULL) {
        p = TT_PTR_INC(tt_u8_t,
                       rp,
                       sizeof(tt_http_rule_startwith_t) + prefix_len);
        tt_memcpy(p, replace, replace_len);
        rp->replace = p;
        rp->replace_len = replace_len;
    } else {
        rp->replace = NULL;
        rp->replace_len = 0;
    }

    return r;
}

tt_bool_t __r_prefix_match(IN tt_http_rule_t *r,
                           IN tt_http_uri_t *uri,
                           IN tt_string_t *path,
                           IN tt_http_inserv_host_ctx_t *ctx)
{
    tt_http_rule_startwith_t *rp =
        TT_HTTP_RULE_CAST(r, tt_http_rule_startwith_t);
    tt_u32_t pos, len, prefix_len;

    pos = ctx->path_pos;
    len = tt_string_len(path);
    TT_ASSERT(pos <= len);
    len -= pos;

    prefix_len = rp->prefix_len;
    if ((prefix_len <= len) &&
        (tt_memcmp(rp->prefix, tt_string_cstr(path) + pos, prefix_len) == 0)) {
        return TT_TRUE;
    } else {
        return TT_FALSE;
    }
}

tt_http_rule_result_t __r_prefix_pre(IN tt_http_rule_t *r,
                                     IN OUT tt_http_uri_t *uri,
                                     IN OUT tt_string_t *path,
                                     IN OUT tt_http_inserv_host_ctx_t *ctx)
{
    tt_http_rule_startwith_t *rp =
        TT_HTTP_RULE_CAST(r, tt_http_rule_startwith_t);

    if (rp->replace != NULL) {
        if (!TT_OK(tt_string_set_range_n(path,
                                         ctx->path_pos,
                                         rp->prefix_len,
                                         (tt_char_t *)rp->replace,
                                         rp->replace_len))) {
            return TT_HTTP_RULE_ERROR;
        }
        ctx->path_pos += rp->replace_len;
        ctx->path_modified = TT_TRUE;
    } else {
        ctx->path_pos += rp->prefix_len;
    }

    // child rule may continue matching
    return TT_HTTP_RULE_NEXT;
}
