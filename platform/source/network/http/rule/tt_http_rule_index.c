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

#include <network/http/rule/tt_http_rule_index.h>

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

static tt_bool_t __r_idx_match(IN tt_http_rule_t *r, IN tt_http_uri_t *uri,
                               IN tt_string_t *path,
                               IN tt_http_inserv_host_ctx_t *ctx);

static tt_http_rule_result_t __r_idx_pre(IN tt_http_rule_t *r,
                                         IN OUT tt_http_uri_t *uri,
                                         IN OUT tt_string_t *path,
                                         IN OUT tt_http_inserv_host_ctx_t *ctx);

static tt_http_rule_itf_t __r_idx_itf = {
    NULL,
    __r_idx_match,
    __r_idx_pre,
    NULL,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_http_rule_t *tt_http_rule_index_create_n(
    IN const tt_char_t *name, IN tt_u32_t name_len,
    IN tt_http_rule_result_t default_result)
{
    tt_http_rule_t *r;
    tt_http_rule_index_t *ri;
    tt_char_t *p;

    TT_ASSERT((name != NULL) && (name_len != 0));

    r = tt_http_rule_create(sizeof(tt_http_rule_index_t) + name_len,
                            &__r_idx_itf, default_result);
    if (r == NULL) { return NULL; }

    ri = TT_HTTP_RULE_CAST(r, tt_http_rule_index_t);

    p = TT_PTR_INC(tt_char_t, ri, sizeof(tt_http_rule_index_t));
    tt_memcpy(p, name, name_len);
    ri->name = p;
    ri->name_len = name_len;

    return r;
}

tt_bool_t __r_idx_match(IN tt_http_rule_t *r, IN tt_http_uri_t *uri,
                        IN tt_string_t *path, IN tt_http_inserv_host_ctx_t *ctx)
{
    return tt_string_endwith_c(path, '/');
}

tt_http_rule_result_t __r_idx_pre(IN tt_http_rule_t *r,
                                  IN OUT tt_http_uri_t *uri,
                                  IN OUT tt_string_t *path,
                                  IN OUT tt_http_inserv_host_ctx_t *ctx)
{
    tt_http_rule_index_t *ri = TT_HTTP_RULE_CAST(r, tt_http_rule_index_t);

    if (!TT_OK(tt_string_append_n(path, ri->name, ri->name_len))) {
        return TT_HTTP_RULE_ERROR;
    }
    ctx->path_modified = TT_TRUE;

    // child rule may continue matching
    return TT_HTTP_RULE_NEXT;
}
