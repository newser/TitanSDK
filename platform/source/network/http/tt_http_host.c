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

tt_result_t tt_http_host_create_n(IN tt_http_host_t *h,
                                  IN OPT const tt_char_t *name,
                                  IN tt_u32_t name_len,
                                  IN OPT tt_http_host_match_t match)
{
    // - no extra destroy
    // - match all uri
    // - no pre
    // - no post
    // - always return TT_HTTP_RULE_NEXT
    static tt_http_rule_itf_t s_root_itf = {0};

    tt_u32_t __done = 0;
#define __HH_ROOT (1 << 0)
#define __HH_NAME (1 << 1)

    TT_ASSERT(h != NULL);

    tt_dnode_init(&h->dnode);

    if (match != NULL) {
        h->match = match;
    } else {
        // use simple compare by default
        h->match = tt_http_host_match_cmp;
    }

    h->root = tt_http_rule_create(0, &s_root_itf, TT_HTTP_RULE_NEXT);
    if (h->root == NULL) {
        goto fail;
    }
    __done |= __HH_ROOT;

    if ((name == NULL) || (match == tt_http_host_match_any)) {
        tt_blob_init(&h->name);
    } else {
        if (!TT_OK(tt_blob_create(&h->name, (tt_u8_t *)name, name_len))) {
            goto fail;
        }
    }
    __done |= __HH_NAME;

    return TT_SUCCESS;

fail:

    if (__done & __HH_NAME) {
        tt_blob_destroy(&h->name);
    }

    if (__done & __HH_ROOT) {
        tt_http_rule_release(h->root);
    }

    return TT_FAIL;
}

void tt_http_host_destroy(IN tt_http_host_t *h)
{
    TT_ASSERT(h != NULL);
    TT_ASSERT(!tt_dnode_in_dlist(&h->dnode));

    tt_http_rule_release(h->root);

    tt_blob_destroy(&h->name);
}

tt_http_rule_result_t tt_http_host_process(IN tt_http_host_t *h,
                                           IN OUT struct tt_string_s *uri,
                                           IN OUT tt_u32_t *pos)
{
    tt_http_rule_result_t result;
    tt_u32_t n = 0;

    while ((result = tt_http_rule_process(h->root, uri, pos)) ==
           TT_HTTP_RULE_CONTINUE) {
        if (n++ > 10) {
            TT_ERROR("too many rule process loop");
            return TT_HTTP_RULE_ERROR;
        }

        // reset pos before next processing
        *pos = 0;
    }
    return result;
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

tt_bool_t tt_http_host_match_any(IN tt_http_host_t *h,
                                 IN tt_char_t *s,
                                 IN tt_u32_t len)
{
    return TT_TRUE;
}
