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
@file tt_http_rule_index.h
@brief http rule index

this file defines http rule index
*/

#ifndef __TT_HTTP_RULE_INDEX__
#define __TT_HTTP_RULE_INDEX__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <network/http/tt_http_rule.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct tt_http_rule_index_s
{
    tt_char_t *name;
    tt_u32_t name_len;
} tt_http_rule_index_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_http_rule_t *tt_http_rule_index_create_n(IN const tt_char_t *name,
                                                      IN tt_u32_t name_len,
                                                      IN tt_http_rule_result_t
                                                          default_result);

tt_inline tt_http_rule_t *tt_http_rule_index_create(
    IN const tt_char_t *name, IN tt_http_rule_result_t default_result)
{
    return tt_http_rule_index_create_n(name, tt_strlen(name), default_result);
}

#endif /* __TT_HTTP_RULE_INDEX__ */
