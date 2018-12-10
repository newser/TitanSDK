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
 * distributed under the License h distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
@file tt_http_host.h
@brief http host

this file defines http host
*/

#ifndef __TT_HTTP_HOST__
#define __TT_HTTP_HOST__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_blob.h>
#include <network/http/tt_http_rule.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_component_s;
struct tt_profile_s;
struct tt_http_host_s;

typedef tt_bool_t (*tt_http_host_match_t)(IN struct tt_http_host_s *h,
                                          IN tt_char_t *s,
                                          IN tt_u32_t len);

typedef struct tt_http_host_s
{
    tt_dnode_t dnode;
    tt_http_host_match_t match;
    tt_http_rule_t *root;
    tt_blob_t name;
} tt_http_host_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_result_t tt_http_host_component_init(
    IN struct tt_component_s *comp, IN struct tt_profile_s *profile);

tt_export void tt_http_host_component_exit(IN struct tt_component_s *comp);

tt_export tt_result_t tt_http_host_create_n(IN tt_http_host_t *h,
                                            IN OPT const tt_char_t *name,
                                            IN tt_u32_t name_len,
                                            IN OPT tt_http_host_match_t match);

tt_inline tt_result_t tt_http_host_create(IN tt_http_host_t *h,
                                          IN const tt_char_t *name,
                                          IN OPT tt_http_host_match_t match)
{
    return tt_http_host_create_n(h, name, tt_strlen(name), match);
}

tt_export void tt_http_host_destroy(IN tt_http_host_t *h);

tt_export tt_http_rule_result_t tt_http_host_process(
    IN tt_http_host_t *h, IN OUT struct tt_string_s *uri, IN OUT tt_u32_t *pos);

tt_inline void tt_http_host_add_rule(IN tt_http_host_t *h,
                                     IN tt_http_rule_t *rule)
{
    tt_http_rule_add(h->root, rule);
}

// ========================================
// host matching helper
// ========================================

tt_export tt_bool_t tt_http_host_match_cmp(IN tt_http_host_t *h,
                                           IN tt_char_t *s,
                                           IN tt_u32_t len);

tt_export tt_bool_t tt_http_host_match_any(IN tt_http_host_t *h,
                                           IN tt_char_t *s,
                                           IN tt_u32_t len);

#endif /* __TT_HTTP_HOST__ */
