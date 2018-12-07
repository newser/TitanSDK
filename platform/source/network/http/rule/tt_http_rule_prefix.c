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

#include <network/http/rule/tt_http_rule_prefix.h>

#include <algorithm/tt_string.h>

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

static tt_bool_t __rule_prefix_match(IN tt_http_rule_t *r,
                                     IN tt_string_t *uri,
                                     IN tt_u32_t pos);

static tt_http_rule_result_t __rule_prefix_pre(IN tt_http_rule_t *r,
                                               IN OUT tt_string_t *uri,
                                               IN OUT tt_u32_t *pos);

static tt_http_rule_itf_t __rule_prefix_itf = {
    NULL, __rule_prefix_match, __rule_prefix_pre, NULL,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_http_rule_t *tt_http_rule_prefix_create_n(IN const tt_char_t *prefix,
                                             IN tt_u32_t prefix_len,
                                             IN OPT const tt_char_t *replace,
                                             IN tt_u32_t replace_len)
{
    tt_http_rule_t *r;
    tt_http_rule_prefix_t *rp;
    tt_u8_t *p;

    TT_ASSERT((prefix != NULL) && (prefix_len != 0));
    TT_ASSERT((replace != NULL) || (replace_len == 0));

    r = tt_http_rule_create(sizeof(tt_http_rule_prefix_t) + prefix_len +
                                replace_len,
                            &__rule_prefix_itf,
                            TT_HTTP_RULE_BREAK);
    if (r == NULL) {
        return NULL;
    }

    rp = TT_HTTP_RULE_CAST(r, tt_http_rule_prefix_t);

    p = TT_PTR_INC(tt_u8_t, rp, sizeof(tt_http_rule_prefix_t));
    tt_memcpy(p, prefix, prefix_len);
    rp->prefix.addr = p;
    rp->prefix.len = prefix_len;

    if (replace != NULL) {
        p = TT_PTR_INC(tt_u8_t, rp, sizeof(tt_http_rule_prefix_t) + prefix_len);
        tt_memcpy(p, replace, replace_len);
        rp->replace.addr = p;
        rp->replace.len = replace_len;
    } else {
        tt_blob_init(&rp->replace);
    }

    return r;
}

tt_bool_t __rule_prefix_match(IN tt_http_rule_t *r,
                              IN tt_string_t *uri,
                              IN tt_u32_t pos)
{
    tt_http_rule_prefix_t *rp = TT_HTTP_RULE_CAST(r, tt_http_rule_prefix_t);
    tt_u32_t len = tt_string_len(uri);
    tt_u32_t n = rp->prefix.len;

    TT_ASSERT(pos <= len);
    len -= pos;

    if ((n <= len) &&
        (tt_memcmp(rp->prefix.addr, tt_string_cstr(uri) + pos, n) == 0)) {
        return TT_TRUE;
    } else {
        return TT_FALSE;
    }
}

tt_http_rule_result_t __rule_prefix_pre(IN tt_http_rule_t *r,
                                        IN OUT tt_string_t *uri,
                                        IN OUT tt_u32_t *pos)
{
    tt_http_rule_prefix_t *rp = TT_HTTP_RULE_CAST(r, tt_http_rule_prefix_t);

    if (rp->replace.addr != NULL) {
        if (!TT_OK(tt_string_set_range_n(uri,
                                         *pos,
                                         rp->prefix.len,
                                         (tt_char_t *)rp->replace.addr,
                                         rp->replace.len))) {
            return TT_HTTP_RULE_ERROR;
        }
        *pos += rp->replace.len;
    } else {
        *pos += rp->prefix.len;
    }

    // child rule may continue matching
    return TT_HTTP_RULE_NEXT;
}
