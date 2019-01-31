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

#include <network/http/tt_http_host.h>

#include <init/tt_component.h>
#include <init/tt_profile.h>
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

tt_result_t tt_http_host_component_init(IN tt_component_t *comp,
                                        IN tt_profile_t *profile)
{
    return TT_SUCCESS;
}

void tt_http_host_component_exit(IN tt_component_t *comp)
{
}

tt_http_host_t *tt_http_host_create_n(IN OPT const tt_char_t *name,
                                      IN tt_u32_t name_len,
                                      IN OPT tt_http_host_match_t match,
                                      IN OPT tt_http_host_attr_t *attr)
{
    // - no extra destroy
    // - match all uri
    // - no pre
    // - no post
    // - always return TT_HTTP_RULE_NEXT
    static tt_http_rule_itf_t s_root_itf = {0};

    tt_http_host_attr_t __attr;
    tt_http_host_t *h;

    tt_u32_t __done = 0;
#define __HH_MEM (1 << 0)
#define __HH_ROOT (1 << 1)
#define __HH_NAME (1 << 2)

    if (attr == NULL) {
        tt_http_host_attr_default(&__attr);
        attr = &__attr;
    }

    h = tt_malloc(sizeof(tt_http_host_t));
    if (h == NULL) {
        TT_ERROR("no mem for http host");
        return NULL;
    }
    __done |= __HH_MEM;

    tt_dnode_init(&h->dnode);

    h->match = match;

    h->root = tt_http_rule_create(0, &s_root_itf, TT_HTTP_RULE_NEXT);
    if (h->root == NULL) {
        goto fail;
    }
    __done |= __HH_ROOT;

    if ((name == NULL) || (match == NULL)) {
        tt_blob_init(&h->name);
    } else {
        if (!TT_OK(tt_blob_create(&h->name, (tt_u8_t *)name, name_len))) {
            goto fail;
        }
    }
    __done |= __HH_NAME;

    h->enable_inserv_file = attr->enable_inserv_file;
    h->enable_inserv_cache = attr->enable_inserv_cache;

    return h;

fail:

    if (__done & __HH_NAME) {
        tt_blob_destroy(&h->name);
    }

    if (__done & __HH_ROOT) {
        tt_http_rule_release(h->root);
    }

    if (__done & __HH_MEM) {
        tt_free(h);
    }

    return NULL;
}

void tt_http_host_attr_default(IN tt_http_host_attr_t *attr)
{
    attr->enable_inserv_file = TT_TRUE;
    attr->enable_inserv_cache = TT_TRUE;
}

void tt_http_host_destroy(IN tt_http_host_t *h)
{
    TT_ASSERT(h != NULL);
    TT_ASSERT(!tt_dnode_in_dlist(&h->dnode));

    tt_http_rule_release(h->root);

    tt_blob_destroy(&h->name);

    tt_free(h);
}

tt_http_rule_result_t tt_http_host_apply(IN tt_http_host_t *h,
                                         IN OUT tt_http_uri_t *uri)
{
    tt_fpath_t *fpath = tt_http_uri_get_path(uri);
    tt_string_t *s = tt_fpath_string(fpath);
    tt_http_rule_ctx_t ctx;
    tt_http_rule_result_t result;
    tt_u32_t n = 0;

    if (tt_http_uri_render(uri) == NULL) {
        return TT_HTTP_RULE_ERROR;
    }

    tt_http_rule_ctx_init(&ctx);

again:
    tt_http_rule_ctx_clear(&ctx);
    result = tt_http_rule_apply(h->root, uri, s, &ctx);
    if ((result == TT_HTTP_RULE_NEXT) || (result == TT_HTTP_RULE_BREAK)) {
        // if path is modified, we must have fpath parse modified string again
        if (ctx.path_modified) {
            if (TT_OK(tt_fpath_parse_self(fpath))) {
                uri->path_modified = TT_TRUE;
            } else {
                goto error;
            }
        }

        tt_http_rule_ctx_destroy(&ctx);
        return result;
    } else if (result == TT_HTTP_RULE_CONTINUE) {
        if (n++ > 10) {
            TT_ERROR("too many rule process loop");
            goto error;
        }

        if (ctx.path_modified) {
            if (TT_OK(tt_fpath_parse_self(fpath))) {
                uri->path_modified = TT_TRUE;
            } else {
                goto error;
            }
        }
        goto again;
    } else {
        goto error;
    }

error:
    tt_http_rule_ctx_destroy(&ctx);
    return TT_HTTP_RULE_ERROR;
}

// ========================================
// host matching helper
// ========================================

tt_bool_t tt_http_host_match_cmp(IN tt_http_host_t *h,
                                 IN tt_char_t *s,
                                 IN tt_u32_t len)
{
    tt_char_t *name = (tt_char_t *)h->name.addr;
    tt_u32_t name_len = h->name.len;

    if ((name != NULL) && (name_len == len) && (tt_memcmp(name, s, len) == 0)) {
        return TT_TRUE;
    } else {
        return TT_FALSE;
    }
}
