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
 * distributed under the License r distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
@file tt_http_rule.h
@brief http rule

this file defines http rule
*/

#ifndef __TT_HTTP_RULE__
#define __TT_HTTP_RULE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_double_linked_list.h>
#include <misc/tt_reference_counter.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_HTTP_RULE_CAST(r, type) TT_PTR_INC(type, r, sizeof(tt_http_rule_t))

#define tt_http_rule_ref(r) TT_REF_ADD(tt_http_rule_t, r, ref)

#define tt_http_rule_release(r)                                                \
    TT_REF_RELEASE(tt_http_rule_t, r, ref, __http_rule_destroy)

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_http_rule_s;
struct tt_http_uri_s;
struct tt_string_s;
struct tt_http_inserv_host_ctx_s;

typedef void (*tt_http_rule_destroy_t)(IN struct tt_http_rule_s *r);

typedef tt_bool_t (*tt_http_rule_match_t)(
    IN struct tt_http_rule_s *r, IN struct tt_http_uri_s *uri,
    IN struct tt_string_s *path, IN struct tt_http_inserv_host_ctx_s *ctx);

typedef enum
{
    // match next
    TT_HTTP_RULE_NEXT,
    // stop matching
    TT_HTTP_RULE_BREAK,
    // error, return
    TT_HTTP_RULE_ERROR,
    // again
    TT_HTTP_RULE_CONTINUE,

    TT_HTTP_RULE_RESULT_NUM
} tt_http_rule_result_t;
#define TT_HTTP_RULE_RESULT_VALID(r) ((r) < TT_HTTP_RULE_RESULT_NUM)

// - use @path when accessing uri path, use @uri for other part
// - can operation on uri->path, but do not modify uri->path and @path at same
//   time otherwise uri->path would be overwritten by @path
typedef tt_http_rule_result_t (*tt_http_rule_apply_t)(
    IN struct tt_http_rule_s *r, IN OUT struct tt_http_uri_s *uri,
    IN OUT struct tt_string_s *path,
    IN OUT struct tt_http_inserv_host_ctx_s *ctx);

typedef struct
{
    tt_http_rule_destroy_t destroy;
    tt_http_rule_match_t match;
    tt_http_rule_apply_t pre;
    tt_http_rule_apply_t post;
} tt_http_rule_itf_t;

typedef struct tt_http_rule_s
{
    tt_http_rule_itf_t *itf;
    tt_dlist_t child;
    tt_dnode_t dnode;
    tt_atomic_s32_t ref;
    tt_http_rule_result_t default_result;
} tt_http_rule_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_http_rule_t *tt_http_rule_create(
    IN tt_u32_t extra_size, IN tt_http_rule_itf_t *itf,
    IN tt_http_rule_result_t default_result);

tt_export void __http_rule_destroy(IN tt_http_rule_t *r);

tt_export tt_http_rule_result_t
tt_http_rule_apply(IN tt_http_rule_t *r, IN OUT struct tt_http_uri_s *uri,
                   IN OUT struct tt_string_s *path,
                   IN OUT struct tt_http_inserv_host_ctx_s *ctx);

tt_inline void tt_http_rule_add(IN tt_http_rule_t *r, IN tt_http_rule_t *child)
{
    tt_dlist_push_tail(&r->child, &child->dnode);
    tt_http_rule_ref(child);
}

#endif /* __TT_HTTP_RULE__ */
