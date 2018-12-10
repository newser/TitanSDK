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

tt_result_t tt_http_host_create_n(IN tt_http_host_t *h,
                                  IN OPT const tt_char_t *name,
                                  IN tt_u32_t name_len)
{
    // no extra destroy, match all uri, no pre, no post, return next
    static tt_http_rule_itf_t s_host_root_itf = {0};

    tt_u32_t __done = 0;
#define __HH_ROOT (1 << 0)
#define __HH_NAME (1 << 1)

    TT_ASSERT(h != NULL);

    h->root = tt_http_rule_create(0, &s_host_root_itf, TT_HTTP_RULE_NEXT);
    if (h->root == NULL) {
        return TT_FAIL;
    }
    __done |= __HH_ROOT;

    tt_dnode_init(&h->dnode);

    if (name != NULL) {
        if (!TT_OK(tt_blob_create(&h->name, (tt_u8_t *)name, name_len))) {
            goto fail;
        }
    } else {
        tt_blob_init(&h->name);
    }
    __done |= __HH_NAME;

    return TT_SUCCESS;

fail:

    if (__done & __HH_ROOT) {
        tt_http_rule_release(h->root);
    }

    if (__done & __HH_NAME) {
        tt_blob_destroy(&h->name);
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
