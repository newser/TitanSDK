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

#include <network/http/tt_http_rule.h>

#include <memory/tt_memory_alloc.h>
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

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_http_rule_t *tt_http_rule_create(IN tt_u32_t extra_size,
                                    IN tt_http_rule_itf_t *itf,
                                    IN tt_http_rule_result_t default_result)
{
    tt_http_rule_t *r;

    TT_ASSERT(itf != NULL);
    TT_ASSERT(TT_HTTP_RULE_RESULT_VALID(default_result));

    r = tt_malloc(sizeof(tt_http_rule_t) + extra_size);
    if (r == NULL) {
        TT_ERROR("no mem for tt_http_rule_t");
        return NULL;
    }

    r->itf = itf;
    tt_dlist_init(&r->child);
    tt_dnode_init(&r->dnode);
    tt_atomic_s32_set(&r->ref, 1);
    r->default_result = default_result;

    return r;
}

void __http_rule_destroy(IN tt_http_rule_t *r)
{
    tt_dnode_t *dn;

    TT_ASSERT(!tt_dnode_in_dlist(&r->dnode));

    while ((dn = tt_dlist_pop_head(&r->child)) != NULL) {
        tt_http_rule_release(TT_CONTAINER(dn, tt_http_rule_t, dnode));
    }

    if (r->itf->destroy != NULL) {
        r->itf->destroy(r);
    }

    tt_free(r);
}

tt_http_rule_result_t tt_http_rule_apply(IN tt_http_rule_t *r,
                                         IN OUT tt_http_uri_t *uri,
                                         IN OUT tt_string_t *path,
                                         IN OUT tt_http_rule_ctx_t *ctx)
{
    tt_http_rule_itf_t *itf = r->itf;
    tt_bool_t matched;
    tt_http_rule_result_t result;
    tt_dnode_t *dn;

    matched =
        TT_COND(itf->match != NULL, itf->match(r, uri, path, ctx), TT_TRUE);
    if (!matched) {
        // does not match this rule, try next
        return TT_HTTP_RULE_NEXT;
    }

    result = TT_COND(itf->pre != NULL,
                     itf->pre(r, uri, path, ctx),
                     TT_HTTP_RULE_NEXT);
    if (result != TT_HTTP_RULE_NEXT) {
        return result;
    }

    dn = tt_dlist_head(&r->child);
    while (dn != NULL) {
        tt_http_rule_t *sub = TT_CONTAINER(dn, tt_http_rule_t, dnode);

        dn = dn->next;

        result = tt_http_rule_apply(sub, uri, path, ctx);
        if (result != TT_HTTP_RULE_NEXT) {
            return result;
        }
    }

    result = TT_COND(itf->post != NULL,
                     itf->post(r, uri, path, ctx),
                     TT_HTTP_RULE_NEXT);
    if (result != TT_HTTP_RULE_NEXT) {
        return result;
    }

    return r->default_result;
}
