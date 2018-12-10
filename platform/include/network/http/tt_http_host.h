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

typedef struct tt_http_host_s
{
    tt_http_rule_t *root;
    tt_dnode_t dnode;
    tt_blob_t name;
} tt_http_host_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_result_t tt_http_host_create_n(IN tt_http_host_t *h,
                                            IN OPT const tt_char_t *name,
                                            IN tt_u32_t name_len);

tt_inline tt_result_t tt_http_host_create(IN tt_http_host_t *h,
                                          IN const tt_char_t *name)
{
    return tt_http_host_create_n(h, name, tt_strlen(name));
}

tt_export void tt_http_host_destroy(IN tt_http_host_t *h);

tt_export tt_http_rule_result_t tt_http_host_process(
    IN tt_http_host_t *h, IN OUT struct tt_string_s *uri, IN OUT tt_u32_t *pos);

tt_inline void tt_http_host_add(IN tt_http_host_t *h, IN tt_http_rule_t *rule)
{
    tt_http_rule_add(h->root, rule);
}

#endif /* __TT_HTTP_HOST__ */
