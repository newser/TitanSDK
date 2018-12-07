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
@file tt_http_rule_prefix.h
@brief http rule prefix

this file defines http rule prefix
*/

#ifndef __TT_HTTP_RULE_PREFIX__
#define __TT_HTTP_RULE_PREFIX__

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

typedef struct tt_http_rule_prefix_s
{
    tt_blob_t prefix;
    tt_blob_t replace;
} tt_http_rule_prefix_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_http_rule_t *tt_http_rule_prefix_create_n(
    IN const tt_char_t *prefix,
    IN tt_u32_t prefix_len,
    IN OPT const tt_char_t *replace,
    IN tt_u32_t replace_len);

tt_inline tt_http_rule_t *tt_http_rule_prefix_create(
    IN const tt_char_t *prefix, IN OPT const tt_char_t *replace)
{
    return tt_http_rule_prefix_create_n(prefix,
                                        tt_strlen(prefix),
                                        replace,
                                        TT_COND(replace != NULL,
                                                tt_strlen(replace),
                                                0));
}

#endif /* __TT_HTTP_RULE_PREFIX__ */
