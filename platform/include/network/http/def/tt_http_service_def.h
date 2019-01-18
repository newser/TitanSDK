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

/**
@file tt_http_service_def.h
@brief http service type def

this file defines http service type
*/

#ifndef __TT_HTTP_SERVICE_DEF__
#define __TT_HTTP_SERVICE_DEF__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_string.h>
#include <io/tt_file_system.h>
#include <json/tt_json_document.h>
#include <network/http/def/tt_http_def.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_http_contype_map_s;

typedef enum {
    TT_HTTP_INSERV_HOST,
    TT_HTTP_INSERV_FILE,
    TT_HTTP_INSERV_CONDITIONAL,
    TT_HTTP_INSERV_PARAM,

    TT_HTTP_INSERV_TYPE_NUM
} tt_http_inserv_type_t;
#define TT_HTTP_INSERV_TYPE_VALID(t) ((t) < TT_HTTP_INSERV_TYPE_NUM)

typedef struct
{
    tt_http_status_t status;
} tt_http_inserv_cond_ctx_t;

typedef struct
{
    tt_file_t f;
    tt_s32_t size;
    tt_bool_t f_valid : 1;
} tt_http_inserv_file_ctx_t;

typedef struct
{
    tt_jdoc_t jdoc;
    tt_string_t body;
    tt_buf_t buf;
} tt_http_inserv_param_ctx_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif /* __TT_HTTP_SERVICE_DEF__ */
